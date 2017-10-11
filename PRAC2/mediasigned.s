.section .data

lista:
   .int -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,0,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1

longlista:	.int (.-lista)/4
cociente:	.int 0
resto:      .int 0
dividendo:  .int 0
divisor:    .int 0
f:	.string "Resultado de la division %d entre %d: \n\tCociente: %d dec\t%x hex\n\tResto: %d dec\t%x hex\n\0"

.section .text
	.extern printf

.global main
main:
	mov $lista, %ebx			#ebx: direccion donde empieza lista
	mov longlista, %ecx		#ecx: longitud de la lista
	call suma					#llamada a suma
	mov  %eax,cociente		#cociente: cociente de la division con idiv
	movl %edx,resto			#resto: resto de la division con idiv

	#metemos en la pila los parametros de printf
	push resto
	push resto
	push cociente
	push cociente
	push divisor
	push dividendo
	push $f
	call printf				#llamamos a printf
	add $28, %esp			#devolvemos a esp el valor original antes de introducir parametros de printf(modificado al introducir parametros)

	#SALIDA DEL PROGRAMA
	mov $1, %eax   #Llamada a exit()
	mov $0, %ebx   #codigo a retornar en la salida
	int $0x80      #interrupcion para llamada a exit

# SUBRUTINA: suma(int* lista, int longlista);
# entrada:	1) %ebx = direccion inicio del array
#			   2) %ecx = numero de elementos a sumar
# salida: 	%eax = resultado de la suma

suma:
	pushl %ebp     #Guardamos ebp
	mov $0, %esi					#esi: 32 bits menos significativos de la suma total
	mov $0, %edi					#edi: 32 bits mas significativos(extension del signo) de la suma total
bucle:
	movl (%ebx,%ebp,4), %eax	#eax: valor de ebx+ebp x 4 en cada pasada del bucle, siendo ebp el contador del bucle
	cltd						      #edx:eax, extension del signo del valor de eax y el valor de eax normal, respectivamente
	addl %eax, %esi				#esi: acumulador de los valores de eax(sin acarreo, suma en 32 bits menos significativos)
	adc %edx, %edi				   #edi: acumulador del acarreo, en caso de que exista, de la extension del signo
	incl	%ebp				      #contador++
	cmpl  %ebp,%ecx				#comprobamos si fin del array
	jne bucle					   #segimos sumando si no fin
	movl %esi, %eax
	movl %edi, %edx

	#aora dividimos edx:eax entre ecx, se guardara el cociente en eax y el resto en edx, y lo mostramos por pantalla  (idiv ecx)
	movl %eax, dividendo
	movl %ecx, divisor
	idiv %ecx

	popl %ebp
	ret            #retornamos eax
