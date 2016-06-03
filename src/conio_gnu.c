/*
 * conio_gnu.c
 *
 *  Created on: 31.05.2016.
 *      Author: www.d-logic.net
 */

#if linux || __linux__ || __APPLE__

#include <termios.h>
#include <stdio.h>

static struct termios old, new;

// Initialize new terminal i/o settings:
void initTermios(int echo)
{
	tcgetattr(0, &old); // grab old terminal i/o settings
	new = old; // make new settings same as old settings
	new.c_lflag &= ~ICANON; // disable buffered i/o
	new.c_lflag &= echo ? ECHO : ~ECHO; // set echo mode
	tcsetattr(0, TCSANOW, &new); // use these new terminal i/o settings now
}

// Restore old terminal i/o settings:
void resetTermios(void)
{
	tcsetattr(0, TCSANOW, &old);
}

// Read 1 character - echo defines echo mode:
char getch_(int echo)
{
	char ch;
	initTermios(echo);
	ch = getchar();
	resetTermios();
	return ch;
}

// Read 1 character without echo:
char getch(void)
{
	return getch_(0);
}

// Read 1 character with echo:
char getche(void)
{
	return getch_(1);
}

// Testing on OSX:
/*
int mygetch( ) {
	struct termios oldt, newt;
	int ch;

	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}
*/

#endif // linux || __linux__ || __APPLE__
