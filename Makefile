# TODO:
# * add target to convert .dot to .png and display it
# * add production CXXFLAGS
# * use c++ style i.e. CXX instead of CC etc.

#
# Compiler and Linker
#
CC          := g++

#
# The Directories, Source, Includes, Objects, Binary and Resources
#
SRCDIR      := src
INCDIR      := inc
BUILDDIR    := obj
TARGETDIR   := bin
DOCDIR		:= doc

SRCEXT      := cc
INCEXT      := h
MAINEXT     := main.$(SRCEXT)
OBJEXT      := o

#
# Flags, Libraries and Includes
#
CXXSTD		:= c++2a
CXXFLAGS    := -Wall -std=$(CXXSTD) -pedantic -Wpointer-arith -Wcast-qual
CXXFLAGS    += -DDEBUG -g
#CXXFLAGS    += -DNDEBUG -Ofast
LIB         :=
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)

#
# Other sources
#
DOC_CONF		:= .doxyfile

#---------------------------------------------------------------------------------
SRC     	:= $(shell find $(SRCDIR) -type f -not -name "*.$(MAINEXT)" -name "*.$(SRCEXT)")
HEADERS		:= $(shell find $(INCDIR) -type f -not -name "*.$(INCEXT)")
OBJS     	:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRC:.$(SRCEXT)=.$(OBJEXT)))
MAINS		:= $(shell find $(SRCDIR) -type f -name "*.main.$(SRCEXT)")
MAIN_OBJS	:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(MAINS:.$(SRCEXT)=.$(OBJEXT)))

#
# Project specific mains.
#
DISTANCE_VECTOR_MAIN_SRC	:= $(SRCDIR)/distance_vector.$(MAINEXT)
DISTANCE_VECTOR_MAIN_OBJ	:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(DISTANCE_VECTOR_MAIN_SRC:.$(SRCEXT)=.$(OBJEXT)))

SARP_MAIN_SRC				:= $(SRCDIR)/sarp.$(MAINEXT)
SARP_MAIN_OBJ				:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SARP_MAIN_SRC:.$(SRCEXT)=.$(OBJEXT)))

RECORD_MAIN_SRC				:= $(SRCDIR)/record_redundancy.$(MAINEXT)
RECORD_MAIN_OBJ				:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(RECORD_MAIN_SRC:.$(SRCEXT)=.$(OBJEXT)))

#---------------------------------------------------------------------------------

#
# Defauilt Make
#
all: directories  $(TARGETDIR)/distance_vector $(TARGETDIR)/sarp $(TARGETDIR)/record_redundancy

#
# Debug
#
debug: CXXFLAGS += -DDEBUG -g
debug: all

#
# Profile
#
profile: CXXFLAGS += -DDEBUG -g -pg
profile: clean all 

#
# Remake
#
remake: cleaner directories all

#
# Make the Directories
#
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

#
# Clean only Objects
#
clean:
	$(RM) -rf $(BUILDDIR)
	$(RM) -rf $(DOCDIR)

#
# Full Clean, Objects and Binaries
#
cleaner: clean
	$(RM) -rf $(TARGETDIR)

#
# Lint
#
cstyle:
	find $(SRCDIR) $(INCDIR) -name '*.$(SRCEXT)' -o -name '*.$(INCEXT)' | while read fname; do clang-format -style=Google "$$fname" | diff -ud "$$fname" -; done

fix-cstyle:
	find $(SRCDIR) $(INCDIR) -name '*.$(SRCEXT)' -o -name '*.$(INCEXT)' | while read fname; do clang-format -style=Google -i "$$fname" ; done

#
# Program Documentation
#
doc: $(DOC_CONF) $(HEADERS)
	doxygen $<

#
# Compile targets
#
$(TARGETDIR)/distance_vector: $(OBJS) $(DISTANCE_VECTOR_MAIN_OBJ) 
	$(CC) $(CXXFLAGS) -o $@ $^

$(TARGETDIR)/sarp: $(OBJS) $(SARP_MAIN_OBJ) 
	$(CC) $(CXXFLAGS) -o $@ $^

$(TARGETDIR)/record_redundancy: $(OBJS) $(RECORD_MAIN_OBJ)
	$(CC) $(CXXFLAGS) -o $@ $^

#
# Compile
#
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CXXFLAGS) $(INC) -c -o $@ $<

#
# Non-File Targets
#
.PHONY: all remake clean cleaner resources cstyle fix-cstyle doc

