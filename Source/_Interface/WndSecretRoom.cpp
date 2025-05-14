#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndSecretRoom.h"
#include "playerdata.h"

#include "SecretRoom.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX


#if __VER >= 12 // __SECRET_ROOM

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

CWndSecretRoomSelection::CWndSecretRoomSelection()
{
	m_vecGuildList.clear();
	m_mapSelectPlayer.clear();
	m_vecSelectPlayer.clear();
}

CWndSecretRoomSelection::~CWndSecretRoomSelection()
{
}

void CWndSecretRoomSelection::AddCombatPlayer(u_long uiPlayer)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);
	m_vecSelectPlayer.push_back(uiPlayer);

	if (!g_pPlayer) return;

	CGuild* pGuild = g_pPlayer->GetGuild();

	map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.find(uiPlayer);
	CGuildMember* pMember = i->second;
	if (!pMember)	return;

	CString str;

	PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
	str.Format("No.%d  Lv%.2d	%.16s %.10s", pWndList->GetCount() + 1, pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[pPlayerData->data.nJob].szName);

	pWndList->AddString(str);

	vector<u_long>::iterator iter = m_vecGuildList.begin();

	int index = -1;
	int count = 0;

	while (iter != m_vecGuildList.end())
	{
		if (*iter == uiPlayer)
		{
			index = count;
			iter = m_vecGuildList.end();
		}
		else
		{
			count++;
			iter++;
		}
	}

	if (index > -1)
		RemoveGuildPlayer(index);
}

void CWndSecretRoomSelection::AddGuildPlayer(u_long uiPlayer)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_vecGuildList.push_back(uiPlayer);

	CGuild* pGuild = g_pPlayer->GetGuild();

	map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.find(uiPlayer);
	CGuildMember* pMember = i->second;

	CString str;
	PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
	str.Format("Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[pPlayerData->data.nJob].szName);
	pWndList->AddString(str);
}

void CWndSecretRoomSelection::RemoveGuildPlayer(int nIndex)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	pWndList->DeleteString(nIndex);
	m_vecGuildList.erase(m_vecGuildList.begin() + nIndex);
}

void CWndSecretRoomSelection::RemoveCombatPlayer(int nIndex)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);

	pWndList->DeleteString(nIndex);
	m_vecSelectPlayer.erase(m_vecSelectPlayer.begin() + nIndex);

	//Align Text No.
	int nListCount = pWndList->GetCount();
	for (int i = nIndex; i < nListCount; i++)
	{
		CString temp;

		CGuild* pGuild = g_pPlayer->GetGuild();
		map<u_long, CGuildMember*>::iterator iter = pGuild->m_mapPMember.find(m_vecSelectPlayer[i]);
		CGuildMember* pMember = iter->second;
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
		temp.Format("No.%d  Lv%.2d	%.16s %.10s", i + 1, pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[pPlayerData->data.nJob].szName);
		pWndList->SetString(i, temp);
	}
}

void CWndSecretRoomSelection::UpDateGuildListBox()
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	if (pWndList)
	{
		pWndList->ResetContent();

		m_vecGuildList.clear();
		m_mapSelectPlayer.clear();

		CGuild* pGuild = g_pPlayer->GetGuild();
		if (pGuild)
		{

			CGuildMember* pMember;
			for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i)
			{
				pMember = i->second;
				PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
				if (pPlayerData->data.uLogin > 0)
					m_mapSelectPlayer.insert(make_pair(pPlayerData->data.nLevel, pMember));
			}


			CString str;
			for (multimap<int, CGuildMember*>::iterator j = m_mapSelectPlayer.begin(); j != m_mapSelectPlayer.end(); ++j)
			{
				pMember = j->second;
				PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
				if (pPlayerData->data.uLogin > 0)
				{
					str.Format("Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[pPlayerData->data.nJob].szName);
					pWndList->AddString(str);
					m_vecGuildList.push_back(pMember->m_idPlayer);
				}
			}
		}
	}
}

u_long CWndSecretRoomSelection::FindCombatPlayer(u_long uiPlayer)
{
	for (int i = 0; i < (int)(m_vecSelectPlayer.size()); i++)
	{
		if (m_vecSelectPlayer[i] == uiPlayer)
			return m_vecSelectPlayer[i];
	}

	return -1;
}

u_long CWndSecretRoomSelection::FindGuildPlayer(u_long uiPlayer)
{
	for (int i = 0; i < (int)(m_vecGuildList.size()); i++)
	{
		if (m_vecGuildList[i] == uiPlayer)
			return m_vecGuildList[i];
	}

	return -1;
}

void CWndSecretRoomSelection::OnDraw(C2DRender* p2DRender)
{
}

void CWndSecretRoomSelection::EnableFinish(BOOL bFlag)
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_FINISH);

	if (pWndButton)
	{
		pWndButton->EnableWindow(bFlag);
	}
}

void CWndSecretRoomSelection::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	//	if( g_GuildCombat1to1Mng.m_nState != CGuildCombat1to1Mng::GC1TO1_OPEN )
	//	{

	//		Destroy();
	//		return;
	//	}

	MoveParentCenter();

	UpDateGuildListBox();
}

