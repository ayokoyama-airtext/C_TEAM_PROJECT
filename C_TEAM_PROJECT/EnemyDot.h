/**
*@file		EnemyDot.h
*@author	A.Yokoyama
*@date		11.August.2019
*@brief		�G�l�~�[�h�b�g�N���X�̐錾
*/
#pragma once
#include "IGameObject.h"

struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;
class CStage;

class CEnemyDot :
	public IGameObject
{
public:
	CEnemyDot(IGameObject *pParentEnemy, int dotNumber, int maxDotNum);
	virtual ~CEnemyDot();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual void damage(float amount) override;

	virtual int GetNumber() override;

	virtual bool IsDead();
	virtual void SetStateZero();
	virtual int GetState() override;

	static void Restore(ID2D1RenderTarget *pTarget, CStage *pStage);
	static void Finalize();
protected:
	FLOAT	m_fX;			//	x���W
	FLOAT	m_fY;			//	y���W
	FLOAT	m_fRad;			//	���a
	INT		m_iState;		//	�h�b�g�̏��	0:���Ă� / 1:�����Ă�
	INT		m_iNumber;		//	�h�b�g�̔ԍ�
	INT		m_iMaxDotNum;	//	�������Ă���G�l�~�[�̍ő�h�b�g��
	INT		m_iTimer;		//	�A�j���[�V�����Ɏg�p
	bool	m_bDamaged;		//	�e�G�l�~�[���h�b�g�̐�������Ɏg�p

	IGameObject	*m_pParent;	//	���̃h�b�g�������Ă���G�l�~�[

	//	�萔
	static const int	DEFAULT_RAD = 24;	//	�f�t�H���g���a

	//	�ÓI�����o(��������Y�ꂸ��)
	static ID2D1Bitmap	*m_pImage;
	static CStage	*m_pStage;
#ifdef _DEBUG
	static ID2D1SolidColorBrush	*m_pBrush;
#endif
};

