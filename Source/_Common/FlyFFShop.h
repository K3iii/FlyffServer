#pragma once

#ifdef __FL_CASH_SHOP
enum purchase_method
{
	CS_METHOD_VP = 1, CS_METHOD_DP, CS_METHOD_BOTH
};

struct FLYFF_SHOP_ITEMS
{
	int nItemIndex;
	DWORD dwItemID;
	int nPurchaseMethod;
	int nVotePrice;
	int nDonatePrice;
	int nCategory;
	int nAmount;
};

class CFlyFFShop
{
	vector<FLYFF_SHOP_ITEMS> m_vecCShopItems;

public:
	CFlyFFShop();
	~CFlyFFShop();
	static CFlyFFShop* GetInstance();

	vector<FLYFF_SHOP_ITEMS>* GetItemVector() { return &m_vecCShopItems; }
	void SerializeFlyffShop(CAr& ar);
};

#ifdef __CLIENT
class CWndFlyFFShopPurchase : public CWndNeuz
{
	FLYFF_SHOP_ITEMS item;
	CWndComboBox* m_pComboFriends;
	CWndEdit* m_pEditNum;
	CWndButton* m_pButtonsCurrency[2];
	CTexture* m_pTexture;
	vector<u_long> m_vecFriends;

public:
	CWndFlyFFShopPurchase(FLYFF_SHOP_ITEMS itm);
	~CWndFlyFFShopPurchase();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnMouseWndSurface(CPoint point);
};

class CWndFlyFFShop : public CWndNeuz
{
	BOOL m_bClickCategory;
	BOOL m_bClickItem;
	int m_nSelectedTab;
	int m_nSelectedPage;
	int m_nMaxItemsX;
	int m_nMaxItemsY;
	CWndEdit* m_pSearchBox;
	CWndComboBox* m_pComboSex;
	CWndButton* m_pCheckBoxDonate;
	CWndButton* m_pCheckBoxVote;
	CTexture* m_pIconPurchase;
	CTexture* m_pIconPreview;
	vector<FLYFF_SHOP_ITEMS> m_vecFilteredShopItems;

	enum page
	{
		PAGE_DEFAULT, PAGE_PREV, PAGE_NEXT, PAGE_MAX
	};

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

	CRect getItemRect(int id, CPoint sizediff = CPoint(0, 0), ctrltype type = PARENT);
	void saveOriginRects();
	void resetItemRects();

public:
	CWndFlyFFShop(void);
	~CWndFlyFFShop(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize);
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);

	void UpdatePoints();
	void UpdateContent();

	void SetPage(int nMode);
	void UpdatePageStatic(int nMaxPage);
	void RefreshMaxItemsValues();

	CString GetTabName(int nTab);
	CString GetTabTexture(int nTab);

	CTexture* m_pIconDonate;
	CTexture* m_pIconVote;

	CWndFlyFFShopPurchase* m_pWndPurchaseItem;
};
#endif
#endif
