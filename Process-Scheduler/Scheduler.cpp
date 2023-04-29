#include "Scheduler.h"
#include <fstream>
#include <sstream>
#include <random>
using namespace std;
class FCFS;
void Scheduler::LoadData()
{
	string FileName = UIptr->ReadFileName();
	ifstream myFile(FileName, ios::in);
	if (!myFile.is_open())
	{
		UIptr->PrintMessage("File Not Found!");
		return;
	}
	File = FileName;
	myFile >> FCFS_Count >> SJF_Count >> RR_Count;
	totalProcessors = FCFS_Count + SJF_Count + RR_Count;
	myFile >> TimeSlice;
	myFile >> RTF >> MaxW >> STL >> Forkability;
	myFile >> ProcessesCount;
	LiveTotalProcesses = ProcessesCount; //updated 
	for (int i = 0; i < ProcessesCount; i++)
	{
		int AT, PID, CT, N;
		myFile >> AT >> PID >> CT >> N;
		PROCESS* tmp = new PROCESS(AT, PID, CT, N); //I assume the constructor to take these values
		NEW.enqueue(tmp);//Processes are first added to NEW queue
		if (N > 0)
		{ 
			string IO;
			myFile >> IO;
			stringstream ss(IO);
			for (int j = 0; j < N; j++)
			{
				int x, y;
				char c1, c2, c3;
				ss >> c1 >> x >> c2 >> y >> c3;
				tmp->set_IO(x, y, j);
				if (j + 1 < N)
				{
					char c4;
					ss >> c4;
				}
			}
		}
	}
	string ignore1;
	string ignore2;
	myFile >> ignore1 >> ignore2;
	int a, b;
	while (myFile >> a)
	{
		myFile >> b;
		TimeOfDeath.enqueue(a);
		ProcessesToBeKilled.enqueue(b);
	}
	myFile.close();
}
void Scheduler::SaveData()
{
	File = "Output_" + File;
	ofstream OutputFile;
	OutputFile.open(File, ios::out);
	if (OutputFile.is_open())
	{
		OutputFile << "TT    PID    AT    CT    IO_D        WT    RT    TRT" << endl;
		while (!TRM.isEmpty())
		{
			PROCESS* temp = TRM.dequeue(temp);
			OutputFile << temp->get_TT() << "    " << temp->get_PID() << "    " << temp->get_AT()
				<< "    " << temp->get_totalIoD() << "        " << temp->get_WT() << "    " << temp->get_TRT() << endl;
		}
		OutputFile << "Processes: " << LiveTotalProcesses << endl;
		OutputFile << "Avg WT = " << AvgWaitingTime << ",      Avg RT = " << AvgResponseTime << ",      Avg TRT = " << AvgTRT << endl;
		OutputFile << "Migration % :        RTF = " << RTF << "%,      MaxW = " << MaxW << "%" << endl;
		OutputFile << "Forked Processes: " << ForkPercent << "%" << endl;
		OutputFile << "Killed Processes: " << KillPercent << "%" << endl;
		OutputFile << endl;
		OutputFile << "Processors: " << totalProcessors << " [" << FCFS_Count << " FCFS, " << SJF_Count << " SJF, "
			<< RR_Count << " RR]" << endl;
		OutputFile << "Processors Load" << endl;
		for (int i = 1; i < totalProcessors; i++)
		{
			OutputFile << "p" << i << "=" << ListOfProcessors[i - 1]->getPLoad() << "%,  ";
		}
		OutputFile << "p" << totalProcessors << "=" << ListOfProcessors[totalProcessors - 1]->getPLoad() << "%\n";
		OutputFile << endl;
		OutputFile << "Processors Utiliz" << endl;
		for (int i = 1; i < totalProcessors; i++)
		{
			OutputFile << "p" << i << "=" << ListOfProcessors[i - 1]->getPUtil() << "%,  ";
		}
		OutputFile << "p" << totalProcessors << "=" << ListOfProcessors[totalProcessors - 1]->getPUtil() << "%\n";
		OutputFile << "Avg utilization = " << AvgUtilization << "%";
		OutputFile.close();
	}

}
void Scheduler::CreateProcessors(int FC, int SJ, int R)
{
	int counter = 0;
	ListOfProcessors = new PROCESSOR* [FC + SJ + R];
	for (int i = 0; i < FC; i++)
	{
		FCFS* tmp = new FCFS();
		ListOfProcessors[counter++] = tmp;
	}
	for (int i = 0; i < SJ; i++)
	{
		SJF* tmp = new SJF(this);
		ListOfProcessors[counter++] = tmp;
	}
	for (int i = 0; i < R; i++)
	{
		RR* tmp = new RR(this);
		ListOfProcessors[counter++] = tmp;
	}
}

void Scheduler::Print(char z)
{
	if (z == 'I')
		UIptr->printInteractive(TIMESTEP, ListOfProcessors, totalProcessors, BLK, BLK_Count, Running, RunningCountIndex, TRM, TRM_Count, RunningCount, ProcessesCount);
	else if (z == 'B')
		UIptr->printStepByStep(TIMESTEP, ListOfProcessors, totalProcessors, BLK, BLK_Count, Running, RunningCountIndex, TRM, TRM_Count, RunningCount);
	else if (z == 'S')
	{
		UIptr->printSilent();
		SaveData();
	}
}

