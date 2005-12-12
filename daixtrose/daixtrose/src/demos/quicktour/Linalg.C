#include "linalg/Linalg.h"

#include <map>
#include <cstddef>
#include <iostream>


int main()
{
  // Shortcuts. Mentioning the allocators is not needed, here the defaults are shown
  typedef Linalg::Matrix<double, 
                         std::map<std::size_t, double>,
                         std::allocator<std::map<std::size_t, double> > > Matrix;
  
  typedef Linalg::Vector<double, 
                         std::allocator<double> > Vector;
  // use operator+,-,*,/ etc
  using namespace Daixt::DefaultOps;
  // allows multiplication of non-disambigauted scalars
  using namespace Daixt::Convenience;
  // output routines 
  using namespace Daixt::OutputUtilities;

  
  std::size_t n = 3; // Only for demontration purposes: this number is a little
                     // bit small, linalg was designed for larger scales

  Matrix M1(n, n);

  // all possible ways to fill in some matrix data:
  for (size_t i = 1; i != n+1; ++i) M1(i, i) = 42.0;

  M1(1, 2) = 21.0;
  M1.SetEntriesInRowTo(2, 3.14);
  M1.SetEntriesInColTo(3, 42.5);

  
  Matrix M2 = M1;

  Matrix M3(n, n); 
  M3 = M1;

  M1.ReplaceRow(2, M1(3));

  M2 += M3;
  M2.swap(M3);

  Matrix M4 = M2 + Lump(Transpose(M1));
  M2 = 0.0;

  Matrix M5(n, n);
  M5 = M2 - M3 + M1;
  
  Vector V1(n);
  std::fill(V1.begin(), V1.end(), 1.0);

  Vector V2 = (2.0 * M2 - Lump(M1)) * V1;

  std::cerr << M1 << M2 << M3 << M4 << M5 << V1 << V2 << std::endl;
}
