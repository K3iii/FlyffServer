#include "stdafx.h"
#include "defineSound.h"
#include "DefineObj.h"
#include "CommonCtrl.h"
#include "Ship.h"
#ifdef __CLIENT
#include "Sfx.h"
#endif

#ifdef __CLIENT
CSfx* CreateSfx(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj,
	D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec, BOOL bManageWorld)
{
	CSfx* pObj = NULL;
	CMover* pSrc = prj.GetMover(idSrc);
	CMover* pDst = prj.GetMover(idDest);

#ifndef __WORLDSERVER
	switch (dwSfxObj)
	{
	case XI_SETIEM_EFFECTHAND:
	case XI_GEN_ITEM_SETITEM03:
	case XI_GEN_ITEM_SETITEM04:
	case XI_GEN_ITEM_SETITEM05:
	case XI_GEN_ITEM_SETITEM06:
	case XI_GEN_ITEM_SETITEM07:
	case XI_GEN_ITEM_SETITEM08:
	case XI_GEN_ITEM_SETITEM09:
	case XI_GEN_ITEM_SETITEM10:
		pObj = new CSfxPartsLinkShoulder(); break;

	case XI_HIT_SWORD01: pObj = new CSfxGenNormalDmg01(); break;
	case XI_HIT_SWORD02: pObj = new CSfxGenNormalDmg01(); break;
	case XI_HIT_STICK01: pObj = new CSfxGenNormalDmg01(); break;
	case XI_HIT_STICK02: pObj = new CSfxGenNormalDmg01(); break;

	case XI_FIR_WAND01: pObj = new CSfxItemWandAtk1(); break;

	case XI_FIR_WAND02: pObj = new CSfxItemWandAtkAir(); break;

	case XI_FIR_RANGE01: pObj = new CSfx(); break;
	case XI_FIR_RANGE02: pObj = new CSfx(); break;

	case XI_GEN_RESTORATION01: pObj = new CSfxGenRestoration01(); break;
	case XI_GEN_LOGIN01: pObj = new CSfxGenLogin(); break;
	case XI_GEN_WARP01: pObj = new CSfxGenWarp(); break;
	case XI_GEN_PC_DIE01: pObj = new CSfxGenPcDie(); break;
	case XI_GEN_MOVEMARK01: pObj = new CSfxGenMoveMark(); break;

	case XI_GEN_RAINCIRCLE01: pObj = new CSfxGenRainCircle(); break;

	case XI_GEN_WATERCIRCLE01: pObj = new CSfxGenWaterCircle(); break;
	case XI_GEN_WATERCROWN01: pObj = new CSfxGenWaterCrown(); PLAYSND(SND_GEN_WATER, &vPosSrc); break;

	case XI_ITEM_WAND_ATK1: pObj = new CSfxItemWandAtk1(); break;
	case XI_ITEM_WAND_ATK2: pObj = new CSfxItemWandAtk2(); break;
	case XI_ITEM_WAND_ATK3: pObj = new CSfxItemWandAtk3(); break;
	case XI_ITEM_WAND_ATK4: pObj = new CSfxItemWandAtk4(); break;

	case XI_ITEM_RANGE_ATK1: pObj = new CSfxItemRangeAtk1(); break;
	case XI_ITEM_RANGE_ATK2: pObj = new CSfxItemRangeAtk1(); break;
	case XI_ITEM_RANGE_ATK3: pObj = new CSfxItemRangeAtk1(); break;
	case XI_ITEM_RANGE_ATK4: pObj = new CSfxItemRangeAtk1(); break;

	case XI_SKILL_VAG_ONE_OVERCUTTER01:
	case XI_SKILL_VAG_ONE_CLEANHIT01:
	case XI_SKILL_VAG_ONE_BRANDISH01:
	case XI_SKILL_MER_ONE_SPECIALHIT02:
	case XI_SKILL_MER_ONE_REFLEXHIT02:
	case XI_SKILL_BIL_KNU_BELIALSMESHING02:
	case XI_SKILL_BLD_DOUBLE_CROSSSTRIKE02:
	case XI_SKILL_BLD_DOUBLE_ARMORPENETRATE02:
	case XI_SKILL_BIL_KNU_BLOODFIST02:
	case XI_SKILL_BLD_DOUBLE_SONICBLADE03:
	case XI_SKILL_BIL_KNU_PIERCINGSERPENT02:
	case XI_SKILL_RIN_SUP_MERKABAHANZELRUSHA03:
	case XI_SKILL_ELE_FIRE_BURINGFIELD02:
	case XI_SKILL_KNT_TWOSW_EARTHDIVIDER02:
	case XI_SKILL_STORM_STORMBLASTE04:
	case XI_SKILL_WINDL_COUNTERBACK01:
		pObj = new CSfxFixed();
		if (pObj && IsValidObj(pSrc))
			pObj->SetAngle(-pSrc->GetAngle());


		break;
	case XI_SKILL_BIL_KNU_SONICHAND02:
	case XI_SKILL_BIL_KNU_ASALRAALAIKUM02:
		pObj = new CSfxFixed();
		if (pObj && IsValidObj(pSrc))
		{
			float fAngle = -pSrc->GetAngle() + 180;
			pObj->SetAngle(fAngle);
		}

		break;

	case XI_HIT_YOY01:
	case XI_HIT_BOW01:
	case XI_SKILL_JST_YOYO_HITOFPENYA02:
	case XI_SKILL_JST_YOYO_VATALSTAB02:
	case XI_SKILL_ACR_YOYO_SLOWSTEP02:
	case XI_SKILL_RAG_BOW_FLAMEARROW03:
	case XI_SKILL_RAG_BOW_PIRCINGARROW03:
	case XI_SKILL_RAG_BOW_POISONARROW03:
	case XI_SKILL_RAG_BOW_SILENTARROW03:
	case XI_SKILL_RAG_SUP_NATURE02:
	case XI_SKILL_RAG_BOW_TRIPLESHOT03:

	case XI_SKILL_ACR_YOYO_SNITCH02:
	case XI_SKILL_ACR_BOW_AIMEDSHOT03:
	case XI_SKILL_ACR_BOW_JUNKBOW03:
	case XI_SKILL_ACR_BOW_SILENTSHOT03:
	case XI_SKILL_ACR_BOW_AUTOSHOT03:
	case XI_SKILL_BLD_DOUBLEAX_HAWKATTACK03:
	case XI_SKILL_BLD_DOUBLESW_BLADEDANCE02:
	case XI_SKILL_BLD_DOUBLESW_BLADEDANCE03:
	case XI_SKILL_KNT_TWO_POWERSWING02:
		pObj = new CSfxFixed();
		if (pObj && IsValidObj(pSrc))
			pObj->SetAngle(180.0f - pSrc->GetAngle());
		break;

	case XI_SKILL_MER_ONE_KEENWHEEL01: pObj = new CSfxSkillMerKeenWheel(); break;
	case XI_SKILL_MER_ONE_SPLMASH01: pObj = new CSfxSkillMerSplmash(); break;
	case XI_SKILL_MER_ONE_BLINDSIDE01: pObj = new CSfxSkillMerBlindSide(); break;
	case XI_SKILL_MAG_WIND_SWORDWIND01: pObj = new CSfxSkillMagSwordWind(); break;
	case XI_SKILL_MAG_WIND_STRONGWIND01: pObj = new CSfxSkillMagStrongWind(); break;
	case XI_SKILL_MAG_FIRE_BOOMERANG01: pObj = new CSfxSkillMagFireBoomerang(); break;
	case XI_SKILL_MAG_FIRE_HOTAIR01: pObj = new CSfxSkillMagHotAir(); break;

	case XI_SKILL_MAG_EARTH_CASTING01:
	case XI_SKILL_MAG_ELECTRICITY_CASTING01:
	case XI_SKILL_MAG_WATER_CASTING01:
	case XI_SKILL_MAG_MAG_CASTING01:
	case XI_SKILL_ELE_FIRE_CASTING01:
	case XI_SKILL_ASS_CHEER_HASCASTING01:
	case XI_SKILL_ELE_MULTI_CASTING01:
	case XI_SKILL_PSY_PSY_CASTING01:
	case XI_SKILL_MAG_FIRE_CASTING01:
	case XI_SKILL_ASS_HEAL_CASTING01:
	case XI_SKILL_ASS_RES_CASTING01:
		pObj = new CSfxSkillMagFireCasting();
		break;
	case XI_SKILL_MENT_DARKNESSRAKE01:
	case XI_SKILL_MENT_DARKNESSSCREAM01:
	case XI_SKILL_LORDK_SHILDSTRIKE01:
	case XI_SKILL_ELE_FINALSPEAR01:
	case XI_SKILL_LORDK_SCOPESTRIKE02:
		pObj = new CSfxSkillCastingFrontDynamic();
		break;
	case XI_SKILL_MENT_DARKNESSRAKE02:
		pObj = new CSfxSkillCastingLSideDynamic();
		break;
	case XI_SKILL_MENT_ATTACKDECREASE01:
	case XI_SKILL_MENT_DEFENDERDECREASE01:
	case XI_SKILL_MENT_SPEEDDECREASE01:
	case XI_SKILL_CRACK_POWERINCREASE01:
	case XI_SKILL_CRACK_CONTROL01:
	case XI_SKILL_WINDL_CONTROLINCREASE01:
	case XI_SKILL_ELE_SLEEPING01:
		pObj = new CSfxSkillAboveTargetDynamic();
		break;

	case XI_SKILL_MAG_WIND_CASTING01: pObj = new CSfxSkillMagWindCasting(); break;

	case XI_SKILL_ASS_CHEER_CASTING01: pObj = new CSfxSkillMagWindCasting(); break;

	case XI_GEN_CURE01: PLAYSND(SND_GEN_LEVELUP, &vPosSrc); pObj = new CSfx(); break;
	case XI_GEN_REF01: PLAYSND(SND_GEN_LEVELUP, &vPosSrc); pObj = new CSfx(); break;

	case XI_HIT_CRITICAL01: pObj = new CSfx(); break;
	case XI_HIT_MISS01: pObj = new CSfx(); break;

	case XI_NPC_RAN_SPITTL: pObj = new CSfxItemWandAtk1(); break;
	case XI_NPC_RAN_GAS: pObj = new CSfxItemWandAtk1(); break;
	case XI_NPC_RAN_MAGICBALL: pObj = new CSfxItemWandAtk1(); break;
	case XI_NPC_RAN_MAGICBLUE: pObj = new CSfxItemWandAtk1(); break;
	case XI_NPC_RAN_CARD: pObj = new CSfxItemWandAtk1(); break;
	case XI_NPC_RAN_CANNON: pObj = new CSfxItemWandAtk1(); break;
	case XI_SKILL_TRO_STRETCHING01: pObj = new CSfxTroStretching01(); break;
	case XI_SKILL_TRO_STRETCHING02: pObj = new CSfxTroStretching02(); break;
	case XI_SKILL_TRO_BLITZ: pObj = new CSfxTroBlitz(); break;
	case XI_SKILL_ASS_KNU_BURSTCRACK01: pObj = new CSfxSkillAssBurstcrack(); break;
	case XI_SKILL_ASS_KNU_TAMPINGHOLE01: pObj = new CSfxSkillAssTampinghole(); break;
	case XI_NPC_DIR_STEAM: pObj = new CSfxNpcDirSteam(); break;
	case XI_ITEM_COLLECT: pObj = new CSfxCollect();		break;
	case XI_NPCSP1DIRAMP:		pObj = new CSfxClockWorksCharge();	break;
	case XI_NPCSP1DIRCANNON:	pObj = new CSfxClockWorksCannon();	break;
	case XI_NPCSP1RANBALL:		pObj = new CSfxAtkStraight();		break;
	case XI_SKILL_MUSHMOOT_CHARGE:	pObj = new CSfxMushmootCharge();	break;
	case XI_SKILL_MAG_WATER_ICEMISSILE01: pObj = new CSfxSkillMagIceMissile(); break;
	case XI_SKILL_MER_ONE_BLOODYSTRIKE02:
		pObj = new CSfx();
		if (pObj)
		{
			CMover* pSrc = prj.GetMover(idSrc);
			CMover* pDst = prj.GetMover(idDest);
			if (IsValidObj(pSrc) && IsValidObj(pDst))
			{
				FLOAT fAngle = GetDegree(pSrc->GetPos(), pDst->GetPos());
				pObj->SetAngle(-fAngle);
			}
		}
		break;

	case XI_SKILL_MAG_ELECTRICITY_LIGHTINGBALL01: 	pObj = new CSfxSkillMagLightningBall();		break;
	case XI_SKILL_MAG_FIRE_FIRESTRIKE02:		pObj = new CSfxShoot();		break;
	case XI_SKILL_MAG_EARTH_SPIKESTONE01:	pObj = new CSfxSkillMagSpikeStone();		break;
	case XI_NAT_WINGANGEL01:
		pObj = new CSfxRotate();
		break;
	case XI_NAT_WASTART01:
		pObj = new CSfx();
		CreateSfx(pd3dDevice, XI_NAT_WINGANGEL01, vPosSrc, idSrc, vPosDest, idDest, nSec);
		break;
	case XI_NAT_CUPITSTART01:
		pObj = new CSfx();
		CreateSfx(pd3dDevice, XI_NAT_WINGANGEL01, vPosSrc, idSrc, vPosDest, idDest, nSec);
		break;
	case XI_SKILL_KNT_TWO_POWERSWING01:
		idDest = NULL_ID;
		pObj = new CSfx();
		break;
	case XI_SKILL_MER_ONE_GUILOTIN02:
		pObj = new CSfxShootWave();
		break;
	case XI_SKILL_DROP_DUST_RAIN:
		pObj = new CSfxItemRangeAtk1_StoneRain();
		break;
	case XI_SKILL_DROP_DUST:
		pObj = new CSfxItemRangeAtk1_Stone();
		break;
	case XI_SKILL_ACR_BOW_ARROWRAIN03:
		pObj = new CSfxItemRangeAtk1_AllowRain();
		break;
	case XI_SKILL_RAG_BOW_ARROWRAIN:
		pObj = new CSfxItemRangeAtk1_Allow();
		break;


	case XI_SKILL_RAG_BOW_ICEARROW01:
	case XI_SKILL_RAG_SUP_FASTATTACK01:
	case XI_SKILL_RAG_BOW_FLAMEARROW01:
	case XI_SKILL_RAG_BOW_PIRCINGARROW01:
	case XI_SKILL_RAG_BOW_POISONARROW01:
	case XI_SKILL_RAG_BOW_SILENTARROW01:
	case XI_SKILL_RAG_BOW_TRIPLESHOT01:

	case XI_SKILL_ACR_BOW_JUNKBOW01:
	case XI_SKILL_ACR_BOW_AIMEDSHOT01:
	case XI_SKILL_ACR_BOW_SILENTSHOT01:
	case XI_SKILL_ACR_BOW_ARROWRAIN01:
	case XI_SKILL_ACR_BOW_AUTOSHOT01:
	case XI_SKILL_MER_ONE_SUPPORT01:
	case XI_SKILL_MER_ONE_SUPPORT02:
	case XI_SKILL_MER_ONE_SUPPORT03:
	case XI_SKILL_MER_ONE_SUPPORT04:
	case XI_SKILL_ASS_KNU_SUPPORT01:
	case XI_SKILL_ASS_KNU_SUPPORT02:
	case XI_SKILL_ASS_KNU_SUPPORT03:
	case XI_SKILL_LORDK_GRANDRAGE01:
	case XI_SKILL_LORDK_TEMPLARPULLING01:
	case XI_SKILL_CRACK_RANGESTRIKE01:
		pObj = new CSfxAllowPartsLink();
		break;

	case XI_SKILL_JST_YOYO_VATALSTAB01:
	case XI_SKILL_JST_YOYO_HITOFPENYA01:
	case XI_SKILL_ACR_YOYO_SLOWSTEP01:
	case XI_SKILL_JST_SUP_POISON01:
	case XI_SKILL_JST_SUP_BLEEDING01:
	case XI_SKILL_JST_YOYO_BACKSTAB01:
	case XI_SKILL_JST_SUP_ABSORB01:
	case XI_SKILL_JST_SUP_CRITICALSWING01:
		pObj = new CSfxPartsLinkJst();
		break;

	case XI_SKILL_BLD_DOUBLEAX_SPRINGATTACK01:
	case XI_SKILL_BLD_DOUBLE_SONICBLADE01:
	case XI_SKILL_KNT_TWOSW_CHARGE01:
	case XI_SKILL_BLD_DOUBLESW_BLADEDANCE01:
		pObj = new CSfxPartsLinkBlade();
		break;
	case XI_EVE_EVENT_FAIRYLY:
		pObj = new CSfxFixed();
		break;

	case XI_CHR_CURSOR1:
		pObj = new CSfxCursor();
		break;

	case XI_BUFFPET_GRADE1:
	case XI_BUFFPET_GRADE2:
	case XI_BUFFPET_GRADE3:
		pObj = new CSfxLinkMover();
		break;

	case XI_INT_INCHANT:
		pObj = new CSfxInchant();
		break;

	case 0: break;
	default: pObj = new CSfx(); break;
	}
	if (pObj)
	{
		int bRet = pObj->SetSfx(pd3dDevice, dwSfxObj, vPosSrc, idSrc, vPosDest, idDest, nSec);
		if (bRet)
		{
			if (pObj->GetModel() == NULL)
			{
				Error("CreateSfx : %d sfx의 m_pModel이 없음", dwSfxObj);
			}
			if (pObj->GetModel()->m_pModelElem == NULL)
			{
				Error("CreateSfx : %d sfx의 m_pModelElem이 없음", dwSfxObj);
			}
			if (pObj->GetModel()->m_pModelElem->m_szPart[0])
				PLAYSND(pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc);


			switch (dwSfxObj)
			{
			case XI_SKILL_MER_ONE_GUILOTIN02:
			{
				CSfxShootWave* pSfxShootWave = (CSfxShootWave*)pObj;
				pSfxShootWave->SetHitSfx(XI_SKILL_MER_ONE_GUILOTIN02, idSrc, idDest, 2.0f);
				pSfxShootWave->SetDir(TRUE);
			}
			break;
			case XI_SKILL_MAG_FIRE_FIRESTRIKE02:
			{
				CSfxShoot* pSfxShoot = (CSfxShoot*)pObj;
				pSfxShoot->SetDir(TRUE);
			}
			break;
			case XI_SKILL_MER_ONE_SNEAKER02:
			{
				CMover* pDst = prj.GetMover(idDest);
				if (IsValidObj(pDst))
				{
					FLOAT fRadius = pDst->GetRadius();
					pObj->SetScale(D3DXVECTOR3(fRadius, fRadius, fRadius));
				}
			}
			break;
			}

			if (bManageWorld)
			{
				if (g_WorldMng.Get()->AddObj(pObj) == FALSE)
					SAFE_DELETE(pObj);
			}
		}
		else
		{
			SAFE_DELETE(pObj);
		}
	}
#endif	// __WORLDSERVER
	return pObj;
}


