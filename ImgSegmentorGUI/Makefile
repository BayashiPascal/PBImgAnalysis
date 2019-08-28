# Build mode
# 0: development (max safety, no optimisation)
# 1: release (min safety, optimisation)
# 2: fast and furious (no safety, optimisation)
BUILD_MODE=0

all: main

# 0: monolith version, the GBSurface is rendered toward a TGA image
# 1: GTK version, the GBSurface is rendered toward a TGA image or 
#    a GtkWidget
BUILDWITHGRAPHICLIB=1

# Makefile definitions
MAKEFILE_INC=../../PBMake/Makefile.inc
include $(MAKEFILE_INC)

GTKPARAM=`pkg-config --cflags gtk+-3.0` -DBUILDWITHGRAPHICLIB=$(BUILDWITHGRAPHICLIB)
GTKLINK=`pkg-config --libs gtk+-3.0`
CAIROPARAM=`pkg-config --cflags cairo`
CAIROLINK=`pkg-config --libs cairo`
GTK_BUILD_ARG:=$(GTKPARAM) $(CAIROPARAM)
GTK_LINK_ARG:=-lm $(GTKLINK) $(CAIROLINK)

# Root depository
repo=pbimganalysis

# Dependencies of the application
APP_DEP=$($(repo)_EXE_DEP) imgsegmentorgui.o
INC_H_APP=$($(repo)_INC_H_EXE) imgsegmentorgui.h
APP_DIR=$($(repo)_DIR)/ImgSegmentorGUI
APP_INC_DIR=$($(repo)_INC_DIR) $(APP_DIR)
APP_EXE_NAME=main

# Rules to make the executable
$(APP_EXE_NAME): \
		$(APP_EXE_NAME).o \
		$($(repo)_DEP) \
		Makefile \
		$(APP_DEP)
	$(COMPILER) `echo "$(APP_DEP) $($(repo)_EXENAME).o" | tr ' ' '\n' | sort -u` $(LINK_ARG) $($(repo)_LINK_ARG) $(GTK_LINK_ARG) -o $(APP_EXE_NAME) 
	
$(APP_EXE_NAME).o: \
		$(APP_DIR)/$(APP_EXE_NAME).c \
		$(INC_H_APP) \
		$(APP_EXE_DEP) \
		Makefile
	$(COMPILER) $(BUILD_ARG) $($(repo)_BUILD_ARG) `echo "$(APP_INC_DIR)" | tr ' ' '\n' | sort -u` $(GTK_BUILD_ARG) -c $(APP_DIR)/$(APP_EXE_NAME).c
	
imgsegmentorgui.o: \
		$(APP_DIR)/imgsegmentorgui.c \
		$(INC_H_APP) \
		$(APP_EXE_DEP) \
		Makefile
	$(COMPILER) $(BUILD_ARG) $($(repo)_BUILD_ARG) `echo "$(APP_INC_DIR)" | tr ' ' '\n' | sort -u` $(GTK_BUILD_ARG) -c $(APP_DIR)/imgsegmentorgui.c
	
