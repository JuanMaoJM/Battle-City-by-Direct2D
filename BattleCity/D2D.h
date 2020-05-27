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

// Direct2D��Ҫ��ͷ�ļ�
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <dwmapi.h>

#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "DWrite.lib")

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

	void SaveTankPos();
	void EnemyTank();
	bool MapCollisionDetect(Tank tank, const char direction);

	void GameMenu();
	void NextLevel();
	void StageChange();
private:
    HWND m_hwnd;

	// �õ�����Դ-----------------------------------------------------
	ID2D1Factory*			m_pD2DFactory;		// D2D����
	IDWriteFactory*			m_pDWriteFactory;	// DWrite����
	IWICImagingFactory*		m_pWICFactory;		// WIC����
	ID2D1HwndRenderTarget*	m_pRT;				// ������
	ID2D1SolidColorBrush *	m_pPainter_CornflowerBlue;	// ʸ��������ˢ
	ID2D1SolidColorBrush *	m_pPainter_White;	// ��ɫ��ˢ
	IDWriteTextFormat*		m_pTextFormat;		// ��ʽ1

	long					run_time;			//��¼����ʱ��
	unsigned int			menu_frame_count;
	unsigned int			stage_change_frame_count;
	unsigned int			frame_count;
	unsigned int			frame_count_2;

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

	std::vector<ID2D1Bitmap*>	m_pPlayer_1;	// 0, 1, 2, 3 ���� U, D, L, R
	std::vector<ID2D1Bitmap*>	m_pPlayer_2;	// 0, 1, 2, 3 ���� U, D, L, R

	std::vector<ID2D1Bitmap*>	m_pEnemy_light_tank;
	std::vector<ID2D1Bitmap*>	m_pEnemy_normal_tank;
	std::vector<ID2D1Bitmap*>	m_pEnemy_medium_tank;
	std::vector<ID2D1Bitmap*>	m_pEnemy_heavy_tank;
	std::vector<ID2D1Bitmap*>	m_pEnemy_super_tank;
	std::vector<ID2D1Bitmap*>	m_pEnemy_gigantic_tank;

	// ��Ϸ��Ҫ����Դ-------------------------------------------------

	//WHICH_PRESSED���: WSAD J K 0-5, UDLR RCTRL RSHIFT 6-11
	bool						WHICH_PRESSED[12] = { 0 };		// ʵ��ÿx(xΪ�Ա���)����Ӧһ�μ�������
	char						PROCESS_WHICH_PRESSED = 0;			// WHICHPRESSED��������
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


