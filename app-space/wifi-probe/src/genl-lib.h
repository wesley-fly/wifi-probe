#ifndef _GENL_LIB_
#define _GENL_LIB_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <linux/genetlink.h>

enum {
	DOC_EXMPL_A_UNSPEC,
	DOC_EXMPL_A_MSG,
	__DOC_EXMPL_A_MAX,
};
#define DOC_EXMPL_A_MAX (__DOC_EXMPL_A_MAX - 1)
enum {
	DOC_EXMPL_C_UNSPEC,
	DOC_EXMPL_C_ECHO,
	__DOC_EXMPL_C_MAX,
};
#define DOC_EXMPL_C_MAX (__DOC_EXMPL_C_MAX - 1)


#define MAX_MSG_SIZE 1024*2

#define GENLMSG_DATA(glh) ((void *)(NLMSG_DATA(glh) + GENL_HDRLEN))
#define NLA_DATA(na) ((void *)((char *)(na) + NLA_HDRLEN))

typedef struct msgtemplate {
    struct nlmsghdr nlh;
    struct genlmsghdr gnlh;
    char data[MAX_MSG_SIZE];
} msgtemplate_t;


int genl_socket_init(void);
int genl_get_family_id(int sock_fd, char *family_name);
int genl_send_msg(int sock_fd, u_int16_t family_id, u_int8_t genl_cmd, u_int16_t nla_type,void *nla_data);
void genl_rcv_msg(int family_id, int sock_fd);

//From Kernel define
#define MAX_PROBED_NUM 		200 //max saved sta device's mac number
#define PROBED_INFO 			7   //mac addr(6Byte)+ signal(1Byte)

extern unsigned char kernel_sended_pre_time;
extern unsigned char kernel_sended_info[MAX_PROBED_NUM][PROBED_INFO];

#endif
