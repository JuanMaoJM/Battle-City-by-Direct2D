//-----------------------------------------------------------------
// 
//
//
//-----------------------------------------------------------------

#include "D2D.h"
#include "BattleTank.h"
#define MAX_LEVEL 5

static Keyboard	kbd;				// 存储键盘输入状态
bool gameover = false;
int	current_map[28][28] = { 0 };	// 存储当前地图(map里的值: 0为可通过陆地，1为红砖，2钢墙，3为水，4为树荫，5为地图边界，100为己方坦克，200为敌方坦克，9为Base)
int current_map_grass[28][28] = { 0 };	//专门用来存储grass(避免因为碰撞判定使得grass消失)
int game_status = 0;				// 0为开始菜单，1为单人游戏，2为多人游戏，3为地图编辑器
int game_status_old = 0;				
char Level[] = "Resources\\maps\\Level_01.dat";
char Level_Enemy[] = "Resources\\maps\\Level_01_Enemy.dat";

//-----------------------------------------------------------------
// 将string转换为wstring
//-----------------------------------------------------------------
std::wstring string_wstring(std::string sToMatch)
{
	int iWLen = MultiByteToWideChar(CP_ACP, 0, sToMatch.c_str(), sToMatch.size(), 0, 0); // 计算转换后宽字符串的长度。（不包含字符串结束符）
	wchar_t *lpwsz = new wchar_t[iWLen + 1];
	MultiByteToWideChar(CP_ACP, 0, sToMatch.c_str(), sToMatch.size(), lpwsz, iWLen); // 正式转换。
	lpwsz[iWLen] = L'\0';
	std::wstring wsToMatch(lpwsz);
	delete[]lpwsz;
	return wsToMatch;

}

//-----------------------------------------------------------------
// 判断两D2D1_RECT_F是否相交，true为相交，false为不相交
//-----------------------------------------------------------------
bool D2D1_RECT_F_Intersaction(D2D1_RECT_F r1, D2D1_RECT_F r2)
{
	return !((r1.right < r2.left || r1.left > r2.right) || (r1.bottom < r2.top || r1.top> r2.bottom));
}

//-----------------------------------------------------------------
// 从文件加载D2D位图
//-----------------------------------------------------------------
HRESULT LoadBitmapFromFile(
	ID2D1RenderTarget *pRenderTarget,
	IWICImagingFactory *pIWICFactory,
	LPCWSTR uri,
	UINT width,
	UINT height,
	ID2D1Bitmap **ppBitmap)
{
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		if (width != 0 || height != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (width == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(height) / static_cast<FLOAT>(originalHeight);
					width = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (height == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(width) / static_cast<FLOAT>(originalWidth);
					height = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						width,
						height,
						WICBitmapInterpolationModeCubic
					);
				}
				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pScaler,
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						NULL,
						0.f,
						WICBitmapPaletteTypeMedianCut
					);
				}
			}
		}
		else // Don't scale the image.
		{
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
			);
		}
	}
	if (SUCCEEDED(hr))
	{

		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
		);
	}

	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pStream);
	SafeRelease(&pConverter);
	SafeRelease(&pScaler);

	return hr;
}




/******************************************************************
程序入口
******************************************************************/
int WINAPI WinMain(
	HINSTANCE	/* hInstance */,
	HINSTANCE	/* hPrevInstance */,
	LPSTR		/* lpCmdLine */,
	int			/* nCmdShow */)
{
	// Ignoring the return value because we want to continue running even in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	srand((unsigned)time(NULL));
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			Game app;

			if (SUCCEEDED(app.Initialize()))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}

	return 0;
}

/******************************************************************
Game实现
******************************************************************/

