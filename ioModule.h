#ifndef FIVE_STATE_IO_H
#define FIVE_STATE_IO_H

#include<vector>
using namespace std;

#include "process.h"

// gives each interrupt the info on what its own ID is and what process ID its associated with
struct IOInterrupt
{
    IOInterrupt() : ioEventID(99999), procID(99999) {};
    IOInterrupt(const unsigned int& eId, const unsigned int& pId) : ioEventID(eId), procID(pId) {};

    unsigned int ioEventID;
    unsigned int procID;
};

// The IOModule object keeps track of the IOInterrupts in a doubly linked list
class IOModule
{
    public:
      // TO CLARIFY: m_intVec is tied to the doubly linked IOInterrupt list "interrupts" passed to the IOModule object at instantiation
      IOModule(list<IOInterrupt>& ioIntVec) : m_intVec(ioIntVec) {}

      // An IOEvent has occurred
      inline void submitIORequest(const int& curTimeStep, const IOEvent& ioEvent, const Process& proc)
      {
        // arguments: final time step at which interrupt occurs (event finishes)
        // Interrupt is assigned the id of the event (based on which ioevent is in front)
        // and interrupt is assigned the id of the running process that just hit the IOEvent
        m_pending.push_back(make_pair(curTimeStep + ioEvent.duration, IOInterrupt(ioEvent.id, proc.id)));
      }

      inline void ioProcessing(const int& curTimeStep)
      {
        // Loops for however many IOInterrupts are in IOModule.m_pending
        for(int i = 0, i_end = m_pending.size(); i < i_end; ++i)
        {
            if(curTimeStep == m_pending[i].first)
            {
                // Add IOInterrupt to back of interrupts
                m_intVec.push_back(m_pending[i].second);

                // Removes the i'th element from m_pending (the member variable)
                m_pending.erase(m_pending.begin() + i);


                // Say m_pending size was 3 when the for loop began --> loop ends when i == 3
                // That means the new m_pending size is actually 2 (because of m_pending.erase())

                // because we're modifying m_pending by removing the <int, IOInterrupt> at position i,
                // we need to remove an iteration from the for loop (so we don't walk off the vector)
                --i;
                --i_end;
            }
        }
      }

    private:
      // Refers to the list of interrupts who indicate that an IOEvent has a finished
      // and that the corresponding process may be released
      list<IOInterrupt>& m_intVec;

      // dyanmic array containing ongoing events (pending interrupts)
      // int represents the time step at which the Event will finish
      vector<pair<int, IOInterrupt> > m_pending; 
};

#endif