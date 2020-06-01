#pragma once
#include <d2d1.h>
#include <windows.h>
#include <vector>
#define BLOCK 30.0F
#define MOVEMENT_SPEED 5

extern int current_map[28][28];
extern bool gameover;

class Keyboard {
public:
	Keyboard();

	//Player_1 Keyboard input
	void OnWPressed();
	void OnSPressed();
	void OnAPressed();
	void OnDPressed();
	void OnJPressed();
	void OnKPressed();

	//Player_2 Keyboard input
	void OnUpPressed();
	void OnDownPressed();
	void OnLeftPressed();
	void OnRightPressed();
	void OnNumpad0Pressed();
	void OnRightCtrlPressed();
	void OnRightShiftPressed();

	//Global input
	void OnSpacePressed();
	void OnEnterPressed();
	void OnEscPressed();

	//Keyboard Release
	void OnWReleased();
	void OnSReleased();
	void OnAReleased();
	void OnDReleased();
	void OnJReleased();
	void OnKReleased();

	void OnUpReleased();
	void OnDownReleased();
	void OnLeftReleased();
	void OnRightReleased();
	void OnNumpad0Released();
	void OnRightCtrlReleased();
	void OnRightShiftReleased();

	void OnSpaceReleased();
	void OnEnterReleased();
	void OnEscReleased();


	//Get keyboard input status
	bool IsWPressed() const;
	bool IsSPressed() const;
	bool IsAPressed() const;
	bool IsDPressed() const;
	bool IsJPressed() const;
	bool IsKPressed() const;

	bool IsUpPressed() const;
	bool IsDownPressed() const;
	bool IsLeftPressed() const;
	bool IsRightPressed() const;
	bool IsNumpad0Pressed() const;
	bool IsRightCtrlPressed() const;
	bool IsRightShiftPressed() const;

	bool IsSpacePressed() const;
	bool IsEnterPressed() const;
	bool IsEscPressed() const;

//private:
	bool wIsPressed;
	bool sIsPressed;
	bool aIsPressed;
	bool dIsPressed;
	bool jIsPressed;
	bool kIsPressed;

	bool upIsPressed;
	bool downIsPressed;
	bool leftIsPressed;
	bool rightIsPressed;
	bool numpad0IsPressed;
	bool rctrlIsPressed;
	bool rshiftIsPressed;

	bool spaceIsPressed;
	bool enterIsPressed;
	bool escIsPressed;

	// Release相关
	bool wIsReleased;
	bool sIsReleased;
	bool aIsReleased;
	bool dIsReleased;
	bool jIsReleased;
	bool kIsReleased;

	bool upIsReleased;
	bool downIsReleased;
	bool leftIsReleased;
	bool rightIsReleased;
	bool numpad0IsReleased;
	bool rctrlIsReleased;
	bool rshiftIsReleased;

	bool spaceIsReleased;
	bool enterIsReleased;
	bool escIsReleased;
};

class Bullet;

class Tank {
public:
	//函数定义---------------------------------------------------
	Tank();			//默认构造函数创建敌人
	Tank(bool isEnemy, D2D1_RECT_F pos, int _health = 1);

	friend class Game;
	void MoveTankUpPixel(float pixel = 1.0f);			// 坦克向上移动pixel个像素
	void MoveTankDownPixel(float pixel = 1.0f);			// 坦克向下移动pixel个像素
	void MoveTankLeftPixel(float pixel = 1.0f);			// 坦克向左移动pixel个像素
	void MoveTankRightPixel(float pixel = 1.0f);		// 坦克向右移动pixel个像素

	int GetDirection() const;
	D2D1_RECT_F GetPosition() const;

private:
	// 变量定义---------------------------------------------------
	bool				isEnemy;						// 1为敌人，0为玩家
	int					collide_with_other_tank;		// 0为未发生碰撞，1为上方碰撞，2为左方碰撞，3为右方碰撞，4为下方碰撞
	int					direction;						// 0为向上，1为向左，2为向右，3为向下；其他参数默认为向右
	int					appear_time;					// 敌方坦克在第几秒出现
	int					health;							// 坦克生命值
	int					stuck_frame;					// 坦克卡住不动的帧数，用于判断是否要转向
	int					alive_frame;					// 坦克存活了多少帧
	D2D1_RECT_F			position;						// 保存坦克位置（是个矩形）
	std::vector<Bullet> bullet;							// 坦克的所有子弹
	std::vector<ID2D1Bitmap*>	m_pTexture;				// 存储坦克4个方向的位图

	bool MoveUp;
	bool MoveDown;
	bool MoveLeft;
	bool MoveRight;
};

class Bullet {
public:
	Bullet();				// 实际不会用到的默认构造函数
	Bullet(Tank tank, int bullet_level = 1, int bullet_speed = 7);

	bool Move();			// 子弹只会沿当前方向飞行所以一个函数就可以实现移动了

	friend class Game;

private:
	int bullet_speed;		// 子弹每帧移动多少像素
	int level;				// 子弹等级（等级高了可以打掉钢墙）
	D2D1_RECT_F position;	// 保存子弹当前位置（是个矩形）
	int direction;			// 0为向上，1为向左，2为向右，3为向下；其他参数默认为向上
};
