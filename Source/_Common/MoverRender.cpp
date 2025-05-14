#include "stdafx.h"
#include "defineObj.h"
#include "guild.h"
#include "authorization.h"
#include "AppDefine.h"
#include "sfx.h"
#include "party.h"


extern	CPartyMng	g_PartyMng;

#include "eveschool.h"
#include "defineskill.h"
#include "commonctrl.h"

extern CGuildCombat g_GuildCombatMng;


#ifndef __WORLDSERVER

////////////////////////////////////////////////////////////////////////////////////////
// CMover
////////////////////////////////////////////////////////////////////////////////////////
void CMover::SetRenderPartsEffect(int nParts)
{
	CModelObject* pModel = (CModelObject*)m_pModel;

	if (!m_pModel)
		return;

	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp = NULL;

#ifndef __MODEL_CHANGE
	if (IsActiveMover())
	{
		pItemElem = GetEquipItem(nParts);
		if (pItemElem)
			pItemProp = pItemElem->GetProp();
	}
	else
	{
		DWORD dwItemID = m_aEquipInfo[nParts].dwId;
		if (dwItemID != NULL_ID)
			pItemProp = prj.GetItemProp(dwItemID);
	}
#else
	if (IsActiveMover())
	{
		pItemElem = GetEquipItem(nParts);
		if (pItemElem)
			pItemProp = pItemElem->IsChangedLook() ? prj.GetItemProp(pItemElem->GetLook()) : pItemElem->GetProp();
	}
	else
	{
		if (m_aEquipInfo[nParts].dwId != NULL_ID)
			pItemProp = m_aEquipInfo[nParts].isLook() ? prj.GetItemProp(m_aEquipInfo[nParts].dwLook) : prj.GetItemProp(m_aEquipInfo[nParts].dwId);
	}
#endif

	if (pItemProp)
	{
		if (pItemProp->nReflect > 0)
		{
			pModel->SetEffect(nParts, XE_REFLECT);

			if (nParts == PARTS_RWEAPON && pItemProp->dwItemKind3 == IK3_YOYO)
				pModel->SetEffect(PARTS_LWEAPON, XE_REFLECT);
		}

		switch (pItemProp->dwSfxElemental)
		{
		case ELEMENTAL_FIRE:	pModel->SetEffect(nParts, XE_ITEM_FIRE | (5 << 24)); break;
		case ELEMENTAL_ELEC:	pModel->SetEffect(nParts, XE_ITEM_ELEC | (0 << 24)); break;
		case ELEMENTAL_WATER:	pModel->SetEffect(nParts, XE_ITEM_WATER | (5 << 24)); break;
		case ELEMENTAL_WIND:	pModel->SetEffect(nParts, XE_ITEM_WIND | (5 << 24)); break;
		case ELEMENTAL_EARTH:	pModel->SetEffect(nParts, XE_ITEM_EARTH | (5 << 24)); break;
			break;
		}

		if (pItemProp->dwSfxElemental == -1)
		{
			int nAttrLevel = 0;
			int	nAttr = 0;
			int nLevel = 0;
			if (pItemElem)
			{
				nAttrLevel = pItemElem->m_nResistAbilityOption;
				nAttr = pItemElem->m_bItemResist;
				nLevel = pItemElem->GetAbilityOption();
			}
			else
			{
				nAttr = (m_aEquipInfo[nParts].nOption & 0x00FF0000) >> 16;
				nAttrLevel = (m_aEquipInfo[nParts].nOption & 0xFF000000) >> 24;
				nLevel = m_aEquipInfo[nParts].nOption & 0xFF;
			}

			int nEffLevel = 0;
#if __VER >= 13 // __EXT_ENCHANT
			if (nAttrLevel > 10)
				nAttrLevel = 10;
#endif // __EXT_ENCHANT
			if (nAttr && (nAttrLevel > 10 || nAttrLevel < 0))
			{
				LPCTSTR szErr = Error("m_nResistAbilityOption=%d %s", nAttrLevel, GetName());
				//ADDERRORMSG( szErr );
				nAttrLevel = 10;
			}

			DWORD dwItemFire = XE_ITEM_FIRE;
			DWORD dwItemElec = XE_ITEM_ELEC;
			DWORD dwItemWater = XE_ITEM_WATER;
			DWORD dwItemWind = XE_ITEM_WIND;
			DWORD dwItemEarth = XE_ITEM_EARTH;
			DWORD dwItemNone = XE_ITEM_GEN;

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
#ifndef __CSC_ENCHANT_EFFECT_2
			if (prj.m_nEnchantLimitLevel[2] >= nLevel)
				return;
#endif //__CSC_ENCHANT_EFFECT_2
			{
				nEffLevel = nLevel;

				switch (pItemProp->dwReferStat1)
				{
				case WEAPON_GENERAL:
				case WEAPON_UNIQUE:
				{
					if (prj.m_nEnchantLimitLevel[0] > nAttrLevel)
						return;
				}
				break;
				case WEAPON_ULTIMATE:
				{
					if (prj.m_nEnchantLimitLevel[1] > nAttrLevel)
						return;

					dwItemFire = XE_ITEM_FIRE_AL;
					dwItemElec = XE_ITEM_ELEC_AL;
					dwItemWater = XE_ITEM_WATER_AL;
					dwItemWind = XE_ITEM_WIND_AL;
					dwItemEarth = XE_ITEM_EARTH_AL;
					dwItemNone = XE_ITEM_GEN_AL;
				}
				break;
				}
			}
#else //__Y_ADV_ENCHANT_EFFECT
			if (nAttrLevel == 10)		nEffLevel = 5;
			else if (nAttrLevel == 9)	nEffLevel = 4;
			else if (nAttrLevel == 8)	nEffLevel = 3;
			else if (nAttrLevel == 7)	nEffLevel = 2;
			else if (nAttrLevel >= 4)	nEffLevel = 1;	// 4 ~ 6
			else
				nEffLevel = 0;
#endif //__Y_ADV_ENCHANT_EFFECT
			if (nEffLevel >= 0)
#ifdef __CSC_ENCHANT_EFFECT_2
#else //__CSC_ENCHANT_EFFECT_2
				if (nEffLevel >= 1)
#endif //__CSC_ENCHANT_EFFECT_2
				{
					nEffLevel <<= 24;

					switch (nAttr)
					{
#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
					case 0:						pModel->SetEffect(nParts, dwItemNone | nEffLevel);	break;
#endif //__Y_ADV_ENCHANT_EFFECT
					case SAI79::FIRE:			pModel->SetEffect(nParts, dwItemFire | nEffLevel);	break;
					case SAI79::ELECTRICITY:	pModel->SetEffect(nParts, dwItemElec | nEffLevel);	break;
					case SAI79::WATER:			pModel->SetEffect(nParts, dwItemWater | nEffLevel);	break;
					case SAI79::WIND:			pModel->SetEffect(nParts, dwItemWind | nEffLevel);	break;
					case SAI79::EARTH:			pModel->SetEffect(nParts, dwItemEarth | nEffLevel);	break;
					}
				}
		}
	}
}
void CMover::Render(LPDIRECT3DDEVICE9 pd3dDevice)
{
	MoverProp* pMoverProp = GetProp();
	if (pMoverProp == NULL)
		Error("CMover::Render : 프로퍼티 못읽음 %s", GetName());

	if (!IsVisible() || IsCull())
		return;

	D3DXVECTOR3 vPos = GetScrPos();

	pd3dDevice->SetMaterial(g_TextureMng.GetMaterial(pd3dDevice, 0));

	D3DXMATRIX  mWorld = m_matWorld;

	switch (GetIndex())
	{
	case MI_SYLIACA1:
	case MI_SYLIACA2:
	case MI_SYLIACA3:
	case MI_SYLIACA4:
		if (m_pModel)
			mWorld._42 = (float)(mWorld._42 + (fabs(m_pModel->m_vMax.y - m_pModel->m_vMin.y) / 2.0));
		break;
	}

	if (GetIndex() == MI_MASA_TINA)
	{
		ResetScale();
		D3DXMatrixScaling(&m_matScale, GetScale().x, GetScale().y, GetScale().z);
	}

	D3DXVECTOR3 vPos2 = GetScrPos() - GetWorld()->m_pCamera->m_vPos;
	int nLength = (int)(D3DXVec3Length(&vPos2));
	if (nLength > CWorld::m_fFarPlane)
		nLength = (int)(CWorld::m_fFarPlane);
	if (nLength > CWorld::m_fFarPlane - (CWorld::m_fFarPlane / 4))
		return;

	CModelObject* pModel = (CModelObject*)m_pModel;

	SetRenderPartsEffect(PARTS_RWEAPON);
	SetRenderPartsEffect(PARTS_LWEAPON);
	SetRenderPartsEffect(PARTS_SHIELD);

	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp = NULL;
	if (IsActiveMover())
	{
		pItemElem = GetEquipItem(PARTS_SHIELD);
		if (pItemElem)
		{
			if (pItemElem->IsChangedLook())
				pItemProp = prj.GetItemProp(pItemElem->GetLook());
			else
				pItemProp = pItemElem->GetProp();
		}
	}
	else
	{
		DWORD dwItemID = m_aEquipInfo[PARTS_SHIELD].dwId;
		if (dwItemID != NULL_ID)
		{
			if (m_aEquipInfo[PARTS_SHIELD].dwLook != NULL_ID)
				pItemProp = prj.GetItemProp(m_aEquipInfo[PARTS_SHIELD].dwLook);
			else
				pItemProp = prj.GetItemProp(dwItemID);
		}
	}
	if (pItemProp)
	{
		if (pItemProp->nReflect > 0)
			pModel->SetEffect(PARTS_SHIELD, XE_REFLECT);
	}


	O3D_ELEMENT* pElem = pModel->GetParts(PARTS_HAIR);
	if (pElem && pElem->m_pObject3D)
	{
		pElem->m_pObject3D->m_fAmbient[0] = m_fHairColorR;
		pElem->m_pObject3D->m_fAmbient[1] = m_fHairColorG;
		pElem->m_pObject3D->m_fAmbient[2] = m_fHairColorB;
	}

	if (GetActiveObj() == this)
		m_pModel->SetGroup(0);
	else
	{
		if (m_pModel->m_nNoEffect == 0)
		{
			float fDist = 50.0f;
			switch (g_Option.m_nObjectDetail)
			{
			case 0:	fDist = 20.0f;	break;
			case 1:	fDist = 10.0f;	break;
			case 2:	fDist = 5.0f;	break;
			}
			int nLevel = (int)(m_fDistCamera / fDist);
			if (nLevel >= 2)
				nLevel = 2;
			if (nLevel < 0)
				nLevel = 0;

			m_pModel->SetGroup(nLevel);
		}
		else
			m_pModel->SetGroup(2);
	}

	if (pModel->m_pModelElem->m_nTextureEx > 0)
		pModel->SetTextureEx(pModel->m_pModelElem->m_nTextureEx);
	else
		pModel->SetTextureEx(0);

	g_WndMng.PrepareMoverModel(this, pModel);

	if (IsMode(TRANSPARENT_MODE))
	{
		BOOL bRender = FALSE;
		if (IsActiveMover())
			bRender = TRUE;

		if (bRender)
		{
			m_pModel->SetBlendFactor(80);
			m_pModel->Render(pd3dDevice, &mWorld);
			m_pModel->SetBlendFactor(255);
		}
	}
	else
	{
		m_pModel->SetBlendFactor(m_wBlendFactor);
		m_pModel->Render(pd3dDevice, &mWorld);
	}

	if (m_pObjHighlight == this)
	{
		if (m_pModel->m_nNoEffect == 0)
		{
			pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		}

		if (IsMode(TRANSPARENT_MODE) == 0)
		{
			if (m_pModel->m_nNoEffect == 0)
			{
				m_pModel->Render(pd3dDevice, &mWorld);
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}
		}

	}

	if (m_pActMover)
		m_pActMover->Render(pd3dDevice, &mWorld);

	if (IsMode(TRANSPARENT_MODE) == 0)
	{
		D3DXMATRIX mRide = m_matWorld;

		if (m_pRide && m_pActMover->IsFly())
		{
			m_pRide->m_nNoEffect = m_pModel->m_nNoEffect;
			ItemProp* pRideProp = prj.GetItemProp(m_dwRideItemIdx);

			if (pRideProp && pRideProp->dwItemKind3 == IK3_STICK)
			{
				if (pModel->GetMatrixBone(0))
				{
					D3DXMATRIX mBoundY;
					D3DXMatrixTranslation(&mBoundY, 0, pModel->GetMatrixBone(0)->_42, 0);
					D3DXMatrixMultiply(&mRide, &mBoundY, &m_matWorld);
				}
			}

			if (pRideProp && pRideProp->dwItemKind3 == IK3_WING)
			{
				static const int SPINE1_BONE = 4;
				static const int SPINE2_BONE = 5;
				D3DXMATRIX* pmatSpine1Bone = pModel->GetMatrixBone(SPINE1_BONE);
				D3DXMATRIX* pmatSpine2Bone = pModel->GetMatrixBone(SPINE2_BONE);
				if (pmatSpine1Bone && pmatSpine2Bone)
				{
					D3DXMATRIX matEvent;
					D3DXMatrixIdentity(&matEvent);

					D3DXMATRIX matRotation;
					D3DXMatrixIdentity(&matRotation);
					D3DXMatrixRotationX(&matRotation, D3DXToRadian(180));
					D3DXMatrixMultiply(&matEvent, &matRotation, &matEvent);
					D3DXMatrixRotationZ(&matRotation, D3DXToRadian(-90));
					D3DXMatrixMultiply(&matEvent, &matRotation, &matEvent);
					D3DXMatrixMultiply(&matEvent, &matEvent, pmatSpine1Bone);

					matEvent._41 = pmatSpine2Bone->_41;
					matEvent._42 = pmatSpine2Bone->_42;
					matEvent._43 = pmatSpine2Bone->_43;

					D3DXMatrixMultiply(&mRide, &matEvent, &m_matWorld);
				}
			}

			m_pRide->Render(pd3dDevice, &mRide);
			m_pRide->m_nNoEffect = 0;

			if (pRideProp && pRideProp->dwID == II_RID_RID_BOR_LADOLF || pRideProp->dwID == II_RID_RID_BOR_JLADOLF
				|| pRideProp->dwID == II_RID_RID_BOR_JLADOLF_S || pRideProp->dwID == II_RID_RID_BOR_LADOLF_S
				)
			{
				if (m_pLadolf == NULL)
				{
					m_pLadolf = new CModelObject;
					m_pLadolf->InitDeviceObjects(pd3dDevice);
					m_pLadolf->LoadBone("mvr_Ladolf.chr");
					m_pLadolf->LoadElement("mvr_Ladolf.o3d", 0);
					m_pLadolf->LoadMotion("mvr_Ladolf_stand.ani");
				}

				D3DXMATRIX mLadolf = mRide;
				D3DXVECTOR3 vLocal;

				FLOAT fAngXZ = GetAngle();
				FLOAT fAngH = GetAngleX();
				AngleToVector(&vLocal, fAngXZ, -fAngH - 10.0f, 1.4f);


				if (m_pActMover->GetStateFlag() & OBJSTAF_ACC)
				{
					if (m_dwLadolfFlag == 0)
					{
						m_pLadolf->LoadMotion("mvr_Ladolf_walk.ani");
						m_pLadolf->SetMotionBlending(TRUE);
						m_pLadolf->SetLoop(ANILOOP_LOOP);
						m_dwLadolfFlag = 1;
					}
				}
				else
				{
					if (D3DXVec3LengthSq(&m_pActMover->m_vDelta) < 0.001f)
					{
						if (m_dwLadolfFlag == 1)
						{
							m_pLadolf->LoadMotion("mvr_Ladolf_stand.ani");
							m_pLadolf->SetMotionBlending(TRUE);
							m_pLadolf->SetLoop(ANILOOP_LOOP);
							m_dwLadolfFlag = 0;
						}
					}
				}

				mLadolf._41 += vLocal.x;
				mLadolf._42 += vLocal.y;
				mLadolf._43 += vLocal.z;

				m_pLadolf->m_nNoEffect = m_pModel->m_nNoEffect;
				m_pLadolf->Render(pd3dDevice, &mLadolf);
				m_pLadolf->m_nNoEffect = 0;
			}
		}

		if (m_pModel->m_nNoEffect != 2)
			m_buffs.Render();
	}

	if (m_pAngelFlag && m_pAngel)
	{
		m_pAngel->m_nNoEffect = m_pModel->m_nNoEffect;
		m_pAngel->Render(pd3dDevice, &m_AngelWorldM);
		m_pAngel->m_nNoEffect = 0;
	}

#ifdef __EVE_BALLOON
	if (m_pBalloonFlag && m_pBalloon)
	{
		m_pBalloon->m_nNoEffect = m_pModel->m_nNoEffect;
		m_pBalloon->Render(pd3dDevice, &m_BalloonWorldM);
		m_pBalloon->m_nNoEffect = 0;
	}
#endif //__EVE_BALLOON

	if (!IsActiveMover() && g_pPlayer->m_pActMover->IsFly())
	{
		if (GetProp()->bKillable == 1 && GetProp()->dwFlying == 1)
		{
			D3DXVECTOR3	vCamera = g_Neuz.m_camera.m_vLookAt - g_Neuz.m_camera.GetPos();
			D3DXVECTOR3 vObj = GetScrPos() - g_Neuz.m_camera.GetPos();
			D3DXVec3Normalize(&vCamera, &vCamera);
			D3DXVec3Normalize(&vObj, &vObj);
			FLOAT fDot = D3DXVec3Dot(&vCamera, &vObj);
			if (fDot > cosf(D3DXToRadian(15.0f)))
			{
				CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.m_pWndWorld;
				if (pWndWorld)
				{
					pWndWorld->AddFlyTarget(GetId());
				}
			}
		}
	}

	if ((m_dwFlag & MVRF_EFFECT1) == 0)
	{
		CSfx* pSfx = NULL;
		switch (m_dwIndex)
		{
		case MI_RISEM1:
		case MI_RISEM2:
		case MI_RISEM3:
		case MI_RISEM4:
			pSfx = CreateSfx(D3DDEVICE, XI_NPCRISEMSIGN, GetPos(), GetId(), D3DXVECTOR3(0, 0, 0), NULL_ID, -1);
			if (pSfx)
				pSfx->SetScale(GetScale());
		}
		if (pSfx)
			m_dwFlag |= MVRF_EFFECT1;
	}
	if (m_pModel->m_nNoEffect == 0)
		RenderQuestEmoticon(pd3dDevice);
}


