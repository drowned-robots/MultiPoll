#include <sys/socket.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <sys/epoll.h> 

#include <locale.h>
#include <unistd.h> 


#include "sql_interaction.h"
#include "log_writing.h"

#include <signal.h>

//больше так не делать.

#define PORT 2154
#define MSG_SIZE 10000
#define MAX_CLIENTS 5000

struct fd_msg{
	int fd;
	char msg[MSG_SIZE];
};
int cur_clientcount=0;
struct fd_msg * clients=NULL;

int communicate(int fd); // read-write client
void signal_handler(int sig);

int main(int argc, char * argv[])
{
    
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        daemon(0, 0);
    }
    
    startlogging();
    //openlog("Multipoll daemon", LOG_PID | LOG_NDELAY | LOG_NOWAIT);
    iferr("Multipoll daemon started", LOG_INFO);
    
	if (signal(SIGINT, signal_handler) == SIG_ERR)
		printf("\ncan't catch SIGINT\n");
	
	setlocale(LC_ALL, "");
	char * logmsg =  malloc(MSG_SIZE*2);
	int sock1, sock2, events_count, epollfd;
	struct epoll_event ev, events[MAX_CLIENTS];
	if((sock1 = socket(AF_INET, SOCK_STREAM, 0))==-1)
		iferr("socket()", LOG_ERR);
	struct sockaddr_in  adr1, adr2;
	adr1.sin_family  = AF_INET;
	adr1.sin_addr.s_addr = htonl(INADDR_ANY);
	adr1.sin_port = htons(PORT);
	if(bind(sock1, (struct sockaddr *)&adr1, sizeof(adr1))==-1)
		iferr("bind", LOG_ERR);
	if(listen(sock1, 5)==-1)
		iferr("listen", LOG_ERR);
	iferr("listening to new connections", LOG_INFO);

	int lenp = sizeof(struct sockaddr_in);
	epollfd = epoll_create(MAX_CLIENTS);
	if(epollfd==-1)
		iferr("epoll_create", LOG_ERR);
	ev.events = EPOLLIN;
	ev.data.fd = sock1;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sock1, &ev)==-1)
		iferr("epoll_ctl", LOG_ERR);

	int i;
	while(1)
	{
		if((events_count = epoll_wait(epollfd, events, MAX_CLIENTS, -1))==-1)
			iferr("epoll_wait", LOG_ERR);
		for(i=0;i< events_count; i++)
		{
			if(events[i].data.fd==sock1)
			{
				sock2 = accept(sock1, (struct sockaddr *)&adr2, &lenp);
				if(sock2==-1)
					iferr("accept", LOG_ERR);
				if(fcntl(sock2, F_SETFL, fcntl(sock2, F_GETFD, 0)|O_NONBLOCK)==-1)
					iferr("fcntl", LOG_ERR);
				// ev.events = EPOLLIN|EPOLLET;
				ev.events = EPOLLIN;
				ev.data.fd = sock2;
				if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sock2, &ev)==-1)
					iferr("epoll_ctl", LOG_ERR);
				cur_clientcount++;	
				sprintf (logmsg, "+ connection: host %s, port %i. connections count:%i\n", inet_ntoa (adr2.sin_addr), ntohs (adr2.sin_port), cur_clientcount);
				iferr(logmsg, LOG_INFO);
				clients = (struct fd_msg *)realloc(clients, sizeof(struct fd_msg)*cur_clientcount);
				clients[cur_clientcount-1].fd= sock2;
				memset(clients[cur_clientcount-1].msg, 0,  sizeof(clients[cur_clientcount-1].msg));
			}
			else
				if(communicate(events[i].data.fd)==0)
				{
					int k=0;
					while(k<cur_clientcount)
					{
						if(clients[k].fd==events[i].data.fd)
						{
							//query

							sprintf(logmsg, "\nMSG\n%s\n", clients[k].msg);
							iferr(logmsg, LOG_INFO);
							char * q = strcasestr(clients[k].msg, "conf=");
							if(q!=NULL)
							{
								char * tosql = malloc(MSG_SIZE);
								memset(tosql, 0, sizeof(tosql));
								if(createquery(q, tosql)!=-1)
								{
									if(sendquery(tosql)==-1)
										iferr("error in sending query\n", LOG_INFO);
									else
										iferr("query is sent\n", LOG_INFO);
								}
								free(tosql); 
							}							
							clients[k].fd = clients[cur_clientcount-1].fd;
							strcpy(clients[k].msg, clients[cur_clientcount-1].msg);
							cur_clientcount--;
							clients= (struct fd_msg *)realloc(clients, sizeof(struct fd_msg)*cur_clientcount);
							break;
						}
						k++;
					}
					close(events[i].data.fd);
					sprintf (logmsg, "- connection. connections count:%i\n", cur_clientcount);
					iferr(logmsg, LOG_INFO);
				}
		}

	}
}

int communicate(int fd)
{
	int k=0;
	while(k<cur_clientcount)
	{
		if(clients[k].fd==fd)
			break;
		k++;
	}
	if(k>=cur_clientcount)
	{
		iferr("FD not found\n", LOG_INFO);
		return -1;
	}
	char msg[513];
	memset(msg, 0,  sizeof(msg));
	int delta_count = recv(fd, msg, 512, 0);
	if(delta_count==0)
		return 0;
	if(delta_count>0)
	{
		if((MSG_SIZE-strlen(clients[k].msg))>=delta_count)
			strcat(clients[k].msg, msg);
		else
		{
			iferr("MSG overflow\n", LOG_INFO);
			return -1;
		}
		char resp[8]="200 OK\n\0";
		send(fd, resp, sizeof(resp), 0);
		return 1;
	}
	return -1;
}


void signal_handler(int sig)
{
	if(sig==SIGINT) {   
		printf("\nSIGINT 1\n");
		iferr("SIGINT", LOG_CRIT);      
	}
}

