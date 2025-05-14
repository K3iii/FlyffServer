#if !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "post.h"
#include "guild.h"

#if __VER >= 13 // __CSC_VER13_2
#include "couple.h"
#include "couplehelper.h"
#endif //__CSC_VER13_2

#ifdef __FL_PARTY_FINDER
#include "party.h"
#endif

const int MAX_WANTED_LIST = 100;
extern 	int g_nSkillCurSelect;

#if __VER >= 13 // __HONORABLE_TITLE
#include "honor.h"
#endif	// __HONORABLE_TITLE

#ifdef __FL_WHEEL_OF_FORTUNE
#include "WheelOfFortune.h"
#endif

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

#ifdef __MAIL_REQUESTING_BOX
#include "WndMailRequestingBox.h"
#endif // __MAIL_REQUESTING_BOX
////////////////////////////////////////////////////////////////////////////////

#define POINT_TO_SCREEN( rect, p ) CPoint point2 = p; ClientToScreen( &point2 ); CRect DrawRect = rect; ClientToScreen( &DrawRect );
#define PUT_TOOL_TIP( rect, p, itm ) POINT_TO_SCREEN( rect, p ) g_WndMng.PutToolTip_Item( itm, point2, &DrawRect );


class CWndApplet : public CWndNeuz
{
public:
	CWndListBox m_wndList;
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;
	CWndButton m_wndButton3;

	CWndApplet();
	virtual ~CWndApplet();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

};

////////////////////////////////////////////////////////////////////////////////


class CWndDropItem : public CWndNeuz
{
public:
	CItemElem* m_pItemElem;
	D3DXVECTOR3 m_vPos;
	CWndEdit* m_pEdit;

