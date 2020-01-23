// matrix.c
//
// Massimo Caccia 12/02/96


#include "vector.h"
#include "matrix.h"

Matrix::Matrix(int nr,int nc)
{
  int i,j;
  
  n_r=nr;
  n_c=nc;
  
  /* alloca lo spazio per un vettore di puntatori */
  //ma=(double *) malloc(n_r*n_c*sizeof(double));
  ma= new double*[nr];

  if (ma == NULL)
    printf("matrix allocation error1\n");
    
  for (i=0;i<n_r;i++)
  {
    *(ma+i)= new double[nc];
    if (*(ma+i) == NULL)
    printf("matrix allocation error1\n");
  }
    /* inizializza a zero tutti gli elementi allocati */
  for (i=0;i<n_r;i++)
    for (j=0;j<n_c;j++)
      //(*((ma)+i)+j) = 0;
      *(*(ma+i)+j) = 0;

} // constructor



Matrix::Matrix(const Matrix &m)
{
  int i,j;
  
  n_r=m.n_r;
  n_c=m.n_c;
  
  /* alloca lo spazio per un vettore di puntatori */
  //ma=(double *) malloc(n_r*n_c*sizeof(double));
  ma= new double*[m.n_r];

  if (ma == NULL)
    printf("matrix allocation error1\n");
    
  for (i=0;i<n_r;i++)
  {
    *(ma+i)= new double[m.n_c];
    if (*(ma+i) == NULL)
    printf("matrix allocation error1\n");
  }
    /* inizializza gli elementi allocati */
  for (i=0;i<n_r;i++)
    for (j=0;j<n_c;j++)
      //(*((ma)+i)+j) = 0;
      *(*(ma+i)+j) = *(*(m.ma+i)+j);

} // constructor

			


Matrix::~Matrix(void)
{
 
 //free(ma);
 for (int i=0;i<n_r;i++)
   delete [] (*(ma+i));  
 delete [] ma;
} // destructor   



void Matrix::read(void)
{
 int i,j;
 
 for(i=0;i<n_r;i++)
  {
   printf("M[%d][1..%d] ",i+1,n_c);
   for(j=0;j<n_c;j++) scanf("%lf",&(*(*(ma+i)+j)));
  }
} // read matrix


void Matrix::fread(FILE *f)
{
 int i,j;
 
 for(i=0;i<n_r;i++)
  {
   for(j=0;j<n_c;j++) fscanf(f,"%lf",&(*(*(ma+i)+j)));
  }
} // read matrix from file


void Matrix::print(void)
{
 int i,j;
 
 for(i=0;i<n_r;i++)
  {
   for(j=0;j<n_c;j++) printf("%g ",*(*(ma+i)+j));
   printf("\n");
  }
}



void Matrix::fprint(FILE *f)
{
 int i,j;
 
 for(i=0;i<n_r;i++)
  {
   for(j=0;j<n_c;j++) fprintf(f,"%gf ",*(*(ma+i)+j));
   //fprintf(f,"\n");
  }
 fprintf(f,"\n");
}



Matrix &Matrix::operator=(const Matrix &other)
{  
 int i,j;
 
 if((n_r!=other.n_r)||(n_c!=other.n_c)) 
  printf("Matrix=:error matrix size\n");
 for(i=0;i<n_r;i++)
  for(j=0;j<n_c;j++)
   *(*(ma+i)+j)=*(*(other.ma+i)+j);
 return *this;
 //return(m); 
}

 

Matrix &Matrix::operator+=(const Matrix &other)
{
 int i,j;
 
 if((n_r!=other.n_r)||(n_c!=other.n_c)) printf("Matrix+=:error matrix size\n");
 for(i=0;i<n_r;i++)
  for(j=0;j<n_c;j++)
   //*(ma+(i*n_c+j))+=*(other.ma+(i*n_c+j)); 
   *(*(ma+i)+j) += *(*(other.ma+i)+j);
 return(*this);
}      


