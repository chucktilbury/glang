
TARGET	=	scan_test
SRCS	=	scanner_test.c \
			../../scanner.c \
			../../char_buffer.c \
			../../memory.c \
			../../errors.c

OBJS	=	$(SRCS:.c=.o)
CC		= 	gcc

%.o: %.c
	$(CC) -g -Wall -Wextra -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -g -o $(TARGET) $(OBJS)

clean:
	-rm -f $(TARGET) $(OBJS)
