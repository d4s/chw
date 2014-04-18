
CFLAGS += -I. -std=gnu99 -Wall -pedantic

OBJS = huffman.o htree.o pqueue.o hpb.pb-c.o

LIBS = -lprotobuf-c

ifeq ($(openmp),enabled)
# OpenMP
CFLAGS += -fopenmp
endif

ifeq ($(mode),release)
CFLAGS        += -O2
else
# Debug
CFLAGS        += -DDEBUG -g -O0
endif


all: huffman

.o: .c
	$(CC) $(CFLAGS) -o $@ $<


huffman: $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@ 

hpb.pb-c.c: hpb.proto
	protoc-c --c_out=. -I=. $<


.PHONY: clean

clean:
		@rm -f $(OBJS) huffman
