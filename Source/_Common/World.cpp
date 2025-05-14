#include "stdafx.h"
#include "region.h"
#include "defineWorld.h"
#include "commonctrl.h"

#ifdef __WORLDSERVER
#ifdef __AZRIA_1023
#include "ticket.h"
#endif	// __AZRIA_1023

#include "..\_aiinterface\aipet.h"

#endif	// __WORLDSERVER

#if __VER >= 9	// __PET_0410
#ifdef __CLIENT
#include "resdata.h"
#endif	// __CLIENT
#endif	// __PET_0410

#include "..\_AIInterface\AIInterface.h"

#ifdef __WORLDSERVER
#include "user.h"
#include "dpsrvr.h"
#include "dpcoreclient.h"
#include "worldmng.h"
#include "guild.h"
#include "eveschool.h"

extern	CDPSrvr			g_DPSrvr;
extern	CDPCoreClient	g_DPCoreClient;
extern	CUserMng		g_UserMng;
extern	CWorldMng		g_WorldMng;
extern	CGuildMng		g_GuildMng;
extern	CGuildCombat	g_GuildCombatMng;
#endif	// __WORLDSERVER

#ifdef __CLIENT
#include "dialogmsg.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#include "WndHousing.h"
#endif

#endif

#ifdef __WORLDSERVER
#include "npchecker.h"
#ifdef __FL_MONSTER_ALBUM
#include "MonsterAlbum.h"
#endif
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
/*
1	= 0  1 ~ 1.9
2	= 1  2 ~ 3
4	= 2  4 ~ 7
8	= 3  8 ~ 15
16	= 4
32	= 5
64	= 6
128	= 7
*/
int	CLandscape::m_nWidthLinkMap[MAX_LINKLEVEL];
#endif	// __WORLDSERVER



#ifndef __WORLDSERVER
CObj* CWorld::m_aobjCull[MAX_DISPLAYOBJ];
CObj* CWorld::m_asfxCull[MAX_DISPLAYSFX];
int CWorld::m_nObjCullSize = 0;
int CWorld::m_nSfxCullSize = 0;
#endif

CWorld::CWorld()
#ifdef __WORLDSERVER
	:
m_cbRunnableObject(0)
#endif	// __WORLDSERVER
{
	m_nLandWidth = 0;
	m_nLandHeight = 0;
	WORLD_WIDTH = 0;
	WORLD_HEIGHT = 0;
	m_dwWorldID = NULL_ID;
	m_dwIdWorldRevival = WI_WORLD_NONE;
	ZeroMemory(m_szKeyRevival, sizeof(m_szKeyRevival));
	m_nDeleteObjs = 0;
	m_szFileName[0] = '\0';
	m_fMaxHeight = 200.0f;
	m_fMinHeight = 85.0f;
	m_bFly = TRUE;
	m_bIsIndoor = FALSE;
	m_dwDiffuse = 0;
	m_v3LightDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_dwIdMusic = 0;
	m_nPKMode = 0;
	m_szWorldName[0] = '\0';

#ifdef __WORLDSERVER
	m_apHeightMap = NULL;
	m_apWaterHeight = NULL;
	m_dwObjNum = 0;
	m_cbAddObjs = 0;
	memset(m_lpszWorld, 0, sizeof(TCHAR) * 64);
	m_cbUser = 0;
	nextptr = NULL;
	m_cbModifyLink = 0;
	m_cbReplaceObj = 0;
	m_bLoadScriptFlag = FALSE;
#else	// __WORLDSERVER
	m_fElapsedTime = 0;
	m_nVisibilityLand = (int)(m_fFarPlane / (MAP_SIZE * MPU));
	m_fFogStartValue = 70;
	m_fFogEndValue = 400;
	m_fFogDensity = 0.0f;
	m_pd3dDevice = NULL;
	m_pBoundBoxVertexBuffer = NULL;
	m_nCharLandPosX = -1;
	m_nCharLandPosZ = -1;

	m_bViewGrid = FALSE;
	m_bViewGridPatch = FALSE;
	m_bViewGridLandscape = FALSE;
	m_bViewWireframe = FALSE;
	m_bViewSkybox = TRUE;
	m_bViewTerrain = TRUE;
	m_bViewAllObjects = TRUE;
	m_bViewFog = TRUE;
	m_bViewWater = TRUE;
	m_bViewName = TRUE;
	m_bViewHP = TRUE;
	m_bViewLight = TRUE;
	m_bViewWeather = TRUE;
	m_bViewBoundBox = FALSE;
	m_bCullObj = TRUE;
	m_bProcess = TRUE;
	m_bViewObj = TRUE;
	m_bViewMover = TRUE;
	m_bViewItem = TRUE;
	m_bViewRegion = TRUE;
	m_bViewSpawn = TRUE;
	m_bViewHeightAttribute = FALSE;
	m_bViewLODObj = TRUE;

	m_nObjCullSize = 0;
	m_nSfxCullSize = 0;
	m_bViewIdState = FALSE;
	m_dwAmbient = D3DCOLOR_ARGB(255, 128, 128, 128);
	m_pObjFocus = NULL;
	m_idObjFocusOld = NULL_ID;
	m_dwLightIndex = 0;
	m_pCamera = NULL;
	m_apLand = NULL;

#if __VER >= 15 // __BS_CHANGING_ENVIR
	m_bProcessingEnvir = FALSE;
	m_dwOldTime = 0;
	m_dwAddedTime = 0;
	m_bUsing24Light = FALSE;
#endif
#endif // __WORLDSERVER
}

CWorld::~CWorld()
{
	Free();
}

#ifdef __WORLDSERVER
void	CWorld::removeObjectAtProcessArray(CObj* pObj)
{
	if (pObj == NULL)
		return;

	for (int nth = 0; nth < m_cbAddObjs; ++nth)
	{
		if (m_apAddObjs[nth] == pObj)
		{
			m_apAddObjs[nth] = NULL;
			break;
		}
	}

	for (int nth = 0; nth < m_cbReplaceObj; ++nth)
	{
		if (m_aReplaceObj[nth].pObj == pObj)
		{
			m_aReplaceObj[nth].pObj = NULL;
			break;
		}
	}

	pObj->SetWorld(NULL);
}
#endif

void CWorld::Free()
{
#ifdef __WORLDSERVER
	CObj* pObjtmp;
	for (DWORD i = 0; i < m_dwObjNum; i++)
	{
		pObjtmp = m_apObject[i];
		SAFE_DELETE(pObjtmp);
	}

	int nBk = m_vecBackground.size();
	for (int i = 0; i < nBk; ++i)
		SAFE_DELETE(m_vecBackground[i]);
	m_vecBackground.clear();

	m_linkMap.Release();
	SAFE_DELETE_ARRAY(m_apHeightMap);
	SAFE_DELETE_ARRAY(m_apWaterHeight);

#else	// __WORLDSERVER

	POSITION pos
		= m_mapLight.GetStartPosition();
	CLight* pLight;
	CString string;
	while (pos)
	{
		m_mapLight.GetNextAssoc(pos, string, (void*&)pLight);
		SAFE_DELETE(pLight);
	}
	m_mapLight.RemoveAll();
	if (m_apLand != NULL)
	{
		for (int i = 0; i < m_nLandWidth * m_nLandHeight; i++)
			SAFE_DELETE(m_apLand[i]);
		SAFE_DELETE_ARRAY(m_apLand);
	}

	m_pObjFocus = NULL;
	m_nDeleteObjs = 0;

#if __VER >= 15 // __BS_CHANGING_ENVIR
	m_strCurContName = "";
#endif

	m_skyBox.m_bLockWeather = FALSE;
	g_Neuz.m_nRealWeather = WEATHER_NONE;
	m_skyBox.SetWeather(WEATHER_NONE, FALSE);
#endif	// __WORLDSERVER
}



