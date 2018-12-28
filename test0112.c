#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


struct HEADER{
    unsigned char ID[4];         //struct members must be allocated memory first  (ie char* id wont work);
    unsigned char size[4];
    unsigned char format[2];
    unsigned char trackamt[2];
    signed char deltadivision[2];
};

struct TRACKCHUNK{
    unsigned char ID[4];
    unsigned char size[4];
    struct TRACKEVENTS* track;
};


struct MIDI{
    unsigned char* delta;
    unsigned char status;
    unsigned char data1;
    unsigned char data2;
};
struct META{
    unsigned char* delta; //flexible length array for delta times
    unsigned char ID;    // 0xFF
    unsigned char type;  // meta type (refer to list)
    unsigned char* size; // length of data
    unsigned char* data;
};

struct SYSEX{
    unsigned char* delta;
    unsigned char ID;    // 0xF0 or 0xF7
    unsigned char* size;
    unsigned char* data;
    unsigned char ID2;   // 0xF7
};

struct TRACKEVENTS{
    struct MIDI* midievents;
    struct META* metaevents;
    struct SYSEX* sysexevents;
    int skipped;
};



unsigned int processdelta(FILE* fp);
int variablelen(FILE*fp, int* cur);

struct HEADER* processheader(FILE* fp);
struct TRACKCHUNK* processtrackchunk(FILE* fp);
struct META* processmeta(FILE*fp, int deltatime, unsigned char IDtemp);
struct MIDI* processmidi(FILE*fp, int deltatime, unsigned char IDtemp);
struct SYSEX* processsysex(FILE*fp, int deltatime, unsigned char IDtemp);
struct TRACKEVENTS* processtrackevents(FILE* fp, unsigned int len);




int main(int argc, char* argv[]){


    FILE* fptr = NULL;
    if (argc >=2){
        char path[256]; strcpy(path,"./");
        fptr = fopen(strcat(path, (argv[1])), "rb");
    }
    else{
        fptr = fopen("./sample.mid", "rb");
    }

    if (fptr == NULL){
        fprintf(stderr, "Error: Invalid file type\n");
        return 1;
    }
    // filesize (cursor goes to eof)
    {
        fseek(fptr,0,SEEK_END);
        int tempsize = ftell(fptr);
        printf("FILESIZE: %ikB\n", tempsize /= 1e3);
    }

    struct HEADER* header = processheader(fptr);



    unsigned int trackamt = 0;
    for (int i=0; i<sizeof(header->trackamt); i++){
        trackamt<<=8; trackamt += header->trackamt[i];
    }
    struct SONGEVENTS* songevents = malloc(sizeof(struct SONGEVENTS*) * trackamt);
    for (int i =0; i < trackamt; i++){
        //printf("\n||||||BYTE:%.2lx (%.2i/%.2i)|||||\n", ftell(fptr), i, trackamt);
        printf("==============================current track:%i/%i==================\n",i+1,trackamt);
        struct TRACKCHUNK* trackchunk = processtrackchunk(fptr);

        int len = 0;
        for (int i2=0; i2 < sizeof(trackchunk->size); i2++){
            len = len<< 8; len += trackchunk->size[i2];
        }

        struct TRACKEVENTS* trackevents = processtrackevents(fptr, len);
        printf("\n END OF TRACK\n");
        printf("BYTES SKIPPED: %.2x\n", trackevents->skipped);
    }


    return 0;

}

struct HEADER* processheader(FILE* fp){

    //malloc struct
    struct HEADER* headertemp = malloc(sizeof(struct HEADER));
    //printf("HEADER*: %lu | HEADER: %lu\n", sizeof(struct HEADER*), sizeof(struct HEADER));
    if (headertemp == NULL){
        perror("Error: ");
        fclose(fp);
        exit(EXIT_FAILURE);
    }
   // fscanf looks for strings, use freadf for binary files

    // check for EOF meta event (cursor alr at SEEK_END)
    fseek(fp, -3, SEEK_CUR);
    unsigned char diu[4] = "";


