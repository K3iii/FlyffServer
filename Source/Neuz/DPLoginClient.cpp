#include "StdAfx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DPCertified.h"
#include "dpclient.h"
#include "DPLoginClient.h"
#include "Network.h"

extern	CDPClient		g_DPlay;
extern	CDPLoginClient	g_dpLoginClient;

CDPLoginClient::CDPLoginClient()
{
	m_timer.Set( SEC( 10 ) );
	m_fConn	= FALSE;
	m_nSlot	= -1;
	m_bShowDisconnectMsg = TRUE;
	m_lError = 0;

	BEGIN_MSG;
	ON_MSG( PACKETTYPE_ERROR, &CDPLoginClient::OnError );
	ON_MSG( PACKETTYPE_PLAYER_LIST, &CDPLoginClient::OnPlayerList );
	ON_MSG( PACKETTYPE_CACHE_ADDR, &CDPLoginClient::OnCacheAddr );
	ON_MSG( PACKETTYPE_QUERYTICKCOUNT, &CDPLoginClient::OnQueryTickCount );
	ON_MSG( PACKETTYPE_PRE_JOIN, &CDPLoginClient::OnPreJoin );
	ON_MSG( PACKETTYPE_ONE_HOUR_NOTIFY, &CDPLoginClient::OnOneHourNotify );

	ON_MSG( PACKETTYPE_PING, &CDPLoginClient::OnPing );
}

CDPLoginClient::~CDPLoginClient()
{

}

LONG CDPLoginClient::GetNetError()
{
	return m_lError;
}


void CDPLoginClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			m_fConn		= TRUE;
			break;

		case DPSYS_DESTROYPLAYERORGROUP:
			{
				#ifdef __INFO_SOCKLIB0516
				{
					LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
					m_lError = lpDestroyPlayer->dwFlags;
				}
				#endif

				m_fConn		= FALSE;
				g_DPlay.DeleteDPObject();

				if( m_bShowDisconnectMsg )
					g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_DIAG_0023), MB_OK, TRUE );
				m_bShowDisconnectMsg = TRUE;
#ifdef __AIO_NEUZAPP
				g_Neuz.Flash();
#endif
				break;
			}
	}
}

void CDPLoginClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
#ifdef __TEST_PACKET_CRYPT
	BYTE* pData = (BYTE*)lpMsg;
	for (DWORD i = 0; i < dwMsgSize; i++)
	{
		pData[i] = pData[i] ^ ((BYTE)(i % sizeof(BYTE)) ^ 171);
		pData[i] = pData[i] ^ ((sizeof(BYTE) - (BYTE)(i % sizeof(BYTE))) ^ 84);
	}
#endif

	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );

	if( pfn )
		( this->*( pfn ) )( ar );
}

#ifdef __TEST_PACKET_CRYPT
BOOL CDPLoginClient::Send(LPVOID lpData, DWORD dwDataSize, DPID dpidTo)
{
	BYTE* pData = (BYTE*)lpData;
	for (DWORD i = 0; i < dwDataSize; i++)
	{
		pData[i] = pData[i] ^ ((BYTE)(i % sizeof(BYTE)) ^ 171);
		pData[i] = pData[i] ^ ((sizeof(BYTE) - (BYTE)(i % sizeof(BYTE))) ^ 84);
	}
	return CDPMng::Send((LPVOID)pData, dwDataSize, dpidTo);
}
#endif

