#include "stdafx.h"
#include "post.h"
#include <afxdisp.h>

#ifdef	__CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif	//__CLIENT

u_long	CMail::s_nMail	= 0;

#ifdef __DBSERVER
#include "dptrans.h"
#endif	// __DBSERVER

CMail::CMail()
{
	m_nMail	= 0;
	m_idSender	= 0;
	m_pItemElem	= NULL;
	m_tmCreate	= 0;
	m_byRead	= FALSE;
	*m_szTitle	= '\0';
	*m_szText	= '\0';
#ifdef __DBSERVER
	m_pMailBox	= NULL;
#endif	// __DBSERVER
	m_nGold		= 0;
}

CMail::CMail( u_long idSender, CItemElem* pItemElem, int nGold, char* szTitle, char* szText )
{
	m_nMail	= 0;
	m_idSender	= idSender;
	m_pItemElem	= pItemElem;
	m_tmCreate	= time_null();
	m_byRead	= FALSE;
	lstrcpy( m_szTitle, szTitle );
	lstrcpy( m_szText, szText );
#ifdef __DBSERVER
	m_pMailBox	= NULL;
#endif	// __DBSERVER
	m_nGold		= 0;
}

CMail::~CMail()
{
	Clear();
}

void CMail::Clear( void )
{
	SAFE_DELETE( m_pItemElem );
}

void CMail::GetMailInfo( int* nKeepingDay, DWORD* dwKeepingTime )
{
	CTime cBase(m_tmCreate);
	CTime cCurrent = CTime::GetCurrentTime();

	CTimeSpan ts = cCurrent - cBase;
	if (ts <= 0)
	{
		*nKeepingDay = MAX_KEEP_MAX_DAY;
		*dwKeepingTime = MAX_KEEP_MAX_DAY * 24;
		return;
	}

	const int nGap = static_cast<int>(MAX_KEEP_MAX_DAY - ts.GetDays());

	if (dwKeepingTime)
	{
		*dwKeepingTime = static_cast<DWORD>((MAX_KEEP_MAX_DAY * 24) - ts.GetTotalHours());
	}

	*nKeepingDay = nGap > 0 ? nGap : 0;
}

void CMail::Serialize( CAr & ar, BOOL bData )
{
	if( bData )
	{
		if( ar.IsStoring() )
		{
			ar << m_nMail << m_idSender;
			if( m_pItemElem )
			{
				ar << (BYTE)1;
				m_pItemElem->Serialize( ar );
			}
			else
			{
				ar << (BYTE)0;
			}
			ar << m_nGold;
	//		ar << m_tmCreate;
			ar << time_null() - m_tmCreate;

			ar << m_byRead;
			ar.WriteString( m_szTitle );
			ar.WriteString( m_szText );
		}
		else
		{
			ar >> m_nMail >> m_idSender;

			BYTE byItemElem;
			ar >> byItemElem;
			if( byItemElem )
			{
				m_pItemElem	= new CItemElem;
				m_pItemElem->Serialize( ar );
			}
			ar >> m_nGold;
			time_t tm;
			ar >> tm;
			m_tmCreate	= time_null() - tm;
	//		ar >> m_tmCreate;
			ar >> m_byRead;
			ar.ReadString( m_szTitle, MAX_MAILTITLE );
			ar.ReadString( m_szText, MAX_MAILTEXT );
		}
	}
	else	// no data
	{
		if( ar.IsStoring() )
		{
			ar << m_nMail;
			ar << time_null() - m_tmCreate;
			ar << m_byRead;
		}
		else	// load
		{
			ar >> m_nMail;
			time_t tm;
			ar >> tm;
			m_tmCreate	= time_null() - tm;
			ar >> m_byRead;
		}
	}
}

CMailBox::CMailBox()
{
	m_idReceiver	= 0;
#ifdef __WORLDSERVER
	m_nStatus	= CMailBox::nodata;
#endif	// __WORLDSERVER
}

CMailBox::CMailBox( u_long idReceiver )
{
	m_idReceiver	= idReceiver;
#ifdef __WORLDSERVER
	m_nStatus	= CMailBox::nodata;
#endif	// __WORLDSERVER
}

CMailBox::~CMailBox()
{
	Clear();

}

CMailBox*	CMailBox::GetInstance( void )
{
	static CMailBox	sMailBox;
	return &sMailBox;
}

void CMailBox::Clear( void )
{
	for( MailVectorItr i = begin(); i != end(); ++i )
	{
#ifdef __DBSERVER
		if( m_pPost )
			m_pPost->m_mapMail4Proc.erase( (*i)->m_nMail );
#endif	// __DBSERVER
		SAFE_DELETE( *i );
	}
	clear();
}

