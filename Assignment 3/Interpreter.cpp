#include <iostream>
#include <fstream>
#include <unordered_map>
using namespace std;

class MIPS {
    private:
    int regs[32]={0};
    int hi,low;
    unordered_map<string,string> asciiz;
    unordered_map<string,int> word;
    Stack S;
    public:
    int getRegValue(int k){
        return regs[k];
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
    void feedReg(int k,int value){
        if (k==0){
            throw "cannot modify $zero register"; 
        }
        else {
            regs[k]=value;
        }
    }
    int getLow(){
        return low;
    }
    int getHigh(){
        return hi;
    }
};





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