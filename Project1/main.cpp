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
const int TICK = CLOCKS_PER_SEC / 20;//致敬mc
clock_t clocktime, clocktime0;
char** mpt = 0, **ppt;
float VELOCITY = 0.5;
const int TOTAL_TIME = 300;//倒计时5分钟
const int MAX_LEVEL = 2;
const int DEFAULTHP = 1000;//扣血
const float BONUS_P = 1;//出道具概率
const float SPEEDUP_P = 0.5;//道具加速概率
const float POWERUP_P = 0.5;//道具炸弹增幅概率
const int effect_ticks = 20 * 20;
const int INF = 0x3f3f3f3f;
const int MOVESCORE = 1;//移动得分
const int BONUSSCORE = 100;//吃道具得分
const int SETBOMBSCORE = 10;//放炸弹得分
const int EXPLODESCORE = 10000;//炸敌人得分

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
	long long speedtime, powertime;
	long long score;
public:
	friend bool trykill(mob& gamemob, int x, int y);
	friend void init(map&);
	friend bool display();
	friend void setbomb(mob*);
	mob(int x, int y) : posx(x), posy(y),
		speed(VELOCITY), remainx(0), remainy(0), 
		hp(DEFAULTHP), power(2), speedtime(0), powertime(0), score(0) {}
	mob() : posx(0), posy(0) {}
	long long usespeedtime() const { return speedtime; }
	long long usepowertime() const { return powertime; }
	long long usescore() const { return score; }
	char usename() const { return name; }
	int usehp() const { return hp; }
	int usex() const { return posx; }
	int usey() const { return posy; }
	void setpower(int x) { power = x; }
	void setspeed(float x) { speed = x; }
	void sethp(int x) { hp = x; }
	void setscore(long long x) { score = x; }
	void addscore(long long x) { score += x; }
	bool move_up();
	bool move_left();
	bool move_down();
	bool move_right();
};
class bomb {
private:
	int posx, posy;
	int power;
	long long triggertime;
	mob* playerpt;
public:
	friend void setbomb(mob*);
	friend void explode(const bomb&);
	bool operator<(const bomb& a) const{
		return this->triggertime > a.triggertime; //小顶堆
	}
	long long usetime() const{ return triggertime; }
	int usepower() const { return power; }
	mob* usemobpt() const { return playerpt; }
	int useposx() const { return posx; }
	int useposy() const { return posy; }
	bomb(mob* plerpt, int x, int y, int p, long long t) :
		posx(x), posy(y), power(p), triggertime(t), playerpt(plerpt) {}
};
priority_queue<bomb>eventlist;


void init(map&);
bool display();
inline bool check(char ch);
void deal_with_input();
void deal_with_eventlist();
void explode(const bomb& gamebomb);
map gamemap(-1);
mob player1(0,0), player2(0,0), 
	mob1(0,0), mob2(0,0);


int main()
{
	srand(clock());
	int lev;
	cout << "请输入关卡数：\n";
	cin >> lev;
	if (lev > MAX_LEVEL || lev <= 0)
		lev = 1;
	clocktime = clocktime0 = clock();
	gamemap = map(lev);
	init(gamemap);
	display();
	while (1)
	{
		clocktime = clock();
		deal_with_input();
		if (display()) break;
		deal_with_eventlist();
	}
	while (!eventlist.empty()) eventlist.pop();
	cnt = 0;
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
bool display()
{
	system("cls");
	if (mob1.hp <= 0 && mob2.hp <= 0) {
		cout << "政清人和";
		Sleep(2000);
		return 1;
	}
	if (player1.hp <= 0 && player2.hp <= 0) {
		cout << "和這群蟲豸在一起，怎能搞好炸弹呢！！";
		Sleep(2000);
		return 1;
	}
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
			else if (mpt[i][j] >= 'x')
				cout << 'x';
			else
				cout << mpt[i][j];
		}
		cout << std::endl;
	}
	cout << std::endl;
	if (player1.hp > 0) {
		cout << "玩家A" << " 血量:" << player1.hp
			<< " 得分：" << player1.usescore();
		if (player1.speedtime > cnt)
			cout << "  速度提升";
		if (player1.powertime > cnt)
			cout << "  爆炸增幅";
	}
	else
		cout << "玩家A RIP";
	cout << std::endl;
	if (player2.hp > 0) {
		cout << "玩家B" << " 血量:" << player2.hp
			<< " 得分：" << player2.usescore();
		if (player2.speedtime > cnt)
			cout << "  速度提升";
		if (player2.powertime > cnt)
			cout << "  爆炸增幅";
	}
	else
		cout << "玩家B RIP";
	cout << std::endl;
	int a = TOTAL_TIME-(clock() - clocktime0) / CLOCKS_PER_SEC;
	cout << "剩余时间： " << '0' << a / 60 << ":";
	if (a % 60 < 10) cout << '0';
	cout << a % 60;
	return 0;
}

