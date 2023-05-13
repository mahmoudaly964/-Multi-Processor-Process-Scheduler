#pragma once
#include<string>
#include"Pair.h"
using namespace std;
class PROCESS
{
	int PID;
	int AT;
	int RT;
	int CT;
	int TT;
	int TRT;
	int WT;
	int  N;
	int StartTime; //the time at which process first enter in any processor
	int Countsteps;// counter for the time steps that process take till now 
	int CountN;	   //number of times process use IO till now 
	bool IsKilled;
	bool IsParent;
	bool IsOrphan;
	Pair* IO;      //IO.first->IO-R   ,  IO.second->IO-D
	enum STATES;
	STATES state;
	int totalIoD;
	int RTF;
	int MaxW;
	PROCESS* Parent;
	PROCESS* Child1;
	PROCESS* Child2;
public:

	PROCESS(int ArrivalTime, int ID, int CPU_Time,	int Number);
	void set_RT(int response);
	void set_TT(int termination);
	void set_TRT(int termination, int ArrivalTime);
	void set_WT(int termination, int ArrivalTime, int CPU_Time);
	void set_IsKilled();
	void set_IsParent(bool Parent);
	void set_IsOrphan();
	int get_PID();
	int get_AT();
	int get_RT();
	int get_CT();
	int get_TT();
	int get_TRT();
	int get_WT();
	void set_starttime(int t); //added
	int get_starttime();	   //added
	int get_countsteps();	   //added
	void incrementCountsteps(int step);//added
	int get_countN();			//added
	void incrementcountN();     //added
	int get_N();                //added
	bool get_IsOrphan();
	void set_IO(int IO_R, int IO_D, int ind);
	bool get_IsKilled();
	bool get_IsParent();
	int get_IO_R(int indx) const;
	int get_IO_D(int) const;
	void set_state(string x);
	string get_state();
	void set_PID(int);
	void set_AT(int);
	void set_CT(int);
	void set_N(int);
	int get_totalIoD() const;
	int calculateTotalID();
	void setParent(PROCESS*);
	void setChild1(PROCESS*);
	void setChild2(PROCESS*);
	PROCESS* getParent() const;
	PROCESS* getChild1() const;
	PROCESS* getChild2() const;
	void DecrementIOD(int);
	~PROCESS();


};
