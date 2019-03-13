/*
 * Copyright (c) 2002 by David I. Bell
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Most simple built-in commands are here.
 */

#include "sash.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <utime.h>
#include <errno.h>

#if	HAVE_LINUX_MOUNT
#include <linux/fs.h>
#endif


void
do_echo(int argc, const char ** argv)
{
	BOOL	first;

	first = TRUE;

	while (argc-- > 1)
	{
		if (!first)
			fputc(' ', stdout);

		first = FALSE;
		fputs(*(++argv), stdout);
	}

	fputc('\n', stdout);
}


void
do_pwd(int argc, const char ** argv)
{
	char	buf[PATH_LEN];

	if (getcwd(buf, PATH_LEN) == NULL)
	{
		fprintf(stderr, "Cannot get current directory\n");

		return;
	}

	printf("%s\n", buf);
}


void
do_cd(int argc, const char ** argv)
{
	const char *	path;

	if (argc > 1)
		path = argv[1];
	else
	{
		path = getenv("HOME");

		if (path == NULL)
		{
			fprintf(stderr, "No HOME environment variable\n");

			return;
		}
	}

	if (chdir(path) < 0)
		perror(path);
}


void
do_mkdir(int argc, const char ** argv)
{
	while (argc-- > 1)
	{
		if (mkdir(argv[1], 0777) < 0)
			perror(argv[1]);

		argv++;
	}
}


void
do_mknod(int argc, const char ** argv)
{
	const char *	cp;
	int		mode;
	int		major;
	int		minor;

	mode = 0666;

	if (strcmp(argv[2], "b") == 0)
		mode |= S_IFBLK;
	else if (strcmp(argv[2], "c") == 0)
		mode |= S_IFCHR;
	else
	{
		fprintf(stderr, "Bad device type\n");

		return;
	}

	major = 0;
	cp = argv[3];

	while (isDecimal(*cp))
		major = major * 10 + *cp++ - '0';

	if (*cp || (major < 0) || (major > 255))
	{
		fprintf(stderr, "Bad major number\n");

		return;
	}

	minor = 0;
	cp = argv[4];

	while (isDecimal(*cp))
		minor = minor * 10 + *cp++ - '0';

	if (*cp || (minor < 0) || (minor > 255))
	{
		fprintf(stderr, "Bad minor number\n");

		return;
	}

	if (mknod(argv[1], mode, major * 256 + minor) < 0)
		perror(argv[1]);
}


void
do_rmdir(int argc, const char ** argv)
{
	while (argc-- > 1)
	{
		if (rmdir(argv[1]) < 0)
			perror(argv[1]);

		argv++;
	}
}


void
do_sync(int argc, const char ** argv)
{
	sync();
}


void
do_rm(int argc, const char ** argv)
{
	while (argc-- > 1)
	{
		if (unlink(argv[1]) < 0)
			perror(argv[1]);

		argv++;
	}
}


void
do_chmod(int argc, const char ** argv)
{
	const char *	cp;
	int		mode;

	mode = 0;
	cp = argv[1];

	while (isOctal(*cp))
		mode = mode * 8 + (*cp++ - '0');

	if (*cp)
	{
		fprintf(stderr, "Mode must be octal\n");

		return;
	}

	argc--;
	argv++;

	while (argc-- > 1)
	{
		if (chmod(argv[1], mode) < 0)
			perror(argv[1]);

		argv++;
	}
}


void
do_chown(int argc, const char ** argv)
{
	const char *	cp;
	int		uid;
	struct passwd *	pwd;
	struct stat	statBuf;

	cp = argv[1];

	if (isDecimal(*cp))
	{
		uid = 0;

		while (isDecimal(*cp))
			uid = uid * 10 + (*cp++ - '0');

		if (*cp)
		{
			fprintf(stderr, "Bad uid value\n");

			return;
		}
	} else {
		pwd = getpwnam(cp);

		if (pwd == NULL)
		{
			fprintf(stderr, "Unknown user name\n");

			return;
		}

		uid = pwd->pw_uid;
	}

	argc--;
	argv++;

	while (argc-- > 1)
	{
		argv++;

		if ((stat(*argv, &statBuf) < 0) ||
			(chown(*argv, uid, statBuf.st_gid) < 0))
		{
			perror(*argv);
		}
	}
}


