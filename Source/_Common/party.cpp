#include "stdafx.h"
#include "defineText.h"
#include "defineSkill.h"
#include "party.h"

#ifdef __CORESERVER
#include "dpcoresrvr.h"
extern	CDPCoreSrvr g_dpCoreSrvr;
#include "player.h"
extern	CPlayerMng	g_PlayerMng;
#endif // __CORESERVER

#ifdef __WORLDSERVER
#include "worldmng.h"
#include "User.h"
extern	CUserMng	g_UserMng;
extern CWorldMng   g_WorldMng;
#include "dpcoreclient.h"
extern	CDPCoreClient	g_DPCoreClient;
#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
#if __VER >= 12 // __LORD
#include "slord.h"
#endif	// __LORD
#endif	// __WORLDSERVER

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef __VM_0819
MemPooler<CParty>* CParty::m_pPool = new MemPooler<CParty>(512, "CParty");
#else	// __VM_0819
MemPooler<CParty>* CParty::m_pPool = new MemPooler<CParty>(512);
#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonParty.h"
#endif // __INSTANCE_DUNGEON
/*--------------------------------------------------------------------------------*/

CParty::CParty()
{
	m_uPartyId = 0;
	m_nSizeofMember = 0;
	memset(m_sParty, 0, sizeof(m_sParty));
	m_nLevel = 1;
	m_nExp = m_nPoint = 0;
	m_nTroupsShareExp = m_nTroupeShareItem = 0;
#ifdef __FL_PARTY_FINDER
	m_bAllowed = TRUE;
#endif
	m_nKindTroup = 0;
	m_nReferens = 0;
	m_nGetItemPlayerId = 0;
	for (int i = 0; i < MAX_PARTYMODE; i++)
	{
		m_nModeTime[i] = 0;
	}
#ifdef __WORLDSERVER
	m_dwWorldId = 0;
#endif // __WORLDSERVER
}

CParty::~CParty()
{
}

void CParty::InitParty()
{
	m_uPartyId = 0;
	m_nSizeofMember = 0;
	memset(m_sParty, 0, sizeof(m_sParty));
	m_nLevel = 1;
	m_nExp = m_nPoint = 0;
	m_nTroupsShareExp = m_nTroupeShareItem = 0;
#ifdef __FL_PARTY_FINDER
	m_bAllowed = TRUE;
#endif
	m_nKindTroup = 0;
	m_nReferens = 0;
	for (int i = 0; i < MAX_PTMEMBER_SIZE; i++)
	{
		m_aMember[i].m_uPlayerId = 0;
		m_aMember[i].m_tTime = CTime::GetCurrentTime();
		m_aMember[i].m_bRemove = FALSE;
	}
}

#ifndef __CORESERVER
CMover* CParty::GetLeader(void)
{
#ifdef __WORLDSERVER
	CMover* pLeader = (CMover*)g_UserMng.GetUserByPlayerID(m_aMember[0].m_uPlayerId);
	return pLeader;
#else
	CMover* pLeader = (CMover*)prj.GetUserByID(m_aMember[0].m_uPlayerId);
	return pLeader;
#endif
}
#endif // __CORESERVER

int CParty::FindMember(u_long uPlayerId)
{
	for (int i = 0; i < m_nSizeofMember; i++)
		if (m_aMember[i].m_uPlayerId == uPlayerId)
			return i;
	return -1;
}

BOOL CParty::NewMember(u_long uPlayerId)
{

	if (IsMember(uPlayerId) == FALSE && m_nSizeofMember < MAX_PTMEMBER_SIZE)
	{
		m_aMember[m_nSizeofMember].m_uPlayerId = uPlayerId;
		m_aMember[m_nSizeofMember].m_bRemove = FALSE;
		m_nSizeofMember++;
		return TRUE;
	}
	return FALSE;
}