void CWorld::CalcBound()
{
	WORLD_WIDTH = MAP_SIZE * m_nLandWidth;
	WORLD_HEIGHT = MAP_SIZE * m_nLandHeight;
}

void CWorld::LoadAllMoverDialog()
{
#ifdef __WORLDSERVER
	for (DWORD i = 0; i < m_dwObjNum; i++)
	{
		if (m_apObject[i] && m_apObject[i]->GetType() == OT_MOVER)
			((CMover*)m_apObject[i])->LoadDialog();
	}
#endif
}

#ifdef __CLIENT
void CWorld::SetObjFocus(CObj* pObj, BOOL bSend)
{
	CtrlProp* pProp = NULL;
	CCtrl* pCtrl = NULL;
	if (pObj && OT_CTRL == pObj->GetType())
	{
		pCtrl = (CCtrl*)pObj;
		pProp = prj.GetCtrlProp(pCtrl->GetIndex());
		if (!pProp)
			return;

		if (CK1_HOUSING == pProp->dwCtrlKind1)
			return;
	}

	if (pObj && OT_CTRL == pObj->GetType())
	{
		if (pProp->IsGuildHousingObj())
		{
			GuildHouse->m_dwSelectedObjID = pCtrl->GetId();

			CWndGuildHousing* pWnd = (CWndGuildHousing*)g_WndMng.GetApplet(APP_GH_FURNITURE_STORAGE);
			if (pWnd)
				pWnd->SetSelectedByObjID(GuildHouse->m_dwSelectedObjID);

			return;
		}
	}

	CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
	if (pWndWorld)
	{
		if (pWndWorld->m_bSetQuestNPCDest)
			pWndWorld->m_bSetQuestNPCDest = FALSE;
	}

	CObj* pOldFocus = m_pObjFocus;

	if (m_pObjFocus && m_pObjFocus->GetType() == OT_MOVER)
		m_idObjFocusOld = ((CMover*)m_pObjFocus)->GetId();

	if (pOldFocus)
	{
		if (pOldFocus->GetType() == OT_MOVER)
		{
			CMover* pMoverTarget = (CMover*)pOldFocus;
			OBJID idTarget = pMoverTarget->GetId();
			if (pOldFocus != pObj && pMoverTarget->IsLive())
			{
				if (bSend)
					g_DPlay.SendSetTarget(idTarget, 1);

				pMoverTarget->m_idTargeter = NULL_ID;
			}
		}
	}

	if (pObj != m_pObjFocus)
	{
		if (pObj)
		{
			if (pObj->GetType() == OT_MOVER)
			{
				CMover* pMoverTarget = (CMover*)pObj;
				OBJID idTarget = pMoverTarget->GetId();
				if (bSend)
					g_DPlay.SendSetTarget(idTarget, 2);
			}
		}
		else
		{
			if (bSend)
				g_DPlay.SendSetTarget(NULL_ID, 2);

			CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
			if (pWndWorld)
			{
				pWndWorld->m_pNextTargetObj = NULL;
				pWndWorld->m_pRenderTargetObj = NULL;
			}

		}
	}

	m_pObjFocus = pObj;

}
#endif


#ifndef __WORLDSERVER
void CWorld::AddLight(CLight* pLight)
{
	pLight->m_dwIndex = m_dwLightIndex++;
	m_mapLight.SetAt(pLight->m_strKey, pLight);
}

CLight* CWorld::GetLight(LPCTSTR lpszKey)
{
	CLight* pLight = NULL;
	m_mapLight.Lookup(lpszKey, (void*&)pLight);
	return pLight;
}
#endif	// __WORLDSERVER

#ifdef __LAYER_1015
BOOL CWorld::AddObj(CObj* pObj, BOOL bAddItToGlobalId, int nLayer)
#else	// __LAYER_1015
BOOL CWorld::AddObj(CObj* pObj, BOOL bAddItToGlobalId)
#endif	// __LAYER_1015
{
	if (pObj->GetWorld())
	{
		WriteError("CWorld::AddObj obj's world is NOT NULL.");
		return FALSE;
	}

	D3DXVECTOR3 v = pObj->GetPos();
	if (VecInWorld(v) == FALSE)
	{
		WriteError("CWorld::AddObj world id:%d x:%f y:%f z:%f", GetID(), v.x, v.y, v.z);
		return FALSE;
	}

	if (pObj->IsDynamicObj())
	{
#ifndef __WORLDSERVER
		if (bAddItToGlobalId)
			((CCtrl*)pObj)->AddItToGlobalId();
#endif	// __WORLDSERVER
		FLOAT fHeight = GetLandHeight(v);
		v.y = (v.y < fHeight ? fHeight : v.y);


		if (pObj->m_dwType == OT_CTRL)
		{
			CCommonCtrl* pCCtrl = (CCommonCtrl*)pObj;
			CtrlProp* pProp = pCCtrl->GetProp();
			if (pProp)
			{
				if (pProp->dwCtrlKind1 == CK1_DOOR)
					v.y = pObj->GetPos().y;
			}
		}

		pObj->SetPos(v);
	}
	pObj->UpdateLocalMatrix();
#ifdef __LAYER_1015
	pObj->SetLayer(nLayer);
#endif	// __LAYER_1015
	pObj->SetWorld(this);

	if (pObj->m_pAIInterface)
		pObj->m_pAIInterface->InitAI();

#ifdef __WORLDSERVER
	if (m_cbAddObjs < 0 || m_cbAddObjs >= MAX_ADDOBJS)
	{
		Error("World AddObj Overflow World: %d, AddObjs: %d", GetID(), m_cbAddObjs);
		return FALSE;
	}

	pObj->m_nAddObjsArrayIndex = m_cbAddObjs;

	m_bAddItToGlobalId[m_cbAddObjs] = bAddItToGlobalId;
	m_apAddObjs[m_cbAddObjs++] = pObj;
#else	// __WORLDSERVER
	InsertObjLink(pObj);
	AddObjArray(pObj);
#endif	// __WORLDSERVER

#ifdef __CLIENT
	if (CMover::GetActiveMover() && CMover::GetActiveMover()->m_pActMover &&
		CMover::GetActiveMover()->m_pActMover->IsFly())
	{
		if (pObj->GetType() == OT_MOVER)
		{
			if (((CMover*)pObj)->GetId() == m_idObjFocusOld)
			{
				SetObjFocus(pObj);
			}
		}
	}
#endif //__CLIENT

	return TRUE;
}

void CWorld::RemoveObj(CObj* pObj)
{
	if (pObj->GetWorld() == NULL)
		return;
	pObj->SetWorld(NULL);
	RemoveObjLink(pObj);
	RemoveObjArray(pObj);
}


