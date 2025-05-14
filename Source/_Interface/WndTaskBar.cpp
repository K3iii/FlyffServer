// WndArcane.cpp: implementation of the CWndNeuz class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DialogMsg.h"
#include "WndField.h"
#include "WndManager.h"
#include "WndRegVend.h"
#include "defineSound.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;


#define WND_WIDTH 210
#define TASKBAR_HEIGHT 48


CWndShortcut::CWndShortcut()
{
}
CWndShortcut::~CWndShortcut()
{
}
void CWndShortcut::OnDraw(C2DRender* p2DRender)
{
	CWndButton::OnDraw(p2DRender);
}
BOOL CWndShortcut::Process()
{
	return CWndButton::Process();
}
void CWndShortcut::PaintFrame(C2DRender* p2DRender)
{
	CWndButton::PaintFrame(p2DRender);
}
void CWndShortcut::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWndButton::OnLButtonUp(nFlags, point);
}
void CWndShortcut::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndButton::OnLButtonDown(nFlags, point);
}
void CWndShortcut::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWndButton::OnRButtonUp(nFlags, point);
}
void CWndShortcut::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWndButton::OnRButtonDown(nFlags, point);
}
void CWndShortcut::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWndButton::OnLButtonDblClk(nFlags, point);
}
void CWndShortcut::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	CWndButton::OnRButtonDblClk(nFlags, point);
}
void CWndShortcut::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWndButton::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CWndShortcut::OnMouseMove(UINT nFlags, CPoint point)
{
	CWndButton::OnMouseMove(nFlags, point);
}

#define ICON_SIZE    32
#define SKILL_SIZE   28


DWORD   POINT_APPLET_X;
DWORD   POINT_APPLET_Y;
DWORD   POINT_ITEM_X;
DWORD   POINT_ITEM_Y;
DWORD   POINT_QUEUE_X;
DWORD   POINT_QUEUE_Y;

#define HORIRC_APPLET( nIndex ) CRect( POINT_APPLET_X + nIndex * ICON_SIZE, 6, POINT_APPLET_X + nIndex * ICON_SIZE + ICON_SIZE , 6 + ICON_SIZE )
#define VERTRC_APPLET( nIndex ) CRect( 6, POINT_APPLET_X + nIndex * ICON_SIZE, 3 + ICON_SIZE, POINT_APPLET_X + nIndex * ICON_SIZE + ICON_SIZE )
#define HORIRC_ITEM( nIndex )   CRect( POINT_ITEM_X + nIndex * ICON_SIZE, 6, POINT_ITEM_X + nIndex * ICON_SIZE + ICON_SIZE , 6 + ICON_SIZE )
#define VERTRC_ITEM( nIndex )   CRect( 6, POINT_ITEM_X + nIndex * ICON_SIZE, 3 + ICON_SIZE, POINT_ITEM_X + nIndex * ICON_SIZE + ICON_SIZE )
#define HORIRC_QUEUE( nIndex )  CRect( POINT_QUEUE_X + nIndex * SKILL_SIZE, 6, POINT_QUEUE_X + nIndex * SKILL_SIZE + SKILL_SIZE, 7 + SKILL_SIZE )
#define VERTRC_QUEUE( nIndex )  CRect( 6, POINT_QUEUE_X + nIndex * SKILL_SIZE, 6 + SKILL_SIZE, POINT_QUEUE_X + nIndex * SKILL_SIZE + SKILL_SIZE )

#define RECT_APPLET CRect( POINT_APPLET_X, POINT_APPLET_Y, POINT_APPLET_X + m_nMaxSlotApplet * ICON_SIZE , POINT_APPLET_Y + ICON_SIZE  )
#define RECT_ITEM   CRect( POINT_ITEM_X  , POINT_ITEM_Y  , POINT_ITEM_X   + MAX_SLOT_ITEM   * ICON_SIZE , POINT_ITEM_Y   + ICON_SIZE  )
#define RECT_QUEUE  CRect( POINT_QUEUE_X , POINT_QUEUE_Y , POINT_QUEUE_X  + MAX_SLOT_QUEUE  * SKILL_SIZE, POINT_QUEUE_Y  + SKILL_SIZE )

#define POINT_APPLET CPoint( POINT_APPLET_X, POINT_APPLET_Y )
#define POINT_ITEM   CPoint( POINT_ITEM_X  , POINT_ITEM_Y   )
#define POINT_QUEUE  CPoint( POINT_QUEUE_X , POINT_QUEUE_Y  )

void CWndTaskBar::OnDestroyChildWnd(CWndBase* pWndChild)
{

}

void CWndTaskBar::Serialize(CAr& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
		InitTaskBar();
		int nCount, nIndex;
		ar >> nCount;
		for (int i = 0; i < nCount; i++)
		{
			ar >> nIndex;
			ar >> m_aSlotApplet[nIndex].m_dwShortcut >> m_aSlotApplet[nIndex].m_dwId >> m_aSlotApplet[nIndex].m_dwType;
			ar >> m_aSlotApplet[nIndex].m_dwIndex >> m_aSlotApplet[nIndex].m_dwUserId >> m_aSlotApplet[nIndex].m_dwData;
			ar >> m_aSlotApplet[nIndex].m_nLocation;

			if (m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT)
				ar.ReadString(m_aSlotApplet[nIndex].m_szString, MAX_SHORTCUT_STRING);
			m_aSlotApplet[nIndex].m_dwIndex = nIndex;
			SetTaskBarTexture(&m_aSlotApplet[nIndex]);

			if (m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_SKILL)
			{
				ItemProp* pProp;
				if (m_aSlotApplet[nIndex].m_dwType == 2)
					pProp = prj.GetPartySkill(m_aSlotApplet[nIndex].m_dwId);
				else
				{
					LPSKILL pSkill = g_pPlayer->GetSkill(0, m_aSlotApplet[nIndex].m_dwId);
					pProp = prj.GetSkillProp(pSkill->dwSkill);
				}
				if (pProp == NULL || (pProp->dwComboStyle != CT_STEP && pProp->dwComboStyle != CT_GENERAL))
				{
					m_aSlotApplet[nIndex].Empty();
				}
			}
			else if (m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_ITEM)
			{
				if (g_pPlayer)
				{
					CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(m_aSlotApplet[nIndex].m_nLocation);
					if (pContainer)
					{
						CItemElem* pItemBase = pContainer->GetAtId(m_aSlotApplet[nIndex].m_dwId);
						if (pItemBase && pItemBase->GetProp()->dwPackMax > 1)
							m_aSlotApplet[nIndex].m_dwItemId = pItemBase->m_dwItemId;
					}
				}
			}
#ifdef __FL_SWITCH_EQ
			else if (m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_EQ_SWITCH)
			{
				if (g_pPlayer && m_aSlotApplet[nIndex].m_dwId >= g_pPlayer->m_Switches.GetTotalSwitches())
					m_aSlotApplet[nIndex].Empty();
			}
#endif
		}

		ar >> nCount;
		int nIndex2;
		for (int i = 0; i < nCount; i++)
		{
			ar >> nIndex >> nIndex2;
			ar >> m_aSlotItem[nIndex][nIndex2].m_dwShortcut >> m_aSlotItem[nIndex][nIndex2].m_dwId >> m_aSlotItem[nIndex][nIndex2].m_dwType;
			ar >> m_aSlotItem[nIndex][nIndex2].m_dwIndex >> m_aSlotItem[nIndex][nIndex2].m_dwUserId >> m_aSlotItem[nIndex][nIndex2].m_dwData;
			ar >> m_aSlotItem[nIndex][nIndex2].m_nLocation;
			if (m_aSlotItem[nIndex][nIndex2].m_dwShortcut == SHORTCUT_CHAT)
				ar.ReadString(m_aSlotItem[nIndex][nIndex2].m_szString, MAX_SHORTCUT_STRING);

			else if (m_aSlotItem[nIndex][nIndex2].m_dwShortcut == SHORTCUT_ITEM)
			{
				if (g_pPlayer)
				{
					CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(m_aSlotApplet[nIndex].m_nLocation);
					if (pContainer)
					{
						CItemElem* pItemBase = pContainer->GetAtId(m_aSlotApplet[nIndex].m_dwId);
						if (pItemBase && pItemBase->GetProp()->dwPackMax > 1)
							m_aSlotItem[nIndex][nIndex2].m_dwItemId = pItemBase->m_dwItemId;
					}
				}
			}

			m_aSlotItem[nIndex][nIndex2].m_dwIndex = nIndex2;
			SetTaskBarTexture(&m_aSlotItem[nIndex][nIndex2]);
		}

		ar >> nCount;
		for (int i = 0; i < nCount; i++)
		{
			ar >> nIndex;
			ar >> m_aSlotQueue[nIndex].m_dwShortcut >> m_aSlotQueue[nIndex].m_dwId >> m_aSlotQueue[nIndex].m_dwType;
			ar >> m_aSlotQueue[nIndex].m_dwIndex >> m_aSlotQueue[nIndex].m_dwUserId >> m_aSlotQueue[nIndex].m_dwData;
			ar >> m_aSlotQueue[nIndex].m_nLocation;

			m_aSlotQueue[nIndex].m_dwIndex = nIndex;

			LPSKILL lpSkill = g_pPlayer->GetSkill(m_aSlotQueue[nIndex].m_dwType, m_aSlotQueue[nIndex].m_dwId);
			m_aSlotQueue[nIndex].m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, lpSkill->GetProp()->szIcon), 0xffff00ff, FALSE);
		}
		ar >> m_nActionPoint;
		m_nCurQueueNum = nCount;
	}
}

void CWndTaskBar::SetTaskBarTexture(LPSHORTCUT pShortcut)
{
	if (pShortcut->m_dwShortcut == SHORTCUT_APPLET)
	{
		AppletFunc* pAppletFunc = g_WndMng.GetAppletFunc(pShortcut->m_dwId);
		if (pAppletFunc)
			pShortcut->m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pAppletFunc->m_pszIconName), 0xffff00ff);
		else
			g_DPlay.SendRemoveAppletTaskBar((BYTE)(pShortcut->m_dwIndex));
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_ITEM)
	{
		CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(pShortcut->m_nLocation);
		if (pContainer)
		{
			CItemElem* pItemBase = pContainer->GetAtId(pShortcut->m_dwId);
			if (pItemBase)
				pShortcut->m_pTexture = pItemBase->GetTexture();
		}
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_SKILL)
	{
		if (pShortcut->m_dwType == 2)
		{
			ItemProp* pProp = prj.GetPartySkill(pShortcut->m_dwId);
			if (pProp)
				pShortcut->m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pProp->szIcon/*pItemBase->GetProp()->szIcon*/), 0xffff00ff);
		}
		else
		{
			LPSKILL lpSkill = g_pPlayer->GetSkill(pShortcut->m_dwType, pShortcut->m_dwId);
			if (lpSkill)
			{
				ItemProp* pSkillProp = prj.m_aPropSkill.GetAt(lpSkill->dwSkill);
				if (pSkillProp)
					pShortcut->m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pSkillProp->szIcon), 0xffff00ff);
			}
		}
	}
#if __VER >= 12 // __LORD
	else if (pShortcut->m_dwShortcut == SHORTCUT_LORDSKILL)
	{
		CCLord* pLord = CCLord::Instance();
		if (pLord)
		{
			if (pLord->GetSkills())
			{
				CLordSkillComponentExecutable* pComponent = pLord->GetSkills()->GetSkill(pShortcut->m_dwId);
				if (pComponent)
					pShortcut->m_pTexture = pComponent->GetTexture();
			}
		}
	}
#endif
	else if (pShortcut->m_dwShortcut == SHORTCUT_MOTION)
	{
		MotionProp* pMotionProp = prj.GetMotionProp(pShortcut->m_dwId);
		if (pMotionProp)
		{
			pMotionProp->pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pMotionProp->szIconName), 0xffff00ff);
			pShortcut->m_pTexture = pMotionProp->pTexture;
		}
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_CHAT)
	{
		pShortcut->m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_MacroChat.dds"), 0xffff00ff);
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_EMOTICON)
	{
		if (pShortcut->m_dwId >= 0 && pShortcut->m_dwId < MAX_EMOTICON_NUM)
		{
			TCHAR buffer[MAX_SHORTCUT_STRING] = { 0 };
			_tcscat(buffer, "/");
			_tcscat(buffer, g_DialogMsg.m_EmiticonCmd[pShortcut->m_dwId].m_szCommand);
			_tcscpy(pShortcut->m_szString, buffer);
			pShortcut->m_pTexture = g_DialogMsg.m_texEmoticonUser.GetAt(pShortcut->m_dwId);
		}
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_SKILLFUN)
	{
		pShortcut->m_pTexture = m_pTexture;
	}
