#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>

#define CLIENT_MAX 5
#define BUFSIZE 1024

int g_clnt_accept[CLIENT_MAX];
int g_clnt_count=0;
pthread_mutex_t g_mutex;

void* clnt_routine(void* arg)
{	
	int clnt_sock = (int)arg;
	int recv_buf[BUFSIZE];
	int recv_len=0;
	
	while(1)
	{
		/*client 메시지 수신*/
		recv_len = read(clnt_sock,recv_buf,BUFSIZE);
		if(recv_len == -1)  // 챗팅방 퇴장 
		{
			printf("[%d]recv fail\n",clnt_sock);
			pthread_mutex_lock(&g_mutex);
			int idx = -1;
			for(int i =0 ; i<g_clnt_count;i++)
			{
				if(g_clnt_accept[i]==clnt_sock) idx = i;
				if((i>=idx) && (idx>-1)) g_clnt_accept[i] = g_clnt_accept[i+1];
			}
			g_clnt_count -= 1;
			pthread_mutex_unlock(&g_mutex);
			pthread_exit(1);
		}
		/*다른 client에 전달*/
		pthread_mutex_lock(&g_mutex);
		for(int i = 0;i<g_clnt_count;i++)
		{
			if(g_clnt_accept[i]==clnt_sock) continue;
			if(write(g_clnt_accept[i],recv_buf,recv_len) != recv_len)
			{
				printf("[%d]send fail\n",g_clnt_accept[i]);	
			}  
		}
		pthread_mutex_unlock(&g_mutex);
		/*전달 메시지 확인*/
		printf("[%d]%s\n",recv_buf);		
	}
	
	pthread_exit(0);
	return NULL
}

int main()
{
	/*접속 소켓 생성*/
	int serv_sock;
	serv_sock = socket(PF_NET,SOCK_STREAM,0); // 프로토콜 설정 
	
	/*접속 소켓 주소 할당(bind 사용 시 수신지)*/ 
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_NET;  // IPv4 
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 수신할  IP 주소 
	serv_addr.sin_port = htons(7989);	 // 수신할  port 설정 
	if(bind(serv_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1) 
	{
		perror("[server]binding fail"); 
		exit(1);
	}
	
	/*접속 대기큐 open*/
	if( listen(serv_sock,CLIENT_MAX)==-1) 
	{
		perror("[server]listen fail");
		exit(1);
	}
	
	pthread_t c_tid;
	ptread_mutex_init(&mutex,NULL);
	while(1)
	{
		/*접속 허용된  클라이언트 소켓 정보 저장*/
		int clnt_sock;
		struct sockaddr_in clnt_addr;
		int clnt_addr_len = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_add_len); 
		
		pthread_mutex_lock(&g_mutex);
		g_clnt_accept[g_clnt_count++] = clnt_sock;
		pthread_mutex_unlock(&g_mutex);
		
		/*접속 허용 된 소켓 전담 쓰레드 생성*/
		pthread_create(&c_tid,NULL,clnt_routine,(void*)clnt_sock);
	} 
	
	return 0;
}
