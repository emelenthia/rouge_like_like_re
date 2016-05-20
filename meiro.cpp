#include<stdio.h>
#include<conio.h> //getch関数
#include<stdlib.h> //rand関数とか
#include<windows.h> //カーソルの位置を移動する
#include<time.h> //time定数
#include<math.h> //ceil関数
//#include<unistd.h>
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "winmm.lib")//ミリ秒の取得
#pragma warning(disable:4996)
#define MAP_X 60 //迷路の横の限界
#define MAP_Y 30 //迷路の縦の限界
#define ENEMY_MAX 15 //同時に存在できる敵の最大数
#define ENEMY_KINDS 26+1 //敵の種類の最大数(+1,0番目は隠しキャラ?)
#define GOLD_MAX 5 //存在できる金塊の総数
#define SCROLL_MAX 5 //存在できる巻物の総数
#define POTION_MAX 5 //存在できる飲薬の総数
#define TRAP_MAX 5 //存在できる罠の総数
#define MAP_CUT_X 3 //マップの縦に関する分割部屋数(線はn-1本)
#define MAP_CUT_Y 3 //マップの横に関する分割部屋数(線はn-1本)
#define BORN_SPEED 25 //モンスターが新たに生まれる早さ。大きいほど遅くなる
#define LOG_LINES 30 //表示するログの行数
#define CURE_SPEED 10 //HPが回復する早さ。大きいほど遅くなる

int debug_kazoe = 0;

//グローバル変数
int inkey = -1;
int playerx = -1;
int playery = -1;
int gexist[5] = {0}; //金塊の存在フラグ
int numgold = 0; //プレイヤーの所持金貨
int numwalk = 0; //プレイヤーの累計歩数
int hungry = 1000; //満腹度。/10して使用
int sexist[5] = {0}; //巻物の存在フラグ
int numscroll[5] = {0}; //各巻物の所持個数。0…scroll of teleportation,
int auto_manage = 0; //0でないとき自動処理中につきキー入力しない。これは害悪なので使わないように
int gameover = 0; //ゲームオーバーフラグ
int playerhpmax = -1; //プレイヤーの最大hp
int playerhp = -1; //プレイヤーの現在hp
int pexist[5] = {0}; //飲薬の存在フラグ
//0:hering,1:invisible,2:focussing(敵の位置が分かる)
int numpotion[5] = {0}; //各飲薬の所持個数。0…potion of curing,
int knock = -1; //敵nとのぶつかりフラグ
int playeratk = 0; //プレイヤーのATK。プレイヤー情報を構造体にまとめる意味はあるか
int playerdef = 0; //プレイヤーのDEf
int use_item_flag = -1; //n番のアイテム使用フラグ。番号は0から順に使用する際に押すアルファベット順
int get_item_flag = 0; //アイテム入手フラグ。
int damage = -1; //与えているダメージ
int aoe = 0; //現在の経験値
int Lv = 1; //現在のレベル
int floor_non_func = 1; //現在の階数
int floor_down_flag = 0; //階段を降りるフラグ
int numweapon[5] = {0}; //所持している装備(現状は武器のみ?)
//1:ショートソード
char get_item_name[32];
char sname[5][32];
char pname[5][32];
int invisible_time = 0; //残り透明時間
int focussing_flag = 0; //集中フラグ
int getgold = 0; //金塊入手フラグ
char log_keep[LOG_LINES][60]; //ログや実況に表示するための文章を保存する。
int log_change_flag = 0; //ログを変更した時のフラグ
int full_house_flag = 0; //大部屋フラグ
int floor_max = 0; //何階まであるのか
int last_floor_flag = 0; //ここがラスト
char playername[21]; //プレイヤーの名前
int game_clear_flag = 0;
int bind_time = 0;
int non_first_keep = 0;

int map[MAP_Y][MAP_X];// = {-1}; //迷路の内容
int map_open_flag[MAP_Y][MAP_X] = {-1}; //迷路可視フラグ。0で見えない、1で見えるが敵は見えない、2で完全に見える。
int start_x = -1; //スタート位置X
int start_y = -1; //スタート位置Y
int goal_x = -1; //ゴール位置X
int goal_y = -1; //ゴール位置Y
int gnumber = 0; //金塊の個数
int gplacex[5] = {-1}; //金塊の位置X
int gplacey[5] = {-1}; //金塊の位置Y
int snumber = 0; //巻物の個数
int splacex[5] = {-1}; //巻物の位置X
int splacey[5] = {-1}; //巻物の位置Y
int pnumber = 0; //飲薬の個数
int pplacex[5] = {-1}; //飲薬の位置X
int pplacey[5] = {-1}; //飲薬の位置Y
int enumber = 0; //敵の数
int wnumber = 0; //装備の数
int wplacex[5] = {-1}; //装備の位置X
int wplacey[5] = {-1}; //装備の位置Y
int wexist[5] = {0}; //装備の存在フラグ
char ename[ENEMY_KINDS][16]; //敵の名前一覧/とりあえず15字まで
int tnumber = 0;; //罠の個数
int tplacex[5] = {0}; //罠の位置X
int tplacey[5] = {0};//罠の位置Y
int texist[5] = {0};//罠の存在フラグ
int tkinds[5] = {0};//各罠の種類
//1:テレポートの罠,2:針っぽいの,
int trap_open_flag[5] = {0};//各罠の見えてるフラグ
int feet_trap_flag = 0; //罠を踏んだ時
char *cache = new char[1];
int player_kind = 0;
char keep_map[1830];//[MAP_Y][MAP_X + 1];

//マップの情報
struct mapdata
{
	int map[MAP_Y][MAP_X]; //迷路の内容
	int map_open_flag[MAP_Y][MAP_X];
	int start_x; //スタート位置X
	int start_y; //スタート位置Y
	int goal_x; //ゴール位置X
	int goal_y; //ゴール位置Y
	int gnumber; //金塊の個数
	int gplacex[5]; //金塊の位置X
	int gplacey[5]; //金塊の位置Y
	int snumber; //巻物の個数
	int splacex[5]; //巻物の位置X
	int splacey[5]; //巻物の位置Y
	int pnumber; //飲薬の個数
	int pplacex[5]; //飲薬の位置X
	int pplacey[5]; //飲薬の位置Y
	int wnumber; //装備の数
	int wplacex[5]; //装備の位置X
	int wplacey[5]; //装備の位置Y
	int wexist[5]; //装備の存在フラグ
	int enumber; //敵の数
	int tnumber; //罠の個数
	int tplacex[5]; //罠の位置X
	int tplacey[5]; //罠の位置Y
	int texist[5]; //罠の存在フラグ
	int tkinds[5]; //各罠の種類
}map1, *p;

FILE *fp;

int eexist = 0; //敵の存在フラグ
int ekind = -1; //敵の種類
int eplacex = -1; //敵の初期位置X
int eplacey = -1; //敵の初期位置Y
int enemyhp = -1; //敵の現在hp
int damaged = -1; //ダメージの受身形。多分
int knocked = 0; //敵nとのぶつかられフラグ。複数体も考慮

//個別の敵の情報
struct enemy_non_change
{
	int eexist; //敵の存在フラグ
	int ekind;
	int eplacex; //敵の位置X
	int eplacey; //敵の位置Y
	int enemyhp; //敵の現在hp
	int damaged; //ダメージの受身形。多分
	int knocked; //敵nとのぶつかられフラグ。複数体も考慮して構造体の配列に入れる
}enemy_nc[ENEMY_MAX], *enc[ENEMY_MAX];

int point = 0;
char pname_clog[21];
typedef struct clear_log
{
	int point;
	char pname_clog[21];
}clog;

int enemyhpmax = -1; //エネミーの最大HP
int enemyatk = -1; //モンスターのATK
int enemydef = -1; //敵キャラのDEF
int enemy_data_count = ENEMY_KINDS; //敵キャラの総数
int enemyexp = 0; //敵キャラを倒した時にもらえるexp
int enemy_first_floor = 0; //その敵が出始める階数
int enemy_last_floor = 0; //その敵が最後に出る階数

//敵個体データ
struct enemy_data
{
	int enemyhpmax;
	int enemyatk;
	int enemydef;
	int enemyexp;
	int enemy_first_floor;
	int enemy_last_floor;
}enemy_d[ENEMY_KINDS], *ed[ENEMY_KINDS];


// 出力先のハンドルを取得
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
// COORD構造体へ座標を設定する&宣言
COORD defPosition = {0, 0};

	HANDLE hStdout; //色を変える処理
    WORD wAttributes;
    CONSOLE_SCREEN_BUFFER_INFO csbi;//構造体です

