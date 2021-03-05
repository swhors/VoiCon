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
	/* Bitmap Size와 버튼 크기를 고려해서 Bitmap 원형이 깨지지 않게 만든다. */
	CBitmap BMP;
	BMP.LoadBitmap(nID);
	if (BMP.GetSafeHandle()==NULL) return;
	BITMAP bm;
	BMP.GetBitmap(&bm);
	/* Bitmap Size와 버튼 크기를 고려해서 Bitmap 원형이 깨지지 않게 만든다. */
	CSize bsize;
	CRect rect;
	GetClientRect(rect); /* Button Size */
	
	if (bStretch) { /* Bitmap 크기를 Button에 맞춘다.*/
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
	/* 읽어들인 이미지를 메모리 DC에 담는다. */
	tmpBMP.CreateCompatibleBitmap(&dc, bm.bmWidth, bm.bmHeight);
	dcMem1.SelectObject(&tmpBMP);
	dcMem1.FillSolidRect(0, 0, bm.bmWidth, bm.bmHeight, m_backColor);
	DrawTransparent(&dcMem1, 0, 0, BMP, transColor);
	/* 최종 이미지를 위한 메모리 DC를 초기화한다. */
	madeBMP.CreateCompatibleBitmap(&dc, rect.Width(),rect.Height());
	dcMem2.SelectObject(&madeBMP); /* m_bitmap와 dcMem2를 연결한다. */
	/* 버튼 배경 초기화.... */
	CRect btnRect;
	GetClientRect(&btnRect);
	dcMem2.FillSolidRect(btnRect, m_backColor);
	/* 읽어들인 이미지를 버튼 영역에 원하는 형태로 변형시킨다. */
	dcMem2.StretchBlt((rect.Width() - bsize.cx) / 2, (rect.Height() - bsize.cy) / 2, 
					 bsize.cx, bsize.cy,/* rect.Width(), rect.Height(), */
					 &dcMem1, 0, 0, bm.bmWidth, bm.bmHeight,
					 SRCCOPY); /* m_bitmap로 stretch되어서 들어간다.*/
	CStatic::SetBitmap((HBITMAP)madeBMP);
	madeBMP.Detach();
	dcMem1.DeleteDC();
	dcMem2.DeleteDC();
	BMP.DeleteObject();    /* 읽어들인 이미지 해제  */
	tmpBMP.DeleteObject(); /* 중간 가공 이미지 해제 */
	madeBMP.DeleteObject();/* 최종 가공 이미지 해제 */
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

	/* 메모리 DC(dcImage)를 생성하고, 거기에 비트맵을 선택한다. */
	CDC dcImage;
	dcImage.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmapImage = dcImage.SelectObject(&orgBMP);
	dcImage.SetMapMode(pDC->GetMapMode());

	/* 두번째 메모리 DC(dcAn)를 생성하고, 그 안에 AND 마스크를 만든다 */
	CDC dcAnd;
	dcAnd.CreateCompatibleDC(pDC);
	dcAnd.SetMapMode(pDC->GetMapMode());

	CBitmap bitmapAnd;
	bitmapAnd.CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	CBitmap* pOldBitmapAnd = dcAnd.SelectObject(&bitmapAnd);

	dcImage.SetBkColor(crColor);
	dcAnd.BitBlt(org.x, org.y, size.x, size.y, &dcImage, org.x, org.y, SRCCOPY);

	/* 세번째 메모리 DC(dcXor)를 생성하고, 그 안에 XOR 마스크를 만든다 */
	CDC dcXor;
	dcXor.CreateCompatibleDC(pDC);
	dcXor.SetMapMode(pDC->GetMapMode());

	CBitmap bitmapXor;
	bitmapXor.CreateCompatibleBitmap(&dcImage, bm.bmWidth, bm.bmHeight);
	CBitmap* pOldBitmapXor = dcXor.SelectObject(&bitmapXor);

	dcXor.BitBlt(org.x, org.y, size.x, size.y, &dcImage, org.x, org.y, SRCCOPY);
	dcXor.BitBlt(org.x, org.y, size.x, size.y, &dcAnd, org.x, org.y, 0x220326);


	/* 목적 사각형 안의 픽실을 임시 메모리 DC(dcTemp)로 복사한다. */
	CDC dcTemp;
	dcTemp.CreateCompatibleDC(pDC);
	dcTemp.SetMapMode(pDC->GetMapMode());

	CBitmap bitmapTemp;
	bitmapTemp.CreateCompatibleBitmap(&dcImage, bm.bmWidth, bm.bmHeight);
	CBitmap* pOldBitmapTemp = dcTemp.SelectObject(&bitmapTemp);

	dcTemp.BitBlt(org.x, org.y, size.x, size.y, pDC, x, y, SRCCOPY);

	/* AND와 XOR 마스크를 적용한 마지막 이미지를 임시 메모리 DC안의 이미지로 생성한다. */
	dcTemp.BitBlt(org.x, org.y, size.x, size.y, &dcAnd, org.x, org.y, SRCAND);

	dcTemp.BitBlt(org.x, org.y, size.x, size.y, &dcXor, org.x, org.y, SRCINVERT);

	/* 화면에 결과 이미지를 블릿한다.*/
	pDC->BitBlt(x, y, size.x, size.y, &dcTemp, org.x, org.y, SRCCOPY);

	/* 기본 비트맵을 복원한다.*/
	dcTemp.SelectObject(pOldBitmapTemp);
	dcXor.SelectObject(pOldBitmapXor);
	dcAnd.SelectObject(pOldBitmapAnd);
	dcImage.SelectObject(pOldBitmapImage);
}