#include <stdio.h>
#include <cs50.h>
#include <ctype.h>
#include <string.h>
int main(void){

    char* c = get_string();
    char* d = calloc(1, sizeof(char) * sizeof(c));
    for (int i = 0, k = 0; i <= strlen(c); i++){
        if (isalpha(c[i])){
            if (((isspace(c[i - 1]) || (ispunct(c[i - 1]))) && i > 0) || i == 0){
                d[k] = toupper(c[i]);
                k++;
            }
            while (isalpha(c[i])){
                    i++;
            }
        }
    }


    fprintf(stdout,"%s\n", d);
    EXIT_SUCCESS;

}