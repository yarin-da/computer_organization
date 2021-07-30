    .section .rodata
invalid_str:
    .string "invalid input!\n"
    
    .section .text
.global pstrlen
.global replaceChar
.global swapCase
.global pstrijcpy
.global pstrijcmp
    
    # %rdi = Pstring *pstr
    # ret char
pstrlen:
    movzbq  (%rdi), %rax            # return *(char *)pstr
    ret

    # %rdi = Pstring *pstr
    # %rsi = char oldChar
    # %rdx = char newChar
    # ret Pstring *
replaceChar:
    leaq    1(%rdi), %rcx           # char *p = pstr->str
replaceChar_loop:
    cmpb    $0, (%rcx)              # if *p=='\0' finish
    je      replaceChar_done
    cmpb    %sil, (%rcx)            # if *p != oldChar
    jne     replaceChar_postif
    movb    %dl, (%rcx)             # *p = newChar
replaceChar_postif:
    inc     %rcx                    # p += 1
    jmp     replaceChar_loop
replaceChar_done:
    movq    %rdi, %rax              # return pstr
    ret

    # %rdi = Pstring *pstr
    # ret Pstring *
swapCase:
    lea     0x1(%rdi), %rcx         # char *p = pstr->str
swapCase_loop:
    xor     %rax, %rax              # insert *p into rax
    movb    (%rcx), %al
    cmpb    $0, %al
    jz      swapCase_done           # if *p == '\0' finish loop
    cmp     $97, %al                # *p < 'a'
    jl      swapCase_upperCase
    cmpb    $122, %al               # *p > 'z'
    jg      swapCase_postifs
    sub     $32, %al                # *p -= 'a' - 'A' (32)
    movb    %al, (%rcx)
    jmp     swapCase_postifs
swapCase_upperCase:
    cmpb    $65, %al                # *p < 'A'
    jl      swapCase_postifs
    cmpb    $90, %al                # *p > 'Z'
    jg      swapCase_postifs
    add     $32, %al                # *p += 'a' - 'A' (32)
    movb    %al, (%rcx)
swapCase_postifs:
    inc     %rcx                    # p += 1
    jmp     swapCase_loop           # loop
swapCase_done:  
    movq    %rdi, %rax              # return pstr
    ret

    # %rdi = Pstring *dst
    # %rsi = Pstring *src
    # %rdx = unsigned char i
    # %rcx = unsigned char j
pstrijcpy:
    and     $0xff, %rdx             # make sure $rdx <= 0xff
    cmpb    %dl, %cl                # j < i
    jb      pstrijcpy_error
    movb    (%rdi), %al
    cmpb    %al, %cl                # j >= dst->len
    jae     pstrijcpy_error
    movb    (%rsi), %al
    cmpb    %al, %cl                # j >= src->len
    jae     pstrijcpy_error
    inc     %rsi                    # char *psrc = src->str
    inc     %rdi                    # char *pdst = &dst->str[i]
pstrijcpy_loop:
    cmpb    %dl, %cl                # if j < i finish loop
    jb      pstrijcpy_done
    movb    (%rdx, %rsi), %al       # pdst[i] = psrc[i]
    movb    %al, (%rdx, %rdi)
    inc     %dl                     # i += 1
    jmp     pstrijcpy_loop          # loop
pstrijcpy_error:
    movq    $invalid_str, %rdi      # printf("invalid input!\n")
    xor     %rax, %rax
    call    printf
pstrijcpy_done:
    movq    %rdi, %rax              # return dst
    ret


    # %rdi = Pstring *str1
    # %rsi = Pstring *str2
    # %rdx = char i
    # %rcx = char j
pstrijcmp:
    and     $0xff, %rdx             # make sure $rdx <= 0xff
    cmpb    %dl, %cl                # j < i
    jb      pstrijcmp_error
    movb    (%rdi), %al
    cmpb    %al, %cl                # j >= pstr1->len
    jae     pstrijcmp_error
    movb    (%rsi), %al
    cmpb    %al, %cl                # j >= pstr2->len
    jae     pstrijcmp_error
    inc     %rsi                    # char *p2 = pstr2->str
    inc     %rdi                    # char *p1 = &pstr1->str[i]
pstrijcmp_loop:
    cmpb    %dl, %cl                # if j < i finish loop
    jb      pstrijcmp_ret_equal
    movb    (%rdx, %rsi), %al       # p2[i] <= p1[i]
    cmpb    %al, (%rdx, %rdi)
    je      pstrijcmp_postifs
    jg      pstrijcmp_else_if
    movl    $-1, %eax               # return -1
    ret
pstrijcmp_else_if:
    movl    $1, %eax                # return 1
    ret
pstrijcmp_postifs:  
    inc     %dl                     # i += 1
    jmp     pstrijcmp_loop
pstrijcmp_ret_equal:
    xor     %rax, %rax
    ret
pstrijcmp_error:
    movq    $invalid_str, %rdi      # printf("invalid input!\n")
    xor     %rax, %rax
    call    printf
    movl    $-2, %eax
    ret