BOOL CWndSecretRoomSelection::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SECRETROOM_SELECTION, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndSecretRoomSelection::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndSecretRoomSelection::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndSecretRoomSelection::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndSecretRoomSelection::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndSecretRoomSelection::Reset()
{
	UpDateGuildListBox();
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);
	pWndListBox->ResetContent();
	m_vecSelectPlayer.clear();
}

BOOL CWndSecretRoomSelection::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON1)
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

		int nCurSel = pWndListBox->GetCurSel();
		if (nCurSel == -1)
			return FALSE;

		if (CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum < (int)(m_vecSelectPlayer.size()))
		{
			CString str;
			str.Format(prj.GetText(TID_GAME_SECRETROOM_SELECTION_MAX), CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum);
			g_WndMng.OpenMessageBox(str);
			return FALSE;
		}

		CGuild* pGuild = g_pPlayer->GetGuild();

		if (pGuild)
		{
			CGuildMember* pGuildMember = pGuild->GetMember(m_vecGuildList[nCurSel]);

			if (pGuildMember)
			{
				if (CPlayerDataCenter::GetInstance()->GetPlayerData(pGuildMember->m_idPlayer)->data.nLevel < CSecretRoomMng::GetInstance()->m_nMinGuildMemberNum)
				{
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SECRETROOM_LIMIT_LEVEL));
					return FALSE;
				}
			}
			else
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SECRETROOM_NOT_GUILD_MEMBER));
				return FALSE;
			}
		}

		u_long uiPlayer;
		uiPlayer = FindCombatPlayer(m_vecGuildList[nCurSel]);

		if (uiPlayer != -1)
		{
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SECRETROOM_ALREADY_ENTRY));
			return FALSE;
		}

		AddCombatPlayer(m_vecGuildList[nCurSel]);
		//RemoveGuildPlayer( nCurSel );
	}
	else if (nID == WIDC_BUTTON2)
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);

		int nCurSel = pWndListBox->GetCurSel();
		if (nCurSel == -1)
			return FALSE;

		u_long uiPlayer;
		uiPlayer = FindGuildPlayer(m_vecSelectPlayer[nCurSel]);

		CGuild* pGuild = g_pPlayer->GetGuild();
		CGuildMember* pGuildMemberl = pGuild->GetMember(m_vecSelectPlayer[nCurSel]);

		if (pGuildMemberl->m_nMemberLv == GUD_MASTER)
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SECRETROOM_DONTREMOVE_GUILDMASTER));
		else
		{
			if (uiPlayer == -1)
			{

				AddGuildPlayer(m_vecSelectPlayer[nCurSel]);
				RemoveCombatPlayer(nCurSel);
			}
			else
			{
				RemoveCombatPlayer(nCurSel);
			}
		}
	}
	else if (nID == WIDC_RESET)
	{
		CWndSecretRoomSelectionResetConfirm* pBox = new CWndSecretRoomSelectionResetConfirm;
		g_WndMng.OpenCustomBox("", pBox);
	}
	else if (nID == WIDC_FINISH)
	{
		if (CSecretRoomMng::GetInstance()->m_nMinGuildMemberNum > (int)(m_vecSelectPlayer.size()))
		{
			CString str;
			str.Format(prj.GetText(TID_GAME_SECRETROOM_MIN_PLAYER), CSecretRoomMng::GetInstance()->m_nMinGuildMemberNum);
			g_WndMng.OpenMessageBox(str);
			return FALSE;
		}

		if (m_vecSelectPlayer.size() == 0)
		{
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SECRETROOM_HAVENOT_PLAYER));
			return FALSE;
		}

		CGuild* pGuild = g_pPlayer->GetGuild();
		CGuildMember* pGuildMemberl;

		if (pGuild)
		{
			BOOL bSkip = FALSE;


			for (int i = 0; i < (int)(m_vecSelectPlayer.size()); i++)
			{
				pGuildMemberl = pGuild->GetMember(m_vecSelectPlayer[i]);

				if (pGuildMemberl)
				{
					if (pGuildMemberl->m_nMemberLv == GUD_MASTER)
					{
						bSkip = TRUE;
						break;
					}
				}
			}

			if (bSkip)
			{
				g_DPlay.SendSecretRoomLineUpMember(m_vecSelectPlayer);
				Destroy();
			}
			else
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SECRETROOM_HAVENOT_MASTER));
				return FALSE;
			}
		}
	}
	else if (nID == WIDC_CLOSE)
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

CWndSecretRoomOffer::CWndSecretRoomOffer()
{
	m_dwMinGold = 0;
	m_dwBackupGold = 0;
	m_dwReqGold = 0;
}

CWndSecretRoomOffer::~CWndSecretRoomOffer()
{
}

void CWndSecretRoomOffer::OnDraw(C2DRender* p2DRender)
{
}

void CWndSecretRoomOffer::EnableAccept(BOOL bFlag)
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_ACCEPT);

	if (pWndButton)
	{
		pWndButton->EnableWindow(bFlag);
	}
}