void CMover::RenderPartsEffect(LPDIRECT3DDEVICE9 pd3dDevice)
{
#ifdef __CLIENT
	if (IsMode(TRANSPARENT_MODE))		// ????? ???? ??.
		return;

	if (!IsVisible() || IsCull())
		return;

	pd3dDevice->SetMaterial(g_TextureMng.GetMaterial(pd3dDevice, 0));

	D3DXVECTOR3 vPos = GetScrPos();
	D3DXMATRIX  matWorld;

	matWorld = m_matWorld;

	D3DXVECTOR3 vPos2 = GetScrPos() - GetWorld()->m_pCamera->m_vPos;
	int nLength = (int)(D3DXVec3Length(&vPos2));
	if (nLength > CWorld::m_fFarPlane)
		nLength = (int)(CWorld::m_fFarPlane);
	if (nLength > CWorld::m_fFarPlane - (CWorld::m_fFarPlane / 4))
		return;


	m_pModel->SetGroup(0);

	int nElement[2];
	nElement[0] = nElement[1] = 0;
	CItemElem* pItemElem[2];
	DWORD dwItemKind[2];
	dwItemKind[0] = dwItemKind[1] = 0;

	if (IsActiveMover())
	{
		if (GetEquipItem(PARTS_SHIELD))
			pItemElem[0] = GetEquipItem(PARTS_SHIELD);
		else
			pItemElem[0] = GetEquipItem(PARTS_LWEAPON);

		pItemElem[1] = GetEquipItem(PARTS_RWEAPON);

		for (int i = 0; i < 2; i++)
		{
			if (pItemElem[i])
			{
				nElement[i] = pItemElem[i]->GetResistAbilityOption();
#ifndef __MODEL_CHANGE
				dwItemKind[i] = pItemElem[i]->GetProp()->dwItemKind3;
				if (pItemElem[i]->GetProp()->dwHanded == HD_TWO)
#else
				DWORD dwVisibleItemID = pItemElem[i]->m_dwItemId;
				if (pItemElem[i]->GetLook() != NULL_ID)
					dwVisibleItemID = pItemElem[i]->GetLook();

				ItemProp* pProp = prj.GetItemProp(dwVisibleItemID);
				if (pProp)
					dwItemKind[i] = pProp->dwItemKind3;

				if (pProp && pProp->dwHanded == HD_TWO)
#endif
				{
					if (dwItemKind[i] == IK3_SWD)
						dwItemKind[i] = IK3_THSWD;
					else if (dwItemKind[i] == IK3_AXE)
						dwItemKind[i] = IK3_THAXE;
				}
			}
		}
		m_pModel->RenderEffect(pd3dDevice, &matWorld, dwItemKind[0], dwItemKind[1], nElement[0], nElement[1]);
	}
	else
	{
		DWORD dwItemID = NULL_ID;
#ifndef __MODEL_CHANGE
		dwItemID = m_aEquipInfo[PARTS_SHIELD].dwId;
#else
		dwItemID = m_aEquipInfo[PARTS_SHIELD].dwLook != NULL_ID ? m_aEquipInfo[PARTS_SHIELD].dwLook : m_aEquipInfo[PARTS_SHIELD].dwId;
#endif
		if (dwItemID != NULL_ID)
		{
			dwItemKind[0] = prj.GetItemProp(dwItemID)->dwItemKind3;
			nElement[0] = (m_aEquipInfo[PARTS_SHIELD].nOption & 0xFF000000) >> 24;
		}
		else
		{
#ifndef __MODEL_CHANGE
			dwItemID = m_aEquipInfo[PARTS_LWEAPON].dwId;
#else
			dwItemID = m_aEquipInfo[PARTS_LWEAPON].dwLook != NULL_ID ? m_aEquipInfo[PARTS_LWEAPON].dwLook : m_aEquipInfo[PARTS_LWEAPON].dwId;
#endif
			if (dwItemID != NULL_ID)
			{
				dwItemKind[0] = prj.GetItemProp(dwItemID)->dwItemKind3;
				nElement[0] = (m_aEquipInfo[PARTS_LWEAPON].nOption & 0xFF000000) >> 24;
			}
		}

#ifndef __MODEL_CHANGE
		dwItemID = m_aEquipInfo[PARTS_RWEAPON].dwId;
#else
		dwItemID = m_aEquipInfo[PARTS_RWEAPON].dwLook != NULL_ID ? m_aEquipInfo[PARTS_RWEAPON].dwLook : m_aEquipInfo[PARTS_RWEAPON].dwId;
#endif
		if (dwItemID != NULL_ID)
		{
			dwItemKind[1] = prj.GetItemProp(dwItemID)->dwItemKind3;
			if (prj.GetItemProp(dwItemID)->dwHanded == HD_TWO)
			{
				if (dwItemKind[1] == IK3_SWD)
					dwItemKind[1] = IK3_THSWD;
				else
					if (dwItemKind[1] == IK3_AXE)
						dwItemKind[1] = IK3_THAXE;
			}
			nElement[1] = (m_aEquipInfo[PARTS_RWEAPON].nOption & 0xFF000000) >> 24;
		}
		m_pModel->RenderEffect(pd3dDevice, &matWorld, dwItemKind[0], dwItemKind[1], nElement[0], nElement[1]);
	}

	extern CPartsLaser g_Laser;
	D3DXMatrixIdentity(&matWorld);
	g_Laser.Render(pd3dDevice, &matWorld, g_ModelGlobal.m_vCameraPos, g_ModelGlobal.m_vCameraForward);

	if (m_dwIndex == MI_DU_METEONYKER || m_dwIndex == MI_DU_METEONYKER2 || m_dwIndex == MI_DU_METEONYKER3 ||
		m_dwIndex == MI_DU_METEONYKER4)
	{
		extern CPartsFireDragon	g_FireDragon;
		g_FireDragon.Render(pd3dDevice, &matWorld);
	}
#endif
}


