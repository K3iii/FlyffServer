#include "stdafx.h"
#include "Sfx.h"

#include "dpclient.h"
extern	CDPClient	g_DPlay;
#include "defineObj.h"

CSfx::CSfx()
{
	m_dwType = OT_SFX;
	m_idSrc = NULL_ID;
	m_idDest = NULL_ID;
	m_vPosDest = D3DXVECTOR3(0, 0, 0);
	m_nFrame = 0;
	m_nSec = -1;
	m_idSfxHit = 0;
	m_nMagicPower = 0;
	m_dwSkill = NULL_ID;
	m_pSfxObj = NULL;
}
CSfx::CSfx(int nIndex, const OBJID idSrc, const D3DXVECTOR3& vPosSrc, const OBJID idDest, const D3DXVECTOR3& vPosDest)
{
	m_idSfxHit = 0;
	m_nMagicPower = 0;
	m_pSfxObj = NULL;
}
CSfx::~CSfx()
{
	if (m_idSfxHit)
	{
		if (m_idSrc == NULL_ID)
		{
			Error("CSfx::~CSfx : %d %d %d", m_dwIndex, m_idSfxHit, m_idSrc);
		}
		else
			g_DPlay.SendSfxClear(m_idSfxHit, m_idSrc);
	}
}

BOOL CSfx::SetIndex(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwIndex, BOOL bInitProp)
{
	BOOL bResult = FALSE;
	m_idSrc = NULL_ID;
	m_idDest = NULL_ID;
	m_vPosDest = D3DXVECTOR3(0, 0, 0);

	if (dwIndex >= 0)
	{
		bResult = SetTypeIndex(pd3dDevice, OT_SFX, dwIndex, bInitProp);
		m_pSfxObj = (CSfxModel*)m_pModel;
	}
	UpdateLocalMatrix();
	return bResult;
}

int	CSfx::SetSfx(LPDIRECT3DDEVICE9 pd3dDevice, int nIndex,
	D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec)
{
	m_idSrc = idSrc;
	m_idDest = idDest;
	m_vPosDest = vPosDest;
	m_nSec = nSec;

	vPosSrc.y += 0.01f;
	SetPos(vPosSrc);

	if (nIndex >= 0)
	{
		SetTypeIndex(pd3dDevice, OT_SFX, nIndex, TRUE);
		m_pSfxObj = (CSfxModel*)m_pModel;
		return 1;
	}
	UpdateLocalMatrix();

	if (m_pSfxObj)
		return 1;

	return 0;
}

void CSfx::DamageToTarget(int nMaxDmgCnt)
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl(m_idSrc);
	CCtrl* pObjDest = prj.GetCtrl(m_idDest);

	if (IsInvalidObj(pObjSrc))
		return;
	if (IsInvalidObj(pObjDest))
		return;

	if (pObjDest->GetType() == OT_MOVER)
	{
		CMover* pMover = (CMover*)pObjDest;
		PLAYSND(pMover->GetProp()->dwSndDmg2, &pMover->GetScrPos());

		if (pObjSrc->IsActiveMover() || (pObjSrc->IsPlayer() == FALSE && pObjDest->IsActiveObj()))
		{
			g_DPlay.SendSfxHit(m_idSfxHit, m_dwSkill, pObjSrc->GetId());
			if (nMaxDmgCnt == 1)
				m_idSfxHit = 0;
		}
	}
}
void CSfx::Process()
{
	m_nFrame++;
	if (m_nSec == 0)
	{
		if (m_pSfxObj->Process())
			Delete();
	}
	else
	{

		if (m_pSfxObj->Process())
			m_pSfxObj->m_nCurFrame = 0;
		if (m_nSec != -1)
		{
			if (m_nFrame > (m_nSec * 60))
				Delete();
		}
	}

	if (m_pSfxObj->m_pSfxBase != NULL)
	{
		for (int j = 0; j < m_pSfxObj->m_pSfxBase->m_apParts.GetSize(); j++)
		{
			if (m_pSfxObj->m_pSfxBase->Part(j)->m_nType != SFXPARTTYPE_MESH)
				continue;

			CModelObject* pMesh = NULL;

			pMesh = g_SfxMeshMng.Mesh(m_pSfxObj->m_pSfxBase->Part(j)->m_strTex);

			if (pMesh)
				pMesh->FrameMove();
		}
	}

	if (m_idDest != NULL_ID)
	{
		CMover* pObjDest = (CMover*)prj.GetCtrl(m_idDest);
		if (IsValidObj(pObjDest))
			m_vPosDest = pObjDest->GetScrPos();
		else
		{
			Delete();
			return;
		}

		SetPos(m_vPosDest);
	}
	else
	{
		if (m_idSrc != NULL_ID)
		{
			CMover* pObjSrc = (CMover*)prj.GetCtrl(m_idSrc);
			if (IsValidObj(pObjSrc))
				SetPos(pObjSrc->GetScrPos());
			else
				Delete();
		}
	}
}
void CSfx::Render(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (!IsVisible() || (IsCull() && GetType() != 1))
		return;
	if (IsUpdateMatrix())
		UpdateMatrix();

	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem(m_dwType, m_dwIndex);

	if (lpModelElem == NULL)
		return;

	CMover* pObjDest = prj.GetMover(m_idDest);
	CMover* pObjSrc = prj.GetMover(m_idSrc);

	m_pSfxObj->m_vPos = GetScrPos();
	switch (m_dwIndex)
	{
	case XI_SKILL_MENT_DARKNESSSCREAM03:
	case XI_SKILL_ELE_SLEEPING02:
	{
		if (IsValidObj(pObjDest))
		{
			m_pSfxObj->m_vPos = pObjDest->GetScrPos();
			m_pSfxObj->m_vPos.y += ((pObjDest->m_pModel->GetMaxHeight() * pObjDest->GetScale().y) - (pObjDest->GetScale().y * 0.7f));
		}
		else
			return;
	}
	break;
	default: m_pSfxObj->m_vPos.y += 0.2f; break;
	}

	switch (m_dwIndex)
	{
	case XI_SKILL_CRACK_HWAKEYE01:
	case XI_SKILL_CRACK_HWAKEYE02:
	{
		if (IsValidObj(pObjSrc))
		{
			m_pSfxObj->m_vRotate.y = -pObjSrc->GetAngle();
		}
	}
	break;
	default: m_pSfxObj->m_vRotate.y = GetAngle(); break;
	}

	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render(pd3dDevice, NULL);

}

