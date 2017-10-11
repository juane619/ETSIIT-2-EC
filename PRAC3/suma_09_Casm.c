//  según la versión de gcc y opciones de optimización usadas, tal vez haga falta
//  usar gcc –fno-omit-frame-pointer si gcc quitara el marco pila (%ebp)

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define SIZE (1<<16)	// tamaño suficiente para tiempo apreciable
unsigned lista[]= {0x80000000, 0x00100000};
int resultado=0;

#define WSIZE 8*sizeof(int)
int pcount_for(unsigned* array, int len){
	int i;
	int result = 0;
	for(int j=0; j<1; j++){
		for (i = 0; i < WSIZE; i++){
			unsigned mask = 1 << i;
			result += (x & mask) != 0;
		}
	}
	return result;
}

/*
int suma2(int* array, int len)
{
    int  i,   res=0;
    for (i=0; i<len; i++)
    asm("add (%[a],%[i],4),%[r]	\n"
     :	[r] "+r" (res)		// output-input
     :	[i]  "r" (i),		// input
	[a]  "r" (array)
    );
    return res;
}

int suma3(int* array, int len)
{
    asm("mov 8(%%ebp), %%ebx	\n"  // array
"	mov 12(%%ebp), %%ecx	\n"  // len
"				\n"
"	mov $0, %%eax		\n"  // retval
"	mov $0, %%edx		\n"  // index
"bucle:				\n"
"	add (%%ebx,%%edx,4),%%eax\n"
"	inc       %%edx		\n"
"	cmp %%edx,%%ecx		\n"
"	jne bucle		\n"
     : 				// output
     : 				// input
     :	"ebx"			// clobber
    );
}
*/
void crono(int (*func)(), char* msg){
    struct timeval tv1,tv2;	// gettimeofday() secs-usecs
    long           tv_usecs;	// y sus cuentas

    gettimeofday(&tv1,NULL);
    resultado = func(lista, SIZE);
    gettimeofday(&tv2,NULL);

    tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
             (tv2.tv_usec-tv1.tv_usec);
    printf("resultado = %d\t", resultado);
    printf("%s:%9ld us\n", msg, tv_usecs);
}

int main()
{
    int i;			// inicializar array
	
	for(i=0; i<sizeof(lista)/4; i++)
		printf("Elemento %d: %x\n", i, lista[i]);
    printf("Resultado de paridad: %u", paridad(lista, sizeof(lista)/4));

    //crono(suma1, "suma1 (en lenguaje C    )");
    //crono(suma2, "suma2 (1 instrucción asm)");
    //crono(suma3, "suma3 (bloque asm entero)");
    //printf("Resultado real: N*(N+1)/2 = %d\n", (SIZE-1)*(SIZE/2)); /*OF*/

    exit(0);
}
