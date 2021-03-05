/*******************************************/
/*                                         */
/* - title : StaticEx.h                    */
/* - description : Static Extends          */
/* - author : you !                        */
/* - date : 2002-09-13                     */
/*                                         */
/*******************************************/

#include "stdafx.h"
#include "StaticEx.h"


/* Constructor (CStaticEx) */
CStaticEx::CStaticEx()
{
	m_textColor = ::GetSysColor(COLOR_WINDOWTEXT);
	m_backColor = ::GetSysColor(COLOR_3DFACE);
	m_backBrush.CreateSolidBrush(m_backColor);
} /* end of constructor (CStaticEx) */


/* Destructor (~CStaticEx) */
CStaticEx::~CStaticEx()
{
	m_backBrush.DeleteObject();
} /* end of destructor (~CStaticEx) */


/* MESSAGE MAP */
BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	//{{AFX_MSG_MAP(CStaticEx)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/* Setting Text Color */
void CStaticEx::SetTextColor(COLORREF Color)
{
	_ASSERT(::IsWindow(m_hWnd));
	m_textColor = Color;
	Invalidate();
}

/* Setting Background Color */
void CStaticEx::SetBkColor(COLORREF Color)
{
	_ASSERT(::IsWindow(m_hWnd));
	m_backColor = Color;
	
	m_backBrush.DeleteObject();
	m_backBrush.CreateSolidBrush(m_backColor);
	Invalidate();
}

/* Draw */
HBRUSH CStaticEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor(m_textColor);
	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)m_backBrush;
}


/* Set Bitmap */
void CStaticEx::SetBitmap(UINT nID, BOOL bStretch, COLORREF transColor)
{
	if (GetBitmap()) ::DeleteObject(GetBitmap());
	ModifyStyle(0, SS_BITMAP);/*|SS_CENTERIMAGE); */
	/* Bitmap Size�� ��ư ũ�⸦ ����ؼ� Bitmap ������ ������ �ʰ� �����. */
	CBitmap BMP;
	BMP.LoadBitmap(nID);
	if (BMP.GetSafeHandle()==NULL) return;
	BITMAP bm;
	BMP.GetBitmap(&bm);
	/* Bitmap Size�� ��ư ũ�⸦ ����ؼ� Bitmap ������ ������ �ʰ� �����. */
	CSize bsize;
	CRect rect;
	GetClientRect(rect); /* Button Size */
	
	if (bStretch) { /* Bitmap ũ�⸦ Button�� �����.*/
		long value1, value2;
		double scale;

		/* Scale */
		bsize.cx = bm.bmWidth;
		bsize.cy = bm.bmHeight;
		if (bsize.cx > bsize.cy) {
			value1 = rect.Width();
			value2 = bsize.cx;
		} else {
			value1 = rect.Height();
			value2 = bsize.cy;
		}
		if (value2)
			scale = (double)value1 / (double)value2;
		else scale = 1;
		bsize.cx = (int)(bsize.cx * scale)-1;
		bsize.cy = (int)(bsize.cy * scale)-1;

	} else {
		bsize.cx = bm.bmWidth;
		bsize.cy = bm.bmHeight;
	} /* end of if */

	CClientDC dc(this);

	CDC dcMem1, dcMem2;
	if (!dcMem1.CreateCompatibleDC(&dc)) return;
	if (!dcMem2.CreateCompatibleDC(&dc)) return;
	
	CBitmap tmpBMP, madeBMP;
	/* �о���� �̹����� �޸� DC�� ��´�. */
	tmpBMP.CreateCompatibleBitmap(&dc, bm.bmWidth, bm.bmHeight);
	dcMem1.SelectObject(&tmpBMP);
	dcMem1.FillSolidRect(0, 0, bm.bmWidth, bm.bmHeight, m_backColor);
	DrawTransparent(&dcMem1, 0, 0, BMP, transColor);
	/* ���� �̹����� ���� �޸� DC�� �ʱ�ȭ�Ѵ�. */
	madeBMP.CreateCompatibleBitmap(&dc, rect.Width(),rect.Height());
	dcMem2.SelectObject(&madeBMP); /* m_bitmap�� dcMem2�� �����Ѵ�. */
	/* ��ư ��� �ʱ�ȭ.... */
	CRect btnRect;
	GetClientRect(&btnRect);
	dcMem2.FillSolidRect(btnRect, m_backColor);
	/* �о���� �̹����� ��ư ������ ���ϴ� ���·� ������Ų��. */
	dcMem2.StretchBlt((rect.Width() - bsize.cx) / 2, (rect.Height() - bsize.cy) / 2, 
					 bsize.cx, bsize.cy,/* rect.Width(), rect.Height(), */
					 &dcMem1, 0, 0, bm.bmWidth, bm.bmHeight,
					 SRCCOPY); /* m_bitmap�� stretch�Ǿ ����.*/
	CStatic::SetBitmap((HBITMAP)madeBMP);
	madeBMP.Detach();
	dcMem1.DeleteDC();
	dcMem2.DeleteDC();
	BMP.DeleteObject();    /* �о���� �̹��� ����  */
	tmpBMP.DeleteObject(); /* �߰� ���� �̹��� ���� */
	madeBMP.DeleteObject();/* ���� ���� �̹��� ���� */
}


