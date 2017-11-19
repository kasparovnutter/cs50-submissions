#import <cs50.h>
#import <stdio.h>

int main(void){
    fprintf(stdout, "Minutes: ");
    int time = get_int();
    // gal = 128 oz
    // bottle = 16 oz
    // shower = 1.5 gal/min
    // bottles = (total shower) / bottle
    fprintf(stdout, "Bottles: %i\n", (time *= (1.5 * 128)) / 16);

}