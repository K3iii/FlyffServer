#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "dpcertified.h"
#include "wndbase.h"
#include "wndcloseexistingconnection.h"
#include "Network.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
static BOOL		g_bRecvSvrList	= FALSE;
CDPCertified	g_dpCertified;


///////////////////////////////////////////////////////////////////////////////////////////////////////
CDPCertified::CDPCertified()
{
	m_timer.Set( MIN( 1 ) );
	m_lError = 0;

	BEGIN_MSG;
	ON_MSG( PACKETTYPE_SRVR_LIST, &CDPCertified::OnSrvrList );
	ON_MSG( PACKETTYPE_ERROR, &CDPCertified::OnError );
	ON_MSG( PACKETTYPE_KEEP_ALIVE, &CDPCertified::OnKeepAlive );
}

CDPCertified::~CDPCertified()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////



LONG CDPCertified::GetNetError()
{
	return m_lError;
}


BOOL CDPCertified::CheckNofityDisconnected()
{

	if( g_bRecvSvrList )
		return FALSE;


	if( m_lError )
		return FALSE;

	return TRUE;
}

void CDPCertified::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				m_fConn		= TRUE;
				g_bRecvSvrList	= FALSE;
			}
			break;
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				m_lError = lpDestroyPlayer->dwFlags;
			}

			m_fConn		= FALSE;

			if( CheckNofityDisconnected() )
			{
				g_WndMng.CloseMessageBox();
				g_WndMng.OpenMessageBox( prj.GetText(TID_DIAG_0023) );
#ifdef __AIO_NEUZAPP
				g_Neuz.Flash();
#endif
				CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
				if( pWndLogin )
					pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );
			}

			m_lError = 0;
			break;
	}
}

void CDPCertified::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
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

	if( pfn ) {
		( this->*( pfn ) )( ar, dpId );
	}
	else {
		/*
		switch( dw ) {
			default:
				TRACE( "Handler not found(%08x)\n", dw );
				break;
		}
		*/
	}
}

#ifdef __TEST_PACKET_CRYPT
BOOL CDPCertified::Send(LPVOID lpData, DWORD dwDataSize, DPID dpidTo)
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



void CDPCertified::SendCertify()
{
	if( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_USA )
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 40 );
	else
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 20 );

	BEFORESEND( ar, PACKETTYPE_CERTIFY );
	ar.WriteString( ::GetProtocolVersion() );
	ar.WriteString( g_Neuz.m_szAccount );

	////////////////////////////////////////////////////////////////////

#ifdef __ENCRYPT_PASSWORD
	char	szPlain[ 16 * MAX_PASSWORD ] = {0, };
	char	szEnc[ 16 * MAX_PASSWORD ] = {0, };

	::memcpy( szPlain, g_Neuz.m_szPassword, sizeof(g_Neuz.m_szPassword) );


	g_xRijndael->ResetChain();
	g_xRijndael->Encrypt( szPlain, szEnc, 16 * MAX_PASSWORD, CRijndael::CBC );

	ar.Write( szEnc, 16 * MAX_PASSWORD );
#else
	ar.WriteString( g_Neuz.m_szPassword );
#endif


	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifndef __V21_CLOSE_EXISTING_CONNECTION
