// vector.h
//
// class Vector: a generic vector
//
// Massimo Caccia 7/03/97

#ifndef VECTOR

#define VECTOR

#include <math.h>
#include <stdio.h>



class Vector 
{	 /* matrice di double */
 public:
  Vector(void) { x[0]=x[1]=x[2]=0.;}	// constructor    
  Vector(const Vector &v);	// constructor
  Vector(double x0,double y0,double z0) { x[0]=x0;x[1]=y0;x[2]=z0; }
  
  void read(void);          // read vector from console
  void fread(FILE *f);			// read vector from file
  void print(void);			// print vector to console
  void fprint(FILE *f);			// print vector to file
  
  Vector &operator=(const Vector &other);	// Vector assignment
  Vector &operator+=(const Vector &other);
  Vector &operator-=(const Vector &other);   
  Vector operator+(const Vector &m2);       // sum
  Vector operator-(const Vector &other);    // subtraction
  Vector operator%(const Vector &other);    // vector product   
  double operator*(const Vector &other);    // scalar product
  Vector operator*(double d);               // Vector * double
  friend Vector operator*(double d,const Vector &other);       // double * Vector
  Vector operator/(double d);               // Vector / double 
  double norm(void) { return(sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2])); }	// norm
  double &operator()(int n) { return(x[n]); }     
  void zeros(void);    // null Vector
  
  ~Vector(void) { }			// destructor  
 
 private:
  double x[3];
};
 
 
#endif 
