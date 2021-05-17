#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;


class Register{

public:

    map<string,int> regs={{"$zero",0},{"$at",0},{"$v0",0},{"$v1",0},{"$a0",0},{"$a1",0},{"$a2",0},{"$a3",0},{"$t0",0},{"$t1",0},{"$t2",0},{"$t3",0},{"$t4",0},{"$t5",0},{"$t6",0},{"$t7",0},{"$s0",0},{"$s1",0},{"$s2",0},{"$s3",0},{"$s4",0},{"$s5",0},{"$s6",0},{"$s7",0},{"$t8",0},{"$t9",0},{"$k0",0},{"$k1",0},{"$gp",0},{"$sp",0},{"$fp",0},{"$ra",0}}; 


    int coreNumber;

    int flag = -1;
    
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

    void printregVal() {
        cout << "\nNon zero register values:"<<endl;

         for (map<string, int>::iterator itr = regs.begin(); itr != regs.end(); ++itr) { 
             if (itr->second != 0){
                cout << itr->first  << " = " << itr->second << " | "; 
             }
        } 
    }

    string getRegVal() {
        string o = "";
        for (map<string, int>::iterator itr = regs.begin(); itr != regs.end(); ++itr) { 
             if (itr->second != 0){
                o += itr->first  + " = " + to_string(itr->second) + " | "; 
             }
        }
        return o;
    }

};