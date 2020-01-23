// matrix.h
//
// class Matrix: a generic matrix mxn of double
//
// Massimo Caccia 12/02/96

#ifndef MATRIX

#define MATRIX

#include <math.h>
#include <stdio.h>
#include "vector.h"
#include "RobotMath.h"

#define EPSILON 1.0e-8



class Matrix 
{	 /* matrice di double */
 public:
  Matrix(int nr,int nc);	// constructor    
  Matrix(const Matrix &m);	// constructor
  
  void read(void);          // read matrix from console
  void fread(FILE *f);      // read matrix from file
  void print(void);			// print matrix to console
  void fprint(FILE *f);			// print matrix to file
  
  Matrix &operator=(const Matrix &other);	// matrix assignment
  Matrix &operator+=(const Matrix &other);
  Matrix &operator-=(const Matrix &other);   
  Matrix operator+(const Matrix &m2);       // sum
  Matrix operator-(const Matrix &other);    // subtraction
  Matrix operator*(const Matrix &other);    // product
  Matrix operator*(double d);
  friend Matrix operator*(double d,const Matrix &other);       // Matrix * double 
  Vector operator*(Vector v); // Matrix * Vector
  Matrix operator/(double d);               // double * Matrix
  double &operator()(int row,int col) { return(*(*(ma+row)+col)); }     // Matrix / double
  Matrix t(void);      // transpose
  Matrix inv(void);    // inverse
  double det(void);    // determinant
  double trace(void);  // trace  
  void eig(double *lamda1,double *lamda2,Matrix *v1,Matrix *v2); // only 2x2 matrices
  void eye(void);      // identity diagonal matrix
  void zeros(void);    // null matrix
  void ones(void);     // all ones matrix
  void diag(Matrix v); // diagonal matrix : v is nx1
  Matrix &include(const Matrix &other); 
  
  int rows(void) { return(n_r); }
  int columns(void) { return(n_c); }
  
  void set(int row,int col,double value) { *(*(ma+row)+col)=value; }
  void set(int row, int col, Matrix m);
  double get(int row,int col) { double value=*(*(ma+row)+col); return(value); }

  bool svd(Matrix &U,Matrix &W,Matrix &V);
  double max_abs();
  void skew(Matrix);
  
  ~Matrix(void);			// destructor  
 
 public:
  Matrix submatrix(int i,int j);
   
 private:
  double **ma;
  int n_r;
  int n_c;
};
 
 
#endif 