//入力されたキーを取得する
int getcursor(void);
//マップを読み込む
void read_map(void);
//マップを描く
void picmap(void);
//行動した位置に物があった場合の処理
void manage_flag(void);
//プレイヤーの情報を描画
void player_information(void);
//アイテムを使う処理
void use_item(void);
//ゲームオーバーになった時の処理
void gameover_manage(void);
//職業決定的な
void questions(void);
//変数の初期設定
void varrangeset(void);
//敵キャラ個体データ入力(番号,hpmax,ATK,DEF,名前)
void parentvarset(int i, int a, int b, int c, int d, int first_floor, int last_floor, char *name);
//実況
void oil_of_vug(void);
//戦闘したい
void battle(void);
//相手の行動
void enemy_act(void);
//敵キャラを生成
void born_enemy(void);
//マップ情報を初期化
void map_clear(void);
//常時リセット変数用
void every_varset(void);
//マップ可視範囲の処理
void map_open(void);
//罠を踏んだ処理
void feet_trap(int kind);
//ログの整理・追加
void shift_log(char *text);
//重くならない画面の描画方法
void print_cache(char *str);
//タイトル画面
void title_prev(void);
//ゲームクリア処理
void game_clear(void);
void debug_now(void);
void ranking_manage(void);
void log_print(void);

void debug_now(void)
{
	debug_kazoe++;
	defPosition.X = 85;
	defPosition.Y = 33;
	// COORD構造体で指定された座標へカーソルを移動させる
	SetConsoleCursorPosition(hOut, defPosition);
	printf("  ");
	defPosition.X = 85;
	defPosition.Y = 33;
	// COORD構造体で指定された座標へカーソルを移動させる
	SetConsoleCursorPosition(hOut, defPosition);
	printf("%d", debug_kazoe);
}


//メイン関数
int main()
{
	// コンソールのサイズを変更
	COORD dwSize = {160, 90};
	SetConsoleScreenBufferSize(hOut, dwSize);

	WCHAR titleTextSize[128]; //最大化
	GetConsoleTitle(titleTextSize, 128);
	HWND consoleWindowTitle = FindWindow(NULL, titleTextSize);
	ShowWindow(consoleWindowTitle, SW_MAXIMIZE);
	
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE); //色を変える処理
    //hStdoutのコンソールスクリーンバッファ情報をcsbiに取得
    GetConsoleScreenBufferInfo(hStdout, &csbi);

	int stop = 0;

	while(1)
	{
		//[statedef 5900]
		varrangeset();
		map_clear();
		//タイトル画面
		title_prev();
		//最初の設定
		questions();

		//最初の描画
		read_map();
		picmap();
		player_information();

		while(/*inkey != 113 &&*/ !gameover && !game_clear_flag)
		{
			every_varset();
			inkey = getcursor();
			if(knock + 1) //敵にぶつかった
			{
				battle();
			}
			use_item();
			if(!floor_down_flag) //階段を降りた
			{
				enemy_act();
				picmap();
				manage_flag();
				player_information();
				oil_of_vug();
				born_enemy();
			}
			if(game_clear_flag)
			{
				game_clear();
			}
		}

		fclose(fp);
		if(gameover)
		{
			gameover_manage();
			/*while(inkey != 113)
			{
				inkey = getch();
			}*/
		}
	}
	return 0;
}


void varrangeset(void)
{
	int i = 0;
	int i2 = 0;

	while(i < 30) //変数リセ
	{
		while(i2 < 30)
		{
			map[i][i2] = -1;
			i2++;
		}
		i2 = 0;
		i++;
	}

	i = 0;
	while(i < ENEMY_MAX) //変数リセ
	{
		enc[i] = &enemy_nc[i];
		i++;
	}

	i = 0;
	while(i < enemy_data_count)
	{
		ed[i] = &enemy_d[i];
		i++;
	}

	//グローバル変数
	inkey = -1;
	playerx = -1;
	playery = -1;
	i = 0;
	while(i < 5)
	{
		gexist[i] = 0; //金塊の存在フラグ
		sexist[i] = 0; //巻物の存在フラグ
		numscroll[i] = 0; //各巻物の所持個数。0…scroll of teleportation,
		pexist[i] = 0; //飲薬の存在フラグ
		numpotion[i] = 0; //各飲薬の所持個数。0…potion of curing,
		numweapon[i] = 0; //所持している装備(現状は武器のみ?)
		i++;
	}
	numgold = 0; //プレイヤーの所持金貨
	numwalk = 0; //プレイヤーの累計歩数
	hungry = 1000; //満腹度。/10して使用
	auto_manage = 0; //0でないとき自動処理中につきキー入力しない。これは害悪なので使わないように
	gameover = 0; //ゲームオーバーフラグ
	playerhpmax = -1; //プレイヤーの最大hp
	playerhp = -1; //プレイヤーの現在hp
	knock = -1; //敵nとのぶつかりフラグ
	playeratk = 0; //プレイヤーのATK。プレイヤー情報を構造体にまとめる意味はあるか
	playerdef = 0; //プレイヤーのDEf
	use_item_flag = -1; //n番のアイテム使用フラグ。番号は0から順に使用する際に押すアルファベット順
	get_item_flag = 0; //アイテム入手フラグ。
	damage = -1; //与えているダメージ
	aoe = 0; //現在の経験値
	Lv = 1; //現在のレベル
	floor_non_func = 1; //現在の階数
	floor_down_flag = 0; //階段を降りるフラグ
	invisible_time = 0; //残り透明時間
	focussing_flag = 0; //集中フラグ
	getgold = 0; //金塊入手フラグ
	log_change_flag = 0; //ログを変更した時のフラグ
	full_house_flag = 0; //大部屋フラグ
	last_floor_flag = 0; //ここがラスト
	game_clear_flag = 0;
	strcpy(playername, "");
	i = 0;
	while(i < LOG_LINES)
	{
		strcpy(log_keep[i], "");
		i++;
	}
	
	parentvarset(1,3,5,32,12,5,6, "ARMOR"); //1ダメージしか受けないし与えられない程度
	parentvarset(2,10,5,2,3,1,2, "BAT"); //Lvを2つ上げると1撃で倒せる程度
	parentvarset(3,26,14,5,13,5,7, "CHIMERA"); //無難な感じ
	parentvarset(4,46,26,20,29,9,10, "DRAGON"); //強い
	parentvarset(5,28,28,9,33,10,10, "EVIL MONSTER"); //火力勢 
	parentvarset(6,31,19,14,20,7,8, "FENRIR"); //無難でかつ攻撃高め
	parentvarset(7,100,33,18,1,11,11, "GUILTY REQUIEM");//難しい方のボス
	parentvarset(8,12,9,3,5,1,3, "HOBGOBLIN"); //Lv1のプレイヤーに2ダメージ与える程度
	parentvarset(9,25,20,3,23,8,9, "INVISIBLE MAN"); //見えない代わりにステは低め
	parentvarset(11,32,24,18,28,9,10, "KING DAEMON"); //強い
	parentvarset(13,4,20,3,15,6,8, "MURDER"); //火力のみ
	parentvarset(15,21,13,9,11,3,5, "ORC"); //序盤の強敵
	parentvarset(16,34,16,13,21,7,8, "PEGASUS"); //無難でかつ防御高め
	parentvarset(18,16,11,3,10,4,6, "RABIT"); //強くない
	parentvarset(19,20,5,4,4,1,2, "SLIME"); //体力が高いだけ
	parentvarset(20,26,14,2,10,4,4, "TROLL"); //体力&攻撃が高い
	parentvarset(21,38,12,16,25,8,10, "UNICORN"); //若干弱い
	parentvarset(22,40,15,12,1,11,11, "VESTA"); //簡単な方のボス
	parentvarset(23,17,11,9,8,3,5, "WOOD MONSTER"); //劣化orc
	parentvarset(26,12,10,6,1,3,4, "ZOMBIE"); //経験値が少ない
	//(アルファベットの番号,hp,atkdef,exp)
	strcpy(sname[0], "teleportation scroll");
	strcpy(sname[1], "hardluck scroll");
	strcpy(sname[2], "grasping scroll");
	strcpy(sname[3], "prasma brast scroll");
	strcpy(sname[4], "bind scroll");
	strcpy(pname[0], "curing potion");
	strcpy(pname[1], "invisible potion");
	strcpy(pname[2], "focussing potion");
	strcpy(pname[3], "strong potion");

	cache = "\0";
}


void parentvarset(int i, int a, int b, int c, int d, int first_floor, int last_floor,  char *name)
{
	ed[i] -> enemyhpmax = a;
	ed[i] -> enemyatk = b;
	ed[i] -> enemydef = c;
	ed[i] -> enemyexp = d;
	ed[i] -> enemy_first_floor = first_floor;
	ed[i] -> enemy_last_floor = last_floor;
	strcpy(ename[i], name);

}


void title_prev(void)
{
	int wait_enter = 0;
	system("cls"); //画面をすべてクリアする
	
	defPosition.X = 20; //真ん中付近に
	defPosition.Y = 10;
	// COORD構造体で指定された座標へカーソルを移動させる
	SetConsoleCursorPosition(hOut, defPosition);
	printf("please, press \"Enter\"");
	
	while(wait_enter != 13)
	{
		wait_enter = getch();
	}
}


