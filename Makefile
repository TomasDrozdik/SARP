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
DATADIR		:= data

SRCEXT      := cc
INCEXT      := h
MAINEXT     := main.$(SRCEXT)
OBJEXT      := o

#
# Flags, Libraries and Includes
#
CXXSTD		:= c++2a
CXXFLAGS    := -Wall -std=$(CXXSTD) -pedantic -Wpointer-arith -Wcast-qual
#CXXFLAGS    += -DDEBUG -g
#CXXFLAGS    += -DDUMP
#CXXFLAGS    += -DPRINT
CXXFLAGS    += -DCSV
CXXFLAGS    += -DNDEBUG -Ofast
LIB         :=
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)

#---------------------------------------------------------------------------------
SRC     	:= $(shell find $(SRCDIR) -type f -not -name "*.$(MAINEXT)" -name "*.$(SRCEXT)")
HEADERS		:= $(shell find $(INCDIR) -type f -not -name "*.$(INCEXT)")
OBJS     	:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRC:.$(SRCEXT)=.$(OBJEXT)))
MAINS		:= $(shell find $(SRCDIR) -type f -name "*.main.$(SRCEXT)")
MAIN_OBJS	:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(MAINS:.$(SRCEXT)=.$(OBJEXT)))
#---------------------------------------------------------------------------------

#
# Defauilt Make
#
all: directories  $(TARGETDIR)/distance_vector $(TARGETDIR)/sarp $(TARGETDIR)/sarp_linear $(TARGETDIR)/sarp_square $(TARGETDIR)/sarp_cube $(TARGETDIR)/sarp_readdress_cube  $(TARGETDIR)/sarp_readdress_square $(TARGETDIR)/sarp_update_threshold $(TARGETDIR)/sarp_big_cube

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
	@mkdir -p $(DATADIR)

#
# Clean only Objects
#
clean:
	$(RM) -rf $(BUILDDIR)
	$(RM) -rf $(DATADIR)

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
# Compile targets
#
$(TARGETDIR)/distance_vector: $(OBJS) $(BUILDDIR)/distance_vector.main.o 
	$(CC) $(CXXFLAGS) -o $@ $^

$(TARGETDIR)/sarp: $(OBJS) $(BUILDDIR)/sarp.main.o
	$(CC) $(CXXFLAGS) -o $@ $^

$(TARGETDIR)/sarp_linear: $(OBJS) $(BUILDDIR)/sarp_linear.main.o
	$(CC) $(CXXFLAGS) -o $@ $^

$(TARGETDIR)/sarp_square: $(OBJS) $(BUILDDIR)/sarp_square.main.o
	$(CC) $(CXXFLAGS) -o $@ $^

$(TARGETDIR)/sarp_cube: $(OBJS) $(BUILDDIR)/sarp_cube.main.o
	$(CC) $(CXXFLAGS) -o $@ $^

$(TARGETDIR)/sarp_readdress_square: $(OBJS) $(BUILDDIR)/sarp_readdress_square.main.o
	$(CC) $(CXXFLAGS) -o $@ $^

$(TARGETDIR)/sarp_readdress_cube: $(OBJS) $(BUILDDIR)/sarp_readdress_cube.main.o
	$(CC) $(CXXFLAGS) -o $@ $^

$(TARGETDIR)/sarp_update_threshold: $(OBJS) $(BUILDDIR)/sarp_update_threshold.main.o
	$(CC) $(CXXFLAGS) -o $@ $^

$(TARGETDIR)/sarp_big_cube: $(OBJS) $(BUILDDIR)/sarp_big_cube.main.o
	$(CC) $(CXXFLAGS) -o $@ $^

#
# Compile
#
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CXXFLAGS) $(INC) -c -o $@ $<

#
# Run the binaries
#
LINEARCSV	:= $(DATADIR)/linear100.csv
SQUARECSV	:= $(DATADIR)/square10x10.csv
CUBECSV		:= $(DATADIR)/cube5x5x4.csv
BIGCUBECSV	:= $(DATADIR)/cube10x10x10.csv
UPDATECSV	:= $(DATADIR)/update_threshold.csv
RSQUARESCV	:= $(DATADIR)/readdress_square5x5.csv
RCUBECSV	:= $(DATADIR)/readdress_cube4x4x4.csv

data: $(LINEARCSV) $(SQUARECSV) $(CUBECSV) $(BIGCUBECSV) $(UPDATECSV) $(RSQUARESCV) $(RCUBECSV)

$(LINEARCSV): $(TARGETDIR)/sarp_linear
	./$< > $@

$(SQUARECSV): $(TARGETDIR)/sarp_square
	./$< > $@

$(CUBECSV): $(TARGETDIR)/sarp_cube
	./$< > $@

$(BIGCUBECSV): $(TARGETDIR)/sarp_big_cube
	./$< > $@

$(UPDATECSV): $(TARGETDIR)/sarp_update_threshold
	./$< > $@

$(RSQUARECSV): $(TARGETDIR)/sarp_readdress_square
	./$< > $@

$(RCUBECSV): $(TARGETDIR)/sarp_readdress_cube
	./$< > $@

plot:

plot_grid_comparison: $(LINEARCSV) $(SQUARECSV) $(CUBECSV) $(BIGCUBECSV)
	Rscript plot/grid_comparison.R

plot_update_threshold: $(UPDATECSV)
	Rscripot plot/update_threshold.R

plot_readdress_square: $(RSQUARECSV) $(RCUBECSV)
	Rscripot plot/readdress.R

#
# Non-File Targets
#
.PHONY: all remake clean cleaner resources cstyle fix-cstyle data plot plot_grid_comparison plot_update_threshold plot_readdress_square

