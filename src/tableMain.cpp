#include "Table.hpp"
#include <thread>
Table* T;
void err(const char* msg){
	fprintf(stderr, "%s\n", msg);
	exit(-1);
}
std::vector<arma::fmat> readMatrixFile(FILE* f){
	std::vector<arma::fmat> ans;
	uint s;
	if(fread(&s,sizeof(uint),1,f)!=1)err("error opening mtx file");
	uint ss=s*s;
	unsigned char*buff=new unsigned char[ss];
	arma::fmat id(s,s,arma::fill::eye);
	
	while(fread(buff,1,ss,f)==ss){
		arma::Mat<unsigned char> mb(buff,s,s,false,true);
		arma::fmat m=arma::conv_to<arma::fmat>::from(mb);
		if(arma::det(m)==0)err("some matrix in the mtx file is singular");
		arma::fmat invm=arma::inv(m);
		arma::fmat multemp=m*invm;
		if(!approx_equal(multemp,id, "absdiff", 0.001f))err("some matrix in the mtx file is not inversible");
		ans.push_back(m);
	}
	delete buff;
	fclose(f);
	return ans;
}
void phiHandler(int sock,sockaddr_in* addr){
	printf("new thread for %s\n",inet_ntoa(addr->sin_addr));
	T->addPhi(addr,sock);
	int ret;
	while(1){
		if(recv(sock,(void*)&ret,sizeof(int),0)!=(int)(sizeof(int))){
			printf("[Error] failed receiving phi eat request\n");
			T->removePhi(sock);
			return;
		}
		if(ret!=1){
			printf("[Error] wrong phi eat request \n");
			T->removePhi(sock);
			return;	
		}
		try{
			T->eatPhi(sock);
		}catch(std::runtime_error e){
			printf("ending thread for %s\n",inet_ntoa(addr->sin_addr));
			return;
		}
	}
}

int main(int argc,char** argv){
	if(argc!=3){
		fprintf(stderr, "[usage] %s [path to mtx file] [max philosopher think time (ms)]\n", argv[0]);
		return -1;
	}
	FILE* matFile=fopen(argv[1],"r");
	if(!matFile)err("error opening mtx file");
	float maxPhiTime=atof(argv[2]);
	if(maxPhiTime<=0)err("invalid max philosopher think time");
	
	printf("Reading %s ...\n",argv[1]);
	T=new Table(readMatrixFile(matFile));
	struct sockaddr_in svaddr;
	int svsock;
	unsigned int clntlen;
	if((svsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)err("socket() failed");
	
	svaddr=(struct sockaddr_in){AF_INET,htons(4242),{INADDR_ANY},0};
	if(bind(svsock,(struct sockaddr*)&svaddr,sizeof(svaddr))<0)err("bind() failed");
	if(listen(svsock, 3)<0)err("listen() failed");
	printf("table started\n");

	while(1){
		clntlen = sizeof(struct sockaddr_in);
		sockaddr_in* clntaddr=new sockaddr_in();
		int* clntsock=new int();
		if((*clntsock=accept(svsock,(struct sockaddr*)clntaddr,&clntlen)) < 0)err("accept() failed");
		printf("handling philosopher %s\n",inet_ntoa(clntaddr->sin_addr));
		new std::thread(phiHandler,*clntsock,clntaddr);
	}
	shutdown(svsock, 2);

	return 0;
}