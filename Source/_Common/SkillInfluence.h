#ifndef __SKILLINFLUENCE_H__
#define	__SKILLINFLUENCE_H__

#define	BUFF_ITEM	0
#define	BUFF_SKILL	1
#define	BUFF_PET	2
#define	BUFF_ITEM2	3
#define	BUFF_EQUIP	4
#define	BUFF_NULL_ID	(WORD)0xFFFF
#define	MAX_SKILLINFLUENCE	64
#define	MAX_SKILLBUFF_COUNT	35

#ifndef __BUFF_1107

#include "ar.h"

typedef	struct	tagSKILLINFLUENCE
{
	WORD	wType;
	WORD	wID;
	DWORD	dwLevel;
	DWORD	tmCount;
	DWORD	tmTime;
	BOOL	bEffect;
#ifdef __PVPDEBUFSKILL
	DWORD	dwAttackerID;
#endif // __PVPDEBUFSKILL
}

SKILLINFLUENCE, * LPSKILLINFLUENCE;

class CMover;
class CSkillInfluence
{
private:
#if __VER < 8 //__CSC_VER8_3
	SKILLINFLUENCE *m_pEmptyNode;
#endif //__CSC_VER8_3
	CMover* m_pMover;
//	CRIT_SEC	m_AddRemoveLock;

	void	RemoveSkillInfluence( SKILLINFLUENCE* pSkillInfluence );

public:
//	BOOL	IsEmpty();
#if !( defined( __CORESERVER ) || defined( __DBSERVER ) )
#ifdef __PVPDEBUFSKILL
	BOOL	RemoveSkillInfluenceFromID( OBJID dwAttackerID );
#endif // __PVPDEBUFSKILL
	BOOL	RemoveSkillInfluence( WORD wType, WORD wID );
	BOOL	RemoveSkillInfluenceState( DWORD dwChrState );
#if __VER >= 11
	BOOL	RemoveSkillInfluenceDestParam( DWORD dwDestParam );
#endif

	BOOL    RemoveAllSkillInfluence();
	BOOL	RemoveAllSkillDebuff( void );
	BOOL	RemoveAllSkillBuff( void );
#if __VER >= 11
	BOOL	RemoveAllBuff( void );
#endif
	BOOL	RemoveOneSkillBuff( void );


#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) )
//	Constructions
	CSkillInfluence();
	virtual	~CSkillInfluence();

	SKILLINFLUENCE		m_aSkillInfluence[ MAX_SKILLINFLUENCE ];

	LPSKILLINFLUENCE	FindPtr( WORD wType, WORD wID );
	void	Init( void );
	void	Destroy( void ) {}

	void	Serialize( CAr & ar );

//	Operations
	void	SetMover( CMover* pMover );
#ifdef __PVPDEBUFSKILL
	BOOL	Set( WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime, DWORD dwAttackerID = NULL_ID );
	BOOL	Set( SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime, DWORD dwAttackerID );
#else // __PVPDEBUFSKILL
	BOOL	Set( WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime );
	BOOL	Set( SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime );
#endif // __PVPDEBUFSKILL

	LPSKILLINFLUENCE	Get( int nIdx );
#if !( defined( __CORESERVER ) || defined( __DBSERVER ) )
	void	Process( void );
#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) )
	void	Reset( void );
	BOOL	HasSkill( WORD wType, WORD wID );

#if __VER >= 9	// __PET_0410
	BOOL	HasPet( void )	{	return FindPet() != NULL;	}
	SKILLINFLUENCE *	FindPet( void );
	BOOL	RemovePet( void );
#endif	// __PET_0410

#ifdef __CLIENT
	DWORD	GetDisguise( void );
#endif // __CLIENT
	BOOL	HasLikeItemBuf( DWORD dwItemKind3 );
	void	RemoveLikeItemBuf( DWORD dwItemKind3 );
	SKILLINFLUENCE *Find( WORD wType, WORD wID );
	SKILLINFLUENCE *GetItemBuf( DWORD dwItemKind3 );

private:
//	Attributes
	void	Remove( SKILLINFLUENCE *pNode );
	BOOL	LikeItemBuf( DWORD dwItemKind3 );
#if __VER >= 8 //__CSC_VER8_3
	SKILLINFLUENCE* SortSkillArray();
#ifdef __PVPDEBUFSKILL
	BOOL InsertBuff(SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime, DWORD dwAttackerID);
#else // __PVPDEBUFSKILL
	BOOL InsertBuff(SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime );
#endif // __PVPDEBUFSKILL
#endif //__CSC_VER8_3
};


inline void CSkillInfluence::SetMover( CMover* pMover )
	{	m_pMover	= pMover;	}


inline SKILLINFLUENCE* CSkillInfluence::Find( WORD wType, WORD wID )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;

	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == wType && pNode->wID == wID )
			return pNode;
	}

	return NULL;
}

#if __VER >= 9	// __PET_0410
inline SKILLINFLUENCE* CSkillInfluence::FindPet( void )
{
	int	i	= MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	while( i-- )
	{
		pNode	= pList++;
		if( pNode->wType == BUFF_PET )
			return pNode;
	}
	return NULL;
}
#endif	// __PET_0410


inline BOOL CSkillInfluence::HasSkill( WORD wType, WORD wID )
{
	return Find( wType, wID ) != NULL;
}


inline LPSKILLINFLUENCE CSkillInfluence::FindPtr( WORD wType, WORD wID )
{
	return( Find( wType, wID ) );
}


inline LPSKILLINFLUENCE CSkillInfluence::Get( int nIdx )
{
	if( nIdx < 0 || nIdx >= MAX_SKILLINFLUENCE )
	{
		Error( "SKILLINFLUENCE::Get() : 범위를 넘어섬 %d", nIdx );
		return( NULL );
	}

	return( m_aSkillInfluence + nIdx );
}


inline void CSkillInfluence::Remove( SKILLINFLUENCE *pNode )
{
	pNode->wType = 0;
	pNode->wID = 0;
	pNode->bEffect = 0;
	pNode->tmTime = 0;
	pNode->tmCount = 0;
#if __VER < 8 //__CSC_VER8_3
	m_pEmptyNode = pNode;
#endif //__CSC_VER8_3
}

#endif	// __BUFF_1107

#endif	// __SKILLINFLUENCE_H__