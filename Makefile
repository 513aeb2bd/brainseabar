CC = clang
LDFLAGS = -s
LDLIBS =
CFLAGS = -O2
COBJDIR = ./OBJS

files = \
        main.o \
        prep-bsbcode.o \
        proc-bsbcode.o
objects = $(addprefix $(COBJDIR)/,$(files))

bsb: $(objects)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(COBJDIR)/main.o: main.c main.h
	$(CC) -c -o $@ $< $(CFLAGS)
$(COBJDIR)/prep-bsbcode.o: prep-bsbcode.c main.h
	$(CC) -c -o $@ $< $(CFLAGS)
$(COBJDIR)/proc-bsbcode.o: proc-bsbcode.c main.h
	$(CC) -c -o $@ $< $(CFLAGS)
