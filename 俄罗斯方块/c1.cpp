#include <stdio.h>
#include <graphics.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib") 

#define COL_BEGIN 1								
#define COL_END   15
#define ROW_BEGIN 0
#define ROW_END   21

static const unsigned int tetristype[7][4] = {			//方块的十六进制存储
	{0x0033,0x0033,0x0033,0x0033},						//o型
	{0x000F,0x1111,0x000F,0x1111},						//I型
	{0x0072,0x0262,0x0270,0x0232},						//T型
	{0x0071,0x0047,0x0322,0x0223},						//~L型
	{0x0036,0x0231,0x0036,0x0231},						//~Z型
	{0x0063,0x0132,0x0063,0x0132},						//Z型
	{0x0017,0x0223,0x0074,0x0311}						//L型
};

//游戏池存储方式为十六进制，初始化为1000000000000001，最后一行初始化1111111111111111

static unsigned int tetrispool[22] = {0x8001,0x8001,0x8001,
0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,
0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,
0x8001,0x8001,0xFFFF};									//游戏池初始化

//创建游戏信息结构体，存储游戏的信息

typedef struct TetrisInformation
{
	unsigned int pool[22];
	unsigned int correntx;
	unsigned int correnty;
	unsigned int type[3];
	unsigned int rotatestate[3];
	unsigned int count;
	unsigned int score;
	BOOL live;
}Tetrisinformation;

//创建游戏控制结构体，存储游戏控制的相关信息

typedef struct TetrisControl
{
	BOOL pause;
	BOOL directionx;
	unsigned int everycolor[21][14];
}TetrisControl;

int StartFace();										//游戏开始界面
int StartGame();										//游戏开始
int PrintBorder();										//输出游戏边框
int PrintPrompt();										//输出游戏的提示信息
int NextTetris();										//输出下一个，下下个方块的信息
int RunGame();											//运行游戏
int PrintGamePool();									//输出游戏池
int GiveTetris();										//产生一个新的方块
int SetPoolColor();										//设置颜色
int JoinTetris();										//将新的方块加入游戏池数组
int CheckCollision();									//碰撞检测
int CheckElim();										//消行检测
int KeyControl();										//接受按键信息
int RotaTetris();										//旋转方块
int HoriMoveTetris();									//平移方块
int DownMoveTetris();									//下移方块
int DownDownMoveTetris();								//直接下落
int RemoveTetris();										//移除当前方块
int TryAgain();											//重新开始游戏
int GameOver();											//游戏结束界面
int Bgm();												//设置背景音乐

//定义方块信息和方块控制两个数据为全局变量

Tetrisinformation tetrisinformation;
TetrisControl tetriscontrol;

int main()
{
	initgraph(600,480);
	Bgm();
	StartFace();
	getch();
	cleardevice();
	do 
	{
		StartGame();
		PrintBorder();
		PrintPrompt();
		RunGame();
	} while (1);
	closegraph();
	return 0;
}

//开始游戏，初始化游戏信息

int StartGame()
{
	memset(tetrisinformation.pool,0,sizeof(tetrisinformation.pool));
	memset(tetriscontrol.everycolor,0,sizeof(tetriscontrol.everycolor));
	memcpy(tetrisinformation.pool, tetrispool, sizeof(unsigned)*22);
	tetrisinformation.live = 0;
	tetriscontrol.pause = 0;
	tetrisinformation.correntx = 6;
	tetrisinformation.score = 0;
	srand((int)time(NULL));
	tetrisinformation.type[1] = rand()%7;
	tetrisinformation.rotatestate[1] = rand()%4;		//产生下一个的方块和旋转类型
	tetrisinformation.type[2] = rand()%7;
	tetrisinformation.rotatestate[2] = rand()%4;		//产生下下个的方块和旋转类型
	GiveTetris();
	SetPoolColor();
	return 0;
}

int GiveTetris()
{
	unsigned int tetris = 0x0;
	tetrisinformation.type[0] = tetrisinformation.type[1];
	tetrisinformation.rotatestate[0] = tetrisinformation.rotatestate[1];		//将下一个方块信息赋值到当前方块
	tetrisinformation.type[1] = tetrisinformation.type[2];
	tetrisinformation.rotatestate[1] = tetrisinformation.rotatestate[2];
	tetrisinformation.type[2] = rand()%7;
	tetrisinformation.rotatestate[2] = rand()%4;								//产生新的方块
	tetris = tetristype[tetrisinformation.type[0]][tetrisinformation.rotatestate[0]];
	if(CheckCollision())
	{
		tetrisinformation.live = 1;												//碰撞检测并判断游戏结束
	}
	else
	{
		JoinTetris();
	}
	return 0;
}

