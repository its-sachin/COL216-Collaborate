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
    int hi,low;
    unordered_map<int, vector<string>> instructions;
    int memory[262144] = {0};

    map<string,int> regs={{"$zero",0},{"$at",0},{"$v0",0},{"$v1",0},{"$a0",0},{"$a1",0},{"$a2",0},{"$a3",0},{"$t0",0},{"$t1",0},{"$t2",0},{"$t3",0},{"$t4",0},{"$t5",0},{"$t6",0},{"$t7",0},{"$s0",0},{"$s1",0},{"$s2",0},{"$s3",0},{"$s4",0},{"$s5",0},{"$s6",0},{"$s7",0},{"$t8",0},{"$t9",0},{"$k0",0},{"$k1",0},{"$gp",0},{"$sp",0},{"$fp",0},{"$ra",0}};   
    
    Stack S;
    public:
    int getRegValue(string s){
        return regs.at(s);
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

    bool sw(int num, int address){
        if (address%4 != 0) {
            cout<< "INVALID Address of memory"<<endl;
            return false;
        }
        else {
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

        regs[reg] = memory[address/4];
        return true;
        
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

    int getLow(){
        return low;
    }

    int getHigh(){
        return hi;
    }

    void printReg(){
        for (auto i = regs.begin(); i != regs.end(); i++) {
            cout << i->first << " : "<< (i->second) << ',';
        }
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
        if (c==',' || c=='(' || c==')' || c==':'){
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
    

}