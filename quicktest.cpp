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
    string outfile="results.csv";
    ofstream out(outfile);
    classifier* c;
    c=new classifier(fname);
    c->process(fname);
    ifstream in("agaricus-lepiota");
    int count=0;
    string hold;
    int num_wrong=0;
    int num_right=0;
    while(getline(in,hold))
    {
        //cout<<hold<<endl;
        strip_char(hold, "\n");
        string* csv=split(hold, ',');
        int ret=c->classify(csv);
//        cout<<"class number calculated: "<<ret<<endl;
//        cout<<"actual: "<<csv[1].at(0)<<endl;
        if(c->class_ind(csv[1].at(0))!=ret)
        {
            num_wrong++;
        }
        else{
            num_right++;
        }
        ++count;
    }
    cout<<num_wrong<<" wrong"<<endl;
    cout<<num_right<<" right"<<endl;
    out<<count<<","<<num_right<<","<<num_wrong<<endl;
    in.close();
    
    
    return 0;
}