	CWndDropItem();
	virtual ~CWndDropItem();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
#if __VER >= 14 // __DROP_CONFIRM_BUG
	virtual BOOL Process(void);
#endif // __DROP_CONFIRM_BUG
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndDropConfirm : public CWndNeuz
{
public:
	CItemElem* m_pItemElem;
	D3DXVECTOR3 m_vPos;

	CWndDropConfirm();
	virtual ~CWndDropConfirm();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
#if __VER >= 14 // __DROP_CONFIRM_BUG
	virtual BOOL Process(void);
#endif // __DROP_CONFIRM_BUG
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndRandomScrollConfirm : public CWndNeuz
{
public:
	BOOL bFlag;
	OBJID objid, objid1;
	void SetItem(OBJID objidBuf, OBJID objid1Buf, BOOL bFlagBuf = FALSE);
public:
	CWndRandomScrollConfirm();
	virtual ~CWndRandomScrollConfirm();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndQuestItemWarning : public CWndNeuz
{
public:
	CItemElem* m_pItemElem;
	D3DXVECTOR3 m_vPos;

	CWndQuestItemWarning();
	virtual ~CWndQuestItemWarning();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};


class CWndGold : public CWndButton
{
public:
	CTexture m_texture;
	CWndGold();
	virtual ~CWndGold();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual	void OnInitialUpdate();
};

class CWndConfirmBuy;

#ifdef __CONTAINER_SORTING
struct sItem
{
	DWORD dwId;
	DWORD dwKind2;
	DWORD dwItemId;
	BYTE nIndex;
	sItem()
	{
		dwId = dwKind2 = dwItemId = nIndex = 0;
	}
	bool operator < (const sItem p2)
	{
		if (dwKind2 == p2.dwKind2)
		{
			return dwItemId < p2.dwItemId;
		}
		else
		{
			return dwKind2 < p2.dwKind2;
		}
	}
};

class CInventorySort0
{
public:
	CInventorySort0()
	{
		m_dwPos = 0;
	}
	~CInventorySort0() {}
private:
	sItem m_Item[MAX_INVENTORY];
	DWORD m_dwPos;
public:
	void Add(BYTE nIndex)
	{
		if (m_dwPos >= MAX_INVENTORY)
		{
			return;
		}
		m_Item[m_dwPos].nIndex = nIndex;
		m_Item[m_dwPos].dwId = m_dwPos;
		m_dwPos++;
	}

	BYTE GetItemSrc(DWORD dwId)
	{
		for (int i = 0; i < MAX_INVENTORY; i++)
		{
			if (m_Item[i].dwId == dwId)
			{
				return m_Item[i].nIndex;
			}
		}
		return 255;
	}

	void MoveItem(DWORD dwSrcId, BYTE dest)
	{
		BYTE nTmp = 0;
		bool bDest = false, bSrc = false;
		for (int i = 0; i < MAX_INVENTORY; i++)
		{
			if (dwSrcId == m_Item[i].dwId)
			{
				nTmp = m_Item[i].nIndex;
				m_Item[i].nIndex = dest;
			}
		}
		for (int i = 0; i < MAX_INVENTORY; i++)
		{
			if (dest == m_Item[i].nIndex)
			{
				m_Item[i].nIndex = nTmp;
			}
		}
	}
};

#endif

class CWndInventory : public CWndNeuz
{
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	CModelObject* m_pModel;
	BOOL				m_bLButtonDownRot;

	CRect		 m_InvenRect[MAX_HUMAN_PARTS];

	CItemElem* m_pSelectItem;
	BOOL m_bReport;

	CTexture* m_TexRemoveItem;
#ifdef __INVENTORY_RESIZE
	enum ctrltype
	{
		PARENT, STATIC, CUSTOM, CHECKBOX
	};

	class ctrlhelp
	{
	public:
		int id;
		ctrltype type;
		CRect rect;

		ctrlhelp(int id, ctrltype type, CRect rect)
		{
			this->id = id;
			this->type = type;
			this->rect = rect;
		}
	};

	vector<ctrlhelp> arOriginalSizes;

	const int nGridTexSize = 32;
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize);
	CRect getItemRect(int id, CPoint sizediff = CPoint(0, 0), ctrltype type = PARENT);
	void saveOriginRects();
	void resetItemRects();
#endif
#ifdef __MODEL_HIDEFASH
	CWndButton			m_awndCheckHide[6];
#endif
public:
	void UpDateModel();

	void UpdateParts();
	CModelObject* GetModel() { return m_pModel; }
	void BaseMouseCursor();
	void RunUpgrade(CItemElem* pItem);
#ifdef __INVENTORY_RESIZE
	void refreshItemRects();
#endif

	CSfx* m_pSfxUpgrade;
	CItemElem* m_pUpgradeItem;
	CItemElem* m_pUpgradeMaterialItem;
	BOOL		m_bIsUpgradeMode;
	DWORD		m_dwEnchantWaitTime;

	CWndConfirmBuy* m_pWndConfirmBuy;
	CWndGold     m_wndGold;
	CWndItemCtrl m_wndItemCtrl;

	CWndInventory();
	virtual ~CWndInventory();
	virtual BOOL Process();
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
	virtual void OnDestroy(void);
	virtual BOOL OnSetCursor(CWndBase* pWndBase, UINT nHitTest, UINT message);
#ifdef __FL_BOXES_V2
	CItemElem* m_pSelectedBox;
	CWndMenu m_wndMenuBoxOpen;
#endif
	void DeleteUpgradeSfx();

#if __VER >= 9 // __CSC_VER9_1
public:
	void UpdateTooltip(void) { m_wndItemCtrl.UpdateTooltip(); }
#endif //__CSC_VER9_1

};

class CWndQueryEquip : public CWndNeuz
{
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	CMover m_Mover;
	BOOL				m_bLButtonDownRot;
	CRect		 m_InvenRect[MAX_HUMAN_PARTS];

	CItemElem	m_aEquipInfoAdd[MAX_HUMAN_PARTS];
public:

	void	SetModel(CMover* pMover);
	CMover* GetMover() { return &m_Mover; };

	void	SetEquipInfoAdd(CItemElem* aEquipInfoAdd);

	CWndQueryEquip();
	virtual ~CWndQueryEquip();
	virtual BOOL Process();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnMouseWndSurface(CPoint point);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};


////////////////////////////////////////////////////////////////////////////////


class CWndStateConfirm : public CWndNeuz
{
public:
	UINT m_nId;
	void OnSetState(UINT nId);
public:
	CWndStateConfirm();
	virtual ~CWndStateConfirm();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};
class CWndCharacterBase : public CWndBase
{
	CTexture m_txInfo;

public:
	CWndCharacterBase();
	virtual ~CWndCharacterBase();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndCharacterDetail : public CWndBase
{
	CWndButton m_wndStr;
	CWndButton m_wndSta;
	CWndButton m_wndDex;
	CWndButton m_wndInt;

	int			m_nCnt;

public:
	BOOL	m_fWaitingConfirm;
	CWndCharacterDetail();
	virtual ~CWndCharacterDetail();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);

protected:
	void RenderATK(C2DRender* p2DRender, int x, int y);
};

#if __VER >= 9 // __CSC_VER9_2
class CWndCharacterDetail2 : public CWndBase
{
public:
	CWndButton m_wndStrPlus, m_wndStrMinus;
	CWndButton m_wndStaPlus, m_wndStaMinus;
	CWndButton m_wndDexPlus, m_wndDexMinus;
	CWndButton m_wndIntPlus, m_wndIntMinus;

	CWndButton m_wndApply, m_wndReset;

	CWndEdit m_editStrCount;
	CWndEdit m_editStaCount;
	CWndEdit m_editDexCount;
	CWndEdit m_editIntCount;

	int m_nStrCount;
	int m_nStaCount;
	int m_nDexCount;
	int m_nIntCount;

	int m_nGpPoint;

	int m_nATK;
	int m_nDEF;
	int m_nCritical;
	int m_nATKSpeed;

public:
	CWndCharacterDetail2();
	virtual ~CWndCharacterDetail2();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual BOOL Process();

	void RefreshStatPoint();
	void GetVirtualATK(int* pnMin, int* pnMax);
	int GetVirtualDEF();
	int GetVirtualCritical();
	float GetVirtualATKSpeed();

protected:
	void RenderATK(C2DRender* p2DRender, int x, int y);
};
#endif //__CSC_VER9_2

class CWndPvpBase : public CWndBase
{
	CTexture m_txInfo;

public:
	CWndButton m_wndChangeJob;
	BOOL	m_fWaitingConfirm;
	BOOL m_bExpert;
	int m_nDisplay;

	CWndPvpBase();
	virtual ~CWndPvpBase();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

#if __VER >= 13 // __RENEW_CHARINFO
class CWndCharInfo : public CWndBase
{

public:
	CWndButton		m_wndChangeJob;
	BOOL			m_fWaitingConfirm;
	BOOL			m_bExpert;
	int				m_nDisplay;

	CWndButton m_wndStrPlus, m_wndStrMinus;
	CWndButton m_wndStaPlus, m_wndStaMinus;
	CWndButton m_wndDexPlus, m_wndDexMinus;
	CWndButton m_wndIntPlus, m_wndIntMinus;

	CWndButton m_wndApply, m_wndReset;

	CWndButton m_wndGuideAccuracy;

	CWndEdit m_editStrCount;
	CWndEdit m_editStaCount;
	CWndEdit m_editDexCount;
	CWndEdit m_editIntCount;

	int m_nStrCount;
	int m_nStaCount;
	int m_nDexCount;
	int m_nIntCount;

	int m_nGpPoint;

	int m_nATK;
	int m_nDEF;
	int m_nCritical;
	int m_nATKSpeed;

	int m_nSpeedRun;
	int m_nCriticalRate;
	int m_nSpellCastSpeed;
	int m_nPvpDamage;
	int m_nHitRate;
	int m_nBlockMelee;
	int m_nBlockRange;
	int m_nDodgeRate;

public:

	CWndCharInfo();
	virtual ~CWndCharInfo();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual BOOL Process();
	void RefreshStatPoint();
	void GetVirtualATK(int* pnMin, int* pnMax);
	int GetVirtualDEF();
	int GetVirtualCritical();
	float GetVirtualATKSpeed();

	int GetVirtualSpeedRun();
	int GetVirtualCriticalRate();
	int GetVirtualSpellCastSpeed();
	int GetVirtualPvpDamage();
	int GetVirtualHitRate();
	int GetVirtualBlockMelee();
	int GetVirtualBlockRange();
	int GetVirtualDodgeRate();


protected:
	void RenderATK(C2DRender* p2DRender, int x, int y);
};

class CWndHonor : public CWndNeuz
{
private:
	int						m_nSelectedId;
	int						m_nCount;
	vector<EarnedTitle>		m_vecTitle;
public:

	CWndHonor();
	virtual ~CWndHonor();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	void RefreshList();

};
#endif // __RENEW_CHARINFO

class CWndCharacter : public CWndNeuz
{
public:
#if __VER >= 13 // __RENEW_CHARINFO
	CWndCharInfo		m_wndCharInfo;
	CWndHonor			m_wndHonor;
#else // __RENEW_CHARINFO
#if __VER >= 9 // __CSC_VER9_2
	CWndCharacterDetail2 m_wndStateDetail;
#else //__CSC_VER9_2
	CWndCharacterDetail m_wndStateDetail;
#endif
	CWndCharacterBase   m_wndStateBase;
	CWndPvpBase         m_wndPvp;
#endif // __RENEW_CHARINFO
public:
	CWndCharacter();
	virtual ~CWndCharacter();
	virtual void SerializeRegInfo(CAr& ar, DWORD& dwVersion);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};


class CWndReSkillControl : public CWndNeuz
{
public:
	CWndReSkillControl();
	virtual ~CWndReSkillControl();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndReSkillWarning : public CWndNeuz
{
	BOOL	m_bParentDestroy;
public:
	CWndReSkillWarning();
	virtual ~CWndReSkillWarning();

	void InitItem(BOOL bParentDestroy);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnDestroy();
};

class CWndStatus : public CWndNeuz
{
	int nWidthOldHp;
	int nWidthOldMp;
	int nWidthOldFp;
	int nWidthOldExp;

	BOOL m_bHPVisible;
	BOOL m_bExpVisible;
public:
	CModel* m_pModel;
	int		m_nDisplay;
public:
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	CTexture m_texGauEmptyNormal;
	CTexture m_texGauEmptySmall;
	CTexture m_texGauFillNormal;
	CTexture m_texGauFillSmall;


	LPDIRECT3DVERTEXBUFFER9 m_pVBHPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBMPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBFPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBEXPGauge;

	BOOL m_bVBHPGauge;
	BOOL m_bVBMPGauge;
	BOOL m_bVBFPGauge;
	BOOL m_bVBEXPGauge;


	int m_nHPWidth;
	int m_nMPWidth;
	int m_nFPWidth;
	int m_nEXPWidth;
	int m_nPXPWidth;

	void MakeGaugeVertex();

	CWndStatus();
	virtual ~CWndStatus();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
};

class CWndQuit : public CWndMessageBox
{
	BOOL	m_bFlag;
public:
	CWndQuit() { }
	virtual ~CWndQuit() { }
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};
class CWndLogOut : public CWndMessageBox
{
public:
	CWndLogOut() { }
	virtual ~CWndLogOut() { }
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};


class CWndLogOutWaitting : public CWndNeuz
{
	BOOL  m_bIsLogOut;
	DWORD dwTime;
public:
	CWndLogOutWaitting();
	virtual ~CWndLogOutWaitting();
	void	SetIsLogOut(BOOL bFlag) { m_bIsLogOut = bFlag; }
	virtual BOOL Process();
	virtual BOOL Initialize(CWndBase* pWndParent);
	virtual	void OnInitialUpdate();
	// message
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};



//BEGIN_WNDCLASS( CWndMacro )
//END_WNDCLASS

//BEGIN_WNDCLASS( CWndStyle )
//END_WNDCLASS

class CWndTradeGold : public CWndNeuz
{
public:
	UINT m_nIdWndTo;
	SHORTCUT m_Shortcut;
	D3DXVECTOR3 m_vPos;
	DWORD m_dwGold;
	CWndBase* m_pWndBase;
	CWndStatic* pStatic;
	CWndStatic* pStaticCount;
#if __VER >= 11 // __SYS_POCKET
	char m_nSlot;
	char m_nPutSlot;
#else
	BYTE m_nSlot;
	BYTE m_nPutSlot;
#endif
	CWndTradeGold();
	virtual ~CWndTradeGold();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL Process(void);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndConfirmTrade : public CWndNeuz
{
	OBJID m_objid;
public:
	CWndConfirmTrade();
	virtual ~CWndConfirmTrade();

	void OnSetName(const char* szName, OBJID objid);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndTradeConfirm : public CWndNeuz
{
public:
	BOOL bMsg;
	CWndTradeConfirm();
	virtual ~CWndTradeConfirm();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

BEGIN_WNDCLASS(CWndTrade)

CWndTradeCtrl m_wndItemCtrlYou;
CWndTradeCtrl m_wndItemCtrlI;
CWndStatic    m_wndGold;
int           m_nGoldI, m_nGoldYou;

void DoCancel();

END_WNDCLASS

////

BEGIN_WNDCLASS(CWndEmotion)
CWndTreeCtrl  m_wndViewCtrl;
CWndEdit      m_wndEdit;
CWndButton    m_wndDefault;
LPTREEELEM    m_lpSelectTreeElem;
END_WNDCLASS


class CWndRevival : public CWndNeuz
{
public:
	CWndButton* m_pLodeStar;
	CWndButton* m_pRevival;
	CWndRevival();
	virtual ~CWndRevival();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	void	EnableButton(int nButton, BOOL bEnable);
};

class CWndResurrectionConfirm : public CWndNeuz
{
public:
	CWndResurrectionConfirm();
	virtual ~CWndResurrectionConfirm();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndCommItemDlg : public CWndNeuz
{
public:
	CWndEdit* m_pWndEdit;
	DWORD			m_dwObjId;
	void			SetItem(DWORD dwDefindText, DWORD dwObjId);
	CWndCommItemDlg();
	virtual ~CWndCommItemDlg();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndPostSendConfirm;

class CWndPostSend : public CWndNeuz
{
	int		m_nCost;
	int		m_nCount;
	BYTE    m_nItem;

public:
	void ClearData();
	CWndPostSend();
	virtual ~CWndPostSend();


	BYTE GetItemId() { return m_nItem; }
	void SetItemId(BYTE nId);
	void SetCost(int nCost) { m_nCost = nCost; }
	void SetCount(int nCount) { m_nCount = nCount; }
	void SetReceive(char* pchar);
	void SetTitle(char* pchar);
	void SetText(char* pchar);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void AdditionalSkinTexture(LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4);
};

class CWndPostItemWarning : public CWndNeuz
{
public:
	int	   m_nMailIndex;

	CWndPostItemWarning();
	virtual ~CWndPostItemWarning();

	void	SetIndex(int nIndex) { m_nMailIndex = nIndex; }
	void	SetString(char* string);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndPostDeleteConfirm;
class CWndPostRead : public CWndNeuz
{
	int					m_nMailIndex;
	CWndGold			m_wndGold;
	BOOL				m_bDrag;
	CWndPostDeleteConfirm* m_pDeleteConfirm;

public:
	CWndPostItemWarning* m_pWndPostItemWarning;
	void MailReceiveItem();
	void MailReceiveGold();
	void ClearData();
	CWndPostRead();
	virtual ~CWndPostRead();

	void SetValue(int nMailIndex);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
};


class CWndPostReceive : public CWndNeuz
{
	CTexture			m_Texture[3];
	CWndScrollBar		m_wndScrollBar;
	int					m_nSelect;
	int					m_nMax;
	CWndPostRead* m_pWndPostRead;
	CWndStatic* m_pStaticCount;

public:
	void UpdateScroll();

	CWndPostReceive();
	virtual ~CWndPostReceive();

	int          GetSelectIndex(const CPoint& point);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual	void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
};
class CWndPost : public CWndNeuz
{
public:
	CWndPostSend		m_PostTabSend;
	CWndPostReceive		m_PostTabReceive;

	CWndPost();
	virtual ~CWndPost();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);

#ifdef __MAIL_REQUESTING_BOX
public:
	void LoadLastMailBox(void);
	void SaveLastMailBox(void);
	CWndMailRequestingBox* m_pWndMailRequestingBox;
#endif // __MAIL_REQUESTING_BOX
};
class CWndPostSendConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;

	BYTE		m_nItem;
	short		m_nItemNum;
	int			m_nGold;
	char		m_szReceiver[MAX_NAME];
	char		m_szTitle[MAX_MAILTITLE];
	char		m_szText[MAX_MAILTEXT];
public:
	CWndPostSendConfirm();
	virtual ~CWndPostSendConfirm();

	void			SetValue(BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText);
	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void	OnDraw(C2DRender* p2DRender);
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual void	OnLButtonUp(UINT nFlags, CPoint point);
	virtual	void	OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndPostDeleteConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
	int		m_nIndex;
public:
	CWndPostDeleteConfirm();
	virtual ~CWndPostDeleteConfirm();

	void			SetValue(int nIndex) { m_nIndex = nIndex; }
	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void	OnDraw(C2DRender* p2DRender);
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual void	OnLButtonUp(UINT nFlags, CPoint point);
	virtual	void	OnLButtonDown(UINT nFlags, CPoint point);
};

struct GUILDLIST
{
	char	 szGuild[MAX_NAME];
	char	 szName[MAX_NAME];
	int		 nNum;
};

class CWndGuildCombatOffer : public CWndNeuz
{
protected:
	DWORD			m_dwReqGold;
	DWORD			m_dwMinGold;
	DWORD			m_dwBackupGold;

public:
	CWndGuildCombatOffer();
	virtual ~CWndGuildCombatOffer();

	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void	OnDraw(C2DRender* p2DRender);
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual void	OnLButtonUp(UINT nFlags, CPoint point);
	virtual	void	OnLButtonDown(UINT nFlags, CPoint point);
	void			SetGold(DWORD nCost);
	void			SetTotalGold(__int64 nCost);
	void			SetMinGold(DWORD dwMinGold) { m_dwMinGold = dwMinGold; }
	void			SetReqGold(DWORD dwReqGold) { m_dwReqGold = dwReqGold; }
	void			SetBackupGold(DWORD dwBackupGold) { m_dwBackupGold = dwBackupGold; }
	void			EnableAccept(BOOL bFlag);
};

class CGuildCombatSelectionClearMessageBox : public CWndMessageBox
{
public:
	CString m_strMsg;

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};

class CGuildCombatInfoMessageBox : public CWndNeuz
{
public:
	CGuildCombatInfoMessageBox();
	virtual ~CGuildCombatInfoMessageBox();
	void	SetString(CHAR* szChar);
	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	virtual void PaintFrame(C2DRender* p2DRender);
	void	SetString(CString strMsg);
#endif //__GUILD_COMBAT_1TO1
};

class CGuildCombatInfoMessageBox2 : public CWndNeuz
{
public:
	CGuildCombatInfoMessageBox2();
	virtual ~CGuildCombatInfoMessageBox2();
	void	SetString(CHAR* szChar);
	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};



class CWndGuildCombatSelection : public CWndNeuz
{
protected:
	multimap<int, CGuildMember*>	m_mapSelectPlayer;

	vector<u_long>					m_vecGuildList;
	vector<u_long>					m_vecSelectPlayer;

	u_long							m_uidDefender;
	CTexture						m_TexDefender;
	int								m_nDefenderIndex;

	int								nMaxJoinMember;
	int								nMaxWarMember;

public:
	void Reset();
	CWndGuildCombatSelection();
	virtual ~CWndGuildCombatSelection();

	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void	OnDraw(C2DRender* p2DRender);
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual void	OnLButtonUp(UINT nFlags, CPoint point);
	virtual	void	OnLButtonDown(UINT nFlags, CPoint point);
	void			EnableFinish(BOOL bFlag);

	void			SetDefender(u_long uiPlayer);
	void			UpDateGuildListBox();

	void			AddCombatPlayer(u_long uiPlayer);
	void			AddGuildPlayer(u_long uiPlayer);

	void			RemoveCombatPlayer(int nIndex);
	void			RemoveGuildPlayer(int nIndex);

	u_long			FindCombatPlayer(u_long uiPlayer);
	u_long			FindGuildPlayer(u_long uiPlayer);

	void			SetMemberSize(int nMaxJoin, int nMaxWar);
};

class CWndGuildCombatState : public CWndNeuz
{
private:
	CTimeSpan		m_ct;
	time_t    		m_tEndTime; // timegettime+
	time_t    		m_tCurrentTime; // timegettime+

public:
	CWndGuildCombatState();
	virtual ~CWndGuildCombatState();

	void		 InsertTitle(const char szTitle[]);
	int          GetSelectIndex(const CPoint& point);
	void		 Init(time_t lTime);
	void		 InsertGuild(const char szGuild[], const char szName[], int nNum);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual	void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	virtual void	PaintFrame(C2DRender* p2DRender);
#endif //__GUILD_COMBAT_1TO1
	virtual BOOL Process();
	void		 SetGold(int nGold);
	void		 SetRate(int nRate);
	void	 	 SetTime(time_t tTime) { m_tCurrentTime = 0; m_tEndTime = time_null() + tTime; }
};

class CWndGuildCombatJoinSelection : public CWndNeuz
{
protected:
	CTimer m_timerInputTimeOut;
	DWORD  m_dwOldTime;

public:
	CWndGuildCombatJoinSelection();
	virtual ~CWndGuildCombatJoinSelection();

	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void	OnDraw(C2DRender* p2DRender);
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual void	OnLButtonUp(UINT nFlags, CPoint point);
	virtual	void	OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL	Process();

	void			SetTimeOut(DWORD dwSec) { m_dwOldTime = g_tmCurrent + dwSec; }
};

class CWndGuildWarAppConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
public:
	CWndGuildWarAppConfirm();
	virtual ~CWndGuildWarAppConfirm();

	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void	OnDraw(C2DRender* p2DRender);
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual void	OnLButtonUp(UINT nFlags, CPoint point);
	virtual	void	OnLButtonDown(UINT nFlags, CPoint point);
};
class CWndGuildWarCancelConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
public:
	CWndGuildWarCancelConfirm();
	virtual ~CWndGuildWarCancelConfirm();

	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void	OnDraw(C2DRender* p2DRender);
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual void	OnLButtonUp(UINT nFlags, CPoint point);
	virtual	void	OnLButtonDown(UINT nFlags, CPoint point);
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	virtual void	PaintFrame(C2DRender* p2DRender);
#endif //__GUILD_COMBAT_1TO1
};
class CWndGuildWarJoinConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
public:
	CWndGuildWarJoinConfirm();
	virtual ~CWndGuildWarJoinConfirm();

	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void	OnDraw(C2DRender* p2DRender);
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual void	OnLButtonUp(UINT nFlags, CPoint point);
	virtual	void	OnLButtonDown(UINT nFlags, CPoint point);
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	virtual void	PaintFrame(C2DRender* p2DRender);
#endif //__GUILD_COMBAT_1TO1
};

class CWndGuildWarState : public CWndNeuz
{
private:
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	GUILDLIST			m_aList[MAX_WANTED_LIST];
	int					m_nSelect;

public:
	CWndGuildWarState();
	virtual ~CWndGuildWarState();

	void		 InsertTitle(const char szTitle[]);
	int          GetSelectIndex(const CPoint& point);
	void		 Init(time_t lTime);
	void		 InsertGuild(const char szGuild[], const char szName[], int nNum);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual	void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
};


struct GUILDNAME
{
	char	 szGuild[MAX_NAME];
};

class CWndGuildCombatRanking : public CWndNeuz
{
private:
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	multimap< int, GUILDNAME > m_multimapRanking;
	int					m_nSelect;

public:
	void SortRanking();
	CWndGuildCombatRanking();
	virtual ~CWndGuildCombatRanking();

	int          GetSelectIndex(const CPoint& point);
	void		 Init(time_t lTime);
	void		 InsertGuild(const char szGuild[], int nWinCount);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual	void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
};


class CWndGuildCombatTabResultRate : public CWndNeuz
{
public:
	CWndGuildCombatTabResultRate();
	virtual ~CWndGuildCombatTabResultRate();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
};

class CWndGuildCombatTabResultLog : public CWndNeuz
{
public:
	CWndGuildCombatTabResultLog();
	virtual ~CWndGuildCombatTabResultLog();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
};


class CWndGuildCombatResult : public CWndNeuz
{
public:
	void InsertLog(CString str);
	void InsertPersonRate(CString str);
	void InsertGuildRate(CString str);
	CWndGuildCombatResult();
	virtual ~CWndGuildCombatResult();

	CWndGuildCombatTabResultRate   m_WndGuildCombatTabResultRate;
	CWndGuildCombatTabResultLog    m_WndGuildCombatTabResultLog;

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
};



typedef struct __GUILDCOMBAT_RANK_INFO
{
	u_long	uidPlayer;
	int		nJob;
} __GUILDCOMBAT_RANK_INFO;

typedef struct __GUILDCOMBAT_RANK_INFO2
{
	CString strName;
	CString strJob;
	u_long	uidPlayer;
	int		nPoint;
} __GUILDCOMBAT_RANK_INFO2;

#define MAX_GUILDCOMBAT_RANK_PER_PAGE 11
#define MAX_GUILDCOMBAT_RANK		  100



class CWndGuildCombatRank_Class : public CWndNeuz
{
public:
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	int					m_nSelect;

	int		m_nRate;
	int		m_nOldRate;

	__GUILDCOMBAT_RANK_INFO2		m_listRank[MAX_GUILDCOMBAT_RANK];

	CWndGuildCombatRank_Class();
	virtual ~CWndGuildCombatRank_Class();

	void		 InsertRank(int nJob, u_long uidPlayer, int nPoint);
	int          GetSelectIndex(const CPoint& point);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual	void OnLButtonDown(UINT nFlags, CPoint point);
};


class CWndGuildCombatRank_Person : public CWndNeuz
{
public:
	multimap< int, __GUILDCOMBAT_RANK_INFO > m_mTotalRanking;

	CWndGuildCombatRank_Person();
	virtual ~CWndGuildCombatRank_Person();

	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Tot;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Mer;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Mag;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Acr;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Ass;

	CWndGuildCombatRank_Class* __GetJobKindWnd(int nJob);
	void						InsertRank(int nJob, u_long	uidPlayer, int nPoint);
	void						UpdateList();
	void						DivisionList();
	void						UpdatePlayer(int nJob, u_long idPlayer);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);

};

class CWndFontEdit : public CWndNeuz
{
	CTexture* m_pTexture;
	CPoint			 m_ColorScrollBar[3];
	CPoint			 m_OriginalColorScrollBar[3];

	CRect			 m_ColorRect[3];
	FLOAT			 m_fColor[3];

	BOOL			 m_bLButtonClick;

public:
	CWndText* m_pWndText;
	CWndFontEdit();
	virtual ~CWndFontEdit();

	void	ReSetBar(FLOAT r, FLOAT g, FLOAT b);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
};

#if __VER >= 8 //__CSC_VER8_3

#include "WndWorld.h"

class CWndBuffStatus : public CWndNeuz
{
public:
	vector< multimap<DWORD, BUFFSKILL> > m_pBuffTexture;
	list<BUFFICON_INFO> m_pBuffIconInfo;

	int m_BuffIconViewOpt;
public:
	CWndBuffStatus();
	virtual ~CWndBuffStatus();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual	void PaintFrame(C2DRender* p2DRender);

#ifdef __BUFF_1107
	void RenderBuffIcon(C2DRender* p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse);
#else	// __BUFF_1107
	void RenderBuffIcon(C2DRender* p2DRender, SKILLINFLUENCE* pSkill, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse);
#endif	// __BUFF_1107
	void RenderOptBuffTime(C2DRender* p2DRender, CPoint& point, CTimeSpan& ct, DWORD dwColor);
	void SetBuffIconInfo();
	BOOL GetHitTestResult();
};
#endif //__CSC_VER8_3

#if __VER >= 9 // __CSC_VER9_1

#include "WndSummonAngel.h"
class CWndMixJewel : public CWndNeuz
{
public:
	CItemElem* m_pItemSun;
	CItemElem* m_pItemMoon;

	LPWNDCTRL wndCtrlSun;
	LPWNDCTRL wndCtrlMoon;

	CTexture* pTexSun;
	CTexture* pTexMoon;
public:

	CWndMixJewel();
	~CWndMixJewel();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	void SetJewel(CItemElem* pItemElem);

	void SetDescription(CHAR* szChar);
	void UpdateButtonState(BOOL bEnabled);
	void ReceiveResult(BOOL bResult);

};

class CWndExtraction : public CWndNeuz
{
public:
	CWndText* m_pText;
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;

public:
	CWndExtraction();
	virtual ~CWndExtraction();

	virtual void OnDestroy();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);

	void SetDescription(CHAR* szChar);
	void ReceiveResult(int result, int nCount);
	void SetWeapon(CItemElem* pItemElem);
};

class CWndSmeltJewel : public CWndNeuz
{
public:
	CModelObject* m_pMainItem;
	CItemElem* m_pItemElem;
	CItemElem* m_pJewelElem;
	CWndText* m_pText;
	DWORD m_dwJewel[5];
	int m_nJewelSlot[5];

	CWndButton* pWndButton[5];
	int m_nInfo[5];
	CItemElem* m_pMoonstone;
	CTexture* m_pTextureMoon;

	int m_nJewelCount;
	int m_nUsableSlot;
	OBJID m_objJewelId;

	float m_fRotate;
	float m_fAddRot;

	int m_nStatus;
	int m_nCount;
	int m_nDelay;
	int m_nAlpha;
	float m_nEyeYPos;
	BOOL m_bFlash;

public:
	CWndSmeltJewel();
	~CWndSmeltJewel();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual	void OnDestroy(void);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual BOOL Process();

	void SetItem(CItemElem* pItemElem);
	void UpdateButtons();


	void SetDescription(CHAR* szChar);
	void ReceiveResult(int result);
	void InitializeJewel(CItemElem* pItemElem);
	void SetJewel(CItemElem* pItemElem);
};
#endif //__CSC_VER9_1

#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND

class CWndHeroSkillUp : public CWndNeuz
{
public:
	CWndText* m_pText;
	CItemElem* m_pItemElem[5];
	int m_JewelID[5];
	int m_SlotID[5];
	int m_PicJewel[5];

public:
	CWndHeroSkillUp();
	virtual ~CWndHeroSkillUp();

	virtual void OnDestroy();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);

	int HitTest(CPoint point);
	void SetDescription(CHAR* szChar);
	void ReceiveResult(int nresult);
	void SetJewel(CItemElem* pItemElem);
};

#endif //__CSC_VER9_1 -> __LEGEND

#ifdef __TRADESYS

#define MAX_LIST_COUNT 15

class CWndDialogEvent : public CWndNeuz
{
public:
	vector<int> m_nVecList;
	vector<int> m_nDescList;

	int m_nChoiceNum;
	int m_nDescCount;
	int m_nCurrentPage;
	int m_nMMI;
	int m_nGap;			//List Gap
	int m_nListOffset;	//Selected List Rect Offset

public:
	CWndDialogEvent();
	virtual ~CWndDialogEvent();

	virtual void OnDestroy();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();

	void SetDescription(CHAR* szChar);
	void SetMMI(int mmi);
	void ReceiveResult(int result);
	BOOL CheckBeforeSending();
	BOOL CheckExtra(DWORD dwId);
};
#endif //__TRADESYS

#if __VER >= 12 // __HEAVEN_TOWER
#define MAX_FLOOR_COUNT 15

class CWndHeavenTower : public CWndNeuz
{
public:
	vector<int> m_nVecList;
	vector<int> m_nDescList;
	vector<int> m_nMsgList;
	vector<int>	m_nCost;

	int m_nChoiceNum;
	int m_nListCount;
	int m_nDescCount;
	int m_nCurrentPage;
	int m_nGap;			//List Gap
	int m_nListOffset;	//Selected List Rect Offset

public:
	CWndHeavenTower();
	virtual ~CWndHeavenTower();

	virtual void OnDestroy();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();

	void InitText();
	void InitWnd();
};

class CWndHeavenTowerEntranceConfirm : public CWndMessageBox
{
public:
	DWORD	m_nFloor;

public:
	void	SetValue(CString str, DWORD nFloor);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};

#endif //__HEAVEN_TOWER

#if __VER >= 10 // __REMOVE_ATTRIBUTE

class CWndRemoveAttributeConfirm : public CWndNeuz
{
public:
	CWndRemoveAttributeConfirm();
	virtual ~CWndRemoveAttributeConfirm();

	virtual void OnDestroy();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndRemoveAttribute : public CWndNeuz
{
public:
	CWndText* m_pText;
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;

	CWndRemoveAttributeConfirm* m_pWndConfirm;

public:
	CWndRemoveAttribute();
	virtual ~CWndRemoveAttribute();

	virtual void OnDestroy();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);

	void SetDescription(CHAR* szChar);
	void ReceiveResult(BOOL result);
	void SetWeapon(CItemElem* pItemElem);
};

#endif //__REMOVE_ATTRIBUTE

#if __VER >= 11 // __PIERCING_REMOVE
class CWndRemovePiercing : public CWndNeuz
{
public:
	CWndText* m_pText;
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;
	int			m_nInfoSlot[10];
	int			m_pSlot;

public:
	CWndRemovePiercing();
	virtual ~CWndRemovePiercing();

	virtual void OnDestroy();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);

	void SetDescription(CHAR* szChar);
	virtual void OnMouseWndSurface(CPoint point);
	void SetItem(CItemElem* pItemElem);
};
#endif //__PIERCING_REMOVE


#if __VER >= 13 // __CSC_VER13_2
class CWndCoupleMessage : public CWndMessageBox
{
public:
	enum { CM_SENDPROPOSE = 0, CM_RECEIVEPROPOSE, CM_CANCELCOUPLE };

