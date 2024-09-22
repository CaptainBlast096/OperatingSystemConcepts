/** 
main.c
Author: Jaleel Rogers
**/

#include <stdio.h>
#include "LuckyNumber.h"
#include "Record.h"

int main()
{
	Record record = {0,0,0};
	char playAgain = 'y';
	
	while (playAgain == 'y' || playAgain == 'Y')
	{
		int guess;
		printf("Enter a guess between 1 and 5: ");
		scanf("%d", &guess);
		
		int result = checkLuckyNumber(guess);
		if (result > 0)
		{
			printf("\nToo high\n");
			record.greaterThan++;
		}
		else if (result < 0)
		{
			printf("\nToo low\n");
			record.lessThan++;
		}
		else
		{
			printf("\nYou won!\n");
			record.playerWins++;
		}
		//Prints current records 
		printf("\nWins: %d\n", record.playerWins);
		printf("Less than: %d\n", record.lessThan);
		printf("Greater than: %d\n", record.greaterThan);

		//Asks if the user wants to play again
		printf("\nDo you want to play again? y/n: ");
		scanf(" %c", &playAgain);
		
		//Checks if the user wants to exit the loop
		if (playAgain == 'n' || playAgain == 'N')
			{
				printf("Game Over\n");
			}
	}
	return 0;
}
