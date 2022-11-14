# pipe-notification Makefile

GTK_PIDGIN_INCLUDES= `pkg-config --cflags gtk+-2.0 pidgin`

GTK_PREFIX=/usr/local
GTK_PREFIX2=/usr

CFLAGS= -O2 -Wall -fpic -g
LDFLAGS= -shared

INCLUDES = \
      -I$(GTK_PREFIX)/include \
      -I$(GTK_PREFIX)/include/gtk-2.0 \
      -I$(GTK_PREFIX)/include/glib-2.0 \
      -I$(GTK_PREFIX)/include/pango-1.0 \
      -I$(GTK_PREFIX)/include/atk-1.0 \
      -I$(GTK_PREFIX)/lib/glib-2.0/include \
      -I$(GTK_PREFIX)/lib/gtk-2.0/include \
      -I$(GTK_PREFIX2)/include \
      -I$(GTK_PREFIX2)/include/gtk-2.0 \
      -I$(GTK_PREFIX2)/include/glib-2.0 \
      -I$(GTK_PREFIX2)/include/pango-1.0 \
      -I$(GTK_PREFIX2)/include/atk-1.0 \
      -I$(GTK_PREFIX2)/lib/glib-2.0/include \
      -I$(GTK_PREFIX2)/lib/gtk-2.0/include \
      $(GTK_PIDGIN_INCLUDES)

led-notification.so: pipe-notification.c
	gcc pipe-notification.c $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o pipe-notification.so

install: pipe-notification.so
	mkdir -p ~/.purple/plugins/
	cp pipe-notification.so ~/.purple/plugins/

uninstall:
	rm -f ~/.purple/plugins/pipe-notification.so

clean:
	rm -f pipe-notification.so

