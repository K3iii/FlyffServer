#include "stdafx.h"
#include "model.h"
#include "sfxbase.h"
#include "..\_Common\ParticleMng.h"


LPDIRECT3DDEVICE9 CSfxMng::m_pd3dDevice;
LPDIRECT3DVERTEXBUFFER9 CSfxMng::m_pSfxVB;
CSfxTexture g_SfxTex;
CSfxMng g_SfxMng;
CSfxMeshMng g_SfxMeshMng;

#define RANDF ((rand()%50000)/50000.0f)






CString GetTextureName(CString strTex, int nIndex)
{
	CString strRet;
	CString strTemp1, strName, strExt;
	strTemp1 = strTex.Left(strTex.GetLength() - 4);
	strTemp1 = strTemp1.Right(2);
	int nTexnumStart = atoi(strTemp1);
	strTemp1.Format("%02d", nTexnumStart + nIndex);
	strName = strTex.Left(strTex.GetLength() - 6);
	strExt = strTex.Right(4);
	strRet = strName + strTemp1 + strExt;
	return strRet;
}

CSfxPart::CSfxPart()
{
}
CSfxPart::~CSfxPart()
{
	for (int i = 0; i < m_apKeyFrames.GetSize(); i++)
		SAFE_DELETE_CONST((SfxKeyFrame*)m_apKeyFrames.GetAt(i));
}

void CSfxPart::AddKeyFrame(WORD nFrame)
{
	if (KeyByFrame(nFrame)) return;
	SfxKeyFrame* pKey = new SfxKeyFrame;

	SfxKeyFrame* pPrevKey = GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey = GetNextKey(nFrame);
	if (pPrevKey == NULL || pNextKey == NULL) {
		pKey->vPos = D3DXVECTOR3(0, 0, 0);
		pKey->vPosRotate.x = pKey->vPosRotate.y = pKey->vPosRotate.z = .0f;
		if (this->m_nType == SFXPARTTYPE_CUSTOMMESH) pKey->vPosRotate.z = 1.0f;
		pKey->vScale.x = pKey->vScale.y = pKey->vScale.z = 1.0f;
		pKey->vRotate.x = pKey->vRotate.y = pKey->vRotate.z = .0f;
		pKey->nAlpha = 255;
	}
	else {
		int dFrame = pNextKey->nFrame - pPrevKey->nFrame;
		*(pKey) = *(pPrevKey);
		if (dFrame != 0)
		{
			pKey->vPos += (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
			pKey->vPosRotate = (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
			pKey->vRotate += (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
			pKey->vScale += (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
			pKey->nAlpha += (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
		}
	}
	if (m_apKeyFrames.GetSize() > 0) {
		for (int i = 0; i < m_apKeyFrames.GetSize(); i++) {
			if (Key(i)->nFrame == nFrame) {
				SAFE_DELETE(pKey);
				return;
			}
			if (Key(i)->nFrame > nFrame) {
				pKey->nFrame = nFrame;
				m_apKeyFrames.InsertAt(i, pKey);
				return;
			}
			//			*(pKey)=*(Key(i));
		}
	}
	pKey->nFrame = nFrame;
	m_apKeyFrames.Add(pKey);
}
void CSfxPart::DeleteKeyFrame(WORD nFrame)
{
	for (int i = 0; i < m_apKeyFrames.GetSize(); i++)
		if (Key(i)->nFrame == nFrame) {
			SAFE_DELETE_CONST(Key(i));
			m_apKeyFrames.RemoveAt(i);
			return;
		}
}
void CSfxPart::DeleteAllKeyFrame()
{
	for (int i = 0; i < m_apKeyFrames.GetSize(); i++)
	{
		if (Key(i))
			SAFE_DELETE_CONST(Key(i));
	}
	m_apKeyFrames.RemoveAll();
}

SfxKeyFrame* CSfxPart::GetPrevKey(WORD nFrame)
{
	SfxKeyFrame* pKey = NULL;
	SfxKeyFrame* pTempKey = NULL;
	for (int i = 0; i < m_apKeyFrames.GetSize(); i++) {
		pTempKey = Key(i);
		if (pTempKey->nFrame > nFrame) break;
		pKey = pTempKey;
	}
	return pKey;
}
SfxKeyFrame* CSfxPart::GetNextKey(WORD nFrame, BOOL bSkip)
{
	SfxKeyFrame* pKey = NULL;
	SfxKeyFrame* pTempKey;
	for (int i = 0; i < m_apKeyFrames.GetSize(); i++)
	{
		pTempKey = Key(i);
		if (bSkip)
		{
			if (pTempKey->nFrame >= nFrame)
			{
				pKey = pTempKey;
				break;
			}
		}
		else
		{
			if (pTempKey->nFrame > nFrame)
			{
				pKey = pTempKey;
				break;
			}
		}
	}
	return pKey;
}

void CSfxPart::AdjustKeyFrame(WORD nFrame, SfxKeyFrame& key)
{
	key.nFrame = nFrame;
	for (int i = 0; i < m_apKeyFrames.GetSize(); i++)
		if (Key(i)->nFrame == nFrame) {
			*(Key(i)) = key;
			return;
		}
}
#ifndef __WORLDSERVER
void CSfxPart::Render(D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale)
{
}
#endif
void CSfxPart::Render2(D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, D3DXVECTOR3 vScale)
{
}
void CSfxPart::Load(CResFile& file)
{
}
void CSfxPart::OldLoad(CResFile& file)
{
}
void CSfxPart::Load2(CResFile& file)
{
}

void CSfxPart::GetKey(WORD nFrame, SfxKeyFrame* pKey)
{
	SfxKeyFrame* pPrevKey = GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey = GetNextKey(nFrame);
	if (pPrevKey == NULL || pNextKey == NULL) return;
	int dFrame = pNextKey->nFrame - pPrevKey->nFrame;
	*pKey = *(pPrevKey);
	if (dFrame != 0) {
		pKey->vPos += (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		pKey->vPosRotate += (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		pKey->vRotate += (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		pKey->vScale += (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		pKey->nAlpha += (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
	}
}

CSfxPartBill::CSfxPartBill()
{
	m_nType = SFXPARTTYPE_BILL;
	m_nTexFrame = 1;
	m_nTexLoop = 1;
	m_bUseing = TRUE;
}

void CSfxPartBill::Render2(D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, D3DXVECTOR3 vScale)
{
#ifdef __CLIENT
	if (m_nTexFrame > 1)
	{
		SfxKeyFrame* pFirstKey = GetNextKey(0);
		if (nFrame < pFirstKey->nFrame) return;
		int nTexFrame = (m_nTexFrame * (nFrame - pFirstKey->nFrame) / m_nTexLoop) % m_nTexFrame;
		CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(GetTextureName(m_strTex, nTexFrame)));
	}
	else
		CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(m_strTex));
	SfxKeyFrame* pPrevKey = GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey = GetNextKey(nFrame);
	if (pPrevKey == NULL || pNextKey == NULL) return;
	SfxKeyFrame Key;
	int dFrame = pNextKey->nFrame - pPrevKey->nFrame;
	Key = *(pPrevKey);

	if (dFrame != 0)
	{
		Key.vPos += (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vPosRotate += (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vRotate += (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vScale += (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.nAlpha += (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp, matTemp1, matTemp2;
	D3DXMATRIX matTrans, matAngle, matScale, matInv;
	D3DXVECTOR3 vTemp;
	D3DXVECTOR3 vScaleTemp = vScale;
	vScaleTemp.x *= Key.vScale.x;
	vScaleTemp.y *= Key.vScale.y;
	vScaleTemp.z *= Key.vScale.z;

	switch (m_nBillType)
	{
	case SFXPARTBILLTYPE_BILL:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		D3DXMatrixScaling(&matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z);
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
		matTemp = matScale * matAngle * g_matInvView;
		break;
	}
	case SFXPARTBILLTYPE_BOTTOM:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z - fAngle.z));
		D3DXMatrixScaling(&matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z);
		D3DXMatrixRotationX(&matTemp2, DEGREETORADIAN(90));
		matTemp = matScale * matAngle * matTemp2;
		break;
	}
	case SFXPARTBILLTYPE_POLE:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		D3DXMatrixScaling(&matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z);
		matTemp2 = g_matView; matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
		matTemp = matScale * matAngle * g_matInvView;
		break;
	}
	case SFXPARTBILLTYPE_NORMAL:
	{
		D3DXMATRIX mRot;
		D3DXVECTOR3 vRot = DEGREETORADIAN(fAngle);
		D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vRotate);
		D3DXMatrixRotationYawPitchRoll(&matAngle, vTemp2.y, vTemp2.x, vTemp2.z);
		D3DXMatrixRotationYawPitchRoll(&mRot, vRot.y, vRot.x, vRot.z);
		D3DXMatrixScaling(&matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z);
		matTemp = matScale * matAngle * mRot;
		break;
	}
	}
	D3DXMATRIX mRot;
	D3DXVECTOR3 vRot = DEGREETORADIAN(fAngle);
	D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vPosRotate);
	D3DXMatrixRotationYawPitchRoll(&matTemp2, vTemp2.y, vTemp2.x, vTemp2.z);
	D3DXMatrixRotationYawPitchRoll(&mRot, vRot.y, vRot.x, vRot.z);

	matTemp2 = mRot * matTemp2;

	D3DXVec3TransformCoord(&vTemp, &Key.vPos, &matTemp2);
	D3DXMatrixTranslation(&matTrans,
		vPos.x + (vTemp.x * vScale.x),
		vPos.y + (vTemp.y * vScale.y),
		vPos.z + (vTemp.z * vScale.z)
	);
	matTemp1 = matTemp * matTrans;

	switch (m_nAlphaType) {
	case SFXPARTALPHATYPE_BLEND:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	}
	case SFXPARTALPHATYPE_GLOW:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	}
	}



	//	D3DXMATRIX matWorld;
	//	CSfxMng::m_pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);
	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD, &matTemp1);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, Key.nAlpha << 24 | 0x404040);

	CSfxMng::m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	//	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
#endif //__CLIENT
}

#ifndef __WORLDSERVER
void CSfxPartBill::Render(D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale)
{
	if (m_nTexFrame > 1)
	{
		SfxKeyFrame* pFirstKey = GetNextKey(0);
		if (nFrame < pFirstKey->nFrame) return;
		int nTexFrame = (m_nTexFrame * (nFrame - pFirstKey->nFrame) / m_nTexLoop) % m_nTexFrame;
		CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(GetTextureName(m_strTex, nTexFrame)));
	}
	else
		CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(m_strTex));
	SfxKeyFrame* pPrevKey = GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey = GetNextKey(nFrame);
	if (pPrevKey == NULL || pNextKey == NULL) return;
	SfxKeyFrame Key;
	int dFrame = pNextKey->nFrame - pPrevKey->nFrame;
	Key = *(pPrevKey);

	if (dFrame != 0)
	{
		Key.vPos += (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vPosRotate += (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vRotate += (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vScale += (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.nAlpha += (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp, matTemp1, matTemp2;
	D3DXMATRIX matTrans, matAngle, matScale, matInv;
	D3DXVECTOR3 vTemp;
	D3DXVECTOR3 vScaleTemp = vScale;
	vScaleTemp.x *= Key.vScale.x;
	vScaleTemp.y *= Key.vScale.y;
	vScaleTemp.z *= Key.vScale.z;

	switch (m_nBillType)
	{
	case SFXPARTBILLTYPE_BILL:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		D3DXMatrixScaling(&matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z);
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
		matTemp = matScale * matAngle * g_matInvView;
		break;
	}
	case SFXPARTBILLTYPE_BOTTOM:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z - fAngle));
		D3DXMatrixScaling(&matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z);
		D3DXMatrixRotationX(&matTemp2, DEGREETORADIAN(90));
		matTemp = matScale * matAngle * matTemp2;
		break;
	}
	case SFXPARTBILLTYPE_POLE:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		D3DXMatrixScaling(&matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z);
		matTemp2 = g_matView; matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
		matTemp = matScale * matAngle * g_matInvView;
		break;
	}
	case SFXPARTBILLTYPE_NORMAL:
	{
		D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vRotate + D3DXVECTOR3(.0f, fAngle, .0f));
		D3DXMatrixRotationYawPitchRoll(&matAngle, vTemp2.y, vTemp2.x, vTemp2.z);
		D3DXMatrixScaling(&matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z);
		matTemp = matScale * matAngle;
		break;
	}
	}
	D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vPosRotate + D3DXVECTOR3(.0f, fAngle, .0f));
	D3DXMatrixRotationYawPitchRoll(&matTemp2, vTemp2.y, vTemp2.x, vTemp2.z);
	D3DXVec3TransformCoord(&vTemp, &Key.vPos, &matTemp2);
	D3DXMatrixTranslation(&matTrans,
		vPos.x + (vTemp.x * vScale.x),
		vPos.y + (vTemp.y * vScale.y),
		vPos.z + (vTemp.z * vScale.z)
	);
	matTemp1 = matTemp * matTrans;

	switch (m_nAlphaType) {
	case SFXPARTALPHATYPE_BLEND:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	}
	case SFXPARTALPHATYPE_GLOW:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	}
	}



	//	D3DXMATRIX matWorld;
	//	CSfxMng::m_pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);
	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD, &matTemp1);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, Key.nAlpha << 24 | 0x404040);

	CSfxMng::m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	//	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
}
#endif

