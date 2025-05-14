// InstanceDungeonBase.cpp: implementation of the CInstanceDungeonBase class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InstanceDungeonBase.h"

#if __VER >= 14 // __INSTANCE_DUNGEON
#ifdef __WORLDSERVER
#include "User.h"
extern CUserMng g_UserMng;
#include "worldmng.h"
extern CWorldMng g_WorldMng;
#include "DPCoreClient.h"
extern CDPCoreClient g_DPCoreClient;
#include "DPDatabaseClient.h"
extern CDPDatabaseClient g_dpDBClient;
#include "party.h"
extern	CPartyMng g_PartyMng;

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif
#endif // __WORLDSERVER

#ifdef __CORESERVER
#include "DPCoreSrvr.h"
extern CDPCoreSrvr g_dpCoreSrvr;
#endif // __CORESERVER

////////////////////////////////////////////////////////////////
// CInstanceDungeonBase
////////////////////////////////////////////////////////////////
CInstanceDungeonBase::CInstanceDungeonBase(int nIDType)
	: m_nIDType(nIDType)
#ifdef __CORESERVER
	, m_nCoolTimeCount(0)
#endif // __CORESERVER
{
}

CInstanceDungeonBase::~CInstanceDungeonBase()
{
	m_mapID.clear();
	m_mapCTInfo.clear();
#ifdef __WORLDSERVER
	m_mapDungeonData.clear();
#endif // __WORLDSERVER
}

void CInstanceDungeonBase::SerializeAllInfo(CAr& ar)
{
	if (ar.IsStoring())
	{

		ar << m_mapID.size();
		for (MAP_IDBASE::iterator itMap = m_mapID.begin(); itMap != m_mapID.end(); itMap++)
		{
			ar << itMap->first;
			VEC_IDINFO* pvecTemp;
			pvecTemp = &itMap->second;
			ar << pvecTemp->size();
			for (VEC_IDINFO::iterator itVec = pvecTemp->begin(); itVec != pvecTemp->end(); itVec++)
				(*itVec).Serialize(ar);
		}


		ar << m_mapCTInfo.size();
		for (MAP_CTINFO::iterator it = m_mapCTInfo.begin(); it != m_mapCTInfo.end(); it++)
		{
			ar << it->first;
			VEC_CTINFO* pvecIDCT;
			pvecIDCT = &it->second;
			ar << pvecIDCT->size();
			for (VEC_CTINFO::iterator itVec = pvecIDCT->begin(); itVec != pvecIDCT->end(); itVec++)
				(*itVec).Serialize(ar);
		}
	}
	else
	{

		m_mapID.clear();
		size_t nSizeMap;
		ar >> nSizeMap;
		for (DWORD i = 0; i < nSizeMap; i++)
		{
			DWORD dwDungeonId;	ar >> dwDungeonId;
			size_t nSizeVec;	ar >> nSizeVec;
			for (DWORD j = 0; j < nSizeVec; j++)
			{
				ID_INFO ID_Info(NULL_ID, NULL_ID);
				ID_Info.Serialize(ar);
				CreateDungeon(ID_Info, dwDungeonId);
			}
		}


		m_mapCTInfo.clear();
		ar >> nSizeMap;
		for (DWORD i = 0; i < nSizeMap; i++)
		{
			DWORD dwPlayerId;	ar >> dwPlayerId;
			size_t nSizeVec;	ar >> nSizeVec;
			for (DWORD j = 0; j < nSizeVec; j++)
			{
				COOLTIME_INFO CT_Info;
				CT_Info.Serialize(ar);
				SetDungeonCoolTimeInfo(CT_Info, dwPlayerId);
			}
		}
	}
}

BOOL CInstanceDungeonBase::CreateDungeonLayer(ID_INFO& ID_Info, DWORD dwLayer)
{
#ifdef __WORLDSERVER
	if (ID_Info.uMultiKey != g_uKey)
		return TRUE;

	CWorld* pWorld = g_WorldMng.GetWorld(ID_Info.dwWorldId);
	if (!pWorld || pWorld->m_linkMap.GetLinkMap(static_cast<int>(dwLayer)))
	{
		Error("CInstanceDungeonBase::CreateDungeonLayer() - LinkMap Exist! Type:%d, DungeonID:%d, WorldID:%d", GetType(), dwLayer, ID_Info.dwWorldId);
		return FALSE;
	}

	if (!pWorld->CreateLayer(static_cast<int>(dwLayer)))
	{
		Error("CInstanceDungeonBase::CreateDungeonLayer() - CreateLayer Failed! Type:%d, DungeonID:%d, WorldID:%d", GetType(), dwLayer, ID_Info.dwWorldId);
		return FALSE;
	}

	MAP_IDDATA::const_iterator it = m_mapDungeonData.find(ID_Info.dwWorldId);
	if (it == m_mapDungeonData.end())
	{
		pWorld->ReleaseLayer(static_cast<int>(dwLayer));
		Error("INVALID DUNGEON DATA Type:%d, DungeonID:%d, WorldID:%d", GetType(), dwLayer, ID_Info.dwWorldId);
		return FALSE;
	}

	ID_INFO* pInfo = GetDungeonInfo(dwLayer, ID_Info.dwWorldId);
	if (pInfo == NULL)
	{
		pWorld->ReleaseLayer(static_cast<int>(dwLayer));
		Error("MUST NOT NULL : Type:%d, DungeonID:%d, WorldID:%d", GetType(), dwLayer, ID_Info.dwWorldId);
		return FALSE;
	}

	++CInstanceDungeonHelper::GetInstance()->m_nIDLayerNum;

	while (GetObjCount(pInfo->dwWorldId, dwLayer) <= 0 && pInfo->nState < ID_BOSS)
		pInfo->nState++;

	CreateMonster(dwLayer, ID_Info.dwWorldId);

#endif // __WORLDSERVER
	return TRUE;
}

