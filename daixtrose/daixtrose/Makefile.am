# Copyright (C) 2003 Markus Werle
#
# This file is part of the Daixtrose C++ Library.  This library is free
# software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the
# Free Software Foundation; either version 2.1, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; see the file COPYING.  If not, send mail to the
# developers of daixtrose (see e.g. http://daixtrose.sourceforge.net/)
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU Lesser General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU Lesser General Public License.

# Where to find the headers
AM_CPPFLAGS = -I$(srcdir)/src -I@BOOSTDIR@ -I@LOKIDIR@ 

# we assume a compliant compiler, but maybe it's a good idea to get this
# refined in configure.in (contributions welcome)
AM_CPPFLAGS += -DBOOST_NO_CONFIG


check_PROGRAMS = \
	test_performance_matrix_times_vector \
	test_simple_get_value_1 \
	test_simple_get_value_2 \
	test_change_disambiguation \
	test_linalg \
	test_rowsum \
	test_tiny_mat \
	test_tiny_vec \
	test_inverse \
	test_block_mat \
	test_l2norm \
	test_matrix_print \
	test_simplify \
	quicktour_mini \
	quicktour_mini_2 \
	quicktour_mini_3 \
	quicktour_mini_4 \
	quicktour_mini_5 \
	quicktour_diff \
	quicktour_tiny \
	quicktour_linalg \
	quicktour_pm_lambda \
	quicktour_pm_lambda_boost

test_performance_matrix_times_vector_SOURCES = $(srcdir)/src/demos/linalg/PerformanceOfMatrixTimesVector.C

test_simple_get_value_1_SOURCES    = $(srcdir)/src/demos/SimpleGetValue.1/main.C
test_simple_get_value_1_CXXFLAGS = $(AM_CXXFLAGS) 

test_simple_get_value_2_SOURCES    = $(srcdir)/src/demos/SimpleGetValue.2/main.C
test_simple_get_value_2_CXXFLAGS = $(AM_CXXFLAGS) 

test_change_disambiguation_SOURCES = $(srcdir)/src/demos/ChangeDisambiguation/main.C
test_linalg_SOURCES                = $(srcdir)/src/demos/linalg/TestLinalg.C
test_rowsum_SOURCES                = $(srcdir)/src/demos/linalg/TestRowSum.C
test_tiny_mat_SOURCES              = $(srcdir)/src/demos/tiny/TestTinyMat.C
test_tiny_vec_SOURCES              = $(srcdir)/src/demos/tiny/TestTinyVec.C
test_inverse_SOURCES               = $(srcdir)/src/demos/linalg/TestInverse.C
test_block_mat_SOURCES             = $(srcdir)/src/demos/linalg/TestBlockedMatAndVec.C
test_l2norm_SOURCES 		   = $(srcdir)/src/demos/linalg/TestL2_Norm.C
test_simplify_SOURCES              = $(srcdir)/src/demos/simplify/TestSimplify.C
test_matrix_print_SOURCES          = $(srcdir)/src/demos/linalg/TestPrintingOfBlockedMatrix.C

quicktour_mini_SOURCES             =  $(srcdir)/src/demos/quicktour/Mini.C
quicktour_mini_2_SOURCES           =  $(srcdir)/src/demos/quicktour/Mini.2.C
quicktour_mini_3_SOURCES           =  $(srcdir)/src/demos/quicktour/Mini.3.C
quicktour_mini_4_SOURCES           =  $(srcdir)/src/demos/quicktour/Mini.4.C
quicktour_mini_5_SOURCES           =  $(srcdir)/src/demos/quicktour/Mini.5.C
quicktour_diff_SOURCES             =  $(srcdir)/src/demos/quicktour/Differentiation.C

quicktour_tiny_SOURCES             =  $(srcdir)/src/demos/quicktour/TinyMatrixAndVector.C 
quicktour_linalg_SOURCES           =  $(srcdir)/src/demos/quicktour/Linalg.C 
quicktour_pm_lambda_SOURCES        =  $(srcdir)/src/demos/quicktour/PoorMansLambda.C 

quicktour_pm_lambda_boost_SOURCES  =  $(srcdir)/src/demos/quicktour/PoorMansLambda.C 
quicktour_pm_lambda_boost_CPPFLAGS =  $(AM_CPPFLAGS) "-DUSE_BOOST_LAMBDA"

################################################################################
#finally we want these headers to be in the distribution


