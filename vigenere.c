// k = argv[1]
// reject if argc <= 1; argv[1]

#include <stdio.h>
#include <ctype.h>
#include <cs50.h>
#include <string.h>

int alphnum(char a);

int main(int argc, char* argv[])
{
    if (argc == 1 || argc > 2 || isalpha(*argv[1]) == 0)
    {
    printf("Error\n");
    return 1;
    }

    //get input
    printf("plaintext: ");
    char* input = get_string();
    int plen = strlen(input);
    int klen = strlen(argv[1]);
    char carr[plen + 1];
    int cyclepause = 0;

    for (int i = 0; i < plen + 1; i++)
    {
        char pchar = input[i];
        int cycle = cyclepause % klen; //klen does not include null character ("fuck" = 4 letters strlen)
        char kchar = argv[1][cycle];
        int kshift = alphnum(kchar);
        int cnum = 0;
        cnum = (int) pchar + kshift;
        if ((int) pchar >= 65 && (int) pchar <= 90)
        {
            cnum %= 64;
            cnum %= 26;
            if (cnum > 0)
            {
                cnum += 64;
            }
            else
            {
                cnum += 90;
            }
            cyclepause += 1;
        }
        else if ((int) pchar >= 97 && (int) pchar <= 122)
        {
            cnum %= 96;
            cnum %= 26;
            if (cnum > 0)
            {
                cnum += 96;
            }
            else
            {
                cnum += 122;
            }
            cyclepause += 1;
        }

        else if ((int) pchar < 65 || ((int) pchar >= 91 && (int) pchar <= 96) || (int) pchar >= 123)
        {
            cnum = (int) pchar;
            cyclepause = cycle;
        }
        char cchar = (char) cnum;
        carr[i] = cchar;
    }


printf("ciphertext: %s\n", carr);

return 0;
}



int alphnum(char a)
{
    int b = (int) a;

    if (b >= 65 && b <= 90)
    {
        b %= 65;
        return b;
    }
    else if (b >= 97 && b <= 122)
    {
        b %= 97;
        return b;
    }
    return 0;
}