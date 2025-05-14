// MiniGame.cpp: implementation of the CMiniGame class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiniGame.h"
#ifdef __WORLDSERVER
#include "User.h"
#include "DPSrvr.h"
#include "dpdatabaseclient.h"
#endif // __WORLDSERVER
#include "defineText.h"

#ifdef __WORLDSERVER
extern	CDPSrvr		g_DPSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
#endif // __WORLDSERVER


CMiniGame::CMiniGame()
{
	__KAWIBAWIBORESULT KawibawiboResult;

	KawibawiboResult.nMy = KAWI;
	KawibawiboResult.nYou	= BAWI;
	KawibawiboResult.nResult = KAWIBAWIBO_LOST;
	m_vecKawibawiboResult.push_back( KawibawiboResult );
	KawibawiboResult.nYou	= BO;
	KawibawiboResult.nResult = KAWIBAWIBO_WIN;
	m_vecKawibawiboResult.push_back( KawibawiboResult );


	KawibawiboResult.nMy = BAWI;
	KawibawiboResult.nYou	= KAWI;
	KawibawiboResult.nResult = KAWIBAWIBO_WIN;
	m_vecKawibawiboResult.push_back( KawibawiboResult );
	KawibawiboResult.nYou	= BO;
	KawibawiboResult.nResult = KAWIBAWIBO_LOST;
	m_vecKawibawiboResult.push_back( KawibawiboResult );


	KawibawiboResult.nMy = BO;
	KawibawiboResult.nYou	= KAWI;
	KawibawiboResult.nResult = KAWIBAWIBO_LOST;
	m_vecKawibawiboResult.push_back( KawibawiboResult );
	KawibawiboResult.nYou	= BAWI;
	KawibawiboResult.nResult = KAWIBAWIBO_WIN;
	m_vecKawibawiboResult.push_back( KawibawiboResult );

	m_nBetMinPenya = 1000000;
	m_nBetMaxPenya = 100000000;
	m_nMultiple = 5;
}

CMiniGame::~CMiniGame()
{

}

#ifdef __WORLDSERVER
int CMiniGame::Result_Kawibawibo( CUser* pUser )
{
	CItemElem* pItemElem = NULL;
	if( pUser->m_nKawibawiboState != KAWIBAWIBO_DRAW && pUser->m_nKawibawiboState != KAWIBAWIBO_WIN )
	{
		pItemElem	= pUser->m_Inventory.GetAtItemId( II_SYS_SYS_EVE_KAWIBAWIBO );
		if( IsUsableItem( pItemElem ) == FALSE )
			return KAWIBAWIBO_FAILED;
	}

	__KAWIBAWIBO Kawibawibo = FindKawibawibo( pUser->m_nKawibawiboWin );

	if( Kawibawibo.nItemCount != 0 )
	{
		if( pUser->m_nKawibawiboState != KAWIBAWIBO_DRAW && pUser->m_nKawibawiboState != KAWIBAWIBO_WIN )
		{
#ifndef __FL_FIX_USELESS_LOGS
			LogItemInfo aLogItem;
			aLogItem.Action = "+";
			aLogItem.SendName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.RecvName = "KAWIBAWIBO_USE_ITEM";
			g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );
#endif

			pUser->RemoveItem( (BYTE)( pItemElem->m_dwObjId ), (short)1 );
		}


		if( pUser->m_nKawibawiboWin >= nKawiBawiBo_MaxWin )
		{
			pUser->m_nKawibawiboWin = 0;

#ifndef __FL_FIX_USELESS_LOGS
			LogItemInfo aLogItem;
			aLogItem.Action = "+";
			aLogItem.SendName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.RecvName = "KAWIBAWIBO_LOST";
			g_DPSrvr.OnLogItem( aLogItem );
#endif

			return KAWIBAWIBO_LOST;
		}

		int nRandom = xRandom( 1000000 );

		if( (DWORD)( nRandom ) < Kawibawibo.dwDraw )	// Draw
		{
#ifndef __FL_FIX_USELESS_LOGS
			LogItemInfo aLogItem;
			aLogItem.Action = "+";
			aLogItem.SendName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.RecvName = "KAWIBAWIBO_DRAW";
			g_DPSrvr.OnLogItem( aLogItem );
#endif

			return KAWIBAWIBO_DRAW;
		}

		else if( (DWORD)( nRandom ) < Kawibawibo.dwDraw + Kawibawibo.dwLost ) // Lost
		{
			pUser->m_nKawibawiboWin = 0;

#ifndef __FL_FIX_USELESS_LOGS
			LogItemInfo aLogItem;
			aLogItem.Action = "+";
			aLogItem.SendName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.RecvName = "KAWIBAWIBO_LOST";
			g_DPSrvr.OnLogItem( aLogItem );
#endif

			return KAWIBAWIBO_LOST;
		}

		++pUser->m_nKawibawiboWin;

#ifndef __FL_FIX_USELESS_LOGS
		LogItemInfo aLogItem;
		aLogItem.Action = "+";
		aLogItem.SendName = pUser->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		aLogItem.RecvName = "KAWIBAWIBO_WIN";
		aLogItem.Gold_1 = pUser->m_nKawibawiboWin;
		g_DPSrvr.OnLogItem( aLogItem );
#endif

		return KAWIBAWIBO_WIN;
	}
	return KAWIBAWIBO_FAILED;
}

