#include "stdafx.h"
#include <algorithm>
#include "RecordBook.h"
#include "defineText.h"
#ifdef __WORLDSERVER
#include "User.h"
#include "DPDatabaseClient.h"
#include "DPCoreClient.h"
extern  CDPCoreClient g_DPCoreClient;
extern	CUserMng g_UserMng;
extern CDPDatabaseClient g_dpDBClient;
#endif
#ifdef __DBSERVER
#include "dptrans.h"
extern AppInfo g_appInfo;
#endif

#ifdef __FL_RECORD_BOOK
CRecordBook::CRecordBook()
{
	for (auto& vector : m_arrayRecords)
		vector.reserve(MAX_TOP_RECORDS + 1);
}

CRecordBook::~CRecordBook()
{
	Clear();
}
void CRecordBook::Clear()
{
#ifdef __DBSERVER
	CMclAutoLock Lock(m_AccessRb);
#endif

	for (auto& vector : m_arrayRecords)
		vector.clear();

	m_mapPlayerEquipments.clear();
}
CRecordBook* CRecordBook::GetInstance()
{
	static CRecordBook	sProperty;
	return &sProperty;
}

vector<PlayerRecord>* CRecordBook::GetVectorRecords(int nType)
{
	if (nType < 0 || nType >= MAX_RECORDTYPE)
		return nullptr;

	return &m_arrayRecords.at(nType);
}

void CRecordBook::SerializeAllRecords(CAr& ar)
{
#ifdef __DBSERVER
	CMclAutoLock Lock(m_AccessRb);
#endif

	if (ar.IsStoring())
	{
		ar << m_arrayRecords.size();
		for (auto& vector : m_arrayRecords)
		{
			ar << vector.size();
			for (auto& record : vector)
				record.Serialize(ar);
		}

		ar << m_mapPlayerEquipments.size();
		for (auto& it : m_mapPlayerEquipments)
		{
			ar << it.first;
			it.second.Serialize(ar);
		}
	}
	else
	{
		for (auto& vector : m_arrayRecords)
			vector.clear();

		size_t arraySize;
		ar >> arraySize;

		for (size_t v = 0; v < arraySize; v++)
		{
			size_t vecSize;
			ar >> vecSize;

			for (size_t r = 0; r < vecSize; r++)
			{
				PlayerRecord tmp;
				tmp.Serialize(ar);

				m_arrayRecords.at(v).push_back(tmp);
			}
		}

		m_mapPlayerEquipments.clear();

		size_t mapSize;
		ar >> mapSize;

		for (size_t i = 0; i < mapSize; i++)
		{
			u_long idPlayer;
			ar >> idPlayer;

			PlayerEquipment tmp;
			tmp.Serialize(ar);

			m_mapPlayerEquipments.insert(make_pair(idPlayer, tmp));
		}
	}
}

