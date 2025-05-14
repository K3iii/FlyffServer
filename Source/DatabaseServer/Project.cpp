#include "stdafx.h"
#include "defineObj.h"
#include "project.h"
//#include "company.h"
#include "lang.h"

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

#include "guild.h"
extern	CGuildMng	g_GuildMng;
//extern	CCompanyMng		g_CompanyMng;
#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

#if __VER >= 12 // __LORD
#include "tlord.h"
#endif	// __LORD

#if __VER >= 13 // __HONORABLE_TITLE
#include "honor.h"
#endif

CProject::CProject()
{
	memset(m_apszWorld, 0, sizeof(TCHAR) * 64 * MAX_WORLD);
	m_nMoverPropSize = 0;
	m_pPropMover = new MoverProp[MAX_PROPMOVER];

	m_fItemDropRate = 1.0f;
	m_fGoldDropRate = 1.0f;
	m_fMonsterExpRate = 1.0f;
	m_fMonsterHitRate = 1.0f;
	m_fShopCost = 1.0f;
#ifdef __S1108_BACK_END_SYSTEM
	m_fMonsterRebirthRate = 1.0f;
	m_fMonsterHitpointRate = 1.0f;
	m_fMonsterAggressiveRate = 0.2f;
	m_fMonsterRespawnRate = 1.0f;
	m_bBaseGameSetting = FALSE;
	m_nMonsterRespawnSize = 0;
	m_nMonsterPropSize = 0;
	m_nAddMonsterRespawnSize = 0;
	m_nAddMonsterPropSize = 0;
	m_nRemoveMonsterPropSize = 0;
	m_bBackEndSystem = FALSE;
	ZeroMemory(m_chBackEndSystemTime, sizeof(m_chBackEndSystemTime));
	ZeroMemory(m_chBackEndSystemChatTime, sizeof(m_chBackEndSystemChatTime));
	ZeroMemory(m_chGMChat, sizeof(m_chGMChat));
#endif // __S1108_BACK_END_SYSTEM
}

CProject::~CProject()
{
	for (CMapStrToPtr::iterator i = m_mapBeginPos.begin(); i != m_mapBeginPos.end(); ++i)
		SAFE_DELETE_CONST((BEGINPOSARR*)i->second);

	SAFE_DELETE_ARRAY(m_pPropMover);
	for (int j = 0; j < m_colorText.GetSize(); j++)
	{
		tagColorText* lpText = m_colorText.GetAt(j);
		if (lpText && lpText->lpszData)
			free(lpText->lpszData);
	}
#ifdef __RULE_0615
	m_sAllowedLetterAuth.clear();
	m_sAllowedLetterGame.clear();
#endif	// __RULE_0615
}

int SortJobSkill(const void* arg1, const void* arg2)
{
	ItemProp* pItemProp1 = *(ItemProp**)arg1;
	ItemProp* pItemProp2 = *(ItemProp**)arg2;
	if (pItemProp1->dwReqDisLV < pItemProp2->dwReqDisLV)
		return -1;
	if (pItemProp1->dwReqDisLV >= pItemProp2->dwReqDisLV)
		return 1;
	return 0;
}

