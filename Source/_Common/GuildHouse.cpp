// GuildHouse.cpp: implementation of the CGuildHouse class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#include "defineText.h"
#ifdef __CLIENT
#include "MsgHdr.h"
#include "DPClient.h"
#include "WndHousing.h"
extern CDPClient g_DPlay;
#endif // __CLIENT
#ifdef __WORLDSERVER
#include "WorldMng.h"
extern CWorldMng g_WorldMng;
#include "guild.h"
extern CGuildMng g_GuildMng;
#include "user.h"
extern CUserMng g_UserMng;
#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;
#include "DPDatabaseClient.h"
extern CDPDatabaseClient g_dpDBClient;
#include "npchecker.h"
#endif // __WORLDSERVER
#ifdef __DBSERVER
#include "GuildHouseDBCtrl.h"
#endif // __DBSERVER

CGuildHouseBase::GuildHouse_Furniture_Info::GuildHouse_Furniture_Info(DWORD dwII, BOOL bS, D3DXVECTOR3 v, float fA, time_t tK)
	:dwItemId(dwII), bSetup(bS), vPos(v), fAngle(fA), objId(NULL_ID), tKeepTime(time_null() + tK)
#ifdef __DBSERVER
	, nSeqNum(0)
#endif // __DBSERVER
{
}

void CGuildHouseBase::GuildHouse_Furniture_Info::Serialize(CAr& ar)
{
	if (ar.IsStoring())
	{
		ar << dwItemId << bSetup << vPos << fAngle << objId;
		if (tKeepTime > 0)
			ar << tKeepTime - time_null();
		else
			ar << static_cast<time_t>(0);
	}
	else
	{
		ar >> dwItemId >> bSetup >> vPos >> fAngle >> objId;
		ar >> tKeepTime;
		if (tKeepTime > 0)
			tKeepTime += time_null();
	}
}


////////////////////////////////////////////////////////////////
// CGuildHouseBase
////////////////////////////////////////////////////////////////
CGuildHouseBase::CGuildHouseBase(DWORD dwGuildId, DWORD dwWorldId)
	:m_dwGuildId(dwGuildId), m_dwWorldId(dwWorldId), m_tUpkeepTime(0)
{
#ifdef __WORLDSERVER
	m_dwWaitDBAckPlayerId = NULL_ID;
#endif // __WORLDSERVER

#ifdef __CLIENT
	m_dwSelectedObjID = NULL_ID;
	m_nWndDeployingIndex = -1;
#endif
}

CGuildHouseBase::~CGuildHouseBase()
{
	Clear();
}

void CGuildHouseBase::Clear()
{
#ifdef __WORLDSERVER
	DestroyGuildHouseRoom();
	for (int i = 0; i < (int)(m_vecFntInfo.size()); i++)
	{
		if (m_vecFntInfo[i].bSetup)
			ResetDSTFunriture(prj.GetItemProp(m_vecFntInfo[i].dwItemId));
	}
#endif // __WORLDSERVER
	m_vecFntInfo.clear();
}

#ifdef __CLIENT
CGuildHouseBase* CGuildHouseBase::GetInstance()
{
	static CGuildHouseBase sGHB(NULL_ID, NULL_ID);
	return &sGHB;
}

void CGuildHouseBase::SendClientToWorld(int nPacketType, GH_Fntr_Info& gfi, int nIndex)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILDHOUSE_PACKET, DPID_UNKNOWN);
	ar << nPacketType << nIndex;
	gfi.Serialize(ar);
	SEND(ar, &g_DPlay, DPID_SERVERPLAYER);
}

struct IsSameID : public binary_function< GH_Fntr_Info, OBJID, bool >
{
	bool operator( ) (const GH_Fntr_Info& kInfo, OBJID id_) const
	{
		return (kInfo.objId == id_);
	}
};

GH_Fntr_Info* CGuildHouseBase::Find(OBJID objID_)
{
	VECFurnitureIter finder = find_if(m_vecFntInfo.begin(), m_vecFntInfo.end(), bind2nd(IsSameID(), objID_));
	if (finder != m_vecFntInfo.end())
		return &(*finder);

	return NULL;
}

int CGuildHouseBase::FindIndex(OBJID objID_)
{
	for (int i = 0; i < GetFurnitureListSize(); ++i)
	{
		GH_Fntr_Info* pInfo = GetFurnitureInfoPtr(i);
		assert(pInfo);

		if (objID_ == pInfo->objId)
		{
			return i;
		}
	}

	return -1;
}

void CGuildHouseBase::Setup(const HOUSING_ITEM& kInfo)
{
	GH_Fntr_Info kSendInfo;
	kSendInfo.dwItemId = kInfo.m_nIndex;
	kSendInfo.bSetup = kInfo.m_bDeploy;
	kSendInfo.vPos = kInfo.m_vPos;
	kSendInfo.fAngle = kInfo.m_fAngle;
	kSendInfo.objId = kInfo.dwItemId;
	kSendInfo.tKeepTime = kInfo.m_dwTime;

	SendClientToWorld(GUILDHOUSE_PCKTTYPE_SETUP, kSendInfo, kInfo.m_nSlotIndex);
	m_dwSelectedObjID = NULL_ID;
	m_iMode = 0;
}

void CGuildHouseBase::Reset()
{

	GH_Fntr_Info* pInfo = Find(m_dwSelectedObjID);
	if (!pInfo)
		return;

	int slotIndex = FindIndex(m_dwSelectedObjID);
	if (slotIndex < 0)
		return;

	HOUSING_ITEM kItem;
	kItem.m_bDeploy = pInfo->bSetup;
	kItem.dwItemId = pInfo->objId;
	kItem.m_nIndex = pInfo->dwItemId;
	kItem.m_vPos = pInfo->vPos;
	kItem.m_fAngle = pInfo->fAngle;
	kItem.m_nSlotIndex = slotIndex;

	ItemProp* pItemProp = prj.GetItemProp(pInfo->dwItemId);
	assert(pItemProp);

	GuildDeploy()->LoadToDeploy(pItemProp->dwLinkKind, kItem);

	m_iMode = GUILDHOUSE_PCKTTYPE_RESET;
}

void CGuildHouseBase::Reset(const HOUSING_ITEM& kInfo)
{

	GH_Fntr_Info kSendInfo;
	kSendInfo.dwItemId = kInfo.m_nIndex;
	kSendInfo.bSetup = kInfo.m_bDeploy;
	kSendInfo.vPos = kInfo.m_vPos;
	kSendInfo.fAngle = kInfo.m_fAngle;
	kSendInfo.objId = kInfo.dwItemId;
	kSendInfo.tKeepTime = kInfo.m_dwTime;

	SendClientToWorld(GUILDHOUSE_PCKTTYPE_RESET, kSendInfo, kInfo.m_nSlotIndex);
	m_dwSelectedObjID = NULL_ID;
	m_iMode = 0;
}

