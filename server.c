// timeserv.c - a socket-based time of day server

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>

#define PORTNUM 8000
#define error(msg) { perror(msg); exit(1); }

void delay(int ms) {
	clock_t start = clock();
	while (clock() < (start + ms * 1000));
}

int main() {
	struct sockaddr_in server;
	struct sockaddr_in client;
	int	sock_id, sock_fd; // line id, file desc
	FILE *res_file;
	char *tok, *method, *route, *protocol;
	const int req_length = 4096;
	char req_buf[req_length];
	const int res_length = 4096;
	char res_buf[res_length];
	char *header_200 = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
	char *header_404 = "HTTP/1.1 404 NOT FOUND\r\n\r\n";

	const int num_routes = 2;
	char *routes[] = {"/", "/styles.css"};
	char *files[] = {"index.html", "styles.css"};

	do {
		sock_id = socket(AF_INET, SOCK_STREAM, 0);
		if (sock_id == -1) {
			printf("Socket failed, trying again.\n");
			delay(1000);
		}
	} while (sock_id == -1);
	printf("Socket successful!\n");

    server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORTNUM);
	server.sin_family = AF_INET;

	int bind_success = 0;
	do {
		bind_success = bind(sock_id, (struct sockaddr*)&server, sizeof(server));
		if (bind_success != 0) {
			printf("Bind failed, trying again.\n");
			delay(1000);
		}
	} while (bind_success != 0);
	printf("Bind successful!\n");

	if (listen(sock_id, 10) != 0) {
		error("listen");
	}

	socklen_t peer_addr_size = sizeof(client);

	while (1) {
		sock_fd = accept(sock_id, (struct sockaddr*)&client, &peer_addr_size);

		if (sock_fd == -1) {
			error("accept");
		}

		bzero(req_buf, req_length);
		read(sock_fd, req_buf, req_length);

		tok = strtok(req_buf, "\n");
		method = strtok(tok, " ");
		route = strtok(NULL, " ");
		protocol = strtok(NULL, " ");

		printf("method: %s\nroute: %s\nprotocol: %s\n\n", method, route, protocol);

		bzero(res_buf, res_length);
		int route_found = 0;
		for (int i = 0; i < num_routes; i++) {
			if (strcmp(route, routes[i]) == 0) {
				route_found = 1;
				printf("Sending %s\n\n", files[i]);
				write(sock_fd, header_200, strlen(header_200));
				res_file = fopen(files[i], "r");
				fread(res_buf, 1, res_length, res_file);
				write(sock_fd, res_buf, strlen(res_buf));
				break;
			}
		}
		if (!route_found) {
			printf("Sending 404\n\n");
			write(sock_fd, header_404, strlen(header_404));
		}

		close(sock_fd);
	}
}
