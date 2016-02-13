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
	int instructionNo;	/* instruction order (needed to print in instruction order) */
	int type;			/* 0 is arithmetic value,  1 is ST */
} instruction;

typedef struct regData {
	int regNum;
	int regValue;
	bool canWrite;
	int instrNum;
	regData(){
		instrNum = -1000;
		canWrite = false;
	}
}  regValue;

queue<instruction*> INM;
instruction* INB;
instruction* AIB;
instruction* SIB;
instruction* PRB;
regValue* ADB;
queue<regValue*> REB;

int totalRGF = 0;	//check how many RGF number
int totalDAM = 0;	//check how many DAM number

int RGF[16];		//-1000 means no value
int DAM[16];

/* instruction works one after another (pipline?), need to check before assign value */
instruction* tempAIB = NULL;
instruction* tempSIB = NULL;
instruction* tempPRB = NULL;
regValue* tempADB = NULL;
instruction* tempINB = NULL;

/* decode and read one by one */

int read(int regNum) {	return RGF[regNum];  }

void decode() {
	
	if(INM.size() > 0)
	{
		tempINB = INM.front();
		int src1 = INM.front()->src1;
		int src2 = INM.front()->src2;
		//cout << "src2 is " << src2 << " and src1 is " << src1 << endl;
		switch(INM.front()->type)
		{
			case 0:		if(read(src2) != -1000 ) 	/* not ST, check both src1 and src2 */
							tempINB->src2 = read(src2);
						else {
							tempINB = NULL;
							break;
						}
			case 1:		if(read(src2) != -1000) { /* ST only need to check src2 */
							tempINB->src1 = read(src1);
							INM.pop();
						}
						else {
							tempINB = NULL;
							break;
						}			
		}				
	}
}


void issue1() {
	if(INB) {	/* need to be careful about the NULL */
		if(INB->type != 1 ) {
			tempAIB = INB;
		}
		else
			tempAIB = NULL;
	}
	else
		tempAIB = NULL;
}

void ASU(regValue* &tempData) {
	if(AIB) {

		tempData->regNum = AIB->dest;
		tempData->instrNum = AIB->instructionNo;
		//cout<<"there!!!" << endl;
		if(AIB->opcode.compare("ADD") == 0) {
			tempData->regValue = AIB->src1 + AIB->src2;
		}else if(AIB->opcode.compare("SUB") == 0) {
			tempData->regValue = AIB->src1 - AIB->src2;
		}
	}
	//cout<<"herere!!!" << endl;
}

void MLU1() {
	if(AIB) {
		if(AIB->opcode.compare("MUL") == 0) {
			tempPRB = AIB;
		}else {
			tempPRB = NULL;
		}
	}
	
}

void MLU2(regValue* &tempData) {
	if(PRB) {
		tempData->regNum = PRB->dest;
		tempData->regValue = PRB->src1 * PRB->src2;
		tempData->instrNum = PRB->instructionNo;
	}
}

void MLU2andAsu() {
	regValue* tempData = new regValue;
	if(PRB || AIB) {
		if(PRB && AIB) {
			regValue* tempData2 = new regValue;
			if(PRB->instructionNo > AIB->instructionNo)
			{	
				/* first ASU then MLU2 */
				ASU(tempData);			
				MLU2(tempData2);			
			}
			else {
				/* first MLU2 then ASU */
				MLU2(tempData);
				ASU(tempData2);
			}
			REB.push(tempData);
			REB.push(tempData2);
		}
		else if(PRB) {
			MLU2(tempData);
			REB.push(tempData);
		}
		else if(AIB) {
			ASU(tempData);
			REB.push(tempData);
		}
	}
	
}


void issue2() {
	if(INB) {
		if(INB->type == 1 ) {
			tempSIB = INB;
		}
		else
			tempSIB = NULL;
	}
	else
		tempSIB = NULL;
}

void adder() {
	if(SIB) {
		tempADB = new regValue;
		tempADB->regNum = SIB->dest;
		tempADB->regValue = SIB->src1 + SIB->src2;
		tempADB->instrNum = SIB->instructionNo;
	}else 
		tempADB = NULL;
}

void store() {
	if(ADB) {
		DAM[ADB->regValue] = RGF[ADB->regNum];	/* assume always have data from RPF */
	}
}

void write() {

}

void sync() {
	INB = tempINB;
	AIB = tempAIB;
	SIB = tempSIB; 
	PRB = tempPRB;
	ADB = tempADB;

}

void checkRGFNum() {
	for(int i = 15; i >= 0; i--)	{
		if(RGF[i] != -1000) {
			totalRGF = i;
			break;
		}
	}
	
}

void checkDAMNum() {
	for(int i = 15; i >= 0; i--)	{
		if(DAM[i] != -1000) {
			totalDAM = i;
			break;
		}
	}
}

