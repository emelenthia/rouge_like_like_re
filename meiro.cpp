#include<stdio.h>
#include<conio.h> //getch�֐�
#include<stdlib.h> //rand�֐��Ƃ�
#include<windows.h> //�J�[�\���̈ʒu���ړ�����
#include<time.h> //time�萔
#include<math.h> //ceil�֐�
//#include<unistd.h>
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "winmm.lib")//�~���b�̎擾
#pragma warning(disable:4996)
#define MAP_X 60 //���H�̉��̌��E
#define MAP_Y 30 //���H�̏c�̌��E
#define ENEMY_MAX 15 //�����ɑ��݂ł���G�̍ő吔
#define ENEMY_KINDS 26+1 //�G�̎�ނ̍ő吔(+1,0�Ԗڂ͉B���L����?)
#define GOLD_MAX 5 //���݂ł������̑���
#define SCROLL_MAX 5 //���݂ł��銪���̑���
#define POTION_MAX 5 //���݂ł������̑���
#define TRAP_MAX 5 //���݂ł���㩂̑���
#define MAP_CUT_X 3 //�}�b�v�̏c�Ɋւ��镪��������(����n-1�{)
#define MAP_CUT_Y 3 //�}�b�v�̉��Ɋւ��镪��������(����n-1�{)
#define BORN_SPEED 25 //�����X�^�[���V���ɐ��܂�鑁���B�傫���قǒx���Ȃ�
#define LOG_LINES 30 //�\�����郍�O�̍s��
#define CURE_SPEED 10 //HP���񕜂��鑁���B�傫���قǒx���Ȃ�

int debug_kazoe = 0;

//�O���[�o���ϐ�
int inkey = -1;
int playerx = -1;
int playery = -1;
int gexist[5] = {0}; //����̑��݃t���O
int numgold = 0; //�v���C���[�̏�������
int numwalk = 0; //�v���C���[�̗݌v����
int hungry = 1000; //�����x�B/10���Ďg�p
int sexist[5] = {0}; //�����̑��݃t���O
int numscroll[5] = {0}; //�e�����̏������B0�cscroll of teleportation,
int auto_manage = 0; //0�łȂ��Ƃ������������ɂ��L�[���͂��Ȃ��B����͊Q���Ȃ̂Ŏg��Ȃ��悤��
int gameover = 0; //�Q�[���I�[�o�[�t���O
int playerhpmax = -1; //�v���C���[�̍ő�hp
int playerhp = -1; //�v���C���[�̌���hp
int pexist[5] = {0}; //����̑��݃t���O
//0:hering,1:invisible,2:focussing(�G�̈ʒu��������)
int numpotion[5] = {0}; //�e����̏������B0�cpotion of curing,
int knock = -1; //�Gn�Ƃ̂Ԃ���t���O
int playeratk = 0; //�v���C���[��ATK�B�v���C���[�����\���̂ɂ܂Ƃ߂�Ӗ��͂��邩
int playerdef = 0; //�v���C���[��DEf
int use_item_flag = -1; //n�Ԃ̃A�C�e���g�p�t���O�B�ԍ���0���珇�Ɏg�p����ۂɉ����A���t�@�x�b�g��
int get_item_flag = 0; //�A�C�e������t���O�B
int damage = -1; //�^���Ă���_���[�W
int aoe = 0; //���݂̌o���l
int Lv = 1; //���݂̃��x��
int floor_non_func = 1; //���݂̊K��
int floor_down_flag = 0; //�K�i���~���t���O
int numweapon[5] = {0}; //�������Ă��鑕��(����͕���̂�?)
//1:�V���[�g�\�[�h
char get_item_name[32];
char sname[5][32];
char pname[5][32];
int invisible_time = 0; //�c�蓧������
int focussing_flag = 0; //�W���t���O
int getgold = 0; //�������t���O
char log_keep[LOG_LINES][60]; //���O������ɕ\�����邽�߂̕��͂�ۑ�����B
int log_change_flag = 0; //���O��ύX�������̃t���O
int full_house_flag = 0; //�啔���t���O
int floor_max = 0; //���K�܂ł���̂�
int last_floor_flag = 0; //���������X�g
char playername[21]; //�v���C���[�̖��O
int game_clear_flag = 0;
int bind_time = 0;
int non_first_keep = 0;

