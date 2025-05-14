#include "stdafx.h"
#include "defineObj.h"
#include "region.h"
#include "WorldMng.h"
#include "lang.h"
#include "respawn.h"
#include "PathObj.h"

#ifdef __CLIENT
#include "ResData.h"
#endif

#ifdef __WORLDSERVER
#include "user.h"
#endif	// __WORLDSERVER

extern	CWorldMng	g_WorldMng;
#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonBase.h"
#endif // __INSTANCE_DUNGEON


BOOL CWorld::OpenWorld( OBJID idWorld, BOOL bDir )
{
	LPWORLD lpWorld	= g_WorldMng.GetWorldStruct( idWorld );
	ASSERT( lpWorld->IsValid() );
	return( OpenWorld( lpWorld->m_szFileName, bDir ) );
}

const char* DEFAULT_FNAME = "default";

BOOL CWorld::OpenWorld( LPCTSTR lpszFileName, BOOL bDir )
{
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	TCHAR szPathName[ _MAX_PATH ];

	if( bDir == FALSE )
	{
		strcpy( szPathName, lpszFileName );
	}
	else
	{
		_splitpath( lpszFileName, drive, dir, name, ext );
		strcpy( szPathName, lpszFileName );
		strcat( szPathName, "\\" );
		strcat( szPathName, name );
		strcat( szPathName, ".wld" );
	}

	CScript script;
	if( script.Load( szPathName ) == FALSE )
		return FALSE;

	bool bLoaded_MPU = false;

	do
	{
		script.GetToken();
		if( script.Token == _T( "size" ) )
		{
			m_nLandWidth = script.GetNumber();
			script.GetToken();
			m_nLandHeight = script.GetNumber();
			CalcBound();
		}
		else
		if( script.Token == _T( "ambient" ) )
		{
		#ifndef __WORLDSERVER
			m_dwAmbient = script.GetHex();
		#endif	// __WORLDSERVER
		}
		else
		if( script.Token == _T( "revival" ) )
		{
			m_dwIdWorldRevival = script.GetNumber();
			script.GetToken();
			strcpy( m_szKeyRevival, script.token );
		}
		else
		if( script.Token == _T( "indoor" ) )
		{
			m_bIsIndoor = script.GetNumber();
		#if defined( __CLIENT )
			m_bViewSkybox = m_bViewWeather = !m_bIsIndoor;
		#endif //__CLIENT
		}
		else
		if( script.Token == _T( "fly" ) )
		{
			m_bFly = script.GetNumber();
		}
		else
		if( script.Token == _T( "bgm" ) )
		{
			m_dwIdMusic = script.GetNumber();
		}
		else
		if( script.Token == _T( "pkmode" ) )
		{
			m_nPKMode = script.GetNumber();
		}
		else
		if( script.Token == _T( "diffuse" ) )
		{
			m_dwDiffuse = script.GetHex();
		}
		else
		if( script.Token == _T( "lightDir" ) )
		{
			m_v3LightDir.x = script.GetFloat();
			m_v3LightDir.y = script.GetFloat();
			m_v3LightDir.z = script.GetFloat();
		}
		else
		if( script.Token == _T( "fogSetting" ) )
		{
		#if defined( __CLIENT )
			m_fFogStartValue = script.GetFloat();
			m_fFogEndValue   = script.GetFloat();
			m_fFogDensity    = script.GetFloat();
		#endif //__CLIENT
		}
		else
		if( script.Token == __T("MPU") )
		{
			bLoaded_MPU = true;
			g_MPU = script.GetNumber( );
		}


#if __VER >= 15 // __BS_CHANGING_ENVIR
		else
		if( script.Token == __T( "sky" ) )
		{
			for( int i=0; i<3; ++i)
			{
				script.GetToken();
				if( DEFAULT_FNAME != script.Token )
				{
					m_kWorldEnvir._kSky[i]._bUse = TRUE;
					m_kWorldEnvir._kSky[i]._strFileName = script.token;
				}
			}

		}
		else
		if( script.Token == __T( "cloud" ) )
		{
			for( int i=0; i<3; ++i)
			{
				script.GetToken();
				if( DEFAULT_FNAME != script.Token )
				{
					m_kWorldEnvir._kCloud[i]._bUse = TRUE;
					m_kWorldEnvir._kCloud[i]._strFileName = script.token;
				}
			}
		}
		else
		if( script.Token  == __T( "sun" ) )
		{
			script.GetToken();
			if( DEFAULT_FNAME != script.Token )
			{
				m_kWorldEnvir._kSun._bUse = TRUE;
				m_kWorldEnvir._kSun._strFileName = script.token;
			}
		}
		else
		if( script.Token == __T( "moon" ) )
		{
			script.GetToken();
			if( DEFAULT_FNAME != script.Token )
			{
				m_kWorldEnvir._kSun._bUse = TRUE;
				m_kWorldEnvir._kSun._strFileName = script.token;
			}
		}
		else
		if( script.Token == __T( "use24light" ) )
		{
			m_bUsing24Light = script.GetNumber( );
		}
#ifdef __BS_CHANGEABLE_WORLD_SEACLOUD
		else
		if( script.Token == __T( "seacloud" ) )
		{
			script.GetToken( );
			if( DEFAULT_FNAME != script.Token )
			{
				m_kSeaCloud._bUse = TRUE;
				m_kSeaCloud._strFileName = script.token;
			}
		}
#endif //__BS_CHANGEABLE_WORLD_SEACLOUD

#endif
	} while( script.tok != FINISHED );

	if( !bLoaded_MPU )
		g_MPU = OLD_MPU;

#ifdef __CLIENT
	m_nVisibilityLand	= (int)( m_fFarPlane / ( MAP_SIZE * MPU ) );
#endif

#if __VER >= 15
	m_iMPU = g_MPU;
#else
	m_iMPU = OLD_MPU;
#endif


	_splitpath( szPathName, drive, dir, name, ext );
	strcpy( m_szFilePath, dir );
	strcpy( m_szFileName, name );
	strcat( m_szFileName, ext );

#ifdef __WORLDSERVER
	_tcscpy( m_lpszWorld, m_szFileName );

	int nView = 1;
	if( m_dwWorldID == WI_WORLD_GUILDWAR )
		nView = 2;

#if __VER >= 15 // __GUILD_HOUSE
	nView = nView + ( OLD_MPU - m_iMPU );
#endif

	m_linkMap.Init( m_nLandWidth, m_nLandHeight, nView, TRUE, m_iMPU );
	m_apHeightMap = new float[ WORLD_WIDTH * WORLD_HEIGHT ];
	memset( m_apHeightMap, 0, sizeof( float ) * WORLD_WIDTH * WORLD_HEIGHT );
	m_apWaterHeight = new WATERHEIGHT[ NUM_PATCHES_PER_SIDE * m_nLandWidth * NUM_PATCHES_PER_SIDE * m_nLandHeight ];
	memset( m_apWaterHeight, 0, sizeof( WATERHEIGHT) * NUM_PATCHES_PER_SIDE * m_nLandWidth * NUM_PATCHES_PER_SIDE * m_nLandHeight );
#else	// __WORLDSERVER
	m_apLand = new CLandscape * [ m_nLandWidth * m_nLandHeight ];
	memset( m_apLand, 0, sizeof( CLandscape*) * m_nLandWidth * m_nLandHeight );
	LoadRegion();

	InitWorldEnvir( szPathName );
	InitContinent( szPathName );



#endif	// __WORLDSERVER

	return TRUE;
}

