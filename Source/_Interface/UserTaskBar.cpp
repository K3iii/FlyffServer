// UserTaskBar.cpp: implementation of the CUserTaskBar class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserTaskBar.h"
#ifdef __WORLDSERVER
#include "User.h"
#endif // __WORLDSERVER

////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////

CUserTaskBar::CUserTaskBar()
{
	InitTaskBar();
}

CUserTaskBar::~CUserTaskBar()
{

}

void CUserTaskBar::InitTaskBar()
{
	memset(m_aSlotApplet, 0, sizeof(m_aSlotApplet));
	memset(m_aSlotItem, 0, sizeof(m_aSlotItem));
	memset(m_aSlotQueue, 0, sizeof(m_aSlotQueue));
	m_nActionPoint = 0;
	m_nUsedSkillQueue = -1;
}

void CUserTaskBar::InitTaskBarShorcutKind(DWORD dwShortcutKind)
{
	for (int i = 0; i < MAX_SLOT_APPLET; i++)
	{
		if (m_aSlotApplet[i].m_dwShortcut == dwShortcutKind)
			memset(&m_aSlotApplet[i], 0, sizeof(SHORTCUT));
	}

	for (int i = 0; i < MAX_SLOT_ITEM_COUNT; i++)
	{
		for (int j = 0; j < MAX_SLOT_ITEM; j++)
		{
			if (m_aSlotItem[i][j].m_dwShortcut == dwShortcutKind)
			{
				memset(&m_aSlotItem[i][j], 0, sizeof(SHORTCUT));
			}
		}
	}

	for (int i = 0; i < MAX_SLOT_QUEUE; i++)
	{
		if (m_aSlotQueue[i].m_dwShortcut == dwShortcutKind)
		{
			memset(&m_aSlotQueue[i], 0, sizeof(SHORTCUT));
		}
	}
}

void CUserTaskBar::Serialize(CAr& ar)
{
	if (ar.IsStoring())
	{
		int nCount = 0;
		u_long uOffset = ar.GetOffset();
		ar << nCount;
		for (int i = 0; i < MAX_SLOT_APPLET; i++)
		{
			if (m_aSlotApplet[i].m_dwShortcut != SHORTCUT_NONE)
			{
				ar << i;
				ar << m_aSlotApplet[i].m_dwShortcut << m_aSlotApplet[i].m_dwId << m_aSlotApplet[i].m_dwType;
				ar << m_aSlotApplet[i].m_dwIndex << m_aSlotApplet[i].m_dwUserId << m_aSlotApplet[i].m_dwData;
				ar << m_aSlotApplet[i].m_nLocation;
				if (m_aSlotApplet[i].m_dwShortcut == SHORTCUT_CHAT)
					ar.WriteString(m_aSlotApplet[i].m_szString);
				nCount++;
			}
		}
		int nBufSize;
		LPBYTE lpBuf = ar.GetBuffer(&nBufSize);
		*(UNALIGNED int*)(lpBuf + uOffset) = nCount;

		nCount = 0;
		uOffset = ar.GetOffset();
		ar << nCount;
		for (int i = 0; i < MAX_SLOT_ITEM_COUNT; i++)
		{
			for (int j = 0; j < MAX_SLOT_ITEM; j++)
			{
				if (m_aSlotItem[i][j].m_dwShortcut != SHORTCUT_NONE)
				{
					ar << i << j;
					ar << m_aSlotItem[i][j].m_dwShortcut << m_aSlotItem[i][j].m_dwId << m_aSlotItem[i][j].m_dwType;
					ar << m_aSlotItem[i][j].m_dwIndex << m_aSlotItem[i][j].m_dwUserId << m_aSlotItem[i][j].m_dwData;
					ar << m_aSlotItem[i][j].m_nLocation;
					if (m_aSlotItem[i][j].m_dwShortcut == SHORTCUT_CHAT)
						ar.WriteString(m_aSlotItem[i][j].m_szString);
					nCount++;

				}
			}
		}
		lpBuf = ar.GetBuffer(&nBufSize);
		*(UNALIGNED int*)(lpBuf + uOffset) = nCount;

		nCount = 0;
		uOffset = ar.GetOffset();
		ar << nCount;
		for (int i = 0; i < MAX_SLOT_QUEUE; i++)
		{
			if (m_aSlotQueue[i].m_dwShortcut != SHORTCUT_NONE)
			{
				ar << i;
				ar << m_aSlotQueue[i].m_dwShortcut << m_aSlotQueue[i].m_dwId << m_aSlotQueue[i].m_dwType;
				ar << m_aSlotQueue[i].m_dwIndex << m_aSlotQueue[i].m_dwUserId << m_aSlotQueue[i].m_dwData;
				ar << m_aSlotQueue[i].m_nLocation;
				nCount++;
			}
		}
		ar << m_nActionPoint;
		lpBuf = ar.GetBuffer(&nBufSize);
		*(UNALIGNED int*)(lpBuf + uOffset) = nCount;
	}
	else
	{
		memset(m_aSlotApplet, 0, sizeof(m_aSlotApplet));
		memset(m_aSlotItem, 0, sizeof(m_aSlotItem));
		memset(m_aSlotQueue, 0, sizeof(m_aSlotQueue));

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
		}

		ar >> nCount;
		for (int i = 0; i < nCount; i++)
		{
			ar >> nIndex;
			ar >> m_aSlotQueue[nIndex].m_dwShortcut >> m_aSlotQueue[nIndex].m_dwId >> m_aSlotQueue[nIndex].m_dwType;
			ar >> m_aSlotQueue[nIndex].m_dwIndex >> m_aSlotQueue[nIndex].m_dwUserId >> m_aSlotQueue[nIndex].m_dwData;
			ar >> m_aSlotQueue[nIndex].m_nLocation;
		}
		ar >> m_nActionPoint;
	}
}

#ifdef __WORLDSERVER
void CUserTaskBar::OnEndSkillQueue(CUser* pUser)
{
	m_nUsedSkillQueue = -1;
	pUser->AddHdr(GETID(pUser), SNAPSHOTTYPE_ENDSKILLQUEUE);
	((CMover*)pUser)->ClearCmd();
}

int	CUserTaskBar::SetNextSkill(CUser* pUser)
{
	m_nUsedSkillQueue++;

	if (m_nUsedSkillQueue < 0 || m_nUsedSkillQueue >= MAX_SLOT_QUEUE)
	{
		OnEndSkillQueue(pUser);
		return 0;
	}

	int nAP = m_nActionPoint;

	if (!(pUser->IsSMMode(SM_ACTPOINT)))
	{
		switch (m_nUsedSkillQueue)
		{
		case 1:	nAP -= 6;	break;
		case 2:	nAP -= 8;	break;
		case 3:	nAP -= 11;	break;
		case 4:	nAP -= 30;	break;
		}
	}

	LPSHORTCUT pShortcut = &m_aSlotQueue[m_nUsedSkillQueue];
	BOOL bResult = nAP < 0;
	if (pShortcut->IsEmpty() || nAP < 0)
	{
		OnEndSkillQueue(pUser);
		return 0;
	}
	else
	{
		if (nAP < 0)	
			nAP = 0;
		m_nActionPoint = nAP;
		pUser->AddSetActionPoint(nAP);
		OBJID idTarget = pUser->m_idSetTarget;

		if (pUser->CMD_SetUseSkill(idTarget, pShortcut->m_dwId, SUT_QUEUEING) == 0)
			SetNextSkill(pUser);

		return 1;
	}

	return 1;
}
#endif // __WORLDSERVER