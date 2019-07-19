#include "genl-lib.h"

unsigned char kernel_sended_pre_time; // 0-200
unsigned char kernel_sended_info[MAX_PROBED_NUM][PROBED_INFO];

int genl_get_family_id(int sock_fd, char *family_name)
{
	msgtemplate_t ans;
	int id, rc;
	struct nlattr *na;
	int rep_len;
	rc = genl_send_msg(sock_fd, GENL_ID_CTRL, CTRL_CMD_GETFAMILY, CTRL_ATTR_FAMILY_NAME, (void *)family_name);
	rep_len = recv(sock_fd, &ans, sizeof(ans), 0);
	if (rep_len < 0) {
		return 1;
	}
	if (ans.nlh.nlmsg_type == NLMSG_ERROR || !NLMSG_OK((&ans.nlh), rep_len))
	{
		return 1;
	}
	na = (struct nlattr *) GENLMSG_DATA(&ans);
	na = (struct nlattr *) ((char *) na + NLA_ALIGN(na->nla_len));
	if (na->nla_type == CTRL_ATTR_FAMILY_ID) 
	{
		id = *(__u16 *) NLA_DATA(na);
	} else {
		id = 0;
	}
	return id;
}

int genl_send_msg(int sock_fd, u_int16_t family_id, u_int8_t genl_cmd, u_int16_t nla_type,void *nla_data)
{
	struct nlattr *na;
	struct sockaddr_nl dst_addr;
	int r, buflen;
	char *buf;
	int nla_len = strlen(nla_data)+1;
	msgtemplate_t msg;
	if (family_id == 0) {
		return 0;
	}
	msg.nlh.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
	msg.nlh.nlmsg_type = family_id;
	msg.nlh.nlmsg_flags = NLM_F_REQUEST;
	msg.nlh.nlmsg_seq = 0;
	msg.nlh.nlmsg_pid = getpid();
	msg.gnlh.cmd = genl_cmd;
	msg.gnlh.version = 0x1;
	na = (struct nlattr *) GENLMSG_DATA(&msg);
	na->nla_type = nla_type;
	na->nla_len = nla_len + 1 + NLA_HDRLEN;
	memcpy(NLA_DATA(na), nla_data, nla_len);
	msg.nlh.nlmsg_len += NLMSG_ALIGN(na->nla_len);
	buf = (char *) &msg;
	buflen = msg.nlh.nlmsg_len ;
	memset(&dst_addr, 0, sizeof(dst_addr));
	dst_addr.nl_family = AF_NETLINK;
	dst_addr.nl_pid = 0;
	dst_addr.nl_groups = 0;
	while ((r = sendto(sock_fd, buf, buflen, 0, (struct sockaddr *) &dst_addr, sizeof(dst_addr))) < buflen) 
	{
		if (r > 0) {
			buf += r;
			buflen -= r;
		} else if (errno != EAGAIN) {
			return -1;
		}
	}
	return 0;
}
void genl_rcv_msg(int family_id, int sock_fd)
{
	int ret;
	struct msgtemplate msg;
	struct nlattr *na;
	ret = recv(sock_fd, &msg, sizeof(msg), 0);
	if (ret < 0) {
		printf("error receiving reply message via Netlink length < 0\n");
		return;
	}
	/* Validate response message */
	if (msg.nlh.nlmsg_type == NLMSG_ERROR) { /* error */
		printf("NLMSG_ERROR error received NACK - leaving \n");
		return;
	}
	if (msg.nlh.nlmsg_type == family_id && family_id != 0) 
	{
		na = (struct nlattr *) GENLMSG_DATA(&msg);
		unsigned char * result = (unsigned char *)NLA_DATA(na);
		int i;
		memcpy(&kernel_sended_pre_time, result,sizeof(kernel_sended_pre_time));
		if(kernel_sended_pre_time > 0 && kernel_sended_pre_time <= MAX_PROBED_NUM )
		{
			memcpy(kernel_sended_info,result + sizeof(kernel_sended_pre_time),kernel_sended_pre_time*PROBED_INFO);
		}
	}
}

int genl_socket_init(void)
{
	struct sockaddr_nl saddr;
	int fd;

	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
	if (fd < 0){
		perror("genl_init: socket create error\n");
		return -1;
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.nl_family = AF_NETLINK;
	saddr.nl_groups = 0;
	saddr.nl_pid = getpid();
	if (bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
	{
		perror("bind failed\n");
		close(fd);
		return -1;
	}

	return fd;
}