CSfx* CreateSfxYoYo(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj,
	D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec)
{
	CSfx* pObj = NULL;
	CMover* pSrc = prj.GetMover(idSrc);
	CMover* pDst = prj.GetMover(idDest);

#ifndef __WORLDSERVER
	pObj = new CSfxItemYoyoAtk();
	if (pObj)
	{
		int bRet = pObj->SetSfx(pd3dDevice, dwSfxObj, vPosSrc, idSrc, vPosDest, idDest, nSec);
		if (bRet)
		{
			if (pObj->GetModel() == NULL)
				Error("CreateSfxYoYo : %d GetModel() null", dwSfxObj);

			if (pObj->GetModel()->m_pModelElem == NULL)
				Error("CreateSfxYoYo : %d m_pModelElem null", dwSfxObj);

			if (pObj->GetModel()->m_pModelElem->m_szPart[0])
				PLAYSND(pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc);

			// __AIO
			if (pSrc)
			{
				CModelObject* pModel = (CModelObject*)pSrc->m_pModel;
				if (pModel)
				{
					O3D_ELEMENT* pElem = pModel->GetParts(PARTS_RWEAPON);
					if (pElem && pElem->m_pObject3D)
					{
						CSfxBase* pSfxBase = pObj->m_pSfxObj->m_pSfxBase;
						if (pSfxBase && pSfxBase->Part(0))
							pSfxBase->Part(0)->m_strTex = pElem->m_pObject3D->m_szFileName;
					}
				}
			}

			if (g_WorldMng.Get()->AddObj(pObj) == FALSE)
				SAFE_DELETE(pObj);
		}
		else
			SAFE_DELETE(pObj);
	}
#endif	// __WORLDSERVER
	return pObj;
}

