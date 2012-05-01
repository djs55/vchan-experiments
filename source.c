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

int libxenvchan_write_all(struct libxenvchan *ctrl, char *buf, int size)
{
	int written = 0;
	int ret;
	while (written < size) {
		ret = libxenvchan_write(ctrl, buf + written, size - written);
		if (ret <= 0) {
			perror("write");
			exit(1);
		}
		written += ret;
	}
	return size;
}

void usage(char** argv)
{
	fprintf(stderr, "usage:\n"
		"%s <bufsize> <remote domid>\n", argv[0]);
	exit(1);
}

char *buf;
int bufsize;

void writer(struct libxenvchan *ctrl)
{
	int size;
	bzero(buf, bufsize);
	for (;;) {
		size = libxenvchan_write_all(ctrl, buf, bufsize);
/*		fprintf(stderr, "# %d\n", BUFSIZE - 1); */
		if (size < 0) {
			perror("vchan write");
			exit(1);
		}
		if (size == 0) {
			perror("write size=0?\n");
			exit(1);
		}
	}
}


/**
	Simple libxenvchan application, both client and server.
	One side does writing, the other side does reading; both from
	standard input/output fds.
*/
int main(int argc, char **argv)
{
	int remote_domid = -1;
	char path[128];
	struct libxenvchan *ctrl = 0;
	int wr = 0;
	if (argc < 3)
		usage(argv);
	bufsize = atoi(argv[1]);
	remote_domid = atoi(argv[2]);

	buf = (char*)malloc(bufsize);
	if (!buf) {
		perror("malloc");
		exit(1);
	}
	sprintf(path, "/local/domain/%d/data/%d/0", remote_domid, remote_domid);
	fprintf(stderr, "remote domid: %d\n", remote_domid);
	fprintf(stderr, "path: %s\n", path);

	ctrl = libxenvchan_client_init(NULL, remote_domid, path);
	if (!ctrl) {
		perror("libxenvchan_*_init");
		exit(1);
	}
	ctrl->blocking = 1;

	writer(ctrl);
	libxenvchan_close(ctrl);
	return 0;
}
