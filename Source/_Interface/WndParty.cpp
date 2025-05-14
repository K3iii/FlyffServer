#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndParty.h"
#include "WndManager.h"
#include "Party.h"
#include "dpclient.h"
extern CDPClient	g_DPlay;

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#ifdef __FL_PARTY_FINDER
#include "MsgHdr.h"
#endif


CWndParty::CWndParty()
{
	m_WndPartyChangeName = NULL;
#if __VER >= 8 //__CSC_VER8_2
	m_pWndPartyQuick = NULL;
#endif //__CSC_VER8_2
}
CWndParty::~CWndParty()
{
#ifdef __FIX_WND_1109
	SAFE_DELETE(m_WndPartyChangeName);
#endif	// __FIX_WND_1109
	DeleteDeviceObjects();
}
void CWndParty::SerializeRegInfo(CAr& ar, DWORD& dwVersion)
{

	CWndNeuz::SerializeRegInfo(ar, dwVersion);
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	if (ar.IsLoading())
	{
		if (dwVersion == 0)
		{
		}
		else
			if (dwVersion == 1)
			{
				int nCurSel;
				ar >> nCurSel;
				lpTabCtrl->SetCurSel(nCurSel);
			}
	}
	else
	{
		dwVersion = 1;
		ar << lpTabCtrl->GetCurSel();
	}

}
void CWndParty::OnDraw(C2DRender* p2DRender)
{
	if (g_Party.GetSizeofMember() < 2)
	{
		m_pWndLeave->EnableWindow(FALSE);
		m_pWndTransfer->EnableWindow(FALSE);
		m_pBtnPartyQuick->EnableWindow(FALSE);
#ifdef __FL_PARTY_SORT
		m_pBtnUp->EnableWindow(FALSE);
		m_pBtnDown->EnableWindow(FALSE);
#endif
	}
	else
	{
		m_pWndLeave->EnableWindow(TRUE);
		m_pBtnPartyQuick->EnableWindow(TRUE);

		if (g_Party.IsLeader(g_pPlayer->m_idPlayer) == FALSE)
		{
			m_pWndTransfer->EnableWindow(FALSE);
#ifdef __FL_PARTY_SORT
			m_pBtnUp->EnableWindow(FALSE);
			m_pBtnDown->EnableWindow(FALSE);
#endif
		}
		else
		{
			m_pWndTransfer->EnableWindow(TRUE);
#ifdef __FL_PARTY_SORT
			m_pBtnUp->EnableWindow(TRUE);
			m_pBtnDown->EnableWindow(TRUE);
#endif
		}
	}

#ifdef __PARTY_DEBUG_0129
	if (g_Party.GetSizeofMember() < 2 && g_Party.m_nLevel > 1)
	{
		m_pWndLeave->EnableWindow(TRUE);
	}
#endif

	if (g_Party.GetLevel() >= 10 && g_Party.IsLeader(g_pPlayer->m_idPlayer))
		m_pWndChange->EnableWindow(TRUE);
	else
		m_pWndChange->EnableWindow(FALSE);


	CWndStatic* pWndStatic;
	CString strTemp;
	if (g_Party.m_nKindTroup == 0)
	{
		strTemp.Format("%s", prj.GetText(TID_GAME_PARTY1));
	}
	else
	{
		if (0 == strlen(g_Party.m_sParty))
			strTemp.Format("%s", prj.GetText(TID_GAME_PARTY2));
		else
			strTemp.Format("%s", g_Party.m_sParty);
	}
	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NAME);
	pWndStatic->SetTitle(strTemp);
	strTemp.Format("%d", g_Party.m_nLevel);
	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_LEVEL);
	pWndStatic->SetTitle(strTemp);

	if (g_Party.m_nLevel >= 10)
	{
		strTemp.Format("%.2f %%", (float)g_Party.m_nExp * 100 / (float)(((50 + g_Party.GetLevel()) * g_Party.GetLevel() / 13) * 10));
	}
	else
	{
		strTemp.Format("%.2f %%", (float)g_Party.m_nExp * 100 / (float)prj.m_aExpParty[g_Party.m_nLevel].Exp);
	}
	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_EXP);
	pWndStatic->SetTitle(strTemp);
	strTemp.Format("%d", g_Party.m_nPoint);
	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_POINT);
	pWndStatic->SetTitle(strTemp);

	CWndButton* pWndButton;
	switch (g_Party.m_nTroupsShareExp)
	{
	case 0:
	{
		pWndButton = (CWndButton*)GetDlgItem(WIDC_EXP_SHARE);
		pWndButton->SetCheck(TRUE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO6);
		pWndButton->SetCheck(FALSE);
	}
	break;
	case 1:
	{
		pWndButton = (CWndButton*)GetDlgItem(WIDC_EXP_SHARE);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO6);
		pWndButton->SetCheck(TRUE);
	}
	break;
	case 2:
	{
		pWndButton = (CWndButton*)GetDlgItem(WIDC_EXP_SHARE);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO6);
		pWndButton->SetCheck(FALSE);
	}
	break;
	}
	switch (g_Party.m_nTroupeShareItem)
	{
	case 0:
	{
		pWndButton = (CWndButton*)GetDlgItem(WIDC_ITEM_SHARE);
		pWndButton->SetCheck(TRUE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO2);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO3);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO4);
		pWndButton->SetCheck(FALSE);
	}
	break;
	case 1:
	{
		pWndButton = (CWndButton*)GetDlgItem(WIDC_ITEM_SHARE);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO2);
		pWndButton->SetCheck(TRUE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO3);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO4);
		pWndButton->SetCheck(FALSE);
	}
	break;
	case 2:
	{
		pWndButton = (CWndButton*)GetDlgItem(WIDC_ITEM_SHARE);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO2);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO3);
		pWndButton->SetCheck(TRUE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO4);
		pWndButton->SetCheck(FALSE);
	}
	break;
	case 3:
	{
		pWndButton = (CWndButton*)GetDlgItem(WIDC_ITEM_SHARE);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO2);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO3);
		pWndButton->SetCheck(FALSE);
		pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO4);
		pWndButton->SetCheck(TRUE);
	}
	break;
	}