void CWndSecretRoomOffer::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("SecretRoomDesc.inc")));
	char* szChar = scanner.m_pProg;

	if (pText != NULL && checkflag)
	{
		pText->m_string.AddParsingString(szChar);
		pText->ResetString();
	}

	MoveParentCenter();
}

BOOL CWndSecretRoomOffer::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SECRETROOM_OFFER, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndSecretRoomOffer::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndSecretRoomOffer::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndSecretRoomOffer::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndSecretRoomOffer::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndSecretRoomOffer::SetGold(DWORD nCost)
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

	if (pWndEdit)
	{
		CString str;
		str.Format("%d", nCost);
		pWndEdit->SetString(str);
	}
}

BOOL CWndSecretRoomOffer::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_ACCEPT)
	{
		if (g_pPlayer)
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

			DWORD nCost;
			CString str = pWndEdit->GetString();
			nCost = atoi(str);

			if (m_dwReqGold != 0)
			{
				if (nCost <= m_dwBackupGold)
				{
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT1TO1_MORE_CURRENT_REQUEST));
					return FALSE;
				}

			}
			else
			{
				if (nCost < m_dwMinGold)
				{
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT1TO1_LIMIT_MIN));
					return FALSE;
				}
			}

			CWndSecretRoomOfferMessageBox* pMsg = new CWndSecretRoomOfferMessageBox;
			g_WndMng.OpenCustomBox("", pMsg, this);

			if (m_dwReqGold == 0)
			{
				str.Format(prj.GetText(TID_GAME_GUILDCOMBAT1TO1_MORE_REQUEST), 0, nCost);
			}
			else
			{
				str.Format(prj.GetText(TID_GAME_GUILDCOMBAT1TO1_MORE_REQUEST), m_dwBackupGold, nCost - m_dwBackupGold);
			}

			pMsg->SetValue(str, nCost);
		}
	}
	else if (nID == WIDC_CLOSE)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
CWndSecretRoomChangeTaxRate::CWndSecretRoomChangeTaxRate()
{
	m_nDefaultSalesTax = 0;
	m_nDefaultPurchaseTax = 0;
	m_nChangeSalesTax = 0;
	m_nChangePurchaseTax = 0;
	m_nMinTax = 0;
	m_nMaxTax = 0;
	m_nCont = 0;
}

CWndSecretRoomChangeTaxRate::~CWndSecretRoomChangeTaxRate()
{

}

void CWndSecretRoomChangeTaxRate::OnDraw(C2DRender* p2DRender)
{
}

void CWndSecretRoomChangeTaxRate::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
	CString strTex;
	strTex.Format("%d%%", m_nDefaultSalesTax);
	pStatic->SetTitle(strTex);

	pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	strTex.Format("%d%%", m_nDefaultPurchaseTax);
	pStatic->SetTitle(strTex);

	pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
	strTex.Format("%d%%", m_nChangeSalesTax);
	pStatic->SetTitle(strTex);

	pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	strTex.Format("%d%%", m_nChangePurchaseTax);
	pStatic->SetTitle(strTex);


	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("SecretRoomChangeTaxRate.inc")));
	char* szChar = scanner.m_pProg;

	if (pText != NULL && checkflag)
	{
		pText->m_string.AddParsingString(szChar);
		pText->ResetString();
	}

	MoveParentCenter();
}

