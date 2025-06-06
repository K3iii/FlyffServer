#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DPClient.h"
#include "party.h"
#include "guild.h"
#include "WndMessengerCtrl.h"
#include "WndMessenger.h"
#include "WndManager.h"
#if __VER >= 13 // __HOUSING
#include "Housing.h"
#endif // __HOUSING
#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

extern CDPClient	g_DPlay;
extern CParty		g_Party;

#if __VER >= 11 // __CSC_VER11_4
////////////////////////////////////////////////////////////////////
// Common Local Func.
////////////////////////////////////////////////////////////////////
bool prLevelAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[player1.m_nJob].dwJobType;
	nPlayer2JobType = prj.m_aJob[player2.m_nJob].dwJobType;

	if (nPlayer1JobType > nPlayer2JobType)
		rtn_val = true;
	else if (nPlayer1JobType == nPlayer2JobType)
	{
		if (player1.m_nLevel > player2.m_nLevel)
			rtn_val = true;
	}

	return rtn_val;
}

bool prLevelDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[player1.m_nJob].dwJobType;
	nPlayer2JobType = prj.m_aJob[player2.m_nJob].dwJobType;

	if (nPlayer1JobType < nPlayer2JobType)
		rtn_val = true;
	else if (nPlayer1JobType == nPlayer2JobType)
	{
		if (player1.m_nLevel < player2.m_nLevel)
			rtn_val = true;
	}

	return rtn_val;
}

bool prJobAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;

	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[player1.m_nJob].dwJobType;
	nPlayer2JobType = prj.m_aJob[player2.m_nJob].dwJobType;

	if (nPlayer1JobType > nPlayer2JobType)
		rtn_val = true;
	else if (nPlayer1JobType == nPlayer2JobType)
	{
		if (player1.m_nJob > player2.m_nJob)
			rtn_val = true;
	}

	return rtn_val;
}

bool prJobDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[player1.m_nJob].dwJobType;
	nPlayer2JobType = prj.m_aJob[player2.m_nJob].dwJobType;

	if (nPlayer1JobType < nPlayer2JobType)
		rtn_val = true;
	else if (nPlayer1JobType == nPlayer2JobType)
	{
		if (player1.m_nJob < player2.m_nJob)
			rtn_val = true;
	}

	return rtn_val;
}

bool prStatusAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1Status, nPlayer2Status;

	nPlayer1Status = player1.m_dwStatus;
	nPlayer2Status = player2.m_dwStatus;


	if (nPlayer1Status == FRS_OFFLINE)
		nPlayer1Status = 12;
	if (nPlayer2Status == FRS_OFFLINE)
		nPlayer2Status = 12;

	if (nPlayer1Status < nPlayer2Status)
		rtn_val = true;

	return rtn_val;
}

bool prStatusDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	int nPlayer1Status, nPlayer2Status;

	nPlayer1Status = player1.m_dwStatus;
	nPlayer2Status = player2.m_dwStatus;


	if (nPlayer1Status == FRS_OFFLINE)
		nPlayer1Status = 12;
	if (nPlayer2Status == FRS_OFFLINE)
		nPlayer2Status = 12;

	if (nPlayer1Status > nPlayer2Status)
		rtn_val = true;

	return rtn_val;

	return rtn_val;
}

bool prChannelAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;

	if (player1.m_nChannel > player2.m_nChannel)
		rtn_val = true;

	return rtn_val;
}

bool prChannelDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;

	if (player1.m_nChannel < player2.m_nChannel)
		rtn_val = true;

	return rtn_val;
}

bool prNameAsce(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.m_szName);
	strplayer2Name.Format("%s", player2.m_szName);

	if (strplayer1Name > strplayer2Name)
		rtn_val = true;

	return rtn_val;
}

bool prNameDesc(__MESSENGER_PLAYER player1, __MESSENGER_PLAYER player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.m_szName);
	strplayer2Name.Format("%s", player2.m_szName);

	if (strplayer1Name < strplayer2Name)
		rtn_val = true;

	return rtn_val;
}

#if __VER >= 15
//-----------------------------------------------------------------------------
__MESSENGER_PLAYER::__MESSENGER_PLAYER(void) :
	m_nChannel(0),
	m_dwStatus(0),
	m_nLevel(0),
	m_nJob(0),
	m_dwPlayerId(0),
	m_bBlock(FALSE),
	m_bVisitAllowed(FALSE)
{
	ZeroMemory(m_szName, sizeof(m_szName));
}
//-----------------------------------------------------------------------------
void __MESSENGER_PLAYER::Initialize(void)
{
	m_nChannel = 0;
	m_dwStatus = 0;
	m_nLevel = 0;
	m_nJob = 0;
	m_dwPlayerId = 0;
	m_bBlock = FALSE;
	m_bVisitAllowed = FALSE;
	ZeroMemory(m_szName, sizeof(m_szName));
}
//-----------------------------------------------------------------------------
#endif

