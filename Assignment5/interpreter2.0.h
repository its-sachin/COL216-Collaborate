#pragma once

#include "Queue.cpp"

class MIPS{

protected:
    unordered_map<int, vector<string>> instructions;
    unordered_map<int,int> lineCount;
    string regSteps = "";

    int coreNum;

    //  -------------------------ass4-addition-------------------------------------------

    unordered_map<string, int> labelLine;

    // --------------------------------------------------------------------------------------------------------

    string inst[10] = {"add","addi","sub","mul","beq","bne","slt","lw","sw","j"};
    int instCount[10] = {0};

public:
    int mainInst;

    unordered_map<int, vector<string>> getInstruction();
    int getLabelLine(string name);
    int getLine(int pc);
    void incInstCount(int instruction);
    vector<string> getInst(int line);
    void setInst(int line, vector<string> input);
    void setInstLine(int addressInst, int line) ;
    void setLabel(string name, int address);
    bool isInstPos(string name);
    void init(int core);
    void printInstSet(vector<string> v);
    void printRegSet2(int k, vector<string> v,string p);
    void printInstCount();
    void printAll();
    bool isError(vector<string> v,int line);
};