void Scheduler::Add_toblocklist(PROCESS* blockedprocess)
{
	BLK.enqueue(blockedprocess);
	BLK_Count++;
}

void Scheduler::Add_toterminatedlist(PROCESS* temp)
{
	TRM.enqueue(temp, temp->get_TT());
	TRM_Count++;
}

int Scheduler::get_TIMESTEP()
{
	return TIMESTEP;
}
int Scheduler::getTimeSlice()
{
	return TimeSlice;
}

int Scheduler::getRTF()
{
	return RTF;
}
Scheduler::Scheduler()
{
	TIMESTEP = 1;
	Running = new PROCESS*[totalProcessors];
	for (int i = 0; i < totalProcessors; i++)
	{
		Running[i] = nullptr;
	}
	TRM_Count = 0;
	BLK_Count = 0;
	FCFS_Count = 0;
	SJF_Count = 0;
	RR_Count = 0;
	ProcessesCount = 0;
	RunningCount = 0;
	RunningCountIndex = 0;
	LiveTotalProcesses = 0;
}
bool Scheduler:: IO_requesthandling(PROCESS* RUN) {
	if (RUN->get_N() > 0 && RUN->get_countN() <= RUN->get_N())
	{
		if (RUN->get_countsteps() == RUN->get_IO_R(RUN->get_countN()))
		{
			RUN->incrementCountsteps(1);
			RUN->incrementcountN();
			Add_toblocklist(RUN);
			RunningCount--;
			return true;
		}
	}
	return false;
}

bool Scheduler::Process_completion(PROCESS* RUN)
{
	if (RUN->get_countsteps() > RUN->get_CT())
	{
		Add_toterminatedlist(RUN);
		RunningCount--;
		return true;
	}
	return false;
}

bool Scheduler::MIG_RR_SJF(PROCESS* run)
{
	int SJF_INDEX;
	if ((run->get_CT() - run->get_countsteps()) < RTF)
	{
		for (int i = 0; i < totalProcessors; i++)
			if (ListOfProcessors[i]->getType() == "SJF")
			{
				SJF_INDEX = i;
				break;
			}
		ListOfProcessors[SJF_INDEX]->addToMyRdy(run);
		return true;
	}
	return false;
}

bool Scheduler::MIG_FCFS_RR(PROCESS* run)
{
	int RR_INDEX;
	if (run->get_WT()>MaxW)
	{
		for (int i = 0; i < totalProcessors; i++)
			if (ListOfProcessors[i]->getType() == "RR")
			{
				RR_INDEX = i;
				break;
			}
		ListOfProcessors[RR_INDEX]->addToMyRdy(run);
		return true;
	}
	return false;
}


void Scheduler::SIMULATE()
{
	int count = 0; //count to randomize process in the processors
	LoadData(); //Step 1 Load Data from Input File
	CreateProcessors(FCFS_Count, SJF_Count, RR_Count);
	while (!AllDone())
	{
		CheckNewArrivals(count); //Step 2 Move processes with AT equaling Timestep to RDY Queue (Their time has come :) )
		PromoteRdyToRun(); //Iterates over all processors and move Rdy processes to Running if possible
		AddToRunning();   //Iterates over all runnings of processors and add them to RUNNING array
		AllocatingProcesses(count); //Iterates over all processes and move them based on randomizer result
		Print('I'); // Print in Interactive Mode
		TIMESTEP++;
	}
	
}

void Scheduler::CheckNewArrivals(int&count)
{
	PROCESS* tmp;
	if(NEW.peek(tmp))
	while(tmp->get_AT() == TIMESTEP)
	{
		if (!NEW.dequeue(tmp)) {
			break;
		}
		ListOfProcessors[count]->addToMyRdy(tmp); //Adds process to ready of each processor (Randomly ofc)
		count = (count + 1) % totalProcessors;
		NEW.peek(tmp);
	}
}

void Scheduler::PromoteRdyToRun()
{
	for (int i = 0; i < totalProcessors; i++)
	{
		if (ListOfProcessors[i]->getRSIZE() > 0)
			if (ListOfProcessors[i]->PromoteProcess(TIMESTEP))
				RunningCount++;
				
	}
}

int Scheduler::Randomize()
{
	random_device rd;
	mt19937 gen(rd());

	// Define the distribution for the random number
	uniform_int_distribution<> dis(1, 100);

	// Generate and return the random number
	return dis(gen);
}

