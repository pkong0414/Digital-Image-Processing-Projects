#this is the make file for project 3

CC = g++
CPPFLAGS = -Wall -std=c++11
TARGET = corpus
SRC = ImageCorpus.cpp
OBJ = ImageCorpus.o

$TARGET: $(OBJ)
	$(CC) $(CPPFLAGS) -o $(TARGET) $(OBJ)

ImageCorpus.o     : ImageCorpus.cpp

clean :
	/bin/rm '-f' *.o $(TARGET)