CSfxShoot* CreateShootSfx(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj,
	D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec)
{
	CSfxShoot* pObj = NULL;
	CMover* pSrc = prj.GetMover(idSrc);
	CMover* pDst = prj.GetMover(idDest);

#ifndef __WORLDSERVER
	switch (dwSfxObj)
	{
	case XI_FIR_WAND01: pObj = new CSfxItemWandAtk1(); break;
	case XI_FIR_WAND02: pObj = new CSfxItemWandAtkAir(); break;

	case XI_ITEM_WAND_ATK1: pObj = new CSfxItemWandAtk1(); break;
	case XI_ITEM_WAND_ATK2: pObj = new CSfxItemWandAtk2(); break;
	case XI_ITEM_WAND_ATK3: pObj = new CSfxItemWandAtk3(); break;
	case XI_ITEM_WAND_ATK4: pObj = new CSfxItemWandAtk4(); break;

	case XI_SKILL_RAG_BOW_TRIPLESHOT02:
	case XI_SKILL_ACR_BOW_JUNKBOW02:
		pObj = new CSfxItemRangeAtk_JunkBow(); break;

	case XI_SKILL_RAG_BOW_ICEARROW02:
	case XI_SKILL_RAG_BOW_FLAMEARROW02:
	case XI_SKILL_RAG_BOW_PIRCINGARROW02:
	case XI_SKILL_RAG_BOW_POISONARROW02:
	case XI_SKILL_RAG_BOW_SILENTARROW02:

	case XI_SKILL_ACR_BOW_AIMEDSHOT02:
	case XI_SKILL_ACR_BOW_SILENTSHOT02:
	case XI_SKILL_ACR_BOW_ARROWRAIN02:
	case XI_SKILL_ACR_BOW_AUTOSHOT02:

	case XI_ITEM_RANGE_ATK1: pObj = new CSfxItemRangeAtk1(); break;
	case XI_ITEM_RANGE_ATK2: pObj = new CSfxItemRangeAtk1(); break;
	case XI_ITEM_RANGE_ATK3: pObj = new CSfxItemRangeAtk1(); break;
	case XI_ITEM_RANGE_ATK4: pObj = new CSfxItemRangeAtk1(); break;

	case XI_SKILL_MAG_WIND_SWORDWIND01: pObj = new CSfxSkillMagSwordWind(); break;
	case XI_SKILL_MAG_WIND_STRONGWIND01: pObj = new CSfxSkillMagStrongWind(); break;
	case XI_SKILL_MAG_FIRE_BOOMERANG01: pObj = new CSfxSkillMagFireBoomerang(); break;
	case XI_SKILL_MAG_FIRE_HOTAIR01: pObj = new CSfxSkillMagHotAir(); break;

	case XI_NPC_RAN_SPITTL:
	case XI_NPC_RAN_GAS:
	case XI_NPC_RAN_MAGICBALL:
	case XI_NPC_RAN_MAGICBLUE:
	case XI_NPC_RAN_CARD:
	case XI_NPC_RAN_CANNON:
		pObj = new CSfxItemWandAtk1(); break;

	case XI_NPCSP1RANBALL:		pObj = new CSfxAtkStraight();		break;

	case XI_SKILL_MAG_WATER_ICEMISSILE01: pObj = new CSfxSkillMagIceMissile(); break;


	case XI_SKILL_MAG_ELECTRICITY_LIGHTINGBALL01: 	pObj = new CSfxSkillMagLightningBall();		break;
	case XI_SKILL_MAG_EARTH_SPIKESTONE01:	pObj = new CSfxSkillMagSpikeStone();		break;
	case XI_SKILL_MER_ONE_GUILOTIN02:
		pObj = new CSfxShootWave();
		break;

	case 0:		break;
	default:
		pObj = new CSfxShoot();
		break;
	}

	if (pObj)
	{
		D3DXVECTOR3 vPosShoot = vPosSrc;	vPosShoot.y += 1.0f;
		int bRet = pObj->SetSfx(pd3dDevice, dwSfxObj, vPosShoot, idSrc, vPosDest, idDest, nSec);
		if (bRet)
		{
			if (pObj->GetModel()->m_pModelElem == NULL)
			{
				Error("CreateShootSfx : %d sfx의 m_pModelElem이 없음", dwSfxObj);
			}
			if (pObj->GetModel()->m_pModelElem->m_szPart[0])
				PLAYSND(pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc);

			pObj->SetDir(TRUE);


			switch (dwSfxObj)
			{
			case XI_SKILL_MER_ONE_GUILOTIN02:
			{
				CSfxShootWave* pSfxShootWave = (CSfxShootWave*)pObj;
				pSfxShootWave->SetHitSfx(XI_SKILL_MER_ONE_GUILOTIN02, idSrc, idDest, 2.0f);
				pSfxShootWave->SetDir(TRUE);
			}
			break;
			}


			if (g_WorldMng.Get()->AddObj(pObj) == FALSE)
				SAFE_DELETE(pObj);
		}
		else
		{
			SAFE_DELETE(pObj);
		}
	}
#endif	// __WORLDSERVER
	return pObj;
}

