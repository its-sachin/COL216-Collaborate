#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <vector>
using namespace std;

struct node {
    int value;
    struct node *prev;
};

class Stack {
    public:
    struct node* first=NULL;
    void push(int value){
        struct node* a=  (struct node*) malloc (sizeof(struct node));
        a->value=value;
        a->prev=first;
        first=a;
    }
    int pop(){
        if (first==NULL){
            throw "Empty Stack"; 
            return 0;
        }
        else {  
            return first->value;
            first=first->prev;
        }
    }
};

string decToHex(int dec){
    string str="";
    char c;
    if (dec==0){
        str="0";
    }
    while (dec !=0){
        int temp=0;
        temp=dec%16;
        if (temp<10){
            c=temp+48;
        }
        else {
            c=temp+55;
        }
        string s(1,c);
        str=s+str;
        dec= dec /16;
    }
    return str;
}

class MIPS {
    private:
    unordered_map<int, vector<string>> instructions;
    int memory[262144] = {0};
    int clock = 0;
    string regSteps = "";

    map<string,int> regs={{"$zero",0},{"$at",0},{"$v0",0},{"$v1",0},{"$a0",0},{"$a1",0},{"$a2",0},{"$a3",0},{"$t0",0},{"$t1",0},{"$t2",0},{"$t3",0},{"$t4",0},{"$t5",0},{"$t6",0},{"$t7",0},{"$s0",0},{"$s1",0},{"$s2",0},{"$s3",0},{"$s4",0},{"$s5",0},{"$s6",0},{"$s7",0},{"$t8",0},{"$t9",0},{"$k0",0},{"$k1",0},{"$gp",0},{"$sp",0},{"$fp",0},{"$ra",0}};   
    string inst[10] = {"add","addi","sub","mul","beq","bne","slt","lw","sw","j"};
    int instCount[10] = {0}; 
    
    Stack S;
    public:
    int getRegValue(string s){
        return regs.at(s);
    }
    void feedReg(string s,int value){
        regs.at(s)=value;
    }

    bool isReg(string s){
        if (regs.find(s) == regs.end()) {
            return false;
        }
        return true;
    }

    // ------------instruction handling--------------------------

    vector<string> getInst(int line){
        
        unordered_map<int, vector<string>>:: iterator it = instructions.find(line);
        return (it->second);
    }

    void setInst(int line, vector<string> input){
        instructions[line] = input;
    }

    bool isInstPos(int line){
        if (line <= 0 || line > instructions.size()){
            return false;
        }
        return true;
    }

    // -------------memory handling-------------------------

    bool sw(int address,int num ){
        if (address%4 != 0) {
            cout<< "INVALID Address of memory"<<endl;
            return false;
        }
        //for stack the adress would be negative
        else if (address<0){  
            push(num);
            return true;
        }
        else if (address > 4*sizeof(memory)){
            cout<<"Memory Overflow!!"<<endl;
        }
        else{
            memory[address/4] = num;
            return true;            
        }
    }

    bool lw(int address, string reg) {
        if (regs.find(reg) == regs.end()) {
            cout << "INVALID register" << endl;
            return false;
        }
        if (address%4 != 0){
            cout << "INVALID address of memory" << endl;
            return false; 
        }
        //for stack the sp value will be negative
        else if (address<0){  
            regs[reg] = pop();
            return true;
        } 
        else if (address > 4*sizeof(memory)){
            cout<<"Memory Overflow!!"<<endl;
        }
        else {
            regs[reg] = memory[address/4];
            return true;            
        }             
    }

    void push(int value){
        S.push(value);
    }

    int pop(){
        try{
            return S.pop();
        } catch (const char* msg){
            cerr<<msg<<endl;
        }
    }

    void printRegSet(string s){
        regSteps = regSteps + s + "\n";
        for (auto i = regs.begin(); i != regs.end(); i++) {
            regSteps = regSteps + i->first + " : " + decToHex((i->second)) + " | " ;
        }
        regSteps = regSteps + "\n\n";
    }

    void printReg(){
        cout<< regSteps<<endl;
    }

    void printClock(){
        cout<< "Number of clock Cycles: " << clock <<"\n" <<endl;
    }

