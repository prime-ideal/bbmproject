#include "other_func.h"
#include <iostream>
using std::cout;
void display(map& gamemap)
{
	system("cls");
	for (int i = 0; i < gamemap.row; ++i) {
		for (int j = 0; j < gamemap.col; ++j)
			cout << gamemap.map_pt[i][j];
		cout << std::endl;
	}
}