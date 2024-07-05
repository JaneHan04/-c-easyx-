#include <stdio.h>
#include <graphics.h> //ͼ�ο⣬���԰������ǿ�������ͼ�α�� easyx
#include <conio.h>
#include <time.h> //��ʱ��Ҫ�õ�
#include <mmsystem.h> //��������ͷ�ļ���
#pragma comment(lib,"winmm.lib")//�������������ӿ�
#include <Windows.h>

//ö��
enum My
{
	WIDTH = 591, //���ڿ�ȸ߶�
	HEIGHT = 862,
	BULLET_NUM = 15, //����ӵ�����
	ENEMY_NUM = 10,//�л�����
	BIG,
	SMALL
};
struct Plane
{
	int x;
	int y;
	bool live;//�Ƿ���
	int width;
	int height;
	int hp;
	int type;//�л�������  big small
	bool exploding = false;
}player, bull[BULLET_NUM],enemy[ENEMY_NUM];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//�������õ���ͼƬ
IMAGE home;//���˵�
IMAGE start;//��ʼ��Ϸ����
IMAGE about; //������Ϸ����
IMAGE highscore;//��ߵ÷ֽ���
IMAGE Over;//��Ϸ��������
IMAGE bk;//����ͼƬ
IMAGE img_role[2];//���ͼƬ
IMAGE img_bull[2];//�ӵ�ͼƬ
IMAGE img_enemy[2][2]; //���ֵл�
IMAGE img_boom[6];//��ըЧ��ͼ

//ȫ�ֱ���
ExMessage msg;
int score = 0; //������Ϸ�÷�
int totalTime = 60;//������Ϸʱ��
int startTime = 0;//��Ϸ��ʼʱ��
bool GameStart = false;//��Ϸ�Ƿ�ʼ
bool GameOver = false;//��Ϸ�Ƿ����

////////////////////////////////////////////////////////////////////////////////////////////////

////�û�����ģ��
void Home(ExMessage* msg);//���˵�����
void startGame();//��ʼ��Ϸ����
void gameOver(ExMessage* msg);//��Ϸ��������
void highScore(ExMessage* msg);//��ߵ÷ֽ���
void aboutGame(ExMessage* msg);//������Ϸ����
bool isInRect(ExMessage* msg, int x, int y, int w, int h);//����Ƿ���ĳ����������

////��Ϸģ��
void loadImg();//����ͼƬ
void gameInit();//��Ϸ��ʼ��
void gameDraw();//��Ϸ�Ļ��ƺ���
bool CheckCollision(Plane& plane, Plane& enemy);//��ײ���
void playerMove(int speed);//�ɻ��ƶ�����ȡ������Ϣ����������
void createBullet();//�����ӵ�
void BulletMove(int speed);//�ӵ��ƶ�
void createEnemy();//�����л�
void enemyMove(int speed);//�л����ƶ�
void enemyHp(int i);//�л�Ѫ��
void playPlance();//��ɻ�
void Score(int score);//�÷�
bool Timer(int ms, int id);//��ʱ��
void displayRemainingTime(int gameDuration, int startTime);//��Ϸʣ��ʱ��

////����
void menuMusic();//���˵�����
void closemenuMusic();//�ر����˵�����
void playMusic();//��ʼ��Ϸ����
void closeplayMusic();//�ر���Ϸ����
void overYesMusic();//���㴴����߷�����
void overNoMusic();//����û������߷�����

////�ļ�����ģ��
void highestscore(int score);//д����ߵ÷�
int lookhighest();//��ȡ��߷�

////////////////////////////////////////////////////////////////////////////////////////////////

////�û�����ģ��///////////////////////////////////////////////////////////////////////////////////////////

