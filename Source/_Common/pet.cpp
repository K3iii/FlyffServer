#include "stdafx.h"
#include "pet.h"
#include "defineobj.h"

#ifdef __WORLDSERVER
#include "user.h"
#if __VER >= 12 // __PET_0519
#include "dpsrvr.h"
extern	CDPSrvr	g_DPSrvr;
#include "definetext.h"
#endif	// __PET_0519
#endif	// __WORLDSERVER

#include "xutil.h"

#if __VER >= 9	// __PET_0410

const	int		nDefaultLife	= 1;

CPetProperty::CPetProperty()
{
	memset( (void*)m_aPetAvailParam, 0, sizeof(PETAVAILPARAM) * PK_MAX );
	memset( (void*)m_anLevelupAvailLevelMax, 0, sizeof(BYTE) * PL_MAX );
	memset( (void*)m_adwLevelupAvailLevelProbability, 0, sizeof(DWORD) * PL_MAX * MAX_PET_AVAIL_LEVEL );
	memset( (void*)m_adwIncrementExp, 0, sizeof(DWORD) * PL_MAX );
	memset( (void*)m_awMaxEnergy, 0, sizeof(WORD) * PL_MAX );
}

CPetProperty::~CPetProperty()
{
	m_aFeedEnergy[0].clear();
	m_aFeedEnergy[1].clear();
	m_awAddLifeProbability.clear();
}

PPETAVAILPARAM	CPetProperty::GetAvailParam( BYTE nKind )
{
	if( nKind >= PK_TIGER && nKind < PK_MAX )
		return &m_aPetAvailParam[nKind];
	return NULL;
}

BYTE	CPetProperty::GetLevelupAvailLevel( BYTE wLevel )
{
	DWORD dwTotal	= 0;
	DWORD dwRandom	= xRandom( 1, 10001 );	// 1 ~ 10000
	for( int i = 0; i <= m_anLevelupAvailLevelMax[wLevel]; i++ )
	{
		dwTotal		+= m_adwLevelupAvailLevelProbability[wLevel][i];
		if(  dwTotal >= dwRandom )
			return i + 1;
	}
	return 0;
}

WORD	CPetProperty::GetFeedEnergy( DWORD dwCost, int nIndex )
{
	if( nIndex < 0 || nIndex > 1 )
		return 0;

	vector<FEEDENERGY>* pArr	= &m_aFeedEnergy[nIndex];
	for( vector<FEEDENERGY>::iterator i = pArr->begin(); i != pArr->end(); ++i )
	{
		if( dwCost >= i->dwCostMin && dwCost <= i->dwCostMax )
			return (WORD)xRandom( i->wEnergyMin, i->wEnergyMax + 1 );	// wEnergyMin ~ wEnergyMax
	}
	return 0;
}

BYTE	CPetProperty::Hatch( void )
{
	DWORD dwTotal	= 0;
	DWORD dwRandom	= xRandom( 1, 10001 );	// 1 - 10000
	for( int i = 0; i < PK_MAX; i++ )
	{
		dwTotal	+= m_aPetAvailParam[i].m_dwProbability;
		if( dwTotal >= dwRandom )
			return i;
	}
	return 0;	// error
}

DWORD	CPetProperty::GetIncrementExp( BYTE nLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_A )
		return m_adwIncrementExp[nLevel-1];
	return 0;
}

WORD CPetProperty::GetMaxEnergy( BYTE nLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_S )
		return m_awMaxEnergy[nLevel-1];
	return 1;
}

WORD CPetProperty::GetAddLife( void )
{
	DWORD dwTotal	= 0;
	DWORD dwRandom	= xRandom( 1, 101 );	// 1 ~ 100
	for( DWORD i = 0; i < m_awAddLifeProbability.size(); i++ )
	{
		dwTotal	+= m_awAddLifeProbability[i];
		if( dwTotal >= dwRandom )
			return	( (WORD)( i + 1 ) );
	}
	return 1;
}

CPetProperty* CPetProperty::GetInstance( void )
{
	static	CPetProperty	sPetProperty;
	return &sPetProperty;
}