    fread(diu, 1,3, fp);
    if (diu[0] != 0xFF || diu[1] != 0x2F || diu[2] != 0x00){
        fprintf(stderr, "Invalid file (No EOF)\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_SET);

    // loads header into struct
    fread(headertemp, sizeof(struct HEADER), 1,  fp);
    if (headertemp->ID[0] != 0x4D || headertemp->ID[1] != 0x54 || headertemp->ID[2] != 0x68 || headertemp->ID[3] != 0x64){
        fclose(fp);
        fprintf(stderr, "Invalid file (Invalid header)\n");
        exit(EXIT_FAILURE);
    }

    // c99 only can init variables one at a time (ie int a,b,c =0; only initializes the first variable)
    int size, format,trackamt; signed int deltadivision;
    size=format=trackamt=deltadivision=0;

    // GETTING VALUES FROM STRUCT ELEMENTS
    for (int i = 0; i< sizeof(headertemp->size); i++){
        size = size << 8;
        size += headertemp->size[i];
    }
    for (int i = 0; i< sizeof(headertemp->format); i++){
        format = format << 8;
        format += headertemp->format[i];
    }
    for (int i = 0; i< sizeof(headertemp->trackamt); i++){
        //trackamt = trackamt << 8;
        trackamt += headertemp->trackamt[i];
        printf("%c", headertemp->trackamt[i]);
    }

    // 0 = ticks per quarter-note
    // 1 = negative SMPTE format (14-8) | ticks per frame (7-0)
// (23-12) IDK IF THIS WORKS, HANDLING DELTA DIVISION TIMING
   if (headertemp->deltadivision[0] & 0x80){
        deltadivision += (headertemp->deltadivision[0] & 0x7F);
        deltadivision <<= 8;
        deltadivision += headertemp->deltadivision[1];
   }
   else{
        for (int i = 0; i< sizeof(headertemp->deltadivision); i++){
            deltadivision = deltadivision << 8;
            deltadivision += headertemp->deltadivision[i];
        }
        headertemp->deltadivision[0] = (unsigned int)deltadivision;
   }
    char* formattext = malloc(sizeof(char)*20);

    switch(format) {
        case(0):
            formattext = "single track";
            break;
        case(1):
            formattext = "multi track";
            break;
        case(2):
            formattext = "multi pattern";
            break;
    }


    //printf("*headertemp: %lu\n, headertemp: %lu\n", sizeof(*headertemp), sizeof(headertemp));
    //printf("*headertemp->format: %lu\n, headertemp->format: %lu\n", sizeof(*headertemp->format), sizeof(headertemp->format));
    printf("<HEADER> (%lu) \n ID:%s | size:%.2x | format:%.2x (%s) | trackamt:%.2x | deltadivision:%.2X PPQ (unsigned hex in printf)\n",
    sizeof(*headertemp),headertemp->ID, size, format,formattext,trackamt
    , headertemp->deltadivision[1]? deltadivision & 0xFF: deltadivision);
    for (int i =0;i<48;i++){
        printf("=");
    }
    printf("\n");
    return headertemp;
}

struct TRACKCHUNK* processtrackchunk(FILE* fp){
    //printf("input byte:%.2X\n", fgetc(fp)); fseek(fp, -1, SEEK_CUR);

    //malloc struct
    struct TRACKCHUNK* tracktemp = malloc(sizeof(struct TRACKCHUNK));
    if (tracktemp == NULL){
        perror("Error: ");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // reading 8 instead of sizeof(struct TRACKCHUNK) cause extra TRACKEVENTS* member
    fread(tracktemp, 8, 1,  fp);
    printf("struct TRACKCHUNK:%lu\n", sizeof(struct TRACKCHUNK));
    if (tracktemp->ID[0] != 0x4D || tracktemp->ID[1] != 0x54 || tracktemp->ID[2] != 0x72 || tracktemp->ID[3] != 0x6B){
        fprintf(stderr, "Invalid file (Invalid track header)\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    // loads track into struct
    int sizetemp = 0;
    for (int i =0; i< sizeof(tracktemp->size); i++){
        sizetemp = sizetemp << 8;
        sizetemp += tracktemp->size[i];
    }
    printf("<TRACKCHUNK> %lu (but read 8B only)\n ID:%s | size:%.2X\n\n", sizeof(*tracktemp),tracktemp->ID, sizetemp);
    return tracktemp;


    //

}

struct TRACKEVENTS* processtrackevents(FILE* fp, unsigned int len){
    struct TRACKEVENTS* trackeventstemp = malloc(sizeof(struct TRACKEVENTS));
    if (trackeventstemp == NULL){
        perror("Error: ");
    }

    for (int i=0; i < len; i++){

        unsigned char eof[3];
        unsigned char deltatemp = 0;
        unsigned int counter = ftell(fp);


        // check if previous chunk was meta EOF
        fseek(fp, -3, SEEK_CUR);
        fread(eof, 1,3,fp);
        if (!(eof[0] == 0xFF && eof[1] == 0x2F && eof[2] == 0x00)){
            int deltatempcur = ftell(fp);
            int deltatemp1 = fgetc(fp); fseek(fp, -1, SEEK_CUR);
            deltatemp = processdelta(fp);
            if (deltatemp >= 0){
                fseek(fp, -1, SEEK_CUR);int deltatemp2 = fgetc(fp);
                printf("BYTE <%.2X-%.2lX> %38s", deltatempcur, ftell(fp),"DELTA");
                printf(" | data: %.2X - %.2X | value: %.2X\n", deltatemp, deltatemp2, deltatempcur);
            }
        }

        unsigned char IDtemp = fgetc(fp);
        // midi
        if (IDtemp >= 0x80 && IDtemp < 0xF0){
            struct MIDI* miditemp = processmidi(fp, (unsigned char) deltatemp, IDtemp);
            len -= ((ftell(fp) - counter - 1));

        }
        // meta
        else if (IDtemp == 0xFF && deltatemp >=0){
            struct META* metatemp = processmeta(fp, (unsigned char) deltatemp, IDtemp);
            len -= ((ftell(fp) - counter - 1));
            if (metatemp == 0){
                // eof
                return trackeventstemp;

            }
        }
        // sysex
        else if (IDtemp == 0xF0 || IDtemp == 0xFF){
            deltatemp = (unsigned char)0;
            struct SYSEX* sysextemp = processsysex(fp, deltatemp, IDtemp);
            len -= ((ftell(fp) - counter - 1));
        }
        else if (IDtemp == 0x4D){
            fseek(fp, -1, SEEK_CUR);
            return trackeventstemp;
        }
        else{
            trackeventstemp->skipped++;
            printf("BYTE <%.2lX> %67s",ftell(fp),"SKIPPED ");
            printf("(%.2X) \n", IDtemp);

            //printf("\n==============SKIPPED (%.2i/%.2i)\n", i, len);
        }
        //printf("\n++++BYTE:%.2lx (%.2i/%.2i)++++\n", ftell(fp), i, len);

    }
    printf("end of track: %.2lX\n", ftell(fp));

    return trackeventstemp;

}


struct META* processmeta(FILE*fp, int deltatime, unsigned char IDtemp){


    //malloc struct + struct->size array
    struct META* metatemp = malloc(sizeof(struct META));
    if (metatemp == NULL){
        perror("Error:");
        exit(EXIT_FAILURE);
    }
    metatemp->size = malloc(sizeof(char*));
    metatemp->data = malloc(sizeof(char*));
    metatemp->delta = malloc(sizeof(char*)); *metatemp->delta = deltatime;

    int metatype = fgetc(fp);

    printf("BYTE <%.2lX>",ftell(fp));
    for (int i =0; i <4; i++){
        printf(".");
    }
    printf("META");
    printf("(%.2X) | ", IDtemp);
    switch(metatype){

        case (0x00):
            printf("seq num");
            *metatemp->size = fgetc(fp); // always 2
            metatemp->data[0] = fgetc(fp);
            metatemp->data[1] = fgetc(fp);
            break;
        case(0x01):
            printf("text event");
            *metatemp->size = variablelen(fp, 0);
            for (int i =0; i< *metatemp->size; i++){
                metatemp->data[i] = fgetc(fp);
            }
            break;
        case(0x02):
            printf("copyright notice");
            *metatemp->size = variablelen(fp, 0);
            for (int i =0; i< *metatemp->size; i++){
                metatemp->data[i] = fgetc(fp);
            }
            break;
        case(0x03):
            printf("seq name");
            *metatemp->size = variablelen(fp, 0);
            for (int i =0; i< *metatemp->size; i++){
                metatemp->data[i] = fgetc(fp);
            }

            break;
        case(0x04):
            printf("instrument name");
            *metatemp->size = variablelen(fp, 0);
            for (int i=0; i< *metatemp->size; i++){
                metatemp->data[i]= fgetc(fp);
            }
            break;
        case(0x05):
            printf("lyrics");
            *metatemp->size = variablelen(fp, 0);
            for (int i=0; i< *metatemp->size; i++){
                metatemp->data[i]= fgetc(fp);
            }
            break;
        case(0x06):
            printf("marker");
            *metatemp->size = variablelen(fp, 0);
            for (int i=0; i< *metatemp->size; i++){
                metatemp->data[i]= fgetc(fp);
            }
            break;
        case(0x07):
            printf("cue point");
            *metatemp->size = variablelen(fp, 0);
            for (int i=0; i< *metatemp->size; i++){
                metatemp->data[i]= fgetc(fp);
            }
            break;
        case(0x20):
            printf("channel prefix");
            *metatemp->size = fgetc(fp); // always 1
            metatemp->data[0]=fgetc(fp);
            break;
        case(0x21):
            printf("port prefix");
            // used to address 1+ sound devices (e.g. soundcards) playing together
            // also used to increase no. of channels

            *metatemp->size = fgetc(fp); // always 1
            metatemp->data[0]=fgetc(fp); // channel number
            break;
        case(0x2F):
            if (fgetc(fp)==0x00){
                fseek(fp, -1, SEEK_CUR);
                printf("EOF (%.2lx) (%.2x)\n", ftell(fp) -2, fgetc(fp));
                return 0;
                break;
            }
            else{
                fprintf(stderr, "Expected EOF, last byte is not 0x00\n");
                exit(EXIT_FAILURE);
            }

            break;
        case(0x51):
            printf("set tempo");
            *metatemp->size=fgetc(fp); // always 3;
            for (int i=0;i<3;i++){
                metatemp->data[i] = fgetc(fp);
            }
            //printf("====TODO: CALC MICROSECONDS PER MINUTE BPM (refer to site)\n");
            break;
        case(0x54):
            printf("SMPTE Offset");
            *metatemp->size = fgetc(fp); //always 5

            // b0 = hours
            //  0 ss hh hhhh
            //  0: reserved bit
            //  ss: framerate (00 | 01 | 10 | 11)
            // b1 = minutes
            // b2 = seconds
            // b3 = framte
            // b4 = subframe
            for (int i =0; i<5; i++){
                metatemp->data[i] = fgetc(fp);
            }
            break;

        case(0x58):
            printf("Time Sig");
            *metatemp->size = fgetc(fp); //always 4
            // b0 = number
            // b1 = denom (1/2^x)
            // b2 = metronome (clocks per click)
            // b3 = 32nd notes per 24 MIDI clock
            for (int i =0; i<4; i++){
                metatemp->data[i] = fgetc(fp);
            }
            break;

        case(0x59):
            printf("Key Sig");
            *metatemp->size = fgetc(fp); //always 2;
            metatemp->data[0] = (signed int) fgetc(fp); // accidentals (pos = sharps, neg = flats)
            metatemp->data[0] = (unsigned int) fgetc(fp); // scale (0 = major, 1 = minor)
            break;

        case(0x7F):
            printf("Manufac Info\n");
            *metatemp->size = variablelen(fp,0);
            for (int i =0 ; i < *metatemp->size; i++){
                metatemp->data[i] = fgetc(fp);
            }
            // refer to list
            break;

        default:
            fprintf(stderr, "undefined\n");
            break;




    }
    metatemp->type = metatype;
    metatemp->ID = IDtemp;

    //print struct values
    {
        printf(" | delta: ");{
            for (int i=0; i<sizeof(*metatemp->delta); i++){
                printf("%.2X", metatemp->delta[i]);
            }
        }


        printf("| ID: %.2X", metatemp->ID);
        printf("| type: %.2X", metatemp->type);

        printf("| size: ");{
            for (int i=0; i<sizeof(*metatemp->size); i++){
                printf("%.2X", metatemp->size[i]);
            }
            printf(" ");
        }
        printf("| data: ");{
            // sizeof(metatemp->data) = size of pointer
            // sizeof(*metatemp->data) = literal type size of value stored at metatemp->data (which is a char, so its 1)
            for (int i=0; i< *metatemp->size; i++){
                printf("%.2X ", metatemp->data[i]);
            }
            printf("\n");
        }
    }
    //printf("<^PROCESSMETA DONE>\n");
    return metatemp;
}


struct MIDI* processmidi(FILE*fp, int deltatime, unsigned char IDtemp){
    //printf("<PROCESSMIDI START>\n");
    struct MIDI* miditemp = malloc(sizeof(struct MIDI));
    if (miditemp == NULL){
        perror("Error:");
        exit(EXIT_FAILURE);
    }

    miditemp->delta = malloc(sizeof(&deltatime)); *miditemp->delta = deltatime;

    // is there a way to fread this starting from miditemp->status
    printf("BYTE <%.2lX> MIDI | ", ftell(fp));
    miditemp->status = IDtemp;
    miditemp->data1 = fgetc(fp);
    miditemp->data2= fgetc(fp);

    {
        printf("status:%.2X ", miditemp->status);
        printf("(ch.%.2i) | ", (miditemp->status & 0xF));
        printf("data1: %.2X |", miditemp->data1);
        printf("data2: %.2X\n", miditemp->data2);
    }
    //printf("<PROCESSMIDI DONE>\n");
    return miditemp;



}

struct SYSEX* processsysex(FILE*fp, int deltatime, unsigned char IDtemp){
    //printf("<PROCESS SYSEX START>\n");
    struct SYSEX* sysextemp = malloc(sizeof(struct SYSEX));
    if (sysextemp==NULL){
        perror("Error:");
        exit(EXIT_FAILURE);
    }

    sysextemp->delta = malloc(sizeof(char*)); *sysextemp->delta = deltatime;
    sysextemp->data = malloc(sizeof(char*));
    sysextemp->size = malloc(sizeof(char*));
    sysextemp->ID = IDtemp;

    printf("BYTE <%.2lX> SYS (%.2X) | ", ftell(fp), IDtemp);
    switch (IDtemp){
        case(0xF0):
            *sysextemp->size = variablelen(fp, 0);
            for (int i=0; i < *sysextemp->size; i++){
                sysextemp->data[i] = fgetc(fp);
            }


            break;
        case(0xF1):
            printf("syscom - undefined (%.2X)\n", IDtemp);
            break;
        case(0xF2):
            printf("syscom - song pos ptr (%.2X)\n", IDtemp);

            break;
        case(0xF3):
            printf("syscom - song select (%.2X)\n",IDtemp);
            break;
        case(0xF4):
            printf("syscom - undefined (%.2X)\n",IDtemp);
            break;
        case(0xF5):
            printf("syscom - undefined (%.2X)\n", IDtemp);
            break;
        case(0xF6):
            printf("syscom - tune request (%.2X)\n", IDtemp);
            break;
        case(0xF7):
            printf("syscom - end of sysex (%.2X)\n", IDtemp);
            break;
        case(0xF8):
            printf("sysRT - timing clock (%.2X)\n", IDtemp);
            break;
        case(0xF9):
            printf("sysRT - undefined (%.2X)\n", IDtemp);
            break;
        case(0xFA):
            printf("sysRT - start (%.2X)\n", IDtemp);
            break;
        case(0xFB):
            printf("sysRT - continue (%.2X)\n", IDtemp);
            break;
        case(0xFC):
            printf("sysRT - stop (%.2X)\n", IDtemp);
            break;
        case(0xFD):
            printf("sysRT - undefined (%.2X)\n", IDtemp);
            break;
        case(0xFE):
            printf("sysRT - active sensing (%.2X)\n", IDtemp);
            break;
        case(0xFF):
            printf("sysRT - sys reset (%.2X)\n", IDtemp);
            break;
    }
    {
        printf("length:");
        for (int i=0; i < sizeof(*sysextemp->size); i++){
            printf("%.2X", sysextemp->size[i]);
        }
        printf("(0x)\n");
    }



    /*
    unsigned int curtemp = fgetc(fp);
    while (!(curtemp ^ 0xF7)){
        *sysextemp->data += curtemp;
        curtemp = fgetc(fp);

    }
    */
    //printf("<PROCESS SYSEX END>\n");
    return sysextemp;
}


unsigned int processdelta(FILE*fp){
    //printf("<   processdelta start>\n");
    int* cur = malloc(sizeof(int*)); *cur = fgetc(fp);
    //printf("    delta input:%.2X (0x)\n", *cur);
    unsigned deltatemp = 0;

    // ignore if sysex msg (no delta needed)
    if (deltatemp ==0){
        if ((*cur | 0xF) == 0xFF){
            deltatemp = (signed int) -1;
            fseek(fp, -1, SEEK_CUR);
            //printf("<   processdelta end>\n");
            return((signed int)deltatemp);
        }

    }
    // last delta byte
    if (!(*cur & 0x80)){
        deltatemp <<= 7; deltatemp += *cur;
    }
    else{
        // if current byte has MSB set
        while ((*cur & 0x80) == 0x80){
            *cur += (*cur & 0x7F);
            deltatemp <<= 7; deltatemp += *cur;
            *cur = fgetc(fp);
        }
    }

    //printf("*cur x 0x80: %.2x\n %.2x\n", (*cur & 0x80), *cur);
    //printf("<   processdelta end>\n");
    return deltatemp;

}

int variablelen(FILE*fp, int* cur){
    unsigned int curtemp = fgetc(fp);

    // FYI (*p = 0) is the same as (*p = NULL);
    // if pointer is void (declared but not malloc'd), malloc;
    if (cur == NULL){
        cur = calloc(1, sizeof(int));
    }


    if (curtemp & 0x80){
        *cur += curtemp;
        *cur = *cur << 8;
        *cur += variablelen(fp, cur);
    }
    else if (curtemp != 0x00){
        *cur += curtemp;
    }



    return *cur;

}