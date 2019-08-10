/**
*@file		Enemy.h
*@author	A.Yokoyama
*@date		10.August.2019
*@brief		�G�l�~�[�N���X�̐錾
*/
#pragma once
#include "IGameObject.h"

struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;
class CStage;

class CEnemy :
	public IGameObject
{
public:
	CEnemy(float x, float y, float scale);
	virtual ~CEnemy();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual void damage(float amount) override;

	static void Restore(ID2D1RenderTarget *pTarget, CStage *pStage);
	static void Finalize();
protected:

	FLOAT	m_fX;			//	x���W(�f�[�^���)
	FLOAT	m_fY;			//	y���W(�f�[�^���)
	FLOAT	m_fVX;			//	x���������x
	FLOAT	m_fVY;			//	y���������x
	FLOAT	m_fRad;			//	�x���g�̔��a
	FLOAT	m_fAngle;		//	��]�p�x (���[���h���W�, ���W�A���P��)->0rad:�E, 0.5PIrad:��, PIrad:��, 1.75PIrad:��
	FLOAT	m_fScale;		//	�傫��(��{1.0f)
	INT		m_iDotNum;		//	�����Ă�h�b�g�̐�
	INT		m_iMaxDotNum;	//	�h�b�g�̍ő��(��������Ƃ��ꂪ������)(�ő�8?)

	bool	m_bDamaged;



	//	�萔
	static const int BELT_RAD = 96;		//	�x���g���a
	static const int DOT_RAD = 24;			//	�h�b�g���a
	static const int CORE_LENGTH = 96;		//	�R�A�̈�ӂ̒���
	static const float ROTATION_SPEED;		//	��]���x
	static const float ENEMY_SPEED;		//	�ړ����x


	//	�ÓI�����o(��������Y�ꂸ��)
	static CStage			*m_pParent;
	static ID2D1Bitmap		*m_pImage;
	static FLOAT m_fFieldWidth;
	static FLOAT m_fFieldHeight;
#ifdef _DEBUG
	static ID2D1SolidColorBrush	*m_pBrush;
	static ID2D1SolidColorBrush	*m_pRedBrush;
#endif // _DEBUG
};

