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

	void MoveReset();
	void MoveTankUp();
	void MoveTankDown();
	void MoveTankLeft();
	void MoveTankRight();

	void MoveTankUpPixel(float pixel = 1.0f);
	void MoveTankDownPixel(float pixel = 1.0f);
	void MoveTankLeftPixel(float pixel = 1.0f);
	void MoveTankRightPixel(float pixel = 1.0f);

	// 变量定义---------------------------------------------------
	bool				isEnemy;						// 1为敌人，0为玩家
	int					collide_with_other_tank;		// 0为未发生碰撞，1为上方碰撞，2为左方碰撞，3为右方碰撞，4为下方碰撞
	int					direction;						// 0为向上，1为向左，2为向右，3为向下；其他参数默认为向右
	int					appear_time;
	int					health;
	int					stuck_frame;
	int					alive_frame;
	D2D1_RECT_F			position;
	std::vector<Bullet> bullet;

	bool MoveUp;
	bool MoveDown;
	bool MoveLeft;
	bool MoveRight;
private:
};

class Bullet {
public:
	Bullet();
	Bullet(Tank tank, int bullet_level = 1, int bullet_speed = 7);

	bool Move();


	D2D1_RECT_F position;
	int direction;	//0为向上，1为向左，2为向右，3为向下；其他参数默认为向上
private:
	int bullet_speed;
	int level;
};