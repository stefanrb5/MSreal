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
	int sval1_i,sval2_i,sval3_i,sval4_i;
	char sval1,sval2,svlal3,sval4;
	size_t num_of_bytes = 6;
	int ispuna=0;
	int svetli=1;
	int faktor_ispune=0;
	long int period = 20000L;
	int vrednost = 0;

	fp1 = fopen("/dev/switch", "r");
	if(fp1==NULL)
	{
		puts("problem pri otvaranju");
		return -1;
	}

	str1=(char *)malloc(num_of_bytes+1);
	getline(&str1,&num_of_bytes,fp1);

	if(fclose(fp1))
	{
		puts("Problem pri zatvaranju");
		return -1;
	}

	sval1 = str1[2]-48;
	sval2 = str1[3]-48;
	sval3 = str1[4]-48;
	sval4 = str1[5]-48;
	free(str);


	if(sval1_i==0)
	{
		svetli=0;
	}
	else
	{
		if(sval2_i==1) ispuna+=0.2;
		if(sval3_i==1) ispuna+=0.1;
		if(sval4_i==1) ispuna+=0.05;
	}

	fp2 = fopen("/dev/button","r" );
	if(fp2==NULL)
	{
		puts("problem pri otvaranju");
		return -1;
	}

	str2=(char *)malloc(num_of_bytes+1);
	getline(&str2,&num_of_bytes,fp2);

	if(fclose(fp2))
	{
		puts("Problem pri zatvaranju");
		return -1;
	}
	taster_n=0;
	if(str[2]-48==1)taster_n=5;
	if(str[3]-48==1)taster_n=1;
	
	if(taster_n!=taster_s)
	{
		taster_s=taster_n;

	free(str2);

	if(taster_n==5 && faktor_ispune<1)
	{
		faktor_ispune+=ispuna;
	}

	if(taster_n==1 && faktor_ispune>0)
	{
		faktorispune-=ispuna;
	}

	usleep(1);

	fp3=fopen("/dev/led","w");
	if(fp==NULL)
	{
		printf("Problem pri otvaranju");
		return -1;
	}

	if(svetli == 0)
	{
		fputs("0x00\n",fp3);
	}
	else
	{

		fputs("0x0F\n",fp3);
		if(fclose(fp))
		{
			printf("problem pri zatvaranju");
			return -1;
		}
		usleep(faktor_ispune*period);

		fp3 = fopen("/dev/led","w");
		if(fp==NULL)
		{
			printf("problem pri otvaranju");
			return -1;
		}
		fputs("0x00\n",fp3);
		if(fclose(fp3))
		{
			printf("problem pri zatvaranju");
			return -1;
		}
		usleep((1-faktor_ispune)*period);
	}
	}
}

	





	
	











		
		







}
