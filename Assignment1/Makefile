#this is the make file for project 3

CC = g++
CPPFLAGS = -Wall -std=c++11
TARGET = browser
SRC = browser.cpp
OBJ = browser.o

$TARGET: $(OBJ)
	$(CC) $(CPPFLAGS) -o $(TARGET) $(OBJ)

browser.o     : browser.cpp

clean :
	/bin/rm '-f' *.o $(TARGET)
