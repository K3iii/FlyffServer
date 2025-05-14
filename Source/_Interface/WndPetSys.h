#if __VER >= 9 // __CSC_VER9_1

#ifndef __WNDPETSYSTEM__H
#define __WNDPETSYSTEM__H


struct PETCAMTABLE
{
	D3DXVECTOR3 CamPos;
	D3DXVECTOR3 CamLook;
	float Scale;
};

#define MAX_PET_CLASS 7

#if __VER >= 12 // __PET_0519
class CWndPetAwakCancel : public CWndNeuz
{
public:
	CWndText* m_pText;
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;

	CWndPetAwakCancel();
	~CWndPetAwakCancel();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual void OnDestroy();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	void SetDescription();
};
#endif

class CWndPetStatus : public CWndNeuz
{
public:
	BOOL m_bHPVisible;
	BOOL m_bExpVisible;

	//CModel* m_pModel;
	CModelObject* m_pPetModel; // Pet Model Object.
	int	m_nDisplay;
	CItemElem*	m_pPetElem;
	PETCAMTABLE m_PetCameTable[MAX_PET_CLASS];

public:
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	CTexture m_texGauEmptyNormal;
	CTexture m_texGauEmptySmall;
	CTexture m_texGauFillNormal;
	CTexture m_texGauFillSmall;

	CTexture* m_pTexPetLv[6];
	CTexture* m_pTexPetLvBg;
	CTexture* m_pTexPetStatusBg;
	int		  m_aPetLv[6];

#ifndef __AIO_PETS
	LPDIRECT3DVERTEXBUFFER9 m_pVBHPGauge;
#endif
	LPDIRECT3DVERTEXBUFFER9 m_pVBEXPGauge;

	BOOL m_bVBHPGauge;
	BOOL m_bVBEXPGauge;

	int m_nHPWidth;
	int m_nEXPWidth;

	BOOL m_bExtension;
	int m_nLockLevel[2];
	BYTE m_nBackUpLevel[2];
	CString m_strPathLvImage[9];
	int m_nChoiceLevel;

	void MakeGaugeVertex();

	CWndPetStatus();
	virtual ~CWndPetStatus();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL Process();
	virtual void OnMouseWndSurface( CPoint point );

	void DrawPetInformation(C2DRender* p2DRender);
	void SetExtension(BOOL eflag);
	void SetPetCamTable();
	void LockShowLevel(BOOL lFlag, int nLevel, int nPos);
};

#if __VER >= 15 // __PETVIS

class CWndConfirmVis : public CWndNeuz
{
public:
	enum ConfirmVisSection
	{
		CVS_EQUIP_VIS,
		CVS_UNEQUIP_VIS,
		CVS_EQUIP_VISKEY,
		CVS_PICKUP_TO_BUFF,

#ifdef __PROTECT_AWAKE
		ETC_PROTECT_AWAKE,
#endif //__PROTECT_AWAKE
	};
	CWndConfirmVis();
	virtual ~CWndConfirmVis();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void SendEquipPacket( );

 	DWORD m_dwItemId;
 	OBJID m_objid;
 	int m_nSlot;
	DWORD m_dwItemIndex;

	//hm.. need refactoring
	ConfirmVisSection m_eSection;
};

class CWndBuffPetStatus : public CWndNeuz
{
public:

	CWndBuffPetStatus( );
	virtual ~CWndBuffPetStatus( );

	virtual void OnDestroy();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual BOOL Process();

	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
//	virtual	void PaintFrame( C2DRender* p2DRender );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void UpdateVisState( );
	CItemElem* GetItemElemByIndex( const int index );
	BOOL DoModal_ConfirmQuestion( DWORD dwItemId, OBJID objid, DWORD dwIndex, int nSlot = 0,  CWndConfirmVis::ConfirmVisSection eSection = CWndConfirmVis::CVS_EQUIP_VIS );			//OK => SendDoUseItem
	int GetSlotIndexByPoint( const CPoint& point );
	BOOL IsVisItem( DWORD index );
	BOOL IsFreeSlot( const int index );
#ifdef __AIO_PETS
	void SetPet(CMover* pPet);
#endif

protected:
	void DrawSlotItems( C2DRender* p2DRender );

protected:
	CModelObject* m_pPetModel; 

	CItemElem*	m_pItemElem[MAX_VIS];
	int			m_nCtrlId[MAX_VIS];
	CTexture*	m_pTexture[MAX_VIS];

	CWndConfirmVis* m_pWndConfirmVis;

	vector<BYTE> m_cVisStates;
	CTexture* m_pTexPetStatusBg;

	float	m_fRadius;
};

#endif //#ifdef __PETVIS

class CWndFoodConfirm : public CWndNeuz
{
public:
	CItemElem* m_pItemElem;
	CWndEdit* m_pEdit;
	int m_nParent;

	CWndFoodConfirm();
	CWndFoodConfirm(int nParent);
	virtual ~CWndFoodConfirm();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void SetItem(DWORD dwObjId);
};

class CWndPetMiracle : public CWndNeuz
{
public:
	int m_nMiracleLv[2];
	int m_nCount[2];
	int m_nDelay[2];
	int m_nStatus[2];
	int m_nPreLvCount;
	int m_nCurLvCount;
	int m_nResPreLevel;
	int m_nResCurLevel;
	int m_nPetLevel;

	BOOL m_bReciveResult[2];
	BOOL m_bEnd;
	BOOL m_bLocked[2];

	DWORD m_dwObjId;

	CString m_strPetLevel[2];
	CString m_strPathLvImage[9];

	CWndText* m_pText;

public:
	CWndPetMiracle();
	virtual ~CWndPetMiracle();

	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL Process ();

	void ReceiveResult(int nPreLevel, int nCurLevel);
	void PreLevelImgProcess();
	void CurLevelImgProcess();
	void SetDescription(CHAR* szChar);
	void SetItem(DWORD dwObjId);
};

class CWndPetFoodMill : public CWndNeuz
{
public:
	OBJID m_dwObjId;
	CItemElem* m_pItemElem;
	CTexture*	m_pTexture;

	CWndFoodConfirm* m_pWndFoodConfrim;
	int m_nItemCount;
public:
	CWndPetFoodMill();
	virtual ~CWndPetFoodMill();

	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
	void SetItem(DWORD dwObjId);
	void SetItemForFeed(CItemElem* pItemElem, int nCount);
	void ReceiveResult(int nResult, int nCount);
};

class CWndPetLifeConfirm : public CWndNeuz
{
public:
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;
	int m_nId;

protected:
	CString m_strText;
	CWndText m_wndText;

public:
	CWndPetLifeConfirm();
	~CWndPetLifeConfirm();

	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void SetItem(int nId);
};

#endif //__WNDPETSYSTEM__H
#endif //__CSC_VER9_1