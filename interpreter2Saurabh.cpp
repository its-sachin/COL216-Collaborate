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
    unordered_map<string,int> waitTime={{"$zero",0},{"$at",0},{"$v0",0},{"$v1",0},{"$a0",0},{"$a1",0},{"$a2",0},{"$a3",0},{"$t0",0},{"$t1",0},{"$t2",0},{"$t3",0},{"$t4",0},{"$t5",0},{"$t6",0},{"$t7",0},{"$s0",0},{"$s1",0},{"$s2",0},{"$s3",0},{"$s4",0},{"$s5",0},{"$s6",0},{"$s7",0},{"$t8",0},{"$t9",0},{"$k0",0},{"$k1",0},{"$gp",0},{"$sp",0},{"$fp",0},{"$ra",0}};
    unordered_map<string,int> occurence={{"$zero",0},{"$at",0},{"$v0",0},{"$v1",0},{"$a0",0},{"$a1",0},{"$a2",0},{"$a3",0},{"$t0",0},{"$t1",0},{"$t2",0},{"$t3",0},{"$t4",0},{"$t5",0},{"$t6",0},{"$t7",0},{"$s0",0},{"$s1",0},{"$s2",0},{"$s3",0},{"$s4",0},{"$s5",0},{"$s6",0},{"$s7",0},{"$t8",0},{"$t9",0},{"$k0",0},{"$k1",0},{"$gp",0},{"$sp",0},{"$fp",0},{"$ra",0}};
    unordered_map<string,bool> flagS={{"$zero",false},{"$at",false},{"$v0",false},{"$v1",false},{"$a0",false},{"$a1",false},{"$a2",false},{"$a3",false},{"$t0",false},{"$t1",false},{"$t2",false},{"$t3",false},{"$t4",false},{"$t5",false},{"$t6",false},{"$t7",false},{"$s0",false},{"$s1",false},{"$s2",false},{"$s3",false},{"$s4",false},{"$s5",false},{"$s6",false},{"$s7",false},{"$t8",false},{"$t9",false},{"$k0",false},{"$k1",false},{"$gp",false},{"$sp",false},{"$fp",false},{"$ra",false}};//change to check sw dependecy
    unordered_map<string,string> message;
    int statRow=-1;
    vector<string> dramQ[1024];//Queue
    string regActive="none";//active register
    unordered_map<string,int> rowNo;//register row access 

    int memory[262144] = {0};
    int clock = 0;
    string regSteps = "";
    string cycleI= "Every Cycle description\n\n";
    int DRAM[1024][256]={0};
    int rowBuffer[256]={0};
    int rowB=-1;
    map<string,int> regs={{"$zero",0},{"$at",0},{"$v0",0},{"$v1",0},{"$a0",0},{"$a1",0},{"$a2",0},{"$a3",0},{"$t0",0},{"$t1",0},{"$t2",0},{"$t3",0},{"$t4",0},{"$t5",0},{"$t6",0},{"$t7",0},{"$s0",0},{"$s1",0},{"$s2",0},{"$s3",0},{"$s4",0},{"$s5",0},{"$s6",0},{"$s7",0},{"$t8",0},{"$t9",0},{"$k0",0},{"$k1",0},{"$gp",0},{"$sp",0},{"$fp",0},{"$ra",0}};   
    string inst[10] = {"add","addi","sub","mul","beq","bne","slt","lw","sw","j"};
    int instCount[10] = {0};    
    Stack S;
    int row_access_delay;
    int col_access_delay;
    int bufferUpdate=0;
    public:
    unordered_map<string,int> labels;//labels impelementation change
    void setDelay(int a,int b){
        row_access_delay=a;
        col_access_delay=b;
    }
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

    bool sw(int address,string reg,int i){
        int t=0;
        if (address%4 != 0) {
            cout<< "INVALID Address of memory"<<endl;
            return false;
        }
        //for stack the adress would be negative
        else if (address<0){  
            push(getRegValue(reg));
            return true;
        }
        else if (address > 4*sizeof(memory)){
            cout<<"Memory Overflow!!"<<endl;
            return false;
        }
        else{
            int b=address%1024;
            int a=(address-b)/1024;
            if (rowB==-1){
                t=row_access_delay+col_access_delay; 
                for (int k = 0; k < 256; k++) {
                    rowBuffer[k] = DRAM[a][k];
                }             
            }
            else if (rowB!=a){
                t=2*row_access_delay+col_access_delay;
                for (int k = 0; k < 256; k++) {
                    DRAM[rowB][k]=rowBuffer[k];
                } 
                for (int k = 0; k < 256; k++) {
                    rowBuffer[k] = DRAM[a][k];
                } 
            }
            else {
                t=2*row_access_delay+col_access_delay;                
            }
            rowB=a;
            b=b/4;
            if (regActive=="none" && a==statRow){
                bufferUpdate-=1;
                t=col_access_delay;
            }
            if (regActive=="none"){
                regActive=reg;
            }
            int value;
            if (reg!="$zero"){
                value=getRegValue(reg.substr(0,3));
                rowBuffer[b]=value;
            }
            else {
                value=0;
            }
            if (dramQ[a].size()!=0){
                bufferUpdate-=1;
                t=col_access_delay;
            }
            if (waitTime.at(reg)!=0){
                occurence.at(reg)+=1;
                int nnn=occurence.at(reg);
                for (int i=0; i<nnn;i++){
                    reg += "'";
                }
                waitTime.insert({reg,t});
                flagS.insert({reg,true});//labels implementation change
            }
            else {
                waitTime.at(reg)=t;
                flagS.at(reg)=true;//labels implementation change
            }
            dramQ[a].push_back(reg);
            rowNo.insert({reg,a});
            message.insert({reg," | memory address "+to_string(address)+"-"+to_string(address+3)+"="+to_string(value)});
            cycleI+=", DRAM request issued and put in queue but waiting for response( has wait time of "+to_string(t)+" cycles.)\n";
            bufferUpdate+=1;
            return true;           
        }
    }

    bool lw(int address, string reg,int i) {
        int t=0;
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
            return false;
        }
        else {
            int b=address%1024;
            int a=(address-b)/1024;
            if (rowB==-1){
                t=row_access_delay+col_access_delay;
                for (int k = 0; k < 256; k++) {
                    rowBuffer[k] = DRAM[a][k];
                }  
            }
            else if (rowB!=a){
                t=2*row_access_delay+col_access_delay;
                for (int k = 0; k < 256; k++) {
                    DRAM[rowB][k]=rowBuffer[k];
                } 
                for (int k = 0; k < 256; k++) {
                    rowBuffer[k] = DRAM[a][k];
                } 
            }
            else {
                t=2*row_access_delay+col_access_delay;                
            }
            rowB=a;
            b=b/4;
            if (regActive=="none" && a==statRow){
                bufferUpdate-=1;
                t=col_access_delay;
            }
            if (regActive=="none"){
                regActive=reg;
            }
            regs[reg] = rowBuffer[b];
            if (dramQ[a].size()!=0){
                t=col_access_delay;
                bufferUpdate-=1;
            }
            dramQ[a].push_back(reg);
            rowNo.insert({reg,a});
            message.insert({reg," | "+reg+"="+to_string(rowBuffer[b])});
            waitTime.at(reg)=t;
            cycleI+=", DRAM request issued and put in queue but waiting for response( has wait time of "+to_string(t)+" cycles.)\n";
            bufferUpdate+=1;
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
            return 0;
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
    void  printBuff(){
        cout<<"Number of updates to buffer:"<<bufferUpdate<<"\n\n";
    }
    void printCycle(){
        cout<<cycleI<<endl;
    }
    void printInstCount() {
        cout<< "Execution count of instructions:"<<endl;
        for (int i =0; i< 10; i++) {
            cout << inst[i] << ": "<< instCount[i]<<endl;
        }
    }

    void nextReg(){
        waitTime.at(regActive)=0;
        flagS.at(regActive)=false;
        message.erase(regActive);
        int p=rowNo.at(regActive);
        rowNo.erase(regActive);
        dramQ[p].erase(dramQ[p].begin());
        if (dramQ[p].size()!=0){
            regActive=dramQ[p].at(0);
        }
        else {
            if (rowNo.size()==0){
                regActive="none";
                statRow=p;
            }
            else {
                auto it=rowNo.begin();
                regActive=dramQ[it->second].at(0);
            }
        }
        return;
    }
    
    void emptyRow(string reg,int i,int f){
        int p=rowNo.at(reg);
        int d=dramQ[p].size();
        regActive=dramQ[p].at(0);
        while (d>0){
            if (f==0){
                cycleI+="cycle "+to_string(clock+1)+"-"+to_string(clock+waitTime.at(regActive))+": emptying the DRAM requests of  row no. "+to_string(p)+" because instruction at "+to_string(i)+" has dependency in this row "+message.at(regActive)+"\n";
            }
            else if (f==1){
                cycleI+="cycle "+to_string(clock+1)+"-"+to_string(clock+waitTime.at(regActive))+": emptying the DRAM requests of  row no. "+to_string(p)+" because instruction at "+to_string(i)+" has dependency and have to empty active row first "+message.at(regActive)+"\n";
            }
            else {
                cycleI+="cycle "+to_string(clock+1)+"-"+to_string(clock+waitTime.at(regActive))+": emptying the DRAM requests of  row no. "+to_string(p)+" because reached the end of instruction "+message.at(regActive)+"\n";
            }
            clock+=waitTime.at(regActive);
            nextReg();
            d=d-1;
        }
        return;
    }

    void dependency(vector<string> registe,int i,bool f){
        int a = registe.size();
        if (a==3){
            bool a =(waitTime.at(registe.at(0))==0);
            bool b=waitTime.at(registe.at(1))==0 || (flagS.at(registe.at(1)));//dependency of sw 
            bool c =waitTime.at(registe.at(2))==0 || (flagS.at(registe.at(2)));//dependency of sw 
            if (!(a && b && c)){
                emptyRow(regActive,i,1);
            } 
            a =(waitTime.at(registe.at(0))==0);
            if (!a){
                emptyRow(registe.at(0),i,0);
            } 
            b=waitTime.at(registe.at(1))==0 || (flagS.at(registe.at(1)));//dependency of sw   
            if (!b){
                emptyRow(registe.at(1),i,0);
            }
            c =waitTime.at(registe.at(2))==0 || (flagS.at(registe.at(2)));//dependency of sw 
            if (!c){
                emptyRow(registe.at(2),i,0);
            }        
        }
        else if (a==2 && f){
            bool a =(waitTime.at(registe.at(0))==0);
            bool b=waitTime.at(registe.at(1))==0 || (flagS.at(registe.at(1)));//dependency of sw 
            if (!(a && b)){
                emptyRow(regActive,i,1);
            } 
            a =(waitTime.at(registe.at(0))==0);
            if (!a){
                emptyRow(registe.at(0),i,0);
            }   
            b=waitTime.at(registe.at(1))==0 || (flagS.at(registe.at(1)));//dependency of sw 
            if (!b){
                emptyRow(registe.at(1),i,0);
            }       
        }
        else {
            bool a =(waitTime.at(registe.at(0))==0) || (flagS.at(registe.at(0)));
            bool b=waitTime.at(registe.at(1))==0 || (flagS.at(registe.at(1)));//dependency of sw
            if (!(a && b)){
                emptyRow(regActive,i,1);
            }
            a =(waitTime.at(registe.at(0))==0) || (flagS.at(registe.at(0)));
            if (!a){
                emptyRow(registe.at(0),i,0);
            } 
            b=waitTime.at(registe.at(1))==0 || (flagS.at(registe.at(1)));//dependency of sw 
            if (!b){
                emptyRow(registe.at(1),i,0);
            }      
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
                    string temp = v.at(5);
                    if (labels.find(temp) == labels.end()){//change in error from line to label
                        cout<<"Invalid label to be jumped at line: "<<line<< endl;
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
                    string temp = v.at(1);
                    if (labels.find(temp) == labels.end()){//change in error from line to label
                        cout<<"Invalid label to be jumped at line: "<<line<< endl;
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

    void update(int i,string s,string info){
        if (s=="sw" || s=="lw"){
            if (regActive=="none"){
                cycleI+="cycle "+to_string(clock)+": Execute "+to_string(i)+" no DRAM in queue";
                return;
            }
            waitTime.at(regActive)-=1;
            if (waitTime.at(regActive)==0){
                cycleI+="cycle "+to_string(clock)+": Execute "+to_string(i)+" DRAM for "+regActive+" completed"+message.at(regActive);
                nextReg();
            }
            else {
                cycleI+="cycle "+to_string(clock)+": Execute "+to_string(i)+" along with DRAM for "+regActive;
            }   
            return;
        }
        else {
            if (regActive=="none"){
                cycleI+="cycle "+to_string(clock)+": Execute "+to_string(i)+" no DRAM in queue |"+info;
                return;
            }
            waitTime.at(regActive)-=1;
            if (waitTime.at(regActive)==0){
                cycleI+="cycle "+to_string(clock)+": Execute "+to_string(i)+" DRAM for "+regActive+" completed"+message.at(regActive)+" |"+info;
                nextReg();
            }
            else {
                cycleI+="cycle "+to_string(clock)+": Execute "+to_string(i)+" along with DRAM for "+regActive+" |"+info;
            }   
            return;          
        }
    }

    //exucuting the file 
    bool executeInst(){
        int i=1;
        int n=instructions.size()+1;
        int k=1;
        while(i<n){ 
            vector<string> v=instructions.at(i);
            k=i;
            if (v.size() ==0 || v.at(1)==":"){
                i+= 1;
            }
            else {
                string ab= v.at(0);
                if (isError(v,i) ==false) {
                    return false;
                }
                if (ab=="add"){
                    string info;
                    vector<string> registe;
                    registe.push_back(v.at(1));
                    registe.push_back(v.at(3));
                    registe.push_back(v.at(5));
                    dependency(registe,i,true);
                    feedReg(v.at(1),getRegValue(v.at(3))+getRegValue(v.at(5)));
                    clock+=1;
                    info=v.at(1)+"="+to_string(getRegValue(v.at(1)))+" (add)\n";
                    update(i,ab,info);
                    instCount[0] += 1;
                    i++;
                }
                else if (ab=="sub"){
                    string info;
                    vector<string> registe;
                    registe.push_back(v.at(1));
                    registe.push_back(v.at(3));
                    registe.push_back(v.at(5));
                    dependency(registe,i,true); 
                    feedReg(v.at(1),getRegValue(v.at(3))-getRegValue(v.at(5)));
                    clock+=1;
                    info=v.at(1)+"="+to_string(getRegValue(v.at(1)))+" (sub)\n";
                    update(i,ab,info);
                    instCount[2] += 1;
                    i++;
                }
                else if (ab=="j"){//extra command
                    string info;
                    clock+=1;
                    int li=labels.at(v.at(1))+1;
                    info="jump to line no."+to_string(li)+" (j) \n";
                    update(i,ab,info);
                    instCount[9] += 1;
                    i=li;
                }
                else if (ab=="slt"){//extra command
                    string info;
                    vector<string> registe;
                    registe.push_back(v.at(1));
                    registe.push_back(v.at(3));
                    registe.push_back(v.at(5));
                    dependency(registe,i,true);
                    if (getRegValue(v.at(3))<getRegValue(v.at(5))){
                        feedReg(v.at(1),1);
                    }
                    else {
                        feedReg(v.at(1),0);
                    } 
                    clock+=1;
                    info=v.at(1)+"="+to_string(getRegValue(v.at(1)))+" (slt)\n";
                    update(i,ab,info);
                    instCount[6] += 1;
                    i++;
                }
                else if (ab=="beq"){//extra command
                    string info;
                    vector<string> registe;
                    registe.push_back(v.at(1));
                    registe.push_back(v.at(3));
                    dependency(registe,i,false);
                    int li; 
                    if (getRegValue(v.at(1))==getRegValue(v.at(3))){
                        li=labels.at(v.at(5))+1;
                    }
                    else {
                        li=i+1;
                    }
                    clock+=1;
                    info="jump to line no."+to_string(li)+" (beq)\n";
                    update(i,ab,info);
                    instCount[4] += 1;
                    i=li;
                }
                else if (ab=="bne"){//extra command
                    string info;
                    vector<string> registe;
                    registe.push_back(v.at(1));
                    registe.push_back(v.at(3));
                    dependency(registe,i,false); 
                    int li; 
                    if (getRegValue(v.at(1))!=getRegValue(v.at(3))){
                        li=labels.at(v.at(5))+1;
                    }
                    else {
                        li=i+1;
                    }
                    clock+=1;
                    info="jump to line no."+to_string(li)+" (beq)\n";
                    update(i,ab,info);
                    instCount[5] += 1;
                    i=li;
                }
                else if (ab=="mul"){
                    string info;
                    vector<string> registe;
                    registe.push_back(v.at(1));
                    registe.push_back(v.at(3));
                    registe.push_back(v.at(5));
                    dependency(registe,i,true); 
                    feedReg(v.at(1),getRegValue(v.at(3))*getRegValue(v.at(5)));
                    clock+=1;
                    info=v.at(1)+"="+to_string(getRegValue(v.at(1)))+" (mul)\n";
                    update(i,ab,info);
                    instCount[3] += 1;
                    i++;
                }
                else if (ab=="addi"){
                    string info;
                    vector<string> registe;
                    registe.push_back(v.at(1));
                    registe.push_back(v.at(3));
                    dependency(registe,i,true);              
                    feedReg(v.at(1),getRegValue(v.at(3))+stoi(v.at(5)));
                    clock += 1;
                    info=v.at(1)+"="+to_string(getRegValue(v.at(1)))+" (addi) \n"; 
                    update(i,ab,info); 
                    instCount[1] += 1;
                    i++;              
                }
                else if (ab=="sw"){
                    if (v.size()==7){
                        vector<string> registe;
                        registe.push_back(v.at(1));
                        registe.push_back(v.at(5));
                        dependency(registe,i,false); 
                        clock+=1;
                        update(i,ab,"");
                        bool done = sw(getRegValue(v.at(5))+stoi(v.at(3)),v.at(1),i);
                        if (done == false) {return true;}
                    }
                    else if (v.size()==6){
                        vector<string> registe;
                        registe.push_back(v.at(1));
                        registe.push_back(v.at(4));
                        dependency(registe,i,true);   
                        clock+=1; 
                        update(i,ab,"");
                        bool done = sw(getRegValue(v.at(4)),v.at(1),i); 
                        if (done == false) {return false;}
                    }
                    instCount[8] += 1;
                    i++;           
                }
                else if (ab=="lw"){
                    if (v.size()==7){
                        vector<string> registe;
                        registe.push_back(v.at(1));
                        registe.push_back(v.at(5));
                        dependency(registe,i,true); ;              
                        clock+=1;
                        update(i,ab,""); 
                        bool done = lw(getRegValue(v.at(5))+stoi(v.at(3)),v.at(1),i);
                        if (done == false) {return false;}
                    }
                    else if (v.size()==6){
                        vector<string> registe;
                        registe.push_back(v.at(1));
                        registe.push_back(v.at(4));
                        dependency(registe,i,true);
                        clock+=1; 
                        update(i,ab,"");
                        bool done = lw(getRegValue(v.at(4)),v.at(1),i); 
                        if (done == false) {return false;}
                    }
                    instCount[7] += 1;
                    i++;
                }
                else {
                    cout<<"Unrecognised operation at line no."<<i<<endl;
                    return false;
                    break;
                }
                // printRegSet("Instruction at line "+to_string(k));
            }
        }
        while (regActive!="none"){
            emptyRow(regActive,i,2);
        }
        //putting buffer back in the memory after all that is done        
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
        while (i<n && (c==' '|| c=='\t')){
            i++;
            if (str!=""){
                v.push_back(str);
                str="";
            }
            c=line[i];
        } 
        if (i>=n){
            break;
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
    if (argc==4){
        program.setDelay(stoi(argv[2]),stoi(argv[3]));
    }
    else {
        cout<<"Please provide delays for memory acsess\n";
        return 0;
    }
    int lineVal = 0;
    while (getline(file,line)){
        lineVal+=1;
        vector<string> currLine = lineToken(line);
        if (currLine.size()==2 && currLine.at(1)==":"){//labels implementation
            program.labels.insert({currLine.at(0),lineVal});
        }

        program.setInst(lineVal,currLine);
    }

    bool isDone = program.executeInst();

    if (isDone == false) {
        return -1;
    }
    else {
        program.printCycle();
        program.printClock();
        program.printBuff();
        return 0;
    }  
}