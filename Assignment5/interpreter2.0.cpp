#include "DRAM.cpp"



class MIPS {
    protected:
    unordered_map<int, vector<string>> instructions;
    unordered_map<int,int> lineCount;
    string regSteps = "";

    int coreNum;
    Register currReg;

    //  -------------------------ass4-addition-------------------------------------------

    unordered_map<string, int> labelLine;

    // --------------------------------------------------------------------------------------------------------

    DRAM ram;  
    string inst[10] = {"add","addi","sub","mul","beq","bne","slt","lw","sw","j"};
    int instCount[10] = {0}; 
    
    public:

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

    void init(int core, int rowd, int cold){
        ram.rowDelay = rowd;
        ram.colDelay = cold;

        coreNum = core;
        currReg.coreNumber = core;
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
            regSteps = regSteps+ "\n  Register modified : " + v.at(1) + " = " + to_string(currReg.getRegValue(v.at(1))); 
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
            int total = 0;
            int curr = 0;
            for (int i =0; i< 1024; i++) {
                if (!ram.rowSort[i].isEmpty()) {
                    curr = ram.rowSort[i].size();
                    total += curr;
                    temp += to_string(i) + ":" + to_string(curr) + " ";
                }
            }
            p = p + "\n  [Instructions in queue: " + to_string(curr) + " (" + temp + ")]";
            if (!ram.isEmpty()) {
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

    void printInstCount() {
        cout<< "Number of row buffer updates: " << ram.updates[0]<<endl;
        cout<< "Total number of changes made in row buffer: " << ram.updates[1]<< "\n" << endl;

        printChangeMem();
        cout<< "\nExecution count of instructions:"<<endl;
        for (int i =0; i< 10; i++) {
            cout << inst[i] << ": "<< instCount[i]<<endl;
        }
        currReg.printregVal();
    }


    // -------------------ass4-addttion------------------------------------


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
            else if (currReg.isReg(v.at(1)) ==false || currReg.isReg(v.at(3)) ==false || (task != "addi" && currReg.isReg(v.at(5)) == false)) {
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
            else if (currReg.isReg(v.at(1)) == false || currReg.isReg(v.at(3)) == false) {
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
                if (currReg.isReg(v.at(5)) ==false){
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
                else if (currReg.isReg(v.at(1)) == false || currReg.isReg(v.at(4)) == false){
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
                else if (currReg.isReg(v.at(1)) == false || currReg.isReg(v.at(5)) == false){
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

    void doLSnot(vector<string> v) {
        ram.doLSnot(v);
        if ((currReg.flag != -1)&& currReg.flag == ram.clock){
            ram.clock += 1;
            currReg.flag = -1;
        }
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
                doLSnot(v);
                currReg.feedReg(v.at(1),currReg.getRegValue(v.at(3))+currReg.getRegValue(v.at(5)));
                instCount[0] += 1;
                i++;
            }
            else if (a=="sub"){
                doLSnot(v);
                currReg.feedReg(v.at(1),currReg.getRegValue(v.at(3))-currReg.getRegValue(v.at(5)));
                instCount[2] += 1;
                i++;
            }
            else if (a=="mul"){
                doLSnot(v);
                currReg.feedReg(v.at(1),currReg.getRegValue(v.at(3))*currReg.getRegValue(v.at(5)));
                instCount[3] += 1;
                i++;              
            }
            else if (a=="addi"){
                doLSnot(v);
                currReg.feedReg(v.at(1),currReg.getRegValue(v.at(3))+stoi(v.at(5)));
                instCount[1] += 1;
                i++;               
            }
            else if (a=="lw" || a=="sw"){

                // -----------------------------------ass4-addition------------------------------------------------
                int add;
                if (v.size() == 6) {
                    add = currReg.getRegValue(v.at(4));
                }
                else if (v.size() == 7) {
                    add = currReg.getRegValue(v.at(5))+stoi(v.at(3));
                }

                if (add%4 !=0 || add<0 || add >= 4*1024*256) {
                    cout << "INVALID address of memory at line "<< k<<endl;
                    return false;
                }
                // if (ram.isFul()) {
                //     cout << "DRAM Queue full!!"<< endl;
                //     return false;
                // }
                ram.doLS(v,&currReg);
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

                if (currReg.getRegValue(v.at(1))==currReg.getRegValue(v.at(3))){
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

                if (currReg.getRegValue(v.at(1))!=currReg.getRegValue(v.at(3))){
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

                doLSnot(v);
                if (currReg.getRegValue(v.at(3))<currReg.getRegValue(v.at(5))){
                    currReg.feedReg(v.at(1),1);
                }
                else {
                    currReg.feedReg(v.at(1),0);
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
