    .section .data
scanf_str:
    .string "%s"
scanf_len:
    .string "%hhu"
scanf_int:
    .string "%d"
    
    .section .text
.global run_main

run_main:
    pushq   %rbp                # stack frame
    movq    %rsp, %rbp

    subq    $528, %rsp          # allocate memory for Pstring p1,p2
                                # they're both struct of size 256
                                # and 16 bytes for int opt (for stack alignment)
    
    movq    $scanf_len, %rdi    # read an integer into p1_len
    leaq    -256(%rbp), %rsi    # rsi=&p1_len
    xorq    %rax, %rax
    call    scanf

    movq    $scanf_str, %rdi    # read a string into p1->str
    leaq    -255(%rbp), %rsi    # load p1->str in to rsi
    xorq    %rax, %rax
    call    scanf
    
    movq    $scanf_len, %rdi    # read an integer into p2_len
    leaq    -512(%rbp), %rsi    # rsi=&p2_len
    xor     %rax, %rax
    call    scanf

    movq    $scanf_str, %rdi    # read a string into p2->str
    leaq    -511(%rbp), %rsi    # load p2->str in to rsi
    xorq    %rax, %rax
    call    scanf

    movq    $scanf_int, %rdi    # read an integer into opt
    leaq    -528(%rbp), %rsi    # get the address of opt
    xorq    %rax, %rax
    call    scanf

    movl    -528(%rbp), %edi    # edi = opt
    leaq    -256(%rbp), %rsi    # rsi = p1
    leaq    -512(%rbp), %rdx    # rdx = p2
    call    run_func
    
    movq    %rbp, %rsp          # restore rbp,rsp
    popq    %rbp
    ret
