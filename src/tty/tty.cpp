#include "tty.hpp"

#include <atomic>
#include <cassert>
#include <csignal>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <thread>
#include <unistd.h>

void set_command(tty_instance* tty, std::string comm)
{
	memcpy(tty->command, comm.c_str(), 1024);
}

std::string read_command(tty_instance* tty)
{
	std::string out_copy = tty->out;
	memset(tty->out, 0, 1024);
	if(!tty->error_out.empty())
	{
		out_copy.append("Errors include:\n");
		out_copy.append(tty->error_out.c_str());
		tty->error_out.clear();
	}
	return out_copy;
}

void tty_loop(tty_instance* tty)
{
	memset(tty->command, 0, 1024);
	memset(tty->out, 0, 1024);

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
		while(tty->active)
		{
			write(pipe_to_sh[1], tty->command, strlen(tty->command));
			write(pipe_to_sh[1], "\n", 1);

			memset(tty->command, 0, 1024);

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
					int len = strlen(tty->out);
					tty->out[len] = c;
					tty->out[len + 1] = '\0';
				}

				// Check for data on the STDERR pipe
				if(FD_ISSET(pipe_to_te_error[0], &rfd))
				{
					read(pipe_to_te_error[0], &c, 1);
					tty->error_out.push_back(c);
				}
			} while(FD_ISSET(pipe_to_te[0], &rfd) ||
					FD_ISSET(pipe_to_te_error[0], &rfd));
		}
		kill(pid, SIGKILL);
	}
	else
	{
		// child
		// Redirect STDIN.
		assert(dup2(pipe_to_sh[0], 0) == 0);

		// Redirect STDOUT.
		assert(dup2(pipe_to_te[1], 1) == 1);

		// Redirect STDERR.
		assert(dup2(pipe_to_te_error[1], 2) == 2);
		execlp("zsh", "/bin/zsh", NULL);
	}
}

tty_instance* create_tty()
{
	tty_instance* tty = new tty_instance;
	*tty->out = 0;
	tty->thread = std::thread(&tty_loop, tty);
	return tty;
}

void stop_tty(tty_instance* tty)
{
	tty->active = false;
	tty->thread.join();
}
