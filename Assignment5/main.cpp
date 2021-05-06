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

bool executeInst(int N,MIPS *programs,int M){
    vector<string> arayIns[N];
    int arr[N]={0};
    unordered_map<string, int> lables[N];
    while (true){
        if (ram.clock==0){
            for (int i=0;i<N;i++){
                arr[i]=programs[i].mainInst;
                arayIns[i]=programs[i].getInst(programs[i].mainInst);
                lables[i]=programs[i].getLabelLine();
                programs[i].printRegSet2(arr[i], arayIns[i], ram.regSteps, "");
            }
        }
        else if (ram.clock>M){
            return true;
        }
        else {
            if (checkFinished(N)){
                // all programs done
                return true;
            }
            else {
                for (int i=0;i<N;i++){
                    if (stuck[i]==-1){
                        // this core is done
                        if (arr[i]+1>programs[i].getInstruction().size()){
                            stuck[i]=-2;
                            vector<string> tombstone;
                            arayIns[i]=tombstone;
                        }

                        // this core neither stuck nor done 
                        else {
                            arr[i]=arr[i]+1;
                            arayIns[i]=programs[i].getInst(arr[i]);
                    
                            programs[i].printRegSet2(arr[i], arayIns[i], ram.regSteps, "");
                        }
                    }

                    // stuck or done
                    else if (stuck[i]==0 || stuck[i]==-2){
                        continue;
                    } 

                    // jump
                    else {
                        arr[i]=stuck[i];
                        arayIns[i]=programs[i].getInst(stuck[i]);
                        stuck[i]=-1;
                    }          
                }
            }
        }
        ram.doIns(N,arayIns,lables);
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
    dependence = new pair<int,int>[N];
    stuck= new int[N];
    *stuck={-1};
    MIPS programs[N];
    bool isError=true;
    for (int i =0; i< N; i++) {
        allReg[i].coreNumber = i;
        programs[i].init(i);


        string line;
        ifstream file(infiles[i]);

        int addressVal = 0;
        int lineVal = 0;
        
        bool isMain = false;
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
        programs[i].mainInst=mainInst;
    }
    for (int j=0;j<N;j++){
        unordered_map<int, vector<string>> instructions=programs[j].getInstruction();
        for (auto i = instructions.begin(); i != instructions.end(); i++){
            isError=isError && programs[j].isError(i->second,i->first);
            if (!isError){
                return -1;
            }
        }
    }

    bool isDone = executeInst(N,programs,M);
    if (isDone == false) {
        return -1;
    }

    for (int i=0; i < N; i++) {
        programs[i].printAll();
    }
    ram.printAll();

    return 0;
}