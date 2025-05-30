; **************************************************************************** ;
;                                                                              ;
;                                                         :::      ::::::::    ;
;    start.s                                            :+:      :+:    :+:    ;
;                                                     +:+ +:+         +:+      ;
;    By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+         ;
;                                                 +#+#+#+#+#+   +#+            ;
;    Created: 2025/05/24 18:19:32 by kiroussa          #+#    #+#              ;
;    Updated: 2025/05/27 21:48:40 by kiroussa         ###   ########.fr        ;
;                                                                              ;
; **************************************************************************** ;

bits 64
default rel
section .note.GNU-stack noalloc noexec nowrite

section .text
	global _start
	extern main 
	extern __libc_start_main

; int __libc_start_main(
;	int *(main) (int, char * *, char * *), 
; 	int argc, 
; 	char * * ubp_av, 
; 	void (*init) (void), 
; 	void (*fini) (void), 
; 	void (*rtld_fini) (void), 
; 	void (* stack_end)
; );

_start:
	; Get argc (number of arguments)
    pop rsi							    ; argc (first value on the stack)

    ; argv starts at ESP
    mov rdx, rsp						; argv (pointer to argument array)

    ; Push and set arguments for __libc_start_main
    xor r9, r9							; rtld_fini (runtime linker cleanup) (NULL)
	xor r8, r8							; fini (cleanup function) (NULL)
	xor rcx, rcx						; init (constructor function) (NULL)
                                		; argv (already in rdx)
                                		; argc (already in rsi)
    lea rdi, [rel main]					; Pointer to main

	and rsp, -24						; stack alignment
    push r9								; stack_end (NULL)

    call __libc_start_main wrt ..plt	; Call __libc_start_main

    ; If it returns, exit with status 1 (should not happen)
    mov rax, 0x3c						; syscall number for exit
    xor rbx, rbx						; exit code 0
	syscall								; syscall
