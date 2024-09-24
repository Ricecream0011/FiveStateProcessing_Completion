#include "processMgmt.h"

void ProcessManagement::readProcessFile(const string& fname)
{
    stringstream ss;
    ifstream in(fname.c_str());
    string line, strItem;
    Process proc;
    unsigned int ioIDctrl(0), procIDctrl(0);
    int ioTime, ioDur;

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

        proc.id = procIDctrl; // setting the first process ID to 0
        ++procIDctrl;

        // initializing processor member variables
        ss >> proc.arrivalTime;
        ss >> proc.reqProcessorTime;

        // this while loop pushes a process' ioevents into the back of its IOEvents list
        proc.ioEvents.clear(); 
        while(ss >> ioTime)
        {
            ss >> ioDur;

            proc.ioEvents.push_back(IOEvent(ioTime, ioDur, ioIDctrl));
            ++ioIDctrl;
        }
        proc.ioEvents.sort(ioComp);

        m_pending.push_back(proc);
    }

    sort(m_pending.begin(), m_pending.end(), procComp);
}

//////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////

bool ProcessManagement::newPtoRun(Process*& runningProc){
    if(m_readyList.size() != 0)
    {
        unsigned int rfrontID = m_readyList.front(); // ID of readyList[0]
        for(size_t i=0; i<m_procList.size(); i++) // iterate through processList
        {
            if(m_procList[i].id == rfrontID)
            {
                runningProc = &m_procList[i];
                return true;
            }
        }
    }

    runningProc = nullptr;
    return false;
}

//////////////////////////////////////////////////////////

bool ProcessManagement::checkIOEvents(Process*& runningProc){
    for(IOEvent& indexedEvent : runningProc->ioEvents)
    {
        if (runningProc->processorTime == indexedEvent.time)
        {
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////

IOEvent& ProcessManagement::whichIOevent(Process*& runningProc)
{
    for(IOEvent& indexedEvent : runningProc->ioEvents)
    {
        if (runningProc->processorTime == indexedEvent.time)
        {
            return indexedEvent;
        }
    }
    
    cerr << "no ioEvent found...?" ;
    IOEvent placeholder; // because this function must return an IOEvent reference
    IOEvent& refHolder = placeholder;
    return refHolder;
}

//////////////////////////////////////////////////////////

bool ProcessManagement::allFinished(){
    for(size_t i=0; i<m_procList.size(); i++)
    {
        if(m_procList[i].state != done) { return false;}
    }
    return true;
}

//////////////////////////////////////////////////////////

bool ProcessManagement::checkIfRunning()
{
    for(Process& iterProc : m_procList)
    {
        if(iterProc.state == processing) { return true;}
    }
    return false;
}