void CWorld::DeleteObj(CObj* pObj)
{
#ifdef __WORLDSERVER
	if (pObj->m_nAddObjsArrayIndex >= 0)
	{
		removeObjectAtProcessArray(pObj);
		SAFE_DELETE(pObj);
		return;
	}
#endif

	if (!pObj->IsDelete())
	{
		pObj->SetDelete(TRUE);
#ifdef __WORLDSERVER
		if (m_nDeleteObjs >= MAX_DELETEOBJS)
		{
			if (pObj->GetType() == OT_MOVER)
				Error("CWorld::DeleteObj : %s %d", ((CMover*)pObj)->GetName(), m_nDeleteObjs);
			else
				Error("CWorld::DeleteObj : type=%d idx=%d %d", pObj->GetType(), pObj->GetIndex(), m_nDeleteObjs);
		}

		if (m_nDeleteObjs >= MAX_DELETEOBJS)
			Error("MAX_DELETEOBJS");
		m_apDeleteObjs[m_nDeleteObjs++] = pObj;
#else

		if (pObj->m_ppViewPtr)
		{
			*pObj->m_ppViewPtr = NULL;
			pObj->m_ppViewPtr = NULL;
		}

		m_apDeleteObjs[m_nDeleteObjs++] = pObj;
#endif
	}
}

BOOL CWorld::AddObjArray(CObj* pObj)
{
#ifdef __WORLDSERVER
	int nIndex;
	if (m_ObjStack.GetCount() > 0)
	{
		nIndex = m_ObjStack.Pop();
		ASSERT(!m_apObject[nIndex]);
	}
	else
	{
		nIndex = m_dwObjNum++;
	}

	if (nIndex == 0xffffffff)
	{
		WriteLog("AddObjArray nIndex = 0xffffffff");
		return FALSE;
	}

	m_apObject[nIndex] = (CCtrl*)pObj;
	pObj->m_dwObjAryIdx = nIndex;

	m_cbRunnableObject++;
#else	// __WORLDSERVER
	CLandscape* pLandscape = GetLandscape(pObj);
	if (NULL != pLandscape)
	{
		pLandscape->AddObjArray(pObj);
	}
#endif	// __WORLDSERVER
	return TRUE;
}

void CWorld::RemoveObjArray(CObj* pObj)
{
#ifdef __WORLDSERVER
	if (pObj->m_dwObjAryIdx < 0 || pObj->m_dwObjAryIdx >= MAX_DYNAMICOBJ)
	{
		WriteError(_T("m_dwObjAryIdx = %u | World = %d | X = %f | Y = %f | Z = %f"), pObj->m_dwObjAryIdx, pObj->GetWorld() ? pObj->GetWorld()->GetID() : NULL, pObj->GetPos().x, pObj->GetPos().y, pObj->GetPos().z);
		return;
	}

	if (m_apObject[pObj->m_dwObjAryIdx] == pObj)
	{
		m_ObjStack.Push(pObj->m_dwObjAryIdx);
		m_apObject[pObj->m_dwObjAryIdx] = NULL;
	}
	else
	{
		WriteError("RemoveObjArray m_apObject[pObj->m_dwObjAryIdx] != pObj ");
	}
	m_cbRunnableObject--;
#else	// __WORLDSERVER
	CLandscape* pLandscape = GetLandscape(pObj);
	if (pLandscape)
		pLandscape->RemoveObjArray(pObj);
#endif	// __WORLDSERVER
}

BOOL CWorld::InsertObjLink(CObj* pObj)
{
	D3DXVECTOR3 vPos = pObj->GetLinkPos();
	if (VecInWorld(vPos) == FALSE)
		return FALSE;

#ifdef __WORLDSERVER
	return m_linkMap.InsertObjLink(pObj);
#else	// __WORLDSERVER
	FLOAT rX = vPos.x / MPU;	// - m_nWorldX;
	FLOAT rZ = vPos.z / MPU;	// - m_nWorldY;
	int mX = int(rX / MAP_SIZE);
	int mZ = int(rZ / MAP_SIZE);

	CLandscape* pLandscape = m_apLand[mX + mZ * m_nLandWidth];
	if (NULL != pLandscape)
	{
		return pLandscape->InsertObjLink(pObj);
	}
	/*
	else
	{
		char szMessage[260]	= { 0, };
		sprintf( szMessage, "IOL error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pObj->GetPos().x, pObj->GetPos().z );
		DEBUGOUT2( szMessage );
	}
	*/
	return FALSE;
#endif	// __WORLDSERVER
}

BOOL CWorld::RemoveObjLink2(CObj* pObj)
{
#ifdef __WORLDSERVER
	return m_linkMap.RemoveObjLink2(pObj);
#else
	return TRUE;
#endif
}

BOOL CWorld::RemoveObjLink(CObj* pObj)
{
	D3DXVECTOR3	vPos = pObj->GetLinkPos();
	if (VecInWorld(vPos) == FALSE)
	{
		WriteError("ROL//INVALID POS//%d, %d, %f, %f, %f, %f, %f, %f",
			pObj->GetType(), pObj->GetIndex(), pObj->GetPos().x, pObj->GetPos().y, pObj->GetPos().z, vPos.x, vPos.y, vPos.z);
		return FALSE;
	}
#ifdef __WORLDSERVER
	return m_linkMap.RemoveObjLink(pObj);
#else	// __WORLDSERVER
	FLOAT rX = vPos.x / MPU;
	FLOAT rZ = vPos.z / MPU;
	int   mX = int(rX / MAP_SIZE);
	int   mZ = int(rZ / MAP_SIZE);

	CLandscape* pLandscape = m_apLand[mX + mZ * m_nLandWidth];
	if (NULL != pLandscape) {
		return pLandscape->RemoveObjLink(pObj);
	}
	/*
	else {
		char szMessage[260]	= { 0, };
		sprintf( szMessage, "ROL error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pObj->GetPos().x, pObj->GetPos().z );
		DEBUGOUT2( szMessage );
	}
	*/
	return FALSE;
#endif	// __WORLDSERVER
}

#ifdef __LAYER_1015
CObj* CWorld::GetObjInLinkMap(const D3DXVECTOR3& vPos, DWORD dwLinkType, int nLinkLevel, int nLayer)
#else	// __LAYER_1015
CObj* CWorld::GetObjInLinkMap(const D3DXVECTOR3& vPos, DWORD dwLinkType, int nLinkLevel)
#endif	// __LAYER_1015
{
	if (VecInWorld(vPos) == FALSE)
		return NULL;
#ifdef __WORLDSERVER
#ifdef __LAYER_1015
	return m_linkMap.GetObjInLinkMap(vPos, dwLinkType, nLinkLevel, nLayer);
#else	// __LAYER_1015
	return m_linkMap.GetObjInLinkMap(vPos, dwLinkType, nLinkLevel);
#endif	// __LAYER_1015
#else	// __WORLDSERVER
	FLOAT rX = vPos.x / MPU;
	FLOAT rZ = vPos.z / MPU;
	int	mX = int(rX / MAP_SIZE);
	int	mZ = int(rZ / MAP_SIZE);
	CLandscape* pLandscape = m_apLand[mX + mZ * m_nLandWidth];
	if (NULL != pLandscape) {
		return pLandscape->GetObjInLinkMap(vPos, dwLinkType, nLinkLevel);
	}
	/*
	else {
		char szMessage[260]	= { 0, };
		sprintf( szMessage, "GOILM error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, vPos.x, vPos.z );
		DEBUGOUT2( szMessage );
	}
	*/
	return NULL;
#endif	// __WORLDSERVER
}