CWndFriendCtrlEx::CWndFriendCtrlEx()
{
	m_nCurSelect = -1;
	m_nFontHeight = 20;
	m_nDrawCount = 0;
	m_bSortbyChannel = TRUE;
	m_bSortbyStatus = FALSE;
	m_bSortbyLevel = TRUE;
	m_bSortbyJob = TRUE;
	m_bSortbyName = TRUE;
	m_nCurSort = SORT_BY_STATUS;

	m_vPlayerList.clear();
}

CWndFriendCtrlEx::~CWndFriendCtrlEx()
{
}

void CWndFriendCtrlEx::Create(RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndBase::Create(WBS_CHILD, rect, pParentWnd, nID);
}

void CWndFriendCtrlEx::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);

	m_menu.CreateMenu(this);

	UpdatePlayerList();
}

void CWndFriendCtrlEx::UpdatePlayerList()
{
	// Set Friends List
	m_vPlayerList.clear();
	__MESSENGER_PLAYER stPlayer;

	vector<DWORD> vecTemp;
	CHousing::GetInstance()->GetVisitAllow(vecTemp);


	for (map<u_long, Friend>::iterator i = g_WndMng.m_RTMessenger.begin(); i != g_WndMng.m_RTMessenger.end(); ++i)
	{
		u_long idPlayer = i->first;
		Friend* pFriend = &i->second;
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPlayer);
		int nJob = pPlayerData->data.nJob;
		int nLevel = pPlayerData->data.nLevel;
		u_long uLogin = pPlayerData->data.uLogin;
		LPCSTR lpszPlayer = pPlayerData->szPlayer;
		DWORD dwState = pFriend->dwState;

		stPlayer.m_dwPlayerId = idPlayer;
		stPlayer.m_dwStatus = dwState;
		stPlayer.m_nJob = nJob;
		stPlayer.m_nLevel = nLevel;
		stPlayer.m_bBlock = pFriend->bBlock;
		stPlayer.m_bVisitAllowed = FALSE;
		vector<DWORD>::iterator iterV = vecTemp.begin();
		for (; iterV != vecTemp.end(); ++iterV)
		{
			if (idPlayer == *iterV)
				stPlayer.m_bVisitAllowed = TRUE;
		}
		if (stPlayer.m_dwStatus == FRS_OFFLINE)
			stPlayer.m_nChannel = 100;
		else
			stPlayer.m_nChannel = uLogin;

		lstrcpy(stPlayer.m_szName, lpszPlayer);

		m_vPlayerList.push_back(stPlayer);
	}

	SetScrollBar();

	switch (m_nCurSort)
	{
	case SORT_BY_CHANNEL:
		SortbyChannel(FALSE);
		break;
	case SORT_BY_STATUS:
		SortbyStatus(FALSE);
		break;
	case SORT_BY_LEVEL:
		SortbyLevel(FALSE);
		break;
	case SORT_BY_JOB:
		SortbyJob(FALSE);
		break;
	case SORT_BY_NAME:
		SortbyName(FALSE);
		break;
	}
}

void CWndFriendCtrlEx::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	DWORD dwColor1 = D3DCOLOR_ARGB(100, 0, 0, 0);
	DWORD dwColor2 = D3DCOLOR_ARGB(255, 240, 240, 240);
	DWORD dwColor3 = D3DCOLOR_ARGB(100, 200, 200, 200);

	p2DRender->RenderFillRect(rect, dwColor1);
	p2DRender->RenderRoundRect(rect, dwColor2);
	rect.DeflateRect(1, 1);
	p2DRender->RenderRect(rect, dwColor2);
	rect.DeflateRect(1, 1);
	p2DRender->RenderRect(rect, dwColor3);
}

