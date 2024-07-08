//Full name: Raz Olewsky
// ID number: 315341396
//Program description: TAKI! the program manages a taki play.
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h> // in order to use the "rand" and "srand" functions
#include <time.h>	// in order to use "time" function

#define CARDS_QUANTITY 14
#define CARDS_GIVEN 4
#define MAX_NAME_LEN 21
#define NO_COLOR -1
#define PICK_CARD_NUM 0
#define PLUS_NUM 10
#define STOP_NUM 11
#define SWITCH_DIRECTION_NUM 12
#define TAKI_NUM 13
#define SWITCH_COLOR_NUM 14

const int DECK_CARDS[9] = { 1,2,3,4,5,6,7,8,9 };
const char CARD[] = "Card #", YELLOW[] = "Yellow", RED[] = "Red", BLUE[] = "Blue", GREEN[] = "Green";
const char TAKI[] = "  TAKI ", SWITCH_COLOR[] = " COLOR ", SWITCH_DIRECTION[] = "  <->  ", STOP[] = "  STOP ", PLUS[] = "   +   ";
const char TAKI_STATS[] = " TAKI  ", STOP_STATS[] = " STOP  ";

typedef struct Card_Info
{
	int num; // card number: 1-9, 10 = PLUS, 11 = STOP, 12 = SWITCH_DIRECTION, 13 = TAKI, 14 = SWITCH_COLOR
	int color; // card char: 0 = RED, 1 = GREEN, 2 = BLUE, 3 = YELLOW
} CARD_INFO;
struct PlayerInfo
{
	char firstName[MAX_NAME_LEN]; // name of each player in the game
	CARD_INFO* stack; // info = number and char of the card
	int cardsInHand; // logic size - quantity of cards in hand of each player
	int phySize; // indicates how much cards a player can hold (realloc)
}; typedef struct PlayerInfo PLAYER_INFO;
typedef struct stats
{
	int cardNum; // card number: 1-9, 10 = PLUS, 11 = STOP, 12 = SWITCH_DIRECTION, 13 = TAKI, 14 = SWITCH_COLOR
	int quantity; // counter of how many times each card was taken from the deck
} STATS;

int welcomeToTaki();
int validChoice(CARD_INFO* upperCard, CARD_INFO* playerCard, int cardsInHand, STATS* stats);
int TAKIvalidChoice(CARD_INFO* upperCard, CARD_INFO* playerCard, int cardsInHand, STATS* stats);
void specialCard(CARD_INFO* upperCard, PLAYER_INFO* currPlayer, int choice, bool* direction, int* i, int numOfPlayers, STATS* stats);
void invalidChoicePrint(int cardsInHand);
void TAKIinvalidChoicePrint(int cardsInHand);
void initStats(STATS* stats);
void getNames(PLAYER_INFO* currPlayer, int numOfPlayers);
int nextTurn(bool direction, int numOfPlayers, int i);
void firstCard(CARD_INFO* firstCard, STATS* stats);
PLAYER_INFO* cardDivision(PLAYER_INFO* currPlayer, int numOfPlayers);
void printUpperCard(CARD_INFO* card);
char numToColor(int num);
void numToString(int cardNum);
void printCard(CARD_INFO* card);
CARD_INFO getCard();
void gameStart(PLAYER_INFO* currPlayer, STATS* stats, int numOfPlayers);
bool cardChoiceCheck(CARD_INFO* upperCard, CARD_INFO* chosenCard, STATS* stats);
CARD_INFO* cardRealloc(CARD_INFO* cards, int cardsInHand, int phySize);
void swapAndDeleteCard(PLAYER_INFO* currPlayer, int choice);
void printStack(PLAYER_INFO* currPlayer);
void choiceCases(CARD_INFO* upperCard, PLAYER_INFO* currPlayer, int choice, bool* direction, int* i, int numOfPlayers, STATS* stats);
int colorChoice();
void upperCardUpdateAndPrint(CARD_INFO* upperCard, PLAYER_INFO* currPlayer, int choice);
void printColorWithChar(char ColorChar);
void printColored(char ch);
void redPrint();
void greenPrint();
void bluePrint();
void yellowPrint();
void whitePrint();
void gameStats(STATS* stats);
void mergeSort(STATS* stats, int size);
void merge(STATS* stats, int sizeStats, STATS* arr, int sizeArr, STATS* res);
void copyArr(STATS* stats, STATS* arr, int size);
char* convertToText(int i);