#ifdef __LAYER_1015
BOOL CWorld::SetObjInLinkMap(const D3DXVECTOR3& vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj, int nLayer)
#else	// __LAYER_1015
BOOL CWorld::SetObjInLinkMap(const D3DXVECTOR3& vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj)
#endif	// __LAYER_1015
{
	if (VecInWorld(vPos) == FALSE)
		return FALSE;
#ifdef __WORLDSERVER
#ifdef __LAYER_1015
	return m_linkMap.SetObjInLinkMap(vPos, dwLinkType, nLinkLevel, pObj, nLayer);
#else	// __LAYER_1015
	return m_linkMap.SetObjInLinkMap(vPos, dwLinkType, nLinkLevel, pObj);
#endif	// __LAYER_1015
#else	// __WORLDSERVER
	FLOAT rX = vPos.x / MPU;
	FLOAT rZ = vPos.z / MPU;
	int mX = int(rX / MAP_SIZE);
	int mZ = int(rZ / MAP_SIZE);
	CLandscape* pLandscape = m_apLand[mX + mZ * m_nLandWidth];
	if (NULL != pLandscape) {
		pLandscape->SetObjInLinkMap(vPos, dwLinkType, nLinkLevel, pObj);	//m_aObjLinkMap[mX_ + mZ_ * MAP_SIZE]	= pObj;
	}
	/*
	else {
		char szMessage[260]	= { 0, };
		sprintf( szMessage, "SOILM error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, vPos.x, vPos.z );
		DEBUGOUT2( szMessage );
	}
	*/
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __WORLDSERVER
void CWorld::Process()
{
	_add();
	_process();		// iocp
	_modifylink();	// _replace();
	_replace();		//_modifylink();
	_delete();

	if (m_bLoadScriptFlag)
	{
		LoadAllMoverDialog();
		m_bLoadScriptFlag = FALSE;
	}
#ifdef __LAYER_1021
	m_linkMap.Process(this);
#endif	// __LAYER_1021
}

#else	// __WORLDSERVER
void CWorld::Process()
{
	int i, j, k, l, x, y;
	WorldPosToLand(m_pCamera->m_vPos, x, y);
	CLandscape* pLand = NULL;
	DWORD dwObjProcessNum = 0;
	D3DXVECTOR3 vPos, * pvCameraPos = &m_pCamera->m_vPos;
	float	fLengthSq;
	float	fFarSq = CWorld::m_fFarPlane / 2;
	fFarSq *= fFarSq;
	int		nNumObj;
	CObj* pObj = NULL;

	int nNonCullNum = 0;
	for (i = 0; i < m_nObjCullSize; i++)
	{
		pObj = m_aobjCull[i];
		if (pObj && pObj->GetType() == OT_OBJ && pObj->GetModel()->m_pModelElem->m_bTrans && pObj->IsCull() == FALSE)
		{
			pObj->Process();
		}
	}

	static int idx[MAX_OBJARRAY] = { 0, 1, 2, 7, 3, 4, 5, 6 };
	for (i = y - m_nVisibilityLand; i <= y + m_nVisibilityLand; i++)
	{
		for (j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++)
		{
			if (LandInWorld(j, i) && m_apLand[i * m_nLandWidth + j])
			{
				pLand = m_apLand[i * m_nLandWidth + j];
				for (k = OT_ANI; k < MAX_OBJARRAY; k++)
				{
					if (idx[k] == OT_SFX)
						continue;

					CObj** apObject = pLand->m_apObject[idx[k]];
					nNumObj = pLand->m_adwObjNum[idx[k]];
					for (l = 0; l < nNumObj; l++)
					{
						if (*apObject && (*apObject)->IsDelete() == FALSE && (*apObject)->GetWorld() != NULL)
						{
							vPos = (*apObject)->GetPos() - *pvCameraPos;
							fLengthSq = D3DXVec3LengthSq(&vPos);
							if (fLengthSq < fFarSq)
							{
								(*apObject)->Process();
							}
						}
						apObject++;
					}
				}
			}
		}
	}

	g_DialogMsg.RemoveDeleteObjMsg();

	// Delete Obj
	DeleteObjects();

	if (m_pCamera)
	{
		if (CMover::GetActiveMover() && CMover::GetActiveMover()->m_pActMover)
		{
			if (CMover::GetActiveMover()->m_pActMover->IsFly())
				m_pCamera->Process(m_pd3dDevice, 10.0f);
			else
				m_pCamera->Process(m_pd3dDevice, 4.0f);
		}
	}
	if (g_pPlayer)
	{
		ReadWorld(g_pPlayer->GetPos());

		if (m_bViewWeather)
		{
			m_skyBox.m_pWorld = this;
			m_skyBox.Process();
		}

#if __VER >= 15 // __BS_CHANGING_ENVIR
		CheckInOutContinent();
#endif
	}

	for (i = 0; i < prj.m_terrainMng.m_nWaterFrame; i++)
	{
		if (i < MAX_WATER)
		{
			prj.m_terrainMng.m_fWaterFrame[i] += prj.m_terrainMng.m_pWaterIndexList[i].fWaterFrame;

			if (prj.m_terrainMng.m_fWaterFrame[i] >= static_cast<FLOAT>(prj.m_terrainMng.m_pWaterIndexList[i].ListCnt))
				prj.m_terrainMng.m_fWaterFrame[i] = 0.0f;
		}
	}

}
#endif	// not __WORLDSERVER

FLOAT CWorld::GetLandHeight(float x, float z)
{
	float _x = x, _z = z;

	if (VecInWorld(x, z) == FALSE)
		return 0;

	x /= m_iMPU;
	z /= m_iMPU;
#ifdef __WORLDSERVER
	int px, pz;
	FLOAT dx, dz;
	FLOAT dy1, dy2, dy3, dy4;
	px = (int)x;
	pz = (int)z;
	dx = x - px;
	dz = z - pz;

	FLOAT y1 = m_apHeightMap[px + pz * WORLD_WIDTH];
	FLOAT y2 = m_apHeightMap[px + 1 + pz * WORLD_WIDTH];

	if (px + (pz + 1) * WORLD_WIDTH >= WORLD_WIDTH * WORLD_HEIGHT)
		Error("CWorld::GetLandHeight : %d %d,  %f %f,  %d %d", px, pz, _x, _z, m_nLandWidth, m_nLandHeight);

	FLOAT y3 = m_apHeightMap[px + (pz + 1) * WORLD_WIDTH];
	FLOAT y4 = m_apHeightMap[px + 1 + (pz + 1) * WORLD_WIDTH];
	if (y1 >= HGT_NOWALK)
	{
		int n = (int)(y1 / HGT_NOWALK);
		y1 -= (float)(HGT_NOWALK * n);
	}
	if (y2 >= HGT_NOWALK)
	{
		int n = (int)(y2 / HGT_NOWALK);
		y2 -= (float)(HGT_NOWALK * n);
	}
	if (y3 >= HGT_NOWALK)
	{
		int n = (int)(y3 / HGT_NOWALK);
		y3 -= (float)(HGT_NOWALK * n);
	}
	if (y4 >= HGT_NOWALK)
	{
		int n = (int)(y4 / HGT_NOWALK);
		y4 -= (float)(HGT_NOWALK * n);
	}
	dy1 = y1 * (1 - dx) * (1 - dz);
	dy2 = y2 * dx * (1 - dz);
	dy3 = y3 * (1 - dx) * dz;
	dy4 = y4 * dx * dz;
	return dy1 + dy2 + dy3 + dy4;
#else
	float rX = x;
	float rZ = z;
	int mX = int(rX / MAP_SIZE);
	int mZ = int(rZ / MAP_SIZE);
	CLandscape* pLand = m_apLand[mX + mZ * m_nLandWidth];
	if (pLand == NULL)
		return 0;

	return pLand->GetHeight(x - (mX * MAP_SIZE), z - (mZ * MAP_SIZE));
#endif
}

FLOAT CWorld::GetLandHeight(const D3DXVECTOR3& vPos)
{
	return GetLandHeight(vPos.x, vPos.z);
}

#ifdef __CLIENT

// GetHeight(D3DXVECTOR vecPos)



FLOAT CWorld::GetLandHeight_Fast(float x, float z)
{
	if (VecInWorld(x, z) == FALSE)
		return 0;
	x /= MPU;
	z /= MPU;
	float rX = x;
	float rZ = z;
	int mX = int(rX / MAP_SIZE);
	int mZ = int(rZ / MAP_SIZE);
	if (m_apLand == NULL)
	{
		ASSERT(0);
		return 0;
	}
	CLandscape* pLand = m_apLand[mX + mZ * m_nLandWidth];
	if (pLand == NULL)
		return 0;
	return pLand->GetHeight_Fast(x - (mX * MAP_SIZE), z - (mZ * MAP_SIZE));
}
#endif

int CWorld::GetLandTris(float x, float z, D3DXVECTOR3* pTris)
{
	int offset = 0;
	if (GetLandTri2(x, z, pTris + offset)) offset += 6;
	if (GetLandTri2(x - m_iMPU, z - m_iMPU, pTris + offset)) offset += 6;
	if (GetLandTri2(x, z - m_iMPU, pTris + offset)) offset += 6;
	if (GetLandTri2(x + m_iMPU, z - m_iMPU, pTris + offset)) offset += 6;
	if (GetLandTri2(x - m_iMPU, z, pTris + offset)) offset += 6;
	if (GetLandTri2(x + m_iMPU, z, pTris + offset)) offset += 6;
	if (GetLandTri2(x - m_iMPU, z + m_iMPU, pTris + offset)) offset += 6;
	if (GetLandTri2(x, z + m_iMPU, pTris + offset)) offset += 6;
	if (GetLandTri2(x + m_iMPU, z + m_iMPU, pTris + offset)) offset += 6;
	return offset;
}
BOOL CWorld::GetLandTri2(float x, float z, D3DXVECTOR3* pTri)
{
	if (VecInWorld(x, z) == FALSE) return FALSE;

	x /= m_iMPU;
	z /= m_iMPU;

#ifdef __WORLDSERVER
	int gx = (int)x;
	int gz = (int)z;
	float glx = x - gx;
	float glz = z - gz;

	if ((gx + gz) % 2 == 0)
	{
		if (glx > glz)
		{
			//0,3,1
			pTri[0] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));

			pTri[3] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[4] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[5] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
		}
		else {
			//0,2,3
			pTri[0] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));

			pTri[3] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[4] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[5] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
		}
	}
	else
	{
		if (glx + glz < 1.0f)
		{
			//0,2,1
			pTri[0] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));

			pTri[3] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[4] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[5] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
		}
		else
		{
			//1,2,3
			pTri[0] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));

			pTri[3] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[4] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[5] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
		}
	}
	float y = pTri[0].y;
	if (y >= HGT_NOWALK)
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[0].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[1].y;
	if (y >= HGT_NOWALK)
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[1].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[2].y;
	if (y >= HGT_NOWALK)
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[2].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[3].y;
	if (y >= HGT_NOWALK)
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[3].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[4].y;
	if (y >= HGT_NOWALK)
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[4].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[5].y;
	if (y >= HGT_NOWALK)
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[5].y -= (float)(HGT_NOWALK * n);
	}