void questions(void)
{
	int key_kind = 0;
	system("cls");

	defPosition.X = 0;
	defPosition.Y = 0;
	// COORD構造体で指定された座標へカーソルを移動させる
	SetConsoleCursorPosition(hOut, defPosition);
	printf("種族を選んでください\n");
	printf("1.HUMAN,2.ELF\n");
	while(1)
	{
		key_kind = getch();
		if(key_kind == 49 || key_kind == 50)
		{
			break;
		}
	}

	if(key_kind == 49) //人間
	{
		playerhpmax = 18;
		playerhp = 14;
		playeratk = 10;
		playerdef = 7;
		player_kind = 1;
	}
	else //エルフ
	{
		playerhpmax = 16;
		playerhp = 12;
		playeratk = 8;
		playerdef = 8;
		player_kind = 2;
	}
	//system("cls");
	//defPosition.X = 0;
	//defPosition.Y = 0;
	// COORD構造体で指定された座標へカーソルを移動させる
	//SetConsoleCursorPosition(hOut, defPosition);
	printf("%d\n難易度を選んでください\n", player_kind);
	printf("1.EASY,2.NORMAL\n");
	key_kind = 0;
	while(1)
	{
		key_kind = getch();
		if(key_kind == 49 || key_kind == 50)
		{
			break;
		}
	}
	printf("%d", key_kind - 48);
	if(key_kind == 49) //簡単
	{
		floor_max = 6;
	}
	else //普通
	{
		floor_max = 11;
	}
}


int getcursor(void) //ななめ移動にも対応するためテンキー操作前提
{
	char key = 0; //文字入力
	int i = 0; //一時計算用
	int x2 = 0; //一時計算用
	int y2 = 0; //一時計算用
	int walking = 0; //歩いたかどうか

	while(!walking )
	{
		key = getch();	//最新版。全部纏めた
		if(49 <= key && key <= 57)
		{
			x2 = playerx + (key%3 ? (key%3 == 1 ? -1 : 0) : 1);
			y2 = playery + (key >= 55 ? -1 : (key <= 51 ? 1 : 0));
			if(map[y2][x2] == 1 || map[y2][x2] == 3 || map[y2][x2] == 5)
			{
				i = 0;
				knock = -1;
				while(i < ENEMY_MAX)
				{
					if(enc[i] -> eexist && y2 == enc[i] -> eplacey && x2 == enc[i] -> eplacex)
					{
						knock = i;
					}
					i++;
				}
				if(knock == -1)
				{
					playerx = x2;
					playery = y2;
				}
				hungry--;
				numwalk++;
				walking++;
				if(hungry > 0)
				if(!(numwalk%CURE_SPEED))
				{
					if(playerhp < playerhpmax)
					{
						playerhp++;
					}
				}
				invisible_time = (invisible_time >0 ? invisible_time - 1 : 0);
				bind_time = (bind_time >0 ? bind_time - 1 : 0);
			}
		}
		else //アイテム等を使った場合
		{
			hungry--;
			walking++;
		}

	}

	inkey = key;
	return(inkey);
}


void read_map(void)
{
	int x = 0;
	int y = 0;
	p = &map1;
	int i = 0; //計算用
	int i2 = 0;
	int flag = 0; //フラグ用
	int flag2 = 0;
	int flag3 = 0;

	if(floor_non_func != floor_max)
	{
		srand((unsigned long)timeGetTime());
		switch(rand()%7) //開くマップをランダムに決定
		{
		case 0:
			fp = fopen("./maps/map1.map", "r");
			break;
		case 1:
			fp = fopen("./maps/map2.map", "r");
			break;
		case 2:
			fp = fopen("./maps/map3.map", "r");
			break;
		case 3:
			fp = fopen("./maps/map4.map", "r");
			break;
		case 4:
			fp = fopen("./maps/map5.map", "r");
			break;
		case 5:
			fp = fopen("./maps/map6.map", "r");
			break;
		case 6:
			fp = fopen("./maps/map7.map", "r");
			full_house_flag++;
			break;
		case 7:
			fp = fopen("./maps/map8.map", "r");
			full_house_flag++;
			break;
		}
	}
	else //最後の階であれば特別な部屋へ
	{
		fp = fopen("./maps/map0.map", "r");
		last_floor_flag++;
	}
	if(fp == NULL)
	{
		printf("ファイルを開けません。\n");
		exit(1);
	}

	x = 0;
	y = 0;
	while(y < MAP_Y)
	{
		while(x < MAP_X)
		{
			fscanf(fp, "%1x", &map[y][x]);
			x++;
		}
		x = 0;
		y++;
	}

	if(!last_floor_flag)
	{
		srand((unsigned)time(NULL));
		while(1) //スタート位置を決める
		{
			p -> start_x = rand()%MAP_X + 1;
			p -> start_y = rand()%MAP_Y + 1;
			if(map[p -> start_y][p -> start_x] == 1)
				break;
		}
		playerx = p -> start_x;
		playery = p -> start_y;

		while(1) //ゴール位置を決める
		{
			p -> goal_x = rand()%MAP_X + 1;
			p -> goal_y = rand()%MAP_Y + 1;
			if(
				map[p -> goal_y][p -> goal_x] == 1 && 
				(p -> goal_y != p -> start_y || p -> goal_x != p -> start_x)
				) //普通の床の上で、プレイヤーの初期位置と被らないこと
				break;
		}

		while(1) //金塊の個数、位置を決める
		{
			p -> gnumber = rand()%5;
			i = 0;
			while(i < p -> gnumber)
			{
				p -> gplacex[i] = rand()%MAP_X + 1;
				p -> gplacey[i] = rand()%MAP_Y + 1;
				if(
					map[p -> gplacey[i]][p -> gplacex[i]] == 1 && 
					(p -> gplacey[i] != p -> start_y || p -> gplacex[i] != p -> start_x) && 
					(p -> gplacey[i] != p -> goal_y || p -> gplacex[i] != p -> goal_x)
					) //普通の床の上で、プレイヤーの初期位置やゴールの位置と被らないこと
				{
					gexist[i] = 1;
					i++;
				}
			}
			break;
		}

		while(1) //巻物の個数、位置を決める
		{
			p -> snumber = rand()%5;
			i = 0;
			while(i < p -> snumber)
			{
				p -> splacex[i] = rand()%MAP_X + 1;
				p -> splacey[i] = rand()%MAP_Y + 1;
				if(
					map[p -> splacey[i]][p -> splacex[i]] == 1 && 
					(p -> splacey[i] != p -> start_y || p -> splacex[i] != p -> start_x) && 
					(p -> splacey[i] != p -> goal_y || p -> splacex[i] != p -> goal_x)
					) //普通の床の上で、プレイヤーの初期位置やゴールの位置と被らないこと
				{
					i2 = 0;
					flag = 0;
					while(i2 < p -> gnumber)
					{
						if(p -> splacex[i] == p -> gplacex[i2] && p -> splacey[i] == p -> gplacey[i2])
						{
							flag++;
							break;

						}
						i2++;
					}
					if(!flag)
					{
						sexist[i] = 1;
						i++;
					}
				}
			}
			break;
		}

		while(1) //飲薬の個数、位置を決める
		{
			p -> pnumber = rand()%5;
			i = 0;
			while(i < p -> pnumber)
			{
				p -> pplacex[i] = rand()%MAP_X + 1;
				p -> pplacey[i] = rand()%MAP_Y + 1;
				if(
					map[p -> pplacey[i]][p -> pplacex[i]] == 1 && 
					(p -> pplacey[i] != p -> start_y || p -> pplacex[i] != p -> start_x) && 
					(p -> pplacey[i] != p -> goal_y || p -> pplacex[i] != p -> goal_x)
					) //普通の床の上で、プレイヤーの初期位置やゴールの位置と被らないこと
				{
					i2 = 0;
					flag = 0;
					while(i2 < p -> gnumber)
					{
						if(p -> pplacex[i] == p -> gplacex[i2] && p -> pplacey[i] == p -> gplacey[i2])
						{
							flag++;
							break;
						}
						i2++;
					}
					i2 = 0;
					flag2 = 0;
					while(i2 < p -> snumber)
					{
						if(p -> pplacex[i] == p -> splacex[i2] && p -> pplacey[i] == p -> splacey[i2])
						{
							flag2++;
							break;
						}
						i2++;
					}
					if(!flag && !flag2)
					{
						pexist[i] = 1;
						i++;
					}
				}
			}
			break;
		}

		while(1) //敵の初期数、種類、位置を決める
		{
			p -> enumber = rand()%5 + 3;
			i = 0;
			while(i < p -> enumber)
			{
				enc[i] -> eplacex = rand()%MAP_X + 1;
				enc[i] -> eplacey = rand()%MAP_Y + 1;
				if(
					map[enc[i] -> eplacey][enc[i] -> eplacex] == 1 && 
					(enc[i] -> eplacey != p -> start_y || enc[i] -> eplacex != p -> start_x) && 
					(enc[i] -> eplacey != p -> goal_y || enc[i] -> eplacex != p -> goal_x)
					) //普通の床の上で、プレイヤーの初期位置やゴールの位置と被らないこと
				{
					i2 = 0;
						while(1)
						{
							enc[i] -> ekind = rand()%ENEMY_MAX + 1; 
							if(ed[enc[i] -> ekind] -> enemy_first_floor <= floor_non_func && ed[enc[i] -> ekind] -> enemy_last_floor >= floor_non_func ) //現状の階がそのモンスターの出現可能範囲であるか
							{
								break;
							}
						}
						enc[i] -> eexist = 1;
						i++;
				}
			}
			break;
		}

		//存在する敵にhpを代入
		i = 0;
		while(enc[i] -> eexist)
		{
			enc[i] -> enemyhp = ed[enc[i] -> ekind] -> enemyhpmax;
			i++;
		}

		while(1) //罠の個数、位置、種類を決める
		{
			p -> tnumber = rand()%5;
			i = 0;
			while(i < p -> tnumber)
			{
				p -> tplacex[i] = rand()%MAP_X + 1;
				p -> tplacey[i] = rand()%MAP_Y + 1;
				if(
					map[p -> tplacey[i]][p -> tplacex[i]] == 1 && 
					(p -> tplacey[i] != p -> start_y || p -> tplacex[i] != p -> start_x) && 
					(p -> tplacey[i] != p -> goal_y || p -> tplacex[i] != p -> goal_x)
					) //普通の床の上で、プレイヤーの初期位置やゴールの位置と被らないこと
				{
					i2 = 0;
					flag = 0;
					while(i2 < p -> tnumber && p -> texist[i2])
					{
						if(p -> tplacex[i] == p -> tplacex[i2] && p -> tplacey[i] == p -> tplacey[i2])
						{
							flag++;
							break;
						}
						i2++;
					}
					if(!flag)
					{
						p -> texist[i] = 1;
						p -> tkinds[i] = rand()%3 + 1;
						trap_open_flag[i] = 0;
						i++;
					}
				}
			}
			break;
		}

		/*while(1) //装備の個数、位置を決める
		{
		p -> wnumber = 0;//;rand()%3;
		i = 0;
		while(i < p -> wnumber)
		{
		p -> wplacex[i] = rand()%MAP_X + 1;
		p -> wplacey[i] = rand()%MAP_Y + 1;
		if(
		map[p -> wplacey[i]][p -> wplacex[i]] == 1 && 
		(p -> wplacey[i] != p -> start_y || p -> wplacex[i] != p -> start_x) && 
		(p -> wplacey[i] != p -> goal_y || p -> wplacex[i] != p -> goal_x)
		) //普通の床の上で、プレイヤーの初期位置やゴールの位置と被らないこと
		{
		i2 = 0;
		flag = 0;
		while(i2 < p -> gnumber)
		{
		if(p -> wplacex[i] == p -> gplacex[i2] && p -> wplacey[i] == p -> gplacey[i2])
		{
		flag++;
		break;
		}
		i2++;
		}
		i2 = 0;
		flag2 = 0;
		while(i2 < p -> snumber)
		{
		if(p -> wplacex[i] == p -> splacex[i2] && p -> wplacey[i] == p -> splacey[i2])
		{
		flag2++;
		break;
		}
		i2++;
		}
		i2 = 0;
		flag3 = 0;
		while(i2 < p -> pnumber) //飲物と被らないこと
		{
		if(p -> wplacex[i] == p -> pplacex[i2] && p -> wplacey[i] == p -> pplacey[i2])
		{
		flag3++;
		break;
		}
		i2++;
		}
		if(!flag && !flag2 && !flag3)
		{
		p -> wexist[i] = 1;
		i++;
		}
		}
		}
		break;
		}*/
	}
	else //最後の階
	{
		playerx = 30;
		playery = 28;
		p -> goal_x = 29;
		p -> goal_y = 1;
		p -> enumber = 1;
		enc[0] -> eplacey = 9;
		enc[0] -> eplacex = 30;
		p -> gnumber = 1;
		p -> gplacex[0] = 30;
		p -> gplacey[0] = 9;
		gexist[0] = 1;
		if(floor_max == 6)
		{
			enc[0] -> ekind = 22;
		}
		else
		{
			enc[0] -> ekind = 7;
		}
		enc[0] -> enemyhp = ed[enc[0] -> ekind] -> enemyhpmax;
		enc[0] -> eexist = 1;
	}

}


