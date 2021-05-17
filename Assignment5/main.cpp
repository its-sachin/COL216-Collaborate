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


bool checkFinished(int N){
    for(int i=0;i<N;i++){
        if (stuck[i]!=-2){
            return false;
        }
    }
    return true;
}

bool executeInst(int N,int M){
    vector<string> arayIns[N];
    unordered_map<string, int> lables[N];
    int lineNo[N] = {0};
    int arr[N]={0};
    int pc = 0;

    while (true){

        if (ram.clock==0){

            for (int i=0;i<N;i++){

                pc = programs[i].mainInst;
                arayIns[i] = programs[i].getInst(pc);
                lineNo[i] = programs[i].getLine(pc);
                arr[i] = pc;
            }
        }
        else if (ram.clock>=M){
            string p = "------------------------------------Stopping the execution-------------------------------------";
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
                        if (curr!=0) {
                            temp += to_string(i) + ":" + to_string(curr) + " ";
                        }
                    }
                }
                p = p + "\n  [Instructions in queue: " + to_string(curr) + " (" + temp + ")]";
                if (!ram.isEmpty()) {
                    p += "\n  [Queue: \n";
                    p += ram.printQ() + "  ]";
                }
            }   
            ram.regSteps = p + "\n";
            return true;
        }
        else {
            if (checkFinished(N)){
                // all programs done
                ram.regSteps ="---------------------------------------Execution complete----------------------------------\n";
                return true;
            }
            else {
                for (int i=0;i<N;i++){
                     cout << ram.clock << " " << i << " " << stuck[i] << endl;
                    if (stuck[i]==-1){
                        // this core is done
                        if (arr[i]+1>programs[i].getInstruction().size()){
                            stuck[i]=-2;
                        }

                        // this core neither stuck nor done 
                        else {
                            pc = arr[i]+1;
                            arayIns[i]=programs[i].getInst(pc);
                            lineNo[i] = programs[i].getLine(pc);
                            arr[i] = pc;
                        }
                    }

                    // stuck or done
                    else if (stuck[i]==0 || stuck[i]==-2){
                        continue;
                    } 

                    // jump
                    else {
                        pc = stuck[i];
                        arayIns[i]=programs[i].getInst(pc);
                        lineNo[i] = programs[i].getLine(pc);
                        arr[i] = pc;
                        stuck[i]=-1;
                    }         
                }
            }
        }
        ram.doIns(N,arayIns,lineNo);
    }
    return true;
}





int main(int argc, char const *argv[])
{
    if (argc == 1) {
        cout << "Enter number of files\n" << endl;
        return -1;
    }
    int N = stoi(argv[1]);

    if (N % 2 !=0 && N!=1){
        cout << "No of cores must be a multiple of 2\n";
        return -1;
    }

    if (argc == 2) {
        cout << "Enter number of cycles\n" << endl;
        return -1;
    }
    M = stoi(argv[2]);

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
    dependence = new pair<int,int>[N];
    stuck= new int[N];
    for (int i=0; i< N; i++) {
        stuck[i] = -1;
    }
    programs = new MIPS[N];

    
    for (int i =0; i< N; i++) {
        allReg[i].coreNumber = i;
        programs[i].init(i);


        string line;
        ifstream file(infiles[i]);

        int addressVal = 0;
        int lineVal = 0;

        bool isError=true;
        
        bool isMain = false;
        int mainInst = 1;

        while (getline(file,line)){
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
            }
        }

        if (isMain == false){
            cout<< "Missing Main Label in file of core: "<< i+1 << endl;
            return -1;
        }
        programs[i].mainInst=mainInst;

        unordered_map<int, vector<string>> instructions=programs[i].getInstruction();
        for (auto j = instructions.begin(); j != instructions.end(); j++){
            isError=isError && programs[i].isError(j->second,programs[i].getLine(j->first));
            if (!isError){
                return -1;
            }
        }

    }

    bool isDone = executeInst(N,M);
    if (isDone == false) {
        return -1;
    }

    for (int i=0; i < N; i++) {
        programs[i].printAll();
    }
    ram.printAll();

    if (argc > 5+N) {

        for (int i=0; i < N; i++) {
            ofstream myfile; 

            myfile.open("regafter" + to_string(i+1) + ".txt");
            myfile<< allReg[i].getRegVal() << endl;   
        }
    }

    return 0;
}