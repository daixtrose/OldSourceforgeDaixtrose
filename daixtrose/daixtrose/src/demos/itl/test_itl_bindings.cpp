#include "linalg/Linalg.h"
#include "tiny/TinyMatAndVec.h"
#include "linalg/RowSum.h"
#include "linalg/ITL_Bindings.h"

#include <itl/interface/detail/mtl_amend.h>

#include "mtl/matrix.h"
#include "mtl/mtl.h"
#include "mtl/utils.h"

#include "itl/interface/mtl.h"
#include "itl/krylov/gmres.h"
#include "itl/preconditioner/ilu.h"
#include "examples/interface/classical_gram_schmidt.h"

#include <vector>
#include <iostream>

typedef double Type;

typedef mtl::matrix<Type, 
                    mtl::rectangle<>,
                    mtl::array< mtl::compressed<> >,
                    mtl::row_major >::type MTL_Matrix;

typedef mtl::dense1D<Type> MTL_Vector;


int main()
{
  int max_iter = 50;

  typedef TinyMat::TinyQuadraticMatrix<double, 3> BlockMatrix;
  typedef Linalg::Matrix<BlockMatrix> Matrix;

  typedef TinyVec::TinyVector<double, 3> BlockVector;
  typedef Linalg::Vector<BlockVector> Vector;

  using namespace Daixt::DefaultOps;
  using namespace Daixt::Convenience;
  
  BlockMatrix BM = 0.0;
  BM(1, 1) = 1.0;
  BM(1, 2) = 2.0;
  BM(1, 3) = 3.0;
  BM(2, 1) = 1.0;
  BM(2, 2) = 0.0;
  BM(2, 3) = 4.0;
  BM(3, 1) = 6.0;
  BM(3, 2) = 7.0;
  BM(3, 3) = 8.0;
  
  
  Matrix M(3, 3);

  M(1, 1) = BM;
  M(1, 2) = 0.5 * BM;
  M(2, 2) = 2.0 * BM;
  M(3, 3) = BM;

  M(1, 3)(1, 3) = 1.0;

  std::cout << M << std::endl;

  Vector V(3);

  BlockVector BV = 0.0;
  BV(1) = 1.0; 
  BV(2) = 2.0;
  BV(3) = 3.0;

  V(1) = BV;
  V(2) = 2.0 * BV;
  V(3) = 1.0;
  
  std::cout << V << std::endl;

  //////////////////////////////////////////////////////////////////////////////

  MTL_Matrix A;
  
  using daixt_to_mpl::operator<=;

  A <= M;

  mtl::print_all_matrix(A);

  MTL_Vector x(itl::nrows(A), Type(0));
  MTL_Vector b;

  b <= V;


  //SSOR<MTL_Matrix> precond(A);
  //identity_preconditioner precond;
  itl::ILU<MTL_Matrix> precond(A);
  
  MTL_Vector b2(b.size(), 0);
  itl::solve(precond(), b, b2);
  
  //iteration
  itl::noisy_iteration<double> iter(b2, max_iter, 1e-12);
  int restart = 20;
  itl::classical_gram_schmidt<MTL_Vector> orth(restart, itl::size(x));

  
  // testen!
  //itl::modified_gram_schmidt<Vec> orth(restart, size(x));

  itl::gmres(A, x, b, precond(), restart,  iter, orth);

  MTL_Vector b1(itl::nrows(A));
  itl::mult(A, x, b1);
  itl::add(b1, scaled(b, -1.), b1);

  if ( itl::two_norm(b1) < 1.e-6) { //output
    for (int i=0; i < A.nrows(); i++) {
      for (int j=0; j < A.ncols(); j++) {
	std::cout.width(6);
	std::cout << A(i,j) << " ";
      }
      std::cout << "  x  ";
      std::cout.width(16);
      std::cout << x[i] << "  =  ";
      std::cout.width(6);
      std::cout << b[i] << std::endl;
    }
  } else {
    std::cout << "Residual " << iter.resid() << std::endl;
  }

  return 0;
}
