#include "pstring.h"
#include <stdio.h>

char pstrlen(Pstring* pstr) {
	return *(unsigned char *)pstr;
}

Pstring* replaceChar(Pstring* pstr, char oldChar, char newChar) {
	unsigned char *p = (unsigned char *)pstr + 1;
	while (*p != '\0') {
		if (*p == oldChar) {
			*p = newChar;
		}
		p += 1;
	}
    return pstr;
}

Pstring* swapCase(Pstring* pstr) {
	const char diff = 'a' - 'A';
	unsigned char *p = (unsigned char *)pstr + 1; 
	while (*p != '\0') {
		if (*p >= 'a' && *p <= 'z') {
			*p -= diff;
		} else if (*p >= 'A' && *p <= 'Z') {
			*p += diff;
		}
		p += 1;
	}
}

Pstring* pstrijcpy(Pstring* dst, Pstring* src, char i, char j) {
	if (i < 0 || j < 0 || i > j ||
        j >= src->len || j >= dst->len) {
		printf("invalid input!\n");
	} else {
        unsigned char *ps = (unsigned char *)src + 1;
        unsigned char *pd = (unsigned char *)dst + 1;
        while (i <= j) {
            pd[i] = ps[i];
            i += 1;
        }
    }
    
	return dst;
}


int pstrijcmp(Pstring* pstr1, Pstring* pstr2, char i, char j) {
	if (i < 0 || j < 0 || i > j ||
        j >= pstr1->len || j >= pstr2->len) {
		printf("invalid input!\n");
        return -2;
	}
    
    unsigned char *p1 = (unsigned char *)pstr1 + 1;
    unsigned char *p2 = (unsigned char *)pstr2 + 1;
    while (i <= j) {
        if (p1[i] > p2[i]) {
            return 1;
        } else if (p1[i] < p2[i]) {
            return -1;
        }
        i += 1;
    }
	return 0;
}

