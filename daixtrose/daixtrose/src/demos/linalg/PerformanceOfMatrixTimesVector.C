#include "linalg/Linalg.h"

#include "boost/timer.hpp"

#include <map>
#include <list>
#include <cstddef>
#include <iostream>

typedef Linalg::Matrix<double, std::map<std::size_t, double> > Matrix;
typedef Linalg::Vector<double> Vector;

const int M=1000;
const int N=1000;

void daixtrose_mult_vec()
{
  Matrix A(M, N);
  Vector B(N), C(M);
  
  for(int i=0;i<M;i++)
    {
      for(int j=0;j<N;j++)
        {
          A(i,j) = i*N+j;
        }
      for(i=0;i<N;i++)
              {
                B(i) = i;
              }
    }

  //I'm desired in the running time of "mult"
  boost::timer t;
  using namespace Daixt::DefaultOps;
  C = A * B;
  std::cerr << "C = A * B took " << t.elapsed() << "s\n";
}


double  A[M][N], B[N], C[M];

void c_mult_vec()
{
  for(int i=0;i<M;i++)
    {
      for(int j=0;j<N;j++)
        {
          A[i][j] = i*N+j;
        }
      for(i=0;i<N;i++)
        {
          B[i] = i;
        }
    }

  //I'm desired in the running time of the code following.
  boost::timer t;
  for(int i=0;i<M;i++)
    for(int k=0;k<N;k++)
      C[i]+=A[i][k]*B[k]; 
  std::cerr << "C version took " << t.elapsed() << "s\n";
}

int main()
{
  try {
    daixtrose_mult_vec();
    c_mult_vec();
  }
  catch (std::exception& e) {
    std::cerr << "\nException:\n" << e.what() << std::endl;
    return EXIT_FAILURE; 
  }
}
