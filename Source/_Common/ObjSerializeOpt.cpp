#include "stdafx.h"

#	if defined(__WORLDSERVER) || defined(__CLIENT)
#include "mover.h"
#include "defineItem.h"
#	else	// __WORLDSERVER, __CLIENT
#include "..\_network\objects\obj.h"
#	endif	// __WORLDSERVER, __CLIENT

#ifndef GETBLOCK
#define	GETBLOCK( ar, lpBuf, nBufSize )	\
										int nBufSize;	\
										LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
#endif	// __GETBLOCK

#include "honor.h"

#ifdef __FIX_SRLZ_STATS
#if defined (__WORLDSERVER) || defined(__CLIENT)
const static int stats[] =
{
	DST_STA, DST_INT, DST_DEX, DST_STR,
	DST_HP_MAX_RATE, DST_HP_MAX, DST_MP_MAX_RATE, DST_MP_MAX, DST_FP_MAX_RATE, DST_FP_MAX,
	DST_JUMPING, DST_SPEED, DST_CHRSTATE, DST_SPELL_RATE, DST_ATTACKSPEED, DST_ATTACKSPEED_RATE, DST_HAWKEYE_RATE
};
#endif
#endif

void CObj::Serialize(CAr& ar)
{
	if (ar.IsStoring())
	{
		ar << (u_char)m_dwType;
		ar << m_dwIndex;
		ar << (u_short)(m_vScale.x * 100.0f);
		ar << m_vPos;
		ar << (short)(m_fAngle * 10.0f);
	}
	else
	{
		u_short u2;
		short i2;
		m_dwType = m_dwIndex = 0;

		ar >> (u_char&)m_dwType;
		ar >> m_dwIndex;
		ar >> u2;
		m_vScale.x = m_vScale.y = m_vScale.z = (float)u2 / 100.0f;
		ar >> m_vPos;
		ar >> i2;
		m_fAngle = (float)i2 / 10.0f;
#if defined(__WORLDSERVER) || defined(__CLIENT)
		SetAngle(m_fAngle);
#endif

#ifdef __WORLDSERVER
		m_vLink = m_vPos;
#endif	// __WORLDSERVER
	}
}

#if defined(__WORLDSERVER) || defined(__CLIENT)
void CMover::OnApplySM()
{
	ItemProp* aItemprop;
	int nAdjParamVal;

	if (m_dwSMTime[SM_MAX_HP] > 0)
	{
		aItemprop = prj.GetItemProp(II_CHR_FOO_COO_BULLHAMS);
		nAdjParamVal = aItemprop->nAdjParamVal2;
		SetDestParam(aItemprop->dwDestParam1, nAdjParamVal, NULL_CHGPARAM, 1);
	}

	if (m_dwSMTime[SM_MAX_HP50] > 0)
	{
		aItemprop = prj.GetItemProp(II_CHR_FOO_COO_GRILLEDEEL);
		nAdjParamVal = m_nPlusMaxHitPoint;
		SetDestParam(aItemprop->dwDestParam1, nAdjParamVal, NULL_CHGPARAM, 1);
	}

	if (m_dwSMTime[SM_VELOCIJUMP] > 0)
	{
		aItemprop = prj.GetItemProp(g_AddSMMode.dwSMItemID[SM_VELOCIJUMP]);
		if (aItemprop)
		{
			if (aItemprop->dwDestParam1 != -1)
				SetDestParam(aItemprop->dwDestParam1, aItemprop->nAdjParamVal1, NULL_CHGPARAM);
			if (aItemprop->dwDestParam2 != -1)
				SetDestParam(aItemprop->dwDestParam2, aItemprop->nAdjParamVal2, NULL_CHGPARAM);
		}
	}
}
#endif // __WORLDSERVER __CLIENT