#ifdef __FL_PARTY_FINDER
	pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndButton->SetCheck(g_Party.m_bAllowed);
#endif

}
void CWndParty::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pWndLeave = (CWndButton*)GetDlgItem(WIDC_LEAVE);
	m_pWndChange = (CWndButton*)GetDlgItem(WIDC_CHANGE);
	m_pWndTransfer = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pBtnPartyQuick = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
#ifdef __FL_PARTY_SORT
	m_pBtnUp = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	m_pBtnDown = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
#endif

	if (g_Party.GetSizeofMember() < 2)
	{
		m_pWndTransfer->EnableWindow(FALSE);
		m_pWndLeave->EnableWindow(FALSE);
		m_pBtnPartyQuick->EnableWindow(FALSE);
#ifdef __FL_PARTY_SORT
		m_pBtnUp->EnableWindow(FALSE);
		m_pBtnDown->EnableWindow(FALSE);
#endif
	}
	m_pWndChange->EnableWindow(FALSE);

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;


	m_wndPartyInfo.Create(WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_PARTY_INFO);
	m_wndPartySkill.Create(WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_PARTY_SKILL);

	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_INFOMATION);
	tabTabItem.pWndBase = &m_wndPartyInfo;
	pWndTabCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.pszText = prj.GetText(TID_APP_SKILL);
	tabTabItem.pWndBase = &m_wndPartySkill;
	pWndTabCtrl->InsertItem(1, &tabTabItem);

	MoveParentCenter();

}