u_long CMailBox::AddMail( CMail* pMail )
{
	if( 0 == pMail->m_nMail )
		pMail->m_nMail	= ++CMail::s_nMail;
	else
		CMail::s_nMail	= pMail->m_nMail;

	push_back( pMail );
#ifdef __DBSERVER
	pMail->SetMailBox( this );
	if( m_pPost )
	{
		bool bResult	= m_pPost->m_mapMail4Proc.insert( map<u_long, CMail*>::value_type( pMail->m_nMail, pMail ) ).second;
		if( bResult == FALSE )
		{
			Error( "AddMail Failed - nMail : %d, idSender : %d", pMail->m_nMail, pMail->m_idSender );
			SAFE_DELETE(pMail);
			return 0;
		}
	}
#endif	// __DBSERVER
	return pMail->m_nMail;
}

#ifdef __DBSERVER
void CMailBox::Write( CAr & ar )
{
	ar << (int)size();

// 	//	BEGINTEST
// 	Error( "CMailBox::Write [%d]", (int)size() );

	for( MailVectorItr i = begin(); i != end(); ++i )
	{
		CMail* pMail	= *i;
		ar << pMail->m_nMail;
		pMail->Serialize( ar, TRUE );
	}
}
#endif	// __DBSERVER

#ifdef __WORLDSERVER
void CMailBox::Read( CAr & ar )
{
	int nSize;
	ar >> nSize;
	CMail temp;
	for( int i = 0; i < nSize; i++ )
	{
		u_long nMail;
		ar >> nMail;
		CMail* pMail	= GetMail( nMail );
		if( pMail )
		{
// 			//	BEGINTEST
// 			Error( "CMailBox::Read " );

			pMail->Clear();
			pMail->Serialize( ar, TRUE );
		}
		else
		{

// 			//	BEGINTEST
// 			Error( "CMailBox::Read pMail == NULL" );

			// mail not found
			Error( "CMailBox::Read - GetMail return NULL. nMail : %d", nMail );
			//temp.Clear();
			//temp.Serialize( ar, TRUE );
			CMail* pNewMail = new CMail;
			if( pNewMail != NULL )
			{
				pNewMail->Clear();
				pNewMail->Serialize( ar, TRUE );
				push_back( pNewMail );
				Error( "CMailBox::Read - Create NewMail. nMail : %d, Sender : %07d", nMail, pNewMail->m_idSender );
			}
			else
			{
				Error( "CMailBox::Read - Create NewMail Failed. nMail : %d", nMail );
				temp.Clear();
				temp.Serialize( ar, TRUE );
			}
		}
	}
	m_nStatus	= CMailBox::data;
}

void CMailBox::ReadReq( CAr & ar )
{
	int nSize;
	ar >> nSize;

	for( int i = 0; i < nSize; i++ )
	{
		u_long nMail;
		ar >> nMail;

		CMail* pMail	= GetMail( nMail );

		if( pMail != NULL )
		{
			Error( "CMailBox::ReadReq - Mail Exist. nMail : %d", nMail );
			pMail->Clear();
			pMail->Serialize( ar, TRUE );
		}
		else
		{
			CMail* pNewMail = new CMail;
			if( pNewMail != NULL )
			{
				pNewMail->Clear();
				pNewMail->Serialize( ar, TRUE );
				push_back( pNewMail );
			}
			else
			{
				Error( "CMailBox::ReadReq - New Failed. nMail : %d", nMail );
				CMail temp;
				temp.Clear();
				temp.Serialize( ar, TRUE );
			}
		}
	}
	m_nStatus	= CMailBox::data;
}
#endif	// __WORLDSERVER

void CMailBox::Serialize( CAr & ar, BOOL bData )
{
	if( ar.IsStoring() )
	{
		ar << m_idReceiver;
		ar << (int)size();
		for( MailVectorItr i = begin(); i != end(); ++i )
		{
			CMail* pMail	= *i;
			pMail->Serialize( ar, bData );
		}
	}
	else
	{
		Clear();
		ar >> m_idReceiver;
		int nSize;
		ar >> nSize;

#ifdef __MAIL_REQUESTING_BOX
		if( g_WndMng.m_bWaitRequestMail && nSize <= 0 )
			g_DPlay.SendQueryMailBox();
#endif //__MAIL_REQUESTING_BOX

		for( int i = 0; i < nSize; i++ )
		{
			CMail* pMail	= new CMail;
			pMail->Serialize( ar, bData );
			AddMail( pMail );
		}
	}
}

MailVectorItr CMailBox::Find( u_long nMail )
{
	MailVectorItr i = begin();
	for( ; i != end(); ++i )
	{
		CMail* pMail	= *i;
		if( pMail->m_nMail == nMail )
			break;
	}
	return i;
}

CMail* CMailBox::GetMail( u_long nMail )
{
	MailVectorItr i = Find( nMail );
	if( i != end() )
		return *i;

	return NULL;
}

