// Exchange.h: interface for the CExchange class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCHANGE_H__24DC28E3_B7E1_44DE_99C9_197B1B1F4AA4__INCLUDED_)
#define AFX_EXCHANGE_H__24DC28E3_B7E1_44DE_99C9_197B1B1F4AA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __TRADESYS
class CExchange
{
public:
	enum { EXCHANGE_SUCCESS, EXCHANGE_FAILED, EXCHANGE_INVENTORY_FAILED, EXCHANGE_CONDITION_FAILED, EXCHANGE_NPC_ERROR };

	struct __ITEM
	{
		DWORD	dwItemId;
		int		nItemNum;
		int		nPayProb;
		BYTE	byFalg;
		__ITEM()
		{
			dwItemId = NULL_ID;
			nItemNum = 0;
			nPayProb = 0;
			byFalg = 0;
		};
	};

	struct __SET
	{
		UINT	nSetTextId;
		vector<int>    vecResultMsg;
		vector<__ITEM> vecCondItem;
		vector<__ITEM> vecRemoveItem;
		vector<__ITEM> vecPayItem;
		int		nPayNum;
		__SET()
		{
			nSetTextId = NULL_ID;
			nPayNum = 0;
		};
	};

	typedef struct __SETLIST
	{
		vector<int>		vecDesciprtionId;
		vector<__SET>	vecSet;
	} SETLIST, *PSETLIST;

	CExchange();
	virtual ~CExchange();

	BOOL			Load_Script();					// Load Script
	vector<int>		GetListTextId( int nMMIId );
	vector<int>		GetDescId( int nMMIId );
	vector<int>		GetResultMsg( int nMMIId, int nListNum );
	PSETLIST		FindExchange( int nMMIID );
#ifdef __WORLDSERVER
	int				ResultExchange(CUser* pUser, int nMMIid, int nListNum);
	BOOL			CheckCondition( CUser* pUser, int nMMIId, int nListNum );
	BOOL			IsFull( CUser* pUser, vector<__ITEM> vecRemoveItem, vector<__ITEM> vecPayItem );
	vector<__ITEM>	GetRemoveItemList( int nMMIId, int nListNum );
	vector<__ITEM>	GetPayItemList( int nMMIId, int nListNum );
#endif // __WORLDSERVER

	map<int, __SETLIST> m_mapExchange;
};

#endif // __TRADESYS

#endif // !defined(AFX_EXCHANGE_H__24DC28E3_B7E1_44DE_99C9_197B1B1F4AA4__INCLUDED_)