void
do_chgrp(int argc, const char ** argv)
{
	const char *	cp;
	int		gid;
	struct group *	grp;
	struct stat	statBuf;

	cp = argv[1];

	if (isDecimal(*cp))
	{
		gid = 0;

		while (isDecimal(*cp))
			gid = gid * 10 + (*cp++ - '0');

		if (*cp)
		{
			fprintf(stderr, "Bad gid value\n");

			return;
		}
	}
	else
	{
		grp = getgrnam(cp);

		if (grp == NULL)
		{
			fprintf(stderr, "Unknown group name\n");

			return;
		}

		gid = grp->gr_gid;
	}

	argc--;
	argv++;

	while (argc-- > 1)
	{
		argv++;

		if ((stat(*argv, &statBuf) < 0) ||
			(chown(*argv, statBuf.st_uid, gid) < 0))
		{
			perror(*argv);
		}
	}
}


void
do_touch(int argc, const char ** argv)
{
	const char *	name;
	int		fd;
	struct utimbuf	now;

	time(&now.actime);
	now.modtime = now.actime;

	while (argc-- > 1)
	{
		name = *(++argv);

		fd = open(name, O_CREAT | O_WRONLY | O_EXCL, 0666);

		if (fd >= 0)
		{
			close(fd);

			continue;
		}

		if (utime(name, &now) < 0)
			perror(name);
	}
}


void
do_mv(int argc, const char ** argv)
{
	const char *	srcName;
	const char *	destName;
	const char *	lastArg;
	BOOL		dirFlag;

	lastArg = argv[argc - 1];

	dirFlag = isDirectory(lastArg);

	if ((argc > 3) && !dirFlag)
	{
		fprintf(stderr, "%s: not a directory\n", lastArg);

		return;
	}

	while (!intFlag && (argc-- > 2))
	{
		srcName = *(++argv);

		if (access(srcName, 0) < 0)
		{
			perror(srcName);

			continue;
		}

		destName = lastArg;

		if (dirFlag)
			destName = buildName(destName, srcName);

		if (rename(srcName, destName) >= 0)
			continue;

		if (errno != EXDEV)
		{
			perror(destName);

			continue;
		}

		if (!copyFile(srcName, destName, TRUE))
			continue;

		if (unlink(srcName) < 0)
			perror(srcName);
	}
}


void
do_ln(int argc, const char ** argv)
{
	const char *	srcName;
	const char *	destName;
	const char *	lastArg;
	BOOL		dirFlag;

	if (argv[1][0] == '-')
	{
		if (strcmp(argv[1], "-s"))
		{
			fprintf(stderr, "Unknown option\n");

			return;
		}

		if (argc != 4)
		{
			fprintf(stderr, "Wrong number of arguments for symbolic link\n");

			return;
		}

#ifdef	S_ISLNK
		if (symlink(argv[2], argv[3]) < 0)
			perror(argv[3]);
#else
		fprintf(stderr, "Symbolic links are not allowed\n");
#endif
		return;
	}

	/*
	 * Here for normal hard links.
	 */
	lastArg = argv[argc - 1];
	dirFlag = isDirectory(lastArg);

	if ((argc > 3) && !dirFlag)
	{
		fprintf(stderr, "%s: not a directory\n", lastArg);

		return;
	}

	while (argc-- > 2)
	{
		srcName = *(++argv);

		if (access(srcName, 0) < 0)
		{
			perror(srcName);

			continue;
		}

		destName = lastArg;

		if (dirFlag)
			destName = buildName(destName, srcName);

		if (link(srcName, destName) < 0)
		{
			perror(destName);

			continue;
		}
	}
}


void
do_cp(int argc, const char ** argv)
{
	const char *	srcName;
	const char *	destName;
	const char *	lastArg;
	BOOL		dirFlag;

	lastArg = argv[argc - 1];

	dirFlag = isDirectory(lastArg);

	if ((argc > 3) && !dirFlag)
	{
		fprintf(stderr, "%s: not a directory\n", lastArg);

		return;
	}

	while (!intFlag && (argc-- > 2))
	{
		srcName = *(++argv);
		destName = lastArg;

		if (dirFlag)
			destName = buildName(destName, srcName);

		(void) copyFile(srcName, destName, FALSE);
	}
}