void CWndFriendCtrlEx::OnDraw(C2DRender* p2DRender)
{
	CPoint pt(3, 3);
	m_nDrawCount = 0;

	if (NULL == g_pPlayer)
		return;

	int nMax = GetDrawCount();
	m_nDrawCount = m_wndScrollBar.GetScrollPos();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[6 * 4 * nMax];
	TEXTUREVERTEX2* pVertices = pVertex;

	vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();

	for (int i = 0; iter < m_vPlayerList.end(); i++, iter++)
	{
		CString strFormat;

		if (i < m_nDrawCount)
			continue;
		if (i >= nMax)
		{
			iter = m_vPlayerList.end();
			continue;
		}

		__MESSENGER_PLAYER stPlayer = *(iter);

		// Draw Channel Icon
		if (stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11)
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(20, pt.y), 34 + stPlayer.m_nChannel - 1, &pVertices, 0xffffffff);

		// Draw Status Icon
		DWORD dwMyState;
		if (stPlayer.m_dwStatus == FRS_ONLINE)
			dwMyState = 2;
		else if (stPlayer.m_dwStatus == FRS_OFFLINE)
			dwMyState = 8;
		else
			dwMyState = stPlayer.m_dwStatus;

		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(76, pt.y), 7 + (dwMyState - 2), &pVertices, 0xffffffff);

		// Draw Level
		strFormat.Format("%d", stPlayer.m_nLevel);

		static const int LEVEL_TEXT_X = 127;
		p2DRender->TextOut(LEVEL_TEXT_X, pt.y + 3, strFormat, 0xff000000);

		// Draw Job Icon
		static const int JOB_TYPE_ICON_X = 174;
		static const int JOB_ICON_X = JOB_TYPE_ICON_X + 20;
		if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_PRO)
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 6), &pVertices, 0xffffffff);
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_MASTER)
		{
			int nMasterIndex = 27;
			if (stPlayer.m_nLevel < 70)
				nMasterIndex = 27;
			else if (stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80)
				nMasterIndex = 28;
			else if (stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90)
				nMasterIndex = 29;
			else if (stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100)
				nMasterIndex = 30;
			else if (stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110)
				nMasterIndex = 31;
			else if (stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120)
				nMasterIndex = 32;

			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_TYPE_ICON_X, pt.y), nMasterIndex, &pVertices, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 16), &pVertices, 0xffffffff);
		}
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_HERO)
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_TYPE_ICON_X, pt.y), 33, &pVertices, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 24), &pVertices, 0xffffffff);
		}
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_LEGEND_HERO)
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_TYPE_ICON_X, pt.y), 33, &pVertices, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 32), &pVertices, 0xffffffff);
		}
		else
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), 14 + stPlayer.m_nJob, &pVertices, 0xffffffff);

		// Draw Name
		DWORD dwColor = 0xff000000;
		if (i == m_nCurSelect)
			dwColor = 0xff6060ff;

		if (stPlayer.m_bBlock)
			dwColor = 0xffff0000;

		strFormat.Format("%s", stPlayer.m_szName);
		g_WndMng.StringSize(strFormat, 10);

		const int NAME_TEXT_X = 226;
		const int NAME_TEXT_Y = pt.y + 3;
		if (stPlayer.m_bVisitAllowed)
			p2DRender->TextOut(NAME_TEXT_X, NAME_TEXT_Y, strFormat, 0xff078a0d);
		else
			p2DRender->TextOut(NAME_TEXT_X, NAME_TEXT_Y, strFormat, dwColor);

		pt.y += m_nFontHeight;
	}

	pWndWorld->m_texPlayerDataIcon.Render(m_pApp->m_pd3dDevice, pVertex, ((int)pVertices - (int)pVertex) / sizeof(TEXTUREVERTEX2));
	SAFE_DELETE_ARRAY(pVertex);
}

void CWndFriendCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt(188, 3);
	CRect rect;

	int nCount = m_vPlayerList.size();
	if (nCount > 10)
		nCount = 10;

	for (int j = 0; j < nCount; j++)
	{
		rect.SetRect(pt.x, pt.y, pt.x + 24, pt.y + m_nFontHeight);
		if (rect.PtInRect(point))
		{
			vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			__MESSENGER_PLAYER stPlayer = *(iter);
			ClientToScreen(&point);
			ClientToScreen(&rect);
			g_toolTip.PutToolTip(100, prj.m_aJob[stPlayer.m_nJob].szName, rect, point, 3);
			j = m_vPlayerList.size();
		}
		pt.y += m_nFontHeight;
	}
}

void CWndFriendCtrlEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	CPoint pt(3, 3);
	CRect rect;

	for (int j = 0; j < (int)(m_vPlayerList.size()); j++)
	{
		rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight);
		if (rect.PtInRect(point))
		{
			int nPos = m_wndScrollBar.GetScrollPos();
			m_nCurSelect = j + nPos;
			return;
		}
		pt.y += m_nFontHeight;
	}
}

void CWndFriendCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	u_long idPlayer = NULL_ID;
	Friend* pFriend = NULL;
	int nSelect = GetSelect(point, idPlayer, &pFriend);
	if (nSelect != -1 && pFriend)
	{
		DWORD dwState = pFriend->dwState;
		if (dwState != FRS_OFFLINE && !pFriend->bBlock)
		{
			m_nCurSelect = nSelect;
			CWndMessage* pWndMessage = g_WndMng.OpenMessage(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
		}
		else
		{
			CString szMessage;
			if (dwState == FRS_OFFLINE)
				szMessage = prj.GetText(TID_GAME_NOTLOGIN);
			else
				szMessage.Format(prj.GetText(TID_GAME_MSGBLOCKCHR), CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
			g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN));
		}
	}
}

void	CWndFriendCtrlEx::GetSelectFriend(int SelectCount, u_long& idPlayer, Friend** ppFriend)
{
	*ppFriend = NULL;
	vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	iter += SelectCount;
	__MESSENGER_PLAYER stPlayer = *(iter);
	idPlayer = stPlayer.m_dwPlayerId;
	*ppFriend = g_WndMng.m_RTMessenger.GetFriend(stPlayer.m_dwPlayerId);
}

u_long CWndFriendCtrlEx::GetSelectId(int SelectCount)
{
	vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	iter += SelectCount;
	__MESSENGER_PLAYER stPlayer = *(iter);

	return stPlayer.m_dwPlayerId;
}

int	CWndFriendCtrlEx::GetSelect(CPoint point, u_long& idPlayer, Friend** ppFriend)
{
	CPoint pt(3, 3);
	CRect rect;
	int rtn_val = -1;

	for (int j = 0; j < (int)(m_vPlayerList.size()); j++)
	{
		rtn_val++;
		rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight);
		if (rect.PtInRect(point))
		{
			vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			rtn_val += nPos;
			__MESSENGER_PLAYER stPlayer = *(iter);
			idPlayer = stPlayer.m_dwPlayerId;
			*ppFriend = g_WndMng.m_RTMessenger.GetFriend(stPlayer.m_dwPlayerId);
			j = m_vPlayerList.size();
		}
		pt.y += m_nFontHeight;
	}

	return rtn_val;
}

BOOL CWndFriendCtrlEx::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	m_menu.SetVisible(FALSE);

	u_long idPlayer;

	switch (nID)
	{
	case 0:
	{
		Friend* pFriend = NULL;
		GetSelectFriend(m_nCurSelect, idPlayer, &pFriend);
		if (pFriend)
			CWndMessage* pWndMessage = g_WndMng.OpenMessage(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
	}
	break;
	case 1:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer != -1)
			g_DPlay.SendFriendInterceptState(uidPlayer);
	}
	break;
	case 2:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer != -1)
			g_DPlay.SendRemoveFriend(uidPlayer);
	}
	break;
	case 3:
	{
		Friend* pFriend = NULL;
		GetSelectFriend(m_nCurSelect, idPlayer, &pFriend);
		if (pFriend)
		{
			CString string;
			string.Format("/teleport \"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
			ParsingCommand(string.LockBuffer(), g_pPlayer);
			string.UnlockBuffer();
		}
	}
	break;
	case 4:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer != -1)
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet(APP_WORLD);
			if (pWndWorld)
				pWndWorld->InviteParty(uidPlayer);
		}
	}
	break;
	case 7:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer > 0)
			g_DPlay.SendHousingReqSetVisitAllow(uidPlayer, FALSE);
	}
	break;
	case 8:
	{
		u_long uidPlayer = GetSelectId(m_nCurSelect);
		if (uidPlayer > 0)
			g_DPlay.SendHousingReqSetVisitAllow(uidPlayer, TRUE);
	}
	break;
	}
	return -1;
}