#ifdef __FL_SWITCH_EQ
	else if (pShortcut->m_dwShortcut == SHORTCUT_EQ_SWITCH)
	{
		if (pShortcut->m_dwId < g_pPlayer->m_Switches.GetTotalSwitches())
			pShortcut->m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "Icon_Pvp.dds"), 0xffff00ff);
	}
#endif
}

CWndTaskBar::CWndTaskBar()
{
	m_nCurQueue = -1;
	m_nCurQueueNum = 0;
	m_nPosition = TASKBAR_BOTTOM;
	memset(m_aSlotApplet, 0, sizeof(m_aSlotApplet));
	memset(m_aSlotItem, 0, sizeof(m_aSlotItem));
	memset(m_aSlotQueue, 0, sizeof(m_aSlotQueue));
	memset(&m_aSlotSkill, 0, sizeof(m_aSlotSkill));

	m_pSelectShortcut = NULL;
	m_bStartTimeBar = FALSE;
	m_nSkillBar = 0;
	m_nUsedSkillQueue = 0;
	m_nExecute = 0;
	m_nSlotIndex = 0;
	m_paSlotItem = m_aSlotItem[m_nSlotIndex];

	m_nActionPoint = 0;
	m_dwHighAlpha = 0;
	//	m_nMaxSlotApplet = 5;

#ifdef __FL_UPDATE_TASKBAR
	for (int i = 0; i < MAX_SLOT_ITEM; i++)
		m_dwHighAlphaClick[i] = 0;
#endif

}

CWndTaskBar::~CWndTaskBar()
{
	DeleteDeviceObjects();
}

void CWndTaskBar::InitTaskBar()
{
	m_nCurQueue = -1;
	m_nCurQueueNum = 0;
	m_nPosition = TASKBAR_BOTTOM;//LEFT;//TASKBAR_TOP;
	memset(m_aSlotApplet, 0, sizeof(m_aSlotApplet));
	memset(m_aSlotItem, 0, sizeof(m_aSlotItem));
	memset(m_aSlotQueue, 0, sizeof(m_aSlotQueue));

	m_pSelectShortcut = NULL;
	m_bStartTimeBar = FALSE;
	m_nSkillBar = 0;
	m_nUsedSkillQueue = 0;
	m_nExecute = 0;
	m_nSlotIndex = 0;
	m_paSlotItem = m_aSlotItem[m_nSlotIndex];
	m_nActionPoint = 0;
	m_dwHighAlpha = 0;
	//m_nMaxSlotApplet = 5;
}
void CWndTaskBar::Copy(CWndTaskBar* pWndOld)
{
	if (!pWndOld)
		return;

	m_nCurQueue = pWndOld->m_nCurQueue;
	m_nCurQueueNum = pWndOld->m_nCurQueueNum;
	m_nPosition = pWndOld->m_nPosition;

	memcpy(m_aSlotApplet, pWndOld->m_aSlotApplet, sizeof(m_aSlotApplet));
	memcpy(m_aSlotItem, pWndOld->m_aSlotItem, sizeof(m_aSlotItem));
	memcpy(m_aSlotQueue, pWndOld->m_aSlotQueue, sizeof(m_aSlotQueue));
	memcpy(&m_aSlotSkill, &pWndOld->m_aSlotSkill, sizeof(m_aSlotSkill));

	m_pSelectShortcut = pWndOld->m_pSelectShortcut;
	m_bStartTimeBar = pWndOld->m_bStartTimeBar;
	m_nSkillBar = pWndOld->m_nSkillBar;
	m_nUsedSkillQueue = pWndOld->m_nUsedSkillQueue;
	m_nExecute = pWndOld->m_nExecute;
	m_nSlotIndex = pWndOld->m_nSlotIndex;
	m_paSlotItem = m_aSlotItem[m_nSlotIndex];
	m_nActionPoint = pWndOld->m_nActionPoint;
	m_dwHighAlpha = pWndOld->m_dwHighAlpha;

#ifdef __FL_UPDATE_TASKBAR
	for (int i = 0; i < MAX_SLOT_ITEM; i++)
		m_dwHighAlphaClick[i] = pWndOld->m_dwHighAlphaClick[i];
#endif
}

void CWndTaskBar::PutTooTip(LPSHORTCUT pShortcut, CPoint point, CRect* pRect)
{
	ClientToScreen(&point);
	ClientToScreen(pRect);
	if (pShortcut->m_dwShortcut == SHORTCUT_APPLET)
	{
		AppletFunc* pAppletFunc = g_WndMng.GetAppletFunc(pShortcut->m_dwId);
		if (pAppletFunc)
		{
			CEditString strEdit;
			if (pAppletFunc->m_cHotkey == 0)
				strEdit.AddString(pAppletFunc->m_pAppletDesc);
			else
			{
				strEdit.AddString(pAppletFunc->m_pAppletDesc);
				strEdit.AddString("\n");
				CString string;
				string.Format("[%s %c]", prj.GetText(TID_GAME_TOOLTIP_HOTKEY), pAppletFunc->m_cHotkey);
				strEdit.AddString(string, 0xff0000ff);
			}
			g_toolTip.PutToolTip(pShortcut->m_dwId, strEdit, *pRect, point, 0);
		}
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_CHAT)
	{
		g_toolTip.PutToolTip(10000, pShortcut->m_szString, *pRect, point, 0);
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_EMOTICON)
	{
		g_toolTip.PutToolTip(10000, pShortcut->m_szString, *pRect, point, 0);
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_ITEM)
	{
		g_WndMng.PutToolTip_Item(pShortcut->m_dwType, pShortcut->m_dwId, pShortcut->m_nLocation, point, pRect);
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_SKILL)
	{
		if (pShortcut->m_dwType == 2)
		{
			g_WndMng.PutToolTip_Troupe(pShortcut->m_dwId, point, pRect);
		}
		else
		{
			LPSKILL lpSkill = g_pPlayer->GetSkill(0, pShortcut->m_dwId);
			g_WndMng.PutToolTip_Skill(lpSkill->dwSkill, lpSkill->dwLevel, point, pRect);
		}
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_LORDSKILL)
	{
		CCLord* pLord = CCLord::Instance();
		CLordSkillComponentExecutable* pComponent = pLord->GetSkills()->GetSkill(pShortcut->m_dwId);
		CString							string, str;
		CEditString						strEdit;

		if (!pComponent) return;

		string.Format("#b#cff2fbe6d%s#nb#nc\n%s", pComponent->GetName(), pComponent->GetDesc());
		if (pComponent->GetTick() != 0)
		{
			str.Format(prj.GetText(TID_TOOLTIP_COOLTIME), pComponent->GetTick(), 0);
			string += "\n";
			string += str;
		}

		strEdit.SetParsingString(string);
		g_toolTip.PutToolTip(10000, strEdit, *pRect, point, 0);
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_MOTION)
	{
		MotionProp* pMotionProp = prj.GetMotionProp(pShortcut->m_dwId);
		if (!pMotionProp)
			return;

		CString string;

		string.Format("%s", pMotionProp->szDesc);

		if (pMotionProp->dwID == MOT_BASE_CHEER)
		{
			if (g_pPlayer)
			{
				CString str;
				str.Format(prj.GetText(TID_CHEER_MESSAGE1), g_pPlayer->m_nCheerPoint);

				string += "\n\n";
				string += str;

				if (g_pPlayer->m_nCheerPoint < MAX_CHEERPOINT)
				{
					CTimeSpan ct(((g_pPlayer->m_dwTickCheer - GetTickCount()) / 1000));

					if (ct.GetMinutes() <= 0)
					{
						str.Format(prj.GetText(TID_PK_LIMIT_SECOND), ct.GetSeconds());
					}
					else
					{
						str.Format(prj.GetText(TID_PK_LIMIT_MINUTE), ct.GetMinutes());
					}

					string += "\n";
					string += (prj.GetText(TID_CHEER_MESSAGE2) + str);
				}
			}
		}

		CEditString strEdit;
		strEdit.SetParsingString(string);
		g_toolTip.PutToolTip(pShortcut->m_dwId, strEdit, *pRect, point, 0);
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_SKILLFUN)
	{
		strcpy(pShortcut->m_szString, prj.GetText(TID_GAME_SKILLSHORTCUT));
		g_toolTip.PutToolTip(10000, pShortcut->m_szString, *pRect, point, 0);
	}
#ifdef __FL_SWITCH_EQ
	else if (pShortcut->m_dwShortcut == SHORTCUT_EQ_SWITCH)
	{
		if (pShortcut->m_dwId < g_pPlayer->m_Switches.GetTotalSwitches())
		{
			EQ_SWITCH* pSwitch = &g_pPlayer->m_Switches.GetSwitches()->at(pShortcut->m_dwId);
			if (pSwitch)
			{
				strcpy(pShortcut->m_szString, pSwitch->szSwitchName);
				g_toolTip.PutToolTip(10000, pShortcut->m_szString, *pRect, point, 0);
			}
		}
	}
#endif

}

void CWndTaskBar::OnMouseWndSurface(CPoint point)
{
	CRect rect = CRect(POINT_APPLET_X, POINT_APPLET_Y, POINT_APPLET_X + ICON_SIZE, POINT_APPLET_Y + ICON_SIZE);
	for (int i = 0; i < m_nMaxSlotApplet; i++)
	{
		LPSHORTCUT lpShortcut = &m_aSlotApplet[i];
		if (!lpShortcut->IsEmpty() && rect.PtInRect(point))
		{
			MotionProp* pMotionProp = prj.GetMotionProp(lpShortcut->m_dwId);

			if (!IsShortcut(lpShortcut, SHORTCUT_MOTION, MOT_BASE_CHEER))
				PutTooTip(lpShortcut, point, &rect);
		}
		rect += CPoint(ICON_SIZE, 0);
	}

	rect = CRect(POINT_ITEM_X, POINT_ITEM_Y, POINT_ITEM_X + ICON_SIZE, POINT_ITEM_Y + ICON_SIZE);
	for (int i = 0; i < MAX_SLOT_ITEM; i++)
	{
		LPSHORTCUT lpShortcut = &m_paSlotItem[i];
		if (!lpShortcut->IsEmpty() && rect.PtInRect(point))
		{
			if (!IsShortcut(lpShortcut, SHORTCUT_MOTION, MOT_BASE_CHEER))
				PutTooTip(lpShortcut, point, &rect);
		}
		rect += CPoint(ICON_SIZE, 0);
	}

	rect = CRect(POINT_QUEUE_X, POINT_QUEUE_Y, POINT_QUEUE_X + SKILL_SIZE, POINT_QUEUE_Y + SKILL_SIZE);
	for (int i = 0; i < MAX_SLOT_QUEUE; i++)
	{
		LPSHORTCUT lpShortcut = &m_aSlotQueue[i];
		if (!lpShortcut->IsEmpty() && rect.PtInRect(point))
		{
			if (!IsShortcut(lpShortcut, SHORTCUT_MOTION, MOT_BASE_CHEER))
				PutTooTip(lpShortcut, point, &rect);
		}
		rect += CPoint(SKILL_SIZE, 0);
	}

	rect = CRect(POINT_QUEUE_X - ICON_SIZE - 5, POINT_QUEUE_Y, (POINT_QUEUE_X - ICON_SIZE - 5) + 32, POINT_QUEUE_Y + 32);

	if (rect.PtInRect(point))
	{
		SHORTCUT Shortcut;
		Shortcut.m_dwShortcut = SHORTCUT_SKILLFUN;
		PutTooTip(&Shortcut, point, &rect);
	}
}

#define DRAW_HOTKEY( p2DRender, point, cHotkey ) { CString string; string.Format( "%c", cHotkey ); \
					p2DRender->TextOut( point.x - 0 + 2, point.y - 0 - 4, string, 0xffffffff ); }