void main()
{
	srand((unsigned int)time(NULL)); //function call makes use of the computer's internal clock
		// to control the choice of the seed. Since time is continually changing,
		// the seed is forever changing. Therefore we'll get different sequence of random
		// values in each program run.
	int playersQuan;
	playersQuan = welcomeToTaki();
	STATS cardsStats[CARDS_QUANTITY]; //array of stract to calculate how much cards has been taken from the deck
	initStats(cardsStats); // initialize the array
	PLAYER_INFO* allPlayers = (PLAYER_INFO*)malloc(playersQuan * sizeof(PLAYER_INFO)); // dinamic array - size = number of players
	if (allPlayers == NULL)
	{
		printf("ERROR! Memory alloction failed");
		exit(1); // indicating an error
	}
	getNames(allPlayers, playersQuan); //each player enters his first name - the turns will change by name of the players
	gameStart(allPlayers, cardsStats, playersQuan); // divide cards to all players + put upperCard + rest of the play
	free(allPlayers);
}
int welcomeToTaki() // Function welcoming the players to the game and returns the quantity of players in the game
{
	int playersQuan;
	printf("************  Welcome to TAKI game !!! ***********\n");
	printf("Please enter the number of players:\n");
	scanf("%d", &playersQuan);
	return playersQuan;
}
void initStats(STATS* stats) // initialize statistics array
{
	int i;
	for (i = 0; i < CARDS_QUANTITY; i++)
	{
		stats[i].cardNum = i + 1; // represent the type of each card
		stats[i].quantity = 0; // counter of each card that has been taken from the deck
	}
}
void getNames(PLAYER_INFO* currPlayer, int numOfPlayers) // function scans the name of each player
{
	int i;
	for (i = 0; i < numOfPlayers; i++)
	{
		printf("Please enter the first name of player #%d:\n", i+1);
		scanf("%s", currPlayer[i].firstName);
	}
}
void gameStart(PLAYER_INFO* currPlayer, STATS* stats, int numOfPlayers) //Main function of the game - uses other function to manage the flow of the game
{
	int i = 0, choice;
	bool winner = false;
	bool direction = true; // represent the direction of the game (can be changed by putting <->) true = order of writing names, false = backwards
	CARD_INFO cardF;
	CARD_INFO* upperCard = &cardF;
	firstCard(upperCard, stats);  // inserts first card and prints it.
	currPlayer = cardDivision(currPlayer, numOfPlayers); // divide 4 cards to each player in the game
	while (winner == false) //while there is not a player with 0 cards (winner)
	{
		printStack(&currPlayer[i]); // shows each players his cards in the stack
		choice = validChoice(upperCard, currPlayer[i].stack, currPlayer[i].cardsInHand, stats); // gets the choice after validatation check
		choiceCases(upperCard, &currPlayer[i], choice, &direction, &i, numOfPlayers, stats); // checks the choice and divides to cases
		if (currPlayer[i].cardsInHand == 0) // (winner)
		{
			winner = true;
			printf("The winner is... %s Congratulations!\n\n", currPlayer[i].firstName);
			mergeSort(stats, CARDS_QUANTITY); //sort the array by quantity of cards appearance
			gameStats(stats); // prints the statistics from the array
			for (i = 0; i < numOfPlayers; i++)
				free(currPlayer->stack); // frees all the dinamic cards array in the game
		}
		i = nextTurn(direction, numOfPlayers, i); //updates the next turn - depend on the direction flag and other terms
	}
}

int validChoice(CARD_INFO* upperCard, CARD_INFO* playerCard, int cardsInHand, STATS* stats) //checks the player's choice from his cards
{
	whitePrint();
	int choice;
	printf("Please enter 0 if you want to take a card from the deck\n");
	printf("or 1-%d if you want to put one of your cards in the middle:\n", cardsInHand);
	scanf("%d", &choice);
	if (choice == 0) // pick a card
		return choice;
	while (cardChoiceCheck(upperCard, &playerCard[choice - 1], stats) == false || (choice < 0 || choice > cardsInHand))
	{// while the player chooses wrong number (by color/number) or enters wrong number
		invalidChoicePrint(cardsInHand); //print function
		scanf("%d", &choice);
		if (choice == 0)
			return choice;
	}
	return choice;
}

int nextTurn(bool direction, int numOfPlayers, int i) // function updates the next turn 
{
	if (direction)
	{
		i++;
		if (i == numOfPlayers) // if end of the array - sends to the first (circle)
			i = 0;
		if (i > numOfPlayers) // stop case
			i = 1;
	}
	else
	{
		i--;
		if (i == -1) // if direction is opposite, sends to the last player in the array (circle)
			i = numOfPlayers - 1;
		if (i < -1) // stop case
			i = numOfPlayers - 2;
	}
	return i;
}
PLAYER_INFO* cardDivision(PLAYER_INFO* currPlayer, int numOfPlayers) // function divides cards to each player
{
	int i, j;
	for (i = 0; i < numOfPlayers; i++)
	{
		currPlayer[i].stack = (CARD_INFO*)malloc(CARDS_GIVEN * sizeof(CARD_INFO)); // dinamic array of cards
		if (currPlayer[i].stack == NULL)
		{
			printf("ERROR! Memory alloction failed");
			exit(1); // indicating an error
		}
		for (j = 0; j < CARDS_GIVEN; j++) // CARDS_GIVEN = 4
		{
			currPlayer[i].stack[j] = getCard();
		}
		currPlayer[i].cardsInHand = CARDS_GIVEN; // updates the logSize of the cards array
		currPlayer[i].phySize = CARDS_GIVEN; // updates the physicSize of the cards array
	}
	return currPlayer;
}
CARD_INFO getCard() // function returns a new card (random)
{
	CARD_INFO res;
	res.num = 1 + (rand() % 14); // card between 1-14
	if (res.num == 14)
		res.color = NO_COLOR;
	else
		res.color = numToColor((rand() % 4)); // color between 0-3
	return res;
}
void firstCard(CARD_INFO* firstCard, STATS* stats) // function prints upperCard to the table
{
	firstCard->num = 1 + (rand() % 9); // regular card - number between 1-9
	firstCard->color = numToColor((rand() % 4)); // color between 0-3
	stats[firstCard->num - 1].quantity++; // adds to statistics
	printUpperCard(firstCard); // print the upperCard
}
void printUpperCard(CARD_INFO* upCard) // function prints the upperCard
{
	whitePrint();
	printf("\nUpper card:\n");
	printCard(upCard);
}

char numToColor(int num) // function converts number between 0-3 to char that represents the first char of the chosen color
{
	switch (num)
	{
	case 0:
	{
		return 'Y';
		break;
	}
	case 1:
	{
		return 'R';
		break;
	}
	case 2:
	{
		return 'B';
	}
	case 3:
	{
		return 'G';
		break;
	}
	default:
		break;
	}
}

void numToString(int cardNum) // function gets special number between 10-14 and returns the specific string the number represents
{
	switch (cardNum)
	{
	case 10:
		printf("*%s*\n", PLUS);
		break;
	case 11:
		printf("*%s*\n", STOP);
		break;
	case 12:
		printf("*%s*\n", SWITCH_DIRECTION);
		break;
	case 13:
		printf("*%s*\n", TAKI);
		break;
	case 14:
	{
		whitePrint();
		printf("*");
		redPrint();
		printf(" %c", 'C');
		yellowPrint();
		printf("%c", 'O');
		greenPrint();
		printf("%c", 'L');
		yellowPrint();
		printf("%c", 'O');
		bluePrint();
		printf("%c ", 'R');
		whitePrint();
		printf("*\n");
		break;
	}
	}
}

void printStack(PLAYER_INFO* currPlayer) //function gets a specific played and prints all his cards in his hands
{
	int i;
	whitePrint();
	printf("%s's turn:\n\n", currPlayer->firstName);
	for (i = 0; i < currPlayer->cardsInHand; i++)
	{
		printf("%s%d:\n", CARD, i+1);
		printCard(&currPlayer->stack[i]);
	}
}

void choiceCases(CARD_INFO* upperCard, PLAYER_INFO* currPlayer, int choice, bool* direction, int* i, int numOfPlayers, STATS* stats)
{ // function gets information on the specific player and his choice.
	// function divided to cases and continues the game depend on the choice case
	if (choice == PICK_CARD_NUM) // Pick a card from deck (choice = 0)
	{
		if (currPlayer->cardsInHand == currPlayer->phySize) // checks if there is enough place to get a card in the cards dinamic array
		{
			currPlayer->stack = cardRealloc(currPlayer->stack, currPlayer->cardsInHand, currPlayer->cardsInHand * 2); // using realloc function to enlarge the physize of the stack (if needed)
			currPlayer->phySize *= 2; // updates the physize of the array (doubled)
		}
		currPlayer->stack[currPlayer->cardsInHand] = getCard(); //adds card to the stack
		stats[currPlayer->stack[currPlayer->cardsInHand].num - 1].quantity++; // update the statistics array with the new card from the deck
		currPlayer->cardsInHand++; //adds to the logicSize of the stack
		printUpperCard(upperCard); // print the new upper card - player's choice
	}
	if (currPlayer->stack[choice - 1].num >= 1 && currPlayer->stack[choice - 1].num <= 9) // puts regular 1-9 card
	{
		upperCardUpdateAndPrint(upperCard, currPlayer, choice); // updates the upper card to be the chosen card and prints it
		swapAndDeleteCard(currPlayer, choice); // delete the chosen card from the hands of the player (used card)
	}
	else // puts special card (switch on 10-14)
		specialCard(upperCard, currPlayer, choice, direction, i, numOfPlayers, stats);
}

void upperCardUpdateAndPrint(CARD_INFO* upperCard, PLAYER_INFO* currPlayer, int choice)
{// function updates the upper card to be the chosen card and prints it
	upperCard->num = currPlayer->stack[choice - 1].num; // updates the uppercard to the chosen card
	upperCard->color = currPlayer->stack[choice - 1].color;// updates the uppercard to the chosen card
	printUpperCard(upperCard); // print the new upper card - player's choice
}

