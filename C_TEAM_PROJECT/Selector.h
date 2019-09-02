/**
* @file Selector.h
* @brief クラスCSelectorの宣言ファイル
*/
#pragma once

#ifndef __IGAMESCENE_H__
#include "IGameScene.h"
#endif

enum GamePhase {
	GAMEPHASE_INIT		= 0,
	GAMEPHASE_RESET		= 1,
	GAMEPHASE_TITLE		= 0x010,
	GAMEPHASE_EXPLAIN	= 0x020,
	GAMEPHASE_GAME		= 0x100,
	GAMEPHASE_GAMEOVER	= 0x200,
	GAMEPHASE_RESULT	= 0x040,
};

enum GameSceneResultCode {
	GAMESCENE_DEFAULT		= 0,
	GAMESCENE_END_OK		= 1,
	GAMESCENE_END_TIMEOUT	= 2,
	GAMESCENE_END_FAILURE	= 3
};

class IGameScene;	// シーン管理クラス
struct ID2D1RenderTarget;
struct IDWriteTextFormat;
struct ID2D1SolidColorBrush;

class CSelector
{
public:
	CSelector(ID2D1RenderTarget *pRenderTarget);
	virtual ~CSelector(void);
	void doAnim();		//	ゲーム全体のアニメート
	void doDraw(ID2D1RenderTarget *pRenderTarget);	//	ゲーム全体の描画

	void SetScoreAndTime(int time, int score) { m_iEndTime = time; m_iEndScore = score; }
	int GetEndTime() { return m_iEndTime; }
	int GetEndScore() { return m_iEndScore; }

	ID2D1RenderTarget	*GetRenderTarget();
	IDWriteTextFormat	*GetTextFormat();
protected:
	ID2D1RenderTarget		*m_pRenderTarget;
	int	m_iEndTime;		//	ゲームクリア/オーバー 時のタイム
	int m_iEndScore;	//	ゲームクリア/オーバー 時のスコア

	IGameScene		*m_pScene;		//	シーン
	GamePhase	m_eGamePhase;	//	状態変数
	INT	m_iCount;	//	カウンター(デバッグ用)

	INT m_iWait;

#ifdef _DEBUG
	IDWriteTextFormat		*m_pTextFormat;
	IDWriteTextFormat		*m_pTextFormat2;
	ID2D1SolidColorBrush	*m_pWhiteBrush;
	ID2D1SolidColorBrush	*m_pRedBrush;
#endif
};

//	マクロ定義
#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if(o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)	if(o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY if(o){ delete [] (o); o = NULL; }