void CWndTaskBar::OnDraw(C2DRender* p2DRender)
{
	if (NULL == g_pPlayer)
		return;

#ifdef __FL_UPDATE_TASKBAR
	RenderClicks(p2DRender);
#endif

	DWORD dwCur = g_tmCurrent;
	CRect rect = GetWindowRect();
	CRect rectSkillQueue;

	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontWndTitle);

	if (m_nPosition == TASKBAR_TOP || m_nPosition == TASKBAR_BOTTOM)
	{
		rectSkillQueue.SetRect(rect.Width() - 32 * 4 - 6, 0, rect.Width(), rect.Height());
	}
	else if (m_nPosition == TASKBAR_LEFT || m_nPosition == TASKBAR_RIGHT)
		rectSkillQueue.SetRect(0, rect.Height() - 32 * 4 - 6, rect.Width(), rect.Height());

	if (m_GlobalShortcut.IsEmpty() == FALSE)
	{
		DWORD dwColor;
		if (m_dwHighAlpha < 128)
			dwColor = D3DCOLOR_ARGB(m_dwHighAlpha, 0, 0, 255);
		else
			dwColor = D3DCOLOR_ARGB(128 - (m_dwHighAlpha - 128), 0, 0, 255);


		LPWNDCTRL pApplet = GetWndCtrl(WIDC_CUSTOM1);
		LPWNDCTRL pItem = GetWndCtrl(WIDC_CUSTOM2);
		LPWNDCTRL pSkill = GetWndCtrl(WIDC_CUSTOM3);

		CRect rectApplet = pApplet->rect;
		CRect rectItem = pItem->rect;
		CRect rectSkill = pSkill->rect;

		rectApplet.top = 43;
		rectApplet.bottom = 48;
		rectApplet.left -= 6;
		rectApplet.right += 6;
		rectItem.top = 43;
		rectItem.bottom = 48;
		rectItem.left -= 6;
		rectItem.right += 6;
		rectSkill.top = 43;
		rectSkill.bottom = 48;
		rectSkill.left -= 3;
		rectSkill.right += 4;

		switch (m_GlobalShortcut.m_dwShortcut)
		{
		case SHORTCUT_APPLET:
		case SHORTCUT_ITEM:
		case SHORTCUT_MOTION:
		case SHORTCUT_SKILL:
#ifdef __FL_SWITCH_EQ
		case SHORTCUT_EQ_SWITCH:
#endif
			p2DRender->RenderFillRect(rectApplet, dwColor);
			p2DRender->RenderFillRect(rectItem, dwColor);
			p2DRender->RenderFillRect(rectSkill, dwColor);
			break;
		}
	}


	CPoint point = POINT_APPLET;
	for (int i = 0; i < m_nMaxSlotApplet; i++)
	{
		LPSHORTCUT lpShortcut = &m_aSlotApplet[i];
		if (!lpShortcut->IsEmpty())
		{
			if (lpShortcut->m_pTexture)
				p2DRender->RenderTexture(point, lpShortcut->m_pTexture);
			if (lpShortcut->m_dwShortcut == SHORTCUT_ITEM)
			{
				CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(lpShortcut->m_nLocation);
				if (pContainer)
				{
					CItemElem* pItemBase = pContainer->GetAtId(lpShortcut->m_dwId);
					CItemElem* pItemElem = pItemBase;
					if (pItemElem)
					{
						if (pItemElem->GetProp()->dwPackMax > 1)
						{
							TCHAR szTemp[32];
							DWORD dwItemNum = pContainer->GetItemCount(pItemElem->m_dwItemId);
							if (dwItemNum > 9999)
								_stprintf(szTemp, "9999");
							else
								_stprintf(szTemp, "%d", dwItemNum);

							CD3DFont* pOldFont = p2DRender->GetFont();
							p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
							CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
							p2DRender->TextOut(point.x + 32 - size.cx - 2, point.y + 32 - size.cy + 2, szTemp, 0xffeaebdd);
							p2DRender->SetFont(pOldFont);
						}
						DWORD dwGroup = g_pPlayer->m_cooltimeMgr.GetGroup(pItemElem->GetProp());
						if (dwGroup)
						{
							DWORD dwEnd = g_pPlayer->m_cooltimeMgr.GetTime(dwGroup);
							if (dwEnd != 0 && dwEnd > dwCur)
							{
								DWORD dwBase = g_pPlayer->m_cooltimeMgr.GetBase(dwGroup);
								RenderRadar(p2DRender, point, dwCur - dwBase, dwEnd - dwBase);
							}
						}
					}
					else
					{
						ItemProp* pItemProp = prj.GetItemProp(lpShortcut->m_dwItemId);
						if (pItemProp && pItemProp->dwPackMax > 1)
						{
							DWORD dwId = pContainer->Find(lpShortcut->m_dwItemId);
							if (dwId != NULL_ID)
							{
								lpShortcut->m_dwId = dwId;
								g_DPlay.SendAddAppletTaskBar(i, lpShortcut);
							}
							else
							{
								lpShortcut->Empty();
								g_DPlay.SendRemoveAppletTaskBar(i);
							}
						}
						else
						{
							lpShortcut->Empty();
							g_DPlay.SendRemoveAppletTaskBar(i);
						}
					}
				}
				else
				{
					lpShortcut->Empty();
					g_DPlay.SendRemoveAppletTaskBar(i);
				}
			}
			else if (lpShortcut->m_dwShortcut == SHORTCUT_APPLET)
			{
				AppletFunc* pAppletFunc = g_WndMng.GetAppletFunc(lpShortcut->m_dwId);
				if (pAppletFunc && pAppletFunc->m_cHotkey)
				{
#ifdef __FL_UPDATE_TASKBAR
					CD3DFont* pOldFont2 = p2DRender->GetFont();
					p2DRender->SetFont(CWndBase::m_Theme.m_pFontTaskbarKeys);
#endif

					CPoint ptHotkey(point.x + 8, point.y - 9);
					DRAW_HOTKEY(p2DRender, ptHotkey, pAppletFunc->m_cHotkey);

#ifdef __FL_UPDATE_TASKBAR
					p2DRender->SetFont(pOldFont2);
#endif
				}
			}
			else if (lpShortcut->m_dwShortcut == SHORTCUT_SKILL && lpShortcut->m_dwType != 2)
			{
				RenderCollTime(point, lpShortcut->m_dwId, p2DRender);
			}
			else if (lpShortcut->m_dwShortcut == SHORTCUT_LORDSKILL)
			{
				RenderLordCollTime(point, lpShortcut->m_dwId, p2DRender);
			}
			else if (lpShortcut->m_dwShortcut == SHORTCUT_MOTION)
			{
				if (lpShortcut->m_dwId == MOT_BASE_ESCAPE)
				{
					ItemProp* pItem = prj.GetItemProp(g_AddSMMode.dwSMItemID[SM_ESCAPE]);

					if (pItem && g_pPlayer)
					{
						CTimeSpan ct(g_pPlayer->m_dwSMTime[SM_ESCAPE]);

						if (ct.GetTotalSeconds())
						{
							point.y -= 1;
							point.x += 1;
							RenderRadar(m_p2DRender, point, (DWORD)(pItem->dwCircleTime - ct.GetTotalSeconds()), pItem->dwCircleTime);
						}
					}
				}
			}
#ifdef __FL_SWITCH_EQ
			else if (lpShortcut->m_dwShortcut == SHORTCUT_EQ_SWITCH)
			{
				if (lpShortcut->m_dwId >= g_pPlayer->m_Switches.GetTotalSwitches())
				{
					lpShortcut->Empty();
					g_DPlay.SendRemoveAppletTaskBar(i);
				}
			}
#endif
		}
		point += CPoint(ICON_SIZE, 0);
	}

	point = POINT_ITEM;
	for (int i = 0; i < MAX_SLOT_ITEM; i++)
	{
		LPSHORTCUT lpShortcut = &m_paSlotItem[i];
		if (!lpShortcut->IsEmpty())
		{
			if (lpShortcut->m_pTexture)
				p2DRender->RenderTexture(point, lpShortcut->m_pTexture);
			if (lpShortcut->m_dwShortcut == SHORTCUT_ITEM)
			{
				CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(lpShortcut->m_nLocation);
				if (pContainer)
				{
					CItemElem* pItemBase = pContainer->GetAtId(lpShortcut->m_dwId);
					CItemElem* pItemElem = pItemBase;
					if (pItemElem)
					{
						if (pItemElem->GetProp()->dwPackMax > 1)
						{
							TCHAR szTemp[32];
							DWORD dwItemNum = pContainer->GetItemCount(pItemElem->m_dwItemId);
							if (dwItemNum > 9999)
								_stprintf(szTemp, "9999");
							else
								_stprintf(szTemp, "%d", dwItemNum);

							CD3DFont* pOldFont = p2DRender->GetFont();
							p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
							CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
							p2DRender->TextOut(point.x + 32 - size.cx - 2, point.y + 32 - size.cy + 2, szTemp, 0xffeaebdd);
							p2DRender->SetFont(pOldFont);
						}

						DWORD dwGroup = g_pPlayer->m_cooltimeMgr.GetGroup(pItemElem->GetProp());
						if (dwGroup)
						{
							DWORD dwEnd = g_pPlayer->m_cooltimeMgr.GetTime(dwGroup);
							if (dwEnd != 0 && dwEnd > dwCur)
							{
								DWORD dwBase = g_pPlayer->m_cooltimeMgr.GetBase(dwGroup);
								RenderRadar(p2DRender, point, dwCur - dwBase, dwEnd - dwBase);
							}
						}
					}
				}
			}
			else if (lpShortcut->m_dwShortcut == SHORTCUT_SKILL && lpShortcut->m_dwType != 2)
			{
				RenderCollTime(point, lpShortcut->m_dwId, p2DRender);
			}
			else if (lpShortcut->m_dwShortcut == SHORTCUT_LORDSKILL)
			{
				RenderLordCollTime(point, lpShortcut->m_dwId, p2DRender);
			}
			else if (lpShortcut->m_dwShortcut == SHORTCUT_MOTION)
			{
				if (lpShortcut->m_dwId == MOT_BASE_ESCAPE)
				{
					ItemProp* pItem = prj.GetItemProp(g_AddSMMode.dwSMItemID[SM_ESCAPE]);

					if (pItem && g_pPlayer)
					{
						CTimeSpan ct(g_pPlayer->m_dwSMTime[SM_ESCAPE]);

						if (ct.GetTotalSeconds())
						{
							point.y -= 1;
							point.x += 1;
							RenderRadar(m_p2DRender, point, (DWORD)(pItem->dwCircleTime - ct.GetTotalSeconds()), pItem->dwCircleTime);
						}
					}
				}
			}
#ifdef __FL_SWITCH_EQ
			else if (lpShortcut->m_dwShortcut == SHORTCUT_EQ_SWITCH)
			{
				if (lpShortcut->m_dwId >= g_pPlayer->m_Switches.GetTotalSwitches())
				{
					lpShortcut->Empty();
					g_DPlay.SendRemoveItemTaskBar(m_nSlotIndex, i);
				}
			}
#endif
		}
#ifndef __FL_UPDATE_TASKBAR
		CHAR cHotkey = (i == 9) ? '0' : '1' + i;
		CPoint ptHotkey(point.x + 8, point.y - 9);
		point += CPoint(ICON_SIZE, 0);
#else
		CPoint ptHotkey(point.x, point.y - 13);
		CString str;
		str.Format("%d", i + 1);
		DrawHotkeys(p2DRender, ptHotkey, str);
		point += CPoint(ICON_SIZE, 0);
#endif
	}

	point = CPoint(POINT_QUEUE_X - ICON_SIZE - 5, POINT_QUEUE_Y);
	p2DRender->RenderTexture(point, m_pTexture);

	point = POINT_QUEUE;
	for (int i = 0; i < m_nCurQueueNum; i++)
	{
		LPSHORTCUT lpShortcut = &m_aSlotQueue[i];
		if (!lpShortcut->IsEmpty())
		{
			if (lpShortcut->m_pTexture)
			{
				p2DRender->RenderTexture(point, lpShortcut->m_pTexture);

			}
		}
		point += CPoint(SKILL_SIZE, 0);
	}


	if (!(g_pPlayer->IsSMMode(SM_ACTPOINT)))
	{
#define LV2MAXAP	10
#define LV3MAXAP	25
#define LV4MAXAP	50
#define LV5MAXAP	100

		point = POINT_QUEUE;
		point.x += SKILL_SIZE;
		{
			int nAP2Size;
			if (m_nActionPoint >= LV4MAXAP)
				nAP2Size = (SKILL_SIZE * 3) + (((m_nActionPoint - LV4MAXAP) * SKILL_SIZE) / (LV5MAXAP - LV4MAXAP));
			else if (m_nActionPoint >= LV3MAXAP)
				nAP2Size = (SKILL_SIZE * 2) + (((m_nActionPoint - LV3MAXAP) * SKILL_SIZE) / (LV4MAXAP - LV3MAXAP));
			else if (m_nActionPoint >= LV2MAXAP)
				nAP2Size = (SKILL_SIZE * 1) + (((m_nActionPoint - LV2MAXAP) * SKILL_SIZE) / (LV3MAXAP - LV2MAXAP));
			else
				nAP2Size = (m_nActionPoint * SKILL_SIZE) / LV2MAXAP;

			p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			point.x += nAP2Size;
			nAP2Size = (SKILL_SIZE * 4) - nAP2Size;
			p2DRender->RenderFillRect(CRect(point.x, point.y, point.x + nAP2Size, point.y + SKILL_SIZE + 2),
				D3DCOLOR_ARGB(0x7f, 0, 0, 0));

			if (m_nCurQueueNum > 0)
			{
				int show = 1;

				if (m_nActionPoint >= LV2MAXAP)
					show = 2;
				if (m_nActionPoint >= LV3MAXAP)
					show = 3;
				if (m_nActionPoint >= LV4MAXAP)
					show = 4;
				if (m_nActionPoint >= LV5MAXAP)
					show = 5;

				if (m_nCurQueueNum <= show)
					show = m_nCurQueueNum;

				CPoint Point = CPoint(POINT_QUEUE_X, POINT_QUEUE_Y + 1);

				RenderOutLineLamp(POINT_QUEUE_X, POINT_QUEUE_Y + 1, show, SKILL_SIZE);
			}
		}
	}

	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_UP);
	rect = pWndButton->GetWindowRect(TRUE);
	rect.top = rect.bottom - 4;
