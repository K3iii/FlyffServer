#include "stdafx.h"
#include "lang.h"
#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

#if !( defined(__DBSERVER) || defined(__VPW) )
#include "project.h"
#endif	// __DBSERVER

BOOL CProject::LoadJobItem(LPCTSTR lpszFileName)
{
	CScript script;
	if (script.Load(lpszFileName) == FALSE)
		return FALSE;
	script.GetToken();
	int nJob, nSex, i, j, nItem;
	memset(m_jobItem, 0xff, sizeof(m_jobItem));
	while (script.tok != FINISHED)
	{
		if (script.Token == _T("job"))
		{
			nJob = script.GetNumber();
			script.GetToken(); // {
			script.GetToken(); // sex
			j = 0;
			while (script.Token != _T("}"))
			{
				if (script.Token == _T("sex"))
				{
					nSex = script.GetNumber();
					script.GetToken(); // {
					i = 0; nItem = script.GetNumber();
					while (script.Token != _T("}"))
					{
						m_jobItem[nJob].adwMale[i][nSex] = nItem;
						nItem = script.GetNumber();
						i++;
					}
				}
				if (script.Token == _T("item"))
				{
					m_jobItem[nJob].beginItem[j].dwItemType = script.GetNumber();
					m_jobItem[nJob].beginItem[j].dwItemId = script.GetNumber();
					m_jobItem[nJob].beginItem[j].dwNum = script.GetNumber();
					j++;
				}
				script.GetToken();
			}
		}
		script.GetToken();
	}
	return TRUE;
}

