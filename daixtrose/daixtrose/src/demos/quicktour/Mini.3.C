#include "daixtrose/Daixt.h"
#include <iostream>
#include <string>

class X 
{
  std::string Name_;
public:
  X(const std::string& Name) : Name_(Name) {}
  std::string Name() const { return Name_; }
  void ResetName(const std::string& NewName)
  {
    Name_ = NewName;
  }
};

std::ostream& operator<<(std::ostream& os, const X& x)
{
  os << x.Name();
  return os;
}


template <class T> 
std::string Evaluate(const Daixt::Expr<T>& arg)
{
  return Evaluate(arg.content());
}

template <class LHS, class RHS, class OP> 
std::string Evaluate(const Daixt::BinOp<LHS, RHS, OP>& arg)
{
  return 
    Evaluate(arg.lhs()) 
    + " " + OP::Symbol() + " " 
    + Evaluate(arg.rhs());
}

template <class ARG, class OP> 
std::string Evaluate(const Daixt::UnOp<ARG, OP>& arg)
{
  return OP::Symbol() + Evaluate(arg.arg());
}

std::string Evaluate(const X& x)
{
  return x.Name();
} 

// operator= must be a member function, 
// but maybe we cannot or do not want to modify X,
// so we use operator%= here
template<class T>
X& operator%=(X& x, const Daixt::Expr<T>& E)
{
  x.ResetName(Evaluate(E));
  return x;
}

int main()
{
  X x1("x1"), x2("x2"), x3("x3");

  using namespace Daixt::DefaultOps;

  X x4("x4");

  std::cerr << x4 << " = ";
  x4 %= ((x1 + x2) / ((-x1) * x3));
  std::cerr << x4;
}
