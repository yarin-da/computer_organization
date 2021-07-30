#ifndef ENDIAN_CHECK_H_
#define ENDIAN_CHECK_H_

int is_big_endian();
unsigned long merge_bytes(unsigned long x, unsigned long int y);
unsigned long put_byte(unsigned long x, unsigned char b, int i);

#endif