Game::Game() :
	m_hwnd(NULL),
	m_pD2DFactory(NULL),
	m_pDWriteFactory(NULL),
	m_pRT(NULL),
	m_pWICFactory(NULL),
	m_pPainter_CornflowerBlue(NULL),
	m_pPainter_White(NULL),
	m_pPlayer_1(4, NULL),
	m_pPlayer_2(4, NULL),
	m_pEnemy_light_tank(4, NULL),
	m_pEnemy_normal_tank(4, NULL),
	m_pEnemy_medium_tank(4, NULL),
	m_pEnemy_heavy_tank(4, NULL),
	m_pEnemy_super_tank(4, NULL),
	m_pEnemy_gigantic_tank(4, NULL),
	run_time(0),
	stage_change_frame_count(0),
	menu_frame_count(0),
	frame_count(0),
	frame_count_2(0),
	p1_bullet(1),
	player_1_life(4),
	respawn_position(0),
	enemy_light_tank(1),
	enemy_normal_tank(1),
	enemy_medium_tank(1),
	enemy_heavy_tank(1),
	enemy_super_tank(1),
	enemy_gigantic_tank(1),
	tank_player_1(0, D2D1::RectF(BLOCK * 9, BLOCK * 25,
		30 + BLOCK * 10,
		30 + BLOCK * 26))					// 初始化玩家坦克位置: 初始位置为9，25 - 10,26	
{
	respawn_position.push_back(D2D1::RectF(30.0F, 30.0F, 90.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(90.0F, 30.0F, 150.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(150.0F, 30.0F, 210.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(210.0F, 30.0F, 270.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(270.0F, 30.0F, 330.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(330.0F, 30.0F, 390.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(390.0F, 30.0F, 450.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(450.0F, 30.0F, 510.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(510.0F, 30.0F, 570.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(570.0F, 30.0F, 630.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(630.0F, 30.0F, 690.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(690.0F, 30.0F, 750.0F, 90.0F));
	respawn_position.push_back(D2D1::RectF(750.0F, 30.0F, 810.0F, 90.0F));
}

Game::~Game()
{
	SafeRelease(&m_pWICFactory);
	SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pRT);
	SafeRelease(&m_pPainter_CornflowerBlue);
	SafeRelease(&m_pPainter_White);
	SafeRelease(&m_pTextFormat);
}

HRESULT Game::Initialize()
{
	HRESULT hr;

	//register window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Game::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"D2DGame";

	RegisterClassEx(&wcex);

	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		// Create the application window.

		FLOAT dpiX, dpiY;
		m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

		m_hwnd = CreateWindow(
			L"D2DGame",
			L"BattleCity -by NathanWong",
			WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,	// 把最大化按钮扣了
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<UINT>(1120 + 15),
			static_cast<UINT>(870 + 10),
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
		);

		// 把最大化按钮功能去掉并且禁止鼠标调整窗口大小
		HMENU hmenu = GetSystemMenu(m_hwnd, false);
		RemoveMenu(hmenu, SC_MAXIMIZE, MF_BYCOMMAND);
		RemoveMenu(hmenu, SC_SIZE, MF_BYCOMMAND);

		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);

			UpdateWindow(m_hwnd);
		}
	}

	return hr;
}

HRESULT Game::CreateDeviceIndependentResources()
{
	HRESULT hr;
	ID2D1GeometrySink *pSink = NULL;

	// 创建D2D工厂
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (m_pDWriteFactory == NULL && SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	}

	if (m_pWICFactory == NULL && SUCCEEDED(hr))
	{
		if (!SUCCEEDED(
			CoCreateInstance(
				CLSID_WICImagingFactory,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&m_pWICFactory)
			)
		))
			return FALSE;
	}

	return hr;
}

HRESULT Game::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRT)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
		);

		// 创建render target
		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRT
		);

		// 创建画刷
		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
				&m_pPainter_CornflowerBlue
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::White),
				&m_pPainter_White
			);
		}

		// 创建字体格式
		if (SUCCEEDED(hr))
		{
			hr = m_pDWriteFactory->CreateTextFormat(
				L"华文中宋",
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				30.0,
				L"zh-cn",
				&m_pTextFormat);
		}

		// 创建位图
		if (SUCCEEDED(hr))
		{
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_light_U.bmp", 0, 0, &m_pEnemy_light_tank[0]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_light_D.bmp", 0, 0, &m_pEnemy_light_tank[1]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_light_L.bmp", 0, 0, &m_pEnemy_light_tank[2]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_light_R.bmp", 0, 0, &m_pEnemy_light_tank[3]);

			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_normal_U.bmp", 0, 0, &m_pEnemy_normal_tank[0]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_normal_D.bmp", 0, 0, &m_pEnemy_normal_tank[1]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_normal_L.bmp", 0, 0, &m_pEnemy_normal_tank[2]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_normal_R.bmp", 0, 0, &m_pEnemy_normal_tank[3]);

			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_medium_U.bmp", 0, 0, &m_pEnemy_medium_tank[0]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_medium_D.bmp", 0, 0, &m_pEnemy_medium_tank[1]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_medium_L.bmp", 0, 0, &m_pEnemy_medium_tank[2]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_medium_R.bmp", 0, 0, &m_pEnemy_medium_tank[3]);

			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_heavy_U.bmp", 0, 0, &m_pEnemy_heavy_tank[0]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_heavy_D.bmp", 0, 0, &m_pEnemy_heavy_tank[1]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_heavy_L.bmp", 0, 0, &m_pEnemy_heavy_tank[2]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_heavy_R.bmp", 0, 0, &m_pEnemy_heavy_tank[3]);

			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_super_U.bmp", 0, 0, &m_pEnemy_super_tank[0]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_super_D.bmp", 0, 0, &m_pEnemy_super_tank[1]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_super_L.bmp", 0, 0, &m_pEnemy_super_tank[2]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_super_R.bmp", 0, 0, &m_pEnemy_super_tank[3]);

			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_gigantic_U.bmp", 0, 0, &m_pEnemy_gigantic_tank[0]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_gigantic_D.bmp", 0, 0, &m_pEnemy_gigantic_tank[1]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_gigantic_L.bmp", 0, 0, &m_pEnemy_gigantic_tank[2]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\enemy_gigantic_R.bmp", 0, 0, &m_pEnemy_gigantic_tank[3]);

			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\p1tankU.bmp", 0, 0, &m_pPlayer_1[0]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\p1tankD.bmp", 0, 0, &m_pPlayer_1[1]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\p1tankL.bmp", 0, 0, &m_pPlayer_1[2]);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\p1tankR.bmp", 0, 0, &m_pPlayer_1[3]);

			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\gametitle.bmp", 0, 0, &m_pGametitle);
			game_menu = D2D1::RectF(0.0F, 870.0F, m_pGametitle->GetSize().width, 870 + m_pGametitle->GetSize().height);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\selecttank.bmp", 0, 0, &m_pSelecttank);
			select_tank = D2D1::RectF(320.0F, 420.0F, 320.0F + m_pSelecttank->GetSize().width, 420.0F + m_pSelecttank->GetSize().height);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\stagecurtain.bmp", 0, 0, &m_pStagecurtain);
			stage_curtain_1 = D2D1::RectF(0.0F, -600.0F, m_pStagecurtain->GetSize().width, 0.0F);
			stage_curtain_2 = D2D1::RectF(0.0F, 840.0F, m_pStagecurtain->GetSize().width, 1420.0F);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\base.bmp", 0, 0, &m_pBase);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\wall.bmp", 0, 0, &m_pWall);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\steel.bmp", 0, 0, &m_pSteel);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\grass.bmp", 0, 0, &m_pGrass);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\water.bmp", 0, 0, &m_pWater);
			LoadBitmapFromFile(m_pRT, m_pWICFactory, L"Resources\\img\\bullet.bmp", 0, 0, &m_pBullet);
		}

		//读取地图
		if (SUCCEEDED(hr))
		{
			/*LoadMap("Resources\\maps\\Level_1.dat", current_map);*/
			LoadMap(Level, current_map);
			LoadEnemy(Level_Enemy);
			/*LoadMap("Resources\\maps\\Level_test.dat", current_map);*/
		}
	}

	return hr;
}

void Game::DiscardDeviceResources()
{
	SafeRelease(&m_pRT);
}