EXTRA_DIST = \
        $(srcdir)/src/daixtrose/BinOps.h \
        $(srcdir)/src/daixtrose/CompileTimeChecks.h \
        $(srcdir)/src/daixtrose/ConstRef.h \
        $(srcdir)/src/daixtrose/CountLeaves.h \
        $(srcdir)/src/daixtrose/CountOccurence.h \
        $(srcdir)/src/daixtrose/Daixt.h \
        $(srcdir)/src/daixtrose/DefaultOps.h \
        $(srcdir)/src/daixtrose/Differentiation.h \
        $(srcdir)/src/daixtrose/Disambiguation.h \
        $(srcdir)/src/daixtrose/Expr.h \
        $(srcdir)/src/daixtrose/ExtractOp.h \
        $(srcdir)/src/daixtrose/MatrixSelect.h \
        $(srcdir)/src/daixtrose/NeutralElements.h \
        $(srcdir)/src/daixtrose/PrettyPrinter.h \
        $(srcdir)/src/daixtrose/Scalar.h \
        $(srcdir)/src/daixtrose/Simplify.h \
        $(srcdir)/src/daixtrose/UnOps.h \
        $(srcdir)/src/daixtrose/ChangeDisambiguation.h \
        $(srcdir)/src/demos/SimpleGetValue.1/main.C \
        $(srcdir)/src/demos/SimpleGetValue.2/main.C \
        $(srcdir)/src/demos/linalg/TestRowSum.C \
        $(srcdir)/src/demos/linalg/TestBlockedMatAndVec.C \
        $(srcdir)/src/demos/linalg/TestInverse.C \
        $(srcdir)/src/demos/linalg/TestLinalg.C \
        $(srcdir)/src/demos/linalg/TestPrintingOfBlockedMatrix.C \
        $(srcdir)/src/demos/linalg/PerformanceOfMatrixTimesVector.C \
        $(srcdir)/src/demos/linalg/TestL2_Norm.C \
        $(srcdir)/src/demos/ChangeDisambiguation/main.C \
        $(srcdir)/src/demos/tiny/TestTinyMat.C \
        $(srcdir)/src/demos/tiny/TestTinyVec.C \
        $(srcdir)/src/demos/simplify/TestSimplify.C \
        $(srcdir)/src/demos/quicktour/Mini.C \
        $(srcdir)/src/demos/quicktour/TinyMatrixAndVector.C \
        $(srcdir)/src/demos/quicktour/Linalg.C \
        $(srcdir)/src/demos/quicktour/Mini.2.C \
        $(srcdir)/src/demos/quicktour/Mini.3.C \
        $(srcdir)/src/demos/quicktour/Mini.4.C \
        $(srcdir)/src/demos/quicktour/Mini.5.C \
        $(srcdir)/src/demos/quicktour/Differentiation.C \
        $(srcdir)/src/demos/quicktour/PoorMansLambda.C \
        $(srcdir)/src/linalg/Disambiguation.h \
        $(srcdir)/src/linalg/Inverse.h \
        $(srcdir)/src/linalg/L2_Norm.h \
        $(srcdir)/src/linalg/Linalg.h \
        $(srcdir)/src/linalg/Lump.h \
        $(srcdir)/src/linalg/Matrix.h \
        $(srcdir)/src/linalg/MatrixVectorOps.h \
        $(srcdir)/src/linalg/PrintBlockedMatrix.h \
        $(srcdir)/src/linalg/RowAndColumCounters.h \
        $(srcdir)/src/linalg/RowAndColumExtractors.h \
        $(srcdir)/src/linalg/RowSum.h \
        $(srcdir)/src/linalg/SliceIterator.h \
        $(srcdir)/src/linalg/SliceVector.h \
        $(srcdir)/src/linalg/Transpose.h \
        $(srcdir)/src/linalg/Vector.h \
        $(srcdir)/src/tiny/GetIndexedValue.h \
        $(srcdir)/src/tiny/MatrixVectorOps.h \
        $(srcdir)/src/tiny/TinyMatAndVec.h \
        $(srcdir)/src/tiny/TinyMatrix.h \
        $(srcdir)/src/tiny/TinyVector.h \
        $(srcdir)/src/error_handling/enforce.h \
        $(srcdir)/wwwdoc/bugs.html \
        $(srcdir)/wwwdoc/contact.html \
        $(srcdir)/wwwdoc/download.html \
        $(srcdir)/wwwdoc/faq.html \
        $(srcdir)/wwwdoc/index.html \
        $(srcdir)/wwwdoc/main.css \
        $(srcdir)/wwwdoc/license.html \
        $(srcdir)/wwwdoc/manual.html \
        $(srcdir)/wwwdoc/quicktour.html \
        $(srcdir)/wwwdoc/css/colors.css \
        $(srcdir)/wwwdoc/css/sflogotrick.css \
        $(srcdir)/wwwdoc/template.html \
        $(srcdir)/wwwdoc/themanual.html \
        $(srcdir)/wwwdoc/images/main.jpg \
        $(srcdir)/wwwdoc/images/sflogo.jpg \
        $(srcdir)/wwwdoc/images/background.jpg \
        $(srcdir)/wwwdoc/images/mini-logo.jpg


# A workaround for a missing automake feature
install-data-local:
	@echo "installing headers"; \
        for f_raw in $(EXTRA_DIST); do \
	  f=`echo $$f_raw | sed -e s,$(srcdir)/src/,,`; \
## Compute the install directory at runtime.
	  d=`echo $$f | sed -e s,[^/]*$$,,`; \
## Make the install directory.
          $(mkinstalldirs) $(DESTDIR)$(includedir)/$$d; \
## Find the header file -- in our case it might be in srcdir or
## it might be in the build directory.  "p" is the variable that
## names the actual file we will install.
          if test -f $(srcdir)/src/$$f; then p=$(srcdir)/src/$$f; else p=$$f; fi; \
## Actually install the file.
	  echo "installing " $(DESTDIR)$(includedir)/$$f; \
          $(INSTALL_DATA) $$p $(DESTDIR)$(includedir)/$$f; \
        done

