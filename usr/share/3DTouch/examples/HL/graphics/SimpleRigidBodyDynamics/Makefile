CC=gcc
CFLAGS+=-W -O2 -DNDEBUG -Dlinux
LIBS = -lHL -lHLU -lHDU -lHD -lGL -lGLU -lglut -lrt -lncurses

TARGET=SimpleRigidBodyDynamics
HDRS= \
	Contact.h \
	ContactList.h \
	draw_string.h \
	DynamicsMath.h \
	DynamicsWorld.h \
	DynEdge.h \
	DynFace.h \
	DynFreePlane.h \
	DynPlane.h \
	OdeSolver.h \
	RigidBody.h \
	RigidBodyBox.h \
	RigidBodyWall.h \
	SimpleRigidBodyDynamicsAfx.h \
	TestScenes.h \
	UnProjectUtilities.h \
	Witness.h

SRCS= \
	Contact.cpp \
	draw_string.cpp \
	DynamicsWorld.cpp \
	DynEdge.cpp \
	DynFace.cpp \
	DynFreePlane.cpp \
	main.cpp \
	OdeSolver.cpp \
	RigidBody.cpp \
	RigidBodyBox.cpp \
	RigidBodyWall.cpp \
	SimpleRigidBodyDynamicsAfx.cpp \
	TestScenes.cpp \
	UnProjectUtilities.cpp \
	Witness.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