Matrix &Matrix::operator-=(const Matrix &other)
{
 int i,j;
 
 if((n_r!=other.n_r)||(n_c!=other.n_c)) printf("Matrix-=:error matrix size\n");
 for(i=0;i<n_r;i++)
  for(j=0;j<n_c;j++)
   //*(ma+(i*n_c+j))-=*(other.ma+(i*n_c+j));      
   *(*(ma+i)+j) -= *(*(other.ma+i)+j);
 return(*this);
}





Matrix Matrix::operator+(const Matrix &m2)
{
 int i,j;
 
 if((n_r!=m2.n_r)||(n_c!=m2.n_c)) printf("Matrix+:error matrix size\n");
 
 Matrix m(n_r,n_c);
 for(i=0;i<n_r;i++)
  for(j=0;j<n_c;j++)
   //*(m.ma+(i*n_c+j))=*(ma+(i*n_c+j))+*(other.ma+(i*n_c+j)); 
   //m(i,j)=(*this)(i,j)+other(i,j); 
   //*(*(m.ma+i)+j) = *(*(ma+i)+j) + *(*(other.ma+i)+j);  
   *(*(m.ma+i)+j) = *(*(ma+i)+j) + *(*(m2.ma+i)+j);
 //return(Matrix(m)); 
 return(m);
}

 
 
 
Matrix Matrix::operator-(const Matrix &other)
{
 int i,j;
 
 if((n_r!=other.n_r)||(n_c!=other.n_c)) printf("Matrix-:error matrix size\n");
 
 Matrix m(n_r,n_c);
 for(i=0;i<n_r;i++)
  for(j=0;j<n_c;j++)
   //*(m.ma+(i*n_c+j))=*(ma+(i*n_c+j))-*(other.ma+(i*n_c+j));
   *(*(m.ma+i)+j) = *(*(ma+i)+j) - *(*(other.ma+i)+j); 
 //return(Matrix(m)); 
 return(m);
} 


  
  
  
Matrix Matrix::operator*(const Matrix &other)
{
 int i,j,k;
 double sum;
 
 if(n_c!=other.n_r) 
  printf("Matrix*:error matrix size %d %d\n",n_c,other.n_r);
 
 Matrix m(n_r,other.n_c);
 for(i=0;i<m.n_r;i++)
  for(j=0;j<m.n_c;j++)
   {
    sum=0.;
    for(k=0;k<n_c;k++)
     //sum+=*(ma+(i*n_c+k))*(*(other.ma+(k*other.n_c+j)));  
     sum += *(*(ma+i)+k) * *(*(other.ma+k)+j);
    *(*(m.ma+i)+j)=sum;
   } 
 //return(Matrix(m));
 return(m);
}   



Vector Matrix::operator*(Vector v)
// Vector = Matrix * Vector  
{
 int i,j;
 double sum;
 
 if((n_c!=3)||(n_r!=3)) 
  printf("Matrix*:error matrix size\n");
 
 Vector vprod;
 for(i=0;i<3;i++)
  {
   sum=0.;
   for(j=0;j<3;j++)
    sum += *(*(ma+i)+j) * v(j);
   vprod(i)=sum;
  } 
 //return(Matrix(m));
 return(vprod);
}   



Matrix Matrix::operator*(double d)
{
 int i,j;
 
 Matrix m(n_r,n_c);
 for(i=0;i<n_r;i++)
  for(j=0;j<n_c;j++)
   // *(m.ma+(i*n_c+j))=*(ma+(i*n_c+j))*d;       
   *(*(m.ma+i)+j) = *(*(ma+i)+j)*d;
 //return(Matrix(m));
 return(m);
}   
  


