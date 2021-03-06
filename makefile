CC = g++
CFLAGS = -Wall
SRC = glarea.cc log.cc main.cc window.cc
RSRCDIR = resources
RSRC = $(RSRCDIR)/shaders.gresource.xml
BIN = virtual_camera
PKG = gtkmm-3.0 epoxy

ifeq ($(OS), Windows_NT)
	CFLAGS += -mwindows
endif

all:
	glib-compile-resources \
		--generate-source \
		--sourcedir=$(RSRCDIR) \
		--target=$(BIN)_gresource.cc \
		$(RSRC)
	$(CC) \
		$(SRC) $(BIN)_gresource.cc \
		-o $(BIN) \
		$(CFLAGS) \
		`pkg-config --cflags --libs $(PKG)`
