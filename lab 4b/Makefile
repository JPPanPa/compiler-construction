CC      = gcc
CFLAGS  = -Wall -c
LIBS    = -lm

TARGET  = kplc

SRCS    = $(wildcard *.c)
OBJS    = $(SRCS:.c=.o)

#build
all: $(TARGET)

$(TARGET): $(OBJS)
    $(CC) $(OBJS) $(LIBS) -o $@

#biên dịch
%.o: %.c
    $(CC) $(CFLAGS) $< -o $@

#dọn dẹp
clean:
    rm -f $(OBJS) $(TARGET) *~