void CSfxPartBill::Load(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;
	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = m_strTex = strTemp;
	file.Read(&m_nTexFrame, sizeof(WORD));
	file.Read(&m_nTexLoop, sizeof(WORD));
	m_bUseing = TRUE;

	if (m_nTexFrame > 1) {
		for (i = 0; i < m_nTexFrame; i++) {
			g_SfxTex.Tex(GetTextureName(strTemp, i));
		}
	}

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		WORD nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
}
void CSfxPartBill::Load2(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = strTemp;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strTex = strTemp;
	file.Read(&m_nTexFrame, sizeof(WORD));
	file.Read(&m_nTexLoop, sizeof(WORD));
	file.Read(&m_bUseing, sizeof(BOOL));

	if (m_nTexFrame > 1) {
		for (i = 0; i < m_nTexFrame; i++) {
			g_SfxTex.Tex(GetTextureName(strTemp, i));
		}
	}

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		WORD nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
}
void CSfxPartBill::OldLoad(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = m_strTex = strTemp;
	m_nTexFrame = 1;
	m_nTexLoop = 1;
	m_bUseing = TRUE;

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		WORD nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
}

CSfxPartParticle::CSfxPartParticle()
{
	m_nType = SFXPARTTYPE_PARTICLE;
	m_nTexFrame = 1;
	m_nTexLoop = 1;
	m_bUseing = TRUE;

	m_nParticleCreate = 0;
	m_nParticleCreateNum = 0;

	m_nParticleFrameAppear = 0;
	m_nParticleFrameKeep = 0;
	m_nParticleFrameDisappear = 0;

	m_fParticleStartPosVar = 0;
	m_fParticleStartPosVarY = 0;

	m_fParticleYLow = 0;
	m_fParticleYHigh = 0;
	m_fParticleXZLow = 0;
	m_fParticleXZHigh = 0;

	m_vParticleAccel = D3DXVECTOR3(.0f, .0f, .0f);

	m_vScale = D3DXVECTOR3(.0f, .0f, .0f);
	m_vScaleSpeed = D3DXVECTOR3(.0f, .0f, .0f);
	m_vRotation = D3DXVECTOR3(.0f, .0f, .0f);

	m_vRotationLow = D3DXVECTOR3(.0f, .0f, .0f);
	m_vRotationHigh = D3DXVECTOR3(.0f, .0f, .0f);


	//#ifdef __ATEST
	m_bRepeatScal = FALSE;
	m_bRepeat = FALSE;
	m_fScalSpeedXLow = 0;
	m_fScalSpeedXHigh = 0;
	m_fScalSpeedYLow = 0;
	m_fScalSpeedYHigh = 0;
	m_fScalSpeedZLow = 0;
	m_fScalSpeedZHigh = 0;
	m_vScaleEnd = D3DXVECTOR3(.0f, .0f, .0f);
	m_vScaleSpeed2 = D3DXVECTOR3(.0f, .0f, .0f);
	//#endif

}

CSfxPartParticle::~CSfxPartParticle()
{
}
#ifndef __WORLDSERVER
void CSfxPartParticle::Render(D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale)
{
	CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(m_strTex));
	SfxKeyFrame* pPrevKey = GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey = GetNextKey(nFrame);
	if (pPrevKey == NULL || pNextKey == NULL) return;
	SfxKeyFrame Key;
	int dFrame = pNextKey->nFrame - pPrevKey->nFrame;
	Key = *(pPrevKey);
	if (dFrame != 0)
	{
		Key.vPos += (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vPosRotate += (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vRotate += (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vScale += (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.nAlpha += (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp, matTemp1, matTemp2;
	D3DXMATRIX matTrans, matAngle, matScale, matInv;
	D3DXVECTOR3 vTemp;
	switch (m_nBillType) {
	case SFXPARTBILLTYPE_BILL:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(fAngle + Key.vRotate.z));
		D3DXMatrixScaling(&matScale, Key.vScale.x, Key.vScale.y, 1.0f);
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
		matTemp = matScale * matAngle * g_matInvView;
		break;
	}
	case SFXPARTBILLTYPE_BOTTOM:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(fAngle + Key.vRotate.z));
		D3DXMatrixScaling(&matScale, Key.vScale.x, Key.vScale.y, 1.0f);
		D3DXMatrixRotationX(&matTemp2, DEGREETORADIAN(90));
		matTemp = matScale * matAngle * matTemp2;
		break;
	}
	case SFXPARTBILLTYPE_POLE:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(fAngle + Key.vRotate.z));
		D3DXMatrixScaling(&matScale, Key.vScale.x, Key.vScale.y, 1.0f);
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
		matTemp = matScale * matAngle * g_matInvView;
		break;
	}
	case SFXPARTBILLTYPE_NORMAL:
	{
		D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vRotate);
		D3DXMatrixRotationYawPitchRoll(&matAngle, vTemp2.y, vTemp2.x, vTemp2.z);
		D3DXMatrixScaling(&matScale, Key.vScale.x, Key.vScale.y, 1.0f);
		matTemp = matScale * matAngle;
		break;
	}
	}
	D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vPosRotate + D3DXVECTOR3(.0f, fAngle, .0f));
	D3DXMatrixRotationYawPitchRoll(&matTemp2, vTemp2.y, vTemp2.x, vTemp2.z);
	D3DXVec3TransformCoord(&vTemp, &Key.vPos, &matTemp2);
	//D3DXMatrixTranslation(&matTrans,vPos.x+vTemp.x,vPos.y+vTemp.y,vPos.z+vTemp.z);
	D3DXMatrixTranslation(&matTrans,
		vPos.x + (vTemp.x * vScale.x),
		vPos.y + (vTemp.y * vScale.y),
		vPos.z + (vTemp.z * vScale.z));

	matTemp1 = matTemp * matTrans;

	switch (m_nAlphaType)
	{
	case SFXPARTALPHATYPE_BLEND:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	}
	case SFXPARTALPHATYPE_GLOW:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	}
	}
	//	D3DXMATRIX matWorld;
	//	CSfxMng::m_pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);
	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD, &matTemp1);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, Key.nAlpha << 24 | 0x404040);

	CSfxMng::m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	//	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
}
#endif // __WORLDSERVER


void CSfxPartParticle::Load(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;
	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = m_strTex = strTemp;
	file.Read(&m_nTexFrame, sizeof(WORD));
	file.Read(&m_nTexLoop, sizeof(WORD));
	m_bUseing = TRUE;
	if (m_nTexFrame > 1) {
		for (i = 0; i < m_nTexFrame; i++) {
			g_SfxTex.Tex(GetTextureName(strTemp, i));
		}
	}

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));

	file.Read(&m_nParticleCreate, sizeof(WORD));
	file.Read(&m_nParticleCreateNum, sizeof(WORD));

	file.Read(&m_nParticleFrameAppear, sizeof(WORD));
	file.Read(&m_nParticleFrameKeep, sizeof(WORD));
	file.Read(&m_nParticleFrameDisappear, sizeof(WORD));

	file.Read(&m_fParticleStartPosVar, sizeof(FLOAT));
	file.Read(&m_fParticleStartPosVarY, sizeof(FLOAT));

	file.Read(&m_fParticleYLow, sizeof(FLOAT));
	file.Read(&m_fParticleYHigh, sizeof(FLOAT));
	file.Read(&m_fParticleXZLow, sizeof(FLOAT));
	file.Read(&m_fParticleXZHigh, sizeof(FLOAT));

	file.Read(&m_vParticleAccel, sizeof(D3DXVECTOR3));

	file.Read(&m_vScale, sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleSpeed, sizeof(D3DXVECTOR3));
	m_vRotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRotationLow = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRotationHigh = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_bRepeatScal = FALSE;
	m_bRepeat = FALSE;
	m_fScalSpeedXLow = 0;
	m_fScalSpeedXHigh = 0;
	m_fScalSpeedYLow = 0;
	m_fScalSpeedYHigh = 0;
	m_fScalSpeedZLow = 0;
	m_fScalSpeedZHigh = 0;
	m_vScaleSpeed2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vScaleEnd = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	int nKey = m_apKeyFrames.GetSize();
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		int nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
}
void CSfxPartParticle::Load2(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = strTemp;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strTex = strTemp;
	file.Read(&m_nTexFrame, sizeof(WORD));
	file.Read(&m_nTexLoop, sizeof(WORD));
	file.Read(&m_bUseing, sizeof(BOOL));
	if (m_nTexFrame > 1) {
		for (i = 0; i < m_nTexFrame; i++) {
			g_SfxTex.Tex(GetTextureName(strTemp, i));
		}
	}

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));

	file.Read(&m_nParticleCreate, sizeof(WORD));
	file.Read(&m_nParticleCreateNum, sizeof(WORD));

	file.Read(&m_nParticleFrameAppear, sizeof(WORD));
	file.Read(&m_nParticleFrameKeep, sizeof(WORD));
	file.Read(&m_nParticleFrameDisappear, sizeof(WORD));

	file.Read(&m_fParticleStartPosVar, sizeof(FLOAT));
	file.Read(&m_fParticleStartPosVarY, sizeof(FLOAT));

	file.Read(&m_fParticleYLow, sizeof(FLOAT));
	file.Read(&m_fParticleYHigh, sizeof(FLOAT));
	file.Read(&m_fParticleXZLow, sizeof(FLOAT));
	file.Read(&m_fParticleXZHigh, sizeof(FLOAT));

	file.Read(&m_vParticleAccel, sizeof(D3DXVECTOR3));

	file.Read(&m_vScale, sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleSpeed, sizeof(D3DXVECTOR3));
	file.Read(&m_vRotation, sizeof(D3DXVECTOR3));
	file.Read(&m_vRotationLow, sizeof(D3DXVECTOR3));
	file.Read(&m_vRotationHigh, sizeof(D3DXVECTOR3));

	file.Read(&m_bRepeatScal, sizeof(BOOL));
	m_bRepeat = FALSE;
	file.Read(&m_fScalSpeedXLow, sizeof(FLOAT));
	file.Read(&m_fScalSpeedXHigh, sizeof(FLOAT));
	file.Read(&m_fScalSpeedYLow, sizeof(FLOAT));
	file.Read(&m_fScalSpeedYHigh, sizeof(FLOAT));
	file.Read(&m_fScalSpeedZLow, sizeof(FLOAT));
	file.Read(&m_fScalSpeedZHigh, sizeof(FLOAT));
	file.Read(&m_vScaleSpeed2, sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleEnd, sizeof(D3DXVECTOR3));

	int nKey = m_apKeyFrames.GetSize();
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		int nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
}
void CSfxPartParticle::Load3(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = strTemp;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strTex = strTemp;
	file.Read(&m_nTexFrame, sizeof(WORD));
	file.Read(&m_nTexLoop, sizeof(WORD));
	file.Read(&m_bUseing, sizeof(BOOL));
	if (m_nTexFrame > 1) {
		for (i = 0; i < m_nTexFrame; i++) {
			g_SfxTex.Tex(GetTextureName(strTemp, i));
		}
	}

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));

	file.Read(&m_nParticleCreate, sizeof(WORD));
	file.Read(&m_nParticleCreateNum, sizeof(WORD));

	file.Read(&m_nParticleFrameAppear, sizeof(WORD));
	file.Read(&m_nParticleFrameKeep, sizeof(WORD));
	file.Read(&m_nParticleFrameDisappear, sizeof(WORD));

	file.Read(&m_fParticleStartPosVar, sizeof(FLOAT));
	file.Read(&m_fParticleStartPosVarY, sizeof(FLOAT));

	file.Read(&m_fParticleYLow, sizeof(FLOAT));
	file.Read(&m_fParticleYHigh, sizeof(FLOAT));
	file.Read(&m_fParticleXZLow, sizeof(FLOAT));
	file.Read(&m_fParticleXZHigh, sizeof(FLOAT));

	file.Read(&m_vParticleAccel, sizeof(D3DXVECTOR3));

	file.Read(&m_vScale, sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleSpeed, sizeof(D3DXVECTOR3));
	file.Read(&m_vRotation, sizeof(D3DXVECTOR3));
	file.Read(&m_vRotationLow, sizeof(D3DXVECTOR3));
	file.Read(&m_vRotationHigh, sizeof(D3DXVECTOR3));

	file.Read(&m_bRepeatScal, sizeof(BOOL));
	file.Read(&m_bRepeat, sizeof(BOOL));
	file.Read(&m_fScalSpeedXLow, sizeof(FLOAT));
	file.Read(&m_fScalSpeedXHigh, sizeof(FLOAT));
	file.Read(&m_fScalSpeedYLow, sizeof(FLOAT));
	file.Read(&m_fScalSpeedYHigh, sizeof(FLOAT));
	file.Read(&m_fScalSpeedZLow, sizeof(FLOAT));
	file.Read(&m_fScalSpeedZHigh, sizeof(FLOAT));
	file.Read(&m_vScaleSpeed2, sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleEnd, sizeof(D3DXVECTOR3));

	int nKey = m_apKeyFrames.GetSize();
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		int nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
}
void CSfxPartParticle::OldLoad(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = m_strTex = strTemp;
	m_nTexFrame = 1;
	m_nTexLoop = 1;
	m_bUseing = TRUE;

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));

	file.Read(&m_nParticleCreate, sizeof(WORD));
	file.Read(&m_nParticleCreateNum, sizeof(WORD));

	file.Read(&m_nParticleFrameAppear, sizeof(WORD));
	file.Read(&m_nParticleFrameKeep, sizeof(WORD));
	file.Read(&m_nParticleFrameDisappear, sizeof(WORD));

	file.Read(&m_fParticleStartPosVar, sizeof(FLOAT));
	file.Read(&m_fParticleStartPosVarY, sizeof(FLOAT));

	file.Read(&m_fParticleYLow, sizeof(FLOAT));
	file.Read(&m_fParticleYHigh, sizeof(FLOAT));
	file.Read(&m_fParticleXZLow, sizeof(FLOAT));
	file.Read(&m_fParticleXZHigh, sizeof(FLOAT));

	file.Read(&m_vParticleAccel, sizeof(D3DXVECTOR3));

	file.Read(&m_vScale, sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleSpeed, sizeof(D3DXVECTOR3));
	m_vRotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRotationLow = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRotationHigh = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_bRepeatScal = FALSE;
	m_bRepeat = FALSE;
	m_fScalSpeedXLow = 0;
	m_fScalSpeedXHigh = 0;
	m_fScalSpeedYLow = 0;
	m_fScalSpeedYHigh = 0;
	m_fScalSpeedZLow = 0;
	m_fScalSpeedZHigh = 0;
	m_vScaleSpeed2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vScaleEnd = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	int nKey = m_apKeyFrames.GetSize();
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		int nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
}

