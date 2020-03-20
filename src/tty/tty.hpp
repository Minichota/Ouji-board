#ifndef TTY_HPP
#define TTY_HPP

#include <string>

void set_command(std::string comm);
std::string read_command();
std::string wait_command();
void tty_loop();
void start_tty();
void stop_tty();

#endif
