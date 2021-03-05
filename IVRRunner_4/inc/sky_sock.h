#pragma once

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

#ifndef __SKY_SOCK_H
#define __SKY_SOCK_H

#include "sky_def.h"

int get_local_ip(char * ip);
int create_sock(int port, int type);
int create_listen_sock(int port);
int create_udp_sock_bindopt(int port,int opt);
int create_udp_sock(int port);
int create_tcp_sock(int port);
int create_sock_withip(char *ip, int port, int type);
int create_tcp_withip(char *ip, int port);
int create_udp_withip(char *ip, int port);
int accept_sock(int sid);
void close_sock(int sid);
int connect_sock(IN int sid,IN char *s_ip,IN int s_port);
int recv_sock(IN int sid,OUT char *rbuff,IN int len);
int recv_sock_ttl(IN int sid,OUT char *rbuff,IN int len,IN int ttl);
int send_sock(IN int sid,IN char *sbuff,IN int len);
int recvfrom_sock(IN int sid,OUT char *rbuff,IN int len,OUT char *sip,OUT int *sport);
int recvfrom_sock_ttl(IN int sid,OUT char *rbuff,IN int len,OUT char *sip,OUT int *sport,IN int tval);
int sendto_sock(IN int sid,IN char *sip,IN int sport,IN char *sbuff,IN int len);
int getsockname_sock(IN int sid,OUT char *ip,IN int iplen, OUT int *port);
int getpeername_sock(IN int sid,OUT char *ip,IN int iplen, OUT int *port);
unsigned long convert_sip_iip(IN char *hostip);

#endif //__SKY_SOCK_H

#ifdef __cplusplus
}
#endif //__cplusplus
