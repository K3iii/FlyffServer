#include "stdafx.h"
#include "MonsterAlbum.h"
#ifdef __WORLDSERVER
#include "defineQuest.h"
#include "defineText.h"
#include "User.h"

#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
extern  CDPCoreClient g_DPCoreClient;
extern CDPDatabaseClient g_dpDBClient;
#endif
#ifdef __CLIENT
#include "ResData.h"
#endif
#if defined (__WORLDSERVER) || defined (__CLIENT)
#include "WorldMng.h"
extern	CWorldMng	g_WorldMng;
#endif

#ifdef __FL_MONSTER_ALBUM
CMonsterAlbumMng::CMonsterAlbumMng()
{

}

CMonsterAlbumMng::~CMonsterAlbumMng()
{
	m_mapMonsters.clear();
	m_mapBonuses.clear();
}
CMonsterAlbumMng* CMonsterAlbumMng::GetInstance()
{
	static CMonsterAlbumMng	sProperty;
	return &sProperty;
}
#if defined (__WORLDSERVER) || defined (__CLIENT)
void CMonsterAlbumMng::LoadMonsterAlbum()
{
	CScript s;
	if (!s.Load("MonsterAlbum.inc"))
	{
		Error("CMonsterAlbumMng::LoadMonsterAlbum cant load MonsterAlbum.inc");
		return;
	}

	m_mapMonsters.clear();
	m_mapBonuses.clear();

	s.GetToken();
	while (s.tok != FINISHED)
	{
		if (s.Token == _T("Monsters"))
		{
			s.GetToken(); // {
			s.GetToken(); // } or MOB_ID

			while (*s.token != '}')
			{
				if (s.Token == _T("MOB_ID"))
				{
					DWORD dwMonsterID = (DWORD)s.GetNumber();

					CARD_STRUC tmp;

					s.GetToken();
					s.GetToken();
					while (*s.token != '}')
					{
						if (s.Token == _T("WORLD_ID"))
						{
							s.GetToken();
							tmp.dwWorldID = (DWORD)s.GetNumber();
							LPWORLD pWorldStruc = g_WorldMng.GetWorldStruct(tmp.dwWorldID);
							if (!pWorldStruc)
							{
								Error("MonsterAlbum : no world, MOB_ID = %d", dwMonsterID);
								tmp.dwWorldID = NULL_ID;
							}
							s.GetToken();
						}
						else if (s.Token == _T("CARD_PIECE_ID"))
						{
							s.GetToken();
							tmp.dwCardPieceID = (DWORD)s.GetNumber();
							ItemProp* pCardPieceProp = prj.GetItemProp(tmp.dwCardPieceID);
							if (!pCardPieceProp)
							{
								Error("MonsterAlbum : no pCardPieceProp, MOB_ID = %d", dwMonsterID);
								tmp.dwCardPieceID = NULL_ID;
							}
							s.GetToken();
						}
						else if (s.Token == _T("CHANCE_TO_OBTAIN"))
						{
							s.GetToken();
							tmp.dwChanceToObtain = (DWORD)s.GetInt64();
							if (tmp.dwChanceToObtain > 3000000000)
							{
								Error("MonsterAlbum : dwChanceToObtain more than 100%, MOB_ID = %d", dwMonsterID);
								tmp.dwChanceToObtain = 0;
							}
							s.GetToken();
						}
					}

					auto it = m_mapMonsters.find(dwMonsterID);
					if (it == m_mapMonsters.end())
					{
						MoverProp* pMoverProp = prj.GetMoverProp(dwMonsterID);
						if (pMoverProp)
						{
							m_mapMonsters.insert(make_pair(dwMonsterID, tmp));
#ifdef __WORLDSERVER
							if (tmp.dwCardPieceID != NULL_ID && tmp.dwChanceToObtain != 0)
							{
								QUESTITEM qi;
								qi.dwQuest = QUEST_SPECIAL_MONSTERALBUM;
								qi.dwState = 0;
								qi.dwIndex = tmp.dwCardPieceID;
								qi.dwProbability = tmp.dwChanceToObtain;
								qi.dwNumber = 1;
								pMoverProp->m_QuestItemGenerator.AddTail(qi);
							}
							else
								Error("MonsterAlbum : cannot add loot to questitemgenerator, MOB_ID = %d", dwMonsterID);
#endif
						}
						else
							Error("MonsterAlbum : cannot find mover prop, MOB_ID = %d", dwMonsterID);
					}
					else
						Error("MonsterAlbum : already added to the list, MOB_ID = %d", dwMonsterID);

					s.GetToken();
				}
			}
		}
		else if (s.Token == _T("Bonuses"))
		{
			s.GetToken(); // {
			s.GetToken(); // } or UnlockedMobs
			while (*s.token != '}')
			{
				if (s.Token == _T("UnlockedMobs"))
				{
					size_t nUnlockedMobs = (size_t)s.GetNumber();

					BONUS_STRUC tmp;

					s.GetToken(); // {
					s.GetToken(); // } or ItemRewards or StatRewards

					while (*s.token != '}')
					{
						if (s.Token == _T("ItemRewards"))
						{
							s.GetToken(); // {

							ITEM_REWARD_STRUC tmpItem;
							tmpItem.dwItemID = (DWORD)s.GetNumber();
							while (*s.token != '}')
							{
								tmpItem.nItemCount = s.GetNumber();
								tmpItem.bBinds = (BOOL)s.GetNumber();

								ItemProp* pProp = prj.GetItemProp(tmpItem.dwItemID);
								if (pProp && tmpItem.nItemCount >= 1 && tmpItem.nItemCount < SHRT_MAX)
									tmp.vecItems.push_back(tmpItem);
								else
									Error("MonsterAlbum : cannot add item for %d unlocked mobs", nUnlockedMobs);
								tmpItem.dwItemID = (DWORD)s.GetNumber();
							}

							s.GetToken();
						}
						else if (s.Token == _T("StatRewards"))
						{
							s.GetToken(); // {

							DST_REWARD_STRUC tmpDst;
							tmpDst.dwDest = (DWORD)s.GetNumber();
							while (*s.token != '}')
							{
								tmpDst.nVal = s.GetNumber();

								if (tmpDst.dwDest > 0 && tmpDst.dwDest < MAX_ADJPARAMARY)
									tmp.vecDst.push_back(tmpDst);
								else
									Error("MonsterAlbum : cannot add stat for %d unlocked mobs", nUnlockedMobs);
								tmpDst.dwDest = (DWORD)s.GetNumber();
							}

							s.GetToken();
						}
					}

					m_mapBonuses.insert(make_pair(nUnlockedMobs, tmp));

					s.GetToken();
				}
			}
		}
		s.GetToken();
	}
}
#endif
bool CMonsterAlbumMng::FindMonster(DWORD dwID)
{
	auto it = m_mapMonsters.find(dwID);
	if (it != m_mapMonsters.end())
		return true;

	return false;
}

