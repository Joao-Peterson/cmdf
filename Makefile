# ---------------------------------------------------------------
# https://www.rapidtables.com/code/linux/gcc/gcc-l.html <- how to link libs
# 
# Commands:
# 	build 		: build lib objects and test files 
# 	release 	: build lib objects, archive and organize the lib files for use in the 'dist/' folder
# 	dist 		: dist just organizes the lib files for use in the 'dist/' folder
# 	clear 		: clear compiled executables
# 	clearall 	: clear compiled objects and lib files in 'build/' and 'dist/' folders as well as executables
# 	install  	: installs bianries, includes and libs to the specified "INSTALL_" path variables

CC := gcc

C_FLAGS :=

I_FLAGS :=
I_FLAGS += -Iinc

L_FLAGS :=

TEST_EXE:= main.exe
TEST_SOURCE := main.c

SOURCES := src/cmdf.c 
SOURCES +=

HEADERS := inc/cmdf.h 
HEADERS +=

LIB_NAME := libcmdf.a

DIST_DIR := dist/
BUILD_DIR := build/

ARCHIVER := ar -rcs

INSTALL_BIN_DIR := /usr/local/bin
INSTALL_LIB_DIR := /usr/local/lib
INSTALL_INC_DIR := /usr/local/include

# ---------------------------------------------------------------

# rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) \
#   $(filter $(subst *,%,$2),$d))

OBJS := $(SOURCES:.c=.o)
OBJS_BUILD := $(addprefix $(BUILD_DIR), $(OBJS))
TEST_OBJ := $(BUILD_DIR)$(TEST_SOURCE:.c=.o)

# MAKEFLAGS += --jobs=$(shell nproc)
# MAKEFLAGS += --output-sync=target

# ---------------------------------------------------------------

.PHONY : build

build : C_FLAGS += -g
build : $(HEADERS)
build : $(TEST_OBJ) $(TEST_EXE)
build : $(OBJS_BUILD) 

release : C_FLAGS += -O2
release : $(HEADERS)
release : clearall $(OBJS_BUILD) dist

$(BUILD_DIR)%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) $(I_FLAGS) -c $< -o $@


dist : $(OBJS_BUILD)
	@mkdir -p $(DIST_DIR)
	$(ARCHIVER) $(DIST_DIR)$(LIB_NAME) $^
	cp $(HEADERS) $(DIST_DIR)


$(TEST_EXE): $(OBJS_BUILD) $(TEST_OBJ)
	$(CC) $^ -o $@

install :
	cp -r dist/*.h $(INSTALL_INC_DIR)/
	cp -r dist/*.a $(INSTALL_LIB_DIR)/

clear : 
	rm -f $(EXE)

clearall : clear
	rm -f -r $(BUILD_DIR)*
	rm -f -r $(DIST_DIR)*
