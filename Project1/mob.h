#pragma once
#ifndef __MOB__
#define __MOB__
class mob
{
	friend class map;
private:
	int posx, posy;

public:
	mob(int x, int y) : posx(x), posy(y) {}
	mob(): posx(0), posy(0){}
	/*void move_w();
	void move_a();
	void move_s();
	void move_d();
	*/
};
#endif // !__MOB__