BOOL CWndParty::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	BOOL bFlag = CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PARTY, 0, CPoint(0, 0), pWndParent);
	return bFlag;
}
BOOL CWndParty::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndParty::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndParty::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndParty::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndParty::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_LEAVE)
	{
		if (m_wndPartyInfo.m_nSelected != -1)
		{
			u_long  nLeaveMember = g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_uPlayerId;
			if (g_Party.m_nSizeofMember != 0)
			{
				SAFE_DELETE(g_WndMng.m_pWndPartyLeaveConfirm);
				g_WndMng.m_pWndPartyLeaveConfirm = new CWndPartyLeaveConfirm;
				g_WndMng.m_pWndPartyLeaveConfirm->Initialize(NULL, APP_PARTYLEAVE_CONFIRM);
				if (g_Party.m_aMember[0].m_uPlayerId == g_pPlayer->m_idPlayer)
					g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId(nLeaveMember);
				else
					g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId(g_pPlayer->m_idPlayer);
			}
		}
		else
		{
			SAFE_DELETE(g_WndMng.m_pWndPartyLeaveConfirm);
			g_WndMng.m_pWndPartyLeaveConfirm = new CWndPartyLeaveConfirm;
			g_WndMng.m_pWndPartyLeaveConfirm->Initialize(NULL, APP_PARTYLEAVE_CONFIRM);
			if (g_Party.m_nSizeofMember != 0)
				g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId(g_pPlayer->m_idPlayer);
		}
		m_wndPartyInfo.m_nSelected = -1;
	}
	else if (nID == WIDC_BUTTON1)
	{
		if (g_Party.m_nSizeofMember != 0)
		{
			if (g_Party.IsLeader(g_pPlayer->m_idPlayer) && m_wndPartyInfo.m_nSelected != -1)
			{
				if (g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_bRemove == FALSE)
				{
					u_long  nLeadMember = g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_uPlayerId;
					if (g_Party.IsLeader(nLeadMember) == FALSE)
						g_DPlay.SendPartyChangeLeader(nLeadMember);
					else
						g_WndMng.PutString(prj.GetText(TID_GAME_PARTY_ALREADYMASTER), NULL, prj.GetTextColor(TID_GAME_PARTY_ALREADYMASTER));
				}
				else
					g_WndMng.PutString(prj.GetText(TID_GAME_MASTER_AWAY), NULL, prj.GetTextColor(TID_GAME_MASTER_AWAY));
			}
			else
				g_WndMng.PutString(prj.GetText(TID_GAME_PARTY_ISNOTMASTER), NULL, prj.GetTextColor(TID_GAME_PARTY_ISNOTMASTER));
		}
	}
	else if (nID == WIDC_CHANGE)
	{
		if (g_Party.m_nKindTroup == 0)
		{
			SAFE_DELETE(m_WndPartyChangeName);
			m_WndPartyChangeName = new CWndPartyChangeName;
			m_WndPartyChangeName->Initialize(this);
		}
	}
	else if (nID == WIDC_BUTTON2)
	{
		if (g_Party.GetSizeofMember() >= 2)
		{
			m_pWndPartyQuick = (CWndPartyQuick*)g_WndMng.GetApplet(APP_PARTY_QUICK);
			if (m_pWndPartyQuick == NULL)
			{
				m_pWndPartyQuick = (CWndPartyQuick*)g_WndMng.CreateApplet(APP_PARTY_QUICK);
				m_pWndPartyQuick->SetActiveMember(g_Party.GetSizeofMember());
			}
			else
			{
				m_pWndPartyQuick->Destroy();
				m_pWndPartyQuick = NULL;
			}
		}
	}
#ifdef __FL_PARTY_SORT
	else if (nID == WIDC_BUTTON3 || nID == WIDC_BUTTON4)
	{
		if (g_Party.m_nSizeofMember != 0)
		{
			if (g_Party.IsLeader(g_pPlayer->m_idPlayer) && m_wndPartyInfo.m_nSelected != -1)
			{
				if (!g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_bRemove)
				{
					u_long uMember = g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_uPlayerId;
					if (!g_Party.IsLeader(uMember))
					{
						g_DPlay.SendMoveMemberParty(uMember, nID == WIDC_BUTTON3 ? TRUE : FALSE);
						m_wndPartyInfo.m_nSelected = -1;
					}
					else
						g_WndMng.PutString("You can't move yourself", NULL, prj.GetTextColor(TID_GAME_PARTY_ALREADYMASTER));
				}
				else
					g_WndMng.PutString("This player is out of the game, wait until he enters the game.", NULL, prj.GetTextColor(TID_GAME_MASTER_AWAY));
			}
			else
				g_WndMng.PutString("You should select member to move him in the party-list ", NULL, prj.GetTextColor(TID_GAME_PARTY_ISNOTMASTER));
		}
	}
#endif
	else if (g_Party.IsLeader(g_pPlayer->m_idPlayer))
	{
		if (g_Party.m_nKindTroup != 0)
		{
			int nTroupsShareExp = g_Party.m_nTroupsShareExp;

			if (nID == WIDC_EXP_SHARE) nTroupsShareExp = 0;
			else if (nID == WIDC_RADIO6) nTroupsShareExp = 1;

			if (nTroupsShareExp == 2)
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0004)));
			else
			{
				if (nTroupsShareExp != g_Party.m_nTroupsShareExp)
					g_DPlay.SendChangeShareExp(nTroupsShareExp);
			}
		}

		int nTroupeShareItem = g_Party.m_nTroupeShareItem;

		if (nID == WIDC_ITEM_SHARE) nTroupeShareItem = 0;
		else if (nID == WIDC_RADIO2) nTroupeShareItem = 1;
		else if (nID == WIDC_RADIO3) nTroupeShareItem = 2;
		else if (nID == WIDC_RADIO4) nTroupeShareItem = 3;

		if (nTroupeShareItem != g_Party.m_nTroupeShareItem)
			g_DPlay.SendChangeShareItem(nTroupeShareItem);

