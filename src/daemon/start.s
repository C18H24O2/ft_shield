; **************************************************************************** ;
;                                                                              ;
;                                                         :::      ::::::::    ;
;    start.s                                            :+:      :+:    :+:    ;
;                                                     +:+ +:+         +:+      ;
;    By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+         ;
;                                                 +#+#+#+#+#+   +#+            ;
;    Created: 2025/03/23 03:07:47 by kiroussa          #+#    #+#              ;
;    Updated: 2025/03/23 03:47:24 by kiroussa         ###   ########.fr        ;
;                                                                              ;
; **************************************************************************** ;

bits 64
default rel
section .note.GNU-stack noalloc noexec nowrite

%define ARCH_SET_FS 0x1002
%define SYS_arch_prctl 158

section .text
	global shield_daemon_start
	extern shield_daemon_main
	extern _init
	extern exit

shield_daemon_start:
    ; Set up end of the stack frame linked list
    mov rbp, 0
    push rbp  ; rip=0
    push rbp  ; rbp=0
    mov rbp, rsp

    ; Save argc and argv on the stack
    ; We need those in a moment when we call main
    push rsi
    push rdi

	; Setup TLS (Thread Local Storage)
	mov rdi, ARCH_SET_FS    ; Request type
	mov rsi, rsp            ; Use the current stack as the TCB (temporary)
	mov eax, SYS_arch_prctl ; syscall number for arch_prctl
	syscall                 ; Call kernel

    ; Run the global constructors.
    call _init

    ; Restore argc and argv before calling main
    pop rdi
    pop rsi

    ; Run main
    call shield_daemon_main

    ; Terminate the process with the exit code 
    ; that was returned from main
    mov edi, eax
    call exit