BOOL CWndSecretRoomChangeTaxRate::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SECRETROOM_TAXRATE_CHANGE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndSecretRoomChangeTaxRate::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_PLUS_STAX:
	{
		m_nChangeSalesTax++;
		if (m_nChangeSalesTax > m_nMaxTax)
			m_nChangeSalesTax = m_nMaxTax;
	}
	break;
	case WIDC_MINUS_STAX:
	{
		m_nChangeSalesTax--;
		if (m_nChangeSalesTax < m_nMinTax)
			m_nChangeSalesTax = m_nMinTax;
	}
	break;
	case WIDC_PLUS_PTAX:
	{
		m_nChangePurchaseTax++;
		if (m_nChangePurchaseTax > m_nMaxTax)
			m_nChangePurchaseTax = m_nMaxTax;
	}
	break;
	case WIDC_MINUS_PTAX:
	{
		m_nChangePurchaseTax--;
		if (m_nChangePurchaseTax < m_nMinTax)
			m_nChangePurchaseTax = m_nMinTax;
	}
	break;
	case WIDC_OK:
	{
		CWndSecretRoomChangeTaxRateMsgBox* pMsg = new CWndSecretRoomChangeTaxRateMsgBox;
		CString strMsg;
		g_WndMng.OpenCustomBox("", pMsg, this);
		strMsg.Format(prj.GetText(TID_GAME_SECRETROOM_CHANGETEX), m_nChangeSalesTax, m_nChangePurchaseTax);
		pMsg->SetValue(strMsg, m_nChangeSalesTax, m_nChangePurchaseTax, m_nCont);
	}
	break;
	}

	if (nID == WIDC_PLUS_STAX || nID == WIDC_MINUS_STAX)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
		CString strTex;
		strTex.Format("%d%%", m_nChangeSalesTax);
		pStatic->SetTitle(strTex);
	}
	else if (nID == WIDC_PLUS_PTAX || nID == WIDC_MINUS_PTAX)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
		CString strTex;
		strTex.Format("%d%%", m_nChangePurchaseTax);
		pStatic->SetTitle(strTex);
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSecretRoomChangeTaxRate::SetDefaultTax(int nMinTax, int nMaxTax, BYTE nCont)
{
	m_nCont = nCont;
	m_nDefaultSalesTax = (int)(CTax::GetInstance()->GetSalesTaxRate(nCont) * 100);
	m_nDefaultPurchaseTax = (int)(CTax::GetInstance()->GetPurchaseTaxRate(nCont) * 100);

	if (m_nDefaultSalesTax < nMinTax)
		m_nChangeSalesTax = nMinTax;
	else
		m_nChangeSalesTax = m_nDefaultSalesTax;

	if (m_nDefaultPurchaseTax < nMinTax)
		m_nChangePurchaseTax = nMinTax;
	else
		m_nChangePurchaseTax = m_nDefaultPurchaseTax;

	m_nMinTax = nMinTax;
	m_nMaxTax = nMaxTax;
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
CWndSecretRoomCheckTaxRate::CWndSecretRoomCheckTaxRate()
{
	m_nSalesTax = 0;
	m_nPurchaseTax = 0;
	m_dwGuildId = 0;
}

CWndSecretRoomCheckTaxRate::~CWndSecretRoomCheckTaxRate()
{

}

void CWndSecretRoomCheckTaxRate::OnDraw(C2DRender* p2DRender)
{
}

void CWndSecretRoomCheckTaxRate::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	BYTE nCont = CTax::GetInstance()->GetContinent(g_pPlayer);

	m_nSalesTax = (int)(CTax::GetInstance()->GetSalesTaxRate(g_pPlayer) * 100);
	m_nPurchaseTax = (int)(CTax::GetInstance()->GetPurchaseTaxRate(g_pPlayer) * 100);
	__TAXINFO* taxInfo = CTax::GetInstance()->GetTaxInfo(nCont);
	if (taxInfo)
		m_dwGuildId = taxInfo->dwId;
	else
		m_dwGuildId = NULL_ID;

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	CString strTex;
	strTex.Format("%d%%", m_nSalesTax);
	pStatic->SetTitle(strTex);

	pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
	strTex.Format("%d%%", m_nPurchaseTax);
	pStatic->SetTitle(strTex);

	CGuild* pGuild = g_GuildMng.GetGuild(m_dwGuildId);

	CString strFormat;
	if (pGuild)
	{
		strFormat = pGuild->m_szGuild;
		g_WndMng.StringSize(strFormat, 16);
		pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
		pStatic->SetTitle(strFormat);

		strFormat = CPlayerDataCenter::GetInstance()->GetPlayerString(pGuild->m_idMaster);
		g_WndMng.StringSize(strFormat, 16);
		pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC9);
		pStatic->SetTitle(strFormat);
	}
	else
	{
		pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
		pStatic->SetTitle(prj.GetText(TID_GAME_SECRETROOM_TEX_NOGUILD));
		pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC9);
		pStatic->SetTitle(prj.GetText(TID_GAME_SECRETROOM_TEX_NOGUILD));
	}


	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("SecretRoomCheckTaxRate.inc")));
	char* szChar = scanner.m_pProg;

	if (pText != NULL && checkflag)
	{
		pText->m_string.AddParsingString(szChar);
		pText->ResetString();
	}

	MoveParentCenter();
}

BOOL CWndSecretRoomCheckTaxRate::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SECRETROOM_TAXRATE_CHECK, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndSecretRoomCheckTaxRate::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
		Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

BOOL CWndSecretRoomSelectionResetConfirm::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndMessageBox::Initialize(prj.GetText(TID_GAME_SECRETROOM_REMAKE_MAKEUP),
		pWndParent,
		MB_OKCANCEL);
}

BOOL CWndSecretRoomSelectionResetConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case IDOK:
	{
		CWndSecretRoomSelection* pWndGuildSecretRoomSelection = (CWndSecretRoomSelection*)g_WndMng.GetWndBase(APP_SECRETROOM_SELECTION);

		if (pWndGuildSecretRoomSelection)
			pWndGuildSecretRoomSelection->Reset();

		Destroy();
	}
	break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
CWndSecretRoomOfferState::CWndSecretRoomOfferState()
{
	Init(0);

	m_tEndTime = 0;
	m_tCurrentTime = 0;
}

CWndSecretRoomOfferState::~CWndSecretRoomOfferState()
{
}

void CWndSecretRoomOfferState::Init(time_t lTime)
{
}

void CWndSecretRoomOfferState::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	MoveParentCenter();
}

void CWndSecretRoomOfferState::InsertTitle(const char szTitle[])
{
	CString strTitle;
	strTitle.Format("%s - %s", GetTitle(), szTitle);
	SetTitle(strTitle);
}

