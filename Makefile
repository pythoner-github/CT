###############################################################################
#
# Generic Makefile for C/C++ Program
#
# Author: whyglinux (whyglinux AT hotmail DOT com)
# Date:   2006/03/04

# Description:
# The makefile searches in <SRCDIRS> directories for the source files
# with extensions specified in <SOURCE_EXT>, then compiles the sources
# and finally produces the <PROGRAM>, the executable file, by linking
# the objectives.

# Usage:
#   $ make           compile and link the program.
#   $ make objs      compile only (no linking. Rarely used).
#   $ make clean     clean the objectives and dependencies.
#   $ make cleanall  clean the objectives, dependencies and executable.
#   $ make rebuild   rebuild the program. The same as make clean && make all.
#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================
# The command used to delete file.
RM  = rm -f
AR  = ar
SED = sed
MV	= mv -f

export LD_LIBRARY_PATH=./lib/
# The executable file name.
# It must be specified.
# PROGRAM   := a.out    # the executable name
PROGRAM   := 09001Color
LIBRARY   := 

# The directories in which source files reside.
# At least one path should be specified.
# SRCDIRS   := .        # current directory
SRCDIRS   := . 
SRCDIRS   += ./base ./calcPeople ./display ./imageProc ./measure ./patient ./probe ./bodymark ./comment ./imageControl ./keyboard 
SRCDIRS   += ./periDevice ./sysMan ./projectMode

# The source file types (headers excluded).
# At least one type should be specified.
# The valid suffixes are among of .c, .C, .cc, .cpp, .CPP, .c++, .cp, or .cxx.
# SRCEXTS   := .c      # C program
# SRCEXTS   := .cpp    # C++ program
# SRCEXTS   := .c .cpp # C/C++ program
SRCEXTS   := .c .cpp

# The flags used by the cpp (man cpp for more).
# CPPFLAGS  := -Wall -Werror # show all warnings and take them as errors
#CPPFLAGS  := -Wall -O2 -I./include 
#CPPFLAGS  := -Wall -ggdb3 -DEMP_161
#CPPFLAGS  := -Wall -ggdb3 -DEMP_340
#CPPFLAGS  := -Wall -ggdb3 -DEMP_340 -DEMP3D 
#CPPFLAGS  := -Wall -ggdb3 -DEMP_430    #A60
#CPPFLAGS  := -Wall -ggdb3 -DEMP3D -DEMP_430
#CPPFLAGS  := -Wall -ggdb3 -DEMP_322
#CPPFLAGS  := -Wall -ggdb3 -DEMP_322 -DEMP3D
#CPPFLAGS  := -Wall -ggdb3 -DEMP_360 -DEMP3D 
#CPPFLAGS  := -Wall -ggdb3 -DEMP_360 
#CPPFLAGS  := -Wall -ggdb3 -DEMP_460  #G40
CPPFLAGS  := -Wall -ggdb3 -DEMP_355  #G30plus
#CPPFLAGS  := -Wall -ggdb3 -DEMP_3410 -DEMP3D #民营软件
#CPPFLAGS  := -Wall -ggdb3 -DEMP_440    #3000外观配A60探头
#CPPFLAGS  := -Wall -ggdb3 -DDEBUG -DEMP_340#-DDEPRECATED_CUPS
#CPPFLAGS  += -fno-stack-protector
#old keyboard 
#CPPFLAGS  += -DK24C
#CPPFLAGS += -DEMP_PROJECT    #工程模式 
#CPPFLAGS += -DTRANSDUCER  #for transducer  吸引器 
#CPPFLAGS += -DVET  #for human or vet: VET-for vet
CPPFLAGS  += -D_IPP_PRIVATE_STRUCTURES  #-O3 -fno-aggressive-loop-optimizations
CPPFLAGS  += -DREPORTPICS_6 #for add 6 pictures to report, if no define add 2 pictures to report

# The compiling flags used only for C.
# If it is a C++ program, no need to set these flags.
# If it is a C and C++ merging program, set these flags for the C parts.
#CFLAGS    := -Wall -O2 -I./include 
CFLAGS    := 

# The compiling flags used only for C++.
# If it is a C program, no need to set these flags.
# If it is a C and C++ merging program, set these flags for the C++ parts.
CXXFLAGS  := `pkg-config --cflags dbus-glib-1 goocanvas opencv` 
CXXFLAGS  += -I./include -I./lib/include -I./lib/include/opencv/include
CXXFLAGS  += -I./base -I./calcPeople -I./display -I./imageProc -I./measure -I./patient -I./probe -I./bodymark -I./comment -I./imageControl 
CXXFLAGS  += -I./keyboard -I./periDevice -I./sysMan -I./projectMode -I./