#ifdef __CLIENT

// nValue ( 1,2,3,4 )
void CMover::RenderGauge(LPDIRECT3DDEVICE9 pd3dDevice, int nValue)
{

	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

	pd3dDevice->GetViewport(&vp);
	vp.X = 0;
	vp.Y = 0;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

	vOut.y -= 40;

	int nGaugeWidth = 80;
	CPoint point((LONG)(vOut.x - nGaugeWidth / 2), (LONG)(vOut.y - 3));
	CRect rect(point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);

	CPoint p = CPoint(rect.left, rect.top);
	LONG   y = 200;

	MakeEven(p.x);

	g_Neuz.m_TextureGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);

	switch (nValue)
	{
	case 4:
		g_Neuz.m_TextureGauge[4].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
	case 3:
		g_Neuz.m_TextureGauge[3].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
	case 2:
		g_Neuz.m_TextureGauge[2].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
	case 1:
		g_Neuz.m_TextureGauge[1].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
		break;
	case 0:
		break;
	default:
		ASSERT(0);
		break;
	}
}

void CMover::RenderTurboGauge(LPDIRECT3DDEVICE9 pd3dDevice, DWORD nColor, int nValue, int nMaxValue)
{
	if (IsMode(TRANSPARENT_MODE))
		return;
	if (!IsVisible() || IsCull())
		return;

	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

	pd3dDevice->GetViewport(&vp);
	vp.X = 0;
	vp.Y = 0;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);


	CPoint point((LONG)(vOut.x - 80 / 2), (LONG)(vOut.y - 40 - 3));
	CPoint p = CPoint(point.x - 2, point.y - 2);

	MakeEven(p.x);

	g_Neuz.m_TextureTurboGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);


	int nWidth = g_Neuz.m_TextureTurboGauge[1].m_size.cx * nValue / nMaxValue;

	CPoint p2 = CPoint(nWidth, g_Neuz.m_TextureTurboGauge[1].m_size.cy);

	if (m_pActMover->GetStateFlag() & OBJSTAF_TURBO)
	{
		if (g_nRenderCnt & 1)
		{
			g_Neuz.m_TextureTurboGauge[1].Render(&g_Neuz.m_2DRender, p, p2, 200, 0.8f, 0.55f);
		}
		else
		{
			int nWidthScale = (int)((96 * nValue / nMaxValue) * 0.8f);
			CRect rect(p.x + 1, p.y + 1, p.x + nWidthScale, p.y + 6);
			g_Neuz.m_2DRender.RenderFillRect(rect, 0xffffffff);
		}

	}
	else
		g_Neuz.m_TextureTurboGauge[1].Render(&g_Neuz.m_2DRender, p, p2, 200, 0.8f, 0.55f);


	extern int g_nFlySpeed;
	char szBuff[64];
	int		nLen, i, nIdx;
	sprintf(szBuff, "%d", g_nFlySpeed);
	nLen = strlen(szBuff);

	vPosHeight.x = 0;
	vPosHeight.z = 0;
	vPosHeight.y = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);



	p.x = (LONG)(vOut.x + 30);
	p.y = (LONG)(vOut.y + 0);

	MakeEven(p.x);

	for (i = 0; i < nLen; i++)
	{
		nIdx = szBuff[i] - '0';
		g_WndMng.m_pWndWorld->m_texFontDigital.GetAt(nIdx)->Render(&g_Neuz.m_2DRender, p);
		p.x += 16;

	}

	g_WndMng.m_pWndWorld->m_texFontDigital.GetAt(10)->Render(&g_Neuz.m_2DRender, p);


}
#endif // __CLIENT

