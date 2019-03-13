/*
 * Copyright (c) 1999 by David I. Bell
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * The "chattr" and "lsattr" built-in commands.
 * These commands are optionally built into sash.
 * They manipulate the important ext2 file system file attribute flags.
 */

#ifdef	HAVE_EXT2

#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/ext2_fs.h>

#include "sash.h"


/*
 * The chattr command.
 * This can turn on or off the immutable and append-only ext2 flags.
 */
void
do_chattr(int argc, const char ** argv)
{
	const char *	fileName;
	const char *	options;
	int *		flagPointer;
	int		offFlags;
	int		onFlags;
	int		oldFlags;
	int		newFlags;
	int		fd;

	argc--;
	argv++;

	/*
	 * Parse the options.
	 */
	onFlags = 0;
	offFlags = 0;

	while ((**argv == '-') || (**argv == '+'))
	{
		options = *argv++;
		argc--;

		/*
		 * Point at the proper flags to be modified.
		 */
		if (*options++ == '+')
			flagPointer = &onFlags;
		else
			flagPointer = &offFlags;

		/*
		 * Parse the flag characters.
		 */
		while (*options)
		{
			switch (*options++)
			{
				case 'i':
					*flagPointer |= EXT2_IMMUTABLE_FL;
					break;

				case 'a':
					*flagPointer |= EXT2_APPEND_FL;
					break;

				default:
					fprintf(stderr, "Unknown flag '%c'\n",
						options[-1]);

					return;
			}
		}
	}

	/*
	 * Make sure that the attributes are reasonable.
	 */
	if ((onFlags == 0) && (offFlags == 0))
	{
		fprintf(stderr, "No attributes specified\n");

		return;
	}

	if ((onFlags & offFlags) != 0)
	{
		fprintf(stderr, "Inconsistent attributes specified\n");

		return;
	}

	/*
	 * Make sure there are some files to affect.
	 */
	if (argc <= 0)
	{
		fprintf(stderr, "No files specified for setting attributes\n");

		return;
	}

	/*
	 * Iterate over all of the file names.
	 */
	while (argc-- > 0)
	{
		fileName = *argv++;

		/*
		 * Open the file name.
		 */
		fd = open(fileName, O_RDONLY | O_NONBLOCK);

		if (fd < 0)
		{
			perror(fileName);

			continue;
		}

		/*
		 * Read the current ext2 flag bits.
		 */
		if (ioctl(fd, EXT2_IOC_GETFLAGS, &oldFlags) < 0)
		{
			perror(fileName);

			(void) close(fd);

			continue;
		}

		/*
		 * Modify the flag bits as specified.
		 */
		newFlags = oldFlags;
		newFlags |= onFlags;
		newFlags &= ~offFlags;

		/*
		 * If the flags aren't being changed, then close this
		 * file and continue.
		 */
		if (newFlags == oldFlags)
		{
			(void) close(fd);

			continue;
		}

		/*
		 * Set the new ext2 flag bits.
		 */
		if (ioctl(fd, EXT2_IOC_SETFLAGS, &newFlags) < 0)
		{
			perror(fileName);

			(void) close(fd);

			continue;
		}

		/*
		 * Close the file.
		 */
		(void) close(fd);
	}
}


/*
 * The lsattr command.
 * This lists the immutable and append-only ext2 flags.
 */
void
do_lsattr(int argc, const char ** argv)
{
	const char *	fileName;
	int		fd;
	int		status;
	int		flags;
	char		string[4];

	argc--;
	argv++;

	/*
	 * Iterate over all of the file names.
	 */
	while (argc-- > 0)
	{
		fileName = *argv++;

		/*
		 * Open the file name.
		 */
		fd = open(fileName, O_RDONLY | O_NONBLOCK);

		if (fd < 0)
		{
			perror(fileName);

			continue;
		}

		/*
		 * Read the ext2 flag bits.
		 */
		status = ioctl(fd, EXT2_IOC_GETFLAGS, &flags);

		/*
		 * Close the file and check the status.
		 */
		(void) close(fd);

		if (status < 0)
		{
			perror(fileName);

			continue;
		}

		/*
		 * Build up the string according to the flags.
		 * This is 'i' for immutable, and 'a' for append only.
		 */
		string[0] = ((flags & EXT2_IMMUTABLE_FL) ? 'i' : '-');
		string[1] = ((flags & EXT2_APPEND_FL) ? 'a' : '-');
		string[2] = '\0';

		/*
		 * Print the flags and the file name.
		 */
		printf("%s  %s\n", string, fileName);
	}
}

#endif


/* END CODE */
