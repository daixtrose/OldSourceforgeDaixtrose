#include "linalg/Linalg.h"

#include <map>
#include <list>
#include <cstddef>
#include <iostream>

using std::size_t;

typedef Linalg::Matrix<double, std::map<std::size_t, double> > Matrix;
typedef Linalg::Vector<double> Vector;

typedef Daixt::Scalar<Vector::Disambiguation> VectorScalar;
typedef Daixt::Scalar<Matrix::Disambiguation> MatrixScalar;



template <class T>
void Print(const T& t)
{
  using namespace Daixt::OutputUtilities;
  std::cerr << t << std::endl;
}

void Fill(Matrix& M, double Value)
{
  size_t imax = M.nrows() + 1;
  size_t jmax = M.ncols() + 1; 
  for (size_t i = 1; i != imax; ++i)
    { 
      for (size_t j = 1; j != jmax; ++j)
        {
          M(i, j) = Value;
        }
    }
}

void FillDiagonal(Matrix& M, double Value)
{
  size_t imax = M.nrows() + 1;
  for (size_t i = 1; i != imax; ++i)
    { 
      M(i, i) = Value;
    }
}



////////////////////////////////////////////////////////////////////////////////


void TestConstructionViaShapeInfo()
{
  std::cerr << "\n --> Construction via shape info\n";
  Matrix M1(3, 3);
}


void TestCopyConstruction()
{
  std::cerr << "\n --> Copy Construction\n";
  Matrix M1(3, 3);

  Fill(M1, 1.0);
  M1(1, 2) = 5.0;
  M1(2, 1) = 6.0;

  Matrix M2 = M1;

  using namespace Daixt::DefaultOps;
  Print(M1 == M2);
}


void TestConstructionFromExpr()
{
  std::cerr << "\n --> Construction from Expression\n";

  Matrix M1(3, 3);
  Matrix M2(3, 3);

  Fill(M1, 1.0);
  FillDiagonal(M2, 3.0);
  
  using namespace Daixt::DefaultOps;
  Matrix M3 = M1 - M2; 
  Print(M3 == M1 - M2);
}


void TestAssigmentFromOtherMatrix()
{
  std::cerr << "\n --> TestAssigmentFromOtherMatrix \n";
  Matrix M1(3, 3);
  Fill(M1, 1.0);
  FillDiagonal(M1, 3.0);


  Matrix M2(3, 3); 

  using namespace Daixt::DefaultOps;
  M2 = M1;
  Print (M2 == M1);

  // self-assigment
  M2 = M2;
  Print (M2 == M2);
}


void TestAssigmentFromValue()
{
  std::cerr << "\n --> TestAssigmentFromValue\n";
  Matrix M1(3, 3);

  FillDiagonal(M1, 42.0);

  Print(M1);
  M1 = 0.0;
  Print(M1);
}


void TestAssigmentFromExpr()
{
  std::cerr << "\n --> Assignment from Expression\n";
  Matrix M1(3, 3);
  Matrix M2(3, 3);
  Matrix M3(3, 3);
  
  Fill(M1, 1.0);
  FillDiagonal(M2, 3.0);
  
  using namespace Daixt::DefaultOps;
  M3 = M1 - M2; 
  Print(M3 == M1 - M2);

  M3 = M3 - M3; 
  Print(M3 == M3 - M3);
}


void TestIndexOperator()
{
  std::cerr << "\n --> Index Operator\n";
  Matrix M1(3, 3);
  
  M1(1, 1) = 5.0;
  M1(2, 2) = 6.0;

  std::list<size_t> WrongIndices;
  WrongIndices.push_back(0);
  WrongIndices.push_back(4);

  // index i
  std::cerr << "\nchecking index checks on index i" << std::endl;
  for (std::list<size_t>::iterator iter = WrongIndices.begin();
       iter != WrongIndices.end(); ++iter)
    {
      double d;
      try {
        d = M1(*iter, 3);
      }
      catch (std::range_error& e) {
        std::cerr << "Expected exception (OK):\n"
                  << e.what() << std::endl;
        continue; 
      }
#ifndef NDEBUG // no errors are catched with NDEBUG flags set
      // if we reach here, something went wrong with catching the range errors
      // above, We expect the program to core dump, but who knows ...
      throw std::logic_error("There is an undetected range error");
#endif
    }

  // index j
  std::cerr << "\nchecking index checks on index j" << std::endl;
  for (std::list<size_t>::iterator iter = WrongIndices.begin();
       iter != WrongIndices.end(); ++iter)
    {
      double d;
      try {
        d = M1(3, *iter);
      }
      catch (std::range_error& e) {
        std::cerr << "Expected exception (OK):\n"
                  << e.what() << std::endl;
        continue;
      }

      std::cerr << '\n';

#ifndef NDEBUG // no errors with NDEBUG, so reaching here is normal
      // if we reach here, something went wrong with catching the range errors
      // above. We expect the program to core dump, but who knows ...
      throw std::logic_error("There is an undetected range error");
#endif
    }
}