Matrix operator*(double d,const Matrix &other)
{
 int i,j;
 
 Matrix m(other.n_r,other.n_c);
 for(i=0;i<other.n_r;i++)
  for(j=0;j<other.n_c;j++)
   // *(m.ma+(i*n_c+j))=*(ma+(i*n_c+j))*d;       
   *(*(m.ma+i)+j) = *(*(other.ma+i)+j)*d;
 //return(Matrix(m)); 
 return(m);
} 




Matrix Matrix::operator/(double d)
{
 int i,j;
 
 Matrix m(n_r,n_c);
 for(i=0;i<n_r;i++)
  for(j=0;j<n_c;j++)
   //*(m.ma+(i*n_c+j))=*(ma+(i*n_c+j))/d;
   *(*(m.ma+i)+j) = *(*(ma+i)+j)/d;
 return(Matrix(m));
}





Matrix Matrix::t(void)
{
 int i,j;
 
 Matrix m(n_c,n_r);
 for (i=0;i<n_c;i++)
  for (j=0;j<n_r;j++)
	//*(m.ma+(i*n_r+j)) = *(ma+(j*n_c+i));
	*(*(m.ma+i)+j) = *(*(ma+j)+i);
 //return(Matrix(m));
 return(m);
} /* trasp */



Matrix Matrix::submatrix(int i,int j)
{
 if(n_r!=n_c) printf("Matrix submatrix: error row!=col\n");

 if(n_r>1)
  { 
   int rm=n_r-1;
   int cm=n_c-1;
   Matrix m(rm,cm);
   
   int im,i0,jm,j0;
   for(im=0;im<n_r;im++)
    for(jm=0;jm<n_c;jm++) 
     {
      if(im>i) i0=im-1;
       else i0=im;
      if(jm>j) j0=jm-1;
       else j0=jm;
      if((im!=i)&&(jm!=j))
      m(i0,j0)=*(*(ma+im)+jm);
     }
   return(Matrix(m));
  }
  else { printf("Matrix submatrix: error n_r==0\n"); return(Matrix(1,1)); }
 }

      

double Matrix::det(void)
{
 if(n_r!=n_c) printf("Matrix det: error row!=col\n");
 if(n_r>1)   
  {
   double d=0.;
   Matrix m(n_r-1,n_c-1);
   for(int j=0;j<n_c;j++)
    {
     if ((*this)(0,j)!=0.)
      {
       m=this->submatrix(0,j);
       d+=(*this)(0,j)*pow(-1.,(double)j)*m.det();
      }
    }
   return(d);
  }
  else // n_r==1
  return(*(*(ma+0)+0));
}




Matrix Matrix::inv(void)
{
 if(n_r!=n_c) printf("Matrix inv: error row!=col\n");
 
 double d=this->det();
 
 //printf("det: %lf\n\n",d);

 Matrix m(n_r,n_c);
 if(d==0.) return(Matrix(m));
 if(n_r==1) {m(0,0)=1/d; return(Matrix(m)); }
 Matrix mij(n_r-1,n_c-1); 
 
 for(int i=0;i<n_r;i++)
  for(int j=0;j<n_c;j++)
   {
    mij=this->submatrix(j,i);
    m(i,j)=pow(-1.,(double)(j+i))*mij.det()/d;

    //printf("i: %d  j: %d   mij.det: %lf   mij: %lf\n",i,j,mij.det(),m(i,j));
   }
  
 return(Matrix(m));
} /* inv */



double Matrix::trace(void)
{
 double t=0.;
 
 if(n_r!=n_c) printf("Matrix trace: error row!=col\n");
 
 for(int i=0;i<n_r;i++) t+=*(*(ma+i)+i);
 return(t);
}




