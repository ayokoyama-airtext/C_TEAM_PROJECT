/**
* @file Selector.h
* @brief �N���XCSelector�̐錾�t�@�C��
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

class IGameScene;	// �V�[���Ǘ��N���X
struct ID2D1RenderTarget;
struct IDWriteTextFormat;
struct ID2D1SolidColorBrush;

class CSelector
{
public:
	CSelector(ID2D1RenderTarget *pRenderTarget);
	virtual ~CSelector(void);
	void doAnim();		//	�Q�[���S�̂̃A�j���[�g
	void doDraw(ID2D1RenderTarget *pRenderTarget);	//	�Q�[���S�̂̕`��

	void SetScoreAndTime(int time, int score) { m_iEndTime = time; m_iEndScore = score; }
	int GetEndTime() { return m_iEndTime; }
	int GetEndScore() { return m_iEndScore; }

	ID2D1RenderTarget	*GetRenderTarget();
	IDWriteTextFormat	*GetTextFormat();
protected:
	ID2D1RenderTarget		*m_pRenderTarget;
	int	m_iEndTime;		//	�Q�[���N���A/�I�[�o�[ ���̃^�C��
	int m_iEndScore;	//	�Q�[���N���A/�I�[�o�[ ���̃X�R�A

	IGameScene		*m_pScene;		//	�V�[��
	GamePhase	m_eGamePhase;	//	��ԕϐ�
	INT	m_iCount;	//	�J�E���^�[(�f�o�b�O�p)

	INT m_iWait;

#ifdef _DEBUG
	IDWriteTextFormat		*m_pTextFormat;
	IDWriteTextFormat		*m_pTextFormat2;
	ID2D1SolidColorBrush	*m_pWhiteBrush;
	ID2D1SolidColorBrush	*m_pRedBrush;
#endif
};

//	�}�N����`
#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if(o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)	if(o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY if(o){ delete [] (o); o = NULL; }
