#------------------------------------------------------------------------------
# Build Configuration
#------------------------------------------------------------------------------
# Update these variables according to your requirements.
VERSION = 0.1

# tools
CC = c99
LD = ${CC}

# flags
INCS      = -Isource/ -Itests/
CPPFLAGS  = -DVERSION=\"${VERSION}\" -D_XOPEN_SOURCE=700
CFLAGS   += ${INCS} ${CPPFLAGS}
LDFLAGS  += ${LIBS}

#------------------------------------------------------------------------------
# Build Targets and Rules
#------------------------------------------------------------------------------
SRCS = source/gc.c     \
       source/gir.c    \
       source/hamt.c   \
       source/parser.c \
       source/slist.c
OBJS = ${SRCS:.c=.o}

REPL_SRCS = ${SRCS} source/main.c
REPL_OBJS = ${REPL_SRCS:.c=.o}

TEST_SRCS= tests/main.c        \
           tests/atf.c         \
           tests/test_block.c  \
           tests/test_false.c  \
           tests/test_list.c   \
           tests/test_map.c    \
           tests/test_num.c    \
           tests/test_string.c \
           tests/test_true.c   \
           tests/test_array.c  \
           tests/test_bool.c   \
           tests/test_gir.c    \
           tests/test_lobby.c  \
           tests/test_nil.c    \
           tests/test_set.c    \
           tests/test_symbol.c
TEST_OBJS = ${TEST_SRCS:.c=.o}

all: options gir testgir

options:
	@echo "CC       = ${CC}"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LD       = ${LD}"
	@echo "LDFLAGS  = ${LDFLAGS}"

gir: ${REPL_OBJS}
	@echo LD $@
	@${LD} -o $@ ${REPL_OBJS} ${LDFLAGS}

testgir: ${TEST_OBJS} ${OBJS}
	@echo LD $@
	@${LD} -o $@ ${TEST_OBJS} ${OBJS} ${LDFLAGS}
	@./testgir

.c.o:
	@echo CC $<
	@${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f gir testgir ${OBJS} ${TEST_OBJS}

.PHONY: all options

