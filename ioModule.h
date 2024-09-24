#ifndef FIVE_STATE_IO_H
#define FIVE_STATE_IO_H

#include<vector>
using namespace std;

#include "process.h"

struct IOInterrupt
{
    IOInterrupt() : ioEventID(99999), procID(99999) {};
    IOInterrupt(const unsigned int& eId, const unsigned int& pId) : ioEventID(eId), procID(pId) {};

    unsigned int ioEventID;
    unsigned int procID;
};

class IOModule
{
    public:
      // TO CLARIFY: m_intVec is tied to the doubly linked IOInterrupt list "interrupts" passed to the IOModule object at instantiation
      IOModule(list<IOInterrupt>& ioIntVec) : m_intVec(ioIntVec) {}

      // An IOEvent has occurred
      // arguments: 
      // final time step at which interrupt occurs (event finishes)
      // Interrupt is assigned the id of the event (based on which ioevent is in front)
      // and interrupt is assigned the id of the running process that just hit the IOEvent
      inline void submitIORequest(const int& curTimeStep, const IOEvent& ioEvent, const Process& proc)
      {
        m_pending.push_back(make_pair(curTimeStep + ioEvent.duration, IOInterrupt(ioEvent.id, proc.id)));
      }

      inline void ioProcessing(const int& curTimeStep)
      {
        // Loops for however many IOInterrupts are in IOModule.m_pending
        for(int i = 0, i_end = m_pending.size(); i < i_end; ++i)
        {
            if(curTimeStep == m_pending[i].first)
            {
                m_intVec.push_back(m_pending[i].second); // Add to generate interrupts
                m_pending.erase(m_pending.begin() + i); // Remove from busy events

                // we need to remove an iteration from the for loop (so we don't walk off the vector)
                --i;
                --i_end;
            }
        }
      }

    private:
      list<IOInterrupt>& m_intVec;

      vector<pair<int, IOInterrupt> > m_pending; 
};

#endif