BOOL CMailBox::RemoveMail( u_long nMail )
{
	MailVectorItr i = Find( nMail );
	if( i != end() )
	{
#ifdef __DBSERVER
		if( m_pPost )
			m_pPost->m_mapMail4Proc.erase( (*i)->m_nMail );
#endif	// __DBSERVER
		SAFE_DELETE( *i );
		erase( i );
		return TRUE;
	}

	return FALSE;
}

BOOL CMailBox::RemoveMailItem( u_long nMail )
{
	CMail* pMail	= GetMail( nMail );
	if( pMail && pMail->m_pItemElem )
	{
		SAFE_DELETE( pMail->m_pItemElem );
		return TRUE;
	}

	return FALSE;
}

BOOL CMailBox::RemoveMailGold( u_long nMail )
{
	CMail* pMail	= GetMail( nMail );
	if( pMail && pMail->m_nGold > 0 )
	{
		pMail->m_nGold	= 0;
		return TRUE;
	}
	return FALSE;
}

BOOL CMailBox::ReadMail( u_long nMail )
{
	CMail* pMail	= GetMail( nMail );
	if( pMail )
	{
		pMail->m_byRead	= TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL CMailBox::IsStampedMailExists( void )
{
	MailVectorItr i = begin();
	for( ; i != end(); ++i )
	{
		CMail* pMail	= *i;
		if( pMail->m_byRead == FALSE )
		{
			return TRUE;
		}
	}

	return FALSE;
}

CPost::CPost()
{
}

CPost::~CPost()
{
	Clear();
}

void CPost::Clear( void )
{
	for( map<u_long, CMailBox*>::iterator i = m_mapMailBox.begin(); i != m_mapMailBox.end(); ++i )
	{
		CMailBox* pMailBox	= i->second;
		SAFE_DELETE( pMailBox );
	}
	m_mapMailBox.clear();
#ifdef __DBSERVER
#ifdef _DEBUG
	TRACE( "CPost::m_mapMail4Proc.size() = %d\n", m_mapMail4Proc.size() );
#endif	// _DEBUG
	m_mapMail4Proc.clear();
#endif	// __DBSERVER
}

u_long CPost::AddMail( u_long idReceiver, CMail* pMail )
{
	CMailBox* pMailBox	= GetMailBox( idReceiver );
	if( !pMailBox )
	{
		pMailBox	= new CMailBox( idReceiver );
		if (AddMailBox(pMailBox) == FALSE)
		{
			Error("AddMailBox Failed - idReceiver : %d", idReceiver);
			SAFE_DELETE(pMailBox);
			return 0;
		}
	}
	return pMailBox->AddMail( pMail );
}

CMailBox* CPost::GetMailBox( u_long idReceiver )
{
	map<u_long, CMailBox*>::iterator i = m_mapMailBox.find( idReceiver );
	if( i != m_mapMailBox.end() )
		return i->second;
	return NULL;
}

BOOL CPost::AddMailBox( CMailBox* pMailBox )
{
#ifdef __DBSERVER
	pMailBox->SetPost( this );
#endif	// __DBSERVER
	return m_mapMailBox.insert( map<u_long, CMailBox*>::value_type( pMailBox->m_idReceiver, pMailBox ) ).second;
}

void CPost::Serialize( CAr & ar, BOOL bData )
{
	if( ar.IsStoring() )
	{
		ar << m_mapMailBox.size();
		for( map<u_long, CMailBox*>::iterator i = m_mapMailBox.begin(); i != m_mapMailBox.end(); ++i )
		{
			CMailBox* pMailBox	= i->second;
			pMailBox->Serialize( ar, bData );
		}
	}
	else
	{
		int nSize;
		ar >> nSize;
		for( int i = 0; i < nSize; i++ )
		{
			CMailBox* pMailBox	= new CMailBox;
			pMailBox->Serialize( ar, bData );
			if (AddMailBox(pMailBox) == FALSE)
			{
				SAFE_DELETE(pMailBox);
			}
		}
	}
}

CPost*	CPost::GetInstance( void )
{
	static CPost	sPost;
	return &sPost;
}

#ifdef __DBSERVER
void CPost::Process( void )
{
	CMclAutoLock	Lock( m_csPost );
	CTime t	= CTime::GetCurrentTime() - CTimeSpan( MAX_KEEP_MAX_DAY, 0, 0, 0 );

	list<CMail*>	lspMail;
	for( map<u_long, CMail*>::iterator i = m_mapMail4Proc.begin(); i != m_mapMail4Proc.end(); ++i )
	{
		CMail* pMail	= i->second;
		if( pMail->m_tmCreate < t.GetTime() )
			lspMail.push_back( pMail );
	}
#ifdef __POST_1204
	g_DbManager.RemoveMail( lspMail, t.GetTime() );
#else	// __POST_1204
	g_DbManager.RemoveMail( lspMail );
#endif	// __POST_1204
	lspMail.clear();
}
#endif	// __DBSERVER