BOOL CPetProperty::LoadScript( LPCTSTR szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "AvailDestParam" ) )
		{
			// 	DST_STR
			int	nKind	= 0;
			s.GetToken();	// {
			DWORD dwDstParam	= s.GetNumber();
			while( *s.token != '}' )
			{
				ASSERT( nKind < PK_MAX );
				m_aPetAvailParam[nKind].dwDstParam	= dwDstParam;
				for( int i = 0; i < MAX_PET_AVAIL_LEVEL; i++ )
					m_aPetAvailParam[nKind].m_anParam[i]	= s.GetNumber();
				m_aPetAvailParam[nKind].m_dwItemId	= s.GetNumber();
				m_aPetAvailParam[nKind].m_adwIndex[0]	= s.GetNumber();
				m_aPetAvailParam[nKind].m_adwIndex[1]	= s.GetNumber();
				m_aPetAvailParam[nKind].m_adwIndex[2]	= s.GetNumber();
				m_aPetAvailParam[nKind++].m_dwProbability	= s.GetNumber();
				dwDstParam	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "LevelupAvail" ) )
		{
			// 	10000	0	0	0	0	0	0	0	0
			int	nLevel	= (int)PL_D;
			s.GetToken();	// {{
			DWORD	dwProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				m_adwLevelupAvailLevelProbability[nLevel][0]	= dwProbability;
				for( int i = 1; i < MAX_PET_AVAIL_LEVEL; i++ )
				{
					m_adwLevelupAvailLevelProbability[nLevel][i]	= s.GetNumber();
					if( m_adwLevelupAvailLevelProbability[nLevel][i] > 0 )
						m_anLevelupAvailLevelMax[nLevel]	= i;
				}
				nLevel++;
				dwProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "FeedEnergy" ) )
		{
			int nIndex	= s.GetNumber();
			ASSERT( nIndex >= 0 && nIndex <= 1 );

			vector<FEEDENERGY>* pArr	= &m_aFeedEnergy[nIndex];
			// 	1	575	7	14
			FEEDENERGY	fe;
			s.GetToken();	// {
			fe.dwCostMin	= s.GetNumber();
			while( *s.token != '}' )
			{
				fe.dwCostMax	= s.GetNumber();
				fe.wEnergyMin	= s.GetNumber();
				fe.wEnergyMax	= s.GetNumber();
				pArr->push_back( fe );	// add
				fe.dwCostMin	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "IncrementExp" ) )
		{

			int	nLevel	= 0;
			s.GetToken();	// {
			DWORD	dwIncrementExp	= s.GetNumber();
			while( *s.token != '}' )
			{
				ASSERT( nLevel < PL_S );
				m_adwIncrementExp[nLevel++]	= dwIncrementExp;
				dwIncrementExp	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "MaxEnergy" ) )
		{

			int	nLevel	= 0;
			s.GetToken();	// {
			DWORD	dwMaxEnergy	= s.GetNumber();
			while( *s.token != '}' )
			{
				ASSERT( nLevel <= PL_S );
				m_awMaxEnergy[nLevel++]	= (WORD)( dwMaxEnergy );
				dwMaxEnergy		= s.GetNumber();
			}
		}
		else if( s.Token == _T( "AddLifeProbability" ) )
		{

			// 	50	// +1
			s.GetToken();	// {
			WORD wProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				m_awAddLifeProbability.push_back( wProbability );
				wProbability	= s.GetNumber();
			}
		}
		s.GetToken();
	}


	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

CPet::CPet()
{
	m_nKind	= 0;
	m_nLevel	= PL_EGG;
	m_dwExp	= 0;
	m_wEnergy	= 0;
	m_wLife	= 0;
	memset( (void*)m_anAvailLevel, 0, sizeof(BYTE) * PL_MAX );
#ifdef __PET_1024
	m_szName[0]	= '\0';
#endif	// __PET_1024
}

CPet::~CPet()
{
}

void CPet::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_nKind;
		ar << (BYTE)m_nLevel;
		ar << m_dwExp;
		ar << m_wEnergy;
		ar << m_wLife;
		ar.Write( (const void*)m_anAvailLevel, sizeof(BYTE) * PL_MAX );
#ifdef __PET_1024
		ar.WriteString( m_szName );
