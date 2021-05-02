#include "interpreter2.0.cpp"



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


// void doLSnot(vector<string> v) {
//     ram.doLSnot(v);
//     if ((allReg[coreNum].flag != -1)&& allReg[coreNum].flag == ram.clock){
//         ram.clock += 1;
//         allReg[coreNum].flag = -1;
//     }
// }

    //exucuting the file 

bool executeInst(int N,MIPS *programs){

    // int i= mainInst;
    // int n=instructions.size()+1;
    // int k=1;
    // string p;
    // while(i<n){
    //     vector<string> v=instructions.at(i);
    //     auto pointer = lineCount.find(i);
    //     k=pointer->second;
    //     p = "";
    //     // cout << "line " <<  k << endl;
    //     // ram.p();
    //     string a= v.at(0);
    //     if (isError(v,k) ==false) {
    //         return false;
    //     }
    //     if (a=="add"){
    //         doLSnot(v);
    //         allReg[coreNum].feedReg(v.at(1),allReg[coreNum].getRegValue(v.at(3))+allReg[coreNum].getRegValue(v.at(5)));
    //         instCount[0] += 1;
    //         i++;
    //     }
    //     else if (a=="sub"){
    //         doLSnot(v);
    //         allReg[coreNum].feedReg(v.at(1),allReg[coreNum].getRegValue(v.at(3))-allReg[coreNum].getRegValue(v.at(5)));
    //         instCount[2] += 1;
    //         i++;
    //     }
    //     else if (a=="mul"){
    //         doLSnot(v);
    //         allReg[coreNum].feedReg(v.at(1),allReg[coreNum].getRegValue(v.at(3))*allReg[coreNum].getRegValue(v.at(5)));
    //         instCount[3] += 1;
    //         i++;              
    //     }
    //     else if (a=="addi"){
    //         doLSnot(v);
    //         allReg[coreNum].feedReg(v.at(1),allReg[coreNum].getRegValue(v.at(3))+stoi(v.at(5)));
    //         instCount[1] += 1;
    //         i++;               
    //     }
    //     else if (a=="lw" || a=="sw"){

    //         // -----------------------------------ass4-addition------------------------------------------------
    //         int add;
    //         if (v.size() == 6) {
    //             add = allReg[coreNum].getRegValue(v.at(4));
    //         }
    //         else if (v.size() == 7) {
    //             add = allReg[coreNum].getRegValue(v.at(5))+stoi(v.at(3));
    //         }

    //         if (add%4 !=0 || add<0 || add >= 4*1024*256) {
    //             cout << "INVALID address of memory at line "<< k<<endl;
    //             return false;
    //         }
    //         // if (ram.isFul()) {
    //         //     cout << "DRAM Queue full!!"<< endl;
    //         //     return false;
    //         // }
    //         ram.doLS(v,coreNum);
    //         p = "\n  DRAM request issued";
    //         i++;
    //         if (a == "sw"){
    //             instCount[8] += 1;    
                
    //         }
    //         else {
    //             instCount[7] += 1;
                
    //         }
    //     }
    //     else if (a=="j"){

    //         //  ---------------------------------ass4-addition-------------------------------------------

    //         int a = labelLine[v.at(1)];
    //         ram.clock += 1;
    //         ram.relClock += 1;
    //         ram.check();
    //         instCount[9] += 1;
    //         i=a;
    //         p = "   Jumping to line: " + to_string(lineCount[i]);


    //     }
    //     else if (a=="beq"){

    //         ram.doLSnot(v);
    //         // ---------------------------------ass4-addition----------------------------------------------------

    //         if (allReg[coreNum].getRegValue(v.at(1))==allReg[coreNum].getRegValue(v.at(3))){
    //             i=labelLine[v.at(5)];
    //             p = "   Jumping to line: " + to_string(lineCount[i]);
    //         }

    //         else {
    //             p = "   No jumping";
    //             i++;
    //         }
    //         instCount[4] += 1;
    //     }
    //     else if (a=="bne"){

    //         ram.doLSnot(v);
    //         // ---------------------------------ass4-addition----------------------------------------------------

    //         if (allReg[coreNum].getRegValue(v.at(1))!=allReg[coreNum].getRegValue(v.at(3))){
    //             i=labelLine[v.at(5)];
    //             p = "   Jumping to line: " + to_string(lineCount[i]);
    //         }

    //         else {
    //             p = "   No jumping";
    //             i++;
    //         } 
    //         instCount[5] += 1;              
    //     }
    //     else if (a=="slt"){

    //         doLSnot(v);
    //         if (allReg[coreNum].getRegValue(v.at(3))<allReg[coreNum].getRegValue(v.at(5))){
    //             allReg[coreNum].feedReg(v.at(1),1);
    //         }
    //         else {
    //             allReg[coreNum].feedReg(v.at(1),0);
    //         }  
    //         i++; 

    //         instCount[6] += 1;          
    //     }
    //     else {
    //         cout<<"Unrecognised operation at line no."<<i<<endl;
    //         return false;
    //         break;
    //     } 
        
    //     printRegSet2(k,v,ram.regSteps,p);
    //     ram.regSteps = "";
    // }

    // if (ram.isOn == true){
    //     ram.doAll();
    //     regSteps += ram.regSteps + "\n";
    // }
    // if (ram.updated == false) {
    //     ram.finalWrite();
    // }
    return true;

}





