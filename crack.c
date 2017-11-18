#define _XOPEN_SOURCE
#include <stdio.h>
//#include <stdlib.h>
//#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <cs50.h>

char charset[] = "ABCDEFGHIJKLMNOPQRSTUWXYZ" "abcdefghijklmnopqrstuwxyz" "0123456789";
//sizeof returns len of its byte string, including the null terminator
int charsetlen = sizeof(charset)/sizeof(charset[0]) - 1;

bool test(int* count, int pos, int* maxlen, char*c, char* target){
    while (*count != 0){
        int pos2 = pos - 1;
        if (pos <= *count){
                if (pos == 1){
                    for (int i = 0; i < charsetlen; i++){
                        c[pos - 1] = charset[i];
                        char* chash = crypt(c, "50");
                        if (strcmp(chash, target) == 0){
                            fprintf(stdout,"pw:%s\n", c);
                            *count = 0;
                            return true;
                        }
                        fprintf(stdout,"count:%i\tpos:%i\tmaxlen:%i\thash:%s (%s)\n",*count,pos,*maxlen,chash, c);
                    }
                    if (*count > 1){
                        return 0;
                    }
                    else if (*count > *maxlen){
                        fprintf(stdout, "password longer than %i characters\n", *maxlen);
                        *count = 0;
                    }
                    (*count)++;
                    return 0;
                }
                //else if (pos != 1)
                else if (pos >1){
                    for (int i = 0; i < charsetlen; i++){
                        c[pos - 1] = charset[i];
                        test(count, pos2, maxlen, c, target);
                    }
                    if (pos == *count && *count < *maxlen){
                        fprintf(stdout,"count:%i\n", *count);
                        (*count)++;
                        fprintf(stdout,"count:%i\n", *count);
                    }
                    else if (*count > *maxlen){
                    fprintf(stdout, "password longer than %i characters\n", *maxlen);
                    *count = 0;
                    }
                return 0;
                }

        }

        while (pos > *count && *count != 0){
            c[pos] = '\0';
            test(count, pos2, maxlen, c, target);
        }

        continue;
    }
return 0;
}



int main(int argc, char* argv[]){
    int maxlen = 4, counta = 1;
    int *countb = &counta;
    if (argc != 2){
        fprintf(stderr, "usage: ./%.*s [hashed pw]\n", (int)(".c" - __FILE__), __FILE__);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "argc:%i\t\t*argv:%s\n",argc, *argv);


    char* c = calloc(maxlen + 1, sizeof(char));

    //for (int k = 0; k < maxlen; k++){
            //for (int i = 0; i < 26; i++){
            //c[k] = i + 'a';
        //}
    //}

    test(countb, maxlen, &maxlen,c, argv[1]);
        fprintf(stdout, "%i\n", charsetlen);
    free(c);
    return 0;
}