void Game::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT Game::OnRender()
{
	HRESULT hr;

	hr = CreateDeviceResources();
	if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		// 开始绘制
		m_pRT->BeginDraw();

		m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		// 开始游戏界面
		if (game_status == 0)
			GameMenu();


		if (game_status != 0)
		{
			if (current_total_enemy == 0)
			{	
				// 存储进行操作前的game_status
				if (game_status != 4)
					game_status_old = game_status;
				game_status = 4;
				StageChange();
				if(stage_change_frame_count > 120)
				{
					game_status = game_status_old;
					stage_change_frame_count = 0;
					NextLevel();
				}
			}

			if (game_status != 4)
			{
				// 初始化地图
				for (int i = 1; i < 27; i++)
				{
					for (int j = 1; j < 27; j++)
					{
						if (current_map[i][j] == 100)
							current_map[i][j] = 0;
						if (current_map[i][j] == 200)
							current_map[i][j] = 0;
					}
				}

				// 将所有坦克位置存储到map中用于碰撞检测
				SaveTankPos();

				// Player_1 Tank死亡
				if (tank_player_1.health <= 0)
				{
					player_1_life--;
					tank_player_1.position = D2D1::RectF(BLOCK * 9, BLOCK * 25, 30 + BLOCK * 10, 30 + BLOCK * 26);
					tank_player_1.health = 1;
				}
				if (player_1_life <= 0)
					gameover = true;

				// 处理键盘事件
				if (kbd.IsWPressed() == true)
					WHICH_PRESSED[0] = true;
				else if (kbd.IsSPressed() == true)
					WHICH_PRESSED[1] = true;
				else if (kbd.IsAPressed() == true)
					WHICH_PRESSED[2] = true;
				else if (kbd.IsDPressed() == true)
					WHICH_PRESSED[3] = true;

				if (WHICH_PRESSED[0] == true && (PROCESS_WHICH_PRESSED == 0 || PROCESS_WHICH_PRESSED == 'W'))
				{
					PROCESS_WHICH_PRESSED = 'W';
					tank_player_1.direction = 0;
					if (MapCollisionDetect(tank_player_1, 'U') == false)
						tank_player_1.MoveTankUpPixel(3);
					frame_count_2++;
					if (frame_count_2 == 10)
					{
						frame_count_2 = 0;
						WHICH_PRESSED[0] = false;
						PROCESS_WHICH_PRESSED = 0;
					}
				}
				else if (WHICH_PRESSED[1] == true && (PROCESS_WHICH_PRESSED == 0 || PROCESS_WHICH_PRESSED == 'D'))
				{
					PROCESS_WHICH_PRESSED = 'D';
					tank_player_1.direction = 3;
					if (MapCollisionDetect(tank_player_1, 'D') == false)
						tank_player_1.MoveTankDownPixel(3);
					frame_count_2++;
					if (frame_count_2 == 10)
					{
						frame_count_2 = 0;
						WHICH_PRESSED[1] = false;
						PROCESS_WHICH_PRESSED = 0;
					}
				}
				else if (WHICH_PRESSED[2] == true && (PROCESS_WHICH_PRESSED == 0 || PROCESS_WHICH_PRESSED == 'L'))
				{
					PROCESS_WHICH_PRESSED = 'L';
					tank_player_1.direction = 1;
					if (MapCollisionDetect(tank_player_1, 'L') == false)
						tank_player_1.MoveTankLeftPixel(3);
					frame_count_2++;
					if (frame_count_2 == 10)
					{
						frame_count_2 = 0;
						WHICH_PRESSED[2] = false;
						PROCESS_WHICH_PRESSED = 0;
					}
				}
				else if (WHICH_PRESSED[3] == true && (PROCESS_WHICH_PRESSED == 0 || PROCESS_WHICH_PRESSED == 'R'))
				{
					PROCESS_WHICH_PRESSED = 'R';
					tank_player_1.direction = 2;
					if (MapCollisionDetect(tank_player_1, 'R') == false)
						tank_player_1.MoveTankRightPixel(3);
					frame_count_2++;
					if (frame_count_2 == 10)
					{
						frame_count_2 = 0;
						WHICH_PRESSED[3] = false;
						PROCESS_WHICH_PRESSED = 0;
					}
				}

				// 转向辅助(废案)		
				/*if (frame_count % 30 == 0)
				{
					tank_player_1.position.left = (int(tank_player_1.position.left) / 30) * 30.0f;
					tank_player_1.position.right = (int(tank_player_1.position.right) / 30) * 30.0f;
					tank_player_1.position.top = (int(tank_player_1.position.top) / 30) * 30.0f;
					tank_player_1.position.bottom = (int(tank_player_1.position.bottom) / 30) * 30.0f;
				}*/
				

				//--------------------------------------------------------------------------
				// 绘制玩家坦克: 初始位置为9，25 - 10,26
				// 0为向上，1为向左，2为向右，3为向下；其他参数默认为向上
				//--------------------------------------------------------------------------
				if (tank_player_1.direction == 1)
				{
					m_pRT->DrawBitmap(
						m_pPlayer_1[2],
						tank_player_1.position);
				}
				else if (tank_player_1.direction == 2)
				{
					m_pRT->DrawBitmap(
						m_pPlayer_1[3],
						tank_player_1.position);
				}
				else if (tank_player_1.direction == 3)
				{
					m_pRT->DrawBitmap(
						m_pPlayer_1[1],
						tank_player_1.position);
				}
				else
				{
					m_pRT->DrawBitmap(
						m_pPlayer_1[0],
						tank_player_1.position);
				}

				// 绘制敌方坦克, 敌方坦克移动，射击等实现		
				EnemyTank();

				//绘制游戏界面
				m_pRT->DrawRectangle(D2D1::RectF(30.0F, 30.0F, 810.0F, 810.0F), m_pPainter_CornflowerBlue);

				// 绘制信息区域屏幕
				PrintSideScreen();

				//绘制地图 (map里的值: 0为可通过陆地，1为红砖，2钢墙，3为水，4为树荫(在子弹绘制后绘制)，9为Base)
				m_pRT->DrawBitmap(
					m_pBase,
					D2D1::RectF(BLOCK * 13, BLOCK * 25 - 3,
						BLOCK * 15,
						BLOCK * 27 - 3));
				for (int i = 1; i < 27; i++)
				{
					for (int j = 1; j < 27; j++)
					{
						switch (current_map[i][j])
						{
						case 1:
							m_pRT->DrawBitmap(
								m_pWall,
								D2D1::RectF(BLOCK * j, BLOCK * i,
									m_pWall->GetSize().width + BLOCK * j,
									m_pWall->GetSize().height + BLOCK * i));
							break;
						case 2:
							m_pRT->DrawBitmap(
								m_pSteel,
								D2D1::RectF(BLOCK * j, BLOCK * i,
									m_pWall->GetSize().width + BLOCK * j,
									m_pWall->GetSize().height + BLOCK * i));
							break;
						case 3:
							m_pRT->DrawBitmap(
								m_pWater,
								D2D1::RectF(BLOCK * j, BLOCK * i,
									m_pWall->GetSize().width + BLOCK * j,
									m_pWall->GetSize().height + BLOCK * i));
							break;
						default:
							//将这部分作为空白绘制
							break;
						}
					}
				}

				// 绘制玩家子弹并进行碰撞判断 
				if (kbd.IsJPressed() == true)
					WHICH_PRESSED[4] = true;

				if (WHICH_PRESSED[4] == true && frame_count % 30 == 0)		// 创建子弹并设置子弹发射冷却时间为30帧
				{
					p1_bullet.push_back(Bullet(tank_player_1));
					WHICH_PRESSED[4] = false;
				}
				for (std::vector<Bullet>::iterator iter = p1_bullet.begin() + 1; iter != p1_bullet.end(); ++iter)
				{	// 环境碰撞检测 + 敌人碰撞检测
					bool hit_enemy = false;

					for (unsigned int i = 1; i < enemy_light_tank.size() && hit_enemy == false; i++)
						if (run_time > enemy_light_tank[i].appear_time && D2D1_RECT_F_Intersaction(enemy_light_tank[i].position, (*iter).position))
						{
							enemy_light_tank[i].health--;
							hit_enemy = true;
						}
					for (unsigned int i = 1; i < enemy_normal_tank.size() && hit_enemy == false; i++)
						if (run_time > enemy_normal_tank[i].appear_time && D2D1_RECT_F_Intersaction(enemy_normal_tank[i].position, (*iter).position))
						{
							enemy_normal_tank[i].health--;
							hit_enemy = true;
						}
					for (unsigned int i = 1; i < enemy_medium_tank.size() && hit_enemy == false; i++)
						if (run_time > enemy_medium_tank[i].appear_time && D2D1_RECT_F_Intersaction(enemy_medium_tank[i].position, (*iter).position))
						{
							enemy_medium_tank[i].health--;
							hit_enemy = true;
						}
					for (unsigned int i = 1; i < enemy_heavy_tank.size() && hit_enemy == false; i++)
						if (run_time > enemy_heavy_tank[i].appear_time && D2D1_RECT_F_Intersaction(enemy_heavy_tank[i].position, (*iter).position))
						{
							enemy_heavy_tank[i].health--;
							hit_enemy = true;
						}
					for (unsigned int i = 1; i < enemy_super_tank.size() && hit_enemy == false; i++)
						if (run_time > enemy_super_tank[i].appear_time && D2D1_RECT_F_Intersaction(enemy_super_tank[i].position, (*iter).position))
						{
							enemy_super_tank[i].health--;
							hit_enemy = true;
						}
					for (unsigned int i = 1; i < enemy_gigantic_tank.size() && hit_enemy == false; i++)
						if (run_time > enemy_gigantic_tank[i].appear_time && D2D1_RECT_F_Intersaction(enemy_gigantic_tank[i].position, (*iter).position))
						{
							enemy_gigantic_tank[i].health--;
							hit_enemy = true;
						}

					if (((*iter).Move() == false) || (hit_enemy == true))
					{
						iter = p1_bullet.erase(iter);
						iter = std::prev(iter);
					}
					m_pRT->DrawBitmap(m_pBullet, (*iter).position);

				}

				// 绘制树荫
				for (int i = 0; i < 27; i++)
				{
					for (int j = 0; j < 27; j++)
					{
						if (current_map_grass[i][j] == 4)
						{
							m_pRT->DrawBitmap(
								m_pGrass,
								D2D1::RectF(BLOCK * j, BLOCK * i,
									m_pWall->GetSize().width + BLOCK * j,
									m_pWall->GetSize().height + BLOCK * i));
						}
					}
				}

				frame_count++;
				if (frame_count % 60 == 0)
					run_time++;
			}
		}

		// 结束绘制
		hr = m_pRT->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}

		// 重置Keyboard状态
		{
			kbd.wIsReleased = false;
			kbd.sIsReleased = false;
			kbd.aIsReleased = false;
			kbd.dIsReleased = false;
			kbd.jIsReleased = false;
			kbd.kIsReleased = false;

			kbd.upIsReleased = false;
			kbd.downIsReleased = false;
			kbd.leftIsReleased = false;
			kbd.rightIsReleased = false;
			kbd.rctrlIsReleased = false;
			kbd.rshiftIsReleased = false;

			kbd.spaceIsReleased = false;
			kbd.enterIsReleased = false;
			kbd.escIsReleased = false;
		}

		if (gameover == true)
		{
			game_status = 0;
			gameover = false;
			m_pRT = NULL;
			menu_frame_count = 0;
			frame_count = 0;
			frame_count_2 = 0;
			run_time = 0;
			enemy_light_tank.resize(1);
			enemy_normal_tank.resize(1);
			enemy_medium_tank.resize(1);
			enemy_heavy_tank.resize(1);
			enemy_super_tank.resize(1);
			enemy_gigantic_tank.resize(1);
			tank_player_1.direction = 0;
			tank_player_1.position = D2D1::RectF(BLOCK * 9, BLOCK * 25, 30 + BLOCK * 10, 30 + BLOCK * 26);
		}
	}

	InvalidateRect(m_hwnd, NULL, FALSE);
	return hr;
}

