#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	FILE* fp1;
	FILE* fp2;
	FILE* fp3;
	int taster_n,taster_s=0;
	char tval1, tval2;
	char *str1;
	char *str2;
	char sval1,sval2,sval3,sval4;
	size_t num_of_bytes = 6;
	float ispuna=0;
	int svetli=1;
	float faktor_ispune=0;
	long int period = 20000L;
	int vrednost = 0;
while(1)
{
	printf("faktor_ispune=%f\n", faktor_ispune);
	fp1 = fopen("/dev/switch", "r");
	if(fp1==NULL)
	{
		puts("problem pri otvaranju /dev/switch\n");
		return -1;
	}

	str1=(char *)malloc(num_of_bytes+1);
	getline(&str1,&num_of_bytes,fp1);

	if(fclose(fp1))
	{
		puts("Problem pri zatvaranju /dev/switch\n");
		return -1;
	}

	sval1 = str1[2]-48;
	sval2 = str1[3]-48;
	sval3 = str1[4]-48;
	sval4 = str1[5]-48;
	free(str1);


	if(sval1==0)
	{
		svetli=0;
	}
	else
	{
		svetli=1;
		if(sval2==1) ispuna+=4;
		if(sval3==1) ispuna+=2;
		if(sval4==1) ispuna+=1;
	}

	ispuna*=0.05;

	fp2 = fopen("/dev/button","r" );
	if(fp2==NULL)
	{
		puts("problem pri otvaranju /dev/button\n");
		return -1;
	}

	str2=(char *)malloc(num_of_bytes+1);
	getline(&str2,&num_of_bytes,fp2);

	if(fclose(fp2))
	{
		puts("Problem pri zatvaranju /dev/button\n");
		return -1;
	}
	taster_n=0;
	if(str2[2]-48==1)taster_n=5;
	if(str2[3]-48==1)taster_n=1;
	
	if(taster_n!=taster_s)
	{
		taster_s=taster_n;

		free(str2);

		if(taster_n==5 && faktor_ispune<1)
		{
			faktor_ispune+=ispuna;
		//	faktor_ispune+=0.01;
			printf("faktor_ispune_povecan\n");
	//		printf("faktor_ispune %f",faktor_ispune);
			if(faktor_ispune > 1)
			{
				faktor_ispune = 1;
			}
		}

		if(taster_n==1 && faktor_ispune>0)
		{
			faktor_ispune-=ispuna;
		//	faktor_ispune-=0.01;
	//		printf("faktor_ispune_smanjen\n");
			if(faktor_ispune < 0)
			{
				faktor_ispune = 0;
			}
		}
	}


	fp3=fopen("/dev/led","w");
	if(fp3==NULL)
	{
		printf("Problem pri otvaranju /dev/led\n");
		return -1;
	}

	if(svetli == 0)
	{
		fputs("0x00\n",fp3);
		printf("ne_svetli\n");
	}
	else
	{

		fputs("0x0F\n",fp3);
		if(fclose(fp3))
		{
			printf("problem pri zatvaranju /dev/led\n");
			return -1;
		}
		usleep(faktor_ispune*period);

		fp3 = fopen("/dev/led","w");
		if(fp3==NULL)
		{
			printf("problem pri otvaranju\n");
			return -1;
		}
		fputs("0x00\n",fp3);
		if(fclose(fp3))
		{
			printf("problem pri zatvaranju\n");
			return -1;
		}
		usleep((1-faktor_ispune)*period);
	}
}
}
