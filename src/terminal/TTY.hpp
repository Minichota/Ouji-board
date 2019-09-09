#ifndef TTY_HPP
#define TTY_HPP
#include <cstdlib>
#include <string>

class TTY
{
	public:
	TTY(size_t width, size_t height);
	~TTY();

	void send(std::string buffer);
	std::pair<std::string,int> get();
	void resize(size_t width, size_t height);

	private:
	int fd, pid;
};
#endif
