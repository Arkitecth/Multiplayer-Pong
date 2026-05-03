#include <cstdlib>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

void* get_in_addr(struct sockaddr* sa);
struct addrinfo* get_client_info(const char* host, const char* port);
int connectToServer(const char* host, const char* port); 
struct State 
{
	int positionX{};
	int positionY{};
};
void receive_data(int sockfd, void * buffer); 

int send_data(int newfd, void * data); 