#else
	int lx = (int)(x / MAP_SIZE);
	int lz = (int)(z / MAP_SIZE);
	DWORD gx = (int)x - lx * MAP_SIZE;
	DWORD gz = (int)z - lz * MAP_SIZE;
	float glx = x - (int)x;
	float glz = z - (int)z;

	CLandscape* pLand = m_apLand[lx + lz * m_nLandWidth];
	if (pLand == NULL) return FALSE;

	if ((gx + gz) % 2 == 0) {
		if (glx > glz) {
			//0,3,1
			pTri[0] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));

			pTri[3] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[4] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[5] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
		}
		else {
			//0,2,3
			pTri[0] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));

			pTri[3] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[4] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[5] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));
		}
	}
	else {
		if (glx + glz < 1.0f) {
			//0,2,1
			pTri[0] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));

			pTri[3] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[4] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[5] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
		}
		else {
			//1,2,3
			pTri[0] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));

			pTri[3] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[4] = D3DXVECTOR3((float)((gx + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx, gz + 1)), (float)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[5] = D3DXVECTOR3((float)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (float)(pLand->GetHeight(gx + 1, gz)), (float)((gz + lz * MAP_SIZE) * m_iMPU));
		}
	}
#endif
	return TRUE;
}

void CWorld::GetLandTri(float x, float z, D3DXVECTOR3* pTri)
{
	if (VecInWorld(x, z) == FALSE) return;

	x /= m_iMPU;			//SMPU
	z /= m_iMPU;

#ifdef __WORLDSERVER
	int gx = (int)x;
	int gz = (int)z;
	float glx = x - gx;
	float glz = z - gz;

	if ((gx + gz) % 2 == 0) {
		if (glx > glz) {
			//0,3,1
			pTri[0] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
		}
		else {
			//0,2,3
			pTri[0] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
		}
	}
	else {
		if (glx + glz < 1.0f) {
			//0,2,1
			pTri[0] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
		}
		else {
			//1,2,3
			pTri[0] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz)*WORLD_WIDTH]), (float)((gz)*m_iMPU));
			pTri[1] = D3DXVECTOR3((float)((gx)*m_iMPU), (float)(m_apHeightMap[(gx)+(gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
			pTri[2] = D3DXVECTOR3((float)((gx + 1) * m_iMPU), (float)(m_apHeightMap[(gx + 1) + (gz + 1) * WORLD_WIDTH]), (float)((gz + 1) * m_iMPU));
		}
	}

	float y = pTri[0].y;
	if (y >= HGT_NOWALK)
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[0].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[1].y;
	if (y >= HGT_NOWALK)
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[1].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[2].y;
	if (y >= HGT_NOWALK)
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[2].y -= (float)(HGT_NOWALK * n);
	}

#else
	int lx = (int)(x / MAP_SIZE);
	int lz = (int)(z / MAP_SIZE);
	DWORD gx = (int)x - lx * MAP_SIZE;
	DWORD gz = (int)z - lz * MAP_SIZE;
	float glx = x - (int)x;
	float glz = z - (int)z;

	CLandscape* pLand = m_apLand[lx + lz * m_nLandWidth];
	if (pLand == NULL) return;

	if ((gx + gz) % 2 == 0) {
		if (glx > glz) {
			//0,3,1
			pTri[0] = D3DXVECTOR3((FLOAT)((gx + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx, gz)), (FLOAT)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[1] = D3DXVECTOR3((FLOAT)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx + 1, gz + 1)), (FLOAT)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[2] = D3DXVECTOR3((FLOAT)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx + 1, gz)), (FLOAT)((gz + lz * MAP_SIZE) * m_iMPU));
		}
		else {
			//0,2,3
			pTri[0] = D3DXVECTOR3((FLOAT)((gx + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx, gz)), (FLOAT)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[1] = D3DXVECTOR3((FLOAT)((gx + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx, gz + 1)), (FLOAT)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[2] = D3DXVECTOR3((FLOAT)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx + 1, gz + 1)), (FLOAT)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
		}
	}
	else {
		if (glx + glz < 1.0f) {
			//0,2,1
			pTri[0] = D3DXVECTOR3((FLOAT)((gx + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx, gz)), (FLOAT)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[1] = D3DXVECTOR3((FLOAT)((gx + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx, gz + 1)), (FLOAT)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[2] = D3DXVECTOR3((FLOAT)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx + 1, gz)), (FLOAT)((gz + lz * MAP_SIZE) * m_iMPU));
		}
		else {
			//1,2,3
			pTri[0] = D3DXVECTOR3((FLOAT)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx + 1, gz)), (FLOAT)((gz + lz * MAP_SIZE) * m_iMPU));
			pTri[1] = D3DXVECTOR3((FLOAT)((gx + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx, gz + 1)), (FLOAT)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
			pTri[2] = D3DXVECTOR3((FLOAT)((gx + 1 + lx * MAP_SIZE) * m_iMPU), (FLOAT)(pLand->GetHeight(gx + 1, gz + 1)), (FLOAT)((gz + 1 + lz * MAP_SIZE) * m_iMPU));
		}
	}
