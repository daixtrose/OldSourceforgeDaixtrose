# Daixtrose - Differentiable EXpression Templates - the Reusable Open Source Engine

This repository contains the code from http://daixtrose.sf.net. 10 Years ago this software was cool. 
Now it probably only compiles after some adoptions. Do not judge my C++ skills by looking at this libary. I was young and motivated, but from today's POV I could have done better. It also was the Good Old Times when Real Programmers (TM) did not need unit tests.

You can get an idea what Daixtrose was for by reading
[the old docs](daixtrose/daixtrose/wwwdoc/manual/MAIN/main.pdf).
Its main purpose and killer feature is automatic symbolic differentiation of formulas *at compile time*.

If you want to play with Expression Templates *today* you'd better have a look at [boost::proto](http://www.boost.org/doc/libs/1_58_0/doc/html/proto.html) from [Eric Niebler](https://github.com/ericniebler/). 
It does not provide automatic differentiation but according to Eric this should be easy. And I recently found [some code from Joel Falcou](https://github.com/NumScale/boost-con-2011/tree/master/proto#part-2-computing-analytical-derivatives) and some [code that shows it might be simple](http://stackoverflow.com/questions/10526950/symbolic-differentiation-using-expression-templates-in-c).
If you pay my salary for 9 months I will rewrite Daixtrose on top of [boost::proto-0x](https://github.com/ericniebler/proto-0x).

You can also take a look at http://2013.cppnow.org/files/2013/03/janssens.abstract.pdf to get an idea what proto can be used for.  

# Download

TBD
