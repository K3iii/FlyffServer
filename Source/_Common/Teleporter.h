#pragma once

enum TeleportTypes
{
	TYPE_CITY = 1,
	TYPE_ZONE,
	TYPE_DUNGEON
};

#ifdef __FL_TELEPORT
struct TELEPORT_PLACES
{
	DWORD dwWorldID;
	D3DXVECTOR3 vPos;
#ifdef __CLIENT
	BYTE nType;
	CString strName;
	CString strTextureName;
	CString strDescription;
	vector<DWORD> vecItems;
#endif
};

class CTeleporter
{
	map<int, TELEPORT_PLACES> mapTeleporter;

public:
	CTeleporter();
	~CTeleporter();
	static CTeleporter* GetInstance();
	map<int, TELEPORT_PLACES>* GetTeleport() { return &mapTeleporter; }

	void ReadConfig(const LPCTSTR strFile);
#ifdef __WORLDSERVER
	void DoTeleport(int nKey, CUser* pUser);
#endif
};


#ifdef __CLIENT
class CWndTeleport : public CWndNeuz
{
private:
	struct FILTERED_PLACES
	{
		int nKey;
		CString strName;
		CString strTextureName;
		CString strDescription;
		vector<DWORD> vecItems;
	};

	vector <FILTERED_PLACES> vecTeleports;
	CWndListBox* pWndListLocations;
	CWndListBox* pWndListItems;
	CWndComboBox* pWndComboFilter;
	CWndStatic* pLocationName;
	CWndText* pWndDescription;
	CTexture* pTexture;

public:
	CWndTeleport(void);
	~CWndTeleport(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	void Refresh();
	void SelectWorld();
	CString GetTypeName(BYTE nType);
	vector<DWORD>* GetList();
};
#endif
#endif