#endif

}


#ifdef __WORLDSERVER
void CWorld::AddItToView(CCtrl* pCtrl)
{
	m_linkMap.AddItToView(pCtrl);
}

void CWorld::ModifyView(CCtrl* pCtrl)
{
	m_linkMap.ModifyView(pCtrl);
}

void CWorld::_add(void)
{
	CObj* pObj;

	if (g_DPCoreClient.CheckIdStack() == FALSE)
		return;

	for (int i = 0; i < m_cbAddObjs; i++)
	{
		pObj = m_apAddObjs[i];
		if (!::IsValidObj(pObj))
			continue;

		pObj->m_nAddObjsArrayIndex = -1;

		if (!pObj->IsVirtual())
		{
			if (pObj->IsDynamicObj())
			{
				CCtrl* pCtrl = static_cast<CCtrl*>(pObj);
				if (pCtrl != NULL)
				{
					if (m_bAddItToGlobalId[i])
						pCtrl->AddItToGlobalId();
					AddItToView((CCtrl*)pObj);
				}
			}
		}

		if (!pObj->IsVirtual())
		{
			if (!InsertObjLink(pObj))
				continue;
		}

		if (!AddObjArray(pObj))
		{
			RemoveObjLink(pObj);
			continue;
		}

		CNpcChecker::GetInstance()->AddNpc(pObj);

	}
	m_cbAddObjs = 0;
}

void CWorld::_modifylink(void)
{
	CObj* pObj, * pObjtmp;
	D3DXVECTOR3 vOld, vCur, vOldtmp, vCurtmp;
	int nLinkLevel;
	DWORD dwLinkType;

	for (int i = 0; i < m_cbModifyLink; i++)
	{
		pObj = m_apModifyLink[i];
		if (IsInvalidObj(pObj))
			continue;

		if (pObj->GetWorld() != this)
		{
			WriteError("LINKMAP world different");
			continue;
		}

		vOld = pObj->GetLinkPos();
		vCur = pObj->GetRemovalPos();
		vOldtmp = vOld / (FLOAT)(m_iMPU);
		vCurtmp = vCur / (FLOAT)(m_iMPU);

		if ((int)vOldtmp.x != (int)vCurtmp.x || (int)vOldtmp.z != (int)vCurtmp.z)
		{
			dwLinkType = pObj->GetLinkType();
			nLinkLevel = (int)pObj->GetLinkLevel();
			int nLayer = pObj->GetLayer();

			if (pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && nLinkLevel != 0)
				WriteError("ML//%s//%d//%d", ((CMover*)pObj)->GetName(), ((CMover*)pObj)->m_idPlayer, nLinkLevel);
			if (!pObj->m_pPrev && GetObjInLinkMap(vOld, dwLinkType, nLinkLevel, nLayer) != pObj)
			{
				WriteError("ML//BINGO//%d//%d//%d", pObj->GetType(), pObj->GetIndex(), pObj->GetLinkLevel());
				RemoveObjLink2(pObj);
			}

			if (GetObjInLinkMap(vOld, dwLinkType, nLinkLevel, nLayer) == pObj)
				SetObjInLinkMap(vOld, dwLinkType, nLinkLevel, pObj->m_pNext, nLayer);

			pObj->DelNode();

			if ((pObjtmp = GetObjInLinkMap(vCur, dwLinkType, nLinkLevel, nLayer)))
				pObjtmp->InsNextNode(pObj);
			else
				SetObjInLinkMap(vCur, dwLinkType, nLinkLevel, pObj, nLayer);

			pObj->SetLinkPos(pObj->GetRemovalPos());
			pObj->SetRemovalPos(D3DXVECTOR3(0, 0, 0));

			if (pObj->IsDynamicObj())
				ModifyView((CCtrl*)pObj);
		}
	}
	m_cbModifyLink = 0;
}

void CWorld::_delete(void)
{
	CObj* pObj;

	for (int i = 0; i < m_nDeleteObjs; i++)
	{
		pObj = m_apDeleteObjs[i];

		for (int j = 0; j < m_cbReplaceObj; j++)
		{
			if (m_aReplaceObj[j].pObj == pObj)
				m_aReplaceObj[j].pObj = NULL;
		}

		for (int j = 0; j < m_cbModifyLink; j++)
		{
			if (m_apModifyLink[j] == pObj)
			{
				m_apModifyLink[j] = NULL;
				pObj->m_vRemoval = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			}
		}

#ifdef __WORLDSERVER
		if (!pObj->IsVirtual())
#endif	// 
			RemoveObjLink(pObj);
		DestroyObj(pObj);
	}
	m_nDeleteObjs = 0;
}

void CWorld::DestroyObj(CObj* pObj)
{
	RemoveObjArray(pObj);
#ifdef __WORLDSERVER
	CNpcChecker::GetInstance()->RemoveNpc(pObj);
#endif	// 
	SAFE_DELETE(pObj);
}

