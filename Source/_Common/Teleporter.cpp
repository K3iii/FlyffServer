#include "StdAfx.h"
#include "Teleporter.h"
#include "defineText.h"
#ifdef __WORLDSERVER
#include "User.h"
#endif
#ifdef __CLIENT
#include "ResData.h"
#include "DPClient.h"
extern CDPClient g_DPlay;
#endif

#ifdef __FL_TELEPORT
CTeleporter::CTeleporter()
{

}
CTeleporter::~CTeleporter()
{
	mapTeleporter.clear();
}
CTeleporter* CTeleporter::GetInstance()
{
	static CTeleporter sTP;
	return &sTP;
}
void CTeleporter::ReadConfig(const LPCTSTR strFile)
{
	CScript s;
	if (!s.Load(strFile))
	{
		Error("CTeleporter::ReadScript cant load %s", strFile);
		return;
	}

	mapTeleporter.clear();
	int nFirstKey = 0;

	s.GetToken();
	while (s.tok != FINISHED)
	{
		if (s.Token == "TELEPORT_CASE")
		{
			TELEPORT_PLACES tmp;
			tmp.dwWorldID = s.GetNumber();
			tmp.vPos.x = static_cast<float>(s.GetNumber());
			tmp.vPos.y = static_cast<float>(s.GetNumber());
			tmp.vPos.z = static_cast<float>(s.GetNumber());

			// Type
			s.GetToken();
			BYTE nType = TYPE_CITY;
			if (s.Token == "TYPE_ZONE")
				nType = TYPE_ZONE;
			else if (s.Token == "TYPE_DUNGEON")
				nType = TYPE_DUNGEON;

			// Name
			s.GetToken();
			CString strName;
			strName.Format("%s", s.Token);

			// Texture
			s.GetToken();
			CString strTexture;
			strTexture.Format("%s", s.Token);

			// Description
			s.GetToken();
			CString strDescription;
			strDescription.Format("%s", s.Token);

#ifdef __CLIENT
			tmp.nType = nType;
			tmp.strName = strName;
			tmp.strTextureName = strTexture;
			tmp.strDescription = strDescription;

			s.GetToken();
			DWORD dwItem = s.GetNumber();
			while (s.Token != "}")
			{
				if (!prj.GetItemProp(dwItem))
				{
					Error("CTeleporter::ReadConfig: wrong item id");
					mapTeleporter.clear();
					return;
				}

				tmp.vecItems.push_back(dwItem);
				dwItem = s.GetNumber();
			}
#endif

			mapTeleporter.insert(make_pair(nFirstKey, tmp));
			nFirstKey++;
		}

		s.GetToken();
	}

}
#ifdef __WORLDSERVER
void CTeleporter::DoTeleport(int nKey, CUser* pUser)
{
	if (!IsValidObj(pUser))
		return;

	auto it = mapTeleporter.find(nKey);
	if (it == mapTeleporter.end())
		return;

	CWorld* pUserWorld = pUser->GetWorld();
	if (!pUserWorld)
		return;

	DWORD dwState = pUser->GetSummonState(FALSE);
	if (dwState != 0)
	{
		pUser->AddDefinedText(dwState);
		return;
	}

	pUser->REPLACE(g_uIdofMulti, it->second.dwWorldID, it->second.vPos, REPLACE_FORCE, nDefaultLayer);
}
#endif

#ifdef __CLIENT
CWndTeleport::CWndTeleport(void)
	: pWndListLocations(NULL)
	, pWndListItems(NULL)
	, pWndComboFilter(NULL)
	, pLocationName(NULL)
	, pWndDescription(NULL)
	, pTexture(NULL)
{

}

CWndTeleport::~CWndTeleport(void)
{
	vecTeleports.clear();
}

BOOL CWndTeleport::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_TELEPORT, 0, CPoint(0, 0), pWndParent);
}

