/**
*@file		Player.h
*@author	A.Yokoyama
*@date		07.August.2019
*@brief		�v���C���[�N���X�̐錾
*@note		Stage.h �̌��#include ���邱��
*/
#pragma once
#include "IGameObject.h"

class CStage;
struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;

class CPlayer :
	public IGameObject
{
public:
	CPlayer(CStage *pStage);
	virtual ~CPlayer();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	void CalcDrawCoord(PLAYER_COORDS *playerCoords);

	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj);
	virtual void damage(float amount) override;
protected:
	CStage	*m_pParent;
	ID2D1Bitmap	*m_pImage;
	
	INT		m_iTimer;
	FLOAT	m_fDrawX;		//	x���W(�`����)
	FLOAT	m_fDrawY;		//	y���W(�`����)
	FLOAT	m_fX;			//	x���W(�f�[�^���)
	FLOAT	m_fY;			//	y���W(�f�[�^���)
	FLOAT	m_fVX;			//	x���������x
	FLOAT	m_fVY;			//	y���������x
	FLOAT	m_fRad;			//	�x���g�̔��a
	FLOAT	m_fCoreRad;	//	�R�A�̔��a
	FLOAT	m_fAngle;		//	��]�p�x (���[���h���W�, ���W�A���P��)->0rad:�E, 0.5PIrad:��, PIrad:��, 1.75PIrad:��
	FLOAT	m_fScale;		//	�傫��(��{1.0f)
	INT		m_iDotNum;		//	�����Ă�h�b�g�̐�
	INT		m_iMaxDotNum;	//	�h�b�g�̍ő��(��������Ƃ��ꂪ������)(�ő�8?)

	bool	m_bDamaged;

	//	�t�B�[���h����
	FLOAT m_fFieldWidth;
	FLOAT m_fFieldHeight;
	

	//	�萔
	static const int PLAYER_START_X = 960;
	static const int PLAYER_START_Y = 540;
	static const int BELT_RAD = 96;		//	�x���g���a
	static const int DOT_RAD = 24;			//	�h�b�g���a
	static const int CORE_LENGTH = 96;		//	�R�A�̈�ӂ̒���
	static const float ROTATION_SPEED;		//	��]���x
	static const float PLAYER_SPEED;		//	�ړ����x

	//	�f�o�b�O�p
#ifdef _DEBUG
	ID2D1SolidColorBrush	*m_pBrush;
	ID2D1SolidColorBrush	*m_pRedBrush;

	//	�f�o�b�O�p�萔
	static const int INC_MAXDOT_KEY = 0x46;	//	F�L�[
	bool	INC_MAXDOT_FLAG = true;
	static const int DEC_MAXDOT_KEY = 0x47;	//	G�L�[
#endif
};