#ifdef __WORLDSERVER


BOOL CWorld::LoadPatrol()
{
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.pat", m_szFilePath, strLandName );
	return CPatrolPath::GetInstance()->LoadPatrol( strLandTemp );
}

BOOL CWorld::LoadObject( int nLayer )
{
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.dyo", m_szFilePath, strLandName );
	CResFile file;

	if( file.Open( strLandTemp,"rb" ) == FALSE )
		return FALSE;

	while( 1 )
	{
		CObj* pObj = ::ReadObj( &file );
		if( pObj == NULL )
			break;

		D3DXVECTOR3 vPos = pObj->GetPos();
		pObj->SetPos( vPos );
		pObj->SetIndex( NULL, pObj->m_dwIndex );
		if( pObj->GetType() == OT_MOVER )
			pObj->InitMotion( ((CMover *)pObj)->m_dwMotion );
		pObj->UpdateLocalMatrix();

		if( IsUsableDYO( pObj ) )
		{
			ADDOBJ( pObj, TRUE, nLayer );
#if __VER >= 13 // __QUEST_HELPER
			if( pObj->GetType() == OT_MOVER && lstrlen( static_cast<CMover*>(pObj)->GetCharacter()->m_szKey ) )
			{
				LPCHARACTER lpChar = static_cast<CMover*>(pObj)->GetCharacter();
				lpChar->m_dwWorldId = GetID();
				lpChar->m_vPos = pObj->GetPos();
			}
#endif // __QUEST_HELPER
#ifdef __FIX_NPC_GENDER
			if (pObj->GetType() == OT_MOVER)
			{
				if (lstrlen(static_cast<CMover*>(pObj)->GetCharacter()->m_szKey))
				{
					LPCHARACTER lpChar = static_cast<CMover*>(pObj)->GetCharacter();
					lpChar->m_dwWorldId = GetID();
					lpChar->m_vPos = pObj->GetPos();
					if (((CMover*)pObj)->m_dwIndex == MI_MALE || ((CMover*)pObj)->m_dwIndex == MI_FEMALE)
					{
						((CMover*)pObj)->m_bySex = ((CMover*)pObj)->m_dwIndex - MI_MALE;
						lpChar->m_dwMoverIdx = ((CMover*)pObj)->m_dwIndex;
					}
				}
				else
				{
					if (pObj->m_dwIndex == MI_MALE || pObj->m_dwIndex == MI_FEMALE)
					{
						if (((CMover*)pObj)->m_dwIndex == MI_MALE || ((CMover*)pObj)->m_dwIndex == MI_FEMALE)
							((CMover*)pObj)->m_bySex = ((CMover*)pObj)->m_dwIndex - MI_MALE;
					}
				}
			}
#endif
		}
		else
		{
			SAFE_DELETE( pObj );
			continue;
		}
	}

	return TRUE;
}
#endif // __WORLDSERVER


