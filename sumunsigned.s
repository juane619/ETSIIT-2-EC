.section .data

	lista:	

		.long 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000

	longlista:	.long (.-lista)/4
	resultado:	.quad 0x0123456789abcdef
	f:	.string "res= %llu = %llx hex\n\0"

.section .text
	.extern printf

	.global main
	main:
		mov    $lista, %ebx		#ebx: direccion donde empieza lista
		mov longlista, %ecx		#ecx: longitud de la lista
		call suma					#llamada a suma
		mov  %eax,resultado		#salvar resultado: (32 bits menos significativos(eax) de resultado)
		movl %edx,resultado+4	#salvamos acarreos: (32 bits mas significativos(edx) de resultado)
		
		#metemos en la pila los parametros de printf
		push resultado+4			#para que printf funcione bien, guardamos primero los bits mas significativos (los que mostrara primero)
		push resultado				#guardamos despues los bits menos significativos (los que mostrara despues)
		push resultado+4
		push resultado
		push $f					#tercer parametro(formato del string)	
		call printf				#llamamos a printf
		add $20, %esp			#devolvemos a esp el valor original(modificado al introducir parametros)
		
		#SALIDA DEL PROGRAMA CORRECTA
		mov $1, %eax		
		mov $0, %ebx
		int $0x80

	# SUBRUTINA: suma(int* lista, int longlista);
	# entrada:	1) %ebx = direccion inicio del array
	#			   2) %ecx = numero de elementos a sumar
	# salida: 	%eax = resultado de la suma
		
	suma:
		#ponemos a cero los registros utilizados
		mov $0, %eax
		mov $0, %edx
		mov $0, %edi
		bucle:
			add (%ebx,%edi,4), %eax		#eax: acumulador de ebx+edix4, siendo edi el contador del bucle
			adc $0, %edx					#sumamos a edx 1 del acarreo, en caso de que exista
			incl	%edi						#contador++
			cmpl  %edi,%ecx				#comprobamos si fin del array
			jne bucle						#segimos sumando si no fin

		ret								#retornamos eax
