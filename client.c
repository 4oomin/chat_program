#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>

#define BUFSIZE 1024

void* recv_routine(void* arg)
{
	int clnt_sock = arg;
	char recv_buf[BUFSIZE];
	int recv_len=0;
	while(1)
	{
		recv_len = read(clnt_sock,recv_buf,BUFSIZE);
		if(recv_len == -1)
		{
			printf("[%d]session fail\n",clnt_sock);
			pthread_exit(1);
		}
		printf("%s\n",recv_buf);
	}

	pthread_exit(0);
	return NULL;
}

void* send_routine(void* arg)
{
	int  clnt_sock = arg;
	char temp_buf[BUFSIZE-24];
	char send_buf[BUFSIZE];
	int send_len=0;
	while(1)
	{
		printf("[%d]:",clnt_sock);
		gets(temp_buf);
		sprintf(send_buf,"[%d]:%s\n",clnt_sock,temp_buf);
		send_len = strlen(send_buf)+1;
		if(write(clnt_sock,send_buf,send_len) != send_len)
		{
			printf("[%d]session fail\n",clnt_sock);
			pthread_exit(1);
		}
		sleep(1);
	}
	
	pthread_exit(0);
	return NULL;
}

int main()
{
	/*���� ����*/
	int clnt_sock;
	clnt_sock = socket(PF_NET,SOCK_STREAM,0); // �������� ���� 
	
	/*���� �ּ� �Ҵ�(connect ȣ��� �۽���)*/ 
	struct sockaddr_in clnt_addr;
	clnt_addr.sin_family = AF_NET;  // IPv4 
	clnt_addr.sin_addr.s_addr = htonl("127.0.0.0"); // �۽� ��  IP �ּ� 
	clnt_addr.sin_port = htons(7989);	 // �۽��� port ���� 
	if(connect(clnt_sock,(struct sockaddr *)&clnt_addr,sizeof(clnt_addr)) == -1) //accept �� return 
	{
		perror("connect fail");
		exit(1); 
	}

	printf("[%d] success to connect\n",clnt_sock);
	/*�ۼ��� ������ �и�*/
	pthread_t recv_tid,send_tid;
	pthread_create(&recv_tid,NULL,recv_routine,(void*)clnt_sock);
	pthread_create(&send_tid,NULL,send_routine,(void*)clnt_sock);

	/*main thread ��ٸ�*/
	pthread_join(recv_tid,NULL);   
	pthread_join(send_tid,NULL);
	
	return 0;
}
