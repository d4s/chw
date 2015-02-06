
CFLAGS += -I. -std=gnu99 -Wall -pedantic

SRCS = hpb.pb-c.c htree.c pqueue.c hblock.c parse_args.c huffman.c
OBJS = $(patsubst %.c,%.o,$(wildcard $(SRCS))) 

LIBS = -lprotobuf-c

TESTFILE ?= test.file

ifeq ($(openmp),enabled)
# OpenMP
CFLAGS += -fopenmp
LIBS += -lgomp
endif

ifneq ($(mode),debug)
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
		

$(OBJS): hpb.pb-c.c

.o: .c
	$(CC) $(CFLAGS) -o $@ $<


huffman: $(OBJS) hpb.pb-c.o
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@ 

hpb.pb-c.o: hpb.pb-c.c

hpb.pb-c.c: hpb.proto
	protoc-c --c_out=. -I=. $<

.PHONY: test ctest dtest udata
udata: gen_unbalanced_data
	@echo Create unbalanced data
	./gen_unbalanced_data > $(TESTFILE)


ctest: udata
	@echo Compression:
	time ./huffman < $(TESTFILE) > $(TESTFILE).hz

dtest:
	@echo Decompression:
	time ./huffman -d < $(TESTFILE).hz >$(TESTFILE).new


test: ctest dtest
	md5sum $(TESTFILE) $(TESTFILE).new

	@echo Compression with output to /dev/null:
	time ./huffman < $(TESTFILE) > /dev/null
	@echo Decompression with output to /dev/null:
	time ./huffman -d < $(TESTFILE).hz > /dev/null

.PHONY: clean

clean:
		@rm -f $(OBJS) hpb.pb-c.[ch] huffman gen_unbalanced_data