#ifndef __FL_UPDATE_TASKBAR
	p2DRender->TextOut(rect.left, rect.top, m_nSlotIndex + 1, 0xffffffff);
#else
	CString str;
	if (!g_Option.m_bRekeyTask)
	{
		str.Format("F%d", m_nSlotIndex + 1);
		rect.left -= 3;
	}
	else
		str.Format("%d", m_nSlotIndex + 1);

	p2DRender->TextOut(rect.left, rect.top, str, 0xffffffff);
#endif
	p2DRender->SetFont(pOldFont);
}

void CWndTaskBar::UpdateItem()
{
	if (g_pPlayer == NULL)
		return;

	for (int i = 0; i < MAX_SLOT_ITEM; i++)
	{
		LPSHORTCUT lpShortcut = &m_paSlotItem[i];
		if (!lpShortcut->IsEmpty())
		{
			if (lpShortcut->m_dwShortcut == SHORTCUT_ITEM)
			{
				CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(lpShortcut->m_nLocation);
				if (pContainer)
				{
					CItemElem* pItemBase = pContainer->GetAtId(lpShortcut->m_dwId);
					CItemElem* pItemElem = pItemBase;
					if (pItemElem)
					{
						if (lpShortcut->m_pTexture != pItemElem->GetTexture())
							lpShortcut->m_pTexture = pItemElem->GetTexture();
					}
					else
					{
						ItemProp* pItemProp = prj.GetItemProp(lpShortcut->m_dwItemId);
						if (pItemProp && pItemProp->dwPackMax > 1)
						{
							DWORD dwId = pContainer->Find(lpShortcut->m_dwItemId);
							if (dwId != NULL_ID)
							{
								lpShortcut->m_dwId = dwId;
								g_DPlay.SendAddItemTaskBar(m_nSlotIndex, i, lpShortcut);
							}
							else
							{
								lpShortcut->Empty();
								g_DPlay.SendRemoveItemTaskBar(m_nSlotIndex, i);
							}
						}
						else
						{
							lpShortcut->Empty();
							g_DPlay.SendRemoveItemTaskBar(m_nSlotIndex, i);
						}
					}
				}
				else
				{
					lpShortcut->Empty();
					g_DPlay.SendRemoveItemTaskBar(m_nSlotIndex, i);
				}
			}
		}
	}
}

void CWndTaskBar::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndTaskBar::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL pApplet = GetWndCtrl(WIDC_CUSTOM1);
	LPWNDCTRL pItem = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL pSkill = GetWndCtrl(WIDC_CUSTOM3);

	POINT_APPLET_X = pApplet->rect.left;
	POINT_APPLET_Y = pApplet->rect.top;
	POINT_ITEM_X = pItem->rect.left;
	POINT_ITEM_Y = pItem->rect.top;
	POINT_QUEUE_X = pSkill->rect.left;
	POINT_QUEUE_Y = pSkill->rect.top;

	CRect rect(5, 7, 754, 40);

	g_WndMng.m_pWndTaskBar = this;

#ifndef __FL_UPDATE_TASKBAR
	rect = g_Neuz.GetDeviceRect();
	m_pWndRoot->m_rectLayout = rect;
	switch (m_nPosition)
	{
	case TASKBAR_TOP:
		rect.bottom = TASKBAR_HEIGHT;
		m_pWndRoot->m_rectLayout.top = rect.bottom;
		break;
	case TASKBAR_BOTTOM:
		rect.top = rect.bottom - TASKBAR_HEIGHT;
		m_pWndRoot->m_rectLayout.bottom = rect.top;
		break;
	case TASKBAR_LEFT:
		rect.right = TASKBAR_HEIGHT;
		m_pWndRoot->m_rectLayout.left = rect.right;
		break;
	case TASKBAR_RIGHT:
		rect.left = rect.right - TASKBAR_HEIGHT;
		m_pWndRoot->m_rectLayout.right = rect.left;
		break;
	}
	SetWndRect(rect);
#else
	rect = g_Neuz.GetDeviceRect();
	m_pWndRoot->m_rectLayout = rect;

	if (g_Option.m_nResWidth < 1024)
	{
		rect.left = rect.right / 2 - 800 / 2;
		rect.right = rect.left + 800;
	}
	else if (g_Option.m_nResWidth >= 1024 && g_Option.m_nResWidth < 1280)
	{
		rect.left = rect.right / 2 - 1024 / 2;
		rect.right = rect.left + 1024;
	}
	else
	{
		rect.left = rect.right / 2 - 1280 / 2;
		rect.right = rect.left + 1280;
	}

	rect.top = rect.bottom - TASKBAR_HEIGHT;

	SetWndRect(rect);
#endif

	m_menuShortcut.CreateMenu(this);
	m_menuShortcut.AppendMenu(0, 0, "Test");

	m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_ActionSkill.dds"), 0xffff00ff);
	m_aSlotSkill.m_dwShortcut = SHORTCUT_SKILLFUN;
	m_aSlotSkill.m_pTexture = m_pTexture;
}

HRESULT CWndTaskBar::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();

	return S_OK;
}

HRESULT CWndTaskBar::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return S_OK;
}

HRESULT CWndTaskBar::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	return 0;
}

BOOL CWndTaskBar::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rect = g_Neuz.GetDeviceRect();
	BOOL bResult;

	if (g_Option.m_nResWidth < 1024)
	{
		m_nMaxSlotApplet = 5;
		bResult = CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_TASKBAR800, 0, CPoint(0, 0), pWndParent);
	}
	else if (g_Option.m_nResWidth >= 1024 && g_Option.m_nResWidth < 1280)
	{
		m_nMaxSlotApplet = 10;
		bResult = CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_TASKBAR1024, 0, CPoint(0, 0), pWndParent);
	}
	else
	{
		m_nMaxSlotApplet = MAX_SLOT_APPLET;
		bResult = CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_TASKBAR1280, 0, CPoint(0, 0), pWndParent);
	}

	DelWndStyle(WBS_MOVE);
	return bResult;

}
void CWndTaskBar::SetItemSlot(int nSlot)
{
	if (nSlot >= 0 && nSlot < MAX_SLOT_ITEM_COUNT)
		m_paSlotItem = m_aSlotItem[m_nSlotIndex = nSlot];
}
BOOL CWndTaskBar::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndButton* pWndButton = (CWndButton*)pLResult;
	if (message == WIN_ITEMDROP)
	{
	}
	else
		switch (nID)
		{
		case WIDC_UP:
			m_nSlotIndex--;
			if (m_nSlotIndex < 0) m_nSlotIndex = MAX_SLOT_ITEM_COUNT - 1;
			m_paSlotItem = m_aSlotItem[m_nSlotIndex];
			break;
		case WIDC_DOWN:
			m_nSlotIndex++;
			if (m_nSlotIndex > MAX_SLOT_ITEM_COUNT - 1) m_nSlotIndex = 0;
			m_paSlotItem = m_aSlotItem[m_nSlotIndex];
			break;
		case WIDC_BUTTON1: // Menu
		{
			g_WndMng.OpenMenu();
			CRect rect = g_Neuz.GetDeviceRect();
#ifndef __FL_UPDATE_TASKBAR
			switch (m_nPosition)
			{
			case TASKBAR_TOP:
				if (g_WndMng.m_pWndMenu)
					g_WndMng.m_pWndMenu->Move(CPoint(0, TASKBAR_HEIGHT));
				break;
			case TASKBAR_BOTTOM:
				if (g_WndMng.m_pWndMenu)
					g_WndMng.m_pWndMenu->Move(CPoint(0, rect.bottom - TASKBAR_HEIGHT - g_WndMng.m_pWndMenu->GetWndRect().Height()));
				break;
			case TASKBAR_LEFT:
				if (g_WndMng.m_pWndMenu)
					g_WndMng.m_pWndMenu->Move(CPoint(TASKBAR_HEIGHT, 0));
				break;
			case TASKBAR_RIGHT:
				if (g_WndMng.m_pWndMenu)
					g_WndMng.m_pWndMenu->Move(CPoint(rect.right - TASKBAR_HEIGHT - g_WndMng.m_pWndMenu->GetWndRect().Width(), 0));
				break;
			}
#else
			if (g_WndMng.m_pWndMenu)
			{
				CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
				if (pTaskBar)
				{
					int x, y;
					x = pTaskBar->GetWindowRect(TRUE).left;
					y = rect.bottom - 48 - g_WndMng.m_pWndMenu->GetWndRect().Height();
					g_WndMng.m_pWndMenu->Move(CPoint(x, y));
				}
				else
					g_WndMng.m_pWndMenu->Move(CPoint(0, rect.bottom - 48 - g_WndMng.m_pWndMenu->GetWndRect().Height()));
			}
#endif

		}
		default:
		{
			switch (pWndButton->m_shortcut.m_dwIndex)
			{
			case APP_TRADE:		g_DPlay.SendTrade((CMover*)g_WorldMng.Get()->GetObjFocus());	break;
			default:
				g_WndMng.ObjectExecutor(&pWndButton->m_shortcut);
				break;
			}
		}
		}
	return TRUE;
}
BOOL CWndTaskBar::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return TRUE;
}
void CWndTaskBar::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndTaskBar::RemoveSkillQueue(int nIndex, BOOL bSend)
{
	if (m_nExecute)
		return;
BACK:
	if (!m_aSlotQueue[nIndex].IsEmpty())
	{
		ItemProp* pItemProp = g_pPlayer->GetSkill(0, m_aSlotQueue[nIndex].m_dwId)->GetProp();
		DWORD dwComboStyleSrc = pItemProp->dwComboStyle;
		int i = NULL;
		for (i = nIndex; i < MAX_SLOT_QUEUE - 1; i++)
		{
			memcpy(&m_aSlotQueue[i], &m_aSlotQueue[i + 1], sizeof(SHORTCUT));
			m_aSlotQueue[i].m_dwIndex = i;
		}
		m_aSlotQueue[i].m_dwShortcut = SHORTCUT_NONE;
		m_aSlotQueue[i].m_dwId = 0;
		m_nCurQueueNum--;
		if (m_nCurQueueNum < 0)
			m_nCurQueueNum = 0;

		PLAYSND(SND_PC_ACTIONREMOVE);

		if (CheckAddSkill(dwComboStyleSrc, nIndex /*- 1*/) == FALSE)
			goto BACK;
	}
	if (bSend)
		g_DPlay.SendSkillTaskBar();

}
void CWndTaskBar::OnRButtonUp(UINT nFlags, CPoint point)
{
	CRect rect = RECT_APPLET;
	if (rect.PtInRect(point))
	{
		point.x -= rect.left;
		point.x /= ICON_SIZE;
		if (FALSE == m_aSlotApplet[point.x].IsEmpty())
		{
			PLAYSND(SND_PC_ACTIONREMOVE);

			m_aSlotApplet[point.x].Empty();
			g_DPlay.SendRemoveAppletTaskBar((BYTE)(point.x));
		}
	}
	rect = RECT_ITEM;
	if (rect.PtInRect(point))
	{
		point.x -= rect.left;

		point.x /= ICON_SIZE;
		if (FALSE == m_paSlotItem[point.x].IsEmpty())
		{
			PLAYSND(SND_PC_ACTIONREMOVE);

			m_paSlotItem[point.x].Empty();
			g_DPlay.SendRemoveItemTaskBar((BYTE)(m_nSlotIndex), (BYTE)(point.x));
		}

	}

	rect = RECT_QUEUE;
	if (rect.PtInRect(point))
	{
		point.x -= rect.left;
		point.y -= rect.top;
		point.x /= SKILL_SIZE;
		RemoveSkillQueue(point.x);
	}
}

