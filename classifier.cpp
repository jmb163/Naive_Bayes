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

//this macro makes it easier and less error prone to increment the given coordinates
#define inc_matrix(m, d) m->set(d, m->at(d)+1)

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

int classifier::class_ind(char c)
{
    return at_list[0]->get_index(c);
}

int classifier::classify(string* csv)
{
//    p_c->print_native();
//    cout<<"end"<<endl;
//    p_attr->print_native();
//    cout<<"end"<<endl;
//    dep_p_attr->print_native();
//    cout<<"end"<<endl;
    
    int* indices =new int[num_attr];
    for(int i=0; i<num_attr; ++i)
    {
        indices[i]=at_list[i+1]->get_index(csv[i+2].at(0));
    }
    //cout<<"here"<<endl;
    double attribute_probabilities=1;
    double* class_probabilities =new double[num_classes];
    double* dependent_probabilities =new double[num_classes];
    for(int i=0; i<num_classes; ++i)
    {
        class_probabilities[i]=1;
        dependent_probabilities[i]=1;
    }
    //set the products to one initially so that the products can accumulate
    //the final probability will be computed from these values when they are ready
    //output and diagnostic information will be written to a log file
    int dependent_indices[]={0,0,0};
    int attribute_indices[]={0,0};
    //    matrix<double>* p_c;       //holds the sum of the classes that appear
    //    matrix<double>* p_attr;    //probability of a given attribute
    //    matrix<double>* dep_p_attr;//probability of a given attribute given its class
    //cout<<"Calculating probability of attributes"<<endl;
    for(int i=0; i<num_attr; ++i)
    {
        attribute_indices[0]=dependent_indices[1]=i;
        attribute_indices[1]=dependent_indices[2]=indices[i];
        attribute_probabilities*=1.1*p_attr->at(attribute_indices);
        //cout<<p_attr->at(attribute_indices)<<"x";
        //cout<<p_attr->at(attribute_indices)<<" x ";
        for(int j=0; j<num_classes; ++j)
        {
            dependent_indices[0]=j;
            //cout<<"attribute num: "<<i<<endl;
            //cout<<dep_p_attr->at(dependent_indices)<<" class: "<<j<<endl;
            dependent_probabilities[j]*=dep_p_attr->at(dependent_indices); //times 10 to avoid numerical underflow
        }
    }
    //cout<<endl;
    
    int class_indices[]={0};
    for(int i=0; i<num_classes; ++i)
    {
        class_indices[0]=i;
        class_probabilities[i]=p_c->at(class_indices);
    }
    ofstream out("logfile.txt", std::ios_base::app); //set the stream to append, an interested user can tail the file
    //for real time diagnostics or scroll through at their own liesure
    if(out.fail()||!out)
    {
        cout<<"issue creating log file, diagnostics will not be recorded!"<<endl;
        out.close();
    }
    double* final_prediction =new double[num_classes];
    int max=0;
    for(int i=0; i<num_classes; ++i)
    {
        final_prediction[i]=((dependent_probabilities[i]*class_probabilities[i])/attribute_probabilities);
    }

    max = std::distance(final_prediction, std::max_element(final_prediction, final_prediction+num_classes));
    for(int i=0; i<num_classes; ++i)
    {
        out<<"Diagnostics info:"<<endl;
        for(int j=0; j<num_attr; ++j)
        {
            out<<csv[j+1]<<",";
        }
        out<<endl;
        out<<"class index-> "<<i<<endl;
        out<<"class value-> "<<at_list[0]->get_entries()[i]<<endl;
        out<<"class probabilty-> "<<class_probabilities[i]<<endl;
        out<<"dependent probabilty-> "<<dependent_probabilities[i]<<endl;
        out<<"probability of given attributes-> "<<attribute_probabilities<<endl;
        final_prediction[i]=((class_probabilities[i]*dependent_probabilities[i])/attribute_probabilities);
        out<<"final prediction for class-> "<<final_prediction[i]<<endl;
    }
    out<<"final prediction is "<<at_list[0]->get_entries()[max]<<endl;
    out<<"max is: "<<max<<endl;
    return max;
    
}

