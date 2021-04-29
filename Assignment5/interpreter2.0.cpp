#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;


// -----------------------------------ass4-addition-------------------------------------------------------

class Waiter {

    public:
    vector<string> inst;
    int rowNum;
    int address;
    string addressReg = "";
    string changeReg = "";

};

// --------------------------------------------------------------------------------------------------------

class DRAM {
    public:
    bool isOn;
    vector<string> currInst;
    int clock = 0;
    int relClock =0;
    int rowDelay=0;
    int colDelay=0;
    int updates[2] = {0};
    bool rowDone = false;

    map<string,int> regs={{"$zero",0},{"$at",0},{"$v0",0},{"$v1",0},{"$a0",0},{"$a1",0},{"$a2",0},{"$a3",0},{"$t0",0},{"$t1",0},{"$t2",0},{"$t3",0},{"$t4",0},{"$t5",0},{"$t6",0},{"$t7",0},{"$s0",0},{"$s1",0},{"$s2",0},{"$s3",0},{"$s4",0},{"$s5",0},{"$s6",0},{"$s7",0},{"$t8",0},{"$t9",0},{"$k0",0},{"$k1",0},{"$gp",0},{"$sp",0},{"$fp",0},{"$ra",0}}; 
    
    int memory[1024][256] = {0};
    int rowBuffer[256] = {0};

    string addressReg = "";
    string changeReg = "";
    int address;
    int rowNum;
    int colNum;
    int currRow = -1;
    bool isFirst = true;
    string regSteps;
    int initialClock;
    bool updated = true;

    void start(vector<string> v){
        currInst = v;
        changeReg  = currInst.at(1);
        if (currInst.size() == 6) {
            addressReg = currInst.at(4);
            address = regs.at(v.at(4));
        }
        else if (currInst.size() == 7) {
            addressReg = currInst.at(5);
            address = regs.at(v.at(5))+stoi(v.at(3));
        }
        isOn = true;
        relClock =0;
        colNum = (address%1024)/4; 
        rowNum = address/1024;
        initialClock = clock;
        rowDone = false;

    }

    void check() {
        if ( rowDone == false && relClock == rowDelay){
            if (rowNum != currRow){
                regSteps += "\n";
                for (int i=0; i <256; i++){
                    if (isFirst == false){
                        memory[currRow][i] = rowBuffer[i];
                    }
                    rowBuffer[i] = memory[rowNum][i];
                }
                if (isFirst == false) {
                    regSteps = regSteps + "\nClock: " + to_string(initialClock+1) + "-"+ to_string(initialClock+rowDelay) +"\n  DRAM: Writeback Row: " + to_string(currRow);
                    initialClock += rowDelay;
                    updated = true;

                }
                else {
                    updated = false;
                }
                isFirst = false;
                regSteps = regSteps + "\nClock: " + to_string(initialClock+1) + "-"+ to_string(initialClock+rowDelay) +"\n  DRAM: Activate Row: " + to_string(rowNum);
                initialClock += rowDelay;
                clock = initialClock; 
                updates[0] += 1;
                currRow = rowNum;
                relClock = 0;
                regSteps += "\n";
            }
            else {
                updated = false;
            }
            rowDone = true;
        }
        else if ((rowNum == currRow) && relClock == colDelay) {
            if (currInst.at(0) == "lw") {
                regSteps = regSteps + "\nClock: " + to_string(initialClock+1) + "-"+ to_string(initialClock +colDelay) +"\n  DRAM: Column Access: " + to_string(colNum);
                regs[changeReg] = rowBuffer[colNum];
                regSteps = regSteps+ "\n  Register modified : " + changeReg + " = " + to_string(regs[changeReg]);
                clock = initialClock + colDelay;
            }
            else if (currInst.at(0) == "sw"){
                regSteps = regSteps + "\nClock: " + to_string(initialClock+1) + "-"+ to_string(initialClock+colDelay) +"\n  DRAM: Column Access: " + to_string(colNum);
                rowBuffer[colNum] = regs[changeReg];
                regSteps = regSteps+ "\n  Memory address modified : " + to_string(address) + " = " + to_string(regs[changeReg]);
                clock = initialClock + colDelay;
                updates[1] += 1;
            }
            isOn = false;
        }
    }