//���˵�����
void Home(ExMessage* msg)
{
	loadimage(&home, "images/option/home.jpg");
	putimage(0, 0, &home);
	menuMusic();
	while (true)
	{
		while (peekmessage(msg, EM_MOUSE))
		{
			//��ʼ��Ϸ
			if (isInRect(msg, 180, 510, 240, 60))
			{
				if (msg->message == WM_LBUTTONDOWN)
				{
					closemenuMusic();
					startGame();
				}
			}
			//��ߵ÷�
			else if (isInRect(msg, 180, 610, 240, 60))
			{
				if (msg->message == WM_LBUTTONDOWN)
				{
					highScore(msg);
				}
			}
			//������Ϸ
			else if (isInRect(msg, 180, 710, 240, 60))
			{
				if (msg->message == WM_LBUTTONDOWN)
				{
					aboutGame(msg);
				}
			}
		}
	}
}
//��ʼ��Ϸ����
void startGame()
{
	startTime = time(0); //��Ϸ��ʼʱ��
	GameStart = true;
	gameInit();
	//˫�����ͼ
	BeginBatchDraw();
	while (!GameOver)
	{
		playMusic();//��ʼ��Ϸ����
		Sleep(10);
		gameDraw();
		if ((time(0) - startTime) > totalTime) {
			GameStart = false;
			GameOver = true;
			break;
		}
		displayRemainingTime(totalTime, startTime);
		FlushBatchDraw();
		playerMove(5);
		BulletMove(20);

		if (Timer(500, 0))
		{
			createEnemy();
		}
		if (Timer(10, 2))
		{
			enemyMove(2);
		}

		playPlance();

		//printf("�ƶ��ɹ�\n");
	}
	if (GameOver)
	{
		gameOver(&msg);
		//������
		closeplayMusic();
	}
	EndBatchDraw();

}
//��ߵ÷ֽ���
void highScore(ExMessage* msg)
{
	loadimage(&highscore, "images/option/highscore.jpg");
	putimage(0, 0, &highscore);

	int grade = lookhighest();

	char a[20]; // ���ڴ洢��ߵ÷ֵ��ַ���

	setbkmode(TRANSPARENT); // �����ı�����͸��
	settextcolor(BLACK); // �����ı���ɫΪ��ɫ

	settextstyle(60, 0, _T("����"));
	sprintf_s(a, "��ߵ÷֣�%d", grade); // ���÷�ת��Ϊ�ַ���
	outtextxy(130, 350, a); // ����Ļ���Ͻ�λ�û��Ƶ÷�

	while (true)
	{
		while (peekmessage(msg, EM_MOUSE))
		{
			if (isInRect(msg, 30, 30, 55, 55))
			{
				if (msg->message == WM_LBUTTONDOWN)
				{
					loadimage(&home, "images/option/home.jpg");
					putimage(0, 0, &home);
					Home(msg);
				}
			}
		}
	}
}
//������Ϸ����
void aboutGame(ExMessage* msg)
{
	loadimage(&about, "images/option/about.jpg");
	putimage(0, 0, &about);
	while (true)
	{
		while (peekmessage(msg, EM_MOUSE))
		{
			if (isInRect(msg, 30, 30, 55, 55))
			{
				if (msg->message == WM_LBUTTONDOWN)
				{
					loadimage(&home, "images/option/home.jpg");
					putimage(0, 0, &home);
					Home(msg);
				}
			}
		}
	}
}
//��Ϸ��������
void gameOver(ExMessage* msg)
{
	setbkmode(TRANSPARENT); // �����ı�����͸��
	settextcolor(BLACK); // �����ı���ɫΪ��ɫ
	loadimage(&Over, "images/option/gameover.jpg");
	putimage(0, 0, &Over);

	int hs = lookhighest();
	if (score > hs)
	{
		overYesMusic();
		highestscore(score);//��Ϸ����������ߵ÷�
		settextstyle(50, 0, _T("����"));
		const char* message = "��ϲ��!!!!";
		const char* message1 = "��������߼�¼��������";
		outtextxy(100, 500, message);
		outtextxy(100, 600, message1);
	}
	else
	{
		overNoMusic();
		settextstyle(50, 0, _T("����"));
		const char* message = "�˾Ͷ���";
		const char* message1 = "�䲻�����";
		outtextxy(100, 500, message);
		outtextxy(100, 600, message1);
	}
	char scoreText[20]; // ���ڴ洢�÷ֵ��ַ���

	settextstyle(60, 0, _T("����"));
	sprintf_s(scoreText, "%d", score); // ���÷�ת��Ϊ�ַ���
	outtextxy(280, 385, scoreText); // ����Ļ���Ͻ�λ�û��Ƶ÷�

}
//����Ƿ���ĳ����������isInRect�����Ͻ����꣬���Ͻǵ����½ǽ�ͼ�ֱ��ʣ�
bool isInRect(ExMessage* msg, int x, int y, int w, int h)
{
	if (msg->x > x && msg->x<x + w && msg->y>y && msg->y < y + h)
		return true;
	return false;
}

