#ifndef _TOOLTIP_H_
#define _TOOLTIP_H_

enum
{
	FIRST_TT,
	MAX_TT
};

#ifdef __FL_PACKED_ITEMS
struct PACKAGE_ITEMS
{
	CTexture* pTexture;
	int nAbilityOption;
	int nItemCount;
};
#endif

class CToolTip
{
	BYTE bElementt;

	BOOL        m_bEnable       ;
	BOOL        m_bPutToolTip   ;
	BOOL        m_bReadyToopTip ;
	DWORD       m_dwToolTipId   ;
	CEditString m_strToolTip    ;
	CTimer      m_timerToopTip  ;
	CPoint      m_ptToolTip     ;
	CPoint      m_ptOldToolTip  ;
	CRect       m_rect          ;
	CRect       m_rectRender    ;
	int         m_nPosition     ;
	BOOL        m_bToolTip      ;
	int         m_nAlpha        ;
	CTexture	m_apTextureToolTip[ MAX_TT * 9 ];
	int			m_nAdded;
	int			m_nAddedJewel[5];
	int			m_nSlot;
	CTexture*	m_pUltimateTexture;
	CTexture*	m_pJewelBgTexture;
	int			m_nSubToolTipFlag;
	CRect		m_nRevisedRect;
	int			m_nSubToolTipNumber;
	vector< DWORD > m_vecMapMonsterID;
#ifdef __FL_PACKED_ITEMS
	vector< PACKAGE_ITEMS> m_vecPackage;
#endif
public:
	CToolTip();
	~CToolTip();
	void SetPosition( int nPos ) { m_nPosition = nPos; }
	void SetEnable( BOOL bEnable ) { m_bEnable = bEnable; }
	BOOL GetEnable() { return m_bEnable; }
	void CancelToolTip();

	void PutToolTip( DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos = 0 );
	void PutToolTip( DWORD dwToolTipId, CString& string, CRect rect, CPoint pt, int nToolTipPos = 0 );
	void PutToolTip( DWORD dwToolTipId, LPCTSTR lpszString, CRect rect, CPoint pt, int nToolTipPos = 0 );
	void PutToolTipEx( DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos = 0, int nSubToolTipFlag = 0 );
	void Process( CPoint pt, C2DRender* p2DRender );
	void Paint( C2DRender* p2DRender );
	void Delete();
	void InitTexture();
	void SetUltimateToolTip(CItemElem* pItemBase);
#ifdef __FL_PACKED_ITEMS
	void SetPackageToolTip(CItemElem* pItemBase);
#endif
	void ResizeMapMonsterToolTip( void );
	void InsertMonsterID( DWORD dwMonsterID );
	const CPoint& GetPointToolTip( void ) const;
	const CRect& GetRect( void ) const;
	void SetRenderRect( const CRect& rectRender );
	const CRect& GetRenderRect( void ) const;
	const CRect& GetRevisedRect( void ) const;
	void SetSubToolTipNumber( int nSubToolTipNumber );
	int GetSubToolTipNumber( void ) const;
	BOOL GetReadyToolTipSwitch( void ) const;
	void SetElementTooltip(DWORD dwToolTipId, BYTE bElement);
};
#endif