void CMover::Serialize(CAr& ar)
{
	CCtrl::Serialize(ar);	//	25
	DWORD dwGold;

	if (ar.IsStoring())	// STORING
	{
		ar << (u_short)m_dwMotion;
		ar << (u_char)m_bPlayer;
		ar << m_nHitPoint;
		ar << m_pActMover->GetState();
		ar << m_pActMover->GetStateFlag();
		ar << (u_char)m_dwBelligerence;
		ar << m_dwMoverSfxId;

		if (m_bPlayer)	// PLAYER
		{
			ar.WriteString(m_szName);
			ar << GetSex();
			ar << (u_char)m_dwSkinSet;
			ar << (u_char)m_dwHairMesh;
			ar << m_dwHairColor;
			ar << (u_char)m_dwHeadMesh;
			ar << m_idPlayer;
			ar << (u_char)m_nJob;
			ar << (u_short)m_nStr;
			ar << (u_short)m_nSta;
			ar << (u_short)m_nDex;
			ar << (u_short)m_nInt;
			ar << (u_short)m_nLevel;
			ar << m_nManaPoint;
			ar << m_nFatiguePoint;
			ar << m_nFuel;
			ar << m_tmAccFuel;
			if (m_idGuild > 0)
			{
				ar << (u_char)1;
				ar << m_idGuild;
			}
			else
			{
				ar << (u_char)0;
			}

			if (m_idparty > 0)
			{
				ar << (u_char)1;
				ar << m_idparty;
			}
			else
			{
				ar << (u_char)0;
			}

			ar << (char)m_dwAuthorization;
			ar << m_dwMode;
			ar << m_dwStateMode;
#ifdef __WORLDSERVER
			CItemElem* pItemElem = m_Inventory.GetAtId(m_dwUseItemId);
			if (pItemElem)
				ar << pItemElem->GetProp()->dwID;
			else
				ar << (OBJID)0;
#else // __WORLDSERVER
			ar << m_dwUseItemId;
#endif // __WORLDSERVER
			ar << m_nHonor;

			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				ar << m_aEquipInfo[i].nOption;
			}

			ar << m_nGuildCombatState;
#ifdef __FL_BATTLE_PASS
			ar << m_nBattlePassLevel;
			ar << m_nBattlePassExp;
#endif
#ifdef __MODEL_HIDEFASH
			for (int z = 0; z < 6; z++)
				ar << m_bHideFashion[z];
#endif
			for (int j = 0; j < SM_MAX; ++j)
				ar << m_dwSMTime[j];

			if (CObj::GetMethod() == METHOD_NONE)
			{
				ar << m_nFxp;

				dwGold = GetGold();
				ar << dwGold;
				ar << m_nExp1;
				ar << m_nSkillLevel;
				ar << m_nSkillPoint;
				DWORD dwJobLv[MAX_JOB] = { 0, };
				ar.Write((void*)dwJobLv, sizeof(DWORD) * MAX_JOB);
				ar << m_idMarkingWorld;
				ar << m_vMarkingPos;
				ar << m_nQuestSize;
				ar.Write(m_aQuest, sizeof(QUEST) * m_nQuestSize);
				ar << m_nCompleteQuestSize;
				ar.Write(m_aCompleteQuest, sizeof(WORD) * m_nCompleteQuestSize);

				ar << m_nCheckedQuestSize;
				ar.Write(m_aCheckedQuest, sizeof(WORD) * m_nCheckedQuestSize);

				ar << (short)m_nRemainGP;
				ar << (short)0;
				{
					for (int i = 0; i < MAX_HUMAN_PARTS; i++)
					{
						ar << m_aEquipInfo[i].dwId;
#ifdef __MODEL_CHANGE
						ar << m_aEquipInfo[i].dwLook;
#endif
					}
				}
				ar.Write((void*)m_aJobSkill, sizeof(SKILL) * (MAX_SKILL_JOB));

				ar << (BYTE)m_nCheerPoint << m_dwTickCheer - GetTickCount();

				ar << m_nSlot;
				for (int k = 0; k < 3; ++k)
					ar << m_dwGoldBank[k];
				for (int k = 0; k < 3; ++k)
					ar << m_idPlayerBank[k];
				ar << m_nPlusMaxHitPoint;

				m_Inventory.Serialize(ar);
				for (int k = 0; k < 3; ++k)
					m_Bank[k].Serialize(ar);

				ar << GetPetId();
#ifdef __FL_CONSUMABLES_STORAGE
				m_Chest.Serialize(ar);
#endif
#ifdef __FL_SWITCH_EQ
				m_Switches.Serialize(ar);
#endif
#ifdef __FL_MONSTER_ALBUM
				m_MonsterAlbum.Serialize(ar);
#endif
#ifdef __JEFF_9_20
				ar << m_dwMute;
#endif

				for (int i = 0; i < MAX_HONOR_TITLE; ++i)
				{
					ar << m_aHonorTitle[i];

				}

			}
			else if (CObj::GetMethod() == METHOD_EXCLUDE_ITEM)
			{
#	if defined (__WORLDSERVER) || defined(__CLIENT)
				ar.WriteString(m_vtInfo.GetTitle());
#	endif
				u_char uSize = 0;
				u_long uOffset = ar.GetOffset();
				ar << uSize;
				for (u_char uParts = 0; uParts < MAX_HUMAN_PARTS; uParts++)
				{
					CItemElem* pItemElem = m_Inventory.GetEquip(uParts);
					if (pItemElem)
					{
						uSize++;
						ar << uParts;
						ar << (u_short)pItemElem->m_dwItemId;
						ar << pItemElem->m_byFlag;
#ifdef __MODEL_CHANGE
						ar << pItemElem->GetLook();
#endif
					}
				}
				GETBLOCK(ar, pBlock, nBlockSize);
				*(UNALIGNED u_char*)(pBlock + uOffset) = uSize;

				DWORD dwPetId = NULL_ID;
#ifdef __PET_1024
				char* pszPetName = "";
				CItemElem* pItemElem = GetPetItem();
				if (pItemElem)
				{
					dwPetId = MAKELONG((WORD)pItemElem->m_pPet->GetIndex(), (WORD)pItemElem->m_pPet->GetLevel());
#ifdef __PET_1024
					pszPetName = const_cast<char*>(pItemElem->m_pPet->GetName());
#endif	// __PET_1024
				}
				ar << dwPetId;
#ifdef __PET_1024
				ar.WriteString(pszPetName);
#endif	// __PET_1024
#endif	// __PET_0410
			}
		}
		else	// NPC
		{
#ifdef __FIX_NPC_GENDER
			ar << m_bySex;
#endif
#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
			SerializeAbnormalNpc(ar);
#endif
			ar << (u_char)m_dwHairMesh << m_dwHairColor << (u_char)m_dwHeadMesh;
			ar.WriteString(m_szCharacterKey);

			u_char uSize = 0;
			u_long uOffset = ar.GetOffset();
			ar << uSize;
			if (IsEquipableNPC())
			{
				CItemElem* pItemElem;
				for (u_char uParts = 0; uParts < MAX_HUMAN_PARTS; uParts++)
				{
					pItemElem = m_Inventory.GetEquip(uParts);
					if (pItemElem)
					{
						uSize++;
						ar << uParts;
						ar << (u_short)pItemElem->m_dwItemId;
#ifdef __MODEL_CHANGE
						ar << pItemElem->GetLook();
#endif
					}
				}
			}
			GETBLOCK(ar, pBlock, nBlockSize);
			*(UNALIGNED u_char*)(pBlock + uOffset) = uSize;


			ar << (u_char)m_bActiveAttack;
			ar << (u_char)m_nMovePattern;
			ar << (u_char)m_nMoveEvent;
			ar << m_nMoveEventCnt;
			ar << m_fSpeedFactor;
		}

		m_buffs.Serialize(ar);

#ifdef __FIX_SRLZ_STATS
#if defined (__WORLDSERVER) || defined(__CLIENT)
		if (CObj::GetMethod() == METHOD_EXCLUDE_ITEM && m_bPlayer)
		{
			for (int i = 0, j = sizeof(stats) / sizeof(stats[0]); i < j; i++)
			{
				ar << m_adjParamAry[stats[i]];
				ar << m_chgParamAry[stats[i]];
			}
		}
#endif
#endif
	}
	else	// LOADING
	{
		m_dwMotion = 0;

		u_short nMotion;
		ar >> nMotion;

#if defined(__WORLDSERVER) || defined(__CLIENT)
		m_dwMotion = (DWORD)nMotion;
#else
		m_dwMotion = (DWORD)nMotion;
#endif

		m_bPlayer =
			m_dwBelligerence =
			m_idGuild =
			m_idparty =
			m_dwRideItemIdx = 0;

		ClearEquipInfo();

		ar >> (u_char&)m_bPlayer;

#ifdef __DBSERVER
		SKILL	aJobSkill[MAX_SKILL_JOB];
		memcpy(aJobSkill, m_aJobSkill, sizeof(aJobSkill));
#endif

		InitProp();

		m_nHitPoint = 0;

		ar >> m_nHitPoint;

		DWORD dw1, dw2;
		ar >> dw1;
		ar >> dw2;
		m_pActMover->ClearStateFlag();
		m_pActMover->AddStateFlag(dw2);
		m_pActMover->__ForceSetState(dw1);
		ar >> (u_char&)m_dwBelligerence;
		ar >> m_dwMoverSfxId;

		if (m_bPlayer)	// PLAYER
		{
			m_dwSkinSet =
				m_dwHairMesh =
				m_dwHeadMesh =
				m_idPlayer =
				m_nJob =
				m_nStr =
				m_nSta =
				m_nDex =
				m_nInt =
				m_nLevel =
				m_nManaPoint =
				m_nFatiguePoint =
				m_dwAuthorization =
				0;

			BYTE bySex;
			ar.ReadString(m_szName, 32);
			ar >> bySex;
			SetSex(bySex);
			ar >> (u_char&)m_dwSkinSet;
			ar >> (u_char&)m_dwHairMesh;
			ar >> m_dwHairColor;
			ar >> (u_char&)m_dwHeadMesh;
#	if defined (__WORLDSERVER) || defined(__CLIENT)
			SetHairColor(m_dwHairColor);
#	endif	// __WORLDSERVER	//__CLIENT
			ar >> m_idPlayer;
			ar >> (u_char&)m_nJob;
			ar >> (u_short&)m_nStr;
			ar >> (u_short&)m_nSta;
			ar >> (u_short&)m_nDex;
			ar >> (u_short&)m_nInt;
			ar >> (u_short&)m_nLevel;
			ar >> m_nManaPoint;
			ar >> m_nFatiguePoint;
			ar >> m_nFuel;
			ar >> m_tmAccFuel;


			u_char u1;
			ar >> u1;
			if (u1 == 1)
			{
				ar >> m_idGuild;
			}

			ar >> u1;
			if (u1 == 1)
			{
				ar >> m_idparty;
			}

			ar >> (char&)m_dwAuthorization;
			ar >> m_dwMode;
			ar >> m_dwStateMode;
#ifdef __WORLDSERVER
			OBJID dwItemTem;
			ar >> dwItemTem;
#else // __WORLDSERVER
			ar >> m_dwUseItemId;
#endif // __WORLDSERVER

			int nTemp = -1;
			ar >> nTemp;
#ifdef __CLIENT
			if (m_nHonor != nTemp)
			{
				m_nHonor = nTemp;
				SetTitle(CTitleManager::Instance()->GetTitle(m_nHonor));
			}
#else	// __CLIENT
			m_nHonor = nTemp;
#endif	// __CLIENT

			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				ar >> m_aEquipInfo[i].nOption;
			}

			ar >> m_nGuildCombatState;
#ifdef __FL_BATTLE_PASS
			ar >> m_nBattlePassLevel;
			ar >> m_nBattlePassExp;
#endif
#ifdef __MODEL_HIDEFASH
			for (int z = 0; z < 6; z++)
				ar >> m_bHideFashion[z];
#endif
			for (int j = 0; j < SM_MAX; ++j)
				ar >> m_dwSMTime[j];

			if (CObj::GetMethod() == METHOD_NONE)
			{
				m_nRemainGP = 0;
				ar >> m_nFxp;

				ar >> dwGold;
				SetGold(dwGold);

				ar >> m_nExp1;
				ar >> m_nSkillLevel;
				ar >> m_nSkillPoint;
				DWORD dwJobLv[MAX_JOB];
				ar.Read((void*)dwJobLv, sizeof(DWORD) * MAX_JOB);
				ar >> m_idMarkingWorld;
				ar >> m_vMarkingPos;
				ar >> m_nQuestSize;
				ar.Read(m_aQuest, sizeof(QUEST) * m_nQuestSize);
				ar >> m_nCompleteQuestSize;
				ar.Read(m_aCompleteQuest, sizeof(WORD) * m_nCompleteQuestSize);

				ar >> m_nCheckedQuestSize;
				ar.Read(m_aCheckedQuest, sizeof(WORD) * m_nCheckedQuestSize);

				short n1, n2;
				ar >> n1 >> n2;
				m_nRemainGP = n1;
				{
					for (int i = 0; i < MAX_HUMAN_PARTS; i++)
					{
						ar >> m_aEquipInfo[i].dwId;
#ifdef __MODEL_CHANGE
						ar >> m_aEquipInfo[i].dwLook;
#endif
					}
				}
#ifdef __DBSERVER
				ar.Read((void*)m_aJobSkill, sizeof(SKILL) * (MAX_SKILL_JOB));
				for (int i = 0; i < MAX_SKILL_JOB; i++)
					m_abUpdateSkill[i] = (BOOL)(memcmp(&m_aJobSkill[i], &aJobSkill[i], sizeof(SKILL)) != 0);
#else
				ar.Read((void*)m_aJobSkill, sizeof(SKILL) * (MAX_SKILL_JOB));
#endif

				m_nCheerPoint = 0;
				ar >> (BYTE&)m_nCheerPoint >> m_dwTickCheer;
				m_dwTickCheer += GetTickCount();
				ar >> m_nSlot;
				for (int k = 0; k < 3; ++k)
					ar >> m_dwGoldBank[k];
				for (int k = 0; k < 3; ++k)
					ar >> m_idPlayerBank[k];

				ar >> m_nPlusMaxHitPoint;

				m_Inventory.Serialize(ar);
				for (int k = 0; k < 3; ++k)
					m_Bank[k].Serialize(ar);

				CItemElem* pItemElem = m_Inventory.GetEquip(PARTS_RIDE);
				if (pItemElem)
					m_dwRideItemIdx = pItemElem->m_dwItemId;

				DWORD dwPetId;
				ar >> dwPetId;
				SetPetId(dwPetId);
#ifdef __PET_1024
#ifdef __CLIENT
				CPet* pPet = GetPet();
				if (pPet)
					m_pet.SetName(const_cast<char*>(pPet->GetName()));
#endif	// __CLIENT
#endif	// __PET_1024

#ifdef __FL_CONSUMABLES_STORAGE
				m_Chest.Serialize(ar);
#endif
#ifdef __FL_SWITCH_EQ
				m_Switches.Serialize(ar);
#endif
#ifdef __FL_MONSTER_ALBUM
				m_MonsterAlbum.Serialize(ar);
#endif
#ifdef __JEFF_9_20
				ar >> m_dwMute;
#endif
#ifdef __CLIENT
				CTitleManager::Instance()->InitEarned();
#endif	// __CLIENT
				for (int l = 0; l < MAX_HONOR_TITLE; ++l)
				{
					ar >> m_aHonorTitle[l];
#ifdef __CLIENT
					int nNeed = CTitleManager::Instance()->GetNeedCount(l, -1);
					if (m_aHonorTitle[l] >= nNeed && nNeed > 0)
					{
						CTitleManager::Instance()->AddEarned(l);
					}
					else
					{

						if (CTitleManager::Instance()->IsEarned(l))
						{
							CTitleManager::Instance()->RemoveEarned(l);
						}
					}
#endif	// __CLIENT
				}
			}
			else if (CObj::GetMethod() == METHOD_EXCLUDE_ITEM)
			{
#if defined (__WORLDSERVER) || defined(__CLIENT)
				char szPVendor[MAX_VENDORNAME] = { 0, };
				ar.ReadString(szPVendor, MAX_VENDORNAME);
				m_vtInfo.SetTitle(szPVendor);
#endif
				for (int i = 0; i < MAX_HUMAN_PARTS; i++)
					m_aEquipInfo[i].dwId = NULL_ID;

				u_char uSize;
				ar >> uSize;
				u_char uParts;
				u_short	u2;
				for (u_char uCount = 0; uCount < uSize; uCount++)
				{
					ar >> uParts;
					ar >> u2;
					m_aEquipInfo[uParts].dwId = (DWORD)u2;
					ar >> m_aEquipInfo[uParts].byFlag;
#ifdef __MODEL_CHANGE
					ar >> m_aEquipInfo[uParts].dwLook;
#endif
				}
				if (m_aEquipInfo[PARTS_RIDE].dwId != NULL_ID)
					m_dwRideItemIdx = m_aEquipInfo[PARTS_RIDE].dwId;

				DWORD dwPetId;
				ar >> dwPetId;
				SetPetId(dwPetId);
#ifdef __PET_1024
				char szPetName[MAX_PET_NAME] = { 0, };
				ar.ReadString(szPetName, MAX_PET_NAME);
#ifdef __CLIENT
				m_pet.SetName(szPetName);
#endif	// __CLIENT
#endif	// __PET_1024
			}

#if defined(__WORLDSERVER) || defined(__CLIENT)
			if (m_dwRideItemIdx)
			{
				ItemProp* pItemProp = prj.GetItemProp(m_dwRideItemIdx);
				if (pItemProp && m_nFuel == -1)
					m_nFuel = (int)(pItemProp->dwFFuelReMax * 0.2f);
			}
			OnApplySM();
#endif	// __WORLDSERVER	// __CLIENT

		}
		else	// NPC
		{
#ifdef __FIX_NPC_GENDER
			ar >> m_bySex;
#endif
#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
			SerializeAbnormalNpc(ar);
#endif

			ar >> (u_char&)m_dwHairMesh >> m_dwHairColor >> (u_char&)m_dwHeadMesh;
			ar.ReadString(m_szCharacterKey, 32);
#if defined (__WORLDSERVER) || defined(__CLIENT)
			SetHairColor(m_dwHairColor);
#endif	// __WORLDSERVER	//__CLIENT

#if !defined (__FL_GW_STATUE_V2) && !defined (__FL_VENDORS)
			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
				m_aEquipInfo[i].dwId = NULL_ID;
#endif
			u_char uSize;
			ar >> uSize;
			if (IsEquipableNPC())
			{
				u_char uParts;
				u_short	u2;
				for (u_char uCount = 0; uCount < uSize; uCount++)
				{
					ar >> uParts;
					ar >> u2;
					m_aEquipInfo[uParts].dwId = (DWORD)u2;
#ifdef __MODEL_CHANGE
					ar >> m_aEquipInfo[uParts].dwLook;
#endif
				}
			}

			m_bActiveAttack =
				m_nMovePattern =
				m_nMoveEvent =
				0;

			ar >> (u_char&)m_bActiveAttack;
			ar >> (u_char&)m_nMovePattern;
			ar >> (u_char&)m_nMoveEvent;
			ar >> m_nMoveEventCnt;
#ifdef __CLIENT
			if (*m_szCharacterKey)
			{
				LPCHARACTER pCharacter = prj.GetCharacter(m_szCharacterKey);
				if (pCharacter)
				{
					_tcscpy(m_szName, pCharacter->m_strName);
					AllocShopInventory(pCharacter);
				}
			}
#endif	// __CLIENT
			ar >> m_fSpeedFactor;
		}

		m_buffs.Serialize(ar);

