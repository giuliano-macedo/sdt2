#include<stdio.h>
#include<vector>
#include<armadillo>
#include<unistd.h>
void err(const char* msg){
	fprintf(stderr, "%s\n",msg );
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
int main(){
	sleep(5);
	std::vector<arma::fmat> t=readMatrixFile(fopen("test.mtx","r"));
	int s=t.size();
	sleep(5);
	for(uint i=0;i<s;i++){
		t.pop_back();
	}
	sleep(5);
}