void TestOperatorPlusOrMinusEqual()
{
  std::cerr << "\n --> TestOperatorPlusOrMinusEqual\n";

  using namespace Daixt::DefaultOps;
  Matrix M1(3, 3);
  
  M1(1, 1) = 1.0;
  M1(1, 2) = 2.0;
  M1(2, 2) = 3.0;


  Matrix M2 = M1;
  M1 += M1;
  M2 += M1;

  Print(M1 == M2);  

  Matrix M3 = M1;
  M1 -= M1;
  M3 -= M1;

  Print(M1 == M3);  


  // provoke an error;
  Matrix M4(4, 4);
  try {
    M1 += M4;
  }
  catch (std::range_error& e) {
    std::cerr << "Expected exception (OK):\n"
              << e.what() << std::endl;
    return;
  }
#ifndef NDEBUG 
  throw 
    std::logic_error
    ("Undetected range error in Matrix<T, RowStorage, Allocator>::operator+=");
#endif
}

void TestMatrixSwap()
{
  std::cerr << "\n --> " << __LINE__ << '\n';

  using namespace Daixt::DefaultOps;
  Matrix M1(3, 3), M2(3, 3);

  M1(1, 1) = 1.0;
  M1(1, 2) = 2.0;
  M1(2, 2) = 3.0;

  M2(2, 1) = 1.0;
  M2(2, 2) = 2.0;
  M2(3, 2) = 3.0;
  
  Print(M1); Print(M2);
  M1.swap(M2);
  Print(M1); Print(M2);
}

void TestRowAndColAccess()
{
  std::cerr << "\n --> " << __LINE__ << '\n';

  using namespace Daixt::DefaultOps;
  Matrix M1(3, 3), M2(3, 3);

  M1(1, 1) = 1.0;
  M1(1, 2) = 2.0;
  M1(2, 2) = 3.0;
  M1(2, 1) = 3.0;
  M1(1, 3) = 999;
  M1(2, 3) = 999;

  M1.SetEntriesInRowTo(2, 42.0);
  M1.SetEntriesInColTo(3, 42.5);
  
  for (size_t i = 1; i !=4; ++i)
    {
      M2.ReplaceRow(i, M1(i));
    }

  Print(M1 == M2);
}


void TestTranspose()
{
  std::cerr << "\n --> " << __LINE__ << '\n';

  using namespace Daixt::DefaultOps;
  Matrix M1(3, 3), M2(3, 3);

  M1(1, 1) = 1.0;
  M1(1, 2) = 2.0;
  M1(2, 2) = 3.0;
  M1(2, 1) = 3.0;
  M1(1, 3) = 999;
  M1(2, 3) = 999;

  std::cerr << "Original:\n";
  Print(M1);

  // transpose:
  for (size_t i = 1; i !=4; ++i)
    {
      M2.ReplaceRow(i, M1.GetColumn(i));
    }

  Matrix M3 = Transpose(M1);

  Print(M2 == M3);
}


void TestLump()
{
  std::cerr << "\n --> " << __LINE__ << '\n';

  using namespace Daixt::DefaultOps;
  Matrix M1(3, 3);

  M1(1, 1) = 1.0;
  M1(1, 2) = 2.0;
  M1(2, 2) = 3.0;
  M1(2, 1) = 3.0;
  M1(1, 3) = 999;
  M1(2, 3) = 999;

  std::cerr << "Original:\n";
  Print(M1);
  
  std::cerr << "Lumped:\n";
  Matrix M2 = Lump(M1);
  Matrix M3 = Transpose(Lump(M1));

  Print(M2 == M3);
}

void TestMatrixTimesVector()
{
  std::cerr << "\n --> TestMatrixTimesVector\n";

  using namespace Daixt::DefaultOps;

  Matrix M1(3, 3);
  
  M1(1, 1) = 1.0;
  M1(1, 2) = 2.0;
  M1(2, 2) = 3.0;

  Vector V1(3);

  V1(1) = 1.0;
  V1(2) = 2.0;
  V1(3) = 3.0;

  
  Vector V2(3);

  // copy construction
  Vector V3(M1 * V1);
  
  // copy assignment
  V2 = M1 * V1;

  Print(V2 == M1 * V1);
  Print(V3 == M1 * V1);

  V2(3) = 77;

  std::cerr << "V2 =\n";
  Print(V2);
  std::cerr << "V1 = 2 * V2 =\n";
  V1 = VectorScalar(2.0) * V2;
  Print(V1);
  std::cerr << "V1 * 2 =\n";
  V1 = V1 * VectorScalar(2.0);
  Print(V1);
}

////////////////////////////////////////////////////////////////////////////////



int main()
{
  try {
    TestConstructionViaShapeInfo();
    TestCopyConstruction();
    TestConstructionFromExpr();
    TestAssigmentFromOtherMatrix();
    TestAssigmentFromValue();
    TestAssigmentFromExpr();
    TestIndexOperator();
    TestOperatorPlusOrMinusEqual();
    TestMatrixSwap();
    TestRowAndColAccess();
    TestTranspose();
    TestLump();
    TestMatrixTimesVector();
  }
  catch (std::exception& e) {
    std::cerr << "\nUnexpected Exception (ERROR):\n" 
              << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cerr << "\nOK - OK - OK --- All Tests succeeded --- OK - OK - OK" 
            << std::endl; 
  exit(EXIT_SUCCESS);
}