void CDPCertified::SendCloseExistingConnection( const char* lpszAccount, const char* lpszpw )
{
	BEFORESEND( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION );
	ar.WriteString( lpszAccount );
	ar.WriteString( lpszpw );
	SEND( ar, this, DPID_SERVERPLAYER );
}
#else
void CDPCertified::SendCloseExistingConnection()
{
	BEFORESEND(ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION);
	ar.WriteString(g_Neuz.m_szAccount);
	ar.WriteString(g_Neuz.m_szPassword);
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif


LPCTSTR CDPCertified::GetServerName( int nServerIndex )
{
	int nCount = 0;
	for( int i = 0; i < (int)( m_dwSizeofServerset ); i++ )
	{
		if( m_aServerset[i].dwParent == NULL_ID )
		{
			if( nCount++ == nServerIndex )
			{
				return m_aServerset[i].lpName;
			}
		}
	}
	return "Unknown";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void CDPCertified::OnKeepAlive( CAr & ar, DPID )
{
//	BEFORESENDSOLE( ar2, PACKETTYPE_KEEP_ALIVE, DPID_UNKNOWN );
	BEFORESEND( ar2, PACKETTYPE_KEEP_ALIVE );
	SEND( ar2, this, DPID_SERVERPLAYER );
}


//	Handlers
void CDPCertified::OnSrvrList( CAr & ar, DPID )
{
	g_bRecvSvrList	= TRUE;

	ar >> g_Neuz.m_dwAuthKey;
	ar >> g_Neuz.m_cbAccountFlag;

	char szBak[MAX_ACCOUNT] = { 0, };
	ar.ReadString(szBak, MAX_ACCOUNT);
	if (lstrcmp(g_Neuz.m_szAccount, szBak))
	{
		Error("CDPCertified.OnSrvrList");
		exit(0);
	}

	CString strAddr	= g_Neuz.m_lpCertifierAddr;
	BOOL bPrivate	= ( ::GetLanguage() == LANG_GER && strAddr.Find( "192.168", 0 ) == 0 );

	ar >> m_dwSizeofServerset;
	LPSERVER_DESC pServer;
	for( int i = 0; i < (int)( m_dwSizeofServerset ); i++ )
	{
		pServer		= m_aServerset + i;
		ar >> pServer->dwParent;
		ar >> pServer->dwID;
		ar.ReadString( pServer->lpName, 36 );
		ar.ReadString( pServer->lpAddr, 16 );


		if( bPrivate && pServer->lpAddr[0] != '\0' )
			lstrcpy( pServer->lpAddr, g_Neuz.m_lpCertifierAddr );

		ar >> pServer->b18;
		ar >> pServer->lCount;
		ar >> pServer->lEnable;
		ar >> pServer->lMax;
	}

	CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_LOGIN );
	if( pWndBase )
		( (CWndLogin*)pWndBase )->Connected( 0 );
}

void CDPCertified::OnError( CAr & ar, DPID dpid )
{
	g_Neuz.m_dwTimeOutDis = 0xffffffff;
	int nText = 0;

	ar >> m_lError;

	switch( m_lError )
	{
		case ERROR_DUPLICATE_ACCOUNT:			// 103L
			{
				g_WndMng.CloseMessageBox();
				SAFE_DELETE(g_WndMng.m_pWndCloseExistingConnection);
				g_WndMng.m_pWndCloseExistingConnection	= new CWndCloseExistingConnection;
				g_WndMng.m_pWndCloseExistingConnection->Initialize();
				return;
			}

		case ERROR_ILLEGAL_VER:
			nText = TID_DIAG_0035;
			break;
#ifdef __SECURITY_0628
		case ERROR_FLYFF_RESOURCE_MODIFIED:
			nText	= TID_GAME_RESOURCE_MODIFIED;
			break;
#endif	// __SECURITY_0628
		case ERROR_ACCOUNT_EXISTS:
			nText = TID_DIAG_0032;
			break;
		case ERROR_FLYFF_PASSWORD:
			nText = TID_DIAG_0016;
			break;
		case ERROR_FLYFF_ACCOUNT:
			nText = TID_DIAG_0038;
			break;
		case ERROR_OVERFLOW:
			nText = TID_DIAG_0041;
			break;
		case ERROR_EXTERNAL_ADDR:
			nText = TID_DIAG_0053;
			break;
		case ERROR_BLOCKGOLD_ACCOUNT:
			nText = TID_DIAG_0015;
			break;
		case ERROR_FLYFF_AUTH:
			nText = TID_DIAG_0026;
			break;
		case ERROR_FLYFF_PERMIT:
			nText = TID_DIAG_0050;
			break;
		case ERROR_FLYFF_NEED_AGREEMENT:
			nText = TID_DIAG_0001;
			break;
		case ERROR_FLYFF_NO_MEMBERSHIP:
			nText = TID_GAME_ACCOUNTWEBRETIRE;
			break;
		case ERROR_TOO_LATE_PLAY:				// 131L
			nText = TID_GAME_LIMITCONNECTION;
			break;
		case ERROR_IP_CUT:						// 132L
			nText = TID_GAME_OTHERCOUNTRY;
			break;
		case ERROR_FLYFF_DB_JOB_ING:			// 133L
			nText = TID_DB_INSPECTION;
			break;
		case ERROR_15SEC_PREVENT:				// 134L
			nText = TID_15SEC_PREVENT;
			break;
		case ERROR_15MIN_PREVENT:				// 135L
			nText = TID_15MIN_PREVENT;
			break;
		case ERROR_CERT_GENERAL:
			nText = TID_ERROR_CERT_GENERAL;
			break;
		case ERROR_FLYFF_EXPIRED_SESSION_PASSWORD:
			nText	= TID_ERROR_EXPIRED_SESSION_PASSWORD;
			break;
	}

	if( nText > 0 )
	{
		g_WndMng.CloseMessageBox();
		g_WndMng.OpenMessageBox( _T( prj.GetText(nText) ) );

		CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
			pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );


		//BEFORESENDSOLE( ar, PACKETTYPE_ERROR, DPID_UNKNOWN );
		BEFORESEND( ar, PACKETTYPE_ERROR );		// chipi_090219
		SEND( ar, this, DPID_SERVERPLAYER );
	}
}


