#include "daixtrose/Daixt.h"
#include <iostream>
#include <string>

class X 
{
  std::string Name_;
public:
  X(const std::string& Name) : Name_(Name) {}
  std::string Name() const { return Name_; }
};

std::ostream& operator<<(std::ostream& os, const X& x)
{
  os << x.Name();
  return os;
}


template <class T> 
void Evaluate(const Daixt::Expr<T>& arg)
{
  Evaluate(arg.content());
}

template <class LHS, class RHS, class OP> 
void Evaluate(const Daixt::BinOp<LHS, RHS, OP>& arg)
{
  Evaluate(arg.lhs());
  std::cerr << OP::Symbol();
  Evaluate(arg.rhs());
}

template <class ARG, class OP> 
void Evaluate(const Daixt::UnOp<ARG, OP>& arg)
{
  std::cerr << OP::Symbol();
  Evaluate(arg.arg());
}

void Evaluate(const X& x)
{
  std::cerr << x;
} 

int main()
{
  X x1("x1"), x2("x2"), x3("x3");

  using namespace Daixt::DefaultOps;

  Evaluate((x1 + x2) / ((-x1) * x3));
}
