#include "daixtrose/Daixt.h"

class X {};


int main()
{
  X x1, x2, x3;

  using namespace Daixt::DefaultOps;

  (
   (x1 + x2) 
   * 
   RationalPow<4, 3>(x3) / Pow<2>(x2) 
   ) 
  / 
  ((-x1) * x3);
}
