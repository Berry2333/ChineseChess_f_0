#include<iostream>
#include<easyx.h>
#include <iomanip>
using namespace std;

#define ROW 10
#define COL 9
#define GRID_SIZE 88
#define INTERVAL 54
#define MYMIN(a,b) ((a)<(b)? (a):(b))

enum Pieces//棋子
{
	SPACE = -1,
	車,馬,象,士,将,炮,卒,
	车,马,相,仕,帅,砲,兵,
	BIGIN,END,
};

enum Pieces redChess[] = { 車,馬,象,士,将,炮,卒 };
enum Pieces blackChess[] = { 车,马,相,仕,帅,砲,兵 };
const char* ChessName[] = { "車","馬","象","士","将","炮","卒",
	                        "车","马","相","仕","帅","砲","兵"
};


//棋子属性
struct Chess
{
	enum Pieces id;
	DWORD type;
	short x;
	short y;
	bool isRiver;
};
//游戏地图
struct Chess Mymap[ROW][COL];
void moveChess();


//打印数组
void show()
{
	for (size_t i = 0; i < ROW; i++) {
		for (size_t j = 0; j < COL; j++) {
			cout <<setw(2)<< Mymap[i][j].id << " ";
		}
		cout << endl;
	}

}


//初始化数据
void init() {
	size_t temp = 0;
	for (size_t i = 0; i < ROW; i++) {
		for (size_t j = 0; j < COL; j++) {
			Mymap[i][j].id = SPACE;
			//黑棋
			if (i <= 4) {
				Mymap[i][j].type = BLACK;
				if (i == 0) {//第一行棋子
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
			//红棋
			else {//红旗
				Mymap[i][j].type = RED;
				if (i == 9) {//第一行棋子
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

//绘制
void draw() {
	setfillcolor(RGB(252, 215, 162));
	setlinestyle(PS_SOLID, 2);
	settextstyle(30, 0, "楷体");
	outtextxy(360, 0, "初始化");//
	for (size_t i = 0; i < ROW; i++) {
		for (size_t j = 0; j < COL; j++) {
			if (Mymap[i][j].id == SPACE) {
				continue;
			}

			settextcolor(Mymap[i][j].type);
			setlinecolor(Mymap[i][j].type);
			//圆圈
			fillcircle(Mymap[i][j].x, Mymap[i][j].y, 30);
			fillcircle(Mymap[i][j].x, Mymap[i][j].y, 25);
			//文字
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
//鼠标操作
void mouseEvent() {
	ExMessage msg;//消息结构体变量
	if (peekmessage(&msg, EM_MOUSE)) {
		if (msg.message == WM_LBUTTONDOWN) {
			//鼠标坐标得到点击数组的坐标
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
							//下标校准
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

//判断起始位置之间有几个棋子
int hasBlock(struct State* state) {
	int cat = 0;
	//判断是水平还是竖直的
	//从小到达便利
	//判断之间是否有棋子
	cout << state->begc << state->begr << state->endc << state->endr << endl;
	if (state->begr == state->endr) {//水平
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
	else if (state->begc == state->endc) {//竖直
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
	cout <<"有" << cat <<"个棋子中间" << endl;
	return cat;
}

//移动棋子
void moveChess() {

	
	bool canMove = false;
	if (!(state.begr == state.endr && state.begc == state.endc) &&
		state.endr != -1 && state.begr != -1 &&
		Mymap[state.begr][state.begc].id != SPACE 
		//Mymap[state.begr][state.begc].type != Mymap[state.endr][state.endc].type
		) {
		switch (Mymap[state.begr][state.begc].id)
		{

		case 車:
		case 车:

			if (state.begr == state.endr || state.begc == state.endc) {
				if (hasBlock(&state) == 0)
					canMove = true;
			}
			break;
		case 馬:
		case 马:
			if ((abs(state.begr - state.endr) == 2 && abs(state.begc - state.endc) == 1) || 
				(abs(state.begr - state.endr) == 1 && abs(state.begc - state.endc) == 2)) {
				//if (hasBlock(&state) == 0)
					canMove = true;

			}
			
			break;

		case 象:
		case 相:
			if ((abs(state.begr - state.endr) == 2 && abs(state.begc - state.endc) == 2) && 
				Mymap[state.begr][state.begc].type== Mymap[state.endr][state.endc].type) {
				//if (hasBlock(&state) == 0)
				canMove = true;

			}
			break;

		case 士:
		case 仕:
			break;

		case 将:
		case 帅:
			break;

		case 炮:
		case 砲:
			break;

		case 卒:
		case 兵:
			break;

		default:
			break;
		}
		if (canMove) {
			if (Mymap[state.endr][state.endc].id != SPACE && Mymap[state.endr][state.endc].type == Mymap[state.begr][state.begc].type) {
				cout << "不能吃自己" << endl;
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
	//创建窗口
	initgraph(812, 899, 1);
	IMAGE img_board;
	loadimage(&img_board, "./res/qipan.png");
	putimage(0, 0, &img_board);
	
	init();
	show();
	//双缓冲绘图 防止闪屏
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