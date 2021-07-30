#include "pstring.h"
#include <stdio.h>

void run_func(int opt, Pstring *p1, Pstring *p2) {
    int i, j;
    char oldChar, newChar;
    switch (opt) {
    case 50:
    case 60:
        printf("first pstring length: %d, second pstring length: %d\n", pstrlen(p1), pstrlen(p2));
        break;
    case 52:
        scanf("%c", &oldChar);
        scanf("%c", &newChar);
        replaceChar(p1, oldChar, newChar);
        replaceChar(p2, oldChar, newChar);
        printf("old char: %c, new char: %c, first string: %s, second string: %s\n", oldChar, newChar, p1->str, p2->str);
        break;
    case 53:
        scanf("%d", &i);
        scanf("%d", &j);
        pstrijcpy(p1, p2, i, j);
        printf("length: %d, string: %s\n", p1->len, p1->str);
        printf("length: %d, string: %s\n", p2->len, p2->str);
        break;
    case 54:
        swapCase(p1);
        swapCase(p2);
        printf("length: %d, string: %s\n", p1->len, p1->str);
        printf("length: %d, string: %s\n", p2->len, p2->str);
        break;
    case 55:
        scanf("%c", &i);
        scanf("%c", &j);
        printf("compare result: %d\n", pstrijcmp(p1, p2, i, j));
        break;
    default:
        puts("invalid option!\n");
        break;
    }
}
