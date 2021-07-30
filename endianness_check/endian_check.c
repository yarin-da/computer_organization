#include "endian_check.h"

/* returns 1 if our machine uses little-endian, otherwise 0 */
int is_big_endian() {
    unsigned int value = 0x1;

    /* point to the smallest memory address of 'value' */
    unsigned char *p = (unsigned char *)&value;

    /* if the LSB is in the smallest memory, then we use little-endian */
    return *p != 0x1;
}

/* returns a long value that contains y's least significant half
   and x's most significant half */
unsigned long merge_bytes(unsigned long x, unsigned long int y) {
    /* compute sizes*/
    unsigned long size = sizeof(x);
    unsigned long half_size = size / 2;

    int i;
    for (i = 0; i < half_size; i++) {
        /* set half_size bytes from the LSB to 0x0 */
        x = put_byte(x, 0x0, size - 1 - i);
        /* set half_size bytes from the MSB to 0x0 */
        y = put_byte(y, 0x0, i);
    }

    /* merge x's and y's bits */
    return x | y;
}

/* returns a long value that contains x's original value with the MSB-i byte set to b */
unsigned long put_byte(unsigned long x, unsigned char b, int i) {
    /* i is out of bounds - we'll simply return x as it is */
    if (i < 0 || i >= sizeof(x)) {
        return x;
    }
    
    unsigned int is_little_endian = !is_big_endian();
    unsigned int bytes = sizeof(x);

    /* point to the byte in the lowest memory */
    unsigned char *msb = (unsigned char *)&x;

    /* if our machine uses big-endian, then we're already at the MSB
       is_little_endian will be FALSE (0), and we'll simply stay at the same address

       otherwise, we'd have to advance to the highest memory
       is_little_endian will be TRUE (1), and we'll add (bytes - 1) to our pointer */
    msb += is_little_endian * (bytes - 1);
    
    /* the position of i in relation to the MSB depends on our endianness
       for little endian machines we want to decrease our memory address */
    int op = is_little_endian ? -1 : 1;
    
    /* compute the exact address of the byte by advancing from MSB */
    unsigned char *requested_byte = msb + i * op;

    /* set the byte to b */
    *requested_byte = b;

    return x;
}
