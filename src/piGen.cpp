#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <gmp.h>

#include <stdio.h>


void printPi(unsigned int d){
	// unsigned long int n=atoi(argv[1]); 
	// unsigned long int k,k8,nd,ten,sixteen;
	// long double s,ans=0;

	unsigned long est;
	mpz_t nd,n;
	mpf_t k,k8,nf,estf,s,stemp,ans,l2,ten,sixteen;
	mpz_inits(nd,n,0);
	mpf_inits(estf,k,k8,ten,sixteen,0);
	
	mpf_init_set_str(l2,"0.3010299956639812",10);
	mpf_init_set_ui(nf,d);
	mpz_set_f(n,nf);
	mpf_div(estf,nf,l2);
	est=mpf_get_ui(estf);
	mpf_clears(l2,estf,0);
	est+=32;

	mpf_init2(ans,est);
	mpf_init2(s,est);

	mpf_init2(stemp,est);
	mpf_init2(ten,est);
	mpf_init2(s,est);
	mpf_init2(k,est);
	mpf_init2(k8,est);



	if(mpz_cmp_ui(n,0UL)>0){
		printf("3.");
		if(mpz_cmp_ui(n,1UL)>0){
			mpf_set_ui(sixteen,1UL);
			mpf_set_ui(ten,1UL);

			// ans=47.0/15.0;
			mpf_set_d(ans,47.0);
			mpf_div_ui(ans,ans,15UL);

			// for(k=1;k<n;k++){
			for( mpf_set_ui(k,1UL); mpf_cmp(k,nf)<0; mpf_add_ui(k,k,1UL)){
				
				// s=0;
				// k8=8.0*k;
				mpf_set_d(s,0.0);
				mpf_mul_ui(k8,k,8UL);


				// k8+=1.0;
				mpf_add_ui(k8,k8,1UL);

				// s+=4.0/k8;
				mpf_ui_div(stemp,4UL,k8);
				mpf_add(s,s,stemp);

				//k8+=3.0;
				mpf_add_ui(k8,k8,3UL);

				//s-=2.0/k8;
				mpf_ui_div(stemp,2UL,k8);
				mpf_sub(s,s,stemp);

				// k8+=1.0;
				mpf_add_ui(k8,k8,1UL);

				// s-=1.0/k8;
				mpf_ui_div(stemp,1UL,k8);
				mpf_sub(s,s,stemp);

				// k8+=1.0;
				mpf_add_ui(k8,k8,1UL);

				// s-=1.0/k8;
				mpf_ui_div(stemp,1UL,k8);
				mpf_sub(s,s,stemp);

				//sixteen*=16;
				//ten*=10;
				mpf_mul_ui(sixteen,sixteen,16UL);
				mpf_mul_ui(ten,ten,10UL);

				// ans+=s/sixteen;
				mpf_div(s,s,sixteen);
				mpf_add(ans,ans,s);

				//nd=ans*ten;
				mpf_mul(stemp,ans,ten);
				mpz_set_f(nd,stemp);

				// gmp_printf("%.32Ff",ans); //debug
				printf("%lu",mpz_mod_ui(nd,nd,10UL));
				// gmp_printf(" %.32Ff\n",stemp); //debug
				fflush(stdout);
				// usleep(75000);
			}
		}
		putchar('\n');
	}
}