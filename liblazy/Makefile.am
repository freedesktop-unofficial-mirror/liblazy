lib_LTLIBRARIES = liblazy.la

AM_CFLAGS = -std=c99 -Wstrict-prototypes -Wall -fPIC -D_GNU_SOURCE

INCLUDES = 

include_HEADERS = liblazy.h

liblazy_la_SOURCES = liblazy_hal.c liblazy_dbus.c liblazy.c liblazy_local.h
liblazy_la_CFLAGS = $(DBUS_CFLAGS) -Wall
liblazy_la_LDFLAGS = -version-info 1:0:0 $(DBUS_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = lazy.pc
