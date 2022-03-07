#include <conio.h>
#include <iostream>
using std::cout;
void deal_with_input()
{
	char ch;
	if (_kbhit()) {
		ch = _getch();
		switch(ch) {
		case 'w':cout << 1; break;
		case 'a':cout << 2; break;
		case 's':cout << 3; break;
		case 'd':cout << 4; break;
		}
	}
}