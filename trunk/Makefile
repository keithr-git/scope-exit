#
# Copyright 2008, 2009 Keith Reynolds.
#
# This program is free software: you can redistribute it
# and/or modify it under the terms of the GNU Lesser General
# Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  See the GNU Lesser General Public License for
# more details.
#
# You should have received a copy of the GNU Lesser General
# Public License along with this program.  If not, see
# <http://www.gnu.org/licenses/>.
#
# CXX		:= icc

SOURCES		= $(wildcard *.cpp)
TARGETS		= $(SOURCES:%.cpp=%)
CPPFLAGS	= -I.
CXXFLAGS	= $(CPPFLAGS) -g -Wall -Werror
CXXFLAGS_icc	= -wd304 -wd981 -wd411
CXXFLAGS_gcc	= -Wextra
CXXFLAGS_g++	= $(CXXFLAGS_gcc)
CXXFLAGS	+= $(CXXFLAGS_$(notdir $(CXX)))

all: $(TARGETS) $(TARGETS:%=%-compliant)

$(TARGETS): %: %.o
	$(CXX) $(CXXFLAGS) -o $@ $*.o $(LIBS)

$(TARGETS:%=%-compliant): %: %.o
	$(CXX) $(CXXFLAGS) -o $@ $*.o $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MD -MF .$*.d -c $*.cpp

%-compliant.o: %.cpp
	$(CXX) $(CXXFLAGS) -DBOOST_TYPEOF_COMPLIANT -MD -MF .$*-compliant.d -o $*-compliant.o -c $*.cpp

%.opt.o: %.cpp
	$(CXX) $(CXXFLAGS) -DASM_LIST -fomit-frame-pointer -O3 -MD -MF .$*.opt.d -o $*.opt.o -c $*.cpp

%.i: %.cpp __ALWAYS__
	$(CXX) -E $(CPPFLAGS) $*.cpp | astyle --style=ansi | sed '/^# [0-9]/d' > $@

%-compliant.i: %.cpp __ALWAYS__
	$(CXX) -E $(CPPFLAGS) -DBOOST_TYPEOF_COMPLIANT $*.cpp | astyle --style=ansi | sed '/^# [0-9]/d' > $@

%.asm: %.o
	objdump -rdSC $*.o > $@

%.lst: %.opt.o
	objdump -rdSC $*.opt.o > $@

DEPEND_FILES	:= $(wildcard .*.d)

.PHONY: __ALWAYS__

ifdef DEPEND_FILES
include $(DEPEND_FILES)
endif

CLEANFILES	+= $(TARGETS)
CLEANFILES	+= $(TARGETS:%=%-compliant)
CLEANFILES	+= $(SOURCES:%.cpp=%.o)
CLEANFILES	+= $(SOURCES:%.cpp=%.opt.o)
CLEANFILES	+= $(SOURCES:%.cpp=%-compliant.o)
CLEANFILES	+= $(SOURCES:%.cpp=%.lst)
CLEANFILES	+= $(SOURCES:%.cpp=%.asm)
CLEANFILES	+= $(SOURCES:%.cpp=%.i)
CLEANFILES	+= $(SOURCES:%.cpp=%-compliant.i)
CLEANFILES	+= $(DEPEND_FILES)

CLEANFILES	:= $(wildcard $(CLEANFILES))
clean:
ifdef CLEANFILES
	rm -f $(CLEANFILES)
endif

#
# All the crazy templating makes the compiler use lots of RAM, so no
# parallel make for us.
#
.NOTPARALLEL:
