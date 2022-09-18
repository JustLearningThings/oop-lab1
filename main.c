#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// MENU macros
#define START 1
#define EXIT 2
#define RESUME 3
#define INVALID_MENU_OPTION -999

// GAME macros
#define NONE -999
#define CHOOSE 1
#define CANNOT_MOVE 2
#define WON 3

// colors
#define RED "\x1B[31m"
#define YELLOW "\x1B[33m"
#define CYAN "\x1B[36m"
#define RESET "\x1B[0m"

// CONTROL macros
#define ENTER ' '
#define UP 65
#define DOWN 66
#define RIGHT 67
#define LEFT 68

int nMoves = 0;
int inputMode = NONE;
int choosenNumberI = 0, choosenNumberJ = 0;
int numI = 0, numJ = 0; // coordinates of number to be chosen
int xi = 0, xj = 0; // coordinates of blank tile
int inputKey = ' ';
int mode = CHOOSE; // input mode
int board[4][4] = {
	{1, 2, 3, 4},
	{5, 6, 7, 8},
	{9, 10, 11, 12},
	{13, 14, 15, 16}
};

int elementNotInList(int element, int* list, int n) {
	for (int i = 0; i < n; i++) 
		if (list[i] == element)
			return 0;

	return 1;
}

int menu(void) {
	int c = INVALID_MENU_OPTION;
	int menuOptions[] = {START, EXIT, RESUME};
	int menuOptionsLen = 3;

	while(elementNotInList(c, menuOptions, menuOptionsLen)) {
		system("clear");
		printf("\t\tMenu:\n\n");
		printf("\t\t1.Start\n");
		printf("\t\t2.Exit\n");
		printf("\t\t3.Resume\n");;
		printf("\t\t\nChoose an option: ");
	
		c = (int)(getc(stdin) - '0');
	}

	return c;
}

void won() {
	int winMatrix[4][4] = {
		{1, 2, 3, 4},
		{5, 6, 7, 8},
		{9, 10, 11, 12},
		{13, 14, 15, 16}
	};

	for (int i = 0; i < 4; i++)
		for (int j = 1; j < 4; j++)
			if (board[i][j] != winMatrix[i][j])
				return;

	mode = WON;
}

void shuffleBoard() {
	int flag;
	int row = rand() % 4, col = rand() % 4;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			flag = board[i][j];
			board[i][j] = board[row][col];
			board[row][col] = flag;
		}
}

// display a single board number
// 16 is considered empty
void displayNumber(int n, char* color, int padding) {	
	if (n == 16)
		printf("%s%*c%s", color, padding, 'X', RESET);
	else
		printf("%s%*d%s", color, padding, n, RESET);
}

void displayBoard() {
	for (int i = 0; i < 6; i++) {
		if (i == 0 || i == 5)
			for (int j = 0; j < 6; j++)
				printf("===");
					
		else
			for (int j = 0; j < 6; j++) {
				if (j == 0 || j == 5) 
					printf("|");
				else {
					if (numI == i - 1 && numJ == j - 1)
						displayNumber(board[i - 1][j - 1], YELLOW, 4);
					else
						displayNumber(board[i - 1][j - 1], RESET, 4);
				}
			}
		
		printf("\n");
	}

	printf("\n\n");
}

void displayProgress() {
	printf("Moves made: %d\n\n", nMoves);
	printf("Currently at (%d, %d)\n", numI, numJ);
	printf("Blank tile at (%d, %d)\n\n", xi, xj);
}

void displayInputHelp() {
	if (mode == NONE)
		return;

	if (mode == CHOOSE)
		printf("Choose a number to move using arrows...\n");
	else if (mode == CANNOT_MOVE)
		printf("%sCannot move this one :/%s\n", RED, RESET);

	printf("\nControls:\n");
	printf("Arrows - move the cursor between tiles\n");
	printf("Space - choose the tile and switch it with the empty tile (X)\n");
	printf("r - reset game\n");
	printf("s - save and quit game\n");
	printf("q - quit game\n\n");
}

// find blank tile coordinates
void getXCoords() {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (board[i][j] == 16) {
				xi = i;
				xj = j;

				return;
			}
}

void move(int direction) {
	if (direction == UP && numI > 0)
		numI--;
	else if (direction == DOWN && numI < 3)
		numI++;
	else if (direction == RIGHT && numJ < 3)
		numJ++;
	else if (direction == LEFT && numJ > 0)
		numJ--;
}

// check if tile is near the empty tile
int canMoveTile() {
	if (abs(xi - numI) <= 1 && abs(xj - numJ) <= 1 && abs(xi - numI) + abs(xj - numJ) <= 1) {
		mode = CANNOT_MOVE;

		return 1;
	}

	return 0;
}

void swapTiles() {
	if (mode != CHOOSE)
		return;

	if (inputKey == ENTER && board[numI][numJ] != 16 && canMoveTile()) {
		board[xi][xj] = board[numI][numJ];
		board[numI][numJ] = 16;

		mode = CHOOSE;
		nMoves++;
	}
	else if (inputKey == UP || inputKey == DOWN || inputKey == RIGHT || inputKey == LEFT)
		move(inputKey);
}

// return 1 for solvable, 0 for non-solvable
int isSolvable() {
	int arr[16];
	int k = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			arr[k] = board[i][j];
			k++;
		}

	int inversions = 0;

	for (int i = 0; i < 15; i++)
		if (arr[i] > arr[i + 1] && arr[i] != 16 && arr[i + 1] != 16)
			inversions++;

	if ((inversions % 2 == 0 && (4 - xi) % 2 != 0) || (inversions % 2 != 0 && (4 - xi) % 2 == 0))
		return 1;

	return 0;
}

void initGame() {
	nMoves = 0;
	mode = CHOOSE;

	shuffleBoard();
	while (isSolvable() == 0)
		shuffleBoard();

	getXCoords();
	numI = 0;
	numJ = 0;
}

void resume() {
	FILE *f;
	char buff[3];

	f = fopen("./state.txt", "r");

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			fscanf(f, "%s", buff);	

			board[i][j] = atoi(buff);
		}

	
	fscanf(f, "\n%s", buff);
	nMoves = atoi(buff);

	fclose(f);

	mode = CHOOSE;
	numI = 0;
	numJ = 0;
	getXCoords();
}

void displayWinText() {
	printf("\n\n\tCongratulation. You have won!\n");
}

void exitGame() {
	printf("\n\nQuiting...\n");
	exit(0);
}

void save() {
	FILE *f;

	f = fopen("./state.txt", "w");

	// save board state
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			fprintf(f, "%d ", board[i][j]);

	// save game state
	fprintf(f, "\n%d", nMoves);

	fclose(f);
}

void play(int shouldResume) {
	// setup
	if (shouldResume == 1)
		resume();
	else
		initGame();
	
	// something like draw() function
	while(1) {
		system("clear");
		displayBoard();

		if (mode == WON) {
			displayWinText();
			exit(0);
		}


		displayProgress();
		displayInputHelp();

		mode = CHOOSE;
		getXCoords();

		inputKey = (int)getchar();

		if (inputKey == 'q')
			exitGame();
		else if (inputKey == 'r')
			initGame();
		else if (inputKey == 's') {
			save();
			exit(0);
		}

		if (mode == CHOOSE)
			swapTiles();

		// if won, game mode should turn WON
		won();
	}
}

int main(void) {
	int c = menu();
	
	switch(c) {
		case START: play(0); break;
		case EXIT: return 0;
		case RESUME: play(1); break;
		default: break;
	}

	return 0;
}
