// amex = 15
// master = 16
// visa = 13/16

// amex = start 34/37
// master = start 51-55
// visa = start 4

// checksum (Luhn)
// a = (second-to-last + every other digit) each *2, THEN sum;
// b = (last + every other digit) sum;
// (a + b % 10 == 0) = VALID

// printf must either be "AMEX\n" or "MASTERCARD\n" or "VISA\n" or "INVALID\n"
// main must return 0
// assume input is numerical
// assume input is long long

#include <stdio.h>
#include <cs50.h>


int f_countdigits(long long a); //count no.of digits
int* f_digitprint(long long inputnum, int numlength); //print value + position of each input digit
const char* f_check(const int* a, int numlength);
int f_luhn(const int *a, int numlength);

//MAIN
int main(void)
{
    printf("Card no: ");
    long long inputnum = get_long_long();
    int count = f_countdigits(inputnum);

    //check if input is positive & min 13-digits
    while (inputnum <= 0 || inputnum < 1000000000000)
    {
        if (inputnum <=0)
        {
            printf("INVALID\n");
            inputnum = get_long_long();
        }

        else if (inputnum <= 1000000000000)
        {
            printf("INVALID\n");
            inputnum = get_long_long();
        }
    }

    const int* array = f_digitprint(inputnum, count);

    //array: *a == cardarray[]
    //arrays can only be pointed to when their memory is set to heap (i.e static address)
    //refer: *(a + 1) == cardarray[1]

    //*c == points to const char at (*c + n)
    // c == points to entire array at *c


    const char* type = f_check(array, count);
    int verify = f_luhn(array, count);
    if (verify == 0)
    {
        printf("%s\n", type);
    }
    else
    {
        printf("INVALID\n");
    }

return 0;
}



//==========================================================================

//FUNC
int f_countdigits(long long a)
{

    int digitcount;
    long long b = llabs(a) / 10;
     for (digitcount = 1; b > 0; digitcount++)
    {
        b = llabs(b) / 10;
    }

    return digitcount;
}

int* f_digitprint(long long inputnum, int numlength)
{
    static int cardarray[20];
    long long numremain = llabs(inputnum);
    for (int digitcount = numlength; digitcount >= 1; digitcount--)
    {
        long long digitvalue = llabs(numremain) % 10;

        if (numremain > 0)
        {
            cardarray[digitcount] = digitvalue;
            numremain /= 10;
        }

        else if (numremain < 1 && numremain > 0)
        {
            digitcount++;
            break;
        }
    }

return cardarray;
}

const char* f_check(const int* a, int numlength)
{
    if (*(a + 1) == 4 && (numlength == 13 || numlength == 16))
    {
        return "VISA";
    }
    else if (*(a + 1) == 3 && (*(a + 2) == 4 || *(a + 2) == 7) && numlength == 15)
    {
        return "AMEX";
    }
    else if (*(a + 1) == 5 && (*(a + 2) >= 1 && *(a + 2) <= 5) && numlength == 16)
    {
        return "MASTERCARD";
    }
    else
    {
        return "INVALID";
    }
}

int f_luhn(const int *a, int numlength)
{
int sum_a = 0;
int digit_a;
int sum_b = 0;
int sum_c;
int cursor = (numlength - 1);

    for (int i = 0; i < numlength; i++)
    {
        if (cursor > 1)
        {
            digit_a = 0;
            digit_a = *(a + cursor);
            digit_a *= 2;
            if (digit_a >= 10)      // if digit_a is two-digits (e.g 8 * 2 = 16), get digit sum (e.g 1 + 6 = 7)
            {
                int d_a1 = (digit_a / 10) % 10;
                int d_a2 = digit_a % 10;
                digit_a = d_a1 + d_a2;
            }
            sum_a += digit_a;
            cursor -= 2;
        }

        else if (cursor == 1)
        {
          digit_a = *(a + cursor);
          digit_a *= 2;
          sum_a += digit_a;
          cursor = numlength;
          i = numlength;
        }

        else
        {

          cursor = numlength;
          i = numlength;
        }
    }

cursor = numlength;
    for (int i = 0; i < numlength; i++)
    {
        if (cursor > 1)
        {
        sum_b += *(a + cursor);
        cursor -= 2;
        }

        else if (cursor == 1)
        {
          sum_b += *(a + cursor);
          cursor = numlength;
          i = numlength;
        }

        else
        {
          cursor = numlength;
          i = numlength;
        }
    }

sum_c = sum_a + sum_b;

    //VALID
    if (sum_c % 10 == 0)
    {
        return 0;
    }
    //INVALID
    else
    {
        return 1;
    }

}