BOOL CParty::DeleteMember(u_long uPlayerId)
{
	int Findid = FindMember(uPlayerId);
	if (Findid >= 0)
	{
#ifndef __FL_PARTY_SORT
		for (int i = Findid; i < m_nSizeofMember - 1; i++)
		{
			memcpy(&m_aMember[i], &m_aMember[i + 1], sizeof(PartyMember));
		}
#else
		PartyMember PartyMemberBuf;
		for (int i = Findid; i < m_nSizeofMember - 1; i++)
		{
			memcpy(&m_aMember[i], &m_aMember[i + 1], sizeof(PartyMember));
			memcpy(&m_aMember[i + 1], &PartyMemberBuf, sizeof(PartyMember));
		}
#endif

		m_nSizeofMember--;
#ifdef __WORLDSERVER
		CInstanceDungeonParty::GetInstance()->SetPartyLeaveTime(uPlayerId);
#endif // __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

void CParty::ChangeLeader(u_long uLeaderId)
{
	int changeIndex = FindMember(uLeaderId);
	SwapPartyMember(0, changeIndex);
}

void CParty::Serialize(CAr& ar)
{
	if (ar.IsStoring())
	{
		ar << m_uPartyId << m_nKindTroup << m_nSizeofMember;
		ar << m_nLevel << m_nExp << m_nPoint;
		ar << m_nTroupsShareExp << m_nTroupeShareItem;
#ifdef __FL_PARTY_FINDER
		ar << m_bAllowed;
#endif
		for (int i = 0; i < MAX_PARTYMODE; i++)
		{
			ar << m_nModeTime[i];
		}
		if (m_nKindTroup)
			ar.WriteString(m_sParty);
		for (int i = 0; i < m_nSizeofMember; i++)
		{
			ar << m_aMember[i].m_uPlayerId;
			ar << m_aMember[i].m_bRemove;
		}
	}
	else
	{
		ar >> m_uPartyId >> m_nKindTroup >> m_nSizeofMember;
		ar >> m_nLevel >> m_nExp >> m_nPoint;
		ar >> m_nTroupsShareExp >> m_nTroupeShareItem;
#ifdef __FL_PARTY_FINDER
		ar >> m_bAllowed;
#endif
		for (int i = 0; i < MAX_PARTYMODE; i++)
		{
			ar >> m_nModeTime[i];
		}
		if (m_nKindTroup)
			ar.ReadString(m_sParty, 33);
		for (int i = 0; i < m_nSizeofMember; i++)
		{
			ar >> m_aMember[i].m_uPlayerId;
			ar >> m_aMember[i].m_bRemove;
		}
	}
}

void CParty::SwapPartyMember(int first, int Second)
{
	PartyMember PartyMemberBuf;

	memcpy(&PartyMemberBuf, &m_aMember[first], sizeof(PartyMember));
	memcpy(&m_aMember[first], &m_aMember[Second], sizeof(PartyMember));
	memcpy(&m_aMember[Second], &PartyMemberBuf, sizeof(PartyMember));
}

int CParty::GetPartyModeTime(int nMode)
{
	return m_nModeTime[nMode];
}
void CParty::SetPartyMode(int nMode, DWORD dwSkillTime, int nCachMode)
{
	if (nCachMode == 1)
		m_nModeTime[nMode] += (int)dwSkillTime;
	else
		m_nModeTime[nMode] = (int)dwSkillTime;
}

#ifdef __WORLDSERVER
void CParty::SetPartyLevel(CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp)
{
	g_DPCoreClient.SendPartyLevel(pUser, dwLevel, dwPoint, dwExp);
}
#endif // __WORLDSERVER

void CParty::GetPoint(int nTotalLevel, int nMemberSize, int nDeadLeavel)
{
#ifdef __WORLDSERVER
	if ((nTotalLevel / nMemberSize) - nDeadLeavel < 5)
	{
		BOOL bExpResult = TRUE;
		BOOL bSuperLeader = FALSE;
#if __VER >= 12
		BOOL bLeaderSMExpUp = FALSE;
#endif
		if (m_nKindTroup == 0 && m_nLevel >= MAX_PARTYLEVEL)
			bExpResult = FALSE;

		CMover* pMover = GetLeader();
		if (pMover && pMover->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SUPERLEADERPARTY))
		{
			bSuperLeader = TRUE;
		}

		if (pMover && (pMover->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01) || pMover->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP02)
			|| pMover->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01_01)))
		{
			bLeaderSMExpUp = TRUE;
		}


		if (bExpResult)
			g_DPCoreClient.SendAddPartyExp(m_uPartyId, nDeadLeavel, bSuperLeader, bLeaderSMExpUp);
	}
#endif // __WORLDSERVER
}