void Game::OnResize(UINT width, UINT height)
{
	if (m_pRT)
	{
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;

		// Note: This method can fail, but it's okay to ignore the
		// error here -- it will be repeated on the next call to
		// EndDraw.
		m_pRT->Resize(size);
	}
}

void Game::PrintSideScreen()
{
	m_pRT->DrawRectangle(D2D1::RectF(840.0F, 30.0F, 1090.0F, 810.0F), m_pPainter_CornflowerBlue);

	std::string str = std::to_string((frame_count % 60));
	std::wstring dstr = string_wstring(str);
	wchar_t dstring[] = { L"60" };
	for (int i = 0; i < 2; i++)
	{
		dstring[i] = dstr[i];
	}
	// 用于显示帧率
	/*m_pRT->DrawText(
	dstring,
	wcslen(L"60"),
	m_pTextFormat,
	D2D1::RectF(840, 100, 1090, 810),
	m_pPainter_White);*/

	m_pRT->DrawText(
		L"亿万长者:",
		wcslen(L"亿万长者:"),
		m_pTextFormat,
		D2D1::RectF(840, 50, 1090, 810),
		m_pPainter_White);

	/*m_pRT->DrawText(
		L"亿万长者:",
		wcslen(L"亿万长者:"),
		m_pTextFormat,
		D2D1::RectF(840, 100, 1090, 810),
		m_pPainter_White);*/

	m_pRT->DrawText(
		L"得分:",
		wcslen(L"得分:"),
		m_pTextFormat,
		D2D1::RectF(840, 150, 1090, 810),
		m_pPainter_White);

	/*m_pRT->DrawText(
		L"得分:",
		wcslen(L"得分:"),
		m_pTextFormat,
		D2D1::RectF(840, 200, 1090, 810),
		m_pPainter_White);*/

	str = std::to_string(player_1_life);
	dstr = string_wstring(str);
	for (int i = 0; i < 2; i++)
	{
		dstring[i] = dstr[i];
	}
	m_pRT->DrawText(
		dstring,
		wcslen(L"P1"),
		m_pTextFormat,
		D2D1::RectF(970, 250, 1090, 810),
		m_pPainter_White);
	m_pRT->DrawText(
		L"P1生命:",
		wcslen(L"P1生命:"),
		m_pTextFormat,
		D2D1::RectF(840, 250, 1090, 810),
		m_pPainter_White);

	// Player_2生命值
	/*str = std::to_string(player_2_life);
	dstr = string_wstring(str);
	for (int i = 0; i < 2; i++)
	{
		dstring[i] = dstr[i];
	}
	m_pRT->DrawText(
		dstring,
		wcslen(L"P1"),
		m_pTextFormat,
		D2D1::RectF(970, 300, 1090, 810),
		m_pPainter_White);
	m_pRT->DrawText(
		L"P1生命:",
		wcslen(L"P1生命:"),
		m_pTextFormat,
		D2D1::RectF(840, 300, 1090, 810),
		m_pPainter_White);*/

	str = std::to_string(current_total_enemy);
	dstr = string_wstring(str);
	for (int i = 0; i < 2; i++)
	{
		dstring[i] = dstr[i];
	}
	m_pRT->DrawText(
		dstring,
		wcslen(L"P1"),
		m_pTextFormat,
		D2D1::RectF(1000, 350, 1090, 810),
		m_pPainter_White);
	m_pRT->DrawText(
		L"剩余敌人:",
		wcslen(L"剩余敌人:"),
		m_pTextFormat,
		D2D1::RectF(840, 350, 1090, 810),
		m_pPainter_White);

	if (gameover == true)
		m_pRT->DrawText(
			L"你家没了！",
			wcslen(L"你家没了！"),
			m_pTextFormat,
			D2D1::RectF(440, 350, 1090, 810),
			m_pPainter_White);
}

