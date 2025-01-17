#pragma once

class CWater
{
public:
	CWater(void);
	~CWater(void);
	void Create(int iWidth,int iHeight);
	void Render(DWORD* pSrcImage,DWORD* pTargetImage);

	void CalcWater(int npage, int density);
	void SmoothWater(int npage);
	void FlattenWater();

	void SineBlob(int x, int y, int radius, int height, int page);
	void WarpBlob(int x, int y, int radius, int height, int page);
	void HeightBox (int x, int y, int radius, int height, int page);
	void HeightBlob(int x, int y, int radius, int height, int page);
	void CalcWaterBigFilter(int npage, int density);

	void DrawWaterNoLight(int page,DWORD* pSrcImage,DWORD* pTargetImage);
	void DrawWaterWithLight(int page, int LightModifier,DWORD* pSrcImage,DWORD* pTargetImage);
	COLORREF GetShiftedColor(COLORREF color,int shift);

	int			m_iWidth;
	int			m_iHeight;
	BOOL		m_bDrawWithLight;
	int			m_iLightModifier;
	int			m_iHpage;// The current heightfield
	int			m_density;// The water density - can change the density...
	//  the height fields
	int*		m_iHeightField1;
	int*		m_iHeightField2;
};