void Matrix::eig(double *lamda1,double *lamda2,Matrix *v1,Matrix *v2)
// computes eigenvalues and eigenvectors
//
// now only for 2x2 matrices
//
{
 if((n_r!=2)||(n_c!=2)) printf("Matrix eig: size error\n");
 
 if((v1->n_r!=2)||(v1->n_c!=1)) printf("Matrix eig: eigenvector size error\n");
 if((v2->n_r!=2)||(v2->n_c!=1)) printf("Matrix eig: eigenvector size error\n");
 
 double b,d,delta,sqrtdelta,v;

 if(fabs((*this)(1,0))>EPSILON)
  {
   b=(*this)(0,0)+(*this)(1,1);
   d=(*this)(0,0)-(*this)(1,1);
   delta=d*d+4.*(*this)(0,1)*(*this)(0,1);
   sqrtdelta=sqrt(delta);
 
   *lamda1=(b-sqrtdelta)/2.;
   *lamda2=(b+sqrtdelta)/2.;
   
   v=(*lamda1-(*this)(0,0))/(*this)(0,1);
   (*v1)(0,0)=cos(atan2(v,1.));
   (*v1)(1,0)=sin(atan2(v,1.)); 
   (*v2)(0,0)=-sin(atan2(v,1.));
   (*v2)(1,0)=cos(atan2(v,1.));
  }
  else
   {
    if((*this)(0,0)<=(*this)(1,1)) 
     {
      *lamda1=(*this)(0,0);
	  (*v1)(0,0)=1.;
      (*v1)(1,0)=0.;
	  *lamda2=(*this)(1,1);
	  (*v2)(0,0)=0.;
      (*v2)(1,0)=1.;
     }
     else
     {
      *lamda2=(*this)(0,0);
	  (*v2)(0,0)=1.;
      (*v2)(1,0)=0.;
	  *lamda1=(*this)(1,1);
	  (*v1)(0,0)=0.;
      (*v1)(1,0)=1.;
     }
   }
}

//Added 08/08/2019
void Matrix::skew(Matrix a)
{
	Matrix m(n_r, n_c);
	m.zeros();
	m(0, 1) = -a(2,0);
	m(0, 2) = a(1, 0);
	m(1, 0) = a(2, 0);
	m(1, 2) = -a(0, 0);
	m(2, 0) = -a(1, 0);
	m(2, 1) = a(0, 0);

	for (int i = 0; i < n_r; i++)
		for (int j = 0; j < n_c; j++)
			* (*(ma + i) + j) = m(i,j);
	return;
}

 
void Matrix::eye(void)
{  
 if(n_r!=n_c) printf("Matrix eye: error row!=col\n");
 
 for(int i=0;i<n_r;i++) 
  for(int j=0;j<n_c;j++)
   if(i==j) *(*(ma+i)+j)=1.;
    else *(*(ma+i)+j)=0.;
}



void Matrix::zeros(void)
{  
 for(int i=0;i<n_r;i++) 
  for(int j=0;j<n_c;j++)
   *(*(ma+i)+j)=0.;
}



void Matrix::ones(void)
{  
 for(int i=0;i<n_r;i++) 
  for(int j=0;j<n_c;j++)
   *(*(ma+i)+j)=1.;
}

  

void Matrix::diag(Matrix v)
{
 if(n_r!=n_c) printf("Matrix diag: error row!=col\n");
 if(n_r!=v.n_r) printf("Matrix diag: error row!=row\n");

 for(int i=0;i<n_r;i++) 
  for(int j=0;j<n_c;j++)
   if(i==j) *(*(ma+i)+j)=v(i,0);
    else *(*(ma+i)+j)=0.;
}




Matrix &Matrix::include(const Matrix &other) 
{
// e' possibile assegnare una matrice piu' piccola ad una piu' grande (mette a zero gli elementi mancanti)
int i,j;

if((n_r<other.n_r)||(n_c<other.n_c)) printf("Matrixinclude:error matrix size\n");
 for(i=0;i<other.n_r;i++)
  for(j=0;j<other.n_c;j++)
   *(*(ma+i)+j)=*(*(other.ma+i)+j);  
 for(i=other.n_r;i<n_r;i++)
  for(j=other.n_c;j<n_c;j++)
   *(*(ma+i)+j)=0.;
 return *this; 
}