void Game::SaveTankPos()
{
	// Light Tank
	for (unsigned int i = 1; i < enemy_light_tank.size(); i++)
	{
		if (run_time >= enemy_light_tank[i].appear_time)
		{
			int row = int(enemy_light_tank[i].position.top / BLOCK);
			int column = int(enemy_light_tank[i].position.left / BLOCK);
			current_map[row][column] = 200;
			current_map[row][column + 1] = 200;
			current_map[row + 1][column] = 200;
			current_map[row + 1][column + 1] = 200;
		}
	}

	// Normal Tank
	for (unsigned int i = 1; i < enemy_normal_tank.size(); i++)
	{
		if (run_time >= enemy_normal_tank[i].appear_time)
		{
			int row = int(enemy_normal_tank[i].position.top / BLOCK);
			int column = int(enemy_normal_tank[i].position.left / BLOCK);
			current_map[row][column] = 200;
			current_map[row][column + 1] = 200;
			current_map[row + 1][column] = 200;
			current_map[row + 1][column + 1] = 200;
		}
	}

	// Medium Tank
	for (unsigned int i = 1; i < enemy_medium_tank.size(); i++)
	{
		if (run_time >= enemy_medium_tank[i].appear_time)
		{
			int row = int(enemy_medium_tank[i].position.top / BLOCK);
			int column = int(enemy_medium_tank[i].position.left / BLOCK);
			current_map[row][column] = 200;
			current_map[row][column + 1] = 200;
			current_map[row + 1][column] = 200;
			current_map[row + 1][column + 1] = 200;
		}
	}

	// Heavy Tank
	for (unsigned int i = 1; i < enemy_heavy_tank.size(); i++)
	{
		if (run_time >= enemy_heavy_tank[i].appear_time)
		{
			int row = int(enemy_heavy_tank[i].position.top / BLOCK);
			int column = int(enemy_heavy_tank[i].position.left / BLOCK);
			current_map[row][column] = 200;
			current_map[row][column + 1] = 200;
			current_map[row + 1][column] = 200;
			current_map[row + 1][column + 1] = 200;
		}
	}

	// Super Tank
	for (unsigned int i = 1; i < enemy_super_tank.size(); i++)
	{
		if (run_time >= enemy_super_tank[i].appear_time)
		{
			int row = int(enemy_super_tank[i].position.top / BLOCK);
			int column = int(enemy_super_tank[i].position.left / BLOCK);
			current_map[row][column] = 200;
			current_map[row][column + 1] = 200;
			current_map[row + 1][column] = 200;
			current_map[row + 1][column + 1] = 200;
		}
	}

	// Gigantic Tank
	for (unsigned int i = 1; i < enemy_gigantic_tank.size(); i++)
	{
		if (run_time >= enemy_gigantic_tank[i].appear_time)
		{
			int row = int(enemy_gigantic_tank[i].position.top / BLOCK);
			int column = int(enemy_gigantic_tank[i].position.left / BLOCK);
			current_map[row][column] = 200;
			current_map[row][column + 1] = 200;
			current_map[row + 1][column] = 200;
			current_map[row + 1][column + 1] = 200;
		}
	}

	// Player_1 Tank
	int row = int(tank_player_1.position.top / BLOCK);
	int column = int(tank_player_1.position.left / BLOCK);
	current_map[row][column] = 100;
	current_map[row][column + 1] = 100;
	current_map[row + 1][column] = 100;
	current_map[row + 1][column + 1] = 100;
}

