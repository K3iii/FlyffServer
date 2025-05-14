#include "stdafx.h"
#include "resData.h"
#include "WndAwakening.h"
#include "DPClient.h"
#include "WndManager.h"
#include "defineText.h"

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#endif	// __SYS_IDENTIFY

extern CDPClient g_DPlay;

#ifdef __PROTECT_AWAKE
CWndSelectAwakeCase::CWndSelectAwakeCase()
	: m_dwDeltaTime(0),
	m_dwItemIndex(0),
	m_pTexture(NULL),
	m_pVertexBufferGauge(NULL),
	m_pTexGuage(NULL)
{
	m_dwOldTime = g_tmCurrent;

	m_byObjID = 0;
	m_dwSerialNum = 0;
	m_n64NewOption = 0;
}

CWndSelectAwakeCase::~CWndSelectAwakeCase()
{
	DeleteDeviceObjects();
}

BOOL CWndSelectAwakeCase::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_AWAKE_SELECTCASE, 0, CPoint(0, 0), pWndParent);
}

void CWndSelectAwakeCase::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();




	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	ItemProp* pProp = (ItemProp*)prj.GetItemProp(m_dwItemIndex);
	if (pProp)
		m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);

	m_pTexGuage = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "Wndguage.tga"), 0xffff00ff);

	if (!m_pTexGuage)
		Error("CWndSelectAwakeCase::OnInitialUpdate m_pTexGuage(Wndguage.tga) is NULL");

	AddWndStyle(WBS_MODAL);

	RestoreDeviceObjects();
}


BOOL CWndSelectAwakeCase::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON1)
	{
		g_DPlay.SendSelectedAwakeningValue(m_byObjID, m_dwSerialNum, _AWAKE_OLD_VALUE);
		g_WndMng.PutString(GETTEXT(TID_GAME_REGARDLESS_USE03));
		Destroy();
	}
	else if (nID == WIDC_BUTTON2)
	{
		g_DPlay.SendSelectedAwakeningValue(m_byObjID, m_dwSerialNum, _AWAKE_NEW_VALUE);
		g_WndMng.PutString(GETTEXT(TID_GAME_REGARDLESS_USE03));
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

const static DWORD AWAKE_KEEP_TIME = 60000;

BOOL CWndSelectAwakeCase::process()
{
	return TRUE;
}

void CWndSelectAwakeCase::OnDraw(C2DRender* p2DRender)
{
	if (!g_pPlayer)
		return;

	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(m_byObjID);
	if (!pItemElem)
	{
		assert(0);
		return;
	}

	m_dwDeltaTime += (g_tmCurrent - m_dwOldTime);
	m_dwOldTime = g_tmCurrent;

	if (m_dwDeltaTime > AWAKE_KEEP_TIME)
	{

		g_DPlay.SendSelectedAwakeningValue(m_byObjID, m_dwSerialNum, _AWAKE_OLD_VALUE);

		g_WndMng.PutString(GETTEXT(TID_GAME_REGARDLESS_USE02));

		m_dwDeltaTime = 0;
		Destroy();
	}

	//old
	OutputOptionString(p2DRender, pItemElem);

	//new
	OutputOptionString(p2DRender, pItemElem, TRUE);


	CRect rect = GetWindowRect();
	rect.left += 14;
	rect.top += 174;


	rect.right = LONG((rect.left + (AWAKE_KEEP_TIME - m_dwDeltaTime) / 100) * 0.6f);
	rect.bottom = rect.top + 20;

	//	if( m_pTexGuage )
	//		p2DRender->RenderFillRect( rect, 0xffff0000, m_pTexGuage->m_pTexture  );

	m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVertexBufferGauge, m_pTexGuage);

	// draw icon
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_STATIC1);
	assert(wndCtrl);

	if (m_pTexture)
		m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
}

void CWndSelectAwakeCase::OutputOptionString(C2DRender* p2DRender, CItemElem* pItemElem, BOOL bNew)
{

	if (!pItemElem)
		return;

	int nSize = 0;
	if (!bNew)
		nSize = g_xRandomOptionProperty->GetRandomOptionSize(pItemElem->GetRandomOptItemId());
	else nSize = g_xRandomOptionProperty->GetViewRandomOptionSize(m_n64NewOption);

	__int64 n64Options = 0;
	n64Options = (bNew ? m_n64NewOption : pItemElem->GetRandomOptItemId());

	CString str;

	// option
	for (int i = 0; i < nSize; i++)
	{
		int nDst = 0, nAdj = 0;
		if (!g_xRandomOptionProperty->GetParam(n64Options, i, &nDst, &nAdj))
			continue;

		if (g_WndMng.IsDstRate(nDst))
		{
			if (nDst == DST_ATTACKSPEED)
				str.Format("\n%s %c%d%% ", g_WndMng.FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj / 2 / 10));
			else
				str.Format("\n%s %c%d%%", g_WndMng.FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj));
		}
		else
		{
			str.Format("\n%s %c%d", g_WndMng.FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj));
		}


		if (bNew)
			p2DRender->TextOut(70, 92 + i * 20, str, 0xff000000);
		else p2DRender->TextOut(70, 20 + i * 20, str, 0xff000000);
	}
}

void CWndSelectAwakeCase::SetData(BYTE byObjID, DWORD dwSerialNum, __int64 n64NewOption)
{

	m_byObjID = byObjID;
	m_dwSerialNum = dwSerialNum;
	m_n64NewOption = n64NewOption;

	if (!g_pPlayer)
	{
		assert(0);
		return;
	}

	CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(m_byObjID);
	if (pItemBase)
	{
		ItemProp* pProp = pItemBase->GetProp();
		if (pProp)
			m_dwItemIndex = pProp->dwID;
	}
}

HRESULT CWndSelectAwakeCase::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if (m_pVertexBufferGauge == NULL)
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			D3DFVF_TEXTUREVERTEX2,
			D3DPOOL_DEFAULT,
			&m_pVertexBufferGauge,
			NULL);
	assert(m_pVertexBufferGauge != NULL);

	return S_OK;
}
HRESULT CWndSelectAwakeCase::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pVertexBufferGauge);
	return S_OK;
}
HRESULT CWndSelectAwakeCase::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE(m_pVertexBufferGauge);
	return S_OK;
}

#endif	//__PROTECT_AWAKE