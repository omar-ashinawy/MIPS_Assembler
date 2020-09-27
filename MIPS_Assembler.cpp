#include <algorithm>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <sstream>
using namespace std;
map<string, string> IArithmeticAndLogic = { { "addi", "001000" }, { "ori", "001101" } };
map<string, string> ILoadAndStore = { { "lw", "100011" }, { "sw", "101011" }};
map<string, string> Rtype = { { "add", "100000" }, { "and", "100100" }, { "or", "100101" }, { "sll", "000000" }, { "jr", "001000" }, { "slt", "101010" } };
map<string, string> Register = { { "$0", "00000" }, { "$zero", "00000" }, { "$at", "00001" }, { "$v0", "00010" }, { "$v1", "00011" }, { "$a0", "00100" }, { "$a1", "00101" }, { "$a2", "00110" }, { "$a3", "00111" }, { "$t0", "01000" }, { "$t1", "01001" }, { "$t2", "01010" }, { "$t3", "01011" }, { "$t4", "01100" }, { "$t5", "01101" }, { "$t6", "01110" }, { "$t7", "01111" }, { "$s0", "10000" }, { "$s1", "10001" }, { "$s2", "10010" }, { "$s3", "10011" }, { "$s4", "10100" }, { "$s5", "10101" }, { "$s6", "10110" }, { "$s7", "10111" }, { "$t8", "11000" }, { "$t9", "11001" }, { "$k0", "11010" }, { "$k1", "11011" }, { "$gp", "11100" }, { "$sp", "11101" }, { "$fp", "11110" }, { "$ra", "11111" } };

// files must be passed by reference
int getNoOfLines(ifstream &file)
{
	string x; int noOfLines =1;
	while (getline(file, x))
	{
		noOfLines++;
	}
	return noOfLines;
}

