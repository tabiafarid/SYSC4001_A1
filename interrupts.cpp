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
    int SAVE_RESTORE_CONTEXT = 10;
    const int ISR_ACTIVITY = 40;  
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

            auto [firstSteps, newTime] = intr_boilerplate(clock, dev, SAVE_RESTORE_CONTEXT, vectors);
            execution += firstSteps;
            clock = newTime;

            int ISR_body = delays.at(dev);
            execution += std::to_string(clock) + ", " + std::to_string(ISR_ACTIVITY)+  ", SYSCALL: run the ISR (device driver)\n";
            clock += ISR_ACTIVITY;
            ISR_body -= ISR_ACTIVITY;

    
            execution += std::to_string(clock) + ", " + std::to_string(ISR_ACTIVITY)+  ", transfer data from device to main memory\n";
            clock += ISR_ACTIVITY;
            ISR_body -= ISR_ACTIVITY;

            int check_errors = ISR_body;
            execution += std::to_string(clock) + ", " + std::to_string(check_errors)+  ", check for errors\n";
            clock += check_errors;

            execution += std::to_string(clock) + ", " + std::to_string(IRET) + ", IRET\n";
            clock += IRET;

            execution += std::to_string(clock) + ", " + std::to_string(SAVE_RESTORE_CONTEXT) + ", restore context\n";
            clock += SAVE_RESTORE_CONTEXT;

            execution += std::to_string(clock) + ", " + std::to_string(MODE_SWITCH) + ", switch to user mode\n";
            clock += MODE_SWITCH;


        }

        //END I/O
        else if (activity == "END_IO") {
            int dev = duration_intr;

            auto [firstSteps, newTime] = intr_boilerplate(clock, dev, SAVE_RESTORE_CONTEXT, vectors);
            execution += firstSteps;
            clock = newTime;

            int ISR_body = delays.at(dev);
            execution += std::to_string(clock) + ", " + std::to_string(ISR_ACTIVITY)+  ", ENDIO: run the ISR (device driver)\n";
            clock += ISR_ACTIVITY;

            ISR_body -= ISR_ACTIVITY;
            execution += std::to_string(clock) + ", " + std::to_string(ISR_body)+  ", confirm data was transferred successfully\n";
            clock += ISR_body;

            execution += std::to_string(clock) + ", " + std::to_string(IRET) + ", IRET\n";
            clock += IRET;

            
            execution += std::to_string(clock) + ", " + std::to_string(SAVE_RESTORE_CONTEXT) + ", restore context\n";
            clock += SAVE_RESTORE_CONTEXT;

            execution += std::to_string(clock) + ", " + std::to_string(MODE_SWITCH) + ", switch to user mode\n";
            clock += MODE_SWITCH;

        }


        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