void Scheduler::AllocatingProcesses(int&count)
{
	for (int i = 0; i < RunningCountIndex; i++) 
		//RunningCountIndex may be changed to become totalProcessors (if agree with me do it)
	{
		int random = Randomize();
		if (!Running[i])
			continue;
		if (RunningCount > 0 && i<totalProcessors)
		{
			if (random >= 1 && random <= 15)
			{
				//MOVE Running[i] to BLK list
				BLK.enqueue(Running[i]);
				BLK_Count++;
				for (int j = 0; j < totalProcessors; j++) //Remove running process from it's original processor RUN*
				{
					ListOfProcessors[j]->ResetRunningProcess(Running[i]->get_PID());
				}
				Running[i] = nullptr;
				if (RunningCount > 0)
					RunningCount--;

			}
			else if (random >= 20 && random <= 30)
			{
				//MOVE Running[i] to RDY list of any processor

				ListOfProcessors[count]->addToMyRdy(Running[i]);
				count = (count + 1) % totalProcessors;
				for (int j = 0; j < totalProcessors; j++)
				{
					ListOfProcessors[j]->ResetRunningProcess(Running[i]->get_PID());
				}
				Running[i] = nullptr;
				if (RunningCount > 0)
					RunningCount--;

			}
			else if (random >= 50 && random <= 60)
			{
				//MOVE Running[i] to TRM list

				TRM.enqueue(Running[i], Running[i]->get_TT());
				TRM_Count++;
				for (int j = 0; j < totalProcessors; j++)
				{
					ListOfProcessors[j]->ResetRunningProcess(Running[i]->get_PID());
				}
				Running[i] = nullptr;
				if (RunningCount > 0)
					RunningCount--;

			}
		}
	}
	
	int random = Randomize();
	if (BLK_Count > 0 && random <= 10)
	{
		//Move BLK_Front to RDY
		/*if (BLK.isEmpty())
			return;*/
		PROCESS* Tmp;
		BLK.dequeue(Tmp);
		BLK_Count--;
		ListOfProcessors[count]->addToMyRdy(Tmp);
		count = (count + 1) % totalProcessors;
		Tmp = nullptr;
	}
	int FCFS_random = Randomize();
	FCFS_random %= LiveTotalProcesses;
	FCFS_random++;
	for (int i = 0; i < FCFS_Count; i++)
	{
		PROCESS* KILLED = dynamic_cast<FCFS*>(ListOfProcessors[i])->KillRandomly(FCFS_random);
		if (KILLED)
		{
			TRM.enqueue(KILLED, KILLED->get_TT());
			TRM_Count++;
		}
	}
}

bool Scheduler::AllDone()
{
	
	return TRM_Count == ProcessesCount;
}

void Scheduler::AddToRunning()
{
	for (int i = 0; i < totalProcessors; i++)  
	{
		if (ListOfProcessors[i]->getState() && !ListOfProcessors[i]->getRunningInSched())
		{
			if (RunningCountIndex >= totalProcessors)
			{
				int newRunningCountIndex = RunningCountIndex % totalProcessors;
				if (Running[newRunningCountIndex] == nullptr) {
					Running[newRunningCountIndex] = ListOfProcessors[i]->getCurrentlyRunning();
					RunningCountIndex++;
					ListOfProcessors[i]->setRunningInSched(1);
				}
				else {
					RunningCountIndex++;
					newRunningCountIndex = RunningCountIndex % totalProcessors;
					if (Running[newRunningCountIndex] == nullptr) {
						Running[newRunningCountIndex] = ListOfProcessors[i]->getCurrentlyRunning();
						RunningCountIndex++;
						ListOfProcessors[i]->setRunningInSched(1);
					}
				}
			}
			else
			{
				Running[RunningCountIndex++] = ListOfProcessors[i]->getCurrentlyRunning();
				ListOfProcessors[i]->setRunningInSched(1);
			}
		}
	}
	
}

void Scheduler::WorkStealing()
{
	if (TIMESTEP % STL == 0) //every STL timestep
	{
		int LQF=-1;
		int SQF=1e9;
		int indxProcessorOfSQF;
		int indxProcessorOfLQF;
		for (int i = 0;i < totalProcessors;i++) {
			int cur = ListOfProcessors[i]->getExpectedFinishTime();
			if (cur >= LQF) {
				LQF = cur;
				indxProcessorOfLQF = i;
			}
			if (cur <= SQF) {
				SQF = cur;
				indxProcessorOfSQF = i;
			}
		}
		StealLimit = (LQF - SQF) / LQF;
		while (StealLimit > 0.4) {
			PROCESS* topLQF = ListOfProcessors[indxProcessorOfLQF]->removeTopOfMyRDY();
			ListOfProcessors[indxProcessorOfSQF]->addToMyRdy(topLQF);
			LQF = ListOfProcessors[indxProcessorOfLQF]->getExpectedFinishTime();
			SQF = ListOfProcessors[indxProcessorOfSQF]->getExpectedFinishTime();
			StealLimit = (LQF - SQF) / LQF;
		}
	}
}

Scheduler::~Scheduler()
{
	Running = nullptr;
	delete[] Running;
	for (int i = 0; i < totalProcessors; i++)
	{
		if (ListOfProcessors[i])
		{
			delete ListOfProcessors[i];
			ListOfProcessors[i] = nullptr;
		}
	}
	delete[] ListOfProcessors;
	TRM.~LinkedPriorityQueue();
}