BOOL CWndTaskBar::SetShortcut(int nIndex, DWORD dwShortcut, DWORD dwType, DWORD dwId, CTexture* pTexture, int nWhere, int nLocation)
{
	LPSHORTCUT pShortcut = NULL;

	PLAYSND(SND_PC_ACTIONREGISTER);

	if (dwShortcut == SHORTCUT_CHAT)
	{
		int nchatshortcut = 0;
		for (int i = 0; i < MAX_SLOT_ITEM_COUNT; i++)
		{
			for (int j = 0; j < MAX_SLOT_ITEM; j++)
			{
				if (m_aSlotItem[i][j].m_dwShortcut == SHORTCUT_CHAT)
					nchatshortcut++;
			}
		}
		for (int k = 0; k < m_nMaxSlotApplet; ++k)
			if (m_aSlotApplet[k].m_dwShortcut == SHORTCUT_CHAT)
				++nchatshortcut;

		if (nchatshortcut > 9)
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_MAX_SHORTCUT_CHAT), NULL, prj.GetTextColor(TID_GAME_MAX_SHORTCUT_CHAT));
			return FALSE;
		}
	}

	switch (m_nPosition)
	{
	case TASKBAR_TOP:
	case TASKBAR_BOTTOM:
		if (nWhere == 0)
			pShortcut = &m_aSlotApplet[nIndex];
		else if (nWhere == 1)
			pShortcut = &m_paSlotItem[nIndex];
		break;
	case TASKBAR_LEFT:
	case TASKBAR_RIGHT:
		if (nWhere == 0)
			pShortcut = &m_aSlotApplet[nIndex];
		else if (nWhere == 1)
			pShortcut = &m_paSlotItem[nIndex];
		break;
	}
	if (pTexture == NULL)
	{
		if (dwShortcut == SHORTCUT_APPLET)
		{
			AppletFunc* pAppletFunc = g_WndMng.GetAppletFunc(dwId);
			if (pAppletFunc)
				pShortcut->m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pAppletFunc->m_pszIconName), 0xffff00ff);
			else
				return FALSE;

		}
		else if (dwShortcut == SHORTCUT_ITEM)
		{
			CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(nLocation);
			if (pContainer)
			{
				CItemElem* pItemBase = pContainer->GetAtId(dwId);
				if (pItemBase && pItemBase->GetProp())
				{
					CTexture* pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemBase->GetProp()->szIcon), 0xffff00ff);
					pItemBase->SetTexture(pTexture);
					pShortcut->m_pTexture = pTexture;
				}
				else
					return FALSE;
			}
			else
				return FALSE;
		}
		else if (dwShortcut == SHORTCUT_MOTION)
		{
			MotionProp* pMotionProp = prj.GetMotionProp(dwId);
			if (pMotionProp)
			{
				pMotionProp->pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pMotionProp->szIconName), 0xffff00ff);
				pShortcut->m_pTexture = pMotionProp->pTexture;
			}
			else
				return FALSE;
		}
		else if (dwShortcut == SHORTCUT_SKILL)
		{
			if (pShortcut->m_dwType == 2)
			{
				ItemProp* pProp = prj.GetPartySkill(pShortcut->m_dwId);
				if (pProp)
					pShortcut->m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pProp->szIcon/*pItemBase->GetProp()->szIcon*/), 0xffff00ff);
				else
					return FALSE;
			}
			else
			{
				LPSKILL lpSkill = g_pPlayer->GetSkill(dwType, dwId);
				if (lpSkill)
					pShortcut->m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, lpSkill->GetProp()->szIcon/*pItemBase->GetProp()->szIcon*/), 0xffff00ff, TRUE);
				else
					return FALSE;
			}
		}
		else if (dwShortcut == SHORTCUT_SKILLFUN)
			pShortcut->m_pTexture = pTexture;
#ifdef __FL_SWITCH_EQ
		else if (dwShortcut == SHORTCUT_EQ_SWITCH)
		{
			if (pShortcut->m_dwId < g_pPlayer->m_Switches.GetTotalSwitches())
				pShortcut->m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "Icon_Pvp.dds"), 0xffff00ff);
			else
				return FALSE;
		}
#endif

	}
	else
		pShortcut->m_pTexture = pTexture;

	if (dwShortcut == SHORTCUT_ITEM)
	{
		CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(nLocation);
		if (pContainer)
		{
			CItemElem* pItemBase = pContainer->GetAtId(dwId);
			if (pShortcut && pItemBase && pItemBase->GetProp()->dwPackMax > 1)
				pShortcut->m_dwItemId = pItemBase->m_dwItemId;
		}
	}

	pShortcut->m_dwShortcut = dwShortcut;
	pShortcut->m_dwType = dwType;
	pShortcut->m_dwIndex = nIndex;//0;//dwIndex;
	pShortcut->m_dwId = dwId;
	pShortcut->m_dwUserId = 0;
	pShortcut->m_dwData = nWhere;
	pShortcut->m_nLocation = nLocation;

	strcpy(pShortcut->m_szString, m_GlobalShortcut.m_szString);//, sizeof(pShortcut->m_szString) );

	if (dwShortcut == SHORTCUT_LORDSKILL)
		pShortcut->m_dwId--;

	if (nWhere == 0) //m_aSlotApplet
	{
		g_DPlay.SendAddAppletTaskBar(nIndex, pShortcut);
	}
	else if (nWhere == 1) //m_paSlotItem
	{
		g_DPlay.SendAddItemTaskBar(m_nSlotIndex, nIndex, pShortcut);
	}
	return TRUE;
}
BOOL CWndTaskBar::CheckAddSkill(int nSkillStyleSrc, int nQueueDest)
{
	DWORD dwComboStyleSrc = nSkillStyleSrc;
	if (nQueueDest == 0)
	{
		if (m_nCurQueueNum == 0)
		{
			if (dwComboStyleSrc == CT_STEP || dwComboStyleSrc == CT_GENERAL)
				return TRUE;

			return	FALSE;
		}
		else
		{
			DWORD dwComboStyleDest;
			DWORD dwSkill = g_pPlayer->GetSkill(0, m_aSlotQueue[nQueueDest].m_dwId)->dwSkill;

			if (dwSkill)
				dwComboStyleDest = prj.GetSkillProp(dwSkill)->dwComboStyle;
			else
				dwComboStyleDest = 0;
			if (dwComboStyleSrc == CT_GENERAL && dwComboStyleDest == CT_GENERAL)
				return TRUE;
			return FALSE;
		}
	}
	else if (nQueueDest == 4)
	{
		if (dwComboStyleSrc == CT_STEP)
			return FALSE;

		DWORD dwSkill = g_pPlayer->GetSkill(0, m_aSlotQueue[nQueueDest - 1].m_dwId)->dwSkill;

		DWORD dwComboStylePrev;
		if (dwSkill)
			dwComboStylePrev = prj.GetSkillProp(dwSkill)->dwComboStyle;
		else
			dwComboStylePrev = 0;
		if (dwComboStylePrev == CT_FINISH)
			return FALSE;

		if (dwComboStyleSrc == CT_CIRCLE && dwComboStylePrev == CT_GENERAL)
			return FALSE;

		if (dwComboStyleSrc == CT_FINISH && dwComboStylePrev == CT_GENERAL)
			return FALSE;

		if (dwComboStyleSrc == CT_GENERAL && dwComboStylePrev != CT_GENERAL)
			return FALSE;

		return TRUE;
	}
	else
	{
		if (dwComboStyleSrc == CT_STEP)
			return FALSE;

		DWORD dwComboStylePrev;
		DWORD dwComboStyleDest;
		DWORD dwSkill = g_pPlayer->GetSkill(0, m_aSlotQueue[nQueueDest - 1].m_dwId)->dwSkill;

		if (dwSkill)
			dwComboStylePrev = prj.GetSkillProp(dwSkill)->dwComboStyle;
		else
			dwComboStylePrev = 0;
		dwSkill = g_pPlayer->GetSkill(0, m_aSlotQueue[nQueueDest].m_dwId)->dwSkill;

		if (dwSkill)
			dwComboStyleDest = prj.GetSkillProp(dwSkill)->dwComboStyle;
		else
			dwComboStyleDest = 0;
		if (dwComboStylePrev == CT_FINISH)
			return FALSE;

		if (dwComboStyleSrc == CT_CIRCLE && dwComboStylePrev == CT_GENERAL)
			return FALSE;

		if (dwComboStyleSrc == CT_FINISH && dwComboStylePrev == CT_GENERAL)
			return FALSE;

		if (dwComboStyleSrc == CT_FINISH && dwComboStyleDest == CT_CIRCLE)
			return FALSE;

		if (dwComboStyleSrc == CT_FINISH && dwComboStyleDest == CT_FINISH)
			return FALSE;

		if (dwComboStyleSrc == CT_GENERAL)
		{
			if (dwComboStylePrev != CT_GENERAL)
				return FALSE;
			if (dwComboStyleDest != CT_GENERAL && m_nCurQueueNum > nQueueDest)
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}
BOOL CWndTaskBar::SetSkillQueue(int nIndex, DWORD dwType, DWORD dwId, CTexture* pTexture)
{
	if (m_nExecute)
		return FALSE;

	if (m_nCurQueueNum >= 5)
		return FALSE;

	for (int i = 0; i < m_nCurQueueNum; i++)
	{
		if (m_aSlotQueue[i].IsEmpty())
		{
			for (i = 0; i < m_nCurQueueNum; i++)
				m_aSlotQueue[i].Empty();

			m_nCurQueueNum = 0;
			break;
		}
	}
	if (nIndex > m_nCurQueueNum)
		nIndex = m_nCurQueueNum;

	PLAYSND(SND_PC_ACTIONREGISTER);

	LPSHORTCUT pShortcut = NULL;
	CRect rect = GetWindowRect();
	LPSKILL lpSkill = g_pPlayer->GetSkill(0, dwId);
	DWORD dwSkill = lpSkill->dwSkill;
	DWORD dwLevel = lpSkill->dwLevel;
	ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
	DWORD dwComboStyleSrc = pSkillProp->dwComboStyle;

	if (dwLevel <= 0)
		return FALSE;

	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, dwLevel);
	ASSERT(pAddSkillProp);
	if ((int)pAddSkillProp->dwCooldown > 0)
	{
		CString str;
		str.Format(prj.GetText(TID_GAME_SKILLLNOTUSE), pSkillProp->szName);
		g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_SKILLLNOTUSE));
		return FALSE;
	}

	if (nIndex < m_nCurQueueNum)
	{
		if (!CheckAddSkill(dwComboStyleSrc, nIndex))
			return FALSE;

		for (int i = m_nCurQueueNum; i > nIndex; i--)
		{
			memcpy(&m_aSlotQueue[i], &m_aSlotQueue[i - 1], sizeof(SHORTCUT));
		}
		pShortcut = &m_aSlotQueue[nIndex];
		m_nCurQueueNum++;
	}
	else // add
	{
		if (!CheckAddSkill(dwComboStyleSrc, m_nCurQueueNum))
			return FALSE;

		pShortcut = &m_aSlotQueue[m_nCurQueueNum];
		nIndex = m_nCurQueueNum;
		m_nCurQueueNum++;
	}

	m_nCurQueue = -1;
	if (pTexture == NULL)
	{
		LPSKILL lpSkill = g_pPlayer->GetSkill(dwType, dwId);
		if (lpSkill)
			pShortcut->m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, lpSkill->GetProp()->szIcon), 0xff000000, FALSE);
	}
	else
		pShortcut->m_pTexture = pTexture;

	pShortcut->m_dwShortcut = SHORTCUT_SKILL;
	pShortcut->m_dwType = dwType;
	pShortcut->m_dwIndex = nIndex;
	pShortcut->m_dwId = dwId;
	pShortcut->m_dwUserId = 0;
	pShortcut->m_dwData = 2;
	_tcscpy(pShortcut->m_szString, m_GlobalShortcut.m_szString);
	g_DPlay.SendSkillTaskBar();
	return TRUE;
}

