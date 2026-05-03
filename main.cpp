#include "iostream"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>




int main(int argc, const char* argv[])
{
	if (argc < 2) 
	{
		std::cout << "Expected 'client host' " << '\n';
		return 0;
	}


	
}