////��Ϸģ��///////////////////////////////////////////////////////////////////////////////////////////

//����ͼƬ
void loadImg()
{
	//���ر���ͼƬ 591*862
	//���·���� ./��ʾ��ǰ�ļ����£� ../��ʾ�ļ��е���һ��Ŀ¼  ./background.jpg
	//ͼƬ������ں�cpp�ļ�ͬһ���ļ�����
	loadimage(&bk, "images/background.jpg");

	//�������ͼƬ(��������ͼ��
	loadimage(&img_role[0], "images/me1_1.jpg");
	loadimage(&img_role[1], "images/me1_2.jpg");

	//�����ӵ�ͼƬ(��������ͼ��
	loadimage(&img_bull[0], "images/bullet1_1.jpg");
	loadimage(&img_bull[1], "images/bullet1_2.jpg");
	//���صл�ͼƬ(��������ͼ��
	loadimage(&img_enemy[0][0], "images/enemy1_1.jpg");
	loadimage(&img_enemy[0][1], "images/enemy1.jpg");

	loadimage(&img_enemy[1][0], "images/enemy2_1.jpg");
	loadimage(&img_enemy[1][1], "images/enemy2.jpg");
	//���صл���ըͼƬ
	loadimage(&img_boom[0], "images/enemy_down1.jpg");
	loadimage(&img_boom[1], "images/enemy_down1.1.jpg");
	loadimage(&img_boom[2], "images/enemy2_down2.jpg");
	loadimage(&img_boom[3], "images/enemy2_down2.1.jpg");
	//�ҷ��ɻ���ըͼƬ
	loadimage(&img_boom[4], "images/me_destroy_3.1.jpg");
	loadimage(&img_boom[5], "images/me_destroy_3.jpg");

}
//��Ϸ��ʼ��
void gameInit()
{
	loadImg();
	player.x = WIDTH / 2;
	player.y = HEIGHT - 126;
	player.live = true;

	//��ʼ���ӵ�
	for (int i = 0; i < BULLET_NUM; i++)
	{
		bull[i].x = 0;
		bull[i].y = 0;
		bull[i].live = false;
	}
	//��ʼ���л�
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemy[i].live = false;
		enemyHp(i);
	}

}
//��Ϸ�Ļ��ƺ���
void gameDraw()
{
	//�ѱ���ͼƬ����������
	putimage(0, 0, &bk);
	Score(score);

	//��������ͼ�������ز�
	putimage(player.x, player.y, &img_role[0], NOTSRCERASE);
	putimage(player.x, player.y, &img_role[1], SRCINVERT);

	//�����ӵ�
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bull[i].live)
		{
			putimage(bull[i].x, bull[i].y, &img_bull[0], NOTSRCERASE);
			putimage(bull[i].x, bull[i].y, &img_bull[1], SRCINVERT);
		}
	}

	//���Ƶл�
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].live)
		{
			if (enemy[i].type == BIG)
			{
				putimage(enemy[i].x, enemy[i].y, &img_enemy[1][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_enemy[1][1], SRCINVERT);
			}
			else
			{
				putimage(enemy[i].x, enemy[i].y, &img_enemy[0][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_enemy[0][1], SRCINVERT);
			}
		}
		if (enemy[i].hp <= 0)
		{
			if (enemy[i].type == BIG)
			{
				putimage(enemy[i].x, enemy[i].y, &img_boom[2], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_boom[3], SRCINVERT);

			}
			else
			{
				putimage(enemy[i].x, enemy[i].y, &img_boom[0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &img_boom[1], SRCINVERT);
			}
		}
	}
}
//��ײ���
bool CheckCollision(Plane& plane, Plane& enemy) {
	// ������ײ����ʾ��
	if (plane.x < enemy.x + enemy.width &&
		plane.x + plane.width > enemy.x &&
		plane.y < enemy.y + enemy.height &&
		plane.y + plane.height > enemy.y) {
		return true;
	}
	return false;
}
//�ɻ��ƶ�����ȡ������Ϣ����������
void playerMove(int speed)
{
#if 0 //WSAD ��˳��
	//����Ƿ��м��̰��£�����м����£��򷵻��棬���򷵻ؼ�
	if (_kbhit())
	{
		//���ַ�ʽ
		//1��_getch()  ������������scanfһ�������û������ͻῨ�ų���һֱ�ȴ����룬����C���Ա�׼����,��Ҫ����ͷ�ļ�conio.h
		char key = _getch();
		switch (key)
		{
		case 'w':
		case 'W':
			player.y -= speed;
			break;
		case 's':
		case 'S':
			player.y += speed;
			break;
		case 'a':
		case 'A':
			player.x -= speed;
			break;
		case 'd':
		case 'D':
			player.x += speed;
			break;
		}
	}
#elif 1  //WSAD��С�������Ҷ����ԣ��˶���˳��
	//2��ʹ��windows������ȡ�������� GetAsyncKeyState  ��������������ǳ�������
	//���������룬��ĸ�����ô�д����Сд���ܼ�⵽��,Сдʲô����ⲻ��
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
	{
		if (player.y > 0)
		{
			player.y -= speed;
		}
	}
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
	{
		if (player.y + 126 < HEIGHT)
		{
			player.y += speed;
		}
	}
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
	{
		if (player.x + 40 > 0)
		{
			player.x -= speed;
		}
	}
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
	{
		if (player.x + 102 - 40 < WIDTH)
		{
			player.x += speed;
		}
	}

#endif // 0	
	if (GetAsyncKeyState(VK_SPACE) && Timer(200, 1))
	{
		//����һ���ӵ�
		createBullet();
	}
}
//�����ӵ�
void createBullet()
{
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (!bull[i].live)
		{
			bull[i].x = player.x + 51;
			bull[i].y = player.y;
			bull[i].live = true;
			break;
		}
	}
}
//�ӵ��ƶ�
void BulletMove(int speed)
{
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bull[i].live)
		{
			bull[i].y -= speed;
			if (bull[i].y < 0)
			{
				bull[i].live = false;
			}
		}
	}
}
//�����л�
void createEnemy()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (!enemy[i].live)
		{
			enemy[i].live = true;
			enemy[i].x = rand() % (WIDTH - enemy[i].width);
			enemy[i].y = 0;
			enemyHp(i);
			break;
		}
	}

}
//�л����ƶ�
void enemyMove(int speed)
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].live)
		{
			enemy[i].y += speed;
			if (enemy[i].y > HEIGHT)
			{
				enemy[i].live = false;
			}
		}
	}
}
//�л�Ѫ��
void enemyHp(int i)
{
	int flag = rand() % 10;
	if (flag >= 0 && flag <= 2) //0-9
	{ //��л�
		enemy[i].type = BIG;
		enemy[i].hp = 3;
		enemy[i].width = 69;
		enemy[i].height = 99;
	}
	else
	{ //С�л�
		enemy[i].type = SMALL;
		enemy[i].hp = 1;
		enemy[i].width = 57;
		enemy[i].height = 43;
	}
}
//��ɻ�
void playPlance()
{
	for (int i = 0; i < ENEMY_NUM; ++i) {
		if (CheckCollision(player, enemy[i])) {
			// ��ײ����������ɻ���ըЧ������Ϸ����
			player.exploding = true;
			GameOver = true; // ������Ϸ������־
			break; // ������ǰ�˳�ѭ������Ϊ��Ϸ�Ѿ�����
		}
	}
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (!enemy[i].live) continue;
		for (int k = 0; k < BULLET_NUM; k++)
		{
			if (!bull[k].live) continue;
			if (bull[k].x > enemy[i].x && bull[k].x< enemy[i].x + enemy[i].width
				&& bull[k].y>enemy[i].y && bull[k].y < enemy[i].y + enemy[i].height)
			{
				bull[k].live = false;
				enemy[i].hp--;
			}
		}
		if (enemy[i].hp <= 0)
		{
			if (enemy[i].type == BIG) score += 2;
			else score += 1;
			Score(score);
			enemy[i].live = false;
		}
	}
}
//�÷�
void Score(int score)
{
	char scoreText[20]; // ���ڴ洢�÷ֵ��ַ���

	setbkmode(TRANSPARENT); // �����ı�����͸��
	settextcolor(BLACK); // �����ı���ɫΪ��ɫ

	settextstyle(30, 0, _T("����"));
	sprintf_s(scoreText, "���ֵ÷�: %d", score); // ���÷�ת��Ϊ�ַ���
	outtextxy(30, 130, scoreText); // ����Ļ���Ͻ�λ�û��Ƶ÷�
}
//��ʱ��
bool Timer(int ms, int id)
{
	if (GameStart)
	{
		static DWORD t[10];
		if (clock() - t[id] > ms)
		{
			t[id] = clock();
			return true;
		}
		return false;
	}
}
//��Ϸʣ��ʱ��
void displayRemainingTime(int gameDuration, int startTime) {
	setbkcolor(WHITE); // ���ñ���ɫΪ��ɫ
	settextcolor(BLACK); // �����ı���ɫΪ��ɫ
	// ����ʣ��ʱ��
	int currentTime = time(0);
	int elapsedTime = currentTime - startTime;
	int remainingTime = gameDuration - elapsedTime;

	// ����ʣ��ʱ��
	char timeString[30]; // ȷ���������㹻��
	settextstyle(30, 0, _T("����"));
	sprintf_s(timeString, "ʣ��ʱ��: %d s", remainingTime);
	outtextxy(30, 100, timeString); // �� (100, 100) ������ʣ��ʱ���ı�

	// ���ʣ��ʱ��С�ڵ��� 0����ʾ��Ϸ������Ϣ
	if (remainingTime <= 0) {
	}
}

