
CFLAGS += -I. -std=c99

OBJS = huffman.o htree.o

# OpenMP
CFLAGS += -fopenmp -DOPENMP

all: huffman


.o: .c
	$(CC) $(CFLAGS) -o $@ $<


huffman: $(OBJS)
		$(CC) $(LDFLAGS) $(LIBS) $(OBJS) -o $@ 

.PHONY: clean

clean:
		@rm -f $(OBJS) huffman
