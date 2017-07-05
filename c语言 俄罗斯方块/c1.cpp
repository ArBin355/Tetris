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

static const unsigned int tetristype[7][4] = {			//�����ʮ�����ƴ洢
	{0x0033,0x0033,0x0033,0x0033},						//o��
	{0x000F,0x1111,0x000F,0x1111},						//I��
	{0x0072,0x0262,0x0270,0x0232},						//T��
	{0x0071,0x0047,0x0322,0x0223},						//~L��
	{0x0036,0x0231,0x0036,0x0231},						//~Z��
	{0x0063,0x0132,0x0063,0x0132},						//Z��
	{0x0017,0x0223,0x0074,0x0311}						//L��
};

//��Ϸ�ش洢��ʽΪʮ�����ƣ���ʼ��Ϊ1000000000000001�����һ�г�ʼ��1111111111111111

static unsigned int tetrispool[22] = {0x8001,0x8001,0x8001,
0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,
0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,
0x8001,0x8001,0xFFFF};									//��Ϸ�س�ʼ��

//������Ϸ��Ϣ�ṹ�壬�洢��Ϸ����Ϣ

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

//������Ϸ���ƽṹ�壬�洢��Ϸ���Ƶ������Ϣ

typedef struct TetrisControl
{
	BOOL pause;
	BOOL directionx;
	unsigned int everycolor[21][14];
}TetrisControl;

int StartFace();										//��Ϸ��ʼ����
int StartGame();										//��Ϸ��ʼ
int PrintBorder();										//�����Ϸ�߿�
int PrintPrompt();										//�����Ϸ����ʾ��Ϣ
int NextTetris();										//�����һ�������¸��������Ϣ
int RunGame();											//������Ϸ
int PrintGamePool();									//�����Ϸ��
int GiveTetris();										//����һ���µķ���
int SetPoolColor();										//������ɫ
int JoinTetris();										//���µķ��������Ϸ������
int CheckCollision();									//��ײ���
int CheckElim();										//���м��
int KeyControl();										//���ܰ�����Ϣ
int RotaTetris();										//��ת����
int HoriMoveTetris();									//ƽ�Ʒ���
int DownMoveTetris();									//���Ʒ���
int DownDownMoveTetris();								//ֱ������
int RemoveTetris();										//�Ƴ���ǰ����
int TryAgain();											//���¿�ʼ��Ϸ
int GameOver();											//��Ϸ��������
int Bgm();												//���ñ�������

//���巽����Ϣ�ͷ��������������Ϊȫ�ֱ���

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

//��ʼ��Ϸ����ʼ����Ϸ��Ϣ

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
	tetrisinformation.rotatestate[1] = rand()%4;		//������һ���ķ������ת����
	tetrisinformation.type[2] = rand()%7;
	tetrisinformation.rotatestate[2] = rand()%4;		//�������¸��ķ������ת����
	GiveTetris();
	SetPoolColor();
	return 0;
}

int GiveTetris()
{
	unsigned int tetris = 0x0;
	tetrisinformation.type[0] = tetrisinformation.type[1];
	tetrisinformation.rotatestate[0] = tetrisinformation.rotatestate[1];		//����һ��������Ϣ��ֵ����ǰ����
	tetrisinformation.type[1] = tetrisinformation.type[2];
	tetrisinformation.rotatestate[1] = tetrisinformation.rotatestate[2];
	tetrisinformation.type[2] = rand()%7;
	tetrisinformation.rotatestate[2] = rand()%4;								//�����µķ���
	tetris = tetristype[tetrisinformation.type[0]][tetrisinformation.rotatestate[0]];
	if(CheckCollision())
	{
		tetrisinformation.live = 1;												//��ײ��Ⲣ�ж���Ϸ����
	}
	else
	{
		JoinTetris();
	}
	return 0;
}

//������Ϸ����ɫ

int SetPoolColor()
{
	unsigned int tetris = tetristype[tetrisinformation.type[0]][tetrisinformation.rotatestate[0]];
	int i = 0;
	int x = 0;
	int y = 0;
	for(i = 0;i < 16;i++)
	{
		y = (i >> 2) + tetrisinformation.correnty;				//��iֵ�仯��y��ȡֵΪ0,1,2,3  +correnty
		if(y > ROW_END)
			break;
		x = (i & 3) + tetrisinformation.correntx;				//��iֵ�仯��x��ȡֵΪ0,1,2,3  +correntx
		if((tetris >> i) & 1)									//��ǰλ���з���
		{
			switch(tetrisinformation.type[0])					//���ӷ������Ͳ�ͬ����ֵ��ͬ��ɫ
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

//��������뵽��Ϸ��������
//ÿ�ζ�4λ�������ƶ�����Ϸ���м�

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
    line(35,15,340,15);									//��
	line(35,15,35,465);									//��
	line(35,465,340,465);								//��
	line(340,20,340,460);								//��
	line(340,15,500,15);								//����
	line(340,465,500,465);								//����
	line(500,20,500,460);								//����
	line(340,200,500,200);								//����
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

//����¸���������·������״����ɫ

int NextTetris()
{
	int i = 0;
	int x = 0;
	int y = 0;
	//��һ������
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
	//���¸�����
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
	while(!tetrisinformation.live)								//����Ƿ���	
	{
		PrintPrompt();
		delay_fps(3);											//��Ϸˢ��Ϊ1/3s
		NextTetris();
		if(kbhit())												//����Ƿ��а�����Ϣ
		{
			KeyControl();
		}
		if(!tetriscontrol.pause)								//�ж���Ϸ�Ƿ���ͣ
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

//�����Ϸ����Ϣ
//�������Ϣ���洢����Ϸ���У�ˢ����Ϸ�ؼ����������

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

//��ײ���
//�����������λ�õ���Ϣ�洢Ϊʮ����������Ȼ���뵱ǰ�����ʮ�����������������㣬�����Ϊ0������ײ

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

//���м��

int CheckElim()
{
	int i = 0;
	int y = tetrisinformation.correnty+3;
	do 																//�������ϼ���Ƿ�Ϊ0xFFFF
	{
		if(y < ROW_END && tetrisinformation.pool[y] == 0xFFFF)
		{
			memmove(tetrisinformation.pool + 1,tetrisinformation.pool,sizeof(unsigned) * y);		//����Ϸ�ظ����ƶ�������
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

//ֱ�����䷽��

int DownDownMoveTetris()
{
	int y = tetrisinformation.correnty;
	RemoveTetris();
	for(;tetrisinformation.correnty < ROW_END;tetrisinformation.correnty++)
	{
		if(CheckCollision())
			break;
	}
	--tetrisinformation.correnty;					//��⵽��ײ����һ��λ��û�з�����ײ
	JoinTetris();
	SetPoolColor();
	CheckElim();
	PrintGamePool();
	return 0;
}

//�Ƴ���ǰ����
//����ǰ����ʮ��������ȡ�����뵱ǰ����λ������

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
	getimage(img,"����.jpg");
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
	getimage(img,"����.jpg");
	putimage (0,0,img);
	getch();
	return 0;
}





