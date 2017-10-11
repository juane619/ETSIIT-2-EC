.section .data
lista:		.int 0xfffffffe, 1
longlista:	.int (.-lista)/4
resultado:	.long 1

.section .text
main:	.global main

	mov    $lista, %ebx
	mov longlista, %ecx
	call suma
	mov %eax, resultado

	mov $1, %eax
	mov $0, %ebx
	int $0x80

#FUNCION SUMA(INCORPORA UN BUCLE Y UN RETURN)
suma:
	push %edx					#Guardar el valor de edx para despues seguir usandolo
	mov $0, %eax				#pone a 0 eax
	mov $0, %edx				#pone a 0 edx
bucle:							#etiqueta bucle
	add (%ebx,%edx,4), %eax		#sumamos a eax, 4*adx+ebx (en ebx, esta el array por lo que sumamos el indice*4 a ebx ya que son 4 bytes cada una de las posiciones del array)
	inc       %edx				#incrementamos el indice edx
	cmp  %edx,%ecx				#comparamos nº de elementos del array con el indice
	jne bucle					#si no son iguales, seguimos aumentando eax

	pop %edx					#devolvemos el valor actual de edx
	ret							#retorna la funcion suma
