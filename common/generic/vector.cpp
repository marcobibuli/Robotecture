// vector.c
//
// Massimo Caccia 7/03/97


#include "vector.h"


Vector::Vector(const Vector &v)
{
 x[0]=v.x[0];
 x[1]=v.x[1];
 x[2]=v.x[2];
} // constructor

			

void Vector::read(void)
{
 int i;
 
 for(i=0;i<3;i++)
  {
   printf("V[%d] ",i);
   scanf("%lf",&x[i]);
  }
} // read Vector


void Vector::fread(FILE *f)
{
 int i;
 
 for(i=0;i<3;i++)
  {
   fscanf(f,"%lf",&x[i]);
  }
} // read Vector


void Vector::print(void)
{
 int i;
 
 for(i=0;i<3;i++)
  printf("%g ",x[i]);
 printf("\n");
}



void Vector::fprint(FILE *f)
{
 int i;
 
 for(i=0;i<3;i++)
  fprintf(f,"%g ",x[i]);
 fprintf(f,"\n");
}



Vector &Vector::operator=(const Vector &other)
{  
 int i;
 
 for(i=0;i<3;i++)
  x[i]=other.x[i];
 return *this;
}

 

Vector &Vector::operator+=(const Vector &other)
{
 int i;
 
 for(i=0;i<3;i++)
  x[i]+=other.x[i];
 return *this;
}
      


Vector &Vector::operator-=(const Vector &other)
{
 int i;
 
 for(i=0;i<3;i++)
  x[i]-=other.x[i];
 return *this;
}





Vector Vector::operator+(const Vector &v2)
{
 int i;
 Vector v;
 
 for(i=0;i<3;i++)
  v.x[i]=x[i]+v2.x[i];
 return(v);
}

 
 
 
Vector Vector::operator-(const Vector &other)
{
 int i;
 Vector v;
 
 for(i=0;i<3;i++)
  v.x[i]=x[i]-other.x[i];
 return(v);
}


  
  
  
Vector Vector::operator%(const Vector &other)  
// vector product
{
 Vector v;

 v.x[0]=x[1]*other.x[2]-other.x[1]*x[2]; 
 v.x[1]=-x[0]*other.x[2]+other.x[0]*x[2];
 v.x[2]=x[0]*other.x[1]-other.x[0]*x[1];

 return(v);
}  
  
  


double Vector::operator*(const Vector &other)  
// scalar product
{
 int i;
 double s=0.;

 for(i=0;i<3;i++)
  s+=x[i]*other.x[i]; 

 return(s);
} 


Vector Vector::operator*(double d)  
// Vector * double
{
 int i;
 Vector v;  
 
 for(i=0;i<3;i++)
  v.x[i]=x[i]*d;
  
 return(v);
}   
  


Vector operator*(double d,const Vector &other)  
// double * Vector
{
 int i;
 Vector v; 
 
 for(i=0;i<3;i++)
  v.x[i]=other.x[i]*d;
 
 return(v);
} 




Vector Vector::operator/(double d)  
// Vector / double
{
 int i;
 Vector v;  
 
 for(i=0;i<3;i++)
  v.x[i]=x[i]/d;
  
 return(v);
}




void Vector::zeros(void)
{  
 for(int i=0;i<3;i++) 
  x[i]=0.;
}



