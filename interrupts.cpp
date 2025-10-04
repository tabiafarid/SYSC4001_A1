/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include<interrupts.hpp>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of st ringsthat contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/

    long long clock = 0;      
    const int MODE_SWITCH = 1;   
    int SAVE_CONTEXT = 10;  
    const int IRET = 1;

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/

        //CPU
        if (activity == "CPU") {
            execution += std::to_string(clock) + ", " + std::to_string(duration_intr) + ", CPU burst\n";
            clock += duration_intr;
        }

        //SYSCALL
        else if (activity == "SYSCALL") {
            int dev = duration_intr;

            auto [firstSteps, newTime] = intr_boilerplate(clock, dev, SAVE_CONTEXT, vectors);
            execution += firstSteps;
            clock = newTime;

            int ISR_body = delays.at(dev);
            execution += std::to_string(clock) + ", " + std::to_string(40)+  ", SYSCALL: call device driver\n";
            clock += 40;

    
            execution += std::to_string(clock) + ", " + std::to_string(40)+  ", transfer data from device to main memory\n";
            clock += 40;

            int check_errors = ISR_body - 80;
            execution += std::to_string(clock) + ", " + std::to_string(check_errors)+  ", transfer data from device to main memory\n";
            clock += check_errors;

            execution += std::to_string(clock) + ", " + std::to_string(IRET) + ", IRET\n";
            clock += IRET;

        }

        //END I/O
        else if (activity == "END_IO") {
            int dev = duration_intr;

            auto [firstSteps, newTime] = intr_boilerplate(clock, dev, SAVE_CONTEXT, vectors);
            execution += firstSteps;
            clock = newTime;

            int ISR_body = delays.at(dev);
            int activity1 = 40;
            execution += std::to_string(clock) + ", " + std::to_string(activity1)+  ", END IO: call device driver\n";
            clock += activity1;

            int time_remaining = ISR_body - activity1;
            execution += std::to_string(clock) + ", " + std::to_string(time_remaining)+  ", confirm data was transferred successfully\n";
            clock += time_remaining;

            execution += std::to_string(clock) + ", " + std::to_string(IRET) + ", IRET\n";
            clock += IRET;

        }


        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