void CWndFriendCtrlEx::OnRButtonUp(UINT nFlags, CPoint point)
{
	u_long idPlayer;
	Friend* pFriend = NULL;
	int nSelect = GetSelect(point, idPlayer, &pFriend);

	if (nSelect != -1 && pFriend != NULL)
	{
		DWORD dwState = pFriend->dwState;
		m_nCurSelect = nSelect;
		ClientToScreen(&point);
		m_menu.DeleteAllMenu();
		if (dwState != FRS_OFFLINE && !pFriend->bBlock)
			m_menu.AppendMenu(0, 0, _T(prj.GetText(TID_APP_MESSAGE)));
		if (pFriend->bBlock)
			m_menu.AppendMenu(0, 1, _T(prj.GetText(TID_FRS_BLOCKRESTORE)));
		else
			m_menu.AppendMenu(0, 1, _T(prj.GetText(TID_FRS_BLOCK)));
		m_menu.AppendMenu(0, 2, _T(prj.GetText(TID_FRS_DELETE)));
		if (dwState != FRS_OFFLINE && !pFriend->bBlock)
		{
			if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
				m_menu.AppendMenu(0, 3, _T(prj.GetText(TID_FRS_MOVE2)));
			if (!pFriend->bBlock)
			{
				if (g_Party.IsMember(idPlayer) == FALSE)
					m_menu.AppendMenu(0, 4, _T(prj.GetText(TID_MMI_INVITE_PARTY)));
			}
		}

		vector<__MESSENGER_PLAYER>::iterator iter = m_vPlayerList.begin();
		int		nCount = 0;
		BOOL	bIsAllowed = FALSE;
		for (; iter != m_vPlayerList.end(); ++iter)
		{
			if (idPlayer == iter->m_dwPlayerId)
			{
				if (iter->m_bVisitAllowed)
					bIsAllowed = TRUE;
			}
		}
		if (bIsAllowed)
			m_menu.AppendMenu(0, 7, _T(prj.GetText(TID_GAME_PROHIBIT_VISIT)));
		else
			m_menu.AppendMenu(0, 8, _T(prj.GetText(TID_GAME_ALLOW_VISIT)));

		m_menu.Move(point);
		m_menu.SetVisible(TRUE);
		m_menu.SetFocus();
	}
}

void CWndFriendCtrlEx::SetScrollBar()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = m_vPlayerList.size();
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
}

void CWndFriendCtrlEx::ScrollBarPos(int nPos)
{
	m_wndScrollBar.SetScrollPos(nPos);
}

int CWndFriendCtrlEx::GetDrawCount(void)
{
	int nMax = g_WndMng.m_RTMessenger.size();
	if (nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage())
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if (nMax < m_wndScrollBar.GetScrollPos())
		nMax = 0;

	return nMax;
}

void CWndFriendCtrlEx::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect(rect);

	CWndBase::OnSize(nType, cx, cy);
}
void CWndFriendCtrlEx::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(3, 3);

	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}

void CWndFriendCtrlEx::SortbyChannel(BOOL bCheckbefore)
{
	if (!bCheckbefore)
		m_bSortbyChannel = !m_bSortbyChannel;

	if (m_bSortbyChannel)
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prChannelAsce);
		m_bSortbyChannel = FALSE;
	}
	else
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prChannelDesc);
		m_bSortbyChannel = TRUE;
	}
	m_nCurSort = SORT_BY_CHANNEL;
}

void CWndFriendCtrlEx::SortbyStatus(BOOL bCheckbefore)
{
	if (!bCheckbefore)
		m_bSortbyStatus = !m_bSortbyStatus;

	if (m_bSortbyStatus)
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prStatusAsce);
		m_bSortbyStatus = FALSE;
	}
	else
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prStatusDesc);
		m_bSortbyStatus = TRUE;
	}
	m_nCurSort = SORT_BY_STATUS;
}

void CWndFriendCtrlEx::SortbyLevel(BOOL bCheckbefore)
{
	if (!bCheckbefore)
		m_bSortbyLevel = !m_bSortbyLevel;

	if (m_bSortbyLevel)
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prLevelAsce);
		m_bSortbyLevel = FALSE;
	}
	else
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prLevelDesc);
		m_bSortbyLevel = TRUE;
	}
	m_nCurSort = SORT_BY_LEVEL;
}

void CWndFriendCtrlEx::SortbyJob(BOOL bCheckbefore)
{
	if (!bCheckbefore)
		m_bSortbyJob = !m_bSortbyJob;

	if (m_bSortbyJob)
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prJobAsce);
		m_bSortbyJob = FALSE;
	}
	else
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prJobDesc);
		m_bSortbyJob = TRUE;
	}
	m_nCurSort = SORT_BY_JOB;
}

void CWndFriendCtrlEx::SortbyName(BOOL bCheckbefore)
{
	if (!bCheckbefore)
		m_bSortbyName = !m_bSortbyName;

	if (m_bSortbyName)
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prNameAsce);
		m_bSortbyName = FALSE;
	}
	else
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prNameDesc);
		m_bSortbyName = TRUE;
	}
	m_nCurSort = SORT_BY_NAME;
}