void CParty::DoUsePartySkill(u_long uPartyId, u_long uLeaderid, int nSkill)
{
#ifdef __WORLDSERVER
	CUser* pMember = NULL;
	int		i;

	if (IsLeader(uLeaderid) && m_nKindTroup == 1)
	{
		ItemProp* pItemProp = prj.GetPartySkill(nSkill);
		if (pItemProp)
		{
#ifndef __PARTYDEBUG
			if (int(GetLevel() - pItemProp->dwReqDisLV) >= 0)
#endif // __PARTYDEBUG
			{
#ifndef __PARTYDEBUG
#if __VER >= 12
				CUser* pLeadertmp = g_UserMng.GetUserByPlayerID(m_aMember[0].m_uPlayerId);
				if (IsValidObj(pLeadertmp) == FALSE)
					return;
				int nHasCashSkill = 0, nFPoint = 0;
				if (pLeadertmp->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01)
					|| pLeadertmp->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02)
					|| pLeadertmp->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01_01))
				{
					if (nSkill == ST_LINKATTACK
						|| nSkill == ST_FORTUNECIRCLE
						|| nSkill == ST_STRETCHING
						|| nSkill == ST_GIFTBOX)
						nHasCashSkill = 1;
				}

				DWORD dwSkillTime = pItemProp->dwSkillTime;
				int nRemovePoint = pItemProp->dwExp;

				nFPoint = int(GetPoint() - pItemProp->dwExp);
#if __VER >= 12 // __LORD



				if (CSLord::Instance()->IsLord(uLeaderid))
					dwSkillTime *= 4;
#endif	// __LORD

				if (nFPoint >= 0)
#else
				if (int(GetPoint() - pItemProp->dwExp) >= 0)
#endif
#endif // __PARTYDEBUG
				{
					switch (nSkill)
					{
					case ST_CALL:
					{
						g_DPCoreClient.SendRemovePartyPoint(uPartyId, pItemProp->dwExp);
						CUser* pLeader = g_UserMng.GetUserByPlayerID(m_aMember[0].m_uPlayerId);
						if (IsValidObj(pLeader) == FALSE)
							break;


						for (i = 0; i < m_nSizeofMember; i++)
						{
							pMember = g_UserMng.GetUserByPlayerID(m_aMember[i].m_uPlayerId);

							if (IsValidObj((CObj*)pMember))
								pMember->AddPartySkillCall(pLeader->GetPos());
						}
					}
					break;
					case ST_BLITZ:
					{
						CUser* pLeader = g_UserMng.GetUserByPlayerID(m_aMember[0].m_uPlayerId);
						if (IsValidObj(pLeader) == FALSE)
							break;

						if (pLeader->m_idSetTarget != NULL_ID)
						{
							CMover* pT = prj.GetMover(pLeader->m_idSetTarget);
							if (pT && !pT->IsPlayer())
							{
								g_DPCoreClient.SendRemovePartyPoint(uPartyId, pItemProp->dwExp);

								for (i = 0; i < m_nSizeofMember; i++)
								{
									pMember = g_UserMng.GetUserByPlayerID(m_aMember[i].m_uPlayerId);
									if (IsValidObj((CObj*)pMember))
									{
										if (m_nModeTime[PARTY_GIFTBOX_MODE] || m_nModeTime[PARTY_FORTUNECIRCLE_MODE])
										{
											pMember->AddPartySkillBlitz(pLeader->m_idSetTarget);
										}
										else
										{
											if (pLeader->IsNearPC(pMember))
												pMember->AddPartySkillBlitz(pLeader->m_idSetTarget);
										}
									}
								}
							}
							else
							{
								pLeader->AddSendErrorParty(ERROR_NOTTARGET, ST_BLITZ);
							}
						}
						else
						{
							pLeader->AddSendErrorParty(ERROR_NOTTARGET, ST_BLITZ);

						}
					}
					break;
					case ST_RETREAT:
					{
						g_DPCoreClient.SendRemovePartyPoint(uPartyId, pItemProp->dwExp);

						CUser* pLeader = g_UserMng.GetUserByPlayerID(m_aMember[0].m_uPlayerId);
						if (IsValidObj(pLeader) == FALSE)
							break;

						for (i = 0; i < m_nSizeofMember; i++)
						{
							pMember = g_UserMng.GetUserByPlayerID(m_aMember[i].m_uPlayerId);
							if (IsValidObj((CObj*)pMember))
							{
								if (m_nModeTime[PARTY_GIFTBOX_MODE] || m_nModeTime[PARTY_FORTUNECIRCLE_MODE])
								{
									pMember->AddHdr(pMember->GetId(), SNAPSHOTTYPE_PARTYSKILL_RETREAT);
								}
								else
								{
									if (pLeader->IsNearPC(pMember))
										pMember->AddHdr(pMember->GetId(), SNAPSHOTTYPE_PARTYSKILL_RETREAT);
								}
							}
						}
					}
					break;
					case ST_SPHERECIRCLE:
					{

						CUser* pLeader = g_UserMng.GetUserByPlayerID(m_aMember[0].m_uPlayerId);
						if (IsValidObj(pLeader) == FALSE)
							break;

						if (pLeader->m_idSetTarget != NULL_ID)
						{
							CMover* pT = prj.GetMover(pLeader->m_idSetTarget);
							if (pT && !pT->IsPlayer())
							{
								g_DPCoreClient.SendRemovePartyPoint(uPartyId, pItemProp->dwExp);
								for (i = 0; i < m_nSizeofMember; i++)
								{
									pMember = g_UserMng.GetUserByPlayerID(m_aMember[i].m_uPlayerId);
									if (IsValidObj((CObj*)pMember))
									{
										if (m_nModeTime[PARTY_GIFTBOX_MODE] || m_nModeTime[PARTY_FORTUNECIRCLE_MODE])
										{
											pMember->AddHdr(pLeader->m_idSetTarget, SNAPSHOTTYPE_PARTYSKILL_SPHERECIRCLE);
											pMember->m_dwFlag |= MVRF_CRITICAL;
										}
										else
										{
											if (pLeader->IsNearPC(pMember))
											{
												pMember->AddHdr(pLeader->m_idSetTarget, SNAPSHOTTYPE_PARTYSKILL_SPHERECIRCLE);
												pMember->m_dwFlag |= MVRF_CRITICAL;
											}
										}
									}
								}
							}
							else
							{
								pLeader->AddSendErrorParty(ERROR_NOTTARGET, ST_SPHERECIRCLE);

							}
						}
						else
						{
							pLeader->AddSendErrorParty(ERROR_NOTTARGET, ST_SPHERECIRCLE);

						}
						//							g_DPCoreClient.SendSetPartyExp( uLeaderid, m_nPoint );
					}
					break;

					case ST_LINKATTACK:
					{

						g_DPCoreClient.SendUserPartySkill(uLeaderid, PARTY_LINKATTACK_MODE, dwSkillTime, nRemovePoint, nHasCashSkill);
					}
					break;
					case ST_FORTUNECIRCLE:
					{

						g_DPCoreClient.SendUserPartySkill(uLeaderid, PARTY_FORTUNECIRCLE_MODE, dwSkillTime, nRemovePoint, nHasCashSkill);
					}
					break;
					case ST_STRETCHING:
					{

						g_DPCoreClient.SendUserPartySkill(uLeaderid, PARTY_STRETCHING_MODE, dwSkillTime, nRemovePoint, nHasCashSkill);
					}
					break;
					case ST_GIFTBOX:
					{

						g_DPCoreClient.SendUserPartySkill(uLeaderid, PARTY_GIFTBOX_MODE, dwSkillTime, nRemovePoint, nHasCashSkill);
					}
					break;
					default:
						break;
					}
				}
#ifndef __PARTYDEBUG
				else
				{
					CUser* pLeader = g_UserMng.GetUserByPlayerID(m_aMember[0].m_uPlayerId);
					if (IsValidObj(pLeader))
					{
						pLeader->AddSendErrorParty(ERROR_NOTPARTYPOINT);
					}


				}
#endif // __PARTYDEBUG
			}
