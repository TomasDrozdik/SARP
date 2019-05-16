#Compiler and Linker
CC          := g++

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
INCDIR      := inc
BUILDDIR    := obj
TARGETDIR   := bin
SRCEXT      := cc
MAINEXT     := main.$(SRCEXT)
DEPEXT      := d
OBJEXT      := o

#Flags, Libraries and Includes
C_STD       := c++17
CFLAGS      := -Wall -std=$(C_STD) -pedantic -Wpointer-arith -Wcast-qual -g
LIB         :=
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)

#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -not -name "*.main.$(SRCEXT)" -name "*.$(SRCEXT)")
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))
MAINS		:= $(shell find $(SRCDIR) -type f -name "*.main.$(SRCEXT)")
MAIN_OBJS	:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(MAINS:.$(SRCEXT)=.$(OBJEXT)))

#Defauilt Make
#TODO: do this in some generic way
all: $(TARGETDIR)/static_forwarding $(TARGETDIR)/dv_routing

#Remake
remake: cleaner directories all

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

#Clean only Objects
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#TODO: do this in some generic way
$(TARGETDIR)/static_forwarding: $(OBJECTS) $(MAIN_OBJS)
	$(CC) -o $@ $(OBJECTS) $(LIB) $(BUILDDIR)/simulation/static_forwarding/static_forwarding.main.o

$(TARGETDIR)/dv_routing: $(OBJECTS) $(MAIN_OBJS)
	$(CC) -o $@ $(OBJECTS) $(LIB) $(BUILDDIR)/simulation/distance_vector_routing/dv.main.o

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

#Non-File Targets
.PHONY: all remake clean cleaner resources