void battle(void)
{
	damage = playeratk - ed[enc[knock] -> ekind] -> enemydef;
	damage = (damage > 0 ? damage : 1);
	enc[knock] -> enemyhp = enc[knock] -> enemyhp - damage;
	enc[knock] -> eexist = (enc[knock] -> enemyhp > 0);
	if(!(enc[knock] -> eexist))
	{
		aoe = aoe + ed[enc[knock] -> ekind] -> enemyexp;
		if(aoe >= (Lv + 2)*(Lv + 4)) //レベルアップ時の処理
		{
			aoe = aoe - (Lv + 2)*(Lv + 4);
			Lv++;
			if(player_kind == 1) //人間
			{
			playeratk = playeratk + 2;
			playerdef++;
			playerhpmax = playerhpmax + 3;
			}
			else //エルフ
			{
			playeratk = playeratk + 1;
			playerdef++;
			playerhpmax = playerhpmax + 2;
			}

		}
	}
}

void picmap(void)
{
	p = &map1;
	int x = 0;
	int y = 0;
	int i = 0;
	int i2 = 0;
	map_open(); //先に可視範囲を処理する
	non_first_keep = 0;

	//画面を初期化
	//system("cls"); //画面がちらつく
	while(y < MAP_Y)
	{
		while(x < MAP_X)
		{
			if(0 || map_open_flag[y][x] || full_house_flag)
			{
				if(1)
				{
					switch(map[y][x])
					{
					case 0:
						print_cache("-");
						break;
					case 1:
						print_cache(".");
						break;
					case 2:
						print_cache(" ");
						break;
					case 3:
						print_cache("#");
						break;
					case 4:
						print_cache("|");
						break;
					case 5:
						print_cache("+");
						break;
					}
				}
				else
				{
					printf("%d", map[y][x]); //デバッグ風味
				}

			}
			else
			{
				print_cache(" ");
			}
			x++;
		}
		print_cache("\n");
		x = 0;
		y++;
	}
	//手前に戻す
	defPosition.X = 0;
	defPosition.Y = 0;
	// COORD構造体で指定された座標へカーソルを移動させる
	SetConsoleCursorPosition(hOut, defPosition);
	printf(keep_map);//;cache);
	/*delete[] cache;
	cache=new char[1];
	cache[0]='\0';*/

	//階段を表示
	if(map_open_flag[p -> goal_y][p -> goal_x] == 1 ||
		map_open_flag[p -> goal_y][p -> goal_x] == 2 ||
		full_house_flag)
	{
		defPosition.X = p -> goal_x;
		defPosition.Y = p -> goal_y;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("%%");
	}


	//罠を表示
	while(i < p -> tnumber)
	{
		if(p -> texist[i] && trap_open_flag[i])
		{
			if(map_open_flag[p -> tplacey[i]][p -> tplacex[i]] == 1 ||
				map_open_flag[p -> tplacey[i]][p -> tplacex[i]] == 2 ||
				full_house_flag)
			{
				defPosition.X = p -> tplacex[i];
				defPosition.Y = p -> tplacey[i];
				SetConsoleCursorPosition(hOut, defPosition);
				printf("^");
			}
		}
		i++;
	}

	//金塊を表示
	i = 0;
	while(i < p -> gnumber)
	{
		if(gexist[i])
		{
			if(map_open_flag[p -> gplacey[i]][p -> gplacex[i]] == 1 ||
				map_open_flag[p -> gplacey[i]][p -> gplacex[i]] == 2 ||
				full_house_flag)
			{
				defPosition.X = p -> gplacex[i];
				defPosition.Y = p -> gplacey[i];
				SetConsoleCursorPosition(hOut, defPosition);
				printf("$");
			}
		}
		i++;
	}

	//巻物を表示
	i = 0;
	while(i < p -> snumber)
	{
		if(sexist[i])
		{
			if(map_open_flag[p -> splacey[i]][p -> splacex[i]] == 1 ||
				map_open_flag[p -> splacey[i]][p -> splacex[i]] == 2 ||
				full_house_flag)
			{
				defPosition.X = p -> splacex[i];
				defPosition.Y = p -> splacey[i];
				SetConsoleCursorPosition(hOut, defPosition);
				printf("?");
			}
		}
		i++;
	}

	//飲薬を表示
	i = 0;
	while(i < p -> pnumber)
	{
		if(pexist[i])
		{
			if(map_open_flag[p -> pplacey[i]][p -> pplacex[i]] == 1 ||
				map_open_flag[p -> pplacey[i]][p -> pplacex[i]] == 2 ||
				full_house_flag)
			{
				defPosition.X = p -> pplacex[i];
				defPosition.Y = p -> pplacey[i];
				SetConsoleCursorPosition(hOut, defPosition);
				printf("!");
			}
		}
		i++;
	}

	//装備を表示
	i = 0;
	while(i < p -> wnumber)
	{
		if(p -> wexist[i])
		{
			if(map_open_flag[p -> wplacey[i]][p -> wplacex[i]] == 1 ||
				map_open_flag[p -> wplacey[i]][p -> wplacex[i]] == 2 ||
				full_house_flag
				)
			{
				defPosition.X = p -> wplacex[i];
				defPosition.Y = p -> wplacey[i];
				SetConsoleCursorPosition(hOut, defPosition);
				printf(")");
			}
		}
		i++;
	}

	if(last_floor_flag) //ゴール
	{
		if(map_open_flag[1][29])
		{
			defPosition.X = 29;
			defPosition.Y = 1;
			SetConsoleCursorPosition(hOut, defPosition);
			printf("*");
		}
	}

	
	//敵を表示
	i = 0;
	while(i < ENEMY_MAX)
	{
		if(enc[i] -> eexist)
		{
			if(map_open_flag[enc[i] -> eplacey][enc[i] -> eplacex] == 2 || focussing_flag ||
				full_house_flag || player_kind == 2)
			{
				defPosition.X = enc[i] -> eplacex;
				defPosition.Y = enc[i] -> eplacey;
				SetConsoleCursorPosition(hOut, defPosition);
				switch(enc[i] -> ekind)
				{
				case 1:
					printf("A");
					break;
				case 2:
					printf("B");
					break;
				case 3:
					printf("C");
					break;
				case 4:
					printf("D");
					break;
				case 5:
					printf("E");
					break;
				case 6:
					printf("F");
					break;
				case 7:
    wAttributes = FOREGROUND_BLUE; //青色を指定
    SetConsoleTextAttribute(hStdout, wAttributes); //色変更
					printf("G");
    wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; //使い終わった後は戻さないとずっとそのまま。これは白
    SetConsoleTextAttribute(hStdout, wAttributes);
					break;
				case 8:
					printf("H");
					break;
				//case 9:
					//printf("I"); //透明な敵
					//break;
				case 10:
					printf("J");
					break;
				case 11:
					printf("K");
					break;
				case 12:
					printf("L");
					break;
				case 13:
					printf("M");
					break;
				case 14:
					printf("N");
					break;
				case 15:
					printf("O");
					break;
				case 16:
					printf("P");
					break;
				case 17:
					printf("Q");
					break;
				case 18:
					printf("R");
					break;
				case 19:
					printf("S");
					break;
				case 20:
					printf("T");
					break;
				case 21:
					printf("U");
					break;
				case 22:
					
    wAttributes = FOREGROUND_RED; //赤色を指定
    SetConsoleTextAttribute(hStdout, wAttributes); //色変更
					printf("V");
    wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; //使い終わった後は戻さないとずっとそのまま。これは白
    SetConsoleTextAttribute(hStdout, wAttributes);
					break;
				case 23:
					printf("W");
					break;
				case 24:
					printf("X");
					break;
				case 25:
					printf("Y");
					break;
				case 26:
					printf("Z");
					break;
				}
			}
		}
		i++;
	}

	//プレイヤーを表示
	defPosition.X = playerx;
	defPosition.Y = playery;
	SetConsoleCursorPosition(hOut, defPosition);
	printf("@");

	//毎F消す処理
	defPosition.X = 0;
	defPosition.Y = MAP_Y; //マップのすぐ下へ
	SetConsoleCursorPosition(hOut, defPosition);
	
}