#ifndef __PARTYDEBUG
			else
			{
				CUser* pLeader = g_UserMng.GetUserByPlayerID(m_aMember[0].m_uPlayerId);
				if (IsValidObj(pLeader))
				{
					pLeader->AddSendErrorParty(ERROR_NOTPARTYSKILL);
				}

			}
#endif // __PARTYDEBUG
		}
	}
	else
	{

	}
#endif	// __WORLDSERVER
}

#ifdef __WORLDSERVER
void CParty::DoUsePartyReCall(u_long uPartyId, u_long uLeaderid, int nSkill)
{
	CUser* pLeaderUser = g_UserMng.GetUserByPlayerID(uLeaderid);
	if (!IsValidObj((CObj*)pLeaderUser))
		return;

	CUser* pMember = NULL;
	for (int i = 0; i < m_nSizeofMember; ++i)
	{
		pMember = g_UserMng.GetUserByPlayerID(m_aMember[i].m_uPlayerId);
		if (IsValidObj((CObj*)pMember))
		{
			if (pMember->m_idPlayer != pLeaderUser->m_idPlayer)
				pLeaderUser->SummonPlayer(pMember);
		}
	}
}

#endif // worldserver

#ifdef __WORLDSERVER
void CParty::ReplaceLodestar(const CRect& rect)
{
	// locked
	CUser* pUser;
	for (int i = 0; i < m_nSizeofMember; i++)
	{
		pUser = g_UserMng.GetUserByPlayerID(m_aMember[i].m_uPlayerId);

		if (IsValidObj(pUser))
		{
			CWorld* pWorld = pUser->GetWorld();
			if (pWorld)
			{
				POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z };
				if (rect.PtInRect(point))
				{
					PRegionElem pRgnElem = NULL;
					if (pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0)
						pRgnElem = g_WorldMng.GetRevivalPos(pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival);
					if (!pRgnElem)	// Find near revival pos
						pRgnElem = g_WorldMng.GetNearRevivalPos(pWorld->GetID(), pUser->GetPos());
					if (pRgnElem)
						pUser->REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer);
				}
			}
		}
	}
}

void CParty::Replace(DWORD dwWorldId, D3DXVECTOR3& vPos, BOOL bMasterAround)
{
	CUser* pMember;
	for (int i = 0; i < m_nSizeofMember; i++)
	{
		pMember = g_UserMng.GetUserByPlayerID(m_aMember[i].m_uPlayerId);

		if (IsValidObj(pMember))
		{
			pMember->REPLACE(g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, nTempLayer);
#ifdef __BUFF_1107
			pMember->m_buffs.RemoveBuffs(RBF_COMMON, 0);
#else	// __BUFF_1107
			pMember->m_SkillState.RemoveAllSkillInfluence();
#endif	// __BUFF_1107
		}
	}
}