void Game::EnemyTank()
{
	// Light Tank
	for (unsigned int i = 1; i < enemy_light_tank.size(); i++)
	{
		if (enemy_light_tank[i].health <= 0)
		{
			std::vector<Tank>::iterator temp = enemy_light_tank.begin() + i;
			enemy_light_tank.erase(temp);
			current_total_enemy--;
			i--;
			continue;
		}

		if (run_time >= enemy_light_tank[i].appear_time)
		{
			if (enemy_light_tank[i].direction == 0)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_light_tank[0],
					enemy_light_tank[i].position);
			}
			else if (enemy_light_tank[i].direction == 3)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_light_tank[1],
					enemy_light_tank[i].position);
			}
			else if (enemy_light_tank[i].direction == 1)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_light_tank[2],
					enemy_light_tank[i].position);
			}
			else
			{
				m_pRT->DrawBitmap(
					m_pEnemy_light_tank[3],
					enemy_light_tank[i].position);
			}
			enemy_light_tank[i].alive_frame++;
		}
	}
	// Normal Tank
	for (unsigned int i = 1; i < enemy_normal_tank.size(); i++)
	{
		if (enemy_normal_tank[i].health <= 0)
		{
			std::vector<Tank>::iterator temp = enemy_normal_tank.begin() + i;
			enemy_normal_tank.erase(temp);
			current_total_enemy--;
			i--;
			continue;
		}

		if (run_time >= enemy_normal_tank[i].appear_time)
		{
			if (enemy_normal_tank[i].alive_frame % 60 == 0)		// 每1s射一次
			{
				enemy_normal_tank[i].bullet.push_back(Bullet(enemy_normal_tank[i]));
			}

			// 判断敌人子弹是否击中玩家并实现地图碰撞
			for (std::vector<Bullet>::iterator iter = enemy_normal_tank[i].bullet.begin() + 1; iter != enemy_normal_tank[i].bullet.end(); ++iter)
			{
				bool hit_enemy = false;
				if (D2D1_RECT_F_Intersaction(tank_player_1.position, (*iter).position))
				{
					tank_player_1.health--;
					hit_enemy = true;
				}
				// 敌人子弹与玩家子弹之间的碰撞检测
				for (std::vector<Bullet>::iterator iter_p1 = p1_bullet.begin() + 1; iter_p1 != p1_bullet.end(); ++iter_p1)
				if (D2D1_RECT_F_Intersaction((*iter).position, (*iter_p1).position))
				{
					hit_enemy = true;
					iter_p1 = p1_bullet.erase(iter_p1);
					iter_p1 = std::prev(iter_p1);
				}

				if (((*iter).Move() == false) || (hit_enemy == true))
				{
					iter = enemy_normal_tank[i].bullet.erase(iter);
					iter = std::prev(iter);
				}
				m_pRT->DrawBitmap(m_pBullet, (*iter).position);
			}

			// 地图碰撞判断并实现卡住超过0.5秒随机转向
			if (enemy_normal_tank[i].direction == 0)
			{
				if (MapCollisionDetect(enemy_normal_tank[i], 'U') == false)
					enemy_normal_tank[i].MoveTankUpPixel(2);
				else
					enemy_normal_tank[i].stuck_frame++;

				m_pRT->DrawBitmap(
					m_pEnemy_normal_tank[0],
					enemy_normal_tank[i].position);

				if (enemy_normal_tank[i].stuck_frame >= 30)
				{
					enemy_normal_tank[i].direction = rand() % 4;
					enemy_normal_tank[i].stuck_frame = 0;
				}
			}
			else if (enemy_normal_tank[i].direction == 3)
			{
				if (MapCollisionDetect(enemy_normal_tank[i], 'D') == false)
					enemy_normal_tank[i].MoveTankDownPixel(2);
				else
					enemy_normal_tank[i].stuck_frame++;

				m_pRT->DrawBitmap(
					m_pEnemy_normal_tank[1],
					enemy_normal_tank[i].position);

				if (enemy_normal_tank[i].stuck_frame >= 30)
				{
					enemy_normal_tank[i].direction = rand() % 4;
					enemy_normal_tank[i].stuck_frame = 0;
				}
			}
			else if (enemy_normal_tank[i].direction == 1)
			{
				if (MapCollisionDetect(enemy_normal_tank[i], 'L') == false)
					enemy_normal_tank[i].MoveTankLeftPixel(2);
				else
					enemy_normal_tank[i].stuck_frame++;

				m_pRT->DrawBitmap(
					m_pEnemy_normal_tank[2],
					enemy_normal_tank[i].position);

				if (enemy_normal_tank[i].stuck_frame >= 30)
				{
					enemy_normal_tank[i].direction = rand() % 4;
					enemy_normal_tank[i].stuck_frame = 0;
				}
			}
			else
			{
				if (MapCollisionDetect(enemy_normal_tank[i], 'R') == false)
					enemy_normal_tank[i].MoveTankRightPixel(2);
				else
					enemy_normal_tank[i].stuck_frame++;

				m_pRT->DrawBitmap(
					m_pEnemy_normal_tank[3],
					enemy_normal_tank[i].position);

				if (enemy_normal_tank[i].stuck_frame >= 30)
				{
					enemy_normal_tank[i].direction = rand() % 4;
					enemy_normal_tank[i].stuck_frame = 0;
				}
			}
			enemy_normal_tank[i].alive_frame++;
		}
	}

	// Medium Tank
	for (unsigned int i = 1; i < enemy_medium_tank.size(); i++)
	{
		if (enemy_medium_tank[i].health <= 0)
		{
			std::vector<Tank>::iterator temp = enemy_medium_tank.begin() + i;
			enemy_medium_tank.erase(temp);
			current_total_enemy--;
			i--;
			continue;
		}

		if (run_time >= enemy_medium_tank[i].appear_time)
		{
			if (enemy_medium_tank[i].direction == 0)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_medium_tank[0],
					enemy_medium_tank[i].position);
			}
			else if (enemy_medium_tank[i].direction == 3)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_medium_tank[1],
					enemy_medium_tank[i].position);
			}
			else if (enemy_medium_tank[i].direction == 1)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_medium_tank[2],
					enemy_medium_tank[i].position);
			}
			else
			{
				m_pRT->DrawBitmap(
					m_pEnemy_medium_tank[3],
					enemy_medium_tank[i].position);
			}
			enemy_medium_tank[i].alive_frame++;
		}
	}
	// Heavy Tank
	for (unsigned int i = 1; i < enemy_heavy_tank.size(); i++)
	{
		if (enemy_heavy_tank[i].health <= 0)
		{
			std::vector<Tank>::iterator temp = enemy_heavy_tank.begin() + i;
			enemy_heavy_tank.erase(temp);
			current_total_enemy--;
			i--;
			continue;
		}

		if (run_time >= enemy_heavy_tank[i].appear_time)
		{
			if (enemy_heavy_tank[i].direction == 0)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_heavy_tank[0],
					enemy_heavy_tank[i].position);
			}
			else if (enemy_heavy_tank[i].direction == 3)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_heavy_tank[1],
					enemy_heavy_tank[i].position);
			}
			else if (enemy_heavy_tank[i].direction == 1)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_heavy_tank[2],
					enemy_heavy_tank[i].position);
			}
			else
			{
				m_pRT->DrawBitmap(
					m_pEnemy_heavy_tank[3],
					enemy_heavy_tank[i].position);
			}
			enemy_heavy_tank[i].alive_frame++;
		}
	}
	// Super Tank
	for (unsigned int i = 1; i < enemy_super_tank.size(); i++)
	{
		if (enemy_super_tank[i].health <= 0)
		{
			std::vector<Tank>::iterator temp = enemy_super_tank.begin() + i;
			enemy_super_tank.erase(temp);
			current_total_enemy--;
			i--;
			continue;
		}

		if (run_time >= enemy_super_tank[i].appear_time)
		{
			if (enemy_super_tank[i].direction == 0)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_super_tank[0],
					enemy_super_tank[i].position);
			}
			else if (enemy_super_tank[i].direction == 3)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_super_tank[1],
					enemy_super_tank[i].position);
			}
			else if (enemy_super_tank[i].direction == 1)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_super_tank[2],
					enemy_super_tank[i].position);
			}
			else
			{
				m_pRT->DrawBitmap(
					m_pEnemy_super_tank[3],
					enemy_super_tank[i].position);
			}
			enemy_super_tank[i].alive_frame++;
		}
	}
	// Gigantic Tank
	for (unsigned int i = 1; i < enemy_gigantic_tank.size(); i++)
	{
		if (enemy_gigantic_tank[i].health <= 0)
		{
			std::vector<Tank>::iterator temp = enemy_gigantic_tank.begin() + i;
			enemy_gigantic_tank.erase(temp);
			current_total_enemy--;
			i--;
			continue;
		}

		if (run_time >= enemy_gigantic_tank[i].appear_time)
		{
			if (enemy_gigantic_tank[i].direction == 0)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_gigantic_tank[0],
					enemy_gigantic_tank[i].position);
			}
			else if (enemy_gigantic_tank[i].direction == 3)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_gigantic_tank[1],
					enemy_gigantic_tank[i].position);
			}
			else if (enemy_gigantic_tank[i].direction == 1)
			{
				m_pRT->DrawBitmap(
					m_pEnemy_gigantic_tank[2],
					enemy_gigantic_tank[i].position);
			}
			else
			{
				m_pRT->DrawBitmap(
					m_pEnemy_gigantic_tank[3],
					enemy_gigantic_tank[i].position);
			}
			enemy_gigantic_tank[i].alive_frame++;
		}
	}
}