void print_cache(char* str)
{

	/*int i = 0;
	int i2 = 0;
	while(1)
	{
		if(str[i] == '\0')
		{
			break;
		}
		i++;
	}
	

	while(1)
	{
		if(cache[i2] == '\0')
		{
			break;
		}
		i2++;
	}
	
	char* new_cache = new char[i + i2 + 1];
	for(int p = 0; p < i2 ; p++)
	{
		new_cache[p] = cache[p];
	}
	for(int q = 0; q < i ; q++)
	{
		new_cache[i2 + q] = str[q];
	}
	new_cache[i2 + i] = '\0';
	
	cache = new_cache;*/
	if(!non_first_keep)
	{
		strcpy(keep_map, str);
	}
	else
	{
		strcat(keep_map, str);
	}
	non_first_keep++;
}


void manage_flag(void) //アイテムを拾う処理とか
{
	int i = 0;
	int i2 = 0;
	int n = 0;
	int flag = 0;
	int flag2 = 0;
	while(i < p -> gnumber)
	{
		if(gexist[i] && (playerx == p -> gplacex[i]) && (playery == p -> gplacey[i]))
		{
			srand((unsigned)time(NULL)); //乱数設定
			getgold = (rand()%26 + 10) + floor_non_func + (last_floor_flag ? (floor_max == 6 ? 75 : 450) : 0); //とりあえず10~35,深いほど多くなる
			numgold = numgold + getgold;
			gexist[i] = 0;
		}
		i++;
	}

	i = 0;
	while(i < p -> snumber)
	{
		if(sexist[i] && (playerx == p -> splacex[i]) && (playery == p -> splacey[i]))
		{
			n = rand()%5;
			numscroll[n]++;
			sexist[i] = 0;
			strcpy(get_item_name, sname[n]);
			get_item_flag++;
		}
		i++;
	}

	i = 0;
	while(i < p -> pnumber)
	{
		if(pexist[i] && (playerx == p -> pplacex[i]) && (playery == p -> pplacey[i]))
		{
			n = rand()%4;
			numpotion[n]++;
			pexist[i] = 0;
			strcpy(get_item_name, pname[n]);
			get_item_flag++;
		}
		i++;
	}

	i = 0;
	while(i < p -> wnumber)
	{
		if(p -> wexist[i] && (playerx == p -> wplacex[i]) && (playery == p -> wplacey[i]))
		{
			numweapon[0]++;
			p -> wexist[i] = 0;
		}
		i++;
	}

	i = 0;
	while(i < p -> tnumber)
	{
		if(p -> texist[i] && (playerx == p -> tplacex[i]) && (playery == p -> tplacey[i]))
		{
			trap_open_flag[i]++;
			switch(p -> tkinds[i])
			{

			case 1: //テレポート
				while(1)
				{
					playerx = rand()%MAP_X;
					playery = rand()%MAP_Y;
					flag = 0;
					if(map[playery][playerx] != 1)
					{
						flag++;
					}
					i2 = 0;
					flag2 = 0;
					while(i2 < ENEMY_MAX)
					{
						if(enc[i2] -> eexist && enc[i2] -> eplacey == playery && enc[i2] -> eplacex == playerx)
						{
							flag2++;
						}
						i2++;
					}
					if(!flag && !flag2)
					{
						feet_trap_flag = 1;
						picmap();
						break;
					}
				}
				break;
			case 2: //ダメージ床
				playerhp = playerhp - 3;
				feet_trap_flag = 2;
				gameover = (playerhp <= 0);
				break;
			case 3: //おなかが減る
				hungry = (hungry > 50 ? hungry - 50 : 0);
				feet_trap_flag = 3;
				break;
			}
		}
		i++;
	}

	if(last_floor_flag)
	{
		if(playerx == 29 && playery == 1) //ゴール
		{
			game_clear_flag++;
		}
	}
}


void player_information(void)
{

	//元の位置に戻す
	defPosition.X = 0;
	defPosition.Y = MAP_Y; //マップのすぐ下へ
	SetConsoleCursorPosition(hOut, defPosition);
	//プレイヤー情報
	printf("x:%d,y:%d,HP:%d/%d,ATK:%d,DEF:%d ", playerx, playery, playerhp, playerhpmax, playeratk, playerdef);
	//printf(",inkey:%d,feet:%d  \n", inkey, map[playery][playerx]);
	printf("\ngold:%d,Lv:%d,exp:%d/%d ,", numgold, Lv , aoe, (Lv + 2)*(Lv + 4));
	if(hungry > 990)
	{
		printf("satiety:100");
	}
	else
	{
		printf("satiety: %d", (int)ceil(hungry / 10.0));
	}
	printf(",floor:%d ", floor_non_func);

	defPosition.X = 0;
	defPosition.Y = MAP_Y + 2;
	SetConsoleCursorPosition(hOut, defPosition);
	if(numscroll[0])
	{
		printf("t>teleportation scroll * %d", numscroll[0]);
	}
	else
	{
		printf("                           ");
	}

	defPosition.X = 60;
	defPosition.Y = MAP_Y + 3;
	SetConsoleCursorPosition(hOut, defPosition);
	if(numscroll[1])
	{
		printf("h>hardluck scroll * %d", numscroll[1]);
	}
	else
	{
		printf("                      ");
	}

	defPosition.X = 30;
	defPosition.Y = MAP_Y + 2;
	SetConsoleCursorPosition(hOut, defPosition);
	if(numpotion[0])
	{
		printf("c>curing potion * %d", numpotion[0]);
	}
	else
	{
		printf("                    ");
	}

	defPosition.X = 60;
	defPosition.Y = MAP_Y + 2;
	SetConsoleCursorPosition(hOut, defPosition);
	if(numscroll[2])
	{
		printf("g>grasping scroll * %d", numscroll[2]);
	}
	else
	{
		printf("                      ");
	}

	defPosition.X = 0;
	defPosition.Y = MAP_Y + 3;
	SetConsoleCursorPosition(hOut, defPosition);
	if(numpotion[1])
	{
		printf("i>invisible potion * %d", numpotion[1]);
	}
	else
	{
		printf("                       ");
	}

	defPosition.X = 30;
	defPosition.Y = MAP_Y + 3;
	SetConsoleCursorPosition(hOut, defPosition);
	if(numpotion[2])
	{
		printf("f>focussing potion * %d", numpotion[2]);
	}
	else
	{
		printf("                       ");
	}
	
	defPosition.X = 0;
	defPosition.Y = MAP_Y + 4;
	SetConsoleCursorPosition(hOut, defPosition);
	if(numscroll[3])
	{           
		printf("p>prasma brast scroll * %d", numscroll[3]);
	}
	else
	{
		printf("                          ");
	}
	
	defPosition.X = 30;
	defPosition.Y = MAP_Y + 4;
	SetConsoleCursorPosition(hOut, defPosition);
	if(numpotion[3])
	{           
		printf("s>strong potion * %d", numpotion[3]);
	}
	else
	{
		printf("                          ");
	}

	defPosition.X = 60;
	defPosition.Y = MAP_Y + 4;
	SetConsoleCursorPosition(hOut, defPosition);
	if(numscroll[4])
	{           
		printf("b>bind scroll * %d", numscroll[4]);
	}
	else
	{
		printf("                          ");
	}

	if(map[playery][playerx] == 2 || map[playery][playerx] == -1)
	{
		shift_log("     *いしのなかにいる*");
		gameover = 1;
	}
}