BOOL CInstanceDungeonBase::DestroyDungeonLayer(ID_INFO& ID_Info, DWORD dwLayer)
{
#ifdef __WORLDSERVER
	if (ID_Info.uMultiKey != g_uKey)
		return TRUE;

	CWorld* pWorld = g_WorldMng.GetWorld(ID_Info.dwWorldId);
	if (!pWorld || !pWorld->m_linkMap.GetLinkMap(static_cast<int>(dwLayer)))
	{
		Error("CInstanceDungeonBase::DestroyDungeonLayer() - LinkMap Not Exist! Type:%d, DungeonID:%d, WorldID:%d", GetType(), dwLayer, ID_Info.dwWorldId);
		return FALSE;
	}

	pWorld->Invalidate(static_cast<int>(dwLayer));

	--CInstanceDungeonHelper::GetInstance()->m_nIDLayerNum;

#endif // __WORLDSERVER
	return TRUE;
}

BOOL CInstanceDungeonBase::CreateDungeon(ID_INFO ID_Info, DWORD dwDungeonId)
{
	VEC_IDINFO* pVecInfo = GetDungeonVector(dwDungeonId);
	if (pVecInfo)
	{
		for (VEC_IDINFO::iterator it = pVecInfo->begin(); it != pVecInfo->end(); it++)
		{
			if ((*it).dwWorldId == ID_Info.dwWorldId)
				return FALSE;
		}

		pVecInfo->push_back(ID_Info);
	}
	else
	{
		VEC_IDINFO vecTemp;
		vecTemp.push_back(ID_Info);
		m_mapID.insert(MAP_IDBASE::value_type(dwDungeonId, vecTemp));
	}

	return CreateDungeonLayer(ID_Info, dwDungeonId);
}

