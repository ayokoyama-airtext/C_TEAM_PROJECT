/**
* @file	My2DAction.cpp
* @author	A.Yokoyama
* @date	05.June.2019
*/
#include "stdafx.h"
#include <crtdbg.h>
#include <tchar.h>
#include <stdio.h>
#include <d2d1.h>
#include <dwrite.h>
#include <cmath>
#include "Selector.h"


#pragma comment(lib,"d2d1.lib")		//	direct2d に必要
#pragma comment(lib,"dwrite.lib")	//	文字表示に必要

#define MAX_LOADSTRING 100

#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if(o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)	if(o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY(o) if(o){ delete [] (o); o = NULL; }


//!	関数WndProcのプロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


//!	グローバル変数
ID2D1Factory			*g_pD2DFactory = NULL;		//	D2D1Factoryオブジェクト
ID2D1HwndRenderTarget	*g_pRenderTarget = NULL;	//	描画ターゲット

double		g_dblDenominator;
double		g_dblFrame;
__int64		g_i64OldTimer;
#define FPS 60.0
#define INTERVAL (1.0/FPS)

CSelector	*g_pSelector = NULL;	//	ゲームシステムオブジェクト

/**
* @fn
* @brief	アプリケーションのエントリーポイント
*/
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	   lpCmdLine,
	int	   nCmdShow)
{
	//! デバッグ用のフラグセット
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CoInitialize(NULL);		// ここからCOM使用可

	//! 変数宣言
	WNDCLASSEX wcex;	//	ウィンドウクラス構造体
	HWND	hWnd;		//	ウィンドウハンドル
	RECT	bounds, client;	//	RECT構造体
	DEVMODE	devMode;


	/*********************
		⑴初期化部分
	*********************/

	//	⑴-a	ウィンドウクラスの登録
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;	//	ウィンドウプロシージャの登録
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;	//	アプリケーションインスタンス
	wcex.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("C_TEAM_PROJECT");	//	ウィンドウクラス名
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wcex);


	

	//	⑴-b ウィンドウの生成
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, wcex.lpszClassName, _T("C_TEAM_PROJECT"),
		WS_VISIBLE | WS_POPUP,
		0, 0, 
		1920,
		1080,
		NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return FALSE;
	



	/*devMode.dmSize = sizeof(DEVMODE);
	devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	devMode.dmPelsWidth = 1920;
	devMode.dmPelsHeight = 1080;
	ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);*/

	// ウィンドウサイズの調整
	GetWindowRect(hWnd, &bounds); // ウィンドウサイズ	:枠を含んだサイズ
	GetClientRect(hWnd, &client); // クライアントサイズ	:枠を含まない表示領域
	MoveWindow(hWnd, bounds.left, bounds.top,
		1920 * 2 - client.right,		// ウィンドウサイズに枠を足す->クライアントサイズを狙ったサイズにできる
		1080 * 2 - client.bottom,
		false);

	// ウィンドウの再表示
	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hWnd);
	//	再表示の際、WndProcが呼び出されることに注意！


	//	⑴-c Direct2Dの初期化
	{
		HRESULT hr;
		//	Direct2D Factory の生成
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
		GetClientRect(hWnd, &client);
		//	RenderTarget のサイズ
		D2D1_SIZE_U size = D2D1::SizeU(
			1920,
			1080
		);

		D2D1_RENDER_TARGET_PROPERTIES target;
		target.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		target.pixelFormat = D2D1::PixelFormat();
		target.usage = D2D1_RENDER_TARGET_USAGE_NONE;
		target.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
		target.dpiX = 0;
		target.dpiY = 0;


		//	Direct2D 描画ターゲット の生成
		hr = g_pD2DFactory->CreateHwndRenderTarget(
			target,
			D2D1::HwndRenderTargetProperties(hWnd, size),
			&g_pRenderTarget
		);
	}


	//	⑴-d タイマーの初期化
	::QueryPerformanceCounter((LARGE_INTEGER*)&g_i64OldTimer);
	__int64		i64Tmp;
	::QueryPerformanceFrequency((LARGE_INTEGER*)&i64Tmp);
	g_dblDenominator = 1.0 / (double)i64Tmp;
	g_dblFrame = 0.0f;

	//	ゲームシステム初期化
	g_pSelector = new CSelector(g_pRenderTarget);

	InvalidateRect(hWnd, NULL, false);


	/**************************
		⑵メッセージループ
	**************************/
	MSG		msg;
	while (true) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {	// PeekMessage() イベントの取り出し
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);		//	ショートカット等の変換
			DispatchMessage(&msg);		//	イベントの配信。WndProc() 呼び出し
		}
		else {

			double	t;
			::QueryPerformanceCounter((LARGE_INTEGER*)&i64Tmp);
			t = (i64Tmp - g_i64OldTimer) * g_dblDenominator;
			g_i64OldTimer = i64Tmp;
			g_dblFrame += t;

			if (g_dblFrame >= INTERVAL) {
				int c = (int)(g_dblFrame / INTERVAL);
				g_dblFrame -= INTERVAL * c;

				if (g_pSelector)
					g_pSelector->doAnim();

				InvalidateRect(hWnd, NULL, false);	//	書き換えの実行
			}
		}
	}



	/**************************
			⑶終了処理
	**************************/

	SAFE_DELETE(g_pSelector);
	SAFE_RELEASE(g_pRenderTarget);
	SAFE_RELEASE(g_pD2DFactory);

	return (int)msg.wParam;
}


/**
* @fn
* @brief	ウィンドウに渡されたイベントのハンドラ
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message) {

	case WM_PAINT:
		if (g_pRenderTarget) {
			g_pRenderTarget->BeginDraw();	//	描画開始

			g_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());	// Transform の設定

			g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::DarkBlue));	//	画面のクリア

			//	この中に描画処理を書く

			if (g_pSelector)
				g_pSelector->doDraw(g_pRenderTarget);

			//	この中に描画処理を書く

			g_pRenderTarget->EndDraw();		//	描画終了
		}
		ValidateRect(hWnd, NULL);
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}