CXX=g++
CXXFLAGS+=-W -fexceptions -O2 -DNDEBUG -Dlinux
LIBS+=-lHD -lHDU -lGL -lGLU -lglut -lrt -lncurses

TARGET=SlidingContact
HDRS= \
	Constants.h \
	ContactModel.h
SRCS= \
	ContactModel.cpp \
	helper.cpp \
	main.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
