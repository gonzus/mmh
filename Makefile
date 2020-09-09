first: all

# compile for C2011
CFLAGS += -std=c11

# turn on all possible AND useful, wonderful warnings
CFLAGS += -Wall -Wextra -Wshadow -Wpedantic

# use the builtin CLZ to count bits in a variable
CFLAGS += -DHAVE_CLZ

# choose whether to compile for debugging or for efficiency
# CFLAGS += -g
CFLAGS += -O

# all additional libraries we need
LIBS +=

C_SRC = \
	mmh.c \
	main.c \

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