void
do_mount(int argc, const char ** argv)
{
	const char *	str;
	const char *	type;
	int		flags;

	argc--;
	argv++;

	type = MOUNT_TYPE;

#if	HAVE_LINUX_MOUNT
	flags = MS_MGC_VAL;
#else
	flags = 0;
#endif

	while ((argc > 0) && (**argv == '-'))
	{
		argc--;
		str = *argv++;

		while (*++str) switch (*str)
		{
			case 't':
				if ((argc <= 0) || (**argv == '-'))
				{
					fprintf(stderr, "Missing file system type\n");

					return;
				}

				type = *argv++;
				argc--;
				break;

#if	HAVE_LINUX_MOUNT
			case 'r':
				flags |= MS_RDONLY;
				break;

			case 'm':
				flags |= MS_REMOUNT;
				break;

			case 's':
				flags |= MS_NOSUID;
				break;

			case 'e':
				flags |= MS_NOEXEC;
				break;

#elif	HAVE_BSD_MOUNT
			case 'r':
				flags |= MNT_RDONLY;
				break;

			case 's':
				flags |= MNT_NOSUID;
				break;

			case 'e':
				flags |= MNT_NOEXEC;
				break;
#endif
			default:
				fprintf(stderr, "Unknown option\n");

				return;
		}
	}

	if (argc != 2)
	{
		fprintf(stderr, "Wrong number of arguments for mount\n");

		return;
	}

#if	HAVE_LINUX_MOUNT

 	if (mount(argv[0], argv[1], type, flags, 0) < 0)
 		perror("mount failed");

#elif	HAVE_BSD_MOUNT
	{
		struct	    ufs_args ufs;
		struct	    adosfs_args adosfs;
		struct	    iso_args iso;
		struct	    mfs_args mfs;
		struct	    msdosfs_args msdosfs;
		void *	    args;

		if(!strcmp(type, "ffs") || !strcmp(type, "ufs")) {
			ufs.fspec = (char*) argv[0];
			args = &ufs;
		} else if(!strcmp(type, "adosfs")) {
			adosfs.fspec = (char*) argv[0];
			adosfs.uid = 0;
			adosfs.gid = 0;
			args = &adosfs;
		} else if(!strcmp(type, "cd9660")) {
			iso.fspec = (char*) argv[0];
			args = &iso;
		} else if(!strcmp(type, "mfs")) {
			mfs.fspec = (char*) argv[0];
			args = &mfs;
		} else if(!strcmp(type, "msdos")) {
			msdosfs.fspec = (char*) argv[0];
			msdosfs.uid = 0;
			msdosfs.gid = 0;
			args = &msdosfs;
		} else {
			fprintf(stderr, "Unknown filesystem type: %s", type);
			fprintf(stderr,
			    "Supported: ffs ufs adosfs cd9660 mfs msdos\n");
			return;
		}

		if (mount(type, argv[1], flags, args) < 0)
		        perror(argv[0]);
	}
#endif
}


void
do_umount(int argc, const char ** argv)
{
#if	HAVE_LINUX_MOUNT
	if (umount(argv[1]) < 0)
		perror(argv[1]);
#elif	HAVE_BSD_MOUNT
	{
		const char *	str;
		int		flags = 0;

		for (argc--, argv++;
		    (argc > 0) && (**argv == '-');) {
			argc--;
			str = *argv++;

			while (*++str) {
				switch (*str)
				{
					case 'f':
						flags = MNT_FORCE;
						break;
				}
			}
		}

		if (unmount(argv[0], flags) < 0)
			perror(argv[0]);
	}
#endif
}


void
do_cmp(int argc, const char ** argv)
{
	int		fd1;
	int		fd2;
	int		cc1;
	int		cc2;
	long		pos;
	const char *	bp1;
	const char *	bp2;
	char		buf1[BUF_SIZE];
	char		buf2[BUF_SIZE];
	struct	stat	statBuf1;
	struct	stat	statBuf2;

	if (stat(argv[1], &statBuf1) < 0)
	{
		perror(argv[1]);

		return;
	}

	if (stat(argv[2], &statBuf2) < 0)
	{
		perror(argv[2]);

		return;
	}

	if ((statBuf1.st_dev == statBuf2.st_dev) &&
		(statBuf1.st_ino == statBuf2.st_ino))
	{
		printf("Files are links to each other\n");

		return;
	}

	if (statBuf1.st_size != statBuf2.st_size)
	{
		printf("Files are different sizes\n");

		return;
	}

	fd1 = open(argv[1], O_RDONLY);

	if (fd1 < 0)
	{
		perror(argv[1]);

		return;
	}

	fd2 = open(argv[2], O_RDONLY);

	if (fd2 < 0)
	{
		perror(argv[2]);
		close(fd1);

		return;
	}

	pos = 0;

	while (TRUE)
	{
		if (intFlag)
			goto closefiles;

		cc1 = read(fd1, buf1, sizeof(buf1));

		if (cc1 < 0)
		{
			perror(argv[1]);
			goto closefiles;
		}

		cc2 = read(fd2, buf2, sizeof(buf2));

		if (cc2 < 0)
		{
			perror(argv[2]);
			goto closefiles;
		}

		if ((cc1 == 0) && (cc2 == 0))
		{
			printf("Files are identical\n");
			goto closefiles;
		}

		if (cc1 < cc2)
		{
			printf("First file is shorter than second\n");
			goto closefiles;
		}

		if (cc1 > cc2)
		{
			printf("Second file is shorter than first\n");
			goto closefiles;
		}

		if (memcmp(buf1, buf2, cc1) == 0)
		{
			pos += cc1;

			continue;
		}

		bp1 = buf1;
		bp2 = buf2;

		while (*bp1++ == *bp2++)
			pos++;

		printf("Files differ at byte position %ld\n", pos);

		goto closefiles;
	}

closefiles:
	close(fd1);
	close(fd2);
}