CMonsterAlbum::CMonsterAlbum(CMover* pMover)
	: m_pMover(pMover)
{
	for (int i = 0; i < MAX_BOSS_CARDS; i++)
		m_arrayBossCards[i] = NULL_ID;
}

CMonsterAlbum::~CMonsterAlbum()
{
	m_mapMonsterProgress.clear();
}
void CMonsterAlbum::Serialize(CAr& ar)
{
	if (ar.IsStoring())
	{
		ar << m_mapMonsterProgress.size();
		for (auto& it : m_mapMonsterProgress)
		{
			ar << it.first;
			ar << it.second;
		}

		for (int i = 0; i < MAX_BOSS_CARDS; i++)
			ar << m_arrayBossCards[i];
	}
	else
	{
		m_mapMonsterProgress.clear();

		size_t size;
		ar >> size;

		for (size_t i = 0; i < size; i++)
		{
			DWORD dwMonster;
			int nProgress;

			ar >> dwMonster;
			ar >> nProgress;

			m_mapMonsterProgress.insert(make_pair(dwMonster, nProgress));
		}

		for (int i = 0; i < MAX_BOSS_CARDS; i++)
			ar >> m_arrayBossCards[i];
	}
}
int CMonsterAlbum::GetMonsterProgress(DWORD dwID)
{
	auto it = m_mapMonsterProgress.find(dwID);
	if (it != m_mapMonsterProgress.end())
		return it->second;

	return 0;
}
size_t CMonsterAlbum::GetCompletedMonstersCount()
{
	size_t nCompleted = 0;

	for (auto& it : m_mapMonsterProgress)
	{
		if (it.second == CARDS_NEEDED_TO_UNLOCK)
			nCompleted++;
	}

	return nCompleted;
}
void CMonsterAlbum::MergeBonuses(unordered_map<DWORD, int>* pMergedDst)
{
	// Monster album part
	size_t nCompleted = GetCompletedMonstersCount();

	for (auto& it : *CMonsterAlbumMng::GetInstance()->GetBonuses())
	{
		if (nCompleted < it.first)
			continue;

		for (auto& it2 : it.second.vecDst)
			(*pMergedDst)[it2.dwDest] += it2.nVal;
	}

	// Boss cards part
	ItemProp* pBossCardProp = nullptr;
	for (int i = 0; i < MAX_BOSS_CARDS; i++)
	{
		if (m_arrayBossCards[i] != NULL_ID && m_arrayBossCards[i] != 0)
		{
			pBossCardProp = prj.GetItemProp(m_arrayBossCards[i]);
			if (pBossCardProp)
			{
				for (int b = 0; b < PROP_BONUSES; b++)
				{
					if (pBossCardProp->dwDestParam[b] != -1)
						(*pMergedDst)[pBossCardProp->dwDestParam[b]] += pBossCardProp->nAdjParamVal[b];
				}
			}
		}
	}
}