#define ITS	30

bool Matrix::svd(Matrix &U,Matrix &W,Matrix &V)
{
//
// A=*this: m x n
// U: m x m
// W: m x n
// V: n x n
// A=U*W*V'
//
int m=n_r;
int n=n_c;
if ((U.n_r!=m)||(U.n_c!=n))
{
	printf("error Matrix::svd U size %d x %d != %d x %d\n",U.n_r,U.n_c,m,n);
	return false;
}
if ((W.n_r!=n)||(W.n_c!=n))
{
	printf("error Matrix::svd W size %d x %d != %d x %d\n",W.n_r,W.n_c,n,n);
	return false;
}
if ((V.n_r!=n)||(V.n_c!=n))
{
	printf("error Matrix::svd V size %d x %d != %d x %d\n",V.n_r,V.n_c,n,n);
	return false;
}

U=*this;
W.zeros();

int flag,i,its,k,j,jj,l,nm;
double anorm,c,f,g,h,s,scale,x,y,z;

double rv1[n];

// Householder reduction to bidiagonal form
g=scale=anorm=0.0;
for (i = 0; i < n; i++)
{
	l=i+1;
	rv1[i]=scale*g;
	g=s=scale=0.0;
	if (i<m)
	{
		for (k = i; k < m; k++) scale += fabs(U(k,i));
		if (scale)
		{
			for (k = i; k < m; k++)
			{
				U(k,i) /= scale;
				s += U(k,i)*U(k,i);
			}
			f=U(i,i);
			g = -SIGN(sqrt(s),f);
			h=f*g-s;
			U(i,i)=f-g;
			for (j = l; j < n; j++)
			{
				for(s=0.0,k=i;k<m;k++) s+=U(k,i)*U(k,j);
				f=s/h;
				for(k=i;k<m;k++) U(k,j)+=f*U(k,i);
			}
			for(k=i;k<m;k++) U(k,i)*=scale;
		}
	}
	W(i,i)=scale*g;
	g=s=scale=0.0;
	if ((i<m)&&(i!=(n-1)))
	{
		for(k=l;k<n;k++) scale+=fabs(U(i,k));
		if (scale)
		{
			for (k = l; k < n; k++)
			{
				U(i,k)/=scale;
				s+=U(i,k)*U(i,k);
			}
			f=U(i,l);
			g=-SIGN(sqrt(s),f);
			h=f*g-s;
			U(i,l)=f-g;
			for(k=l;k<n;k++) rv1[k]=U(i,k)/h;
			for (j = l; j < m; j++)
			{
				for(s=0.0,k=l;k<n;k++) s+=U(j,k)*U(i,k);
				for(k=l;k<n;k++) U(j,k)+=s*rv1[k];
			}
			for(k=l;k<n;k++) U(i,k)*=scale;
		}
	}
	anorm=DMAX(anorm,(fabs(W(i,i))+fabs(rv1[i])));
}

// Accumulation of right-hand transformations
for (i = n-1; i >= 0; i--)
{
	if (i<(n-1))
	{
		if (g)
		{
			for (j = l; j < n; j++) // Double division to avoid possible underflow
				V(j,i)=(U(i,j)/U(i,l))/g;
			for (j = l; j < n; j++)
			{
				for(s=0.0,k=l;k<n;k++) s+=U(i,k)*V(k,j);
				for(k=l;k<n;k++) V(k,j)+=s*V(k,i);
			}
		}
		for(j=l;j<n;j++) V(i,j)=V(j,i)=0.0;
	}
	V(i,i)=1.0;
	g=rv1[i];
	l=i;
}

// Accumulation of left-hand transformations
for(i=IMIN(m-1,n-1);i>=0;i--)
{
	l=i+1;
	g=W(i,i);
	for(j=l;j<n;j++) U(i,j)=0.0;
	if (g)
	{
		g=1.0/g;
		for (j = l; j < n; j++)
		{
			for(s=0.0,k=l;k<m;k++) s+=U(k,i)*U(k,j);
			f=(s/U(i,i))*g;
			for(k=i;k<m;k++) U(k,j)+=f*U(k,i);
		}
		for(j=i;j<m;j++) U(j,i)*=g;
	}
	else
		for(j=i;j<m;j++) U(j,i)=0.0;
    ++U(i,i);
}

 

// Diagonalization of the bidiagonal form:
// loop over singular values, and over allowed iterations
for (k = n-1; k >= 0; k--)
{
	for (its = 1; its <= ITS; its++)
	{
		flag=1;
		for (l = k; l >= 0; l--)
		{
			nm=l-1;
			if((double)(fabs(rv1[l])+anorm)==anorm)
			{
				flag=0;
				break;
			}
			if((double)(fabs(W(nm,nm))+anorm)==anorm) break;
		}
		if (flag)
		{
			c=0.0;
			s=1.0;
			for (i = l; i <= k; i++)
			{
				f=s*rv1[i];
				rv1[i]=c*rv1[i];
				if((double)(fabs(f)+anorm)==anorm) break;
				g=W(i,i);
				h=pythag(f,g);
				W(i,i)=h;
				h=1.0/h;
				c=g*h;
				s=-f*h;
				for (j = 0; j < m; j++)
				{
					y=U(j,nm);
					z=U(j,i);
					U(j,nm)=y*c+z*s;
					U(j,i)=z*c-y*s;
				}
			}
		}
		z=W(k,k);
		if (l==k)
		{
			if (z<0.0)
			{
				W(k,k)=-z;
				for(j=0;j<n;j++) V(j,k)=-V(j,k);
			}
			break;
		}
		if (its==ITS)
		{
			printf("error Matrix::svd no convergence in 30 iterations\n");
			return false;
		}
		x=W(l,l);
		nm=k-1;
		y=W(nm,nm);
		g=rv1[nm];
		h=rv1[k];
		f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
		g=pythag(f,1.0);
		f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
		c=s=1.0;
		for (j = l; j <= nm; j++) // ????????  j<nm ????????????
		{
			i=j+1;
			g=rv1[i];
			y=W(i,i);
			h=s*g;
			g=c*g;
			z=pythag(f,h);
			rv1[j]=z;
			c=f/z;
			s=h/z;
			f=x*c+g*s;
			g=g*c-x*s;
			h=y*s;
			y*=c;
			for (jj = 0; jj < n; jj++)
			{
				x=V(jj,j);
				z=V(jj,i);
				V(jj,j)=x*c+z*s;
				V(jj,i)=z*c-x*s;
			}
			z=pythag(f,h);
			W(j,j)=z;
			if (z)
			{
				z=1.0/z;
				c=f*z;
				s=h*z;
			}
			f=c*g+s*y;
			x=c*y-s*g;
			for (jj = 0; jj < m; jj++)
			{
				y=U(jj,j);
				z=U(jj,i);
				U(jj,j)=y*c+z*s;
				U(jj,i)=z*c-y*s;
			}
		}
		rv1[l]=0.0;
		rv1[k]=f;
		W(k,k)=x;
	}
}

return true;
}

double Matrix::max_abs()
{
	double maxValue=0.0;
	for (int i = 0; i < n_r; i++)
	{
		for (int j = 0; j < n_c; j++)
		{
			if (fabs(*(*(ma+i)+j)) > maxValue)
				maxValue=fabs(*(*(ma+i)+j));
		}
	}
	return maxValue;
}
	

void Matrix::set(int row, int col, Matrix m)
{
	for (int i = 0; i < m.rows(); i++)
		for (int j = 0; j < m.columns(); j++)
			ma[row + i][col + j] = m(i, j);
}