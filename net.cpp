#include "net.h"
#include <iostream>

void* get_in_addr(struct sockaddr* sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &((struct sockaddr_in*)sa)->sin_addr; 
	}
		return &((struct sockaddr_in6*)sa)->sin6_addr; 
}


struct addrinfo* get_client_info(const char* host, const char* port)
{
	struct addrinfo hints{}; 
	struct addrinfo *p{};
	struct addrinfo *results{};

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(host, port,  &hints, &results);
	if (status != 0) 
	{	
		std::cout << "getaddrinfo:" << gai_strerror(status) << '\n';
		return nullptr;
	}
	return results;
}

int connectToServer(const char* host, const char* port)
{
	struct addrinfo* results = get_client_info(host, port); 
	if (results == nullptr) {
		std::cout << "Failed to get Client Info" << '\n'; 
		return -1;
	}
	struct addrinfo* p; 
	int sockfd{};
	int status{};
	char server_ip_address[14]{};

	for (p = results; p != nullptr; p = p->ai_next) 
	{
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1) 
		{
			perror("socket:"); 
			continue;
		}
		inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), server_ip_address, sizeof(server_ip_address));

		std::cout << "Attempting to connect to " << server_ip_address << '\n';

		status = connect(sockfd, p->ai_addr, p->ai_addrlen);
		if (status == -1) 
		{
			perror("connect:"); 
			continue;
		}
		break;
	}
	freeaddrinfo(results);
	if (p == nullptr) 
	{
		std::cout << "An error occurred trying to connect to server" << '\n';
		return -1;
	}

	return sockfd;
}
void receive_data(int sockfd, void * buffer)
{
	int num_bytes = recv(sockfd, buffer, 30, 0); 

	if (num_bytes == -1) {
		perror("recv"); 
		close(sockfd); 
		exit(1); 
	}
	if (num_bytes > 0) 
	{
		std::cout << "Get from server: " << reinterpret_cast<char*>(buffer) << '\n';
	}
}

int send_data(int newfd, void * data) 
{
	int status = send(newfd, data, sizeof(data), 0); 
	return status;
}