void CGuildHouseBase::Remove()
{
	int slotIndex = FindIndex(m_dwSelectedObjID);

	if (slotIndex < 0)
		return;

	GH_Fntr_Info* pInfo = Find(m_dwSelectedObjID);
	if (!pInfo)
		return;

	//gmpbigsun: testing
	SendClientToWorld(GUILDHOUSE_PCKTTYPE_REMOVE, *pInfo, slotIndex);
	m_dwSelectedObjID = NULL_ID;
	m_iMode = 0;
}

void CGuildHouseBase::ApplyEFTexture()
{
	CWorld* pWorld = g_WorldMng.Get();
	if (!pWorld)
		return;

	if (!pWorld->IsWorldGuildHouse())
		return;


	BOOL bWallTex = FALSE;
	BOOL bTileTex = FALSE;

	int iMaxSize = GetFurnitureListSize();
	for (int i = 0; i < iMaxSize; ++i)
	{
		GH_Fntr_Info* pInfo = GetFurnitureInfoPtr(i);
		if (!pInfo)
			continue;

		ItemProp* pProp = prj.GetItemProp(pInfo->dwItemId);
		if (!pProp)
		{
			assert(0);
			continue;
		}

		if (IK2_GUILDHOUSE_FURNITURE != pProp->dwItemKind2 && IK2_GUILDHOUSE_PAPERING != pProp->dwItemKind2)
			continue;

		if (pInfo->bSetup)
		{
			if (IK3_WALLPAPER == pProp->dwItemKind3)
			{
				GuildDeploy()->ChangeWallTex(pProp->szTextFileName);
				bWallTex = TRUE;
			}


			if (IK3_CARPET == pProp->dwItemKind3)
			{
				GuildDeploy()->ChangeTileTex(pProp->szTextFileName);
				bTileTex = TRUE;
			}
		}
	}


	if (!bWallTex)
		GuildDeploy()->ChangeWallTex();

	if (!bTileTex)
		GuildDeploy()->ChangeTileTex();
}

void CGuildHouseBase::ResetValues()
{
	m_dwGuildId = 0;
	m_dwWorldId = 0;
	m_vecFntInfo.clear();

	m_dwSelectedObjID = 0;
	m_iMode = 0;
	m_nWndDeployingIndex = 0;
	m_bSetFurnitureChannel = FALSE;
}

#endif // __CLIENT

#ifdef __WORLDSERVER
BOOL CGuildHouseBase::CreateGuildHouseRoom()
{
	CWorld* pWorld = g_WorldMng.GetWorld(m_dwWorldId);
	if (!pWorld) return FALSE;

	if (pWorld->m_linkMap.GetLinkMap(static_cast<int>(m_dwGuildId)))
		return TRUE;

	if (pWorld->CreateLayer(static_cast<int>(m_dwGuildId)))
	{
		for (int i = 0; i < (int)(m_vecFntInfo.size()); i++)
		{
			if (m_vecFntInfo[i].bSetup)
			{
				SetupFurnitureCtrl(i, m_vecFntInfo[i]);
				SendWorldToClient(GUILDHOUSE_PCKTTYPE_REFRESH, m_vecFntInfo[i], i);
			}
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CGuildHouseBase::DestroyGuildHouseRoom()
{
	CWorld* pWorld = g_WorldMng.GetWorld(m_dwWorldId);
	if (!pWorld || !pWorld->m_linkMap.GetLinkMap(static_cast<int>(m_dwGuildId)))
		return FALSE;

	for (int i = 0; i < (int)(m_vecFntInfo.size()); i++)
	{
		if (m_vecFntInfo[i].bSetup)
		{
			m_vecFntInfo[i].objId = NULL_ID;
			SendWorldToClient(GUILDHOUSE_PCKTTYPE_REFRESH, m_vecFntInfo[i], i);
		}
	}

	pWorld->Invalidate(static_cast<int>(m_dwGuildId));
	return TRUE;
}

BOOL CGuildHouseBase::IsEnteranceAble(CUser* pUser)
{
	if (m_tUpkeepTime == 0)
	{
		pUser->AddDefinedText(TID_GAME_GUILDHOUSE_EXPIRATION);
		return FALSE;
	}

	if (!IsValidObj(pUser) || pUser->m_idGuild != m_dwGuildId)
		return FALSE;

	if (CreateGuildHouseRoom())
		return TRUE;

	return FALSE;
}

BOOL CGuildHouseBase::EnteranceGuildHouseRoom(CUser* pUser)
{
	if (IsEnteranceAble(pUser))
	{
		return pUser->REPLACE(g_uIdofMulti, m_dwWorldId, GetEnterPos(), REPLACE_NORMAL, static_cast<int>(m_dwGuildId));
	}

	return FALSE;
}

void CGuildHouseBase::GoOutGuildHouseRoom(CUser* pUser)
{
	if (IsValidObj(pUser) && pUser->GetWorld()->GetID() == m_dwWorldId && pUser->GetLayer() == m_dwGuildId)
		pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, pUser->m_vMarkingPos, REPLACE_FORCE, nDefaultLayer);
}

void CGuildHouseBase::CheckDestroyRoom(CUser* pUser)
{
	CWorld* pWorld = g_WorldMng.GetWorld(m_dwWorldId);
	if (pWorld && pWorld->m_linkMap.GetLinkMap(static_cast<int>(m_dwGuildId)))
	{

		if (g_UserMng.HasUserSameWorldnLayer(pUser))
			return;

		DestroyGuildHouseRoom();
	}
}

BOOL CGuildHouseBase::IsAuthority(CUser* pUser, int nPacketType)
{
	if (!IsValidObj(pUser) || pUser->m_idGuild != m_dwGuildId)	return FALSE;
	CGuild* pGuild = g_GuildMng.GetGuild(m_dwGuildId);
	if (!pGuild)	return FALSE;

	if (nPacketType == GUILDHOUSE_PCKTTYPE_UPKEEP)
	{
		if (!pGuild->IsAuthority(pUser->m_idPlayer, PF_GUILDHOUSE_UPKEEP))
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_TAX_LEVEL);
			return FALSE;
		}
	}
	else
	{
		if (!pGuild->IsAuthority(pUser->m_idPlayer, PF_GUILDHOUSE_FURNITURE))
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_INSIDE_INSTALL_LEVEL);
			return FALSE;
		}
		else if (pUser->GetWorld()->GetID() != m_dwWorldId || pUser->GetLayer() != m_dwGuildId)
			return FALSE;
	}

	return TRUE;
}

