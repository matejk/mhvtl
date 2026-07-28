/*
 * Run subprocess
 *
 * Copyright (C) 2012 Ivo De Decker ivo.dedecker@ugent.be
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "logging.h"

static volatile sig_atomic_t pid;
static volatile sig_atomic_t timedout;

void alarm_timeout(int sig) {
	(void)sig;
	alarm(0);
	timedout = 1;
	/* pid is cleared before the child is reaped, so this cannot signal a
	 * pid the system has already recycled.
	 */
	if (pid)
		kill((pid_t)pid, SIGKILL);
}

int run_command(char *command, int timeout) {
	pid_t child = fork();

	pid = child;
	if (!child) {
		/* child */
		execlp("/bin/sh", "/bin/sh", "-c", command, (char *)NULL);
		_exit(127); /* execlp only returns on failure */
	} else if (child < 0) {
		/* TODO error handling */
		return -1;
	} else {
		signal(SIGALRM, alarm_timeout);
		timedout = 0;
		alarm(timeout);
		int status;

		while (waitpid(child, &status, 0) <= 0)
			usleep(1);

		alarm(0);
		pid = 0;

		if (WIFEXITED(status)) {
			int res = WEXITSTATUS(status);
			return res;
		} else if (WIFSIGNALED(status)) {
			int sig = WTERMSIG(status);
			MHVTL_DBG(1, "command died with signal: %d "
						 "(timedout: %d)\n",
					  sig, timedout);
			return -sig;
		}
	}

	return -1;
}
