#ifndef __FLYFFEVENT_H__
#define	__FLYFFEVENT_H__

enum
{
	ENABLE_GUILD_INVENTORY,
	EVE_WORMON,
	EVE_RESPAWN,
	EVE_STEAL,
	EVE_GUILDCOMBAT,
	EVE_DROPITEMREMOVE,
#ifdef __JEFF_11_4
	EVE_ARENA,
#endif // __JEFF_11_4
	EVE_SECRETROOM,
	EVE_RANGDA,


	
	
	EVE_MAX // Max event
};

#define	MAX_EVENT	EVE_MAX + 1

class CFlyffEvent
{
private:
	BYTE	m_aEvent[MAX_EVENT];
public:
//	Constructions
	CFlyffEvent();
	~CFlyffEvent()	{}
//	Operations
	BOOL	SetState( int id, BYTE nState );
	BYTE	GetState( int id );
	BOOL	ClearEvent( int id )	{	return SetState( id, 0 );	}

	void	Serialize( CAr & ar );
};

#endif	// __FLYFFEVENT_H__