BOOL CGuildHouseBase::PreCheckPacket(CUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex)
{

	if (!IsAuthority(pUser, nPacketType))
		return FALSE;

	switch (nPacketType)
	{
	case GUILDHOUSE_PCKTTYPE_LISTUP:
	{
		if (!IsListUpAble(gfi.dwItemId))
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_INSIDE_SPACE);
			return FALSE;
		}
		break;
	}

	case GUILDHOUSE_PCKTTYPE_SETUP:
	{
		if (!g_eLocal.GetState(ENABLE_GUILD_INVENTORY))
			return FALSE;

		if (nIndex >= (int)(m_vecFntInfo.size()))	return FALSE;
		if (m_vecFntInfo[nIndex].bSetup)	return FALSE;

		gfi.bSetup = TRUE;
		ItemProp* pItemProp = prj.GetItemProp(gfi.dwItemId);
		if (!pItemProp) return FALSE;
		if (pItemProp->dwItemKind3 == IK3_TELEPORTER)
		{
			gfi.vPos = GetTeleporterPos();
			gfi.fAngle = 0.0f;
		}
		if (m_vecFntInfo[nIndex].tKeepTime == 0)
			gfi.tKeepTime = time_null() + (pItemProp->dwAbilityMin * 60);

		return IsSetupAble(pUser, nIndex);
	}

	case GUILDHOUSE_PCKTTYPE_REMOVE:
		if (!g_eLocal.GetState(ENABLE_GUILD_INVENTORY))
			return FALSE;

		if (nIndex >= (int)(m_vecFntInfo.size()))	return FALSE;
		if (!m_vecFntInfo[nIndex].bSetup)	return FALSE;
		gfi.objId = NULL_ID;
		gfi.bSetup = FALSE;
		break;

	case GUILDHOUSE_PCKTTYPE_RESET:
		if (!g_eLocal.GetState(ENABLE_GUILD_INVENTORY))
			return FALSE;

		if (nIndex >= (int)(m_vecFntInfo.size()))	return FALSE;
		if (!m_vecFntInfo[nIndex].bSetup)	return FALSE;
		gfi.bSetup = TRUE;
		gfi.tKeepTime = m_vecFntInfo[nIndex].tKeepTime;
		break;

	case GUILDHOUSE_PCKTTYPE_UPKEEP:
	{
		gfi.tKeepTime = gfi.dwItemId * UPKEEP_DAY_TIME + (GetUpkeepTime() ? GetUpkeepTime() : time_null());
		int nTotalDays = (gfi.tKeepTime - time_null()) / UPKEEP_DAY_TIME;
		if (nTotalDays >= MAX_UPKEEP_DAY || nTotalDays <= 0)
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_TAX_MAX, "%d", MAX_UPKEEP_DAY);
			return FALSE;
		}

		const __int64 n64PayPenya = gfi.dwItemId * GetUpkeepPenya();
		if (n64PayPenya < 0 || n64PayPenya > INT_MAX)
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_BUY_EXPENSE);
			return FALSE;
		}

		const int nPayPenya = static_cast<int>(n64PayPenya);
		if (pUser->CheckUserGold(nPayPenya, false) == true)
		{
			pUser->AddGold(-nPayPenya);
			g_DPSrvr.PutPenyaLog(pUser, "f", "GUILDHOUSE_UPKEEP", nPayPenya);
		}
		else
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_BUY_EXPENSE);
			return FALSE;
		}

		break;


	}
	}

	return TRUE;
}

void CGuildHouseBase::AfterFailedGuildHousePacket(int nPacketType, GH_Fntr_Info& gfi, int nIndex)
{
	CUser* pUser = static_cast<CUser*>(prj.GetUserByID(GetWaitDBAckPlayerId()));
	if (!IsValidObj(pUser))
	{
		Error("CGuildHouseBase::AfterFailedGuildHousePacket() : Invalid User! [Packet:%d], [User:%d], [ItemId:%d]", nPacketType, GetWaitDBAckPlayerId(), gfi.dwItemId);
		return;
	}

	pUser->AddDefinedText(TID_GAME_GUILDHOUSE_LATER);
	switch (nPacketType)
	{
	case GUILDHOUSE_PCKTTYPE_LISTUP:
	{
		CItemElem itemElem;
		itemElem.m_dwItemId = gfi.dwItemId;
		itemElem.m_nItemNum = 1;
		if (pUser->CreateItem(&itemElem))
		{
			g_DPSrvr.PutItemLog(pUser, "f", "GUILDHOUSE_LISTUP_REPAIR", &itemElem, 1);
		}
		else
			Error("CGuildHouseBase::AfterFailedGuildHousePacket() : CreateItem Failed! [User:%d], [ItemId:%d]", pUser->m_idPlayer, gfi.dwItemId);
		break;
	}

	case GUILDHOUSE_PCKTTYPE_UPKEEP:
	{
		pUser->AddGold(gfi.dwItemId * GetUpkeepPenya());
		g_DPSrvr.PutPenyaLog(pUser, "f", "GUILDHOUSE_UPKEEP_REPAIR", (int)(gfi.dwItemId) * GetUpkeepPenya() * (-1));		//sun : safecast

		break;
	}
	}
}

BOOL CGuildHouseBase::SendWorldToDatabase(CUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex)
{
	if (!IsValidObj(pUser))
		return FALSE;

	if (IsWaitDBAck())
	{
		pUser->AddDefinedText(TID_GAME_GUILDHOUSE_LATER);
		return FALSE;
	}

	if (!PreCheckPacket(pUser, nPacketType, gfi, nIndex))
		return FALSE;

	SetWaitDBAckPlayerId(pUser->m_idPlayer);
	{
		BEFORESENDDUAL(ar, PACKETTYPE_GUILDHOUSE_PACKET, DPID_UNKNOWN, DPID_UNKNOWN);
		ar << pUser->m_idGuild;
		ar << nPacketType << nIndex;
		gfi.Serialize(ar);
		SEND(ar, &g_dpDBClient, DPID_SERVERPLAYER);
	}

	return TRUE;
}

void CGuildHouseBase::SendWorldToClient(int nPacketType, GH_Fntr_Info& gfi, int nIndex)
{
	CGuild* pGuild = g_GuildMng.GetGuild(m_dwGuildId);
	if (pGuild)
	{
		for (map<u_long, CGuildMember*>::iterator it = pGuild->m_mapPMember.begin(); it != pGuild->m_mapPMember.end(); it++)
		{
			CUser* pUser = static_cast<CUser*>(prj.GetUserByID(it->first));
			if (IsValidObj(pUser))
				pUser->AddGuildHousePakcet(nPacketType, gfi, nIndex);
		}
	}
}