int map[MAP_Y][MAP_X];// = {-1}; //���H�̓��e
int map_open_flag[MAP_Y][MAP_X] = {-1}; //���H���t���O�B0�Ō����Ȃ��A1�Ō����邪�G�͌����Ȃ��A2�Ŋ��S�Ɍ�����B
int start_x = -1; //�X�^�[�g�ʒuX
int start_y = -1; //�X�^�[�g�ʒuY
int goal_x = -1; //�S�[���ʒuX
int goal_y = -1; //�S�[���ʒuY
int gnumber = 0; //����̌�
int gplacex[5] = {-1}; //����̈ʒuX
int gplacey[5] = {-1}; //����̈ʒuY
int snumber = 0; //�����̌�
int splacex[5] = {-1}; //�����̈ʒuX
int splacey[5] = {-1}; //�����̈ʒuY
int pnumber = 0; //����̌�
int pplacex[5] = {-1}; //����̈ʒuX
int pplacey[5] = {-1}; //����̈ʒuY
int enumber = 0; //�G�̐�
int wnumber = 0; //�����̐�
int wplacex[5] = {-1}; //�����̈ʒuX
int wplacey[5] = {-1}; //�����̈ʒuY
int wexist[5] = {0}; //�����̑��݃t���O
char ename[ENEMY_KINDS][16]; //�G�̖��O�ꗗ/�Ƃ肠����15���܂�
int tnumber = 0;; //㩂̌�
int tplacex[5] = {0}; //㩂̈ʒuX
int tplacey[5] = {0};//㩂̈ʒuY
int texist[5] = {0};//㩂̑��݃t���O
int tkinds[5] = {0};//�e㩂̎��
//1:�e���|�[�g���,2:�j���ۂ���,
int trap_open_flag[5] = {0};//�e㩂̌����Ă�t���O
int feet_trap_flag = 0; //㩂𓥂񂾎�
char *cache = new char[1];
int player_kind = 0;
char keep_map[1830];//[MAP_Y][MAP_X + 1];

//�}�b�v�̏��
struct mapdata
{
	int map[MAP_Y][MAP_X]; //���H�̓��e
	int map_open_flag[MAP_Y][MAP_X];
	int start_x; //�X�^�[�g�ʒuX
	int start_y; //�X�^�[�g�ʒuY
	int goal_x; //�S�[���ʒuX
	int goal_y; //�S�[���ʒuY
	int gnumber; //����̌�
	int gplacex[5]; //����̈ʒuX
	int gplacey[5]; //����̈ʒuY
	int snumber; //�����̌�
	int splacex[5]; //�����̈ʒuX
	int splacey[5]; //�����̈ʒuY
	int pnumber; //����̌�
	int pplacex[5]; //����̈ʒuX
	int pplacey[5]; //����̈ʒuY
	int wnumber; //�����̐�
	int wplacex[5]; //�����̈ʒuX
	int wplacey[5]; //�����̈ʒuY
	int wexist[5]; //�����̑��݃t���O
	int enumber; //�G�̐�
	int tnumber; //㩂̌�
	int tplacex[5]; //㩂̈ʒuX
	int tplacey[5]; //㩂̈ʒuY
	int texist[5]; //㩂̑��݃t���O
	int tkinds[5]; //�e㩂̎��
}map1, *p;

FILE *fp;

int eexist = 0; //�G�̑��݃t���O
int ekind = -1; //�G�̎��
int eplacex = -1; //�G�̏����ʒuX
int eplacey = -1; //�G�̏����ʒuY
int enemyhp = -1; //�G�̌���hp
int damaged = -1; //�_���[�W�̎�g�`�B����
int knocked = 0; //�Gn�Ƃ̂Ԃ����t���O�B�����̂��l��

//�ʂ̓G�̏��
struct enemy_non_change
{
	int eexist; //�G�̑��݃t���O
	int ekind;
	int eplacex; //�G�̈ʒuX
	int eplacey; //�G�̈ʒuY
	int enemyhp; //�G�̌���hp
	int damaged; //�_���[�W�̎�g�`�B����
	int knocked; //�Gn�Ƃ̂Ԃ����t���O�B�����̂��l�����č\���̂̔z��ɓ����
}enemy_nc[ENEMY_MAX], *enc[ENEMY_MAX];

int point = 0;
char pname_clog[21];
typedef struct clear_log
{
	int point;
	char pname_clog[21];
}clog;

int enemyhpmax = -1; //�G�l�~�[�̍ő�HP
int enemyatk = -1; //�����X�^�[��ATK
int enemydef = -1; //�G�L������DEF
int enemy_data_count = ENEMY_KINDS; //�G�L�����̑���
int enemyexp = 0; //�G�L������|�������ɂ��炦��exp
int enemy_first_floor = 0; //���̓G���o�n�߂�K��
int enemy_last_floor = 0; //���̓G���Ō�ɏo��K��

//�G�̃f�[�^
struct enemy_data
{
	int enemyhpmax;
	int enemyatk;
	int enemydef;
	int enemyexp;
	int enemy_first_floor;
	int enemy_last_floor;
}enemy_d[ENEMY_KINDS], *ed[ENEMY_KINDS];


// �o�͐�̃n���h�����擾
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
// COORD�\���̂֍��W��ݒ肷��&�錾
COORD defPosition = {0, 0};

	HANDLE hStdout; //�F��ς��鏈��
    WORD wAttributes;
    CONSOLE_SCREEN_BUFFER_INFO csbi;//�\���̂ł�

//���͂��ꂽ�L�[���擾����
int getcursor(void);
//�}�b�v��ǂݍ���
void read_map(void);
//�}�b�v��`��
void picmap(void);
//�s�������ʒu�ɕ����������ꍇ�̏���
void manage_flag(void);
//�v���C���[�̏���`��
void player_information(void);
//�A�C�e�����g������
void use_item(void);
//�Q�[���I�[�o�[�ɂȂ������̏���
void gameover_manage(void);
//�E�ƌ���I��
void questions(void);
//�ϐ��̏����ݒ�
void varrangeset(void);
//�G�L�����̃f�[�^����(�ԍ�,hpmax,ATK,DEF,���O)
void parentvarset(int i, int a, int b, int c, int d, int first_floor, int last_floor, char *name);
//����
void oil_of_vug(void);
//�퓬������
void battle(void);
//����̍s��
void enemy_act(void);
//�G�L�����𐶐�
void born_enemy(void);
//�}�b�v����������
void map_clear(void);
//�펞���Z�b�g�ϐ��p
void every_varset(void);
//�}�b�v���͈͂̏���
void map_open(void);
//㩂𓥂񂾏���
void feet_trap(int kind);
//���O�̐����E�ǉ�
void shift_log(char *text);
//�d���Ȃ�Ȃ���ʂ̕`����@
void print_cache(char *str);
//�^�C�g�����
void title_prev(void);
//�Q�[���N���A����
void game_clear(void);
void debug_now(void);
void ranking_manage(void);
void log_print(void);

void debug_now(void)
{
	debug_kazoe++;
	defPosition.X = 85;
	defPosition.Y = 33;
	// COORD�\���̂Ŏw�肳�ꂽ���W�փJ�[�\�����ړ�������
	SetConsoleCursorPosition(hOut, defPosition);
	printf("  ");
	defPosition.X = 85;
	defPosition.Y = 33;
	// COORD�\���̂Ŏw�肳�ꂽ���W�փJ�[�\�����ړ�������
	SetConsoleCursorPosition(hOut, defPosition);
	printf("%d", debug_kazoe);
}