bool Game::MapCollisionDetect(Tank tank, const char direction)	// 未发生碰撞返回false，发生碰撞返回true
{
	int i = int(tank.position.top / BLOCK);
	int j = int(tank.position.left / BLOCK);
	if (direction == 'L')
	{
		if ((int(tank.position.left) % int(BLOCK)) == 0)
		{
			if ((int(tank.position.top) % int(BLOCK)) == 0)
			{
				if ((current_map[i][j - 1] == 0 || current_map[i][j - 1] == 4) && (current_map[i + 1][j - 1] == 0 || current_map[i + 1][j - 1] == 4))
					return false;
				else
					return true;
			}
			else
			{
				if ((current_map[i][j - 1] == 0 || current_map[i][j - 1] == 4) && (current_map[i + 1][j - 1] == 0 || current_map[i + 1][j - 1] == 4) && (current_map[i + 2][j - 1] == 0 || current_map[i + 2][j - 1] == 0))
					return false;
				else
					return true;
			}
		}
		else
			return false;
	}
	else if (direction == 'U')
	{
		if ((int(tank.position.top) % int(BLOCK)) == 0)
		{
			if ((int(tank.position.left) % int(BLOCK)) == 0)
			{
				if ((current_map[i - 1][j] == 0 || current_map[i - 1][j] == 4) && (current_map[i - 1][j + 1] == 0 || current_map[i - 1][j + 1] == 4))
					return false;
				else
					return true;
			}
			else
			{
				if ((current_map[i - 1][j] == 0 || current_map[i - 1][j] == 4) && (current_map[i - 1][j + 1] == 0 || current_map[i - 1][j + 1] == 4) && (current_map[i - 1][j + 2] == 0 || current_map[i - 1][j + 2] == 4))
					return false;
				else
					return true;
			}
		}
		else
			return false;
	}
	else if (direction == 'R')
	{
		if ((int(tank.position.right) % int(BLOCK)) == 0)
		{
			if ((int(tank.position.top) % int(BLOCK)) == 0)
			{
				if ((current_map[i][j + 2] == 0 || current_map[i][j + 2] == 4) && (current_map[i + 1][j + 2] == 0 || current_map[i + 1][j + 2] == 4))
					return false;
				else
					return true;
			}
			else
			{
				if ((current_map[i][j + 2] == 0 || current_map[i][j + 2] == 4) && (current_map[i + 1][j + 2] == 0 || current_map[i + 1][j + 2] == 4) && (current_map[i + 2][j + 2] == 0 || current_map[i + 2][j + 2] == 4))
					return false;
				else
					return true;
			}
		}
		else
			return false;
	}
	else if (direction == 'D')
	{
		if ((int(tank.position.bottom) % int(BLOCK)) == 0)
		{
			if ((int(tank.position.left) % int(BLOCK)) == 0)
			{
				if ((current_map[i + 2][j] == 0 || current_map[i + 2][j] == 4) && (current_map[i + 2][j + 1] == 0 || current_map[i + 2][j + 1] == 4))
					return false;
				else
					return true;
			}
			else
			{
				if ((current_map[i + 2][j] == 0 || current_map[i + 2][j] == 4) && (current_map[i + 2][j + 1] == 0 || current_map[i + 2][j + 1] == 4) && (current_map[i + 2][j + 2] == 0 || current_map[i + 2][j + 2] == 4))
					return false;
				else
					return true;
			}
		}
		else
			return false;
	}
	return false;
}


//-----------------------------------------------------------------
// 从文件加载地图
//-----------------------------------------------------------------
void Game::LoadMap(const char* filepath, int map[28][28])
{
	std::ifstream map_in;
	map_in.open(filepath);
	for (int i = 0; i < 28; i++)
	{
		for (int j = 0; j < 28; j++)
		{
			char str_temp[10];
			map_in.getline(str_temp, 2, ',');
			map[i][j] = str_temp[0] - 48;
		}
		std::string temp;
		getline(map_in, temp);
	}
	map_in.close();

	for (int i = 0; i < 27; i++)
		for (int j = 0; j < 27; j++)
			if (current_map[i][j] == 4)
				current_map_grass[i][j] = 4;	
}

void Game::LoadEnemy(const char* filepath)
{
	current_total_enemy = 0;
	std::ifstream enemy_set_in;
	enemy_set_in.open(filepath);
	int Num[6] = { 0 };
	for (int i = 0; i < 6; i++)
	{
		enemy_set_in >> Num[i];
		current_total_enemy += Num[i];
	}
	enemy_light_tank.resize(Num[0] + 1);
	enemy_normal_tank.resize(Num[1] + 1);
	enemy_medium_tank.resize(Num[2] + 1);
	enemy_heavy_tank.resize(Num[3] + 1);
	enemy_super_tank.resize(Num[4] + 1);
	enemy_gigantic_tank.resize(Num[5] + 1);

	int iter_l = 1, iter_n = 1, iter_m = 1, iter_h = 1, iter_s = 1, iter_g = 1;

	for (int i = 0; i < current_total_enemy; i++)
	{
		char info_header[4] = { 0 };
		char time[4] = { 0 };

		enemy_set_in >> info_header;
		enemy_set_in >> time;
		int tank_appear_time = (time[0] - 48) * 100 + (time[1] - 48) * 10 + (time[2] - 48);
		int respawn_pos = (info_header[1] - 48) * 10 + (info_header[2] - 48);
		switch (info_header[0])
		{
		case 'L':
			enemy_light_tank[iter_l].position = respawn_position[respawn_pos];
			enemy_light_tank[iter_l].appear_time = tank_appear_time;
			iter_l++;
			break;

		case 'N':
			enemy_normal_tank[iter_n].position = respawn_position[respawn_pos];
			enemy_normal_tank[iter_n].appear_time = tank_appear_time;
			iter_n++;
			break;

		case 'M':
			enemy_medium_tank[iter_m].position = respawn_position[respawn_pos];
			enemy_medium_tank[iter_m].appear_time = tank_appear_time;
			enemy_medium_tank[iter_m].health = 2;
			iter_m++;
			break;

		case 'H':
			enemy_heavy_tank[iter_h].position = respawn_position[respawn_pos];
			enemy_heavy_tank[iter_h].appear_time = tank_appear_time;
			enemy_heavy_tank[iter_h].health = 3;
			iter_h++;
			break;

		case 'S':
			enemy_super_tank[iter_s].position = respawn_position[respawn_pos];
			enemy_super_tank[iter_s].appear_time = tank_appear_time;
			enemy_super_tank[iter_s].health = 4;
			iter_s++;
			break;

		case 'G':
			enemy_gigantic_tank[iter_g].position = respawn_position[respawn_pos];
			enemy_gigantic_tank[iter_g].appear_time = tank_appear_time;
			enemy_gigantic_tank[iter_g].health = 5;
			iter_g++;
			break;

		default:
			break;
		}
	}

	enemy_set_in.close();
}