#endif	// __PET_1024
	}
	else
	{
		m_nKind	= PK_TIGER;
		m_nLevel	= PL_EGG;
		ar >> m_nKind;
		ar >> (BYTE &)m_nLevel;
		ar >> m_dwExp;
		ar >> m_wEnergy;
		ar >> m_wLife;
		ar.Read( (void *)m_anAvailLevel, sizeof(BYTE) * PL_MAX );
#ifdef __PET_1024
		ar.ReadString( m_szName, MAX_PET_NAME );
#endif	// __PET_1024
	}
}

void CPet::SetEnergy( WORD wEnergy )
{
#ifdef __DBSERVER
	m_wEnergy	= wEnergy;
#else	// __DBSERVER
	WORD wMaxEnergy		= GetMaxEnergy();
	m_wEnergy	= wEnergy > wMaxEnergy? wMaxEnergy: wEnergy;
#endif	// __DBSERVER
}

void CPet::SetExp( DWORD dwExp )
{
	m_dwExp		= dwExp > (DWORD)( GetMaxExp() ) ? GetMaxExp(): dwExp;
}

WORD CPet::GetLife( void )
{
	return ( m_nLevel == PL_EGG )? 0: m_wLife;
}

void CPet::SetAvailLevel( BYTE nLevel,  BYTE nAvailLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_S )
		m_anAvailLevel[nLevel-1]	= nAvailLevel;
}

BYTE CPet::GetAvailLevel( BYTE nLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_S )
		return m_anAvailLevel[nLevel-1];
	return 0;
}

#ifdef _DEBUG
#include "defineattribute.h"
#endif	// _DEBUG

void CPet::GetAvailDestParam( DWORD & dwDestParam, int & nParam )
{
	dwDestParam	= nParam	= 0;
	PPETAVAILPARAM pAvailParam	= CPetProperty::GetInstance()->GetAvailParam( m_nKind );
	if( pAvailParam )
	{
		dwDestParam	= pAvailParam->dwDstParam;
		for( int i = PL_D; i <= m_nLevel; i++ )
			nParam += pAvailParam->m_anParam[m_anAvailLevel[i-1] - 1];
	}
}

DWORD CPet::GetIndex( void )
{
	PPETAVAILPARAM pPetAvailParam	= CPetProperty::GetInstance()->GetAvailParam( m_nKind );
	if( m_nLevel == PL_EGG )
		return MI_PET_EGG;
	else if( m_nLevel > PL_EGG && m_nLevel < PL_B )
		return pPetAvailParam->m_adwIndex[0];
	else if( m_nLevel < PL_S )
		return pPetAvailParam->m_adwIndex[1];
	else
		return pPetAvailParam->m_adwIndex[2];
}

void CPet::InitEgg( void )
{
	SetLife( nDefaultLife );
	SetEnergy( GetMaxEnergy() );
}

CPet& CPet::operator=( CPet& pet )
{
	m_nKind		= pet.GetKind();
	m_nLevel	= pet.GetLevel();
	m_dwExp		= pet.GetExp();
	m_wEnergy	= pet.GetEnergy();
	m_wLife		= pet.GetLife();
	memcpy( m_anAvailLevel, pet.m_anAvailLevel, sizeof(BYTE) * PL_MAX );
#ifdef __PET_1024
	strcpy( m_szName, pet.GetName() );
#endif	// __PET_1024
	return *this;
}

#ifdef __CLIENT
enum
{
	STATE_INIT	= 1,
	STATE_IDLE,
	STATE_RAGE
};

BEGIN_AISTATE_MAP( CAIEgg, CAIInterface )

	ON_STATE( STATE_INIT, &CAIEgg::StateInit )
	ON_STATE( STATE_IDLE, &CAIEgg::StateIdle )
	ON_STATE( STATE_RAGE, &CAIEgg::StateRage )

END_AISTATE_MAP()

#define		PETSTATE_IDLE		0
#define		PETSTATE_TRACE		1

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAIEgg::CAIEgg( CObj* pObj, OBJID idOwner ) : CAIInterface( pObj )
{
	Init();
	m_idOwner	= idOwner;
}

