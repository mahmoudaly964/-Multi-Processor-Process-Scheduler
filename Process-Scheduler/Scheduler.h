#pragma once
#include "LinkedQueue.h"
#include "PROCESS.h"
#include "PROCESSOR.h"
#include "UI.h"
#include "PROCESS.h"
#include "FCFS.h"
#include "SJF.h"
#include "RR.h"
class Scheduler
{
	int TIMESTEP;
	PROCESSOR** ListOfProcessors;
	int totalProcessors;
	//These 3 queues are common across all processes
	LinkedQueue<PROCESS*> NEW;
	LinkedQueue<PROCESS*> BLK;
	LinkedQueue<PROCESS*> TRM;
	UI* UIptr;
	int FCFS_Count;
	int SJF_Count;
	int RR_Count;
	int TimeSlice;
	int RTF;
	int MaxW;
	int STL;
	int Forkability;
	int ProcessesCount;
	LinkedQueue<int>ProcessesToBeKilled;
	LinkedQueue<int>TimeOfDeath;
	int BLK_Count;
	int RUN_Count;
	int TRM_Count;
	int RunningCount; // Total number of currently running processes
	int* Running; //This is an arry containg IDs of Running processes from each processor

public:
	void LoadData(); // open the input file and load all processes into NEW list
	void SaveData(); // produce the output file at end of simulation
	void CreateProcessors(int, int, int);
	void Print(char); //Decides which mode ([I]nteractive - [S]ilent - Step[B]yStep) 
	void Add_toblocklist(PROCESS*blockedprocess);
	void Add_toterminatedlist(PROCESS* terminatedprocess);
	int get_TIMESTEP();
	int getTimeSlice();
	int getRTF();
	Scheduler();
	void UpdateRunningProcesses(); //Iterates over all processors and adds Currently Running Processes
	~Scheduler();
};