void
do_more(int argc, const char ** argv)
{
	FILE *		fp;
	const char *	name;
	int		ch;
	int		line;
	int		col;
	int		pageLines;
	int		pageColumns;
	char		buf[80];

	/*
	 * Get the width and height of the screen if it is set.
	 * If not, then default it.
	 */
	pageLines = 0;
	pageColumns = 0;

	name = getenv("LINES");

	if (name)
		pageLines = atoi(name);

	name = getenv("COLS");

	if (name)
		pageColumns = atoi(name);

	if (pageLines <= 0)
		pageLines = 24;

	if (pageColumns <= 0)
		pageColumns = 80;

	/*
	 * OK, process each file.
	 */
	while (argc-- > 1)
	{
		name = *(++argv);

		fp = fopen(name, "r");

		if (fp == NULL)
		{
			perror(name);

			return;
		}

		printf("<< %s >>\n", name);
		line = 1;
		col = 0;

		while (fp && ((ch = fgetc(fp)) != EOF))
		{
			switch (ch)
			{
				case '\r':
					col = 0;
					break;

				case '\n':
					line++;
					col = 0;
					break;

				case '\t':
					col = ((col + 1) | 0x07) + 1;
					break;

				case '\b':
					if (col > 0)
						col--;
					break;

				default:
					col++;
			}

			putchar(ch);

			if (col >= pageColumns)
			{
				col -= pageColumns;
				line++;
			}

			if (line < pageLines)
				continue;

			if (col > 0)
				putchar('\n');

			printf("--More--");
			fflush(stdout);

			if (intFlag || (read(0, buf, sizeof(buf)) < 0))
			{
				if (fp)
					fclose(fp);

				return;
			}

			ch = buf[0];

			if (ch == ':')
				ch = buf[1];

			switch (ch)
			{
				case 'N':
				case 'n':
					fclose(fp);
					fp = NULL;
					break;

				case 'Q':
				case 'q':
					fclose(fp);

					return;
			}

			col = 0;
			line = 1;
		}

		if (fp)
			fclose(fp);
	}
}


void
do_sum(int argc, const char ** argv)
{
	const char *	name;
	int		fd;
	int		cc;
	int		ch;
	int		i;
	unsigned long	checksum;
	char		buf[BUF_SIZE];

	argc--;
	argv++;

	while (argc-- > 0)
	{
		name = *argv++;

		fd = open(name, O_RDONLY);

		if (fd < 0)
		{
			perror(name);

			continue;
		}

		checksum = 0;

		while ((cc = read(fd, buf, sizeof(buf))) > 0)
		{
			for (i = 0; i < cc; i++)
			{
				ch = buf[i];

				if ((checksum & 0x01) != 0)
					checksum = (checksum >> 1) + 0x8000;
				else
					checksum = (checksum >> 1);

				checksum = (checksum + ch) & 0xffff;
			}
		}

		if (cc < 0)
		{
			perror(name);

			(void) close(fd);

			continue;
		}

		(void) close(fd);

		printf("%05lu %s\n", checksum, name);
	}
}


void
do_exit(int argc, const char ** argv)
{
	if (getpid() == 1)
	{
		fprintf(stderr, "You are the INIT process!\n");

		return;
	}

	exit(0);
}


void
do_setenv(int argc, const char ** argv)
{
	const char *	name;
	const char *	value;
	char *		str;

	name = argv[1];
	value = argv[2];

	/*
	 * The value given to putenv must remain around, so we must malloc it.
	 * Note: memory is not reclaimed if the same variable is redefined.
	 */
	str = malloc(strlen(name) + strlen(value) + 2);

	if (str == NULL)
	{
		fprintf(stderr, "Cannot allocate memory\n");

		return;
	}

	strcpy(str, name);
	strcat(str, "=");
	strcat(str, value);

	putenv(str);
}


