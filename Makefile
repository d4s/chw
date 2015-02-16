
CFLAGS += -I. -std=gnu99 -Wall -pedantic

SRCS = hpb.pb-c.c htree.c pqueue.c hblock.c parse_args.c huffman.c
OBJS = $(patsubst %.c,%.o,$(wildcard $(SRCS))) 

LIBS = -lprotobuf-c

TESTFILE ?= test.file

TESTS ?= 3

ifeq ($(openmp),enabled)
# OpenMP
CFLAGS += -fopenmp
LIBS += -lgomp
endif

ifneq ($(mode),debug)
CFLAGS        += -O4
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

.PHONY: test ctest dtest
udata: gen_unbalanced_data
	@echo Create unbalanced data
	./gen_unbalanced_data > $(TESTFILE)

$(TESTFILE): udata

ctest:
	@echo Compression:
	time ./huffman -c $(TESTFILE) $(TESTFILE).hz
	@ls -l $(TESTFILE) $(TESTFILE).hz
	@echo Triple compression test with null output: 
	@for((i=0;i<$(TESTS);i++)); do time -f "Pass: $$i %U" ./huffman -c $(TESTFILE) /dev/null; done

dtest:
	@echo Decompression:
	time ./huffman -x $(TESTFILE).hz $(TESTFILE).new
	@echo Triple decompression test with null output: 
	@for((i=0;i<$(TESTS);i++)); do time -f "Pass: $$i %U" ./huffman -x $(TESTFILE).hz /dev/null; done

test: ctest dtest
	@echo Checking if original and decompressed files are the same:
	@md5sum $(TESTFILE) $(TESTFILE).new

.PHONY: clean

clean:
		@rm -f $(OBJS) hpb.pb-c.[ch] huffman gen_unbalanced_data $(TESTFILE) $(TESTFILE).hz $(TESTFILE).new