    void finalWrite() {
        for (int i=0;i<256;i++) {
            memory[currRow][i] = rowBuffer[i];
        }
        // regSteps = regSteps + "\nClock: " + to_string(clock+1) + "-"+ to_string(clock + rowDelay) +"\nDRAM: Writeback Row: " + to_string(currRow);
        // clock += rowDelay;
    }

    // -----------------------------------ass4-addition-------------------------------------------------------

    unordered_map<int, Waiter> queue;
    unordered_map<int, vector<int>> rowSort;

    int bounds(bool isMin) {
        int min = -1;
        int max = -1;
        for (auto& it : queue) {
            if (min == -1) {
                min = it.first;
            }

            else if(it.first < min) {
                min = it.first;
            }

            if (max < it.first) {
                max = it.first;
            }
        }

        if (isMin) {
            return min;
        }
        return max;
    }

    void start(Waiter w) {
        currInst = w.inst;
        changeReg  = w.changeReg;
        addressReg = w.addressReg;
        address = w.address;
        isOn = true;
        relClock =0;
        colNum = (address%1024)/4; 
        rowNum = address/1024;
        initialClock = clock;
        rowDone = false;
    }


   void initWaiter(vector<string> v) {
        Waiter w;
        w.inst = v;
        w.changeReg  = v.at(1);

        int add;
        if (v.size() == 6) {
            w.addressReg = v.at(4);
            add = regs.at(v.at(4));
        }
        else if (v.size() == 7) {
            w.addressReg = v.at(5);
            add = regs.at(v.at(5))+stoi(v.at(3));
        }
        w.address = add;
        int row = add/1024;
        w.rowNum = row;

        if (v.at(0) == "lw") {
            for (auto it = queue.begin(); it != queue.end(); ){
                if (it->second.inst.at(0) == "lw" && w.changeReg == it->second.changeReg) {
                    rowSort[it->second.rowNum].erase(remove(rowSort[it->second.rowNum].begin(), rowSort[it->second.rowNum].end(), it->first), rowSort[it->second.rowNum].end());
                    queue.erase(it++);
                    // cout << "kk" << endl;
                }
                else {
                    ++it;
                }
            }
        }
        else {
            for (auto it = rowSort[row].begin(); it != rowSort[row].end();){
                if (queue[*it].inst.at(0) == "sw" && add == queue[*it].address) {
                    it = rowSort[row].erase(it);
                    queue.erase(*it);
                }
                else {
                    ++it;
                }
            }
        }
        
        int max = bounds(false);
        queue[max+1] = w;
        rowSort[w.rowNum].push_back(max+1);
    }

    bool currIsDep(vector<string> v) {
        string task = v.at(0);
        if (task == "add" || task == "addi" || task == "mul" || task == "sub" || task == "slt") {
            if (addressReg == v.at(1) || changeReg == v.at(1) || (currInst.at(0) == "lw" && (changeReg == v.at(3) || changeReg == v.at(5)))){
                return true;
            }
        }

        else if (task == "bne" || task == "beq"){
            if (currInst.at(0) == "lw" && (changeReg == v.at(1) || changeReg == v.at(3))){
                return true;
            }
        }  

        else if (task == "lw" || task == "sw") {
            string c1  = v.at(1);
            string cs = changeReg;
            string a1;
            string as = addressReg;
            int add;
            if (v.size() == 6) {
                a1 = v.at(4);
                add = regs.at(v.at(4));
            }
            else if (v.size() == 7) {
                a1 = v.at(5);
                add = regs.at(v.at(5))+stoi(v.at(3));
            }

            if (currInst.at(0) == "lw") {

                if (task == "lw" && (c1 == as || a1 == cs)){
                    return true;
                }

                else if (task == "sw" && (c1 == cs || a1 == cs || (add == address))){
                    return true;
                }
            }

            else{

                if (task == "lw" && (c1 == cs || c1 == as || (add == address))){
                    return true;
                }

            }

        }
 
        return false;
    }

