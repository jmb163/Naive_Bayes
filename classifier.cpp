#include"classifier.hpp"
#include"util.hpp"
#include<fstream>
#include<iostream>
#include<boost/property_tree/xml_parser.hpp>
#include<boost/property_tree/ptree.hpp>
#include<string>
#include<locale>
//#include"matrix.cpp"

using namespace std;

classifier::classifier(string fname)
{
    init(fname);
}

classifier::~classifier()
{
    max_attr=0;
    num_attr=0;
    num_classes=0;
}

void classifier::create_config()
{
    namespace pt=boost::property_tree;
    pt::ptree* pt1=new pt::ptree[num_attr+1];
    pt::ptree* classifier; //root node
    classifier=new pt::ptree;
    int num_e;
    pt::ptree* atrribute_list=new pt::ptree;
    
    classifier->put("param.max_attr",to_string(max_attr));
    classifier->put("param.num_attr",to_string(num_attr));
    classifier->put("param.num_classes", to_string(num_classes));
    classifier->put("at_list.num_lists",to_string(num_attr+1));
    for(int i=0; i<num_attr+1; ++i)
    {
        num_e=at_list[i]->num_entries;
        string putme=to_string(num_e);
        pt1[i].put("num_entries",putme);
        pt1[i].put("number", to_string(i));
        pt::ptree *attr_p=new pt::ptree[num_e];
        char* attrs=at_list[i]->get_entries();
        for(int j=0; j<num_e; ++j)
        {
            attr_p[j].put("char", attrs[j]);
            pt1[i].add_child("entry", attr_p[j]);
        }
    }
    
    for(int i=0; i<num_attr+1;++i)
    {
        classifier->add_child("at_list.list", pt1[i]);
    }
    const string filename="nbayes_config.xml";
    const pt::ptree save_me=*classifier;
    //ofstream outfile(filename);
    const boost::property_tree::xml_writer_settings<pt::ptree::key_type>& settings=boost::property_tree::xml_writer_make_settings<pt::ptree::key_type>('\t', 1);
    pt::xml_parser::write_xml(filename, save_me, std::locale(), settings);
    //pt::write_xml(filename,save_me);
    return;
}

void classifier::load_attr_list(boost::property_tree::ptree pt)
{
    list** attr_list=new list*[num_attr+1];
    for(int i=0;i<num_attr+1;++i)
    {
        attr_list[i]=new list;
    }
    //ptr::ptree pt1=new ptr::ptree;
    
}

void classifier::load_config(boost::property_tree::ptree pt)
{
    boost::property_tree::read_xml("nbayes_config.xml", pt);
    string clf="classifier";
    max_attr=pt.get<int>(clf+".param.max_attr");
    num_attr=pt.get<int>(clf+".param.num_attr");
    num_classes=pt.get<int>(clf+".param.num_classes");
    
}

void classifier::init(string fname)
{
    max_attr=0;
    num_attr=0;
    num_classes=0;
    ifstream in;
    in.open("nbayes_config.xml");
    if(!in||in.fail())
    {
        at_list=preprocess(fname);
        create_config();
    }
    //preprocessing and config is now done, at this point, begin loading data
    in.close();
    boost::property_tree::ptree pt;
    //boost::property_tree::read_xml("nbayes_config.xml", pt);
    return;
}

list** classifier::preprocess(string fname)
{
    //cout<<"made it to the preprocessor"<<endl;
    ifstream in;
    in.open(fname);
    if(!in||in.fail())
    {
        cout<<"problem opening data file"<<endl;
        return NULL;
    }
    string hold;
    string* dat;
    getline(in, hold);
    strip_char(hold, "\n");
    dat=split(hold,',');
    num_attr=stoi(dat[0]);
    list** attr_list= new list*[num_attr];
    for(int i=0; i<num_attr-1;++i)
    {
        attr_list[i]=new list;
    }
    while(getline(in,hold))
    {
        //first run through, basically creating metadata here
        strip_char(hold, "\n");
        dat=split(hold, ','); //split csv
        for(int i=0; i<num_attr-1; ++i)
        {
            if(!attr_list[i]->d_in_list((char)dat[i+1].at(0)))
            {
                attr_list[i]->d_add((char)dat[i+1].at(0));
                //cout<<i<<": "<<dat[i+1].at(0)<<endl;
            }
        }
    }
    for(int i=0; i<num_attr-1;++i)
    {
        //find the max number of attributes in the list
        //cout<<attr_list[i]->num_entries<<endl;
        max_attr=(attr_list[i]->num_entries > max_attr) ? attr_list[i]->num_entries : max_attr;
    }
    num_classes=attr_list[0]->num_entries;
    num_attr-=2;
    in.close();
//    cout<<num_classes<<endl;
//    cout<<num_attr<<endl;
//    cout<<max_attr<<endl;
    return attr_list;
}

datum::datum()
{
    next=NULL;
    data='\0';
}

datum::~datum()
{
    delete next;
    next=NULL;
    data='\0';
}

list::list()
{
    head=NULL;
    num_entries=0;
}

list::~list()
{
    delete head;
    num_entries=0;
}

void list::d_add(char c)
{
    if(head==NULL)//first entry
    {
        head=new datum();
        head->data=c;
        head->next=NULL;
    }
    else //add to tail
    {
        datum* d=new datum();
        datum* temp;
        temp=head;
        d->data=c;
        d->next=NULL;
        while(temp->next!=NULL)
        {
            temp=temp->next;
        }
        temp->next=d;
    }
    num_entries++;
    return;
}

bool list::d_in_list(char c)
{
    datum* temp;
    temp=head;
    while(temp!=NULL)
    {
        if(temp->data==c)
        {
            return true;
        }
        temp=temp->next;
    }
    return false;
}

char* list::get_entries()
{
    //cout<<num_entries<<endl;
    char* ret=new char[num_entries];
    datum* temp=head;
    //cout<<"here"<<endl;
    for(int i=0;temp!=NULL;i++)
    {
        //cout<<temp->data<<endl;
        ret[i]=temp->data;
        temp=temp->next;
    }
    return ret;
}




