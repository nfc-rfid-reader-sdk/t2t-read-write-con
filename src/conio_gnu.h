/*
 * conio_gnu.h
 *
 *  Created on: 31.05.2016.
 *      Author: www.d-logic.net
 */

#ifndef CONIO_GNU_H_
#define CONIO_GNU_H_
#if linux || __linux__ || __APPLE__

// Read 1 character without echo:
char getch(void);

// Read 1 character with echo:
char getche(void);

#endif // linux || __linux__ || __APPLE__
#endif /* CONIO_GNU_H_ */