BOOL CWndSecretRoomOfferState::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SECRETROOM_OFFERSTATE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndSecretRoomOfferState::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndSecretRoomOfferState::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndSecretRoomOfferState::OnLButtonUp(UINT nFlags, CPoint point)
{
}


int CWndSecretRoomOfferState::GetSelectIndex(const CPoint& point)
{
	return -1;
}

void CWndSecretRoomOfferState::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndSecretRoomOfferState::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void CWndSecretRoomOfferState::OnRButtonDown(UINT nFlags, CPoint point)
{
}

void CWndSecretRoomOfferState::OnRButtonUp(UINT nFlags, CPoint point)
{
}

BOOL CWndSecretRoomOfferState::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_CLOSE)
	{
		Destroy();
	}
	else
		if (nID == WIDC_BUTTON1)
		{
			Destroy();
			g_DPlay.SendSecretRoomTenderOpenWnd();
		}
		else
			if (nID == WIDC_BUTTON2)
			{
				g_DPlay.SendSecretRoomTenderView();
			}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSecretRoomOfferState::InsertGuild(const char szGuild[], const char szName[], int nNum)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	CString str;
	str.Format("No.%.2d		%s", nNum, szGuild);
	pWndList->AddString(str);
}

void CWndSecretRoomOfferState::OnDraw(C2DRender* p2DRender)
{
	if (m_tCurrentTime > 0)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);

		CString str1;
		CString str2;
		CString str3;
		CString str4;
		CString str;

		str1.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(m_ct.GetDays()));
		str2.Format(prj.GetText(TID_PK_LIMIT_HOUR), m_ct.GetHours());
		str3.Format(prj.GetText(TID_PK_LIMIT_MINUTE), m_ct.GetMinutes());
		str4.Format(prj.GetText(TID_PK_LIMIT_SECOND), m_ct.GetSeconds());

		str.Format("%s %s %s %s", str1, str2, str3, str4);
		pStatic->SetTitle(str);
	}
}

void CWndSecretRoomOfferState::SetRate(int nRate)
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);

	CString str;
	str.Format("%d", nRate);

	if (nRate == -1)
		str = " ";

	pWndStatic->SetTitle(str);
	if (8 < nRate)
		pWndStatic->m_dwColor = 0xFFFF0000;
}

BOOL CWndSecretRoomOfferState::Process()
{
	m_tCurrentTime = (m_tEndTime - time_null());

	if (m_tEndTime && m_tCurrentTime > 0)
	{
		m_ct = m_tCurrentTime;//long)(m_dwCurrentTime / 1000.0f) );
	}

	return TRUE;
}

void CWndSecretRoomOfferState::SetGold(int nGold)
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	pWndStatic->AddWndStyle(WSS_MONEY);

	CString str;
	str.Format("%d", nGold);
	pWndStatic->SetTitle(str);
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

BOOL CWndSecretRoomOfferMessageBox::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndMessageBox::Initialize("", pWndParent, MB_OKCANCEL);
}

void CWndSecretRoomOfferMessageBox::SetValue(CString str, DWORD nCost)
{
	m_wndText.SetString(str);
	m_nCost = nCost;
}

BOOL CWndSecretRoomOfferMessageBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case IDOK:
		g_DPlay.SendSecretRoomTender(m_nCost);
		SAFE_DELETE(g_WndMng.m_pWndSecretRoomOffer);
		Destroy();
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

CWndSecretRoomInfoMsgBox::CWndSecretRoomInfoMsgBox()
{
}

CWndSecretRoomInfoMsgBox::~CWndSecretRoomInfoMsgBox()
{
}

BOOL CWndSecretRoomInfoMsgBox::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SECRETROOM_MSG, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndSecretRoomInfoMsgBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_YES:
	{
		g_DPlay.SendSecretRoomTeleportToNPC();
		Destroy();
	}
	break;
	case WIDC_NO:
	{
		Destroy();
	}
	case 10000:
	{
		Destroy();
	}
	break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSecretRoomInfoMsgBox::SetString(CHAR* szChar)
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	pWndText->m_string.AddParsingString(szChar);
	pWndText->ResetString();
}

void CWndSecretRoomInfoMsgBox::SetString(CString strMsg)
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	pWndText->m_string.AddParsingString(strMsg);
	pWndText->ResetString();
}

void CWndSecretRoomInfoMsgBox::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	MoveParentCenter();
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

BOOL CWndSecretRoomChangeTaxRateMsgBox::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndMessageBox::Initialize("", pWndParent, MB_OKCANCEL);
}

void CWndSecretRoomChangeTaxRateMsgBox::SetValue(CString str, int nSalesTax, int nPurchaseTax, BYTE nCont)
{
	m_wndText.SetString(str);
	m_nSalesTax = nSalesTax;
	m_nPurchaseTax = nPurchaseTax;
	m_nCont = nCont;
}

BOOL CWndSecretRoomChangeTaxRateMsgBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case IDOK:
		g_DPlay.SendTaxRate(m_nCont, m_nSalesTax, m_nPurchaseTax);
		SAFE_DELETE(g_WndMng.m_pWndSecretRoomChangeTaxRate);
		Destroy();
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