//���C���֐�
int main()
{
	// �R���\�[���̃T�C�Y��ύX
	COORD dwSize = {160, 90};
	SetConsoleScreenBufferSize(hOut, dwSize);

	WCHAR titleTextSize[128]; //�ő剻
	GetConsoleTitle(titleTextSize, 128);
	HWND consoleWindowTitle = FindWindow(NULL, titleTextSize);
	ShowWindow(consoleWindowTitle, SW_MAXIMIZE);
	
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE); //�F��ς��鏈��
    //hStdout�̃R���\�[���X�N���[���o�b�t�@����csbi�Ɏ擾
    GetConsoleScreenBufferInfo(hStdout, &csbi);

	int stop = 0;

	while(1)
	{
		//[statedef 5900]
		varrangeset();
		map_clear();
		//�^�C�g�����
		title_prev();
		//�ŏ��̐ݒ�
		questions();

		//�ŏ��̕`��
		read_map();
		picmap();
		player_information();

		while(/*inkey != 113 &&*/ !gameover && !game_clear_flag)
		{
			every_varset();
			inkey = getcursor();
			if(knock + 1) //�G�ɂԂ�����
			{
				battle();
			}
			use_item();
			if(!floor_down_flag) //�K�i���~�肽
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

	while(i < 30) //�ϐ����Z
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
	while(i < ENEMY_MAX) //�ϐ����Z
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

	//�O���[�o���ϐ�
	inkey = -1;
	playerx = -1;
	playery = -1;
	i = 0;
	while(i < 5)
	{
		gexist[i] = 0; //����̑��݃t���O
		sexist[i] = 0; //�����̑��݃t���O
		numscroll[i] = 0; //�e�����̏������B0�cscroll of teleportation,
		pexist[i] = 0; //����̑��݃t���O
		numpotion[i] = 0; //�e����̏������B0�cpotion of curing,
		numweapon[i] = 0; //�������Ă��鑕��(����͕���̂�?)
		i++;
	}
	numgold = 0; //�v���C���[�̏�������
	numwalk = 0; //�v���C���[�̗݌v����
	hungry = 1000; //�����x�B/10���Ďg�p
	auto_manage = 0; //0�łȂ��Ƃ������������ɂ��L�[���͂��Ȃ��B����͊Q���Ȃ̂Ŏg��Ȃ��悤��
	gameover = 0; //�Q�[���I�[�o�[�t���O
	playerhpmax = -1; //�v���C���[�̍ő�hp
	playerhp = -1; //�v���C���[�̌���hp
	knock = -1; //�Gn�Ƃ̂Ԃ���t���O
	playeratk = 0; //�v���C���[��ATK�B�v���C���[�����\���̂ɂ܂Ƃ߂�Ӗ��͂��邩
	playerdef = 0; //�v���C���[��DEf
	use_item_flag = -1; //n�Ԃ̃A�C�e���g�p�t���O�B�ԍ���0���珇�Ɏg�p����ۂɉ����A���t�@�x�b�g��
	get_item_flag = 0; //�A�C�e������t���O�B
	damage = -1; //�^���Ă���_���[�W
	aoe = 0; //���݂̌o���l
	Lv = 1; //���݂̃��x��
	floor_non_func = 1; //���݂̊K��
	floor_down_flag = 0; //�K�i���~���t���O
	invisible_time = 0; //�c�蓧������
	focussing_flag = 0; //�W���t���O
	getgold = 0; //�������t���O
	log_change_flag = 0; //���O��ύX�������̃t���O
	full_house_flag = 0; //�啔���t���O
	last_floor_flag = 0; //���������X�g
	game_clear_flag = 0;
	strcpy(playername, "");
	i = 0;
	while(i < LOG_LINES)
	{
		strcpy(log_keep[i], "");
		i++;
	}
	
	parentvarset(1,3,5,32,12,5,6, "ARMOR"); //1�_���[�W�����󂯂Ȃ����^�����Ȃ����x
	parentvarset(2,10,5,2,3,1,2, "BAT"); //Lv��2�グ���1���œ|������x
	parentvarset(3,26,14,5,13,5,7, "CHIMERA"); //����Ȋ���
	parentvarset(4,46,26,20,29,9,10, "DRAGON"); //����
	parentvarset(5,28,28,9,33,10,10, "EVIL MONSTER"); //�Η͐� 
	parentvarset(6,31,19,14,20,7,8, "FENRIR"); //����ł��U������
	parentvarset(7,100,33,18,1,11,11, "GUILTY REQUIEM");//������̃{�X
	parentvarset(8,12,9,3,5,1,3, "HOBGOBLIN"); //Lv1�̃v���C���[��2�_���[�W�^������x
	parentvarset(9,25,20,3,23,8,9, "INVISIBLE MAN"); //�����Ȃ�����ɃX�e�͒��
	parentvarset(11,32,24,18,28,9,10, "KING DAEMON"); //����
	parentvarset(13,4,20,3,15,6,8, "MURDER"); //�Η͂̂�
	parentvarset(15,21,13,9,11,3,5, "ORC"); //���Ղ̋��G
	parentvarset(16,34,16,13,21,7,8, "PEGASUS"); //����ł��h�䍂��
	parentvarset(18,16,11,3,10,4,6, "RABIT"); //�����Ȃ�
	parentvarset(19,20,5,4,4,1,2, "SLIME"); //�̗͂���������
	parentvarset(20,26,14,2,10,4,4, "TROLL"); //�̗�&�U��������
	parentvarset(21,38,12,16,25,8,10, "UNICORN"); //�኱�ア
	parentvarset(22,40,15,12,1,11,11, "VESTA"); //�ȒP�ȕ��̃{�X
	parentvarset(23,17,11,9,8,3,5, "WOOD MONSTER"); //��orc
	parentvarset(26,12,10,6,1,3,4, "ZOMBIE"); //�o���l�����Ȃ�
	//(�A���t�@�x�b�g�̔ԍ�,hp,atkdef,exp)
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
	system("cls"); //��ʂ����ׂăN���A����
	
	defPosition.X = 20; //�^�񒆕t�߂�
	defPosition.Y = 10;
	// COORD�\���̂Ŏw�肳�ꂽ���W�փJ�[�\�����ړ�������
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
	// COORD�\���̂Ŏw�肳�ꂽ���W�փJ�[�\�����ړ�������
	SetConsoleCursorPosition(hOut, defPosition);
	printf("�푰��I��ł�������\n");
	printf("1.HUMAN,2.ELF\n");
	while(1)
	{
		key_kind = getch();
		if(key_kind == 49 || key_kind == 50)
		{
			break;
		}
	}

	if(key_kind == 49) //�l��
	{
		playerhpmax = 18;
		playerhp = 14;
		playeratk = 10;
		playerdef = 7;
		player_kind = 1;
	}
	else //�G���t
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
	// COORD�\���̂Ŏw�肳�ꂽ���W�փJ�[�\�����ړ�������
	//SetConsoleCursorPosition(hOut, defPosition);
	printf("%d\n��Փx��I��ł�������\n", player_kind);
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
	if(key_kind == 49) //�ȒP
	{
		floor_max = 6;
	}
	else //����
	{
		floor_max = 11;
	}
}


int getcursor(void) //�ȂȂ߈ړ��ɂ��Ή����邽�߃e���L�[����O��
{
	char key = 0; //��������
	int i = 0; //�ꎞ�v�Z�p
	int x2 = 0; //�ꎞ�v�Z�p
	int y2 = 0; //�ꎞ�v�Z�p
	int walking = 0; //���������ǂ���

	while(!walking )
	{
		key = getch();	//�ŐV�ŁB�S���Z�߂�
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
		else //�A�C�e�������g�����ꍇ
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
	int i = 0; //�v�Z�p
	int i2 = 0;
	int flag = 0; //�t���O�p
	int flag2 = 0;
	int flag3 = 0;

	if(floor_non_func != floor_max)
	{
		srand((unsigned long)timeGetTime());
		switch(rand()%7) //�J���}�b�v�������_���Ɍ���
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
	else //�Ō�̊K�ł���Γ��ʂȕ�����
	{
		fp = fopen("./maps/map0.map", "r");
		last_floor_flag++;
	}
	if(fp == NULL)
	{
		printf("�t�@�C�����J���܂���B\n");
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
		while(1) //�X�^�[�g�ʒu�����߂�
		{
			p -> start_x = rand()%MAP_X + 1;
			p -> start_y = rand()%MAP_Y + 1;
			if(map[p -> start_y][p -> start_x] == 1)
				break;
		}
		playerx = p -> start_x;
		playery = p -> start_y;

		while(1) //�S�[���ʒu�����߂�
		{
			p -> goal_x = rand()%MAP_X + 1;
			p -> goal_y = rand()%MAP_Y + 1;
			if(
				map[p -> goal_y][p -> goal_x] == 1 && 
				(p -> goal_y != p -> start_y || p -> goal_x != p -> start_x)
				) //���ʂ̏��̏�ŁA�v���C���[�̏����ʒu�Ɣ��Ȃ�����
				break;
		}

		while(1) //����̌��A�ʒu�����߂�
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
					) //���ʂ̏��̏�ŁA�v���C���[�̏����ʒu��S�[���̈ʒu�Ɣ��Ȃ�����
				{
					gexist[i] = 1;
					i++;
				}
			}
			break;
		}

		while(1) //�����̌��A�ʒu�����߂�
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
					) //���ʂ̏��̏�ŁA�v���C���[�̏����ʒu��S�[���̈ʒu�Ɣ��Ȃ�����
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

		while(1) //����̌��A�ʒu�����߂�
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
					) //���ʂ̏��̏�ŁA�v���C���[�̏����ʒu��S�[���̈ʒu�Ɣ��Ȃ�����
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

		while(1) //�G�̏������A��ށA�ʒu�����߂�
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
					) //���ʂ̏��̏�ŁA�v���C���[�̏����ʒu��S�[���̈ʒu�Ɣ��Ȃ�����
				{
					i2 = 0;
						while(1)
						{
							enc[i] -> ekind = rand()%ENEMY_MAX + 1; 
							if(ed[enc[i] -> ekind] -> enemy_first_floor <= floor_non_func && ed[enc[i] -> ekind] -> enemy_last_floor >= floor_non_func ) //����̊K�����̃����X�^�[�̏o���\�͈͂ł��邩
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

		//���݂���G��hp����
		i = 0;
		while(enc[i] -> eexist)
		{
			enc[i] -> enemyhp = ed[enc[i] -> ekind] -> enemyhpmax;
			i++;
		}

		while(1) //㩂̌��A�ʒu�A��ނ����߂�
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
					) //���ʂ̏��̏�ŁA�v���C���[�̏����ʒu��S�[���̈ʒu�Ɣ��Ȃ�����
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

		/*while(1) //�����̌��A�ʒu�����߂�
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
		) //���ʂ̏��̏�ŁA�v���C���[�̏����ʒu��S�[���̈ʒu�Ɣ��Ȃ�����
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
		while(i2 < p -> pnumber) //�����Ɣ��Ȃ�����
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
	else //�Ō�̊K
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
		if(aoe >= (Lv + 2)*(Lv + 4)) //���x���A�b�v���̏���
		{
			aoe = aoe - (Lv + 2)*(Lv + 4);
			Lv++;
			if(player_kind == 1) //�l��
			{
			playeratk = playeratk + 2;
			playerdef++;
			playerhpmax = playerhpmax + 3;
			}
			else //�G���t
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
	map_open(); //��ɉ��͈͂���������
	non_first_keep = 0;

	//��ʂ�������
	//system("cls"); //��ʂ������
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
					printf("%d", map[y][x]); //�f�o�b�O����
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
	//��O�ɖ߂�
	defPosition.X = 0;
	defPosition.Y = 0;
	// COORD�\���̂Ŏw�肳�ꂽ���W�փJ�[�\�����ړ�������
	SetConsoleCursorPosition(hOut, defPosition);
	printf(keep_map);//;cache);
	/*delete[] cache;
	cache=new char[1];
	cache[0]='\0';*/

	//�K�i��\��
	if(map_open_flag[p -> goal_y][p -> goal_x] == 1 ||
		map_open_flag[p -> goal_y][p -> goal_x] == 2 ||
		full_house_flag)
	{
		defPosition.X = p -> goal_x;
		defPosition.Y = p -> goal_y;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("%%");
	}


	//㩂�\��
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

	//�����\��
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

	//������\��
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

	//�����\��
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

	//������\��
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

	if(last_floor_flag) //�S�[��
	{
		if(map_open_flag[1][29])
		{
			defPosition.X = 29;
			defPosition.Y = 1;
			SetConsoleCursorPosition(hOut, defPosition);
			printf("*");
		}
	}

	
	//�G��\��
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
    wAttributes = FOREGROUND_BLUE; //�F���w��
    SetConsoleTextAttribute(hStdout, wAttributes); //�F�ύX
					printf("G");
    wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; //�g���I�������͖߂��Ȃ��Ƃ����Ƃ��̂܂܁B����͔�
    SetConsoleTextAttribute(hStdout, wAttributes);
					break;
				case 8:
					printf("H");
					break;
				//case 9:
					//printf("I"); //�����ȓG
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
					
    wAttributes = FOREGROUND_RED; //�ԐF���w��
    SetConsoleTextAttribute(hStdout, wAttributes); //�F�ύX
					printf("V");
    wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; //�g���I�������͖߂��Ȃ��Ƃ����Ƃ��̂܂܁B����͔�
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

	//�v���C���[��\��
	defPosition.X = playerx;
	defPosition.Y = playery;
	SetConsoleCursorPosition(hOut, defPosition);
	printf("@");

	//��F��������
	defPosition.X = 0;
	defPosition.Y = MAP_Y; //�}�b�v�̂�������
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


void manage_flag(void) //�A�C�e�����E�������Ƃ�
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
			srand((unsigned)time(NULL)); //�����ݒ�
			getgold = (rand()%26 + 10) + floor_non_func + (last_floor_flag ? (floor_max == 6 ? 75 : 450) : 0); //�Ƃ肠����10~35,�[���قǑ����Ȃ�
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

			case 1: //�e���|�[�g
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
			case 2: //�_���[�W��
				playerhp = playerhp - 3;
				feet_trap_flag = 2;
				gameover = (playerhp <= 0);
				break;
			case 3: //���Ȃ�������
				hungry = (hungry > 50 ? hungry - 50 : 0);
				feet_trap_flag = 3;
				break;
			}
		}
		i++;
	}

	if(last_floor_flag)
	{
		if(playerx == 29 && playery == 1) //�S�[��
		{
			game_clear_flag++;
		}
	}
}


