#include"classifier.hpp"
#include"util.hpp"
#include<fstream>
#include<iostream>
#include<boost/property_tree/xml_parser.hpp>
#include<boost/property_tree/ptree.hpp>
#include<boost/foreach.hpp>
#include<string>
#include<locale>
#include<algorithm>
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

char classifier::classify(string* csv)
{
    //new line should already be stripped iterate over the values
    //record the error of the classification
    char cls=csv[1].at(0);
    //compute the probability of each class and then report the char of the most probable
    double pattr=1;
    double* dep_pattr=new double[num_classes];
    double* pc=new double[num_classes];
    double* p=new double[num_classes]; //this variable will hold the final probability
    //the other variables will hold intermediate values to aid in calculation
    dep_pattr[0]=dep_pattr[1]=1;
    pc[0]=pc[1]=1;
    for(int i=0; i<num_classes; ++i)
    {
        int dim[]={i};
        pc[i]=p_c->at(dim);
    }
    //compute dependent probability
    for(int i=0; i<num_attr; ++i)
    {
        int subind[]={i,at_list[i+1]->get_index(csv[i+1].at(0))};
        pattr*=p_attr->at(subind);
    }
    for(int i=0; i<num_classes; ++i)
    {
        for(int j=0; j<num_attr; ++j)
        {
            int num_e=at_list[j+1]->num_entries;
            int subind[]={i,j,at_list[j+1]->get_index(csv[j+1].at(0))};
            dep_pattr[i]*=dep_p_attr->at(subind);
        }
    }
    for(int i=0; i<num_classes; ++i)
    {
        p[i]=((dep_pattr[i]*pc[i])/pattr);
    }
    char* possible=at_list[0]->get_entries();
    double max=p[0];
    for(int i=0;i<num_classes; ++i)
    {
        max=(p[i]>max) ? p[i] : max;
        //cout<<"probability "<<i<<": "<<pc[i]<<endl;
    }
    int index=0;
    for(int i=0; i<num_classes; ++i)
    {
        index = (p[i]==max) ? i : index;
    }
    for(int i=0; i<num_classes; ++i)
    {
        cout<<"probabilty of class "<<i<<": "<<pc[i]<<endl;
        cout<<"probability of attributes: "<<pattr<<endl;
        cout<<"dependent probabilty of attributes given class "<<i<<": "<<dep_pattr[i]<<endl;
    }
    return possible[index];
}

void classifier::create_config()
{
    //print_attr();
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
        //pt1[i].put("num_entries",putme);
        pt1[i].put("number", to_string(i));
        pt::ptree *attr_p=new pt::ptree[num_e];
        char* attrs=at_list[i]->get_entries();
        pt1[i].put("entries", char_array_to_string(attrs, num_e));
    }
    for(int i=0; i<num_attr+1;++i)
    {
        classifier->add_child("at_list.list", pt1[i]);
    }
    const string filename="nbayes_config.xml";
    const pt::ptree save_me=*classifier;
    const boost::property_tree::xml_writer_settings<pt::ptree::key_type>& settings=boost::property_tree::xml_writer_make_settings<pt::ptree::key_type>('\t', 1);
    pt::xml_parser::write_xml(filename, save_me, std::locale(), settings);
    return;
}

void classifier::load_attr_list(boost::property_tree::ptree pt)
{
    namespace ptr=boost::property_tree;
    list** attr_list=new list*[num_attr+1];
    for(int i=0;i<num_attr+1;++i)
    {
        attr_list[i]=new list;
    }
    ptr::ptree p;
    ptr::ptree temp;
    p=pt.get_child("at_list");
    temp=p.get_child("list");
    int count=0;
    BOOST_FOREACH(ptr::ptree::value_type& v, pt.get_child("at_list"))
    {
        BOOST_FOREACH(ptr::ptree::value_type& s, v.second.get_child(""))
        {
            string hold=s.second.data();
            string first=s.first;
            string check=s.first;
            if(check=="entries")
            {
                string hold=s.second.data();
                for(int i=0; i<hold.length(); ++i)
                {
                    attr_list[count]->d_add(hold.at(i));
                }
                count++;
            }
        }
    }
    at_list=attr_list;
    //print_attr();
    return;
}

void classifier::load_config(boost::property_tree::ptree pt)
{
    boost::property_tree::read_xml("nbayes_config.xml", pt);
    max_attr=pt.get<int>("param.max_attr");
    num_attr=pt.get<int>("param.num_attr");
    num_classes=pt.get<int>("param.num_classes");
    load_attr_list(pt);
    return;
}

