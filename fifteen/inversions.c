
#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
int main (void)
{
    int flat_board[] = {12,1,10,2,7,11,4,14,5,0,9,15,8,13,6,3};
    int inversions = 0;
    for (int i = 0; i < 16; i++)
    {
        int temp = flat_board[i];
        for (int j = i + 1; j < 16; j++)
        {
            if(temp > flat_board[j])
            {
                inversions++;
            }
        }
    }
    printf("%i", inversions);
}    