	CString m_strText;
	int m_nMode;
	CObj* m_pTargetObj;

public:
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

public:
	void SetMessageMod(CString strText, int nMode, CObj* pTargetObj = NULL);
};

class CWndCoupleTabInfo : public CWndNeuz
{
public:
	CCouple* m_pCouple;

	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

public:
	CWndCoupleTabInfo();
	virtual ~CWndCoupleTabInfo();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
};

class CWndCoupleTabSkill : public CWndNeuz
{
public:
	CWndText* m_pText;
	CTexture* m_pSkillBgTexture;

public:
	CWndCoupleTabSkill();
	virtual ~CWndCoupleTabSkill();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
};

class CWndCoupleManager : public CWndNeuz
{
public:
	CWndCoupleTabInfo	m_wndCoupleTabInfo;
	CWndCoupleTabSkill	m_wndCoupleTabSkill;
public:
	CWndCoupleManager();
	virtual ~CWndCoupleManager();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void OnInitialUpdate();
};
#endif //__CSC_VER13_2

class CWndSmeltSafety : public CWndNeuz
{
	const static int GENERAL_NON_USING_SCROLL2_LEVEL = 7;
	const static int ELEMENTAL_NON_USING_SCROLL2_LEVEL = 10;

	CItemElem* m_pItemElem;
	CItemElem* m_pItemElemMaterial;
	CItemElem* m_pItemElemScroll_1;
	CItemElem* m_pItemElemScroll_2;