void CGuildHouseBase::SetApplyDST(CUser* pUser)
{
	if (!IsValidObj(pUser))
		return;

	for (int i = 0; i < (int)(m_vecFntInfo.size()); i++)
	{
		if (m_vecFntInfo[i].bSetup)
		{
			ItemProp* pItemProp = prj.GetItemProp(m_vecFntInfo[i].dwItemId);
			if (pItemProp)
			{
				for (int i = 0; i < PROP_BONUSES; i++)
					pUser->SetDestParam(i, pItemProp);
			}
		}
	}
}

void CGuildHouseBase::ResetApplyDST(CUser* pUser)
{
	if (!IsValidObj(pUser))
		return;

	for (int i = 0; i < (int)(m_vecFntInfo.size()); i++)
	{
		if (m_vecFntInfo[i].bSetup)
		{
			ItemProp* pItemProp = prj.GetItemProp(m_vecFntInfo[i].dwItemId);
			if (pItemProp)
			{
				for (int i = 0; i < PROP_BONUSES; i++)
					pUser->ResetDestParam(i, pItemProp);
			}
		}
	}
}

void CGuildHouseBase::SetDSTFunriture(ItemProp* pItemProp)
{
	if (!pItemProp)
		return;

	CGuild* pGuild = g_GuildMng.GetGuild(m_dwGuildId);
	if (pGuild)
	{
		for (map<u_long, CGuildMember*>::iterator it = pGuild->m_mapPMember.begin(); it != pGuild->m_mapPMember.end(); it++)
		{
			CUser* pUser = static_cast<CUser*>(prj.GetUserByID(it->first));
			if (IsValidObj(pUser))
			{
				for (int i = 0; i < PROP_BONUSES; i++)
					pUser->SetDestParam(i, pItemProp);
			}
		}
	}
}

void CGuildHouseBase::ResetDSTFunriture(ItemProp* pItemProp)
{
	if (!pItemProp)
		return;

	CGuild* pGuild = g_GuildMng.GetGuild(m_dwGuildId);
	if (pGuild)
	{
		for (map<u_long, CGuildMember*>::iterator it = pGuild->m_mapPMember.begin(); it != pGuild->m_mapPMember.end(); it++)
		{
			CUser* pUser = static_cast<CUser*>(prj.GetUserByID(it->first));
			if (IsValidObj(pUser))
			{
				for (int i = 0; i < PROP_BONUSES; i++)
					pUser->ResetDestParam(i, pItemProp);
			}
		}
	}
}

BOOL CGuildHouseBase::IsListUpAble(DWORD dwItemId)
{
	if (m_vecFntInfo.size() >= MAX_LISTUP_NUM)
		return FALSE;

	ItemProp* pItemProp = prj.GetItemProp(dwItemId);
	if (!pItemProp)
		return FALSE;

	int nMaxNum = GetMaxListUpNum(pItemProp);
	int nItemCount = 0;
	for (int i = 0; i < (int)(m_vecFntInfo.size()); i++)
	{
		ItemProp* pTargetProp = prj.GetItemProp(m_vecFntInfo[i].dwItemId);
		if (pTargetProp && pItemProp->dwItemKind3 == pTargetProp->dwItemKind3)
			nItemCount++;

		if (nItemCount >= nMaxNum)
			return FALSE;
	}

	return TRUE;
}
#endif // __WORLDSERVER

#ifdef __DBSERVER
int	CGuildHouseBase::GetFirstExpiredFurnitureIndex()
{
	for (int i = 0; i < (int)(m_vecFntInfo.size()); i++)
	{
		if (m_vecFntInfo[i].tKeepTime != 0 && m_vecFntInfo[i].tKeepTime <= time_null())
			return i;
	}

	return NULL_ID;
}
#endif // __DBSERVER

void CGuildHouseBase::SerializeAllInfo(CAr& ar)
{
	if (ar.IsStoring())
	{
		ar << m_dwGuildId << m_dwWorldId;
		if (m_tUpkeepTime > 0)
			ar << m_tUpkeepTime - time_null();
		else
			ar << static_cast<time_t>(0);
		ar << m_vecFntInfo.size();
		for (int i = 0; i < (int)(m_vecFntInfo.size()); i++)
			m_vecFntInfo[i].Serialize(ar);
	}
	else
	{
		ar >> m_dwGuildId >> m_dwWorldId >> m_tUpkeepTime;
		if (m_tUpkeepTime > 0)
			m_tUpkeepTime += time_null();
		int nSize;
		ar >> nSize;
		m_vecFntInfo.clear(); m_vecFntInfo.resize(nSize);
		for (int i = 0; i < nSize; i++)
			m_vecFntInfo[i].Serialize(ar);
	}
}

BOOL CGuildHouseBase::OnGuildHousePacket(int nPacketType, GuildHouse_Furniture_Info& gfi, int nIndex)
{
	switch (nPacketType)
	{
	case GUILDHOUSE_PCKTTYPE_LISTUP:
		return ListUpFurniture(gfi);

	case GUILDHOUSE_PCKTTYPE_LISTDROP:
		return ListDropFurniture(nIndex);

	case GUILDHOUSE_PCKTTYPE_SETUP:
#ifdef __CLIENT
		{
		ItemProp* pProp = prj.GetItemProp(gfi.dwItemId);
		if (pProp)
		{
			CString str;
			str.Format(GETTEXT(TID_GAME_GUILDHOUSE_FUR_INSTALL_SUCCESS), pProp->szName);
			g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_GUILDHOUSE_FUR_INSTALL_SUCCESS));
		}
	}

#endif // __CLIENT
#ifdef __WORLDSERVER
		g_dpDBClient.SendLogGuildFurniture(m_dwGuildId, gfi, 'S');
#endif // __WORLDSERVER
		return SetupFurnitureCtrl(nIndex, gfi);

	case GUILDHOUSE_PCKTTYPE_REMOVE:
#ifdef __CLIENT
		{
		ItemProp* pProp = prj.GetItemProp(gfi.dwItemId);
		if (pProp)
		{
			CString str;
			str.Format(GETTEXT(TID_GAME_GUILDHOUSE_FUR_DISMANTLE_SUCCESS), pProp->szName);
			g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_GUILDHOUSE_FUR_DISMANTLE_SUCCESS));
		}
	}