void CParty::Replace(DWORD dwWorldId, LPCTSTR sKey)
{
	CUser* pMember;
	for (int i = 0; i < m_nSizeofMember; i++)
	{
		pMember = g_UserMng.GetUserByPlayerID(m_aMember[i].m_uPlayerId);

		if (IsValidObj(pMember))
		{
			PRegionElem pRgnElem = g_WorldMng.GetRevivalPos(dwWorldId, sKey);
			if (NULL != pRgnElem)
				pMember->REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer);
		}
	}
}

BOOL CParty::ReplaceChkLv(int Lv)
{
	CUser* pMember;
	for (int i = 0; i < m_nSizeofMember; i++)
	{
		pMember = g_UserMng.GetUserByPlayerID(m_aMember[i].m_uPlayerId);

		if (IsValidObj(pMember))
		{
			if (pMember->GetLevel() > Lv)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
#endif //__WORLDSERVER



CParty g_Party;
/*--------------------------------------------------------------------------------*/
#ifndef __CLIENT

CPartyMng::CPartyMng()
{
	m_id = 0;
#ifdef __WORLDSERVER
	m_dwPartyMapInfoLastTick = 0;
#endif // __WORLDSERVER
#ifdef __CORESERVER
	m_hWorker = m_hCloseWorker = NULL;
#endif // __CORESERVER
}

CPartyMng::~CPartyMng()
{
	Clear();
}

void CPartyMng::Clear(void)
{
#ifndef __WORLDSERVER
	m_AddRemoveLock.Enter(theLineFile);	// lock1
#endif	// __WORLDSERVER

	for (C2PartyPtr::iterator i = m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i)
		SAFE_DELETE(i->second);

	m_2PartyPtr.clear();

#ifdef __CORESERVER
	CloseWorkers();
#endif // __CORESERVER

#ifndef __WORLDSERVER
	m_AddRemoveLock.Leave(theLineFile);	// unlock1
#endif	// __WORLDSERVER
}




u_long	CPartyMng::NewParty(u_long uLeaderId, u_long uMemberId, u_long uPartyId)
{
	//	locked
	if (0 == uPartyId)
	{
		m_id++;
	}
	else
	{
		m_id = uPartyId;
	}


	if (NULL == GetParty(m_id))
	{
		CParty* pParty = new CParty;

		pParty->SetPartyId(m_id);
		if (TRUE == pParty->NewMember(uLeaderId) && TRUE == pParty->NewMember(uMemberId))
		{
			m_2PartyPtr.insert(C2PartyPtr::value_type(m_id, pParty));
			pParty->m_nGetItemPlayerId = pParty->m_aMember[0].m_uPlayerId;
			return m_id;
		}
		else
		{
			SAFE_DELETE(pParty);
		}
	}
	return 0;
}

BOOL CPartyMng::DeleteParty(u_long uPartyId)
{
	CParty* pParty = GetParty(uPartyId);
	if (pParty)
	{
#ifdef __WORLDSERVER
#if __VER >= 14 // __INSTANCE_DUNGEON
		CInstanceDungeonParty::GetInstance()->DestroyAllDungeonByDungeonID(uPartyId);
#endif // __INSTANCE_DUNGEON
#endif // __WORLDSERVER

		SAFE_DELETE(pParty);
		m_2PartyPtr.erase(uPartyId);
		return TRUE;
	}
	return FALSE;
}

CParty* CPartyMng::GetParty(u_long uPartyId)
{
	C2PartyPtr::iterator i = m_2PartyPtr.find(uPartyId);
	if (i != m_2PartyPtr.end())
		return i->second;
	return NULL;
}

void CPartyMng::Serialize(CAr& ar)
{
#ifndef __WORLDSERVER
	m_AddRemoveLock.Enter(theLineFile);	// lock1
#endif	// __WORLDSERVER

	if (ar.IsStoring())
	{
		ar << m_id;
		int nCount = 0;
		u_long uOffset = ar.GetOffset();
		ar << nCount;
		for (C2PartyPtr::iterator i = m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i)
		{
			CParty* pParty = (CParty*)i->second;
			pParty->Serialize(ar);
			nCount++;
		}
		int nBufSize1;
		LPBYTE lpBuf1 = ar.GetBuffer(&nBufSize1);
		*(UNALIGNED int*)(lpBuf1 + uOffset) = nCount;
	}
	else
	{
		int nCount;
		ar >> m_id;
		ar >> nCount;
		for (int i = 0; i < nCount; i++)
		{
			CParty* pParty = new CParty;
			pParty->Serialize(ar);
			bool bResult = m_2PartyPtr.insert(C2PartyPtr::value_type(pParty->m_uPartyId, pParty)).second;
			if (bResult == false)
			{
				SAFE_DELETE(pParty);
			}
		}
	}
#ifndef __WORLDSERVER
	m_AddRemoveLock.Leave(theLineFile);	// unlock1
#endif	// __WORLDSERVER
}

#ifdef __CORESERVER
BOOL CPartyMng::IsPartyNameId(u_long uidPlayer)
{
	if (m_2PartyNameLongPtr.end() != m_2PartyNameLongPtr.find(uidPlayer))
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CPartyMng::IsPartyName(const char* szPartyName)
{
	if (m_2PartyNameStringPtr.end() != m_2PartyNameStringPtr.find(szPartyName))
	{
		return TRUE;
	}
	return FALSE;
}

LPCSTR CPartyMng::GetPartyString(u_long uidPlayer)
{
	ULONG2STRING::iterator i = m_2PartyNameLongPtr.find(uidPlayer);
	if (i != m_2PartyNameLongPtr.end())
		return(i->second.data());
	return NULL;
}

u_long CPartyMng::GetPartyID(const char* szPartyName)
{
	STRING2ULONG::iterator i = m_2PartyNameStringPtr.find(szPartyName);
	if (i != m_2PartyNameStringPtr.end())
		return(i->second);
	return 0;
}



void CPartyMng::AddPartyName(u_long uidPlayer, const char* szPartyName)
{
	m_2PartyNameLongPtr.insert(map<u_long, string>::value_type(uidPlayer, szPartyName));
	m_2PartyNameStringPtr.insert(map<string, u_long>::value_type(szPartyName, uidPlayer));
}

void CPartyMng::RemovePartyName(u_long uidPlayer, const char* szPartyName)
{
	ULONG2STRING::iterator iter = m_2PartyNameLongPtr.find(uidPlayer);
	STRING2ULONG::iterator iter1 = m_2PartyNameStringPtr.find(szPartyName);

	if (iter != m_2PartyNameLongPtr.end())
		m_2PartyNameLongPtr.erase(iter);

	if (iter1 != m_2PartyNameStringPtr.end())
		m_2PartyNameStringPtr.erase(iter1);
	//	m_2PartyNameLongPtr.insert(  map<u_long, string>::value_type( uidPlayer, szPartyName ) );
	//	m_2PartyNameStringPtr.insert(  map<string, u_long>::value_type(szPartyName, uidPlayer ) );
}

BOOL CPartyMng::CreateWorkers(void)
{
	DWORD dwThreadId;
	m_hCloseWorker = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hWorker = chBEGINTHREADEX(NULL, 0, _Worker, this, 0, &dwThreadId);
	if (m_hWorker == NULL)
		return FALSE;
	return TRUE;
}

void CPartyMng::CloseWorkers(void)
{
	CLOSE_THREAD(m_hWorker, m_hCloseWorker);
}

UINT CPartyMng::_Worker(LPVOID pParam)
{
	CPartyMng* pPartyMng = (CPartyMng*)pParam;
	pPartyMng->Worker();
	return 0;
}

void CPartyMng::Worker(void)
{
	CPlayer* pMember;

	HANDLE hHandle = m_hCloseWorker;
	while (WaitForSingleObject(hHandle, 1000) == WAIT_TIMEOUT)
	{
		CTime timeCurr = CTime::GetCurrentTime();

		CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
		CMclAutoLock	Lock2(m_AddRemoveLock);

		for (C2PartyPtr::iterator i = m_2PartyPtr.begin(); i != m_2PartyPtr.end(); )
		{
			CParty* pParty = (CParty*)i->second;
			++i;
			if (pParty->m_nReferens > 0)
			{
				for (int j = 1; j < pParty->m_nSizeofMember; j++)
				{
					if (pParty->m_aMember[j].m_uPlayerId != 0 && pParty->m_aMember[j].m_bRemove)
					{
						if (timeCurr.GetTime() - pParty->m_aMember[j].m_tTime.GetTime() > 60 * 10)
						{
							u_long idMember = pParty->GetPlayerId(j);
							if (pParty->DeleteMember(idMember))
							{
								pParty->m_nReferens--;

								g_dpCoreSrvr.SendRemoveParty(pParty->m_uPartyId, pParty->m_aMember[0].m_uPlayerId, idMember);

								if (pParty->GetSizeofMember() < 2)
								{
									pMember = g_PlayerMng.GetPlayer(pParty->GetPlayerId(0));
									if (pMember)
										pMember->m_uPartyId = 0;
									DeleteParty(pParty->m_uPartyId);
									break;
								}

								if (pParty->m_nReferens == 0)
									break;
							}
						}
					}
				}	// for
			}	// if

			for (int j = 0; j < MAX_PARTYMODE; j++)
			{
				if (pParty->m_nModeTime[j])
				{
#if __VER >= 12
					if (j == PARTY_PARSKILL_MODE)
						continue;
#endif
					pParty->m_nModeTime[j] -= 1000;
					if (pParty->m_nModeTime[j] <= 0)
					{
						pParty->m_nModeTime[j] = 0;
						g_dpCoreSrvr.SendSetPartyMode(pParty->m_uPartyId, j, FALSE);
					}
				}
			}

		}	// for
//		TRACE( "Worker Party\n" );
	}
}

void CPartyMng::AddConnection(CPlayer* pPlayer)
{
	CParty* pParty;
	CMclAutoLock	Lock(m_AddRemoveLock);

	pParty = GetParty(pPlayer->m_uPartyId);
	if (pParty)
	{
		int i = pParty->FindMember(pPlayer->uKey);
		if (i < 0)
		{
			pPlayer->m_uPartyId = 0;
			return;
		}

		pParty->m_aMember[i].m_bRemove = FALSE;
		pParty->m_nReferens--;

		BEFORESENDDUAL(ar, PACKETTYPE_ADDPLAYERPARTY, DPID_UNKNOWN, DPID_UNKNOWN);
		ar << pPlayer->m_uPartyId << pPlayer->uKey;
		SEND(ar, &g_dpCoreSrvr, DPID_ALLPLAYERS);
	}
	else
		pPlayer->m_uPartyId = 0;
}

void CPartyMng::RemoveConnection(CPlayer* pPlayer)
{
	if (pPlayer->m_uPartyId == 0)
		return;

	CParty* pParty;
	CMclAutoLock	Lock(m_AddRemoveLock);

	pParty = GetParty(pPlayer->m_uPartyId);
	if (pParty)
	{
		int i = pParty->FindMember(pPlayer->uKey);
		if (i < 0)
			return;
		pParty->m_aMember[i].m_tTime = CTime::GetCurrentTime();
		pParty->m_aMember[i].m_bRemove = TRUE;
		pParty->m_nReferens++;

		BEFORESENDDUAL(ar, PACKETTYPE_REMOVEPLAYERPARTY, DPID_UNKNOWN, DPID_UNKNOWN);
		ar << pPlayer->m_uPartyId << pPlayer->uKey;
		SEND(ar, &g_dpCoreSrvr, DPID_ALLPLAYERS);

#if __VER >= 12
		if (pParty->m_nModeTime[PARTY_PARSKILL_MODE])
			g_dpCoreSrvr.SendSetPartyMode(pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE);
#endif

		if (i == 0)
		{
			bool fRemoveParty = true;
			for (int j = 1; j < pParty->m_nSizeofMember; j++)
			{
				if (pParty->m_aMember[j].m_bRemove == FALSE)
				{
					fRemoveParty = false;
					pParty->SwapPartyMember(0, j);
					break;
				}
			}

			if (fRemoveParty)
			{
				CPlayer* pPlayer;
				for (int j = 0; j < pParty->m_nSizeofMember; j++)
				{
					pPlayer = g_PlayerMng.GetPlayer(pParty->GetPlayerId(j));
					if (pPlayer)
						pPlayer->m_uPartyId = 0;
				}
				DeleteParty(pParty->m_uPartyId);
			}
		}
	}
}
#endif // __CORESERVER

#ifdef __WORLDSERVER
#ifdef __FL_PARTY_FINDER
void CPartyMng::AddPartyList(CUser* pUser)
{
	if (!IsValidObj(pUser))
		return;

	vector<PARTY_LIST> vecParties;

	for (auto& it : m_2PartyPtr)
	{
		CParty* pParty = (CParty*)it.second;
		if (!pParty)
			continue;

		if (!pParty->m_bAllowed)
			continue;

		CUser* pLeader = g_UserMng.GetUserByPlayerID(pParty->m_aMember[0].m_uPlayerId);

		PARTY_LIST party;

		// Leader info
		party.m_dwLeaderId = pParty->m_aMember[0].m_uPlayerId;

		if (IsValidObj(pLeader))
		{
			party.m_nLeaderLevel = pLeader->GetLevel();
			party.m_nLeaderJob = pLeader->GetJob();
			party.m_strLeaderName = pLeader->GetName();
		}
		else
		{
			PlayerData* pPlayerDataLeader = CPlayerDataCenter::GetInstance()->GetPlayerData(party.m_dwLeaderId);
			if (pPlayerDataLeader)
			{
				party.m_nLeaderLevel = pPlayerDataLeader->data.nLevel;
				party.m_nLeaderJob = pPlayerDataLeader->data.nJob;
				party.m_strLeaderName = pPlayerDataLeader->szPlayer;
			}
		}

		// Party info
		party.m_dwPartyId = pParty->m_uPartyId;
		party.m_nPartyMembers = pParty->m_nSizeofMember;
		party.m_nPartyLevel = pParty->m_nLevel;
		party.m_nPartyPoint = pParty->m_nPoint;

		// Skills/Scrolls
		party.m_bLink = pParty->m_nModeTime[PARTY_LINKATTACK_MODE] ? TRUE : FALSE;
		party.m_bDrop = pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE] ? TRUE : FALSE;
		party.m_bGift = pParty->m_nModeTime[PARTY_GIFTBOX_MODE] ? TRUE : FALSE;
		party.m_bResting = pParty->m_nModeTime[PARTY_STRETCHING_MODE] ? TRUE : FALSE;
		party.m_bRedScroll = pParty->m_nModeTime[PARTY_PARSKILL_MODE] ? TRUE : FALSE;

		if (IsValidObj(pLeader)) // todo: rework how this scrolls working (do it same as red scroll)
		{
			party.m_bOrangeScroll = (pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01) || pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP02) || pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01_01));
			party.m_bGreenScroll = (pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01) || pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02) || pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01_01));
		}

		vecParties.push_back(party);
	}

	pUser->AddPartyFinder(&vecParties);
}
#endif

