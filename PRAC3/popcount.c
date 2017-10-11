#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

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

int pcount_1(unsigned* array, int len){
	int i, j;
	int result = 0;
	for(j=0; j<len; j++){
		unsigned x= array[j];
		for (i = 0; i < WSIZE; i++){
			unsigned mask = 1 << i;
			result += (x & mask) != 0;
		}
	}
	return result;
}

int pcount_2(unsigned* array, int len){
	int result = 0, j=0, x;
	for(j=0; j<len; j++){
		unsigned x= array[j];
		while(x){
			result += x & 0x1;
			x >>= 1;
		}
	}
	return result;
}

int pcount_3(unsigned* array, int len){
	int result = 0, i, x;
	for(i=0; i<len; i++){
		unsigned x= array[i];
		asm("\n"
         "ini3: \n\t"
			"shr $1, %[x] \n\t"
			"adc $0,%[r] \n\t"
			"cmp $0,%[x] \n\t"
			"ja ini3 \n\t"

		: [r]"+r"(result)
		: [x] "r" (x));

	}
	return result;
}

int pcount_4(unsigned* array, int len){
	int j = 0, i, x=0, val=0, result=0;
	unsigned mask=0x01010101;

	for(j=0; j<len; j++){
		x= array[j];
		val=0;
		//printf("x= %d\n", x);
		for(i=0; i<8; i++){
			val+=x&mask;
			x>>=1;
		}
		val+=(val>>16);
		val+=(val>>8);
		result+=val&0xFF;
	}
	return result;
}

int pcount_5(unsigned* array, int len){
   int i;
   int val, result=0;
   int SSE_mask[]= {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
   int SSE_LUTb[]= {0x02010100, 0x03020201, 0x03020201, 0x04030302};
               //    3 2 1 0     7 6 5 4     1110 9 8    15141312

   if(len & 0x3) printf("leyendo 128b pero len no multiplo de 4?\n");

   for(i=0; i<len; i+=4){   //El bucle tiene que hacer 3 lecturas de memoria, por lo que se produce cuello de botella. Si sacaramos la copia de memoria de LUT y de la mascara, se optimizaria bastante mas el ejercicio.
      asm("movdqu %[x], %%xmm0\n\t"
      "movdqa %%xmm0, %%xmm1\n\t"   //Dos copias de x

      "movdqu %[m], %%xmm6 \n\t"    //mascara
      "psrlw $4, %%xmm1 \n\t"
      "pand %%xmm6, %%xmm0 \n\t"    //xmm0 - nibbles inferiores
      "pand %%xmm6, %%xmm1 \n\t"    //xmm1 - nibbles superiores

      "movdqu %[l], %%xmm2 \n\t"    //Guardamos LUT porque pshufb la sobreescribe
      "movdqa %%xmm2, %%xmm3 \n\t"  //Copiamos de nuevo LUT

      "pshufb %%xmm0, %%xmm2 \n\t"  //xmm2= vector popcount inferiores
      "pshufb %%xmm1, %%xmm3 \n\t"  //xmm3= vector popcount superiores

      "paddb %%xmm2, %%xmm3 \n\t"   //xmm3 - vector popcount bytes
      "pxor %%xmm0, %%xmm0 \n\t"    //xmm0= 0,0,0,0
      "psadbw %%xmm0, %%xmm3 \n\t"  //xmm3= [pcnt bytes0..7|pcnt bytes8..15]
      "movhlps %%xmm3, %%xmm0 \n\t" //xmm0= [   0  |pcnt bytes0..7]
      "paddd %%xmm3, %%xmm0 \n\t"   //xmm0= [   no usado |pcnt bytes0..15]
      "movd %%xmm0, %[val] \n\t"
      :[val]"=r" (val)
      :  [x] "m" (array[i]),
         [m] "m" (SSE_mask[0]),
         [l] "m" (SSE_LUTb[0])
      );

      result += val;
   }
   return result;

}

//Version SSE4.2(popcount)
int pcount_6(unsigned* array, int len){
	int i, x=0, val=0, result=0;

	for(i=0; i<len; i++){
		x= array[i];
		asm("popcnt %[x], %[val]"
		:[val] "=r" (val)
		:[x] "r" (x)
		);
		result+= val;
	}
	return result;
}

//64 bit p/mejorar prestaciones
int pcount_7(unsigned* array, int len){
	int i, x1, x2, val=0, result=0;

	if(len & 0x1)
      printf("leer 64b y len impar?\n");

	for(i=0; i<len; i+=2){
		x1= array[i];
		x2= array[i+1];
		asm("popcnt %[x1], %[val]\n\t"
		"popcnt %[x2], %%edi\n\t"
		"add %%edi, %[val]\n\t"
		:[val] "=&r" (val)
		:[x1] "r" (x1),
		[x2] "r" (x2)
		:"edi");

		result+= val;
	}
	return result;
}

void crono(int (*func)(), char* msg){
    struct timeval tv1,tv2;	// gettimeofday() secs-usecs
    long           tv_usecs;	// y sus cuentas
   long leng= sizeof(lista)/4;

    gettimeofday(&tv1,NULL);
    resultado = func(lista, leng);
    gettimeofday(&tv2,NULL);

    tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
             (tv2.tv_usec-tv1.tv_usec);
    #if ! COPY_PASTE_CALC
		 printf("resultado = %d\t", resultado);
		 printf("%s:%9ld us\n", msg, tv_usecs);
	 #else
		 printf("%ld\n", tv_usecs);
   #endif
}

int main() {
#if ! TEST
    int i; 	 	 	// inicializar array
	 for (i=0; i<SIZE; i++)
		 lista[i]=i;
#endif
    crono(pcount_1, "popcount1 (lenguaje C -      for)");
    crono(pcount_2, "popcount1 (lenguaje C -      for)");
    crono(pcount_3, "popcount1 (lenguaje C -      for)");
    crono(pcount_4, "popcount1 (lenguaje C -      for)");
    crono(pcount_5, "popcount1 (lenguaje C -      for)");
    crono(pcount_6, "popcount1 (lenguaje C -      for)");
    crono(pcount_7, "popcount1 (lenguaje C -      for)");

#if ! COPY_PASTE_CALC
    printf("calculado = %ld\n", RESULT);
#endif

	exit(0);
}