CAIEgg::~CAIEgg()
{
	Destroy();
}

void CAIEgg::Init( void )
{
	m_idOwner = NULL_ID;
	m_nState = PETSTATE_IDLE;
}

void CAIEgg::Destroy( void )
{
	Init();
}

void CAIEgg::InitAI()
{
	PostAIMsg( AIMSG_SETSTATE, STATE_IDLE );
}

void CAIEgg::MoveToDst(D3DXVECTOR3 vDst)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	pMover->SetDestPos(vDst, true, FALSE);
}

void CAIEgg::MoveToDst(OBJID idTarget)
{
	CMover* pMover = GetMover();
	if (pMover->GetDestId() == idTarget && pMover->m_fArrivalRange == 0.0f)
		return;
	pMover->SetDestObj(idTarget);
}

BOOL CAIEgg::StateInit( const AIMSG & msg )
{
	return TRUE;
}

BOOL CAIEgg::MoveProcessIdle( const AIMSG & msg )
{
//	static	CTimer	tmTrace( 333.33F );

	CMover *pMover = GetMover();
	CMover *pOwner = prj.GetMover( m_idOwner );
	CWorld *pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();


	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;


	if( m_nState == PETSTATE_IDLE )
	{
		FLOAT fAngXZ	= pOwner->GetAngle();
		FLOAT fAngH		= pOwner->GetAngleX();

		D3DXVECTOR3 vPos;
//		AngleToVector( &vPos, fAngXZ, -fAngH, 1.0f );
//		vPos	+= pOwner->GetPos();
		vPos	= pOwner->GetPos();
		vPos.y	= pOwner->GetWorld()->GetLandHeight( pOwner->GetPos() );

		D3DXVECTOR3 vPos1, vPos2;
		fAngXZ	= pOwner->GetAngle();
		fAngH  = pOwner->GetAngleX();
		fAngXZ -= 90.0f;
		if( fAngXZ < 0 )
			fAngXZ += 360.0f;
		AngleToVector( &vPos1, fAngXZ, -fAngH, 1.0f );
		vPos1 += vPos;
		fAngXZ	= pOwner->GetAngle();
		fAngH  = pOwner->GetAngleX();
		fAngXZ += 90.0f;
		if( fAngXZ > 360.0f )
			fAngXZ -= 360.0f;
		AngleToVector( &vPos2, fAngXZ, -fAngH, 1.0f );
		vPos2	+= vPos;

		D3DXVECTOR3 v1	= vPos1 - pMover->GetPos();
		D3DXVECTOR3 v2	= vPos2 - pMover->GetPos();
		double d1	= (double)D3DXVec3LengthSq( &v1 );
		double d2	= (double)D3DXVec3LengthSq( &v2 );
		if( d1 < d2 )
			vPos	= vPos1;
		else
			vPos	= vPos2;
		D3DXVECTOR3 vDist = vPos - pMover->GetPos();
		vDist.y	= 0;
		FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
		if( fDistSq >1.0f )
		{
			MoveToDst( vPos );
//			m_nState = PETSTATE_TRACE;
		}
	}

	return TRUE;
}

BOOL CAIEgg::StateIdle( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS )
		MoveProcessIdle( msg );

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE )
		SendAIMsg( AIMSG_EXIT );

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )

	EndAIHandler( )

	return TRUE;
}

BOOL CAIEgg::MoveProcessRage( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();


	if( pMover->IsDie() || ( pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL ) )
		return FALSE;
	return TRUE;
}

BOOL CAIEgg::StateRage( const AIMSG & msg )
{
	CMover* pMover	= GetMover();
	CWorld* pWorld	= GetWorld();

	if( IsInvalidObj( pMover ) )
		return FALSE;

	BeginAIHandler( )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS )
		MoveProcessRage( msg );

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE )
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_BEGINMOVE )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )

	EndAIHandler( )

	return TRUE;
}
#endif	// __CLIENT

#endif	// __PET_0410

#ifdef __PET_1024
void CPet::SetName( const char* szName )
{
	strncpy( m_szName, szName, MAX_PET_NAME - 1 );
	m_szName[MAX_PET_NAME-1]	= '\0';
}
#endif	// __PET_1024