#include <conio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <queue>
#include <vector>
#include <Windows.h>
using std::cin;
using std::cout;
using std::priority_queue;
using std::vector;

long long cnt = 0;
const int TICK = CLOCKS_PER_SEC / 20;//ÖÂ¾´mc
clock_t clocktime;
char** mpt = 0, **ppt;
float VELOCITY = 0.5;
const int MAX_LEVEL = 2;
const int DEFAULTHP = 1000;
const float BONUS_P = 0.2;
const float SPEEDUP_P = 0.5;
const float POWERUP_P = 0.5;

class map
{
private:
	int level, row, col;
	char** map_pt;//pointer to a map
	char** map_pos;//pointer to a map of objects
public:
	friend void init(map&);
	map(int gamelevel) ://create a map with row* column
		level(gamelevel)
	{}
	~map()
	{
		if (!mpt)return;
		for (int i = 0; i < row + 2; ++i)
			delete[] map_pt[i];
		delete[] map_pt;
		for (int i = 0; i < row + 2; ++i)
			delete[] map_pos[i];
		delete[] map_pos;
	}
	int userow() { return row; }
	int usecol() { return col; }
};
class mob
{
private:
	char name;
	int posx, posy;
	float speed, remainx, remainy;
	int hp;
	int power;
public:
	friend void trykill(mob& gamemob, int x, int y);
	friend void init(map&);
	friend void display();
	friend void setbomb(mob&);
	mob(int x, int y) : posx(x), posy(y),
		speed(VELOCITY), remainx(0), remainy(0), 
		hp(DEFAULTHP), power(2) {}
	mob() : posx(0), posy(0) {}
	bool move_up(float);
	bool move_left(float);
	bool move_down(float);
	bool move_right(float);
};
class bomb {
private:
	int posx, posy;
	int power;
	int triggertime;
	mob* player;
public:
	friend void setbomb(mob&);
	friend void explode(const bomb&);
	bool operator<(const bomb& a) const{
		return this->triggertime > a.triggertime; //Ð¡¶¥¶Ñ
	}
	int usetime() const{ return triggertime; }
	int usepower() const { return power; }
	bomb(int x, int y, int p, int t) :
		posx(x), posy(y), power(p), triggertime(t) {}
};
priority_queue<bomb>eventlist;


void init(map&);
void display();
inline bool check(char ch);
void deal_with_input();
void explode(const bomb& gamebomb);
map gamemap(-1);
mob player1(0,0), player2(0,0), 
	mob1(0,0), mob2(0,0);


int main()
{
	srand(clock());
	/*int lev;
	cout << "ÇëÊäÈë¹Ø¿¨Êý£º\n";
	cin >> lev;
	if (lev > MAX_LEVEL || lev <= 0)
		lev = 1;
	*/
	gamemap = map(2);
	init(gamemap);
	display();
	while (1)
	{
		clocktime = clock();
		deal_with_input();
		display();
		while (!eventlist.empty() && eventlist.top().usetime() <= cnt) {
			if (eventlist.top().usepower() > 0)
				explode(eventlist.top());
			else if (eventlist.top().usepower() == -1);

			else if (eventlist.top().usepower() == -2);

			eventlist.pop();
		}
		while (clock() - clocktime <= TICK);
		++cnt;
	}
	return 0;
}


void init(map& gamemap)
{
	int gamelevel = gamemap.level;
	std::ifstream fin;
	std::string str1 = "resource_pack\\level",
		str2 = "01", str3 = "_map.txt";
	str2[0] = gamelevel / 10 ^ '0';
	str2[1] = gamelevel % 10 ^ '0';
	fin.open(str1 + str2 + str3);
	fin >> gamemap.row >> gamemap.col;
	fin >> player1.posx >> player1.posy;
	fin >> player2.posx >> player2.posy;
	fin >> mob1.posx >> mob1.posy;
	fin >> mob2.posx >> mob2.posy;
	int row = gamemap.row, col = gamemap.col;
	mpt = gamemap.map_pt = new char* [row + 2];
	ppt = gamemap.map_pos = new char* [row + 2];
	for (int i = 0; i < row + 2; ++i) {
		mpt[i] = new char[col + 2]{};
		ppt[i] = new char[col + 2]{};
		for (int j = 0; j < col + 2; ++j) {
			fin >> mpt[i][j];
			if (mpt[i][j] == '0')
				mpt[i][j] = ' ';
		}
	}
	player1.name = 'A';
	player2.name = 'B';
	mob1.name = 'S';
	mob2.name = 'T';
	ppt[player1.posx][player1.posy] = 'A';
	ppt[player2.posx][player2.posy] = 'B';
	ppt[mob1.posx][mob2.posy] = 'S';
	ppt[mob2.posx][mob2.posy] = 'T';
}
void display()
{
	system("cls");
	if (mob2.hp >= 0)
	ppt[mob2.posx][mob2.posy] = mob2.name;
	if (mob1.hp >= 0)
	ppt[mob1.posx][mob1.posy] = mob1.name;
	if (player2.hp >= 0)
	ppt[player2.posx][player2.posy] = player2.name;
	if (player1.hp >= 0)
	ppt[player1.posx][player1.posy] = player1.name;
	for (int i = 1; i <= gamemap.userow(); ++i) {
		for (int j = 1; j <= gamemap.usecol(); ++j) {
			if (ppt[i][j])
				cout << ppt[i][j];
			else
				cout << mpt[i][j];
		}
		cout << std::endl;
	}
}

