#include<random>

#include<stdio.h>
#include<unistd.h>
#include<string.h>

#include <arpa/inet.h>
#include <sys/socket.h> 

#include<armadillo>
void err(const char* msg){
	fprintf(stderr, "%s\n", msg);
	exit(-1);
}
enum Modes{
	PI,
	SLEEP,
	MANUAL
};
void printPi(uint di);
int main(int argc,char** argv){
	if(argc<3){
		fprintf(stderr, "[usage] %s [ip to table] [mode] [params] ...\n",argv[0] );
		fprintf(stderr, "modes \n" );
		fprintf(stderr, "pi [min digit] [max digit] \n" );
		fprintf(stderr, "sleep [min(ms)] [max(ms)] \n" );
		fprintf(stderr, "manual\n" );
		return 1;
	}
	in_addr_t ip = inet_addr(argv[1]);
	if(ip==(in_addr_t)(-1))err("invalid ip");
	std::default_random_engine gen;
	std::uniform_int_distribution<uint> dis;
	enum Modes mode;
	if(!strcmp(argv[2],"pi")){
		mode=PI;
		if(argc!=5){
			fprintf(stderr, "[usage] %s pi [min digit] [max digit] \n",argv[0]);
			return 1;
		}
		if(atoi(argv[3])<0)err("invalid pi min digit");
		if(atoi(argv[4])<0)err("invalid pi max digit");
		dis=std::uniform_int_distribution<uint>(atoi(argv[3]),atoi(argv[4]));
	}
	else if(!strcmp(argv[2],"sleep")){
		mode=SLEEP;
		if(argc!=5){
			fprintf(stderr, "[usage] %s sleep [min(ms)] [max(ms)] \n",argv[0]);
			return 1;
		}
		if(atoi(argv[3])<0)err("invalid sleep min");
		if(atoi(argv[4])<0)err("invalid sleep max");
		dis=std::uniform_int_distribution<uint>(atoi(argv[3]),atoi(argv[4]));
	}
	else if(!strcmp(argv[2],"manual")){
		mode=MANUAL;
	}
	else{
		fprintf(stderr, "unknow mode\n");
		return -1;
	}
	int sock;
	struct sockaddr_in svaddr;

	if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0)err("socket() failed");
	svaddr = (struct sockaddr_in){AF_INET,htons(4242),{ip},0};
	if(connect(sock, (struct sockaddr *) &svaddr, sizeof(svaddr)) < 0)err("connect() failed");
	if(mode==MANUAL){printf("press enter to think\n");}
	while(1){
		uint ru=dis(gen);
		switch(mode){
			case PI:
				printf("generating %u pi digits\n",ru);
				printPi(ru);
				break;
			case SLEEP:
				printf("sleeping for %u ms\n",ru);
				usleep(ru*1000);
				break;
			case MANUAL:
				getchar();
				break;
		}
		int msg=1;
		if(send(sock,(void*)&msg,sizeof(int),0)!=(int)sizeof(int))err("error sending think request");
		uint d;
		printf("waiting to eat...\n");
		if(recv(sock,(void*)&d,sizeof(uint),0)!=(int)sizeof(uint))err("error receiving matrix dim");
		uint dd=d*d;
		float*buff=new float[dd];
		if(recv(sock,(void*)buff,sizeof(float)*dd,0)!=(int)sizeof(float)*dd)err("error receiving matrix");
		arma::fmat m(buff,d,d,false,true);
		arma::fmat invm=arma::inv(m);
		if(send(sock,(void*)invm.memptr(),sizeof(float)*dd,0)!=(int)sizeof(float)*dd)err("error sending invmatrix");
		printf("finished eating\n");
	}
	return 0;
}