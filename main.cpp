#include "process.h" // Defines IOEvent and Process
#include "ioModule.h" // Defines IOInterrupt and IOModule
#include "processMgmt.h" // Declares ProcessManagement

#include <chrono> // for sleep
#include <thread> // for sleep

int main(int argc, char* argv[])
{
    // single thread processor
    // it's either processing something or it's not
//    bool processorAvailable = true;

    // vector of processes, processes will appear here when they are created by
    // the ProcessMgmt object (in other words, automatically at the appropriate time)
    vector<Process> processList;

    // queue to hold processes ready to run (using ProcessIDs)
    list<int> readyProcessesList;

    // this will orchestrate process creation in our system, it will add processes to 
    // processList when they are created and ready to be run/managed
    ProcessManagement processMgmt(processList, readyProcessesList);

    // this is where interrupts will appear when the ioModule detects that an IO operation is complete
    list<IOInterrupt> interrupts;   

    // this manages io operations and will raise interrupts to signal io completion
    IOModule ioModule(interrupts);  

    // Do not touch
    long time = 1;
    long sleepDuration = 50;
    string file;
    stringstream ss;
    enum stepActionEnum {noAct, admitNewProc, handleInterrupt, beginRun, continueRun, ioRequest, complete} stepAction;

    // Do not touch
    switch(argc)
    {
        case 1:
            file = "./procList.txt";  // default input file
            break;
        case 2:
            file = argv[1];         // file given from command line
            break;
        case 3:
            file = argv[1];         // file given
            ss.str(argv[2]);        // sleep duration given
            ss >> sleepDuration;
            break;
        default:
            cerr << "incorrect number of command line arguments" << endl;
            cout << "usage: " << argv[0] << " [file] [sleepDuration]" << endl;
            return 1;
            break;
    }

    processMgmt.readProcessFile(file);


    time = 0;

    // Initializing the first avaialbe process as the running process 
    // to redefine and use each time step
    Process* runningProc = &processList.front();

    // keep running the loop until all processes have been added and have run to completion
    // --> Running as long as there are still running processes
    while(processMgmt.moreProcessesComing() || !processMgmt.allFinished())
    {
       
        //Update our current time step
        ++time;

        //let new processes in (ready) if there are any
        processMgmt.activateProcesses(time);

        //update the status for any active IO requests
        ioModule.ioProcessing(time);

        //init the stepAction, update below
        stepAction = noAct;
        
        // Is there a process running?
        if(processMgmt.checkIfRunning())
        {
            // update total time given to this process
            runningProc->processorTime++;
          
            // TEST IF IOEVENT OCCURS
            if(processMgmt.checkIOEvents(runningProc))
            {
                ioModule.submitIORequest(time, processMgmt.whichIOevent(runningProc), *runningProc);
                runningProc->state = blocked;
                stepAction = ioRequest;

                // remove running process from ready list
                readyProcessesList.pop_front();
            }
            // TEST IF PROCESS FINISHED
            else if(runningProc->processorTime == runningProc->reqProcessorTime)
            {
                runningProc->state = done;
                stepAction = complete;

                // remove running process from ready list
                readyProcessesList.pop_front();
            }
            else // CONTINUE RUNNING
            { 
                stepAction = continueRun;
            }
        }
        // Otherwise, is there a newArrival?
        else if (processList.back().state == newArrival)
        {
          // need to find first new element in vector which is not ready (n)
          int i=0;
          while(processList[i].state == ready) { i++; } // increments until non-ready element ofund
          processList[i].state = ready;
          stepAction = admitNewProc;

          // add new ready process to ready processes list
          readyProcessesList.push_back(processList[i].id);

        }
        // Otherwise, has an event finished? (interrupt generated?)
        else if (!interrupts.empty())
        {
          for(Process& indexedProc : processList)
          {
            if((indexedProc.state == blocked) && (indexedProc.id == interrupts.front().procID))
            {
              indexedProc.state = ready;
              interrupts.pop_front();
              readyProcessesList.push_back(indexedProc.id); // push to back of ready list
              stepAction = handleInterrupt;
              break;
            }
          }
        }
        
        // Otherwise, find a ready process
        else if (processMgmt.newPtoRun(runningProc))
        {
            runningProc->state = processing;
            stepAction = beginRun;
        }

        // Leave the below alone (at least for final submission, we are counting on the output being in expected format)
        cout << setw(5) << time << "\t"; 
        
        switch(stepAction)
        {
            case admitNewProc:
              cout << "[  admit]\t";
              break;
            case handleInterrupt:
              cout << "[ inrtpt]\t";
              break;
            case beginRun:
              cout << "[  begin]\t";
              break;
            case continueRun:
              cout << "[contRun]\t";
              break;
            case ioRequest:
              cout << "[  ioReq]\t";
              break;
            case complete:
              cout << "[ finish]\t";
              break;
            case noAct:
              cout << "[*noAct*]\t";
              break;
        }

        // You may wish to use a second vector of processes (you don't need to, but you can)
        printProcessStates(processList); // change processList to another vector of processes if desired

        this_thread::sleep_for(chrono::milliseconds(sleepDuration));
    }

    return 0;
}