void CDPLoginClient::QueryTickCount( void )
{
	BEFORESENDSOLE( ar, PACKETTYPE_QUERYTICKCOUNT, DPID_UNKNOWN );
	ar << (DWORD)timeGetTime();
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPLoginClient::OnQueryTickCount( CAr & ar )
{
	DWORD dwTime;
	__int64 nTickCount;
	ar >> dwTime >> nTickCount;

	dwTime	= timeGetTime() - dwTime;
	nTickCount	+= ( dwTime * 5000 );	// ( dwTime / 2 ) * 10000

	g_TickCount.Start( nTickCount );

	TRACE( "OnQueryTickCount, %d\n", dwTime );
}

void CDPLoginClient::SendGetPlayerList( DWORD dwID, LPCSTR lpszAccount, LPCSTR lpszpw )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GETPLAYERLIST, DPID_UNKNOWN );
	ar.WriteString( ::GetProtocolVersion() );
	ar << g_Neuz.m_dwAuthKey;
	ar.WriteString( lpszAccount );
	ar.WriteString( lpszpw );
	ar << dwID;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPLoginClient::SendCreatePlayer(BYTE nSlot, LPCSTR lpszPlayer/*, LPDWORD adwEquipment*/, BYTE nFace, BYTE nCostume, BYTE nSkinSet, BYTE nHairMesh, DWORD dwHairColor, BYTE nSex, BYTE nJob, BYTE nHeadMesh, int nBankPW )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CREATE_PLAYER, DPID_UNKNOWN );
	ar.WriteString( g_Neuz.m_szAccount );
	ar.WriteString( g_Neuz.m_szPassword );
	ar << nSlot;
	ar.WriteString( lpszPlayer );

	ar << nFace << nCostume << nSkinSet << nHairMesh;
	ar << dwHairColor;
	ar << nSex << nJob << nHeadMesh;
	ar << nBankPW;
	ar << g_Neuz.m_dwAuthKey;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPLoginClient::SendDeletePlayer( BYTE nSlot, LPCTSTR szNo )
{
	BEFORESENDSOLE( ar, PACKETTYPE_DEL_PLAYER, DPID_UNKNOWN );

	ar.WriteString( g_Neuz.m_szAccount );
	ar.WriteString( g_Neuz.m_szPassword );
	ar.WriteString(szNo);
	ar << g_Neuz.m_apPlayer[nSlot]->m_idPlayer;
	ar << g_Neuz.m_dwAuthKey;
	ar << g_Neuz.m_aRTMessenger[nSlot].size();
	for( CRTMessenger::iterator it=g_Neuz.m_aRTMessenger[nSlot].begin(); it!=g_Neuz.m_aRTMessenger[nSlot].end(); it++ )
	{
		u_long uTemp = it->first;
		ar << it->first;
	}

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPLoginClient::SendPreJoin( const TCHAR* lpszAccount, u_long idPlayer, const TCHAR* lpszPlayer, int nSlot)
{
	m_nSlot		= nSlot;
	BEFORESENDSOLE( ar, PACKETTYPE_PRE_JOIN, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPLoginClient::OnPreJoin( CAr & ar )
{
	g_Neuz.m_dwTimeOutDis = 0xffffffff;

	g_DPlay.SendJoin( (BYTE)m_nSlot, g_Neuz.m_adwWorldID[m_nSlot], g_Neuz.m_apPlayer[m_nSlot], &g_Neuz.m_aRTMessenger[m_nSlot], g_Neuz.m_uIdofMulti );

	if( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_USA )
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 50 );
	else
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 25 );

}

void CDPLoginClient::OnOneHourNotify( CAr & ar )
{
	g_WndMng.OpenMessageBox( prj.GetText(TID_DIAG_EXPIRYTIME) );
}

void CDPLoginClient::OnPing( CAr & ar )
{
	DWORD dwPingTime;
	ar >> dwPingTime;
	g_Neuz.m_dwPingTime = GetCurrentTime() - dwPingTime;
	g_Neuz.m_nPingCount = 0;
}

void CDPLoginClient::OnError( CAr & ar )
{
	ar >> m_lError;

	switch( m_lError )
	{
		case ERROR_DUPLICATE_ACCOUNT:
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0042) ) );

				break;
			}
		case ERROR_USER_EXISTS:
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0034) ) );

				CWndCreateChar* pWnd = (CWndCreateChar*)g_WndMng.GetWndBase( APP_CREATE_CHAR );
				if( pWnd )
				{
					pWnd->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );
					pWnd->GetDlgItem( WIDC_CANCEL )->EnableWindow( TRUE );
				}
				break;
			}
		case ERROR_DUPLICATE_SLOT:
			{
				CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CREATE_CHAR );
				if( pWndBase )
					( (CWndCreateChar*)pWndBase )->Destroy();
				g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SELECT_CHAR );
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0018) ) );

				break;
			}
		case ERROR_ILLEGAL_VER:
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0035) ) );

				break;
			}
		case ERROR_INVALID_NAME_CHARACTER:
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0020) ) );

				CWndCreateChar* pWnd = (CWndCreateChar*)g_WndMng.GetWndBase( APP_CREATE_CHAR );
				if( pWnd )
					pWnd->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );
				break;
			}
		case ERROR_NO_SUCH_GROUP:
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0039) ) );

				CWndDeleteChar* pWnd = (CWndDeleteChar*)g_WndMng.GetWndBase( APP_DELETE_CHAR );
				if( pWnd )
				{
					CWndEdit* pWndEdit = (CWndEdit*)pWnd->GetDlgItem( WIDC_EDIT1 );
					pWndEdit->SetString( "" );
					CWndButton* pWndButton = (CWndButton*)pWnd->GetDlgItem( WIDC_OK );
					pWndButton->EnableWindow( TRUE );
					pWndButton	= (CWndButton*)pWnd->GetDlgItem( WIDC_CANCEL );
					pWndButton->EnableWindow( TRUE );
				}
				break;
			}

		case ERROR_WARTIME:
			{
				CWndDeleteChar* pWnd = (CWndDeleteChar*)g_WndMng.GetWndBase( APP_DELETE_CHAR );
				if( pWnd )
				{
					CWndEdit* pWndEdit = (CWndEdit*)pWnd->GetDlgItem( WIDC_EDIT1 );
					pWndEdit->SetString( "" );
					CWndButton* pWndButton = (CWndButton*)pWnd->GetDlgItem( WIDC_OK );
					pWndButton->EnableWindow( TRUE );
					pWndButton	= (CWndButton*)pWnd->GetDlgItem( WIDC_CANCEL );
					pWndButton->EnableWindow( TRUE );
				}
				break;
			}
		case ERROR_15MIN_PREVENT:
			{
				g_WndMng.CloseMessageBox();
				g_WndMng.OpenMessageBox( _T( prj.GetText( TID_15MIN_PREVENT ) ) );
				g_Neuz.m_dwTimeOutDis = 0xffffffff;
				break;
			}
	}
}