    void printInstCount() {
        cout<< "Execution count of instructions:"<<endl;
        for (int i =0; i< 10; i++) {
            cout << inst[i] << ": "<< instCount[i]<<endl;
        }
    }

    bool isError(vector<string> v,int line) {

        string task = v.at(0);
        if (task == "add" || task == "sub" || task == "mul"  || task =="addi") {
            if (v.size() != 6){
                cout <<"Syntax error at line: "<<line<<endl;
                return false;
            }
            else if (v.at(2) != "," || v.at(4) != ","){
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }
            else if (isReg(v.at(1)) ==false || isReg(v.at(3)) ==false || (task != "addi" && isReg(v.at(5)) == false)) {
                cout << "Syntax error: Invalid register at line: "<<line<<endl;
                return false;
            }
            else if (v.at(1) == "$zero"){
                cout << "Syntax error: Cannot change value of zero register at line: "<<line<<endl;
                return false;
            }
            else if (task == "addi"){
                try{
                    stoi(v.at(5));
                }	
                catch(exception &err)
                {
                    cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                    return false;
                }
            }

            return true;
        }
        else if (task == "bne" || task == "beq" || task == "slt") {
            if (v.size() != 6 || v.at(2) != "," || v.at(4) != ",") {
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }
            else if (isReg(v.at(1)) == false || isReg(v.at(3)) == false) {
                cout << "Syntax error: Invalid register at line: "<<line<<endl;
                return false;
            }
            else if (task != "slt"){
                try {
                    int temp = stoi(v.at(5));
                    if (isInstPos(temp) == false){
                        cout<<"Invalid Instruction to be jumped at line: "<<line<< endl;
                        return false;
                    }
                }
                catch(exception &error){
                    cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                    return false;
                }
            }
            else {
                if (isReg(v.at(5)) ==false){
                    cout << "Syntax error: Invalid register at line: "<<line<<endl;
                    return false;
                } 
            }
            return true;
        }

        else if (task == "j"){
            if (v.size() != 2){
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }
            else{
                try {
                    int temp = stoi(v.at(1));
                    if (isInstPos(temp) == false){
                         cout<<"Invalid Instruction to be jumped at line: "<<line<< endl;
                        return false;
                    }
                }
                catch(exception &error){
                    cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                    return false;
                }
            }
            return true;
        }

        else if (task == "lw" || task == "sw") {
            if (v.size() <6 || v.size() > 7){
                cout << "Syntax error at line: "<<line<<endl;
                return false;
            }

            if (task == "lw" && v.at(1) == "$zero") {
                cout << "Syntax error: Cannot load value to zero register at line: "<<line<<endl;
                return false;
            }

            if (v.size() == 6){
                if (v.at(2) != "," || v.at(3) != "(" || v.at(5) != ")"){
                    cout << "Syntax error at line: "<<line<<endl;
                    return false;
                }
                else if (isReg(v.at(1)) == false || isReg(v.at(4)) == false){
                    cout << "Syntax error: Invalid register at line: "<<line<<endl;
                    return false;
                }
                return true;
            }

            else{
                if (v.at(2) != "," || v.at(4) != "(" || v.at(6) != ")"){
                    cout << "Syntax error at line: "<<line<<endl;
                    return false;
                }
                else if (isReg(v.at(1)) == false || isReg(v.at(5)) == false){
                    cout << "Syntax error: Invalid register at line: "<<line<<endl;
                    return false;
                }
                else{
                    try{
                        stoi(v.at(3));
                    }	
                    catch(exception &err)
                    {
                        cout<<"Syntax error: Type mismatch at line: " <<line <<endl;
                        return false;
                    }
                }
                return true;
            }
        }

        cout<<"Instruction not defined at line: " << line << endl;
        return false;
    }


