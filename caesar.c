//assume only single command-line arg
//assume non-negative integer (k)
//reject zero or more than one arg (return 1 from main)

//assume k is <= 2^31 - 26
//assume k modulus 26 (e.g it wraps around the alphabet)
//preserve casing (e.g FucK must still be YjhN)

//include
//printf("plaintext:") (w/o newline or spacing)
//then get_string
//then printf("ciphertext:%s", ciphertext)
//then printf"\n"
//then return 0


#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char* argv[])
{
    if (argc <= 0 || argc >= 3)
    {
        printf("error\n");
        return 1;
    }

printf("plaintext:");
string input = get_string();
const int textlen = strlen(input);

int k = atol(argv[1]);

char parray[textlen + 1];

    for (int i = 0; i < textlen; i++)
    {
        parray[i] = input[i];
        if ((int) parray[i] >= 0 && (int) parray [i] < 65 && parray[i] != 32)
        {
            printf("error\n");
            return 1;
        }
        else if ((int) parray[i] >= 91 && (int) parray [i] < 97)
        {
            printf("error\n");
            return 1;
        }
        else if ((int) parray[i] >= 123 && (int) parray [i] <= 127)
        {
            printf("error\n");
            return 1;
        }

        //print the array characters one-by-one
        //printf("%c", parray[i]);
        //if (i == (textlen -1))
        //{
        //    printf("\n");
        //}
    }

char carray[textlen + 1];
int cycle = abs(k) % 26; //just wrap around the alphabet, casing comes automatically (e.g max positions shifted = 25, since 26 % 26 = 0)
for (int i = 0; i < textlen + 1; i++)
    {
        int lettertemp = (int) parray[i];
        int letter = lettertemp + cycle;
        if (letter >= 91 && lettertemp < 91)    //if cipher > 'Z' and plain is uppercase (<=90)
        {
            int letterover = letter % 90;
            letter = 64 + letterover;           // 'A' = 65 (e.g '~' = 91, over 1 letter = 64 + 1 = 'A')
        }
        else if (letter >= 123 && lettertemp < 123) //if cipher > 'z' and plain is lowercase (<=122)
        {
            int letterover = letter % 122;
            letter = 96 + letterover;
        }
        else if (lettertemp < 65)
        {
            letter = lettertemp;
        }
        else if (lettertemp == '\0')
        {
            letter = lettertemp;
        }
        carray[i] = (char) letter;
    }

printf("ciphertext:%s", carray);
printf("\n");
return 0;
}
