// “I have neither given nor received any unauthorized aid on this assignment”
//	please use g++ -std=c++0x MIPSsim.cpp -o MIPSsim to complie

/*
	# name: Xixi Ruan
	# course: 
	# Due: 02/17/2016
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <queue>

using namespace std;

typedef struct instruction {
	string opcode;
	int dest;
	int src1;
	int src2;			/* if type is ST, src2 is immediate value */
	int instructionNo;	/* instruction order */
	int type;			/* 0 is arithmetic value,  1 is ST */

} instruction;

typedef struct regData {
	int regNum;
	int regValue;
	bool canWrite;
	int instrNum;
}  regValue;

queue<instruction> INM;
instruction* INB;
instruction* AIB;
instruction* SIB;
instruction* PRB;
regValue* ADB;
queue<instruction> REB;

int reg[16];		//-1 means no value
int dataMM[16];

void decode() {

}

void read() {

}

void issue1() {

}

void ASU() {

}

void MLU1() {

}

void MLU2() {

}


void issue2() {

}

void adder() {

}

void store() {

}

void write() {

}

void initial(queue<instruction> &instrContainer) {
	ifstream regFile("registers.txt");
	ifstream dataMMFile("datamemory.txt");
	ifstream instrFile("instructions.txt");
	ofstream simulation("simulation.txt");

	bool isBreak = false;
	string tempfile;
	vector<int> temVal;

	/* inital register value */
	for(int i = 0; i < 16; i++)
	{	
		reg[i] = -1; 
		dataMM[i] = -1;
	}


	/* registers inital */
	while(getline(regFile, tempfile))
	{
		/* get ride of '<R' and '>' in the registers file */
		if(tempfile[tempfile.length()-2] == '>') 
            tempfile = tempfile.substr(2, tempfile.length()-4);
        else 
            tempfile = tempfile.substr(2, tempfile.length()-3);
		
		/* copy corresponding valu to the register */
		int value;
		stringstream ss(tempfile);	

		while(ss >> value) 
		{
			temVal.push_back(value);
			if(ss.peek() == ',')
				ss.ignore();
		}
		reg[temVal[0]] = temVal[1];
		cout << "registers " << temVal[0] << " value is " << dataMM[temVal[0]] << endl;
		temVal.clear();
	}

	/* datamemory inital */
	while(getline(dataMMFile, tempfile))
	{
		/* get ride of '<' and '>' in the datamemory file */
		if(tempfile[tempfile.length()-2] == '>') 
            tempfile = tempfile.substr(1, tempfile.length()-3);
        else 
            tempfile = tempfile.substr(1, tempfile.length()-2);
        
		//cout << tempfile << endl;
		
		//copy corresponding valu to memory address
		int value;
		stringstream ss(tempfile);

		while(ss >> value) 
		{
			temVal.push_back(value);
			if(ss.peek() == ',')
				ss.ignore();
		}
		dataMM[temVal[0]] = temVal[1];
		cout << "memory " << temVal[0] << " value is " << dataMM[temVal[0]] << endl;
		temVal.clear();
	}

	/* instruction inital */
	int instructionOrder = 0;
	while(getline(instrFile, tempfile))
	{
		/* get ride of '<' and '>' in the instruction file */
		if(tempfile[tempfile.length()-2] == '>') 
            tempfile = tempfile.substr(1, tempfile.length()-3);
        else 
            tempfile = tempfile.substr(1, tempfile.length()-2);
		cout << tempfile << endl;

		stringstream ss(tempfile);
		int position = 0;
		string tokenVal;
		instruction newInstr;
		instructionOrder++;

		/* get token values for the instruction */
		while(getline(ss, tokenVal, ','))
		{
			switch(position) 
			{
				case 0:		if(tokenVal.compare("ST") == 0) {
								newInstr.type = 1;
								cout << newInstr.type << "\t";
							}else {
								newInstr.type = 0;
							}
							newInstr.opcode = tokenVal;
							cout << newInstr.opcode << " \n";
							break;
				
				case 1:		
				case 2:		tokenVal = tokenVal.substr(1, tokenVal.length()-1);
							
							if(position == 1) {
								newInstr.dest = stoi(tokenVal, nullptr);
								
							}else {
								newInstr.src1 = stoi(tokenVal, nullptr);
							}
							break;
				case 3:		if(newInstr.type == 0)
								tokenVal = tokenVal.substr(1, tokenVal.length()-1); /* get ride of 'R' */
							newInstr.src2 = stoi(tokenVal, nullptr);
				default:	break;
			}
			position++;
		}
		instrContainer.push(newInstr);
	}
}

void simulatePrint(queue<instruction> instrContainer) {
	
	int step = 0;

	int startInstrNum = (instrContainer.size() >= 16)? 16 : instrContainer.size();
	bool isBreak = false;

	/* move first 16 instructions to INM only */
	for(int i = 0; i < startInstrNum; i++)
	{
		INM.push(instrContainer.front()	);
		instrContainer.pop();
	} 

	//while(1)
	for(int j = 0; j < 10; j++)
	{
		cout << "STEP " << step++ << ":" << endl;
		cout << "INM:";
		if(INM.size() != 0)
		{
			queue<instruction> tempInstr (INM);
			for(int i = 0; i < INM.size(); i++)
			{
				if(tempInstr.front().type == 0) 
					cout << "<" << tempInstr.front().opcode << ",R" << tempInstr.front().dest << ",R" << tempInstr.front().src1 << ",R" << tempInstr.front().src2 << ">";		
				else
					cout << "<" << tempInstr.front().opcode << ",R" << tempInstr.front().dest << ",R" << tempInstr.front().src1 << "," << tempInstr.front().src2 << ">";		
				tempInstr.pop();
					
				if(i == INM.size()-1)
					cout<<endl;
				else
					cout<<",";
			}
		}
		else
			cout << endl;
		cout << "INB:";
		if(INB != NULL)
		{

		}
		else
			cout << endl;
		cout << "AIB:";
		if(AIB)
		{

		}
		else
			cout << endl;
		cout << "SIB:";
		if(SIB)
		{

		}
		else
			cout << endl;
		cout << "PRB:";
		if(PRB)
		{

		}
		else
			cout << endl;
		cout << "ADB:";
		if(ADB)
		{

		}
		else
			cout << endl;
		cout << "REB:";
		if(REB.empty())
		{

		}
		else
			cout << endl;
		cout << "RGF:";
		cout << endl;
		cout << "DAM:";
		cout << endl;	
		if(isBreak) 
			break;
		if(INM.size() == 0)
			isBreak = true;
	}
}

int main()
{
	queue<instruction> instrContainer;
	initial(instrContainer);
	simulatePrint(instrContainer);

	cout << "queue size is " << instrContainer.size() << endl;

	return 0;
}










