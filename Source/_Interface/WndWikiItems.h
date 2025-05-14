#pragma once

#ifdef __WIKI_ITEMS_V2
class CWndWikiItems: public CWndNeuz
{
private:
#ifdef __FL_FARM_DISPLAY
	int m_nMode;
#endif
	CWndEdit* m_pNameFilter;
	CWndEdit* m_pMinLevelFilter;
	CWndEdit* m_pMaxLevelFilter;

	CWndComboBox* m_pComboSex;
	CWndComboBox* m_pComboJob;
	CWndListBox* m_pListItems;

	CString m_strKeyword;
	CWndMenu m_Menu;
	vector<ItemProp*> m_vecItems;

	CWndComboBox* m_pWndComboFilter;

public:
	CWndWikiItems();
	~CWndWikiItems();
	virtual BOOL Initialize(CWndBase *pWndParent = NULL, DWORD dwStyle = 0);
	virtual void OnInitialUpdate();
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT *pLResult);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	void ApplyFilters();
	void SortByItemLevel(BOOL bHighest);

#ifdef __FL_FARM_DISPLAY
	void SetMode(int nMode) { m_nMode = nMode; };
#endif

	vector<ItemProp*>* GetItemList()
	{
		return &m_vecItems;
	}
};
#endif