////////////////////////////////////////////////////////////////////
// Messenger Guild Tab Ctrl
////////////////////////////////////////////////////////////////////

CWndGuildCtrlEx::CWndGuildCtrlEx()
{
	m_nCurSelect = -1;
	m_nFontHeight = 20;
	m_nDrawCount = 0;
	m_bSortbyChannel = TRUE;
	m_bSortbyStatus = FALSE;
	m_bSortbyLevel = TRUE;
	m_bSortbyJob = TRUE;
	m_bSortbyName = TRUE;
	m_nCurSort = SORT_BY_STATUS;

	m_vPlayerList.clear();
}

CWndGuildCtrlEx::~CWndGuildCtrlEx()
{
}

void CWndGuildCtrlEx::Create(RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndBase::Create(WBS_CHILD, rect, pParentWnd, nID);
}

void CWndGuildCtrlEx::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);

	UpdatePlayerList();
}

void CWndGuildCtrlEx::UpdatePlayerList()
{
	if (!g_pPlayer)
		return;

	// Set GuildMemeber List
	m_vPlayerList.clear();
	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild)
	{
		map<u_long, CGuildMember*>::iterator iter = pGuild->m_mapPMember.begin();

		for (; iter != pGuild->m_mapPMember.end(); ++iter)
		{
			__MESSENGER_PLAYER stPlayer;
			CGuildMember* pGuildMember = (CGuildMember*)iter->second;

			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pGuildMember->m_idPlayer);
			stPlayer.m_nJob = pPlayerData->data.nJob;
			stPlayer.m_nLevel = pPlayerData->data.nLevel;
			stPlayer.m_dwPlayerId = pGuildMember->m_idPlayer;
			stPlayer.m_nChannel = pPlayerData->data.uLogin;
			if (pPlayerData->data.uLogin > 0)
				stPlayer.m_dwStatus = FRS_ONLINE;
			else
				stPlayer.m_dwStatus = FRS_OFFLINE;

			lstrcpy(stPlayer.m_szName, pPlayerData->szPlayer);

			m_vPlayerList.push_back(stPlayer);
		}
	}

	SetScrollBar();

	switch (m_nCurSort)
	{
	case SORT_BY_CHANNEL:
		SortbyChannel(FALSE);
		break;
	case SORT_BY_STATUS:
		SortbyStatus(FALSE);
		break;
	case SORT_BY_LEVEL:
		SortbyLevel(FALSE);
		break;
	case SORT_BY_JOB:
		SortbyJob(FALSE);
		break;
	case SORT_BY_NAME:
		SortbyName(FALSE);
		break;
	}
}

void CWndGuildCtrlEx::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	DWORD dwColor1 = D3DCOLOR_ARGB(100, 0, 0, 0);
	DWORD dwColor2 = D3DCOLOR_ARGB(255, 240, 240, 240);
	DWORD dwColor3 = D3DCOLOR_ARGB(100, 200, 200, 200);

	p2DRender->RenderFillRect(rect, dwColor1);
	p2DRender->RenderRoundRect(rect, dwColor2);
	rect.DeflateRect(1, 1);
	p2DRender->RenderRect(rect, dwColor2);
	rect.DeflateRect(1, 1);
	p2DRender->RenderRect(rect, dwColor3);
}

