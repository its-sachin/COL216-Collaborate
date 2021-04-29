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
    program.init(1,rowdel,coldel);


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