#include<iostream>
#include<easyx.h>
#include <iomanip>
using namespace std;

#define ROW 10
#define COL 9
#define GRID_SIZE 88
#define INTERVAL 54
#define MYMIN(a,b) ((a)<(b)? (a):(b))

enum Pieces//����
{
	SPACE = -1,
	܇,�R,��,ʿ,��,��,��,
	��,��,��,��,˧,�h,��,
	BIGIN,END,
};

enum Pieces redChess[] = { ܇,�R,��,ʿ,��,��,�� };
enum Pieces blackChess[] = { ��,��,��,��,˧,�h,�� };
const char* ChessName[] = { "܇","�R","��","ʿ","��","��","��",
	                        "��","��","��","��","˧","�h","��"
};


//��������
struct Chess
{
	enum Pieces id;
	DWORD type;
	short x;
	short y;
	bool isRiver;
};
//��Ϸ��ͼ
struct Chess Mymap[ROW][COL];
void moveChess();


//��ӡ����
void show()
{
	for (size_t i = 0; i < ROW; i++) {
		for (size_t j = 0; j < COL; j++) {
			cout <<setw(2)<< Mymap[i][j].id << " ";
		}
		cout << endl;
	}

}


//��ʼ������
void init() {
	size_t temp = 0;
	for (size_t i = 0; i < ROW; i++) {
		for (size_t j = 0; j < COL; j++) {
			Mymap[i][j].id = SPACE;
			//����
			if (i <= 4) {
				Mymap[i][j].type = BLACK;
				if (i == 0) {//��һ������
					//0 1 2 3 4 3 2 1 0
					//0 1 2 3
					if (j <= 4) {
						temp = j;
					}
					else {
						temp = 8 - j;
					}
					Mymap[i][j].id = blackChess[temp];
				}
				if (i == 2 && (j == 2 || j == 7)) {
					Mymap[i][j].id = blackChess[5];
				}
				if (i == 3 && j % 2 == 0) {
					Mymap[i][j].id = blackChess[6];
				}
			}
			//����
			else {//����
				Mymap[i][j].type = RED;
				if (i == 9) {//��һ������
					//0 1 2 3 4 3 2 1 0
					//0 1 2 3
					if (j <= 4) {
						temp = j;
					}
					else {
						temp = 8 - j;
					}
					Mymap[i][j].id = redChess[temp];
				}
				if (i == 7 && (j == 2 || j == 7)) {
					Mymap[i][j].id = redChess[5];
				}
				if (i == 6 && j % 2 == 0) {
					Mymap[i][j].id = redChess[6];
				}

			}

			Mymap[i][j].isRiver = 0;
			Mymap[i][j].x = j * GRID_SIZE + INTERVAL;
			Mymap[i][j].y = i * GRID_SIZE + INTERVAL;
		}
		
	}

}

//����
void draw() {
	setfillcolor(RGB(252, 215, 162));
	setlinestyle(PS_SOLID, 2);
	settextstyle(30, 0, "����");
	outtextxy(360, 0, "��ʼ��");//
	for (size_t i = 0; i < ROW; i++) {
		for (size_t j = 0; j < COL; j++) {
			if (Mymap[i][j].id == SPACE) {
				continue;
			}

			settextcolor(Mymap[i][j].type);
			setlinecolor(Mymap[i][j].type);
			//ԲȦ
			fillcircle(Mymap[i][j].x, Mymap[i][j].y, 30);
			fillcircle(Mymap[i][j].x, Mymap[i][j].y, 25);
			//����
			setbkmode(TRANSPARENT);
			outtextxy(Mymap[i][j].x - 14, Mymap[i][j].y-14, ChessName[Mymap[i][j].id]);
		}
	}
}


//zhuangtai
struct State {
	int begr;
	int endr;
	int begc;
	int endc;
	int state;

}state{ -1,-1,-1,-1,BIGIN };
//������
void mouseEvent() {
	ExMessage msg;//��Ϣ�ṹ�����
	if (peekmessage(&msg, EM_MOUSE)) {
		if (msg.message == WM_LBUTTONDOWN) {
			//�������õ�������������
			//k*GRID_SIZE + INTERVAL
			if (msg.x > 360 && msg.x < 440 && msg.y < 10) {
				init();
				state.state = BIGIN;
				cout << "restart" << endl;
			}


			if (msg.x>24&& msg.y >24&&((msg.x - INTERVAL) % GRID_SIZE <= 30 || (msg.x - INTERVAL) % GRID_SIZE >= 58) && ((msg.y - INTERVAL) / GRID_SIZE <= 10 || (msg.y - INTERVAL) / GRID_SIZE >= 58)) {
				int col = (msg.x - INTERVAL) / GRID_SIZE;
				int row = (msg.y - INTERVAL) / GRID_SIZE;
				//cout << '(' << row << ',' << col << ')' << endl;
							//�±�У׼
				if (msg.x > Mymap[row][col].x + 30 && msg.y < Mymap[row][col].y + 30) {
					col++;
				}
				if (msg.x < Mymap[row][col].x + 30 && msg.y > Mymap[row][col].y + 30) {
					row++;
				}
				if (msg.x > Mymap[row][col].x + 30 && msg.y > Mymap[row][col].y + 30) {
					col++;
					row++;
				}
				cout << '(' << row << ',' << col << ')' << endl;

				if (state.state == BIGIN) {
					state.begr = row;
					state.begc = col;
					state.state = END;
				}else if (state.state == END) {
					state.endr = row;
					state.endc = col;
					state.state = BIGIN;
					//cout << '(' << row << ',' << col << ')' << endl;
					
					moveChess();
					
					
				}
				
			}
			


		}
	}

}

