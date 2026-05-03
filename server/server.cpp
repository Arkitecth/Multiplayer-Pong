#include "iostream"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/poll.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

const std::string PORT = "3490";

void* get_in_addr(struct sockaddr* sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &((struct sockaddr_in*)sa)->sin_addr; 
	}
		return &((struct sockaddr_in6*)sa)->sin6_addr; 
}


struct addrinfo* get_server_info(const char* port)
{
	struct addrinfo hints{}; 
	struct addrinfo *p{};
	struct addrinfo *results{};
	int sockfd{};

	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(NULL, PORT.c_str(),  &hints, &results);
	if (status != 0) 
	{	
		std::cout << "getaddrinfo:" << gai_strerror(status) << '\n';
		return nullptr;
	}

	return results;
}

int listenOnPort(const char* port)
{
	struct addrinfo* results = get_server_info(port); 
	struct addrinfo* p{};
	int sockfd{};

	for (p = results; p != NULL; p = p->ai_next) 
	{
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1) 
		{
			perror("socket:"); 
			continue;
		}

		int status = bind(sockfd, p->ai_addr, p->ai_addrlen); 
		if (status == -1) 
		{
			perror("bind:"); 
			continue;
		}
		break;
	}
	freeaddrinfo(results);

	if (p == NULL) 
	{
		std::cout << "Failed to Bind" << '\n';
		return 1; 
	}

	int status = listen(sockfd, 13);
	if (status == -1) {
		perror("listen:"); 
		return status;
	}
	return sockfd;
}


int handleConnection(int sockfd)
{
	struct sockaddr_storage client_addr{};
	socklen_t client_addr_size{};
	char client_ip_address[14]{};

	client_addr_size = sizeof(client_addr); 
	int newfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_size);

	if (newfd != -1) 
	{
	
		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr*)&client_addr),  client_ip_address, sizeof(client_ip_address));

		std::cout << "Found address: " << client_ip_address << '\n';
	}

	return newfd;
}

struct State 
{
	int positionX{};
	int positionY{};
};

void receive_data(int sockfd, void * data)
{
	int num_bytes = recv(sockfd, data, 30, 0); 

	if (num_bytes == -1) {
		perror("recv"); 
		close(sockfd); 
		exit(1); 
	}
	if (num_bytes > 0) 
	{

		std::cout << "Get from server: " << reinterpret_cast<State*>(data)->positionX  << '\n';
	}
}


int send_info(int newfd) 
{
	int status = send(newfd, "Hello From Server\n", 18, 0); 
	return status;
}


int main()
{

	int sockfd = listenOnPort(PORT.c_str());
	State state{};
	if (sockfd == -1) {
		std::cout << "An error while binding to port" << '\n';
		return 1; 
	}
	std::cout << "Listening on Port: " << PORT << '\n';

	while (true) 
	{
		int newfd = handleConnection(sockfd); 
		if (newfd == -1) 
		{
			perror("accept:"); 
			return 1;
		}

		if(send_info(newfd) == -1)
		{
			perror("send:"); 
			close(newfd); 
			return 1;
		}
	}
}