#ifdef __CLIENT
void CMover::RenderQuestEmoticon(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (IsPlayer() || m_bShowQuestEmoticon == FALSE)
		return;
	LPCHARACTER lpCharacter = GetCharacter();

	if (lpCharacter == NULL) return;


	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

	pd3dDevice->GetViewport(&vp);

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	//vOutFLOAT fHeight = ( vOut[0].y - vOut[4].y ) - 10f;
	CPoint point;
	point.x = (LONG)(vOut.x);
	point.y = (LONG)(vOut.y);

	MakeEven(point.x);
	//point.x -= pFont->GetTextExtent( szName ).cx / 2;
	point.y -= 50;

	if (m_nQuestEmoticonIndex > 0)
	{

		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		point.x = (LONG)(point.x - (16 * m_fQuestEmoticonScale));
		point.y = (LONG)(point.y - (16 * m_fQuestEmoticonScale));
		g_Neuz.m_texQuestEmoticon.GetAt(m_nQuestEmoticonIndex)->RenderScal(&(g_Neuz.m_2DRender), point, 255, m_fQuestEmoticonScale, m_fQuestEmoticonScale);

		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
}
#endif // __CLIENT

void CMover::RenderChrState(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (!IsVisible() || IsCull())
		return;

	if (!IsPlayer())
		return;

	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB;

	if (m_pModel)
		pBB = m_pModel->GetBBVector();
	else
		return;

	pd3dDevice->GetViewport(&vp);

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);

	D3DXMatrixMultiply(&matWorld, &matWorld, &GetMatrixScale());
	D3DXMatrixMultiply(&matWorld, &matWorld, &GetMatrixRotation());
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

	CPoint point;
	point.x = (LONG)(vOut.x - 32 / 2);
	point.y = (LONG)(vOut.y - 32);

	MakeEven(point.x);

	CTexture* pTexture = NULL;
	if (GetAdjParam(DST_CHRSTATE) & CHS_STUN)
	{
		pTexture = g_DialogMsg.m_texEmoticon.GetAt(DLGEMOT_STUN);
	}

	if (pTexture)
	{
		g_Neuz.m_2DRender.RenderTexture(point, pTexture, 220);
	}
}
#if __VER >= 13 // __HONORABLE_TITLE
LPCTSTR	CMover::GetTitle()
{
	return m_strTitle.GetBuffer(0);
}