void CWorld::_replace(void)
{
	CMover* pMover;
	DWORD		dwWorldID;
	D3DXVECTOR3 vPos;
	u_long		uIdofMulti;

	for (int i = 0; i < m_cbReplaceObj; ++i)
	{
		pMover = m_aReplaceObj[i].pObj;
		if (::IsInvalidObj(pMover))
			continue;

		dwWorldID = m_aReplaceObj[i].dwWorldID;
		vPos = m_aReplaceObj[i].vPos;
		uIdofMulti = m_aReplaceObj[i].uIdofMulti;
		int nLayer = m_aReplaceObj[i].nLayer;

		if (uIdofMulti != g_uIdofMulti)
			continue;

		if (GetID() == dwWorldID && pMover->GetLayer() == nLayer)
		{
			if (vPos.y == 0.0f)
			{
				vPos.y = 100.0f;
				vPos.y = GetFullHeight(vPos);
			}

			pMover->ClearDest();
			pMover->SendActMsg(OBJMSG_ACC_STOP);
			pMover->SendActMsg(OBJMSG_STOP_TURN);
			pMover->SendActMsg(OBJMSG_STAND);

			g_UserMng.AddSetPos((CCtrl*)pMover, vPos);
			pMover->SetPos(vPos);
			if (pMover->IsPlayer())
				((CUser*)pMover)->Notify();
		}
		else
		{
			if (pMover->IsPlayer() == FALSE)
				continue;

			CUser* pUser = (CUser*)pMover;
			if (GuildHouseMng->IsGuildHouse(GetID()))
				GuildHouseMng->CheckDestroyGuildHouse(pUser);

			CWorld* pWorld = g_WorldMng.GetWorld(dwWorldID);
			if (pWorld && pWorld->VecInWorld(vPos))
			{
				CLinkMap* pLinkMap = pWorld->m_linkMap.GetLinkMap(nLayer);
				if (!pLinkMap)
				{
					Error("[ LinkMap Invalid. WorldID(%d), LayerID(%d) ]", dwWorldID, nLayer);
					continue;
				}

#ifdef __FL_EQUALIZED_MODE
				pMover->TemporalOut();
				for (auto& it : prj.m_vecTmpWorlds)
				{
					if (it.dwWorldID == dwWorldID)
					{
						if (pMover->TemporalPreCheck(dwWorldID))
							pUser->AddOpenTempWindow(it.nMode);

						break;
					}
				}
#endif

				pUser->RemoveAllEnemies();

				pUser->RemoveItFromView(TRUE);		// pc
				pUser->RemoveItFromView2(TRUE);		// npc

				if (pUser->HasPet())
					pUser->RemovePet();

#ifdef __AIO_PETS
				if (pUser->HasActivatedEatPet())
				{
					CMover* pEatPet = prj.GetMover(pUser->GetEatPetId());
					if (IsValidObj(pEatPet))
					{
						CAIPet* pAIPet = static_cast<CAIPet*>(pEatPet->m_pAIInterface);
						if (pAIPet)
							pUser->SetSavedEatPetId(pAIPet->GetPetItemId());
					}

					pUser->InactivateEatPet();
				}
#endif

				if (pUser->GetWorld() && CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(pUser->GetWorld()->GetID()) == TRUE)
					CInstanceDungeonHelper::GetInstance()->LeaveDungeon(pUser, pUser->GetWorld()->GetID());
				else if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(dwWorldID) == TRUE)
					CInstanceDungeonHelper::GetInstance()->IncreasePlayerCount(dwWorldID, nLayer);

				GuildHouseMng->ResetApplyDST(pUser);
#ifdef __FL_MONSTER_ALBUM
				pUser->m_MonsterAlbum.ResetApplyDST();
#endif

				pUser->ClearDest();
				pUser->SendActMsg(OBJMSG_ACC_STOP);
				pUser->SendActMsg(OBJMSG_STOP_TURN);
				pUser->SendActMsg(OBJMSG_STAND);

				pUser->AddReplace(dwWorldID, vPos);
				pUser->Notify();
				RemoveObj(pUser);
				pUser->SetPos(vPos);
				pUser->PCSetAt(pUser->GetId(), pUser);
				pUser->AddAddObj(pUser);
				pWorld->ADDOBJ(pUser, FALSE, nLayer);
				pUser->AddSMModeAll();
				pUser->AddEnvironment();

				GuildHouseMng->SetApplyDST(pUser);
#ifdef __FL_MONSTER_ALBUM
				pUser->m_MonsterAlbum.SetApplyDST();
#endif

				if (dwWorldID == WI_WORLD_MINIROOM)
				{
					CHousing* pHousing = CHousingMng::GetInstance()->GetHousing(static_cast<DWORD>(nLayer));
					if (pHousing)
					{
						pUser->AddHousingPaperingInfo(NULL_ID, FALSE);
						vector<DWORD> vecTemp = pHousing->GetAllPaperingInfo();
						for (DWORD i = 0; i < vecTemp.size(); i++)
						{
							pUser->AddHousingPaperingInfo(vecTemp[i], TRUE);
						}
					}
				}

#ifdef __FL_VENDORS
				pUser->ActivateClientVendor();
#endif

#ifdef __AIO_PETS
				if (pUser->GetSavedEatPetId() != NULL_ID)
				{
					CItemElem* pItemElem = static_cast<CItemElem*>(pUser->m_Inventory.GetAtId(pUser->GetSavedEatPetId()));
					pUser->InactivateEatPet();
					pUser->ActivateEatPet(pItemElem);

					pUser->SetSavedEatPetId(NULL_ID);
				}
#endif

			}
			else
			{
				ASSERT(0);
				WriteError("_replace world id:%d x:%f y:%f z:%f", dwWorldID, vPos.x, vPos.y, vPos.z);
			}
		}
	}
	m_cbReplaceObj = 0;

}

void CWorld::_process(void)
{
	CObj* pObj;
	for (DWORD i = 0; i < m_dwObjNum; i++)
	{
		pObj = m_apObject[i];
		if (pObj && !pObj->IsDelete() && pObj->m_dwObjAryIdx != 0xffffffff)
			pObj->Process();
	}
}
#endif	// __WORLDSERVER

#ifndef __WORLDSERVER
LPWATERHEIGHT CWorld::GetWaterHeight(int x, int z)
{
	if (VecInWorld((FLOAT)(x), (FLOAT)(z)) == FALSE)
		return NULL;
	x /= m_iMPU;
	z /= m_iMPU;
	if (x < 0 || z < 0 || (int)x >= m_nLandWidth * MAP_SIZE || (int)z >= m_nLandHeight * MAP_SIZE)
		return 0;
	float rX = (float)(x);// - m_nWorldX;
	float rZ = (float)(z);// - m_nWorldY;
	int mX = int(rX / MAP_SIZE);
	int mZ = int(rZ / MAP_SIZE);

	if (m_apLand[mX + mZ * m_nLandWidth] == NULL)
		return 0;

	int tx = (x % MAP_SIZE) / PATCH_SIZE;
	int tz = (z % MAP_SIZE) / PATCH_SIZE;

	return m_apLand[mX + mZ * m_nLandWidth]->GetWaterHeight(tx, tz);
}
int CWorld::GetHeightAttribute(float x, float z)
{
	if (VecInWorld(x, z) == FALSE)
		return -1;

	float mpuInv = 1.0f / (float)m_iMPU;

	x *= mpuInv; //x /= MPU;
	z *= mpuInv; //z /= MPU;

	int rX = (int)(x);
	int rZ = (int)(z);
	int mX = int(rX / MAP_SIZE);
	int mZ = int(rZ / MAP_SIZE);

	if (m_apLand[mX + mZ * m_nLandWidth] == NULL)
		return -1;
	return	(m_apLand[mX + mZ * m_nLandWidth]->GetHeightAttribute((int)(x - (mX * MAP_SIZE)), (int)(z - (mZ * MAP_SIZE))));
}

