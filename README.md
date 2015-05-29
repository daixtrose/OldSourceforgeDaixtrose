# Daixtrose - Differentiable EXpression Templates - the Reusable Open Source Engine

This repository contains the code from http://daixtrose.sf.net. 10 Years ago this software was cool. 
Now it probably only compiles after some adoptions. Do not judge my C++ skills by looking at this libary. I was young and motivated, but from today's POV I could have done better. 

You can get an idea what Daixtrose was for by reading
[the old docs](daixtrose/daixtrose/wwwdoc/manual/MAIN/main.pdf).
Its main purpose and killer feature is automatic symbolic differentiation of formulas *at compile time*.

If you want to play with Expression Templates *today* you'd better have a look at [boost::proto](http://www.boost.org/doc/libs/1_58_0/doc/html/proto.html) from @EricNiebler. 
It does not provide automatic differentiation but according to Eric this should be easy. 
If you pay my salary for 9 months I will rewrite Daixtrose on top of [boost::proto-0x](https://github.com/ericniebler/proto-0x).

You can also take a look at http://2013.cppnow.org/files/2013/03/janssens.abstract.pdf to get an idea what proto can be used for.  