#endif // __CLIENT
#ifdef __WORLDSERVER
		g_dpDBClient.SendLogGuildFurniture(m_dwGuildId, gfi, 'R');
#endif // __WORLDSERVER
		return RemoveFurnitureCtrl(nIndex, gfi);

	case GUILDHOUSE_PCKTTYPE_RESET:
#ifdef __CLIENT
		{
		ItemProp* pProp = prj.GetItemProp(gfi.dwItemId);
		if (pProp)
		{
			CString str;
			str.Format(GETTEXT(TID_GAME_GUILDHOUSE_FUR_REINSTALL_SUCCESS), pProp->szName);
			g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_GUILDHOUSE_FUR_REINSTALL_SUCCESS));
		}
	}
#endif
#ifdef __WORLDSERVER
		g_dpDBClient.SendLogGuildFurniture(m_dwGuildId, gfi, 'C');
#endif // __WORLDSERVER
		return ResetFurnitureCtrl(nIndex, gfi);
#ifdef __CLIENT
	case GUILDHOUSE_PCKTTYPE_REFRESH:
		return SetupFurnitureCtrl(nIndex, gfi);
#endif // __CLIENT

	case GUILDHOUSE_PCKTTYPE_UPKEEP:
		SetUpkeeptime(gfi.tKeepTime);
		return TRUE;

	case GUILDHOUSE_PCKTTYPE_EXPIRED:
		return ExpiredGuildHouse();
	}

	return FALSE;
	}

BOOL CGuildHouseBase::ListUpFurniture(GuildHouse_Furniture_Info& gfi)
{
	m_vecFntInfo.push_back(gfi);
#ifdef __WORLDSERVER
	g_dpDBClient.SendLogGuildFurniture(m_dwGuildId, gfi, 'I');
#endif // __WORLDSERVER
	return TRUE;
}

BOOL CGuildHouseBase::ListDropFurniture(int nIndex)
{
	if ((int)(m_vecFntInfo.size()) <= nIndex)
		return FALSE;
#ifdef __WORLDSERVER
	if (m_vecFntInfo[nIndex].bSetup == TRUE)
		RemoveFurnitureCtrl(nIndex, m_vecFntInfo[nIndex]);

	g_dpDBClient.SendLogGuildFurniture(m_dwGuildId, m_vecFntInfo[nIndex], 'D');
#endif // __WORLDSERVER
	m_vecFntInfo.erase(m_vecFntInfo.begin() + nIndex);
	return TRUE;
}

BOOL CGuildHouseBase::SetupFurnitureCtrl(int nIndex, GuildHouse_Furniture_Info& gfi)
{
	if (nIndex < 0 || (int)(m_vecFntInfo.size()) <= nIndex || m_vecFntInfo.at(nIndex).dwItemId != gfi.dwItemId)	return FALSE;
	ItemProp* pItemProp = prj.GetItemProp(gfi.dwItemId);
	if (!pItemProp)	return FALSE;

#ifdef __WORLDSERVER
	CWorld* pWorld = g_WorldMng.GetWorld(m_dwWorldId);
	if (pWorld && pWorld->m_linkMap.GetLinkMap(static_cast<int>(m_dwGuildId)))
	{
		if (pWorld->VecInWorld(gfi.vPos) == FALSE)
		{
			return FALSE;
		}

		CCtrl* pCtrl = NULL;
		if (pItemProp->dwItemKind2 == IK2_GUILDHOUSE_NPC)
		{
			pCtrl = static_cast<CCtrl*>(CreateObj(D3DDEVICE, OT_MOVER, pItemProp->dwLinkKind));
			CMover* pMover = static_cast<CMover*>(pCtrl);
			if (pMover)
			{
				lstrcpy(pMover->m_szCharacterKey, pItemProp->szTextFileName);
				pMover->InitNPCProperty();
				pMover->InitCharacter(pMover->GetCharacter());
				pMover->InitMotion(MTI_STAND);
				pMover->UpdateLocalMatrix();
			}
		}
		else if (pItemProp->dwItemKind2 == IK2_GUILDHOUSE_FURNITURE)
			pCtrl = static_cast<CCtrl*>(CreateObj(D3DDEVICE, OT_CTRL, pItemProp->dwLinkKind));

		if (pCtrl)
		{
			pCtrl->SetPos(gfi.vPos);
			pCtrl->SetAngle(gfi.fAngle);
			pCtrl->AddItToGlobalId();
			gfi.objId = pCtrl->GetId();
			if (pWorld->ADDOBJ(pCtrl, FALSE, static_cast<int>(m_dwGuildId)) == FALSE)
			{
				SAFE_DELETE(pCtrl);
				return FALSE;
			}
		}
	}

	if (!m_vecFntInfo[nIndex].bSetup)
		SetDSTFunriture(pItemProp);
#endif // __WORLDSERVER

	m_vecFntInfo[nIndex] = gfi;
	return TRUE;
}

BOOL CGuildHouseBase::RemoveFurnitureCtrl(int nIndex, GuildHouse_Furniture_Info& gfi)
{
	if ((int)(m_vecFntInfo.size()) <= nIndex || m_vecFntInfo[nIndex].dwItemId != gfi.dwItemId)	return FALSE;
	ItemProp* pItemProp = prj.GetItemProp(m_vecFntInfo[nIndex].dwItemId);
	if (!pItemProp)	return FALSE;

#ifdef __WORLDSERVER
	CWorld* pWorld = g_WorldMng.GetWorld(m_dwWorldId);
	if (pWorld && pWorld->m_linkMap.GetLinkMap(static_cast<int>(m_dwGuildId)))
	{

		CCtrl* pCtrl = prj.GetCtrl(m_vecFntInfo[nIndex].objId);
		if (pCtrl)
			pCtrl->Delete();
	}

	if (m_vecFntInfo[nIndex].bSetup)
		ResetDSTFunriture(pItemProp);
#endif // __WORLDSERVER
	m_vecFntInfo[nIndex] = gfi;
	return TRUE;
}