void use_item(void)
{
	int i = 0;
	int y = 0;
	int x = 0;
	int flag = 0;
	int flag2 = 0;

	switch(inkey)
	{
	case 99: //c>potion of curing
		if(numpotion[0])
		{
			playerhp = playerhp + 10;
			if(playerhp > playerhpmax)
			{
				playerhp = playerhpmax;
			}
			hungry = (hungry > 950 ? 1000 : hungry + 50);
			numpotion[0]--;
			use_item_flag = 3;
		}
		break;
	case 105: //i>invisible of potion
		if(numpotion[1])
		{
			invisible_time = 30;
			hungry = (hungry > 950 ? 1000 : hungry + 50);
			numpotion[1]--;
			use_item_flag = 9;
		}
		break;
	case 102: //f>potion of focussing
		if(numpotion[2])
		{
			if(!last_floor_flag)
			{
				focussing_flag++;
				hungry = (hungry > 950 ? 1000 : hungry + 50);
				numpotion[2]--;
				use_item_flag = 6;
			}
			else
			{
				shift_log("ここでは意味がなさそうだ");
			}
		}
		break;
	case 115: //s>strong potion
		if(numpotion[3])
		{
			if(rand()%2)
			{
				playeratk++;
			}
			else
			{
				playerdef++;
			}
			hungry = (hungry > 950 ? 1000 : hungry + 50);
			numpotion[3]--;
			use_item_flag = 19;
		}
		break;
	case 116: //t>scroll of teleportation
		if(numscroll[0])
		{
			if(!last_floor_flag)
			{
				while(1)
				{
					playerx = rand()%MAP_X;
					playery = rand()%MAP_Y;
					flag = 0;
					if(map[playery][playerx] != 1)
					{
						flag++;
					}
					i = 0;
					flag2 = 0;
					while(i < ENEMY_MAX)
					{
						if(enc[i] -> eexist && enc[i] -> eplacey == playery && enc[i] -> eplacex == playerx)
						{
							flag2++;
						}
						i++;
					}
					if(!flag && !flag2)
					{
						break;
					}
				}
				numscroll[0]--;
				use_item_flag = 20;

			}
			else
			{
				shift_log("ここでは意味がなさそうだ");
			}
		}
		break;
	case 104: //h>hardluck scroll
		if(numscroll[1])
		{
			while(i < ENEMY_MAX)
			{
				if((enc[i] -> eplacey - playery) * (enc[i] -> eplacey - playery) + (enc[i] -> eplacex - playerx) * (enc[i] -> eplacex - playerx) <= 16)
				{
					enc[i] -> eexist = 0;
				}
				i++;
			}
			numscroll[1]--;
			use_item_flag = 8;
		}
		break;
	case 103: //g>scroll of grasping
		if(numscroll[2])
		{
			if(!last_floor_flag)
			{
				while(y < MAP_Y)
				{
					while(x < MAP_X)
					{
						map_open_flag[y][x] = 2;
						x++;
					}
					y++;
					x = 0;
				}
				numscroll[2]--;
				use_item_flag = 7;

				numscroll[0]--;
				use_item_flag = 20;

			}
			else
			{
				shift_log("ここでは意味がなさそうだ");
			}
		}
		break;
	case 98: //b>bind scroll
		if(numscroll[4])
		{
			bind_time = 25 + Lv;
			numscroll[4]--;
			use_item_flag = 2;
		}
		break;
	case 112: //p>prasma brast scroll
		if(numscroll[3])
		{
			while(i < ENEMY_MAX)
			{
				if((enc[i] -> eplacey - playery) * (enc[i] -> eplacey - playery) + (enc[i] -> eplacex - playerx) * (enc[i] -> eplacex - playerx) <= 36)
				{
					enc[i] -> enemyhp = (enc[i] -> enemyhp > Lv * 2 ? enc[i] -> enemyhp - Lv * 2 : 1);
				}
				i++;
			}
			numscroll[3]--;
			use_item_flag = 16;
		}
		break;
	case 13: //階段
		if(playerx == p -> goal_x && playery == p -> goal_y)
		{
			floor_non_func++;
			floor_down_flag++;
			focussing_flag = 0;
			full_house_flag = 0;
			bind_time = 0;
			shift_log("階段を降りた");
			map_clear();
			read_map();
			picmap();
			player_information();
			oil_of_vug();
		}
		break;
	}
}



void oil_of_vug(void)
{
	int i = 0;
	int i2 = 0;
	char keep[60];
	char keep2[60]; //変数を文字列に追加するための一時記憶用
	int nagasa = 0; //文字列の長さ一時記憶用。良い名前が思いつかなかった

	defPosition.X = MAP_X + 5;
	defPosition.Y = 0;
	SetConsoleCursorPosition(hOut, defPosition);

	if(get_item_flag)
	{
		strcpy(keep, get_item_name);
		strcat(keep, "を拾った。");
		shift_log(keep);
		get_item_flag = 0;
		strcpy(get_item_name,"");
	}
	if(getgold)
	{
		sprintf(keep, "%d", getgold); //数値をコピーする際にはこちらが必要
		strcat(keep, "goldを拾った。");
		shift_log(keep);
		getgold = 0;
	}

	if(use_item_flag + 1)
	{
		switch(use_item_flag)
		{
		case 2:
			shift_log("敵を縛り付けた");
			break;
		case 6:
			shift_log("集中力が増した");
			break;
		case 3:
			shift_log("元気が出てきた");
			break;
			//default: //いつか入れたい
			//printf("あなたはそれを持っていない");
		case 8:
			shift_log("魔神ハードラックを呼び出した");
			break;
		case 9:
			shift_log("体が透明になった");
			break;
		case 7:
			shift_log("地形を把握した");
			break;
		case 16:
			shift_log("雷が周囲の敵に降り注いだ");
			break;
		case 19:
			shift_log("体が熱くなった");
			break;
		case 20:
			strcpy(keep, "(");
			sprintf(keep2, "%d", playerx);
			strcat(keep, keep2);
			strcat(keep, ",");
			sprintf(keep2, "%d", playery);
			strcat(keep, keep2);
			strcat(keep, ")に移動した");
			shift_log(keep);
			break;
		}
	}

	if(feet_trap_flag)
	{
		switch(feet_trap_flag)
		{
		case 1:
			shift_log("此処は何処だ!");
			break;
		case 2:
			shift_log("いてっ!");
			break;
		case 3:
			shift_log("良い匂いがする");
			break;
		}
		feet_trap_flag = 0;
	}

	if(knock + 1)
	{
		strcpy(keep, ename[enc[knock] -> ekind]);
		strcat(keep, "に");
		sprintf(keep2, "%d", damage);
		strcat(keep, keep2);
		strcat(keep, "のダメージを与えた");
		shift_log(keep);
		if(!enc[knock] -> eexist)
		{
			strcpy(keep, ename[enc[knock] -> ekind]);
			strcat(keep, "を倒した。exp+");
			sprintf(keep2, "%d", ed[enc[knock] -> ekind] -> enemyexp);
			strcat(keep, keep2);
			if(aoe < ed[enc[knock] -> ekind] -> enemyexp)
			{
				strcat(keep, "。");
				sprintf(keep2, "%d", Lv);
				strcat(keep, keep2);
				strcat(keep, "Lvにレベルアップ");
			}
			shift_log(keep);
		}
	}

	i = 0;
	while(i < ENEMY_MAX)
	{
		if(enc[i] -> eexist && enc[i] -> knocked)
		{
			strcpy(keep , ename[enc[i] -> ekind]);
			strcat(keep, "から");
			sprintf(keep2, "%d", enc[i] -> damaged);
			strcat(keep, keep2);
			strcat(keep, "のダメージを受けた");
			shift_log(keep);
		}
		i++;
	}
	use_item_flag = -1;

	if(playerhp <= 0)
	{
		gameover++;
	}


	//実況欄を消す処理。何かいい方法はないものか
	//if(log_change_flag)
	//{
		//i = 0;
	//	i2 = 0;

		//strcpy(keep, keep2);
		/*while(i < LOG_LINES)
		{
		//実況用の位置に移動
		defPosition.X = MAP_X + 5;
		defPosition.Y = i;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("                                                           "); //59字
		i++;
		}*/
	//}
	//ログの描画
	/*if(log_change_flag)
	{
		i = 0;
		while(i < LOG_LINES)
		{
			defPosition.X = MAP_X + 5;
			defPosition.Y = i;
			SetConsoleCursorPosition(hOut, defPosition);
			if(log_keep[i] != NULL)
			{
				printf("%s", log_keep[i]);
				nagasa = strlen(log_keep[i]);
				i2 = 1;
				while(i2 < 60 - nagasa)
				{
					printf(" ");
					i2++;
				}
			}
			i++;
		}
	}
	log_change_flag = 0;*/
	log_print();
}