void player_information(void)
{

	//���̈ʒu�ɖ߂�
	defPosition.X = 0;
	defPosition.Y = MAP_Y; //�}�b�v�̂�������
	SetConsoleCursorPosition(hOut, defPosition);
	//�v���C���[���
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
		shift_log("     *�����̂Ȃ��ɂ���*");
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
				shift_log("�����ł͈Ӗ����Ȃ�������");
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
				shift_log("�����ł͈Ӗ����Ȃ�������");
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
				shift_log("�����ł͈Ӗ����Ȃ�������");
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
	case 13: //�K�i
		if(playerx == p -> goal_x && playery == p -> goal_y)
		{
			floor_non_func++;
			floor_down_flag++;
			focussing_flag = 0;
			full_house_flag = 0;
			bind_time = 0;
			shift_log("�K�i���~�肽");
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
	char keep2[60]; //�ϐ��𕶎���ɒǉ����邽�߂̈ꎞ�L���p
	int nagasa = 0; //������̒����ꎞ�L���p�B�ǂ����O���v�����Ȃ�����

	defPosition.X = MAP_X + 5;
	defPosition.Y = 0;
	SetConsoleCursorPosition(hOut, defPosition);

	if(get_item_flag)
	{
		strcpy(keep, get_item_name);
		strcat(keep, "���E�����B");
		shift_log(keep);
		get_item_flag = 0;
		strcpy(get_item_name,"");
	}
	if(getgold)
	{
		sprintf(keep, "%d", getgold); //���l���R�s�[����ۂɂ͂����炪�K�v
		strcat(keep, "gold���E�����B");
		shift_log(keep);
		getgold = 0;
	}

	if(use_item_flag + 1)
	{
		switch(use_item_flag)
		{
		case 2:
			shift_log("�G�𔛂�t����");
			break;
		case 6:
			shift_log("�W���͂�������");
			break;
		case 3:
			shift_log("���C���o�Ă���");
			break;
			//default: //�������ꂽ��
			//printf("���Ȃ��͂���������Ă��Ȃ�");
		case 8:
			shift_log("���_�n�[�h���b�N���Ăяo����");
			break;
		case 9:
			shift_log("�̂������ɂȂ���");
			break;
		case 7:
			shift_log("�n�`��c������");
			break;
		case 16:
			shift_log("�������͂̓G�ɍ~�蒍����");
			break;
		case 19:
			shift_log("�̂��M���Ȃ���");
			break;
		case 20:
			strcpy(keep, "(");
			sprintf(keep2, "%d", playerx);
			strcat(keep, keep2);
			strcat(keep, ",");
			sprintf(keep2, "%d", playery);
			strcat(keep, keep2);
			strcat(keep, ")�Ɉړ�����");
			shift_log(keep);
			break;
		}
	}

	if(feet_trap_flag)
	{
		switch(feet_trap_flag)
		{
		case 1:
			shift_log("�����͉�����!");
			break;
		case 2:
			shift_log("���Ă�!");
			break;
		case 3:
			shift_log("�ǂ�����������");
			break;
		}
		feet_trap_flag = 0;
	}

	if(knock + 1)
	{
		strcpy(keep, ename[enc[knock] -> ekind]);
		strcat(keep, "��");
		sprintf(keep2, "%d", damage);
		strcat(keep, keep2);
		strcat(keep, "�̃_���[�W��^����");
		shift_log(keep);
		if(!enc[knock] -> eexist)
		{
			strcpy(keep, ename[enc[knock] -> ekind]);
			strcat(keep, "��|�����Bexp+");
			sprintf(keep2, "%d", ed[enc[knock] -> ekind] -> enemyexp);
			strcat(keep, keep2);
			if(aoe < ed[enc[knock] -> ekind] -> enemyexp)
			{
				strcat(keep, "�B");
				sprintf(keep2, "%d", Lv);
				strcat(keep, keep2);
				strcat(keep, "Lv�Ƀ��x���A�b�v");
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
			strcat(keep, "����");
			sprintf(keep2, "%d", enc[i] -> damaged);
			strcat(keep, keep2);
			strcat(keep, "�̃_���[�W���󂯂�");
			shift_log(keep);
		}
		i++;
	}
	use_item_flag = -1;

	if(playerhp <= 0)
	{
		gameover++;
	}


	//�����������������B�����������@�͂Ȃ����̂�
	//if(log_change_flag)
	//{
		//i = 0;
	//	i2 = 0;

		//strcpy(keep, keep2);
		/*while(i < LOG_LINES)
		{
		//�����p�̈ʒu�Ɉړ�
		defPosition.X = MAP_X + 5;
		defPosition.Y = i;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("                                                           "); //59��
		i++;
		}*/
	//}
	//���O�̕`��
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
	//���O�̕`��
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
				if(!invisible_time) //�v���C���[�̓����t���OOFF
				{
					i2 = 0;
					while(i2 < 3) //����8�}�X�Ƀv���C���[�����邩
					{
						eny = enc[i] -> eplacey + (i2 - 1);
						i3 = 0;
						while(i3 < 3)
						{
							enx = enc[i] -> eplacex + (i3 - 1);
							if(eny == playery && enx == playerx) //�ʒu����v�����ꍇ
							{
								enc[i] -> damaged = ed[enc[i] -> ekind] -> enemyatk - playerdef;
								enc[i] -> damaged = (enc[i] -> damaged > 0 ? enc[i] -> damaged : 1);
								playerhp = playerhp - enc[i] -> damaged;
								gameover = (playerhp <= 0);
								enc[i] -> knocked = 1; //�Ԃ����t���O
								goto loop_out;
							}
							i3++;
						}
						i2++;
					}

					if(!bind_time)
					{
					y = (playery / (MAP_Y / MAP_CUT_Y)); //�u���b�N�̂ǂ��ɂ��邩
					x = (playerx / (MAP_X / MAP_CUT_X));	
					ey = (enc[i] -> eplacey / (MAP_Y / MAP_CUT_Y)); //�u���b�N�̂ǂ��ɂ��邩
					ex = (enc[i] -> eplacex / (MAP_X / MAP_CUT_X));
					if(enc[i] -> ekind != 2) //BAT�͂ӂ�ӂ瓮��
					{
						if(y == ey && x == ex || full_house_flag) //�v���C���[�Ɠ����u���b�N�ɂ���ꍇ
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
									while(i4 < ENEMY_MAX) //���̓G�ƈʒu������ĂȂ���
									{
										if(eny2 == enc[i4] -> eplacey && enx2 == enc[i4] -> eplacex)
										{
											flag++;
											break;
										}
										i4++;
									}
									if(!(map[eny2][enx2] != 1 && map[eny2][enx2] != 3 && map[eny2][enx2] != 5)
										&& !(eny2 == playery && enx2 == playerx)) //���̏�Ńv���C���[�Ɣ���Ă��Ȃ���
									{
										pdistance2 = (playerx - enx2) * (playerx - enx2) + (playery - eny2) * (playery - eny2);
										if(!flag &&(pdistance > pdistance2 || !pdistance)) //���������߂�����I��
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
				while(n < 100) //100�񃋁[�v���Ă��ʖڂȂ瓮���Ȃ�
				{
					n++;
					millitime = (unsigned long)timeGetTime() + n; //���ʂɃ~���b���擾������͂�������
					srand(millitime);

					eny = enc[i] -> eplacey + (rand()%3 - 1);
					enx = enc[i] -> eplacex + (rand()%3 - 1);
					if((map[eny][enx] == 1 || map[eny][enx] == 3 || map[eny][enx] == 5) && !(eny == playery && enx == playerx)) //���������Ńv���C���[�ɔ���ĂȂ���
					{
						i2 = 0;
						flag = 0;
						while(i2 < ENEMY_MAX) //���̓G�ƈʒu������ĂȂ���
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
						if(!flag) //��肪�Ȃ���Έړ�
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
	else //�Ō�̊K
	{
		enc[0] -> knocked = 0;
		enc[0] -> damaged = -1;
		if(enc[0] -> eexist)
		{
			if(!invisible_time) //�v���C���[�̓����t���OOFF
			{
				i2 = 0;
				while(i2 < 5) //����25�}�X�Ƀv���C���[�����邩
				{
					eny = enc[0] -> eplacey + (i2 - 1);
					i3 = 0;
					while(i3 < 5)
					{
						enx = enc[0] -> eplacex + (i3 - 1);
						if(eny == playery && enx == playerx) //�ʒu����v�����ꍇ
						{
							enc[0] -> damaged = ed[enc[0] -> ekind] -> enemyatk - playerdef;
							enc[0] -> damaged = (enc[0] -> damaged > 0 ? enc[0] -> damaged : 1);
							playerhp = playerhp - enc[0] -> damaged;
							gameover = (playerhp <= 0);
							enc[0] -> knocked = 1; //�Ԃ����t���O
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

	if(!last_floor_flag) //�Ō�̊K�łȂ�����
	{
		srand((unsigned long)timeGetTime());
		if(!(rand()%BORN_SPEED))
		{
			while(i < ENEMY_MAX)//�ی�
			{
				if(!(enc[i] -> eexist))
				{
					while(1)
					{
						n++;
						millitime = (unsigned long)timeGetTime() + n; //���ʂɃ~���b���擾������͂�������
						srand(millitime);
						enc[i] -> eplacex = rand()%MAP_X + 1;
						enc[i] -> eplacey = rand()%MAP_Y + 1;
						if(  map[enc[i] -> eplacey][enc[i] -> eplacex] == 1 && 
							(enc[i] -> eplacey != playery || enc[i] -> eplacex != playerx)
							) //���ʂ̏��̏�ŁA�v���C���[�̈ʒu�Ɣ��Ȃ�����
						{
							flag = 0;
							y = (playery / (MAP_Y / MAP_CUT_Y)); //�u���b�N�̂ǂ��ɂ��邩
							x = (playerx / (MAP_X / MAP_CUT_X));
							ey = (enc[i] -> eplacey / (MAP_Y / MAP_CUT_Y)); //�u���b�N�̂ǂ��ɂ��邩
							ex = (enc[i] -> eplacex / (MAP_X / MAP_CUT_X)); 
							if(ey == y && ex == x) //�v���C���[�̋߂��łȂ�����
							{
								flag++;
							}
							if(!flag || full_house_flag || !(rand()%99)) //�ی�
							{
								while(1)
								{
									enc[i] -> ekind = rand()%ENEMY_MAX + 1; 
									if(ed[enc[i] -> ekind] -> enemy_first_floor <= floor_non_func && ed[enc[i] -> ekind] -> enemy_last_floor >= floor_non_func ) //����̊K�����̃����X�^�[�̏o���\�͈͂ł��邩
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
		while(y < MAP_Y) //�������痣�ꂽ�猩���Ȃ��Ȃ鏈��
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

		if(map[playery][playerx] == 3) //�ʘH��1�}�X��܂Ō�����
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
		else if(map[playery][playerx] == 1 || map[playery][playerx] == 5) //�����̒��������͔�
		{
			i = 0;
			i2 = 0;
			y = (playery / (MAP_Y / MAP_CUT_Y)); //�u���b�N�̂ǂ��ɂ��邩
			x = (playerx / (MAP_X / MAP_CUT_X));
			while(i < (MAP_Y / MAP_CUT_Y))
			{
				while(i2 < (MAP_X / MAP_CUT_X))
				{
					if(map[(MAP_Y / MAP_CUT_Y) * y + i][(MAP_X / MAP_CUT_X) * x + i2] != 3) //�������ʘH�łȂ�
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
	else //	�Ō�̊K
	{
		if(playerx == 30 && playery == 28) //�ŏ��̈ʒu
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
		if(map[playery][playerx] == 3 || map[playery][playerx] == 5) //�ʘH��1�}�X��܂Ō�����
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
		if((playerx == 30 || playerx == 29) && playery == 14) //�G�̂��镔���ɓ��ݍ���
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

		if(((playerx == 30 || playerx == 29) && playery == 4) || //�K�i�̂��镔���ɓ��ݍ���
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
		printf("�M���͖����A�����������{����E�o���邱�Ƃ��ł����B");
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

		int point = 0; //�\���̂̒��g
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
			printf("�t�@�C���̓ǂݍ��݂Ɏ��s���܂���");
			exit(1);
		}
		
		fprintf(gameload, "%s %d\n", playername, numgold); //�܂����ʂ���������

		
		fclose(gameload);
		gameload = fopen("./clear_log.txt", "a+");
		if(fp == NULL)
		{
			printf("�t�@�C���̓ǂݍ��݂Ɏ��s���܂���");
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
			printf("�t�@�C���̓ǂݍ��݂Ɏ��s���܂���");
			exit(1);
		}

		clog* clear_log_point;
		clear_log_point = new clog[cnt];
		i = 0;
		while(i < cnt)
		{
			fscanf(gameload, "%s%d", clear_log_point[i].pname_clog, &(clear_log_point[i].point)); //�X�R�A��ǂݍ���
			i++;
		}



		defPosition.Y = 25;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("                                 ");

		defPosition.Y = 30;
		SetConsoleCursorPosition(hOut, defPosition);
		printf("%s's score is %d\n", playername, numgold);
		
		i = 0;
		while(i < cnt) //���ʂ̃\�[�g
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