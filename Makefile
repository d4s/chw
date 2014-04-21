
CFLAGS += -I. -std=gnu99 -Wall -pedantic

SRCS = huffman.c htree.c pqueue.c hpb.pb-c.c
OBJS = $(patsubst %.c,%.o,$(wildcard $(SRCS))) 

LIBS = -lprotobuf-c

ifneq ($(openmp),disabled)
# OpenMP
CFLAGS += -fopenmp
LIBS += -lgomp
endif

ifeq ($(mode),release)
CFLAGS        += -O2
else
# Debug
CFLAGS        += -DDEBUG -g -O0
endif


all: huffman

hpb: hpbr hpbw

hpbr: hpb.pb-c.o testpbread.o
		$(CC) $(LDFLAGS) hpb.pb-c.o testpbread.o $(LIBS) -o $@ 
		

hpbw: hpb.pb-c.o testpbwrite.o
		$(CC) $(LDFLAGS) hpb.pb-c.o testpbwrite.o $(LIBS) -o $@ 
		


.o: .c
	$(CC) $(CFLAGS) -o $@ $<


huffman: $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@ 

hpb.pb-c.c: hpb.proto
	protoc-c --c_out=. -I=. $<


.PHONY: clean

clean:
		@rm -f $(OBJS) huffman
