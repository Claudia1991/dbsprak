TARGET = ManHunt
OBJS = ManHunt.o
CC = g++
CFLAGS = -pthread -DUSE_AFFINITY -DCALC_TIME

all: ${TARGET}

${TARGET}: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(RMFLAGS) ${TARGET} ${OBJS}

build:
	g++ -pthread -o ManHunt ManHunt.cpp -Wall
