#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>


const char* PORT = "3490";



int listenOnPort(const char* port)
{
	struct addrinfo hints{}; 
	struct addrinfo *result{}; 
	struct addrinfo *p{}; 
	int yes{};
	int sockfd{};
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(NULL, port, &hints, &result); 
	if (status != 0) 
	{
		std::cout << "getaddrinfo:" << gai_strerror(status); 
		return -1;
	}

	for (p = result; p != NULL; p = p->ai_next) 
	{
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol); 
		if (sockfd == -1) 
		{	
			perror("socket:"); 
			continue;
		}

		status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (status == -1) 
		{
			perror("setsockopt:"); 
			break;
		}

		status = bind(sockfd, p->ai_addr, p->ai_addrlen); 
		if (status == -1) 
		{
			perror("bind:"); 
			continue;
		}

		break;
	}

	if (p == NULL) 
	{
		std::cout << "Failed to bind to port" << '\n';
		return -1; 
	}

	status = listen(sockfd, 3); 
	if (status == -1) 
	{
		perror("listen:"); 
		return -1;
	}

	return sockfd;
}

void add_to_pfds(struct pollfd* pollfds, int new_fd, int* size)
{
	assert(*(size) < 2 && "Server is at capacity");

	pollfds[*(size)].fd = new_fd;
	pollfds[*(size)].events = POLL_IN;
	pollfds[*(size)].revents = 0; 

	size += 1;
}

void handle_new_connections(int sockfd, struct pollfd* fds, int* size)
{
	sockaddr_storage remote_addr{};
	socklen_t remote_addr_size = sizeof(remote_addr);

	int new_fd = accept(sockfd,  (struct sockaddr*)&remote_addr,  &remote_addr_size);
	if (new_fd == -1) 
	{
		perror("accept:"); 
	}

	add_to_pfds(fds, new_fd, size);
}



int main()
{
	struct pollfd pollfds[2]; 

	poll(pollfds, 2, 10);
}
