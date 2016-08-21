CC = g++ -Wall
SRC = glarea.cc log.cc main.cc window.cc
RSRCDIR = resources
RSRC = $(RSRCDIR)/shaders.gresource.xml
BIN = virtual_camera
PKG = gtkmm-3.0 epoxy

all:
	glib-compile-resources \
		--generate-source \
		--sourcedir=$(RSRCDIR) \
		--target=$(BIN)_gresource.cc \
		$(RSRC)
	$(CC) \
		$(SRC) $(BIN)_gresource.cc \
		-o $(BIN) \
		`pkg-config --cflags --libs $(PKG)`
