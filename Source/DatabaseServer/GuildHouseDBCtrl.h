// GuildHouseDBCtrl.h: interface for the CGuildHouseDBCtrl class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_GUILDHOUSEDBCTRL_H__C2521D9F_8544_4454_810A_F334CA03AABC__INCLUDED_)
#define AFX_GUILDHOUSEDBCTRL_H__C2521D9F_8544_4454_810A_F334CA03AABC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 15 // __GUILD_HOUSE
#include "dbcontroller.h"
#include "GuildHouse.h"

enum
{
	GUILDHOUSE_LOAD = 0,
	GUILDHOUSE_SEND,
	GUILDHOUSE_CREATE,
	GUILDHOUSE_REMOVE,
	GUILDHOUSE_DEFAULT_PACKET,
};

class CGuildHouseDBCtrl : public CDbController
{
public:
	CGuildHouseDBCtrl();
	virtual ~CGuildHouseDBCtrl();

	virtual	void Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );
	virtual void OnTimer();

private:
	BOOL	LoadGuildHouse() { return GuildHouseMng->DBLoadGuildHouse( GetQueryObject() ); }
	void	SendGuildHouse( DPID dpId );
	void	CreateGuildHouse( CAr & ar, DPID dpId );
	void	RemoveGuildHouse( DWORD dwGuildId );
	void	RunDefaultPacket( CAr & ar, DPID dpId );
	BOOL	ExcuteQuery( DWORD dwGuildId, CGuildHouseBase* pGuildHouse, int nPacketType, GH_Fntr_Info & gfi, int nIndex );
};


#define GuildHouseDBMng CGuildHouseDBMng::GetInstance()
class CGuildHouseDBMng
{
public:
	CGuildHouseDBMng();
	~CGuildHouseDBMng();
	static CGuildHouseDBMng* GetInstance();

	BOOL PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 )
	{ return m_GHDBCtrl.PostRequest( nQuery, lpBuf, nBufSize, dwCompletionKey );	}

private:
	CGuildHouseDBCtrl	m_GHDBCtrl;
};

#endif // __GUILD_HOUSE
#endif // !defined(AFX_GUILDHOUSEDBCTRL_H__C2521D9F_8544_4454_810A_F334CA03AABC__INCLUDED_)