void CWndGuildCtrlEx::OnDraw(C2DRender* p2DRender)
{
	CPoint pt(3, 3);
	m_nDrawCount = 0;

	if (NULL == g_pPlayer)
		return;

	int nMax = GetDrawCount();
	m_nDrawCount = m_wndScrollBar.GetScrollPos();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[6 * 4 * nMax];
	TEXTUREVERTEX2* pVertices = pVertex;

	vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();

	for (int i = 0; iter < m_vPlayerList.end(); i++, iter++)
	{
		CString strFormat;

		if (i < m_nDrawCount)
			continue;
		if (i >= nMax)
		{
			iter = m_vPlayerList.end();
			continue;
		}

		__MESSENGER_PLAYER stPlayer = *(iter);

		// Draw Channel Icon
		if (stPlayer.m_nChannel != 100 && stPlayer.m_nChannel > 0 && stPlayer.m_nChannel < 11)
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(20, pt.y), 34 + stPlayer.m_nChannel - 1, &pVertices, 0xffffffff);

		// Draw Status Icon
		DWORD dwMyState;
		if (stPlayer.m_dwStatus == FRS_ONLINE)
			dwMyState = 2;
		else if (stPlayer.m_dwStatus == FRS_OFFLINE)
			dwMyState = 8;
		else
			dwMyState = stPlayer.m_dwStatus;

		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(76, pt.y), 7 + (dwMyState - 2), &pVertices, 0xffffffff);

		// Draw Level
		strFormat.Format("%d", stPlayer.m_nLevel);
		p2DRender->TextOut(127, pt.y + 3, strFormat, 0xff000000);

		// Draw Job Icon
		static const int JOB_TYPE_ICON_X = 174;
		static const int JOB_ICON_X = JOB_TYPE_ICON_X + 20;
		if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_PRO)
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 6), &pVertices, 0xffffffff);
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_MASTER)
		{
			int nMasterIndex = 27;
			if (stPlayer.m_nLevel < 70)
				nMasterIndex = 27;
			else if (stPlayer.m_nLevel >= 70 && stPlayer.m_nLevel < 80)
				nMasterIndex = 28;
			else if (stPlayer.m_nLevel >= 80 && stPlayer.m_nLevel < 90)
				nMasterIndex = 29;
			else if (stPlayer.m_nLevel >= 90 && stPlayer.m_nLevel < 100)
				nMasterIndex = 30;
			else if (stPlayer.m_nLevel >= 100 && stPlayer.m_nLevel < 110)
				nMasterIndex = 31;
			else if (stPlayer.m_nLevel >= 110 && stPlayer.m_nLevel <= 120)
				nMasterIndex = 32;

			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_TYPE_ICON_X, pt.y), nMasterIndex, &pVertices, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 16), &pVertices, 0xffffffff);
		}
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_HERO)
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_TYPE_ICON_X, pt.y), 33, &pVertices, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 24), &pVertices, 0xffffffff);
		}
		else if (prj.m_aJob[stPlayer.m_nJob].dwJobType == JTYPE_LEGEND_HERO)
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_TYPE_ICON_X, pt.y), 33, &pVertices, 0xffffffff);
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), (19 + stPlayer.m_nJob - 32), &pVertices, 0xffffffff);
		}
		else
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(JOB_ICON_X, pt.y), 14 + stPlayer.m_nJob, &pVertices, 0xffffffff);

		// Draw Name
		DWORD dwColor = 0xff000000;
		if (i == m_nCurSelect)
			dwColor = 0xff6060ff;

		strFormat.Format("%s", stPlayer.m_szName);
		g_WndMng.StringSize(strFormat, 10);
		p2DRender->TextOut(226, pt.y + 3, strFormat, dwColor);

		pt.y += m_nFontHeight;
	}

	pWndWorld->m_texPlayerDataIcon.Render(m_pApp->m_pd3dDevice, pVertex, ((int)pVertices - (int)pVertex) / sizeof(TEXTUREVERTEX2));
	SAFE_DELETE_ARRAY(pVertex);
}

void CWndGuildCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt(188, 3);
	CRect rect;

	int nCount = m_vPlayerList.size();
	if (nCount > 10)
		nCount = 10;

	for (int j = 0; j < nCount; j++)
	{
		rect.SetRect(pt.x, pt.y, pt.x + 24, pt.y + m_nFontHeight);
		if (rect.PtInRect(point))
		{
			vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
			int nPos = m_wndScrollBar.GetScrollPos();
			iter += j + nPos;
			__MESSENGER_PLAYER stPlayer = *(iter);
			ClientToScreen(&point);
			ClientToScreen(&rect);
			g_toolTip.PutToolTip(100, prj.m_aJob[stPlayer.m_nJob].szName, rect, point, 3);
			j = m_vPlayerList.size();
		}
		pt.y += m_nFontHeight;
	}
}

void CWndGuildCtrlEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	CPoint pt(3, 3);
	CRect rect;

	for (int j = 0; j < (int)(m_vPlayerList.size()); j++)
	{
		rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight);
		if (rect.PtInRect(point))
		{
			int nPos = m_wndScrollBar.GetScrollPos();
			m_nCurSelect = j + nPos;
			return;
		}
		pt.y += m_nFontHeight;
	}
}

void CWndGuildCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	u_long idPlayer;
	CGuildMember* pGuildMember = NULL;
	int nSelect = GetSelect(point, idPlayer, &pGuildMember);
	if (nSelect != -1 && pGuildMember)
	{
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pGuildMember->m_idPlayer);

		if (pPlayerData && pPlayerData->data.uLogin > 0) // state online
		{
			m_nCurSelect = nSelect;
			CWndMessage* pWndMessage = g_WndMng.OpenMessage(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
		}
		else
		{
			CString szMessage;
			szMessage = prj.GetText(TID_GAME_NOTLOGIN);
			g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN));
		}
	}
}

