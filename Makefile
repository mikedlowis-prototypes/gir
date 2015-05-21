LD = $(CC)
MAKEDEPEND = $(CPP) -M $(CPPFLAGS) -o  $<
CCDEPGEN = -MMD -MF $*.d
CFLAGS = -Wall -Wextra -O3 --std=c99 --pedantic

SOURCES = $(wildcard source/*.c)

-include $(SOURCES:.c=.d)

parser: source/lex.yy.o source/main.o
	$(LD) $(LDFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CCDEPGEN) $(CFLAGS) -c $< -o $@

source/lex.yy.c: source/lexer.l
	$(LEX) $(LFLAGS) -o $@ $^

clean:
	rm -f source/*.o
	rm -f source/*.d
	rm -f source/lex.yy.c
	rm -f parser parser.exe

