INCLUDES = \
    source/ \
    modules/libcds/source/ \
    modules/libcds/source/buffer/ \
    modules/libcds/source/cmp/ \
    modules/libcds/source/exn/ \
    modules/libcds/source/list/ \
    modules/libcds/source/map/ \
    modules/libcds/source/mem/ \
    modules/libcds/source/murmur3/ \
    modules/libcds/source/rbt/ \
    modules/libcds/source/set/ \
    modules/libcds/source/string/ \
    modules/libcds/source/vector/

SOURCES = $(wildcard source/*.c) \
          $(wildcard modules/libcds/source/*/*.c)

LD = $(CC)
MAKEDEPEND = $(CPP) -M $(CPPFLAGS) -o  $<
CCDEPGEN = -MMD -MF $*.d
CFLAGS = -Wall -Wextra -O3 --std=c99 --pedantic $(addprefix -I,$(INCLUDES))

-include $(SOURCES:.c=.d)

parser: source/lex.yy.o $(SOURCES:.c=.o)
	$(LD) $(LDFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CCDEPGEN) $(CFLAGS) -c $< -o $@

source/lex.yy.c: source/lexer.l
	$(LEX) $(LFLAGS) -o $@ $^

clean:
	rm -f $(SOURCES:.c=.o)
	rm -f $(SOURCES:.c=.d)
	rm -f source/lex.yy.c
	rm -f parser parser.exe