//void classifier::process(string fname)
//{
//    ifstream in(fname);
//    if(!in||in.fail())
//    {
//        cout<<"problem loading the given file, exiting now..."<<endl;
//        return;
//    }
//    string hold;
//    int* c_summation=new int[num_classes];
//    int** attribute_summation[num_attr];
//    int*** dependent_summation[num_classes];
//    
//    int* num_e=new int[num_attr];
//    for(int i=0; i<num_attr; ++i)
//    {
//        num_e[i]=at_list[i+1]->num_entries;
//    }
//    
//    for(int i=0; i<num_classes; ++i)
//    {
//        dependent_summation[i]=new int*[num_attr];
//        for(int j=0; j<num_attr; j++)
//        {
//            dependent_summation[i][j]=new int[num_e[i]];
//        }
//    }
//    for(int i=0; i<num_attr; ++i)
//    {
//        attribute_summation[i]=new int[num_e[i]];
//    }
//    //done instantiating summation matrices
//    while(getline(in, hold))
//    {
//        stip_char(hold, "\n");
//        string* csv=split(hold,',');
//        int* indexes=new int[num_attr];
//        for(int i=0; i<num_attr+1; ++i)
//        {
//            indexes[i]=at_list[i]->get_index(csv[i+1].at(0));
//        }
//        int class_ind[]={indexes[0]};
//        //c_summation->set(class_ind, c_summation->at(class_ind)+1);
//        inc_matrix(c_summation, class_ind);
//        int pattr_index[]={0,0};
//        int dep_attr_index[]={indexes[0], 0, 0};
//        for(int i=0; i<num_attr; ++i)
//        {
//            pattr_index[0]=dep_attr_index[1]=i;
//            pattr_index[1]=dep_attr_index[2]=indexes[i+1]; //attribute subindex
//            inc_matrix(attribute_summation, pattr_index);
//            inc_matrix(dependent_summation, dep_attr_index);
//        }
//
//    }
//}

