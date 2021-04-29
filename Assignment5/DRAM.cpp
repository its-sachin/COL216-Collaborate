
#include "Queue.cpp"



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

    Register *currReg;
    
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

    void start(vector<string> v, Register *greg){
        currInst = v;
        changeReg  = currInst.at(1);
        if (currInst.size() == 6) {
            addressReg = currInst.at(4);
            address = greg->getRegValue(v.at(4));
        }
        else if (currInst.size() == 7) {
            addressReg = currInst.at(5);
            address = greg->getRegValue(v.at(5))+stoi(v.at(3));
        }
        isOn = true;
        relClock =0;
        colNum = (address%1024)/4; 
        rowNum = address/1024;
        initialClock = clock;
        rowDone = false;
        currReg = greg;

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
                currReg->feedReg(changeReg,rowBuffer[colNum]);
                regSteps = regSteps+ "\n  Register modified : " + changeReg + " = " + to_string(currReg->getRegValue(changeReg));
                clock = initialClock + colDelay;
                currReg->flag = clock;
            }
            else if (currInst.at(0) == "sw"){
                regSteps = regSteps + "\nClock: " + to_string(initialClock+1) + "-"+ to_string(initialClock+colDelay) +"\n  DRAM: Column Access: " + to_string(colNum);
                rowBuffer[colNum] = currReg->getRegValue(changeReg);
                regSteps = regSteps+ "\n  Memory address modified : " + to_string(address) + " = " + to_string(currReg->getRegValue(changeReg));
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

        currReg = w.currReg;
    }


   void initWaiter(vector<string> v, Register *greg) {
        Waiter w;
        w.inst = v;
        w.changeReg  = v.at(1);

        int add;
        if (v.size() == 6) {
            w.addressReg = v.at(4);
            add = currReg->getRegValue(v.at(4));
        }
        else if (v.size() == 7) {
            w.addressReg = v.at(5);
            add = currReg->getRegValue(v.at(5))+stoi(v.at(3));
        }
        w.address = add;
        int row = add/1024;
        w.rowNum = row;
        w.currReg = greg;


        // if (v.at(0) == "lw") {
        //     for (auto it = queue.begin(); it != queue.end(); ){
        //         if (it->second.inst.at(0) == "lw" && w.changeReg == it->second.changeReg) {
        //             rowSort[it->second.rowNum].erase(remove(rowSort[it->second.rowNum].begin(), rowSort[it->second.rowNum].end(), it->first), rowSort[it->second.rowNum].end());
        //             queue.erase(it++);
        //             // cout << "kk" << endl;
        //         }
        //         else {
        //             ++it;
        //         }
        //     }
        // }
        // else {
        //     for (auto it = rowSort[row].begin(); it != rowSort[row].end();){
        //         if (queue[*it].inst.at(0) == "sw" && add == queue[*it].address) {
        //             it = rowSort[row].erase(it);
        //             queue.erase(*it);
        //         }
        //         else {
        //             ++it;
        //         }
        //     }
        // }
        
        rowSort[w.rowNum].add(w);
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
                add = currReg->getRegValue(v.at(4));
            }
            else if (v.size() == 7) {
                a1 = v.at(5);
                add = currReg->getRegValue(v.at(5))+stoi(v.at(3));
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
                add = currReg->getRegValue(v.at(4));
            }
            else if (v.size() == 7) {
                a1 = v.at(5);
                add = currReg->getRegValue(v.at(5))+stoi(v.at(3));
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
        for (int i = rowSort[row].r - 1; i != rowSort[row].f - 1; i = (i-1)%rowSort[row].N){
            if (isDep(v, rowSort[row].Q[i])){
                return i;
            }
        }
        return -1;
    }

    unordered_map<int,int> allDep(vector<string> v) {
        unordered_map<int,int> ans;
        for (int i = 0; i < 1024; i++ ) {
            if (i != rowNum) {
                int curr = depInRow(v, i); 
                if (curr != -1) {
                    ans[i] = curr;
                }
            }
        }
        return ans;
    }

    void doRow(int row, int bound) {
        while (true) {
            if (rowSort[row].isEmpty()) {
                break;
            }
            int beg = rowSort[row].f;
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

    void doDep(vector<string> v) {
        unordered_map<int,int> all = allDep(v); 
        if (all.empty()) {
            if (rowSort[rowNum].isEmpty()) {
                for (int i =0; i< 1024; i++) {
                if (!rowSort[i].isEmpty()) {
                    start(rowSort[i].pop());
                }
            }
            }
            else {
                
                int curr = depInRow(v, rowNum) ;
                if (curr== -1) {
                    start(rowSort[rowNum].pop());
                }

                else {
                    doRow(rowNum, curr);
                    if (isOn == false) {

                        initDep();
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
                initDep();
            }
        }
    }


    void doLS(vector<string> v, Register *greg) {

        if (isOn == false) {
            clock += 1;
            start(v,greg);
            return;
        }

        unordered_map<int,int> all = allDep(v); 

        if (all.empty()) {
            relClock += 1;
            clock += 1;
            check();
            if (isOn) {
                initWaiter(v,greg);
            }
            else if (isEmpty()){
                start(v,greg);
            }
            else {
                initDep();
                initWaiter(v,greg);
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
                if (isEmpty()){
                    start(v,greg);
                    return;
                }

                else {
                    initDep();
                    initWaiter(v,greg);
                }

            }

            else {
                initWaiter(v,greg);
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

                if (isEmpty()){
                    clock += 1;
                    return;
                }

                else {
                    doDep(v);
                    clock += 1;
                }

            }

            else  {
                if (isEmpty() || (depInRow(v,rowNum) == -1 && allDep(v).empty())){
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
            for (int i=rowSort[row].f; i <rowSort[row].r; i = (i+1)%rowSort[row].N) {
                string a = "";
                for (auto& i: rowSort[row].Q[i].inst) {
                    a += " " + i;
                }
                out += "    " + a + "\n";
            }
        }
        return out;
    }

    // --------------------------------------------------------------------------------------------------------

};