CSfx* CreateItemReadySfx(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj,
	D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec)
{
	CSfx* pObj = NULL;
	CMover* pSrc = prj.GetMover(idSrc);
	CMover* pDst = prj.GetMover(idDest);
#ifndef __WORLDSERVER
	pObj = new CSfxReady();
	if (pObj)
	{
		int bRet = pObj->SetSfx(pd3dDevice, dwSfxObj, vPosSrc, idSrc, vPosDest, idDest, nSec);
		if (bRet)
		{
			if (pObj->GetModel() == NULL)
			{
				Error("CreateItemReadySfx : %d sfx의 m_pModel이 없음", dwSfxObj);
			}
			if (pObj->GetModel()->m_pModelElem == NULL)
			{
				Error("CreateItemReadySfx : %d sfx의 m_pModelElem이 없음", dwSfxObj);
			}
			if (pObj->GetModel()->m_pModelElem->m_szPart[0])
				PLAYSND(pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc);

			if (g_WorldMng.Get()->AddObj(pObj) == FALSE)
				SAFE_DELETE(pObj);
		}
		else
			SAFE_DELETE(pObj);
	}
#endif // not World

	return pObj;
}

#endif //__CLIENT

#ifdef __LAYER_1015
CMover* CreateMover(CWorld* pWorld, DWORD dwID, const D3DXVECTOR3& vPos, int nLayer)
#else	// __LAYER_1015
CMover* CreateMover(CWorld* pWorld, DWORD dwID, D3DXVECTOR3 vPos)
#endif	// __LAYER_1015
{
	CMover* pMover;

	pMover = (CMover*)CreateObj(D3DDEVICE, OT_MOVER, dwID);
	if (pMover == NULL)
		return NULL;
	pMover->SetPos(vPos);
	pMover->InitMotion(MTI_STAND);
	pMover->UpdateLocalMatrix();

	pMover->AddItToGlobalId();
	if (pWorld->ADDOBJ(pMover, FALSE, nLayer) == FALSE)
	{
		SAFE_DELETE(pMover);
		return NULL;
	}

	return pMover;
}

