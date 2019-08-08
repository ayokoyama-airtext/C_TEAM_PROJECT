#pragma once
#include "IGameScene.h"

enum ResultPhase {
	RESULT_INIT = 0,
	RESULT_RUN = 1,
	RESULT_FADE = 2,
	RESULT_DONE = 3
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
	ID2D1Bitmap	*m_pImage;
	ID2D1SolidColorBrush	*m_pBlack;
	bool	m_bFlag;
	INT		m_iTimer;
	INT		m_iFadeTimer;
};