void CPartyMng::PartyMapInfo()
{
	const float MOVE_DIST_MIN = 0.0f;
	const float PARTY_MAP_AROUND = 32.0f * 32.0f;

	//2. interval check
	if (m_dwPartyMapInfoLastTick + PARTY_MAP_SEC * 1000 > ::GetTickCount())
		return;

	m_dwPartyMapInfoLastTick = ::GetTickCount();

	//3.
	for (C2PartyPtr::iterator i = m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i)
	{
		D3DXVECTOR3 vPosBuf;
		float fDist;

		CParty* pParty = (CParty*)i->second;
		for (int j = 0; j < pParty->GetSizeofMember(); ++j)
		{
			CMover* pMover = prj.GetUserByID(pParty->GetPlayerId(j));
			if (!IsValidObj(pMover))
				continue;

			if (pMover->GetWorld() == NULL || pMover->GetWorld()->m_linkMap.GetLinkMap(pMover->GetLayer()) == NULL)
				continue;


			vPosBuf = pMover->GetPos() - pParty->GetPos(j);
			fDist = D3DXVec3LengthSq(&vPosBuf);
			if (MOVE_DIST_MIN > fDist)
				continue;

			pParty->SetPos(j, pMover->GetPos());

			for (int k = 0; k < pParty->GetSizeofMember(); ++k)
			{
				if (k == j)
					continue;

				CMover* pSendMover = prj.GetUserByID(pParty->GetPlayerId(k));
				if (!IsValidObj(pSendMover))
					continue;

				if (pSendMover->GetWorld() == NULL)
					continue;

				if (pSendMover->GetWorld() != pMover->GetWorld())
					continue;

				vPosBuf = pSendMover->GetPos() - pMover->GetPos();
				fDist = D3DXVec3LengthSq(&vPosBuf);

				if (pMover->GetWorld() == pSendMover->GetWorld() && fDist > PARTY_MAP_AROUND)
					((CUser*)pSendMover)->AddPartyMapInfo(j, pMover->GetPos());
			}
		}
	}
}