BOOL CWndTaskBar::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CRect rect = GetWindowRect();
	rect.DeflateRect(3, 3);

	if (pShortcut->m_pFromWnd == this)
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pShortcut->m_dwData;
		if (GetClientRect().PtInRect(point) == FALSE)
		{
			if (lpShortcut->m_dwData == 0)
				g_DPlay.SendRemoveAppletTaskBar((BYTE)(lpShortcut->m_dwIndex));
			else if (lpShortcut->m_dwData == 1)
				g_DPlay.SendRemoveItemTaskBar(m_nSlotIndex, (BYTE)(lpShortcut->m_dwIndex));
			else if (lpShortcut->m_dwData == 2)
			{
				RemoveSkillQueue(lpShortcut->m_dwIndex);
				return TRUE;
			}
			lpShortcut->Empty();
			return TRUE;
		}
		//	return TRUE;
	}

	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (pWndFrame
		&& (pWndFrame->GetWndId() != APP_INVENTORY
			&& pWndFrame != g_WndMng.m_pWndTaskBar
			&& pWndFrame->GetWndId() != APP_COMMUNICATION_CHAT
			&& pWndFrame->GetWndId() != APP_SKILL4
			&& pWndFrame->GetWndId() != APP_MOTION
			&& pWndFrame->GetWndId() != APP_PARTY
			&& pWndFrame->GetWndId() != APP_LORD_SKILL
			&& pWndFrame->GetWndId() != APP_CHEST
			&& pWndFrame->GetWndId() != APP_COMMON_BANK
#ifdef __FL_SWITCH_EQ
			&& pWndFrame->GetWndId() != APP_EQ_SWITCH
#endif
			))
	{
		SetForbid(TRUE);
		return FALSE;
	}

	if (pShortcut->m_dwShortcut == SHORTCUT_SKILL && pShortcut->m_dwType != 2)
	{
		LPSKILL pSkill = g_pPlayer->GetSkill(0, pShortcut->m_dwId);
		if (pSkill && (pSkill->dwLevel <= 0 || g_pPlayer->CheckSkill(pSkill->dwSkill) == FALSE))
		{
			SetForbid(TRUE);
			return FALSE;
		}
	}

	if (m_nPosition == TASKBAR_TOP || m_nPosition == TASKBAR_BOTTOM)
	{
		BOOL bForbid = TRUE;
		CRect rect = RECT_APPLET;
		if (rect.PtInRect(point))
		{
			point.x -= rect.left;
			point.y -= rect.top;
			point.x /= ICON_SIZE;

			if (pShortcut->m_pFromWnd == this)
			{
				LPSHORTCUT lpCurShortcut = (LPSHORTCUT)pShortcut->m_dwData;

				if (&m_aSlotApplet[point.x] == lpCurShortcut)
				{
					//SetForbid( TRUE );
					return FALSE;
				}
			}
			if (pShortcut->m_dwShortcut == SHORTCUT_SKILL)
			{
				ItemProp* pProp;
				if (pShortcut->m_dwType == 2)
					pProp = prj.GetPartySkill(pShortcut->m_dwId);
				else
				{
					LPSKILL pSkill = g_pPlayer->GetSkill(0, pShortcut->m_dwId);
					pProp = prj.GetSkillProp(pSkill->dwSkill);
				}
				if (pProp->dwComboStyle != CT_STEP && pProp->dwComboStyle != CT_GENERAL)
				{
					SetForbid(TRUE);
					return FALSE;
				}
			}

			SetShortcut(point.x, pShortcut->m_dwShortcut, pShortcut->m_dwType, pShortcut->m_dwId, pShortcut->m_pTexture, 0, pShortcut->m_nLocation);

			bForbid = FALSE;
		}
		rect = RECT_ITEM;
		if (rect.PtInRect(point))
		{
			point.x -= rect.left;
			point.y -= rect.top;
			point.x /= ICON_SIZE;

			if (pShortcut->m_pFromWnd == this)
			{
				LPSHORTCUT lpCurShortcut = (LPSHORTCUT)pShortcut->m_dwData;
				if (&m_aSlotItem[m_nSlotIndex][point.x] == lpCurShortcut)
					return FALSE;
			}
			if (pShortcut->m_dwShortcut == SHORTCUT_CHAT)
			{
				CString sChat = m_GlobalShortcut.m_szString;
				sChat.Replace("--", "");
				sChat.Replace("'", "");
				sChat.Replace("/*", "");
				sChat.Replace("*/", "");
				strcpy(m_GlobalShortcut.m_szString, (const char*)sChat);
			}
			else if (pShortcut->m_dwShortcut == SHORTCUT_SKILL)
			{
				ItemProp* pProp;
				if (pShortcut->m_dwType == 2)
					pProp = prj.GetPartySkill(pShortcut->m_dwId);
				else
				{
					LPSKILL pSkill = g_pPlayer->GetSkill(0, pShortcut->m_dwId);
					pProp = prj.GetSkillProp(pSkill->dwSkill);
				}
				if (pProp->dwComboStyle != CT_STEP && pProp->dwComboStyle != CT_GENERAL)
				{
					SetForbid(TRUE);
					return FALSE;
				}
			}

			SetShortcut(point.x, pShortcut->m_dwShortcut, pShortcut->m_dwType, pShortcut->m_dwId, pShortcut->m_pTexture, 1, pShortcut->m_nLocation);
			bForbid = FALSE;
		}

		rect = RECT_QUEUE;
		if (rect.PtInRect(point))
		{
			if (pShortcut->m_dwShortcut == SHORTCUT_SKILL && pShortcut->m_dwType == 0)
			{
				point.x -= rect.left;
				point.y -= rect.top;
				point.x /= SKILL_SIZE;

				if (pShortcut->m_pFromWnd == this)
				{
					LPSHORTCUT lpShortcut = (LPSHORTCUT)pShortcut->m_dwData;
					if (lpShortcut->m_dwData == 2)
					{
						RemoveSkillQueue(lpShortcut->m_dwIndex, FALSE);
						SetSkillQueue(point.x, pShortcut->m_dwType, pShortcut->m_dwId, pShortcut->m_pTexture);// == FALSE )
						return TRUE;
					}
				}
				if (SetSkillQueue(point.x, pShortcut->m_dwType, pShortcut->m_dwId, pShortcut->m_pTexture) == FALSE)
					SetForbid(TRUE);
				bForbid = FALSE;
			}
		}
		if (pShortcut->m_pFromWnd != this)
			SetForbid(bForbid);
	}

	if (pShortcut->m_pFromWnd == this)
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pShortcut->m_dwData;

		if (lpShortcut->m_dwData == 0)
			g_DPlay.SendRemoveAppletTaskBar((BYTE)(lpShortcut->m_dwIndex));
		else if (lpShortcut->m_dwData == 1)
			g_DPlay.SendRemoveItemTaskBar(m_nSlotIndex, (BYTE)(lpShortcut->m_dwIndex));
		else if (lpShortcut->m_dwData == 2)
		{
			RemoveSkillQueue(lpShortcut->m_dwIndex);
			return TRUE;
		}
		lpShortcut->Empty();
	}
	return TRUE;
}
LPSHORTCUT CWndTaskBar::Select(CPoint point)
{
	CRect rect = RECT_APPLET;
	LPSHORTCUT pShortcut = NULL;
	if (rect.PtInRect(point))
	{
		point.x -= rect.left;
		point.y -= rect.top;
		point.x /= ICON_SIZE;
		pShortcut = &m_aSlotApplet[point.x];
		if (!pShortcut->IsEmpty())
		{
			return pShortcut;
		}
	}
	rect = RECT_ITEM;
	if (rect.PtInRect(point))
	{
		point.x -= rect.left;
		point.y -= rect.top;
		point.x /= ICON_SIZE;
		pShortcut = &m_paSlotItem[point.x];
		if (!pShortcut->IsEmpty())
			return pShortcut;
	}

	rect = RECT_QUEUE;
	if (rect.PtInRect(point))
	{
		point.x -= rect.left;
		point.y -= rect.top;
		point.x /= SKILL_SIZE;
		pShortcut = &m_aSlotQueue[point.x];
		if (!pShortcut->IsEmpty())
			return pShortcut;

	}
	rect = CRect(POINT_QUEUE_X - ICON_SIZE - 5, POINT_QUEUE_Y, (POINT_QUEUE_X - ICON_SIZE - 5) + 32, POINT_QUEUE_Y + 32);
	if (rect.PtInRect(point))
	{
		m_aSlotSkill.m_dwShortcut = SHORTCUT_SKILLFUN;
		m_aSlotSkill.m_pTexture = m_pTexture;

		pShortcut = &m_aSlotSkill;

		if (!pShortcut->IsEmpty())
			return pShortcut;
	}

	return NULL;
}
void CWndTaskBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLButtonDown && IsPush())
	{
		if (m_pSelectShortcut && !m_pSelectShortcut->IsEmpty()) //&& m_pSelectShortcut->m_dwShortcut != SHORTCUT_SKILL )
		{
#if __VER >= 12 // __LORD
			if (m_pSelectShortcut->m_dwShortcut == SHORTCUT_LORDSKILL)
			{
				m_pSelectShortcut->m_dwId++;
			}
#endif
			memcpy(&m_GlobalShortcut, m_pSelectShortcut, sizeof(m_GlobalShortcut));
			m_GlobalShortcut.m_pFromWnd = this;
			m_GlobalShortcut.m_dwData = (DWORD)m_pSelectShortcut;
			m_pSelectShortcut = NULL;
		}
	}
	CWndNeuz::OnMouseMove(nFlags, point);
}

void CWndTaskBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (g_WndMng.m_pWndMenu && g_WndMng.m_pWndMenu->IsVisible())
		g_WndMng.m_pWndMenu->SetVisible(FALSE);

	m_pSelectShortcut = Select(point);
	SetCapture();
}
void CWndTaskBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	LPSHORTCUT pShortcut = Select(point);

	if (pShortcut && pShortcut == m_pSelectShortcut && !pShortcut->IsEmpty() && pShortcut->m_dwData != 2)
	{
		g_WndMng.ObjectExecutor(pShortcut);
	}

}

BOOL CWndTaskBar::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
void CWndTaskBar::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	//m_pTheme->RenderWndTaskBar( p2DRender, &rect );
	CWndBase::PaintFrame(p2DRender);
}

LPSKILL CWndTaskBar::GetCurrentSkillQueue()
{
	LPSKILL pSkill = NULL;
	LPSHORTCUT pShortcut = &m_aSlotQueue[m_nUsedSkillQueue];

	if (pShortcut && pShortcut->IsEmpty() == FALSE)
		pSkill = g_pPlayer->GetSkill(0, pShortcut->m_dwId);

	return pSkill;
}



BOOL CWndTaskBar::UseSkillQueue(CCtrl* pTargetObj)
{
	if (pTargetObj && pTargetObj->GetType() != OT_MOVER)
		return FALSE;

	if (!g_pPlayer || !g_pPlayer->IsPossibleSetCmd())
		return FALSE;

	if (pTargetObj)
	{
		if (pTargetObj->GetType() == OT_OBJ)
			m_idTarget = NULL_ID;
		else
			m_idTarget = ((CCtrl*)pTargetObj)->GetId();
	}
	else
		m_idTarget = NULL_ID;
	CMover* pTargetMover = (CMover*)pTargetObj;
	if (IsInvalidObj(pTargetMover))
		m_idTarget = NULL_ID;

	m_nUsedSkillQueue = 0;

	LPSHORTCUT pShortcut = &m_aSlotQueue[m_nUsedSkillQueue];

	if (pShortcut->IsEmpty() == FALSE)
	{
		LPSKILL pSkill = g_pPlayer->GetSkill(0, pShortcut->m_dwId);
		if (pSkill == NULL)
		{
			OnCancelSkill();
			return FALSE;
		}

		if (g_pPlayer->IsBullet(pSkill->GetProp()) == FALSE)
		{
			OnCancelSkill();
			return FALSE;
		}

		m_nExecute = 1;
		if (g_pPlayer->CMD_SetUseSkill(m_idTarget, pShortcut->m_dwId, SUT_QUEUESTART) == 0)
			OnCancelSkill();

		return TRUE;
	}
	else
		g_WndMng.m_pWndWorld->SetNextSkill(NEXTSKILL_NONE);

	return FALSE;
}

