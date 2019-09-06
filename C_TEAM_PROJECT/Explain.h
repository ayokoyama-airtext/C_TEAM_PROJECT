/**
* @file Explain.h
* @brief �N���XCExplain�̐錾�t�@�C��
* @author A.Yokoyama
*/

#pragma once
#include "IGameScene.h"

enum EXPLAIN_PHASE {
	EXPLAIN_INIT = 1,
	EXPLAIN_RUN_1 = 2,
	EXPLAIN_RUN_2 = 5,
	EXPLAIN_FADE = 3,
	EXPLAIN_DONE = 4,
};

class CSelector;
struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;

class CExplain :
	public IGameScene
{
public:
	CExplain(CSelector *pSystem);
	virtual ~CExplain();
	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
protected:
	CSelector	*m_pSystem;
	EXPLAIN_PHASE	m_ePhase;
	ID2D1Bitmap	*m_pExplainImage;
	ID2D1Bitmap *m_pBGImage;
	ID2D1SolidColorBrush	*m_pBlack;
	bool	m_bFlag;
	bool	m_bPhaseDone;
	INT		m_iTimer;
	INT		m_iFadeTimer;
};