//rewrite
void deal_with_input()
{
	char ch;
	if (_kbhit()) {
		if (GetAsyncKeyState('W'))
			player1.move_up(VELOCITY);
		if (GetAsyncKeyState('A'))
			player1.move_left(VELOCITY);
		if (GetAsyncKeyState('S'))
			player1.move_down(VELOCITY);
		if (GetAsyncKeyState('D'))
			player1.move_right(VELOCITY);
		if (GetAsyncKeyState('I'))
			player2.move_up(VELOCITY);
		if (GetAsyncKeyState('J'))
			player2.move_left(VELOCITY);
		if (GetAsyncKeyState('K'))
			player2.move_down(VELOCITY);
		if (GetAsyncKeyState('L'))
			player2.move_right(VELOCITY);
		if (GetAsyncKeyState(VK_SPACE))
			setbomb(player1);
		if (GetAsyncKeyState(VK_RETURN))
			setbomb(player2);
	}
}
bool mob::move_up(float sp)
{
	if (hp <= 0) return 0;
	remainx -= sp;
	if (remainx > -1) return true;
	remainx += 1;
	if (check(mpt[posx - 1][posy])) {
		ppt[posx][posy] = 0;
		--posx;
		ppt[posx][posy] = name;
		return 1;
	}
	else if (mpt[posx - 1][posy] == '?') {

		ppt[posx][posy] = 0;
		--posx;
		ppt[posx][posy] = name;
		return 1;
	}
	else return 0;
}
bool mob::move_left(float sp)
{
	if (hp <= 0) return 0;
	remainy -= sp;
	if (remainy > -1) return true;
	remainy += 1;
	if (check(mpt[posx][posy - 1])) {
		ppt[posx][posy] = 0;
		--posy;
		ppt[posx][posy] = name;
		return 1;
	}
	else return 0;
}
bool mob::move_down(float sp)
{
	if (hp <= 0) return 0;
	remainx += sp;
	if (remainx < 1) return true;
	remainx -= 1;
	if (check(mpt[posx + 1][posy])) {
		ppt[posx][posy] = 0;
		++posx;
		ppt[posx][posy] = name;
		return 1;
	}
	else return 0;
}
bool mob::move_right(float sp)
{
	if (hp <= 0) return 0;
	remainy += sp;
	if (remainy < 1) return true;
	remainy -= 1;
	if (check(mpt[posx][posy + 1])) {
		ppt[posx][posy] = 0;
		++posy;
		ppt[posx][posy] = name;
		return 1;
	}
	else return 0;
}
inline bool check(char ch) {
	if (ch == 'H' || ch == '*' || ch == 'o')
		return 0;
	return 1;
}
void setbomb(mob& gamemob)
{
	int x = gamemob.posx,
		y = gamemob.posy;
	mpt[x][y] = 'o';
	eventlist.push(bomb(x, y, gamemob.power, cnt + 3 * 20));
}
void explode(const bomb& gamebomb) 
{
	int const power = gamebomb.power,
		const x = gamebomb.posx,
		const y = gamebomb.posy;
	if (power > 0) {
		mpt[x][y] = ' ';
		for (int i = x; i > 0 && i > x - power; --i) {
			if (mpt[i][y] != '*' && !check(mpt[i][y])) break;
			if (mpt[i][y] == '*'){
				if ((rand() % 100) / 100.0 < BONUS_P)
					mpt[i][y] = '?';
				else
					mpt[i][y] = ' ';
			}
			trykill(player1, i, y);
			trykill(player2, i, y);
			trykill(mob1, i, y);
			trykill(mob2, i, y);
		}
		for (int i = x; i <= gamemap.userow() && i < x + power; ++i) {
			if (mpt[i][y] != '*' && !check(mpt[i][y])) break;
			if (mpt[i][y] == '*'){
				if ((rand() % 100) / 100.0 < BONUS_P)
					mpt[i][y] = '?';
				else
					mpt[i][y] = ' ';
			}
			trykill(player1, i, y);
			trykill(player2, i, y);
			trykill(mob1, i, y);
			trykill(mob2, i, y);
		}
		for (int i = y; i > 0 && i > y - power; --i) {
			if (mpt[x][i] != '*' && !check(mpt[x][i])) break;
			if (mpt[x][i] == '*') {
				if ((rand() % 100) / 100.0 < BONUS_P)
					mpt[x][i] = '?';
				else
					mpt[x][i] = ' ';
			}
			trykill(player1, x, i);
			trykill(player2, x, i);
			trykill(mob1, x, i);
			trykill(mob2, x, i);
		}
		for (int i = y; i < gamemap.usecol() && i < y + power; ++i) {
			if (mpt[x][i] != '*' && !check(mpt[x][i])) break;
			if (mpt[x][i] == '*') {
				if ((rand() % 100) / 100.0 < BONUS_P)
					mpt[x][i] = '?';
				else
					mpt[x][i] = ' ';
			}
			trykill(player1, x, i);
			trykill(player2, x, i);
			trykill(mob1, x, i);
			trykill(mob2, x, i);
		}
	}
}
void trykill(mob& gamemob, int x, int y) {
	if (gamemob.posx == x && gamemob.posy == y) {
		gamemob.hp -= DEFAULTHP;
		if (gamemob.hp <= 0) {
			gamemob.hp = -1;
			ppt[gamemob.posx][gamemob.posy] = 0;
			gamemob.posx = 0;
			gamemob.posy = 0;
		}
	}
}