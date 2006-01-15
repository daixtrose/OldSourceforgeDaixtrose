#include "daixtrose/Daixt.h"
#include "tiny/TinyMatAndVec.h" 

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>

// code for ML response December 2005

////////////////////////////////////////////////////////////////////////////////
// ************************************************************************** //
////////////////////////////////////////////////////////////////////////////////

// A named and compile-time-numbered variable which can be used for
// differentiation. 

struct DisambiguatedVariable {};

template <std::size_t Number> 
class Variable
{
public:
  inline Variable() {} // intel's C++-70 needs this one
  typedef DisambiguatedVariable Disambiguation;
  std::string GetName() const;// { return "undefined"; }
};


////////////////////////////////////////////////////////////////////////////
// convenience macros

#define DECLARE_VARIABLE(NAME, NUMBER, LATEX_NAME)            \
template <>                                                   \
std::string Variable<NUMBER>::GetName() const                 \
{                                                             \
  return LATEX_NAME;                                          \
}                                                             \
                                                              \
Variable<NUMBER> NAME

#define EXTERN_DECLARE_VARIABLE(NAME, NUMBER)                 \
extern Variable<NUMBER> NAME


////////////////////////////////////////////////////////////////////////////
// access to the expression name

template <std::size_t Number>
inline std::string GetName(const Variable<Number> & v) 
{ 
  return v.GetName(); 
} 
   
template <class ARG, class OP> 
inline std::string GetName(const Daixt::UnOp<ARG, OP>& UO) 
{ 
  return std::string("(") + OP::Symbol() + " " + GetName(UO.arg()) + ")";
} 
   
template <class LHS, class RHS, class OP> 
inline std::string GetName(const Daixt::BinOp<LHS, RHS, OP>& BO) 
{ 
  return 
    "(" + GetName(BO.lhs()) 
    + " " + OP::Symbol() + " " 
    + GetName(BO.rhs()) + ")"; 
} 

template<class T> 
inline std::string GetName(const Daixt::Expr<T>& E) 
{ 
  return GetName(E.content()); 
} 

inline std::string 
GetName(Daixt::IsNull<DisambiguatedVariable>) 
{
  return "0";
}
   
inline std::string 
GetName(Daixt::IsOne<DisambiguatedVariable>) 
{
  return "1";
}

inline std::string 
GetName(const Daixt::Scalar<DisambiguatedVariable> & v)
{
  return boost::lexical_cast<std::string>(v.Value());
}

template<typename ARG, int m, int n> 
inline std::string 
GetName(const Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<m, n> >& UO)
{
  return "(" + GetName(UO.arg()) + ")^(" 
    + boost::lexical_cast<std::string>(m) 
    + "/" 
    + boost::lexical_cast<std::string>(n) + ")"; 
}



////////////////////////////////////////////////////////////////////////////
// some features for our expressions 

struct AccessibleBaseClass 
{ 
  virtual std::string GetName() const = 0; 
  virtual AccessibleBaseClass* DeepCopy() const = 0; 
  virtual ~AccessibleBaseClass() {} 
};

 
// specializations which allows us to plug polymorphic behviour to our 
// expressions

namespace Daixt 
{ 

template <class T> 
class FeaturesOfExpression<AccessibleBaseClass, T> 
  : public AccessibleBaseClass 
{ 
public: 
  std::string GetName() const 
  { 
    return ::GetName(static_cast<const T&>(*this)); 
  } 
  
  FeaturesOfExpression<AccessibleBaseClass, T>* DeepCopy() const 
  { 
    return new T(static_cast<const T&>(*this)); 
  } 
  
  virtual ~FeaturesOfExpression() {}; 
}; 

} // namespace Daixt 
   

   

////////////////////////////////////////////////////////////////////////////
// ********************************************************************** //
////////////////////////////////////////////////////////////////////////////

template <int NumberOfVars, 
          typename VectorT, 
          typename MatrixT>
class Solver
{
  VectorT Expressions_;
  MatrixT Jacobian_;

  template<int Row, int Col = NumberOfVars>
  struct StoreDiff
  {
    template<typename T>
    static inline void 
    Apply(Daixt::Expr<T> const & e, MatrixT & Jacobian)
    {
      using namespace Daixt::ExprManip;
      using namespace Daixt::Differentiation;

      Variable<Col> Differ;

      
      Jacobian(Row, Col) = 
        Daixt::ChangeDisambiguation<AccessibleBaseClass>
        (Simplify(Diff(e, Differ))).DeepCopy(); 
      
      std::cerr << "Stored d(" << GetName(e) 
                << ") / d(" << Differ.GetName()
                << ") = " 
                << Jacobian(Row, Col)->GetName()
                << " at (" << Row << ", " << Col << ")" << std::endl;
      

      // recursive call
      StoreDiff<Row, Col-1>::Apply(e, Jacobian);
    }
  };

  
  // end of recursion
  template<int Row>
  struct StoreDiff<Row, 1>
  {
    template<typename T>
    static inline void 
    Apply(Daixt::Expr<T> const & e, MatrixT & Jacobian)
    {
      using namespace Daixt::ExprManip;
      using namespace Daixt::Differentiation;

      Variable<1> Differ;

      
      Jacobian(Row, 1) = 
        Daixt::ChangeDisambiguation<AccessibleBaseClass>
        (Simplify(Diff(e, Differ))).DeepCopy(); 

      std::cerr << "Stored d(" << GetName(e) 
                << ") / d(" << GetName(Differ) 
                << ") = " 
                << Jacobian(Row, 1)->GetName()
                << " at (" << Row << ", " << 1 << ")" << std::endl;


      // no more recursive call
    }
  };
  
public:

  template<int Row, typename T>
  void AddExpression(Daixt::Expr<T> const & e)
  {
    // you must delete this pointer in the destructor
    Expressions_(Row) = 
      Daixt::ChangeDisambiguation<AccessibleBaseClass>(e).DeepCopy(); 

    std::cerr << "registrating expression '" 
              << Expressions_(Row)->GetName() << "'" << std::endl;

    // this stores all d(e)/d(var(i))
      StoreDiff<Row, NumberOfVars>::Apply(e, Jacobian_);
  }
};


////////////////////////////////////////////////////////////////////////////////
// ************************************************************************** //
////////////////////////////////////////////////////////////////////////////////


DECLARE_VARIABLE(a, 1, "a");
DECLARE_VARIABLE(b, 2, "b");
DECLARE_VARIABLE(c, 3, "c");
DECLARE_VARIABLE(d, 4, "d");



int main()
{

  using namespace Daixt::DefaultOps;
  using namespace TinyMatAndVec; 

  Solver<4, 
         TinyVector<AccessibleBaseClass *, 4>, 
         TinyQuadraticMatrix<AccessibleBaseClass *, 4> 
    > MySolver;

  MySolver.AddExpression<1>((a + b) * c);
  MySolver.AddExpression<2>(d - a);
  MySolver.AddExpression<3>(RationalPow<3,2>(a));
  MySolver.AddExpression<4>((a + b) * c);

}
