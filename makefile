CC=g++
#Flags for the compiler
CFLAGS=-w -g --std=c++11 -c -I$(INCDIR)/
#Flags for the linker
LDFLAGS=-lSDL2

#Includes Directory
INCDIR=include
#Sources Directory
SRCDIR=src
#Binaries Directory
BINDIR=bin
#Objects Directory
OBJDIR=$(BINDIR)/obj
#Executable name
EXECNAME=main
#Executable path+name
EXECUTABLE=$(BINDIR)/main

#Files inside source directory
SOURCES=$(wildcard $(SRCDIR)/*)
#Substitute suffix in SOURCES for objects
OBJNAMES=$(SOURCES:.cpp=.o)
#Corrected object (path+name)s
OBJECTS=$(subst src,bin/obj,$(OBJNAMES))
#Main object
MAINOBJ=$(OBJDIR)/$(EXECNAME).o

#Link all objects and the main object to generate executable
all: $(OBJECTS) $(MAINOBJ)
	$(CC) $(OBJECTS) $(MAINOBJ) -o $(EXECUTABLE) $(LDFLAGS)

#To get an object file, compile the matching source file inside src/
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(INCDIR)/%.h $(OBJDIR) $(BINDIR)
	$(CC) -o $@ $< $(CFLAGS)

#Or, use the source outside src/
$(OBJDIR)/%.o: %.cpp | $(OBJDIR) $(BINDIR)
	$(CC) -o $@ $< $(CFLAGS)

#Create oject directory
$(OBJDIR): | $(BINDIR)
	mkdir $(OBJDIR)

#Create binary directory
$(BINDIR):
	mkdir $(BINDIR)

#Clean the binaries
clean:
	rm -rf bin/*
