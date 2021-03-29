#
#  Makefile for fusiontree
#
#  Created by Mateus Bezrutchka on 2/3/18 based on Makefiles for projects from
#  MIT class 6.172
#

#
#  Usage:
#  		"make" will make the main program by default
#			"make clean" remove all files created by previous compilations
#     "make format" formats all source code according to Google's format for C++
#

HEADERS = $(wildcard *.hpp)
SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)
PROGRAM = main

COMP = clang++
COMPFLAGS = -Wall -g -mavx2 -pg
LDFLAGS = -lm

ifeq ($(DEBUG),1)
	COMPFLAGS += -O0
else
	COMPFLAGS += -O3
endif

ifeq ($(NAIVE),1)
	COMPFLAGS += -DNAIVE=1
endif

all:		$(PROGRAM)

format:
	clang-format -style='Google' -i *.cpp *.hpp

clean:
	$(RM) $(PROGRAM) *.o *.out

%.o:    %.cpp $(HEADERS)
	$(COMP) $(COMPFLAGS) -o $@ -c $<

$(PROGRAM): $(OBJECTS)
	$(COMP) $(LDFLAGS) -o $@ $(OBJECTS)