void specialCard(CARD_INFO* upperCard, PLAYER_INFO* currPlayer, int choice, bool* direction, int* i, int numOfPlayers, STATS* stats)
{ // function divided to cases by the choice of the player - special card has been chosen (10-14)
	switch (currPlayer->stack[choice - 1].num)
	{
	case PLUS_NUM: // PLUS - player puts the card + gets another turn
	{
		upperCardUpdateAndPrint(upperCard, currPlayer, choice); // updates the upper card to be the chosen card and prints it
		swapAndDeleteCard(currPlayer, choice); // delete the chosen card from the hands of the player (used card)
		if (currPlayer->cardsInHand == 0) // if the player put "PLUS" as the last card in his hands - gets another card
		{
			currPlayer->cardsInHand++; // adding to the logic size the new card
			currPlayer->stack[currPlayer->cardsInHand-1] = getCard(); // get new card
		}
		printStack(currPlayer); // print the updated stack of the player to play another turn
		choice = validChoice(upperCard, currPlayer->stack, currPlayer->cardsInHand, stats); // checks the choice
		choiceCases(upperCard, currPlayer, choice, direction, i, numOfPlayers, stats); // send the choice to the function again to play the case of the card
		break;
	}
	case STOP_NUM: // STOP - skips the next player in the game
	{
		upperCardUpdateAndPrint(upperCard, currPlayer, choice); // updates the upper card to be the chosen card and prints it
		if (numOfPlayers == 2 && currPlayer->cardsInHand == 1) // if there are only 2 players in the game and the player plays "STOP" as his last turn, he gets another card and turn
		{
			currPlayer->cardsInHand++;
			currPlayer->stack[currPlayer->cardsInHand] = getCard();
		}
		swapAndDeleteCard(currPlayer, choice); // delete the chosen card from the hands of the player (used card)
		if (numOfPlayers > 1)
		{
			if (*direction) // checks the direction of the play
				(*i)++; //skips the next player
			else
				(*i)--; //skips the next player
		}
		break;
	}
	case SWITCH_DIRECTION_NUM: // SWITCH_DIRECTION - swithces the direction of the players
	{
		upperCardUpdateAndPrint(upperCard, currPlayer, choice); // updates the upper card to be the chosen card and prints it
		swapAndDeleteCard(currPlayer, choice); // delete the chosen card from the hands of the player (used card)
		if (*direction) // checks the direction of the play
			*direction = false; // changes the direction of the game
		else
			*direction = true; // changes the direction of the game
		break;
	}
	case TAKI_NUM: // TAKI - player can put TAKI on any upperCard and allowed to put cards in the same color
	{
		upperCardUpdateAndPrint(upperCard, currPlayer, choice); // updates the upper card to be the chosen card and prints it
		swapAndDeleteCard(currPlayer, choice); // delete the chosen card from the hands of the player (used card)
		if (currPlayer->cardsInHand == 0) // checks if the player wins
			if (upperCard->num != 10) // checks if his last card is plus
				break;
			else
				choiceCases(upperCard, currPlayer, choice, direction, i, numOfPlayers, stats);
		printStack(currPlayer); // print the updated stack of the player to play another turn
		choice = TAKIvalidChoice(upperCard, currPlayer->stack, currPlayer->cardsInHand, stats); // checks the choice
		if (choice == 0 || currPlayer->stack[choice - 1].num == 14) // if the player played his last card in the TAKI flow (last card played regulary) - last card = pick a card or change color
			choiceCases(upperCard, currPlayer, choice, direction, i, numOfPlayers, stats);
		while (choice > 0 && choice < 14)
		{
			upperCardUpdateAndPrint(upperCard, currPlayer, choice); // updates the upper card to be the chosen card and prints it
			swapAndDeleteCard(currPlayer, choice); // delete the chosen card from the hands of the player (used card)
			printStack(currPlayer); // print the updated stack of the player to play another turn
			choice = TAKIvalidChoice(upperCard, currPlayer->stack, currPlayer->cardsInHand, stats);
			if (choice == 0 || currPlayer->stack[choice - 1].num == 14) // if the player played his last card in the TAKI flow (last card played regulary) = pick a card or change color
				choiceCases(upperCard, currPlayer, choice, direction, i, numOfPlayers, stats);
			if (currPlayer->cardsInHand == 0) // checks if the player wins
				if (upperCard->num != 10) // checks if his last card is plus
					break;
				else
					choiceCases(upperCard, currPlayer, choice, direction, i, numOfPlayers, stats);
		}
		break;
	}
	case SWITCH_COLOR_NUM: // SWITCH_COLOR -  player can put SWITCH_COLOR on any upperCard and needs to declare on the color that countinues the game
	{
		int newColor;
		CARD_INFO colorCard;
		newColor = colorChoice(); // gets the choice of the new color to continue the game
		while (newColor < 1 || newColor > 4) // if choice is wrong
		{
			printf("Invalid choice! Try again.\n");
			newColor = colorChoice();
		}
		colorCard.color = numToColor(newColor -1); //updates the card to the chosen color
		upperCard->color = colorCard.color; // updates the upperCard to the chosen card
		printColorWithChar(upperCard->color); // prints the card (specific function to print that kind of card)
		swapAndDeleteCard(currPlayer, choice); // delete the chosen card from the hands of the player (used card)
		break;
	}
	default:
		break;
	}
}

