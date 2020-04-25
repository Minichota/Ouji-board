#ifndef TTY_HPP
#define TTY_HPP

#include <string>
#include <thread>

struct tty_instance
{
	std::thread thread;
	bool active = true;
	char* out = (char*)malloc(1024);
	std::string error_out = "";
	char* command = (char*)malloc(1024);
};

void set_command(tty_instance* tty, std::string comm);
std::string read_command(tty_instance* tty);
void tty_loop(tty_instance* instance);
tty_instance* create_tty();
void start_tty(tty_instance* thread);
void stop_tty(tty_instance* thread);

#endif
