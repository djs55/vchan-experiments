/**
 * @file
 * @section AUTHORS
 *
 * Copyright (C) 2010  Rafal Wojtczuk  <rafal@invisiblethingslab.com>
 *
 *  Authors:
 *       Rafal Wojtczuk  <rafal@invisiblethingslab.com>
 *       Daniel De Graaf <dgdegra@tycho.nsa.gov>
 *
 * @section LICENSE
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * @section DESCRIPTION
 *
 * This is a test program for libxenvchan.  Communications are in one direction,
 * either server (grant offeror) to client or vice versa.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <libxenvchan.h>

void usage(char** argv)
{
	fprintf(stderr, "usage:\n"
		"%s <bufsize> <my domid> <remote domid>\n", argv[0]);
	exit(1);
}

char *buf;
int bufsize;

void reader(struct libxenvchan *ctrl)
{
	uint64_t nbytes = 0;
	int size;
	time_t start;
	time_t end;
	double nsecs;
	for (;;) {
		size = libxenvchan_read(ctrl, buf, bufsize);
		if (nbytes == 0)
			start = time(NULL);
/*		fprintf(stderr, "# %d\n", size); */
		if (size < 0) {
			end = time(NULL);
			nsecs = difftime(end, start);
			fprintf(stderr, "%Ld bytes in %.1f seconds\n", nbytes, nsecs);
			fprintf(stderr, "%d MiB/sec\n", (int) ( (double)(nbytes / (1024 * 1024)) /nsecs ));
			perror("read vchan");
			libxenvchan_close(ctrl);
			exit(1);
		}
		nbytes += size;
	}
}

/**
	Listens for connections and writes everything to /dev/null
*/
int main(int argc, char **argv)
{
	int my_domid = -1;
	int remote_domid = -1;
	char path[128];
	struct libxenvchan *ctrl = 0;
	if (argc < 4)
		usage(argv);

	bufsize = atoi(argv[1]);
	my_domid = atoi(argv[2]);
	buf = (char*)malloc(bufsize);
	if (!buf) {
		perror("malloc");
		exit(1);
	}
	remote_domid = atoi(argv[3]);
	sprintf(path, "/local/domain/%d/data/%d/0", my_domid, my_domid);
	fprintf(stderr, "Path: %s\n", path);

	ctrl = libxenvchan_server_init(NULL, remote_domid, path, 1024, 1024);
	if (!ctrl) {
		perror("libxenvchan_*_init");
		exit(1);
	}
	ctrl->blocking = 1;

	reader(ctrl);
	libxenvchan_close(ctrl);
	return 0;
}