	LPWNDCTRL wndCtrlElemMaterial;
	LPWNDCTRL wndElemScroll_1;
	LPWNDCTRL wndElemScroll_2;

	CTexture* m_pItemElemTexture;
	CTexture* m_pNowGaugeTexture;
	CTexture* m_pSuccessTexture;
	CTexture* m_pFailureTexture;

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;
	ItemProp* m_pSelectedElementalCardItemProp;
	BOOL m_bStart;
	BOOL m_bRealStart;
	DWORD m_dwEnchantTimeStart;
	DWORD m_dwEnchantTimeEnd;
	int nLastResult;
	int nTotalAttempts, nSuccessAttempts, nFailureAttempts;

public:
	enum WndMode { WND_NORMAL, WND_ACCESSARY, WND_PIERCING, WND_ELEMENT, WND_UNIQUE, WND_ULTIMATE };
	WndMode m_eWndMode;

	CWndSmeltSafety(WndMode eWndMode);
	virtual ~CWndSmeltSafety();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	void OnInitialUpdate();
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL Process();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	BOOL IsPossibleStart();

	void StartProcess(BOOL bManual);
	void StopProcess(BOOL bFake = FALSE);
	void SetResult(int nResult);
	void AddSuccess() { nTotalAttempts += 1; nSuccessAttempts += 1; };
	void AddFailure() { nTotalAttempts += 1; nFailureAttempts += 1; };