# The library and the link options ( C and C++ common).
LDFLAGS   := `pkg-config --libs goocanvas dbus-glib-1 gthread-2.0 opencv`
LDFLAGS   += -lpthread -lm
LDFLAGS   += -lX11 -lXtst -lXext -lXft
LDFLAGS	  += -ldicom -lImageFile -ljpeg 
LDFLAGS   += -ldcmdata -ldcmdsig -ldcmimage -ldcmimgle -ldcmjpeg -ldcmnet -ldcmpstat 
LDFLAGS   += -ldcmqrdb -ldcmsr -ldcmtls -ldcmwlm -lijg12 -lijg16 -lijg8 -lofstd 
LDFLAGS   += -ltiff -lxml2 -lssl -lcrypto -lwrap -lpng 
LDFLAGS   += -lDSC -lEmpRegister -lDSP 
LDFLAGS   += -lippi -lipps -lippm -limf -lirc
LDFLAGS   += -lsqlite3 -lcups
LDFLAGS   += -lUIS4DReceiveInterface
LDFLAGS   += -lIniFile
LDFLAGS   += -lIMT -lhasp_linux_106877
LDFLAGS   += -L./lib -L./lib/opencv -L./lib/dcm -L/usr/local/lib -L/usr/local/ssl/lib  
LDFLAGS   += -lx264 -lavutil -lswscale -lavformat -lavcodec
#LDFLAGS   += -lFilter -lImageMix -lColormap -lIniFile
#LDFLAGS   += -lml -lcvaux -lhighgui -lcv -lcxcore
LDFLAGS   += -lcgos -lhal

# The object file for 
EXOBJS    := 

## Implict Section: change the following only when necessary.
##=============================================================================
# The C program compiler. Uncomment it to specify yours explicitly.
CC      = gcc# -Wl, --hash -style=sysv

# The C++ program compiler. Uncomment it to specify yours explicitly.
CXX     = g++ #-Wl, --hash -style=sysv

# Uncomment the 2 lines to compile C programs as C++ ones.
#CC      = $(CXX)
#CFLAGS  = $(CXXFLAGS)

## Stable Section: usually no need to be changed. But you can add more.
##=============================================================================
SHELL   = /bin/sh
SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
OBJS    = $(foreach x,$(SRCEXTS), \
	  $(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES))))
DEPS    = $(patsubst %.o,%.d,$(OBJS))

.PHONY : all objs clean cleanall rebuild po mo

all : $(PROGRAM) $(LIBRARY)
#all : $(LIBRARY)

# Rules for creating the dependency files (.d).
#---------------------------------------------------
%.d : %.c
	@$(CC) -MM -MD $(CFLAGS) $< -o $@
	@$(SED) 's,$(notdir $*)\.o,$*\.o,' $@ > $@.tmp
	@$(MV) $@.tmp $@

%.d : %.C
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@
	@$(SED) 's,$(notdir $*)\.o,$*\.o,' $@ > $@.tmp
	@$(MV) $@.tmp $@

%.d : %.cc
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@
	@$(SED) 's,$(notdir $*)\.o,$*\.o,' $@ > $@.tmp
	@$(MV) $@.tmp $@

%.d : %.cpp
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@ 
	@$(SED) 's,$(notdir $*)\.o,$*\.o,' $@ > $@.tmp
	@$(MV) $@.tmp $@

%.d : %.CPP
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@
	@$(SED) 's,$(notdir $*)\.o,$*\.o,' $@ > $@.tmp
	@$(MV) $@.tmp $@

%.d : %.c++
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@
	@$(SED) 's,$(notdir $*)\.o,$*\.o,' $@ > $@.tmp
	@$(MV) $@.tmp $@

%.d : %.cp
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@
	@$(SED) 's,$(notdir $*)\.o,$*\.o,' $@ > $@.tmp
	@$(MV) $@.tmp $@

%.d : %.cxx
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@
	@$(SED) 's,$(notdir $*)\.o,$*\.o,' $@ > $@.tmp
	@$(MV) $@.tmp $@

# Rules for producing the objects.
#---------------------------------------------------
objs : $(OBJS)

%.o : %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

%.o : %.C
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.CPP
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.c++
	$(CXX -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.cp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(LIBRARY): $(OBJS)
	$(AR) -r $(LIBRARY) $(OBJS)

# Rules for producing the executable.
#----------------------------------------------

$(PROGRAM): $(OBJS) 
ifeq ($(strip $(SRCEXTS)), .c)  # C file
	$(CC) -o $(PROGRAM) $(OBJS) $(EXOBJS) $(LDFLAGS)
else                            # C++ file
	$(CXX) -o $(PROGRAM) $(OBJS) $(EXOBJS) $(LDFLAGS)
endif

-include $(DEPS)

rebuild: clean all

clean :
	$(RM) $(OBJS) $(DEPS) $(PROGRAM)

cleanall: clean
	$(RM) $(PROGRAM) $(PROGRAM).exe

po :
	./update_po.sh

mo :
	msgfmt -o ./res/i18n/09001_lang.mo ./res/i18n/09001_lang.po
	msgfmt -o ./res/i18n/09001_ru.mo ./res/i18n/09001_russian.po
	msgfmt -o ./res/i18n/09001_pl.mo ./res/i18n/09001_polish.po
	msgfmt -o ./res/i18n/09001_fr.mo ./res/i18n/09001_frence.po
	msgfmt -o ./res/i18n/09001_de.mo ./res/i18n/09001_de.po

make:
	make -j2

### End of the Makefile ##  Suggestions are welcome  ## All rights reserved ###
###############################################################################