void	CMover::SetTitle(LPCTSTR pTitle)
{
	m_strTitle = pTitle;
}
#endif	// __HONORABLE_TITLE

void CMover::RenderName(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont)
{
	if (!IsVisible() || IsCull())
		return;

	if (g_Option.m_nPlayerName == FALSE && IsActiveMover())
		return;

	if (g_Option.m_nOtherPlayerName == FALSE && !IsActiveMover() && IsPlayer())
		return;

#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
	if (GetNpcType() != NPC_TYPE_NORMAL)
	{
		RenderPlayerCopy(pd3dDevice, pFont);

		if (GetNpcType() == NPC_TYPE_MVPSTATUE || GetNpcType() == NPC_TYPE_GUILDLEADERSTATUE)
			RenderStatueEmoticon(pd3dDevice);

		return;
	}
#endif

	char szName[256];
	strcpy(szName, m_szName);

	BOOL  bOtherColor = FALSE;
	DWORD dwNewColor;
	DWORD dwColor = COLOR_PLAYER;


	if (IsPlayer())
	{
		if (GetWorld()->GetID() == WI_WORLD_GUILDWAR && g_pPlayer->GetGuild() && GetGuild())
		{
			bOtherColor = TRUE;
			if (g_pPlayer->GetGuild()->GetGuildId() == GetGuild()->GetGuildId())
				dwNewColor = COLOR_GUILDCOMBAT;
			else
				dwNewColor = COLOR_PLAYER;
		}

#ifdef __FL_TDM
		if (GetWorld()->IsTDM())
		{
			if (g_WndMng.m_pWndWorld)
			{
				bOtherColor = TRUE;

				TDM_TEAMS team = g_WndMng.m_pWndWorld->GetTeamByPlayerId(m_idPlayer);
				if (team == TDM_TEAM_RED)
					dwNewColor = 0xffff0000;
				else if (team == TDM_TEAM_BLUE)
					dwNewColor = 0xff6980f0;
				else
					dwNewColor = COLOR_PLAYER;
			}
		}
#endif

		switch (m_dwAuthorization)
		{
		case AUTH_GAMEMASTER: dwColor = COLOR_GAMEMASTER; break;
		case AUTH_GAMEMASTER2: dwColor = COLOR_GAMEMASTER; break;
		case AUTH_ADMINISTRATOR: dwColor = COLOR_ADMINISTRATOR; break;
		default: dwColor = COLOR_PLAYER; break;
		}

		CString strFameName = GetTitle();
		if (strFameName.IsEmpty() == FALSE)
		{
			CString strName;
			strName = "[";
			strName += strFameName;
			strName += "] ";
			strName += m_szName;
			strcpy(szName, (LPCTSTR)strName);
		}

		CString strName;
		strName = szName;

		switch (m_dwAuthorization)
		{
		case AUTH_GAMEMASTER: strName += " [Moderator]"; break;
		case AUTH_GAMEMASTER2: strName += " [GM]"; break;
		case AUTH_ADMINISTRATOR: strName += " [Admin]"; break;
		default: break;
		}

		strcpy(szName, (LPCTSTR)strName);
	}

	D3DXVECTOR3 vOut, vPos, vPosHeight;

	MoverProp* pMoverProp = GetProp();
	vPos = GetScrPos();

	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

	pd3dDevice->GetViewport(&vp);

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

	CPoint point;
	point.x = (LONG)(vOut.x);
	point.y = (LONG)(vOut.y);

	MakeEven(point.x);
	point.x -= pFont->GetTextExtent(szName).cx / 2;
	point.y -= 20;
	point.x -= 3;

	if (IsPlayer())
	{
		if (dwColor == COLOR_PLAYER)
			if (g_Party.FindMember(m_idPlayer) != -1)
				dwColor = COLOR_PARTY;

		if (bOtherColor)
			dwColor = dwNewColor;

		pFont->DrawText((FLOAT)(point.x), (FLOAT)(point.y), dwColor, szName);
	}
	else
	{
		if (g_Option.m_nMonName)
		{
			LPCHARACTER lpCharacter = GetCharacter();
			if (g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->m_bCtrlPushed)
			{
				if (!IsPeaceful())
				{
					CString strName;
					CString szLvl;
					
					if (g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR))
						szLvl.Format(" [Lv. %d | ID %d]", GetLevel(), pMoverProp->dwID);
					else
						szLvl.Format(" [Lv. %d]", GetLevel());
					strName = m_szName;
					strName += szLvl;
					strcpy(szName, (LPCTSTR)strName);

					point.x -= pFont->GetTextExtent(szLvl).cx / 2;
				}
			}
			if (lpCharacter && lpCharacter->m_nStructure != -1)
			{
				LPSTRUCTURE lpStr = &prj.m_aStructure[lpCharacter->m_nStructure];
				TCHAR szStr[64];
				sprintf(szStr, "[%s]", lpStr->szName);
				point.x = (LONG)(vOut.x);
				point.y = (LONG)(vOut.y);
				MakeEven(point.x);
				point.x -= pFont->GetTextExtent(szStr).cx / 2;
				point.y -= 35;
				pFont->DrawText((FLOAT)(point.x), (FLOAT)(point.y), COLOR_NPC, szStr);
				point.x = (LONG)(vOut.x);
				point.y = (LONG)(vOut.y);
				MakeEven(point.x);
				point.x -= pFont->GetTextExtent(szName).cx / 2;
				point.y -= 20;
			}

			if (IsPeaceful())
			{
				pFont->DrawText((FLOAT)(point.x), (FLOAT)(point.y), COLOR_NPC, szName);
			}
			else
			{
				if (m_bActiveAttack)
				{
					pFont->DrawText((FLOAT)(point.x), (FLOAT)(point.y), 0xffff0000, szName);
				}
				else
				{
					pFont->DrawText((FLOAT)(point.x), (FLOAT)(point.y), COLOR_MONSTER, szName);
				}
			}
		}
	}

	if (GetWorld()->m_bViewIdState)
	{
		char szBuff[256];
		CString strName;
		CMover* pAttacker = prj.GetMover(m_idAttacker);
		if (pAttacker)
			strName = pAttacker->m_szName;
		sprintf(szBuff, "0x%x %08x, %d %d, %d(%s)", m_pActMover->GetStateFlag(), m_pActMover->GetState(), GetId(), m_pActMover->m_bGround, (int)m_idAttacker, strName);
		pFont->DrawText(vOut.x, vOut.y - 50, dwColor, szBuff);

	}


	// Render Guild Name & Logo
	CGuild* pGuild = GetGuild();

	point.x = (LONG)(vOut.x);
	point.y = (LONG)(vOut.y);
	MakeEven(point.x);

	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	if (IsPlayer())
	{
		BYTE checkhero = GetLegendChar();
		CTexture* pTexture;
		CString strPath;
		CSize cs1 = pFont->GetTextExtent(szName);
		CSize cs2;

		if (pGuild != NULL)
		{
			cs2 = pFont->GetTextExtent(pGuild->m_szGuild);

			if (cs1.cx > cs2.cx)
				point.x += cs1.cx / 2;
			else
				point.x += cs2.cx / 2;
		}
		else
			point.x += cs1.cx / 2;

		if (checkhero == LEGEND_CLASS_MASTER && m_nLevel >= 60 && m_nLevel <= 120)
		{
			if (m_nLevel < 70)
				pTexture = g_Neuz.m_pMasterIcon[0];
			else if (m_nLevel >= 70 && m_nLevel < 80)
				//	strPath = MakePath( DIR_ICON, "icon_Expert2.dds");
				pTexture = g_Neuz.m_pMasterIcon[1];
			else if (m_nLevel >= 80 && m_nLevel < 90)
				//	strPath = MakePath( DIR_ICON, "icon_Expert3.dds");
				pTexture = g_Neuz.m_pMasterIcon[2];
			else if (m_nLevel >= 90 && m_nLevel < 100)
				//	strPath = MakePath( DIR_ICON, "icon_Expert4.dds");
				pTexture = g_Neuz.m_pMasterIcon[3];
			else if (m_nLevel >= 100 && m_nLevel < 110)
				//	strPath = MakePath( DIR_ICON, "icon_Expert5.dds");
				pTexture = g_Neuz.m_pMasterIcon[4];
			else if (m_nLevel >= 110 && m_nLevel <= 120)
				//	strPath = MakePath( DIR_ICON, "icon_Expert6.dds");
				pTexture = g_Neuz.m_pMasterIcon[5];

			//pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
			if (pTexture != NULL)
			{
				point.y -= pTexture->m_size.cy + 5;
				pTexture->Render(&g_Neuz.m_2DRender, point);
			}
		}
		else if (checkhero == LEGEND_CLASS_HERO || checkhero == LEGEND_CLASS_LEGENDHERO)
		{
			pTexture = g_Neuz.m_pHeroIcon;
			if (pTexture != NULL)
			{
				point.y -= pTexture->m_size.cy + 5;
				pTexture->Render(&g_Neuz.m_2DRender, point);
			}
		}
	}

	if (pGuild)
	{
		point.x = (LONG)(vOut.x);
		point.y = (LONG)(vOut.y);
		MakeEven(point.x);
		point.x -= pFont->GetTextExtent(pGuild->m_szGuild).cx / 2;
		point.y -= 32;
		point.x -= 3;

		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

		dwColor = 0xFFFFFFFF;
		if (pGuild->IsMaster(m_idPlayer))
			dwColor = 0xFFFFFF99;
		else if (bOtherColor)
			dwColor = dwNewColor;

		if (g_GuildCombatMng.m_nGCState >= CGuildCombat::MAINTENANCE_STATE)
		{
			if (pWndWorld && pWndWorld->GetGCStatusDefender(m_idPlayer) != -1)
			{
				dwColor = 0xFFFEA8FF;
			}
		}


		BOOL bRenderCombatBestLogo = FALSE;
		BOOL bRenderCombatWinLogo = FALSE;


		if (g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE)
		{
			if (g_GuildCombatMng.m_uBestPlayer == m_idPlayer)
				bRenderCombatBestLogo = TRUE;
			else
				if (g_GuildCombatMng.m_uWinGuildId == pGuild->GetGuildId())
					bRenderCombatWinLogo = TRUE;
		}

		pFont->DrawText((FLOAT)(point.x), (FLOAT)(point.y), dwColor, pGuild->m_szGuild);

		if (bRenderCombatBestLogo || bRenderCombatWinLogo)
		{
			D3DDEVICE->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			int nLogoSizey = 30;

			point.x = (LONG)(vOut.x);
			point.y = (LONG)(vOut.y);

			MakeEven(point.x);

			if (pFont->GetTextExtent(szName).cx > pFont->GetTextExtent(pGuild->m_szGuild).cx)
				point.x -= pFont->GetTextExtent(szName).cx / 2;
			else
				point.x -= pFont->GetTextExtent(pGuild->m_szGuild).cx / 2;

			point.y -= nLogoSizey;
			point.x -= nLogoSizey;

			if (bRenderCombatBestLogo)
				pWndWorld->m_TexGuildBest.Render(&g_Neuz.m_2DRender, point, 255);
			else if (bRenderCombatWinLogo)
				pWndWorld->m_TexGuildWinner.Render(&g_Neuz.m_2DRender, point, 255);
		}
	}
}


