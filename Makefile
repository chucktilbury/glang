#SUFFIXES	+=	.d
NODEPS	=	clean real_clean

# BINDIR	=	./bin/
# SRCDIR	=	./src/
# OBJDIR	=	./obj/

TARGET	=	$(BINDIR)glang

SRCS	=	main.c \
			scanner.c \
			memory.c \
			char_buffer.c \
			errors.c \
			parser.c

#SRCS	:=	$(addprefix $(SRCDIR), $(RSRC))

OBJS	:=	$(SRCS:.c=.o)
#OBJS	:=	$(addprefix $(OBJDIR), $(ROBJ))
DEPS	:=	$(SRCS:.c=.d)
#DEPS	:=	$(addprefix $(OBJDIR), $(RDEP))

CC		=	gcc
#CC		=	clang
DEBUG	=	-g -DTRACE
CARGS	=	-Wall -Wextra
LDIRS	= 	-L./
LIBS	=	-lm
INCS	=	-I./

.PHONY: all deps clean real_clean

all: $(TARGET)

$(TARGET): $(OBJS) $(DEPS)
	$(CC) $(DEBUG) $(CARGS) $(LDIRS) $(INCS) -o $@ $(LIBS) $(OBJS)

%.o: %.c %.d $(wildcard *%.h)
	$(CC) $(INCS) $(CARGS) $(DEBUG) -c $< -o $@

%.d: %.c
	$(CC) $(INCS) -MM -MG -MP -MF$@ -MT$@ $<

#$(OBJDIR)%.d: $(SRCDIR)%.c
#	$(CC) $(INCS) -MM -MG -MP -MF$@ -MT$@ $<

ifeq (0, $(words $(findstring $(MAKECMGLOBALA), $(NODEPS))))
-include $(DEPS)
endif

clean:
	-rm -f $(DEPS) $(OBJS)

real_clean: clean
	-rm -f $(TARGET)