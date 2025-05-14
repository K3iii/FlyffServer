#ifndef __MESSENGER_H__
#define	__MESSENGER_H__

#include "Ar.h"

#define FRS_ONLINE		0
#define FRS_OFFLINE		1
#define FRS_BLOCK	2
#define FRS_ABSENT	3
#define FRS_HARDPLAY	4
#define FRS_EAT		5
#define FRS_REST	6
#define FRS_MOVE	7
#define FRS_DIE		8
#define FRS_DANGER		9
#define FRS_OFFLINEBLOCK	10
#define MAX_FRIENDSTAT		11

#define MAX_FRIEND		200

typedef struct tagFRIEND
{
	BOOL bGroup;
	DWORD dwState;
	DWORD dwUserId;
	BOOL  bSave;
#ifdef __CLIENT
	u_long m_uIdofMulti;
#endif // __CLIENT

	tagFRIEND()
	{
		bGroup = FALSE;
		dwState = FRS_OFFLINE;
		dwUserId = 0;
		bSave = FALSE;
#ifdef __CLIENT
		m_uIdofMulti = 100;
#endif // __CLIENT
	}
} FRIEND,* LPFRIEND;

typedef	map< u_long, FRIEND*>	C2FriendPtr;

class CMessenger
{
public:
	C2FriendPtr m_aFriend;
	C2FriendPtr m_adifferntFriend;
	DWORD		m_dwMyState;

public:
	BOOL		AddFriend( u_long uidPlayer );
	BOOL		AddFriend( u_long uidPlayer, DWORD dwSate, BOOL bSave = FALSE );
	void		SetSave( void );
	BOOL		RemoveFriend( u_long uidPlayer );
	BOOL		RemoveDifferntFriend( u_long uidPlayer );
	LPFRIEND	GetFriend( u_long uidPlayer );
	LPFRIEND	GetDefferntFriend( u_long uidPlayer );
	BOOL		IsFriend( u_long uidPlayer );
	BOOL		IsDefferntFriend( u_long uidPlayer );
	BOOL		AddDefferntFriend( u_long uidPlayer );
	void		Clear( void );
	void		Serialize( CAr & ar );
	DWORD		GetState() { return m_dwMyState; };
	LPFRIEND    GetAt( int nIndex );
	int			GetSize();
	virtual	CMessenger&	CMessenger::operator=( CMessenger & rMessenger );

	CMessenger();
	~CMessenger();
};

#endif	//	_MESSENGER_H