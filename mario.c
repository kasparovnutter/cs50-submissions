#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFLEN 10
#define MINSIZE 0
#define MAXSIZE 23

// REMEMBER STDIN IS STORED AS CHARS
// "0" from keyboard = "48"
//  ' ' = character (string literal, no null terminating char)
// " " = string array (with null terminating char);

// snprintf doesn't handle newlines or whitespace for some reason

static int get_int(char* output, char* buffer, unsigned int size){

    if (output){
        printf("%s",output);
    }
    if (fgets(buffer, size, stdin)==NULL){
        fprintf(stderr,"Invalid height.\n");
        return 1;
    }
    int buflen = strlen(buffer);
    int bufferint = 0;
    if (strcmp(&buffer[buflen],"\n") == 0|| strcmp(&buffer[buflen],"\0") ==0 ){
           buflen--;
    }
    buflen = buflen <1?1:buflen;

    for (int i =0 ; i < buflen; i++){
        if (isdigit(buffer[i]) == 0 || bufferint > MAXSIZE){
            fprintf(stderr, "We only accept numbers from %i-%i here.\n", MINSIZE, MAXSIZE);
            return 1;
        }
        bufferint *= 10;
        bufferint += strtol(&buffer[i], NULL, 10);
    }

    return 0;
}
int main(void){
    char buffer[10];
    while(get_int("test:", buffer, sizeof(buffer))){

    };
    int buflen = strtol(buffer, NULL, 10);
    for(int i=0;i< buflen;i++){

        // padding starts from ROW 1 not 0
        int padding = (buflen - (i+1));
        char* rowpadding=calloc(sizeof(char*),1);
        char* rowhash=calloc(sizeof(char*),1);
        char* row =calloc(sizeof(char*),1);
        for(int i2=0; i2<padding;i2++){
            rowpadding[i2] = ' ';
        }
        for (int i2=0;i2<=i;i2++){
            rowhash[i2] = '#';
        }

        printf("%s%s  %s\n", rowpadding,rowhash,rowhash);

    }
    return 0;
}
