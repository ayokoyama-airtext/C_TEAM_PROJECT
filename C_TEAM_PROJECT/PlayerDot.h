/**
*@file		PlayerDot.h
*@author	A.Yokoyama
*@date		09.August.2019
*@brief		�v���C���[�h�b�g�N���X�̐錾
*/
#pragma once
#include "IGameObject.h"

struct ID2D1Bitmap;
class CStage;
class CPlayer;

class CPlayerDot :
	public IGameObject
{
public:
	CPlayerDot();
	virtual ~CPlayerDot();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual void damage(float amount) override;

	virtual int GetState() override;
	virtual float GetX() override;
	virtual float GetY() override;

	static void Restore(ID2D1RenderTarget *pTarget, CStage *pStage, CPlayer *pPlayer);
	static void Finalize();
protected:
	FLOAT	m_fX;		//	x���W
	FLOAT	m_fY;		//	y���W
	FLOAT	m_fRad;		//	���a
	INT		m_iState;	//	�h�b�g�̏��	0:���Ă� / 1:�����Ă�
	INT		m_iNumber;	//	�h�b�g�̔ԍ�
	INT		m_iTimer;

	//	�萔
	static const int	DEFAULT_RAD = 24;	//	�f�t�H���g���a

	//	�ÓI�����o(��������Y�ꂸ��)
	static ID2D1Bitmap	*m_pImage;
	static CStage	*m_pParent;
	static CPlayer	*m_pPlayer;
	static int	m_iCount;	//	�h�b�g�̃C���X�^���X�̐�
#ifdef _DEBUG
	static ID2D1SolidColorBrush	*m_pBrushBlack;
	static ID2D1SolidColorBrush *m_pBrushWhite;
#endif
};

