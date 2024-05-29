CC = clang
LDFLAGS = -s
LDLIBS =
CFLAGS = -c -O2
COBJDIR = ./OBJS

files = \
        main.o \
        prep-bsbcode.o \
        proc-bsbcode.o
objects = $(addprefix $(COBJDIR)/,$(files))

bsb: $(objects)
	@echo $(CC) -o $@ $(COBJDIR)/\*.o $(LDFLAGS) $(LDLIBS)
	@$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	rm $(COBJDIR)/*.o

$(COBJDIR)/main.o: main.c main.h
$(COBJDIR)/prep-bsbcode.o: prep-bsbcode.c main.h
$(COBJDIR)/proc-bsbcode.o: proc-bsbcode.c main.h

$(objects):
	$(CC) -o $@ $< $(CFLAGS)