void CMover::RenderHP(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (!IsVisible() || IsCull())
		return;

	if (IsMode(TRANSPARENT_MODE))
		return;

	if (IsMode(GCWAR_RENDER_SKIP_MODE))
		return;

	if (g_Option.m_nPlayerName == FALSE && IsActiveMover())
		return;

	if (g_Option.m_nOtherPlayerName == FALSE && !IsActiveMover() && IsPlayer())
		return;

	if (g_WorldMng()->m_bViewHP == FALSE)
		return;


	if (IsAuthHigher(AUTH_GAMEMASTER) == TRUE)
	{
		if (IsPlayer() && HasBuffByIk3(IK3_TEXT_DISGUISE))
			return;
	}

	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	D3DXVECTOR3	vOut(0.0f, 0.0f, 0.0f),
		vPos(0.0f, 0.0f, 0.0f),
		vPosHeight(0.0f, 0.0f, 0.0f);

	D3DVIEWPORT9	vp;
	ZeroMemory(&vp, sizeof(D3DVIEWPORT9));
	pd3dDevice->GetViewport(&vp);
	vp.X = 0;
	vp.Y = 0;

	vPos = GetScrPos();

	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	if (pBB == NULL)
		return;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);

	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj, &GetWorld()->m_pCamera->m_matView, &matWorld);

	vOut.y -= 34;
	int nGaugeWidth = 80;
	CPoint point((int)(vOut.x - nGaugeWidth / 2), (int)(vOut.y - 3));
	CRect rect(point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);

	CPoint p = CPoint(rect.left, rect.top);
	MakeEven(p.x);

	BOOL bDbuff = FALSE;

	if (IsStateDbuff())
		bDbuff = TRUE;

	g_Neuz.m_TextureHPGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);

	int nWidthClient = g_Neuz.m_TextureHPGauge[1].m_size.cx;
	int nWidth = (int)((__int64)nWidthClient * (__int64)GetHitPoint() / (__int64)GetMaxHitPoint());

	if (nWidth > nWidthClient)
		nWidth = nWidthClient;

	if (nWidthOld > nWidthClient || nWidthOld < 0)
		nWidthOld = nWidthOld > nWidthClient ? nWidthClient : 0;

	if (nWidthOld < nWidth)
	{
		int nWidthMinusOld = nWidth - nWidthOld;
		nWidthMinusOld = nWidthMinusOld / 5;
		if (nWidthMinusOld <= 1)
			nWidthMinusOld = 1;

		nWidth = nWidthOld + (nWidth - nWidthOld > nWidthMinusOld ? nWidthMinusOld : nWidth - nWidthOld);
		nWidthOld = nWidth;
	}
	else if (nWidthOld > nWidth)
	{
		int nWidthMinusOld = nWidthOld - nWidth;
		nWidthMinusOld = nWidthMinusOld / 5;
		if (nWidthMinusOld <= 1)
			nWidthMinusOld = 1;

		nWidth = nWidthOld - (nWidthOld - nWidth > nWidthMinusOld ? nWidthMinusOld : nWidthOld - nWidth);
		nWidthOld = nWidth;
	}

	CPoint p2 = CPoint(nWidth, g_Neuz.m_TextureHPGauge[1].m_size.cy);

	if (bDbuff)
		g_Neuz.m_TextureHPGauge[2].Render(&(g_Neuz.m_2DRender), p, p2, 200, 0.8f, 0.55f);
	else
	{
		g_Neuz.m_TextureHPGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 200, 0.8f, 0.55f);
	}

}