	void SetItem(CItemElem* pItemElem);
	void ResetItems();
	void DisableScroll2(void);
	void RefreshText(void);
	int GetNowSmeltValue(void);
	int GetDefaultMaxSmeltValue(void);
	BOOL IsAcceptableMaterial(ItemProp* pItemProp);
	BOOL IsAcceptableScroll1(ItemProp* pItemProp);
	BOOL IsAcceptableScroll2(ItemProp* pItemProp);
	void DrawListItem(C2DRender* p2DRender);
};

class CWndSmeltSafetyConfirm : public CWndNeuz
{
public:
	enum ErrorMode { WND_ERROR1, WND_ERROR2, WND_ERROR3 };

private:
	ErrorMode m_eErrorMode;
	CItemElem* m_pItemElem;

public:
	CWndSmeltSafetyConfirm(ErrorMode eErrorMode);
	virtual ~CWndSmeltSafetyConfirm();

public:
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	void OnInitialUpdate();
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDestroy(void);

	void SetWndMode(CItemElem* pItemElem);
};

#if __VER >= 14 // __RESTATE_CONFIRM
class CWndRestateConfirm : public CWndNeuz
{
public:
	CWndRestateConfirm(DWORD dwItemID);
	virtual ~CWndRestateConfirm(void);

public:
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	void OnInitialUpdate(void);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

public:
	void SetInformation(DWORD dwItemObjID);

private:
	DWORD m_dwItemID;
	DWORD m_dwItemObjID;
};
#endif // __RESTATE_CONFIRM

#ifdef __FL_DUNGEON_TREASURE
class CWndDungeonTreasure : public CWndNeuz
{
	CWndButton* pWndButton[4];
	DWORD dwCloseTime;

public:
	CWndDungeonTreasure();
	virtual ~CWndDungeonTreasure(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual	void OnInitialUpdate(void);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL Process(void);

	void ReceiveResult(int nTreasureWin, BOOL bUserWin);
};
#endif

#ifdef __FL_BATTLE_PASS
class CWndBattlePass : public CWndNeuz
{
private:
	int m_nPage;
	int m_nBPnextEXP;
	CEditString strEdit;

public:
	CWndBattlePass(void);
	~CWndBattlePass(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = 1);

	void UpdateStaticParams();
	void UpdatePage(BOOL bNext);
	void UpdateStatics();
};
#endif

#ifdef __FL_CONSUMABLES_STORAGE
class CWndChest : public CWndNeuz
{
private:

public:
	CWndItemCtrl m_wndItemCtrl;

	CWndChest();
	virtual ~CWndChest();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};
#endif

#ifdef __FL_DPS_DISPLAY
class CWndDpsDisplay : public CWndNeuz
{
	const int WINDOWX = 220;
	const int WINDOWY = 25;
	const int MAX_DISPLAY_LIST = 10;
	const int UPDATE_DELAY = 2;
	const int REMOVE_DELAY = 10;

	u_long dwTargetID;
	DWORD dwLastDpsUpdate;
	CWndMenu m_ButtonsMenu;

public:
	CWndDpsDisplay();
	virtual ~CWndDpsDisplay();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	BOOL Process();
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = 1);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);

	DWORD GetTargetId(void) { return dwTargetID; };
	void Add(CString str, int nJob, DWORD nNum);

	struct DPS_MEMBERS
	{
		CString strMemberName;
		int nMemberJob;
		int nTotalDamageForDps;
		int nStaticDpsValue;
		DWORD dwLastHitTime;
	};

private:
	vector<DPS_MEMBERS> vecDpsMembers;
	void UpdateSize();

};
#endif

