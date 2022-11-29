## IMS ballistics simulation
#
# Brief: Makefile
# Author: David Konečný (xkonec83)
# Date: 29.11.2022

CC = g++
CFLAGS = -g -O2
PROJECT = ballistics

$(PROJECT): $(PROJECT).o
	$(CC) $(CFLAGS) -o $(PROJECT) $(PROJECT).o -lsimlib -lm

$(PROJECT).o: $(PROJECT).cpp
	$(CC) $(CFLAGS) -c $(PROJECT).cpp -lsimlib -lm

clean:
	rm $(PROJECT) $(PROJECT).o
