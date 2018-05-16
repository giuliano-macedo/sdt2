#include <stdio.h>

#include <armadillo>
void err(const char* msg){
	fprintf(stderr, "%s\n",msg );
	exit(-1);
}
int main(int argc,char** argv){
	if(argc!=4){
		fprintf(stderr, "[usage] %s [matrices dimension] [no. of matrices] [out.mtx]\n", argv[0]);
		return -1;
	}
	if(atoi(argv[1])<=0)err("invalid matrix dimension");
	uint d=atoi(argv[1]);
	uint dd=d*d;
	if(atoi(argv[2])<=0)err("invalid no of matrices");
	uint n=atoi(argv[2]);
	FILE* f=fopen(argv[3],"w");
	if(!f)err("cannot open mtx file");
	if(fwrite((void*)&d,sizeof(uint),1,f)!=1)err("failed writting to mtx file");
	
	arma::fmat id(d,d,arma::fill::eye);
	arma::fmat m;
	arma::fmat invm;
	arma::Mat<unsigned char> bm;
	
	for(uint i=0;i<n;i++){
		do{
			m=arma::randi<arma::fmat>(d,d,arma::distr_param(0,255));
			if(arma::det(m)==0)continue;
			invm=arma::inv(m);
		}while(!approx_equal(m*invm, id, "absdiff", 0.001f));
		bm=arma::conv_to<arma::Mat<unsigned char>>::from(m);
		if(fwrite((void*)bm.memptr(),1,dd,f)!=dd)err("failed writting to mtx file");
	}
	fclose(f);

}