#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>


int i,j,k,l;
typedef uint8_t BYTE;

enum settings
{
    BLOCK = 512,
    MAX_CHAR = 240,
};

typedef struct{
    BYTE b1;
    BYTE b2;
    BYTE b3;
    BYTE b4;
    BYTE b5[508];
} __attribute__((__packed__))
HEADER;

HEADER ohe;

char* outname;
char** cli;

char* infile;
FILE* inptr;
FILE* outptr;
int fcount = 0, open = 0;



int main(int argc, char* argv[])
{
    if (argc != 2){
        cli = argv;
        fprintf(stderr, "Usage:./%.*s <INPUT>\n", (int)(".c" - __FILE__), __FILE__);
        return 1;
    }

    infile = argv[1];
    inptr = fopen(infile, "rb");

    if (inptr == NULL){
        perror("Error ");
        return 2;
    }

    outname = malloc(MAX_CHAR * sizeof(char));
    snprintf(outname, (9 * sizeof(char)), "%.3i.jpg", fcount);
    outptr = fopen(outname, "wb");
    fseek(inptr, 0, SEEK_SET);

    // apparently feof returns non-zero when EOF is ALREADY reached,
    // not if the next loop will reach EOF
    while(fread(&ohe, BLOCK, 1, inptr) == 1){
        fseek(inptr, -BLOCK, SEEK_CUR);
        fread(&ohe, BLOCK, 1, inptr);
        //if jpg signature detected
        if (ohe.b1 == 0xff &&
            ohe.b2 == 0xd8 &&
            ohe.b3 == 0xff &&
            (ohe.b4 & 0xe0) == 0xe0){
                //if outfile already open,
                //close and make newfile
                if (open == 1){
                    fclose(outptr);
                    fcount++;
                    snprintf(outname, (9 * sizeof(char)), "%.3i.jpg", fcount);
                    fopen(outname, "wb");
                    fwrite(&ohe, BLOCK, 1, outptr);
                }
                //if outfile not open
                //(i.e the first jpg detected, after factory init zero-bytes)
                else if (open == 0){
                    open = 1;
                    fwrite(&ohe, BLOCK, 1, outptr);
                }
                continue;
            }
        //write ONLY if jpg signature has been written
        else if (open == 1){
            fwrite(&ohe, BLOCK, 1, outptr);
        }
    }

fclose(outptr);
fclose(inptr);
return 0;
}


//make inptr
//read inptr header
//set header count to 1
//write
//if new header found (ie header count alr 1)
    //close outfile
    //make new outfile
//continue
//if infile feof
    //return 0
    //end