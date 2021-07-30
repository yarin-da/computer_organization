    .section .rodata    
invalid_str:
    .string "invalid option!\n"
pstrlen_print:
    .string "first pstring length: %d, second pstring length: %d\n"
replaceChar_print:
    .string "old char: %c, new char: %c, first string: %s, second string: %s\n"
pstring_printf:
    .string "length: %d, string: %s\n"
pstrijcmp_print:
    .string "compare result: %d\n"
scan_char:
    .string " %c"
scan_len:
    .string "%hhu"
jump_table:
    .quad case_50
    .quad case_default
    .quad case_52
    .quad case_53
    .quad case_54
    .quad case_55
    .quad case_default
    .quad case_default
    .quad case_default
    .quad case_default
    .quad case_50

    .section .text
.global run_func

    # %rdi = int opt
    # %rsi = Pstring *p1
    # %rdx = Pstring *p2
run_func:
    pushq   %rbp                        # stack frame
    pushq   %r12                        # save before use
    pushq   %r13                        
    movq    %rsp, %rbp
    movq    %rsi, %r12                  # save p1,p2 in r12,r13
    movq    %rdx, %r13
    subl    $50, %edi                   # opt -= 50
    cmpl    $10, %edi                   # if opt >u 10 goto default
    ja      case_default
    jmp     *jump_table(,%edi,8)        # jmp to the proper case
run_func_done:
    movq    %rbp, %rsp                  # restore rsp and ebp
    popq    %r13
    popq    %r12
    popq    %rbp
    ret

case_50:
    movq    %r12, %rdi                  # call pstrlen on p1
    call    pstrlen                     
    pushq   %rax                        # save ret value in rcx

    movq    %r13, %rdi                  # call pstrlen on p2
    call    pstrlen

    movq    $pstrlen_print, %rdi        # print the lengths
    popq    %rsi                        # get last pstrlen's ret value
    movq    %rax, %rdx
    xor     %rax, %rax
    call    printf

    jmp     run_func_done

case_52:
    subq    $16, %rsp                   # char oldChar, newChar
    movq    $scan_char, %rdi        
    leaq    1(%rsp), %rsi
    xorq    %rax, %rax
    call    scanf                       # scanf(" %c", &oldChar)
    
    movq    $scan_char, %rdi
    movq    %rsp, %rsi
    xorq    %rax, %rax
    call    scanf                       # scanf(" %c", &newChar)

    movq    %r12, %rdi                  # call replaceChar on p1
    movb    1(%rsp), %sil
    movb    (%rsp), %dl
    call    replaceChar                 

    movq    %r13, %rdi                  # call replaceChar on p2
    movb    1(%rsp), %sil
    movb    (%rsp), %dl
    call    replaceChar

    movq    $replaceChar_print, %rdi    # load args and call printf
    movq    1(%rsp), %rsi
    movq    (%rsp), %rdx
    leaq    1(%r12), %rcx               # load p1->str into rcx
    leaq    1(%r13), %r8                # load p2->str into r8
    xorq    %rax, %rax
    call    printf

    jmp     run_func_done
    
case_53:
    sub     $16, %rsp                   # int i, j
    movq    $scan_len, %rdi             # read an integer into i
    leaq    1(%rsp), %rsi
    call    scanf

    movq    $scan_len, %rdi             # read an integer into j
    movq    %rsp, %rsi
    call    scanf

    movq    %r12, %rdi                  # strijcpy(p1, p2, i, j)
    movq    %r13, %rsi
    movb    1(%rsp), %dl
    movb    (%rsp), %cl
    call    pstrijcpy
    
    movq    $pstring_printf, %rdi       # print p1
    movzbq  (%r12), %rsi
    leaq    1(%r12), %rdx               # load p1->str into rdx
    xor     %rax, %rax
    call    printf
    
    movq    $pstring_printf, %rdi       # print p2
    movzbq  (%r13), %rsi
    leaq    1(%r13), %rdx
    xor     %rax, %rax
    call    printf
    
    jmp     run_func_done
    
case_54:
    movq    %r12, %rdi                  # call swapCase on p1
    call    swapCase
    
    movq    %r13, %rdi                  # call swapCase on p2
    call    swapCase

    movq    $pstring_printf, %rdi       # print p1
    movzbq  (%r12), %rsi
    leaq    1(%r12), %rdx               # load p1->str into rdx
    xor     %rax, %rax
    call    printf
    
    movq    $pstring_printf, %rdi       # print p2
    movzbq  (%r13), %rsi
    leaq    1(%r13), %rdx               # load p2->str into rdx
    xor     %rax, %rax
    call    printf
    
    jmp     run_func_done
    
case_55:
    subq    $16, %rsp                   # int i, j
    movq    $scan_len, %rdi             # read an integer into i
    leaq    1(%rsp), %rsi
    call    scanf

    movq    $scan_len, %rdi             # read an integer into j
    movq    %rsp, %rsi
    call    scanf

    movq    %r12, %rdi                  # call pstrijcmp on p1 and p2
    movq    %r13, %rsi
    movb    1(%rsp), %dl
    movb    (%rsp), %cl
    call    pstrijcmp
    
    movq    $pstrijcmp_print, %rdi      # print compare result
    movl    %eax, %esi
    xor     %rax, %rax
    call    printf

    jmp     run_func_done
    
case_default:
    mov     $invalid_str, %rdi          # print invalid option
    xor     %rax, %rax
    call    printf

    jmp     run_func_done

