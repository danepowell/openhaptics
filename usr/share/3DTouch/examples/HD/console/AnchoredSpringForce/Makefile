CC=gcc
CFLAGS+=-W -O2 -DNDEBUG -Dlinux
LIBS = -lHDU -lHD -lrt 

TARGET=AnchoredSpringForce
HDRS=
SRCS=AnchoredSpringForce.c conio.c
OBJS=$(SRCS:.c=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)

