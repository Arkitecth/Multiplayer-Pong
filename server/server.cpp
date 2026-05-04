#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/socket.h>

const char* PORT = "3490";

struct GameState
{
	float positionX{};
	float positionY{};
	float width{};
	float height{};
	float speed{1};
}; 

int send_to_client(int sockfd, GameState* state, int size)
{
	int status = send(sockfd, state, size, 0); 
	if (status == -1) 
	{
		perror("send"); 
	}
	return status;
}


const char* inet_ntop2(sockaddr_storage* addr, char* buffer, size_t size)
{
	struct sockaddr_in* sa4{};
	struct sockaddr_in6* sa6{};
	void* src{};

	switch (addr->ss_family) 
	{
		case AF_INET:
		{
			sa4 = (struct sockaddr_in*)addr;
			src = &(sa4->sin_addr);
			break;
		}

		case AF_INET6:
		{

			sa6 = (struct sockaddr_in6*)addr;
			src = &(sa6->sin6_addr);
			break;
		}
		default:
			return NULL;
	}

	return inet_ntop(addr->ss_family,src, buffer, size);
}


int listenOnPort(const char* port)
{
	struct addrinfo hints{}; 
	struct addrinfo *result{}; 
	struct addrinfo *p{}; 
	int yes{1};
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

void add_to_pfds(struct pollfd pollfds[], int new_fd, int* size)
{
	assert(*(size) < 3 && "Server is at capacity");

	pollfds[*(size)].fd = new_fd;
	pollfds[*(size)].events = POLL_IN;
	pollfds[*(size)].revents = 0; 

	*(size) += 1;
}

void handle_new_connections(int sockfd, struct pollfd fds[], int* size)
{
	sockaddr_storage remote_addr{};
	socklen_t remote_addr_size = sizeof(remote_addr);
	char remote_addr_name[INET6_ADDRSTRLEN]{};
	GameState empty_state{};

	GameState player1_initial_state{50, 100, 15, 100};

	GameState player2_initial_state{750, 100, 15, 100};

	GameState waiting_state[2]{player1_initial_state, empty_state}; 

	GameState initial_state[2]{player1_initial_state, player2_initial_state}; 

	int new_fd = accept(sockfd,  (struct sockaddr*)&remote_addr,  &remote_addr_size);
	if (new_fd == -1) 
	{
		perror("accept:"); 
		return; 
	}

	std::cout << "Connected to " << inet_ntop2(&remote_addr, remote_addr_name, 30) << '\n'; 


	add_to_pfds(fds, new_fd, size);
	
	if (*(size) == 2) 
	{
		send_to_client(new_fd, waiting_state, sizeof(waiting_state)); 
	} 
	else if(*(size) == 3)
	{
		send_to_client(new_fd, initial_state, sizeof(initial_state)); 
		send_to_client(fds[1].fd, initial_state, sizeof(initial_state));
	}
}


void handle_client_data(int client_fd, struct pollfd fds[], int* size)
{
	char buf[256]{};
	int num_bytes = recv(client_fd, buf,  sizeof(buf), 0);
	if (num_bytes < 0) 
	{
		std::cout << "Cient refused to talk to me" << '\n';
	}
	else if(num_bytes > 0)
	{
		for(int i = 0; i < *(size); i++)
		{
			if (i == 0 || fds[i].fd == client_fd) 
			{
				continue;
			}
			int status = send(fds[i].fd, buf, num_bytes, 0);
			if(status == -1)
			{
				perror("send"); 
				return;
			}
		}
	}
}

void process_communication(struct pollfd fds[], int* size)
{
	for(int i = 0; i < *size; i++)
	{
		if (fds[i].revents & (POLL_IN | POLL_HUP)) 
		{
			if (i == 0) 
			{
				handle_new_connections(fds[i].fd, fds, size);
			} 
			else 
			{
				handle_client_data(fds[i].fd, fds, size);
			}
		}
	}
}


int main()
{
	struct pollfd pollfds[3]{}; 
	int size{};

	int listenerSocket = listenOnPort(PORT); 
	if (listenerSocket == -1) 
	{
		std::cout << "Error getting socket" << '\n';
		exit(1);
	}


	pollfds[0].events = POLL_IN; 
	pollfds[0].fd = listenerSocket;
	size += 1; 

	std::cout << "Now listening on port: " << PORT << '\n';

	while (true) 
	{
		int poll_count = poll(pollfds, size, -1);

		if (poll_count == -1) 
		{
			perror("pollcount:"); 
			break;
		}

		process_communication(pollfds, &size);
	}

}