BOOL CGuildHouseBase::ResetFurnitureCtrl(int nIndex, GuildHouse_Furniture_Info& gfi)
{
	if (nIndex < 0 || (int)(m_vecFntInfo.size()) <= nIndex || m_vecFntInfo.at(nIndex).dwItemId != gfi.dwItemId)	return FALSE;
	ItemProp* pItemProp = prj.GetItemProp(gfi.dwItemId);
	if (!pItemProp)	return FALSE;
	if (pItemProp->dwItemKind3 == IK3_TELEPORTER)	return FALSE;

#ifdef __WORLDSERVER
	CWorld* pWorld = g_WorldMng.GetWorld(m_dwWorldId);
	if (pWorld && pWorld->m_linkMap.GetLinkMap(static_cast<int>(m_dwGuildId)))
	{
		CCtrl* pCtrl = prj.GetCtrl(m_vecFntInfo[nIndex].objId);
		if (pCtrl)
		{
			pCtrl->Delete();

			pCtrl = (CCtrl*)CreateObj(D3DDEVICE, OT_CTRL, pItemProp->dwLinkKind);
			if (pCtrl)
			{
				pCtrl->SetPos(gfi.vPos);
				pCtrl->SetAngle(gfi.fAngle);
				pCtrl->AddItToGlobalId();
				gfi.objId = pCtrl->GetId();
				if (pWorld->ADDOBJ(pCtrl, FALSE, static_cast<int>(m_dwGuildId)) == FALSE)
				{
					SAFE_DELETE(pCtrl);
				}
			}
		}
	}
#endif // __WORLDSERVER
	m_vecFntInfo[nIndex] = gfi;
	return TRUE;
}

BOOL CGuildHouseBase::ExpiredGuildHouse()
{
	for (int i = 0; i < (int)(m_vecFntInfo.size()); i++)
	{
		if (m_vecFntInfo[i].bSetup)
		{
			RemoveFurnitureCtrl(i, m_vecFntInfo[i]);
			m_vecFntInfo[i].bSetup = FALSE;
			m_vecFntInfo[i].objId = NULL_ID;
		}
	}
	m_tUpkeepTime = 0;
#ifdef __WORLDSERVER
	DestroyGuildHouseRoom();
#endif // __WORLDSERVER
	return TRUE;
}


#ifndef __CLIENT
////////////////////////////////////////////////////////////////
// CGuildHouseSmall : CGuildBase
////////////////////////////////////////////////////////////////
CGuildHouseSmall::CGuildHouseSmall(DWORD dwGuildId)
	: CGuildHouseBase(dwGuildId, WI_GUILDHOUSE_SMALL)
{
}

CGuildHouseSmall::~CGuildHouseSmall()
{
}


#ifdef __WORLDSERVER
int	CGuildHouseSmall::GetMaxListUpNum(ItemProp* pItemProp)
{
	if (!pItemProp)
		return 0;

	return MAX_LISTUP_NUM;
}


BOOL CGuildHouseSmall::IsSetupAble(CUser* pUser, int nIndex)
{
	if (!IsValidObj(pUser))
		return FALSE;

	ItemProp* pItemProp = prj.GetItemProp(m_vecFntInfo[nIndex].dwItemId);
	if (!pItemProp)
		return FALSE;

	if (pItemProp->dwAbilityMax > 1)
	{
		pUser->AddDefinedText(TID_GAME_GUILDHOUSE_FUR_INSTALL_LEVEL, "%d", pItemProp->dwAbilityMax);
		return FALSE;
	}

	int nMaxSetupNum = 0;
	switch (pItemProp->dwItemKind3)
	{
	case IK3_CHAIR:	nMaxSetupNum = 4;	break;
	default:		nMaxSetupNum = 1;	break;
	}

	int nSetupNum = 0;
	for (int i = 0; i < (int)(m_vecFntInfo.size()); i++)
	{
		if (!m_vecFntInfo[i].bSetup)
			continue;

		ItemProp* pSetup = prj.GetItemProp(m_vecFntInfo[i].dwItemId);
		if (pItemProp->dwItemKind3 == pSetup->dwItemKind3)
			nSetupNum++;

		if (nSetupNum >= nMaxSetupNum)
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_INSIDE_INSTALL_KIND, "%d", nMaxSetupNum);
			return FALSE;
		}
	}

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return FALSE;

	if (pWorld->GetID() != WI_GUILDHOUSE_SMALL && pWorld->GetID() != WI_GUILDHOUSE_MIDDLE && pWorld->GetID() != WI_GUILDHOUSE_LARGE)
		return FALSE;

	if (m_vecFntInfo[nIndex].bSetup)
	{
		if (pItemProp->dwItemKind3 != IK3_WALLPAPER && pItemProp->dwItemKind3 != IK3_CARPET)
		{
			if (!pWorld->VecInWorld(m_vecFntInfo[nIndex].vPos))
				return FALSE;
		}
	}

	return TRUE;
}
#endif // __WORLDSERVER

////////////////////////////////////////////////////////////////
// CGuildHouseMiddle : CGuildBase
////////////////////////////////////////////////////////////////
CGuildHouseMiddle::CGuildHouseMiddle(DWORD dwGuildId)
	: CGuildHouseBase(dwGuildId, WI_GUILDHOUSE_MIDDLE)
{
}

CGuildHouseMiddle::~CGuildHouseMiddle()
{
}



/*
////////////////////////////////////////////////////////////////
// CGuildHouseLarge : CGuildBase
////////////////////////////////////////////////////////////////
CGuildHouseLarge::CGuildHouseLarge( DWORD dwGuildId )
: CGuildHouseBase( dwGuildId, WI_GUILDHOUSE_LARGE )
{
}

CGuildHouseLarge::~CGuildHouseLarge()
{
}
*/



////////////////////////////////////////////////////////////////
// CGuildHouseMng
////////////////////////////////////////////////////////////////
CGuildHouseMng::CGuildHouseMng()
#ifdef __DBSERVER
	: m_nSeqNum(0)
#endif // __DBSERVER

{
}

CGuildHouseMng::~CGuildHouseMng()
{
	for (MapGuildHouse::iterator it = m_mapGuildHouse.begin(); it != m_mapGuildHouse.end(); it++)
		delete it->second;
	m_mapGuildHouse.clear();
}

CGuildHouseMng* CGuildHouseMng::GetInstance()
{
	static CGuildHouseMng sGHM;
	return &sGHM;
}

CGuildHouseBase* CGuildHouseMng::MakeGuildHouse(DWORD dwGuildId, DWORD dwWorldId)
{
	switch (dwWorldId)
	{
	case WI_GUILDHOUSE_SMALL:	return new CGuildHouseSmall(dwGuildId);
	case WI_GUILDHOUSE_MIDDLE:	return new CGuildHouseMiddle(dwGuildId);

	}

	return NULL;
}

BOOL CGuildHouseMng::RemoveGuildHouse(DWORD dwGuildId)
{
	MapGuildHouse::iterator it = m_mapGuildHouse.find(dwGuildId);
	if (it == m_mapGuildHouse.end())
		return FALSE;

	SAFE_DELETE(it->second);
	m_mapGuildHouse.erase(it);
	return TRUE;
}