CWndSecretRoomCancelConfirm::CWndSecretRoomCancelConfirm()
{
}

CWndSecretRoomCancelConfirm::~CWndSecretRoomCancelConfirm()
{
}

void CWndSecretRoomCancelConfirm::OnDraw(C2DRender* p2DRender)
{
}

void CWndSecretRoomCancelConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	if (pWndText)
	{
		pWndText->SetString(_T(prj.GetText(TID_GAME_SECRETROOM_CANCEL)), 0xff000000);
		pWndText->ResetString();
	}

	MoveParentCenter();
}

BOOL CWndSecretRoomCancelConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SECRETROOM_CANCEL_CONFIRM, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndSecretRoomCancelConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndSecretRoomCancelConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndSecretRoomCancelConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndSecretRoomCancelConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndSecretRoomCancelConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_YES)
	{
		if (g_pPlayer)
			g_DPlay.SendSecretRoomTenderCancelReturn();
		Destroy();
	}
	else if (nID == WIDC_NO)
		Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
CWndSecretRoomBoard::CWndSecretRoomBoard()
{
}

CWndSecretRoomBoard::~CWndSecretRoomBoard()
{
}

void CWndSecretRoomBoard::OnDraw(C2DRender* p2DRender)
{
}

void CWndSecretRoomBoard::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	rect.left += 8;
	rect.right -= 4;
	rect.top += 8;
	rect.bottom /= 2;

	MoveParentCenter();
}

BOOL CWndSecretRoomBoard::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SECRETROOM_BOARD, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndSecretRoomBoard::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndSecretRoomBoard::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndSecretRoomBoard::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndSecretRoomBoard::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndSecretRoomBoard::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON1)
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSecretRoomBoard::SetString()
{
	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("SecretRoomBoard.inc")));
	char* szChar = scanner.m_pProg;

	if (pText != NULL && checkflag)
	{
		pText->m_string.AddParsingString(szChar);
		pText->ResetString();
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndSecretRoomQuick
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool prMemberLevelDesc(u_long player1, u_long player2)
{
	bool rtn_val = false;

	CGuildMember* pMember1 = g_pPlayer->GetGuild()->GetMember(player1);
	CGuildMember* pMember2 = g_pPlayer->GetGuild()->GetMember(player2);

	if (pMember1 && pMember2)
	{
		PlayerData* pPlayerData1 = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember1->m_idPlayer);
		PlayerData* pPlayerData2 = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember2->m_idPlayer);

		if (pPlayerData1 && pPlayerData2)
		{
			if (pPlayerData1->data.nLevel < pPlayerData2->data.nLevel)
				rtn_val = true;
			else if (pPlayerData1->data.nLevel == pPlayerData2->data.nLevel)
			{
				if (pPlayerData1->data.nJob > pPlayerData2->data.nJob)
					rtn_val = true;
			}
		}
	}

	return rtn_val;
}

CWndSecretRoomQuick::CWndSecretRoomQuick()
{
	m_pVBGauge = NULL;
	m_MemberCount = 0;
	m_nWndHeight = 0;
	m_bMini = FALSE;
	m_FocusMemberid = 0;

	if (g_pPlayer->GetGuild())
		m_vecGuildMemberId.push_back(g_pPlayer->GetGuild()->m_idMaster);

	m_StaticID[0] = WIDC_STATIC_MEM1;
	m_StaticID[1] = WIDC_STATIC_MEM2;
	m_StaticID[2] = WIDC_STATIC_MEM3;
	m_StaticID[3] = WIDC_STATIC_MEM4;
	m_StaticID[4] = WIDC_STATIC_MEM5;
	m_StaticID[5] = WIDC_STATIC_MEM6;
	m_StaticID[6] = WIDC_STATIC_MEM7;
	m_StaticID[7] = WIDC_STATIC_MEM8;
	m_StaticID[8] = WIDC_STATIC_MEM9;
	m_StaticID[9] = WIDC_STATIC_MEM10;
	m_StaticID[10] = WIDC_STATIC_MEM11;
}

CWndSecretRoomQuick::~CWndSecretRoomQuick()
{
	DeleteDeviceObjects();
}

HRESULT CWndSecretRoomQuick::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if (m_pVBGauge == NULL)
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL);
	return S_OK;
}
HRESULT CWndSecretRoomQuick::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);
	return S_OK;
}
HRESULT CWndSecretRoomQuick::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);
	return S_OK;
}

void CWndSecretRoomQuick::SerializeRegInfo(CAr& ar, DWORD& dwVersion)
{
	CWndNeuz::SerializeRegInfo(ar, dwVersion);
}

