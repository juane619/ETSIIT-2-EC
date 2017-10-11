.section .data

	lista:
		.int 0xF0000000, 0xe0000000, 0xe0000000, 0xd0000000, 0xF0000000, 0xe0000000, 0xe0000000, 0xd0000000, 0xF0000000, 0xe0000000, 0xe0000000, 0xd0000000, 0xF0000000, 0xe0000000, 0xe0000000, 0xd0000000, 0xF0000000, 0xe0000000, 0xe0000000, 0xd0000000, 0xF0000000, 0xe0000000, 0xe0000000, 0xd0000000, 0xF0000000, 0xe0000000, 0xe0000000, 0xd0000000, 0xF0000000, 0xe0000000, 0xe0000000, 0xd0000000

	longlista:	.int (.-lista)/4
	resultado:	.quad 0x0123456789abcdef
	f:	.string "res= %lld = %llx hex\n\0"

.section .text
	.extern printf

	.global main
	main:
		mov $lista, %ebx			#ebx: direccion donde empieza lista
		mov longlista, %ecx		#ecx: longitud de la lista
		call suma					#llamada a suma
		movl  %eax,resultado		#salvar resultado (32 bits menos significativos(eax) de resultado)
		movl %edx,resultado+4	#salvamos acarreos (32 bits mas significativos(edx) de resultado)

		#metemos en la pila los parametros de printf
		pushl resultado+4			#para que printf funcione bien, guardamos primero los bits mas significativos (los que mostrara primero)
		pushl resultado			#guardamos despues los bits menos significativos (los que mostrara despues)
		pushl resultado+4
		pushl resultado
		pushl $f					
		call printf				
		add $20, %esp				#devolvemos a esp el valor original(modificado al introducir parametros)

		#SALIDA DEL PROGRAMA CORRECTA
		mov $1, %eax
		mov $0, %ebx
		int $0x80

		# SUBRUTINA: suma(int* lista, int longlista);
		# entrada:	1) %ebx = direccion inicio del array
		#			2) %ecx = numero de elementos a sumar
		# salida: 	   %eax = resultado de la suma

	suma:
		pushl %ebp		#Guardamos el valor de ebp para restaurarlo al salir de la funcion
		#ponemos a cero los registros
		mov $0, %esi		#esi: 32 bits menos significativos de la suma total
		mov $0, %edi		#edi: 32 bits mas significativos(extension del signo) de la suma total
		
		bucle:
			movl (%ebx,%ebp,4), %eax	#eax: valor de ebx+ebp x 4 en cada pasada del bucle, siendo ebp el contador del bucle
			cltd								#edx:eax, extension del signo del valor de eax y el valor de eax normal, respectivamente
			addl %eax, %esi				#esi: acumulador de los valores de eax(sin acarreo, suma en 32 bits menos significativos)
			adc %edx, %edi					#edi: acumulador del acarreo, en caso de que exista, de la extension del signo
			incl	%ebp						#contador++
			cmpl  %ebp,%ecx				#comprobamos si fin del array
			jne bucle						#segimos sumando si no fin
		
		movl %esi, %eax
		movl %edi, %edx
		popl %ebp
		ret							#retornamos eax como resultado de la funcion
