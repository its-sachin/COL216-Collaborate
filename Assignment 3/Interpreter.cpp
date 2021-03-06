    #include <iostream>
#include <fstream>
#include <unordered_map>
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
    int regs[32]={0};
    int hi,low;
    unordered_map<string,string> asciiz;
    unordered_map<string,int> word;
    unordered_map<string,int> regName={{"$zero",0},{"$at",1},{"$v0",2},{"$v1",3},{"$a0",4},{"$a1",5},{"$a2",6},{"$a3",7},{"$t0",8},{"$t1",9},{"$t2",10},{"$t3",11},{"$t4",12},{"$t5",13},{"$t6",14},{"$t7",15},{"$s0",16},{"$s1",17},{"$s2",18},{"$s3",19},{"$s4",20},{"$s5",21},{"$s6",22},{"$s7",23},{"$t8",24},{"$t9",25},{"$k0",26},{"$k1",27},{"$gp",28},{"$sp",29},{"$fp",30},{"$ra",31}};   
    string arr[32]={"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"};
    Stack S;
    public:
    int getRegValue(string s){
        int no =getRegNo(s);
        return regs[no];
    }
    void stringIntoM(string name ,string value){
        asciiz[name]=value;
    }
    string mIntoString(string name ){
        unordered_map<string,string>:: iterator it;
        it=asciiz.find(name);
        if (it==asciiz.end()){
            throw "Not Declared this value in data segment";    
        }
        else {
            return (it->second);
        }
    }
    void wordIntoM(string name ,int value){
        asciiz[name]=value;
    }
    int mIntoWord(string name ){
        unordered_map<string,int>:: iterator it;
        it=word.find(name);
        if (it==word.end()){
            throw "Not Declared this value in data segment"; 
        }
        else {
            return (it->second);
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
    void feedReg(string s,int value){
        int no=getRegNo(s);
        if (no==0){
            throw "cannot modify $zero register"; 
        }
        else {
            regs[no]=value;
        }
    }
    int getLow(){
        return low;
    }
    int getHigh(){
        return hi;
    }
    int getRegNo(string regN){
        unordered_map<string,int>:: iterator it;
        it=regName.find(regN);
        if (it==regName.end()){
            throw "Register name not correct";    
        }
        else {
            return (it->second);
        }
    }
    void printRegValue(string s){
        cout<<s<<endl;
        cout<<"[";
        for(int i=0;i<31;i++){
            cout<<arr[i]<<":"<<decToHex(regs[i])<<",";
        }
        cout<<arr[31]<<":"<<decToHex(regs[31])<<"]"<<endl;
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

bool isVal(string line,string value) {
    int n = line.length();
    int j = 0;
    int k = 0;
    while (k<n &&(line.at(k) == ' ')){
        k += 1;
    }
    int i=k;
    while (i<n && (line.at(i)!='#')){
        i+=1;
    }
    while (j< i && (line.at(n-j-1) == ' ')){
        j+=1;
    }

    line = line.substr(k,n-j-k);
    n = value.length();

    if (line.substr(0,n) == value){
        if (line.length() > n){
            char c = line[n];
            while (isspace(c) != 0){
                n += 1;
                c = line[n];
            }
            if (c == '#'){
                return true;
            }
            return false;
        }
        return true;
    }
    return false;
}

vector<string> dataToken(string line){
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
        if (c==':'){
            if (str!=""){
                v.push_back(str);
                str="";
            }
            string sc(1,c);
            v.push_back(sc);
        }
        else if (c =='"') {
            str = str+c;
            while (true) {
                i+=1;
                str = str + line[i];
                if (line[i] == '"' &&(i >= n || line[i+1] != '"')){
                    if (str != ""){
                        v.push_back(str);
                        str = "";
                    }
                    break;
                }
                else if (i >=n){
                    if (str != ""){
                        v.push_back(str);
                        str = "";
                    }
                    break;
                }
            }
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

    unordered_map<string,vector<vector<string>>> functions;

    bool isfirst = true;

    while (getline(file, line)){
        if (line.front() == '#') {}
        else {
            if (isVal(line,".data")){
                if (isfirst) {
                    //....data
                    isfirst = false;
                }
                else {
                    cout << "Syntax Error" << endl;
                }
            }

            else if (isVal(line, ".text")){
                //.....text
                isfirst = false;
            }
        }
    }
}