#if __VER >= 11 // __SYS_COLLECTING
void CMover::RenderCltGauge(LPDIRECT3DDEVICE9 pd3dDevice)
{

	if (!IsVisible() || IsCull())
		return;

	if (!IsCollecting()) return;

	if (g_Option.m_nOtherPlayerName == FALSE && !IsActiveMover() && IsPlayer())
		return;


	D3DXVECTOR3 vOut, vPos, vPosHeight;
	D3DVIEWPORT9 vp;

	MoverProp* pMoverProp = GetProp();
	vPos = GetScrPos();

	const BOUND_BOX* pBB = m_pModel->GetBBVector();

	pd3dDevice->GetViewport(&vp);
	vp.X = 0;
	vp.Y = 0;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

	vOut.y -= 48;
	int nGaugeWidth = 80;
	CPoint point((int)(vOut.x - nGaugeWidth / 2), (int)(vOut.y - 3));
	CRect rect(point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);

	CPoint p = CPoint(rect.left, rect.top);
	MakeEven(p.x);

	g_Neuz.m_TexCltGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 255, 0.8f, 0.55f);

	int nWidth = g_Neuz.m_TexCltGauge[1].m_size.cx * m_nCltTime / m_nMaxCltTime;

	CPoint p2 = CPoint(nWidth, g_Neuz.m_TexCltGauge[1].m_size.cy);

	g_Neuz.m_TexCltGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 255, 0.8f, 0.55f);
}
#endif

void CMover::RenderCasting(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (g_pPlayer->IsStateMode(STATE_BASEMOTION_MODE) == FALSE)
		return;
	if (g_pPlayer->m_nReadyTime == 0)
		return;
	if (!IsVisible() || IsCull())
		return;

	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

	pd3dDevice->GetViewport(&vp);
	vp.X = 0;
	vp.Y = 0;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

	vOut.y -= 48;
	int nGaugeWidth = 80;

	CPoint point((int)(vOut.x - nGaugeWidth / 2), (int)(vOut.y - 3));
	CRect rect(point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);

	CPoint p = CPoint(rect.left, rect.top);

	MakeEven(p.x);

	g_Neuz.m_TextureCastingGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);

	ItemProp* pItemProp = prj.GetItemProp(g_pPlayer->m_dwUseItemId);
	if (pItemProp)
	{
		DWORD dwTime = timeGetTime() - g_pPlayer->m_nReadyTime;
		int nWidth = (int)(g_Neuz.m_TextureHPGauge[1].m_size.cx * ((float)dwTime / (float)pItemProp->dwSkillReadyType));
		if (112 < nWidth)
			nWidth = 112;
		if (nWidth < 0)
			nWidth = 0;

		CPoint p2 = CPoint(nWidth, g_Neuz.m_TextureHPGauge[1].m_size.cy);

		g_Neuz.m_TextureCastingGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 255, 0.8f, 0.55f);
	}
}



#if __VER >= 10
void CMover::RenderSkillCasting(LPDIRECT3DDEVICE9 pd3dDevice)
{
	DWORD dwtmpTick = GetTickCount();
	DWORD dwTmpCastingEndTick = m_pActMover->GetCastingEndTick();
	if (dwTmpCastingEndTick < dwtmpTick)
		return;

	int dwTmpSkillID = m_pActMover->GetCastingSKillID();
	if (dwTmpSkillID != SI_KNT_HERO_DRAWING && dwTmpSkillID != SI_RIG_HERO_RETURN)
		return;

	if (!IsVisible() || IsCull())
		return;

	DWORD dwTmpCastingTick = (DWORD)m_pActMover->GetCastingTick();


	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	D3DXVECTOR3 vOut, vPos = GetScrPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

	pd3dDevice->GetViewport(&vp);
	vp.X = 0;
	vp.Y = 0;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

	vOut.y -= 48;
	int nGaugeWidth = 80;

	CPoint point((int)(vOut.x - nGaugeWidth / 2), (int)(vOut.y - 3));
	CRect rect(point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);

	CPoint p = CPoint(rect.left, rect.top);

	MakeEven(p.x);

	g_Neuz.m_TextureCastingGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);

	{
		DWORD dwTime = dwTmpCastingTick - (dwTmpCastingEndTick - dwtmpTick);
		int nWidth = (int)(g_Neuz.m_TextureHPGauge[1].m_size.cx * ((float)dwTime / dwTmpCastingTick));
		if (112 < nWidth)
			nWidth = 112;
		if (nWidth < 0)
			nWidth = 0;

		CPoint p2 = CPoint(nWidth, g_Neuz.m_TextureHPGauge[1].m_size.cy);

		g_Neuz.m_TextureCastingGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 255, 0.8f, 0.55f);
	}
}
#endif