BOOL CInstanceDungeonBase::DestroyDungeon(ID_INFO ID_Info, DWORD dwDungeonId)
{
	MAP_IDBASE::iterator it = m_mapID.find(dwDungeonId);
	if (it != m_mapID.end())
	{
		for (VEC_IDINFO::iterator itVec = it->second.begin(); itVec != it->second.end(); itVec++)
		{
			if ((*itVec).dwWorldId == ID_Info.dwWorldId)
			{
				if (!DestroyDungeonLayer(ID_Info, dwDungeonId))
					return FALSE;

				it->second.erase(itVec);
				if (it->second.empty())
					m_mapID.erase(it);
				ResetDungeonCoolTimeInfo(ID_Info.dwWorldId, dwDungeonId);
#ifdef __CORESERVER
				CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonDestroy(GetType(), dwDungeonId, ID_Info);
#endif // __CORESERVER
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CInstanceDungeonBase::DestroyAllDungeonByDungeonID(DWORD dwDungeonId)
{
	VEC_IDINFO* pvecTemp = GetDungeonVector(dwDungeonId);
	if (pvecTemp)
	{
		VEC_IDINFO vecTemp;
		vecTemp.assign(pvecTemp->begin(), pvecTemp->end());
		for (VEC_IDINFO::iterator itVec = vecTemp.begin(); itVec != vecTemp.end(); itVec++)
#ifdef __CORESERVER
			DestroyDungeon((*itVec), dwDungeonId);
#endif // __CORESERVER
#ifdef __WORLDSERVER
		CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonDestroy(GetType(), dwDungeonId, (*itVec));
#endif // __WORLDSERVER
	}
}

void CInstanceDungeonBase::DestroyAllDungeonByMultiKey(ULONG uMultiKey)
{
	vector< pair<ID_INFO, DWORD> > vecpairTemp;
	for (MAP_IDBASE::iterator it = m_mapID.begin(); it != m_mapID.end(); it++)
	{
		for (VEC_IDINFO::iterator itVec = it->second.begin(); itVec != it->second.end(); itVec++)
		{
			if (uMultiKey == (*itVec).uMultiKey)
				vecpairTemp.push_back(make_pair((*itVec), it->first));
		}
	}

	for (DWORD i = 0; i < vecpairTemp.size(); i++)
#ifdef __CORESERVER
		DestroyDungeon(vecpairTemp[i].first, vecpairTemp[i].second);
#endif // __CORESERVER
#ifdef __WORLDSERVER

	CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonDestroy(GetType(), vecpairTemp[i].second, vecpairTemp[i].first);
#endif // __WORLDSERVER
}

VEC_IDINFO* CInstanceDungeonBase::GetDungeonVector(DWORD dwDungeonId)
{
	MAP_IDBASE::iterator it = m_mapID.find(dwDungeonId);
	if (it != m_mapID.end())
		return &it->second;

	return NULL;
}

ID_INFO* CInstanceDungeonBase::GetDungeonInfo(DWORD dwDungeonId, DWORD dwWorldId)
{
	VEC_IDINFO* pVecInfo = GetDungeonVector(dwDungeonId);
	if (pVecInfo)
	{
		for (VEC_IDINFO::iterator it = pVecInfo->begin(); it != pVecInfo->end(); it++)
		{
			if ((*it).dwWorldId == dwWorldId)
				return &(*it);
		}
	}

	return NULL;
}

VEC_CTINFO* CInstanceDungeonBase::GetCoolTimeVector(DWORD dwPlayerId)
{
	MAP_CTINFO::iterator it = m_mapCTInfo.find(dwPlayerId);
	if (it != m_mapCTInfo.end())
		return &it->second;

	return NULL;
}

COOLTIME_INFO* CInstanceDungeonBase::GetCoolTimeInfo(DWORD dwPlayerId, DWORD dwWorldId)
{
	VEC_CTINFO* pVecCT = GetCoolTimeVector(dwPlayerId);
	if (pVecCT)
	{
		for (VEC_CTINFO::iterator it = pVecCT->begin(); it != pVecCT->end(); it++)
		{
			if ((*it).dwWorldId == dwWorldId)
				return &(*it);
		}
	}
	return NULL;
}

void CInstanceDungeonBase::SetDungeonCoolTimeInfo(COOLTIME_INFO CT_Info, DWORD dwPlayerId)
{
	VEC_CTINFO* pVecCT = GetCoolTimeVector(dwPlayerId);
	if (pVecCT)
	{
		for (VEC_CTINFO::iterator it = pVecCT->begin(); it != pVecCT->end(); it++)
		{
			if ((*it).dwWorldId == CT_Info.dwWorldId && (*it).dwDungeonId != CT_Info.dwDungeonId)
			{
				(*it).dwDungeonId = CT_Info.dwDungeonId;
				(*it).dwCoolTime = CT_Info.dwCoolTime;
				return;
			}
		}
		pVecCT->push_back(CT_Info);
	}
	else
	{
		VEC_CTINFO vecTemp;
		vecTemp.push_back(CT_Info);
		m_mapCTInfo.insert(MAP_CTINFO::value_type(dwPlayerId, vecTemp));
	}
}

void CInstanceDungeonBase::ResetDungeonCoolTimeInfo(DWORD dwWorldId, DWORD dwDungeonId)
{
	for (MAP_CTINFO::iterator it = m_mapCTInfo.begin(); it != m_mapCTInfo.end(); it++)
	{
		for (VEC_CTINFO::iterator itVec = it->second.begin(); itVec != it->second.end(); itVec++)
		{
			if ((*itVec).dwWorldId == dwWorldId && (*itVec).dwDungeonId == dwDungeonId)
				(*itVec).dwDungeonId = NULL_ID;
		}
	}
}

#ifdef __CORESERVER
void CInstanceDungeonBase::DungeonCoolTimeCountProcess()
{
	if (++m_nCoolTimeCount >= COOLTIMECOUNT)
	{
		m_nCoolTimeCount = 0;
		UpdateDungeonCoolTimeInfo();
	}
}

void CInstanceDungeonBase::UpdateDungeonCoolTimeInfo()
{
	DWORD dwTickCount = GetTickCount();
	for (MAP_CTINFO::iterator itMap = m_mapCTInfo.begin(); itMap != m_mapCTInfo.end(); )
	{
		BOOL bErase = TRUE;
		for (VEC_CTINFO::iterator itVec = itMap->second.begin(); itVec != itMap->second.end(); itVec++)
		{
			if ((*itVec).dwCoolTime > dwTickCount)
			{
				bErase = FALSE;
				break;
			}
		}

		if (bErase)
		{
			CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonDeleteCoolTimeInfo(GetType(), itMap->first);
			m_mapCTInfo.erase(itMap++);
		}
		else
			itMap++;
	}
}
#endif // __CORESERVER

#ifdef __WORLDSERVER
void CInstanceDungeonBase::LoadScript(const char* szFilename)
{
	CLuaBase Lua;
	if (Lua.RunScript(szFilename) != 0)
	{
		Error("%s Run Failed!!!", szFilename);
		exit(0);
	}

	m_nMaxInstanceDungeon = static_cast<int>(Lua.GetGlobalNumber("nMaxInstanceDungeon"));

	Lua.GetGloabal("tDungeon");
	Lua.PushNil();
	while (Lua.TableLoop(-2))
	{
		DUNGEON_DATA data;
		for (int i = ID_NORMAL; i <= ID_BOSS; i++)
			data.mapObjCount.insert(map<int, int>::value_type(i, 0));

		DWORD dwWorldId = static_cast<DWORD>(CScript::GetDefineNum(Lua.GetFieldToString(-1, "strWorldId")));
		data.dwClass = static_cast<DWORD>(Lua.GetFieldToNumber(-1, "dwClass"));
		data.nMinLevel = static_cast<int>(Lua.GetFieldToNumber(-1, "nMinLevel"));
		data.nMaxLevel = static_cast<int>(Lua.GetFieldToNumber(-1, "nMaxLevel"));
		data.dwCoolTime = static_cast<DWORD>(Lua.GetFieldToNumber(-1, "dwCoolTime"));
		MAP_IDDATA::iterator it = m_mapDungeonData.insert(MAP_IDDATA::value_type(dwWorldId, data)).first;

		Lua.GetField(-1, "tTeleport");
		Lua.PushNil();
		CWorld* pWorld = g_WorldMng.GetWorld(dwWorldId);
		while (Lua.TableLoop(-2))
		{
			int	nState = static_cast<int>(Lua.GetFieldToNumber(-1, "nType"));
			D3DXVECTOR3 vPos;
			vPos.x = static_cast<float>(Lua.GetFieldToNumber(-1, "x"));
			vPos.y = static_cast<float>(Lua.GetFieldToNumber(-1, "y"));
			vPos.z = static_cast<float>(Lua.GetFieldToNumber(-1, "z"));
			if (!pWorld && pWorld->VecInWorld(vPos) == FALSE)
				Error("Invalid World Pos! WorldId : %d, x : %f, y : %f, z : %f", dwWorldId, vPos.x, vPos.y, vPos.z);
			it->second.mapTeleportPos.insert(map<int, D3DXVECTOR3>::value_type(nState, vPos));
			Lua.Pop(1);
		}
		Lua.Pop(1);

		Lua.GetField(-1, "tMonster");
		Lua.PushNil();
		while (Lua.TableLoop(-2))
		{
			DUNGEON_DATA::MONSTER monster;
			monster.nState = static_cast<int>(Lua.GetFieldToNumber(-1, "nType"));
			monster.dwMonsterId = static_cast<DWORD>(CScript::GetDefineNum(Lua.GetFieldToString(-1, "strMonsterId")));
			monster.bRed = static_cast<BOOL>(Lua.GetFieldToBool(-1, "bRed"));
			monster.vPos.x = static_cast<float>(Lua.GetFieldToNumber(-1, "x"));
			monster.vPos.y = static_cast<float>(Lua.GetFieldToNumber(-1, "y"));
			monster.vPos.z = static_cast<float>(Lua.GetFieldToNumber(-1, "z"));
			if (!prj.GetMoverProp(monster.dwMonsterId))
				Error("Invalid Monster! MonsterId : %d", monster.dwMonsterId);
			it->second.mapObjCount.find(monster.nState)->second++;
			it->second.vecMonster.push_back(monster);

			Lua.Pop(1);
		}
		Lua.Pop(1);

		CInstanceDungeonHelper::GetInstance()->SetDungeonType(dwWorldId, GetType());

		Lua.Pop(1);
	}
	Lua.Pop(0);
}

int	CInstanceDungeonBase::GetState(DWORD dwWorldId, DWORD dwDungeonId)
{
	MAP_IDBASE::iterator it = m_mapID.find(dwDungeonId);
	if (it != m_mapID.end())
	{
		for (DWORD i = 0; i < it->second.size(); i++)
		{
			if (it->second[i].dwWorldId == dwWorldId)
				return it->second[i].nState;
		}
	}

	return ID_NORMAL;
}

void CInstanceDungeonBase::SetNextState(ID_INFO* pInfo, DWORD dwDungeonId)
{
	if (pInfo && pInfo->nState < ID_BOSS)
	{
		D3DXVECTOR3 vPrevPos = GetTeleportPos(pInfo->dwWorldId, dwDungeonId);
		pInfo->nState++;
		pInfo->nKillCount = 0;
		CreateMonster(dwDungeonId, pInfo->dwWorldId);

		D3DXVECTOR3 vNextPos = GetTeleportPos(pInfo->dwWorldId, dwDungeonId);
		if (vPrevPos != vNextPos && vNextPos != D3DXVECTOR3(0, 0, 0))
		{
			CWorld* pWorld = g_WorldMng.GetWorld(pInfo->dwWorldId);
			if (!pWorld)
				return;

			CObj* pObj;
			for (DWORD i = 0; i < pWorld->m_dwObjNum; i++)
			{
				pObj = pWorld->m_apObject[i];
				if (IsValidObj(pObj) && pObj->GetLayer() == static_cast<int>(dwDungeonId)
					&& pObj->GetType() == OT_MOVER && static_cast<CMover*>(pObj)->IsPlayer())
					static_cast<CUser*>(pObj)->REPLACE(g_uIdofMulti, pWorld->GetID(), vNextPos, REPLACE_NORMAL, static_cast<int>(dwDungeonId));
			}
		}

		if (pInfo->nKillCount >= GetObjCount(pInfo->dwWorldId, dwDungeonId))
			SetNextState(pInfo, dwDungeonId);
	}
	else if (pInfo)
	{
#ifdef __FL_DUNGEON_TREASURE
		ApplyTreasure(pInfo, dwDungeonId);
#endif

#ifdef __FL_RECORD_BOOK
		CWorld* pWorld = g_WorldMng.GetWorld(pInfo->dwWorldId);
		if (pWorld)
		{
			CObj* pObj;
			for (DWORD i = 0; i < pWorld->m_dwObjNum; i++)
			{
				pObj = pWorld->m_apObject[i];
				if (IsValidObj(pObj) && pObj->GetLayer() == static_cast<int>(dwDungeonId)
					&& pObj->GetType() == OT_MOVER && static_cast<CMover*>(pObj)->IsPlayer())
				{
					CRecordBook::GetInstance()->AddPlayerRecord((CUser*)pObj, RecordType::DUNGEON_RUNS, 1, true);
				}

			}
		}
#endif

	}
}

#ifdef __FL_DUNGEON_TREASURE
void CInstanceDungeonBase::ApplyTreasure(ID_INFO* pInfo, DWORD dwDungeonId)
{
	if (!pInfo)
		return;

	CWorld* pWorld = g_WorldMng.GetWorld(pInfo->dwWorldId);
	if (!pWorld)
		return;

	for (auto& it : prj.m_vecDungeonTreasures)
	{
		if (it.dwWorldID != pInfo->dwWorldId)
			continue;

		int nRandom = rand() % 100 + 1;
		if (nRandom > it.nChance)
			break;

		// Find all players inside dungeon
		CUser* pListMember[10];
		int	nMaxListMember = 0;
		memset(pListMember, 0, sizeof(pListMember));

		CObj* pObj;
		for (DWORD i = 0; i < pWorld->m_dwObjNum; i++)
		{
			pObj = pWorld->m_apObject[i];
			if (IsValidObj(pObj) && pObj->GetLayer() == static_cast<int>(dwDungeonId)
				&& pObj->GetType() == OT_MOVER && static_cast<CMover*>(pObj)->IsPlayer())
			{
				if (nMaxListMember >= 9)
					break;
				else
					pListMember[nMaxListMember++] = ((CUser*)pObj);
			}
		}

		// Find lucky player
		CUser* pLucky = NULL;
		int nRandomPlayer = rand() % nMaxListMember;
		pLucky = pListMember[nRandomPlayer];

		if (!IsValidObj(pLucky))
			break;

		// Add text for all members from the list + add open window
		for (int i = 0; i < 10; i++)
		{
			if (IsValidObj(pListMember[i]))
				pListMember[i]->AddDefinedCaption(TRUE, TID_RECEIVE_WINDOW, "%s", pLucky->GetName());
		}

		if (pLucky->GetTreasureOpenState() == FALSE)
		{
			pLucky->SetTreasureOpenState(TRUE);
			pLucky->AddDungeonTreasureWnd(TRUE);
		}
		break;
	}
}
#endif

D3DXVECTOR3 CInstanceDungeonBase::GetTeleportPos(DWORD dwWorldId, DWORD dwDungeonId)
{
	MAP_IDDATA::iterator it = m_mapDungeonData.find(dwWorldId);
	if (it == m_mapDungeonData.end())
		return D3DXVECTOR3(0, 0, 0);

	map<int, D3DXVECTOR3>::iterator it2 = it->second.mapTeleportPos.find(GetState(dwWorldId, dwDungeonId));
	if (it2 == it->second.mapTeleportPos.end())
		return D3DXVECTOR3(0, 0, 0);

	return it2->second;
}

BOOL CInstanceDungeonBase::CheckClassLevel(CUser* pUser, DWORD dwWorldId)
{
	MAP_IDDATA::iterator it = m_mapDungeonData.find(dwWorldId);
	if (it == m_mapDungeonData.end())
		return FALSE;

	WORD wClass = 0x0000;
	switch (pUser->GetLegendChar())
	{
	case LEGEND_CLASS_NORMAL: wClass = CLASS_NORMAL; break;
	case LEGEND_CLASS_MASTER: wClass = CLASS_MASTER; break;
	case LEGEND_CLASS_HERO: wClass = CLASS_HERO; break;
	case LEGEND_CLASS_LEGENDHERO: wClass = CLASS_LEGEND_HERO; break;

	default: wClass = 0x0000;
	}

	if (wClass & it->second.dwClass)
	{
		if (pUser->GetLevel() < it->second.nMinLevel || pUser->GetLevel() > it->second.nMaxLevel)
		{
			pUser->AddDefinedText(TID_GAME_INSTANCE_LEVEL, "%d %d", it->second.nMinLevel, it->second.nMaxLevel);
			return FALSE;
		}

		return TRUE;
	}
	else
		pUser->AddDefinedText(TID_GAME_INSTANCE_CLASS);

	return FALSE;
}

BOOL CInstanceDungeonBase::IncreasePlayerCount(DWORD dwDungeonId, DWORD dwWorldId)
{
	ID_INFO* pInfo = GetDungeonInfo(dwDungeonId, dwWorldId);
	if (pInfo)
	{
		pInfo->nPlayerCount++;
		return TRUE;
	}

	return FALSE;
}

BOOL CInstanceDungeonBase::DecreasePlayerCount(CUser* pUser, DWORD dwDungeonId, DWORD dwWorldId)
{
	ID_INFO* pInfo = GetDungeonInfo(dwDungeonId, dwWorldId);
	if (pInfo)
	{
		AddAnnounce(pUser, *pInfo, dwWorldId, FALSE);

		if (--pInfo->nPlayerCount <= 0)
		{
			if (GetType() == IDTYPE_PARTY)
			{
				CParty* pParty = g_PartyMng.GetParty(dwDungeonId);
				if (pParty != NULL)
				{
					for (int i = 0; i < pParty->GetSizeofMember(); ++i)
					{
						CUser* pPartyMember = g_UserMng.GetUserByPlayerID(pParty->GetPlayerId(i));
						if (IsValidObj(pPartyMember) == TRUE && pPartyMember != pUser)
						{
							if (pPartyMember->GetWorld() != NULL && pPartyMember->GetWorld()->GetID() == dwWorldId
								&& pPartyMember->GetLayer() == dwDungeonId)
							{
								return FALSE;
							}
						}
					}
				}
				else
				{
					return TRUE;
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}

void CInstanceDungeonBase::CreateMonster(DWORD dwDungeonId, DWORD dwWorldId)
{
	CWorld* pWorld = g_WorldMng.GetWorld(dwWorldId);
	if (!pWorld || !pWorld->m_linkMap.GetLinkMap(static_cast<int>(dwDungeonId)))
		return;

	MAP_IDDATA::iterator it = m_mapDungeonData.find(pWorld->GetID());
	if (it != m_mapDungeonData.end())
	{
		int nState = GetState(dwWorldId, dwDungeonId);
		for (vector<DUNGEON_DATA::MONSTER>::iterator itVecMon = it->second.vecMonster.begin(); itVecMon != it->second.vecMonster.end(); itVecMon++)
		{
			if ((*itVecMon).nState != nState)
				continue;

			MoverProp* pMoverProp = prj.GetMoverProp((*itVecMon).dwMonsterId);
			if (pMoverProp)
			{
				CObj* pObj = CreateObj(D3DDEVICE, OT_MOVER, pMoverProp->dwID);
				if (NULL == pObj)
					return;

				pObj->SetPos((*itVecMon).vPos);
				pObj->InitMotion(MTI_STAND);
				pObj->UpdateLocalMatrix();

				((CMover*)pObj)->m_bActiveAttack = (*itVecMon).bRed;
				if (pWorld->ADDOBJ(pObj, TRUE, dwDungeonId) == FALSE)
				{
					SAFE_DELETE(pObj);
				}
			}
		}
	}
}

BOOL CInstanceDungeonBase::TeleportToDungeon(CUser* pUser, DWORD dwWorldId, DWORD dwDungeonId)
{
	if (pUser->HasBuffByIk3(IK3_TEXT_DISGUISE))
	{
		pUser->AddDefinedText(TID_GAME_INSTANCE_DISGUISE02);
		return FALSE;
	}

	CWorld* pWorld = g_WorldMng.GetWorld(dwWorldId);
	if (!pWorld)
		return FALSE;

	COOLTIME_INFO* pCT_Info = GetCoolTimeInfo(pUser->m_idPlayer, dwWorldId);
	if (pCT_Info)
	{
		if (pCT_Info->dwDungeonId != dwDungeonId)
		{
			DWORD dwRemainCoolTime = pCT_Info->dwCoolTime - GetTickCount();
			if (static_cast<int>(dwRemainCoolTime) > 0)
			{
				DWORD dwRemainMin = dwRemainCoolTime / MIN(1);
				DWORD dwRemainSec = (dwRemainCoolTime % MIN(1)) / SEC(1);
				pUser->AddDefinedText(TID_GAME_INSTANCE_COOLTIME, "%d %d", dwRemainMin, dwRemainSec);
				return FALSE;
			}
		}
	}

	ID_INFO* pInfo = GetDungeonInfo(dwDungeonId, dwWorldId);
	if (pInfo)
	{
		if (pInfo->uMultiKey != g_uKey)
		{
			pUser->AddDefinedText(TID_GAME_INSTANCE_CHANNEL);
			return FALSE;
		}

		if (!CheckClassLevel(pUser, dwWorldId))
			return FALSE;

		D3DXVECTOR3 vPos = GetTeleportPos(dwWorldId, dwDungeonId);
		if (vPos == D3DXVECTOR3(0, 0, 0))
		{
			REGIONELEM* pPortkey = pUser->UpdateRegionAttr();
			if (pPortkey && pPortkey->m_dwIdTeleWorld == dwWorldId)
				vPos = pPortkey->m_vTeleWorld;
			else
				return FALSE;
		}

		if (pWorld->m_linkMap.GetLinkMap(static_cast<int>(dwDungeonId)))
		{
			SetLeaveMarkingPos(pUser, dwWorldId, vPos);
			int nRandx = xRandom(4) - 2;
			int nRandz = xRandom(4) - 2;
			vPos += D3DXVECTOR3((float)(nRandx), (float)(0), (float)(nRandz));

			if (pUser->REPLACE(g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, static_cast<int>(dwDungeonId)) == FALSE)
			{
				pUser->AddDefinedText(TID_GAME_LORD_SKILL_USE_E009);
				return FALSE;
			}

			AddAnnounce(pUser, *pInfo, dwWorldId, TRUE);

			if (!pCT_Info || pCT_Info->dwDungeonId != dwDungeonId)
				CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonSetCoolTimeInfo(g_uKey, GetType(), pUser->m_idPlayer, COOLTIME_INFO(dwWorldId, dwDungeonId, (GetCoolTime(dwWorldId) + GetTickCount())));

			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		if (!CheckClassLevel(pUser, dwWorldId))
			return FALSE;

		if (CInstanceDungeonHelper::GetInstance()->m_nIDLayerNum < m_nMaxInstanceDungeon)
			CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonCreate(GetType(), dwDungeonId, ID_INFO(dwWorldId, g_uKey)); 
		else
		{
			pUser->AddDefinedText(TID_GAME_INSTANCE_MAX);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CInstanceDungeonBase::LeaveToOutside(CUser* pUser, DWORD dwWorldId, DWORD dwDungeonId)
{
#ifdef __FL_DUNGEON_TREASURE
	if (IsValidObj(pUser))
	{
		if (pUser->GetTreasureOpenState() == TRUE)
		{
			pUser->SetTreasureOpenState(FALSE);
			pUser->AddDungeonTreasureWnd(FALSE);
		}
	}
#endif
	if (DecreasePlayerCount(pUser, dwDungeonId, dwWorldId))
	{
		CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonDestroy(GetType(), dwDungeonId, ID_INFO(dwWorldId, g_uKey));
		return TRUE;
	}
	return FALSE;
}

void CInstanceDungeonBase::SetLeaveMarkingPos(CUser* pUser, DWORD dwWorldId, D3DXVECTOR3 vPos)
{
	pUser->m_idMarkingWorld = WI_WORLD_MADRIGAL;
	pUser->m_vMarkingPos = D3DXVECTOR3(6983.0f, 0.0f, 3330.0f);

	CWorld* pWorld = g_WorldMng.GetWorld(dwWorldId);
	if (!pWorld)
		return;

	float fDist = static_cast<float>(INT_MAX);
	for (int i = 0; i < pWorld->m_aRegion.GetSize(); i++)
	{
		LPREGIONELEM lpRegionElem = pWorld->m_aRegion.GetAt(i);
		if (!lpRegionElem || lpRegionElem->m_dwIdTeleWorld == WI_WORLD_NONE)
			continue;
		float fTemp = D3DXVec3LengthSq(&(vPos - lpRegionElem->m_vPos));
		if (fTemp < fDist)
		{
			fDist = fTemp;
			pUser->m_vMarkingPos = lpRegionElem->m_vTeleWorld;
			pUser->m_idMarkingWorld = lpRegionElem->m_dwIdTeleWorld;
		}
	}
}


DWORD CInstanceDungeonBase::GetCoolTime(DWORD dwWorldId)
{
	MAP_IDDATA::iterator it = m_mapDungeonData.find(dwWorldId);
	if (it != m_mapDungeonData.end())
		return it->second.dwCoolTime;

	return 0;
}

BOOL CInstanceDungeonBase::IsNowStateMonster(DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId)
{
	MAP_IDDATA::iterator it = m_mapDungeonData.find(dwWorldId);
	if (it == m_mapDungeonData.end())
		return FALSE;

	int nNowState = GetState(dwWorldId, dwDungeonId);
	for (vector<DUNGEON_DATA::MONSTER>::iterator i = it->second.vecMonster.begin(); i != it->second.vecMonster.end(); i++)
	{
		if ((*i).nState == nNowState && (*i).dwMonsterId == dwMonsterId)
			return TRUE;
	}

	return FALSE;
}

int CInstanceDungeonBase::GetObjCount(DWORD dwWorldId, DWORD dwDungeonId)
{
	MAP_IDDATA::iterator it = m_mapDungeonData.find(dwWorldId);
	if (it == m_mapDungeonData.end())
		return 0;

	map<int, int>::iterator itObjCount = it->second.mapObjCount.find(GetState(dwWorldId, dwDungeonId));
	if (itObjCount == it->second.mapObjCount.end())
		return 0;

	return itObjCount->second;
}

void CInstanceDungeonBase::SetInstanceDungeonKill(DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId)
{
	if (IsNowStateMonster(dwWorldId, dwDungeonId, dwMonsterId))
	{
		ID_INFO* pInfo = GetDungeonInfo(dwDungeonId, dwWorldId);
		if (pInfo)
		{
			pInfo->nKillCount++;
			if (pInfo->nKillCount >= GetObjCount(dwWorldId, dwDungeonId))
				SetNextState(pInfo, dwDungeonId);
		}
	}
}

void CInstanceDungeonBase::DeleteDungeonCoolTimeInfo(DWORD dwPlayerId)
{
	MAP_CTINFO::iterator itMap = m_mapCTInfo.find(dwPlayerId);
	if (itMap != m_mapCTInfo.end())
		m_mapCTInfo.erase(itMap);
}

void CInstanceDungeonBase::AddAnnounce(CUser* pUser, ID_INFO& ID_Info, DWORD dwWorldId, BOOL bEnter)
{
	if (!pUser)
		return;

	CParty* pParty = g_PartyMng.GetParty(pUser->m_idparty);
	if (pParty)
	{
		CString strWorldName;
		strWorldName.Format("[%s] %s", g_WorldMng.GetWorldStruct(dwWorldId)->m_szWorldName, pUser->GetName());

		DWORD dwText;
		if (bEnter)
		{
			if (ID_Info.nPlayerCount == 0)
				dwText = TID_DUNGEON_OPENED;
			else
				dwText = TID_DUNGEON_ENTER;
		}
		else
		{
			if (ID_Info.nPlayerCount == 1)
				dwText = TID_DUNGEON_CLOSED;
			else
				dwText = TID_DUNGEON_OUT;
		}

		CUser* pMember;
		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			pMember = g_UserMng.GetUserByPlayerID(pParty->GetPlayerId(i));
			if (pMember)
				pMember->AddDefinedCaption(TRUE, dwText, "\"%s\"", strWorldName);
		}
	}
}
#endif // __WORLDSERVER


////////////////////////////////////////////////////////////////
// CInstanceDungeonHelper
////////////////////////////////////////////////////////////////
#include "InstanceDungeonParty.h"

CInstanceDungeonHelper::CInstanceDungeonHelper()
{
#ifdef __WORLDSERVER
	m_uCertifyKey = NULL_ID;
	m_nIDLayerNum = 0;
	CInstanceDungeonParty::GetInstance()->LoadScript("DungeonParty.lua");
#ifdef __FL_SOLO_DUNGEONS
	CInstanceDungeonSolo::GetInstance()->LoadScript("DungeonSolo.lua");
#endif
#endif // __WORLDSERVER
}

CInstanceDungeonHelper::~CInstanceDungeonHelper()
{
#ifdef __WORLDSERVER
	m_mapDungeonType.clear();
#endif // __WORLDSERVER
}

CInstanceDungeonHelper* CInstanceDungeonHelper::GetInstance()
{
	static CInstanceDungeonHelper sIDH;
	return &sIDH;
}

CInstanceDungeonBase* CInstanceDungeonHelper::GetDungeonPtr(int nType)
{
	switch (nType)
	{
	case IDTYPE_PARTY: return CInstanceDungeonParty::GetInstance();
#ifdef __FL_SOLO_DUNGEONS
	case IDTYPE_SOLO: return CInstanceDungeonSolo::GetInstance();
#endif
	}
	return NULL;
}

#ifdef __WORLDSERVER
void CInstanceDungeonHelper::IncreasePlayerCount(const DWORD dwWorldID, const DWORD dwDungeonId)
{
	CInstanceDungeonBase* pID = GetDungeonPtr(GetType(dwWorldID));
	if (pID)
		pID->IncreasePlayerCount(dwDungeonId, dwWorldID);
}
#endif //__WORLDSERVER

void CInstanceDungeonHelper::OnCreateDungeon(int nType, ID_INFO ID_Info, DWORD dwDungeonId)
{
#ifdef __WORLDSERVER
	SetCertifyKey(ID_Info.uMultiKey);
#endif // __WORLDSERVER
	CInstanceDungeonBase* pID = GetDungeonPtr(nType);
	if (pID)
	{
		pID->CreateDungeon(ID_Info, dwDungeonId);
#ifdef __CORESERVER
		SendInstanceDungeonCreate(nType, dwDungeonId, ID_Info);
#endif // __CORESERVER
	}
}

void CInstanceDungeonHelper::OnDestroyDungeon(int nType, ID_INFO ID_Info, DWORD dwDungeonId)
{
	CInstanceDungeonBase* pID = GetDungeonPtr(nType);
	if (pID)
		pID->DestroyDungeon(ID_Info, dwDungeonId);
}

void CInstanceDungeonHelper::OnSetDungeonCoolTimeInfo(ULONG uKey, int nType, COOLTIME_INFO CT_Info, DWORD dwPlayerId)
{
#ifdef __WORLDSERVER
	SetCertifyKey(uKey);
#endif // __WORLDSERVER
	CInstanceDungeonBase* pID = GetDungeonPtr(nType);
	if (pID)
	{
		pID->SetDungeonCoolTimeInfo(CT_Info, dwPlayerId);
#ifdef __CORESERVER
		SendInstanceDungeonSetCoolTimeInfo(uKey, nType, dwPlayerId, CT_Info);
		pID->DungeonCoolTimeCountProcess();
#endif // __CORESERVER
}
	}

void CInstanceDungeonHelper::SendInstanceDungeonCreate(int nType, DWORD dwDungeonId, ID_INFO& ID_Info)
{
#ifdef __WORLDSERVER
	if (!IsCertifying())
	{
		SetCertifyKey();
		g_DPCoreClient.SendInstanceDungeonCreate(nType, dwDungeonId, ID_Info);
	}
#endif // __WORLDSERVER
#ifdef __CORESERVER
	g_dpCoreSrvr.SendInstanceDungeonCreate(nType, dwDungeonId, ID_Info);
#endif// __CORESERVER
}

void CInstanceDungeonHelper::SendInstanceDungeonDestroy(int nType, DWORD dwDungeonId, ID_INFO& ID_Info)
{
#ifdef __WORLDSERVER
	g_DPCoreClient.SendInstanceDungeonDestroy(nType, dwDungeonId, ID_Info);
#endif // __WORLDSERVER
#ifdef __CORESERVER
	g_dpCoreSrvr.SendInstanceDungeonDestroy(nType, dwDungeonId, ID_Info);
#endif// __CORESERVER
}

void CInstanceDungeonHelper::SendInstanceDungeonSetCoolTimeInfo(ULONG uKey, int nType, DWORD dwPlayerId, COOLTIME_INFO& CT_Info)
{
#ifdef __WORLDSERVER
	if (!IsCertifying())
	{
		SetCertifyKey();
		g_DPCoreClient.SendInstanceDungeonSetCoolTimeInfo(nType, dwPlayerId, CT_Info);
	}
#endif // __WORLDSERVER
#ifdef __CORESERVER
	g_dpCoreSrvr.SendInstanceDungeonSetCoolTimeInfo(uKey, nType, dwPlayerId, CT_Info);
#endif// __CORESERVER
}

void CInstanceDungeonHelper::DestroyAllDungeonByMultiKey(ULONG uMultiKey)
{
	for (int i = IDTYPE_SOLO; i < IDTYPE_MAX; i++)
	{
		CInstanceDungeonBase* pID = GetDungeonPtr(i);
		if (pID)
			pID->DestroyAllDungeonByMultiKey(uMultiKey);
	}
}

#ifdef __CORESERVER
void CInstanceDungeonHelper::SendInstanceDungeonAllInfo(DWORD dpId)
{
	for (int i = IDTYPE_SOLO; i < IDTYPE_MAX; i++)
	{
		CInstanceDungeonBase* pID = GetDungeonPtr(i);
		if (pID)
			g_dpCoreSrvr.SendInstanceDungeonAllInfo(i, pID, dpId);
	}
}

void CInstanceDungeonHelper::SendInstanceDungeonDeleteCoolTimeInfo(int nType, DWORD dwPlayerId)
{
	g_dpCoreSrvr.SendInstanceDungeonDeleteCoolTimeInfo(nType, dwPlayerId);
}
#endif // __CORESERVER

#ifdef __WORLDSERVER
void CInstanceDungeonHelper::OnInstanceDungeonAllInfo(CAr& ar)
{
	int nType;
	ar >> nType;
	CInstanceDungeonBase* pID = GetDungeonPtr(nType);
	if (pID)
		pID->SerializeAllInfo(ar);
	else
	{
		Error("CDPCoreClient::OnInstanceDungeonAllInfo() - Wrong Party Type - %d", nType);
		exit(0);
	}
}

void CInstanceDungeonHelper::SetCertifyKey(ULONG uMultiKey)
{
	if (uMultiKey == NULL_ID)
		m_uCertifyKey = g_uKey;
	else if (m_uCertifyKey == uMultiKey)
		m_uCertifyKey = NULL_ID;
}

void CInstanceDungeonHelper::SetDungeonType(DWORD dwWorldId, int nType)
{
	m_mapDungeonType.insert(MAP_WORLD2TYPE::value_type(dwWorldId, nType));
}

int CInstanceDungeonHelper::GetType(DWORD dwWorldId)
{
	MAP_WORLD2TYPE::iterator it = m_mapDungeonType.find(dwWorldId);
	if (it == m_mapDungeonType.end())
		return NULL_ID;

	return it->second;
}

BOOL CInstanceDungeonHelper::EnteranceDungeon(CUser* pUser, DWORD dwWorldId)
{
	CInstanceDungeonBase* pID = GetDungeonPtr(GetType(dwWorldId));
	if (IsValidObj(pUser) && pID)
		return pID->EnteranceDungeon(pUser, dwWorldId);

	return FALSE;
}

BOOL CInstanceDungeonHelper::LeaveDungeon(CUser* pUser, DWORD dwWorldId)
{
	CInstanceDungeonBase* pID = GetDungeonPtr(GetType(dwWorldId));
	if (pID)
	{
		if (pID->LeaveDungeon(pUser, dwWorldId))
			return TRUE;
	}

	return FALSE;
}


void CInstanceDungeonHelper::GoOut(CUser* pUser)
{
	if (pUser->GetWorld() == NULL)
		return;

	pUser->REPLACE(g_uIdofMulti, pUser->m_idMarkingWorld, pUser->m_vMarkingPos, REPLACE_FORCE, nDefaultLayer);
}

void CInstanceDungeonHelper::SetInstanceDungeonKill(DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId)
{
	CInstanceDungeonBase* pID = GetDungeonPtr(GetType(dwWorldId));
	if (pID)
		pID->SetInstanceDungeonKill(dwWorldId, dwDungeonId, dwMonsterId);
}

void CInstanceDungeonHelper::OnDeleteDungeonCoolTimeInfo(int nType, DWORD dwPlayerId)
{
	CInstanceDungeonBase* pID = GetDungeonPtr(nType);
	if (pID)
		pID->DeleteDungeonCoolTimeInfo(dwPlayerId);
}
#endif // __WORLDSERVER

#endif // __INSTANCE_DUNGEON