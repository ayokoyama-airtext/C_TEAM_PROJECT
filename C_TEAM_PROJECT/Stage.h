/**
* @file Stage.h
* @brief �N���X CStage �̐錾
*/
#pragma once
#include "Selector.h"

#ifndef __IGAMEOBJECT_H__
#include "IGameObject.h"
#endif

//  ���g���b�L�[�ȑO���錾
namespace std {
	template<class _Ty> class allocator;
	template <class _Ty, class _Alloc = allocator<_Ty>>class list;
}

enum STAGE_PHASE {
	STAGE_INIT = 0x01,
	STAGE_RUN = 0x02,
	STAGE_DONE = 0x03,
};

enum DRAW_SITUATION {
	DRAW_STATIC_MIN			= 0x01,
	DRAW_CENTERING_PLAYER	= 0x02,
	DRAW_STATIC_MAX			= 0x04,
};

typedef struct {
	FLOAT playerX;
	FLOAT playerY;
	FLOAT playerDrawX;
	FLOAT playerDrawY;
	DRAW_SITUATION XaxisDraw;
	DRAW_SITUATION YaxisDraw;
}	PLAYER_COORDS;


class CPlayer;
class CBG;
struct ID2D1SolidColorBrush;


class CStage : public IGameScene
{
public:
	CStage(CSelector *pSystem);
	virtual ~CStage(void);
	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	ID2D1RenderTarget *GetRenderTarget();

	//	���J�����o�ϐ�
	PLAYER_COORDS playerCoords;

	//	�t�B�[���h�����萔
	static const FLOAT FIELD_WIDTH;
	static const FLOAT FIELD_HEIGHT;
protected:
	CSelector	*m_pSystem;
	CPlayer		*m_pPlayer;
	CBG			*m_pBG;
	STAGE_PHASE	m_ePhase;
	UINT		m_iGameFinishState;

	//	m_iGameFinishState �p�萔
	static const int GAME_CLEAR = 0x02;
	static const int GAME_OVER = 0x04;


#ifdef _DEBUG
	ID2D1SolidColorBrush	*m_pBrush;
#endif
};


//	�萔
#define DISPLAY_RESOLUTION_WIDTH	1920
#define DISPLAY_RESOLUTION_HEIGHT	1080

