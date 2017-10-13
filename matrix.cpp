#include<iostream>

using namespace std;

/*
 this class can be used to create arrays of arbitrary dimensionality through the use of
 char pointers and a clever algorithm that manages the given indexes to virtualize the
 effects of being able to deference an array the way one thinks of it traditionally
 
 *note: might not be scalable, but if numpy can handle monstrous arrays, surely c++
 can handle something even greater
 */

template<typename t>
class matrix
{
private:
    int d; //what is the dimensionality of the matrix
    int* rcd; //the dimensions of the matrix in array form, [row, column, depth]
    t* data;
    
    void init(int* dims, int n_dims ,t def)
    {
        //the int[] argument contains the measure of the dimensions of the matrix
        //the other int argument is the number of dimensions
        int product=1;
        if(n_dims<1)
        {
            cout<<"matrix cannot be instantiated with zero value for dimension"<<endl;
            return;
        }
        for(int i=0; i<n_dims; ++i)
        {
            product*=dims[i];
        }
        //cout<<product<<endl;
        data=new t[product];
        for(int i=0; i<product; ++i)
        {
            data[i]=def; //set the default value in the matrix
        }
        d=n_dims;
        rcd=new int[n_dims];
        for(int i=0; i<n_dims; ++i)
        {
            rcd[i]=dims[i];
        }
        return;
    }
    
    void set(int internal_index, t value)
    {
        data[internal_index]=value;
    }
    int get_index(int* dims)
    {
        int index=0;
        index=(d>=2) ? dims[0]*rcd[1]+dims[1] : dims[0];
//        for(int i=0; i<d; ++i)
//        {
//            cout<<dims[i]<<" ";
//        }
//        cout<<endl;
        //cout<<"index"<<index<<endl;
        if(d>2)
        {
            int product=rcd[0]*rcd[1];
            for(int i=2; i<d; ++i)
            {
                index+=dims[i]*product;
                //product*= (i==d-1) ? rcd[i] : 1;
                product*=rcd[i];
            }
            //index+=product;
        }
        //cout<<"final index: "<<index<<endl;
        return index;
    }
    int get_num_values()
    {
        int product=1;
        for(int i=0; i<d; ++i)
        {
            product*=rcd[i];
        }
        return product;
    }
    
public:
    
//    void intmatrix::set(int rw, int cl, int val)
//    {
//        data[rw*c+cl]=val;
//    }
    t at(int* dims)
    {
        return data[get_index(dims)];
    }
    t get(int internal_index)
    {
        return data[internal_index];
    }
    
    void set(int* dims, t val)
    {
        data[get_index(dims)]=val;
    }
    
    int* get_dims()
    {
        return rcd;
    }
    
    matrix(int* d, int nd, t def)
    {
        init(d, nd, def);
    }
    
    ~matrix()
    {
        delete[] data;
        d=0;
        delete[] rcd;
    }
    
    void print(void(*f)(t))
    {
        for(int i=0; i<get_num_values(); ++i)
        {
            f(data[i]);
        }
    }
};

/*
class intmatrix
{
private:
    int r;//rows
    int c;//columns
    int* data; //the matrix
    
    void init(int, int);
    void set(int, int); //set values in the matrix without overhead
    int get(int); //private way to access matrix
    
public:
    int at(int, int); //get the integer at an index
    void set(int, int, int); //set an integer at an index
    intmatrix(int, int); //initialize a new matrix with all values set to 0;
    ~intmatrix();
    int get_r();
    int get_c();
    
    intmatrix* deep_copy();
    void print();
};
*/
