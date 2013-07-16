CC=gcc
CFLAGS+=-W -O2 -DNDEBUG -Dlinux
LIBS = -lHL -lHLU -lHDU -lHD -lGL -lGLU -lglut -lrt -lncurses

TARGET=SimpleDeformableSurface
HDRS= \
	Constraint.h \
	draw_string.h \
	DynamicsMath.h \
	HapticDeviceConstraint.h \
	MouseSpringConstraint.h \
	NailConstraint.h \
	OdeSolver.h \
	Particle.h \
	ParticleSystem.h \
	SpringConstraint.h \
	Surface.h \
	UnProjectUtilities.h
SRCS= \
	draw_string.cpp \
	HapticDeviceConstraint.cpp \
	main.cpp \
	MouseSpringConstraint.cpp \
	NailConstraint.cpp \
	OdeSolver.cpp \
	Particle.cpp \
	ParticleSystem.cpp \
	SpringConstraint.cpp \
	Surface.cpp \
	UnProjectUtilities.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
