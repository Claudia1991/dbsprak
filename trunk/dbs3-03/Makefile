TARGET = ManHunt
OBJS	= ManHunt.o Matrix.o

#CC = gcc
CC = g++

CFLAGS = -pthread -DUSE_MUTEX -DUSE_AFFINITY -DCALC_TIME

all: ${TARGET}

${TARGET}: $(OBJS)
ifdef DEBUG
	$(CC) $(CFLAGS) -D_DEBUG -o $@ $^
else
	$(CC) $(CFLAGS) -o $@ $^
endif

%.o: %.c
ifdef DEBUG
	$(CC) $(CFLAGS) -D_DEBUG -c $<
else
	$(CC) $(CFLAGS) -c $<
endif

clean:
	$(RM) $(RMFLAGS) ${TARGET} ${OBJS}
