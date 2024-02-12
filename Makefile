# Define the compiler
CXX = g++

# Define any compile-time flags
CXXFLAGS = -Wall -g

# Define any directories containing header files other than /usr/include
INCLUDES =

# Define library paths in addition to /usr/lib
LFLAGS =

# Define any libraries to link into executable:
LIBS = -linput -ludev -lX11 -lXtst

# Define the source file(s)
SRCS = gestures.cpp

# Define the executable file name
MAIN = gesture_monitor

.PHONY: clean

all:    $(MAIN)
	@echo  Gesture monitor has been compiled

$(MAIN): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(MAIN) $(SRCS) $(LFLAGS) $(LIBS)

clean:
	$(RM) *.o *~ $(MAIN)
