#include "daixtrose/Daixt.h"

#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// ************************************************************************** //
////////////////////////////////////////////////////////////////////////////////

// A named and compile-time-numbered variable 

// The name handling is separated because the profiler gives us 5% more speed
template <std::size_t Number> struct NameOfVariable;

struct DisambiguatedVariable {};

template <std::size_t Number> 
class Variable
{
public:
  inline Variable() {} // intel's C++-70 needs this one
  typedef DisambiguatedVariable Disambiguation;
  std::string Name() const;// { return "undefined"; }
};

////////////////////////////////////////////////////////////////////////////////
// output of Variable
template <std::size_t Number>
inline std::ostream& operator<<(std::ostream& os, const Variable<Number>& v)
{
  return os << v.Name();
}

// convenience 
#define DECLARE_VARIABLE(NAME, NUMBER, LATEX_NAME) \
template <>                                                   \
std::string Variable<NUMBER>::Name() const                    \
{                                                             \
  return LATEX_NAME;                                          \
}                                                             \
                                                              \
Variable<NUMBER> NAME

#define EXTERN_DECLARE_VARIABLE(NAME, NUMBER) \
extern Variable<NUMBER> NAME


////////////////////////////////////////////////////////////////////////////////
// ************************************************************************** //
////////////////////////////////////////////////////////////////////////////////

template <class T>
void TestSimplify(const T& t) 
{
  using namespace Daixt::OutputUtilities;
  using namespace Daixt::ExprManip;

  std::cerr << "Simplify(" << t << ") = " << Simplify(t) << std::endl; 
}

template <class T, class D>
void TestDiff(const T& t, const D& d) 
{
  using namespace Daixt::OutputUtilities;
  using namespace Daixt::ExprManip;
  using namespace Daixt::Differentiation;

  std::cerr << "Diff(" << t << ", " << d << ")\n   = "
            <<  Diff(t, d) << "\n   = " << Simplify(Diff(t, d)) << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// ************************************************************************** //
////////////////////////////////////////////////////////////////////////////////


DECLARE_VARIABLE(a, 1, "a");
DECLARE_VARIABLE(b, 2, "b");
DECLARE_VARIABLE(c, 3, "c");
DECLARE_VARIABLE(d, 4, "d");



int main()
{
  Daixt::IsNull<DisambiguatedVariable> Null;
  Daixt::IsOne<DisambiguatedVariable> One;
  typedef Daixt::Scalar<DisambiguatedVariable> Scalar;

  using namespace Daixt::DefaultOps;

  TestSimplify(a * Null);
  TestSimplify(Null * (a + b));
  TestSimplify(a * One);
  TestSimplify(One * (a + b));
  TestSimplify(a + Null);
  TestSimplify(Null + (a + a));
  TestSimplify(a + One);
  TestSimplify(One + (a + b));
  TestSimplify(One / One);

  TestSimplify(Null / (a / a));
  TestSimplify(a / One);
  TestSimplify(One / (a / b));

  TestSimplify( a - b);
  TestSimplify( - b + a);
  TestSimplify(RationalPow<3, 2>(a + b / (c - d)));
  TestSimplify(RationalPow<3, 2>(a + b / (c - d)));

  TestSimplify((Scalar(3.0) / Scalar(2.0)) * RationalPow<3, 2>(a/d));

  TestSimplify(a * (-b));
  TestSimplify(a * (-One));
  TestSimplify(a * One);
  TestSimplify(a * (-One));
  TestSimplify(- (a * One));

  TestSimplify((-a) * b);
  TestSimplify((-a) * (-b));
  TestSimplify(a * (-b));
  TestSimplify(-(-a));


  //////////////////////////////////////////////////////////////////////////////
  // compile time differentiation

  TestDiff(RationalPow<3, 2>(d), d);
  TestDiff(a + b / (c - d), d);
  TestDiff(a/d, d);
  TestDiff(Daixt::Differentiation::Diff(a/d, d), d);
}
