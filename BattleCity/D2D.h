#pragma once

// �������Ҫ������ָ����ƽ̨֮ǰ��λƽ̨�����޸����¶��塣
// �ο�MSDN�����µ�ƽ̨����Ӧ��ֵ
#ifndef WINVER              // ������Win7��Win7���ϰ汾
#define WINVER 0x0700       // �޸����ֵ��ȷ����ͬ��windows�汾
#endif

#ifndef _WIN32_WINNT        // ������Win7��Win7���ϰ汾
#define _WIN32_WINNT 0x0700 // �޸����ֵ��ȷ����ͬ��windows�汾
#endif

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN     // ��windowsͷ�ļ���ȥ������ʹ�õ�����

// windowsͷ�ļ�
#include <windows.h>

// c����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include <mmsystem.h>

// Direct2D��Ҫ��ͷ�ļ�
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <dwmapi.h>

#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "DWrite.lib")
#pragma comment(lib, "Winmm.lib")

/******************************************************************
��Ҫ�õ��ĺ궨��
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
	// D2D������----------------------------------------------------
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

	// ��Ϸʵ�����----------------------------------------------------
	void LoadMap(const char* filepath, int map[28][28]);
	void LoadEnemy(const char* filepath);
	void PrintSideScreen();

	void KeyboardMoveOper();
	void KeyboardShootOper();
	void SaveTankPos();
	void EnemyTank();
	void EnemyAI(std::vector<Tank> & enemy_tank);
	void DrawEnemyTankWhenTimeStopped(std::vector<Tank> & enemy_tank);
	void Gadgets();
	void Bomb_eaten(std::vector<Tank> & enemy_tank);
	bool MapCollisionDetect(Tank tank, const char direction);

	void GameMenu();
	void StageChange();
	void NextStage();
private:
    HWND m_hwnd;

	// �õ�����Դ-----------------------------------------------------
	ID2D1Factory*			m_pD2DFactory;				// D2D����
	IDWriteFactory*			m_pDWriteFactory;			// DWrite����
	IWICImagingFactory*		m_pWICFactory;				// WIC����
	ID2D1HwndRenderTarget*	m_pRT;						// ������
	ID2D1SolidColorBrush *	m_pPainter_CornflowerBlue;	// ʸ��������ˢ
	ID2D1SolidColorBrush *	m_pPainter_White;			// ��ɫ��ˢ
	ID2D1SolidColorBrush *	m_pPainter_Black;			// ��ɫ��ˢ
	IDWriteTextFormat*		m_pTextFormat;				// ����Ļ������
	IDWriteTextFormat*		m_pTextFormat_Stage;		// �ؿ���ʾ������
	IDWriteTextFormat*		m_pTextFormat_Gameover;		// Game Over������

	long					run_time;			//��¼����ʱ��
	long					time_stop_run_time = 0;
	int						menu_frame_count;
	unsigned int			stage_change_frame_count;
	unsigned int			frame_count;				// ��¼��Ϸ����֡��
	unsigned int			frame_count_player_1;		// ��¼P1�ƶ�֡��
	unsigned int			frame_count_player_2;		// ��¼P2�ƶ�֡��
	unsigned int			frame_count_player_1_shoot;	// ��¼P1�ӵ�����֡��
	unsigned int			frame_count_player_2_shoot;	// ��¼P2�ӵ�����֡��

	// λͼ-----------------------------------------------------------		
	ID2D1Bitmap*				m_pGametitle;
	ID2D1Bitmap*				m_pSelecttank;
	ID2D1Bitmap*				m_pStagecurtain;

	ID2D1Bitmap*				m_pBase;
	ID2D1Bitmap*				m_pWall;
	ID2D1Bitmap*				m_pSteel;
	ID2D1Bitmap*				m_pGrass;
	ID2D1Bitmap*				m_pWater;
	ID2D1Bitmap*				m_pBullet;

	ID2D1Bitmap*				m_pStar;
	ID2D1Bitmap*				m_pBomb;
	ID2D1Bitmap*				m_pTheWorld;

	std::vector<ID2D1Bitmap*>	m_pPlayer_1;	// 0, 1, 2, 3 ���� U, D, L, R
	std::vector<ID2D1Bitmap*>	m_pPlayer_2;	// 0, 1, 2, 3 ���� U, D, L, R

	// ��Ϸ��Ҫ����Դ-------------------------------------------------

	bool						time_stop = false;
	//WHICH_PRESSED���: WSAD J K 0-5, UDLR RCTRL RSHIFT 6-11
	bool						WHICH_PRESSED[12] = { 0 };			// ʵ��ÿx֡��Ӧһ�μ�������
	char						PROCESS_WHICH_PRESSED = 0;			// WHICHPRESSED��������
	char						PROCESS_WHICH_PRESSED_P2 = 0;		// WHICHPRESSED Player_2��������
	int							high_score = 2000;
	int							current_total_enemy = 0;
	int							player_1_life;
	int							player_2_life;
	Tank						tank_player_1;
	Tank						tank_player_2;

	D2D1_RECT_F					game_menu;
	D2D1_RECT_F					select_tank;
	D2D1_RECT_F					stage_curtain_1;
	D2D1_RECT_F					stage_curtain_2;

	std::vector<D2D1_RECT_F>	star_pos;
	std::vector<D2D1_RECT_F>	bomb_pos;
	std::vector<D2D1_RECT_F>	TheWorld_pos;

	std::vector<Bullet>			p1_bullet;
	std::vector<Bullet>			p2_bullet;
	std::vector<Tank>			enemy_light_tank;
	std::vector<Tank>			enemy_normal_tank;
	std::vector<Tank>			enemy_medium_tank;
	std::vector<Tank>			enemy_heavy_tank;
	std::vector<Tank>			enemy_super_tank;
	std::vector<Tank>			enemy_gigantic_tank;
	std::vector<D2D1_RECT_F>	respawn_position;
};


