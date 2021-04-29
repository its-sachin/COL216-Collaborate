#include "Waiter.cpp"

class Queue{

public:
    int N = 64;
    Waiter Q[64];
    int f;
    int r;

    int size(){
        return(N-f+r)%N;
    }

    bool isEmpty(){
        return(f=r);
    }

    bool isFull() {
        int s = size();
        return (s = N -1);
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

};