int main(int argc, char const *argv[])
{
    if (argc == 1) {
        cout << "Enter number of files\n" << endl;
        return -1;
    }

    int N = stoi(argv[1]);

    if (argc == 2) {
        cout << "Enter number of cycles\n" << endl;
        return -1;
    }

    int M = stoi(argv[2]);

    if (argc < 2+N) {
        cout << "Enter all files path\n" << endl;
    }

    string infiles[N];

    for (int i = 0; i < N; i++) {
        infiles[i] = argv[3+i];
    }


    if (argc < 4+N){
        cout << "Enter delays \n" << endl;
        return 0;
    }
    int rowdel =  stoi(argv[3+N]);
    int coldel = stoi(argv[4+N]);

    if (rowdel < 0 || coldel < 0){
        cout << "Delays can not be negative \n" << endl;
        return 0;
    }
    ram.rowDelay = rowdel;
    ram.colDelay = coldel;


    allReg = new Register[N];
    dependence = new vector<pair<int,int>>[N];
    priority = new int[N];
    priority = {0};
    MIPS programs[N];

    for (int i =0; i< N; i++) {
        allReg[i].coreNumber = i;
        programs[i].init(i);


        string line;
        ifstream file(infiles[i]);

        int addressVal = 0;
        int lineVal = 0;
        
        bool isMain = false;
        bool isError = true;
        int mainInst = 1;

        while (getline(file,line)){
            if (!isError) {
                return -1;
            }
            vector<string> currLine = lineToken(line);
            if (currLine.size() ==0 || int(currLine.at(0).at(0)) == 0){
                lineVal += 1;
            }
            else if (currLine.size() > 1 && currLine.at(1) == ":") {
                lineVal += 1;
                if (currLine.size() != 2){
                    isError = false;
                    cout<<"Invalid label at line: " << lineVal << "of core: " << i+1 << endl;
                    break;
                }
                if (currLine.at(0) == "main"){
                    isMain = true;
                    mainInst = addressVal + 1;
                }
                programs[i].setLabel(currLine.at(0),addressVal+1);
            }
            else {
                addressVal+=1;
                lineVal += 1;
                programs[i].setInst(addressVal,currLine);
                programs[i].setInstLine(addressVal,lineVal);
                isError = isError & programs[i].isError(currLine,lineVal);
            }
        }

        if (isMain == false){
            cout<< "Missing Main Label in file of core: "<< i+1 << endl;
            return -1;
        }
    }


    
    bool isDone = executeInst(N,programs);
    if (isDone == false) {
        return -1;
    }

    for (int i=0; i < N; i++) {
        programs[i].printAll();
    }
    ram.printAll();

    return 0;
}