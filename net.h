#include <cstdlib>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct GameState
{
	float positionX{};
	float positionY{};
	float width{};
	float height{};
	float speed{1};
}; 


void* get_in_addr(struct sockaddr* sa);
struct addrinfo* get_client_info(const char* host, const char* port);
int connectToServer(const char* host, const char* port); 
void receive_data(int sockfd, GameState state[2], bool blocking); 
int send_data(int newfd, void * data); 