#ifdef __FL_PARTY_FINDER
		if (nID == WIDC_CHECK1)
			g_DPlay.SendHdr(PACKETTYPE_PARTYCHANGEJOINMODE);
#endif
	}
	else
	{
		if (nID != WTBID_CLOSE && nID != WTBID_HELP)
			g_WndMng.PutString(prj.GetText(TID_DIAG_0008), NULL, 0xFFFF0000);
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndParty::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_WndPartyChangeName)
		SAFE_DELETE(m_WndPartyChangeName);
}
#ifdef __FL_PARTY_SORT
void CWndParty::SelectTarget(int nPos)
{
	m_wndPartyInfo.m_nSelected = nPos;
}
#endif


CWndPartyInfo::CWndPartyInfo()
{
	m_nSelected = -1;
	m_pVBGauge = NULL;
}
CWndPartyInfo::~CWndPartyInfo()
{
	DeleteDeviceObjects();
}
void CWndPartyInfo::OnDraw(C2DRender* p2DRender)
{
	CString strTemp;


	if (m_nSelected != -1)
	{
		CRect rect(5, 8 + m_nSelected * 15, 410, 22 + m_nSelected * 15);
		p2DRender->RenderFillRect(rect, 0x60ffff00);
	}

	if (g_Party.GetSizeofMember() < m_nSelected + 1)
	{
		m_nSelected = -1;
	}
	int y = 10;

	DWORD dwColor;
	for (int i = 0; i < g_Party.m_nSizeofMember; i++)
	{
		dwColor = 0xff000000;
		CMover* pObjMember = prj.GetUserByID(g_Party.m_aMember[i].m_uPlayerId);

		CRect rectTemp, rect;
		CString strName;
		rectTemp = CRect(10, y + 1, 20, y + 13);
		p2DRender->RenderFillRect(rectTemp, dwColor);
		rectTemp = CRect(11, y + 2, 19, y + 12);


		DWORD colorStatus = 0xff0000ff;
		if (IsValidObj(pObjMember))
		{
			if (pObjMember->GetHitPoint() == 0)
				colorStatus = 0xffff0000;
			else
			{
				if (pObjMember->GetMaxHitPoint() > 0 && ((FLOAT)pObjMember->GetHitPoint()) / ((FLOAT)pObjMember->GetMaxHitPoint()) < .1f)
					colorStatus = 0xffffff00;
			}
		}
		else
		{
			colorStatus = 0xff878787;
			if (g_Party.m_aMember[i].m_bRemove)
				colorStatus = 0xff000000;
		}
		p2DRender->RenderFillRect(rectTemp, colorStatus);


		u_long  nLeadMember = g_Party.m_aMember[i].m_uPlayerId;
		if (g_Party.IsLeader(nLeadMember))
			dwColor = 0xff1fb72d;

		int nJob;
		int nLevel;
		CString strName2;
		if (IsValidObj(pObjMember))
		{
			nJob = pObjMember->GetJob();
			nLevel = pObjMember->GetLevel();
			strName = pObjMember->GetName();
		}
		else
		{
			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(g_Party.m_aMember[i].m_uPlayerId);
			nJob = pPlayerData->data.nJob;
			nLevel = pPlayerData->data.nLevel;
			strName = pPlayerData->szPlayer;
		}

		// Level
		if (MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER)
			strTemp.Format("%d%s", nLevel, prj.GetText(TID_GAME_TOOLTIP_MARK_MASTER));
		else if (MAX_MASTER <= nJob)
			strTemp.Format("%d%s", nLevel, prj.GetText(TID_GAME_TOOLTIP_MARK_HERO));
		else if (MAX_HERO <= nJob)
			strTemp.Format("%d%s", nLevel, prj.GetText(TID_GAME_TOOLTIP_MARK_HERO));
		else
			strTemp.Format("%d", nLevel);

		p2DRender->TextOut(30, y, 1.0f, 1.0f, strTemp, dwColor);

		// Master/Hero and Job Icon
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
		if (pWndWorld)
		{
			if (prj.m_aJob[nJob].dwJobType == JTYPE_PRO)
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(114, y), (19 + nJob - 6), 0xffffffff, 0.9f, 0.85f);
			else if (prj.m_aJob[nJob].dwJobType == JTYPE_MASTER)
			{
				int nMasterIndex = 27;
				if (nLevel < 70)
					nMasterIndex = 27;
				else if (nLevel >= 70 && nLevel < 80)
					nMasterIndex = 28;
				else if (nLevel >= 80 && nLevel < 90)
					nMasterIndex = 29;
				else if (nLevel >= 90 && nLevel < 100)
					nMasterIndex = 30;
				else if (nLevel >= 100 && nLevel < 110)
					nMasterIndex = 31;
				else if (nLevel >= 110 && nLevel <= 120)
					nMasterIndex = 32;

				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(95, y), nMasterIndex, 0xffffffff, 0.9f, 0.85f);
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(114, y), (19 + nJob - 16), 0xffffffff, 0.9f, 0.85f);
			}
			else if (prj.m_aJob[nJob].dwJobType == JTYPE_HERO)
			{
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(95, y), 33, 0xffffffff, 0.9f, 0.9f);
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(114, y), (19 + nJob - 24), 0xffffffff, 0.9f, 0.85f);
			}
			else if (prj.m_aJob[nJob].dwJobType == JTYPE_LEGEND_HERO)
			{
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(95, y), 33, 0xffffffff, 0.9f, 0.9f);
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(114, y), (19 + nJob - 32), 0xffffffff, 0.9f, 0.85f);
			}
			else
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(114, y), 14 + nJob, 0xffffffff, 0.9f, 0.85f);
		}

		g_WndMng.StringSize(strName, 10);
		strTemp.Format("%s", strName);
		p2DRender->TextOut(135, y, 1.0f, 1.0f, strTemp, dwColor);

		int nWidth = (IsValidObj(pObjMember) ? 110 * pObjMember->GetHitPoint() / pObjMember->GetMaxHitPoint() : 0);
		rect.SetRect(300, y, 300 + 110, y + 12);

		rectTemp = rect;
		rectTemp.right = rectTemp.left + nWidth;
		m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal);
		if (IsValidObj(pObjMember))
			m_pTheme->RenderGauge(p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal);

		y += 15;
	}
}
HRESULT CWndPartyInfo::RestoreDeviceObjects()
{
	CWndNeuz::RestoreDeviceObjects();
	if (m_pVBGauge == NULL)
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL);
	return S_OK;
}
HRESULT CWndPartyInfo::InvalidateDeviceObjects()
{
	CWndNeuz::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);
	return S_OK;
}
HRESULT CWndPartyInfo::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();
	InvalidateDeviceObjects();
	return S_OK;
}

void CWndPartyInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);
	m_texGauFillNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndPartyInfo::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PARTY_INFO, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndPartyInfo::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPartyInfo::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPartyInfo::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPartyInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
	if (((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack || pTaskBar->m_nExecute != 0)
		return;

	CRect rect(10, 10, 365, 10 + g_Party.GetSizeofMember() * 15);
	if (rect.PtInRect(point))
		m_nSelected = (point.y - 10) / 15;
	else
		m_nSelected = -1;
	if (m_nSelected != -1)
	{

		((CWndWorld*)g_WndMng.m_pWndWorld)->m_pSelectRenderObj = NULL;
		CMover* pObjMember = prj.GetUserByID(g_Party.m_aMember[m_nSelected].m_uPlayerId);
		if (g_pPlayer != pObjMember)
		{
			if (IsValidObj(pObjMember))
			{
				g_WorldMng()->SetObjFocus(pObjMember);
				CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
				if (pWndWorld)
					pWndWorld->m_pRenderTargetObj = NULL;
			}
		}
		else
			g_WorldMng()->SetObjFocus(NULL);
	}
	else
		g_WorldMng()->SetObjFocus(NULL);
}
BOOL CWndPartyInfo::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



CWndPartySkill::CWndPartySkill()
{
	ZeroMemory(m_atexSkill, sizeof(m_atexSkill));
	m_nSkillSelect = -1;
}
CWndPartySkill::~CWndPartySkill()
{
}
void CWndPartySkill::OnDraw(C2DRender* p2DRender)
{
	//	if( g_Party.m_nKindTroup )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 3;
		int nHeight = rect.Height() / 3;
		int nCount = 0;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++, nCount++)
			{
				ItemProp* pItemProp = prj.GetPartySkill(nCount + 1);
				if (pItemProp) //&& g_Party.m_nSizeofMember >= 2 )
				{
					m_atexSkill[i * 3 + j] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pItemProp->szIcon/*pItemBase->GetProp()->szIcon*/), 0xffff00ff, FALSE);
					CPoint point1 = CPoint(j * nWidth + 35, i * nHeight + 6);
					CPoint point2 = CPoint(j * nWidth + 3, i * nHeight + 3);

					if (g_Party.m_nKindTroup && int(g_Party.GetLevel() - pItemProp->dwReqDisLV) >= 0)
					{
						if ((g_Party.GetPoint() - pItemProp->dwExp) >= 0)
						{

							p2DRender->TextOut(point1.x, point1.y, pItemProp->szName, 0xff000000);
							//m_aSlotQueue[nIndex].m_pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, lpSkill->GetProp()->szIcon/*pItemBase->GetProp()->szIcon*/), 0xffff00ff, FALSE );
							p2DRender->RenderTexture(point2, m_atexSkill[i * 3 + j]);

						}
						else
						{

							p2DRender->TextOut(point1.x, point1.y, pItemProp->szName, 0xff0000ff);
							p2DRender->RenderTexture(point2, m_atexSkill[i * 3 + j]);

						}
					}
					else
					{

						p2DRender->TextOut(point1.x, point1.y, pItemProp->szName, 0xffff0000);
						p2DRender->RenderTexture(point2, m_atexSkill[i * 3 + j], 54);
					}
				}
			}
		}
	}
}
void CWndPartySkill::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 3;
	int nHeight = rect.Height() / 3;
	point.x /= nWidth;
	point.y /= nHeight;
	int nSkill = point.y * 3 + point.x;

	ItemProp* pItemProp = prj.GetPartySkill(nSkill + 1);
	if (pItemProp)
	{
		m_nSkillSelect = nSkill;
		return;
	}
	m_nSkillSelect = -1;
}
void CWndPartySkill::OnMouseWndSurface(CPoint point)
{
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 3;
	int nHeight = rect.Height() / 3;
	int nCount = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++, nCount++)
		{
			ItemProp* pItemProp = prj.GetPartySkill(nCount + 1);
			if (pItemProp) //&& g_Party.m_nSizeofMember >= 2 )
			{
				CRect rect(j * nWidth + 3, i * nHeight + 3, j * nWidth + 3 + nWidth, i * nHeight + 3 + nHeight);
				if (rect.PtInRect(point))
				{
					CPoint point2 = point;
					ClientToScreen(&point2);
					ClientToScreen(&rect);

					g_WndMng.PutToolTip_Troupe(i * 3 + j + 1, point2, &rect);
				}
			}
		}
	}
}
void CWndPartySkill::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_nSkillSelect = -1;
}

