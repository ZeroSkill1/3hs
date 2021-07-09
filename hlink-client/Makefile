
OBJS = hlink.o main.o
CFLAGS = -pedantic -Wall

.PHONY: clean all
all: hlink
clean:
	@rm -f $(OBJS) hlink

%.o: %.c
	gcc -c $(<) -o $(@) $(CFLAGS)

hlink: $(OBJS)
	gcc $(^) -o hlink $(CFLAGS)

