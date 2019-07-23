#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <json-c/json.h>
#include <string.h>
#include <sys/wait.h>
#include <net/if.h>
#include <stdarg.h>
#include "genl-lib.h"

struct {
	time_t cur_time;
	unsigned char addr[6];
	char signal;
} kernel_sended_mac_list[MAX_PROBED_NUM];

int format_mac_info(void)
{
	int i;
	time_t now;
	time(&now);

	for(i=0;i<kernel_sended_pre_time;i++)
	{
		memcpy(kernel_sended_mac_list[i].addr,kernel_sended_info[i],6);
		memcpy(&(kernel_sended_mac_list[i].signal),kernel_sended_info[i]+6,sizeof(char));

		kernel_sended_mac_list[i].cur_time = now;
	}

	return 0;
}

void send_json_by_mosquitto(void)
{
	printf("kernel_sended_pre_time = %d\n\n",kernel_sended_pre_time);
	if(kernel_sended_pre_time == 0)
	return;

	int i;
	json_object *pData = json_object_new_array();
	json_object *pRespObj = json_object_new_object();
	for(i=0;i<kernel_sended_pre_time;i++)
	{
		json_object *jvalue = json_object_new_object();
		struct tm tm_fr;
		char fr_t[64]={0};
		char pmac[32]={0};
		char signal[16]={0};

		localtime_r(&kernel_sended_mac_list[i].cur_time, &tm_fr);
		strftime(fr_t,sizeof(fr_t),"%Y-%m-%d %H:%M:%S",&tm_fr);
		sprintf(pmac,"%02X:%02X:%02X:%02X:%02X:%02X",
		kernel_sended_mac_list[i].addr[0],kernel_sended_mac_list[i].addr[1],kernel_sended_mac_list[i].addr[2],
		kernel_sended_mac_list[i].addr[3],kernel_sended_mac_list[i].addr[4],kernel_sended_mac_list[i].addr[5]);
		sprintf(signal,"%d",kernel_sended_mac_list[i].signal);

		json_object_object_add(jvalue,"signal",json_object_new_string(signal));
		json_object_object_add(jvalue,"pt",json_object_new_string(fr_t));
		json_object_object_add(jvalue,"mac",json_object_new_string(pmac));

		json_object_array_add(pData,jvalue);
	}
	json_object_object_add(pRespObj,"probes",pData);

	const char *pJsonStr = json_object_get_string(pRespObj);
	printf("------------\n%s\n------------\n\n",pJsonStr);
	char cmdstring[200*64] = {0};
	sprintf(cmdstring, "echo \'%s\' > /tmp/tb-report && /bin/sh /usr/sbin/wifi-report /tmp/tb-report", pJsonStr);
	system(cmdstring);
	return;
}
int main()
{
	memset(kernel_sended_mac_list,0,sizeof(kernel_sended_mac_list));

	int sock_fd;
	sock_fd = genl_socket_init();
	if(sock_fd < 0){
		printf("create_nl_socket create failure\n");
		return 0;
	}

	int family_id = genl_get_family_id(sock_fd, "ProbeMacList");

	for(;;){
		genl_send_msg(sock_fd, family_id, DOC_EXMPL_C_ECHO, DOC_EXMPL_A_MSG, "s");
		genl_rcv_msg(family_id,sock_fd);
		format_mac_info();
		send_json_by_mosquitto();
		sleep(5);
	}
}
