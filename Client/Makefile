CC = g++

CFLAGS  = -O2 -Wall -Wextra -g3 \
          -I./ \
          -I./src

# Directories
BUILDDIR = bin
OBJDIR   = obj
SRCDIR   = src

# Target name
TARGET = $(BUILDDIR)/Client

# Libraries
LIBS = 

# Files and folders
SRCS    = $(wildcard $(SRCDIR)/**/*.cpp $(SRCDIR)/*.cpp)
SRCDIRS = $(shell find . -name '*.cpp' | dirname {} | sort | uniq | sed 's/\/$(SRCDIR)//g' )
OBJS    = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

all: $(TARGET)

# Targets
$(TARGET): $(OBJS)
	mkdir -p "$(BUILDDIR)"
	mkdir -p "$(OBJDIR)"
	$(CC) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p "$(@D)"
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	$(RM) -r $(TARGET) $(BUILDDIR) $(OBJS) $(OBJDIR)/* $(OBJDIR)