class CWndSkill_16 : public CWndNeuz
{
protected:
	BOOL			m_bDrag;
	LPSKILL			m_pFocusItem;
	int				m_nCurrSkillPoint;
	CTexturePack	m_textPackNum;
	DWORD			m_dwMouseSkill;
	CWndButton* m_pWndButton[5];		//+, -, reset, finish
	LPSKILL			m_apSkill;
	CTexture* m_atexSkill[MAX_SKILL_JOB];
	CTexture* m_aTexSkillTrees[4];
	char			m_cOldJob;
	char			m_cActiveSlot;
	char			m_cActiveJobType;
	CTexture* m_atexTopDown[2];	//these store the up and down icons
	CPoint			m_pointTreePos;
	CTexture* m_texSelectSkill;
	CString			m_strHeroSkillIcon;
public:
	BOOL	IsReSkillPointDone();
	int		GetCurrSkillPoint() { return m_nCurrSkillPoint; }
	void	SubSkillPointDown(LPSKILL lpSkill);
	LPSKILL GetSkill() { return m_apSkill; }
	BOOL	IsDownPoint(DWORD skill);
	int		GetCurSelect() { return g_nSkillCurSelect; }
	BOOL	GetSkillPoint(DWORD dwSkillID, CRect& rect);
	LPSKILL GetSkill(int i);
	LPSKILL GetdwSkill(DWORD dwSkill);
	BOOL	CheckSkill(int i);
	void LoadTextureSkillicon();
	void InitItem(int nJob, LPSKILL apSkill, BOOL bReset = FALSE);
	CWndSkill_16();
	virtual ~CWndSkill_16();

	void SetSlot(char cSlot);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL Process();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	// message
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	void SetActiveSlot(int nSlot, BOOL bFlag);
	virtual void SerializeRegInfo(CAr& ar, DWORD& dwVersion);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
};

#ifdef __FL_FAST_ELEMENT
class CWndChangeAttributeNew : public CWndNeuz
{
public:
	int m_nInvPosition;
	int m_nAttributeNum;
	int m_nAttributeStaticID[5];
	int m_nTooltipTextIndx[6];
	CItemElem* m_pItemElem;
	CTexture* m_pTexture;

	CItemElem* m_pItemElemScroll;
	CTexture* m_pTextureScroll;

	CWndChangeAttributeNew();
	virtual ~CWndChangeAttributeNew();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual BOOL OnSetCursor(CWndBase* pWndBase, UINT nHitTest, UINT message);
	virtual BOOL Process();

	void ClearMainItem();
	void ClearScroll();
	void SetScroll(CItemElem* pItemElem);
	void FillRect(C2DRender* p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend);
};
#endif

#ifdef __FL_SHOP_SEARCH
class CWndShopSearch : public CWndNeuz
{
public:
	CWndShopSearch(void);
	~CWndShopSearch(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL Process(void);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = 1);
	void AcceptResult(CAr& ar);
	void UpdateData();
	void ClearItems();

	struct SHOP_ITEMS
	{
		CItemElem* pItem;
		OBJID dwOwnerID;
		CString strOwnerName;
		CString strVendorTitle;
	};

private:
	CWndButton* pWndButtonRefresh;
	CWndButton* pWndButtonReset;
	CWndButton* pWndButtonTeleport;
	CWndButton* pWndButtonPM;

	CWndEdit* m_pNameFilter;
	CWndEdit* m_pMinLevelFilter;
	CWndEdit* m_pMaxLevelFilter;

	CWndComboBox* m_pComboSex;
	CWndComboBox* m_pComboJob;

	CWndListBox* m_pListItems;
	vector<SHOP_ITEMS> vecServerShopsItems;
	vector<SHOP_ITEMS> vecClientShopsItems;

public:
	vector<SHOP_ITEMS>* GetItemList() { return &vecClientShopsItems; }

};
#endif

#ifdef __FL_PET_FILTER
class CWndPetLootOption : public CWndNeuz
{
	DWORD dwSaveTime;

public:
	CWndPetLootOption();
	~CWndPetLootOption();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Process(void);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	void UpdateCheckboxes();
	void ReloadPresets();
	BOOL CheckPresetValidName(CString str);
	void SaveData();

	int m_nLootType;
	BYTE m_nLootPlace;
};
#endif
#ifdef __FL_EQUALIZED_MODE
class CWndSelectTmpClass : public CWndNeuz
{
private:
	int nSelectedClass;

public:
	CWndSelectTmpClass();
	virtual ~CWndSelectTmpClass();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL Process();
	void SetClass(int nClassType);
};
#endif

class CWndAccept1 : public CWndMessageBox
{
public:
	CWndAccept1() { }
	virtual ~CWndAccept1() { }
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};

class CWndCollectingItems : public CWndNeuz
{
private:
	CWndListBox* m_pListbox;
	CTexture* m_pTexLevel, * m_pTexMoon;

public:
	CWndCollectingItems(void);
	~CWndCollectingItems(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);

};

#ifdef __FL_GW_STATUE_V2
class CWndStatueConfiguration : public CWndNeuz
{
private:
	int m_nMode;
	CTexture* m_pTexture;
	CWndEdit* m_pEdit;

public:

	CWndStatueConfiguration(void);
	~CWndStatueConfiguration(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	void SetWndMode(int nMod) { m_nMode = nMod; };
	void UpdateStaticParams();
};
class CWndStatuePurchase : public CWndNeuz
{
private:
	int m_nMode;
	int m_nBuffPrice;
	CTexture* m_pTextureWnd;
	CTexture* m_pTextureItem;

public:

	CWndStatuePurchase(void);
	~CWndStatuePurchase(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	void SetWndMode(int nMod) { m_nMode = nMod; };
	void SetBuff(int nPrice);
	void UpdateStaticParams();
};
#endif

#ifdef __FL_FFA
class CWndFFARegister : public CWndNeuz
{
private:
	CTexture* m_pItemTexture;
	DWORD m_dwRewardID;
	int m_nRewardAmount;

	BOOL m_bLastFFA;
	CTimeSpan m_ct;
	time_t m_tEndTime;
	time_t m_tCurrentTime;
	int m_nNextRefresh;

public:
	CWndFFARegister(void);
	~CWndFFARegister(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL Process(void);

	void LoadData();
	void AcceptData(CAr& ar);
	void SetTime(time_t tTime) { m_tCurrentTime = 0; m_tEndTime = time_null() + tTime; }

};

class CWndFFAResult : public CWndNeuz
{
private:
	vector<FFA_PLAYER> vecPlayerFFA;

public:
	CWndFFAResult(void);
	~CWndFFAResult(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL Process(void);

	vector<FFA_PLAYER>* GetPlayerList()
	{
		return &vecPlayerFFA;
	}
};
#endif

#ifdef __FL_DAILY_REWARDS
class CWndDailyGift : public CWndNeuz
{
private:
	int m_nDay;
	DWORD m_dwItemID[2];
	int m_nItemNum[2];
	BOOL m_bBind[2];
	CTexture* m_pItemTexture[2];

public:

	CWndDailyGift(void);
	~CWndDailyGift(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	void SetData(CAr& ar);
};
#endif

#ifdef __MODEL_CHANGE
class CWndLookChange : public CWndNeuz
{
private:
	CItemElem* m_pItemSource;
	CItemElem* m_pItemDest;
	CRect* m_pRectDest;
	CRect* m_pRectSource;
	CRect* m_pRectResult;
	CWndButton* m_pStart;
public:
	CWndLookChange();
	virtual void OnDestroy();
	virtual void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwStyle = 0);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	BOOL SetItem(CItemElem* pItem);
private:
	int IsValidItem(CItemElem* pItem);
};
#endif

#ifdef __JOB_CHANGE
class CWndNewChangeJob : public CWndNeuz
{
	CWndButton* m_pWndButton[4];
	CWndButton* m_pWndButtonOK;
	int m_nJob;

public:
	CWndNewChangeJob();
	virtual ~CWndNewChangeJob();

	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL Process();
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};

class CWndNewChangeJob2 : public CWndNeuz
{
	CWndButton* m_pWndButton[2];
	CWndButton* m_pWndButtonOK;
	int m_nJob;

public:
	CWndNewChangeJob2();
	virtual ~CWndNewChangeJob2();

	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL Process();
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	int GetNewJob();
};

class CWndNewChangeJob3 : public CWndNeuz
{
	CWndButton* m_pWndButton;
	CWndButton* m_pWndButtonOK;
	int m_nJob;

public:
	CWndNewChangeJob3();
	virtual ~CWndNewChangeJob3();

	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL Process();
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual int GetNewJob();
};
#endif
#ifdef __FL_RECYCLE
class CWndRecycle : public CWndNeuz
{
private:
	CWndEdit* m_pText;
	CItemElem* m_pItemElem;
	ItemProp* m_pEItemProp;
	CTexture* m_pTexture;
	CWndButton* m_pButtonStart;
	CWndButton* m_pButtonCheckbox;
	CWndListBox* m_pListBox;
	CWndEdit* m_pNameFilter;
	CWndComboBox* m_pWndComboFilter;

public:
	CWndRecycle();
	~CWndRecycle();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnDestroy();
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);

	BOOL SetItem(CItemElem* pItemElem);
	BOOL IsPossibleForAuto(CItemElem* pItemElem);
	void SendToServer();
	void ClearItem();
	void ApplyFilters();
	void SortByValue(BOOL bHighest);
	void SortByItemLevel(BOOL bHighest);

