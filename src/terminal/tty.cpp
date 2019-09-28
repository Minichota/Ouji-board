#include <string>
#include <iostream>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <pty.h>

#include "tty.hpp"


TTY::TTY(size_t width, size_t height)
{
	winsize ws = {};
	ws.ws_col = width;
	ws.ws_row = height;
	pid = forkpty(&fd, nullptr, nullptr, &ws);
	if(!pid)
	{
		static char termstr[] = "TERM=xterm";
		putenv(termstr);
		execl(std::getenv("SHELL"), std::getenv("SHELL"), "-i", "-l", nullptr);
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

TTY::~TTY()
{
}

int& TTY::get_fd()
{
	return fd;
}

int& TTY::get_pid()
{
	return pid;
}

void TTY::clear()
{
	kill(pid, SIGTERM);
	close(fd);
	waitpid(pid, nullptr, 0);
}

void TTY::destroy(int signal)
{
	kill(pid, signal);
}

void TTY::send(std::string msg)
{
	write(fd, msg.data(), msg.size());
}

std::pair<std::string,int> TTY::get()
{
	char buffer[4096];
	std::pair<std::string,int> result;
	result.second = read(fd, buffer, sizeof(buffer));
	if(result.second > 0)
	{
		result.first.assign(buffer, buffer+result.second);
	}
	return result;
}