int classifier::process(string fname)
{
    ifstream in(fname);
    if(in.fail()||!in)
    {
        cout<<"problem loading training set, exiting now..."<<endl;
        return -1;
    }
    string hold;
    int c_dims[]={num_classes};
    int attr_dims[]={num_attr, max_attr};
    int dep_dims[]={num_classes, num_attr, max_attr};
    
    matrix<int>* c_summation=new matrix<int>(c_dims, 1, 0);
    matrix<int>* attribute_summation=new matrix<int>(attr_dims, 2, 0);
    matrix<int>* dependent_summation=new matrix<int>(dep_dims, 3, 0);
    int* indexes=new int[num_attr+1];
    
    int num_train=0;
    while(getline(in, hold))
    {
        //record of indexes in attribute list for easier times later
        strip_char(hold, "\n");
        string* csv=split(hold, ',');
        for(int i=0; i<num_attr+1; ++i)
        {
            indexes[i]=at_list[i]->get_index(csv[i+1].at(0));
        }
        int class_ind[]={indexes[0]};
        //c_summation->set(class_ind, c_summation->at(class_ind)+1);
        inc_matrix(c_summation, class_ind);
        int pattr_index[]={0,0};
        int dep_attr_index[]={indexes[0], 0, 0};
        for(int i=0; i<num_attr; ++i)
        {
            pattr_index[0]=dep_attr_index[1]=i;
            pattr_index[1]=dep_attr_index[2]=indexes[i+1]; //attribute subindex
            inc_matrix(attribute_summation, pattr_index);
            inc_matrix(dependent_summation, dep_attr_index);
        }
        num_train++;
    }
//    c_summation->print_native();
//    cout<<"end"<<endl;
//    attribute_summation->print_native();
//    cout<<"end"<<endl;
//    dependent_summation->print_native();
//    cout<<"end"<<endl;
    //cout<<"finished summation phase 1"<<endl;
    //now that the summation matrices are complete, we must compute probabilties
    //implement the laplace transform, add 1 to all values, divide by one more,
    //this method avoids numerical underflow or division by zero errors
    int* at_sums=new int[num_attr];
    int** dep_sums=new int*[num_classes];
    int* num_e=new int[num_attr];
    
    int start_value=1;
    int c_sums=start_value;
    //can be changed to help with laplace transform
    //make sure that there are no garbage values anywhere;
    
    //in this case I am setting the start value to one, as division by zero will cause many more errors in the case that any
    //sum ever comes out to zero
    for(int i=0; i<num_classes; ++i)
    {
        dep_sums[i]=new int[num_attr];
    }
    for(int i=0; i<num_attr; ++i)
    {
        num_e[i]=at_list[i+1]->num_entries;
        at_sums[i]=start_value;
        for(int j=0; j<num_classes; ++j)
        {
            dep_sums[j][i]=start_value;
        
        }
    }
    
    for(int i=0; i<num_classes; ++i)
    {
        int cind[]={i};
        c_sums+=c_summation->at(cind);
    }
    int pattr_index[]={0,0};
    int dep_attr_index[]={0, 0, 0};
    //cout<<"here"<<endl;
    for(int i=0; i<num_attr; ++i)
    {
        for(int j=0; j<num_e[i]; ++j)
        {
            pattr_index[0]=dep_attr_index[1]=i;
            pattr_index[1]=dep_attr_index[2]=j;
            //cout<<"wow "<<i<<" "<<j<<endl;
            at_sums[i]+=attribute_summation->at(pattr_index);
            //cout<<"adding "<<attribute_summation->at(pattr_index)<<" to at_sums["<<i<<"]"<<endl;
            for(int h=0; h<num_classes; ++h)
            {
                //cout<<"in here"<<endl;
                dep_attr_index[0]=h;
                //cout<<"adding "<<dependent_summation->at(dep_attr_index)<<" to dep_sums["<<i<<"]["<<h<<"]"<<endl;
                dep_sums[h][i]+=dependent_summation->at(dep_attr_index);
            }
        }
    }
    for(int i=0; i<num_classes; ++i)
    {
        int cind[]={i};
        p_c->set(cind, (((double)c_summation->at(cind)/(double)c_sums)));
    }
    double laplace_value=1;
    for(int i=0; i<num_attr; ++i)
    {
        for(int j=0; j<num_e[i]; ++j)
        {
            pattr_index[0]=dep_attr_index[1]=i;
            pattr_index[1]=dep_attr_index[2]=j;
            //cout<<"---> "<<(double)attribute_summation->at(pattr_index)+laplace_value<<" / "<<at_sums[0]<<endl;
            p_attr->set(pattr_index,((double)attribute_summation->at(pattr_index)+laplace_value)/(double)at_sums[0]);
            //cout<<"result---> "<<p_attr->at(pattr_index)<<endl;
            for(int h=0; h<num_classes; ++h)
            {
                //cout<<(((double)dependent_summation->at(dep_attr_index)+laplace_value)/dep_sums[h][i])<<endl;
                dep_attr_index[0]=h;
                //cout<<"--->"<<(double)dependent_summation->at(dep_attr_index)+laplace_value<<" / "<<dep_sums[h][i]<<endl;
                dep_p_attr->set(dep_attr_index,((double)dependent_summation->at(dep_attr_index)+laplace_value)/(double)dep_sums[h][0]);
                //cout<<"result---->"<<dep_p_attr->at(dep_attr_index)<<endl;
            }
        }
    }
//    p_attr->print_native();
//    cout<<"end"<<endl;
//    dep_p_attr->print_native();
//    cout<<"end"<<endl;
    delete[] num_e;
    delete[] indexes;
    delete[] at_sums;
    for(int i=0; i<num_classes; ++i)
    {
        delete[] dep_sums[i];
    }
    delete attribute_summation;
    delete dependent_summation;
    delete c_summation;
    delete[] dep_sums;
    
    //free all the memory used in the processing stage
    return num_train;
    //build probability matrices from summation matrices
    
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
    p_c=new matrix<double>(pc_dems, 1, 1);
    p_attr=new matrix<double>(pattr_dems, 2, 1);
    dep_p_attr=new matrix<double>(dep_dems, 3, 0.1);
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