void RegionDesc_SaveFormatToString( wchar_t* pwszDesc )
{
	wchar_t* pwszSrc = pwszDesc;
	while( *pwszSrc )
	{
		if( *pwszSrc == '\\' && *(pwszSrc + 1 )== 'n')
		{
			*(pwszSrc + 0) = '\r';
			*(pwszSrc + 1) = '\n';
		}
		pwszSrc++;
	}
}

void RegionDesc_StringToSaveFormat( wchar_t* pwszDesc )
{
	wchar_t* pwszSrc = pwszDesc;
	while( *pwszSrc )
	{
		if( *pwszSrc == '\r' && *(pwszSrc + 1 )== '\n')
		{
			*(pwszSrc + 0) = '\\';
			*(pwszSrc + 1) = 'n';
		}
		pwszSrc++;
	}
}

BOOL CWorld::ReadRegion( CScript& s )
{
	BOOL bNewFormat = FALSE;
	BOOL bNewFormat3 = FALSE;
	if( s.Token == _T( "region2" ) )
		bNewFormat = TRUE;
	if( s.Token == _T( "region3" ) )
	{
		bNewFormat3 = TRUE;
		bNewFormat = TRUE;
	}
	DWORD dwType = s.GetNumber();
	DWORD dwIndex = s.GetNumber();
	D3DXVECTOR3 vPos;
	vPos.x = s.GetFloat();
	vPos.y = s.GetFloat();
	vPos.z = s.GetFloat();

	REGIONELEM re,* pRe = &re;
	ZeroMemory( &re, sizeof( re ) );

	pRe->m_uItemId		= 0xffffffff;
	pRe->m_uiItemCount	= 0xffffffff;
	pRe->m_uiMinLevel	= 0xffffffff;
	pRe->m_uiMaxLevel	= 0xffffffff;
	pRe->m_iQuest		= 0xffffffff;
	pRe->m_iQuestFlag	= 0xffffffff;
	pRe->m_iJob			= 0xffffffff;
	pRe->m_iGender		= 0xffffffff;
	pRe->m_bCheckParty	= FALSE;
	pRe->m_bCheckGuild	= FALSE;
	pRe->m_bChaoKey		= FALSE;
	pRe->m_dwAttribute	= s.GetNumber();
	pRe->m_dwIdMusic = s.GetNumber();
	pRe->m_bDirectMusic = (bool)(s.GetNumber() == 1);

	s.GetToken(); _tcscpy( pRe->m_szScript, s.token );
	s.GetToken(); _tcscpy( pRe->m_szSound, s.token );
	pRe->m_dwIdTeleWorld = s.GetNumber();

	pRe->m_vPos        = vPos;
	pRe->m_vTeleWorld.x = s.GetFloat();
	pRe->m_vTeleWorld.y = s.GetFloat();
	pRe->m_vTeleWorld.z = s.GetFloat();
	pRe->m_rect.left   = (LONG)s.GetNumber();
	pRe->m_rect.top	   = (LONG)s.GetNumber();
	pRe->m_rect.right  = (LONG)s.GetNumber();
	pRe->m_rect.bottom = (LONG)s.GetNumber();
	s.GetToken();
	strcpy( pRe->m_szKey, s.Token );
	pRe->m_bTargetKey  =  (bool)(s.GetNumber() == 1);
	if( bNewFormat3 )
	{
		pRe->m_uItemId		= (u_long)s.GetNumber();
		pRe->m_uiItemCount	= (UINT)s.GetNumber();
		pRe->m_uiMinLevel	= (UINT)s.GetNumber();
		pRe->m_uiMaxLevel	= (UINT)s.GetNumber();
		pRe->m_iQuest		= s.GetNumber();
		pRe->m_iQuestFlag	= s.GetNumber();
		pRe->m_iJob			= s.GetNumber();
		pRe->m_iGender		= s.GetNumber();
		pRe->m_bCheckParty	= (BOOL)s.GetNumber();
		pRe->m_bCheckGuild	= (BOOL)s.GetNumber();
		pRe->m_bChaoKey		= (BOOL)s.GetNumber();
	}

	if( bNewFormat == FALSE )
	{
		pRe->m_cDescSize = (char)( (LONG)s.GetNumber() );
		if( pRe->m_cDescSize )
		{
			pRe->m_cDescSize = 1;
			s.GetToken(); // {
			s.GetToken();

			s.Token.Replace( "\\n", "\r\n" );
			strcpy( pRe->m_szDesc, s.Token );

			s.GetToken();	// }

		}
	}
	else
	{
		s.GetToken(); // get "title"
		BOOL bDesc = s.GetNumber();
		if( bDesc )
		{
			s.GetToken(); // {
			s.GetToken();

			s.Token.Replace( "\\n", "\r\n" );
			strcpy( pRe->m_szTitle, s.Token );

			s.GetToken();	// }
		}

		s.GetToken(); // get "desc"
		bDesc = s.GetNumber();
		if( bDesc )
		{
			pRe->m_cDescSize = 1;
			s.GetToken(); // {
			s.GetToken();

			s.Token.Replace( "\\n", "\r\n" );
			strcpy( pRe->m_szDesc, s.Token );

			s.GetToken(); // }
		}
	}

	switch( dwIndex )
	{
	case RI_BEGIN:
	case RI_REVIVAL:
		break;
	case RI_STRUCTURE:
		m_aStructure.AddTail( pRe );
		break;
	default:
		{
			m_aRegion.AddTail( pRe );
			break;
		}
	}
	return TRUE;
}

