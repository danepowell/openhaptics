CXX=g++
CXXFLAGS+=-W -fexceptions -g -D_DEBUG -Dlinux
LIBS+=-lHD -lHDU -lrt -lGL -lGLU -lglut -lncurses

TARGET=ParticleWaltz
HDRS= \
	ActorDynamics.h \
	Constants.h \
	DynamicsSimulator.h \
	ForceModel.h
SRCS= \
	ActorDynamics.cpp \
	DynamicsSimulator.cpp \
	ForceModel.cpp \
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