void classifier::print_attr()
{
    for(int i=0; i < num_attr+1; ++i)
    {
        cout<<"list: "<<i<<endl;
        char* a=at_list[i]->get_entries();
        for(int j=0; j<at_list[i]->num_entries; ++j)
        {
            cout<<" "<<a[j]<<endl;
        }
    }
    return;
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
        in.close();
        at_list=preprocess(fname);
        create_config();
        
    }
    else
    {
        in.close();
        boost::property_tree::ptree pt;
        boost::property_tree::xml_parser::read_xml("nbayes_config.xml", pt);
        load_config(pt);
    }
    int pc_dems[]={num_classes};
    int pattr_dems[]={num_attr, max_attr};
    int dep_dems[]={num_classes, num_attr, max_attr};
    
    //allocate summation matrices
    p_c=new matrix<double>(pc_dems, 1, 0);
    p_attr=new matrix<double>(pattr_dems, 2, 0);
    dep_p_attr=new matrix<double>(dep_dems, 3, 0);
    //config is loaded, and or created, proceed to count through data,
    //structures will now be initialized to hold probabilities
    //basically the training set will be iterated through, and the success of the model
    //determined by the accuracy on the test set
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

void classifier::process(string fname)
{
    //read in the data, build probability matrices
    ifstream in(fname);
    if(in.fail()||!in)
    {
        cout<<"problem loading file"<<endl;
        return;
    }
    string hold;
    int count=0;
    
    int pc_dems[]={num_classes};
    int pattr_dems[]={num_attr, max_attr};
    int dep_dems[]={num_classes, num_attr, max_attr};
    
    //allocate summation matrices
    matrix<int>* s_p_c=new matrix<int>(pc_dems, 1, 0);
    matrix<int>* s_p_attr=new matrix<int>(pattr_dems, 2, 0);
    matrix<int>* s_dep_p_attr=new matrix<int>(dep_dems, 3, 0);
    
    while(getline(in, hold))
    {
        strip_char(hold, "\n");
        string* dat=split(hold,',');
        char d=dat[1].at(0);
        int c_ind=at_list[0]->get_index(d);//what is the index of the class label in question
        int pcd[]={c_ind};
        s_p_c->set(pcd, s_p_c->at(pcd)+1);//increment the class counter
        //now sum over the attributes
        for(int i=0; i<num_attr; ++i)
        {
            //this is a little confusing but basically we are just taking a sum
            //of what particular data appears to be used later in probability computations
            d=dat[i+2].at(0);
            int at_index=at_list[i+1]->get_index(d);
            int pad[]={i, at_index};
            s_p_attr->set(pad, s_p_attr->at(pad)+1);
            int pdepd[]={c_ind,i,at_index};
            s_dep_p_attr->set(pdepd, s_dep_p_attr->at(pdepd)+1);
            //cout<<s_dep_p_attr->at(pdepd)<<endl;
        }
        ++count; //keep count of how many entries, useful for later calculations
    }
    char* temp=at_list[0]->get_entries();
    //set the class probabilities
    for(int i=0; i<num_classes; ++i)
    {
        int ind[]={at_list[0]->get_index(temp[i])};
        p_c->set(ind, (double)(s_p_c->at(ind)/count));
    }
    //set the attribute probabilities
    for(int i=0; i<num_attr; ++i)
    {
        int n_at=at_list[i+1]->num_entries;
        temp=at_list[i+1]->get_entries();
        for(int j=0; j<n_at; ++j)
        {
            char d=temp[j];
            int ind[]={i, at_list[i+1]->get_index(d)};
            p_attr->set(ind, (double)s_p_attr->at(ind)/count);
            //cout<<(double)s_p_attr->at(ind)/count<<endl;
        }
    }
    int dep_count=0;
    //set the dependent probabilities from the summation matrices
    //a little dense perhaps, but it should work
    
    //[class index][attribute_number][index of attribute in list]
    //sum across the class values to get the total of an attribute given its class
    int dims[]={0,0,0};
    for(int i=0; i<num_attr; ++i)
    {
        dims[1]=i;
        int n_at=at_list[i+1]->num_entries;
        temp=at_list[i+1]->get_entries();
        for(int j=0; j<n_at; ++j)
        {
            dims[2]=j;
            for(int h=0; h<num_classes; ++h)
            {
                dims[0]=h;
                dep_count+=s_p_attr->at(dims); //sum across the classes
                //calculate the dependent probability of each attribute
            }
            for(int h=0; h<num_classes; ++h)
            {
                dims[0]=h;
                dep_p_attr->set(dims, (double)s_dep_p_attr->at(dims)/dep_count);
                cout<<"dependent probabilty: "<<dep_p_attr->at(dims)<<endl;
            }
            dep_count=0;
            //dims[2]=0;
        }
    }
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

int list::get_index(char c)
{
    datum* temp=head;
    int ret=0;
    bool found=false;
    while(temp!=NULL)
    {
        if(temp->data==c)
        {
            found=true;
            break;
        }
        temp=temp->next;
        ret++;
    }
    return (found) ? ret : -1;
}