#endif // not WORLDSERVER

#ifdef __WORLDSERVER
int CWorld::GetHeightAttribute(float x, float z)
{
	if (VecInWorld(x, z) == FALSE)
		return -1;

	float inv_mpu = 1.0f / (float)m_iMPU;

	x *= inv_mpu; //x /= MPU;
	z *= inv_mpu; //z /= MPU;

	FLOAT fHeight = m_apHeightMap[(int)x + (int)z * WORLD_WIDTH]; //WORLD_WIDTH = MAP_SIZE * m_nLandWidth
	if (fHeight >= HGT_NOWALK)
	{
		if (fHeight >= HGT_DIE)
			return HATTR_DIE;
		if (fHeight >= HGT_NOMOVE)
			return HATTR_NOMOVE;
		if (fHeight >= HGT_NOFLY)
			return HATTR_NOFLY;
		return HATTR_NOWALK;
	}
	return HATTR_NONE;
}
LPWATERHEIGHT CWorld::GetWaterHeight(int x, int z)
{
	if (VecInWorld((float)(x), (float)(z)) == FALSE)
		return NULL;

	x /= m_iMPU;
	z /= m_iMPU;

	int tx = x / PATCH_SIZE;
	int tz = z / PATCH_SIZE;

	LPWATERHEIGHT lpWaterHeight = &m_apWaterHeight[tx + tz * (NUM_PATCHES_PER_SIDE * m_nLandWidth)];
	return lpWaterHeight;
}

#endif // WORLDSERVER



// ------------------------------------------





// (&vPos, AF_MAGICSKILL, this, nSkill, 5.0f, 0.0, 1.0f )
void	CWorld::SendDamageAround(const D3DXVECTOR3* pvPos, int nDmgType, CMover* pAttacker, int nApplyType, int nAttackID, float fRange)
{
#ifdef __WORLDSERVER
	int nRange = 4;
	float fDistSq;
	CObj* pObj;
	CMover* pTarget;
	D3DXVECTOR3 vPos = *pvPos;
	D3DXVECTOR3 vDist;

	if (fRange <= 4.0f)
		nRange = 4;
	else if (fRange <= 8.0f)
		nRange = 8;
	else if (fRange <= 16.0f)
		nRange = 16;
	else if (fRange <= 32.0f)
		nRange = 32;
	else
		nRange = 64;

	if (fRange <= 0)
		Error("CWorld::SendDamageAround : D:%d,%d,%d A:%s %d %f", pvPos->x, pvPos->y, pvPos->z, pAttacker->GetName(), nAttackID, fRange);

	ItemProp* pProp;
	if (nDmgType == AF_MAGICSKILL)
	{
		pProp = prj.GetSkillProp(nAttackID);
		if (pProp == NULL)
		{
			Error("CWorld::SendDamageAround : %s. 스킬(%d)의 프로퍼티가 업ㅂ다.", pAttacker->GetName(), nAttackID);
			return;	// property not found
		}
	}
	else
	{
		pProp = prj.GetItemProp(nAttackID);
		if (pProp == NULL)
		{
			Error("CWorld::SendDamageAround : %s. 아이템(%d)의 프로퍼티가 업ㅂ다.", pAttacker->GetName(), nAttackID);
			return;	// property not found
		}
	}


	if (nApplyType & OBJTYPE_PLAYER)
	{
		FOR_LINKMAP(this, vPos, pObj, nRange, CObj::linkPlayer, pAttacker->GetLayer())
		{
			if (pObj->GetType() == OT_MOVER)
			{
				vDist = pObj->GetPos() - vPos;
				fDistSq = D3DXVec3LengthSq(&vDist);
				if (fDistSq < fRange * fRange)
				{
					if (pObj != pAttacker)
					{
						pTarget = (CMover*)pObj;
						if (IsValidObj((CObj*)pTarget) && pTarget->IsLive())
						{
							if (pProp->dwComboStyle == CT_FINISH)
								pTarget->m_pActMover->SendDamageForce(nDmgType, pAttacker->GetId(), (nAttackID << 16));
							else
								pTarget->m_pActMover->SendDamage(nDmgType, pAttacker->GetId(), (nAttackID << 16));
						}
					}
				}

			}
		}
		END_LINKMAP
	}


	if (nApplyType & OBJTYPE_MONSTER)
	{
		FOR_LINKMAP(this, vPos, pObj, nRange, CObj::linkDynamic, pAttacker->GetLayer())
		{
			if (pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPeaceful() == FALSE)
			{
				vDist = pObj->GetPos() - vPos;
				fDistSq = D3DXVec3LengthSq(&vDist);
				if (fDistSq < fRange * fRange)
				{
					if (pObj != pAttacker)
					{
						pTarget = (CMover*)pObj;
						if (IsValidObj((CObj*)pTarget) && pTarget->IsLive())
						{
							if (pProp->dwComboStyle == CT_FINISH)
								pTarget->m_pActMover->SendDamageForce(nDmgType, pAttacker->GetId(), (nAttackID << 16));
							else
								pTarget->m_pActMover->SendDamage(nDmgType, pAttacker->GetId(), (nAttackID << 16));
						}
					}
				}
			}
		}
		END_LINKMAP
	}

#endif // WORLDSERVER
} // SendDamageAround()

#ifdef __CLIENT
void CWorld::ProcessAllSfx(void)
{
	for (int i = 0; i < m_nLandWidth; i++)
	{
		for (int j = 0; j < m_nLandWidth; j++)
		{
			if (LandInWorld(j, i) && m_apLand[i * m_nLandWidth + j])
			{
				CLandscape* pLand = m_apLand[i * m_nLandWidth + j];
				CObj** apObject = pLand->m_apObject[OT_SFX];
				int nNumObj = pLand->m_adwObjNum[OT_SFX];
				for (int l = 0; l < nNumObj; l++)
				{
					if (*apObject && (*apObject)->IsDelete() == FALSE && (*apObject)->GetWorld() != NULL)
						(*apObject)->Process();
					apObject++;
				}
			}
		}
	}

}
#endif	// __CLIENT


#ifdef __LAYER_1021
void CWorld::Expand()
{
	m_respawner.Expand(nDefaultLayer);
#ifdef __AZRIA_1023
	int nExpand = CTicketProperty::GetInstance()->GetExpanedLayer(GetID());
	for (int nLayer = -1, i = 0; i < nExpand; i++, nLayer--)
		CreateLayer(nLayer);
#endif	// __AZRIA_1023
}
#endif	// __LAYER_1021

