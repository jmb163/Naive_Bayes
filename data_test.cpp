#include<fstream>
#include"classifier.hpp"
#include"util.hpp"
#include<iostream>
#include<string>
#include<sstream>

using namespace std;


int main(int argc, char** argv)
{
    float start=0.01;
    string train="train.csv";
    string test="test.csv";
    for(int i=0; i<8; ++i)
    {
        //start=0.1*(i+1);
        start+=(i*0.01);
        split_set("agaricus-lepiota", start);
    }
    
    
    
    srandomdev();
    //	string fname=argv[1];
    //	float prop=stof(argv[2]);
    //	split_set(fname, prop);
    //	return 0;
    string fname=argv[1];
    string outfile=argv[2];
    ofstream out(outfile);
    start=0.01;
    cout<<"training now"<<endl;
    for(int i=0; i<8; ++i)
    {
        //start=0.1*(i+1);
        start+=(i*0.01);
        float test_size=1-start;
        stringstream ss;
        ss<<fixed<<setprecision(2)<<start;
        
        string tr_set=ss.str();
        string training_set_name="agaricus-lepiota"+ss.str();
        ss.str(string());
        
        ss<<fixed<<setprecision(2)<<test_size;
        
        string testing_set_name="agaricus-lepiota"+ss.str();
        
        testing_set_name+=test;
        training_set_name+=train;
        
        classifier* c;
        c=new classifier(fname);
        int num_trained=c->process(training_set_name);
        ifstream in(testing_set_name);
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
        out<<count<<","<<num_trained<<","<<num_right<<","<<num_wrong<<","<<((double)num_right/count)<<endl;
    }
    out.close();
    
    
    
    
    return 0;
}