void CWndTeleport::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	pWndListLocations = (CWndListBox*)GetDlgItem(WIDC_LISTBOX);
	pWndListItems = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	pLocationName = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	pWndDescription = (CWndText*)GetDlgItem(WIDC_TEXT);

	pWndComboFilter = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	pWndComboFilter->AddWndStyle(EBS_READONLY);
	pWndComboFilter->AddString("All");
	pWndComboFilter->AddString("City");
	pWndComboFilter->AddString("Zone");
	pWndComboFilter->AddString("Dungeon");
	pWndComboFilter->SetCurSel(0);

	Refresh();
	MoveParentCenter();
}

void CWndTeleport::OnDraw(C2DRender* p2DRender)
{
	if (pTexture)
	{
		CWndStatic* lpWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
		pTexture->Render(p2DRender, lpWndStatic->GetWndRect().TopLeft());
	}
	CWndNeuz::OnDraw(p2DRender);
}

void CWndTeleport::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndTeleport::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndTeleport::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_SELCHANGE)
	{
		if (nID == WIDC_COMBOBOX1)
			Refresh();
		else if (nID == WIDC_LISTBOX)
			SelectWorld();
	}
	else if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
		{
			if (pWndListLocations->GetCurSel() >= 0)
			{
				g_DPlay.SendTeleportRequest(vecTeleports[pWndListLocations->GetCurSel()].nKey);
				Destroy();
			}
		}
	}
	else if (message == WNM_DBLCLK)
	{
		if (nID == WIDC_LISTBOX1)
		{
			if (GetList() && pWndListItems->GetCurSel() >= 0)
			{
				ItemProp* pProp = prj.GetItemProp(GetList()->at(pWndListItems->GetCurSel()));
#ifdef __MODEL_VIEW
				if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
				{
					if (pProp)
					{
						CItemElem pItemElem;
						pItemElem.m_dwItemId = pProp->dwID;
						g_WndMng.PreviewModel(&pItemElem);
					}
				}
#endif
			}
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndTeleport::Refresh()
{
	vecTeleports.clear();
	pWndListLocations->ResetContent();

	for (auto& it : *CTeleporter::GetInstance()->GetTeleport())
	{
		if (pWndComboFilter->GetCurSel() > 0)
		{
			if (it.second.nType != pWndComboFilter->GetCurSel())
				continue;
		}

		FILTERED_PLACES tmp;
		tmp.nKey = it.first;
		tmp.strName.Format("[%s] %s", GetTypeName(it.second.nType), it.second.strName);
		tmp.strTextureName = it.second.strTextureName;
		tmp.strDescription = it.second.strDescription;
		tmp.vecItems = it.second.vecItems;
		vecTeleports.push_back(tmp);
		pWndListLocations->AddString(tmp.strName);
	}

	if (vecTeleports.size())
	{
		pWndListLocations->SetCurSel(0);
		SelectWorld();
	}
}
void CWndTeleport::SelectWorld()
{
	pLocationName->SetTitle(vecTeleports[pWndListLocations->GetCurSel()].strName);
	pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, vecTeleports[pWndListLocations->GetCurSel()].strTextureName), 0xffff00ff);

	pWndListItems->ResetContent();
	for (auto& it : *GetList())
		pWndListItems->AddString("");

	pWndDescription->SetString(vecTeleports[pWndListLocations->GetCurSel()].strDescription);
}
CString CWndTeleport::GetTypeName(BYTE nType)
{
	if (nType == TYPE_CITY)
		return "City";
	else if (nType == TYPE_ZONE)
		return "Zone";
	else if (nType == TYPE_DUNGEON)
		return "Dungeon";
	else
		return "";
}
vector<DWORD>* CWndTeleport::GetList()
{
	if (pWndListLocations->GetCurSel() >= 0)
		return &vecTeleports[pWndListLocations->GetCurSel()].vecItems;

	return NULL;
}
#endif
#endif