#ifdef __FIX_SRLZ_STATS
#if defined (__WORLDSERVER) || defined(__CLIENT)
		if (CObj::GetMethod() == METHOD_EXCLUDE_ITEM && m_bPlayer)
		{
			for (int i = 0, j = sizeof(stats) / sizeof(stats[0]); i < j; i++)
			{
				ar >> m_adjParamAry[stats[i]];
				ar >> m_chgParamAry[stats[i]];
			}
		}
#endif
#endif

#ifdef __CLIENT
		AutoSynchronizer()->Initialize();
#endif

	} // LOADING

#ifdef __CLIENT
	m_fDestScale = GetScale().x;
	if (m_fDestScale > 10.0f)
		Error("SerializeOpt:%f %s", m_fDestScale, GetName());
#endif
}

#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
void CMover::SerializeAbnormalNpc(CAr& ar)
{
	if (ar.IsStoring())	// STORING
	{
#if defined (__WORLDSERVER) || defined(__CLIENT)
		BYTE nNpcType = GetNpcType();
		u_long idNpcOwner = GetNpcOwner();

		ar << nNpcType;
		ar << idNpcOwner;

		if (nNpcType != NPC_TYPE_NORMAL)
			ar.WriteString(m_szName);

		if (nNpcType == NPC_TYPE_MVPSTATUE
			|| nNpcType == NPC_TYPE_GUILDLEADERSTATUE
			|| nNpcType == NPC_TYPE_VENDOR_HUMAN
			)
		{
			ar << (u_char)m_nJob;
			ar << (u_short)m_nLevel;
			if (m_idGuild > 0)
			{
				ar << (u_char)1;
				ar << m_idGuild;
			}
			else
			{
				ar << (u_char)0;
			}

			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				ar << m_aEquipInfo[i].dwId;
				ar << m_aEquipInfo[i].nOption;
#ifdef __MODEL_CHANGE
				ar << m_aEquipInfo[i].dwLook;
#endif
			}
		}

		if (nNpcType == NPC_TYPE_VENDOR_HUMAN || nNpcType == NPC_TYPE_VENDOR_NPC)
		{
			std::string title(m_vtInfo.GetTitle());
			title.resize(MAX_VENDORNAME);
			ar.WriteString(title.c_str());
		}
#endif
	}
	else
	{
#if defined (__WORLDSERVER) || defined(__CLIENT)
		BYTE nNpcType = NPC_TYPE_NORMAL;
		u_long idNpcOwner = NULL_ID;

		ar >> nNpcType;
		ar >> idNpcOwner;

		SetNpcType(nNpcType);
		SetNpcOwner(idNpcOwner);

		if (nNpcType != NPC_TYPE_NORMAL)
			ar.ReadString(m_szName, 64);

		if (GetNpcType() == NPC_TYPE_MVPSTATUE
			|| GetNpcType() == NPC_TYPE_GUILDLEADERSTATUE
			|| GetNpcType() == NPC_TYPE_VENDOR_HUMAN
			)
		{
			m_nJob = m_nLevel = 0;
			ar >> (u_char&)m_nJob;
			ar >> (u_short&)m_nLevel;

			u_char u1;
			ar >> u1;
			if (u1 == 1)
			{
				ar >> m_idGuild;
			}

			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				ar >> m_aEquipInfo[i].dwId;
				ar >> m_aEquipInfo[i].nOption;
#ifdef __MODEL_CHANGE
				ar >> m_aEquipInfo[i].dwLook;
#endif
			}
		}
		else
		{
			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
				m_aEquipInfo[i].dwId = NULL_ID;
		}

		if (nNpcType == NPC_TYPE_VENDOR_HUMAN || nNpcType == NPC_TYPE_VENDOR_NPC)
		{
			char szPVendor[MAX_VENDORNAME] = { 0, };
			ar.ReadString(szPVendor, MAX_VENDORNAME);
			m_vtInfo.SetTitle(szPVendor);
		}
#else
		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			m_aEquipInfo[i].dwId = NULL_ID;
#endif
	}
}
#endif
