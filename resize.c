#include <cs50.h>
#include <stdio.h>
#include <math.h>

#include "bmp.h"

void upscale(void);
void downscale(void);
void init(void);

BITMAPFILEHEADER ibf, obf;
BITMAPINFOHEADER ibi, obi;
RGBTRIPLE bgr;

char** cli;
FILE* inptr;
FILE* outptr;

float f = 0;
int i , j, k, l, m, n, o, p, padding, fpadding;

int main(int argc, char* argv[])
{

    f = atof(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];
    cli = argv;
    inptr = fopen(infile, "r");
    outptr = fopen(outfile, "w");
    fread(&ibf, sizeof(BITMAPFILEHEADER), 1, inptr);
    fread(&ibi, sizeof(BITMAPINFOHEADER), 1, inptr);
    //padding = B
    //biWidth = px
    padding = (4 - ((ibi.biWidth * sizeof(RGBTRIPLE)) % 4)) % 4;

        if (f < 1.00 && f >= 0.00){
            downscale();
        }
        else {
            upscale();
        }
        //init();
    fclose(inptr);
    fclose(outptr);

    if (argc != 4){
        return 1;
    }
return 0;
}

void init(void)
{
    fwrite(&ibf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&ibi, sizeof(BITMAPINFOHEADER), 1, outptr);

    for(i = 0; i < abs(ibi.biHeight); i++){
        for(j = 0; j < abs(ibi.biWidth); j++){
            fread(&bgr, sizeof(RGBTRIPLE), 1, inptr);
            fwrite(&bgr, sizeof(RGBTRIPLE), 1, outptr);
        }
        fseek(inptr, padding, SEEK_CUR);
        for(k = 0; k < padding; k++){
            putc(0x00, outptr);
        }
    }
}



void upscale(void)
{
    fseek(inptr, 0 , SEEK_SET);
    fread(&obf, sizeof(BITMAPFILEHEADER), 1, inptr);
    fread(&obi, sizeof(BITMAPINFOHEADER), 1, inptr);
    obi.biHeight *= f;
    obi.biWidth *= f;
    //fpadding is padding PER LINE
    fpadding = (4 - ((obi.biWidth * sizeof(RGBTRIPLE)) % 4)) % 4;
    //biSizeImage = pixel * width * height * padding for EVERY line
    obi.biSizeImage = (sizeof(RGBTRIPLE) * abs(obi.biHeight) * obi.biWidth) + (fpadding * abs(obi.biHeight));
    obf.bfSize = obf.bfOffBits + obi.biSizeImage;
    fwrite(&obf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&obi, sizeof(BITMAPINFOHEADER), 1, outptr);


    RGBTRIPLE *scanline[ibi.biWidth + 1];


    for(i = 0; i < abs(ibi.biHeight); i++){

        //apparently fread(scanline, sizeof(RGBTRIPLE), ibi.biWidth, inptr)
        //doesn't work (why can't you read to a struct array, n elements at one shot?)
        for (k = 0; k < abs(ibi.biWidth); k++){
            fread(&scanline[k], sizeof(RGBTRIPLE), 1, inptr);
        }
        for (p = 0; p < f; p++){
            for(k = 0; k < abs(ibi.biWidth); k++){
                for (j = 0; j < f; j++){
                    fwrite(&scanline[k], sizeof(RGBTRIPLE), 1, outptr);
                }
            }
            for(l = 0; l < fpadding; l++){
                fputc(0x00, outptr);
            }
        }
    fseek(inptr, padding, SEEK_CUR);
    }
return;
}

void downscale(void)
{
    fseek(inptr, 0 , SEEK_SET);
    fread(&obf, sizeof(BITMAPFILEHEADER), 1, inptr);
    fread(&obi, sizeof(BITMAPINFOHEADER), 1, inptr);
    obi.biHeight *= f;
    obi.biWidth *= f;
    fpadding = (4 - ((obi.biWidth * sizeof(RGBTRIPLE)) % 4)) % 4;
    obi.biSizeImage = (sizeof(RGBTRIPLE) * abs(obi.biHeight) * obi.biWidth) + (fpadding * abs(obi.biHeight));
    obf.bfSize = obf.bfOffBits + obi.biSizeImage;
    fwrite(&obf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&obi, sizeof(BITMAPINFOHEADER), 1, outptr);


    RGBTRIPLE* scanline[ibi.biWidth];

    int iheightct = 0;
    float iwidthct = 0, iwidthctmin = 0, iwidthctmax = 0;
    float f4 = 1 / (f);


    for (i = 0; i < abs(obi.biHeight); i++){

        //initial scanline read
        fprintf(stdout, "input row %i\n", iheightct + 1);
        for (o = 0; o < ibi.biWidth; o++){
            fread(&scanline[o], sizeof(RGBTRIPLE), 1, inptr);
        }
        fseek(inptr, padding, SEEK_CUR);
        iheightct++;

        //write scanline[k/f] to outptr[k]
        for (k = 0; k < obi.biWidth; k++){
            iwidthctmin = (float)k * f4;
            iwidthctmax = (float)(k + 1) * f4;

            //  out     in
            // 1 px = (1/f) px
            // ++++++++++++++++++++++
            //  1  |  2  |  3  |  4     input
            // ++++++++++++++++++++++
            // <-1/f-><-1/f->
            // ++++++++++++++++++++++
            //  1     |    2    |       output
            // ++++++++++++++++++++++
            //
            // output px 1 = {px 0 - 1.25} (input)
            // output px 1 = ceil(middle of {0 - 1.25})
            // output px 2 = {px 1.25 - 2.5} (input)
            // output px 2 = ceil(middle of {1.25 - 2.5})

            iwidthct = round((iwidthctmin + iwidthctmax) / 2);
            fwrite(&scanline[(int)iwidthct], sizeof(RGBTRIPLE), 1, outptr);
        }
        for (j = 0; j < fpadding; j++){
            fputc(0x00, outptr);
        }

        float iheightctmin = (i + 1) * f4;
        float iheightctmax = (i + 2) * f4;
        float iheightctavg = round((iheightctmin + iheightctmax) / 2);

        for (n = iheightct; n < iheightctavg; n++){
            fseek(inptr, (int)(ibi.biWidth * sizeof(RGBTRIPLE)), SEEK_CUR);
            fseek(inptr, padding, SEEK_CUR);
            iheightct++;
        }

    }
return;

}