#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
void CMover::RenderPlayerCopy(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont)
{
	D3DXVECTOR3 vOut, vPos, vPosHeight;
	vPos = GetScrPos();

	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

	pd3dDevice->GetViewport(&vp);

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

	// Name
	DWORD dwColor = 0xffffffff;
	CString str = GetName();
	CPoint point;

	switch (GetNpcType())
	{
#ifdef __FL_GW_STATUE_V2
	case NPC_TYPE_MVPSTATUE:
		str.Format("[MvP Statue] %s", str);
		break;
	case NPC_TYPE_GUILDLEADERSTATUE:
		str.Format("[Guild Leader Statue] %s", str);
		break;
#endif
#ifdef __FL_VENDORS
	case NPC_TYPE_VENDOR_HUMAN:
	case NPC_TYPE_VENDOR_NPC:
		str.Format("[Vendor] %s", str);
		break;
#endif
	default: break;
	}

	point.x = (LONG)(vOut.x);
	point.y = (LONG)(vOut.y);
	MakeEven(point.x);
	point.x -= pFont->GetTextExtent(str).cx / 2;
	point.y -= 19;
	pFont->DrawText((FLOAT)(point.x), (FLOAT)(point.y), dwColor, str);
	MakeEven(point.x);

	BYTE nType = GetNpcType();
	switch (nType)
	{
	case NPC_TYPE_MVPSTATUE:
	case NPC_TYPE_GUILDLEADERSTATUE:
	case NPC_TYPE_VENDOR_HUMAN:
		break;
	default: return;
	}

	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// Level icon
	BYTE checkhero = GetLegendChar();
	CTexture* pTexture = NULL;
	CSize cs1 = pFont->GetTextExtent(str);
	CSize cs2;
	CGuild* pGuild = GetGuild();
	point.x = (LONG)(vOut.x);
	point.y = (LONG)(vOut.y);
	point.y += 4;
	point.x += 4;

	if (pGuild != NULL)
	{
		cs2 = pFont->GetTextExtent(pGuild->m_szGuild);

		if (cs1.cx > cs2.cx)
			point.x += cs1.cx / 2;
		else
			point.x += cs2.cx / 2;
	}
	else
		point.x += cs1.cx / 2;

	if (checkhero == LEGEND_CLASS_MASTER && m_nLevel >= 60 && m_nLevel <= 120)
	{
		if (m_nLevel < 70)
			pTexture = g_Neuz.m_pMasterIcon[0];
		else if (m_nLevel >= 70 && m_nLevel < 80)
			pTexture = g_Neuz.m_pMasterIcon[1];
		else if (m_nLevel >= 80 && m_nLevel < 90)
			pTexture = g_Neuz.m_pMasterIcon[2];
		else if (m_nLevel >= 90 && m_nLevel < 100)
			pTexture = g_Neuz.m_pMasterIcon[3];
		else if (m_nLevel >= 100 && m_nLevel < 110)
			pTexture = g_Neuz.m_pMasterIcon[4];
		else if (m_nLevel >= 110 && m_nLevel <= 120)
			pTexture = g_Neuz.m_pMasterIcon[5];

		if (pTexture != NULL)
		{
			point.y -= pTexture->m_size.cy + 5;
			pTexture->Render(&g_Neuz.m_2DRender, point);
		}
	}
	else if (checkhero == LEGEND_CLASS_HERO || checkhero == LEGEND_CLASS_LEGENDHERO)
	{
		pTexture = g_Neuz.m_pHeroIcon;
		if (pTexture != NULL)
		{
			point.y -= pTexture->m_size.cy + 5;
			pTexture->Render(&g_Neuz.m_2DRender, point);
		}
	}

	// Guild name and icons
	if (pGuild)
	{
		// Guild name
		point.x = (LONG)(vOut.x);
		point.y = (LONG)(vOut.y);
		MakeEven(point.x);
		point.x -= pFont->GetTextExtent(pGuild->m_szGuild).cx / 2;
		point.y -= 32;

		if (GetNpcType() == NPC_TYPE_GUILDLEADERSTATUE)
			dwColor = 0xFFFFFF99;
		else
		{
			if (pGuild->IsMaster(GetNpcOwner()))
				dwColor = 0xFFFFFF99;
		}

		pFont->DrawText((FLOAT)(point.x), (FLOAT)(point.y), dwColor, pGuild->m_szGuild);

		// Guild related icons
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

		BOOL bRenderCombatBestLogo = FALSE;
		BOOL bRenderCombatWinLogo = FALSE;

		switch (GetNpcType())
		{
		case NPC_TYPE_MVPSTATUE:
		{
			bRenderCombatBestLogo = TRUE;
			bRenderCombatWinLogo = FALSE;
		}
		break;
		case NPC_TYPE_GUILDLEADERSTATUE:
		{
			bRenderCombatBestLogo = FALSE;
			bRenderCombatWinLogo = TRUE;
		}
		break;
		default:
		{
			if (g_GuildCombatMng.m_uBestPlayer == GetNpcOwner())
				bRenderCombatBestLogo = TRUE;
			else if (g_GuildCombatMng.m_uWinGuildId == pGuild->GetGuildId())
				bRenderCombatWinLogo = TRUE;
		}
		break;
		}


		if (bRenderCombatBestLogo || bRenderCombatWinLogo)
		{
			D3DDEVICE->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			int nLogoSizey = 30;

			point.x = (LONG)(vOut.x);
			point.y = (LONG)(vOut.y);

			MakeEven(point.x);

			if (pFont->GetTextExtent(str).cx > pFont->GetTextExtent(pGuild->m_szGuild).cx)
				point.x -= pFont->GetTextExtent(str).cx / 2;
			else
				point.x -= pFont->GetTextExtent(pGuild->m_szGuild).cx / 2;

			point.y -= nLogoSizey;
			point.x -= nLogoSizey;

			if (bRenderCombatBestLogo)
				pWndWorld->m_TexGuildBest.Render(&g_Neuz.m_2DRender, point, 255);
			else if (bRenderCombatWinLogo)
				pWndWorld->m_TexGuildWinner.Render(&g_Neuz.m_2DRender, point, 255);
		}
	}
}
#endif

#ifdef __FL_GW_STATUE_V2
void CMover::RenderStatueEmoticon(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (!g_pPlayer)
		return;

	D3DXVECTOR3 vOut, vPos = GetPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

	pd3dDevice->GetViewport(&vp);

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

	CPoint point;
	point.x = (LONG)(vOut.x);
	point.y = (LONG)(vOut.y);

	MakeEven(point.x);
	point.y -= 50;

	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	point.x = (LONG)(point.x - (16 * m_fQuestEmoticonScale));
	point.y = (LONG)(point.y - (16 * m_fQuestEmoticonScale));
	g_Neuz.m_IconStatueQuestion.RenderScal(&(g_Neuz.m_2DRender), point, 255, m_fQuestEmoticonScale, m_fQuestEmoticonScale);
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}
#endif

#endif // not worldserver