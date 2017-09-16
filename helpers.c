/**
 * helpers.c
 *
 * Helper functions for Problem Set 3.
 */

#include <cs50.h>
#include <stdio.h>

#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    // TODO: implement a searching algorithm
    int i = (n - 1) / 2;
    int max = n - 1;
    int min = 0;
    printf("value= %i\n", value);

    while(1)
    {

 printf("haystack[%i]= %i, max= %i, min= %i\n", i, values[i], max, min);

        if (values[i] == value)
        {
            return true;
            break;
        }

        else if (values[i] != value)
        {
            if (i == 0 || i == max)
            {
                return false;
            }

            if (max - min == 1)
            {
                i = max;
                continue;
            }

            if (values[i] > value)
            {
                max = i;
                i = (max + min) / 2;
            }

            else if (values[i] < value)
            {
                min = i;
                i = (max + min) / 2;
            }

        continue;
        }
    }
}

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    // TODO: implement a sorting algorithm


for (int i = 0; i <= n - 2; i++)
{
     if (values[i] > values[i + 1])
     {
         int p1 = values[i];
         int p2 = values[i + 1];
         values[i] = p2;
         values[i + 1] = p1;
         i = 0;
         continue;
     }

     else if (values[0] > values[i])
     {
         int p1 = values[0];
         int p2 = values[i];
         values[0] = p2;
         values[i] = p1;
         i = 0;
     }

     if (values[0] > values[n - 1])
     {
         int p1 = values[0];
         int p2 = values[n - 1];
         values[0] = p2;
         values[n - 1] = p1;
         i = 0;
     }
}
return;
}