void CWndSecretRoomQuick::OnDraw(C2DRender* p2DRender)
{

	if (m_MemberCount <= 0 || m_MemberCount != m_vecGuildMemberId.size() || g_pPlayer->GetGuild() == NULL)
		return;

	int nDrawCount = 0;
	if (m_bMini)
		nDrawCount = 1;
	else
		nDrawCount = m_vecGuildMemberId.size();

	for (int i = 0; i < nDrawCount; i++)
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl(m_StaticID[i]);

		if (lpWndCtrl == NULL)
		{
			Error("SecretRoomQuick Draw i=%d", i);
			continue;
		}

		CRect rect = lpWndCtrl->rect;
		u_long uPlayerid = m_vecGuildMemberId[i];

		CGuildMember* pMember = g_pPlayer->GetGuild()->GetMember(uPlayerid);
		if (pMember)
		{
			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
			CMover* pObjMember = pMember->GetMover();

			CMover* pFocusObjMember = NULL;
			if (m_FocusMemberid > 0)
				pFocusObjMember = g_pPlayer->GetGuild()->GetMember(m_FocusMemberid)->GetMover();

			if (pPlayerData == NULL || pWndWorld == NULL)
				return;

			CString strMember;

			if (m_bMini && pFocusObjMember)
				pObjMember = pFocusObjMember;

			if (pFocusObjMember && pFocusObjMember == pObjMember)
				p2DRender->RenderFillRect(rect, 0x60ffff00);


			DWORD dwColor = 0xff000000;

			if (IsValidObj(pObjMember))
			{
				if (pObjMember->GetHitPoint() == 0)
					dwColor = 0xffff0000;
				else if (((FLOAT)pObjMember->GetHitPoint()) / ((FLOAT)pObjMember->GetMaxHitPoint()) < .1f)
					dwColor = 0xffffff00;

				if (i == 0)
				{
					dwColor = 0xff1fb72d;

					if (pObjMember->IsMaster())
						strMember.Format(prj.GetText(TID_GAME_QUICK_MARK_MASTER), pObjMember->GetLevel(), pObjMember->GetName());
					else if (pObjMember->IsHero() || pObjMember->IsLegendHero())
						strMember.Format(prj.GetText(TID_GAME_QUICK_MARK_HERO), pObjMember->GetLevel(), pObjMember->GetName());
					else
						strMember.Format("%d. %s", pObjMember->GetLevel(), pObjMember->GetName());
				}
				else
				{
					if (pObjMember->IsMaster())
						strMember.Format(prj.GetText(TID_GAME_QUICK_MARK_MASTER), pObjMember->GetLevel(), pObjMember->GetName());
					else if (pObjMember->IsHero() || pObjMember->IsLegendHero())
						strMember.Format(prj.GetText(TID_GAME_QUICK_MARK_HERO), pObjMember->GetLevel(), pObjMember->GetName());
					else
						strMember.Format("%d. %s", pObjMember->GetLevel(), pObjMember->GetName());
				}
			}
			else
			{
				dwColor = 0xff878787;
				if (pPlayerData->data.uLogin <= 0)
					dwColor = 0xff000000;

				CString	strTemp2;
				int nLevel = pPlayerData->data.nLevel;
				int nJob = pPlayerData->data.nJob;

				if (MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER)
					strTemp2.Format("%d%s", nLevel, prj.GetText(TID_GAME_TOOLTIP_MARK_MASTER));
				else if (MAX_MASTER <= nJob)
					strTemp2.Format("%d%s", nLevel, prj.GetText(TID_GAME_TOOLTIP_MARK_HERO));
				else if (MAX_HERO <= nJob)
					strTemp2.Format("%d%s", nLevel, prj.GetText(TID_GAME_TOOLTIP_MARK_HERO));
				else
					strTemp2.Format("%d", nLevel);

				if (i == 0)
					strMember.Format("%s. %s", strTemp2, pPlayerData->szPlayer);
				else
					strMember.Format("%s. %s", strTemp2, pPlayerData->szPlayer);
			}
			//Member - Level, Name Draw
			g_WndMng.StringSize(strMember, 13);
			p2DRender->TextOut(rect.TopLeft().x + 5, rect.TopLeft().y + 5, strMember, dwColor);

			//Member - Gauge Draw
			rect.TopLeft().y += 18;
			rect.TopLeft().x += 2;
			rect.BottomRight().x -= 2;
			int nWidth = pObjMember ? pObjMember->GetHitPointPercent(rect.Width()) : 0;
			CRect rectTemp = rect;
			rectTemp.right = rectTemp.left + nWidth;
			if (rect.right < rectTemp.right)
				rectTemp.right = rect.right;
			m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal);
			m_pTheme->RenderGauge(p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal);
		}
	}
}

void CWndSecretRoomQuick::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	for (int i = 0; i < MAX_SECRETROOM_MEMBER; i++)
		m_pWndMemberStatic[i] = (CWndStatic*)GetDlgItem(m_StaticID[i]);

	m_texGauEmptyNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptySmall.bmp"), 0xffff00ff, TRUE);
	m_texGauFillNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauFillSmall.bmp"), 0xffff00ff, TRUE);

	SetActiveMember(m_MemberCount);
	SortMemberList();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 112 + 48);
	Move(point);
}


BOOL CWndSecretRoomQuick::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SECRETROOM_QUICK, 0, CPoint(0, 0), pWndParent);
}