//设置游戏池颜色

int SetPoolColor()
{
	unsigned int tetris = tetristype[tetrisinformation.type[0]][tetrisinformation.rotatestate[0]];
	int i = 0;
	int x = 0;
	int y = 0;
	for(i = 0;i < 16;i++)
	{
		y = (i >> 2) + tetrisinformation.correnty;				//随i值变化，y的取值为0,1,2,3  +correnty
		if(y > ROW_END)
			break;
		x = (i & 3) + tetrisinformation.correntx;				//随i值变化，x的取值为0,1,2,3  +correntx
		if((tetris >> i) & 1)									//当前位置有方块
		{
			switch(tetrisinformation.type[0])					//更加方块类型不同，赋值不同颜色
			{
				case 0:
					tetriscontrol.everycolor[y][x] = RED;
					break;
				case 1:
					tetriscontrol.everycolor[y][x] = YELLOW;
					break;
				case 2:
					tetriscontrol.everycolor[y][x] = BLUE;
					break;
				case 3:
					tetriscontrol.everycolor[y][x] = GREEN;
					break;
				case 4:
					tetriscontrol.everycolor[y][x] = 0xCAE1FF;
					break;
				case 5:
					tetriscontrol.everycolor[y][x] = 0xB8860B;
					break;
				case 6:
					tetriscontrol.everycolor[y][x] = 0xC0FF3E;
					break;
			}
		}
	}
	return 0;
}

//将方块加入到游戏池数组中
//每次读4位，并且移动到游戏池中间

int JoinTetris()
{
	unsigned int tetris = tetristype[tetrisinformation.type[0]][tetrisinformation.rotatestate[0]];
	tetrisinformation.pool[tetrisinformation.correnty+0] |= (((tetris >> 0x0) & 0x000F) << tetrisinformation.correntx);
	tetrisinformation.pool[tetrisinformation.correnty+1] |= (((tetris >> 0x4) & 0x000F) << tetrisinformation.correntx);
	tetrisinformation.pool[tetrisinformation.correnty+2] |= (((tetris >> 0x8) & 0x000F) << tetrisinformation.correntx);
	tetrisinformation.pool[tetrisinformation.correnty+3] |= (((tetris >> 0xC) & 0x000F) << tetrisinformation.correntx);
	return 0;
}

int PrintBorder()
{
	setcolor(RGB(141,238,180));
	setlinestyle(SOLID_LINE,NULL,10);
    line(35,15,340,15);									//上
	line(35,15,35,465);									//左
	line(35,465,340,465);								//下
	line(340,20,340,460);								//右
	line(340,15,500,15);								//副上
	line(340,465,500,465);								//副下
	line(500,20,500,460);								//副右
	line(340,200,500,200);								//副中
	return 0;
}

int PrintPrompt()
{
	char temp[10];
	outtextxy(360,220,"LEFT           A");
	outtextxy(360,250,"RIGHT         D");
	outtextxy(360,280,"DOWN        S");
	outtextxy(360,310,"CHANGE     W");
	outtextxy(360,340,"PAUSE     SPACE");
	outtextxy(360,390,"Score:");
	sprintf(temp,"%d",tetrisinformation.score);
	xyprintf(420,390,temp);
	return 0;
}

//输出下个方块和下下方块的形状和颜色