BOOL CWndTaskBar::Process(void)
{
	CPoint point = GetMousePoint();
	CRect wndRect = GetWindowRect();
	if (wndRect.PtInRect(point))
	{
		CRect rect = CRect(POINT_APPLET_X, POINT_APPLET_Y, POINT_APPLET_X + ICON_SIZE, POINT_APPLET_Y + ICON_SIZE);
		for (int i = 0; i < m_nMaxSlotApplet; i++)
		{
			LPSHORTCUT lpShortcut = &m_aSlotApplet[i];
			if (!lpShortcut->IsEmpty() && rect.PtInRect(point))
			{
				MotionProp* pMotionProp = prj.GetMotionProp(lpShortcut->m_dwId);

				if (IsShortcut(lpShortcut, SHORTCUT_MOTION, MOT_BASE_CHEER))
					PutTooTip(lpShortcut, point, &rect);
			}
			rect += CPoint(ICON_SIZE, 0);
		}

		rect = CRect(POINT_ITEM_X, POINT_ITEM_Y, POINT_ITEM_X + ICON_SIZE, POINT_ITEM_Y + ICON_SIZE);
		for (int i = 0; i < MAX_SLOT_ITEM; i++)
		{
			LPSHORTCUT lpShortcut = &m_paSlotItem[i];
			if (!lpShortcut->IsEmpty() && rect.PtInRect(point))
			{
				if (IsShortcut(lpShortcut, SHORTCUT_MOTION, MOT_BASE_CHEER))
					PutTooTip(lpShortcut, point, &rect);
			}
			rect += CPoint(ICON_SIZE, 0);
		}

		rect = CRect(POINT_QUEUE_X, POINT_QUEUE_Y, POINT_QUEUE_X + SKILL_SIZE, POINT_QUEUE_Y + SKILL_SIZE);
		for (int i = 0; i < MAX_SLOT_QUEUE; i++)
		{
			LPSHORTCUT lpShortcut = &m_aSlotQueue[i];
			if (!lpShortcut->IsEmpty() && rect.PtInRect(point))
			{
				if (IsShortcut(lpShortcut, SHORTCUT_MOTION, MOT_BASE_CHEER))
					PutTooTip(lpShortcut, point, &rect);
			}
			rect += CPoint(SKILL_SIZE, 0);
		}
	}
	if (m_nExecute)
	{
		if (m_idTarget != NULL_ID)
		{
			CMover* pObj = prj.GetMover(m_idTarget);
			if (IsInvalidObj(pObj) || pObj->IsDie())
			{
				OnCancelSkill();
			}
		}
	}
	if (m_GlobalShortcut.IsEmpty() == FALSE)
	{
		m_dwHighAlpha += 10;
		if (m_dwHighAlpha >= 256)
			m_dwHighAlpha = 0;
	}
	else
	{
		if (g_bKeyTable[VK_RETURN])
		{
			g_bKeyTable[VK_RETURN] = FALSE;
			CWndBase* pWndBase = GetWndBase(APP_COMMUNICATION_CHAT);
			if (pWndBase == NULL)
			{
				g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_COMMUNICATION_CHAT);
				CWndChat* pWndChat = (CWndChat*)GetWndBase(APP_COMMUNICATION_CHAT);
				if (pWndChat != NULL)
				{
					pWndChat->m_wndEdit.SetFocus();
				}
			}
		}
		else
		{
			AppletFunc* pApplet;
			DWORD dwIdApplet;
			POSITION pos = g_WndMng.m_mapAppletFunc.GetStartPosition();
			BOOL bKey = FALSE;
			while (pos)
			{
				g_WndMng.m_mapAppletFunc.GetNextAssoc(pos, dwIdApplet, (void*&)pApplet);

				if (g_bKeyTable[pApplet->m_cHotkey])
				{
					g_WndMng.ObjectExecutor(SHORTCUT_APPLET, pApplet->m_dwIdApplet);
					g_bKeyTable[pApplet->m_cHotkey] = FALSE;
				}
			}

			CWndRegVend* pWndRegVend = (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_SELL);
			CWndRegVend* pWndVendorBuy = (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_BUY);

			if (!pWndRegVend || !pWndVendorBuy)
			{
				DWORD* dwHotkey;
				DWORD dwHotkey1[9] = { 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78 };
				DWORD dwHotkey2[9] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
#ifdef __FL_UPDATE_TASKBAR
				if (!g_Option.m_bRekeyTask)
					dwHotkey = dwHotkey2;
				else
#endif
					dwHotkey = dwHotkey1;

				for (int i = 0; i < 9; i++)
				{
					if (g_bKeyTable[dwHotkey[i]] && !g_bSlotSwitchAboutEquipItem[i])
					{
						LPSHORTCUT lpShortcut = &m_paSlotItem[i];
						if (lpShortcut && lpShortcut->m_dwShortcut == SHORTCUT_ITEM)
						{
							CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(lpShortcut->m_nLocation);
							if (pContainer)
							{
								CItemElem* pItemBase = pContainer->GetAtId(lpShortcut->m_dwId);
								if (pItemBase)
								{
									ItemProp* pProp = pItemBase->GetProp();
									if (pProp)
									{
										int nPart = pProp->dwParts;
										if (nPart != -1)
											g_bSlotSwitchAboutEquipItem[i] = TRUE;
									}
								}
							}
						}
						g_WndMng.ObjectExecutor(lpShortcut);
						g_bKeyTable[dwHotkey[i]] = 0;
#ifdef __FL_UPDATE_TASKBAR
						m_dwHighAlphaClick[i] = 132;
#endif
					}
				}
			}
		}
	}

	UpdateItem();

	return CWndBase::Process();
}

#ifdef __FL_UPDATE_TASKBAR
void CWndTaskBar::DrawHotkeys(C2DRender* p2DRender, CPoint pt, CString str)
{
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontTaskbarKeys);

	CString tmpstr = str;
	if (g_Option.m_bRekeyTask)
	{
		tmpstr = "F";
		tmpstr += str;
	}
	else
		pt.x += 3;

	p2DRender->TextOut(pt.x + 7, pt.y, tmpstr, 0xffffffff);
	p2DRender->SetFont(pOldFont);
}
#endif

void CWndTaskBar::OnCancelSkill(void)
{
	if (!g_pPlayer)
		return;

	if (g_pPlayer->m_dwReqFlag & REQ_USESKILL)
		return;

	m_nExecute = 0;
	m_nUsedSkillQueue = 0;
	g_pPlayer->ClearDestObj();
	g_WndMng.m_pWndWorld->SetNextSkill(NEXTSKILL_NONE);
	g_pPlayer->ClearCmd();
}


CWndTaskMenu::CWndTaskMenu()
{
	m_pMenu1 = NULL;
	m_pMenu2 = NULL;
	m_pMenu3 = NULL;
	m_pMenu4 = NULL;
}

CWndTaskMenu::~CWndTaskMenu()
{
	SAFE_DELETE(m_pMenu1);
	SAFE_DELETE(m_pMenu2);
	SAFE_DELETE(m_pMenu3);
	SAFE_DELETE(m_pMenu4);
}
void CWndTaskMenu::OnDraw(C2DRender* p2DRender)
{
	return;
}
BOOL CWndTaskMenu::Process()
{
	if (CWndBase::m_GlobalShortcut.m_dwShortcut != SHORTCUT_NONE)
		return CWndBase::Process();

	if (IsVisible() == FALSE)
		return CWndBase::Process();

	for (int i = 0; i < m_awndMenuItem.GetSize(); i++)
	{
		BOOL bHighLight = FALSE;
		CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt(i);
		if (pWndButton->GetClientRect(TRUE).PtInRect(m_ptMouse))
		{
			if (pWndButton->m_pWndMenu == NULL || pWndButton->m_pWndMenu->IsVisible() == FALSE)
			{
				for (int i2 = 0; i2 < m_awndMenuItem.GetSize(); i2++)
				{
					if (((CWndButton*)m_awndMenuItem.GetAt(i2))->m_pWndMenu)
						((CWndButton*)m_awndMenuItem.GetAt(i2))->m_pWndMenu->SetVisibleSub(FALSE);
				}
			}
			if (pWndButton->m_pWndMenu)
			{
				if (pWndButton->m_pWndMenu->IsVisible() == FALSE)
				{
					CRect rect = pWndButton->GetScreenRect();
					pWndButton->m_pWndMenu->Move(CPoint(rect.right, rect.top));
				}
				CRect rcButton = pWndButton->GetScreenRect();
				pWndButton->m_pWndMenu->Move(CPoint(rcButton.right, rcButton.top));
				CRect rcMenu = pWndButton->m_pWndMenu->GetScreenRect();
				CRect rcLayout = m_pWndRoot->GetLayoutRect();
				CPoint pt = rcMenu.TopLeft();
				if (rcMenu.right > rcLayout.right)
				{
					pt.x = rcButton.left - rcMenu.Width();
				}
				if (rcMenu.bottom > rcLayout.bottom)
				{
					pt.y -= rcMenu.bottom - rcLayout.bottom;
				}
				pWndButton->m_pWndMenu->Move(pt);
				pWndButton->m_pWndMenu->SetVisible(TRUE);
				pWndButton->m_pWndMenu->SetFocus();
			}
		}
	}
	return CWndBase::Process();
}
void CWndTaskMenu::SetTexture(CWndButton* pWndButton)
{
	pWndButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, g_WndMng.GetAppletFunc(pWndButton->GetWndId())->m_pszIconName));
}

void CWndTaskMenu::OnInitialUpdate()
{
	CWndMenu::OnInitialUpdate();

	CWndButton* pWndButton;

	if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		pWndButton = AppendMenu(this, 0, APP_DEBUGINFO, GETTEXT(TID_APP_DEBUGINFO));

	// Classic Applets
	CWndButton* pWndButton1 = AppendMenu(this, 0, 0, GETTEXT(TID_APP_APPLET)); pWndButton1->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, _T("Icon_Hunter_BaseFolder.tga")));
	m_pMenu1 = new CWndMenu;
	m_pMenu1->CreateMenu(this);
	pWndButton = AppendMenu(m_pMenu1, 0, APP_STATUS1, GETTEXT(TID_APP_STATUS));
	pWndButton = AppendMenu(m_pMenu1, 0, APP_NAVIGATOR, GETTEXT(TID_APP_NAVIGATOR));
	pWndButton = AppendMenu(m_pMenu1, 0, APP_CHARACTER3, GETTEXT(TID_APP_CHARACTER));
	pWndButton = AppendMenu(m_pMenu1, 0, APP_INVENTORY, GETTEXT(TID_APP_INVENTORY));
	pWndButton = AppendMenu(m_pMenu1, 0, APP_HOUSING, GETTEXT(TID_GAME_HOUSING_BOX));
	pWndButton = AppendMenu(m_pMenu1, 0, APP_SKILL4, GETTEXT(TID_APP_SKILL));
	pWndButton = AppendMenu(m_pMenu1, 0, APP_QUEST_EX_LIST, GETTEXT(TID_APP_QUEST));
	pWndButton = AppendMenu(m_pMenu1, 0, APP_MOTION, GETTEXT(TID_APP_MOTION));
	pWndButton = AppendMenu(m_pMenu1, 0, APP_MAP_EX, GETTEXT(TID_APP_MAP));
	pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_APPLET; SetTexture(pWndButton);
	pWndButton1->SetMenu(m_pMenu1);

	// Communication
	CWndButton* pWndButton2 = AppendMenu(this, 0, 0, GETTEXT(TID_APP_COMMUNICATION)); pWndButton2->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, _T("Icon_Hunter_BaseFolder.tga")));
	m_pMenu2 = new CWndMenu;
	m_pMenu2->CreateMenu(this);
	pWndButton = AppendMenu(m_pMenu2, 0, APP_COMMUNICATION_CHAT, GETTEXT(TID_APP_COMMUNICATION_CHAT));
	pWndButton = AppendMenu(m_pMenu2, 0, APP_MESSENGER_, GETTEXT(TID_APP_MESSENGER));
	pWndButton = AppendMenu(m_pMenu2, 0, APP_COUPLE_MAIN, GETTEXT(TID_GAME_COUPLE));
	pWndButton = AppendMenu(m_pMenu2, 0, APP_PARTY, GETTEXT(TID_APP_PARTY));
	pWndButton = AppendMenu(m_pMenu2, 0, APP_GUILD, GETTEXT(TID_APP_COMPANY));
	pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_APPLET; SetTexture(pWndButton);
	pWndButton2->SetMenu(m_pMenu2);

	// Helper
	CWndButton* pWndButton3 = AppendMenu(this, 0, 0, GETTEXT(TID_APP_HELPER)); pWndButton3->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, _T("Icon_Hunter_BaseFolder.tga")));
	m_pMenu3 = new CWndMenu;
	m_pMenu3->CreateMenu(this);
	pWndButton = AppendMenu(m_pMenu3, 0, APP_HELPER_HELP, GETTEXT(TID_APP_HELPER_HELP));
	pWndButton = AppendMenu(m_pMenu3, 0, APP_HELPER_TIP, GETTEXT(TID_APP_HELPER_TIP));
	pWndButton = AppendMenu(m_pMenu3, 0, APP_HELPER_FAQ, GETTEXT(TID_APP_HELPER_FAQ));
	pWndButton = AppendMenu(m_pMenu3, 0, APP_INFO_NOTICE, GETTEXT(TID_APP_INFO_NOTICE));
	pWndButton = AppendMenu(m_pMenu3, 0, APP_COMM_ITEM, GETTEXT(TID_APP_ITEMTIME));
	pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_APPLET; SetTexture(pWndButton);
	pWndButton3->SetMenu(m_pMenu3);

	// Custom Content
	CWndButton* pWndButton4 = AppendMenu(this, 0, 0, "Custom"); pWndButton4->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, _T("Icon_Hunter_BaseFolder.tga")));
	m_pMenu4 = new CWndMenu;
	m_pMenu4->CreateMenu(this);
