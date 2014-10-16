# http://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -std=c11 -Wall -lreadline

# the build target executable:
TARGET = bsh

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) -o $(TARGET) $(TARGET).c $(CFLAGS)

clean:
	$(RM) $(TARGET)
    