int NextTetris()
{
	int i = 0;
	int x = 0;
	int y = 0;
	//下一个方块
	unsigned int tetris = tetristype[tetrisinformation.type[1]][tetrisinformation.rotatestate[1]];
	for(i = 0;i < 16;i++)
	{
		y = i >> 2;
		x = i & 3;
		setfillcolor(NULL);
		bar(370+x*21,50+y*21,390+x*21,70+y*21);
		if(tetris)
		{
			if((tetris & 1) == 1)
			{
				switch(tetrisinformation.type[1])
				{
				case 0:
					setfillcolor(RED);
					break;
				case 1:
					setfillcolor(YELLOW); 
					break;
				case 2:
					setfillcolor(BLUE);
					break;
				case 3:
					setfillcolor(GREEN);
					break;
				case 4:
					setfillcolor(0xCAE1FF);
					break;
				case 5:
					setfillcolor(0xB8860B);
					break;
				case 6:
					setfillcolor(0xC0FF3E);
					break;
				}
				bar(370+x*21,50+y*21,390+x*21,70+y*21);
			}
			else
			{
				setfillcolor(NULL);
				bar(370+x*21,50+y*21,390+x*21,70+y*21);
			}
			tetris = tetris >> 1;
		}
	}
	//下下个方块
	tetris = tetristype[tetrisinformation.type[2]][tetrisinformation.rotatestate[2]];
		for(i = 0;i < 16;i++)
		{
			y = i >> 2;
			x = i & 3;
			setfillcolor(NULL);
			bar(435+x*11,135+y*11,445+x*11,145+y*11);
			if(tetris)
			{
				if((tetris & 1) == 1)
				{
					switch(tetrisinformation.type[2])
					{
					case 0:
						setfillcolor(RED);
						break;
					case 1:
						setfillcolor(YELLOW); 
						break;
					case 2:
						setfillcolor(BLUE);
						break;
					case 3:
						setfillcolor(GREEN);
						break;
					case 4:
						setfillcolor(0xCAE1FF);
						break;
					case 5:
						setfillcolor(0xB8860B);
						break;
					case 6:
						setfillcolor(0xC0FF3E);
						break;
					}
					bar(435+x*11,135+y*11,445+x*11,145+y*11);
				}
				else
				{
					setfillcolor(NULL);
					bar(435+x*11,135+y*11,445+x*11,145+y*11);
				}
				tetris = tetris >> 1;
			}
		}
	return 0;
}

int RunGame()
{
	PrintGamePool();
	while(!tetrisinformation.live)								//检测是否存活	
	{
		PrintPrompt();
		delay_fps(3);											//游戏刷新为1/3s
		NextTetris();
		if(kbhit())												//检测是否有按键消息
		{
			KeyControl();
		}
		if(!tetriscontrol.pause)								//判断游戏是否暂停
		{
			DownMoveTetris();
		}
	}
	if(tetrisinformation.live)
	{
		tetrisinformation.score = 0;
		TryAgain();
	}
	return 0;
}

//输出游戏池信息
//方块的信息都存储在游戏池中，刷新游戏池即可输出方块

int PrintGamePool()
{
	int x = 0;
	int y = 0;
	for(y = ROW_BEGIN;y < ROW_END;y++)
	{
		for(x = COL_BEGIN;x < COL_END;x++)
		{
			if(tetrisinformation.pool[y] >> x & 1)
			{
				setfillcolor(tetriscontrol.everycolor[y][x]);
				bar(20+x*21,20+y*21,40+x*21,40+y*21);
			}
			else
			{
				setfillcolor(NULL);
				bar(20+x*21,20+y*21,40+x*21,40+y*21);
			}
		}
	}
	return 0;
}

//碰撞检测
//将即将下落的位置的信息存储为十六进制数，然后与当前方块的十六进制数进行与运算，结果不为0则发生碰撞

int CheckCollision()
{
	unsigned int tetris = tetristype[tetrisinformation.type[0]][tetrisinformation.rotatestate[0]];
	unsigned int flag = 0;
	flag |= (((tetrisinformation.pool[tetrisinformation.correnty+0] >> tetrisinformation.correntx) << 0x0) & 0x000F);
	flag |= (((tetrisinformation.pool[tetrisinformation.correnty+1] >> tetrisinformation.correntx) << 0x4) & 0x00F0);
	flag |= (((tetrisinformation.pool[tetrisinformation.correnty+2] >> tetrisinformation.correntx) << 0x8) & 0x0F00);
	flag |= (((tetrisinformation.pool[tetrisinformation.correnty+3] >> tetrisinformation.correntx) << 0xC) & 0xF000);
	return ((flag & tetris) != 0);
}

//消行检测

