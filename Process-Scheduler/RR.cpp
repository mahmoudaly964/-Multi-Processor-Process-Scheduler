 #include "RR.h"
#include "Scheduler.h"

RR::RR(Scheduler*scheduler)
{
	STATE = 0;
	SchedPtr = scheduler;
	RUN = nullptr;
	TYPE = "RR";
	RSIZE = 0;
}

RR::~RR()
{
}

void RR::ScheduleAlgo()
{
		if (!STATE)//the processor is IDLE
		{
			PROCESS* front;
			if(RDY.dequeue(front))
			{
				RUN = front;
				STATE = 1;
				RUN->set_starttime(SchedPtr->get_TIMESTEP());
				RUN->incrementCountsteps(1);
			}
			
		}
		else {
			if (SchedPtr->Process_completion(RUN))
			{
				RUN = nullptr;
				STATE = 0;
			}
			else if (SchedPtr->IO_requesthandling(RUN))
			{
				RUN = nullptr;
				STATE = 0;
			}
			else {
				if (RUN->get_countsteps() < SchedPtr->getTimeSlice())
				{
					if ((RUN->get_CT() - RUN->get_countsteps()) < SchedPtr->getRTF())
					{
						RUN->incrementCountsteps(1);
					}
					else
					{
						//migrate to SJF
					}
				}
				else
				{
					if ((RUN->get_CT() - RUN->get_countsteps()) > SchedPtr->getRTF())
					{
						//migrate to SJF
					}
					else
					{
						RDY.enqueue(RUN);
					}

				}
			}

		}

}

void RR::PrintMyReady()
{
	RDY.printContents();
}

void RR::addToMyRdy(PROCESS* TMP)
{
	RDY.enqueue(TMP);
	ExpectedFinishTime += TMP->get_CT();
	RSIZE++;
}

bool RR::PromoteProcess(int x)
{
	if (!STATE && !RDY.isEmpty())// the processor is IDLE
	{
		PROCESS* toberun;
		//If RDY.peek() exists I want to check if the timestep is equal AT, if this is the case return false
		if(RDY.peek(toberun))
		if (x == toberun->get_AT())
			return false;
		if (RDY.dequeue(toberun))
		{
			RUN = toberun;
			STATE = 1;
			RSIZE--;
			return true;
		}
	}
	return false;
}