    //exucuting the file 
    bool executeInst(){
        int i=1;
        int n=instructions.size()+1;
        int k=1;
        while(i<n){
            vector<string> v=instructions.at(i);
            k=i;
            if (v.size() ==0){
                i+= 1;
            }
            else {
                string a= v.at(0);
                if (isError(v,i) ==false) {
                    return false;
                }
                if (a=="add"){
                    feedReg(v.at(1),getRegValue(v.at(3))+getRegValue(v.at(5)));
                    clock += 1;
                    instCount[0] += 1;
                    i++;
                }
                else if (a=="sub"){
                    feedReg(v.at(1),getRegValue(v.at(3))-getRegValue(v.at(5)));
                    clock += 1;
                    instCount[2] += 1;
                    i++;
                }
                else if (a=="mul"){
                    feedReg(v.at(1),getRegValue(v.at(3))*getRegValue(v.at(5))); 
                    clock += 1; 
                    instCount[3] += 1;
                    i++;              
                }
                else if (a=="sw"){
                    if (v.size()==7){
                        bool done = sw(getRegValue(v.at(5))+stoi(v.at(3)),getRegValue(v.at(1)));
                        if (done == false) {return false;}
                    }
                    else if (v.size()==6){
                    bool done = sw(getRegValue(v.at(4)),getRegValue(v.at(1))); 
                    if (done == false) {return false;}
                    }
                    clock += 1;
                    instCount[8] += 1;
                    i++ ;              
                }
                else if (a=="addi"){
                    feedReg(v.at(1),getRegValue(v.at(3))+stoi(v.at(5))); 
                    clock += 1;
                    instCount[1] += 1;
                    i++;               
                }
                else if (a=="lw"){
                    if (v.size()==7){
                        bool done = lw(getRegValue(v.at(5))+stoi(v.at(3)),v.at(1));
                        if (done == false) {return false;}
                    }
                    else if (v.size()==6){
                    bool done = lw(getRegValue(v.at(4)),v.at(1)); 
                    if (done == false) {return false;}
                    }
                    clock += 1;
                    instCount[7] += 1;
                    i++;
                }
                else if (a=="j"){
                    int a=stoi(v.at(1));
                    if (a<1 || a>instructions.size()){
                        cout<<"invalid jump line at line no"<<i<<endl;
                        break;
                    }
                    else {
                        clock += 1;
                        instCount[9] += 1;
                        i=a;
                    }
                }
                else if (a=="beq"){
                    if (getRegValue(v.at(1))==getRegValue(v.at(3))){
                        i=stoi(v.at(5));
                    }
                    else {
                        i++;
                    }
                    clock += 1; 
                    instCount[4] += 1;             
                }
                else if (a=="bne"){
                    if (getRegValue(v.at(1))!=getRegValue(v.at(3))){
                        i=stoi(v.at(5));
                    }
                    else {
                        i++;
                    } 
                    clock += 1; 
                    instCount[5] += 1;              
                }
                else if (a=="slt"){
                    if (getRegValue(v.at(3))<getRegValue(v.at(5))){
                        feedReg(v.at(1),1);
                    }
                    else {
                        feedReg(v.at(1),0);
                    }  
                    i++; 
                    clock += 1; 
                    instCount[6] += 1;            
                }
                else {
                    cout<<"Unrecognised operation at line no."<<i<<endl;
                    return false;
                    break;
                } 
            }
            printRegSet("Instruction at line "+to_string(k));
        }
        return true;

    }
};




vector<string> lineToken(string line) {
    vector<string> v;
    int n=line.length();
    int i=0;
    string str="";
    while (i<n){
        char c=line[i];
        while (c==' '|| c=='\t'){
            i++;
            if (str!=""){
                v.push_back(str);
                str="";
            }
            c=line[i];
        }
        if (c=='#'){
            if (str!=""){
                v.push_back(str);
            }
            break;
        }
        if (c==',' || c=='(' || c==')'){
            if (str!=""){
                v.push_back(str);
                str="";
            }
            string sc(1,c);
            v.push_back(sc);
        }
        else {
            string s(1,c);
            str=str+s; 
        }
        i++;      
    }
    if (str!=""){
        v.push_back(str);
        str="";
    }
    return v;
}


int main(int argc, char** argv) {

    if (argc == 1) {
        cout << "Enter the file name\n" << endl;
        return 0;
    }
    string infile = argv[1];
    string line;
    ifstream file(infile);

    MIPS program;

    int lineVal = 0;
    while (getline(file,line)){
        lineVal+=1;
        vector<string> currLine = lineToken(line);
        program.setInst(lineVal,currLine);
    }

    bool isDone = program.executeInst();

    if (isDone == false) {
        return -1;
    }
    else {
        program.printReg();
        program.printClock();
        program.printInstCount();
        return 0;
    }
    

}