int CheckElim()
{
	int i = 0;
	int y = tetrisinformation.correnty+3;
	do 																//从下往上检测是否为0xFFFF
	{
		if(y < ROW_END && tetrisinformation.pool[y] == 0xFFFF)
		{
			memmove(tetrisinformation.pool + 1,tetrisinformation.pool,sizeof(unsigned) * y);		//将游戏池覆盖移动，消行
			memmove(tetriscontrol.everycolor + 1,tetriscontrol.everycolor,sizeof(unsigned) * y);
			tetrisinformation.score += 10;
		}
		else
		{
			--y;
			++i;
		}
	} while (y >= tetrisinformation.correnty && i < 4);
	tetrisinformation.correntx = 6;
	tetrisinformation.correnty = 0;
	GiveTetris();
	SetPoolColor();
	return 0;
}

int KeyControl()
{
	int key = 0;
	key = getch();
	if(key == 32)
	{
		tetriscontrol.pause = !tetriscontrol.pause;
	}
	if(tetriscontrol.pause)
	{
		return 0;
	}
	switch(key)
	{
		case 97:
			tetriscontrol.directionx = 0;
			HoriMoveTetris();
			break;
		case 100:
			tetriscontrol.directionx = 1;
			HoriMoveTetris();
			break;
		case 115:
			DownDownMoveTetris();
			break;
		case 119:
			RotaTetris();
			break;
	}
	return 0;
}

int RotaTetris()
{
	unsigned int orien = tetrisinformation.rotatestate[0];
	RemoveTetris();
	tetrisinformation.rotatestate[0] = (orien + 1) & 3;
	if(CheckCollision())
	{
		tetrisinformation.rotatestate[0] = orien;
		JoinTetris();
	}
	else
	{
		JoinTetris();
		SetPoolColor();
		PrintGamePool();
	}
	return 0;
}

int HoriMoveTetris()
{
	int x = tetrisinformation.correntx;
	RemoveTetris();
	tetriscontrol.directionx == 0 ? (--tetrisinformation.correntx) : (++tetrisinformation.correntx);
	if(CheckCollision())
	{
		tetrisinformation.correntx = x;
		JoinTetris();
	}
	else
	{
		JoinTetris();
		SetPoolColor();
		PrintGamePool();
	}
	return 0;
}

int DownMoveTetris()
{
	int y = tetrisinformation.correnty;
	RemoveTetris();
	++tetrisinformation.correnty;
	if(CheckCollision())
	{
		tetrisinformation.correnty = y;
		JoinTetris();
		CheckElim();
	}
	else
	{
		JoinTetris();
		SetPoolColor();
		PrintGamePool();
	}
	return 0;
}

//直接下落方块

int DownDownMoveTetris()
{
	int y = tetrisinformation.correnty;
	RemoveTetris();
	for(;tetrisinformation.correnty < ROW_END;tetrisinformation.correnty++)
	{
		if(CheckCollision())
			break;
	}
	--tetrisinformation.correnty;					//检测到碰撞后，上一个位置没有发生碰撞
	JoinTetris();
	SetPoolColor();
	CheckElim();
	PrintGamePool();
	return 0;
}

//移除当前方块
//将当前方块十六进制数取反，与当前方块位与运算

int RemoveTetris()
{
	int tetris = tetristype[tetrisinformation.type[0]][tetrisinformation.rotatestate[0]];
	tetrisinformation.pool[tetrisinformation.correnty + 0] &= ~(((tetris >> 0x0) & 0x000F) << tetrisinformation.correntx);
	tetrisinformation.pool[tetrisinformation.correnty + 1] &= ~(((tetris >> 0x4) & 0x000F) << tetrisinformation.correntx);
	tetrisinformation.pool[tetrisinformation.correnty + 2] &= ~(((tetris >> 0x8) & 0x000F) << tetrisinformation.correntx);
	tetrisinformation.pool[tetrisinformation.correnty + 3] &= ~(((tetris >> 0xC) & 0x000F) << tetrisinformation.correntx);
	return 0;
}

int TryAgain()
{
	GameOver();
	cleardevice();
	return 0;
}

int StartFace()
{
	PIMAGE img = newimage();
	getimage(img,"界面.jpg");
	putimage (0,0,img);
	return 0;
}

int Bgm()
{
	PlaySound(TEXT("bgm.wav"),NULL,SND_LOOP|SND_ASYNC);
	return 0;
}

int GameOver()
{
	PIMAGE img = newimage();
	getimage(img,"结束.jpg");
	putimage (0,0,img);
	getch();
	return 0;
}





