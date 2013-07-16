CC=gcc
CFLAGS+=-W -O2 -DNDEBUG -Dlinux -D_cplusplus
LIBS = -lHDU -lHD -lrt 

TARGET=CommandJointTorque
HDRS=conio.h
SRCS=CommandJointTorque.cpp conio.c
OBJS=$(patsubst %.rc,%.res,$(patsubst %.cxx,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(filter %.c %.cc %.cpp %.cxx %.rc,$(SRCS)))))))


.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
