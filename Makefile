# compiler to use
CC = clang

# flags to pass compiler
CFLAGS = -ggdb3 -O0 -Qunused-arguments -std=c11 -Wall -Werror

#gddb<level>
#	GNU Debugger (logging level, 3 = verbose)

#O<level>
#	Optimize compilation

#std=c11
#	compile with C11(ie C99) standard

#wall
#	Enables all(ie most) warnings, including macros
#werror
#	Make all warnings into errors.

#https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html


# name for executable
EXE = speller

# space-separated list of header files
HDRS = dictionary.h

# space-separated list of libraries, if any,
# each of which should be prefixed with -l
LIBS =

# space-separated list of source files
SRCS = speller.c dictionary.c

# automatically generated list of object files
OBJS = $(SRCS:.c=.o)


# default target
$(EXE): $(OBJS) $(HDRS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# dependencies
$(OBJS): $(HDRS) Makefile

# housekeeping
clean:
	rm -f core $(EXE) *.o
