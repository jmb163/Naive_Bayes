#include"util.hpp"
#include"classifier.hpp"
#include<stdlib.h>
#include<iostream>
#include<string>

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
    return 0;
}