	struct RECYCLE_ITEMS
	{
		ItemProp* pItemProp;
		int nPrice;
	};

	vector<RECYCLE_ITEMS>* GetItemList()
	{
		return &m_vecItems;
	}

private:
	vector<RECYCLE_ITEMS> m_vecItems;

};
#endif

#ifdef __FL_PARTY_FINDER
class CWndPartyFinder : public CWndNeuz
{
private:
	vector<PARTY_LIST> m_vecPartyListOriginal;
	vector<PARTY_LIST> m_vecPartyListFiltered;
	CWndEdit* m_pSearchBox;
	CWndListBox* m_pListItems;
	CWndComboBox* m_pWndComboFilter;
	CWndMenu m_Menu;

public:
	CWndPartyFinder();
	~CWndPartyFinder();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);

	void SendRefreshRequest();
	void ReceiveList(CAr& ar);
	void Filter();
	void SortByLeaderLevel(BOOL bHighest);
	void SortByMembers(BOOL bHighest);
	void SortByPoints(BOOL bHighest);
	void SortByPartyLevel(BOOL bHighest);

	vector<PARTY_LIST>* GetList() { return &m_vecPartyListFiltered; }

	CTexture* m_pTexRed;
	CTexture* m_pTexOrange;
	CTexture* m_pTexGreen;
	CTexture* m_pTexLink;
	CTexture* m_pTexDrop;
	CTexture* m_pTexGift;
	CTexture* m_pTextResting;
};
#endif

#ifdef __FL_VENDORS
class CWndVendorItemSet : public CWndNeuz
{
	CString m_strNameVendor;
	CItemElem* m_pItemElem;
	CTexture* m_pItemTexture;

public:
	CWndVendorItemSet(CString str);
	virtual ~CWndVendorItemSet();

	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);

	void SetItem(CItemElem* pItemElem);
};

class CWndVendorManagement : public CWndNeuz
{

public:
	CWndVendorManagement();
	virtual ~CWndVendorManagement();

	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);

	BOOL CheckVendorBuff();
};
#endif

#ifdef __FL_NEW_BOXES
class CWndNewBoxOpen : public CWndNeuz
{
private:
	CItemElem* m_pItemElem;
	vector<NEWBOXELEM>* m_vecElements;
	CTexture* m_pTexture;

	CWndListBox* m_pWndListItems;
	CWndButton* m_pWndButton;

public:
	CWndNewBoxOpen(CItemElem* pItemElem, vector<NEWBOXELEM>* vec);
	~CWndNewBoxOpen();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnDestroy(void);
	virtual void OnMouseWndSurface(CPoint point);
};
#endif

#ifdef __FL_SWITCH_EQ
class CWndSwitchEquip_Create : public CWndNeuz
{
	DWORD m_dwLooter, m_dwPet, m_dwRegen;
	CRect m_RectLooter, m_RectPet, m_RectRegen;
	CWndEdit* m_pEdit;
	int m_nSelected;

public:
	CWndSwitchEquip_Create(int nExisting);
	~CWndSwitchEquip_Create();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);

	void Clear();
	void FindAndRemove(DWORD dwObjId);
};

class CWndSwitchEquip : public CWndNeuz
{
	CMover m_Mover;
	CRect m_InvenRect[MAX_HUMAN_PARTS];
	CRect m_RectLooter, m_RectPet, m_RectRegen;

	CWndListBox* m_pWndListSwitches;
	CWndButton* m_pButtonUse;
	CWndButton* m_pButtonDelete;
	CWndButton* m_pButtonModify;
	CWndButton* m_pButtonCreate;
	CWndStatic* m_pStaticShortcut;

	CTexture* m_pTextureDark, * m_pTextureNormal;

public:
	CWndSwitchEquip();
	virtual ~CWndSwitchEquip();
	virtual BOOL Process();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnMouseWndSurface(CPoint point);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	CWndSwitchEquip_Create* m_pWndNewSwitch;

	void ResetListbox();
	void UpdateStatic();
	void UpdateModel();
	void UpdateParts();
	BOOL IsSelected();
	void SetUsedPreset(int nSel);
};
#endif

#ifdef __FL_GUILD_FINDER
class CWndGuildFinder : public CWndNeuz
{
public:
	struct GUILD_MEMBER
	{
		CString m_strName;
		int m_nLevel;
		int m_nJob;

		GUILD_MEMBER() { Init(); };
		~GUILD_MEMBER() {};

		void Init()
		{
			m_strName = "";
			m_nLevel = 1;
			m_nJob = JOB_VAGRANT;
		};
	};

	struct GUILD_LIST
	{
		// Guild info (serialization)
		u_long m_dwGuildId;
		BOOL m_bGuildHouse;
		int m_nMembers;
		int m_nLevel;

		// Guild info (existing)
		char m_szGuild[MAX_G_NAME];
		int m_nMaxMembers;

		// Leader info (serialization)
		int m_nLeaderLevel;
		int m_nLeaderJob;
		CString m_strLeaderName;

		// Leader info (existing)
		DWORD m_dwLeaderId;

		// Furniture (serialization)
		vector<DWORD> m_vecFurniture;

		// Additional info
		CString m_strGuildDesc;
		vector<GUILD_MEMBER> m_vecMembers;

		GUILD_LIST() { Init(); };
		~GUILD_LIST() { Clear(); };

		void Init()
		{
			m_dwGuildId = NULL_ID;
			m_bGuildHouse = FALSE;
			m_nMembers = 0;
			*m_szGuild = '\0';
			m_nMaxMembers = 0;
			m_nLevel = 0;

			m_nLeaderLevel = 1;
			m_nLeaderJob = JOB_VAGRANT;
			m_strLeaderName = "";
			m_dwLeaderId = NULL_ID;

			m_strGuildDesc = "";
		};

		void Clear()
		{
			m_vecFurniture.clear();
			m_vecMembers.clear();
		};
	};

private:
	vector<GUILD_LIST> m_vecGuildListOriginal;
	vector<GUILD_LIST> m_vecGuildListFiltered;

	CWndListBox* m_pListGuilds;
	CWndListBox* m_pListMembers;
	CWndText* m_pTextDetails;
	CWndEdit* m_pSearchBox;
	CWndEdit* m_pMinLevelFilter;
	CWndEdit* m_pMaxLevelFilter;
	CWndComboBox* m_pWndComboFilter;
	CWndButton* m_pWndButtonShowFull;
	CWndButton* m_pWndButtonHouse;
	CWndButton* m_pWndButtonFurni;
	CWndButton* m_pWndButtonJoin;
	CWndButton* m_pWndButtonCancel;
	CWndButton* m_pWndButtonSort;

public:
	CWndGuildFinder();
	~CWndGuildFinder();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL Process(void);

	vector<GUILD_LIST>* GetList() { return &m_vecGuildListFiltered; }
	vector<GUILD_MEMBER>* GetListMembers();

	void SendRefreshRequest();
	void ReceiveList(CAr& ar);
	void Filter();
	BOOL IsSelected();

	void SortByGuildLevel(BOOL bHighest);
	void SortByMembers(BOOL bHighest);
	void SortByMasterLevel(BOOL bHighest);

	void ResetAdditionalInfo();
	void SetAdditionalInfo(u_long idGuild);
	void ReceiveAdditionalInfo(CAr& ar);

	CTexture* m_pTexHouse;
};
#endif

#ifdef __AI_SYSTEM
class CWndRefresher : public CWndNeuz
{
public:
	CItemElem* m_pItemElem[4];
	CTexture* m_pTexture[4];
	DWORD m_dwitemid[4];
	int n_MoverSelectCount;
	DWORD m_CoolTime;
	CSfx* dSfx;
	DWORD m_dwsfxtime;
	BOOL bStart;
	BOOL bStartmp;
	BOOL bStartfp;
	CRect    m_rect[60];
	vector<int> Skill;
	vector<int> BuffItem;
	CWndRefresher(void);
	~CWndRefresher(void);


	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();


public:
	CRect      m_Rect[4];
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnInitialUpdate();
	virtual BOOL Process();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL    OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	BOOL IsBuffItem(DWORD itemId);

};
#endif

#ifdef __FL_FARM_DISPLAY
class CWndFarmDisplaySet : public CWndNeuz
{
	CWndEdit* m_pEditHour;
	CWndEdit* m_pEditMin;
	CWndEdit* m_pEditSec;

public:
	CWndFarmDisplaySet();
	~CWndFarmDisplaySet();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};
class CWndFarmDisplay : public CWndNeuz
{
	const int ONEHEIGHT = 24;
	const int DEFAULTELEMENTS = 6;
	const DWORD m_dwLineColor = 0xffa1a1a1;
	const DWORD m_dwTextColor = 0xFFEBAD18;

	int m_nWndState;

	int m_nTotalKills, m_nTotalKillsOld;
	__int64 m_iTotalDamage, m_iTotalDamageOld;
	__int64 m_iTotalPenya;
	int m_nPenyaOld;

