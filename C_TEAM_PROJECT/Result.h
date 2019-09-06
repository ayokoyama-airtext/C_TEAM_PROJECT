/**
* @file Result.h
* @brief クラスCResultの宣言ファイル
* @author A.Yokoyama
*/
#pragma once
#include "IGameScene.h"

enum ResultPhase {
	RESULT_INIT = 0,
	RESULT_RUN = 1,
	RESULT_END_OK = 2,
	RESULT_FADE = 3,
	RESULT_DONE = 4
};

class CSelector;
struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;

class CResult :
	public IGameScene
{
public:
	CResult(CSelector *pSystem);
	virtual ~CResult();
	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
protected:
	CSelector	*m_pSystem;
	ResultPhase	m_ePhase;
	ID2D1Bitmap	*m_pBGImage;
	ID2D1Bitmap *m_pRankImage;
	ID2D1Bitmap *m_pDigitImage;
	ID2D1Bitmap *m_pScoreImage;
	ID2D1Bitmap *m_pTextRankImage;
	ID2D1Bitmap *m_pTextTimeImage;
	ID2D1Bitmap *m_pBackToTitleImage;
	ID2D1Bitmap *m_pTextBackToTitleImage;
	ID2D1Bitmap	*m_pTextResultImage;


	ID2D1SolidColorBrush	*m_pBlack;
	bool	m_bFlag;
	INT		m_iTimer;
	INT		m_iFadeTimer;
	INT		m_iScore;
	INT		m_iEndTime;

	float	m_fBGX;
};