#ifdef __FL_FAST_ELEMENT
	pWndButton = AppendMenu(m_pMenu4, 0, APP_FAST_ELEMENT, "Element Changer");
#endif
#ifdef __FL_SHOP_SEARCH
	pWndButton = AppendMenu(m_pMenu4, 0, APP_SHOP_SEARCH, "Shop Search");
#endif
#ifdef __WIKI_ITEMS_V2
	pWndButton = AppendMenu(m_pMenu4, 0, APP_WIKI_ITEMS, "Wiki - Items");
#endif
#ifdef __FL_TELEPORT
	pWndButton = AppendMenu(m_pMenu4, 0, APP_TELEPORT, "Teleporter");
#endif
#ifdef __FL_DPS_DISPLAY
	pWndButton = AppendMenu(m_pMenu4, 0, APP_DPS_DISPLAY, "DPS Display");
#endif
#ifdef __FL_BATTLE_PASS
	pWndButton = AppendMenu(m_pMenu4, 0, APP_BATTLE_PASS, "Battle Pass");
#endif
#ifdef __FL_PET_FILTER
	pWndButton = AppendMenu(m_pMenu4, 0, APP_PET_LOOT_OPTION, "Pet Filter");
#endif
#ifdef __FL_CONSUMABLES_STORAGE
	pWndButton = AppendMenu(m_pMenu4, 0, APP_CHEST, "Chest");
#endif
#ifdef __FL_RECYCLE
	pWndButton = AppendMenu(m_pMenu4, 0, APP_RECYCLE, "Recycle");
#endif
#ifdef __FL_PARTY_FINDER
	pWndButton = AppendMenu(m_pMenu4, 0, APP_PARTY_FINDER, "Party-Finder");
#endif
#ifdef __FL_VENDORS
	pWndButton = AppendMenu(m_pMenu4, 0, APP_VENDOR_MANAGE, "Offline Vendor Menu");
#endif
#ifdef __FL_CASH_SHOP
	pWndButton = AppendMenu(m_pMenu4, 0, APP_CASH_SHOP, "FlyFF Cash Shop");
#endif
#ifdef __FL_SWITCH_EQ
	pWndButton = AppendMenu(m_pMenu4, 0, APP_EQ_SWITCH, "Switch Equipment");
#endif
#ifdef __FL_GUILD_FINDER
	pWndButton = AppendMenu(m_pMenu4, 0, APP_GUILD_FINDER, "Guild Finder");
#endif
#ifdef __FL_FARM_DISPLAY
	pWndButton = AppendMenu(m_pMenu4, 0, APP_FARM_DISPLAY, "Farm Display");
#endif
#ifdef __FL_PACKED_ITEMS
	pWndButton = AppendMenu(m_pMenu4, 0, APP_PACKED_ITEMS, "Preview Custom Package");
#endif
#ifdef __FL_RECORD_BOOK
	pWndButton = AppendMenu(m_pMenu4, 0, APP_RECORD_BOOK, "Record Book");
#endif
#ifdef __FL_MONSTER_ALBUM
	pWndButton = AppendMenu(m_pMenu4, 0, APP_MONSTER_ALBUM, "Monster Album");
#endif
#ifdef __FL_TDM
	pWndButton = AppendMenu(m_pMenu4, 0, APP_PVP_ACTIVITIES, "PvP & PvE Activities");
#endif
#ifdef __AI_SYSTEM
	pWndButton = AppendMenu(m_pMenu4, 0, APP_REFRESHER, "Test");
#endif

	pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_APPLET; SetTexture(pWndButton);
	pWndButton4->SetMenu(m_pMenu4);

	// Single things
	pWndButton = AppendMenu(this, 0, APP_VENDOR_REVISION, GETTEXT(TID_APP_VENDOR));
	pWndButton = AppendMenu(this, 0, APP_OPTIONEX, GETTEXT(TID_APP_OPTION));
	pWndButton = AppendMenu(this, 0, APP_LOGOUT, GETTEXT(TID_APP_LOGOUT));
	pWndButton = AppendMenu(this, 0, APP_QUIT, GETTEXT(TID_APP_QUIT));


	CWndBase::SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("WndTaskMenu.tga")), TRUE);

	for (int i = 0; i < m_awndMenuItem.GetSize(); i++)
	{
		CWndButton* pWndButton = GetMenuItem(i);
		pWndButton->SetWndRect(CRect(10, 50 + (i * 22), m_pTexture->m_size.cx - 20, 50 + 20 + (i * 22)));
	}
	SetWndRect(CRect(0, 0, m_pTexture->m_size.cx, m_pTexture->m_size.cy));

}
CWndButton* CWndTaskMenu::AppendMenu(CWndMenu* pWndMenu, UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem)
{
	CWndButton* pWndButton = (CWndButton*)pWndMenu->AppendMenu(nFlags, nIDNewItem, lpszNewItem);
	AppletFunc* pAppletFunc = g_WndMng.GetAppletFunc(nIDNewItem);
	if (pAppletFunc)
	{
		pWndButton->m_cHotkey = pAppletFunc->m_cHotkey;
		CString string;
		if (pAppletFunc->m_cHotkey == 0)
			string.Format("%s", pAppletFunc->m_pAppletDesc);
		else
			string.Format("%s\n[%s %c]", pAppletFunc->m_pAppletDesc, prj.GetText(TID_GAME_TOOLTIP_HOTKEY), pAppletFunc->m_cHotkey);
		pWndButton->m_strToolTip = string;
		pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_APPLET;
		SetTexture(pWndButton);
	}

	return pWndButton;
}

BOOL CWndTaskMenu::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CreateMenu(&g_WndMng);
	SetVisible(TRUE);
	return TRUE;
}

void CWndTaskMenu::OnKillFocus(CWndBase* pNewWnd)
{
	if (g_WndMng.m_pWndTaskBar != pNewWnd)
		CWndMenu::OnKillFocus(pNewWnd);
}

BOOL CWndTaskMenu::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return TRUE;
}
BOOL CWndTaskMenu::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
		g_WndMng.ObjectExecutor(SHORTCUT_APPLET, nID);
	return TRUE;
}
void CWndTaskMenu::OnSize(UINT nType, int cx, int cy)
{
	CWndMenu::OnSize(nType, cx, cy);
}
void CWndTaskMenu::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndTaskMenu::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;
}

void CWndTaskMenu::PaintFrame(C2DRender* p2DRender)
{
	CWndBase::PaintFrame(p2DRender);
}
BOOL CWndTaskMenu::OnEraseBkgnd(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	rect.top += 50;
	return CWndBase::OnEraseBkgnd(p2DRender);
}

void CWndTaskBar::RenderCollTime(CPoint pt, DWORD dwSkillId, C2DRender* p2DRender)
{
	LPSKILL lpSkill = &g_pPlayer->m_aJobSkill[dwSkillId];
	if (lpSkill)
	{
		DWORD dwDelay = g_pPlayer->GetReuseDelay(dwSkillId);
		if (dwDelay > 0)
		{
			ItemProp* pSkillProp = lpSkill->GetProp();
			ASSERT(pSkillProp);
			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, lpSkill->dwLevel);
			ASSERT(pAddSkillProp);
			RenderRadar(p2DRender, pt,
				pAddSkillProp->dwCooldown - dwDelay,
				pAddSkillProp->dwCooldown);
		}
	}
}

void CWndTaskBar::RenderLordCollTime(CPoint pt, DWORD dwSkillId, C2DRender* p2DRender)
{
	CCLord* pLord = CCLord::Instance();
	CLordSkillComponentExecutable* pComponent = pLord->GetSkills()->GetSkill(dwSkillId);
	if (!pComponent) return;
	DWORD dwDelay = 0;
	if (pComponent->GetTick() > 0)
		RenderRadar(p2DRender, pt, pComponent->GetCooltime() - pComponent->GetTick(), pComponent->GetCooltime());
}

void CWndTaskBar::RenderOutLineLamp(int x, int y, int num, DWORD size)
{
	CPoint Point = CPoint(x, y);
	LONG   thick = 10;

	D3DXCOLOR dwColorDest2 = D3DCOLOR_ARGB(0, 40, 0, 255);

	D3DXCOLOR dwColor = D3DCOLOR_ARGB(196, 40, 0, 255);

	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB(0, 255, 255, 255);
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB(0, 255, 255, 255);

	static BOOL  bReverse = FALSE;
	static FLOAT fLerp = 0.0f;

	if (bReverse)
		fLerp -= 0.05f;
	else
		fLerp += 0.05f;

	if (fLerp > 1.0f)
	{
		bReverse = TRUE;
		fLerp = 1.0f;
	}
	else if (fLerp < 0.0f)
	{
		bReverse = FALSE;
		fLerp = 0.0f;
	}

	D3DXColorLerp(&dwColor2, &dwColor, &dwColorDest2, fLerp);

	CRect Rect = CRect(0, 0, (size * num), size);

	m_p2DRender->RenderFillRect(CRect((Point.x + Rect.left) - thick, Point.y + Rect.top, (Point.x + Rect.left), Point.y + Rect.bottom),
		dwColor1, dwColor2, dwColor1, dwColor2);

	m_p2DRender->RenderFillRect(CRect((Point.x + Rect.left), (Point.y + Rect.top) - thick, (Point.x + Rect.right), Point.y + Rect.top),
		dwColor1, dwColor1, dwColor2, dwColor2);

	m_p2DRender->RenderFillRect(CRect((Point.x + Rect.right), Point.y + Rect.top, (Point.x + Rect.right) + thick, Point.y + Rect.bottom),
		dwColor2, dwColor1, dwColor2, dwColor1);

	m_p2DRender->RenderFillRect(CRect((Point.x + Rect.left), (Point.y + Rect.bottom), (Point.x + Rect.right), (Point.y + Rect.bottom) + thick),
		dwColor2, dwColor2, dwColor1, dwColor1);
}

BOOL CWndTaskBar::IsShortcut(LPSHORTCUT lpShortcut, DWORD dwShortcut, DWORD dwId)
{
	if (lpShortcut->m_dwShortcut == SHORTCUT_APPLET)
	{
	}
	else if (lpShortcut->m_dwShortcut == SHORTCUT_SKILL)
	{
	}
	else if (lpShortcut->m_dwShortcut == SHORTCUT_ITEM)
	{
	}
	else if (lpShortcut->m_dwShortcut == SHORTCUT_CHAT)
	{
	}
	else if (lpShortcut->m_dwShortcut == SHORTCUT_MOTION)
	{
		if (lpShortcut->m_dwShortcut == dwShortcut)
		{
			MotionProp* pMotionProp = prj.GetMotionProp(dwId);

			if (pMotionProp)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

#ifdef __FL_UPDATE_TASKBAR
void CWndTaskBar::RenderClicks(C2DRender* p2DRender)
{
	for (int i = 0; i < MAX_SLOT_ITEM; i++)
	{
		if (m_dwHighAlphaClick[i] > 0)
		{
			DWORD dwColor = D3DCOLOR_ARGB(m_dwHighAlphaClick[i], 0, 0, 255);
			LPWNDCTRL pItem = GetWndCtrl(WIDC_CUSTOM2);
			CRect rectItem = pItem->rect;
			rectItem.top = 43;
			rectItem.bottom = 48;
			rectItem.left = rectItem.left + i * ICON_SIZE;
			rectItem.right = rectItem.left + ICON_SIZE;
			p2DRender->RenderFillRect(rectItem, dwColor);

			m_dwHighAlphaClick[i] -= 3;
		}
	}
}
#endif