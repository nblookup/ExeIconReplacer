############################################################################
## Makefile
## Copyright (C) 2015 Katayama Hirofumi MZ.  All rights reserved.
############################################################################

#
# platform difference
#
DOTOBJ = .o
DOTEXE = .exe

#
# C++ compiler
#
CXX = g++
CXXFLAGS = -O2 -static -Wall -pedantic

#
# resource compiler
#
RC = windres
RCFLAGS = 

#
# objects
#
OBJS = \
	ExeIconReplacer$(DOTOBJ) \
	ExeIconReplacer_res$(DOTOBJ) \

#
# rules
#
all: ExeIconReplacer$(DOTEXE)

ExeIconReplacer$(DOTEXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o ExeIconReplacer$(DOTEXE) $(OBJS)

ExeIconReplacer$(DOTOBJ): ExeIconReplacer.h ExeIconReplacer.cpp
	$(CXX) $(CXXFLAGS) -c -o ExeIconReplacer$(DOTOBJ) ExeIconReplacer.cpp

ExeIconReplacer_res$(DOTOBJ): ExeIconReplacer.rc
	$(RC) $(RCFLAGS) -i ExeIconReplacer.rc -o ExeIconReplacer_res$(DOTOBJ)

clean:
	rm -f *.o
