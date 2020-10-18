# Project: cmd_friend
# Author: João Peterson Scheffer
# Date of creation: 12.08.2020
# 
# Usage:
# - Compile lib as debug:                      			- "make build"
# - Compile lib as release:                    			- "make release"
# - Clear dependencies and object files:			   	- "make clear"
# - Clear dependencies, object, .exe, .dll, .a files:	- "make clearall"

# Main program
LIB_NAME = libcmdf

# Compiler definitions :
CC = gcc
CFLAGS = -std=c11 $(HEADERS_PATH)
OFLAGS =
LFLAGS = $(LINK_PATH) $(LIBS)

# Directories :
HEADERS_PATH = -I./
LINK_PATH = -L./
LIB_EXPORT_PATH = ./lib

# Libraries :
LIBS =
LIBS +=

# Tools :
AR_UTILITY = ar
AR_FLAGS = rcs


# END OF USER AREA --------------------------------------------------------------------------------------------

# Recursive wildcard definition :
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) \
  $(filter $(subst *,%,$2),$d))


# Source files to find :
SOURCE_FILES += $(wildcard *.c) # list source files in main dir
OBJECT_FILES := $(SOURCE_FILES:.c=.o) # make object files list to compile later 
DEPENDENCE_FILES := $(OBJECT_FILES:.o=.d) # make dependencie files for each source file, necessary to track modification to header files includeded in each source 
DEP_FLAGS = -MMD -MF $(@:.o=.d) # Arguments to generate dependency files when compiling


# Rules -------------------------------------------------------------------------------------------------------

# Default
all: release

# Rebuild
rebuild: clear build

# Compile as release, overloading $(MAIN_EXE) rule with -Ofast
release: OFLAGS += -Ofast 
release: lib

# Compile as debug (default), overloading $(MAIN_EXE) rule with -g
build: OFLAGS += -g 
build: lib

# Compile lib
lib : $(OBJECT_FILES)
	rm -r $(LIB_EXPORT_PATH)
	mkdir $(LIB_EXPORT_PATH)
	@echo Archiving static library...
	$(AR_UTILITY) $(AR_FLAGS) $(LIB_EXPORT_PATH)/$(LIB_NAME).a $^ 
	@echo Linking shared library...
	$(CC) -shared -o $(LIB_EXPORT_PATH)/$(LIB_NAME).dll $^ 
	@echo Done Building.

# Clear dependencie, libraries, object and executable files
clear : 
	@echo Cleaning...
	rm -f $(SOURCE_FILES:.c=.o)
	rm -f $(SOURCE_FILES:.c=.d)
	@echo Done cleaning.

# Clear dependencie, compressed, documentation, libraries, object and executable files
clearall : $(SOURCE_FILES)
	@echo Cleaning...
	rm -f $(SOURCE_FILES:.c=.o)
	rm -f $(SOURCE_FILES:.c=.d)
	rm -f $(LIB_NAME).a
	rm -f $(LIB_NAME).dll
	rm -f $(TAR_NAME)
	rm -f -r doc/html
	rm -f $(MAIN_EXE).exe
	@echo Done cleaning.

# General rule for compiling source files into object files
%.o : %.c
	@echo Compiling object file... $<
	$(CC) $(OFLAGS) $(CFLAGS) -c $< $(DEP_FLAGS) -o $@ 
-include $(DEPENDENCE_FILES)