#include "linalg/Linalg.h"
#include "tiny/TinyMatAndVec.h"

#include <map>

int main()
{
  typedef TinyMat::TinyQuadraticMatrix<double, 3> BlockMatrix;
  typedef 
    Linalg::Matrix<BlockMatrix, std::map<std::size_t, BlockMatrix> > Matrix;

  typedef TinyVec::TinyVector<double, 3> BlockVector;
  typedef Linalg::Vector<BlockVector> Vector;

  using namespace Daixt::DefaultOps;

  BlockMatrix B = 0.0;
  B(1, 1) = 1.0;
  B(2, 2) = 2.0;
  B(3, 3) = 3.0;

  Matrix M(4, 4);
  M(1, 1) = B;
  M(2, 1) = B;
  M(2, 2) = B;
  M(3, 3) = B;
//   M(3, 3) = 5.0;
//   M(3, 4) = B;
  M(4, 4) = B;

  Matrix M2 = Linalg::Inverse(Linalg::Lump(M));

  std::cerr << M << "\n\n\n" << M2 << '\n';
}