/* Draw Trans Parent */
void CStaticEx::DrawTransparent(CDC*pDC, int x, int y, CBitmap& orgBMP, COLORREF crColor)
{
	BITMAP bm;
	orgBMP.GetObject(sizeof(BITMAP), &bm);
	CPoint size(bm.bmWidth, bm.bmHeight);
	pDC->DPtoLP(&size);
	CPoint org(0, 0);
	pDC->DPtoLP(&org);

	/* �޸� DC(dcImage)�� �����ϰ�, �ű⿡ ��Ʈ���� �����Ѵ�. */
	CDC dcImage;
	dcImage.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmapImage = dcImage.SelectObject(&orgBMP);
	dcImage.SetMapMode(pDC->GetMapMode());

	/* �ι�° �޸� DC(dcAn)�� �����ϰ�, �� �ȿ� AND ����ũ�� ����� */
	CDC dcAnd;
	dcAnd.CreateCompatibleDC(pDC);
	dcAnd.SetMapMode(pDC->GetMapMode());

	CBitmap bitmapAnd;
	bitmapAnd.CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	CBitmap* pOldBitmapAnd = dcAnd.SelectObject(&bitmapAnd);

	dcImage.SetBkColor(crColor);
	dcAnd.BitBlt(org.x, org.y, size.x, size.y, &dcImage, org.x, org.y, SRCCOPY);

	/* ����° �޸� DC(dcXor)�� �����ϰ�, �� �ȿ� XOR ����ũ�� ����� */
	CDC dcXor;
	dcXor.CreateCompatibleDC(pDC);
	dcXor.SetMapMode(pDC->GetMapMode());

	CBitmap bitmapXor;
	bitmapXor.CreateCompatibleBitmap(&dcImage, bm.bmWidth, bm.bmHeight);
	CBitmap* pOldBitmapXor = dcXor.SelectObject(&bitmapXor);

	dcXor.BitBlt(org.x, org.y, size.x, size.y, &dcImage, org.x, org.y, SRCCOPY);
	dcXor.BitBlt(org.x, org.y, size.x, size.y, &dcAnd, org.x, org.y, 0x220326);


	/* ���� �簢�� ���� �Ƚ��� �ӽ� �޸� DC(dcTemp)�� �����Ѵ�. */
	CDC dcTemp;
	dcTemp.CreateCompatibleDC(pDC);
	dcTemp.SetMapMode(pDC->GetMapMode());

	CBitmap bitmapTemp;
	bitmapTemp.CreateCompatibleBitmap(&dcImage, bm.bmWidth, bm.bmHeight);
	CBitmap* pOldBitmapTemp = dcTemp.SelectObject(&bitmapTemp);

	dcTemp.BitBlt(org.x, org.y, size.x, size.y, pDC, x, y, SRCCOPY);

	/* AND�� XOR ����ũ�� ������ ������ �̹����� �ӽ� �޸� DC���� �̹����� �����Ѵ�. */
	dcTemp.BitBlt(org.x, org.y, size.x, size.y, &dcAnd, org.x, org.y, SRCAND);

	dcTemp.BitBlt(org.x, org.y, size.x, size.y, &dcXor, org.x, org.y, SRCINVERT);

	/* ȭ�鿡 ��� �̹����� ���Ѵ�.*/
	pDC->BitBlt(x, y, size.x, size.y, &dcTemp, org.x, org.y, SRCCOPY);

	/* �⺻ ��Ʈ���� �����Ѵ�.*/
	dcTemp.SelectObject(pOldBitmapTemp);
	dcXor.SelectObject(pOldBitmapXor);
	dcAnd.SelectObject(pOldBitmapAnd);
	dcImage.SelectObject(pOldBitmapImage);
}