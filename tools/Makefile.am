noinst_PROGRAMS = test

INCLUDES = -I$(top_srcdir)/liblazy

test_SOURCES = test.c
test_LDFLAGS = `pkg-config --libs dbus-1`
test_LDADD = $(top_builddir)/liblazy/liblazy.la 
test_CFLAGS = -Wall -g `pkg-config --cflags dbus-1`
