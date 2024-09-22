/**
LuckyNumber.c
Author: Jaleel Rogers
**/

#include "LuckyNumber.h" //LuckyNumber.c is dependent on LuckyNumber.h
#include <stdlib.h>
#include <time.h>

int checkLuckyNumber(int guess)
{
	srand(time(0));
	
	int randNum = (rand() % 5) + 1; //Random number between 1-5

	if (guess > randNum)
	{
		return guess - randNum; //Returns a positive number
	}
	else if (guess < randNum)
	{
		return guess - randNum; //Returns a negative number
	}
	else
	{
		return 0;
	}
}
