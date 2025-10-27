#include <cstdio>

template<typename T>
T getNum(){
    T res=0; char c;
    while(1){
        c=getchar();
        if(c==' '||c=='\n')continue;
        else break;
    }
    res=c-'0';
    while(1){
        c=getchar();
        if(c>='0'&&c<=9)res=10*res+c-'0';
        else break;
    }
    return res;
}


class Jobs{
    int t1,t2;
    public:
    Jobs (int a,int b):t1(a),t2(b){
    }
    int readT1() {return t1;}
    int readT2() {return t2;}

    bool operator<(const Jobs &b)const{
        return (min(t1, b.t2) < min(t2, b.t1));
    }
};

int main(){

}