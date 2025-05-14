
//Guild house windows
//date : 2009_10_26
//author : gmpbigsun

#pragma  once

#if __VER >= 15 // __GUILD_HOUSE

#include "WndHousing.h"
#include "GuildHouse.h"

class CWndGHTeleporter;
class CWndGHUpkeep;
class CWndGHMainMenu : public CWndNeuz			//GH : Guild House
{

public:
	CWndGHMainMenu( );
	virtual ~CWndGHMainMenu( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
//	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	BOOL Process();

	void RefreshWndTelelpoter( );

protected:
	CWndButton* m_pSetupPower;
	CWndButton* m_pFurniture;
	CWndButton* m_pStorage;
	CWndButton* m_pTeleporter;
	CWndButton* m_pUpkeep;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndGHTeleporter
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_TELEPORTER_SLOT  (5)

class CWndGHTeleporter : public CWndNeuz
{


public:
	CWndGHTeleporter( );
	virtual ~CWndGHTeleporter( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate();
//	virtual void OnMouseWndSurface( CPoint point );

	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();


	int GetSlotIndexByPoint( const CPoint& point );
	void RefreshItems( );
	void UpdateButtons( );

protected:
	void DrawSlotItems( C2DRender* p2DRender );
	GH_Fntr_Info* GetInstalledItem( OUT int& serverSlot );

	int GetCountListUp( );

protected:
	CWndButton* m_pOK;
	CWndButton* m_pFire;

	int			m_nCtrlId[MAX_TELEPORTER_SLOT];
	CTexture*	m_pTexture[MAX_TELEPORTER_SLOT];
	vector<HOUSING_ITEM>	m_cWndItems;
	int			m_nSelectedSlot;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CWndGHUpkeep
////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CWndGHUpkeep : public CWndNeuz
{
public:
	CWndGHUpkeep();
	virtual ~CWndGHUpkeep();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

protected:
	void		OnOK();
	void		OnChangeBuyCount( DWORD dwBuy );

public:
	CWndEdit*		m_pEdit;
	CWndStatic*		m_pStatic;
	CWndStatic*		m_pStaticGold;
#if __VER >= 11 // __CSC_VER11_3
	int				m_nBuyType;
#endif //__CSC_VER11_3
};


#endif 