CObj* CreateObj(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwObjType, DWORD dwObjIndex, BOOL bInitProp)
{
	CObj* pObj = NULL;
	switch (dwObjType)
	{
	case OT_OBJ: pObj = new CObj;			break;
	case OT_CTRL: pObj = new CCommonCtrl;	break;
	case OT_SFX:
#ifdef __CLIENT
		pObj = new CSfx;
#else
		return NULL;
#endif
		break;
	case OT_ITEM: pObj = new CItem;		break;
	case OT_MOVER: pObj = new CMover;		break;
	}
	pObj->m_dwType = dwObjType;

	if (pObj && pObj->GetType() == OT_SFX)
	{
#ifdef __CLIENT
		((CSfx*)pObj)->SetSfx(pd3dDevice, dwObjIndex, D3DXVECTOR3(0.0f, 0.0f, 0.0f), NULL_ID, D3DXVECTOR3(0.0f, 0.0f, 0.0f), NULL_ID);
#endif
		return pObj;
	}
	else if (pObj->SetIndex(pd3dDevice, dwObjIndex, bInitProp) == TRUE)
	{
		pObj->SetMotion(MTI_STAND);
		if (dwObjType == OT_MOVER)
		{
			CMover* pMover = (CMover*)pObj;
			pMover->m_pActMover->m_fSpeed = pMover->GetProp()->fSpeed;

			MoverProp* pProp = pMover->GetProp();
			if (pProp->dwAI == AII_MONSTER)
			{
				pMover->m_pActMover->m_fSpeed /= 2.0F;
			}
			else if (pProp->dwAI == AII_PET)
				pMover->m_pActMover->m_fSpeed *= 3.5F;
		}
		return pObj;
	}

	Error("CreateObj : type=%d, index=%d, bInitProp=%d", dwObjType, dwObjIndex, bInitProp);
	SAFE_DELETE(pObj);
	return NULL;
}


