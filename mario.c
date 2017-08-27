#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    printf("Height:\n");
    int height = get_int();
    int startd = 2;                              //starting no. of '#'
    {
        if (height < 0 || height >= 'a')        //if get_int is a negative value or a ASCII letter
        {
            printf("Retry:\n");
        }
        else if (height > 23 || height == 0)    //if get_int is '0', or greater than 24
        {
            return 2;                           //abort program
        }
    }

for (int n = 0; n < height; n++)            //n = line iteration
    {
        int d;                              //d = no. of # printed (current line)
        int dneed = n + startd;             //dneed = no. of # needed (current line + starting no. of '#')
        int maxwidth = height + startd;     //maxwidth = max width of pyramid
        int s = 0;                          //s = no. of '' printed (current line)
        int sneed = maxwidth - dneed;       //sneed = no. of '' needed
        if (s == 0)                         //print current line (starting from 1, not 0) before everything else
        {
            printf("(%i)",n + 1);
        }

        for (s = 0; s < sneed; s++)            //print "" until needed
        {
            printf(" ");
        }

        for (d = 0; d < dneed; d++)            //print '#' until needed
        {
            printf("#");
        }

        if (d == dneed && d > 0)                 //when total '#' = line number, newline
        {
            printf("\n");
        }
    }
}