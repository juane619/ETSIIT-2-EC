#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <string.h>	// para strncmp()/strlen()
#include <sys/time.h>	// para gettimeofday(), struct timeval
#include <stdbool.h>
#include <ctype.h>

int passcode=7777;
char password[10]="\nLIVOM";

void boom(){
	printf("***************\n");
	printf("*** BOOM!!! ***\n");
	printf("***************\n");
	exit(-1);
}

void defused(){
	printf("*************************\n");
	printf("*** bomba desactivada ***\n");
	printf("*************************\n");
	exit(0);
}

bool check_pass(const char *p){
    bool bien=true;
    int len_micadena= strlen(p);
    //COMPROBAR SI CADENA COINCIDE AL REVES
    int j = len_micadena-1;
    for (int i=0; i<len_micadena; i++) {
            if(password[i] != toupper(p[j]))
                return false;
            j--;
    }
    
    return bien;
}

int main(){
#define SIZE 100
	char pass[SIZE];
	int  pasv;
#define TLIM 60
	struct timeval tv1,tv2;	// gettimeofday() secs-usecs

	gettimeofday(&tv1,NULL);

        
	printf("Introduce la contraseña: ");
	fgets(pass,SIZE,stdin);
	
        if (!check_pass(pass))
	    boom();

	gettimeofday(&tv2,NULL);
	if (tv2.tv_sec - tv1.tv_sec > TLIM)
	    boom();

	printf("Introduce el código: ");
	scanf("%i",&pasv);
	if (pasv!=passcode) boom();

	gettimeofday(&tv1,NULL);
	if (tv1.tv_sec - tv2.tv_sec > TLIM)
	    boom();

	defused();
}