CSfxPartMesh::CSfxPartMesh()
{
	m_nType = SFXPARTTYPE_MESH;
	m_nTexFrame = 1;
	m_nTexLoop = 1;
	m_bUseing = TRUE;
}
CSfxPartMesh::~CSfxPartMesh()
{
}
void CSfxPartMesh::Load(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;
	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = m_strTex = strTemp;
	file.Read(&m_nTexFrame, sizeof(WORD));
	file.Read(&m_nTexLoop, sizeof(WORD));
	m_bUseing = TRUE;

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		WORD nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
}
void CSfxPartMesh::Load2(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = strTemp;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strTex = strTemp;
	file.Read(&m_nTexFrame, sizeof(WORD));
	file.Read(&m_nTexLoop, sizeof(WORD));
	file.Read(&m_bUseing, sizeof(BOOL));

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		WORD nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
}


void CSfxPartMesh::OldLoad(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = m_strTex = strTemp;
	m_nTexFrame = 1;
	m_nTexLoop = 1;
	m_bUseing = TRUE;

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		WORD nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
}

#ifndef __WORLDSERVER
void CSfxPartMesh::Render(D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale)
{
	SfxKeyFrame* pPrevKey = GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey = GetNextKey(nFrame);
	if (pPrevKey == NULL || pNextKey == NULL)
	{
		return;
	}
	SfxKeyFrame Key;
	int dFrame = pNextKey->nFrame - pPrevKey->nFrame;
	Key = *(pPrevKey);
	if (dFrame != 0) {
		Key.vPos += (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vPosRotate += (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vRotate += (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vScale += (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.nAlpha += (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp, matTemp1, matTemp2;
	D3DXMATRIX matTrans, matAngle, matScale, matInv;
	D3DXVECTOR3 vTemp;
	D3DXVECTOR3 vScaleTemp = vScale;
	vScaleTemp.x *= Key.vScale.x;
	vScaleTemp.y *= Key.vScale.y;
	vScaleTemp.z *= Key.vScale.z;

	{
		D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vRotate + D3DXVECTOR3(.0f, fAngle, .0f));
		D3DXMatrixRotationYawPitchRoll(&matAngle, vTemp2.y, vTemp2.x, vTemp2.z);
		D3DXMatrixScaling(&matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z);
		matTemp = matScale * matAngle;
		//matTemp=matScale*matAngle*g_matInvView;
	}

	D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vPosRotate + D3DXVECTOR3(.0f, fAngle, .0f));
	D3DXMatrixRotationYawPitchRoll(&matTemp2, vTemp2.y, vTemp2.x, vTemp2.z);
	D3DXVec3TransformCoord(&vTemp, &Key.vPos, &matTemp2);
	D3DXMatrixTranslation(&matTrans,
		vPos.x + (vTemp.x * vScale.x),
		vPos.y + (vTemp.y * vScale.y),
		vPos.z + (vTemp.z * vScale.z));
	matTemp1 = matTemp * matTrans;

	switch (m_nAlphaType) {
	case SFXPARTALPHATYPE_BLEND:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	}
	case SFXPARTALPHATYPE_GLOW:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	}
	}
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, Key.nAlpha << 24 | 0x404040);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD, &matTemp1);
	CModelObject* pMesh = g_SfxMeshMng.Mesh(m_strTex);

	if (pMesh)
	{
		//		pMesh->m_nNoEffect = 1;
		/*		DWORD v;
				pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,   &v);
				pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &v);
				pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &v );
				pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1, &v );
				pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,   &v );
				pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &v );
				pd3dDevice->GetRenderState( D3DRS_SRCBLEND, &v );
				pd3dDevice->GetRenderState( D3DRS_DESTBLEND, &v );
				pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &v );
				pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &v );
		*/
		pMesh->m_nNoEffect = 1;

		if (m_nAlphaType == SFXPARTALPHATYPE_BLEND)
		{
			pMesh->m_nNoEffect = 0;
			CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		}

		pMesh->Render(CSfxMng::m_pd3dDevice, &matTemp1);

		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, Key.nAlpha << 24 | 0x404040);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	}
	//CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
	//CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}
#endif // __WORLDSERVER
CSfxPartCustomMesh::CSfxPartCustomMesh()
{
	m_nType = SFXPARTTYPE_CUSTOMMESH;
	m_nTexFrame = 1;
	m_nTexLoop = 1;
	m_nPoints = 20;
	m_bUseing = TRUE;
}
CSfxPartCustomMesh::~CSfxPartCustomMesh()
{
}

void CSfxPartCustomMesh::Load(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;
	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = m_strTex = strTemp;
	file.Read(&m_nTexFrame, sizeof(WORD));
	file.Read(&m_nTexLoop, sizeof(WORD));
	m_bUseing = TRUE;

	if (m_nTexFrame > 1) {
		for (i = 0; i < m_nTexFrame; i++) {
			g_SfxTex.Tex(GetTextureName(strTemp, i));
		}
	}

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		WORD nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
	file.Read(&m_nPoints, sizeof(int));
}
void CSfxPartCustomMesh::Load2(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = strTemp;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strTex = strTemp;
	file.Read(&m_nTexFrame, sizeof(WORD));
	file.Read(&m_nTexLoop, sizeof(WORD));
	file.Read(&m_bUseing, sizeof(BOOL));
	if (m_nTexFrame > 1) {
		for (i = 0; i < m_nTexFrame; i++) {
			g_SfxTex.Tex(GetTextureName(strTemp, i));
		}
	}

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		WORD nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
	file.Read(&m_nPoints, sizeof(int));
}
void CSfxPartCustomMesh::OldLoad(CResFile& file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen, sizeof(int));
	file.Read(strTemp, nStrLen); strTemp[nStrLen] = 0;
	m_strName = m_strTex = strTemp;
	m_nTexFrame = 1;
	m_nTexLoop = 1;
	m_bUseing = TRUE;

	file.Read(&m_nBillType, sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType, sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey, sizeof(int));
	for (i = 0; i < nKey; i++) {
		WORD nFrame;
		file.Read(&nFrame, sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale), sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate), sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha), sizeof(int));
		AdjustKeyFrame(nFrame, Key);
	}
	file.Read(&m_nPoints, sizeof(int));
}