int TAKIvalidChoice(CARD_INFO* upperCard, CARD_INFO* playerCard, int cardsInHand, STATS* stats)
{
	whitePrint();
	int choice;
	printf("Please enter 0 if you want to finish your turn\n");
	printf("or 1-%d if you want to put one of your cards in the middle:\n", cardsInHand);
	scanf("%d", &choice);
	if (choice == 0 || playerCard[choice - 1].num == 14)
		return choice;
	while (upperCard->color != playerCard[choice - 1].color|| (choice < 0 || choice > cardsInHand))
	{
		TAKIinvalidChoicePrint(cardsInHand);
		scanf("%d", &choice);
		if (choice == 0 || playerCard[choice - 1].num == 14)
			return choice;
	}
	return choice;
}

int colorChoice() //prints the options to change the color of the game (in case chioce = "change color")
{
	int color;
	printf("Please enter your color choice:\n");
	printf("1 - %s\n", YELLOW);
	printf("2 - %s\n", RED);
	printf("3 - %s\n", BLUE);
	printf("4 - %s\n", GREEN);
	scanf("%d", &color);
	return color;
}

bool cardChoiceCheck(CARD_INFO* upperCard, CARD_INFO* chosenCard, STATS* stats) //checks the choice of the player to put a card
{
	bool res = false;
	if (chosenCard->num == 14 || chosenCard->num == 0) //color card or pick a card
		res = true;
	if (upperCard->num == chosenCard->num) // checks if the chosen card is in the same number of the upper card
	{
		res = true;
	}
	if (upperCard->color == chosenCard->color) // checks if the chosen card is in the same number of the upper card
		res = true;
	return res;
}

void TAKIinvalidChoicePrint(int cardsInHand) // wrong choice while in taki flow
{
	printf("Invalid choice! Try again.\n");
	printf("Please enter 0 if you want to finish your turn\n");
	printf("or 1-%d if you want to put one of your cards in the middle:\n", cardsInHand);
}

void invalidChoicePrint(int cardsInHand) // player put wrong card
{
	printf("Invalid choice! Try again.\n");
	printf("Please enter 0 if you want to take a card from the deck\n");
	printf("or 1-%d if you want to put one of your cards in the middle:\n", cardsInHand);
}

void printCard(CARD_INFO* card) // function gets a specific card and prints it with the specific way it should
{
	whitePrint();
	if (card->color !=14) // checks in which color to print the card
		printColored(card->color);
	printf("*********\n");
	printf("*       *\n");
	if (card->num < 10) // card between 1-9
	{
		printf("*   %d   *\n", card->num);
		printf("*   %c   *\n", card->color);
	}
	else if (card->num >= 10 && card->num <= 13) // special card
	{
		numToString(card->num); // prints the special card
		printf("*   %c   *\n", card->color);
	}
	else
	{
		numToString(card->num); // prints the special card
		printf("*       *\n");
	}
	printf("*       *\n");
	printf("*********\n");
	printf("\n");
	whitePrint();
}
void printColorWithChar(char ColorChar) //prints color card with the chosen color to continue the game
{
	whitePrint();
	printf("*********\n");
	printf("*       *\n");
	numToString(14);
	printf("*   ");
	printColored(ColorChar);
	printf("%c   ", ColorChar);
	whitePrint();
	printf("*\n");
	printf("*       *\n");
	printf("*********\n");
	printf("\n");
}