    bool isDep(vector<string> v, Waiter w) {
        string task = v.at(0);
        if (task == "add" || task == "addi" || task == "mul" || task == "sub" || task == "slt") {
            if (w.addressReg == v.at(1) || w.changeReg == v.at(1) || (w.inst.at(0) == "lw" && (w.changeReg == v.at(3) || w.changeReg == v.at(5)))){
                return true;
            }
        }

        else if (task == "bne" || task == "beq"){
            if (w.inst.at(0) == "lw" && (w.changeReg == v.at(1) || w.changeReg == v.at(3))){
                return true;
            }
        }   

        else if (task == "lw" || task == "sw") {
            string c1  = v.at(1);
            string cs = w.changeReg;
            string a1;
            string as = w.addressReg;
            int add;
            if (v.size() == 6) {
                a1 = v.at(4);
                add = regs.at(v.at(4));
            }
            else if (v.size() == 7) {
                a1 = v.at(5);
                add = regs.at(v.at(5))+stoi(v.at(3));
            }

            if (w.inst.at(0) == "lw") {

                if (task == "lw" && (c1 == as || a1 == cs)){
                    return true;
                }

                else if (task == "sw" && (c1 == cs || a1 == cs || (add == w.address))){
                    return true;
                }
            }

            else{

                if (task == "lw" && (c1 == cs || c1 == as || (add == w.address))){
                    return true;
                }

            }

        }
        return false;
    }

    int depInRow(vector<string> v, int row){
        for (auto it = rowSort[row].rbegin(); it != rowSort[row].rend(); it++){
            if (isDep(v, queue[*it])){
                return *it;
            }
        }
        return -1;
    }

    unordered_map<int,int> allDep(vector<string> v) {
        unordered_map<int,int> ans;
        for ( const auto &it : rowSort ) {
            if (it.first != rowNum) {
                int curr = depInRow(v, it.first); 
                if (curr != -1) {
                    ans[it.first] = curr;
                }
            }
        }
        return ans;
    }

    void doRow(int row, int bound) {
        while (true) {
            if (rowSort[row].empty()) {
                rowSort.erase(row);
                break;
            }
            int beg = *rowSort[row].begin();
            start(queue[beg]);
            rowSort[row].erase(rowSort[row].begin());
            queue.erase(beg);
            if (bound != -1 && beg>bound){
                break;
            }
            relClock = rowDelay;
            check();
            relClock = colDelay;
            check();
        }
    }

    void initDep() {
        if (rowSort[rowNum].empty()) {
            rowSort.erase(rowNum);
            int min = bounds(true);
            start(queue[min]);
            int row = queue[min].rowNum;
            rowSort[row].erase(remove(rowSort[row].begin(), rowSort[row].end(), min), rowSort[row].end());
            queue.erase(min);
            if (rowSort[row].empty()) {
                rowSort.erase(row);
            }
        }

        else {
            int beg = *rowSort[rowNum].begin();
            start(queue[beg]);
            rowSort[rowNum].erase(rowSort[rowNum].begin());
            queue.erase(beg);
        }
    }

