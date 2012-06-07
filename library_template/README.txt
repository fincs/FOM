FOM Library Template
====================

Description
-----------

include
|
|--- MyLibrary.h: Public header file which contains the API

source
|
|--- Library.cpp: contains the implementation of the library interface
|--- TestClassImpl.h: class definition for the example TestClass
|--- TestClassImpl.cpp: method implementations of TestClass

library_template.fomreg: contains the names of the classes that this library implements
                         (required for FOM registration). Must have the same name as
                         the Makefile's TARGET variable.

Compiling
---------

"make install" should suffice.
