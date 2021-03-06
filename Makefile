NAME = mmh

# uncomment to make warnings into errors
# CFLAGS += -Werror

# uncomment to stop complaints about unused functions
# CFLAGS += -Wno-unused-function

CFLAGS += -std=c11
CFLAGS += -Wall -Wextra -Wshadow -Wpedantic
CFLAGS += -DHAVE_CLZ
CFLAGS += -I.
# CFLAGS += -g
CFLAGS += -O

LIBRARY = lib$(NAME).a

all: $(LIBRARY)

C_SRC_LIB = \
	random.c \
	mmh.c \

C_OBJ_LIB = $(C_SRC_LIB:.c=.o)

$(LIBRARY): $(C_OBJ_LIB)
	ar -crs $@ $^

C_SRC_TEST = $(wildcard t/*.c)
C_OBJ_TEST = $(patsubst %.c, %.o, $(C_SRC_TEST))
C_EXE_TEST = $(patsubst %.c, %, $(C_SRC_TEST))

%.o: %.c
	cc $(CFLAGS) -c -o $@ $^

$(C_EXE_TEST): %: %.o $(LIBRARY)
	cc $(CFLAGS) $(LDFLAGS) -o $@ $^ -ltap -lpthread

tests: $(C_EXE_TEST)

test: tests
	@for t in $(C_EXE_TEST); do ./$$t; done

valgrind: tests
	@for t in $(C_EXE_TEST); do valgrind ./$$t; done

clean:
	rm -f *.o
	rm -f $(LIBRARY)
	rm -f $(C_OBJ_TEST) $(C_EXE_TEST)
