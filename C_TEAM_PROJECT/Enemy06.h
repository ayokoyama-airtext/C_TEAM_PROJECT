/**
*@file		Enemy06.h
*@author
*@date		11.August.2019
*@brief		�G�l�~�[06�N���X�̐錾
*/
#pragma once
#include "IGameObject.h"
#include <vector>

struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;
class CStage;
class CEnemyDot;


class CEnemy06 :
	public IGameObject
{
public:
	CEnemy06(float x, float y, float scale);
	virtual ~CEnemy06();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual void damage(float amount) override;

	virtual float GetX() override;
	virtual float GetY() override;
	virtual float GetRad() override;
	virtual float GetAngle() override;

	static void Restore(ID2D1RenderTarget *pTarget, CStage *pStage);
	static void Finalize();
protected:

	FLOAT	m_fX;			//	x���W(�f�[�^���)
	FLOAT	m_fY;			//	y���W(�f�[�^���)
	FLOAT	m_fVX;			//	x���������x
	FLOAT	m_fVY;			//	y���������x
	FLOAT	m_fRad;			//	�x���g�̔��a
	FLOAT	m_fCoreRad;		//	�R�A���a
	FLOAT	m_fAngle;		//	��]�p�x (���[���h���W�, ���W�A���P��)->0rad:�E, 0.5PIrad:��, PIrad:��, 1.75PIrad:��
	FLOAT	m_fScale;		//	�傫��(��{1.0f)
	INT		m_iDotNum;		//	�����Ă�h�b�g�̐�
	INT		m_iMaxDotNum;	//	�h�b�g�̍ő��(��������Ƃ��ꂪ������)(�ő�8?)

	std::vector<CEnemyDot*>	m_pDots;	//	�ێ����Ă���h�b�g�ւ̃|�C���^�̔z��

	bool	m_bDamaged;
	INT		m_iTimer;
	INT		m_iDamagedTimer;	//	���G���Ԃ̊Ǘ�

	int		m_iBehaviorFlag;	//	�G�l�~�[�̍s����Ԃ��Ǘ�����t���O
	//	m_iBehaviorFlag�p�萔
	static const int EFLAG_IDLE = 0x01;		//	��~
	static const int EFLAG_SEARCH = 0x02;	//	���G
	static const int EFLAG_CHASE = 0x04;	//	�ǐ�
	static const int EFLAG_ESCAPE = 0x08;	//	����

	//	�萔
	static const int BELT_RAD = 96;				//	�x���g���a
	static const int DOT_RAD = 24;				//	�h�b�g���a
	static const int CORE_LENGTH = 96;			//	�R�A�̈�ӂ̒���
	static const float ROTATION_SPEED;			//	��]���x
	static const float ENEMY_SPEED;				//	�ړ����x
	static const float ENEMY_ESCAPE_SPEED;		//	�������x
	static const float ENEMY_ESCAPE_ROTATION_SPEED;
	static const float ENEMY_ESCAPE_ANGLE;
	static const float SEARCH_ANGLE;			//	���G�p�x
	static const int SEARCH_LENGTH = 1200;	//	���G����
	static const int ESCAPE_DURATION = 90;		//	��������
	static const int DAMAGED_DURATION = 30;		//	���G����


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

