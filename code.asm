section .data
formatin : db "%d",0
formatout : db "%d",10,0
section .bss
lsum7: resw 1
lk7: resw 1
lx5: resw 1
section .text
 extern printf
 extern scanf
 GLOBAL main
main:
mov rdi,formatin
lea rsi,[lx5]
mov al,0
call scanf
mov dx,100
mov word[lsum7],dx
mov word[lk7],1
cmp word[lk7],10
jg L2
L1:
mov dx,[lsum7]
mov ax,dx
movsx rax,ax
push rax
mov dx,[lk7]
mov ax,dx
movsx rax,ax
push rax
mov dx,[lx5]
pop rax
imul dx
mov dx,ax
pop rax
add dx,ax
mov word[lsum7],dx
mov rdi,formatout
movsx rsi,word[lsum7]
mov al,0
call printf
inc word[lk7]
cmp word[lk7],10
jle L1
L2:
mov rdi,formatout
movsx rsi,word[lsum7]
mov al,0
call printf
mov ebx,0
mov eax,1
int 0x80
