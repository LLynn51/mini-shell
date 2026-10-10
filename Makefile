CC = gcc
CPPFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -Wextra -std=c11 -g
OBJS = main.o parser.o builtin.o executor.o history.o

shell: $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c shell.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f shell $(OBJS)