BOOL CWorld::ReadRespawn( CScript& s )
{
	int nVersion = 1;
	if( s.Token == _T( "respawn2" ) )
		nVersion = 2;
	if( s.Token == _T( "respawn3" ) )
		nVersion = 3;
	//__YAIMONSTER_EX
	//__Y_PATROL
	if( s.Token == _T( "respawn4" ) )
		nVersion = 4;
	if( s.Token == _T( "respawn5" ) )
		nVersion = 5;
	if( s.Token == _T( "respawn6" ) )
		nVersion = 6;
#if __VER >= 10 // __LEGEND
	if( s.Token == _T( "respawn7" ) )
		nVersion = 7;
#endif //__LEGEND


	CRespawnInfo ri,* pInfo = &ri;
	DWORD dwType;
	DWORD dwIndex;
	D3DXVECTOR3 vPos;
	dwType = s.GetNumber();

	dwIndex = s.GetNumber();
	vPos.x = s.GetFloat();
	vPos.y = s.GetFloat();
	vPos.z = s.GetFloat();

	pInfo->m_dwIndex     = dwIndex;
	pInfo->m_dwType	= dwType;

#ifdef __S1108_BACK_END_SYSTEM
	pInfo->m_nMaxcb		 = s.GetNumber();
	pInfo->m_cb          = 0;
#else // __S1108_BACK_END_SYSTEM
	pInfo->m_cb          = (u_short)s.GetNumber();
#endif // __S1108_BACK_END_SYSTEM

	pInfo->m_uTime = (short)s.GetNumber();

#ifdef __S1108_BACK_END_SYSTEM
	pInfo->m_nMaxAttackNum = (long)s.GetNumber();
	pInfo->m_nActiveAttackNum = 0;
#else // __S1108_BACK_END_SYSTEM
	pInfo->m_nActiveAttackNum = (long)s.GetNumber();
#endif // __S1108_BACK_END_SYSTEM

	pInfo->m_vPos	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	pInfo->m_fY = vPos.y;
	if( dwType == OT_MOVER )
	{
		MoverProp* pMoverProp	= prj.GetMoverProp( dwIndex );
#ifdef __WORLDSERVER
		if( pMoverProp )
		{
			if( pMoverProp->dwClass == RANK_MATERIAL )
				return TRUE;

			if( pMoverProp->dwClass != RANK_BOSS )
			{
				switch( pMoverProp->dwID )
				{
					case MI_AIBATT1:
					case MI_AIBATT2:
					case MI_AIBATT3:
					case MI_MUSHPANG1:
					case MI_MUSHPANG2:
					case MI_MUSHPANG3:
					case MI_BURUDENG1:
					case MI_BURUDENG2:
					case MI_BURUDENG3:
						pInfo->m_uTime	= (u_short)( pInfo->m_uTime	* 0.5 );
						break;
					default:
						pInfo->m_uTime	= (u_short)( pInfo->m_uTime * 0.7 );
						break;
				}
			}
		if( pMoverProp->dwFlying )
			pInfo->m_vPos	= vPos;
		}
#endif	// __WORLDSERVER
	}
	pInfo->m_rect.left   = (LONG)s.GetNumber();
	pInfo->m_rect.top    = (LONG)s.GetNumber();
	pInfo->m_rect.right  = (LONG)s.GetNumber();
	pInfo->m_rect.bottom = (LONG)s.GetNumber();
	if( nVersion >= 2 )
	{
		pInfo->m_nDayMin = s.GetNumber();
		pInfo->m_nDayMax = s.GetNumber();
		pInfo->m_nHourMin = s.GetNumber();
		pInfo->m_nHourMax = s.GetNumber();
		pInfo->m_nItemMin = s.GetNumber();
		pInfo->m_nItemMax = s.GetNumber();
	}

	if( nVersion >= 4 )
	{
		pInfo->m_dwAiState = s.GetNumber();
		pInfo->m_fAngle    = s.GetFloat();

		if( pInfo->m_nMaxcb == 1 )
		{
			if( pInfo->m_dwAiState == 8 || pInfo->m_dwAiState == 9 )
			{
				pInfo->m_vPos = vPos;
			}
		}
	}

	if( nVersion >= 5 )
	{
		pInfo->m_dwPatrolIndex = s.GetNumber();
	}

	if( nVersion >= 6 )
	{
		pInfo->m_bPatrolCycle  = s.GetNumber();
	}

	if( nVersion >= 3 )
	{
		int nContrl = s.GetNumber();
		if( nContrl )
		{
			pInfo->m_CtrlElem.m_dwSet = s.GetNumber();
			pInfo->m_CtrlElem.m_dwSetItem = s.GetNumber();

			if (pInfo->m_CtrlElem.m_dwSet & UA_ITEM)
			{
				ItemProp* pProp = prj.GetItemProp(pInfo->m_CtrlElem.m_dwSetItem);
				if (!pProp || pProp->dwParts != NULL_ID)
				{
					Error("Error with CTRL object, wrong key. World ID: %d", GetID());
					pInfo->m_CtrlElem.m_dwSet = 0;
					pInfo->m_CtrlElem.m_dwSetItem = 0;
				}
			}

			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwSetItemCount = s.GetNumber();
			}
			else
			{
				pInfo->m_CtrlElem.m_dwSetItemCount = 1;
			}
			pInfo->m_CtrlElem.m_dwSetLevel = s.GetNumber();
			pInfo->m_CtrlElem.m_dwSetQuestNum = s.GetNumber();
			pInfo->m_CtrlElem.m_dwSetFlagNum = s.GetNumber();
			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwSetQuestNum1 = s.GetNumber();
				pInfo->m_CtrlElem.m_dwSetFlagNum1 = s.GetNumber();
				pInfo->m_CtrlElem.m_dwSetQuestNum2 = s.GetNumber();
				pInfo->m_CtrlElem.m_dwSetFlagNum2 = s.GetNumber();
			}
			pInfo->m_CtrlElem.m_dwSetGender = s.GetNumber();

			int maxjob = 0;
			if(nVersion <= 6)
				maxjob = 16;
			else
				maxjob = 32;

			for( int i = 0; i < maxjob; i++ )
				pInfo->m_CtrlElem.m_bSetJob[ i ] = s.GetNumber();

			pInfo->m_CtrlElem.m_dwSetEndu = s.GetNumber();
			pInfo->m_CtrlElem.m_dwMinItemNum = s.GetNumber();
			pInfo->m_CtrlElem.m_dwMaxiItemNum = s.GetNumber();
			for( int i = 0; i < MAX_CTRLDROPITEM; i++ )
				pInfo->m_CtrlElem.m_dwInsideItemKind[ i ] = s.GetNumber();
			for( int i = 0; i < MAX_CTRLDROPITEM; i++ )
				pInfo->m_CtrlElem.m_dwInsideItemPer[ i ] = s.GetNumber();
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonResKind[ i ] = s.GetNumber();
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonResNum[ i ] = s.GetNumber();
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonActAttack[ i ] = s.GetNumber();
			pInfo->m_CtrlElem.m_dwTrapOperType = s.GetNumber();
			pInfo->m_CtrlElem.m_dwTrapRandomPer = s.GetNumber();
			pInfo->m_CtrlElem.m_dwTrapDelay = s.GetNumber();
			for( int i = 0; i < MAX_TRAP; i++ )
				pInfo->m_CtrlElem.m_dwTrapKind[ i ] = s.GetNumber();
			for( int i = 0; i < MAX_TRAP; i++ )
				pInfo->m_CtrlElem.m_dwTrapLevel[ i ] = s.GetNumber();

			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwTeleWorldId = s.GetNumber();
				pInfo->m_CtrlElem.m_dwTeleX = s.GetNumber();
				pInfo->m_CtrlElem.m_dwTeleY = s.GetNumber();
				pInfo->m_CtrlElem.m_dwTeleZ = s.GetNumber();
			}
		}
	}

