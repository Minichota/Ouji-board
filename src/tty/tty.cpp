#include "tty.hpp"

#include <cassert>
#include <csignal>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

void send_command(std::string command)
{
	int pipe_to_sh[2];
	int pipe_to_te[2];
	int pipe_to_te_error[2];

	// instantiate
	int r1 = pipe(pipe_to_sh);
	int r2 = pipe(pipe_to_te);
	int r3 = pipe(pipe_to_te_error);

	if(r1 < 0 || r2 < 0 || r3 < 0)
	{
		printf("Failed to create pipe");
		exit(-1);
	}

	// process is shared
	int pid = fork();

	if(pid != 0)
	{
		write(pipe_to_sh[1], command.c_str(), command.size());
		write(pipe_to_sh[1], "\n", 1);

		// parent
		char c;
		fd_set rfd;
		fd_set wfd;
		fd_set efd;
		timeval tv;

		// exit
		do
		{
			FD_ZERO(&rfd);
			FD_ZERO(&wfd);
			FD_ZERO(&efd);
			FD_SET(pipe_to_te[0], &rfd);
			FD_SET(pipe_to_te_error[0], &rfd);
			tv.tv_sec = 0;
			tv.tv_usec = 100000;
			select(100, &rfd, &wfd, &efd, &tv);

			if(FD_ISSET(pipe_to_te[0], &rfd))
			{
				read(pipe_to_te[0], &c, 1);
				std::cout << c << std::flush;
			}

			// Check for data on the STDERR pipe; the
			// executed console will write error messages
			// on this pipe (for instance the error message
			// that results from typing an unexisting command.
			// Just for display purposes, the STDERR messages
			// will be shown in upper case.
			if(FD_ISSET(pipe_to_te_error[0], &rfd))
			{
				read(pipe_to_te_error[0], &c, 1);
				std::cout << (char)toupper(c) << std::flush;
			}
		} while(FD_ISSET(pipe_to_te[0], &rfd) ||
				FD_ISSET(pipe_to_te_error[0], &rfd));
		kill(pid, SIGKILL);
	}
	else
	{
		// Redirect STDIN.
		assert(dup2(pipe_to_sh[0], 0) == 0);

		// Redirect STDOUT.
		assert(dup2(pipe_to_te[1], 1) == 1);

		// Redirect STDERR.
		assert(dup2(pipe_to_te_error[1], 2) == 2);
		execlp("zsh", "/bin/zsh", NULL);
	}
}

std::string read_command()
{
	return "";
}