#ifndef __WORLDSERVER
void CSfxPartCustomMesh::Render(D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale)
{
	if (m_nTexFrame > 1)
	{
		SfxKeyFrame* pFirstKey = GetNextKey(0);
		if (nFrame < pFirstKey->nFrame) return;
		int nTexFrame = (m_nTexFrame * (nFrame - pFirstKey->nFrame) / m_nTexLoop) % m_nTexFrame;
		CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(GetTextureName(m_strTex, nTexFrame)));
	}
	else CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(m_strTex));
	SfxKeyFrame* pPrevKey = GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey = GetNextKey(nFrame);
	if (pPrevKey == NULL || pNextKey == NULL) return;
	SfxKeyFrame Key;
	int dFrame = pNextKey->nFrame - pPrevKey->nFrame;
	Key = *(pPrevKey);
	if (dFrame != 0)
	{
		Key.vPos += (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vPosRotate += (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vRotate += (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vScale += (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.nAlpha += (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp, matTemp1, matTemp2;
	D3DXMATRIX matTrans, matAngle, matScale, matInv;
	D3DXVECTOR3 vTemp;
	switch (m_nBillType) {
	case SFXPARTBILLTYPE_BILL:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z - fAngle));
		//			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		D3DXMatrixInverse(&matInv, NULL, &matTemp2);
		matTemp = matAngle * matInv;
		break;
	}
	case SFXPARTBILLTYPE_BOTTOM:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		//			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
		D3DXMatrixRotationX(&matTemp2, DEGREETORADIAN(90));
		matTemp = matAngle * matTemp2;
		break;
	}
	case SFXPARTBILLTYPE_POLE:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		//			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		D3DXMatrixInverse(&matInv, NULL, &matTemp2);
		matTemp = matAngle * matInv;
		break;
	}
	case SFXPARTBILLTYPE_NORMAL:
	{
		D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vRotate + D3DXVECTOR3(.0f, fAngle, .0f));
		D3DXMatrixRotationYawPitchRoll(&matAngle, vTemp2.y, vTemp2.x, vTemp2.z);
		//			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
		matTemp = matAngle;
		break;
	}
	}
	/*
		D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vPosRotate+D3DXVECTOR3(.0f,fAngle,.0f));
		D3DXMatrixRotationYawPitchRoll(&matTemp2,vTemp2.y,vTemp2.x,vTemp2.z);
		D3DXVec3TransformCoord(&vTemp,&Key.vPos,&matTemp2);
	*/
	vTemp = Key.vPos;
	//*
	D3DXMATRIX matAngle2;
	D3DXVECTOR3 v3Rot = DEGREETORADIAN(D3DXVECTOR3(.0f, fAngle, .0f));
	D3DXMatrixRotationYawPitchRoll(&matAngle2, v3Rot.y, v3Rot.x, v3Rot.z);
	D3DXVec3TransformCoord(&vTemp, &vTemp, &matAngle2);
	/**/
		//D3DXMatrixTranslation(&matTrans,vPos.x+vTemp.x,vPos.y+vTemp.y,vPos.z+vTemp.z);
	D3DXMatrixTranslation(&matTrans,
		vPos.x + (vTemp.x * vScale.x),
		vPos.y + (vTemp.y * vScale.y),
		vPos.z + (vTemp.z * vScale.z));

	matTemp1 = matTemp * matTrans;

	switch (m_nAlphaType)
	{
	case SFXPARTALPHATYPE_BLEND:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	}
	case SFXPARTALPHATYPE_GLOW:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	}
	}


	D3DSFXVERTEX* pVertices = new D3DSFXVERTEX[((m_nPoints * 2) + 1) * 2];
	for (int i = 0; i < (m_nPoints * 2) + 1; i++)
	{
		pVertices[i * 2].p = D3DXVECTOR3(sin(DEGREETORADIAN(360 / (m_nPoints * 2) * i)) * Key.vScale.x, Key.vScale.y, cos(DEGREETORADIAN(360 / (m_nPoints * 2) * i)) * Key.vScale.x);
		pVertices[i * 2 + 1].p = D3DXVECTOR3(sin(DEGREETORADIAN(360 / (m_nPoints * 2) * i)) * Key.vScale.z, 0, cos(DEGREETORADIAN(360 / (m_nPoints * 2) * i)) * Key.vScale.z);
		pVertices[i * 2].p.x *= vScale.x;
		pVertices[i * 2].p.y *= vScale.y;
		pVertices[i * 2].p.z *= vScale.z;
		pVertices[i * 2 + 1].p.x *= vScale.x;
		pVertices[i * 2 + 1].p.y *= vScale.y;
		pVertices[i * 2 + 1].p.z *= vScale.z;

		pVertices[i * 2].tv1 = Key.vPosRotate.y + 0.02f; pVertices[i * 2 + 1].tv1 = Key.vPosRotate.z;
		//		pVertices[i*2].tv1 = Key.vPosRotate.y; pVertices[i*2+1].tv1 = Key.vPosRotate.z;
		if (Key.vPosRotate.x == .0f)
		{
			if (i / 2 * 2 == i)
			{
				pVertices[i * 2].tu1 = .0f; pVertices[i * 2 + 1].tu1 = .0f;
			}
			else
			{
				pVertices[i * 2].tu1 = 1.0f; pVertices[i * 2 + 1].tu1 = 1.0f;
			}
		}
		else
		{
			pVertices[i * 2].tu1 = Key.vPosRotate.x * ((FLOAT)i) / (m_nPoints * 2); pVertices[i * 2 + 1].tu1 = Key.vPosRotate.x * ((FLOAT)i) / (m_nPoints * 2);
		}
	}

	//	CSfxMng::m_pd3dDevice->SetStreamSource( 0, CSfxMng::m_pSfxVB, 0, sizeof( D3DSFXVERTEX ) );
	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD, &matTemp1);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, Key.nAlpha << 24 | 0x404040);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	CSfxMng::m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, m_nPoints * 4, pVertices, sizeof(D3DSFXVERTEX));
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	CSfxMng::m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, m_nPoints * 4, pVertices, sizeof(D3DSFXVERTEX));
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	SAFE_DELETE_ARRAY(pVertices);
}
#endif

void CSfxPartCustomMesh::Render2(D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, D3DXVECTOR3 vScale)
{
#ifdef __CLIENT
	if (m_nTexFrame > 1)
	{
		SfxKeyFrame* pFirstKey = GetNextKey(0);
		if (nFrame < pFirstKey->nFrame) return;
		int nTexFrame = (m_nTexFrame * (nFrame - pFirstKey->nFrame) / m_nTexLoop) % m_nTexFrame;
		CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(GetTextureName(m_strTex, nTexFrame)));
	}
	else CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(m_strTex));
	SfxKeyFrame* pPrevKey = GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey = GetNextKey(nFrame);
	if (pPrevKey == NULL || pNextKey == NULL) return;
	SfxKeyFrame Key;
	int dFrame = pNextKey->nFrame - pPrevKey->nFrame;
	Key = *(pPrevKey);
	if (dFrame != 0)
	{
		Key.vPos += (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vPosRotate += (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vRotate += (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vScale += (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.nAlpha += (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp, matTemp1, matTemp2;
	D3DXMATRIX matTrans, matAngle, matScale, matInv;
	D3DXVECTOR3 vTemp;
	switch (m_nBillType) {
	case SFXPARTBILLTYPE_BILL:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z - fAngle.z));
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		D3DXMatrixInverse(&matInv, NULL, &matTemp2);
		matTemp = matAngle * matInv;
		break;
	}
	case SFXPARTBILLTYPE_BOTTOM:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		D3DXMatrixRotationX(&matTemp2, DEGREETORADIAN(90));
		matTemp = matAngle * matTemp2;
		break;
	}
	case SFXPARTBILLTYPE_POLE:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		D3DXMatrixInverse(&matInv, NULL, &matTemp2);
		matTemp = matAngle * matInv;
		break;
	}
	case SFXPARTBILLTYPE_NORMAL:
	{
		D3DXMATRIX mRot;
		D3DXVECTOR3 vRot = DEGREETORADIAN(fAngle);
		D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vRotate);
		D3DXMatrixRotationYawPitchRoll(&matAngle, vTemp2.y, vTemp2.x, vTemp2.z);
		D3DXMatrixRotationYawPitchRoll(&mRot, vRot.y, vRot.x, vRot.z);
		matTemp = matAngle * mRot;
		break;
	}
	}

	vTemp = Key.vPos;

	D3DXMATRIX matAngle2;
	D3DXVECTOR3 v3Rot = DEGREETORADIAN(fAngle);
	D3DXMatrixRotationYawPitchRoll(&matAngle2, v3Rot.y, v3Rot.x, v3Rot.z);
	D3DXVec3TransformCoord(&vTemp, &vTemp, &matAngle2);

	D3DXMatrixTranslation(&matTrans,
		vPos.x + (vTemp.x * vScale.x),
		vPos.y + (vTemp.y * vScale.y),
		vPos.z + (vTemp.z * vScale.z));

	matTemp1 = matTemp * matTrans;

	switch (m_nAlphaType)
	{
	case SFXPARTALPHATYPE_BLEND:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	}
	case SFXPARTALPHATYPE_GLOW:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	}
	}


	D3DSFXVERTEX* pVertices = new D3DSFXVERTEX[((m_nPoints * 2) + 1) * 2];
	for (int i = 0; i < (m_nPoints * 2) + 1; i++)
	{
		pVertices[i * 2].p = D3DXVECTOR3(sin(DEGREETORADIAN(360 / (m_nPoints * 2) * i)) * Key.vScale.x, Key.vScale.y, cos(DEGREETORADIAN(360 / (m_nPoints * 2) * i)) * Key.vScale.x);
		pVertices[i * 2 + 1].p = D3DXVECTOR3(sin(DEGREETORADIAN(360 / (m_nPoints * 2) * i)) * Key.vScale.z, 0, cos(DEGREETORADIAN(360 / (m_nPoints * 2) * i)) * Key.vScale.z);
		pVertices[i * 2].p.x *= vScale.x;
		pVertices[i * 2].p.y *= vScale.y;
		pVertices[i * 2].p.z *= vScale.z;
		pVertices[i * 2 + 1].p.x *= vScale.x;
		pVertices[i * 2 + 1].p.y *= vScale.y;
		pVertices[i * 2 + 1].p.z *= vScale.z;

		pVertices[i * 2].tv1 = Key.vPosRotate.y + 0.1f; pVertices[i * 2 + 1].tv1 = Key.vPosRotate.z;
		if (Key.vPosRotate.x == .0f)
		{
			if (i / 2 * 2 == i)
			{
				pVertices[i * 2].tu1 = .0f; pVertices[i * 2 + 1].tu1 = .0f;
			}
			else
			{
				pVertices[i * 2].tu1 = 1.0f; pVertices[i * 2 + 1].tu1 = 1.0f;
			}
		}
		else
		{
			pVertices[i * 2].tu1 = Key.vPosRotate.x * ((FLOAT)i) / (m_nPoints * 2); pVertices[i * 2 + 1].tu1 = Key.vPosRotate.x * ((FLOAT)i) / (m_nPoints * 2);
		}
	}

	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD, &matTemp1);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, Key.nAlpha << 24 | 0x404040);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	CSfxMng::m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, m_nPoints * 4, pVertices, sizeof(D3DSFXVERTEX));
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	CSfxMng::m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, m_nPoints * 4, pVertices, sizeof(D3DSFXVERTEX));
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	SAFE_DELETE_ARRAY(pVertices);
#endif //__CLIENT
}

CSfxBase::CSfxBase()
{
}
CSfxBase::~CSfxBase()
{
	for (int i = 0; i < m_apParts.GetSize(); i++)
		SAFE_DELETE_CONST((CSfxPart*)m_apParts.GetAt(i));
	m_apParts.RemoveAll();
}

CSfxPart* CSfxBase::AddPart(SFXPARTTYPE nType)
{
	CSfxPart* pSfxPart;
	switch (nType) {
	case SFXPARTTYPE_BILL:
	{
		pSfxPart = new CSfxPartBill;
		break;
	}
	case SFXPARTTYPE_PARTICLE:
	{
		pSfxPart = new CSfxPartParticle;
		break;
	}
	case SFXPARTTYPE_MESH:
	{
		pSfxPart = new CSfxPartMesh;
		break;
	}
	case SFXPARTTYPE_CUSTOMMESH:
	{
		pSfxPart = new CSfxPartCustomMesh;
		break;
	}
	}
	pSfxPart->m_nType = nType;
	pSfxPart->m_strTex = "";
	pSfxPart->m_nBillType = SFXPARTBILLTYPE_BILL;
	pSfxPart->m_nAlphaType = SFXPARTALPHATYPE_BLEND;
	m_apParts.Add(pSfxPart);
	return pSfxPart;
}

void CSfxBase::DeletePart(BYTE nIndex)
{
	CSfxPart* pSfxPart = Part(nIndex);
	if (pSfxPart) {
		SAFE_DELETE(pSfxPart);
		m_apParts.RemoveAt(nIndex);
	}
	CString strFilename;
}

BOOL CSfxBase::LoadMerge()
{
	CString strFilename = MakePath(DIR_SFX, LPCTSTR(m_strName + ".sfx"));
	int i;
	CResFile file;
	if (file.Open(strFilename, "rb"))
	{
		char strTemp[9];
		file.Read(strTemp, 8);
		strTemp[8] = 0;
		CString strTemp2(strTemp);
		if (strTemp2.Left(6) == "SFX0.1") {
			int nPart;
			file.Read(&nPart, sizeof(int));
			for (i = 0; i < nPart; i++)
			{
				CSfxPart* pPart;
				SFXPARTTYPE nType;
				file.Read(&nType, sizeof(SFXPARTTYPE));
				pPart = AddPart(nType);
				pPart->Load(file);
			}
		}
		else
			if (strTemp2.Left(6) == "SFX0.2") {

				int nPart;
				file.Read(&nPart, sizeof(int));
				for (i = 0; i < nPart; i++)
				{
					CSfxPart* pPart;
					SFXPARTTYPE nType;
					file.Read(&nType, sizeof(SFXPARTTYPE));
					pPart = AddPart(nType);
					pPart->Load2(file);
				}
			}
			else
				if (strTemp2.Left(6) == "SFX0.3") {

					int nPart;
					file.Read(&nPart, sizeof(int));
					for (i = 0; i < nPart; i++)
					{
						CSfxPart* pPart;
						SFXPARTTYPE nType;
						file.Read(&nType, sizeof(SFXPARTTYPE));
						pPart = AddPart(nType);
						if (nType == SFXPARTTYPE_PARTICLE)
							pPart->Load3(file);
						else
							pPart->Load2(file);
					}
				}
				else
				{

					file.Seek(0, SEEK_SET);
					int nPart;
					file.Read(&nPart, sizeof(int));
					for (i = 0; i < nPart; i++)
					{
						CSfxPart* pPart;
						SFXPARTTYPE nType;
						file.Read(&nType, sizeof(SFXPARTTYPE));
						pPart = AddPart(nType);
						pPart->OldLoad(file);
					}
				}
	}
	else
	{
		return FALSE;
	}
	file.Close();
	return TRUE;
}

