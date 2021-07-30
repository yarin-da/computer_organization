#include <stdio.h>
#include <string.h>

/* function ret values */
#define ERR          (-1)
#define SUCCESS      (0)

/* carriage return and line feed */
#define CR           (0xd)
#define LF           (0xa)

/* we want to read multiple bytes per iteration
   to minimize to amount of calls to fread */
#define BUF_SIZE     (1024)
#define READ_SIZE    (512)

/* used for bit shifting */
#define BYTE         (8)
/* define a utf-16 character as 'short' for pointer type casting */
#define CHAR16_SIZE  (2)

/* typedefs */
typedef unsigned short uchar16;
typedef unsigned char  uchar8;
typedef enum { FALSE=0, TRUE }  bool;
typedef enum { MAC, UNIX, WIN } OS;

/* struct that holds the options requested by the user */
typedef struct {
    uchar8 *src_filename;
    uchar8 *dest_filename;
    /* used to determine which newlines to read and write */
    OS src_os;
    OS dest_os;
    /* used to handle cases when CRLF is not entirely in the buffer */
    bool partial_newline;
    /* case 2 - switch newlines between OS */
    bool change_newline;
    /* case 3 - swap bytes */
    bool swap;
    /* endianness of the input file */
    bool big_endian;
} Options;


/* original 'argv' and 'argc' from main and an uninitialized Options struct
   read arguments from the command line and set 'opt' accordingly 
   return SUCCESS if arguments are valid, otherwise ERR */
int readArgs(int argc, char *argv[], Options *opt) {
    if (argc < 3 || argc > 6) {
        /* not enough arguments (we require at least src/dest filenames) 
           or too many arguments (we require at most 5 arguments) */
        return ERR;
    }

    /* false by default, unless requested by the user */
    opt->change_newline = opt->swap = FALSE;
    /* we can assign filenames, because we know argc >= 3*/
    opt->src_filename = argv[1];
    opt->dest_filename = argv[2];

    /* flags to determine if the args provided are invalid */
    bool got_src_newline = FALSE;

    
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp("-swap", argv[i]) == 0) {
            opt->swap = TRUE;
        } else if (strcmp("-mac", argv[i]) == 0 ||
                   strcmp("-win", argv[i]) == 0 ||
                   strcmp("-unix", argv[i]) == 0) {
            /* set the os provided */
            OS os;
            if (strcmp("-mac", argv[i]) == 0) {
                os = MAC;
            } else if (strcmp("-win", argv[i]) == 0) {
                os = WIN;
            } else {
                os = UNIX;
            }

            /* use the flag to figure out which OS to assign */
            if (got_src_newline) {
                opt->dest_os = os;
                opt->change_newline = TRUE;
            } else {
                opt->src_os = os;
            }

            /* set the flag to its negative - we want it to be FALSE
             after reading 0 or 2 OS arguments */
            got_src_newline = !got_src_newline;
        }
    }

    /* if we're missing filenames, or got odd number of OS arguments */
    if (got_src_newline) {
        return ERR;
    }
    
    return SUCCESS;
}

/* simple swap function between 2 chars */
void swap(uchar8 *a, uchar8 *b) {
    uchar8 temp = *a;
    *a = *b;
    *b = temp;
}

/* check if the 2 first bytes of 'buf' hold the same value as 'ch' */
bool equal(uchar8 *buf, uchar16 ch, bool big_endian) {
    uchar16 buf_ch;
    /* the order of the bytes depends on the endianness */
    if (big_endian) {
        buf_ch = *(buf + 1) | *buf << BYTE;
    } else {
        buf_ch = *buf | *(buf + 1) << BYTE;
    }
    
    return buf_ch == ch;
}

/* return TRUE if the next characters in src_buf represent a newline
   buf_end is used to determine if CRLF is not entirely in src_buf  */
bool isNewline(uchar8 *src_buf, uchar8 *buf_end, Options *opt) {
    switch(opt->src_os) {
    case WIN:
        /* if we already read CR, then simply check if the next char is LF */
        if (opt->partial_newline) {
            opt->partial_newline = FALSE;
            return equal(src_buf, LF, opt->big_endian);
        }
        /* if we can only read one utf-16 char, then set opt->partial_newline */
        if (src_buf + CHAR16_SIZE == buf_end) {
            opt->partial_newline = equal(src_buf, CR, opt->big_endian);
            return FALSE;
        }

        /* check if the next 4 bytes are CRLF */
        return equal(src_buf, CR, opt->big_endian) 
            && equal(src_buf + CHAR16_SIZE, LF, opt->big_endian);
    case MAC:
        /* check if the next 2 bytes are CR */
        return equal(src_buf, CR, opt->big_endian);
    default:
        /* check if the next 2 bytes are LF*/
        return equal(src_buf, LF, opt->big_endian);
    }
}

