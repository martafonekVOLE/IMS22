## IMS balistika ve vojenství
#
# Brief: Makefile
# Author: David Konečný (xkonec83)
# Date: 4.12.2022

CC = g++
CFLAGS = -g -O2
PROJECT = ballistics
OBJECTS = ballistics.o arg_parse.o

$(PROJECT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROJECT) $(PROJECT).o arg_parse.o -lsimlib -lm

$(PROJECT).o: $(PROJECT).cpp
	$(CC) $(CFLAGS) -c $(PROJECT).cpp -lsimlib -lm

arg_parse.o: arg_parse.cpp
	$(CC) $(CFLAGS) -c arg_parse.cpp -lsimlib -lm

clean:
	rm $(PROJECT) $(PROJECT).o arg_parse.o