//�ж���ʼλ��֮���м�������
int hasBlock(struct State* state) {
	int cat = 0;
	//�ж���ˮƽ������ֱ��
	//��С�������
	//�ж�֮���Ƿ�������
	cout << state->begc << state->begr << state->endc << state->endr << endl;
	if (state->begr == state->endr) {//ˮƽ
		/*if (state->begc > state->endc) {
			for (int i = 1; i < state->begc - state->endc; i++) {
				if (Mymap[state->begr][state->endc+i].id != SPACE) {
					cat++;
				}
			}
		}
		else if(state->begc < state->endc) {
			for (int i = 1; i < state->endc - state->begc; i++) {
				if (Mymap[state->begr][state->begc + i].id != SPACE) {
					cat++;
				}
			}
		}*/
		for (int i = 1; i < abs(state->begc - state->endc); i++) {
			if (Mymap[state->begr][MYMIN(state->endc, state->begc) + i].id != SPACE) {
				cat++;
			}
		}

	}
	else if (state->begc == state->endc) {//��ֱ
		if (state->begr > state->endr) {
			for (int i = 1; i < state->begr - state->endr; i++) {
				if (Mymap[state->endr+i][state->endc].id != SPACE) {
					cat++;
				}
			}
		}
		else if (state->begr < state->endr) {
			for (int i = 1; i < state->endr - state->begr; i++) {
				if (Mymap[state->begr+i][state->begc].id != SPACE) {
					cat++;
				}
			}
		}

	}
	cout <<"��" << cat <<"�������м�" << endl;
	return cat;
}

//�ƶ�����
void moveChess() {

	
	bool canMove = false;
	if (!(state.begr == state.endr && state.begc == state.endc) &&
		state.endr != -1 && state.begr != -1 &&
		Mymap[state.begr][state.begc].id != SPACE 
		//Mymap[state.begr][state.begc].type != Mymap[state.endr][state.endc].type
		) {
		switch (Mymap[state.begr][state.begc].id)
		{

		case ܇:
		case ��:

			if (state.begr == state.endr || state.begc == state.endc) {
				if (hasBlock(&state) == 0)
					canMove = true;
			}
			break;
		case �R:
		case ��:
			if ((abs(state.begr - state.endr) == 2 && abs(state.begc - state.endc) == 1) || 
				(abs(state.begr - state.endr) == 1 && abs(state.begc - state.endc) == 2)) {
				//if (hasBlock(&state) == 0)
					canMove = true;

			}
			
			break;

		case ��:
		case ��:
			if ((abs(state.begr - state.endr) == 2 && abs(state.begc - state.endc) == 2) && 
				Mymap[state.begr][state.begc].type== Mymap[state.endr][state.endc].type) {
				//if (hasBlock(&state) == 0)
				canMove = true;

			}
			break;

		case ʿ:
		case ��:
			break;

		case ��:
		case ˧:
			break;

		case ��:
		case �h:
			break;

		case ��:
		case ��:
			break;

		default:
			break;
		}
		if (canMove) {
			if (Mymap[state.endr][state.endc].id != SPACE && Mymap[state.endr][state.endc].type == Mymap[state.begr][state.begc].type) {
				cout << "���ܳ��Լ�" << endl;
				return;
			}
			cout << "canMove\n";
			cout << '(' << state.begr << ',' << state.begc << ')' << '(' << state.endr << ',' << state.endc << ')' << endl;
			Mymap[state.endr][state.endc].id = Mymap[state.begr][state.begc].id;
			Mymap[state.begr][state.begc].id = SPACE;

			Mymap[state.endr][state.endc].isRiver = Mymap[state.begr][state.begc].isRiver;
			Mymap[state.endr][state.endc].type = Mymap[state.begr][state.begc].type;


		}
	}
}

int main() {
	//��������
	initgraph(812, 899, 1);
	IMAGE img_board;
	loadimage(&img_board, "./res/qipan.png");
	putimage(0, 0, &img_board);
	
	init();
	show();
	//˫�����ͼ ��ֹ����
	BeginBatchDraw();

	while (true) {
		cleardevice();
		putimage(0, 0, &img_board);
		draw();
		mouseEvent();

		FlushBatchDraw();
	}
	EndBatchDraw();


	getchar();
	//system("pause");

	return 0;
}