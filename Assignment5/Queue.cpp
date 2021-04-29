#include "Waiter.cpp"

class Queue{

private:
    int f = 0;
    int r= 0;


public:
    int N = 64;
    Waiter Q[64];
    

    int size(){
        return(N-f+r)%N;
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

    Waiter front() {
        return Q[f];
    }

    Waiter pop(){
        Waiter temp = Q[f];
        f = (f+1)%N;
        return temp;
    }

    void add(Waiter o){
        Q[r] = o;
        r = (r +1)%N;
    }

    int getf(){return f;}
    int getr(){return r;}

};