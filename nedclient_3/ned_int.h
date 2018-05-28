#ifndef __NED_INT_H__
#define __NED_INT_H__

#include "ned_client.h"

/* ned_cif.c */
void sig_pipe(int dummy);

/* ned_sk.c */
int connect_server(char *machine, char *service, int port);
int disconnect_server();
int send_cmd(char *cmd);
int ned_gets(char *string);

int  connectSock(char *machine, char *service, int port);
int disconnectSock();
int send_cmdSock(int sock, char *cmd);
int read_inbuf(int sock);
int get_stringSock(int sock, char *string);

/* ned_cli.c */
int ned_query(int cmd_code, ...);
int ned_getint(int *n);
int ned_getdouble(double *d);
int ned_qst(int *st);
int ned_qno(int *no);
int ned_objects(ObjInfo **objp, int no_obj);
int clean_obj_socket(int no_obj);
int radius_out_range(double radius);

#endif
