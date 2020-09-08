first: all

CFLAGS += -std=c11
CFLAGS += -Wall -Wextra -Wshadow -Wpedantic
# TODO: get rid of these at some point
# CFLAGS += -Wno-unused-function -Wno-unused-parameter
CFLAGS += -g
# CFLAGS += -D_DEFAULT_SOURCE -D_SVID_SOURCE -D_XOPEN_SOURCE -D_GNU_SOURCE
# CFLAGS += -fPIC
# CFLAGS += -DLOG_LEVEL=0
# CFLAGS += -I/usr/include/mariadb

LIBS += -lm

C_SRC = \
	mmh.c \

C_EXE = mmh

C_OBJ = $(C_SRC:.c=.o)

%.o: %.c
	cc -c $(CFLAGS) -o $@ $^

$(C_EXE): $(C_OBJ)
	cc -o $@ $^ $(LIBS)

clean:
	rm -f $(C_OBJ)
	rm -f $(C_EXE)

all: $(C_EXE)
