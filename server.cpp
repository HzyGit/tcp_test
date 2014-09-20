#include "std.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <error.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

const char *g_program="server";

static void usage(int eno)
{
	if(EXIT_SUCCESS!=eno)
		fprintf(stderr,"Try %s --help for more information\n",g_program);
	else
	{
		printf("%s [OPTIONS]: ",g_program);
		printf("tcp server test\n");
		printf("\nOPTIONS:\n");
		printf("  -h,--help display this information\n");
	}
	exit(eno);
}

static void parse_arg(int argc,char **argv)
{
	struct option opt[]=
	{
		{"helo",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};
	int ch;
	while((ch=getopt_long(argc,argv,":h",opt,NULL))!=-1)
	{
		switch(ch)
		{
			case 'h':
				usage(EXIT_SUCCESS);
			case '?':
				fprintf(stderr,"unknow options:%c\n",optopt);
				usage(EXIT_FAILURE);
		}
	}
}

/// @brif 设置进程的信号处理
static int set_signal()
{
}

/// @brief 创建监听套接字
/// @retval >=0 成功, -1失败
static int create_socket()
{
	int sd=0;
	if((sd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		error_at_line(0,errno,__FILE__,__LINE__,"socket error");
		return -1;
	}
	/// 绑定0.0.0.0:SERV_PORT
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(sd,(struct sockaddr*)&servaddr,sizeof(servaddr))==-1)
	{
		error_at_line(0,errno,__FILE__,__LINE__,"bind error");
		return -1;
	}
	/// listen
	if(listen(sd,5)==-1)
	{
		error_at_line(0,errno,__FILE__,__LINE__,"listen error");
		return -1;
	}
	return sd;
}

/// @brief 客户端回显
int str_echo(int sd)
{
	const int SIZE=256;
	char buf[SIZE];
	int len;
	while((len=read(sd,buf,SIZE))!=-1)
	{
		if(len==0)
		{
			close(sd);
			return 0;
		}
		if(write(sd,buf,len)==-1)
		{
			error_at_line(0,errno,__FILE__,__LINE__,"\t%d:write error",getpid());
			return -1;
		}
	}
	error_at_line(0,errno,__FILE__,__LINE__,"\t%d:read error",getpid());
	return -1;
}

/// @breif fork进程处理客户请求
/// @retval -1 失败 0成功
static int server(int sd)
{
	int pid;
	if((pid=fork())==-1)
	{
		error_at_line(0,errno,__FILE__,__LINE__,"fork error");
		return -1;
	}
	if(pid==0)    /// 子进程
	{
		if(str_echo(sd)==-1)
			error_at_line(0,0,__FILE__,__LINE__,"\t%d:exit error",getpid());
		else
			error_at_line(0,0,__FILE__,__LINE__,"\t%d:exit ok",getpid());
		exit(EXIT_SUCCESS);
	}
	else         /// 父进程
	{
		close(sd);
		printf("fork %d for a client\n",pid);
		return 0;
	}
}

/// @brief 打印ipv4地址
static void display_addr(const struct sockaddr_in *addr, FILE *fp)
{
	in_port_t port;
	port=ntohs(addr->sin_port);
	char str_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET,&addr->sin_addr,str_ip,sizeof(str_ip));
	fprintf(fp,"%s:%d",str_ip,port);
}

int main(int argc,char **argv)
{
	parse_arg(argc,argv);
	/// 设置信号处理
	if(set_signal()==-1)
		error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"set signal error");
	/// 创建监听套接字
	int sd;
	if((sd=create_socket())==-1)
		error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"create socket error");
	/// 循环等待客户端请求
	for(;;)
	{
		printf("wait a client\n");
		int newsd;
		struct sockaddr_in clientaddr;
		socklen_t len;
		if((newsd=accept(sd,(struct sockaddr*)&clientaddr,&len))==-1)
		{
			if(EINTR==errno)
				continue;
			error_at_line(0,errno,__FILE__,__LINE__,"accept error");
		}
		FILE *fp=stdout;
		fprintf(fp,"accept a client:");
		display_addr(&clientaddr,fp);
		fprintf(fp,"\n");
		if(server(newsd)==-1)
			error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"handle server error");
	}
}

