RM  = rm -f
AR  = ar
SED = sed
MV = mv -f

export LD_LIBRARY_PATH=./lib/

PROGRAM   := 09001Color
LIBRARY   :=

SRCDIRS   := .
SRCDIRS   += ./base ./calcPeople ./display ./imageProc ./measure ./patient ./probe ./bodymark ./comment ./imageControl ./keyboard 
SRCDIRS   += ./periDevice ./sysMan ./projectMode

SRCEXTS   := .c .cpp

CPPFLAGS  := -Wall -ggdb3 -DEMP_355
CPPFLAGS  += -D_IPP_PRIVATE_STRUCTURES
CPPFLAGS  += -DREPORTPICS_6 #for add 6 pictures to report, if no define add 2 pictures to report

CFLAGS    :=

CXXFLAGS  := `pkg-config --cflags dbus-glib-1 goocanvas opencv`
CXXFLAGS  += -I. -I./include -I./lib/include

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
