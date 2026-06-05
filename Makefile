
######################################
# CONFIG
######################################

# Build directory
DIR_BUILD = .builds

# Output name
TARGET = sPostBuild.exe

# Build optimization
OPTIMIZATION = -Ofast

# Compiler flags
CFLAGS = -Wall -Wextra -std=c++17

# Compiler name
TC_CC = g++.exe

# Makefile name
MAKEFILE = Makefile


######################################
# FILES, INCLUDES AND DEFINES
######################################

CPP_FILES = \
Main.cpp \

INCLUDE_PATHS = \
-I. \

DEFINES = \


######################################
# BUILD
######################################

OBJECTS = $(addprefix $(DIR_BUILD)/,$(notdir $(CPP_FILES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_FILES)))

# Build
all: $(TARGET)

# Compile
$(DIR_BUILD)/%.o: %.cpp | $(MAKEFILE)
	$(TC_CC) $(CFLAGS) $(OPTIMIZATION) $(DEFINES) $(INCLUDE_PATHS) -c $< -o $@

# Link
$(TARGET): $(OBJECTS) | $(DIR_OUTPUT) $(MAKEFILE)
	$(TC_CC) $(OBJECTS) -o $@

# Create build directory
$(DIR_BUILD):
	mkdir $@

# Clean
clean:
	rmdir /s /q .builds
	mkdir .builds