void CWndSecretRoomQuick::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
	if (((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack || pTaskBar->m_nExecute != 0)
		return;

	for (int i = 0; i < m_MemberCount; i++)
	{
		lpWndCtrl = GetWndCtrl(m_StaticID[i]);
		rect = lpWndCtrl->rect;
		if (rect.PtInRect(point))
		{
			((CWndWorld*)g_WndMng.m_pWndWorld)->m_pSelectRenderObj = NULL;

			CGuildMember* pMember = NULL;
			if (g_pPlayer->GetGuild())
				pMember = g_pPlayer->GetGuild()->GetMember(m_vecGuildMemberId[i]);

			if (pMember)
			{
				CMover* pMover = pMember->GetMover();
				if (g_pPlayer != pMover)
				{
					if (IsValidObj(pMover))
					{
						g_WorldMng()->SetObjFocus(pMover);
						CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
						if (pWndWorld)
							pWndWorld->m_pRenderTargetObj = NULL;

						m_FocusMemberid = m_vecGuildMemberId[i];
					}
				}
				else
				{
					m_FocusMemberid = 0;
					g_WorldMng()->SetObjFocus(NULL);
				}
			}

			return;
		}
	}
}

BOOL CWndSecretRoomQuick::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == 10000)
	{
		if (message == WNM_CLICKED)
		{
			if (m_MemberCount == 1)
			{
				m_bMini = FALSE;
				return TRUE;
			}

			CRect rect = GetWindowRect(TRUE);
			if (m_nWndHeight != 0)
			{
				if (rect.bottom - rect.top == m_nWndHeight)
				{
					rect.bottom = rect.top + 80;
					for (int i = 1; i < m_MemberCount; i++)
					{
						CWndStatic* lpWndStatic;
						lpWndStatic = (CWndStatic*)GetDlgItem(m_StaticID[i]);
						lpWndStatic->EnableWindow(FALSE);
						lpWndStatic->SetVisible(FALSE);
					}

					m_bMini = TRUE;
				}
				else if (rect.bottom - rect.top == 80)
				{
					rect.bottom = rect.top + m_nWndHeight;
					for (int i = 1; i < m_MemberCount; i++)
					{
						CWndStatic* lpWndStatic;
						lpWndStatic = (CWndStatic*)GetDlgItem(m_StaticID[i]);
						lpWndStatic->EnableWindow(TRUE);
						lpWndStatic->SetVisible(TRUE);
					}

					m_bMini = FALSE;
				}

				SetWndRect(rect);
			}

			return TRUE;
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSecretRoomQuick::SetActiveMember(int MemberNum)
{
	int i, height;
	CRect rect;

	rect = GetWindowRect(TRUE);
	height = 40;

	for (i = 0; i < MemberNum; i++)
	{
		CWndStatic* lpWndStatic;
		lpWndStatic = (CWndStatic*)GetDlgItem(m_StaticID[i]);
		lpWndStatic->EnableWindow(TRUE);
		lpWndStatic->SetVisible(TRUE);
		height += 34;
	}
	for (i = MemberNum; i < MAX_SECRETROOM_MEMBER; i++)
	{
		CWndStatic* lpWndStatic;
		lpWndStatic = (CWndStatic*)GetDlgItem(m_StaticID[i]);
		lpWndStatic->EnableWindow(FALSE);
		lpWndStatic->SetVisible(FALSE);
	}
	rect.bottom = rect.top + height;

	SetWndRect(rect);
	rect = GetWindowRect(TRUE);
	m_nWndHeight = rect.bottom - rect.top;

	AdjustWndBase();
}

void CWndSecretRoomQuick::SetGuildMember(u_long uGuildMemberId)
{
	if (m_vecGuildMemberId.size() < MAX_SECRETROOM_MEMBER)
	{
		if (g_pPlayer->GetGuild())
		{
			CGuildMember* pMember = g_pPlayer->GetGuild()->GetMember(uGuildMemberId);
			if (pMember && pMember->m_nMemberLv != GUD_MASTER)
				m_vecGuildMemberId.push_back(uGuildMemberId);
		}
	}
}

void CWndSecretRoomQuick::SortMemberList()
{
	u_long memberList[MAX_SECRETROOM_MEMBER];
	memset(memberList, 0, sizeof(u_long) * MAX_SECRETROOM_MEMBER);

	int nCount = m_vecGuildMemberId.size();
	for (int i = 0; i < nCount - 1; i++)
		memberList[i] = m_vecGuildMemberId[i + 1];

	std::sort(memberList, memberList + nCount, prMemberLevelDesc);

	m_vecGuildMemberId.clear();
	if (g_pPlayer->GetGuild())
		m_vecGuildMemberId.push_back(g_pPlayer->GetGuild()->m_idMaster);

	for (int i = 0; i < nCount; i++)
	{
		if (g_pPlayer->GetGuild())
		{
			CGuildMember* pMember = g_pPlayer->GetGuild()->GetMember(memberList[i]);
			if (pMember && pMember->m_nMemberLv != GUD_MASTER)
				m_vecGuildMemberId.push_back(memberList[i]);
		}
	}
}

#endif //__SECRET_ROOM