#ifdef __WORLDSERVER
void CMonsterAlbum::SetApplyDST()
{
	unordered_map<DWORD, int> mapBonuses;
	MergeBonuses(&mapBonuses);

	for (auto & it : mapBonuses)
		m_pMover->SetDestParam(it.first, it.second, NULL_CHGPARAM);
}
void CMonsterAlbum::ResetApplyDST()
{
	unordered_map<DWORD, int> mapBonuses;
	MergeBonuses(&mapBonuses);

	for (auto& it : mapBonuses)
		m_pMover->ResetDestParam(it.first, it.second);
}
void CMonsterAlbum::AddMonsterCardPiece(DWORD dwCardObjId, int nType)
{
	if (!m_pMover->IsPlayer())
		return;

	CUser* pUser = (CUser*)m_pMover;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CItemContainer<CItemElem>* pContainer = pUser->GetContainerByType(nType);
	if (!pContainer)
		return;

	CItemElem* pItemElemCardPiece = pContainer->GetAtId(dwCardObjId);
	if (!IsUsableItem(pItemElemCardPiece))
		return;

	ItemProp* pCardPieceProp = pItemElemCardPiece->GetProp();
	if (!pCardPieceProp)
		return;

	if (pCardPieceProp->dwItemKind3 != IK3_MONSTER_ALBUM)
		return;

	DWORD dwLinkedMobID = pCardPieceProp->dwAbilityMin;
	if (dwLinkedMobID == NULL_ID)
		return;

	if (!CMonsterAlbumMng::GetInstance()->FindMonster(dwLinkedMobID))
		return;

	int nItemNum = pItemElemCardPiece->m_nItemNum;
	int nPiecesNeededToUnlock = CARDS_NEEDED_TO_UNLOCK - m_mapMonsterProgress[dwLinkedMobID];

	if (nPiecesNeededToUnlock == 0)
		return;

	if (nItemNum > nPiecesNeededToUnlock)
		nItemNum = nPiecesNeededToUnlock;

	m_mapMonsterProgress[dwLinkedMobID] += nItemNum;
	pUser->RemoveItem((BYTE)(dwCardObjId), nItemNum, nType);

	if (m_mapMonsterProgress[dwLinkedMobID] == CARDS_NEEDED_TO_UNLOCK)
	{
		size_t nCompleted = GetCompletedMonstersCount();

		auto itBonus = CMonsterAlbumMng::GetInstance()->GetBonuses()->find(nCompleted);
		if (itBonus != CMonsterAlbumMng::GetInstance()->GetBonuses()->end())
		{
			for (auto& itItem : itBonus->second.vecItems)
			{
				CItemElem itemElem;
				itemElem.m_dwItemId = itItem.dwItemID;
				itemElem.m_nItemNum = itItem.nItemCount;
				if (itItem.bBinds)
					itemElem.SetFlag(CItemElem::binds);
				itemElem.SetSerialNumber();

				if (pUser->CreateItem(&itemElem) == FALSE)
					g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, itemElem, 0, "Monster Album", "Your reward for unlocking monster album card.");
			}

			for (auto& itStat : itBonus->second.vecDst)
				m_pMover->SetDestParam(itStat.dwDest, itStat.nVal, NULL_CHGPARAM);
		}

		if (nCompleted == CMonsterAlbumMng::GetInstance()->GetMonsters()->size())
		{
			CString str;
			str.Format("%s unlocked all possible monsters in his Monster Album!", pUser->GetName());
			g_DPCoreClient.SendCaption(str);
		}
	}

	pUser->AddMAAddCardPiece(dwLinkedMobID, nItemNum);
}
void CMonsterAlbum::BossCardSlotUnlock(DWORD dwKeyObjId, int nType)
{
	if (!m_pMover->IsPlayer())
		return;

	CUser* pUser = (CUser*)m_pMover;

	CItemContainer<CItemElem>* pContainer = pUser->GetContainerByType(nType);
	if (!pContainer)
		return;

	CItemElem* pItemElemKey = pContainer->GetAtId(dwKeyObjId);
	if (!IsUsableItem(pItemElemKey))
		return;

	ItemProp* pKeyProp = pItemElemKey->GetProp();
	if (!pKeyProp)
		return;

	if (pKeyProp->dwID != II_SYS_SYS_MA_SLOT_UNLOCK)
		return;

	int nFirstLockedSlot = -1;
	for (int i = 0; i < MAX_BOSS_CARDS; i++)
	{
		if (m_arrayBossCards[i] == NULL_ID)
		{
			nFirstLockedSlot = i;
			break;
		}
	}

	if (nFirstLockedSlot == -1)
		return;

	m_arrayBossCards[nFirstLockedSlot] = 0;
	pUser->RemoveItem((BYTE)(dwKeyObjId), 1, nType);

	pUser->AddMABossCardSlotUnlock(nFirstLockedSlot);
}
void CMonsterAlbum::BossCardSlotInsert(DWORD dwBossCardObjId, int nType)
{
	if (!m_pMover->IsPlayer())
		return;

	CUser* pUser = (CUser*)m_pMover;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CItemContainer<CItemElem>* pContainer = pUser->GetContainerByType(nType);
	if (!pContainer)
		return;

	CItemElem* pItemElemBossCard = pContainer->GetAtId(dwBossCardObjId);
	if (!IsUsableItem(pItemElemBossCard))
		return;

	ItemProp* pBossCardProp = pItemElemBossCard->GetProp();
	if (!pBossCardProp)
		return;

	if (pBossCardProp->dwItemKind3 != IK3_MONSTER_ALBUM_BOSS_CARD)
		return;

	int nFirstAvailableSlot = -1;
	for (int i = 0; i < MAX_BOSS_CARDS; i++)
	{
		if (m_arrayBossCards[i] == 0 && nFirstAvailableSlot == -1)
			nFirstAvailableSlot = i;

		if (m_arrayBossCards[i] == pBossCardProp->dwID)
		{
			pUser->AddText("You already have this boss card activated!", 0xffffffff);
			return;
		}
	}

	if (nFirstAvailableSlot == -1)
		return;

	m_arrayBossCards[nFirstAvailableSlot] = pBossCardProp->dwID;
	pUser->RemoveItem((BYTE)(dwBossCardObjId), 1, nType);

	for (int i = 0; i < PROP_BONUSES; i++)
		m_pMover->SetDestParam(i, pBossCardProp);

	pUser->AddMABossCardSlotInsert(nFirstAvailableSlot, pBossCardProp->dwID);
}
void CMonsterAlbum::BossCardSlotRestore(DWORD dwKeyObjId, int nType, int nSlot)
{
	if (!m_pMover->IsPlayer())
		return;

	CUser* pUser = (CUser*)m_pMover;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CItemContainer<CItemElem>* pContainer = pUser->GetContainerByType(nType);
	if (!pContainer)
		return;

	CItemElem* pItemElemKey = pContainer->GetAtId(dwKeyObjId);
	if (!IsUsableItem(pItemElemKey))
		return;

	ItemProp* pKeyProp = pItemElemKey->GetProp();
	if (!pKeyProp)
		return;

	if (pKeyProp->dwID != II_SYS_SYS_MA_SLOT_RESTORE)
		return;

	if (nSlot < 0 || nSlot >= MAX_BOSS_CARDS)
		return;

	if (m_arrayBossCards[nSlot] == NULL_ID || m_arrayBossCards[nSlot] == 0)
		return;

	ItemProp* pBossCardProp = prj.GetItemProp(m_arrayBossCards[nSlot]);
	if (!pBossCardProp)
	{
		pUser->AddText("Cannot restore this card.");
		return;
	}

	CItemElem itemElem;
	itemElem.m_dwItemId = pBossCardProp->dwID;
	itemElem.m_nItemNum = 1;
	itemElem.SetSerialNumber();

	if (!pUser->CreateItem(&itemElem))
		g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, itemElem, 0, "Boss Card", "Your restored boss card.");

	m_arrayBossCards[nSlot] = 0;
	pUser->RemoveItem((BYTE)(dwKeyObjId), 1, nType);

	for (int i = 0; i < PROP_BONUSES; i++)
		m_pMover->ResetDestParam(i, pBossCardProp);

	pUser->AddMABossCardSlotRestore(nSlot);
}
#endif
#ifdef __CLIENT
void CMonsterAlbum::AddMonsterCardPiece(DWORD dwMobID, int nAddedPieces)
{
	m_mapMonsterProgress[dwMobID] += nAddedPieces;
	
	if (m_mapMonsterProgress[dwMobID] == CARDS_NEEDED_TO_UNLOCK)
		g_WndMng.PutString("Monster unlocked!");
	else
		g_WndMng.PutString("Card pieces added!");

	CWndMonsterAlbum* pWnd = (CWndMonsterAlbum*)g_WndMng.GetWndBase(APP_MONSTER_ALBUM);
	if (pWnd)
		pWnd->UpdateProgress(dwMobID);
}
void CMonsterAlbum::BossCardSlotUnlock(int nSlot)
{
	m_arrayBossCards[nSlot] = 0;
	
	g_WndMng.PutString("Boss Card Slot unlocked!");
}
void CMonsterAlbum::BossCardSlotInsert(int nSlot, DWORD dwBossCardID)
{
	m_arrayBossCards[nSlot] = dwBossCardID;

	g_WndMng.PutString("Boss Card inserted!");

	CWndMonsterAlbum* pWnd = (CWndMonsterAlbum*)g_WndMng.GetWndBase(APP_MONSTER_ALBUM);
	if (pWnd)
	{
		pWnd->UpdateBossCardTexture(nSlot);
		pWnd->UpdateStatSum();
	}
}
void CMonsterAlbum::BossCardSlotRestore(int nSlot)
{
	m_arrayBossCards[nSlot] = 0;

	g_WndMng.PutString("Boss Card restored!");

	CWndMonsterAlbum* pWnd = (CWndMonsterAlbum*)g_WndMng.GetWndBase(APP_MONSTER_ALBUM);
	if (pWnd)
		pWnd->UpdateStatSum();
}
#endif

#endif