BOOL CSfxBase::Load(void)
{
	for (int i = 0; i < m_apParts.GetSize(); i++)
		SAFE_DELETE_CONST((CSfxPart*)m_apParts.GetAt(i));
	m_apParts.RemoveAll();
	CString strFilename = _T(MakePath(DIR_SFX, LPCTSTR(m_strName + ".sfx")));
	CResFile file;
	if (file.Open(strFilename, "rb"))
	{
		char strTemp[9];
		file.Read(strTemp, 8);
		strTemp[8] = 0;
		CString strTemp2(strTemp);
		if (strTemp2.Left(6) == "SFX0.1") {
			int nPart;
			file.Read(&nPart, sizeof(int));
			for (int i = 0; i < nPart; i++)
			{
				CSfxPart* pPart;
				SFXPARTTYPE nType;
				file.Read(&nType, sizeof(SFXPARTTYPE));
				pPart = AddPart(nType);
				pPart->Load(file);
			}
		}
		else
			if (strTemp2.Left(6) == "SFX0.2") {

				int nPart;
				file.Read(&nPart, sizeof(int));
				for (int i = 0; i < nPart; i++)
				{
					CSfxPart* pPart;
					SFXPARTTYPE nType;
					file.Read(&nType, sizeof(SFXPARTTYPE));
					pPart = AddPart(nType);
					pPart->Load2(file);
				}
			}
			else
				if (strTemp2.Left(6) == "SFX0.3") {

					int nPart;
					file.Read(&nPart, sizeof(int));
					for (int i = 0; i < nPart; i++)
					{
						CSfxPart* pPart;
						SFXPARTTYPE nType;
						file.Read(&nType, sizeof(SFXPARTTYPE));
						pPart = AddPart(nType);
						if (nType == SFXPARTTYPE_PARTICLE)
							pPart->Load3(file);
						else
							pPart->Load2(file);
					}
				}
				else
				{

					file.Seek(0, SEEK_SET);
					int nPart;
					file.Read(&nPart, sizeof(int));
					for (int i = 0; i < nPart; i++)
					{
						CSfxPart* pPart;
						SFXPARTTYPE nType;
						file.Read(&nType, sizeof(SFXPARTTYPE));
						pPart = AddPart(nType);
						pPart->OldLoad(file);
					}
				}
	}
	else
	{
		LPCTSTR szErr = Error("CSfxBase::Load %s ã�� �� ����", strFilename);
		//ADDERRORMSG( szErr );
		return FALSE;
	}
	file.Close();
	return TRUE;
}


CSfxMng::~CSfxMng()
{
	for (int i = 0; i < m_apSfxBase.GetSize(); i++)
		SAFE_DELETE_CONST((CSfxBase*)m_apSfxBase.GetAt(i));
}
void CSfxMng::AddSfxBase(CSfxBase* pSfxBase)
{
	m_apSfxBase.Add(pSfxBase);
}
CSfxBase* CSfxMng::GetSfxBase(BYTE nIndex)
{
	return (CSfxBase*)(m_apSfxBase[nIndex]);
}
CSfxBase* CSfxMng::GetSfxBase(CString strSfxName)
{
	for (int i = 0; i < m_apSfxBase.GetSize(); i++)
	{
		if (GetSfxBase(i)->m_strName == strSfxName)
			return GetSfxBase(i);
	}

	CSfxBase* pSfxBase = new CSfxBase;
	pSfxBase->m_strName = strSfxName;
	if (pSfxBase->Load())
	{
		AddSfxBase(pSfxBase);
		return pSfxBase;
	}
	else
	{
		SAFE_DELETE(pSfxBase);
		return NULL;
	}
}
HRESULT CSfxMng::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;
	return S_OK;
}
HRESULT CSfxMng::RestoreDeviceObjects()
{
	m_pd3dDevice->CreateVertexBuffer(sizeof(D3DSFXVERTEX) * 4, D3DUSAGE_WRITEONLY, D3DFVF_D3DSFXVERTEX, D3DPOOL_SYSTEMMEM, &m_pSfxVB, NULL);
	D3DSFXVERTEX* pVertices;
	m_pSfxVB->Lock(0, sizeof(D3DSFXVERTEX) * 4, (void**)&pVertices, 0);
	pVertices->p = D3DXVECTOR3(-m_fScale, m_fScale, .0f);
	pVertices->tu1 = .0f;
	pVertices->tv1 = .0f;
	pVertices++;
	pVertices->p = D3DXVECTOR3(m_fScale, m_fScale, .0f);
	pVertices->tu1 = 1.0f;
	pVertices->tv1 = .0f;
	pVertices++;
	pVertices->p = D3DXVECTOR3(m_fScale, -m_fScale, .0f);
	pVertices->tu1 = 1.0f;
	pVertices->tv1 = 1.0f;
	pVertices++;
	pVertices->p = D3DXVECTOR3(-m_fScale, -m_fScale, .0f);
	pVertices->tu1 = .0f;
	pVertices->tv1 = 1.0f;
	pVertices++;

	m_pSfxVB->Unlock();
	return S_OK;
}
HRESULT CSfxMng::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pSfxVB);
	return S_OK;
}
HRESULT CSfxMng::DeleteDeviceObjects()
{
	return S_OK;
}

#ifdef __CLIENT
#ifndef __VM_0820
MemPooler<CSfxModel>* CSfxModel::m_pPool = new MemPooler<CSfxModel>(256);
#endif	// __VM_0820
#endif	// __CLIENT
CSfxModel::CSfxModel()
{
	m_vPos = D3DXVECTOR3(0, 0, 0);
	m_vRotate = D3DXVECTOR3(0, 0, 0);
	m_vScale = D3DXVECTOR3(1, 1, 1);
	m_nCurFrame = 0;
	m_pSfxBase = NULL;
}
CSfxModel::~CSfxModel()
{
	DeleteAll();
}

void CSfxModel::DeleteAll(void)
{
	int i, j;
	CPtrArray* pParticles;
	for (i = 0; i < m_apParticles.GetSize(); i++)
	{
		pParticles = (CPtrArray*)m_apParticles[i];
		if (pParticles)
		{
			for (j = 0; j < pParticles->GetSize(); j++)
			{
				SAFE_DELETE_CONST((Particle*)pParticles->GetAt(j));
			}
		}
		SAFE_DELETE(pParticles);
	}
	m_apParticles.RemoveAll();
}
void CSfxModel::RemovePart(int nIndex)
{
	CSfxPart* pPart = m_pSfxBase->Part(nIndex);
	pPart->DeleteAllKeyFrame();
	m_pSfxBase->DeletePart(nIndex);
	if (m_apParticles.GetSize())
	{
		CPtrArray* pParticles = (CPtrArray*)m_apParticles.GetAt(nIndex);
		if (pParticles)
		{
			for (int i = 0; i < pParticles->GetSize(); i++)
			{
				SAFE_DELETE_CONST((Particle*)pParticles->GetAt(i));
			}
		}
		SAFE_DELETE(pParticles);
		m_apParticles.RemoveAt(nIndex);
	}
}
void CSfxModel::SetSfx(CSfxBase* pSfxBase)
{
	m_vPos = D3DXVECTOR3(0, 0, 0);
	m_vRotate = D3DXVECTOR3(0, 0, 0);
	m_vScale = D3DXVECTOR3(1, 1, 1);
	m_nCurFrame = 0;
	m_pSfxBase = NULL;

	m_pSfxBase = pSfxBase;
	DeleteAll();
	for (int i = 0; i < pSfxBase->m_apParts.GetSize(); i++)
	{
		if (pSfxBase->Part(i)->m_nType == SFXPARTTYPE_PARTICLE)
		{
			CPtrArray* pParticles = new CPtrArray;
			m_apParticles.Add(pParticles);
		}
		else
		{
			m_apParticles.Add(NULL);
		}
	}

	m_vMin = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	m_vMax = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_BB.m_vPos[0] = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
	m_BB.m_vPos[1] = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	m_BB.m_vPos[2] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_BB.m_vPos[3] = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	m_BB.m_vPos[4] = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	m_BB.m_vPos[5] = D3DXVECTOR3(1.0f, 0.0f, -1.0f);
	m_BB.m_vPos[6] = D3DXVECTOR3(1.0f, 0.0f, 1.0f);
	m_BB.m_vPos[7] = D3DXVECTOR3(-1.0f, 0.0f, 1.0f);
}
void CSfxModel::SetSfx(LPCTSTR szSfxName)
{
	m_pSfxBase = g_SfxMng.GetSfxBase(szSfxName);
	if (m_pSfxBase)
	{
		DeleteAll();
		for (int i = 0; i < m_pSfxBase->m_apParts.GetSize(); i++)
		{
			if (m_pSfxBase->Part(i)->m_nType == SFXPARTTYPE_PARTICLE)
			{
				CPtrArray* pParticles = new CPtrArray;
				m_apParticles.Add(pParticles);
			}
			else
			{
				m_apParticles.Add(NULL);
			}
		}
	}

	m_vMin = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	m_vMax = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_BB.m_vPos[0] = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
	m_BB.m_vPos[1] = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	m_BB.m_vPos[2] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_BB.m_vPos[3] = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	m_BB.m_vPos[4] = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	m_BB.m_vPos[5] = D3DXVECTOR3(1.0f, 0.0f, -1.0f);
	m_BB.m_vPos[6] = D3DXVECTOR3(1.0f, 0.0f, 1.0f);
	m_BB.m_vPos[7] = D3DXVECTOR3(-1.0f, 0.0f, 1.0f);
}

void CSfxModel::SetSfx(DWORD dwIndex)
{
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem(OT_SFX, dwIndex);

	if (lpModelElem)
	{
		m_pSfxBase = g_SfxMng.GetSfxBase(lpModelElem->m_szName);

		if (m_pSfxBase)
		{
			DeleteAll();
			for (int i = 0; i < m_pSfxBase->m_apParts.GetSize(); i++)
			{
				if (m_pSfxBase->Part(i)->m_nType == SFXPARTTYPE_PARTICLE)
				{
					CPtrArray* pParticles = new CPtrArray;
					m_apParticles.Add(pParticles);
				}
				else
				{
					m_apParticles.Add(NULL);
				}
			}
		}
	}

	m_vMin = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	m_vMax = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_BB.m_vPos[0] = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
	m_BB.m_vPos[1] = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	m_BB.m_vPos[2] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_BB.m_vPos[3] = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	m_BB.m_vPos[4] = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	m_BB.m_vPos[5] = D3DXVECTOR3(1.0f, 0.0f, -1.0f);
	m_BB.m_vPos[6] = D3DXVECTOR3(1.0f, 0.0f, 1.0f);
	m_BB.m_vPos[7] = D3DXVECTOR3(-1.0f, 0.0f, 1.0f);
}

