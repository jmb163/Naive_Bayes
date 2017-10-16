#include"util.hpp"
#include"classifier.hpp"
#include<stdlib.h>
#include<iostream>
#include<string>
#include<fstream>

using namespace std;

int main(int argc, char* argv[])
{
	srandomdev();
//	string fname=argv[1];
//	float prop=stof(argv[2]);
//	split_set(fname, prop);
//	return 0;
    string fname=argv[1];
    classifier* c;
    c=new classifier(fname);
    c->process(fname);
    cout<<"here"<<endl;
    ifstream in("agaricus-lepiota");
    int count=0;
    string hold;
    while(getline(in,hold) && count<10)
    {
        //cout<<hold<<endl;
        strip_char(hold, "\n");
        string* csv=split(hold, ',');
        //cout<<c->classify(csv)<<endl;
        c->classify(csv);
        ++count;
    }
    in.close();
    return 0;
}
