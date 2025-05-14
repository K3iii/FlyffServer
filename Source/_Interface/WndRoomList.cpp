

#include "stdafx.h"
#include "resData.h"
#include "WndRoomList.h"
#include "defineText.h"
#include "playerdata.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if __VER >= 13 // __HOUSING


CWndRoomList::CWndRoomList()
{
}
CWndRoomList::~CWndRoomList()
{

}
void CWndRoomList::OnDraw( C2DRender* p2DRender )
{
}

void CWndRoomList::Refresh()
{

	CWndListBox*				pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	vector<DWORD>::iterator		iter		= CHousing::GetInstance()->m_vecVisitable.begin();
	int							nIndex		= 0;

	pWndListBox->ResetContent();

	for(;iter != CHousing::GetInstance()->m_vecVisitable.end(); ++iter)
	{
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData(*iter);
		if(pPlayerData)
		{
			CString	strName;
			strName.Format("%d. %s", nIndex + 1, pPlayerData->szPlayer);
			pWndListBox->AddString(strName);
			pWndListBox->SetItemData(nIndex, *iter);
			++nIndex;
		}
	}
}

void CWndRoomList::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CHousing::GetInstance()->m_vecVisitable.clear();
	g_DPlay.SendHousingReqVisitableList();

	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->m_nSelectColor = D3DCOLOR_ARGB(255, 255, 0, 0);


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndRoomList::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MINIROOM_LIST, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRoomList::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndRoomList::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndRoomList::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndRoomList::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndRoomList::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	switch(nID)
	{
		case WIDC_BUTTON1:
			if(pWndListBox->GetCurSel() >= 0)
			{

				DWORD dwID = pWndListBox->GetItemData(pWndListBox->GetCurSel());
				if(dwID >= 0)
					g_DPlay.SendHousingVisitRoom(dwID);
				Destroy();
			}
			break;

		case WIDC_BUTTON2:
			g_DPlay.SendHousingReqVisitableList();
			break;

		case WIDC_BUTTON3:
			Destroy();
			break;
	};

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

#endif // __HOUSING