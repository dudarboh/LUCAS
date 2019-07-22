#
# LumiCalSimulator
name := Lucas
G4TARGET := $(name)
G4EXLIB := true

ifndef G4INSTALL
	@echo G4INSTALL not defined !
endif

include $(G4INSTALL)/config/architecture.gmk

EXTRALIBS += $(shell root-config --libs)
EXTRALIBS += $(shell root-config --glibs)
CPPFLAGS  += $(shell root-config --cflags)

ifdef G4UI_USE_QT
EXTRALIBS += -L$(QTDIR)/lib -lQtCore
endif

ifndef G4WORKDIR
	@echo "G4WORKDIR not defined !"
	@echo "G4WORKDIR set to "`pwd`
	G4WORKDIR := `pwd`
endif

# dictionary settings
Dictname := $(name)Dict
Libname	 := lib$(Dictname)
DICTPATH := ${G4WORKDIR}/tmp/${G4SYSTEM}/$(name)
DICTHEAD := Track_t.hh
DICTHEAD += Hit_t.hh
DICTHEAD += LinkDef.h

.PHONY: all
all: dictionary lib bin

dictionary:
ifndef ROOTSYS
	@echo ROOTSYS not defined!
endif
	@echo " Building dictionary "$(Libname)
	@echo " using headers "$(DICTHEAD)
	mkdir -p ${DICTPATH} ; 
	${ROOTSYS}/bin/rootcint -f ${G4WORKDIR}/src/$(Dictname).cc \
	-c -I${G4WORKDIR}/include ${DICTHEAD}
	mv ${G4WORKDIR}/src/${Dictname}.h ${G4WORKDIR}/include;
	${CXX} -o $(DICTPATH)/$(Libname).so -fpic -shared  \
	${G4WORKDIR}/src/$(Dictname).cc                        \
	$(CPPFLAGS) $(EXTRALIBS)
	mkdir -p ${G4WORKDIR}/lib/${G4SYSTEM}
	ln -sf $(DICTPATH)/$(Libname).so ${G4WORKDIR}/lib/${G4SYSTEM}
	@echo " ..... done"

include $(G4INSTALL)/config/binmake.gmk