u_long CWndGuildCtrlEx::GetSelectId(int SelectCount)
{
	vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
	iter += SelectCount;
	__MESSENGER_PLAYER stPlayer = *(iter);

	return stPlayer.m_dwPlayerId;
}

int	CWndGuildCtrlEx::GetSelect(CPoint point, u_long& idPlayer, CGuildMember** lppGuildMember)
{
	CPoint pt(3, 3);
	CRect rect;
	int rtn_val = -1;

	CGuild* pGuild = g_pPlayer->GetGuild();

	if (pGuild)
	{
		for (int j = 0; j < (int)(m_vPlayerList.size()); j++)
		{
			rtn_val++;
			rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight);
			if (rect.PtInRect(point))
			{
				vector < __MESSENGER_PLAYER >::iterator iter = m_vPlayerList.begin();
				int nPos = m_wndScrollBar.GetScrollPos();
				iter += j + nPos;
				rtn_val += nPos;
				__MESSENGER_PLAYER stPlayer = *(iter);

				idPlayer = stPlayer.m_dwPlayerId;
				CGuildMember* lpGuildMember = pGuild->GetMember(idPlayer);
				if (lpGuildMember)
					*lppGuildMember = lpGuildMember;

				j = m_vPlayerList.size();
			}
			pt.y += m_nFontHeight;
		}
	}

	return rtn_val;
}

void CWndGuildCtrlEx::SetScrollBar()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = m_vPlayerList.size();
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
}

void CWndGuildCtrlEx::ScrollBarPos(int nPos)
{
	m_wndScrollBar.SetScrollPos(nPos);
}

int CWndGuildCtrlEx::GetDrawCount(void)
{
	CGuild* pGuild = g_pPlayer->GetGuild();
	int nMax = 0;
	if (pGuild)
		nMax = pGuild->GetSize();

	if (nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage())
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if (nMax < m_wndScrollBar.GetScrollPos())
		nMax = 0;

	return nMax;
}

void CWndGuildCtrlEx::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect(rect);

	CWndBase::OnSize(nType, cx, cy);
}
void CWndGuildCtrlEx::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(3, 3);

	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}

void CWndGuildCtrlEx::SortbyChannel(BOOL bCheckbefore)
{
	if (!bCheckbefore)
		m_bSortbyChannel = !m_bSortbyChannel;

	if (m_bSortbyChannel)
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prChannelAsce);
		m_bSortbyChannel = FALSE;
	}
	else
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prChannelDesc);
		m_bSortbyChannel = TRUE;
	}
	m_nCurSort = SORT_BY_CHANNEL;
}

void CWndGuildCtrlEx::SortbyStatus(BOOL bCheckbefore)
{
	if (!bCheckbefore)
		m_bSortbyStatus = !m_bSortbyStatus;

	if (m_bSortbyStatus)
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prStatusAsce);
		m_bSortbyStatus = FALSE;
	}
	else
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prStatusDesc);
		m_bSortbyStatus = TRUE;
	}
	m_nCurSort = SORT_BY_STATUS;
}

void CWndGuildCtrlEx::SortbyLevel(BOOL bCheckbefore)
{
	if (!bCheckbefore)
		m_bSortbyLevel = !m_bSortbyLevel;

	if (m_bSortbyLevel)
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prLevelAsce);
		m_bSortbyLevel = FALSE;
	}
	else
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prLevelDesc);
		m_bSortbyLevel = TRUE;
	}
	m_nCurSort = SORT_BY_LEVEL;
}

void CWndGuildCtrlEx::SortbyJob(BOOL bCheckbefore)
{
	if (!bCheckbefore)
		m_bSortbyJob = !m_bSortbyJob;

	if (m_bSortbyJob)
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prJobAsce);
		m_bSortbyJob = FALSE;
	}
	else
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prJobDesc);
		m_bSortbyJob = TRUE;
	}
	m_nCurSort = SORT_BY_JOB;
}

void CWndGuildCtrlEx::SortbyName(BOOL bCheckbefore)
{
	if (!bCheckbefore)
		m_bSortbyName = !m_bSortbyName;

	if (m_bSortbyName)
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prNameAsce);
		m_bSortbyName = FALSE;
	}
	else
	{
		std::sort(m_vPlayerList.begin(), m_vPlayerList.end(), prNameDesc);
		m_bSortbyName = TRUE;
	}
	m_nCurSort = SORT_BY_NAME;
}

#endif //__CSC_VER11_4