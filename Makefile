AS      =as
LD      =ld
CC      =g++
AR      =ar



CFLAGS =  -g -Wall -pedantic -std=c++0x -I./
LDFLAGS = -std=c++0x -I./
SRCS 	=  Channel.cpp EPoller.cpp EventLoop.cpp  
OBJS := $(SRCS:.cpp=.o)
EXE1 := SimplePingPong.exe 
EXE2 := SimplePingPong-client.exe

all:$(OBJS) $(EXE1)  $(EXE2)

$(EXE1):$(OBJS) 
	$(CC) -g -o  $@ SimplePingPong.cpp $(OBJS) $(LDFLAGS)

$(EXE2):$(OBJS) 
	$(CC) -g -o $@ SimplePingPong-client.cpp $(OBJS) $(LDFLAGS)

$(OBJS):%.o:%.cpp	
	$(CC) -g -c -o $@  $< $(CFLAGS)
.SUFFIXES:
.SUFFIXES: .c .o

.PHONY:clean
clean: 
	$(RM) *.o 
	$(RM) ${OBJS} ${EXE1} $(EXE2)
