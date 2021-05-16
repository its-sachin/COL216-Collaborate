
#include "interpreter2.0.h"

Register *allReg;
pair<int,int>* dependence;
int *stuck;
MIPS *programs;

class DRAM {
    public:
    int TotalCore;
    bool isOn;
    vector<string> currInst;
    int clock = 0;
    int relClock =0;
    int rowDelay=0;
    int colDelay=0;
    int updates[2] = {0};
    bool rowDone = false;
    int wasted = 0;

    int currI = -1;

    vector<int> priority;
    vector<pair<int,int>> dependency;
    int currCore = -1;
    
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



    int offsetAdd(int a,int coreID){
        if (a>= 1024*1024/TotalCore){
            throw "Inavlid Adress is provided";
        }
        return a+ coreID*1024*1024/TotalCore;
    }

    void start(vector<string> v, int coreID){
        currCore = coreID;
        currInst = v;
        changeReg  = currInst.at(1);
        if (currInst.size() == 6) {
            addressReg = currInst.at(4);
            try {
                address = offsetAdd(allReg[currCore].getRegValue(v.at(4)),currCore);              
            } catch (const char* msg) {
                cerr << msg << endl;
            }
        }
        else if (currInst.size() == 7) {
            addressReg = currInst.at(5);
            try {
                address = offsetAdd(allReg[currCore].getRegValue(v.at(5))+stoi(v.at(3)),currCore);              
            } catch (const char* msg) {
                cerr << msg << endl;
            }
        }
        isOn = true;
        relClock =0;
        colNum = (address%1024)/4; 
        rowNum = address/1024;
        initialClock = clock;
        rowDone = false;
        if (v.at(0) == "lw") {
            programs[coreID].incInstCount(7);
        }

        else {
            programs[coreID].incInstCount(8);
        }

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
                allReg[currCore].feedReg(changeReg,rowBuffer[colNum]);
                regSteps = regSteps+ "\n  Register modified : " + changeReg + " = " + to_string(allReg[currCore].getRegValue(changeReg));
                clock = initialClock + colDelay;
                allReg[currCore].flag = clock;
            }
            else if (currInst.at(0) == "sw"){
                regSteps = regSteps + "\nClock: " + to_string(initialClock+1) + "-"+ to_string(initialClock+colDelay) +"\n  DRAM: Column Access: " + to_string(colNum);
                rowBuffer[colNum] = allReg[currCore].getRegValue(changeReg);
                regSteps = regSteps+ "\n  Memory address modified : " + to_string(address) + " = " + to_string(allReg[currCore].getRegValue(changeReg));
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

    Queue rowSort[1024];

    // void p() {
    //     for (int i =0; i< 1024; i++) {
    //         if (!rowSort[i].isEmpty()){
    //             for (int j = rowSort[i].getf(); j != rowSort[i].getr(); j = (j+1)%rowSort[i].N){
    //                 cout<< i << " "<< rowSort[i].Q->inst.at(1) << endl;
    //             }
    //         }
    //     }
    // }
    
    bool isEmpty() {
        for (int i =0; i< 1024; i++) {
            if (!rowSort[i].isEmpty()){
                return false;
            }
        }
        return true;
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

        currCore = w.coreID;
        currI = w.currI;
    }


    void initWaiter(vector<string> v, int coreID) {
        Waiter w;
        w.inst = v;
        w.changeReg  = v.at(1);

        int add;
        if (v.size() == 6) {
            w.addressReg = v.at(4);
            try {
                add = offsetAdd(allReg[coreID].getRegValue(v.at(4)),coreID);              
            } catch (const char* msg) {
                cerr << msg << endl;
            }
        }
        else if (v.size() == 7) {
            w.addressReg = v.at(5);
            try {
                add = offsetAdd(allReg[coreID].getRegValue(v.at(5))+stoi(v.at(3)),coreID);              
            } catch (const char* msg) {
               cerr << msg << endl;
            }
        }
        w.address = add;
        int row = add/1024;
        w.rowNum = row;
        w.coreID = coreID;

        w.currI = rowSort[w.rowNum].getr();
        rowSort[w.rowNum].add(w);

        if (v.at(0) == "lw") {
            programs[coreID].incInstCount(7);
        }

        else {
            programs[coreID].incInstCount(8);
        }
    }

    bool currIsDep(vector<string> v,int coreNo) {
        if (currCore!=coreNo){
            return false;
        }
        string task = v.at(0);
        if (task == "add" || task == "addi" || task == "mul" || task == "sub" || task == "slt") {
            if (changeReg == v.at(1) || (currInst.at(0) == "lw" && (changeReg == v.at(3) || changeReg == v.at(5)))){
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
                try {
                    add = offsetAdd(allReg[currCore].getRegValue(v.at(4)),currCore);              
                } catch (const char* msg) {
                    cerr << msg << endl;
                }
            }
            else if (v.size() == 7) {
                a1 = v.at(5);
                try {
                    address = offsetAdd(allReg[currCore].getRegValue(v.at(5))+stoi(v.at(3)),currCore);              
                } catch (const char* msg) {
                    cerr << msg << endl;
                }
            }

            if (currInst.at(0) == "lw") {

                if (task == "lw" && ( a1 == cs)){
                    return true;
                }

                else if (task == "sw" && (c1 == cs || a1 == cs || (add == address))){
                    return true;
                }
            }

            else{

                if (task == "lw" && (c1 == cs || (add == address))){
                    return true;
                }

            }

        }
 
        return false;
    }

    bool isDep(vector<string> v, Waiter w,int coreNo) {
        if (w.coreID!=coreNo){
            return false;
        }
        string task = v.at(0);
        if (task == "add" || task == "addi" || task == "mul" || task == "sub" || task == "slt") {
            if (w.changeReg == v.at(1) || (w.inst.at(0) == "lw" && (w.changeReg == v.at(3) || w.changeReg == v.at(5)))){
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
                try {
                    add = offsetAdd(allReg[w.coreID].getRegValue(v.at(4)),w.coreID);              
                } catch (const char* msg) {
                    cerr << msg << endl;
                }
            }
            else if (v.size() == 7) {
                a1 = v.at(5);
                try {
                    add = offsetAdd(allReg[w.coreID].getRegValue(v.at(5))+stoi(v.at(3)),w.coreID);              
                } catch (const char* msg) {
                    cerr << msg << endl;
                }
            }

            if (w.inst.at(0) == "lw") {

                if (task == "lw" && ( a1 == cs)){
                    return true;
                }

                else if (task == "sw" && (c1 == cs || a1 == cs || (add == w.address))){
                    return true;
                }
            }

            else{

                if (task == "lw" && (c1 == cs || (add == w.address))){
                    return true;
                }

            }

        }
        return false;
    }

    int depInRow(vector<string> v, int row, int coreNo){

        if (rowSort[row].isEmpty()){
            return -1;
        }
        int r = rowSort[row].getr();
        int f = rowSort[row].getf();
        for (int i = f; i != r; i = (i+1)%rowSort[row].N){

            if (isDep(v, rowSort[row].Q[r+f-i-1],coreNo)){
                return (r+f-1-i);
            }

            if (rowSort[row].Q[i].inst[0] == "lw" && v[0] == "lw") {
                if (rowSort[row].Q[i].changeReg == v.at(1)) {
                    rowSort[row].addBubble(i);
                }
            }

            else if (rowSort[row].Q[i].inst[0] == "sw" && v[0] == "sw") {
                int add;

                if (v.size() == 6) {
                    try {
                        add = offsetAdd(allReg[coreNo].getRegValue(v.at(4)),coreNo);              
                    } catch (const char* msg) {
                        cerr << msg << endl;
                    }
                }
                else if (v.size() == 7) {
                    try {
                        add = offsetAdd(allReg[coreNo].getRegValue(v.at(5))+stoi(v.at(3)),coreNo);              
                    } catch (const char* msg) {
                    cerr << msg << endl;
                    }
                }

                if (rowSort[row].Q[i].address == add) {
                    rowSort[row].addBubble(i);
                }
            }

        }
        return -1;
    }

    vector<pair<int,int>> allDep(vector<string> v,int coreNo) {

        string a = "";
        for (auto& i: v) {
            a += " " + i;
        }

        regSteps = regSteps + "\nClock: " + to_string(clock) +"\n  DRAM: Dependency Check Delay for core: " + to_string(coreNo+1) + "\n   For Instruction: " + a + "\n";
        clock += 1;
        wasted += 1;


        vector<pair<int,int>> ans;
        for (int i = 0; i < 1024; i++ ) {
            int curr = depInRow(v, i,coreNo); 
            if (curr != -1) {
                ans.push_back(make_pair(i,curr));
            }
        }
        if (currIsDep(v,coreNo) && isOn){
            ans.push_back(make_pair(rowNum,-1));
        }
        return ans;
    }

    void doRow(int row, int bound) {
        while (true) {
            if (rowSort[row].isEmpty()) {

                break;
            }
            int beg = rowSort[row].getf();
            start(rowSort[row].pop());
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
        if (rowSort[rowNum].isEmpty()) {
            for (int i =0; i< 1024; i++) {
                if (!rowSort[i].isEmpty()) {
                    start(rowSort[i].pop());
                }
            }
        }

        else {
            start(rowSort[rowNum].pop());
        }
    }

    void doDep(vector<string> v, int coreNo) {
        vector<pair<int,int>> all = allDep(v,coreNo); 
        if (all.empty()) {
            if (rowSort[rowNum].isEmpty()) {
                for (int i =0; i< 1024; i++) {
                    if (!rowSort[i].isEmpty()) {
                        start(rowSort[i].pop());
                    }
                }
            }
            else {
                int curr = depInRow(v, rowNum,coreNo) ;
                if (curr== -1) {
                    start(rowSort[rowNum].pop());
                }

                else {
                    doRow(rowNum, curr);
                    if (isOn == false) {

                        if (isEmpty()) {
                            return;
                        }

                        initDep();
                    }
                }
            }
        }

        else {
            doRow(rowNum, -1);
            int i = 0;

            for (auto& it : all ){
                i += 1;
                if (it.first != rowNum) {
                    if (i == all.size()) {
                        doRow(it.first, it.second);
                    }
                    else {
                        doRow(it.first,-1);
                    }
                }
            }

            all.clear();

            if (isOn == false) {
                initDep();
            }
        }
    }

    void performInst(vector<string> v,int coreNo, int lineNo){

        if ((allReg[coreNo].flag != -1)&& allReg[coreNo].flag == clock){
            clock += 1;
            allReg[coreNo].flag = -1;
        }


        string a= v.at(0);
        string p = "";

        if (a=="add"){
            allReg[coreNo].feedReg(v.at(1),allReg[coreNo].getRegValue(v.at(3))+allReg[coreNo].getRegValue(v.at(5)));
            programs[coreNo].incInstCount(0);
        }
        else if (a=="sub"){
            allReg[coreNo].feedReg(v.at(1),allReg[coreNo].getRegValue(v.at(3))-allReg[coreNo].getRegValue(v.at(5)));
            programs[coreNo].incInstCount(2);
        }
        else if (a=="mul"){        
            allReg[coreNo].feedReg(v.at(1),allReg[coreNo].getRegValue(v.at(3))*allReg[coreNo].getRegValue(v.at(5)));
            programs[coreNo].incInstCount(3);
        }
        else if (a=="addi"){
            allReg[coreNo].feedReg(v.at(1),allReg[coreNo].getRegValue(v.at(3))+stoi(v.at(5)));               
            programs[coreNo].incInstCount(1);
        }
        else if (a=="slt"){
            if (allReg[coreNo].getRegValue(v.at(3))<allReg[coreNo].getRegValue(v.at(5))){
                allReg[coreNo].feedReg(v.at(1),1);
            }
            else {
                allReg[coreNo].feedReg(v.at(1),0);
            }        
            programs[coreNo].incInstCount(6);
        }
        else if (a=="beq"){

            if (allReg[coreNo].getRegValue(v.at(1))==allReg[coreNo].getRegValue(v.at(3))){
                int i = programs[coreNo].getLabelLine(v.at(5));
                stuck[coreNo]=i;
                p = "   Jumping to line " + to_string(programs[coreNo].getLine(i));
            }
            else {
                p = "   No jumping";
            }
            programs[coreNo].incInstCount(4);
            
        }
        else if (a=="bne"){

            if (allReg[coreNo].getRegValue(v.at(1))!=allReg[coreNo].getRegValue(v.at(3))){
                int i=programs[coreNo].getLabelLine(v.at(5));  
                stuck[coreNo]=i;
                p = "   Jumping to line " + to_string(programs[coreNo].getLine(i));
            }
            else {
                p = "   No jumping";
            }
            programs[coreNo].incInstCount(5);
            
        }
        else if (a=="j"){   
            int i = programs[coreNo].getLabelLine(v.at(1));
            stuck[coreNo]=i;
            programs[coreNo].incInstCount(9);
            p = "   Jumping to line " + to_string(programs[coreNo].getLine(i));
        }
        programs[coreNo].printRegSet2(lineNo, v, p);
    }

    void doIns(int N, vector<string> *arrayIns, int *lineNo) {
        TotalCore=N;

        if (isOn) {
            relClock += 1;
            clock += 1;
            check();
            handleBlock(arrayIns);
        }
        if (isOn) {
            
            for (int i =0; i< N; i++) {
                if (arrayIns[i].size() != 0 && stuck[i] != -2) {
                    vector<pair<int,int>> all = allDep(arrayIns[i],i);
                    if (arrayIns[i].at(0) == "lw" || arrayIns[i].at(0) == "sw"){
                        //no dependency
                        if (all.empty()) {
                            if (isOn) {
                                initWaiter(arrayIns[i],i);
                            }
                            else if (isEmpty()){
                                start(arrayIns[i],i);
                            }
                            else {
                                initDep();
                                initWaiter(arrayIns[i],i);
                            }
                            programs[i].printRegSet2(lineNo[i], arrayIns[i], "    DRAM Request Issued");
                        }
                        //depenedency hence this core is now blocked
                        else { 
                            if (stuck[i]!=0){
                                if (priority.size() == 0){
                                    dependency = all;
                                }
                                priority.push_back(i);
                                stuck[i]=0;
                            }
                        }        
                    }
                    else {
                        //no dependency
                        if (all.empty()){
                            performInst(arrayIns[i],i,lineNo[i]);
                        }
                        //dependency so this core  is blocked
                        else {
                            if (stuck[i]!=0){
                                if (priority.size() == 0){
                                    dependency = all;
                                }
                                priority.push_back(i);
                                stuck[i]=0;
                            }
                        }                  
                    }
                }          
            }
        }
        else {
            bool started = false;
            clock+=1;
            for (int i =0; i< N; i++) {
                if (arrayIns[i].size() !=0 && stuck[i] != -2) {
                    if ((arrayIns[i].at(0) == "lw" || arrayIns[i].at(0) == "sw")) {
                        programs[i].printRegSet2(lineNo[i], arrayIns[i], "  DRAM Request Issued");
                        if (started == false) {
                            start(arrayIns[i],i);
                            started = true;
                        }
                        else {
                            initWaiter(arrayIns[i],i);
                        }
                    }
                    else {
                        performInst(arrayIns[i],i,lineNo[i]);
                    }
                }
            }
        }
    }
    void initTask(vector<string> inst,int coreNo) {

        if (rowSort[rowNum].isEmpty()) {
            for (int i =0; i< 1024; i++) {
                if (depInRow(inst,i,coreNo)!=-1) {
                    start(rowSort[i].pop());
                }
            }
        }

        else {
            start(rowSort[rowNum].pop());
        }
    }

    int getDepCurr() {

        for (auto& it : dependency ){
            if (it.first == rowNum) {
                return it.second;
            }
        }
        return -1;

    }
    void handleBlock(vector<string> *arrayIns){
        if (!isOn && !priority.empty()){

            int currRowDep = getDepCurr();

            if (currRowDep != -1 && currI != -1){

                // current row complete
                int i = 0;
                if (currI == currRowDep){
                    for (auto& it : dependency ){
                        if (it.first == rowNum) {
                            dependency.erase(dependency.begin() + i);
                            break;
                        }
                        i += 1;
                    }

                    if (dependency.empty() ) {
                        stuck[priority[0]]=-1;
                        priority.erase(priority.begin());
                    }

                }

            }
            if (priority.empty()){
                return;
            }

            initTask(arrayIns[priority[0]],priority[0]);
            
        }
    }

    void doAll() {
        relClock = rowDelay;
        check();
        relClock = colDelay;
        check();
        doRow(rowNum, -1);
        for (int row = 0; row< 1024; row++ ){
            while (true) {
                if (rowSort[row].isEmpty()) {
                    break;
                }
                start(rowSort[row].pop());
                relClock = rowDelay;
                check();
                relClock = colDelay;
                check();
            }
        }
    }

    string printQ(){
        string out = "";
        for (int row = 0; row< 1024; row++ ){
            for (int i=rowSort[row].getf(); i <rowSort[row].getr(); i = (i+1)%rowSort[row].N) {
                string a = "";
                for (auto& i: rowSort[row].Q[i].inst) {
                    a += " " + i;
                }
                out += "    " + a + "\n";
            }
        }
        return out;
    }

    void printClock(){
        cout<< "Number of clock Cycles: " << clock <<"\n" <<endl;
        cout << "Clocks delay in DRAM: " <<wasted << "\n" << endl;
    }

    void printChangeMem() {
        cout << "Memory Updates" << endl;
        for(int i =0 ; i<1024; i++) {
            for (int j =0; j<256; j++) {
                if (memory[i][j] != 0) {
                    cout<< 4*(j +256*i) << "-" << 4*(j +256*i) +3<< " =" <<memory[i][j] <<endl;
                }
            }
        }
    }

    void printUpdates() {
        cout<< "Number of row buffer updates: " << updates[0]<<endl;
        cout<< "Total number of changes made in row buffer: " << updates[1]<< "\n" << endl;
    }

    void printAll() {
        cout << "\n" << regSteps << endl;
        printClock();
        printUpdates();
        printChangeMem();
        
    }

    // --------------------------------------------------------------------------------------------------------

};


