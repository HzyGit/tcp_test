#include "std.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <error.h>
#include <getopt.h>
#include <errno.h>

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

int main(int argc,char **argv)
{
	parse_arg(argc,argv);
}