////����///////////////////////////////////////////////////////////////////////////////////////////

//���˵�����
void menuMusic()//���˵�����
{
	mciSendString("open asset/major.mp3", NULL, 0, NULL);
	mciSendString("play asset/major.mp3", NULL, 0, NULL);
}
//�ر����˵�����
void closemenuMusic()
{
	mciSendString("stop asset/major.mp3", NULL, 0, NULL);
	mciSendString("close asset/major.mp3", NULL, 0, NULL);
}
//��ʼ��Ϸ����
void playMusic()//��ʼ��Ϸ����
{
	mciSendString("open asset/back.mp3", NULL, 0, NULL);
	mciSendString("play asset/back.mp3 repeat", NULL, 0, NULL); //ѭ������
}
void overNoMusic()
{
	mciSendString("open asset/over2.mp3", NULL, 0, NULL);
	mciSendString("play asset/over2.mp3", NULL, 0, NULL);
}
//�ر���Ϸ����
void closeplayMusic()
{
	mciSendString("stop asset/back.mp3", NULL, 0, NULL);
	mciSendString("close asset/back.mp3", NULL, 0, NULL);
}
//���㴴����߷�����
void overYesMusic()
{
	mciSendString("open asset/over.mp3", NULL, 0, NULL);
	mciSendString("play asset/over.mp3", NULL, 0, NULL);
}
//����û������߷�����


////�ļ�����ģ��///////////////////////////////////////////////////////////////////////////////////////////

//д����߷�
void highestscore(int s)
{
	FILE* file;
	errno_t err;

	// ʹ�� fopen_s ��ȫ�ش��ļ�
	err = fopen_s(&file, "highest_score.txt", "w");
	if (err != 0) {
		printf("�޷����ļ� highest_score.txt\n");
		return;
	}

	// ����ߵ÷�д���ļ�
	fprintf(file, "%d", s);
	fclose(file);
}
//��ȡ��ߵ÷�
int lookhighest()
{
	FILE* file;
	int hScore = 0;
	errno_t err;

	// ʹ�� fopen_s ��ȫ�ش��ļ�
	err = fopen_s(&file, "highest_score.txt", "r");
	if (err != 0) {
		printf("�޷����ļ� highest_score.txt\n");
		return hScore;
	}

	// ���ļ��ж�ȡ��ߵ÷�
	fscanf_s(file, "%d", &hScore);
	fclose(file);

	printf("��ߵ÷���: %d\n", hScore);

	return hScore;
}

////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
	
	initgraph(591, 862); // ��ʼ��ͼ�ν���
	//initgraph(591, 862, SHOWCONSOLE);  
	Home(&msg);
	return 0;
}