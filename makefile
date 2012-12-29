# for mac
CC = g++

.cpp.o:
	$(CC) -c $<

PROGRAM=delcmt

SOURCES=delcmt.cpp

all: $(PROGRAM)

$(PROGRAM): ${SOURCES:.cpp=.o}
	$(CC) -o $@ $^