BOOL CSfxModel::Render2(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld)
{
#ifdef __CLIENT
	if (m_pSfxBase)
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		CSfxMng::m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		for (int i = 0; i < m_pSfxBase->m_apParts.GetSize(); i++)
		{
			if (m_pSfxBase->Part(i)->m_bUseing == FALSE)
				continue;

			switch (m_pSfxBase->Part(i)->m_nType)
			{
			case SFXPARTTYPE_BILL:
			{
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

				CSfxMng::m_pd3dDevice->SetVertexShader(NULL);
				CSfxMng::m_pd3dDevice->SetVertexDeclaration(NULL);
				CSfxMng::m_pd3dDevice->SetFVF(D3DFVF_D3DSFXVERTEX);
				CSfxMng::m_pd3dDevice->SetStreamSource(0, CSfxMng::m_pSfxVB, 0, sizeof(D3DSFXVERTEX));

				m_pSfxBase->Part(i)->Render2(m_vPos, m_nCurFrame, m_vRotate, m_vScale);

				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			}
			break;
			case SFXPARTTYPE_PARTICLE:
			{
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				if (i < m_apParticles.GetSize())
				{
					CSfxMng::m_pd3dDevice->SetVertexShader(NULL);
					CSfxMng::m_pd3dDevice->SetVertexDeclaration(NULL);
					CSfxMng::m_pd3dDevice->SetFVF(D3DFVF_D3DSFXVERTEX);
					CSfxMng::m_pd3dDevice->SetStreamSource(0, CSfxMng::m_pSfxVB, 0, sizeof(D3DSFXVERTEX));
					RenderParticles2(m_vPos, m_nCurFrame, m_vRotate, (CSfxPartParticle*)(m_pSfxBase->Part(i)), (CPtrArray*)(m_apParticles[i]), m_vScale);
				}
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			}
			break;
			case SFXPARTTYPE_MESH:
			{
				m_pSfxBase->Part(i)->Render(m_vPos, m_nCurFrame, m_vRotate.y, m_vScale);
			}
			break;
			case SFXPARTTYPE_CUSTOMMESH:
			{
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				CSfxMng::m_pd3dDevice->SetVertexShader(NULL);
				CSfxMng::m_pd3dDevice->SetVertexDeclaration(NULL);
				CSfxMng::m_pd3dDevice->SetFVF(D3DFVF_D3DSFXVERTEX);
				m_pSfxBase->Part(i)->Render2(m_vPos, m_nCurFrame, m_vRotate, m_vScale);
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			}
			break;
			}
		}
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
#endif //__CLIENT
	return TRUE;
}
void CSfxModel::RenderParticles2(D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, CSfxPartParticle* pPartParticle, CPtrArray* pParticles, D3DXVECTOR3 vScale)
{
#ifdef __CLIENT
	CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(pPartParticle->m_strTex));
	SfxKeyFrame* pPrevKey = pPartParticle->GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey = pPartParticle->GetNextKey(nFrame);
	if (pPrevKey == NULL || pNextKey == NULL) return;
	SfxKeyFrame Key;
	int dFrame = pNextKey->nFrame - pPrevKey->nFrame;
	Key = *(pPrevKey);
	if (dFrame != 0)
	{
		Key.vPos += (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vPosRotate += (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vRotate += (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);

		if (!pPartParticle->m_bRepeatScal)
		{
			Key.vScale += (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		}


		Key.nAlpha += (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp, matTemp1, matTemp2;
	D3DXMATRIX matTrans, matAngle, matScale, matInv;
	D3DXVECTOR3 vTemp;
	switch (pPartParticle->m_nBillType)
	{
	case SFXPARTBILLTYPE_BILL:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		matTemp = matAngle * g_matInvView;
		break;
	}
	case SFXPARTBILLTYPE_BOTTOM:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		D3DXMatrixRotationX(&matTemp2, DEGREETORADIAN(90));
		matTemp = matAngle * matTemp2;
		break;
	}
	case SFXPARTBILLTYPE_POLE:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		matTemp = matAngle * g_matInvView;
		break;
	}
	case SFXPARTBILLTYPE_NORMAL:
	{
		D3DXMATRIX mRot;
		D3DXVECTOR3 vRot = DEGREETORADIAN(fAngle);
		D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vRotate);
		D3DXMatrixRotationYawPitchRoll(&matAngle, vTemp2.y, vTemp2.x, vTemp2.z);
		D3DXMatrixRotationYawPitchRoll(&mRot, vRot.y, vRot.x, vRot.z);
		matTemp = matAngle * mRot;
		break;
	}
	}
	switch (pPartParticle->m_nAlphaType)
	{
	case SFXPARTALPHATYPE_BLEND:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	}
	case SFXPARTALPHATYPE_GLOW:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	}
	}

	D3DXMATRIX  mRot;
	D3DXVECTOR3 vAngle = DEGREETORADIAN(fAngle);
	D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vPosRotate);

	D3DXMatrixRotationYawPitchRoll(&mRot, vAngle.y, vAngle.x, vAngle.z);
	D3DXMatrixRotationYawPitchRoll(&matTemp2, vTemp2.y, vTemp2.x, vTemp2.z);
	matTemp2 = mRot * matTemp2;
	D3DXVec3TransformCoord(&vTemp, &(Key.vPos), &matTemp2);


	D3DXVECTOR3 vTemp3;
	Particle* pParticle = NULL;
	CString		TexName;

	for (int i = 0; i < pParticles->GetSize(); i++)
	{
		pParticle = (Particle*)(pParticles->GetAt(i));
#ifdef __SFX_OPT
		if (pPartParticle->m_nTexFrame > 1)
		{
			int nTexFrame = (pPartParticle->m_nTexFrame * (pParticle->nFrame) / pPartParticle->m_nTexLoop) % pPartParticle->m_nTexFrame;
			CString tempTexName = GetTextureName(pPartParticle->m_strTex, nTexFrame);
			if (TexName != tempTexName)
			{
				CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(tempTexName));
				TexName = tempTexName;
			}
		}
		else
		{
			if (TexName != pPartParticle->m_strTex)
			{
				CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(pPartParticle->m_strTex));
				TexName = pPartParticle->m_strTex;
			}
		}
#else
		if (pPartParticle->m_nTexFrame > 1)
		{
			int nTexFrame = (pPartParticle->m_nTexFrame * (pParticle->nFrame) / pPartParticle->m_nTexLoop) % pPartParticle->m_nTexFrame;
			CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(GetTextureName(pPartParticle->m_strTex, nTexFrame)));
		}
		else
			CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(pPartParticle->m_strTex));
#endif
		if (pPartParticle->m_bRepeatScal)
		{

			if (D3DXVec3Length(&pParticle->vScale) >= D3DXVec3Length(&pParticle->vScaleEnd))
				pParticle->bSwScal = TRUE;
			else
				if (D3DXVec3Length(&pParticle->vScale) <= D3DXVec3Length(&pParticle->vScaleStart))
					pParticle->bSwScal = FALSE;

			if (pParticle->bSwScal)
				pParticle->vScale -= pParticle->vScaleSpeed;
			else
				pParticle->vScale += pParticle->vScaleSpeed;
		}

		vTemp3 = DEGREETORADIAN(Key.vRotate + Key.vPosRotate);
		D3DXMatrixRotationYawPitchRoll(&matTemp2, vTemp3.y, vTemp3.x, vTemp3.z);
		matTemp2 *= mRot;
		D3DXVec3TransformCoord(&vTemp3, &(pParticle->vPos), &matTemp2);

		D3DXMatrixTranslation(&matTrans,
			vTemp.x + (vTemp3.x * m_vScale.x) + vPos.x,
			vTemp.y + (vTemp3.y * m_vScale.y) + vPos.y,
			vTemp.z + (vTemp3.z * m_vScale.z) + vPos.z);

		D3DXMatrixScaling(&matScale, pParticle->vScale.x * m_vScale.x, pParticle->vScale.y * m_vScale.y, 1.0f * m_vScale.z);

		D3DXMATRIX matRot;
		D3DXVECTOR3 v3Rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		if (dFrame != 0)
		{
			FLOAT Lerp = (FLOAT)(nFrame - pPrevKey->nFrame) / dFrame;
			D3DXVec3Lerp(&v3Rot, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &pParticle->vRotation, Lerp);
		}

		D3DXMatrixRotationYawPitchRoll(&matRot, DEGREETORADIAN(v3Rot.y), DEGREETORADIAN(v3Rot.x), DEGREETORADIAN(v3Rot.z));

		matTemp1 = matRot * matScale * matTemp * matTrans;

		CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD, &matTemp1);


		int nTempAlpha = 0;
		if (pParticle->nFrame < pPartParticle->m_nParticleFrameAppear)
		{
			nTempAlpha = pParticle->nFrame * Key.nAlpha / pPartParticle->m_nParticleFrameAppear;
		}
		else
			if (pParticle->nFrame > pPartParticle->m_nParticleFrameKeep)
			{
				nTempAlpha = Key.nAlpha - (Key.nAlpha * (pParticle->nFrame - pPartParticle->m_nParticleFrameKeep) / (pPartParticle->m_nParticleFrameDisappear - pPartParticle->m_nParticleFrameKeep));
			}
			else
			{
				nTempAlpha = Key.nAlpha;
			}
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, nTempAlpha << 24 | 0x404040);

		CSfxMng::m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	}
#endif //__CLIENT
}

#ifndef __WORLDSERVER
BOOL CSfxModel::RenderZ(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld)
{
	if (m_pSfxBase)
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		CSfxMng::m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		for (int i = 0; i < m_pSfxBase->m_apParts.GetSize(); i++)
		{
			if (m_pSfxBase->Part(i)->m_bUseing == FALSE)
				continue;

			switch (m_pSfxBase->Part(i)->m_nType)
			{
			case SFXPARTTYPE_BILL:
			{
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				CSfxMng::m_pd3dDevice->SetVertexShader(NULL);
				CSfxMng::m_pd3dDevice->SetVertexDeclaration(NULL);
				CSfxMng::m_pd3dDevice->SetFVF(D3DFVF_D3DSFXVERTEX);
				CSfxMng::m_pd3dDevice->SetStreamSource(0, CSfxMng::m_pSfxVB, 0, sizeof(D3DSFXVERTEX));
				m_pSfxBase->Part(i)->Render(m_vPos, m_nCurFrame, m_vRotate.y, m_vScale);
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			}
			break;
			case SFXPARTTYPE_PARTICLE:
			{
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

				if (i < m_apParticles.GetSize())
				{
					CSfxMng::m_pd3dDevice->SetVertexShader(NULL);
					CSfxMng::m_pd3dDevice->SetVertexDeclaration(NULL);
					CSfxMng::m_pd3dDevice->SetFVF(D3DFVF_D3DSFXVERTEX);
					CSfxMng::m_pd3dDevice->SetStreamSource(0, CSfxMng::m_pSfxVB, 0, sizeof(D3DSFXVERTEX));
					RenderParticles(m_vPos, m_nCurFrame, m_vRotate.y, (CSfxPartParticle*)(m_pSfxBase->Part(i)), (CPtrArray*)(m_apParticles[i]), m_vScale);
				}

				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

			}
			break;
			case SFXPARTTYPE_MESH:
			{
				m_pSfxBase->Part(i)->Render(m_vPos, m_nCurFrame, m_vRotate.y, m_vScale);
			}
			break;
			case SFXPARTTYPE_CUSTOMMESH:
			{
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
				CSfxMng::m_pd3dDevice->SetFVF(D3DFVF_D3DSFXVERTEX);
				m_pSfxBase->Part(i)->Render(m_vPos, m_nCurFrame, m_vRotate.y, m_vScale);

			}
			break;
			}
		}
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	return TRUE;
}

BOOL CSfxModel::Render(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld)
{
	if (m_pSfxBase)
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		CSfxMng::m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		CSfxMng::m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

		for (int i = 0; i < m_pSfxBase->m_apParts.GetSize(); i++)
		{
			if (m_pSfxBase->Part(i)->m_bUseing == FALSE)
				continue;

			switch (m_pSfxBase->Part(i)->m_nType)
			{
			case SFXPARTTYPE_BILL:
			{
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				CSfxMng::m_pd3dDevice->SetVertexShader(NULL);
				CSfxMng::m_pd3dDevice->SetVertexDeclaration(NULL);
				CSfxMng::m_pd3dDevice->SetFVF(D3DFVF_D3DSFXVERTEX);
				CSfxMng::m_pd3dDevice->SetStreamSource(0, CSfxMng::m_pSfxVB, 0, sizeof(D3DSFXVERTEX));
				m_pSfxBase->Part(i)->Render(m_vPos, m_nCurFrame, m_vRotate.y, m_vScale);
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			}
			break;
			case SFXPARTTYPE_PARTICLE:
			{



				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

				if (i < m_apParticles.GetSize())
				{
					CSfxMng::m_pd3dDevice->SetVertexShader(NULL);
					CSfxMng::m_pd3dDevice->SetVertexDeclaration(NULL);
					CSfxMng::m_pd3dDevice->SetFVF(D3DFVF_D3DSFXVERTEX);
					CSfxMng::m_pd3dDevice->SetStreamSource(0, CSfxMng::m_pSfxVB, 0, sizeof(D3DSFXVERTEX));
					RenderParticles(m_vPos, m_nCurFrame, m_vRotate.y, (CSfxPartParticle*)(m_pSfxBase->Part(i)), (CPtrArray*)(m_apParticles[i]), m_vScale);
				}

				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

			}
			break;
			case SFXPARTTYPE_MESH:
			{
				m_pSfxBase->Part(i)->Render(m_vPos, m_nCurFrame, m_vRotate.y, m_vScale);
			}
			break;
			case SFXPARTTYPE_CUSTOMMESH:
			{
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				CSfxMng::m_pd3dDevice->SetFVF(D3DFVF_D3DSFXVERTEX);
				m_pSfxBase->Part(i)->Render(m_vPos, m_nCurFrame, m_vRotate.y, m_vScale);
				CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			}
			break;
			}
		}
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	}
	return TRUE;
}

