# for mac
CC = g++
TARGET=delcmt
LIBS=

ifeq ($(OS),Windows_NT)
LIBS+=-lversion -lstdc++
endif

.cpp.o:
	$(CC) -c $<

SOURCES=delcmt.cpp

all: $(TARGET)

$(TARGET): ${SOURCES:.cpp=.o}
	$(CC) -o $@ $^ $(LIBS)
