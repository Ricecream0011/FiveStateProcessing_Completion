#ifndef PROCESS_MGMT_H
#define PROCESS_MGMT_H

#include<vector>
#include<algorithm>  // for sort
using namespace std;

#include "process.h"

inline bool procComp(const Process& p1, const Process& p2)
{
    return p1.arrivalTime > p2.arrivalTime;
}

inline bool ioComp(const IOEvent& e1, const IOEvent& e2)
{
    return e1.time < e2.time;
}


class ProcessManagement
{
    // constructor
    public:
      ProcessManagement(vector<Process>& procList, list<int>& readyList) : m_procList(procList), m_readyList(readyList) {};


      // ************** PUBLIC MEMBER FUNCTIONS ******************

      // Returns true if there are new processes not in the main Process List
      bool moreProcessesComing() {return m_pending.size() != 0;}

      // Reads an input file and initailizes processes based on it
      void readProcessFile(const string& fname);

      // Moves process from new --> ready
      void activateProcesses(const int& time);

      // Returns true if there is a new process to run,
      // Also reassigns runningProc to the new process (process at start of readyList)
      bool newPtoRun(Process*& runningProc);

      // Returns true if an IOEvent is detected
      bool checkIOEvents(Process*& runningProc);

      // Returns the detected IOEvent reference
      IOEvent& whichIOevent(Process*& runningProc);

      // Returns true if all the processes have ran to completion
      bool allFinished();

      // Returns true if there are any currently running processes
      bool checkIfRunning();

    private:
      vector<Process> m_pending; // Holds new processes 

      vector<Process>& m_procList; // Refers to the main process list

      list<int>& m_readyList; // List of processor IDs used to determine which process should run
};

#endif