void CSfxModel::RenderParticles(D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, CSfxPartParticle* pPartParticle, CPtrArray* pParticles, D3DXVECTOR3 vScale)
{
	SfxKeyFrame* pPrevKey = pPartParticle->GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey = pPartParticle->GetNextKey(nFrame);
	if (pPrevKey == NULL || pNextKey == NULL) return;
	SfxKeyFrame Key;
	int dFrame = pNextKey->nFrame - pPrevKey->nFrame;
	Key = *(pPrevKey);
	if (dFrame != 0)
	{
		Key.vPos += (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vPosRotate += (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		Key.vRotate += (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);

		if (!pPartParticle->m_bRepeatScal)
		{
			Key.vScale += (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)((nFrame - pPrevKey->nFrame)) / (FLOAT)(dFrame);
		}
		Key.nAlpha += (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp, matTemp1, matTemp2;
	D3DXMATRIX matTrans, matAngle, matScale, matInv;
	D3DXVECTOR3 vTemp;
	switch (pPartParticle->m_nBillType)
	{
	case SFXPARTBILLTYPE_BILL:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		matTemp = matAngle * g_matInvView;
		break;
	}
	case SFXPARTBILLTYPE_BOTTOM:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z - fAngle));
		D3DXMatrixRotationX(&matTemp2, DEGREETORADIAN(90));
		matTemp = matAngle * matTemp2;
		break;
	}
	case SFXPARTBILLTYPE_POLE:
	{
		D3DXMatrixRotationZ(&matAngle, DEGREETORADIAN(Key.vRotate.z));
		matTemp2 = g_matView;	matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
		matTemp = matAngle * g_matInvView;
		break;
	}
	case SFXPARTBILLTYPE_NORMAL:
	{
		D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vRotate + D3DXVECTOR3(.0f, fAngle, .0f));
		D3DXMatrixRotationYawPitchRoll(&matAngle, vTemp2.y, vTemp2.x, vTemp2.z);
		matTemp = matAngle;
		break;
	}
	}
	switch (pPartParticle->m_nAlphaType)
	{
	case SFXPARTALPHATYPE_BLEND:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	}
	case SFXPARTALPHATYPE_GLOW:
	{
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	}
	}

	D3DXVECTOR3 vTemp2 = DEGREETORADIAN(Key.vPosRotate + D3DXVECTOR3(.0f, fAngle, .0f));
	D3DXMatrixRotationYawPitchRoll(&matTemp2, vTemp2.y, vTemp2.x, vTemp2.z);
	D3DXVec3TransformCoord(&vTemp, &(Key.vPos), &matTemp2);

	if (pParticles == NULL)
		return;


	D3DXVECTOR3 vTemp3;
	Particle* pParticle = NULL;
	CString TexName;

	for (int i = 0; i < pParticles->GetSize(); i++)
	{
		pParticle = (Particle*)(pParticles->GetAt(i));
#ifdef __SFX_OPT
		if (pPartParticle->m_nTexFrame > 1)
		{
			int nTexFrame = (pPartParticle->m_nTexFrame * (pParticle->nFrame) / pPartParticle->m_nTexLoop) % pPartParticle->m_nTexFrame;
			CString tempTexName = GetTextureName(pPartParticle->m_strTex, nTexFrame);
			if (TexName != tempTexName)
			{
				CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(tempTexName));
				TexName = tempTexName;
			}
		}
		else
		{
			if (TexName != pPartParticle->m_strTex)
			{
				CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(pPartParticle->m_strTex));
				TexName = pPartParticle->m_strTex;
			}
		}
#else
		if (pPartParticle->m_nTexFrame > 1)
		{
			int nTexFrame = (pPartParticle->m_nTexFrame * (pParticle->nFrame) / pPartParticle->m_nTexLoop) % pPartParticle->m_nTexFrame;
			CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(GetTextureName(pPartParticle->m_strTex, nTexFrame)));
		}
		else
			CSfxMng::m_pd3dDevice->SetTexture(0, g_SfxTex.Tex(pPartParticle->m_strTex));
#endif

		if (pPartParticle->m_bRepeatScal)
		{

			if (D3DXVec3Length(&pParticle->vScale) >= D3DXVec3Length(&pParticle->vScaleEnd))
				pParticle->bSwScal = TRUE;
			else
				if (D3DXVec3Length(&pParticle->vScale) <= D3DXVec3Length(&pParticle->vScaleStart))
					pParticle->bSwScal = FALSE;

			if (pParticle->bSwScal)
				pParticle->vScale -= pParticle->vScaleSpeed;
			else
				pParticle->vScale += pParticle->vScaleSpeed;
		}

		vTemp3 = DEGREETORADIAN(Key.vRotate + Key.vPosRotate + D3DXVECTOR3(.0f, fAngle, .0f));
		D3DXMatrixRotationYawPitchRoll(&matTemp2, vTemp3.y, vTemp3.x, vTemp3.z);
		D3DXVec3TransformCoord(&vTemp3, &(pParticle->vPos), &matTemp2);

		D3DXMatrixTranslation(&matTrans,
			vTemp.x + (vTemp3.x * m_vScale.x) + vPos.x,
			vTemp.y + (vTemp3.y * m_vScale.y) + vPos.y,
			vTemp.z + (vTemp3.z * m_vScale.z) + vPos.z);

		D3DXMatrixScaling(&matScale, pParticle->vScale.x * m_vScale.x, pParticle->vScale.y * m_vScale.y, 1.0f * m_vScale.z);
		D3DXMATRIX matRot;
		D3DXVECTOR3 v3Rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		if (dFrame != 0)
		{
			FLOAT Lerp = (FLOAT)(nFrame - pPrevKey->nFrame) / dFrame;
			D3DXVec3Lerp(&v3Rot, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &pParticle->vRotation, Lerp);
		}

		D3DXMatrixRotationYawPitchRoll(&matRot, DEGREETORADIAN(v3Rot.y), DEGREETORADIAN(v3Rot.x), DEGREETORADIAN(v3Rot.z));

		matTemp1 = matRot * matScale * matTemp * matTrans;

		CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD, &matTemp1);


		int nTempAlpha = 0;
		if (pParticle->nFrame < pPartParticle->m_nParticleFrameAppear)
		{
			nTempAlpha = pParticle->nFrame * Key.nAlpha / pPartParticle->m_nParticleFrameAppear;
		}
		else
			if (pParticle->nFrame > pPartParticle->m_nParticleFrameKeep)
			{
				nTempAlpha = Key.nAlpha - (Key.nAlpha * (pParticle->nFrame - pPartParticle->m_nParticleFrameKeep) / (pPartParticle->m_nParticleFrameDisappear - pPartParticle->m_nParticleFrameKeep));
			}
			else
			{
				nTempAlpha = Key.nAlpha;
			}
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, nTempAlpha << 24 | 0x404040);

		CSfxMng::m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	}
}
#endif
BOOL CSfxModel::Process(void)
{
	BOOL ret = TRUE;
	CSfxPartParticle* pPartParticle;
	Particle* pParticle;

	m_nCurFrame++;

	for (int i = 0; i < m_apParticles.GetSize(); i++)
	{
		CPtrArray* pParticles = (CPtrArray*)m_apParticles[i];
		if (pParticles)
		{
			pPartParticle = (CSfxPartParticle*)(m_pSfxBase->Part(i));

			if (pPartParticle->m_bUseing == FALSE)
				continue;

			SfxKeyFrame Key;
			pPartParticle->GetKey(m_nCurFrame, &Key);

			int j;
			for (j = 0; j < pParticles->GetSize(); j++)
			{
				pParticle = (Particle*)(pParticles->GetAt(j));
				pParticle->vPos += pParticle->vSpeed;
				pParticle->vSpeed += pPartParticle->m_vParticleAccel;

				if (!pPartParticle->m_bRepeatScal)
					pParticle->vScale += pPartParticle->m_vScaleSpeed;

				pParticle->nFrame++;

				if (pParticle->nFrame >= pPartParticle->m_nParticleFrameDisappear)
				{
					SAFE_DELETE(pParticle);
					pParticles->RemoveAt(j);
					j--;
				}
			}
			int nStartFrame = 0;
			int nEndFrame = 0;
			if (pPartParticle->Key(0))
			{
				nStartFrame = pPartParticle->Key(0)->nFrame;
				nEndFrame = pPartParticle->Key(pPartParticle->m_apKeyFrames.GetSize() - 1)->nFrame;
			}
			if (m_nCurFrame >= nStartFrame && m_nCurFrame <= nEndFrame - pPartParticle->m_nParticleFrameDisappear)
			{
#ifdef __SFX_OPT
				float fRand1 = RANDF;
				float fRand2 = RANDF;
				float fRand3 = RANDF;

				if (pPartParticle->m_nParticleCreate == 0)
				{
					if (m_nCurFrame == nStartFrame)
					{
						for (int i = 0; i < pPartParticle->m_nParticleCreateNum; i++)
						{
							pParticle = new Particle;
							pParticle->nFrame = 0;
							float fAngle = RANDF * 360.0f;
							pParticle->vPos =
								D3DXVECTOR3(
									sin(fAngle) * pPartParticle->m_fParticleStartPosVar,
									fRand1 * pPartParticle->m_fParticleStartPosVarY,
									cos(fAngle) * pPartParticle->m_fParticleStartPosVar
								);
							float fFactor = pPartParticle->m_fParticleXZLow + fRand2 * (pPartParticle->m_fParticleXZHigh - pPartParticle->m_fParticleXZLow);
							pParticle->vSpeed =
								D3DXVECTOR3(
									sin(fAngle) * fFactor,
									pPartParticle->m_fParticleYLow + fRand2 * (pPartParticle->m_fParticleYHigh - pPartParticle->m_fParticleYLow),
									cos(fAngle) * fFactor
								);
							pParticle->vScaleStart = pParticle->vScale = pPartParticle->m_vScale;
							//#ifdef __ATEST
							pParticle->vRotation = D3DXVECTOR3(pPartParticle->m_vRotationLow.x + fRand1 *
								(pPartParticle->m_vRotationHigh.x - pPartParticle->m_vRotationLow.x),
								pPartParticle->m_vRotationLow.y + fRand3 *
								(pPartParticle->m_vRotationHigh.y - pPartParticle->m_vRotationLow.y),
								pPartParticle->m_vRotationLow.z + fRand2 *
								(pPartParticle->m_vRotationHigh.z - pPartParticle->m_vRotationLow.z));

							pParticle->bSwScal = FALSE;
							pParticle->vScaleEnd = pPartParticle->m_vScaleEnd;
							pParticle->vScaleSpeed = D3DXVECTOR3(pPartParticle->m_fScalSpeedXLow + fRand3 *
								(pPartParticle->m_fScalSpeedXHigh - pPartParticle->m_fScalSpeedXLow),
								pPartParticle->m_fScalSpeedYLow + fRand2 *
								(pPartParticle->m_fScalSpeedYHigh - pPartParticle->m_fScalSpeedYLow),
								pPartParticle->m_fScalSpeedZLow + fRand1 *
								(pPartParticle->m_fScalSpeedZHigh - pPartParticle->m_fScalSpeedZLow));
							//#endif
							pParticles->Add(pParticle);
						}
					}
				}
				else
				{
					if (((m_nCurFrame)-nStartFrame) % pPartParticle->m_nParticleCreate == 0)
					{
						for (int i = 0; i < pPartParticle->m_nParticleCreateNum; i++)
						{
							pParticle = new Particle;
							pParticle->nFrame = 0;
							float fAngle = RANDF * 360.0f;
							pParticle->vPos =
								D3DXVECTOR3(
									sin(fAngle) * pPartParticle->m_fParticleStartPosVar,
									fRand1 * pPartParticle->m_fParticleStartPosVarY,
									cos(fAngle) * pPartParticle->m_fParticleStartPosVar
								);
							float fFactor = pPartParticle->m_fParticleXZLow + fRand2 * (pPartParticle->m_fParticleXZHigh - pPartParticle->m_fParticleXZLow);
							pParticle->vSpeed =
								D3DXVECTOR3(
									sin(fAngle) * fFactor,
									pPartParticle->m_fParticleYLow + fRand2 * (pPartParticle->m_fParticleYHigh - pPartParticle->m_fParticleYLow),
									cos(fAngle) * fFactor
								);
							pParticle->vScaleStart = pParticle->vScale = pPartParticle->m_vScale;
							//#ifdef __ATEST
							pParticle->vRotation = D3DXVECTOR3(pPartParticle->m_vRotationLow.x + fRand1 *
								(pPartParticle->m_vRotationHigh.x - pPartParticle->m_vRotationLow.x),
								pPartParticle->m_vRotationLow.y + fRand3 *
								(pPartParticle->m_vRotationHigh.y - pPartParticle->m_vRotationLow.y),
								pPartParticle->m_vRotationLow.z + fRand2 *
								(pPartParticle->m_vRotationHigh.z - pPartParticle->m_vRotationLow.z));


							pParticle->bSwScal = FALSE;
							pParticle->vScaleEnd = pPartParticle->m_vScaleEnd;
							pParticle->vScaleSpeed = D3DXVECTOR3(pPartParticle->m_fScalSpeedXLow + fRand3 *
								(pPartParticle->m_fScalSpeedXHigh - pPartParticle->m_fScalSpeedXLow),
								pPartParticle->m_fScalSpeedYLow + fRand2 *
								(pPartParticle->m_fScalSpeedYHigh - pPartParticle->m_fScalSpeedYLow),
								pPartParticle->m_fScalSpeedZLow + fRand1 *
								(pPartParticle->m_fScalSpeedZHigh - pPartParticle->m_fScalSpeedZLow));


							//#endif
							pParticles->Add(pParticle);
						}
					}
					else
						ret = FALSE;
				}
#else
				if (pPartParticle->m_nParticleCreate == 0)
				{
					if (m_nCurFrame == nStartFrame)
					{
						for (int i = 0; i < pPartParticle->m_nParticleCreateNum; i++)
						{
							pParticle = new Particle;
							pParticle->nFrame = 0;
							float fAngle = RANDF * 360.0f;
							pParticle->vPos =
								D3DXVECTOR3(
									sin(fAngle) * pPartParticle->m_fParticleStartPosVar,
									RANDF * pPartParticle->m_fParticleStartPosVarY,
									cos(fAngle) * pPartParticle->m_fParticleStartPosVar
								);
							float fFactor = pPartParticle->m_fParticleXZLow + RANDF * (pPartParticle->m_fParticleXZHigh - pPartParticle->m_fParticleXZLow);
							pParticle->vSpeed =
								D3DXVECTOR3(
									sin(fAngle) * fFactor,
									pPartParticle->m_fParticleYLow + RANDF * (pPartParticle->m_fParticleYHigh - pPartParticle->m_fParticleYLow),
									cos(fAngle) * fFactor
								);
							pParticle->vScaleStart = pParticle->vScale = pPartParticle->m_vScale;
							//#ifdef __ATEST
							pParticle->vRotation = D3DXVECTOR3(pPartParticle->m_vRotationLow.x + RANDF *
								(pPartParticle->m_vRotationHigh.x - pPartParticle->m_vRotationLow.x),
								pPartParticle->m_vRotationLow.y + RANDF *
								(pPartParticle->m_vRotationHigh.y - pPartParticle->m_vRotationLow.y),
								pPartParticle->m_vRotationLow.z + RANDF *
								(pPartParticle->m_vRotationHigh.z - pPartParticle->m_vRotationLow.z));

							pParticle->bSwScal = FALSE;
							pParticle->vScaleEnd = pPartParticle->m_vScaleEnd;
							pParticle->vScaleSpeed = D3DXVECTOR3(pPartParticle->m_fScalSpeedXLow + RANDF *
								(pPartParticle->m_fScalSpeedXHigh - pPartParticle->m_fScalSpeedXLow),
								pPartParticle->m_fScalSpeedYLow + RANDF *
								(pPartParticle->m_fScalSpeedYHigh - pPartParticle->m_fScalSpeedYLow),
								pPartParticle->m_fScalSpeedZLow + RANDF *
								(pPartParticle->m_fScalSpeedZHigh - pPartParticle->m_fScalSpeedZLow));
							//#endif
							pParticles->Add(pParticle);
						}
					}
				}
				else
				{
					if (((m_nCurFrame)-nStartFrame) % pPartParticle->m_nParticleCreate == 0)
					{
						for (int i = 0; i < pPartParticle->m_nParticleCreateNum; i++)
						{
							pParticle = new Particle;
							pParticle->nFrame = 0;
							float fAngle = RANDF * 360.0f;
							pParticle->vPos =
								D3DXVECTOR3(
									sin(fAngle) * pPartParticle->m_fParticleStartPosVar,
									RANDF * pPartParticle->m_fParticleStartPosVarY,
									cos(fAngle) * pPartParticle->m_fParticleStartPosVar
								);
							float fFactor = pPartParticle->m_fParticleXZLow + RANDF * (pPartParticle->m_fParticleXZHigh - pPartParticle->m_fParticleXZLow);
							pParticle->vSpeed =
								D3DXVECTOR3(
									sin(fAngle) * fFactor,
									pPartParticle->m_fParticleYLow + RANDF * (pPartParticle->m_fParticleYHigh - pPartParticle->m_fParticleYLow),
									cos(fAngle) * fFactor
								);
							pParticle->vScaleStart = pParticle->vScale = pPartParticle->m_vScale;
							//#ifdef __ATEST
							pParticle->vRotation = D3DXVECTOR3(pPartParticle->m_vRotationLow.x + RANDF *
								(pPartParticle->m_vRotationHigh.x - pPartParticle->m_vRotationLow.x),
								pPartParticle->m_vRotationLow.y + RANDF *
								(pPartParticle->m_vRotationHigh.y - pPartParticle->m_vRotationLow.y),
								pPartParticle->m_vRotationLow.z + RANDF *
								(pPartParticle->m_vRotationHigh.z - pPartParticle->m_vRotationLow.z));


							pParticle->bSwScal = FALSE;
							pParticle->vScaleEnd = pPartParticle->m_vScaleEnd;
							pParticle->vScaleSpeed = D3DXVECTOR3(pPartParticle->m_fScalSpeedXLow + RANDF *
								(pPartParticle->m_fScalSpeedXHigh - pPartParticle->m_fScalSpeedXLow),
								pPartParticle->m_fScalSpeedYLow + RANDF *
								(pPartParticle->m_fScalSpeedYHigh - pPartParticle->m_fScalSpeedYLow),
								pPartParticle->m_fScalSpeedZLow + RANDF *
								(pPartParticle->m_fScalSpeedZHigh - pPartParticle->m_fScalSpeedZLow));


							//#endif
							pParticles->Add(pParticle);
						}
					}
					else
						ret = FALSE;
				}
#endif
			}
			if (pParticles->GetSize() > 0)
				ret = FALSE;

			if (m_nCurFrame < nStartFrame)
				ret = FALSE;

			if (pPartParticle->m_bRepeat)
			{
				if (nEndFrame >= 0)
				{
					FLOAT f = (FLOAT)m_nCurFrame / (FLOAT)nEndFrame;

					if (f >= 0.65f)
					{
						m_nCurFrame = (int)(nEndFrame * 0.6f);
					}
				}
				else
				{
					Error(" CSfxModel::Process()���� nEndFrame < 0");
				}
			}

		}
		else
		{
			if (m_pSfxBase->Part(i)->GetNextKey(m_nCurFrame))
				ret = FALSE;
		}
	}
	return ret;
}
BOOL CSfxModel::SetFrame(int nFrame)
{
	BOOL ret = TRUE;
	//	CSfxPartParticle* pPartParticle;
	//	Particle* pParticle;

	for (int i = 0; i < m_apParticles.GetSize(); i++)
	{
		CPtrArray* pParticles = (CPtrArray*)m_apParticles[i];

		{
			if (m_pSfxBase->Part(i)->GetNextKey(nFrame))
			{
				ret = FALSE;
				m_nCurFrame = nFrame;
			}

		}
	}

	return ret;
}