#ifdef __WORLDSERVER
void CRecordBook::AddPlayerRecord(CMover* pMover, int nRecordType, __int64 n64Value, bool bAddToCurrent)
{
	if (!IsValidObj(pMover) || !pMover->IsPlayer())
		return;

	CUser* pUser = (CUser*)pMover;

	if (nRecordType < 0 || nRecordType >= MAX_RECORDTYPE)
		return;

	if (n64Value <= 0)
		return;

	if (__LOCAL_TESTS == FALSE)
	{
		if (pMover->IsAuthHigher(AUTH_GAMEMASTER))
			return;
	}

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
		return;
#endif

	PlayerRecord_Own tmp;
	tmp.bUpdated = true;
	tmp.nLevel = pMover->GetLevel();
	tmp.nJob = pMover->GetJob();
	tmp.n64Value = n64Value;

	auto& oldRecord = pUser->m_arrayRecords.at(nRecordType);
	if (bAddToCurrent)
	{
		oldRecord.bUpdated = true;
		oldRecord.nLevel = tmp.nLevel;
		oldRecord.nJob = tmp.nJob;
		oldRecord.n64Value += tmp.n64Value;
	}
	else
	{
		if (tmp.n64Value > oldRecord.n64Value)
			oldRecord = tmp;
	}
}
#endif
#ifdef __DBSERVER
void CRecordBook::UpdateGlobalRecords(CMover* pMover)
{
	CMclAutoLock Lock(m_AccessRb);

	if (!pMover)
		return;

	bool bInsertEquipment = false;

	for (int i = 0; i < MAX_RECORDTYPE; i++)
	{
		PlayerRecord_Own* pPlayerRecord = &pMover->m_arrayRecords.at(i);
		if (!pPlayerRecord->bUpdated)
			continue;

		pPlayerRecord->bUpdated = false;

		auto& vectorRecords = m_arrayRecords.at(i);

		int playerIndex = FindPlayerIndex(pMover->m_idPlayer, &vectorRecords);
		if (playerIndex != -1)
		{
			bInsertEquipment = true;

			PlayerRecord& existingPlayerRecord = vectorRecords.at(playerIndex);
			if (existingPlayerRecord.n64Value < pPlayerRecord->n64Value)
			{
				existingPlayerRecord.nLevel = pPlayerRecord->nLevel;
				existingPlayerRecord.nJob = pPlayerRecord->nJob;
				existingPlayerRecord.n64Value = pPlayerRecord->n64Value;

				std::sort(vectorRecords.begin(), vectorRecords.end(), CompareByValueDescending());
			}
		}
		else
		{
			bool bAddToVector = false;
			if (vectorRecords.size() < MAX_TOP_RECORDS)
				bAddToVector = true;
			else
			{
				if (pPlayerRecord->n64Value > vectorRecords.back().n64Value)
					bAddToVector = true;
			}

			if (bAddToVector)
			{
				bInsertEquipment = true;

				PlayerRecord tmp;
				tmp.idPlayer = pMover->m_idPlayer;
				tmp.nLevel = pPlayerRecord->nLevel;
				tmp.nJob = pPlayerRecord->nJob;
				tmp.n64Value = pPlayerRecord->n64Value;

				vectorRecords.push_back(tmp);

				std::sort(vectorRecords.begin(), vectorRecords.end(), CompareByValueDescending());

				if (vectorRecords.size() > MAX_TOP_RECORDS)
				{
					u_long idPlayerRemove = vectorRecords.back().idPlayer;
					vectorRecords.pop_back();
					RemovePlayerEquipment(idPlayerRemove);
				}
			}
		}
	}

	if (bInsertEquipment)
		InsertPlayerEquipment(pMover);
}
void CRecordBook::InsertPlayerEquipment(CMover* pMover)
{
	CMclAutoLock Lock(m_AccessRb);

	if (!pMover)
		return;

	PlayerEquipment tmp;
	tmp.nJob = pMover->m_nJob;
	tmp.nSex = pMover->GetSex();
	tmp.dwHairMesh = pMover->m_dwHairMesh;
	tmp.dwHairColor = pMover->m_dwHairColor;
	tmp.dwHeadMesh = pMover->m_dwHeadMesh;

	for (DWORD i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (i == PARTS_RIDE)
			continue;

		CItemElem* pItemElem = pMover->m_Inventory.GetEquip(i);
		if (pItemElem)
		{
			tmp.aEquipInfo[i].dwId = pItemElem->m_dwItemId;
			tmp.aEquipInfo[i].nOption = pItemElem->GetAttrOption();
#ifdef __MODEL_CHANGE
			tmp.aEquipInfo[i].dwLook = pItemElem->GetLook();
#endif
		}
	}

	auto it = m_mapPlayerEquipments.find(pMover->m_idPlayer); // Check if player equipment already in the list
	if (it != m_mapPlayerEquipments.end())
	{
		auto& playerEquipment = it->second;
		playerEquipment = tmp;
	}
	else
		m_mapPlayerEquipments.insert(make_pair(pMover->m_idPlayer, tmp));
}
void CRecordBook::RemovePlayerEquipment(u_long idPlayer)
{
	CMclAutoLock Lock(m_AccessRb);

	if (FindPlayer(idPlayer)) // Check if player have at least one record (means his equipment still needed to know)
		return;

	auto it = m_mapPlayerEquipments.find(idPlayer);
	if (it == m_mapPlayerEquipments.end())
		return;

	m_mapPlayerEquipments.erase(it);
}
bool CRecordBook::FindPlayer(u_long idPlayer)
{
	CMclAutoLock Lock(m_AccessRb);

	for (auto& vector : m_arrayRecords)
	{
		for (auto& record : vector)
		{
			if (record.idPlayer == idPlayer)
				return true;
		}
	}

	return false;
}
int CRecordBook::FindPlayerIndex(u_long idPlayer, vector<PlayerRecord>* pVec)
{
	CMclAutoLock Lock(m_AccessRb);

	for (int i = 0; i < (int)pVec->size(); i++)
	{
		PlayerRecord* record = &pVec->at(i);
		if (record->idPlayer == idPlayer)
			return i;
	}

	return -1;
}
void CRecordBook::LoadRecordBook()
{
	CMclAutoLock Lock(m_AccessRb);

	CQuery* pQuery = g_DbManager.m_apQuery[0];
	if (!pQuery)
		return;

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspRecordBookGet '%02d'", g_appInfo.dwSys);

	if (!pQuery->Exec(szQuery))
	{
		Error("LoadRecordBook - %s", szQuery);
		return;
	}

	for (auto& vector : m_arrayRecords)
		vector.clear();

	int nRecordType;
	u_long idPlayer;
	PlayerRecord playerRecord;

	std::unordered_set<u_long> setPlayers;

	while (pQuery->Fetch())
	{
		nRecordType = pQuery->GetInt("nRecordType");
		idPlayer = (u_long)pQuery->GetInt("idPlayer");

		playerRecord.idPlayer = idPlayer;
		playerRecord.nLevel = pQuery->GetInt("nLevel");
		playerRecord.nJob = pQuery->GetInt("nJob");
		playerRecord.n64Value = pQuery->GetInt64("n64Value");

		if (nRecordType < 0 || nRecordType >= MAX_RECORDTYPE)
			continue;

		m_arrayRecords.at(nRecordType).push_back(playerRecord);

		setPlayers.insert(idPlayer);
	}

	SortAllRecords();

	LoadPlayerEquipments(&setPlayers);
}
void CRecordBook::LoadPlayerEquipments(std::unordered_set<u_long>* setPlayers)
{
	CMclAutoLock Lock(m_AccessRb);

	if (setPlayers->empty())
		return;

	CQuery* pQuery = g_DbManager.m_apQuery[0];
	if (!pQuery)
		return;

	char szQuery[QUERY_SIZE] = { 0, };

	m_mapPlayerEquipments.clear();

	for (auto& it : *setPlayers)
	{
		sprintf(szQuery, "uspRecordBookGetEquipment '%07d', '%02d'", it, g_appInfo.dwSys);
		if (!pQuery->Exec(szQuery))
		{
			Error("CRecordBookDbController::LoadPlayerEquipments error: %s", szQuery);
			continue;
		}

		if (!pQuery->Fetch())
			continue;

		PlayerEquipment tmp;
		tmp.nJob = pQuery->GetInt("m_nJob");
		tmp.nSex = pQuery->GetInt("m_dwSex");
		tmp.dwHairMesh = (DWORD)pQuery->GetInt("m_dwHairMesh");
		tmp.dwHairColor = (DWORD)pQuery->GetInt("m_dwHairColor");
		tmp.dwHeadMesh = (DWORD)pQuery->GetInt("m_dwHeadMesh");

		CItemContainer< CItemElem > tmpInventoryOwner;
		tmpInventoryOwner.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);

		ItemContainerStruct	ics;
		sprintf(ics.szItem, "m_Inventory");
		sprintf(ics.szIndex, "m_apIndex");
		g_DbManager.GetContainer(pQuery, &tmpInventoryOwner, &ics);

		for (DWORD i = 0; i < MAX_HUMAN_PARTS; i++)
		{
			if (i == PARTS_RIDE)
				continue;

			CItemElem* pItemElem = tmpInventoryOwner.GetEquip(i);
			if (pItemElem)
			{
				tmp.aEquipInfo[i].dwId = pItemElem->m_dwItemId;
				tmp.aEquipInfo[i].nOption = pItemElem->GetAttrOption();
#ifdef __MODEL_CHANGE
				tmp.aEquipInfo[i].dwLook = pItemElem->GetLook();
#endif
			}
		}

		m_mapPlayerEquipments.insert(make_pair(it, tmp));
	}
}
void CRecordBook::SaveRecordBook()
{
	CMclAutoLock Lock(m_AccessRb);

	CQuery* pQuery = g_DbManager.m_apQuery[0];
	if (!pQuery)
		return;

	bool bEmpty = true;
	for (auto& vector : m_arrayRecords)
	{
		if (!vector.empty())
		{
			bEmpty = false;
			break;
		}
	}

	if (bEmpty)
		return;

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspRecordBookDelete '%02d'", g_appInfo.dwSys);

	if (!pQuery->Exec(szQuery))
	{
		Error("SaveRecordBook - %s", szQuery);
		return;
	}

	int nRecordType = 0;
	for (auto& vector : m_arrayRecords)
	{
		for (auto& record : vector)
		{
			sprintf(szQuery, "uspRecordBookInsert '%02d', %d, %d, %d, %d, %I64d", g_appInfo.dwSys, nRecordType, record.idPlayer, record.nLevel, record.nJob, record.n64Value);

			if (!pQuery->Exec(szQuery))
				Error("Cant execute %s", szQuery);
		}

		nRecordType++;
	}
}
void CRecordBook::SortAllRecords()
{
	CMclAutoLock Lock(m_AccessRb);

	for (auto& vector : m_arrayRecords)
		std::sort(vector.begin(), vector.end(), CompareByValueDescending());
}
void CRecordBook::SendRecordBook(DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_RECORD_BOOK, DPID_UNKNOWN, DPID_UNKNOWN);
	SerializeAllRecords(ar);
	SEND(ar, CDPTrans::GetInstance(), dpId);
}
#endif
#endif