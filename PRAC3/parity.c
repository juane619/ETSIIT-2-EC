#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

//Usamos estas comparaciones de compilacion para facilitar la creacion de la lista cambiando su tamaño

#define WSIZE 8*sizeof(int)
int resultado=0;

#define TEST    0
#define COPY_PASTE_CALC 1

#if ! TEST
  #define NBITS 20
  #define SIZE (1<<NBITS) 	// tamaño suficiente para tiempo apreciaable

  unsigned lista[SIZE];
  #define RESULT 1 	// fórmula
#else


#define SIZE 4
unsigned lista[SIZE]={0x80000000, 0x00100000, 0x00000800, 0x000000011};
#define RESULT 4

#endif

int parity1(int* array, int len){	//suma normal
  int result=0;
    int x, bit;

  for(int i = 0; i < len; i++){
    x = array[i];
    bit = 0;
    for(int j = 0; j < 32; j++){
      bit = bit^( x & 0x1);		//acumulamos con xor el valor del ultimo bit del elemento actual del array y el ultimo calculo hecho
      x >>= 1;
    }
    result += bit;
  }

  return result;
}

int parity2(int* array, int len){
  int result=0;
    int x, bit;

  for(int i = 0; i < len; i++){
    x = array[i];
    bit = 0;
    while(x){	//mientras x no valga cero segun los desplazamientos realizados(por si podemos ahorrarnos vueltas)
      bit = bit^( x & 0x1);
      x >>= 1;
    }
    result += bit;
  }

  return result;
}

int parity3(int* array, int len){
    int result=0, i, x;
    for(i=0; i<len;i++){
        x=array[i];
        int val = 0;
        while (x) {
            val ^= x;
            x >>= 1;
        }
        result+= val & 0x1;
    }
    return result;
}

int parity4(int* array, int len){
    int result=0, i, x, val;
    for(i=0; i<len;i++){
        x=array[i];
        val = 0;
        //asm
        asm("\n"		//al igual que el anterior pero en asm
        "ini3:  \n\t"		//saltamos aqui mientras x no valga cero
        "xor %[x], %[v] \n\t"
        "shr $1, %[x]   \n\t"
        "cmp $0,%[x] \n\t"
        "ja ini3 \n\t"
        :[v]"+r"(val)
        :[x]"r" (x)
        );
        result+= val & 0x1;
    }
    return result;
}

int parity5(int* array, int len){
    int result=0, i,j, x;
    for(i=0; i<len;i++){
        x=array[i];
        
        for(j=16; j>1; j/=2){		//VAmos acumulando con xor, x desplazada 16, 8, 4, 1 
            x^=x>>j;
        }
        
        result+= x & 0x1;
    }
    return result;
}

int parity6(int* array, int len){
    int result=0, i,j, x;
    for(i=0; i<len;i++){
        x=array[i];
        
        asm("\n"
        "mov %[x], %%edx    \n\t"
        "shr $16, %[x] \n\t"		//desplazamos x 16 posiciones a la derecha
        "xor %[x], %%edx   \n\t"	//edx: x xor (x>>16)
        "setp %%dl\n\t"				//registro %dl: byte si PF=1		
        "movzx %%dl, %[x]  \n\t"	//movemos el byte a un registro grande(x, guardado en registro)
        : [x]"+r" (x)
        :
        : "edx"
);
        
        result+= x & 0x1;
    }
    return result;
}

void crono(int (*func)(), char* msg){
    int result=0;
    struct timeval tv1,tv2;	// gettimeofday() secs-usecs
    long           tv_usecs;	// y sus cuentas

    gettimeofday(&tv1,NULL);
    result = func(lista, SIZE);
    gettimeofday(&tv2,NULL);

    tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
             (tv2.tv_usec-tv1.tv_usec);
    //Compilacion para facilitar el copy paste de los resultados
    #if ! COPY_PASTE_CALC
		 printf("resultado = %d\t", resultado);
		 printf("%s:%9ld us\n", msg, tv_usecs);
	 #else
		 printf("%ld\n", tv_usecs);
   #endif
}

int main()
{   
    #if ! TEST
    int i; 	 	 	// inicializar array
	 for (i=0; i<SIZE; i++)
		 lista[i]=i;
#endif
    crono(parity1, "parity1 (lenguaje C -      for)");
    crono(parity2, "parity2 (lenguaje C -      for)");
     crono(parity3, "parity3 (lenguaje C -      for)");
     crono(parity4, "parity4 (lenguaje C -      for)");
     crono(parity5, "parity5 (lenguaje C -      for)");
     crono(parity6, "parity6 (lenguaje C -      for)");
     
     #if ! COPY_PASTE_CALC
    printf("calculado = %ld\n", RESULT);
#endif
    exit(0);
}