CSfxTexture::CSfxTexture()
{
}
CSfxTexture::~CSfxTexture()
{
	DeleteAll();
}

LPDIRECT3DTEXTURE9 CSfxTexture::AddTex(CString str)
{
	LPDIRECT3DTEXTURE9 pTex;
	HRESULT hr = LoadTextureFromRes(CSfxMng::m_pd3dDevice, _T(MakePath(DIR_SFXTEX, LPCTSTR(str))),
		D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_A1R5G5B5,
		D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR,
		D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, 0, NULL, NULL, &pTex);

	if (hr != D3D_OK)
	{
		TRACE("CSfxTexture::AddTex���� �ؽ��� ���� %s", str);
		return NULL;
	}
	m_apTexture[str] = pTex;
	return pTex;
}
void CSfxTexture::DeleteTex(CString str)
{
	LPDIRECT3DTEXTURE9 pTex = Tex(str);
	SAFE_RELEASE(pTex);
}
LPDIRECT3DTEXTURE9 CSfxTexture::Tex(CString str)
{
	if (str.IsEmpty()) return NULL;
	LPDIRECT3DTEXTURE9 pTex = (LPDIRECT3DTEXTURE9)(m_apTexture[str]);
	if (pTex) return pTex;
	return AddTex(str);
}

void CSfxTexture::DeleteAll(void)
{
	POSITION pos;

	CString key;
	LPDIRECT3DTEXTURE9 pTex;

	for (pos = m_apTexture.GetStartPosition(); pos != NULL; ) {
		m_apTexture.GetNextAssoc(pos, key, (void*&)pTex);
		SAFE_RELEASE(pTex);
	}
	m_apTexture.RemoveAll();
}
CSfxMeshMng::CSfxMeshMng()
{
}
CSfxMeshMng::~CSfxMeshMng()
{
	DeleteAll();
}

CModelObject* CSfxMeshMng::AddMesh(CString str)
{
	CModelObject* pMesh = new CModelObject;
	pMesh->InitDeviceObjects(m_pd3dDevice);
#ifndef __SERVER
	int nTex = g_Option.m_nTextureQuality;
	g_Option.m_nTextureQuality = 0;
#endif // not server
	pMesh->LoadModel(str);
	pMesh->RestoreDeviceObjects();
#ifndef __SERVER
	g_Option.m_nTextureQuality = nTex;
#endif // not server

	m_apMesh[str] = pMesh;
	return pMesh;
}
void CSfxMeshMng::DeleteMesh(CString str)
{
	CModelObject* pMesh = Mesh(str);
	if (pMesh)
		SAFE_DELETE(pMesh);
}
CModelObject* CSfxMeshMng::Mesh(CString str)
{
	if (str.IsEmpty()) return NULL;
	CModelObject* pMesh = (CModelObject*)(m_apMesh[str]);
	if (pMesh) return pMesh;
	return AddMesh(str);
}

void CSfxMeshMng::DeleteAll(void)
{
	POSITION pos;
	CString key;
	CModelObject* pMesh;
	for (pos = m_apMesh.GetStartPosition(); pos != NULL; ) {
		m_apMesh.GetNextAssoc(pos, key, (void*&)pMesh);
		if (pMesh) SAFE_DELETE(pMesh);
	}
	m_apMesh.RemoveAll();
}

HRESULT CSfxMeshMng::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;
	return S_OK;
}
HRESULT CSfxMeshMng::RestoreDeviceObjects()
{
	POSITION pos;
	CString key;
	CModelObject* pMesh;
	for (pos = m_apMesh.GetStartPosition(); pos != NULL; ) {
		m_apMesh.GetNextAssoc(pos, key, (void*&)pMesh);
		if (pMesh) pMesh->RestoreDeviceObjects();
	}
	return S_OK;
}
HRESULT CSfxMeshMng::InvalidateDeviceObjects()
{
	POSITION pos;
	CString key;
	CModelObject* pMesh;
	for (pos = m_apMesh.GetStartPosition(); pos != NULL; )
	{
		m_apMesh.GetNextAssoc(pos, key, (void*&)pMesh);
		if (pMesh) pMesh->InvalidateDeviceObjects();
	}
	return S_OK;
}
HRESULT CSfxMeshMng::DeleteDeviceObjects()
{
	POSITION pos;
	CString key;
	CModelObject* pMesh;
	for (pos = m_apMesh.GetStartPosition(); pos != NULL; )
	{
		m_apMesh.GetNextAssoc(pos, key, (void*&)pMesh);
		if (pMesh) pMesh->DeleteDeviceObjects();
	}
	return S_OK;
}