void initial(queue<instruction*> &instrContainer) {
	ifstream regFile("registers.txt");
	ifstream dataMMFile("datamemory.txt");
	ifstream instrFile("instructions.txt");

	bool isBreak = false;
	string tempfile;
	vector<int> temVal;

	/* inital register value */
	for(int i = 0; i < 16; i++)
	{	
		RGF[i] = -1000; 
		DAM[i] = -1000;
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
		RGF[temVal[0]] = temVal[1];
		//cout << "registers " << temVal[0] << " value is " << RGF[temVal[0]] << endl;
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
		DAM[temVal[0]] = temVal[1];
		//cout << "memory " << temVal[0] << " value is " << DAM[temVal[0]] << endl;
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
		//cout << tempfile << endl;

		stringstream ss(tempfile);
		int position = 0;
		string tokenVal;
		instruction* newInstr = new instruction; /* has to create new object in order to assign address & value */
		newInstr->instructionNo = instructionOrder++;
		//instructionOrder++;

		/* get token values for the instruction */
		while(getline(ss, tokenVal, ','))
		{
			switch(position) 
			{

				case 0:		if(tokenVal.compare("ST") == 0) {
								newInstr->type = 1;
								//cout << newInstr->type << "\t";
							}else {
								newInstr->type = 0;
							}
							newInstr->opcode = tokenVal;
							//cout << newInstr->opcode << " \n";
							break;
				
				case 1:		
				case 2:		tokenVal = tokenVal.substr(1, tokenVal.length()-1);
							
							if(position == 1) {
								newInstr->dest = stoi(tokenVal, nullptr);
								
							}else {
								newInstr->src1 = stoi(tokenVal, nullptr);
							}
							break;
				case 3:		if(newInstr->type == 0)
								tokenVal = tokenVal.substr(1, tokenVal.length()-1); /* get ride of 'R' */
							newInstr->src2 = stoi(tokenVal, nullptr);
				default:	break;
			}
			position++;
		}
		instrContainer.push(newInstr);
	}

	regFile.close();
    dataMMFile.close();
    instrFile.close();
}

void simulatePrint(queue<instruction*> instrContainer) {
	
	ofstream simulation("simulation.txt");
	streambuf *coutbuffer = cout.rdbuf();
	//cout.rdbuf(simulation.rdbuf());
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
	for(int j = 0; j < 16; j++)
	{
		cout << "STEP " << step++ << ":" << endl;
		cout << "INM:";
		if(INM.size() != 0)
		{
			queue<instruction*> tempInstr (INM);
			for(int i = 0; i < INM.size(); i++)
			{
				if(tempInstr.front()->type == 0) 
					cout << "<" << tempInstr.front()->opcode << ",R" << tempInstr.front()->dest << ",R" << tempInstr.front()->src1 << ",R" << tempInstr.front()->src2 << ">";		
				else
					cout << "<" << tempInstr.front()->opcode << ",R" << tempInstr.front()->dest << ",R" << tempInstr.front()->src1 << "," << tempInstr.front()->src2 << ">";		
				tempInstr.pop();
					
				if(i+1 >= INM.size())
					cout<<endl;
				else
					cout<<",";
			}
		}
		else
			cout << endl;
		cout << "INB:";
		if(INB)
		{
			 cout << "<" << INB->opcode << ",R" << INB->dest << "," << INB->src1 << "," << INB->src2 << ">";
		}
		cout << endl;
		cout << "AIB:";
		if(AIB)
		{
			cout << "<" << AIB->opcode << ",R" << AIB->dest << "," << AIB->src1 << "," << AIB->src2 << ">";
		}
		cout << endl;
		cout << "SIB:";
		if(SIB)
		{
			cout << "<" << SIB->opcode << ",R" << SIB->dest << "," << SIB->src1 << "," << SIB->src2 << ">" ;
		}
		cout << endl;
		cout << "PRB:";
		if(PRB)
		{
			cout << "<" << PRB->opcode << ",R" << PRB->dest << "," << PRB->src1 << "," << PRB->src2 << ">";
		}
		cout << endl;
		cout << "ADB:";
		if(ADB)
		{
			cout << "<R" << ADB->regNum << "," << ADB->regValue << ">";
		}
		cout << endl;
		cout << "REB:";
		if(!REB.empty())
		{
			queue<regValue*> tempREB (REB);
			for(int i = 0; i < REB.size(); i++)
			{
				cout << "<R" << tempREB.front()->regNum << "," << tempREB.front()->regValue << ">";		
				tempREB.pop();		
				if(i+1 >= REB.size())
					cout<<endl;
				else
					cout<<",";
			}
		}
		else
			cout << endl;
		cout << "RGF:";
		checkRGFNum();
		for(int i = 0; i < 16; i++)
		{
			if(RGF[i] != -1000) {
				cout << "<R" << i << "," << RGF[i] << ">";
				if(i != totalRGF)
					cout << ',';
			}	
		}
		cout << endl;
		cout << "DAM:";
		checkDAMNum();
		for(int i = 0; i < 16; i++)
		{
			if(DAM[i] != -1000) {
				cout << "<" << i << "," << DAM[i] << ">";
				if(i != totalDAM)
					cout << ',';
			}
			
		}
		cout << endl;	
		if(isBreak) 
			break;
		if(INM.size() == 0)
			isBreak = true;
		cout << endl;

		decode();		/* decode will call read function */
		issue1();
		issue2();
		MLU1();
		adder();
		store();
		MLU2andAsu(); 	/*this include function ASU() and MLU2()*/
		write();
		sync();
		
	}	

	//cout.rdbuf(coutbuffer);
}

int main()
{
	queue<instruction*> instrContainer;
	initial(instrContainer);
	simulatePrint(instrContainer);

	//cout << "queue size is " << instrContainer.size() << endl;

	return 0;
}