void deal_with_input()
{
	if (_kbhit()) {
		if (GetAsyncKeyState('W'))
			player1.move_up();
		if (GetAsyncKeyState('A'))
			player1.move_left();
		if (GetAsyncKeyState('S'))
			player1.move_down();
		if (GetAsyncKeyState('D'))
			player1.move_right();
		if (GetAsyncKeyState('I'))
			player2.move_up();
		if (GetAsyncKeyState('J'))
			player2.move_left();
		if (GetAsyncKeyState('K'))
			player2.move_down();
		if (GetAsyncKeyState('L'))
			player2.move_right();
		if (GetAsyncKeyState(VK_SPACE) && player1.usehp() > 0)
			if (mpt[player1.usex()][player1.usey()] != 'o')
				setbomb(&player1);
		if (GetAsyncKeyState(VK_RETURN) && player2.usehp() > 0)
			if (mpt[player2.usex()][player2.usey()] != 'o')
				setbomb(&player2);
		if (GetAsyncKeyState('1')) {//一键开挂
			if (player1.usehp() > 0) player1.sethp(INF);
			if (player2.usehp() > 0) player2.sethp(INF);
		}
		if (GetAsyncKeyState('2')) {//一键回血
			if (player1.usehp() > 0) player1.sethp(DEFAULTHP);
			if (player2.usehp() > 0) player2.sethp(DEFAULTHP);
		}
	}
}
void deal_with_eventlist() {
	while (!eventlist.empty() && eventlist.top().usetime() <= cnt) {
		if (eventlist.top().usepower() > 0) {
			explode(eventlist.top());
		}
		else if (eventlist.top().usepower() == -1 &&
			eventlist.top().usetime() >= eventlist.top().usemobpt()->usespeedtime()) {
			eventlist.top().usemobpt()->setspeed(VELOCITY);//速度提升
		}
		else if (eventlist.top().usepower() == -2 &&
			eventlist.top().usetime() >= eventlist.top().usemobpt()->usepowertime()) {
			eventlist.top().usemobpt()->setpower(2);//爆炸增幅
		}
		else if (eventlist.top().usepower() == -3) {
			int x = eventlist.top().useposx();
			int y = eventlist.top().useposy();
			if (mpt[x][y] == 'x') mpt[x][y] = ' ';
			if (mpt[x][y] > 'x') --mpt[x][y];
		}
		eventlist.pop();
	}
	while (clock() - clocktime <= TICK);
	++cnt;
}
bool mob::move_up()
{
	if (hp <= 0) return 0;
	remainx -= speed;
	if (remainx > -1) return true;
	remainx += 1;
	if (check(mpt[posx - 1][posy]) && mpt[posx - 1][posy] != '?') {
		ppt[posx][posy] = 0;
		--posx;
		ppt[posx][posy] = name;
		score += MOVESCORE;
		return 1;
	}
	else if (mpt[posx - 1][posy] == '?') {
		if ((rand() % 100) / 100.0 < SPEEDUP_P) {
			speed = VELOCITY * 1.5;
			eventlist.push(bomb(this, 0, 0, -1, cnt + effect_ticks));
			speedtime = cnt + effect_ticks;
		}
		else if ((rand() % 100) / 100.0 < (1 - SPEEDUP_P) / POWERUP_P) {
			power = 3;
			eventlist.push(bomb(this, 0, 0, -2, cnt + effect_ticks));
			powertime = cnt + effect_ticks;
		}
		ppt[posx][posy] = 0;
		--posx;
		ppt[posx][posy] = name;
		mpt[posx][posy] = ' ';
		score += BONUSSCORE;
		return 1;
	}
	else return 0;
}
bool mob::move_left()
{
	if (hp <= 0) return 0;
	remainy -= speed;
	if (remainy > -1) return true;
	remainy += 1;
	if (check(mpt[posx][posy - 1]) && mpt[posx][posy - 1] != '?') {
		ppt[posx][posy] = 0;
		--posy;
		ppt[posx][posy] = name;
		score += MOVESCORE;
		return 1;
	}
	else if (mpt[posx][posy - 1] == '?') {
		if ((rand() % 100) / 100.0 < SPEEDUP_P) {
			speed = VELOCITY * 1.5;
			eventlist.push(bomb(this, 0, 0, -1, cnt + effect_ticks));
			speedtime = cnt + effect_ticks;
		}
		else if ((rand() % 100) / 100.0 < (1 - SPEEDUP_P) / POWERUP_P) {
			power = 3;
			eventlist.push(bomb(this, 0, 0, -2, cnt + effect_ticks));
			powertime = cnt + effect_ticks;
		}
		ppt[posx][posy] = 0;
		--posy;
		ppt[posx][posy] = name;
		mpt[posx][posy] = ' ';
		score += BONUSSCORE;
		return 1;
	}
	else return 0;
}
bool mob::move_down()
{
	if (hp <= 0) return 0;
	remainx += speed;
	if (remainx < 1) return true;
	remainx -= 1;
	if (check(mpt[posx + 1][posy]) && mpt[posx + 1][posy] != '?') {
		ppt[posx][posy] = 0;
		++posx;
		ppt[posx][posy] = name;
		score += MOVESCORE;
		return 1;
	}
	else if (mpt[posx + 1][posy] == '?') {
		if ((rand() % 100) / 100.0 < SPEEDUP_P) {
			speed = VELOCITY * 1.5;
			eventlist.push(bomb(this, 0, 0, -1, cnt + effect_ticks));
			speedtime = cnt + effect_ticks;
		}
		else if ((rand() % 100) / 100.0 < (1 - SPEEDUP_P) / POWERUP_P) {
			power = 3;
			eventlist.push(bomb(this, 0, 0, -2, cnt + effect_ticks));
			powertime = cnt + effect_ticks;
		}
		ppt[posx][posy] = 0;
		++posx;
		ppt[posx][posy] = name;
		mpt[posx][posy] = ' ';
		score += BONUSSCORE;
		return 1;
	}
	else return 0;
}
bool mob::move_right()
{
	if (hp <= 0) return 0;
	remainy += speed;
	if (remainy < 1) return true;
	remainy -= 1;
	if (check(mpt[posx][posy + 1]) && mpt[posx][posy + 1] != '?') {
		ppt[posx][posy] = 0;
		++posy;
		ppt[posx][posy] = name;
		score += MOVESCORE;
		return 1;
	}
	else if (mpt[posx][posy + 1] == '?') {
		if ((rand() % 100) / 100.0 < SPEEDUP_P) {
			speed = VELOCITY * 1.5;
			eventlist.push(bomb(this, 0, 0, -1, cnt + effect_ticks));
			speedtime = cnt + effect_ticks;
		}
		else if ((rand() % 100) / 100.0 < (1 - SPEEDUP_P) / POWERUP_P) {
			power = 3;
			eventlist.push(bomb(this, 0, 0, -2, cnt + effect_ticks));
			powertime = cnt + effect_ticks;
		}
		ppt[posx][posy] = 0;
		++posy;
		ppt[posx][posy] = name;
		mpt[posx][posy] = ' ';
		score += BONUSSCORE;
		return 1;
	}
	else return 0;
}
inline bool check(char ch) {
	if (ch == 'H' || ch == '*' || ch == 'o' || ch >= 'x')
		return 0;
	return 1;
}
void setbomb(mob* gamemob)
{
	if (gamemob->hp <= 0 || gamemob->score == 0) return;
	if (clock() - clocktime0 <= CLOCKS_PER_SEC) return;
	int x = gamemob->posx,
		y = gamemob->posy;
	mpt[x][y] = 'o';
	eventlist.push(bomb(gamemob, x, y, gamemob->power, cnt + 3 * 20));
	gamemob->score += SETBOMBSCORE;
}
void explode(const bomb& gamebomb) 
{
	int const power = gamebomb.power,
		const x = gamebomb.posx,
		const y = gamebomb.posy;
	char ch = gamebomb.usemobpt()->usename();
	int res = 0;
	if (power > 0) {
		mpt[x][y] = ' ';
		for (int i = x; i > 0 && i > x - power; --i) {
			if (mpt[i][y] != '*' && !check(mpt[i][y]) && mpt[i][y] < 'x') break;
			if (mpt[i][y] == '*'){
				if ((rand() % 100) / 100.0 < BONUS_P)
					mpt[i][y] = '?';
				else
					mpt[i][y] = ' ';
			}
			if (mpt[i][y] != '?') {
				if (mpt[i][y] < 'x') mpt[i][y] = 'x';
				else ++mpt[i][y];
				eventlist.push(bomb(&player1, i, y, -3, cnt + 20));//火焰1秒
			}
			res+=
			(trykill(player1, i, y) +
				trykill(player2, i, y) +
				trykill(mob1, i, y) +
				trykill(mob2, i, y))* EXPLODESCORE;
		}
		for (int i = x + 1; i <= gamemap.userow() && i < x + power; ++i) {
			if (mpt[i][y] != '*' && !check(mpt[i][y]) && mpt[i][y] < 'x') break;
			if (mpt[i][y] == '*'){
				if ((rand() % 100) / 100.0 < BONUS_P)
					mpt[i][y] = '?';
				else
					mpt[i][y] = ' ';
			}
			if (mpt[i][y] != '?') {
				if (mpt[i][y] < 'x') mpt[i][y] = 'x';
				else ++mpt[i][y];
				eventlist.push(bomb(&player1, i, y, -3, cnt + 20));
			}
			res +=
				(trykill(player1, i, y) +
					trykill(player2, i, y) +
					trykill(mob1, i, y) +
					trykill(mob2, i, y)) * EXPLODESCORE;
		}
		for (int i = y - 1; i > 0 && i > y - power; --i) {
			if (mpt[x][i] != '*' && !check(mpt[x][i]) && mpt[x][i] < 'x') break;
			if (mpt[x][i] == '*') {
				if ((rand() % 100) / 100.0 < BONUS_P)
					mpt[x][i] = '?';
				else
					mpt[x][i] = ' ';
			}
			if (mpt[x][i] != '?') {
				if (mpt[x][i] < 'x') mpt[x][i] = 'x';
				else ++mpt[x][i];
				eventlist.push(bomb(&player1, x, i, -3, cnt + 20));
			}
			res +=
				(trykill(player1, x, i) +
					trykill(player2, x, i) +
					trykill(mob1, x, i) +
					trykill(mob2, x, i))* EXPLODESCORE;
		}
		for (int i = y + 1; i < gamemap.usecol() && i < y + power; ++i) {
			if (mpt[x][i] != '*' && !check(mpt[x][i]) && mpt[x][i] < 'x') break;
			if (mpt[x][i] == '*') {
				if ((rand() % 100) / 100.0 < BONUS_P)
					mpt[x][i] = '?';
				else
					mpt[x][i] = ' ';
			}
			if (mpt[x][i] != '?') {
				if (mpt[x][i] < 'x') mpt[x][i] = 'x';
				else ++mpt[x][i];
				eventlist.push(bomb(&player1, x, i, -3, cnt + 20));
			}
			res +=
				(trykill(player1, x, i) +
					trykill(player2, x, i) +
					trykill(mob1, x, i) +
					trykill(mob2, x, i))* EXPLODESCORE;
		}
	}
	if (ch == 'A') player1.addscore(res);
	if (ch == 'B') player2.addscore(res);
	if (ch == 'S') mob1.addscore(res);
	if (ch == 'T') mob2.addscore(res);
}
bool trykill(mob& gamemob, int x, int y) {
	if (gamemob.posx == x && gamemob.posy == y) {
		gamemob.hp -= DEFAULTHP;
		if (gamemob.hp <= 0) {
			gamemob.hp = -1;
			ppt[gamemob.posx][gamemob.posy] = 0;
			gamemob.posx = 0;
			gamemob.posy = 0;
		}
		return 1;
	}
	else
		return 0;
}