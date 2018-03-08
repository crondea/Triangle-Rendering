# Corey Rondeau
# ECE 4680 Lab 5
# March 1, 2018
#
# The purpose of this file is to build my solution including helper programs as well as the 
# main compression file. For submission, the helper programs will be commented out.

comp = gcc
comp_flags = -g -O4
comp_libs = -lm
OBJECTS = prog 

all: $(OBJECTS)

prog: Lab5Main.c render.c render.h
	$(comp) $(comp_flags) render.c Lab5Main.c -o prog $(comp_libs)

clean:
	rm -f $(OBJECTS) *.o core*