#include "daixtrose/Daixt.h"

template <const char Name[256]> class Variable {};

template <const char Name[256]>
inline std::ostream& operator<<(std::ostream& os, const Variable<Name>& v)
{
  return os << Name;
}

struct VariableDisambiguation {};
namespace Daixt 
{ 
template <const char Name[256]> 
struct Disambiguator<Variable<Name> > 
{
  static const bool is_specialized = true;
  typedef VariableDisambiguation Disambiguation;
};
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


//////////////////////////////////////////////////////////////////////
#define DECLARE_VARIABLE(VARNAME) \
extern const char NameOf##VARNAME[256] = #VARNAME ; \
Variable<NameOf##VARNAME> VARNAME


DECLARE_VARIABLE(a);
DECLARE_VARIABLE(b);
DECLARE_VARIABLE(c);
DECLARE_VARIABLE(d);


int main()
{
  using namespace Daixt::DefaultOps;

  TestDiff(RationalPow<3, 2>(d), d);
  TestDiff(a + b / (c - d), d);
  TestDiff(a/d, d);
}

