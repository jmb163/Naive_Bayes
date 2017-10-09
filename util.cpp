#include"util.hpp"
#include<string>
#include<algorithm>
#include<sstream>
#include<iostream>
#include<sstream>
#include<fstream>
#include<stdlib.h>
#include<iomanip>
#include<cstring>

using namespace std;

//void strip_char(string& inpt, char chr)
//{
//    for(int j=0; j < inpt.length(); j++)
//    {
//        if(inpt.at(j)==chr)
//        {
//            inpt=inpt.erase(j, j+1);
//        }
//    }
//}

void strip_char(string& inpt, char* chrs)
{
    for (unsigned int i = 0; i < strlen(chrs); ++i)
    {
        inpt.erase(remove(inpt.begin(), inpt.end(), chrs[i]), inpt.end());
    }
}

//for when there's a lot of space either before or after the meat of a string
void strip_space(string& inpt, bool back)
{
    //int pos=inpt.find_first_of(" ");
    int pos = (back) ? inpt.find_first_of(" ") : inpt.find_last_of(" ");
    while(pos!=string::npos)
    {
        if(back)
        {
            if(inpt[pos+1]==' ')
            {
                inpt=inpt.substr(0, pos);
                break;
            }
            pos=inpt.find_first_of(" ",pos+1);
        }
        else
        {
            if(inpt[pos-1]==' ')
            {
                inpt=inpt.substr(pos+1, inpt.length()-pos-1);
                break;
            }
            pos=inpt.find_last_of(" ",pos-1);
        }
    }
    return;
}

string caps(const string in, bool upper)
{
    //lowercase 97 - 122
    //uppercase 65 - 90
    //add 32 to uppercase to get lower
    //sub 32 from lower   to get upper
    string copy=in;
    for(int i=0;i<in.length(); ++i)
    {
        copy[i] = (upper) ? (((int)copy[i]<=122 && (int)copy[i]>=97) ? copy[i]=(char)((int)copy[i]-32) : copy[i]) : (((int)copy[i]<=90 && (int)copy[i]>=65) ? copy[i]=(char)((int)copy[i]+32) : copy[i]);
        //I understand that this is a mess of ternary operators, but it works quite elegantly
    }
    return copy;
}

//for this program, every hash will be created from a version of the place's name, in which
//the name will be cast to lowercase and all the spaces removed. making the search more flexible
int hash_f(const string s, int size)
{
    int value = 79561;
    for(int i = 0; i < s.length(); ++i)
    {
        value = value+69+s[i];
    }
    if(value < 0)
    {
        return (-1)*value;
    }
    return value % size;
}

string* split(const string s, char d)
{
    //split a string s along a delimeter;
    //only works on formatted data (think csv)
    int divs=0;
    for(int i=0;i<s.length();++i)
    {
        if(s.at(i)==d)
        {
            divs++;
        }
    }
    string* answer=new string[divs+2];//+2 because there will be one more string than there are dividers,
    //and there needs to be space to indicate how many strings there are in total
    answer[0]=to_string(divs+2);
    int start=0;
    int end=0;
    int length=0;
    for(int i=0;i<divs+1;++i)
    {
        end=(i < divs) ? s.find_first_of(d, start) : (s.length());
        length=end-start;
        answer[i+1]=s.substr(start, length);
        start=end+1;
    }
    return answer;
}

int random_range(const int start, const int end)
{
    return (start + random()%(end-start+1));
}

void split_set(string fname, float proportion)
{
    //splits the given file into two new files
    //one training set, and one testing set according to the
    //given proportion
    //the given proportion determines the size of the test set with regard to the full set
    if(proportion>=1)
    {
        cout<<"invalid proportion"<<endl;
        return;
    }
    srandomdev();
    string train_set_name=fname;
    string test_set_name=fname;
    
    float test_size=1-proportion;
    stringstream ss;
    ss<<fixed<<setprecision(2)<<proportion;
    
    string tr_set=ss.str();
    
    ss.str(string());
    ss<<fixed<<setprecision(2)<<test_size;
    
    string te_set=ss.str();
    
    train_set_name+=tr_set; //create logical filenames for soon to be created files
    test_set_name+=te_set;
    train_set_name+="train.csv";
    test_set_name+="test.csv";
    
    int cutoff=(int)(proportion*100);
    ofstream train(train_set_name);
    ofstream test(test_set_name);
    ifstream in(fname);
    if(train.fail()||test.fail()||in.fail())
    {
        cout<<"Error loading or creating one of the files"<<endl;
        return;
    }
    string hold;
    int indicator;
    while(getline(in,hold))
    {
        indicator=random_range(0,100);
        if(indicator>=cutoff)
        {
            test<<hold<<endl;
        }
        else
        {
            train<<hold<<endl;
        }
    }
    in.close();
    test.close();
    train.close();
    return;
}

int get_index(char* arr, int size, char check)
{
    for(int i=0; i<size; ++i)
    {
        if(arr[i]==check)
        {
            return i;
        }
    }
    return -1; //return negative if the index is not found
}

//end=(i < divs) ? s.find_first_of(d, start) : (s.length()-1);
//length+=end;
//answer[i+1]=s.substr(start, end);
//start=end+1;