BOOL CProject::OpenProject(LPCTSTR lpszFileName)
{
	CScanner s;
	if (s.Load(lpszFileName) == FALSE)
		return FALSE;

	LoadPreFiles();

	do {
		s.GetToken();

		if (s.Token == _T("propItem"))
		{
			s.GetToken();
			LoadPropItem(s.token, &m_aPropItem);
		}
		else if (s.Token == _T("propMover"))
		{
			s.GetToken();
			LoadPropMover(s.token);
		}
		else if (s.Token == _T("propSkill"))
		{
			s.GetToken();
			LoadPropSkill(s.token, &m_aPropSkill);
			memset(m_aJobSkillNum, 0, sizeof(m_aJobSkillNum));
			for (int i = 1; i < m_aPropSkill.GetSize(); i++)
			{
				ItemProp* pItemProp = (ItemProp*)m_aPropSkill.GetAt(i);
				if (pItemProp)
				{
					if (pItemProp->dwItemKind2 == -1)
					{
						Error("ItmeProp(%s) dwItemKind2 = -1", pItemProp->szName);
					}

					if (pItemProp->dwItemKind1 == JTYPE_COMMON)
						continue;

					ItemProp** apJobSkill = m_aJobSkill[pItemProp->dwItemKind2];
					apJobSkill[m_aJobSkillNum[pItemProp->dwItemKind2]] = pItemProp;
					m_aJobSkillNum[pItemProp->dwItemKind2]++;
				}
				else
				{
					TRACE("PropSkill Prop NULL SkillIndex = %d now no problem\n", i);
				}
			}
			for (int i = 0; i < MAX_JOB; i++)
			{
				ItemProp** apJobSkill = m_aJobSkill[i];
				qsort((void*)apJobSkill, (size_t)m_aJobSkillNum[i], sizeof(ItemProp*), SortJobSkill);
			}
		}
		else if (s.Token == _T("world"))
		{
			s.GetToken();
			LoadDefOfWorld(s.token);
		}
		else if (s.Token == _T("expTable"))
		{
			s.GetToken();
			LoadExpTable(s.token);
		}
	} while (s.tok != FINISHED);

	LoadBeginPos();
	LoadJobItem(_T("jobItem.inc"));

	m_EventLua.LoadScript();
	m_EventLua.CheckEventState();

#ifdef __RULE_0615
	LoadAllowedLetter();
#endif	// __RULE_0615

	CPetProperty::GetInstance()->LoadScript("pet.inc");
	CTax::GetInstance()->LoadScript();
	CTitleManager::Instance()->LoadTitle("honorList.txt");
	CCoupleHelper::Instance()->Initialize();

	return TRUE;
}

void CProject::LoadBeginPos(void)
{
	WIN32_FIND_DATA ffdFoundData;
	HANDLE hFound;
	CHAR lpFileName[MAX_PATH];

	BOOL bDoneWithHandle = FALSE;
	hFound = FindFirstFile(MakePath(DIR_WORLD, "*.*"), (LPWIN32_FIND_DATA)&ffdFoundData);

	if ((HANDLE)(-1) == hFound)
		bDoneWithHandle = TRUE;

	D3DXVECTOR3 vPos;
	DWORD dwIndex;

	while (!bDoneWithHandle)
	{
		if ((FILE_ATTRIBUTE_DIRECTORY & ffdFoundData.dwFileAttributes) && (0 != strcmp(".", ffdFoundData.cFileName)) && (0 != strcmp("..", ffdFoundData.cFileName)))
		{
			sprintf(lpFileName, "%s%s\\%s.rgn", DIR_WORLD, ffdFoundData.cFileName, ffdFoundData.cFileName);
			CScanner s;
			if (s.Load(lpFileName) == TRUE)
			{
				s.GetToken();
				while (s.tok != FINISHED)
				{
					if (s.Token == _T("region") || s.Token == _T("region2") || s.Token == _T("region3"))
					{
						BOOL bNewFormat = FALSE;
						BOOL bNewFormat3 = FALSE;
						if (s.Token == _T("region2"))
							bNewFormat = TRUE;
						if (s.Token == _T("region3"))
						{
							bNewFormat3 = TRUE;
							bNewFormat = TRUE;
						}

						s.GetNumber();	// type
						dwIndex = s.GetNumber();
						vPos.x = s.GetFloat();
						vPos.y = s.GetFloat();
						vPos.z = s.GetFloat();
						s.GetNumber();	s.GetNumber(); s.GetToken();	s.GetToken();	s.GetToken();	// attribute, id, script, music, sound
						s.GetToken();	s.GetFloat();	s.GetFloat();	s.GetFloat();	// teleport
						s.GetNumber();	s.GetNumber();	s.GetNumber();	s.GetNumber();	// rect
						s.GetToken();	s.GetNumber();	// key, target
						if (bNewFormat3)
						{
							s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber();
							s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber();
							s.GetNumber();
						}

						if (bNewFormat == FALSE)
						{
							char cbDesc = s.GetNumber();	// size
							for (int i = 0; i < cbDesc; i++)
								s.GetToken();	// desc
						}
						else
						{
							s.GetToken(); // get "title"
							BOOL bDesc = s.GetNumber();
							if (bDesc)
							{
								CString string;
								s.GetToken(); // {
								do
									s.GetToken();
								while (*s.token != '}');
							}
							s.GetToken(); // get "desc"
							bDesc = s.GetNumber();
							if (bDesc)
							{
								CString string;
								s.GetToken(); // {
								do
									s.GetToken();
								while (*s.token != '}');
							}
						}
						if (RI_BEGIN == dwIndex)
						{
							strlwr(ffdFoundData.cFileName);
							AddBeginPos(ffdFoundData.cFileName, vPos);
						}
					}
					else if (s.Token == _T("respawn"))
					{
						// type, index, x, y, z, cb, time, left, top, right, bottom
						s.GetNumber();	s.GetNumber();	s.GetFloat();	s.GetFloat();	s.GetFloat();	s.GetNumber();	s.GetNumber();
						s.GetNumber();	// m_nActiveAttackNum
						s.GetNumber();	s.GetNumber();	s.GetNumber();	s.GetNumber();
					}
					s.GetToken();
				}
			}
		}
		bDoneWithHandle
			= !FindNextFile(hFound, (LPWIN32_FIND_DATA)&ffdFoundData);
	}
	FindClose(hFound);
}

