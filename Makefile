#------------------------------------------------------------------------------
# Build Configuration
#------------------------------------------------------------------------------
# Update these variables according to your requirements.

# tools
CC = c99
LD = ${CC}
AR = ar

# flags
INCS      = -Isource/ -Itests/
CPPFLAGS  = -D_XOPEN_SOURCE=700
CFLAGS   += ${INCS} ${CPPFLAGS}
LDFLAGS  += ${LIBS}
ARFLAGS   = rcs

#------------------------------------------------------------------------------
# Build Targets and Rules
#------------------------------------------------------------------------------
SRCS = source/gc.c     \
       source/gir.c    \
       source/hamt.c   \
       source/parser.c \
       source/slist.c  \
       source/main.c
OBJS = ${SRCS:.c=.o}
LIB  = libgir.a
BIN  = gir

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
TEST_BIN  = testgir

all: options ${BIN} ${TEST_BIN}

options:
	@echo "Toolset Configuration:"
	@echo "  CC       = ${CC}"
	@echo "  CFLAGS   = ${CFLAGS}"
	@echo "  LD       = ${LD}"
	@echo "  LDFLAGS  = ${LDFLAGS}"
	@echo "  AR       = ${AR}"
	@echo "  ARFLAGS  = ${ARFLAGS}"

${LIB}: ${OBJS}
	@echo AR $@
	@${AR} ${ARFLAGS} $@ ${OBJS}

${BIN}: ${LIB}
	@echo LD $@
	@${LD} -o $@ ${LIB} ${LDFLAGS}

${TEST_BIN}: ${TEST_OBJS} ${OBJS}
	@echo LD $@
	@${LD} -o $@ ${TEST_OBJS} ${LIB} ${LDFLAGS}
	@./$@

.c.o:
	@echo CC $<
	@${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f ${BIN} ${TEST_BIN} ${LIB} ${OBJS} ${TEST_OBJS}

.PHONY: all options

