#include "processMgmt.h"

void ProcessManagement::readProcessFile(const string& fname)
{
    stringstream ss;
    ifstream in(fname.c_str());
    string line, strItem;
    Process proc;
    unsigned int ioIDctrl(0), procIDctrl(0);
    int ioTime, ioDur;

    // REMINDER: m_pending is a private <vector> owned by ProcessManagement object
    m_pending.clear();

    if(!in.good())
    {
        cerr << "initProcessSetFromFile error     unable to open file \"" << fname << "\"" << endl;
        return;
    }

    m_pending.reserve(20);

    while(getline(in, line))
    {
        ss.clear();
        ss.str(line);

        proc.id = procIDctrl; // setting process ID to 0 initially
        ++procIDctrl;

        // initializing
        ss >> proc.arrivalTime; // first value
        ss >> proc.reqProcessorTime; // second value

        // REMINDER ioEvents is a doubly linked list (list<IOEvents) belonging to Processor objects
        // so each process has its own set of interrupting io events
        proc.ioEvents.clear(); 
        while(ss >> ioTime) // third value
        {
            ss >> ioDur; // fourth value

            proc.ioEvents.push_back(IOEvent(ioTime, ioDur, ioIDctrl));
            ++ioIDctrl;
        }
        proc.ioEvents.sort(ioComp);

        m_pending.push_back(proc);
    }

    sort(m_pending.begin(), m_pending.end(), procComp);
}


// Here the m_pending list is basically being used as a reverse stack to 
// push the released process to the back of the ready queue while also removing
// the released pending process from the back of the vector
void ProcessManagement::activateProcesses(const int& time)
{
    if(m_pending.size() > 0)
    {
        while(m_pending.back().arrivalTime == time)
        {
            m_procList.push_back(m_pending.back());
            m_pending.pop_back();
        }
    }
}