void CProject::AddBeginPos(const CHAR* lpszWorld, const D3DXVECTOR3& vPos)
{


	CMapStrToPtr::iterator i = m_mapBeginPos.find(lpszWorld);
	if (i == m_mapBeginPos.end())
	{
		LPBEGINPOSARR lpBeginPosArr = new BEGINPOSARR;
		lpBeginPosArr->uSize = 0;
		lpBeginPosArr->avPos[lpBeginPosArr->uSize++] = vPos;
		m_mapBeginPos[lpszWorld] = (void*)lpBeginPosArr;
	}
	else
	{
		LPBEGINPOSARR lpBeginPosArr = (LPBEGINPOSARR)i->second;
		lpBeginPosArr->avPos[lpBeginPosArr->uSize++] = vPos;
	}
}

BOOL CProject::GetRandomBeginPos(DWORD dwWorldID, D3DXVECTOR3* pvOut)
{
	char* lpszWorld
		= m_apszWorld[dwWorldID];

	CMapStrToPtr::iterator i = m_mapBeginPos.find(lpszWorld);
	if (i != m_mapBeginPos.end())
	{
		LPBEGINPOSARR lpBeginPosArr = (LPBEGINPOSARR)i->second;
		*pvOut = lpBeginPosArr->avPos[xRandom(lpBeginPosArr->uSize)];
		return TRUE;
	}
	return FALSE;
}

BOOL CProject::LoadDefOfWorld(LPCTSTR lpszFileName)
{
	CScript s;
	if (s.Load(lpszFileName) == FALSE)
		return(FALSE);

	int nBrace = 1;
	s.SetMark();
	int i = s.GetNumber();	// folder or id

	while (nBrace)
	{
		if (MAX_WORLD <= i)
			Error("Error LoadDefOfWorld i = %d Max값보다 많음", i);

		if (*s.token == '}' || s.tok == FINISHED)
		{
			nBrace--;
			if (nBrace > 0)
			{
				s.SetMark();
				i = s.GetNumber();	// folder or id
				continue;
			}
			if (nBrace == 0)
				continue;
		}
		s.GetToken();		// { or filename
		if (s.Token == "SetTitle")
		{
			s.GetToken(); // (
			strcpy(m_apszWorldName[i], GetLangScript(s));
		}
		else
		{
			s.GoMark();
			i = s.GetNumber(); // id

			s.GetToken();		// filename
			_tcscpy(m_apszWorld[i], s.token);
			_tcslwr(m_apszWorld[i]);
		}
		s.SetMark();
		i = s.GetNumber();	// texture fileName
	}

	return(TRUE);
}

BOOL CProject::LoadExpTable(LPCTSTR lpszFileName)
{
#ifdef __DBSERVER
	CScript s;
	if (s.Load(lpszFileName) == FALSE)
		return FALSE;
	int i = 0;

	s.GetToken();	// subject or FINISHED
	while (s.tok != FINISHED)
	{
		i = 0;
		if (s.Token == _T("expCompanyTest"))
		{
			CGuildTable::GetInstance().ReadBlock(s);
		}
			else if (s.Token == _T("expParty"))
			{
				ZeroMemory(m_aExpParty, sizeof(m_aExpParty));
				s.GetToken();	// {
				DWORD dwVal = s.GetNumber();
				while (*s.token != '}')
				{
					m_aExpParty[i].Exp = dwVal;
					m_aExpParty[i++].Point = s.GetNumber();
					dwVal = s.GetNumber();
				}
			}

		s.GetToken();	// type name or }
	}
#endif	// __DBSERVER
	return TRUE;
}

CProject	prj;