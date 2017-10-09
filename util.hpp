#ifndef UTIL_HPP_
#define UTIL_HPP_
#include<string>
using namespace std;

void strip_char(string&, char*);

void strip_space(string&,bool);

string caps(const string, bool);

int hash_f(const string, int);

string* split(const string, char);

void split_set(string fname, float proportion);

int random_range(const int start, const int end);

int get_index(char*, int, char);

#endif /* UTIL_HPP_ */