    void doDep(vector<string> v) {
        unordered_map<int,int> all = allDep(v); 
        if (all.empty()) {
            if (rowSort[rowNum].empty()) {
                int min = bounds(true); 
                start(queue[min]);
                int row = queue[min].rowNum;
                rowSort[row].erase(remove(rowSort[row].begin(), rowSort[row].end(), min), rowSort[row].end());
                queue.erase(min);
                if (rowSort[row].empty()) {
                    rowSort.erase(row);
                }
            }
            else {
                
                int curr = depInRow(v, rowNum) ;
                if (curr== -1) {
                    int beg = *rowSort[rowNum].begin();
                    start(queue[beg]);
                    rowSort[rowNum].erase(rowSort[rowNum].begin());
                    queue.erase(beg);
                }

                else {
                    doRow(rowNum, curr);
                    if (isOn == false) {

                        if (queue.empty()){
                            return;
                        }

                        else {
                            initDep();
                        }

                    }
                }
            }
        }

        else {
            doRow(rowNum, -1);

            for (auto it = all.begin(); it != all.end(); ){
                if (all.size() == 1) {
                    doRow(it->first, it->second);
                }
                else {
                    doRow(it->first,-1);
                }
                it = all.erase(it++);
            }

            if (isOn == false) {
                if (queue.empty()){
                    return;
                }

                else {
                    initDep();
                }

            }
        }
    }


    void doLS(vector<string> v) {

        if (isOn == false) {
            clock += 1;
            start(v);
            return;
        }

        unordered_map<int,int> all = allDep(v); 

        if (all.empty()) {
            relClock += 1;
            clock += 1;
            check();
            if (isOn) {
                initWaiter(v);
            }
            else if (queue.empty()){
                start(v);
            }
            else {
                initDep();
                initWaiter(v);
            }
            return;
        }

        else {
            regSteps += "\n  [Found Dependence]\n";
            relClock = rowDelay;
            check();
            relClock = colDelay;
            check();
            doRow(rowNum, -1);

            for (auto it = all.begin(); it != all.end(); ){
                if (all.size() == 1) {
                    doRow(it->first, it->second);
                }
                else {
                    doRow(it->first,-1);
                }
                it = all.erase(it++);
            }

            clock += 1;
            if (isOn == false) {
                if (queue.empty()){
                    start(v);
                    return;
                }

                else {
                    initDep();
                    initWaiter(v);
                }

            }

            else {
                initWaiter(v);
            }
        }
    }

    void doLSnot(vector<string> v) {
        if (isOn) {
            if (currIsDep(v)){
                regSteps += "\n  [Found Dependence]\n";
                relClock = rowDelay;
                check();
                relClock = colDelay;
                check();

                if (queue.empty()){
                    clock += 1;
                    return;
                }

                else {
                    doDep(v);
                    clock += 1;
                }

            }

            else  {
                if (queue.empty() || (depInRow(v,rowNum) == -1 && allDep(v).empty())){
                    relClock += 1;
                    clock += 1;
                    check();
                    return;
                }

                else {
                    regSteps += "\n  [Found Dependence]\n";
                    relClock = rowDelay;
                    check();
                    relClock = colDelay;
                    check();
                    doDep(v);
                    clock += 1;
                }
            }
        }

        else {
            clock += 1;
        }
    }

    void doAll() {
        relClock = rowDelay;
        check();
        relClock = colDelay;
        check();
        doRow(rowNum, -1);
        for (auto it = rowSort.begin(); it != rowSort.end(); ){
            int row = it->first;
            while (true) {
                if (rowSort[row].empty()) {
                    break;
                }
                int beg = *rowSort[row].begin();
                start(queue[beg]);
                relClock = rowDelay;
                check();
                relClock = colDelay;
                check();
                rowSort[row].erase(rowSort[row].begin());
                queue.erase(beg);
            }
            it = rowSort.erase(it++);
        }
    }

    string printQ(){
        string out = "";
        for (auto& it : queue) {
            string a = "";
            for (auto& i: it.second.inst) {
                a += " " + i;
            }
            out += "    " + to_string(it.first) + ":" + a + "\n";
        }
        return out;
    }

    // --------------------------------------------------------------------------------------------------------

};






class MIPS {
    protected:
    unordered_map<int, vector<string>> instructions;
    unordered_map<int,int> lineCount;
    string regSteps = "";

    //  -------------------------ass4-addition-------------------------------------------

    unordered_map<string, int> labelLine;