void CWndPartySkill::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nSkillSelect == -1 || IsPush() == FALSE)
		return;

	CPoint pt(3, 3);
	CRect rect;
	ItemProp* pItemProp = prj.GetPartySkill(m_nSkillSelect + 1);
	if (pItemProp)
	{
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_SKILL;
		m_GlobalShortcut.m_dwType = 2;
		m_GlobalShortcut.m_dwIndex = m_nSkillSelect + 1;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId = m_nSkillSelect + 1;
		m_GlobalShortcut.m_pTexture = m_atexSkill[m_nSkillSelect];
		_tcscpy(m_GlobalShortcut.m_szString, pItemProp->szName);
	}
}

void CWndPartySkill::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndPartySkill::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PARTY_SKILL, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPartySkill::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPartySkill::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPartySkill::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//	if( g_Party.m_nSizeofMember >= 2 && g_Party.m_nKindTroup == 1 )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 3;
		int nHeight = rect.Height() / 3;
		point.x /= nWidth;
		point.y /= nHeight;
		int nSkill = point.y * 3 + point.x;

		ItemProp* pItemProp = prj.GetPartySkill(nSkill + 1);
		if (pItemProp)
		{
			g_WndMng.ObjectExecutor(SHORTCUT_SKILL, nSkill + 1, 2);
		}
	}
}
BOOL CWndPartySkill::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