void Game::GameMenu()
{
	if (menu_frame_count <= 180)
	{
		if (kbd.spaceIsReleased == true || kbd.jIsReleased == true)
		{
			game_menu.top = -30;
			game_menu.bottom = 810;
			menu_frame_count = 180;
		}
		else
		{
			game_menu.top -= 5;
			game_menu.bottom -= 5;
		}
		m_pRT->DrawBitmap(m_pGametitle, game_menu);
	}
	else
	{
		m_pRT->DrawBitmap(m_pGametitle, game_menu);

		m_pRT->DrawBitmap(m_pSelecttank, select_tank);

		if (kbd.wIsReleased == true && select_tank.top > 420)
		{
			select_tank.top -= 64;
			select_tank.bottom -= 64;
		}

		if (kbd.sIsReleased == true && select_tank.top < 548)
		{
			select_tank.top += 64;
			select_tank.bottom += 64;
		}

		if (kbd.jIsReleased == true)
		{
			if (select_tank.top == 420)
				game_status = 1;
			else if (select_tank.top == 484)
				game_status = 2;
			else if (select_tank.top == 548)
				game_status = 3;
		}

		// 老实现了(废案)
		/*if (kbd.IsWPressed() == true)
			WHICH_PRESSED[0] = true;
		else if (kbd.IsSPressed() == true)
			WHICH_PRESSED[1] = true;

		if (WHICH_PRESSED[0] == true && menu_frame_count % 10 == 0 && select_tank.top > 420)
		{
			select_tank.top -= 64;
			select_tank.bottom -= 64;
			WHICH_PRESSED[0] = false;
		}

		if (WHICH_PRESSED[1] == true && menu_frame_count % 10 == 0 && select_tank.top < 548)
		{
			select_tank.top += 64;
			select_tank.bottom += 64;
			WHICH_PRESSED[1] = false;
		}*/
	}
	menu_frame_count++;
}

void Game::NextLevel()
{
	if (Level[22] != '9')
	{
		Level[22]++;
		Level_Enemy[22]++;
	}
	else
	{
		Level[21]++;
		Level_Enemy[21]++;
		Level[22] = '0';
		Level_Enemy[22] = '0';
	}
	enemy_light_tank.resize(1);
	enemy_normal_tank.resize(1);
	enemy_medium_tank.resize(1);
	enemy_heavy_tank.resize(1);
	enemy_super_tank.resize(1);
	enemy_gigantic_tank.resize(1);
	LoadMap(Level, current_map);
	LoadEnemy(Level_Enemy);
	tank_player_1.position = D2D1::RectF(BLOCK * 9, BLOCK * 25, 30 + BLOCK * 10, 30 + BLOCK * 26);
	frame_count = 0;
	frame_count_2 = 0;
	run_time = 0;
}

void Game::StageChange()
{
	if (game_status == 4)
	{
		if (stage_change_frame_count <= 120)
		{
			stage_curtain_1.top += 5;
			stage_curtain_1.bottom += 5;
			stage_curtain_2.top -= 5;
			stage_curtain_2.bottom -= 5;

			m_pRT->DrawBitmap(m_pStagecurtain, stage_curtain_1);
			m_pRT->DrawBitmap(m_pStagecurtain, stage_curtain_2);
		}

		stage_change_frame_count++;
	}
}

LRESULT CALLBACK Game::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		Game *pGame = (Game *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pGame)
		);

		result = 1;
	}
	else
	{
		Game *pGame = reinterpret_cast<Game *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
			)));

		bool wasHandled = false;

		if (pGame)
		{
			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pGame->OnResize(width, height);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_PAINT:
			case WM_DISPLAYCHANGE:
			{
				PAINTSTRUCT ps;
				BeginPaint(hwnd, &ps);

				pGame->OnRender();
				EndPaint(hwnd, &ps);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_KEYDOWN:
				switch (wParam)
				{
				case 0x57:
					kbd.OnWPressed();
					break;
				case 0x53:
					kbd.OnSPressed();
					break;
				case 0x41:
					kbd.OnAPressed();
					break;
				case 0x44:
					kbd.OnDPressed();
					break;
				case 0x4A:
					kbd.OnJPressed();
					break;
				case 0x4B:
					kbd.OnKPressed();
					break;

				case VK_UP:
					kbd.OnUpPressed();
					break;
				case VK_DOWN:
					kbd.OnDownPressed();
					break;
				case VK_LEFT:
					kbd.OnLeftPressed();
					break;
				case VK_RIGHT:
					kbd.OnRightPressed();
					break;
				case VK_RCONTROL:
					kbd.OnRightCtrlPressed();
					break;
				case VK_RSHIFT:
					kbd.OnRightShiftPressed();
					break;

				case VK_SPACE:
					kbd.OnSpacePressed();
					break;
				case VK_RETURN:
					kbd.OnEnterPressed();
					break;
				case VK_ESCAPE:
					kbd.OnEscPressed();
					PostQuitMessage(0);
					break;
				}
				break;

			case WM_KEYUP:
				switch (wParam)
				{
				case 0x57:
					kbd.OnWReleased();
					break;
				case 0x53:
					kbd.OnSReleased();
					break;
				case 0x41:
					kbd.OnAReleased();
					break;
				case 0x44:
					kbd.OnDReleased();
					break;
				case 0x4A:
					kbd.OnJReleased();
					break;
				case 0x4B:
					kbd.OnKReleased();
					break;

				case VK_UP:
					kbd.OnUpReleased();
					break;
				case VK_DOWN:
					kbd.OnDownReleased();
					break;
				case VK_LEFT:
					kbd.OnLeftReleased();
					break;
				case VK_RIGHT:
					kbd.OnRightReleased();
					break;
				case VK_RCONTROL:
					kbd.OnRightCtrlReleased();
					break;
				case VK_RSHIFT:
					kbd.OnRightShiftReleased();
					break;

				case VK_SPACE:
					kbd.OnSpaceReleased();
					break;
				case VK_RETURN:
					kbd.OnEnterReleased();
					break;
				case VK_ESCAPE:
					kbd.OnEscReleased();
					break;
				}
				break;
			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			result = 1;
			wasHandled = true;
			break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}