    // --------------------------------------------------------------------------------------------------------

    DRAM ram;  
    string inst[10] = {"add","addi","sub","mul","beq","bne","slt","lw","sw","j"};
    int instCount[10] = {0}; 
    
    public:
    int getRegValue(string s){
        return ram.regs.at(s);
    }
    void feedReg(string s,int value){
        ram.regs.at(s)=value;
    }

    bool isReg(string s){
        if (ram.regs.find(s) == ram.regs.end()) {
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

    void setInstLine(int addressInst, int line) {
        lineCount[addressInst] = line;
    }

    //  -------------------------ass4-addition-------------------------------------------

    void setLabel(string name, int address) {
        labelLine[name] = address;
    }

    bool isInstPos(string name){
        if (labelLine.find(name) == labelLine.end()){
            return false;
        }
        else if (instructions.find(labelLine[name]) == instructions.end()){
            return false;
        }
        return true;
    }

    // --------------------------------------------------------------------------------------------------------

    void setDelay(int rowd, int cold){
        ram.rowDelay = rowd;
        ram.colDelay = cold;
    }

    void printInstSet(vector<string> v){
        for (auto& it : v) { 
            string str = "";
            if (it != ")" && it != "(" && it != ",") {str = " ";}
            regSteps = regSteps + str +  it; 
        } 
    }

    //  -------------------------ass4-addition-------------------------------------------

    void printRegSet2(int k, vector<string> v, string ramRegSteps,string p){
        string task = v.at(0);
        regSteps = regSteps + "\n-----------------------------Line: " +to_string(k) + "----------------------------";


        if (ramRegSteps != ""){
            regSteps += ramRegSteps + "\n";   
        }

        regSteps+= "\nClock: " + to_string(ram.clock) +"\n  Instruction called: ";
        printInstSet(v);   

        if (task == "add" || task == "sub" || task == "mul" || task == "addi" || task == "slt"){
            regSteps = regSteps+ "\n  Register modified : " + v.at(1) + " = " + to_string(ram.regs[v.at(1)]); 
        }
        else {
            regSteps += "\n" + p;
        }

        p = "";
        if (ram.isOn) {
            string a;
            for (auto& it : ram.currInst) { 
                a = a + " " +  it; 
            }
            p += "\n";
            p += "\n  [DRAM Execution going on: " + a+  ']';
            p += "\n  [Row active: " + to_string(ram.rowNum) + "]";
            string temp = "";
            for (auto &it : ram.rowSort) {
                temp += to_string(it.first) + ":" + to_string(it.second.size()) + " ";
            }
            p = p + "\n  [Instructions in queue: " + to_string(ram.queue.size()) + " (" + temp + ")]";
            if (ram.queue.size() != 0) {
                p += "\n  [Queue: \n";
                p += ram.printQ() + "  ]";
            }
        }      

        regSteps = regSteps + p + "\n";
    }

    // --------------------------------------------------------------------------------------------------------

    void printReg(){
        cout<< regSteps + "\n"<<endl;
    }

    void printClock(){
        cout<< "---------------------------------------Execution complete----------------------------------\n"<<endl;
        cout<< "Number of clock Cycles: " << ram.clock <<"\n" <<endl;
    }

    void printChangeMem() {
        cout << "Memory Updates" << endl;
        for(int i =0 ; i<1024; i++) {
            for (int j =0; j<256; j++) {
                if (ram.memory[i][j] != 0) {
                    cout<< 4*(j +256*i) << "-" << 4*(j +256*i) +3<< " =" <<ram.memory[i][j] <<endl;
                }
            }
        }
    }
    void printregVal() {
        cout << "\nNon zero register values:"<<endl;

         for (map<string, int>::iterator itr = ram.regs.begin(); itr != ram.regs.end(); ++itr) { 
             if (itr->second != 0){
                cout << itr->first  << " = " << itr->second << " | "; 
             }
        } 
    }

    void printInstCount() {
        cout<< "Number of row buffer updates: " << ram.updates[0]<<endl;
        cout<< "Total number of changes made in row buffer: " << ram.updates[1]<< "\n" << endl;

        printChangeMem();
        cout<< "\nExecution count of instructions:"<<endl;
        for (int i =0; i< 10; i++) {
            cout << inst[i] << ": "<< instCount[i]<<endl;
        }
        printregVal();
    }


    // -------------------ass4-addttion------------------------------------

    string getOut() {
        string out = "";

        for(int i =0 ; i<1024; i++) {
            for (int j =0; j<256; j++) {
                if (ram.memory[i][j] != 0) {
                   out = out + to_string(4*(j +256*i)) + "-" + to_string(4*(j +256*i) +3) + " =" + to_string(ram.memory[i][j]) + "\n";
                }
            }
        }

        for (int i =0; i< 10; i++) {
            out = out + inst[i] + ": "+ to_string(instCount[i]) + "\n";
        }

        for (map<string, int>::iterator itr = ram.regs.begin(); itr != ram.regs.end(); ++itr) { 
             if (itr->second != 0){
                out = out + itr->first  + " = " + to_string(itr->second) + " | "; 
             }
        } 

        return out;
    }


    int getClock() {
        return ram.clock;
    }

    // --------------------------------------------------------------------------------------------------------

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

                    // -----------------------------------ass4-addition------------------------------------------------
                    
                    string temp = (v.at(5));

                    // --------------------------------------------------------------------------------------------------------
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

                    // -----------------------------------ass4-addition------------------------------------------------

                    string temp = (v.at(1));

                    // --------------------------------------------------------------------------------------------------------
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

    bool executeInst(int mainInst){

        int i= mainInst;
        int n=instructions.size()+1;
        int k=1;
        string p;
        while(i<n){
            vector<string> v=instructions.at(i);
            auto pointer = lineCount.find(i);
            k=pointer->second;
            p = "";
            // cout << ram.printQ() << endl;
            string a= v.at(0);
            if (isError(v,k) ==false) {
                return false;
            }
            if (a=="add"){
                ram.doLSnot(v);
                feedReg(v.at(1),getRegValue(v.at(3))+getRegValue(v.at(5)));
                instCount[0] += 1;
                i++;
            }
            else if (a=="sub"){
                ram.doLSnot(v);
                feedReg(v.at(1),getRegValue(v.at(3))-getRegValue(v.at(5)));
                instCount[2] += 1;
                i++;
            }
            else if (a=="mul"){
                ram.doLSnot(v);
                feedReg(v.at(1),getRegValue(v.at(3))*getRegValue(v.at(5)));
                instCount[3] += 1;
                i++;              
            }
            else if (a=="addi"){
                ram.doLSnot(v);
                feedReg(v.at(1),getRegValue(v.at(3))+stoi(v.at(5)));
                instCount[1] += 1;
                i++;               
            }
            else if (a=="lw" || a=="sw"){

                // -----------------------------------ass4-addition------------------------------------------------
                int add;
                if (v.size() == 6) {
                    add = ram.regs.at(v.at(4));
                }
                else if (v.size() == 7) {
                    add = ram.regs.at(v.at(5))+stoi(v.at(3));
                }

                if (add%4 !=0 || add<0 || add >= 4*1024*256) {
                    cout << "INVALID address of memory at line "<< k<<endl;
                    return false;
                }
                ram.doLS(v);
                p = "\n  DRAM request issued";
                i++;
                if (a == "sw"){
                    instCount[8] += 1;    
                    
                }
                else {
                    instCount[7] += 1;
                    
                }
            }
            else if (a=="j"){

                //  ---------------------------------ass4-addition-------------------------------------------

                int a = labelLine[v.at(1)];
                ram.clock += 1;
                ram.relClock += 1;
                ram.check();
                instCount[9] += 1;
                i=a;
                p = "   Jumping to line: " + to_string(lineCount[i]);


            }
            else if (a=="beq"){

                ram.doLSnot(v);
                // ---------------------------------ass4-addition----------------------------------------------------

                if (getRegValue(v.at(1))==getRegValue(v.at(3))){
                    i=labelLine[v.at(5)];
                    p = "   Jumping to line: " + to_string(lineCount[i]);
                }

                else {
                    p = "   No jumping";
                    i++;
                }
                instCount[4] += 1;
            }
            else if (a=="bne"){

                ram.doLSnot(v);
                // ---------------------------------ass4-addition----------------------------------------------------

                if (getRegValue(v.at(1))!=getRegValue(v.at(3))){
                    i=labelLine[v.at(5)];
                    p = "   Jumping to line: " + to_string(lineCount[i]);
                }

                else {
                    p = "   No jumping";
                    i++;
                } 
                instCount[5] += 1;              
            }
            else if (a=="slt"){

                ram.doLSnot(v);
                if (getRegValue(v.at(3))<getRegValue(v.at(5))){
                    feedReg(v.at(1),1);
                }
                else {
                    feedReg(v.at(1),0);
                }  
                i++; 

                instCount[6] += 1;          
            }
            else {
                cout<<"Unrecognised operation at line no."<<i<<endl;
                return false;
                break;
            } 
            
            printRegSet2(k,v,ram.regSteps,p);
            ram.regSteps = "";
        }

        if (ram.isOn == true){
            ram.doAll();
            regSteps += ram.regSteps + "\n";
        }
        if (ram.updated == false) {
            ram.finalWrite();
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
        if (isspace(line[i])){
            if (str != "") {
                v.push_back(str);
                str = "";
            }
        }
        else if (line[i] == ',' || line[i] == ':' || line[i] == '(' || line[i] == ')') {
            if (str != "") {
                v.push_back(str);
                str = "";
            }
            v.push_back(string (1, line[i]));
        }
        else if (line[i] == '#') {
            if (str != "") {
                v.push_back(str);
                str = "";
            }
            break;
        }
        else {
            str += line[i];
        }
        i += 1;
    }
    if (str != "") {
        v.push_back(str);
    }
    return v;
}
int main(int argc, char const *argv[])
{

    if (argc == 1) {
        cout << "Enter the file name\n" << endl;
        return 0;
    }

    string infile = argv[1];
    string line;
    ifstream file(infile);

    if (argc < 4){
        cout << "Enter delays \n" << endl;
        return 0;
    }
    int rowdel =  stoi(argv[2]);
    int coldel = stoi(argv[3]);

    if (rowdel < 0 || coldel < 0){
        cout << "Delays can not be negative \n" << endl;
        return 0;
    }

    MIPS program;
    program.setDelay(rowdel,coldel);


    int addressVal = 0;
    int lineVal = 0;
    
    bool isMain = false;
    bool isError = false;
    int mainInst = 1;

    while (getline(file,line)){
        vector<string> currLine = lineToken(line);
        if (currLine.size() ==0 || int(currLine.at(0).at(0)) == 0){
                lineVal += 1;
        }
        else if (currLine.size() > 1 && currLine.at(1) == ":") {
            lineVal += 1;
            if (currLine.size() != 2){
                isError = true;
                break;
            }
            if (currLine.at(0) == "main"){
                isMain = true;
                mainInst = addressVal + 1;
            }
            program.setLabel(currLine.at(0),addressVal+1);
        }
        else {
            addressVal+=1;
            lineVal += 1;
            program.setInst(addressVal,currLine);
            program.setInstLine(addressVal,lineVal);
        }
    }

    if (isError) {
        cout<<"Invalid label at line: " << lineVal << endl;
        return -1;
    }

    if (isMain == false){
        cout<< "Missing Main Label" << endl;
        return -1;
    }

    
    bool isDone = program.executeInst(mainInst);
    if (isDone == false) {
        return -1;
    }

    program.printReg();
    program.printClock();
    program.printInstCount();

    return 0;
}