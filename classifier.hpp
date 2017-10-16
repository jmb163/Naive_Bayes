#ifndef CLASSIFIER_HPP_
#define CLASSIFIER_HPP_
#include<iostream>
#include<string>
#include<boost/property_tree/xml_parser.hpp>
#include<boost/property_tree/ptree.hpp>
#include"matrix.cpp"


using namespace std;

/*
 the preprocessor should be run on the full dataset to extract all of the necessary
 metadata.
 
 It is imperative to know the metadata ahead of time so that the probabilty matrices can
 be allocated properly
 
 
 */

struct datum
{
    char data;
    datum* next;
    
    datum();
    ~datum();
};

struct list
{
    datum* head;
    int num_entries;
    
    list();
    ~list();
    void d_add(char);
    bool d_in_list(char);
    char* get_entries();
    int get_index(char); //get the index of a char (position in linked list actually) or -1 if
    //not found
};

class classifier
{
private:
    int max_attr; //the max number of different possibilities for attribute values
    //for any given attribute
    int num_classes;//how many different classes are possible
    int num_attr; //number of atributes for the data
    
    matrix<double>* p_c;       //holds the sum of the classes that appear
    matrix<double>* p_attr;    //probability of a given attribute
    matrix<double>* dep_p_attr;//probability of a given attribute given its class
    
    /*
     p_c should be [num_classes]
     p_attr should be [at_list_index][number of attributes]
     dep_p_attr [num_classes][at_list_size][attribute]
     
     1D 2D 3D matrices
     */
    
    
    list** at_list;
    
    void init(string); //should be called after config
    list** preprocess(string);
    void create_config();
    void load_config(boost::property_tree::ptree);
    void load_attr_list(boost::property_tree::ptree);//creates an xml config file, if a config exists, it will be
    //able to make the classifier skip the preprocessing stage, and ostensibly the creation of
    //attribute tables.
    
    //the key here is that the attribute tables will be kept constant across all models to
    //increase modularity and reusability in the classifier
    

public:
    classifier(string);
    ~classifier();
    void query(string*); //return a class score
    void print_attr();
    int class_ind(char c);
    
    void process(string); //read data from a file, and populate the probability matrices
    int classify(string* csv); //given some comma seperated values, the model will output
    //classification based on the attributes
};






#endif /* CLASSIFIER_HPP_ */
