# Define the compiler
CXX = g++

# Define the build directory
BUILDDIR = build

# Define any compile-time flags
CXXFLAGS = -Wall -g

# Define any directories containing header files other than /usr/include
# Add the headers/ directory to the include path
INCLUDES = -Iinclude/

# Define library paths in addition to /usr/lib
LFLAGS =

# Define any libraries to link into executable:
LIBS = -linput -ludev -lX11 -lXtst

# Define the source file(s)
# Add source/functions.cpp to the list of source files
SRCS = src/GesturesMonitor.cpp src/functions.cpp

# Define object files based on source files
OBJS = $(SRCS:source/%.cpp=$(BUILDDIR)/%.o)

# Define the executable file name
MAIN = $(BUILDDIR)/gesture_monitor


.PHONY: clean

all: $(MAIN)
	@echo Gesture monitor has been compiled

$(MAIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# Pattern rule for object files
$(BUILDDIR)/%.o: source/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) -r $(BUILDDIR)