#ifdef __WORLDSERVER
#ifdef __RES0807
	if( FALSE == ri.GenResPoint( this ) )
	{
		char lpOutputString[256]	=	{ 0,};
		sprintf( lpOutputString, "type=%d, index=%d, left=%d, top=%d, right=%d, bottom=%d, num=%d",
			ri.m_dwType, ri.m_dwIndex, ri.m_rect.left, ri.m_rect.top, ri.m_rect.right, ri.m_rect.bottom, ri.m_nMaxcb );
		OutputDebugString( lpOutputString );
		return FALSE;
	}
#endif	// __RES0807
	m_respawner.Add( ri );
#endif	// __WORLDSERVER

	return TRUE;
}

BOOL CWorld::LoadRegion()
{
#if defined( __WORLDSERVER ) || defined(__CLIENT)
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.rgn", m_szFilePath, strLandName );

	CScript s;
	if( s.Load( strLandTemp, FALSE ) )
	{
		D3DXVECTOR3 vPos;
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == _T( "region" ) || s.Token == _T( "region2" ) || s.Token == _T( "region3" ) )
			{
				ReadRegion( s );
			}
			else
			if( s.Token == _T( "respawn" ) || s.Token == _T( "respawn2" ) || s.Token == _T( "respawn3" ) || s.Token == _T( "respawn4" ) || s.Token == _T( "respawn5" ) || s.Token == _T( "respawn6" ) || s.Token == _T( "respawn7" ))
			{
				ReadRespawn( s );
			}
			s.GetToken();
		}