void printColored(char ch) // changes the color of the print depend on the char
{
	if (ch == 'R')
		redPrint();
	else if (ch == 'G')
		greenPrint();
	else if (ch == 'B')
		bluePrint();
	else if (ch == 'Y')
		yellowPrint();
}
void whitePrint() // function to print in white
{
	printf("\033[0m");
}
void redPrint() // function to print in red
{
	printf("\033[0;31m");
}
void greenPrint() // function to print in green
{
	printf("\033[0;32m");
}
void bluePrint() // function to print in blue
{
	printf("\033[0;34m");
}
void yellowPrint() // function to print in yellow
{
	printf("\033[0;33m");
}

void swapAndDeleteCard(PLAYER_INFO* currPlayer, int choice) // function gets player's stack and a choice. Than swaps the choice to the last place in the array and deletes it
{
	int len = currPlayer->cardsInHand;
	currPlayer->stack[choice - 1] = currPlayer->stack[len - 1]; // swap with the last card in the array
	currPlayer->cardsInHand--; // reduce the logic size = delete chosen card
}
CARD_INFO* cardRealloc(CARD_INFO* cards, int cardsInHand, int phySize) // function enlarge the dinamic array to the new physic size
{
	CARD_INFO* newArr = (CARD_INFO*)malloc(phySize * sizeof(CARD_INFO)); // new array
	int i;
	if (newArr == NULL)
	{
		printf("ERROR! Memory realloction failed");
		exit(1); // indicating an error
	}
	else
	{
		for (i = 0; i < cardsInHand; i++) // copy the array to the new array
			newArr[i] = cards[i];
	}
	free(cards);
	return newArr;
}
void gameStats(STATS* stats) // function prints the statistics of the cards taken from the deck
{
	int i;
	printf("************ Game Statistics ************\n");
	printf("%s | Frequency\n", CARD);
	printf("__________________\n");
	for (i = CARDS_QUANTITY - 1; i >= 0; i--)
	{
		if (stats[i].cardNum > 9)
			printf("%s|%5d\n", convertToText(stats[i].cardNum), stats[i].quantity); // special cards (10-14)
		else
			printf("%4d   |%5d\n", stats[i].cardNum, stats[i].quantity); // regular cards (1-9)
	}
}

void mergeSort(STATS* stats, int size) // sort the statistics array (recurtion)
{
	STATS arr [CARDS_QUANTITY];
	if (size <= 1)
		return;
	mergeSort(stats, size / 2); //first half of the array
	mergeSort(stats + size / 2, size-size/2); // second half of the array
		merge(stats, size / 2, stats + size / 2, size-size/2, arr);
		copyArr(stats, arr, size); // copy values from arr to stats
}
void merge(STATS* stats, int sizeStats, STATS* arr, int sizeArr, STATS* res) 
{// function gets two sorted arrays and their size. function return array that includes both arrays sorted
	int ind1 = 0, ind2 = 0, resInd = 0;
	while ((ind1 < sizeStats) && (ind2 < sizeArr))
	{
		if (stats[ind1].quantity <= arr[ind2].quantity)
		{
			res[resInd] = stats[ind1];
			ind1++;
		}
		else
		{
			res[resInd] = arr[ind2];
			ind2++;
		}
		resInd++;
	}
	while (ind1 < sizeStats)
	{
		res[resInd] = stats[ind1];
		ind1++;
		resInd++;
	}
	while (ind2 < sizeArr)
	{
		res[resInd] = arr[ind2];
		ind2++;
		resInd++;
	}
}

void copyArr(STATS* stats, STATS* arr, int size)
{// copy values from arr to stats
	int i;
	for (i = 0; i < size; i++)
		stats[i] = arr[i];
}

char* convertToText(int num) // function gets a number and returns a string that the number represents (special card)
{
	switch (num)
	{
	case 10:
		return (PLUS);
	case 11:
		return (STOP_STATS);
	case 12:
		return (SWITCH_DIRECTION);
	case 13:
		return (TAKI_STATS);
	case 14:
		return (SWITCH_COLOR);
	default:
		break;
	}
}
