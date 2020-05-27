#pragma once

// 如果你需要在以下指定的平台之前定位平台，请修改以下定义。
// 参考MSDN上最新的平台的相应的值
#ifndef WINVER              // 适用于Win7及Win7以上版本
#define WINVER 0x0700       // 修改这个值来确定不同的windows版本
#endif

#ifndef _WIN32_WINNT        // 适用于Win7及Win7以上版本
#define _WIN32_WINNT 0x0700 // 修改这个值来确定不同的windows版本
#endif

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN     // 从windows头文件中去除很少使用的内容

// windows头文件
#include <windows.h>

// c运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

// Direct2D需要的头文件
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <dwmapi.h>

#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "DWrite.lib")

/******************************************************************
需要用到的宏定义
******************************************************************/

template<class Interface>
inline void
SafeRelease(
    Interface **ppInterfaceToRelease
    )
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif


#include <math.h>
#include "BattleTank.h"

/******************************************************************
Game
******************************************************************/
class Game
{
public:
	Game();
    ~Game();

    HRESULT Initialize();

    void RunMessageLoop();
	
private:
	// D2D框架相关----------------------------------------------------
    HRESULT CreateDeviceIndependentResources();
    HRESULT CreateDeviceResources();
    void DiscardDeviceResources();

    HRESULT OnRender();

    void OnResize(
        UINT width,
        UINT height
        );

    void OnTimer();

    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
        );

	// 游戏实现相关----------------------------------------------------
	void LoadMap(const char* filepath, int map[28][28]);
	void LoadEnemy(const char* filepath);
	void PrintSideScreen();

	void SaveTankPos();
	void EnemyTank();
	bool MapCollisionDetect(Tank tank, const char direction);

	void GameMenu();
	void NextLevel();
	void StageChange();
private:
    HWND m_hwnd;

	// 用到的资源-----------------------------------------------------
	ID2D1Factory*			m_pD2DFactory;		// D2D工厂
	IDWriteFactory*			m_pDWriteFactory;	// DWrite工厂
	IWICImagingFactory*		m_pWICFactory;		// WIC工厂
	ID2D1HwndRenderTarget*	m_pRT;				// 呈现器
	ID2D1SolidColorBrush *	m_pPainter_CornflowerBlue;	// 矢车菊蓝画刷
	ID2D1SolidColorBrush *	m_pPainter_White;	// 白色画刷
	IDWriteTextFormat*		m_pTextFormat;		// 格式1

	long					run_time;			//记录运行时间
	unsigned int			menu_frame_count;
	unsigned int			stage_change_frame_count;
	unsigned int			frame_count;
	unsigned int			frame_count_2;

	// 位图-----------------------------------------------------------		
	ID2D1Bitmap*				m_pGametitle;
	ID2D1Bitmap*				m_pSelecttank;
	ID2D1Bitmap*				m_pStagecurtain;

	ID2D1Bitmap*				m_pBase;
	ID2D1Bitmap*				m_pWall;
	ID2D1Bitmap*				m_pSteel;
	ID2D1Bitmap*				m_pGrass;
	ID2D1Bitmap*				m_pWater;
	ID2D1Bitmap*				m_pBullet;

	std::vector<ID2D1Bitmap*>	m_pPlayer_1;	// 0, 1, 2, 3 —— U, D, L, R
	std::vector<ID2D1Bitmap*>	m_pPlayer_2;	// 0, 1, 2, 3 —— U, D, L, R

	std::vector<ID2D1Bitmap*>	m_pEnemy_light_tank;
	std::vector<ID2D1Bitmap*>	m_pEnemy_normal_tank;
	std::vector<ID2D1Bitmap*>	m_pEnemy_medium_tank;
	std::vector<ID2D1Bitmap*>	m_pEnemy_heavy_tank;
	std::vector<ID2D1Bitmap*>	m_pEnemy_super_tank;
	std::vector<ID2D1Bitmap*>	m_pEnemy_gigantic_tank;

	// 游戏需要的资源-------------------------------------------------

	//WHICH_PRESSED编号: WSAD J K 0-5, UDLR RCTRL RSHIFT 6-11
	bool						WHICH_PRESSED[12] = { 0 };		// 实现每x(x为自变量)秒响应一次键盘输入
	char						PROCESS_WHICH_PRESSED = 0;			// WHICHPRESSED辅助变量
	int							current_total_enemy = 0;
	int							player_1_life;
	Tank						tank_player_1;

	D2D1_RECT_F					game_menu;
	D2D1_RECT_F					select_tank;
	D2D1_RECT_F					stage_curtain_1;
	D2D1_RECT_F					stage_curtain_2;
	std::vector<Bullet>			p1_bullet;
	std::vector<Tank>			enemy_light_tank;
	std::vector<Tank>			enemy_normal_tank;
	std::vector<Tank>			enemy_medium_tank;
	std::vector<Tank>			enemy_heavy_tank;
	std::vector<Tank>			enemy_super_tank;
	std::vector<Tank>			enemy_gigantic_tank;
	std::vector<D2D1_RECT_F>	respawn_position;
};