void log_print(void)
{
	int i = 0;
	int i2 = 0;
	int nagasa = 0;
	//ログの描画
	if(log_change_flag)
	{
		i = 0;
		while(i < LOG_LINES)
		{
			defPosition.X = MAP_X + 5;
			defPosition.Y = i;
			SetConsoleCursorPosition(hOut, defPosition);
			if(log_keep[i] != NULL)
			{
				printf("%s", log_keep[i]);
				nagasa = strlen(log_keep[i]);
				i2 = 1;
				while(i2 < 60 - nagasa)
				{
					printf(" ");
					i2++;
				}
			}
			i++;
		}
	}
	log_change_flag = 0;
}


void shift_log(char *text)
{
	int i = 1;
	//int i2 = 0;
	//int nagasa = 0;
	while(i < LOG_LINES)
	{
		strcpy(log_keep[LOG_LINES - i], log_keep[LOG_LINES - (i + 1)]);
		i++;
	}

	strcpy(log_keep[0], text);
	log_change_flag++;
}


void gameover_manage(void)
{
	int key = 0;
	shift_log("                   GAME OVER");
	shift_log("                   please,press \"q\"");
	log_print();
	while(1)
	{
		key = getch();
		if(key == 113)
		{
			break;
		}
	}
	system("cls");
	ranking_manage();
}


void enemy_act(void)
{
	int i = 0;
	int i2 = 0;
	int i3 = 0;
	int i4 = 0;
	int enx = 0;
	int eny = 0;
	int enx2 = 0;
	int eny2 = 0;
	int pdistance = 0;
	int pdistance2 = 0;
	int flag = 0;
	int y = 0;
	int x = 0;
	int ey = 0;
	int ex = 0;
	int n = 0;
	int millitime = 0;

	if(!last_floor_flag)
	{
		while(i < ENEMY_MAX)
		{
			enc[i] -> knocked = 0;
			enc[i] -> damaged = -1;
			if(enc[i] -> eexist)
			{
				if(!invisible_time) //プレイヤーの透明フラグOFF
				{
					i2 = 0;
					while(i2 < 3) //周囲8マスにプレイヤーがいるか
					{
						eny = enc[i] -> eplacey + (i2 - 1);
						i3 = 0;
						while(i3 < 3)
						{
							enx = enc[i] -> eplacex + (i3 - 1);
							if(eny == playery && enx == playerx) //位置が一致した場合
							{
								enc[i] -> damaged = ed[enc[i] -> ekind] -> enemyatk - playerdef;
								enc[i] -> damaged = (enc[i] -> damaged > 0 ? enc[i] -> damaged : 1);
								playerhp = playerhp - enc[i] -> damaged;
								gameover = (playerhp <= 0);
								enc[i] -> knocked = 1; //ぶつかられフラグ
								goto loop_out;
							}
							i3++;
						}
						i2++;
					}

					if(!bind_time)
					{
					y = (playery / (MAP_Y / MAP_CUT_Y)); //ブロックのどこにいるか
					x = (playerx / (MAP_X / MAP_CUT_X));	
					ey = (enc[i] -> eplacey / (MAP_Y / MAP_CUT_Y)); //ブロックのどこにいるか
					ex = (enc[i] -> eplacex / (MAP_X / MAP_CUT_X));
					if(enc[i] -> ekind != 2) //BATはふらふら動く
					{
						if(y == ey && x == ex || full_house_flag) //プレイヤーと同じブロックにいる場合
						{
							i2 = 0;
							pdistance = 0;
							pdistance2 = 0;
							while(i2 < 3)
							{
								i3 = 0;
								eny2 = enc[i] -> eplacey + (i2 - 1);
								while(i3 < 3)
								{
									enx2 = enc[i] -> eplacex + (i3 - 1);
									i4 = 0;
									flag = 0;
									while(i4 < ENEMY_MAX) //他の敵と位置が被ってないか
									{
										if(eny2 == enc[i4] -> eplacey && enx2 == enc[i4] -> eplacex)
										{
											flag++;
											break;
										}
										i4++;
									}
									if(!(map[eny2][enx2] != 1 && map[eny2][enx2] != 3 && map[eny2][enx2] != 5)
										&& !(eny2 == playery && enx2 == playerx)) //床の上でプレイヤーと被っていないか
									{
										pdistance2 = (playerx - enx2) * (playerx - enx2) + (playery - eny2) * (playery - eny2);
										if(!flag &&(pdistance > pdistance2 || !pdistance)) //距離がより近い方を選ぶ
										{
											pdistance = pdistance2;
											eny = eny2;
											enx = enx2;
										}
									}
									i3++;
								}
								i2++;
							}
							enc[i] -> eplacey = eny;
							enc[i] -> eplacex = enx;
							goto loop_out;
						}
					}
				}
				}

				if(!bind_time)
				{
				n = 0;
				while(n < 100) //100回ループしても駄目なら動かない
				{
					n++;
					millitime = (unsigned long)timeGetTime() + n; //普通にミリ秒を取得するよりはいい感じ
					srand(millitime);

					eny = enc[i] -> eplacey + (rand()%3 - 1);
					enx = enc[i] -> eplacex + (rand()%3 - 1);
					if((map[eny][enx] == 1 || map[eny][enx] == 3 || map[eny][enx] == 5) && !(eny == playery && enx == playerx)) //足元が床でプレイヤーに被ってないか
					{
						i2 = 0;
						flag = 0;
						while(i2 < ENEMY_MAX) //他の敵と位置が被ってないか
						{
							if(i != i2)
							{
								if(enc[i2] -> eexist)
								{
									if(eny == enc[i2] -> eplacey && enx == enc[i2] -> eplacex)
									{
										flag++;
										break;
									}
								}
							}
							i2++;
						}
						if(!flag) //問題がなければ移動
						{
							enc[i] -> eplacey = eny;
							enc[i] -> eplacex = enx;
							break;
						}
					}
				}
				}
			}
loop_out:
			i++;
		}
	}
	else //最後の階
	{
		enc[0] -> knocked = 0;
		enc[0] -> damaged = -1;
		if(enc[0] -> eexist)
		{
			if(!invisible_time) //プレイヤーの透明フラグOFF
			{
				i2 = 0;
				while(i2 < 5) //周囲25マスにプレイヤーがいるか
				{
					eny = enc[0] -> eplacey + (i2 - 1);
					i3 = 0;
					while(i3 < 5)
					{
						enx = enc[0] -> eplacex + (i3 - 1);
						if(eny == playery && enx == playerx) //位置が一致した場合
						{
							enc[0] -> damaged = ed[enc[0] -> ekind] -> enemyatk - playerdef;
							enc[0] -> damaged = (enc[0] -> damaged > 0 ? enc[0] -> damaged : 1);
							playerhp = playerhp - enc[0] -> damaged;
							gameover = (playerhp <= 0);
							enc[0] -> knocked = 1; //ぶつかられフラグ
							goto loop_out2;
						}
						i3++;
					}
					i2++;
				}
			}
		}
	}
loop_out2:
	i++;
}


void born_enemy(void)
{
	int i = 0;
	int i2 = 0;
	int flag = 0;
	int flag2 = 0;
	int flag3 = 0;
	int n = 0;
	int millitime = 0;
	int x = 0;
	int y = 0;
	int ex = 0;
	int ey = 0;

	if(!last_floor_flag) //最後の階でないこと
	{
		srand((unsigned long)timeGetTime());
		if(!(rand()%BORN_SPEED))
		{
			while(i < ENEMY_MAX)//保険
			{
				if(!(enc[i] -> eexist))
				{
					while(1)
					{
						n++;
						millitime = (unsigned long)timeGetTime() + n; //普通にミリ秒を取得するよりはいい感じ
						srand(millitime);
						enc[i] -> eplacex = rand()%MAP_X + 1;
						enc[i] -> eplacey = rand()%MAP_Y + 1;
						if(  map[enc[i] -> eplacey][enc[i] -> eplacex] == 1 && 
							(enc[i] -> eplacey != playery || enc[i] -> eplacex != playerx)
							) //普通の床の上で、プレイヤーの位置と被らないこと
						{
							flag = 0;
							y = (playery / (MAP_Y / MAP_CUT_Y)); //ブロックのどこにいるか
							x = (playerx / (MAP_X / MAP_CUT_X));
							ey = (enc[i] -> eplacey / (MAP_Y / MAP_CUT_Y)); //ブロックのどこにいるか
							ex = (enc[i] -> eplacex / (MAP_X / MAP_CUT_X)); 
							if(ey == y && ex == x) //プレイヤーの近くでないこと
							{
								flag++;
							}
							if(!flag || full_house_flag || !(rand()%99)) //保険
							{
								while(1)
								{
									enc[i] -> ekind = rand()%ENEMY_MAX + 1; 
									if(ed[enc[i] -> ekind] -> enemy_first_floor <= floor_non_func && ed[enc[i] -> ekind] -> enemy_last_floor >= floor_non_func ) //現状の階がそのモンスターの出現可能範囲であるか
									{
										break;
									}
								}
								enc[i] -> eexist = 1;
								enc[i] -> enemyhp = ed[enc[i] -> ekind] -> enemyhpmax;
								break;
							}
						}
					}
					break;
				}
				i++;
			}
		}
	}
}