CObj* ReadObj(CFileIO* pFile)
{
	DWORD dwObjType;
	pFile->Read(&dwObjType, sizeof(DWORD), 1);

	CObj* pObj = NULL;
	switch (dwObjType)
	{
	case OT_OBJ: pObj = new CObj; break;
	case OT_CTRL: pObj = new CCommonCtrl; break;
	case OT_SFX:
#ifdef __CLIENT
		pObj = new CSfx;
#endif
		break;
	case OT_ITEM: pObj = new CItem; break;
	case OT_MOVER:
		pObj = new CMover;
		((CMover*)pObj)->InitNPCProperty();
		break;
	}

	if (pObj)
	{
		pObj->Read(pFile);
		return pObj;
	}
	return NULL;
}
BOOL CheckVersion(FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion)
{
	CHAR szName[32];
	CHAR szVersion[32];

	fread(szName, sizeof(szName), 1, fp);
	fread(szVersion, sizeof(szVersion), 1, fp);

	if (!strcmp(szName, lpszName) && !strcmp(szVersion, lpszVersion))
		return TRUE;

	return FALSE;
}
BOOL WriteVersion(FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion)
{
	if (strlen(lpszName) >= 32 || strlen(lpszVersion) >= 32)
		return FALSE;
	fwrite(lpszName, sizeof(32), 1, fp);
	fwrite(lpszVersion, sizeof(32), 1, fp);
	return TRUE;
}
BOOL CheckVersion(FILE* fp, DWORD dwVersion)
{
	DWORD dwVer;
	fread(&dwVer, sizeof(DWORD), 1, fp);

	if (dwVer == dwVersion)
		return TRUE;

	return FALSE;
}
BOOL WriteVersion(FILE* fp, DWORD dwVersion)
{
	fwrite(&dwVersion, sizeof(DWORD), 1, fp);
	return TRUE;
}