string getRegistersAndInstruction(string instructionLine,string &rs,string &rt,string &rd,string &shamt)
{
	string instruction;

	if (instructionLine.substr(0, 3) != "nop")
	{
		
		if (instructionLine.substr(0,1) != "j" && instructionLine.substr(0,3)!= "jal")
		{
			instruction = instructionLine.substr(0, instructionLine.find("$"));
 			instructionLine = instructionLine.substr(instructionLine.find("$"));
		}
		else if (instructionLine.substr(0, 1) == "j")
		{
			instruction = "j";
			instructionLine = instructionLine.substr(1);
		}
		else if (instructionLine.substr(0, 3) == "jal")
		{
			instruction = "jal";
			instructionLine = instructionLine.substr(3);
		}
	}
	else
	{
		instruction = "nop";
		instructionLine = "-";
	}
	
	if (Rtype.find(instruction) != Rtype.end())
	{
		if (instruction == "jr")
		{
			rs = instructionLine;
			rt = rd = "$0";
			shamt = "00000";
		}
		else if (instruction == "sll")
		{
			rs = "00000";
			rd = instructionLine.substr(0, instructionLine.find(","));
			instructionLine.erase(0, instructionLine.find(",") + 1);
			rt = instructionLine.substr(0, instructionLine.find(","));
			instructionLine.erase(0, instructionLine.find(",") + 1);
			shamt = instructionLine;
		}
		else
		{
			rd = instructionLine.substr(0, instructionLine.find(","));
			instructionLine.erase(0, instructionLine.find(",") + 1);
			rs = instructionLine.substr(0, instructionLine.find(","));
			instructionLine.erase(0, instructionLine.find(",") + 1);
			rt = instructionLine.substr(0, instructionLine.find(","));
			instructionLine.erase(0, instructionLine.find(",") + 1);
			shamt = "00000";
		}
	}
	else if (ILoadAndStore.find(instruction) != ILoadAndStore.end())
	{
		rt = instructionLine.substr(0, instructionLine.find(","));
		instructionLine.erase(0, instructionLine.find(",") + 1);
		rd = instructionLine.substr(0, instructionLine.find("("));//rd is used to carry the value of immediate. 
		rs = instructionLine.substr(instructionLine.find("(") + 1, 3);
	}
	else if (IArithmeticAndLogic.find(instruction) != IArithmeticAndLogic.end())
	{
		rt = instructionLine.substr(0, instructionLine.find(","));
		instructionLine.erase(0, instructionLine.find(",") + 1);
		rs = instructionLine.substr(0, instructionLine.find(","));
		rd = instructionLine.substr(instructionLine.find(",") + 1, instructionLine.length());//rd is used to carry the value of immediate.
	}
	else if (instruction == "beq" || instruction == "bne")
	{
		rs = instructionLine.substr(0, instructionLine.find(","));
		instructionLine.erase(0, instructionLine.find(",") + 1);
		rt = instructionLine.substr(0, instructionLine.find(","));
		rd = instructionLine.substr(instructionLine.find(",") + 1, instructionLine.length());//rd is used to carry the lable.
	}
	else if (instruction == "jal" || instruction == "j")
	{
		rd = instructionLine;
	}

	return instruction;

}
string decToBinary(int n, int x)
{
	string binary="";
	int a = n;
	// Size of an integer is assumed to be x bits 
	for (int i = x-1; i >= 0; i--) 
	{
		int k = n >> i;
		n = a;
		if (k & 1)
			binary+= "1";
		else
			binary+= "0";
	}
	return binary;
}
int main()
{
	ofstream machinecodeFile("C:\Users\omar\Desktop\machinecodeFile1.txt");
	string machineCode = "";
	ifstream assemblyFile("C:\Users\omar\Desktop\assembly1.txt");
	int noOfLines = getNoOfLines(assemblyFile);
	assemblyFile.close();
	assemblyFile.open("C:\Users\omar\Desktop\assembly1.txt");
	vector <string> assembly(noOfLines);
	vector <string>::iterator it = assembly.begin(); //::is called scope resolution
	int index;
	map<string, int> Lables;
	if (assemblyFile.is_open())
	{
		while (getline(assemblyFile, *it))
		{
			it->erase(remove(it->begin(), it->end(), ' '), it->end());
			for (int i = 0; i < it->length(); i++)
			{
				if (it->at(i) >= 65 && it->at(i) <= 92)
				{
					it->at(i) = it->at(i) + 32;
				}
			}
			index = distance(assembly.begin(), it); //The distance(first, last) function tells you how many items are between the iterator at first and last . Note that pointers are iterators, random-access iterators to be specific.
			if (it->find(":") != string::npos)
			{
				Lables[it->substr(0, it->find(":"))] = index;
				*it = it->substr(it->find(":") + 1);
			}
			if (it->find("#") != string::npos)
			{
				*it = it->substr(0, it->find("#"));
			}
			it++;
		}
	}

	assemblyFile.close();
	assembly.pop_back();
	
	for (it = assembly.begin();it != assembly.end(); it++)
	{
			index = distance(assembly.begin(), it);
			string instruction = "";
			string rs = "", rt = "", rd = "", shamt = "";
			
			instruction = getRegistersAndInstruction(*it, rs, rt, rd, shamt);
			//cout << instruction << '-' << rs << '-' << rt << '-' << rd << '-' << shamt << '-' << endl;
			if (Rtype.find(instruction) != Rtype.end())
			{
				machineCode += "000000";
				if (instruction == "jr")
				{
					machineCode += Register[rs] + "000000000000000" + Rtype["jr"];
				}
				else if (instruction == "sll")
				{
					machineCode += "00000" + Register[rt] + Register[rd] + decToBinary(atof(shamt.c_str()), 5) + Rtype["sll"];
				}
				else
				{
					machineCode += Register[rs] + Register[rt] + Register[rd] + "00000" + Rtype[instruction];
				}
			}
			else if (ILoadAndStore.find(instruction) != ILoadAndStore.end())
			{
				//rd carrying the immediate value
				machineCode += ILoadAndStore[instruction] + Register[rs] + Register[rt] + decToBinary(atof(rd.c_str()), 16);
			}
			else if (IArithmeticAndLogic.find(instruction) != IArithmeticAndLogic.end())
			{
				machineCode += IArithmeticAndLogic[instruction] + Register[rs] + Register[rt] + decToBinary(atof(rd.c_str()), 16);
			}
			else if (instruction == "beq")
			{
				machineCode += "000100" + Register[rs] + Register[rt] + decToBinary((Lables[rd] - index - 1), 16);

			}
			else if (instruction == "bne")
			{
				machineCode += "000101" + Register[rs] + Register[rt] + decToBinary((Lables[rd] - index - 1), 16);
			}
			else if (instruction == "jal")
			{
				machineCode += "000011" + decToBinary((Lables[rd]), 26);

			}
			else if (instruction == "j")
			{
				machineCode += "000010" + decToBinary((Lables[rd]), 26);
			}
			
			machinecodeFile << machineCode << "\n";
			machineCode = "";
	}

		assemblyFile.close();
		machinecodeFile.close();
		/*cout << "******************************************************\n";
		
		for (it = assembly.begin(); it != assembly.end(); it++)
		{
			cout << *it << endl;
		}
		cout << "******************************************************\n";
		
		for (map<string, int>::iterator LablesIt = Lables.begin(); LablesIt != Lables.end(); LablesIt++)
		{
			cout << LablesIt->first << "--" << LablesIt->second << endl;
		}
*/
		system("pause");
	
	return 0;
}