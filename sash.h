/*
 * Copyright (c) 2002 by David I. Bell
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Definitions for stand-alone shell for system maintainance for Linux.
 */

#ifndef	SASH_H
#define	SASH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <ctype.h>

#if __OpenBSD__
#include <sys/param.h>
#endif

#if __Linux__
#include <malloc.h>
#endif


#define	PATH_LEN	1024
#define	CMD_LEN		10240
#define	ALIAS_ALLOC	20
#define	EXPAND_ALLOC	1024
#define	STDIN		0
#define	STDOUT		1
#define	MAX_SOURCE	10
#define	BUF_SIZE	8192


#define	isBlank(ch)	(((ch) == ' ') || ((ch) == '\t'))
#define	isDecimal(ch)	(((ch) >= '0') && ((ch) <= '9'))
#define	isOctal(ch)	(((ch) >= '0') && ((ch) <= '7'))
#define	isWildCard(ch)	(((ch) == '*') || ((ch) == '?') || ((ch) == '['))

#ifndef MAX
#define MAX(x, y)	((x) > (y) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x, y)	((x) < (y) ? (x) : (y))
#endif

typedef	int	BOOL;

#define	FALSE	((BOOL) 0)
#define	TRUE	((BOOL) 1)


/*
 * Built-in command functions.
 */
extern	void	do_alias(int argc, const char ** argv);
extern	void	do_aliasall(int argc, const char ** argv);
extern	void	do_cd(int argc, const char ** argv);
extern	void	do_exec(int argc, const char ** argv);
extern	void	do_exit(int argc, const char ** argv);
extern	void	do_prompt(int argc, const char ** argv);
extern	void	do_source(int argc, const char ** argv);
extern	void	do_umask(int argc, const char ** argv);
extern	void	do_unalias(int argc, const char ** argv);
extern	void	do_help(int argc, const char ** argv);
extern	void	do_ln(int argc, const char ** argv);
extern	void	do_cp(int argc, const char ** argv);
extern	void	do_mv(int argc, const char ** argv);
extern	void	do_rm(int argc, const char ** argv);
extern	void	do_chmod(int argc, const char ** argv);
extern	void	do_mkdir(int argc, const char ** argv);
extern	void	do_rmdir(int argc, const char ** argv);
extern	void	do_mknod(int argc, const char ** argv);
extern	void	do_chown(int argc, const char ** argv);
extern	void	do_chgrp(int argc, const char ** argv);
extern	void	do_sum(int argc, const char ** argv);
extern	void	do_sync(int argc, const char ** argv);
extern	void	do_printenv(int argc, const char ** argv);
extern	void	do_more(int argc, const char ** argv);
extern	void	do_cmp(int argc, const char ** argv);
extern	void	do_touch(int argc, const char ** argv);
extern	void	do_ls(int argc, const char ** argv);
extern	void	do_dd(int argc, const char ** argv);
extern	void	do_tar(int argc, const char ** argv);
extern	void	do_ar(int argc, const char ** argv);
extern	void	do_mount(int argc, const char ** argv);
extern	void	do_umount(int argc, const char ** argv);
extern	void	do_setenv(int argc, const char ** argv);
extern	void	do_pwd(int argc, const char ** argv);
extern	void	do_echo(int argc, const char ** argv);
extern	void	do_kill(int argc, const char ** argv);
extern	void	do_grep(int argc, const char ** argv);
extern	void	do_file(int argc, const char ** argv);
extern	void	do_find(int argc, const char ** argv);
extern	void	do_ed(int argc, const char ** argv);
extern	void	do_where(int argc, const char ** argv);

#if	HAVE_GZIP
extern	void	do_gzip(int argc, const char ** argv);
extern	void	do_gunzip(int argc, const char ** argv);
#endif

#if	HAVE_LINUX_ATTR
extern	void	do_lsattr(int argc, const char ** argv);
extern	void	do_chattr(int argc, const char ** argv);
#endif


/*
 * Global utility routines.
 */
extern	const char *	modeString(int mode);
extern	const char *	timeString(time_t timeVal);
extern	BOOL		isDirectory(const char * name);
extern	BOOL		isDevice(const char * name);
extern	int		nameSort(const void * p1, const void * p2);
extern	char *		getChunk(int size);
extern	char *		chunkstrdup(const char *);
extern	void		freeChunks(void);
extern	int		fullWrite(int fd, const char * buf, int len);
extern	int		fullRead(int fd, char * buf, int len);
extern	BOOL		match(const char * text, const char * pattern);

extern	const char *	buildName
	(const char * dirName, const char * fileName);

extern	BOOL	makeArgs
	(const char * cmd, int * argcPtr, const char *** argvPtr);

extern	BOOL	copyFile
	(const char * srcName, const char * destName, BOOL setModes);

extern	BOOL	makeString
	(int argc, const char ** argv, char * buf, int bufLen);

extern	int	expandWildCards
	(const char * fileNamePattern, const char *** retFileTable);


/*
 * Global variable to indicate that an SIGINT occurred.
 * This is used to stop processing.
 */
extern	BOOL	intFlag;

#endif

/* END CODE */