DWORD	CPartyMng::CanInviteParty(const u_long uLeaderPlayerID, const u_long uMemberPlayerID)
{
	CUser* pLeader = g_UserMng.GetUserByPlayerID(uLeaderPlayerID);
	if (IsValidObj(pLeader) == FALSE)
		return 0;

	CUser* pMember = g_UserMng.GetUserByPlayerID(uMemberPlayerID);
	if (IsValidObj(pMember) == FALSE)
		return 0;

	CParty* pMemberParty = GetParty(pMember->m_idparty);
	if (pMemberParty != NULL)
		return 1;

	CParty* pLeaderParty = GetParty(pLeader->m_idparty);
	if (pLeaderParty != NULL)	// 기존 극단에 멤버 추가인 경우..
	{
		if (pLeaderParty->IsLeader(uLeaderPlayerID) == FALSE)
			return 2;

		if (pLeaderParty->GetSizeofMember() >= MAX_PTMEMBER_SIZE_SPECIAL)
			return 3;
	}

	return 5;
}

void	CPartyMng::RequestAddPartyMemberToCS(const u_long uLeaderPlayerID, const u_long uMemberPlayerID) const
{
	BEFORESENDDUAL(ar, PACKETTYPE_ADDPARTYMEMBER, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uLeaderPlayerID << uMemberPlayerID;
	SEND(ar, &g_DPCoreClient, DPID_SERVERPLAYER);
}

#endif // __WORLDSERVER

CPartyMng	g_PartyMng;
#endif // not client


#ifdef __FL_PARTY_SORT
BOOL CParty::ChangePosition(u_long uSelectedMember, BOOL bUp)
{
	int nFirst = FindMember(uSelectedMember);
	if (nFirst <= 0)
		return FALSE;

	int nSecond = nFirst;
	if (bUp)
	{
		nSecond = nFirst - 1;
		if (nSecond <= 0)
			return FALSE;
	}
	else
	{
		nSecond = nFirst + 1;
		if (nSecond >= MAX_PTMEMBER_SIZE_SPECIAL)
			return FALSE;
	}

	if (!IsMember(m_aMember[nSecond].m_uPlayerId))
		return FALSE;

	SwapPartyMember(nFirst, nSecond);

	return TRUE;
}
#endif