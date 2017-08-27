#include <stdio.h>
#include <stdlib.h>

char bottles(char* a);
int checkvalid(char* b);

int main()
{
printf("Minutes:\n");
char m = getchar();
8
checkvalid(&m); //return an exit code (int) from checkvalid
char b = bottles(&m);
printf("\033[2J");
printf("Minutes:%c\n",m);
printf("Bottles:%c\n",b);
return 0;
}

char bottles(char* a) //*a is int m,
{
    int b = (*a) * 12; //b = bottles drank, char to input int literally (e.g '9' = 57)
    return b;
}

int checkvalid(char* b)
{
    if ((int) *b >58 || (int) *b <47) //if m is ASCII letter, return 1
    {
    printf("\033[2J");
    printf("enter a number\n");
    return 1;
    }
return 0;
}
