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

static std::string out;
std::string error_out;
std::atomic<char*> command;
static bool active = true;
std::atomic<bool> done = false;

void set_command(std::string comm)
{
	strcpy(command.load(), comm.c_str());
}

std::string read_command()
{
	std::string out_copy = out;
	out.clear();
	if(!error_out.empty())
	{
		out_copy.append("Errors include:\n");
		out_copy.append(error_out.c_str());
		error_out.clear();
	}
	return out_copy;
}

std::string wait_command()
{
	while(true)
	{
		if(done.load())
		{
			done = false;
			return read_command();
		}
	}
}

void tty_loop()
{
	command = (char*)malloc(1024);

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
		while(active)
		{
			if(*command.load() != '\0')
			{
				write(pipe_to_sh[1], command.load(), strlen(command.load()));
				write(pipe_to_sh[1], "\n", 1);

				memset(command.load(), 0, 1024);

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
						out.push_back(c);
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
						error_out.push_back(c);
					}
				} while(FD_ISSET(pipe_to_te[0], &rfd) ||
						FD_ISSET(pipe_to_te_error[0], &rfd));
				done = true;
			}
		}
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
	free(command);
}

std::thread tty_thread;

void start_tty()
{
	tty_thread = std::thread(&tty_loop);
}

void stop_tty()
{
	active = false;
	tty_thread.join();
}
