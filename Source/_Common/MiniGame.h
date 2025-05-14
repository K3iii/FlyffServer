// MiniGame.h: interface for the CMiniGame class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_MINIGAME_H__79AC4FCB_D41A_4407_ACF3_0FDF796F2ED1__INCLUDED_)
#define AFX_MINIGAME_H__79AC4FCB_D41A_4407_ACF3_0FDF796F2ED1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __EVE_MINIGAME

class CUser;
class CMiniGame
{
public:
	enum { KAWIBAWIBO_WIN, KAWIBAWIBO_LOST, KAWIBAWIBO_DRAW, KAWIBAWIBO_FAILED, KAWIBAWIBO_GETITEM	};
	enum { KAWI, BAWI, BO };


	struct __KAWIBAWIBORESULT
	{
		int		nMy;
		int		nYou;
		int		nResult;
	};

	struct __KAWIBAWIBO
	{
		DWORD	dwDraw;
		DWORD	dwLost;
		DWORD	dwWin;
		DWORD	dwItemId;
		int		nItemCount;
		__KAWIBAWIBO()
		{
			dwDraw = 0;
			dwLost = 0;
			dwWin = 0;
			dwItemId = 0;
			nItemCount = 0;
		};
	};

public:
	CMiniGame();
	virtual ~CMiniGame();

	BOOL Load_KawiBawiBo();
	int	GetKawibawiboYou( int nMy, int nResult );
#ifdef __WORLDSERVER
	int Result_Kawibawibo( CUser* pUser );
	BOOL ResultItem_Kawibawibo( CUser* pUser );
	__KAWIBAWIBO FindKawibawibo( int nWinCount );
#endif // __WORLDSERVER

	int m_nBetMinPenya;
	int m_nBetMaxPenya;
	int m_nMultiple;
	int nKawiBawiBo_MaxWin;

	map<int, __KAWIBAWIBO>  m_mapKawiBawiBo;
	vector<__KAWIBAWIBORESULT> m_vecKawibawiboResult;
};

#endif // __EVE_MINIGAME
#endif // !defined(AFX_MINIGAME_H__79AC4FCB_D41A_4407_ACF3_0FDF796F2ED1__INCLUDED_)