CGuildHouseBase* CGuildHouseMng::GetGuildHouse(DWORD dwGuildId)
{
	MapGuildHouse::iterator it = m_mapGuildHouse.find(dwGuildId);
	if (it == m_mapGuildHouse.end())
		return NULL;

	return it->second;
}

void CGuildHouseMng::Serialize(CAr& ar)
{
	if (ar.IsStoring())
	{
		ar << m_mapGuildHouse.size();
		for (MapGuildHouse::iterator it = m_mapGuildHouse.begin(); it != m_mapGuildHouse.end(); it++)
		{
			ar << it->first << it->second->GetType();
			it->second->SerializeAllInfo(ar);
		}
	}
	else
	{
		m_mapGuildHouse.clear();
		int nSize;
		DWORD dwGuildId, dwType;
		ar >> nSize;
		for (int i = 0; i < nSize; i++)
		{
			ar >> dwGuildId >> dwType;
			CGuildHouseBase* pGuildHouse = MakeGuildHouse(dwGuildId, dwType);

			if (pGuildHouse && AddGuildHouse(dwGuildId, pGuildHouse))
				pGuildHouse->SerializeAllInfo(ar);
			else
			{
				SAFE_DELETE(pGuildHouse);
				CGuildHouseSmall temp(NULL_ID);
				temp.SerializeAllInfo(ar);
			}
		}
	}
}

#ifdef __WORLDSERVER
BOOL CGuildHouseMng::IsBuyAble(CUser* pUser)
{
	if (!IsValidObj(pUser))
		return FALSE;

	CGuild* pGuild = pUser->GetGuild();
	if (pGuild)
	{
		if (!pGuild->IsMaster(pUser->m_idPlayer))
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_BUY_MASTER);
			return FALSE;
		}

		if (pGuild->m_nLevel < BUY_MIN_LEVEL)
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_BUY_LEVEL, "%d", BUY_MIN_LEVEL);
			return FALSE;
		}

		if (pGuild->GetSize() < BUY_MIN_MEMBERSIZE)
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_BUY_POPULATION, "%d", BUY_MIN_MEMBERSIZE);
			return FALSE;
		}

		if (pUser->CheckUserGold(BUY_PENYA, false) == false)
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_BUY_EXPENSE);
			return FALSE;
		}

		if (m_mapGuildHouse.find(pGuild->GetGuildId()) != m_mapGuildHouse.end())
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_BUY_HAVE);
			return FALSE;
		}
	}
	else
	{
		pUser->AddDefinedText(TID_GAME_GUILDHOUSE_BUY_MASTER);
		return FALSE;
	}

	return TRUE;
}

void CGuildHouseMng::ReqBuyGuildHouse(CUser* pUser)
{
	if (IsBuyAble(pUser))
	{
		pUser->AddGold(-BUY_PENYA);
		g_DPSrvr.PutPenyaLog(pUser, "f", "GUILDHOUSE_BUY", BUY_PENYA);

		BEFORESENDDUAL(ar, PACKETTYPE_GUILDHOUSE_BUY, DPID_UNKNOWN, DPID_UNKNOWN);
		ar << pUser->m_idPlayer << pUser->m_idGuild;
		SEND(ar, &g_dpDBClient, DPID_SERVERPLAYER);
	}
}

void CGuildHouseMng::OnBuyGuildHouse(CAr& ar)
{
	BOOL bResult;
	DWORD dwPlayerId, dwGuildId;

	ar >> dwPlayerId >> dwGuildId >> bResult;
	if (bResult)
	{
		CGuildHouseBase* pGuildHouse = MakeGuildHouse(dwGuildId, WI_GUILDHOUSE_SMALL);
		if (pGuildHouse && AddGuildHouse(dwGuildId, pGuildHouse))
		{
			pGuildHouse->SerializeAllInfo(ar);
			CUser* pUser = static_cast<CUser*>(prj.GetUserByID(dwPlayerId));
			//if( IsValidObj( pUser ) )
			//	pUser->AddDefinedText( TID_GAME_GUILDHOUSE_PURCHASE );

			CGuild* pGuild = g_GuildMng.GetGuild(dwGuildId);
			if (pGuild)
			{
				for (map<u_long, CGuildMember*>::iterator it = pGuild->m_mapPMember.begin(); it != pGuild->m_mapPMember.end(); it++)
				{
					pUser = static_cast<CUser*>(prj.GetUserByID(it->first));
					if (IsValidObj(pUser))
					{
						pUser->AddGuildHouseAllInfo(pGuildHouse);
						pUser->AddDefinedText(TID_GAME_GUILDHOUSE_PURCHASE);
						pGuildHouse->SetApplyDST(pUser);
					}
				}
			}
		}
		else
		{
			SAFE_DELETE(pGuildHouse);
			Error("CGuildHouseMng::BuyGuildHouse - CreateGuildHouse Failed!!! [User:%07d, Guild:%06d]", dwPlayerId, dwGuildId);
			CGuildHouseSmall temp(NULL_ID);
			temp.SerializeAllInfo(ar);
		}
	}
	else
	{
		CUser* pUser = static_cast<CUser*>(prj.GetUserByID(dwPlayerId));
		if (IsValidObj(pUser))
		{
			pUser->AddGold(BUY_PENYA);

			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_BUY_HAVE);
		}
	}
}

void CGuildHouseMng::OnRemoveGuildHouse(CAr& ar)
{
	DWORD dwGuildId;
	ar >> dwGuildId;

	RemoveGuildHouse(dwGuildId);

	CGuild* pGuild = g_GuildMng.GetGuild(dwGuildId);
	if (pGuild)
	{
		for (map<u_long, CGuildMember*>::iterator it = pGuild->m_mapPMember.begin(); it != pGuild->m_mapPMember.end(); it++)
		{
			CUser* pUser = static_cast<CUser*>(prj.GetUserByID(it->first));
			if (IsValidObj(pUser))
			{
				pUser->AddGuildHouseRemvoe();
			}
		}
	}
}

BOOL CGuildHouseMng::SendWorldToDatabase(CUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex)
{
	if (!IsValidObj(pUser))
		return FALSE;

	CGuildHouseBase* pGuildHouse = GetGuildHouse(pUser->m_idGuild);
	if (pGuildHouse)
		return pGuildHouse->SendWorldToDatabase(pUser, nPacketType, gfi, nIndex);

	return FALSE;
}

BOOL CGuildHouseMng::IsGuildHouse(DWORD dwWorldId)
{
	switch (dwWorldId)
	{
	case WI_GUILDHOUSE_SMALL:
	case WI_GUILDHOUSE_MIDDLE:
	case WI_GUILDHOUSE_LARGE:
		return TRUE;
	}

	return FALSE;
}