BOOL CMiniGame::ResultItem_Kawibawibo( CUser* pUser )
{
	if( 0 >= pUser->m_nKawibawiboWin )
		return FALSE;

	__KAWIBAWIBO Kawibawibo = FindKawibawibo( pUser->m_nKawibawiboWin );
	if( Kawibawibo.nItemCount != 0 )
	{
		LogItemInfo aLogItem;
		aLogItem.Gold_1 = pUser->m_nKawibawiboWin;

		pUser->m_nKawibawiboWin = 0;
		pUser->m_nKawibawiboState = KAWIBAWIBO_GETITEM;

		CItemElem itemElem;
		itemElem.m_dwItemId = Kawibawibo.dwItemId;
		itemElem.m_nItemNum		= Kawibawibo.nItemCount;
		itemElem.SetSerialNumber();
		ItemProp* pItemProp		= itemElem.GetProp();
		if( pItemProp )
			itemElem.m_nHitPoint	= pItemProp->dwEndurance;
		else
			itemElem.m_nHitPoint	= 0;

		if( pUser->CreateItem( &itemElem ) )
		{
			aLogItem.RecvName = "KAWIBAWIBO_PRIZE";
			pUser->AddKawibawiboResult( KAWIBAWIBO_GETITEM, aLogItem.Gold_1, Kawibawibo.dwItemId, Kawibawibo.nItemCount );
		}
		else
		{
			aLogItem.RecvName = "KAWIBAWIBO_PRIZE_POST";
			g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, itemElem.GetProp()->szName, (char*)GETTEXT( TID_MMI_KAWIBAWIBO ) );
			pUser->AddDefinedText( TID_GAME_MINIGAME_ITEM_POST, "" );
		}
		aLogItem.Action = "+";
		aLogItem.SendName = pUser->GetName();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
#ifndef __FL_FIX_USELESS_LOGS
		g_DPSrvr.OnLogItem( aLogItem, &itemElem, Kawibawibo.nItemCount );
#endif
	}
	return TRUE;
}

CMiniGame::__KAWIBAWIBO CMiniGame::FindKawibawibo(int nWinCount)
{
	__KAWIBAWIBO Kawibawibo;
	int nUserWin = nWinCount;
	if (0 <= nUserWin)
	{
		if (nKawiBawiBo_MaxWin < nUserWin)
			nUserWin = nKawiBawiBo_MaxWin;

		map<int, __KAWIBAWIBO>::iterator it = m_mapKawiBawiBo.find(nUserWin);
		if (it != m_mapKawiBawiBo.end())
			Kawibawibo = it->second;
	}

	return Kawibawibo;
}
#endif // __WORLDSERVER

int CMiniGame::GetKawibawiboYou(int nMy, int nResult)
{
	for (DWORD i = 0; i < m_vecKawibawiboResult.size(); ++i)
	{
		if (nMy == m_vecKawibawiboResult[i].nMy && nResult == m_vecKawibawiboResult[i].nResult)
			return m_vecKawibawiboResult[i].nYou;
	}

	return nMy;
}

BOOL CMiniGame::Load_KawiBawiBo()
{
	CScanner s;

	if (!s.Load("MiniGame_KawiBawiBo.txt", 0))
		return FALSE;

	s.GetToken();

	int nMaxWin = 0;
	while (s.tok != FINISHED)
	{
		if (s.Token == _T("SENDITEM"))
		{
			__KAWIBAWIBO	KawiBawiBo;

			int nWinCount = s.GetNumber();
			KawiBawiBo.dwDraw = s.GetNumber();
			KawiBawiBo.dwLost = s.GetNumber();
			KawiBawiBo.dwWin = s.GetNumber();
			s.GetToken();
			KawiBawiBo.dwItemId = CScript::GetDefineNum(s.Token);
			if (KawiBawiBo.dwItemId == -1)
			{
				Error("CMiniGame::Load_KawiBawiBo() 아이템이 없는것을 설정했음");
				return FALSE;
			}

			KawiBawiBo.nItemCount = s.GetNumber();

			m_mapKawiBawiBo.insert(make_pair(nWinCount, KawiBawiBo));
			//			m_mapKawiBawiBo.insert( map<int, __KAWIBAWIBO>::value_type( nWinCount, KawiBawiBo ) );

			if (nMaxWin < nWinCount)
				nMaxWin = nWinCount;
		}
		s.GetToken();
	}

	nKawiBawiBo_MaxWin = nMaxWin;
	return TRUE;
}