void CDPLoginClient::OnPlayerList( CAr & ar )
{
	g_Neuz.m_dwTimeOutDis = 0xffffffff;

	CWndBase* pWndBase = g_WndMng.GetWndBase( APP_SELECT_SERVER );
	if( pWndBase )
		((CWndSelectServer*)pWndBase)->Connected();

	if( ( pWndBase = g_WndMng.GetWndBase( APP_CREATE_CHAR ) ) )
	{
		( (CWndCreateChar*)pWndBase )->Destroy();
		g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SELECT_CHAR );
	}
	if( ( pWndBase = g_WndMng.GetWndBase( APP_DELETE_CHAR ) ) )
	{
		( (CWndDeleteChar*)pWndBase )->Destroy();
	}

	DWORD dwAuthKey;
	ar >> dwAuthKey;
	if( dwAuthKey != g_Neuz.m_dwAuthKey )
	{
		return;
	}

	DWORD dwIndex;

	CObj::SetMethod( METHOD_NONE );

	for( int i = 0 ; i < MAX_CHARACTER_LIST ; i++ )
	{
		g_Neuz.m_nCharacterBlock[i] = 2;
		SAFE_DELETE( g_Neuz.m_apPlayer[i] );
	}

	int countPlayer = 0;
	ar >> countPlayer;

	for( int i = 0 ; i < countPlayer ; i++ )
	{
//		CMover* pMover;
		BYTE bySex;
		int slot;
		DWORD dwJobLv;

		ar >> slot;

		ar >> g_Neuz.m_nCharacterBlock[slot];

		ar >> g_Neuz.m_adwWorldID[slot];
		ar >> dwIndex;


		CMover* pMover = new CMover;
		pMover->m_dwType = OT_MOVER;
		pMover->m_dwIndex = dwIndex;
		g_Neuz.m_apPlayer[slot]	= pMover;

		//g_Neuz.m_apPlayer[slot]	= (CMover*)CreateObj( g_Neuz.m_pd3dDevice, OT_MOVER, dwIndex, FALSE );
		g_Neuz.m_apPlayer[slot]->m_bPlayer	= TRUE;
		g_Neuz.m_apPlayer[slot]->InitProp();

		ar.ReadString(  (char*)g_Neuz.m_apPlayer[slot]->GetName(), MAX_NAME );
		D3DXVECTOR3 Pos;
		ar >> Pos;
		g_Neuz.m_apPlayer[slot]->SetPos(Pos);
		ar >> g_Neuz.m_apPlayer[slot]->m_idPlayer;
		ar >> g_Neuz.m_apPlayer[slot]->m_idparty;
		ar >> g_Neuz.m_apPlayer[slot]->m_idGuild;
		ar >> g_Neuz.m_apPlayer[slot]->m_dwSkinSet;
		ar >> g_Neuz.m_apPlayer[slot]->m_dwHairMesh;
		ar >> g_Neuz.m_apPlayer[slot]->m_dwHairColor;

		ar >> g_Neuz.m_apPlayer[slot]->m_dwHeadMesh;
		ar >> bySex;
		g_Neuz.m_apPlayer[slot]->SetSex( bySex );
		ar >> g_Neuz.m_apPlayer[slot]->m_nJob;
		ar >> g_Neuz.m_apPlayer[slot]->m_nLevel;
		ar >> dwJobLv;
		ar >> g_Neuz.m_apPlayer[slot]->m_nStr;
		ar >> g_Neuz.m_apPlayer[slot]->m_nSta;
		ar >> g_Neuz.m_apPlayer[slot]->m_nDex;
		ar >> g_Neuz.m_apPlayer[slot]->m_nInt;
		g_Neuz.m_apPlayer[slot]->SetHairColor( g_Neuz.m_apPlayer[slot]->m_dwHairColor );
		ar >> g_Neuz.m_apPlayer[slot]->m_dwMode;
#ifdef __MODEL_HIDEFASH
		for (int z = 0; z < 6; z++)
			ar >> g_Neuz.m_apPlayer[slot]->m_bHideFashion[z];
#endif

		int CountEquip = 0;
		ar >> CountEquip;

		ItemProp* pItemProp;
		DWORD dwItemId;
#ifdef __MODEL_CHANGE
		DWORD dwLook;
#endif
		int nParts = 0;
		for( int i = 0; i < CountEquip ; i++ )
		{
			ar >> dwItemId;
#ifdef __MODEL_CHANGE
			ar >> dwLook;
#endif

			pItemProp	= prj.GetItemProp( dwItemId );
			if( pItemProp == NULL)
				Error( "CDPLoginClient::OnPlayerList : %d아이템 프로퍼티 없음", dwItemId );

			nParts = (int)pItemProp->dwParts;

			if( nParts == PARTS_RWEAPON && g_Neuz.m_apPlayer[slot]->m_aEquipInfo[nParts].dwId != NULL_ID )
			{
				g_Neuz.m_apPlayer[slot]->m_aEquipInfo[PARTS_LWEAPON].dwId = g_Neuz.m_apPlayer[slot]->m_aEquipInfo[PARTS_RWEAPON].dwId;
#ifdef __MODEL_CHANGE
				g_Neuz.m_apPlayer[slot]->m_aEquipInfo[PARTS_LWEAPON].dwLook = g_Neuz.m_apPlayer[slot]->m_aEquipInfo[PARTS_RWEAPON].dwLook;
#endif
			}
			g_Neuz.m_apPlayer[slot]->m_aEquipInfo[nParts].dwId	= dwItemId;
#ifdef __MODEL_CHANGE
			g_Neuz.m_apPlayer[slot]->m_aEquipInfo[nParts].dwLook = dwLook;
#endif
		}
	}
	int CountMessenger = 0;
	ar >> CountMessenger;
	BYTE nSlot = 0;
	for( int i = 0 ; i < CountMessenger ; i++ )
	{
		ar >> nSlot;
		g_Neuz.m_aRTMessenger[nSlot].Serialize( ar );
	}

	CWndSelectChar* pWndSelectChar = (CWndSelectChar*)g_WndMng.GetWndBase( APP_SELECT_CHAR );
	if( pWndSelectChar )
	{
		pWndSelectChar->UpdateCharacter();
		if( g_Neuz.m_nCharacterBlock[ pWndSelectChar->m_nSelectCharacter ] == 0 )
		{
			for( int i = 0 ; i < MAX_CHARACTER_LIST ; i++ )
			{
				if( g_Neuz.m_nCharacterBlock[i] == 1 )
				{
					pWndSelectChar->m_nSelectCharacter = i;
					break;
				}
			}
		}
		pWndSelectChar->SelectCharacter( pWndSelectChar->m_nSelectCharacter );
	}
}

void CDPLoginClient::OnCacheAddr( CAr & ar )
{
	ar.ReadString( g_Neuz.m_lpCacheAddr, 16 );


	CString strAddr	= g_Neuz.m_lpCertifierAddr;
	if( ::GetLanguage() == LANG_GER && strAddr.Find( "192.168", 0 ) == 0 )
		lstrcpy( g_Neuz.m_lpCacheAddr, g_Neuz.m_lpCertifierAddr );
}

CDPLoginClient	g_dpLoginClient;