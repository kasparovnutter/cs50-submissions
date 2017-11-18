#include <cs50.h>
#include <stdio.h>



int main(void)
{
    printf("===Change owed===\nChange: $");

    float change;
    change = get_float();
    while (change < 0 || change >= 'a')
    {
        printf("enter an actual number\n");
        change = get_float();
    }

    int cents = change * 100;
    int flag = 0;
    int used = 0;
    int usedtemp = 0;
    int a = 50;
    int b = 25;
    int c = 5;
    int d = 1;

    {
        while (flag == 0)
        {
            {
                if (cents % a > 0 && cents >= a)
                {
                    used = (cents / a);
                    cents = cents - (used * a);
                    flag = 0;
                }
                else if (cents % a == 0)
                {
                    used = cents / a;
                    flag = 1;
                    break;
                }
                else
                {
                    flag = 0;
                }

            }
            {
                if (cents % b > 0 && cents >= b)
                {
                    usedtemp += (cents / b);        //add the 0.50 coins already used
                    cents = cents - (used * b);     //remaining amount (before counting in current coins)
                    used = usedtemp;                //
                    flag = 0;
                }
                else if (cents % b == 0)
                {
                    used += cents / b;
                    flag = 1;
                    break;
                }
                else
                {
                    flag = 0;
                }
            }

            {
                if (cents % c > 0 && cents >= c)
                {
                    usedtemp += cents / c;
                    cents = cents - (used * c);
                    used = usedtemp;
                    flag = 0;
                }
                else if (cents % c == 0)
                {
                    used += cents / c;
                    flag = 1;
                    break;
                }
                else
                {
                    flag = 0;
                }
            }

            {
                if (cents % d >= 0)
                {
                    used += cents / d;
                    flag = 1;
                    break;
                }
            }
        }
    }
while (flag == 1)
{
printf("Coins used: %i\n", used);
break;
}

return 0;


}
