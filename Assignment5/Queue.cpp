#include <bits/stdc++.h>
#include "Waiter.cpp"


class Queue{

private:
    int f = 0;
    int r= 0;
    string bubble = "";


public:
    int N = 32;
    Waiter Q[32];
    

    int size(){

        stringstream ss(bubble);
        string newBubble = "";

        int i = 0;
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');

            i +=1;    
        } 

        return(N-f+r-i)%N;
    }

    bool isEmpty(){
        // cout << "f "<< f << " r " <<r <<endl;
        if (f == r) {
            return true;
        }
        return false;
    }

    bool isFull() {
        int s = size();
        return (s == N -1);
    }

    void addBubble(int index) {
        if (bubble == "") {
            bubble = to_string(index);
        }
        else if (!isBubble(index)){
            bubble += "," + to_string(index);
        }
    }

    Waiter front() {
        return Q[f];
    }

    Waiter pop(){
        Waiter temp = Q[f];

        int next = (f+1)%N;

        if (bubble != ""){

            stringstream ss(bubble);
            string newBubble = "";
    
            while (ss.good()) {
                string substr;
                getline(ss, substr, ',');
                int num = stoi(substr);

                if (num == next) {
                    next = (next+1)%N;
                }
                else {
                    if (newBubble == "") {newBubble = substr;}
                    else {
                        newBubble = newBubble + "," + substr;
                    }
                }
            } 

            bubble = newBubble;
        }
        f = next;
        return temp;
    }

    void add(Waiter o){
        Q[r] = o;
        r = (r +1)%N;
    }

    bool isBubble(int index) {

        if (bubble == ""){
            return false;
        }
        stringstream ss(bubble);

        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            // cout << substr << endl;
            if (stoi(substr) == index) {
                return true;
            }
        }
        return false;
    }

    int getf(){return f;}
    int getr(){return r;}

};