	CWndButton* m_pWndBtnStartStop;
	CWndButton* m_pWndBtnPause;
	CWndButton* m_pWndBtnRefresh;
	CWndButton* m_pWndBtnWiki;
	CWndButton* m_pWndBtnSize;
	CWndButton* m_pWndBtnClose;

	CTexture* m_pTextureStartTime;
	CTexture* m_pTextureTotalKills;
	CTexture* m_pTextureTotalDamage;
	CTexture* m_pTextureTotalPenya;
	CTexture* m_pTextureTotalPerins;

	BOOL m_bSmall;

	time_t m_tTimer;
	time_t m_tPauseTimer;

	CWndFarmDisplaySet* m_pWndFarmDisplaySet;

	CTexture* m_pTexBtnStart;
	CTexture* m_pTexBtnStop;

public:
	enum { STATE_OFF, STATE_ON, STATE_PAUSE };

	CWndFarmDisplay();
	virtual ~CWndFarmDisplay();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	BOOL Process();
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = 1);
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);

	void UpdateSize(BOOL bManual = FALSE);
	void AddKills() { m_nWndState == STATE_ON ? m_nTotalKills++ : 0; };
	void AddDamage(__int64 nVal) { m_nWndState == STATE_ON ? m_iTotalDamage += nVal : 0; };
	void AddPenya(__int64 nVal) { m_nWndState == STATE_ON ? m_iTotalPenya += nVal : 0; };
	void AddItem(DWORD dwItem, int nVal);
	void AddFromWiki(DWORD dwItem);
	void ResetValues();
	void InitState(int nState, int nHour = 0, int nMin = 0, int nSec = 0);
};
#endif

#ifdef __FL_PACKED_ITEMS
class CWndPackedItems : public CWndNeuz
{
	CItemElem* m_pItemElem;
	CWndListBox* m_pListBox;
	CWndButton* m_pButtonUse;
	CWndButton* m_pButtonRestore;
	DWORD m_dwHighAlpha;

public:
	CWndPackedItems();
	virtual ~CWndPackedItems();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	BOOL Process();
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = 1);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);

	bool SetItem(CItemElem* pItemElem);
	void ClearItem();
	void RefreshListBox();
	CItemElem* GetItem() { return m_pItemElem; }
	void DrawItemlist(C2DRender* p2DRender);
	bool IsListboxBgState();
};
class CWndPackedItemsPreview : public CWndNeuz
{
	CItemElem m_ItemElem;
	CWndListBox* m_pListBox;

public:
	CWndPackedItemsPreview();
	virtual ~CWndPackedItemsPreview();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);

	bool SetItem(CItemElem* pItemElem);
	void DrawItemlist(C2DRender* p2DRender);
};
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
class CWndWof : public CWndNeuz
{
	const CPoint CENTER = CPoint(270, 280);
	const int RADIUS = 240;
	const int RADIUS_ITEMS = RADIUS - 28;

	int m_nWndState;

	double m_arrowAngle;
	double m_arrowDistance;
	double m_initialArrowDistance;

	int m_nLampStateNoneRenderMode; // 0 - m_nCurrentYellowLamp, 1 - even, 2 - odd
	DWORD m_dwTimerChangeLampMode;
	int m_nTotalLampModeChanges;
	bool m_bLampGoToNext;
	int m_nCurrentYellowLamp;
	DWORD m_dwTimerChangeLamp;

	bool m_bWinItemFlash;
	DWORD m_dwTimerChangeWinItemFlash;
	int m_nTotalItemFlashChanges;

	CWndButton* m_pWndBtnInit;
	CWndButton* m_pWndBtnClose;

	CTexture* m_pTexFLGray;
	CTexture* m_pTexFLYellow;
	CTexture* m_pTexArrow;
	CTexture* m_pTexItemBg;

	vector<WOF_ITEM> m_vecSortedItem;
	CWndListBox* m_pWndListChances;

public:
	CWndWof();
	virtual ~CWndWof();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	BOOL Process();

	enum { STATE_NONE, STATE_WAIT_INIT, STATE_INIT, STATE_WAIT_FINISH, STATE_FINISH };
	void SetState(int nState, int nVal = -1);
	
	void DrawWheel(C2DRender* p2DRender);
	void DrawTicket(C2DRender* p2DRender);
	void DrawChances(C2DRender* p2DRender);
	void DrawHistory(C2DRender* p2DRender);

	void ProcessLamp();
	void ProcessGame();
	void ProcessItemFlash();

	double GetAngle(int nZone);
	double CalculateSpeed(double initialSpeed, double remainingDistance, double totalDistance);

	void ResetLamps();
	void ResetListChances();
};
#endif

#ifdef __FL_RECORD_BOOK
class CWndRecordBook : public CWndNeuz
{
	CWndTreeCtrl* m_pWndTreeCategories;
	CWndListBox* m_pListBoxRecords;
	CWndStatic* m_pStaticPlayerName;

	std::string m_strKeyword;
	std::unordered_map<std::string, RecordType> m_mapKeyword;

	CMover m_Mover;
	FLOAT	m_fRot;
	bool	m_bLButtonDownRot;
	CPoint	m_OldPos;

	CTexture* m_pTexTop[3];

public:
	CWndRecordBook();
	virtual ~CWndRecordBook();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	BOOL Process();
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	int GetCategoryByKeyword();
	void RefreshListBox();
	void DrawListBox(C2DRender* p2DRender);
	bool IsTimeValue(int nRecordType);
	void RefreshSelectedCharacter();
};
#endif

#ifdef __FL_MONSTER_ALBUM
class CWndMonsterAlbum : public CWndNeuz
{
	CWndListBox* m_pListBoxMonsters;
	CWndButton* m_pWndBtnHideUnlocked;
	LPWNDCTRL m_wndCtrlProgressBar;
	CTexture* m_pLockTexture;
	CTexture* m_pRewardTexture;

	struct ALBUM_STRUCT
	{
		MoverProp* pMoverProp;
		ItemProp* pCardPieceProp;
		DWORD dwWorldID;
		DWORD dwChanceToObtain;
		int nProgress;

		ALBUM_STRUCT()
		{
			pMoverProp = nullptr;
			pCardPieceProp = nullptr;
			dwWorldID = NULL_ID;
			dwChanceToObtain = 0;
			nProgress = 0;
		}

		bool operator<(const ALBUM_STRUCT& other) const 
		{
			return pMoverProp->dwLevel < other.pMoverProp->dwLevel;
		}
	};

	struct MODEL_STRUCT
	{
		CModelObject* pModelObject;
		float fRadius;

		MODEL_STRUCT()
		{
			pModelObject = nullptr;
			fRadius = 0.0f;
		}
	};

	vector<ALBUM_STRUCT> m_vecSortedMonsters;
	unordered_map<DWORD, MODEL_STRUCT> m_mapMonsterModels;

	LPWNDCTRL m_wndCtrl[MAX_BOSS_CARDS];
	CTexture* m_pBossCardTexture[MAX_BOSS_CARDS];

	DWORD m_dwHighAlpha;

	//CWndText* m_pWndTextStats;
	CWndText* m_pWndTextDescription;

public:
	CWndMonsterAlbum();
	virtual ~CWndMonsterAlbum();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	BOOL Process();
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void SerializeRegInfo(CAr& ar, DWORD& dwVersion);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);

	void DrawProgressBar(C2DRender* p2DRender);
	void DrawListBox(C2DRender* p2DRender);
	bool IsBossCardSlotBgState();

	void UpdateMonsterList();
	void UpdateProgress(DWORD dwID);
	void UpdateBossCardTexture(int nSlot);
	void UpdateStatSum();
	void UpdateDescription();
};
#endif

#ifdef __FL_TDM
class CWndTDMRegister : public CWndNeuz
{
	CWndListBox* m_pListPlayers;

	bool m_bLastTDM;
	CTimeSpan m_ct;
	time_t m_tEndTime;
	time_t m_tCurrentTime;
	int m_nNextRefresh;

	int m_nMaxPlayerCount;
	vector<u_long> m_vecRegisteredPlayers;

public:
	CWndTDMRegister(void);
	~CWndTDMRegister(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL Process(void);

	void DrawListBox(C2DRender* p2DRender);

	void LoadData();
	void AcceptData(CAr& ar);
	void RegisterPlayer(u_long idPlayer);
	void UpdateStatic();
	void SetTime(time_t tTime) { m_tCurrentTime = 0; m_tEndTime = time_null() + tTime; }
};

class CWndActivities : public CWndNeuz
{
public:
#ifdef __FL_FFA
	CWndFFARegister		m_wndFFA;
#endif
#ifdef __FL_TDM
	CWndTDMRegister		m_wndTDM;
#endif

	CWndActivities();
	~CWndActivities();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
};

class CWndTDMResult : public CWndNeuz
{
private:
	vector<TDM_PLAYER> m_vecPlayersTDM;

public:
	CWndTDMResult(void);
	~CWndTDMResult(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	vector<TDM_PLAYER>* GetPlayerList()
	{
		return &m_vecPlayersTDM;
	}
};
#endif

#endif // !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)

