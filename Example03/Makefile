# Build mode
# 0: development (max safety, no optimisation)
# 1: release (min safety, optimisation)
# 2: fast and furious (no safety, optimisation)
BUILD_MODE?=1

all: main

# 0: monolith version, the GBSurface is rendered toward a TGA image
# 1: GTK version, the GBSurface is rendered toward a TGA image or 
#    a GtkWidget
BUILDWITHGRAPHICLIB=1

# Makefile definitions
MAKEFILE_INC=../../PBMake/Makefile.inc
include $(MAKEFILE_INC)

# Rules to make the executable
repo=pbimganalysis
main: \
		main.o \
		$($(repo)_EXE_DEP) \
		$($(repo)_DEP)
	$(COMPILER) `echo "$($(repo)_EXE_DEP) $($(repo)_EXENAME).o" | tr ' ' '\n' | sort -u` $(LINK_ARG) $($(repo)_LINK_ARG) $(genbrush_LINK_ARG) -o $($(repo)_EXENAME) 
	
main.o: \
		main.c \
		$($(repo)_INC_H_EXE) \
		$($(repo)_EXE_DEP)
	$(COMPILER) $(BUILD_ARG) $(genbrush_BUILD_ARG) $($(repo)_BUILD_ARG) `echo "$($(repo)_INC_DIR)" | tr ' ' '\n' | sort -u` -c main.c
	
run:
	rm *_checkpoint.json weights.json res.tga; main -nbEpoch 1000 -bestVal 0.99 -train -test -pred ../Data/003/img001.tga res.tga