void map_clear(void)
{
	int x = 0;
	int y = 0;
	int i = 0;
	p = &map1;

	fp = NULL;

	while(y < MAP_Y)
	{
		while(x < MAP_X)
		{
			map[y][x] = 2;
			x++;
		}
		x = 0;
		y++;
	}

	y = 0;
	while(y < MAP_Y)
	{
		x = 0;
		while(x < MAP_X)
		{
			map_open_flag[y][x] = 0;
			x++;
		}
		y++;
	}

	p -> start_x = -1;
	p -> start_y = -1;
	playerx = -1;
	playery = -1;
	p -> goal_x = -1;
	p -> goal_y = -1;
	p -> gnumber = 0;
	while(i < GOLD_MAX)
	{
		p -> gplacex[i] = -1;
		p -> gplacey[i] = -1;
		gexist[i] = 0;
		i++;
	}

	p -> snumber = 0;
	i = 0;
	while(i < SCROLL_MAX)
	{
		p -> splacex[i] = -1;
		p -> splacey[i] = -1;
		sexist[i] = 0;
		i++;
	}

	p -> pnumber = 0;
	i = 0;
	while(i < POTION_MAX)
	{
		p -> pplacex[i] = -1;
		p -> pplacey[i] = -1;
		pexist[i] = 0;
		i++;
	}

	p -> enumber = 0;
	i = 0;
	while(i < ENEMY_MAX)
	{
		enc[i] -> eplacex = -1;
		enc[i] -> eplacey = -1;
		enc[i] -> ekind = -1;
		enc[i] -> eexist = 0;
		enc[i] -> enemyhp = 0;
		i++;
	}

	p -> tnumber = 0;
	i = 0;
	while(i < TRAP_MAX)
	{
		p -> tplacex[i] = -1;
		p -> tplacey[i] = -1;
		p -> texist[i] = 0;
		p -> tkinds[i] = 0;
		i++;
	}


}


void every_varset(void)
{
	inkey = 0;
	floor_down_flag = 0;
	knock = -1;
}


void map_open(void)
{
	int i = -1;
	int i2 = -1;
	int y = 0;
	int x = 0;

	if(!last_floor_flag)
	{
		while(y < MAP_Y) //部屋から離れたら見えなくなる処理
		{
			while(x < MAP_X)
			{
				switch(map_open_flag[y][x])
				{
				case 2:
					map_open_flag[y][x] = 1;
					break;
				}
				x++;
			}
			x = 0;
			y++;
		}
		x = 0;
		y = 0;

		if(map[playery][playerx] == 3) //通路は1マス先まで見える
		{
			i = 0;
			while(i < 3)
			{
				i2 = 0;
				while(i2 < 3)
				{
					map_open_flag[playery + (i - 1)][playerx + (i2 - 1)] = 2;
					i2++;
				}
				i++;
			}
		}
		else if(map[playery][playerx] == 1 || map[playery][playerx] == 5) //部屋の中もしくは扉
		{
			i = 0;
			i2 = 0;
			y = (playery / (MAP_Y / MAP_CUT_Y)); //ブロックのどこにいるか
			x = (playerx / (MAP_X / MAP_CUT_X));
			while(i < (MAP_Y / MAP_CUT_Y))
			{
				while(i2 < (MAP_X / MAP_CUT_X))
				{
					if(map[(MAP_Y / MAP_CUT_Y) * y + i][(MAP_X / MAP_CUT_X) * x + i2] != 3) //ただし通路でない
					{
						map_open_flag[(MAP_Y / MAP_CUT_Y) * y + i][(MAP_X / MAP_CUT_X) * x + i2] = 2;
					}
					i2++;
				}
				i2 = 0;
				i++;
			}
		}
	}
	else //	最後の階
	{
		if(playerx == 30 && playery == 28) //最初の位置
		{
			i = 0;
			while(i < 9)
			{
				i2 = 0;
				while(i2 < 12)
				{
					map_open_flag[21 + i][24 + i2] = 2;
					i2++;
				}
				i++;
			}
		}
		if(map[playery][playerx] == 3 || map[playery][playerx] == 5) //通路は1マス先まで見える
		{
			i = 0;
			while(i < 3)
			{
				i2 = 0;
				while(i2 < 3)
				{
					map_open_flag[playery + (i - 1)][playerx + (i2 - 1)] = 2;
					i2++;
				}
				i2 = -1;
				i++;
			}
		}
		if((playerx == 30 || playerx == 29) && playery == 14) //敵のいる部屋に踏み込んだ
		{
			i = 0;
			while(i < 8)
			{
				i2 = 0;
				while(i2 < 18)
				{
					map_open_flag[7 + i][21 + i2] = 2;
					i2++;
				}
				i++;
			}
		}

		if(((playerx == 30 || playerx == 29) && playery == 4) || //階段のある部屋に踏み込んだ
			((playerx == 25 || playerx == 34) && playery == 1)
			)
		{
			i = 0;
			while(i < 5)
			{
				i2 = 0;
				while(i2 < 10)
				{
					map_open_flag[0 + i][25 + i2] = 2;
					i2++;
				}
				i++;
			}
		}
	}
}


void game_clear(void)
{
	if(game_clear_flag)
	{


		
		system("cls");
		
		defPosition.X = 20;
		defPosition.Y = 15;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("貴方は無事、秘宝を見つけ迷宮から脱出することができた。");
		defPosition.X = 25;
		defPosition.Y = 20;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("GAME CLEAR\n");
		ranking_manage();
	}
}


void ranking_manage(void)
{
	
		FILE *gameload;
		int i = 0;
		int i2 = 0;
		char keep_name[21];
	    int keep_score = 0;

		int point = 0; //構造体の中身
		char pname_clog[21];
		defPosition.X = 20;
		defPosition.Y = 25;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("please, press your name & \"Enter\"\n");
		scanf("%20s", &playername);
		fflush(stdin);


		gameload = fopen("./clear_log.txt", "a+");
		if(fp == NULL)
		{
			printf("ファイルの読み込みに失敗しました");
			exit(1);
		}
		
		fprintf(gameload, "%s %d\n", playername, numgold); //まず結果を書き込む

		
		fclose(gameload);
		gameload = fopen("./clear_log.txt", "a+");
		if(fp == NULL)
		{
			printf("ファイルの読み込みに失敗しました");
			exit(1);
		}

		int c = 0;
		int cnt = 0;


		while(c != EOF) //26
		{
			c = getc(gameload);
			if(c == '\n')
			{
				cnt++;
			}
		}
		fclose(gameload);

		gameload = fopen("./clear_log.txt", "a+");
		if(fp == NULL)
		{
			printf("ファイルの読み込みに失敗しました");
			exit(1);
		}

		clog* clear_log_point;
		clear_log_point = new clog[cnt];
		i = 0;
		while(i < cnt)
		{
			fscanf(gameload, "%s%d", clear_log_point[i].pname_clog, &(clear_log_point[i].point)); //スコアを読み込む
			i++;
		}



		defPosition.Y = 25;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("                                 ");

		defPosition.Y = 30;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("%s's score is %d\n", playername, numgold);
		
		i = 0;
		while(i < cnt) //結果のソート
		{
			i2 = 0;
			while(i2 + i< cnt)
			{
				if(clear_log_point[i].point < clear_log_point[i2 + i].point)
				{
					keep_score = clear_log_point[i].point;
					clear_log_point[i].point = clear_log_point[i2 + i].point;
					clear_log_point[i2 + i].point = keep_score;

					strcpy(keep_name, clear_log_point[i].pname_clog);
					strcpy(clear_log_point[i].pname_clog, clear_log_point[i2 + i].pname_clog);
					strcpy(clear_log_point[i2 + i].pname_clog, keep_name);
				}
				i2++;
			}
			i++;
		}


		i = 0;
		while(i < cnt && i < 30)
		{
			defPosition.X = 65;
			defPosition.Y = 5 + i;
			SetConsoleCursorPosition(hOut, defPosition);
			printf("rank%d,%s:%d", i + 1, clear_log_point[i].pname_clog, clear_log_point[i].point);
			i++;
		}

		fclose(gameload);
		delete []clear_log_point;
		getch();
}