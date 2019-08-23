#pragma once
#include "IGameObject.h"

class CPlayer;
class CStage;
struct ID2D1SolidColorBrush;
struct ID2D1Bitmap;

class CShot :
	public IGameObject
{
public:
	CShot(float x, float y, float vx, float vy);
	virtual ~CShot();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;

	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual void damage(float amount) override;

	static void Restore(CStage *pStage ,CPlayer *pPlayer, ID2D1RenderTarget *pRenderTarget);
	static void Finalize();
protected:
	float	m_fX;
	float	m_fY;
	float	m_fVX;
	float	m_fVY;
	float	m_fRad;

	bool	m_bDamaged;

	//	�萔
	static const int SHOT_SPEED = 20;
	static const int DEFAULT_RAD = 24;	//	�f�t�H���g���a

	//	static�����o(��������Y�ꂸ��)
	static CPlayer		*m_pPlayer;
	static CStage		*m_pParent;
	static ID2D1Bitmap	*m_pImage;

#ifdef _DEBUG
	static ID2D1SolidColorBrush	*m_pBrush;
#endif
};

