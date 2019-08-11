#pragma once

class CStage;
struct ID2D1RenderTarget;
struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;

class CUI
{
public:
	CUI(CStage *pStage);
	virtual ~CUI();
	virtual void move();
	virtual void draw(ID2D1RenderTarget *pRenderTarget);
protected:
	CStage *m_pParent;
	ID2D1Bitmap	*m_pImage;	//	画像ファイルの読み込み先

	UINT	m_iScore;	//	スコア
	UINT	m_iTimer;	//	経過時間

#ifdef _DEBUG
	ID2D1SolidColorBrush	*m_pBrush;	//	デバッグ用のブラシ
#endif // _DEBUG

};

