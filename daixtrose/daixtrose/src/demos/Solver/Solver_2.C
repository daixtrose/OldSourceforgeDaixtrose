#include "daixtrose/Daixt.h"
#include "tiny/TinyMatAndVec.h" 

// FIXIT: find out why wstring did not work on some gcc
#define DISABLE_WIDE_CHAR_SUPPORT
#include <boost/lexical_cast.hpp>
#include <boost/mpl/int.hpp>

#include <iostream>
#include <string>
#include <vector>

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

#define DECLARE_VARIABLE(NAME, NUMBER, LATEX_NAME)	\
	template <>										\
	std::string Variable<NUMBER>::GetName() const	\
	{												\
		return LATEX_NAME;							\
	}												\
													\
	Variable<NUMBER> NAME

#define EXTERN_DECLARE_VARIABLE(NAME, NUMBER)	\
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

template <int NumberOfVars>
struct AccessibleBaseClass 
{ 
	virtual std::string GetName() const = 0; 
	virtual double 
	GetValue(std::vector<double> const & V,
			 std::size_t index) const = 0; 
	virtual AccessibleBaseClass<NumberOfVars> * DeepCopy() const = 0; 
	virtual ~AccessibleBaseClass() {} 
};

 
// specializations which allows us to plug polymorphic behviour to our 
// expressions

namespace Daixt 
{ 

template <class T, int NumberOfVars> 
class FeaturesOfExpression<AccessibleBaseClass<NumberOfVars>, T> 
	: public AccessibleBaseClass<NumberOfVars> 
{ 
public: 
	std::string GetName() const 
	{ 
		return ::GetName(static_cast<const T&>(*this)); 
	} 
  
	////////////////////////////////////////////////////////////////////////
	// getting values 

private:
	////////////////////////////////////////////////////////////////////////
	// access to the expression values:

	template <std::size_t Number>
	inline double GetValue(const Variable<Number> & v,
						   std::vector<double> const & V,
						   std::size_t index) const 
	{ 
		// assuming memory layout 
		//
		// var_1[0]
		// var_2[0]
		// ...
		// var_N[0]
		// var_1[1]
		// var_2[1]
		// ...
		// var_N[1]
		// etc ...

		// please check this quick hack access formula, it might be wrong!
		// Number starts at index 1, but std::vector at index 0
		std::size_t const myindex = index * NumberOfVars + Number - 1;
#ifndef NDEBUG
		return V.at(myindex);
#else
		return V[myindex];
#endif
	}
   
	template <class ARG, class OP> 
	inline double GetValue(const Daixt::UnOp<ARG, OP>& UO,
						   std::vector<double> const & V,
						   std::size_t index) const 
	{ 
		return OP::Apply(this->GetValue(UO.arg(), V, index), 
						 Daixt::Hint<double>());
	} 
   
	template <class LHS, class RHS, class OP> 
	inline double GetValue(const Daixt::BinOp<LHS, RHS, OP>& BO,
						   std::vector<double> const & V,
						   std::size_t index) const 
	{ 
		return OP::Apply(this->GetValue(BO.lhs(), V, index), 
						 this->GetValue(BO.rhs(), V, index), 
						 Daixt::Hint<double>());
	} 

	template<class TT> 
	inline double GetValue(const Daixt::Expr<TT>& E,
						   std::vector<double> const & V,
						   std::size_t index) const 
	{ 
		return this->GetValue(E.content(), V, index); 
	} 

	inline double 
	GetValue(Daixt::IsNull<DisambiguatedVariable>,
			 std::vector<double> const & V,
			 std::size_t index) const 
	{
		return 0.0;
	}
   
	inline double 
	GetValue(Daixt::IsOne<DisambiguatedVariable>,
			 std::vector<double> const & V,
			 std::size_t index) const 
	{
		return 1.0;
	}

	inline double 
	GetValue(const Daixt::Scalar<DisambiguatedVariable> & v,
			 std::vector<double> const & V,
			 std::size_t index) const
	{
		return v.Value();
	}

public:
	////////////////////////////////////////////////////////////////////////

	double GetValue(std::vector<double> const & V,
					std::size_t index) const
	{
		return this->GetValue(static_cast<const T&>(*this), V, index);
	}

	FeaturesOfExpression<
		AccessibleBaseClass<NumberOfVars>, T>* DeepCopy() const 
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

	typedef AccessibleBaseClass<NumberOfVars> Accessor;

	typedef boost::mpl::int_<NumberOfVars> number_of_vars_t;

	// storing values outside the variable objects !!!!
	// this allows sharing with f77 or C legacy code
	std::vector<double> Values_;

public:
	void Resize(std::size_t size)
	{
		Values_.resize(NumberOfVars * size);

		// fill in some useful values, please replace by your own
		// initialize method
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < NumberOfVars; ++j)
			{
				std::cerr << "Values_[" << i * NumberOfVars + j << "] = " 
						  << j + 1.0 << std::endl;
				Values_[i * NumberOfVars + j] = j + 1.0;
			}
		}
	}
	
private:
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
				Daixt::ChangeDisambiguation<Accessor>
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
				Daixt::ChangeDisambiguation<Accessor>
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
			Daixt::ChangeDisambiguation<Accessor>(e).
			DeepCopy(); 

		std::cerr << "registrating expression '" 
				  << Expressions_(Row)->GetName() << "'" << std::endl;

		// this stores all d(e)/d(var(i))
		StoreDiff<Row, NumberOfVars>::Apply(e, Jacobian_);
	}


public:
	template<typename T>
	inline void PrintValue(Daixt::Expr<T> const & e,
						   std::size_t index)
	{
		std::cout << "Value of "
				  << (Daixt::ChangeDisambiguation<Accessor>(e)
					  .GetName())
				  << "[" << index << "] = "
				  << (Daixt::ChangeDisambiguation<Accessor>(e)
					  .GetValue(Values_, index))
				  << std::endl;
	}

	inline void PrintJacobian(std::size_t index)
	{
		std::cout << "\nValues of Jacobian at " << index << ":\n";

		for (int j = 1; j < NumberOfVars + 1; ++j)
		{ 
			for (int i = 1; i < NumberOfVars + 1; ++i)
			{ 
				std::cout << Jacobian_(i, j)->GetValue(Values_, index) << " ";
			}
			std::cout << std::endl;
		}
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
		TinyVector<AccessibleBaseClass<4> *, 4>, 
		TinyQuadraticMatrix<AccessibleBaseClass<4> *, 4> 
		> MySolver;

	MySolver.AddExpression<1>((a + b) * c);
	MySolver.AddExpression<2>(d - a);
	MySolver.AddExpression<3>(RationalPow<3,2>(a));
	MySolver.AddExpression<4>((a + b) * c);

	MySolver.Resize(3);

	MySolver.PrintValue(Daixt::make_expr(a), 2);
	MySolver.PrintValue(Daixt::make_expr(b), 2);
	MySolver.PrintValue(Daixt::make_expr(c), 2);
	MySolver.PrintValue(Daixt::make_expr(d), 2);

	MySolver.PrintJacobian(2);
}