void
do_printenv(int argc, const char ** argv)
{
	const char **	env;
	extern char **	environ;
	int		len;

	env = (const char **) environ;

	if (argc == 1)
	{
		while (*env)
			printf("%s\n", *env++);

		return;
	}

	len = strlen(argv[1]);

	while (*env)
	{
		if ((strlen(*env) > len) && (env[0][len] == '=') &&
			(memcmp(argv[1], *env, len) == 0))
		{
			printf("%s\n", &env[0][len+1]);

			return;
		}
		env++;
	}
}


void
do_umask(int argc, const char ** argv)
{
	const char *	cp;
	int		mask;

	if (argc <= 1)
	{
		mask = umask(0);
		umask(mask);
		printf("%03o\n", mask);

		return;
	}

	mask = 0;
	cp = argv[1];

	while (isOctal(*cp))
		mask = mask * 8 + *cp++ - '0';

	if (*cp || (mask & ~0777))
	{
		fprintf(stderr, "Bad umask value\n");

		return;
	}

	umask(mask);
}


void
do_kill(int argc, const char ** argv)
{
	const char *	cp;
	int		sig;
	int		pid;

	sig = SIGTERM;

	if (argv[1][0] == '-')
	{
		cp = &argv[1][1];

		if (strcmp(cp, "HUP") == 0)
			sig = SIGHUP;
		else if (strcmp(cp, "INT") == 0)
			sig = SIGINT;
		else if (strcmp(cp, "QUIT") == 0)
			sig = SIGQUIT;
		else if (strcmp(cp, "KILL") == 0)
			sig = SIGKILL;
		else if (strcmp(cp, "STOP") == 0)
			sig = SIGSTOP;
		else if (strcmp(cp, "CONT") == 0)
			sig = SIGCONT;
		else if (strcmp(cp, "USR1") == 0)
			sig = SIGUSR1;
		else if (strcmp(cp, "USR2") == 0)
			sig = SIGUSR2;
		else if (strcmp(cp, "TERM") == 0)
			sig = SIGTERM;
		else
		{
			sig = 0;

			while (isDecimal(*cp))
				sig = sig * 10 + *cp++ - '0';

			if (*cp)
			{
				fprintf(stderr, "Unknown signal\n");

				return;
			}
		}

		argc--;
		argv++;
	}

	while (argc-- > 1)
	{
		cp = *(++argv);
		pid = 0;

		while (isDecimal(*cp))
			pid = pid * 10 + *cp++ - '0';

		if (*cp)
		{
			fprintf(stderr, "Non-numeric pid\n");

			return;
		}

		if (kill(pid, sig) < 0)
			perror(*argv);
	}
}


void
do_where(int argc, const char ** argv)
{
	const char *	program;
	const char *	dirName;
	char *		path;
	char *		endPath;
	char *		fullPath;
	BOOL		found;

	found = FALSE;
	program = argv[1];

	if (strchr(program, '/') != NULL)
	{
		fprintf(stderr, "Program name cannot include a path\n");

		return;
	}

	path = getenv("PATH");

	fullPath = getChunk(strlen(path) + strlen(program) + 2);
	path = chunkstrdup(path);

	if ((path == NULL) || (fullPath == NULL))
	{
		fprintf(stderr, "Memory allocation failed\n");

		return;
	}

	/*
	 * Check out each path to see if the program exists and is
	 * executable in that path.
	 */
	for (; path; path = endPath)
	{
		/*
		 * Find the end of the next path and NULL terminate
		 * it if necessary.
		 */
		endPath = strchr(path, ':');

		if (endPath)
			*endPath++ = '\0';

		/*
		 * Get the directory name, defaulting it to DOT if
		 * it is null.
		 */
		dirName = path;

		if (dirName == '\0')
			dirName = ".";

		/*
		 * Construct the full path of the program.
		 */
		strcpy(fullPath, dirName);
		strcat(fullPath, "/");
		strcat(fullPath, program);

		/*
		 * See if the program exists and is executable.
		 */
		if (access(fullPath, X_OK) < 0)
		{
			if (errno != ENOENT)
				printf("%s: %s\n", fullPath, strerror(errno));

			continue;
		}

		printf("%s\n", fullPath);
		found = TRUE;
	}

	if (!found)
		printf("Program \"%s\" not found in PATH\n", program);
}

/* END CODE */
