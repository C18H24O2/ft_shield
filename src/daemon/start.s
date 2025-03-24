; **************************************************************************** ;
;                                                                              ;
;                                                         :::      ::::::::    ;
;    start.s                                            :+:      :+:    :+:    ;
;                                                     +:+ +:+         +:+      ;
;    By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+         ;
;                                                 +#+#+#+#+#+   +#+            ;
;    Created: 2025/03/23 03:07:47 by kiroussa          #+#    #+#              ;
;    Updated: 2025/03/24 15:10:28 by kiroussa         ###   ########.fr        ;
;                                                                              ;
; **************************************************************************** ;

bits 64
default rel
section .note.GNU-stack noalloc noexec nowrite

section .text
	global shield_daemon_start
	extern shield_daemon_main
	extern __libc_start_main
	extern _init
	extern _fini

; int __libc_start_main(
;	int *(main) (int, char * *, char * *), 
; 	int argc, 
; 	char * * ubp_av, 
; 	void (*init) (void), 
; 	void (*fini) (void), 
; 	void (*rtld_fini) (void), 
; 	void (* stack_end)
; );

shield_daemon_start:
	; Get argc (number of arguments)
    pop rsi                     ; argc (first value on the stack)

    ; argv starts at ESP
    mov rdx, rsp				; argv (pointer to argument array)

    ; Push and set arguments for __libc_start_main
    xor r9, r9                  ; rtld_fini (NULL)
	mov r8, _fini               ; fini (cleanup function)
    mov rcx, _init              ; init (constructor function)
                                ; argv (already in rdx)
                                ; argc (already in rsi)
    mov rdi, shield_daemon_main ; Pointer to main

	and rsp, -24				; stack alignment
    push r9						; stack_end (NULL)

    call __libc_start_main		; Call __libc_start_main

    ; If it returns, exit with status 1 (should not happen)
    mov rax, 0x3c				; syscall number for exit
    xor rbx, rbx				; exit code 0
	syscall						; syscall