/* write 'ch' to 'dest' and take endianness and swap in consideration */
void writeChar16(uchar8 *dest, uchar16 ch, bool big_endian, bool swap_bytes) {
    /* we're writing to indexes 0 and 1, the order depends on the value
       of big_endian
       big_endian and !big_endian are either 0 or 1
       therefore we can use them as an index */
    dest[big_endian] = ch & 0xff;
    dest[!big_endian] = (ch & 0xff00) >> BYTE;

    if (swap_bytes) {
        swap(dest, dest + 1);
    }
}

/* read 'src_buf' and write to 'dest_buf' according to the values in 'opt'
   return the numbers of bytes written to dest_buf */
size_t convertBytes(uchar8 *dest_buf, uchar8 *src_buf, size_t size, Options *opt) {
    uchar8 *src = src_buf;
    uchar8 *src_end = src_buf + size;
    uchar8 *dest = dest_buf;
    while (src < src_end) {
        /* save opt->partial_newline before the call to isNewline */
        bool advance_src = !opt->partial_newline;
        if (opt->change_newline && isNewline(src, src_end, opt)) {
            switch (opt->dest_os) {
            case WIN:
                writeChar16(dest, CR, opt->big_endian, opt->swap);
                /* we need to advance to the next utf-16 ch (CRLF) */
                dest += CHAR16_SIZE;
            case UNIX:
                writeChar16(dest, LF, opt->big_endian, opt->swap);
                break;
            default:
                writeChar16(dest, CR, opt->big_endian, opt->swap);
                break;
            }

            /* if CRLF has been read fully by isNewline
                then we can advance to the next utf-16 char */
            if (opt->src_os == WIN && advance_src) {
                /* we need to advance an extra utf-16 ch (CRLF) */
                src += CHAR16_SIZE;
            }
        } else {
            /* write one utf-16 character to dest in order */
            *dest = *src;
            *(dest + 1) = *(src + 1);

            if (opt->swap) {
                swap(dest, dest + 1);
            }
        }

        src += CHAR16_SIZE;
        /* don't advance when src_os is WIN and src_newline has been
            partially read */
        if (opt->src_os != WIN || !opt->partial_newline) {
            dest += CHAR16_SIZE;
        }
    }

    /* dest_buf is the start address, therefore dest - dest_buf 
       is the amount of bytes are wrote into the destination buffer */
    return dest - dest_buf;
}

int main(int argc, char *argv[]) {    
    /* initialize opt according to cmd args */
    Options opt;
    if (readArgs(argc, argv, &opt) == ERR) {
        /* invalid arguments - we exit the program */
        return ERR;
    }
    
    FILE *input_file = fopen(opt.src_filename, "rb");
    if (input_file == NULL) {
        /* input_file doesn't exist - we exit the program */
        return ERR;
    }
    
    FILE *output_file = fopen(opt.dest_filename, "wb");

    size_t bytes_read;
    size_t bytes_written;
    uchar8 read_buf[BUF_SIZE];
    uchar8 write_buf[BUF_SIZE];

    /* read BOM */
    opt.big_endian = TRUE;
    uchar8 bom[CHAR16_SIZE];
    bytes_read = fread(&bom, sizeof(uchar8), CHAR16_SIZE, input_file);
    if (bytes_read == CHAR16_SIZE) {
        /* the file is in little endian if BOM = FFFE 
            if BOM is missing - assume the file is in big endian
            according the recommendation in RFC 2781 */
        opt.big_endian = *bom != 0xff || *(bom + 1) != 0xfe;
        bytes_written = convertBytes(write_buf, bom, bytes_read, &opt);
        fwrite(write_buf, sizeof(uchar8), CHAR16_SIZE, output_file);
    }
    
    do {
        /* read READ_SIZE bytes
           the more the better, because fread is expensive */
        bytes_read = fread(read_buf, sizeof(uchar8), READ_SIZE, input_file);
        /* convert our input according to opt */
        bytes_written = convertBytes(write_buf, read_buf, bytes_read, &opt);
        fwrite(write_buf, sizeof(uchar8), bytes_written, output_file);
        /* finish after we read less than READ_SIZE
           which means we reached EOF */
    } while (bytes_read == READ_SIZE);

    /* close the files we opened */
    fclose(input_file);
    fclose(output_file);

    return SUCCESS;
}
