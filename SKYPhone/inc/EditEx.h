#if !defined(AFX_EDITEX_H__8D5D35B1_C8DD_11D3_A636_00105A7C2F91__INCLUDED_)
#define AFX_EDITEX_H__8D5D35B1_C8DD_11D3_A636_00105A7C2F91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditEx window
struct CLogFont : public LOGFONT
{
    CLogFont( LONG fontHeight      = -8, 
              LONG fontWeight      = FW_NORMAL,
              UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
              LPCSTR faceName      = _T("MS Sans Serif" )
            )
    {
        const int size = sizeof(*this);

        memset( this, 0, size );

        lfHeight         = fontHeight;
        lfWeight         = fontWeight;
        lfPitchAndFamily = pitchAndFamily;

        _ASSERT( LF_FACESIZE > lstrlen( faceName ) );

        lstrcpy( lfFaceName, faceName );
    }

    // Note: No need for CLogFont& operator =(const CLogFont& lf) {...}
    //       We let the compiler handle it...

    void lfFaceNameSet( LPCSTR faceName )
    {
        _ASSERT( faceName );

        if ( !faceName ) return;

        _ASSERT( LF_FACESIZE > lstrlen( faceName ) );

        lstrcpy( lfFaceName, faceName );        
    }
};

class CEditMask
{
public:
    virtual BOOL AddChar( UINT nChar ) = 0;
};


class CEditEx : public CEdit
{
// Construction
public:
	CEditEx();

    DECLARE_DYNCREATE( CEditEx )

// Copy Semantics

   // Block copy construction...
   //
   // No m_hWnd defined for object. 
   //
private:
   CEditEx( const CEditEx& ); 

public:
   // Allow basics to be copied...
   CEditEx& operator = ( const CEditEx& );

// Attributes
public:

    void setCustomMask( CEditMask* editMask )
    {
        if ( m_pEditMask ) delete m_pEditMask;
        m_pEditMask = editMask;
    } 

    COLORREF bkColor  ( COLORREF );    
    COLORREF textColor( COLORREF );

    COLORREF bkColor()   const { return m_crBkColor;   }   
    COLORREF textColor() const { return m_crTextColor; }

    void setFont( const LOGFONT* lpLogFont );

    void setFont( LONG fontHeight      = -8, 
                  LONG fontWeight      = FW_NORMAL,
                  UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
                  LPCSTR faceName      = _T("MS Sans Serif" ) );
    
    void definePopupMenu( UINT uResourceID = 0 /* 0 uses default */ )
    {
        m_MenuResourceID = uResourceID;
    }

// Operations:
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditEx)
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

    // Customize your brush
    //
    virtual BOOL CreateBrushType();

private:
    CBrush   m_brBkGround;
    COLORREF m_crBkColor;
    COLORREF m_crTextColor;

    CEditMask* m_pEditMask;

    CFont* m_pCFont;

    UINT m_MenuResourceID;
};


class CListBoxEx : public CListBox
{
// Construction
public:
	CListBoxEx();

    DECLARE_DYNCREATE( CListBoxEx )

// Copy Semantics

   // Block copy construction...
   //
   // No m_hWnd defined for object. 
   //
private:
   CListBoxEx( const CListBoxEx& ); 
   
public:
   // Allow basics to be copied...
   CListBoxEx& operator = ( const CListBoxEx& );
   int AddString(LPCTSTR lpszItem);											// Adds a string to the list box
   int AddString(LPCTSTR lpszItem, COLORREF rgb);						// Adds a colored string to the list box

// Attributes
public:

    
    COLORREF bkColor  ( COLORREF );    
    COLORREF textColor( COLORREF );

    COLORREF bkColor()   const { return m_crBkColor;   }   
    COLORREF textColor() const { return m_crTextColor; }

    void setFont( const LOGFONT* lpLogFont );

    void setFont( LONG fontHeight      = -8, 
                  LONG fontWeight      = FW_NORMAL,
                  UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
                  LPCSTR faceName      = _T("MS Sans Serif" ) );
    
    void definePopupMenu( UINT uResourceID = 0 /* 0 uses default */ )
    {
        m_MenuResourceID = uResourceID;
    }

// Operations:
public:
	void SetItemColor(int nIndex, COLORREF rgb);							// Sets the color of an item in the list box
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBoxEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditEx)
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	
	DECLARE_MESSAGE_MAP()

    // Customize your brush
    //
    virtual BOOL CreateBrushType();

private:
    CBrush   m_brBkGround;
    COLORREF m_crBkColor;
    COLORREF m_crTextColor;
    
    CFont* m_pCFont;

    UINT m_MenuResourceID;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITEX_H__8D5D35B1_C8DD_11D3_A636_00105A7C2F91__INCLUDED_)
