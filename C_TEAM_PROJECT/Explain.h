#pragma once
#include "IGameScene.h"

enum EXPLAIN_PHASE {
	EXPLAIN_INIT = 0x01,
	EXPLAIN_RUN = 0x02,
	EXPLAIN_FADE = 0x03,
	EXPLAIN_DONE = 0x04,
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
	ID2D1Bitmap	*m_pImage;
	ID2D1SolidColorBrush	*m_pBlack;
	bool	m_bFlag;
	INT		m_iTimer;
	INT		m_iFadeTimer;
};