BOOL CGuildHouseMng::IsLoginAble(CUser* pUser, DWORD dwWorldId, int nLayer)
{
	if (!IsGuildHouse(dwWorldId))
		return TRUE;

	if (pUser->m_idGuild != nLayer)
		return FALSE;

	CGuildHouseBase* pGuildHouse = GetGuildHouse(pUser->m_idGuild);
	if (pGuildHouse)
		return pGuildHouse->IsEnteranceAble(pUser);

	return FALSE;
}

BOOL CGuildHouseMng::EnteranceGuildHouse(CUser* pUser, DWORD dwComebackItemId)
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse(pUser->m_idGuild);
	if (!pGuildHouse)
	{
		pUser->AddDefinedText(TID_GAME_GUILDHOUSE_ENTER_HAVE);
		return FALSE;
	}

	switch (dwComebackItemId)
	{
	case NULL_ID:
	{
		if (CNpcChecker::GetInstance()->IsCloseNpc(MMI_GUILDHOUSE_ENTER, pUser->GetWorld(), pUser->GetPos()))
			pUser->SetMarkingPos();
		else
			return FALSE;
		break;
	}

	default:
	{
		if (pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MADRIGAL)
		{
			pUser->m_idMarkingWorld = WI_WORLD_MADRIGAL;
			pUser->m_vMarkingPos = D3DXVECTOR3(6968.0f, 0.0f, 3328.8f);
		}
		else
		{
			pUser->AddDefinedText(TID_GAME_GUILDHOUSE_COMEBACK_ERROR01);
			return FALSE;
		}
		break;
	}
	}

	return pGuildHouse->EnteranceGuildHouseRoom(pUser);
}

void CGuildHouseMng::GoOutGuildHouse(CUser* pUser)
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse(pUser->m_idGuild);
	if (pGuildHouse)
		pGuildHouse->GoOutGuildHouseRoom(pUser);
}

void CGuildHouseMng::CheckDestroyGuildHouse(CUser* pUser)
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse(pUser->m_idGuild);
	if (pGuildHouse)
		pGuildHouse->CheckDestroyRoom(pUser);
}

void CGuildHouseMng::SetApplyDST(CUser* pUser)
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse(pUser->m_idGuild);
	if (pGuildHouse)
		pGuildHouse->SetApplyDST(pUser);
}

void CGuildHouseMng::ResetApplyDST(CUser* pUser)
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse(pUser->m_idGuild);
	if (pGuildHouse)
		pGuildHouse->ResetApplyDST(pUser);
}

#endif // __WORLDSERVER

#ifdef __DBSERVER
extern	APP_INFO	g_appInfo;
void CGuildHouseMng::ProcessExpired()
{
	for (MapGuildHouse::iterator it = m_mapGuildHouse.begin(); it != m_mapGuildHouse.end(); it++)
	{
		int nIndex = it->second->GetFirstExpiredFurnitureIndex();
		if (nIndex != NULL_ID)
		{
			CAr ar;
			ar << it->first << GUILDHOUSE_PCKTTYPE_LISTDROP << nIndex;
			it->second->GetFurnitureInfo(nIndex).Serialize(ar);
			int nBufSize;
			LPBYTE lpBuf = ar.GetBuffer(&nBufSize);
			GuildHouseDBMng->PostRequest(GUILDHOUSE_DEFAULT_PACKET, lpBuf, nBufSize);
		}

		if (it->second->GetUpkeepTime() <= time_null())
		{
			if (it->second->GetUpkeepTime() > 0)
			{
				GH_Fntr_Info gfiTemp;
				CAr ar;
				ar << it->first << GUILDHOUSE_PCKTTYPE_EXPIRED << NULL_ID;
				gfiTemp.Serialize(ar);
				int nBufSize;
				LPBYTE lpBuf = ar.GetBuffer(&nBufSize);
				GuildHouseDBMng->PostRequest(GUILDHOUSE_DEFAULT_PACKET, lpBuf, nBufSize);
			}
		}
	}
}

BOOL CGuildHouseMng::DBLoadGuildHouse(CQuery* pQuery)
{
	if (pQuery->Execute("usp_GuildHouse_MaxSEQ '%02d'", g_appInfo.dwSys))
	{
		if (pQuery->Fetch())
			m_nSeqNum = pQuery->GetInt("SEQ");
	}
	else
		return FALSE;

	if (pQuery->Execute("usp_GuildHouse_Load '%02d'", g_appInfo.dwSys))
	{
		while (pQuery->Fetch())
		{
			DWORD dwGuildId = pQuery->GetInt("m_idGuild");
			DWORD dwWorldId = pQuery->GetInt("dwWorldID");
			time_t tKeepTime = pQuery->GetInt("tKeepTime");

			CGuildHouseBase* pGuildHouse = MakeGuildHouse(dwGuildId, dwWorldId);
			if (pGuildHouse && AddGuildHouse(dwGuildId, pGuildHouse))
			{
				pGuildHouse->SetUpkeeptime(tKeepTime);
			}
			else
				Error("CGuildHouseMng::DBLoadGuildHouse() : AddGuildHouse() Failed!!! [GuildId:%06d]", dwGuildId);
		}
	}
	else
		return FALSE;

	for (MapGuildHouse::iterator it = m_mapGuildHouse.begin(); it != m_mapGuildHouse.end(); it++)
	{
		if (pQuery->Execute("usp_GuildFurniture_Load  '%02d', '%06d'", g_appInfo.dwSys, it->first))
		{
			while (pQuery->Fetch())
			{
				GH_Fntr_Info gfiData;
				gfiData.nSeqNum = pQuery->GetInt("SEQ");
				gfiData.dwItemId = static_cast<DWORD>(pQuery->GetInt("ItemIndex"));
				gfiData.bSetup = static_cast<BOOL>(pQuery->GetInt("bSetup"));
				gfiData.vPos.x = static_cast<float>(pQuery->GetFloat("x_Pos"));
				gfiData.vPos.y = static_cast<float>(pQuery->GetFloat("y_Pos"));
				gfiData.vPos.z = static_cast<float>(pQuery->GetFloat("z_Pos"));
				gfiData.fAngle = static_cast<float>(pQuery->GetFloat("fAngle"));
				gfiData.tKeepTime = pQuery->GetInt("tKeepTime");
				it->second->OnGuildHousePacket(GUILDHOUSE_PCKTTYPE_LISTUP, gfiData, NULL_ID);
			}
		}
		else
			return FALSE;
	}

	return TRUE;
}
#endif // \__DBSERVER


#endif // n__CLIENT

#endif // __GUILD_HOUSE