#include "map.h"
#include <fstream>
#include <cstring>
void map::newmap(int gamelevel)
{
	std::ifstream fin;
	std::string str1 = "resource_pack\\level", 
		str2 = "01", str3 = "_map.txt";
	str2[0] = gamelevel / 10 ^ '0';
	str2[1] = gamelevel % 10 ^ '0';
	fin.open(str1 + str2 + str3);
	fin >> row >> col;
	fin >> player1.posx >> player1.posy;
	fin >> player2.posx >> player2.posy;
	fin >> mob1.posx >> mob1.posy;
	fin >> mob2.posx >> mob2.posy;
	map_pt = new char* [row];
	for (int i = 0; i < row; ++i) {
		map_pt[i] = new char[col];
		for (int j = 0; j < col; ++j) {
			fin >> map_pt[i][j];
			if (map_pt[i][j] == '0')
				map_pt[i][j] = ' ';
		}
	}
	map_pt[player1.posx][player1.posy] = 'A';
	map_pt[player2.posx][player2.posy] = 'B';
	map_pt[mob1.posx][mob2.posy] = 'S';
	map_pt[mob2.posx][mob2.posy] = 'T';
}