#pragma once
#ifndef __MAP__
#define __MAP__
#include "mob.h"
#include "other_func.h"
class map
{
	friend class mob;
private:
	int level, row, col;
	char** map_pt;//pointer to a map
	mob player1, player2, mob1, mob2;
public:
	map(int gamelevel) ://create a map with row* column
		level(gamelevel), player1(), player2(), mob1(), mob2()
	{
		newmap(gamelevel);
	}
	void newmap(int gamelevel);
	~map() 
	{
		for (int i = 0; i < row; ++i)
			delete[] map_pt[i];
		delete[] map_pt;
	}
	char** usemap() { return map_pt; }
	int userow() { return row; }
	int usecol() { return col; }
	friend void display(map&);
};
#endif // !__MAP__