#ifdef __LAYER_1021
		Expand();
#endif	// __LAYER_1021
		return TRUE;
	}
#endif

	return FALSE;
}


#ifdef __WORLDSERVER
BOOL CWorld::ReadWorld( const CRect & rcLandscape )
{
	CString strFileName;
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	FLOAT aHeightMap[ ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) ];
	WATERHEIGHT aWaterHeight[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	BYTE aLocation[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	int xLand, yLand;
	D3DXVECTOR3	vPos;

	CObj* pBkgnd;
	DWORD dwObjNum;

	for( int y = 0; y < m_nLandHeight; y++ )
	{
		for( int x = 0; x < m_nLandWidth; x++ )
		{
			strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLandName, x , y );

			CFileIO f;
			if( f.Open( strLandTemp, "rb" ) )
			{
				DWORD dwVersion;
				f.Read( &dwVersion, sizeof( DWORD) );
				if( dwVersion >= 1 )
				{
					f.Read( &xLand, sizeof( xLand ) );
					f.Read( &yLand, sizeof( yLand ) );
				}
				f.Read( (void*)aHeightMap, sizeof(FLOAT), ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) );

				for( int y2 = 0; y2 < MAP_SIZE; y2++ )
				{
					for( int x2 = 0; x2 < MAP_SIZE; x2++ )
					{
						m_apHeightMap[ ( ( ( y * MAP_SIZE ) + y2 ) * WORLD_WIDTH ) + ( ( x * MAP_SIZE ) + x2 ) ]
							= aHeightMap[ ( y2 * ( MAP_SIZE + 1 ) ) + x2 ];
					}
				}

				f.Read( (void*)aWaterHeight, sizeof( WATERHEIGHT ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
				for( int y2 = 0; y2 < NUM_PATCHES_PER_SIDE; y2++ )
				{
					for( int x2 = 0; x2 < NUM_PATCHES_PER_SIDE; x2++ )
					{
						m_apWaterHeight[ ( ( ( y * NUM_PATCHES_PER_SIDE ) + y2 ) * ( NUM_PATCHES_PER_SIDE * m_nLandWidth ) ) + ( ( x * NUM_PATCHES_PER_SIDE ) + x2 ) ]
							= aWaterHeight[ ( y2 * ( NUM_PATCHES_PER_SIDE ) ) + x2 ];
					}
				}
				if( dwVersion >= 2 )
				{
					// skip
					f.Read( aLocation, sizeof( BYTE ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
				}
				BYTE nLayer;
				f.Read( &nLayer, sizeof(BYTE), 1 );
				for( BYTE u = 0; u < nLayer; u++ ) {
					f.Seek( sizeof(WORD), SEEK_CUR );
					f.Seek( ( sizeof(BOOL) * NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ), SEEK_CUR );
					f.Seek( ( sizeof(DWORD) * MAP_SIZE * MAP_SIZE ), SEEK_CUR );
				}

				// read objects
				f.Read( (void*)&dwObjNum, sizeof(DWORD), 1 );
				for( DWORD dw =  0; dw < dwObjNum; dw++ )
				{
					pBkgnd	= ReadObj( &f );
					vPos	= pBkgnd->GetPos();

				//	vPos.x	+= x * LANDREALSCALE;
				//	vPos.z	+= y * LANDREALSCALE;

					vPos.x	+= x * MAP_SIZE * m_iMPU;
					vPos.z	+= y * MAP_SIZE * m_iMPU;

					pBkgnd->SetPos( vPos );

					if( pBkgnd->m_dwIndex <= 10 )
						pBkgnd->SetAngle( (float)( rand() % 360 ) );

					pBkgnd->SetIndex( D3DDEVICE, pBkgnd->m_dwIndex );

					if( pBkgnd->GetType() != OT_OBJ )
					{
						SAFE_DELETE( pBkgnd );
						continue;
					}

					if( ::GetLanguage() == LANG_JAP && pBkgnd->m_dwIndex == 360 )
					{
						SAFE_DELETE( pBkgnd );
					}
					else
					{
						if( pBkgnd->m_pModel && TRUE == pBkgnd->m_pModel->IsHaveCollObj() )
						{
							pBkgnd->SetWorld( this );
							InsertObjLink( pBkgnd );
							m_vecBackground.push_back( pBkgnd );
							pBkgnd->UpdateLocalMatrix();
						}
						else
						{
							SAFE_DELETE( pBkgnd );
						}
					}
				}
			}
			f.Close();

#ifdef __MAP_SECURITY
			g_WorldMng.AddMapCheckInfo( strLandTemp );
#endif // __MAP_SECURITY
		}
	}
	return TRUE;
}
#endif	// __WORLDSERVER

#if defined( __CLIENT )

BOOL CWorld::ReadWorld( D3DXVECTOR3 vPos, BOOL bEraseOldLand  )
{
	CString strFileName;
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );

	int x, z;
	WorldPosToLand( vPos, x, z );

	if( x == m_nCharLandPosX && z == m_nCharLandPosZ )
		return FALSE;

	DeleteObjects();

	m_nCharLandPosX = x;
	m_nCharLandPosZ = z;

	for(int i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
	{
		for(int j = x - m_nVisibilityLand; j <= ( x + m_nVisibilityLand ); j++)
		{
			if( LandInWorld( j, i ) )
			{
				CLandscape* pLand = m_apLand[ i * m_nLandWidth + j];
				if( pLand == NULL )
				{
					strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLandName, j , i );
					pLand = new CLandscape;
					pLand->m_nWorldX = j * MAP_SIZE;
					pLand->m_nWorldY = i * MAP_SIZE;
					pLand->InitDeviceObjects( m_pd3dDevice, this );

					if( pLand->LoadLandscape( strLandTemp, j, i ) == FALSE )
					{
						SAFE_DELETE(pLand);
						continue;
					}
					pLand->RestoreDeviceObjects( m_pd3dDevice );

					m_apLand[ i * m_nLandWidth + j] = pLand;

#ifdef __MAP_SECURITY
					g_WorldMng.AddMapCheckInfo( strLandTemp );
#endif // __MAP_SECURITY
				}
			}
		}
	}

	if( bEraseOldLand )
	{
		CRect rect( x - m_nVisibilityLand - 1, z - m_nVisibilityLand - 1, x + m_nVisibilityLand + 2, z + m_nVisibilityLand + 2 );
		for( int i = 0; i < m_nLandHeight; i++ )
		{
			for( int j = 0; j < m_nLandWidth; j++ )
			{
				CLandscape* pLand = m_apLand[ i * m_nLandWidth + j];
				if( pLand )
				{
					if( rect.PtInRect( CPoint( j, i ) ) == FALSE )
					{
						m_apLand[ i * m_nLandWidth + j ]->InvalidateDeviceObjects();
						m_apLand[ i * m_nLandWidth + j ]->DeleteDeviceObjects();
						SAFE_DELETE( m_apLand[ i * m_nLandWidth + j] );
						m_nObjCullSize = 0;
						m_nSfxCullSize = 0;
					}
					else
						pLand->SetUsedAllObjects();
				}

			}
		}


		for( MapStrToPtrItor itor = prj.m_modelMng.m_mapFileToMesh.begin(); itor != prj.m_modelMng.m_mapFileToMesh.end(); /*itor++*/ )
		{
			CModel* pModel = ( CModel* )(*itor).second;
			pModel->DeleteDeviceObjects();
			if( pModel->m_pModelElem->m_bUsed == FALSE &&  pModel->m_pModelElem->m_dwType != OT_ITEM )
			{
				SAFE_DELETE( pModel );
				prj.m_modelMng.m_mapFileToMesh.erase( itor++ );
			}
			else
			{
				pModel->m_pModelElem->m_bUsed = FALSE;
				++itor;
			}
		}
	}
	return TRUE;
}

void	CWorld::DeleteObjects()
{
	ProcessAllSfx();

	for (int i = 0; i < m_nDeleteObjs; i++)
	{
		CObj* pObj = m_apDeleteObjs[i];
		if (!pObj)
		{
			Error("m_apDeleteObjs %d is NULL", i);
			continue;
		}

		if (m_pObjFocus == pObj)
			SetObjFocus(NULL);


		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
		if (pWndWorld)
		{
			if (pWndWorld->m_pSelectRenderObj == pObj)
				pWndWorld->m_pSelectRenderObj = NULL;
			else if (pWndWorld->m_pNextTargetObj == pObj)
				pWndWorld->m_pNextTargetObj = NULL;
		}

		if (CObj::m_pObjHighlight == pObj)
			CObj::m_pObjHighlight = NULL;

		if (pObj->m_ppViewPtr)
		{
			*pObj->m_ppViewPtr = NULL;
			pObj->m_ppViewPtr = NULL;
		}

		RemoveObjLink(pObj);
		RemoveObjArray(pObj);
		SAFE_DELETE(pObj);
	}

	m_nDeleteObjs = 0;
	memset(m_apDeleteObjs, 0, sizeof(m_apDeleteObjs));
}
#endif // __CLIENT


#ifdef __WORLDSERVER
BOOL CWorld::IsUsableDYO( CObj* pObj )
{
	if( pObj->GetType() == OT_MOVER )
	{
		LPCHARACTER pCharacter	= ( (CMover*)pObj )->GetCharacter();
		if( pCharacter )
		{
			if( stricmp( pCharacter->m_szKey, "MaFl_GuildWar" ) == 0
					|| stricmp( pCharacter->m_szKey, "MaFl_Donaris" ) == 0 )
			{
				if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == FALSE )
					return FALSE;
			}

#ifdef __JEFF_11_4
			if( stricmp( pCharacter->m_szKey, "MaFl_Ray" ) == 0 )
 			{
 				if( g_eLocal.GetState( EVE_ARENA ) == FALSE )
 					return FALSE;
 			}
#endif // __JEFF_11_4

			if( stricmp( pCharacter->m_szKey, "MaFl_SecretRoom_EAST" ) == 0
				|| stricmp( pCharacter->m_szKey, "MaDa_SecretRoom_WEST" ) == 0 )
			{
				if( g_eLocal.GetState( EVE_SECRETROOM ) == FALSE )
					return FALSE;
			}

			// don't load designer & artist in taiwan & japan

			if( ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK )// || ::GetLanguage() == LANG_JAP )
			{
				if( stricmp( pCharacter->m_szKey, "MaFl_HairShop" ) == 0
					|| stricmp( pCharacter->m_szKey, "MaFl_FaceOff" ) == 0 )
					return FALSE;
			}

#ifdef __CHIPI_DYO
			return IsUsableDYO2( pCharacter );
#endif // __CHIPI_DYO

		}
	}
	return TRUE;
}


#ifdef __CHIPI_DYO
BOOL CWorld::IsUsableDYO2( LPCHARACTER pCharacter )
{
	if( pCharacter->m_vecdwLanguage.empty() )
		return pCharacter->bOutput;

	for( DWORD i=0; i<pCharacter->m_vecdwLanguage.size(); i++ )
	{
		if( HIWORD(pCharacter->m_vecdwLanguage[i]) == ::GetLanguage()
			&& LOWORD(pCharacter->m_vecdwLanguage[i]) == ::GetSubLanguage() )
			return pCharacter->bOutput;
	}

	return !(pCharacter->bOutput);
}
#endif // __CHIPI_DYO
#endif	// __WORLDSERVER

#ifdef __LAYER_1021
BOOL CWorld::CreateLayer( int nLayer )
{
//	ASSERT( IsExpandable() );
//	if( !IsExpandable() )
//		return FALSE;
	ASSERT( nLayer != 0 );
	if( nLayer == 0 )
		return FALSE;
	if( !m_linkMap.CreateLinkMap( nLayer ) )
		return FALSE;
	if( !LoadObject( nLayer ) )
	{
		m_linkMap.DeleteLinkMap( nLayer );
		return FALSE;
	}
	m_respawner.Expand( nLayer );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL CWorld::HasNobody( int nLayer )
{
	if( !HasNobody_Process( nLayer ) )
		return FALSE;
	if( !HasNoObj_Add( nLayer ) )
		return FALSE;
	if( !HasNobody_Replace( nLayer ) )
		return FALSE;
	return TRUE;
}

BOOL CWorld::HasNobody_Process( int nLayer )
{
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		if( IsLayerPlayer( m_apObject[i], nLayer ) )
			return FALSE;
	}
	return TRUE;
}

BOOL CWorld::HasNoObj_Add( int nLayer )
{
	for( int i = 0; i < m_cbAddObjs; i++ )
	{
		if( m_apAddObjs[i] && m_apAddObjs[i]->GetLayer() == nLayer )
			return FALSE;
	}
	return TRUE;
}

BOOL CWorld::HasNobody_Replace( int nLayer )
{
#if __VER >= 15 // __GUILD_HOUSE
	return g_WorldMng.HasNobody_Replace( GetID(), nLayer );
#else // __GUILD_HOUSE
	for( int i = 0; i < m_cbReplaceObj; ++i )
	{
		if( m_aReplaceObj[i].nLayer == nLayer )
			return FALSE;
	}
#endif // __GUILD_HOUSE
	return TRUE;
}

BOOL CWorld::IsLayerPlayer( CObj* pObj, int nLayer )
{
	return ( pObj && pObj->GetLayer() == nLayer && pObj->GetType() == OT_MOVER && static_cast<CMover*>( pObj )->IsPlayer() );
}

void CWorld::DriveOut( int nLayer )
{
#ifdef __WORLDSERVER
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		if( IsLayerPlayer( m_apObject[i], nLayer ) )
		{
			CUser* pUser	= static_cast<CUser*>( m_apObject[i] );
			switch( GetID() )
			{
#if __VER >= 13 // __HOUSING
				case WI_WORLD_MINIROOM : CHousingMng::GetInstance()->GoOut( pUser ); break;
#endif // __HOUSING
				default :
#if __VER >= 14 // __INSTANCE_DUNGEON
					if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( GetID() ) )
					{
						CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
						break;
					}
#endif // __INSTANCE_DUNGEON
#if __VER >= 15 // __GUILD_HOUSE
					else if( GuildHouseMng->IsGuildHouse( GetID() ) )
					{
						//pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, pUser->m_vMarkingPos, REPLACE_FORCE, nDefaultLayer );
						Invalidate( nLayer, FALSE );
						break;
					}
#endif // __GUILD_HOUSE
					pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ), REPLACE_FORCE, nDefaultLayer );
					break;
			}
		}
	}
#endif	// __WORLDSERVER
}

BOOL CWorld::ReleaseLayer( int nLayer )
{
	DestroyObj( nLayer );
	m_linkMap.DeleteLinkMap( nLayer );
	m_respawner.Release( nLayer );
	return TRUE;
}

void CWorld::DestroyObj( int nLayer )
{
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		CObj* pObj	= m_apObject[i];
		if( pObj && pObj->GetLayer() == nLayer )
			DestroyObj( pObj );
	}
}
//////////////////////////////////////////////////////////////////////////
#endif	// __LAYER_1021