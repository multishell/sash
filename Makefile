#
# Makefile for sash
#
# The HAVE_GZIP definition adds the -gzip and -gunzip commands.
# The HAVE_EXT2 definition adds the -chattr and -lsattr comamnds.
#

CFLAGS = -O3 -Wall -Wmissing-prototypes -DHAVE_GZIP -DHAVE_EXT2
LDFLAGS = -static -s
LIBS = -lz


BINDIR = /bin
MANDIR = /usr/man/man1


OBJS = sash.o cmds.o cmd_dd.o cmd_ed.o cmd_grep.o cmd_ls.o cmd_tar.o \
	cmd_gzip.o cmd_find.o cmd_file.o cmd_chattr.o cmd_ar.o utils.o


sash:	$(OBJS)
	$(CC) $(LDFLAGS) -o sash $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) sash

install: sash
	cp sash $(BINDIR)/sash
	cp sash.1 $(MANDIR)/sash.1

$(OBJS):	sash.h
