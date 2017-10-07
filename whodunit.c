#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

void error(int err);
void stats(BITMAPFILEHEADER a, BITMAPINFOHEADER b);
void clean (FILE* inptr, FILE* outptr);

enum _error_list
{
CLI_ERR_USAGE   = 1,   // cli arg not 2
FREAD_ERR_I     = 2,   // input cannot read
FWRITE_ERR_O    = 3,   // output cannot write
FREAD_ERR_I_C   = 4,   // input is compressed
FREAD_ERR_I_F   = 5,   // input is not bitmap format
};


BITMAPFILEHEADER bf;
BITMAPINFOHEADER bi;
RGBTRIPLE bgr;

int main(int argc, char *argv[]) {
    if (argc != 3){
        error(1);
        return 1;
    }

char* infile = argv[1];
char* outfile = argv[2];

FILE* inptr = fopen(infile, "r");
FILE* outptr = fopen(outfile, "w");

    if (inptr == NULL){
        error(2);
        return 2;
    }
    else if (outptr == NULL){
        fclose(inptr);
        error(3);
        return 3;
    }

fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    if (bi.biHeight == 0){
        error(4);
        return 4;
    }
    else if (bf.bfType != 19778){
        error(5);
        return 4;
    }
stats(bf, bi);
clean(inptr, outptr);

return 0;
}


void error(int err)
{
    switch(err) {
        case CLI_ERR_USAGE  :
        fprintf(stderr,
        "Usage: ./%.*s [input] [output]\n", (int)(".c" - __FILE__), __FILE__);
        break;

        case FREAD_ERR_I    :
        fprintf(stderr,
        "Cannot open input.\n");
        break;

        case FWRITE_ERR_O   :
        fprintf(stderr,
        "Cannot create output.\n");
        break;

        case FREAD_ERR_I_C  :
        fprintf(stderr,
        "(Invalid input format)\nInput must be uncompressed, 24-bit BMP 4.0 format\n");
        break;

        case FREAD_ERR_I_F  :
        fprintf(stderr,
        "(Invalid input format)\nInput must be .bmp format.\n");
        break;
    }
return;
}

void stats(BITMAPFILEHEADER a, BITMAPINFOHEADER b)
{
    fprintf(stdout, "\nBMP, %hu bits ", b.biBitCount);
    if (b.biHeight > 0){
        fprintf(stdout, "(bottom-top,");
    }
    else {
        fprintf(stdout, "(top-bottom,");
    }
    if(b.biCompression == 0){
        fprintf(stdout, " lossless)\n");
    }
    else{
        fprintf(stdout, " %u)\n", b.biCompression);
    }

    int c = (int)a.bfSize;
    if (c >= 1024 && c < (1024 * 1024)){
        fprintf(stdout, "Filesize: %uKB\n", (int)(a.bfSize / (1024)) );
    }
    else if (c >= (1024 * 1024)){
        fprintf(stdout, "Filesize: %.3uMB\n", (int)(a.bfSize / (1024 * 1024)) );
    }
    else if (c < 1024){
        fprintf(stdout, "Filesize: %uB\n", (int)a.bfSize );
    }

    fprintf(stdout, "height: %dpx\n", abs(b.biHeight));
    fprintf(stdout, "width: %dpx\n", b.biWidth);
return;
}

void clean (FILE* inptr, FILE* outptr)
{
fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
int padding = (4 - (bi.biWidth % 4)) % 4;

    for (int i = 0, k = abs(bi.biHeight); i < k; i++){
        for (int j = 0; j < bi.biWidth; j++){
            fread(&bgr, sizeof(RGBTRIPLE), 1, inptr);
            // bgr = current pixel array
            // you're editing the array's value
            // cursor is still at the end of this RGBTRIPLE array.
            // fread has nothing to do with the below function
            // 0xffffff = white, 0x000000 = black

            if (bgr.rgbtRed == 0xff){
                fseek(inptr,  -(int)sizeof(RGBTRIPLE), SEEK_CUR);
                for (int c = 0; c < 3; c++){
                    putc(0xff, outptr);
                }
                fread(&bgr, sizeof(RGBTRIPLE), 1, inptr);
            }
            else {
                bgr.rgbtBlue = 0x00;
                bgr.rgbtGreen = 0x00;
                bgr.rgbtRed = 0x00;
                fwrite(&bgr, sizeof(RGBTRIPLE), 1, outptr);
            }


        }
        fseek(inptr, padding, SEEK_CUR);
        for (int p = 0; p < padding; p++){
            fputc(0x00, outptr);
        }
    }
fclose(inptr);
fclose(outptr);
return;
}