BOOL CProject::LoadPropMover(LPCTSTR lpszFileName)
{
	CScript scanner;
	if (scanner.Load(lpszFileName, FALSE) == FALSE)
	{
#ifdef __CLIENT
		MessageBox(g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Mover", "프리프", MB_OK);

#endif //__CLIENT
		return FALSE;
	}

	std::set<int> checker;

	TCHAR szLastName[256];
	int i = 0;
	memset(szLastName, 0, sizeof(szLastName));
	int old;
	while (1)
	{
		old = i;
		i = scanner.GetNumber();		// id
		if (scanner.tok == FINISHED)
			break;

		if (i == 0)
			continue;

		if (i < 0 || i >= MAX_PROPMOVER)
		{
			Error("too many mover property or invalid id:%d FILE:%s\n", i, lpszFileName);
			return FALSE;
		}

		if (!checker.insert(i).second)
		{
			Error("Last read:%s, ID:%d is duplicated. 컬럼이 밀려도 이런에러가 난다. FILE:%s\n", szLastName, i, lpszFileName);
			return FALSE;
		}

		MoverProp* pProperty = &m_pPropMover[i];
		pProperty->dwID = i;

		scanner.GetToken();
		_tcscpy(szLastName, scanner.token);
		_tcscpy(pProperty->szName, scanner.token);

#if !( defined(__DBSERVER) || defined(__VPW) )
		m_mapMVI.insert(map<string, DWORD>::value_type(pProperty->szName, pProperty->dwID));
#endif	// __DBSERVER

		pProperty->dwAI = scanner.GetNumber();
		pProperty->dwStr = scanner.GetNumber();
		pProperty->dwSta = scanner.GetNumber();
		pProperty->dwDex = scanner.GetNumber();
		pProperty->dwInt = scanner.GetNumber();
		pProperty->dwHR = scanner.GetNumber();
		pProperty->dwER = scanner.GetNumber();
		pProperty->dwRace = scanner.GetNumber();
		pProperty->dwBelligerence = scanner.GetNumber();
		pProperty->dwGender = scanner.GetNumber();
		pProperty->dwLevel = scanner.GetNumber();
		pProperty->dwFlightLevel = scanner.GetNumber();
		pProperty->dwSize = scanner.GetNumber();
		pProperty->dwClass = scanner.GetNumber();
		pProperty->bIfParts = scanner.GetNumber();

		if (pProperty->bIfParts == NULL_ID)
			Error("LoadPropMover : bIfParts엔 -1을 넣으면 안된다. %s \r\n컬럼이 밀렸거나 워킹폴더를 잘못지정했을수도있음 ", pProperty->szName);

		int nTmp = scanner.GetNumber();
#ifdef __S1108_BACK_END_SYSTEM
		pProperty->dwUseable = scanner.GetNumber();
#else // __S1108_BACK_END_SYSTEM
		pProperty->dwDefExtent = scanner.GetNumber();
#endif // __S1108_BACK_END_SYSTEM
		pProperty->dwActionRadius = scanner.GetNumber();
		pProperty->dwAtkMin = scanner.GetNumber();
		pProperty->dwAtkMax = scanner.GetNumber();
		pProperty->dwAtk1 = scanner.GetNumber();
		pProperty->dwAtk2 = scanner.GetNumber();
		pProperty->dwAtk3 = scanner.GetNumber();
		pProperty->dwAtk4 = scanner.GetNumber();
#if __VER >= 9	//__AI_0509
		pProperty->fFrame = scanner.GetFloat();
		if (abs(-1.0F - pProperty->fFrame) < 0.000001F)
			pProperty->fFrame = 1.0F;
		pProperty->dwOrthograde = scanner.GetNumber();
#else	// __AI_0509
		pProperty->dwVerticalRate = scanner.GetNumber();
		pProperty->dwDiagonalRate = scanner.GetNumber();
#endif	// __AI_0509
		pProperty->dwThrustRate = scanner.GetNumber();

		pProperty->dwChestRate = scanner.GetNumber();
		pProperty->dwHeadRate = scanner.GetNumber();
		pProperty->dwArmRate = scanner.GetNumber();
		pProperty->dwLegRate = scanner.GetNumber();

		pProperty->dwAttackSpeed = scanner.GetNumber();
		pProperty->dwReAttackDelay = scanner.GetNumber();
		pProperty->dwAddHp = scanner.GetNumber();
		pProperty->dwAddMp = scanner.GetNumber();
		pProperty->dwNaturalArmor = scanner.GetNumber();
		pProperty->nAbrasion = scanner.GetNumber();
		pProperty->nHardness = scanner.GetNumber();
		pProperty->dwAdjAtkDelay = scanner.GetNumber();

		pProperty->eElementType = static_cast<SAI79::ePropType>(scanner.GetNumber());
		pProperty->wElementAtk = static_cast<short>(scanner.GetNumber());

		pProperty->dwHideLevel = scanner.GetNumber();
		pProperty->fSpeed = scanner.GetFloat();
		pProperty->dwShelter = scanner.GetNumber();
		pProperty->dwFlying = scanner.GetNumber();
		pProperty->dwJumpIng = scanner.GetNumber();
		pProperty->dwAirJump = scanner.GetNumber();
		pProperty->bTaming = scanner.GetNumber();
		pProperty->dwResisMgic = scanner.GetNumber();

		pProperty->nResistElecricity = (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistFire = (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistWind = (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistWater = (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistEarth = (int)(scanner.GetFloat() * 100.0f);

		pProperty->dwCash = scanner.GetNumber();
		pProperty->dwSourceMaterial = scanner.GetNumber();
		pProperty->dwMaterialAmount = scanner.GetNumber();
		pProperty->dwCohesion = scanner.GetNumber();
		pProperty->dwHoldingTime = scanner.GetNumber();
		pProperty->dwCorrectionValue = scanner.GetNumber();
		pProperty->nExpValue = scanner.GetExpInteger();
		pProperty->nFxpValue = scanner.GetNumber();
		pProperty->nBodyState = scanner.GetNumber();
		pProperty->dwAddAbility = scanner.GetNumber();
		pProperty->bKillable = scanner.GetNumber();

		pProperty->dwVirtItem[0] = scanner.GetNumber();
		pProperty->dwVirtItem[1] = scanner.GetNumber();
		pProperty->dwVirtItem[2] = scanner.GetNumber();
		pProperty->bVirtType[0] = scanner.GetNumber();
		pProperty->bVirtType[1] = scanner.GetNumber();
		pProperty->bVirtType[2] = scanner.GetNumber();

		pProperty->dwSndAtk1 = scanner.GetNumber();
		pProperty->dwSndAtk2 = scanner.GetNumber();

		pProperty->dwSndDie1 = scanner.GetNumber();
		pProperty->dwSndDie2 = scanner.GetNumber();

		pProperty->dwSndDmg1 = scanner.GetNumber();
		pProperty->dwSndDmg2 = scanner.GetNumber();
		pProperty->dwSndDmg3 = scanner.GetNumber();

		pProperty->dwSndIdle1 = scanner.GetNumber();
		pProperty->dwSndIdle2 = scanner.GetNumber();


		scanner.GetToken();

#ifdef __S1108_BACK_END_SYSTEM
		pProperty->m_fHitPoint_Rate = 1.0f;
		pProperty->m_fAttackPower_Rate = 1.0f;
		pProperty->m_fDefence_Rate = 1.0f;
		pProperty->m_fExp_Rate = 1.0f;
		pProperty->m_fItemDrop_Rate = 1.0f;
		pProperty->m_fPenya_Rate = 1.0f;
		pProperty->m_bRate = FALSE;
#endif // __S1108_BACK_END_SYSTEM
	}

	if (checker.size() > 0)
		m_nMoverPropSize = *(checker.rbegin()) + 1;

#ifdef __CLIENT
	m_bLoadedMoverProp = TRUE;
#endif
	return TRUE;
}

BOOL CProject::LoadPropSkill(LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp)
{
	CScript scanner;
	if (scanner.Load(lpszFileName, FALSE) == FALSE)
	{
#ifdef __CLIENT
		MessageBox(g_Neuz.GetSafeHwnd(), "LoadPropItem", "flyff", MB_OK);
#endif //__CLIENT
		return FALSE;
	}

	ItemProp prop;
	ZeroMemory(&prop, sizeof(prop));

	int nVer = scanner.GetNumber();	
	while (scanner.tok != FINISHED)
	{
		DWORD dwID = scanner.GetNumber();	
		if (dwID <= 0)
		{
			Error("%s ID:%d last_read(ID:%d name:%s) (LoadPropSkill)", lpszFileName, dwID, prop.dwID, prop.szName);
			return FALSE;
		}

		prop.nVer = nVer;
		prop.dwID = dwID;
		scanner.GetToken();
		scanner.Token.TrimRight();
		_tcscpy(prop.szName, (LPCSTR)scanner.Token);

		prop.dwNum = scanner.GetNumber();
		prop.dwPackMax = scanner.GetNumber();
		prop.dwItemKind1 = scanner.GetNumber();
		prop.dwItemKind2 = scanner.GetNumber();
		prop.dwItemKind3 = scanner.GetNumber();
		prop.dwItemJob = scanner.GetNumber();
		prop.bPermanence = static_cast<BOOL>(scanner.GetNumber() != 0);
		prop.dwUseable = scanner.GetNumber();
		prop.dwItemSex = scanner.GetNumber();
		prop.dwCost = scanner.GetNumber();
		prop.dwEndurance = scanner.GetNumber();
		prop.nAbrasion = scanner.GetNumber();
		prop.nMaxRepair = scanner.GetNumber();
		prop.dwHanded = scanner.GetNumber();
		prop.dwFlag = scanner.GetNumber();
		prop.dwParts = scanner.GetNumber();
		prop.dwPartsub = scanner.GetNumber();
		prop.bPartsFile = static_cast<BOOL>(scanner.GetNumber() != 0);
		prop.dwExclusive = scanner.GetNumber();
		prop.dwBasePartsIgnore = scanner.GetNumber();
		prop.dwItemLV = scanner.GetNumber();
		prop.dwItemRare = scanner.GetNumber();
		prop.dwShopAble = scanner.GetNumber();
		prop.nLog = scanner.GetNumber();

		BOOL bTempShit = static_cast<BOOL>(scanner.GetNumber() != 0);

		prop.dwLinkKindBullet = scanner.GetNumber();
		prop.dwLinkKind = scanner.GetNumber();
		prop.dwAbilityMin = scanner.GetNumber();
		prop.dwAbilityMax = scanner.GetNumber();
		prop.eItemType = static_cast<SAI79::ePropType>(scanner.GetNumber());
		prop.wItemEatk = static_cast<short>(scanner.GetNumber());
		prop.dwParry = scanner.GetNumber();
		prop.dwblockRating = scanner.GetNumber();
		prop.nAddSkillMin = scanner.GetNumber();
		prop.nAddSkillMax = scanner.GetNumber();
		prop.dwAtkStyle = scanner.GetNumber();
		prop.dwWeaponType = scanner.GetNumber();
		prop.dwItemAtkOrder1 = scanner.GetNumber();
		prop.dwItemAtkOrder2 = scanner.GetNumber();
		prop.dwItemAtkOrder3 = scanner.GetNumber();
		prop.dwItemAtkOrder4 = scanner.GetNumber();
		prop.tmContinuousPain = scanner.GetNumber();
		prop.nShellQuantity = scanner.GetNumber();
		prop.dwRecoil = scanner.GetNumber();
		prop.dwLoadingTime = scanner.GetNumber();
		prop.nAdjHitRate = scanner.GetNumber();
		prop.fAttackSpeed = scanner.GetFloat();
		prop.dwDmgShift = scanner.GetNumber();
		prop.dwAttackRange = scanner.GetNumber();
		prop.nProbability = scanner.GetNumber();
		prop.dwDestParam[0] = scanner.GetNumber();
		prop.dwDestParam[1] = scanner.GetNumber();
		prop.dwDestParam[2] = scanner.GetNumber();
		prop.nAdjParamVal[0] = scanner.GetNumber();
		prop.nAdjParamVal[1] = scanner.GetNumber();
		prop.nAdjParamVal[2] = scanner.GetNumber();
		prop.dwChgParamVal[0] = scanner.GetNumber();
		prop.dwChgParamVal[1] = scanner.GetNumber();
		prop.dwChgParamVal[2] = scanner.GetNumber();
		prop.nDestData1[0] = scanner.GetNumber();
		prop.nDestData1[1] = scanner.GetNumber();
		prop.nDestData1[2] = scanner.GetNumber();
		prop.dwActiveSkill = scanner.GetNumber();
		prop.dwActiveSkillLv = scanner.GetNumber();
		prop.dwActiveSkillRate = scanner.GetNumber();
		prop.dwReqMp = scanner.GetNumber();
		prop.dwReqFp = scanner.GetNumber();
		prop.dwReqDisLV = scanner.GetNumber();
		prop.dwReSkill1 = scanner.GetNumber();
		prop.dwReSkillLevel1 = scanner.GetNumber();
		prop.dwReSkill2 = scanner.GetNumber();
		prop.dwReSkillLevel2 = scanner.GetNumber();
		prop.dwSkillReadyType = scanner.GetNumber();
		prop.dwSkillReady = scanner.GetNumber();
		prop._dwSkillRange = scanner.GetNumber();
		prop.dwSfxElemental = scanner.GetNumber();
		prop.dwSfxObj = scanner.GetNumber();
		prop.dwSfxObj2 = scanner.GetNumber();
		prop.dwSfxObj3 = scanner.GetNumber();
		prop.dwSfxObj4 = scanner.GetNumber();
		prop.dwSfxObj5 = scanner.GetNumber();
		prop.dwUseMotion = scanner.GetNumber();
		prop.dwCircleTime = scanner.GetNumber();
		prop.dwSkillTime = scanner.GetNumber();
		prop.dwExeTarget = scanner.GetNumber();
		prop.dwUseChance = scanner.GetNumber();
		prop.dwSpellRegion = scanner.GetNumber();
		prop.dwSpellType = scanner.GetNumber();
		prop.dwReferStat1 = scanner.GetNumber();
		prop.dwReferStat2 = scanner.GetNumber();
		prop.dwReferTarget1 = scanner.GetNumber();
		prop.dwReferTarget2 = scanner.GetNumber();
		prop.dwReferValue1 = scanner.GetNumber();
		prop.dwReferValue2 = scanner.GetNumber();
		prop.dwSkillType = scanner.GetNumber();
		prop.nItemResistElecricity = (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistFire = (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistWind = (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistWater = (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistEarth = (int)(scanner.GetFloat() * 100.0f);
		prop.nEvildoing = scanner.GetNumber();
		prop.dwExpertLV = scanner.GetNumber();
		prop.dwExpertMax = scanner.GetNumber();
		prop.dwSubDefine = scanner.GetNumber();
		prop.dwExp = scanner.GetNumber();
		prop.dwComboStyle = scanner.GetNumber();
		prop.fFlightSpeed = scanner.GetFloat();
		prop.fFlightLRAngle = scanner.GetFloat();
		prop.fFlightTBAngle = scanner.GetFloat();
		prop.dwFlightLimit = scanner.GetNumber();
		prop.dwFFuelReMax = scanner.GetNumber();
		prop.dwAFuelReMax = scanner.GetNumber();
		prop.dwFuelRe = scanner.GetNumber();
		prop.dwLimitLevel1 = scanner.GetNumber();
		prop.nReflect = scanner.GetNumber();
		prop.dwSndAttack1 = scanner.GetNumber();
		prop.dwSndAttack2 = scanner.GetNumber();

		// szIcon
		scanner.GetToken();
		scanner.GetToken();
#ifdef __CLIENT
		_tcsncpy(prop.szIcon, scanner.token, sizeof(prop.szIcon) - 1);
		prop.szCommand[sizeof(prop.szIcon) - 1] = 0;
#endif

		scanner.GetToken();

		// dwQuestId
		prop.dwQuestId = scanner.GetNumber();

		// szTextFileName
		scanner.GetToken();
		scanner.GetToken();
		_tcscpy(prop.szTextFileName, scanner.token);
		scanner.GetToken();

		// szCommand
		scanner.GetToken();
#ifdef __CLIENT
		_tcsncpy(prop.szCommand, scanner.token, sizeof(prop.szCommand) - 1);
		prop.szCommand[sizeof(prop.szCommand) - 1] = 0;
#endif // __CLIENT

		m_mapII.insert(map<string, DWORD>::value_type(prop.szName, prop.dwID));

#ifdef __CLIENT
		if (prop.IsUltimate())
		{
			CString strName = prop.szName;
			int	nFind = strName.Find("@", 0);
			if (nFind > -1)
			{
				strName.Delete(nFind - 1, 2);
				lstrcpy(prop.szName, (LPCTSTR)strName);
			}
		}
#endif	// __CLIENT

		apObjProp->SetAtGrow(prop.dwID, &prop);

		nVer = scanner.GetNumber();	// version;
	}
	apObjProp->Optimize();
	return TRUE;
}

BOOL CProject::LoadPropItem(LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp)
{
	CScript scanner;
	if (scanner.Load(lpszFileName, FALSE) == FALSE)
	{
#ifdef __CLIENT
		MessageBox(g_Neuz.GetSafeHwnd(), "LoadPropItem", "flyff", MB_OK);
#endif //__CLIENT
		return FALSE;
	}

	ItemProp prop;
	ZeroMemory(&prop, sizeof(prop));

#ifndef __DBSERVER
	int nAmpCopyID = 65500;
#endif // __DBSERVER

	int nVer = scanner.GetNumber();	
	while (scanner.tok != FINISHED)
	{
		DWORD dwID = scanner.GetNumber();	
		if (dwID <= 0)
		{
			Error("%s ID:%d last_read(ID:%d name:%s) (LoadPropItem)", lpszFileName, dwID, prop.dwID, prop.szName);
			return FALSE;
		}

		prop.nVer = nVer;
		prop.dwID = dwID;
		scanner.GetToken();
		scanner.Token.TrimRight();
		_tcscpy(prop.szName, (LPCSTR)scanner.Token);

		prop.dwNum = scanner.GetNumber();
		prop.dwPackMax = scanner.GetNumber();
		prop.dwItemKind1 = scanner.GetNumber();
		prop.dwItemKind2 = scanner.GetNumber();
		prop.dwItemKind3 = scanner.GetNumber();
		prop.dwItemJob = scanner.GetNumber();
		prop.bPermanence = static_cast<BOOL>(scanner.GetNumber() != 0);
		prop.dwUseable = scanner.GetNumber();
		prop.dwItemSex = scanner.GetNumber();
		prop.dwCost = scanner.GetNumber();
		prop.dwEndurance = scanner.GetNumber();
		prop.nAbrasion = scanner.GetNumber();
		prop.nMaxRepair = scanner.GetNumber();
		prop.dwHanded = scanner.GetNumber();
		prop.dwFlag = scanner.GetNumber();
		prop.dwParts = scanner.GetNumber();
		prop.dwPartsub = scanner.GetNumber();
		prop.bPartsFile = static_cast<BOOL>(scanner.GetNumber() != 0);
		prop.dwExclusive = scanner.GetNumber();
		prop.dwBasePartsIgnore = scanner.GetNumber();
		prop.dwItemLV = scanner.GetNumber();
		prop.dwItemRare = scanner.GetNumber();
		prop.dwShopAble = scanner.GetNumber();
		prop.nLog = scanner.GetNumber();

		BOOL bTempShit = static_cast<BOOL>(scanner.GetNumber() != 0);

		prop.dwLinkKindBullet = scanner.GetNumber();
		prop.dwLinkKind = scanner.GetNumber();
		prop.dwAbilityMin = scanner.GetNumber();
		prop.dwAbilityMax = scanner.GetNumber();
		prop.eItemType = static_cast<SAI79::ePropType>(scanner.GetNumber());
		prop.wItemEatk = static_cast<short>(scanner.GetNumber());
		prop.dwParry = scanner.GetNumber();
		prop.dwblockRating = scanner.GetNumber();
		prop.nAddSkillMin = scanner.GetNumber();
		prop.nAddSkillMax = scanner.GetNumber();
		prop.dwAtkStyle = scanner.GetNumber();
		prop.dwWeaponType = scanner.GetNumber();
		prop.dwItemAtkOrder1 = scanner.GetNumber();
		prop.dwItemAtkOrder2 = scanner.GetNumber();
		prop.dwItemAtkOrder3 = scanner.GetNumber();
		prop.dwItemAtkOrder4 = scanner.GetNumber();
		prop.tmContinuousPain = scanner.GetNumber();
		prop.nShellQuantity = scanner.GetNumber();
		prop.dwRecoil = scanner.GetNumber();
		prop.dwLoadingTime = scanner.GetNumber();
		prop.nAdjHitRate = scanner.GetNumber();
		prop.fAttackSpeed = scanner.GetFloat();
		prop.dwDmgShift = scanner.GetNumber();
		prop.dwAttackRange = scanner.GetNumber();
		prop.nProbability = scanner.GetNumber();

		for (int i = 0; i < PROP_BONUSES; i++)
			prop.dwDestParam[i] = scanner.GetNumber();
		for (int i = 0; i < PROP_BONUSES; i++)
			prop.nAdjParamVal[i] = scanner.GetNumber();
		for (int i = 0; i < PROP_BONUSES; i++)
			prop.dwChgParamVal[i] = scanner.GetNumber();
		for (int i = 0; i < PROP_BONUSES; i++)
			prop.nDestData1[i] = scanner.GetNumber();

		prop.dwActiveSkill = scanner.GetNumber();
		prop.dwActiveSkillLv = scanner.GetNumber();
		prop.dwActiveSkillRate = scanner.GetNumber();
		prop.dwReqMp = scanner.GetNumber();
		prop.dwReqFp = scanner.GetNumber();
		prop.dwReqDisLV = scanner.GetNumber();
		prop.dwReSkill1 = scanner.GetNumber();
		prop.dwReSkillLevel1 = scanner.GetNumber();
		prop.dwReSkill2 = scanner.GetNumber();
		prop.dwReSkillLevel2 = scanner.GetNumber();
		prop.dwSkillReadyType = scanner.GetNumber();
		prop.dwSkillReady = scanner.GetNumber();
#ifdef __AIO_PETS
		if (prop.dwItemKind3 == IK3_PET || prop.dwItemKind3 == IK3_EGG)
			prop.dwSkillReady = 300;
#endif
		prop._dwSkillRange = scanner.GetNumber();
		prop.dwSfxElemental = scanner.GetNumber();
		prop.dwSfxObj = scanner.GetNumber();
		prop.dwSfxObj2 = scanner.GetNumber();
		prop.dwSfxObj3 = scanner.GetNumber();
		prop.dwSfxObj4 = scanner.GetNumber();
		prop.dwSfxObj5 = scanner.GetNumber();
		prop.dwUseMotion = scanner.GetNumber();
		prop.dwCircleTime = scanner.GetNumber();
		prop.dwSkillTime = scanner.GetNumber();
		prop.dwExeTarget = scanner.GetNumber();
		prop.dwUseChance = scanner.GetNumber();
		prop.dwSpellRegion = scanner.GetNumber();
		prop.dwSpellType = scanner.GetNumber();
		prop.dwReferStat1 = scanner.GetNumber();
		prop.dwReferStat2 = scanner.GetNumber();
		prop.dwReferTarget1 = scanner.GetNumber();
		prop.dwReferTarget2 = scanner.GetNumber();
		prop.dwReferValue1 = scanner.GetNumber();
		prop.dwReferValue2 = scanner.GetNumber();
		prop.dwSkillType = scanner.GetNumber();
		prop.nEvildoing = scanner.GetNumber();
		prop.dwExpertLV = scanner.GetNumber();
		prop.dwExpertMax = scanner.GetNumber();
		prop.dwSubDefine = scanner.GetNumber();
		prop.dwExp = scanner.GetNumber();
		prop.dwComboStyle = scanner.GetNumber();
		prop.fFlightSpeed = scanner.GetFloat();
		prop.fFlightLRAngle = scanner.GetFloat();
		prop.fFlightTBAngle = scanner.GetFloat();
		prop.dwFlightLimit = scanner.GetNumber();
		prop.dwFFuelReMax = scanner.GetNumber();
		prop.dwAFuelReMax = scanner.GetNumber();
		prop.dwFuelRe = scanner.GetNumber();
		prop.dwLimitLevel1 = scanner.GetNumber();
		prop.nReflect = scanner.GetNumber();
		prop.dwSndAttack1 = scanner.GetNumber();
		prop.dwSndAttack2 = scanner.GetNumber();

		// szIcon
		scanner.GetToken();
		scanner.GetToken();
#ifdef __CLIENT
		_tcsncpy(prop.szIcon, scanner.token, sizeof(prop.szIcon) - 1);
		prop.szCommand[sizeof(prop.szIcon) - 1] = 0;
#endif

		scanner.GetToken();

		// dwQuestId
		prop.dwQuestId = scanner.GetNumber();

		// szTextFileName
		scanner.GetToken();
		scanner.GetToken();
		_tcscpy(prop.szTextFileName, scanner.token);
		scanner.GetToken();

		// szCommand
		scanner.GetToken();
#ifdef __CLIENT
		_tcsncpy(prop.szCommand, scanner.token, sizeof(prop.szCommand) - 1);
		prop.szCommand[sizeof(prop.szCommand) - 1] = 0;
#endif // __CLIENT

#ifdef __FL_BOXES_V2
		prop.nBoxType = BOX_TYPE_NOTBOX;
#endif

		m_mapII.insert(map<string, DWORD>::value_type(prop.szName, prop.dwID));

#ifdef __CLIENT
		if (prop.IsUltimate())
		{
			CString strName = prop.szName;
			int	nFind = strName.Find("@", 0);
			if (nFind > -1)
			{
				strName.Delete(nFind - 1, 2);
				lstrcpy(prop.szName, (LPCTSTR)strName);
			}
		}
#endif	// __CLIENT

		apObjProp->SetAtGrow(prop.dwID, &prop);

#ifndef __DBSERVER
		if (prop.IsExperienceScroll())
		{
			int nOrginalID = prop.dwID;
			int nCopy[4];
			nCopy[0], nCopy[1], nCopy[2], nCopy[3] = 0;
			prop.dwID = nAmpCopyID;

			for (int i = 0; i < 4; i++)
			{
				nCopy[i] = prop.dwID;
				m_mapII.insert(map<string, DWORD>::value_type(prop.szName, prop.dwID));
				apObjProp->SetAtGrow(prop.dwID, &prop);

				prop.dwID++;
				nAmpCopyID++;
			}
			AMP_STORE_INFO* pAmpInfo = new AMP_STORE_INFO;
			prj.m_mapAmpStore.insert(make_pair(nOrginalID, pAmpInfo));
			pAmpInfo->dwIDSecond = nCopy[0];
			pAmpInfo->dwIDThird = nCopy[1];
			pAmpInfo->dwIDFourth = nCopy[2];
			pAmpInfo->dwIDFifth = nCopy[3];
		}
#endif // __DBSERVER

		nVer = scanner.GetNumber();	// version;
	}
	apObjProp->Optimize();

#ifdef __CLIENT
	m_bLoadedItemProp = TRUE;
#endif

	return TRUE;
}

BOOL CProject::LoadText(LPCTSTR lpszFileName)
{
	tagColorText colorText, * pColorText;

	CScript scanner;
	if (scanner.Load(lpszFileName) == FALSE)
		return FALSE;

	CStringArray strArray;
	CDWordArray  colorArray;
	DWORD dwId = scanner.GetNumber();

	do {
		DWORD dwColor = scanner.GetNumber(); // color
		scanner.GetToken();
		if (*scanner.token == '{')
		{

			scanner.GetToken();
			CString str = scanner.token;
			str.Replace("\"", "");
			//			if( str.IsEmpty() )
			//				str	= "Empty";
#ifdef _DEBUG
			if (strArray.GetSize() > (int)(dwId))
				if (strArray.GetAt(dwId).IsEmpty() == FALSE)
					Error("CProject::LoadText : 같은 아이디 존재 %d - %s", dwId, str);
#endif	// _DEBUG
			strArray.SetAtGrow(dwId, str);
			colorArray.SetAtGrow(dwId, dwColor);
			scanner.GetToken();	// }
		}
		dwId = scanner.GetNumber();	// next
	} while (scanner.tok != FINISHED);

	for (int i = 0; i < strArray.GetSize(); i++)
	{
		if (strArray.GetAt(i).IsEmpty() == FALSE)
		{
			m_colorText.SetAtGrow(i, &colorText);

			pColorText = m_colorText.GetAt(i);
			pColorText->dwColor = colorArray.GetAt(i);
			pColorText->lpszData = strdup(strArray.GetAt(i));
		}
	}
	m_colorText.Optimize();
	return TRUE;
}

CString CProject::GetLangScript(CScript& script)
{
	CString string;
	script.GetToken();
	string = script.Token;
	script.GetToken();	// )
	script.GetToken(); // ;
	if (string.IsEmpty())
		string = " ";
	return string;
}

void CProject::LoadStrings()
{
	CScript scanner;
	DWORD dwLanguage = ::GetLanguage();
	CString fileName;

	fileName = "PreLoadText.inc";
	if (scanner.Load(fileName) == FALSE)
	{
		Error("%s not found !", fileName);
		exit(0);
	}
	else
	{
		scanner.GetToken();
		while (scanner.tok != FINISHED)
		{
			if (scanner.Token == "CLIENT")
			{
#ifdef __CLIENT
				scanner.GetToken();
				CString str = scanner.token;
				CScript s;
				if (s.Load(str) == TRUE)
				{
					s.LoadString();
				}
				else
					Error("%s not found !", str);
#endif // __CLIENT
			}
			else if (scanner.Token == "SERVER")
			{
#if defined(__DBSERVER) || defined(__WORLDSERVER)
				scanner.GetToken();
				CString str = scanner.token;
				CScript s;
				if (s.Load(str) == TRUE)
				{
					s.LoadString();
				}
				else
					Error("%s not found !", str);
#endif // (__DBSERVER) || defined(__WORLDSERVER)
			}
			else if (scanner.Token == "COMMON")
			{
				scanner.GetToken();
				CString str = scanner.token;
				CScript s;
				if (s.Load(str) == TRUE)
				{
					s.LoadString();
				}
				else
					Error("%s not found !", str);
			}
			scanner.GetToken();
		}
	}

	LoadText("textClient.inc");
}

void CProject::LoadDefines()
{
	CScript scanner;
	if (scanner.Load("PreLoadDefine.inc") == FALSE)
	{
		Error("PreLoadDefine.inc not found");
		exit(0);
	}
	else
	{
		scanner.GetToken();
		while (scanner.tok != FINISHED)
		{
			if (scanner.Token == "CLIENT")
			{
#ifdef __CLIENT
				scanner.GetToken();
				CString str = scanner.token;
				CScript s;

				if (s.Load(str) == TRUE)
					s.PreScan();
				else
					Error("%s not found !", str);
#endif // __CLIENT
			}
			else if (scanner.Token == "SERVER")
			{
#if defined(__DBSERVER) || defined(__WORLDSERVER)
				scanner.GetToken();
				CString str = scanner.token;
				CScript s;
				if (s.Load(str) == TRUE)
					s.PreScan();
				else
					Error("%s not found !", str);
#endif // (__DBSERVER) || defined(__WORLDSERVER)
			}
			else if (scanner.Token == "COMMON")
			{
				scanner.GetToken();
				CString str = scanner.token;
				CScript s;
				if (s.Load(str) == TRUE)
					s.PreScan();
				else
					Error("%s not found !", str);
			}
			scanner.GetToken();
		}
	}
}

void CProject::LoadPreFiles()
{
#ifdef __SECURITY_0628
#ifdef __CLIENT
	CResFile::LoadAuthFile();
#endif	// __CLIENT
#endif	// __SECURITY_0628

	LoadDefines();
	LoadStrings();

#ifdef __LANG_1013
	CLangMan::GetInstance()->Load("propLang.txt");
#endif	// __LANG_1013
}


#ifdef __RULE_0615
BOOL	CProject::LoadAllowedLetter(void)
{
	CScanner s;
	set<char>* ptr = &m_sAllowedLetterAuth;
	if (s.Load("LetterAuth.inc"))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token.GetLength())
				ptr->insert(s.Token.GetAt(0));
			s.GetToken();
		}
	}
	else
	{
		Error("Cant load allowed letters (auth), return");
		return FALSE;
	}

	CScanner s2;
	set<char>* ptr2 = &m_sAllowedLetterGame;
	if (s2.Load("LetterGame.inc"))
	{
		s2.GetToken();
		while (s2.tok != FINISHED)
		{
			if (s2.Token.GetLength())
				ptr2->insert(s2.Token.GetAt(0));
			s2.GetToken();
		}
	}
	else
	{
		Error("Cant load allowed letters (game), return");
		return FALSE;
	}

	return TRUE;
}

BOOL	CProject::IsAllowedLetter(LPCSTR szName, BOOL bAuth)
{
	set<char>* ptr = NULL;
	if (bAuth)
		ptr = &m_sAllowedLetterAuth;
	else
		ptr = &m_sAllowedLetterGame;

	if (!ptr->size())
		return TRUE;

	int nLen = lstrlen(szName);
	for (int i = 0; i < nLen; i++)
	{
		char chLetter = szName[i];
		set<char>::iterator it = ptr->find(chLetter);
		if (it == ptr->end())
		{
			return FALSE;
		}
	}
	return TRUE;
}

void	CProject::Formalize(LPSTR szName)
{
	int nLen = lstrlen(szName);
	if (nLen == 0)
		return;

	DWORD dwLanguage = ::GetLanguage();
	switch (dwLanguage)
	{
	case LANG_GER:
	case LANG_FRE:
	{
		_strlwr(szName);
		char szBuffer[2];
		szBuffer[0] = szName[0];
		szBuffer[1] = '\0';
		_strupr(szBuffer);
		szName[0] = szBuffer[0];
		break;
	}
	default:
	{
		break;
	}
	}
}
#endif	// __RULE_0615
