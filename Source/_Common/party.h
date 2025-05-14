#ifndef __PARTY_H__
#define	__PARTY_H__

#ifdef __CORESERVER
#include "Ar.h"
#include "projectcmn.h"
#endif // __CORESERVER

//#ifdef __WORLDSERVER
//#include "User.h"
//extern	CUserMng	g_UserMng;
//#endif // worldserver

#include "mempooler.h"
#include <map>


// 1, 2, 4, 8, 10, 12
#define PARTY_LINKATTACK_MODE		0
#define PARTY_FORTUNECIRCLE_MODE	1
#define PARTY_STRETCHING_MODE		2
#define PARTY_GIFTBOX_MODE			3

#define PARTY_PARSKILL_MODE			4
#define MAX_PARTYMODE				5

#define	PP_REMOVE	0

#define	MAX_PTMEMBER_SIZE			15
#define MAX_PTMEMBER_SIZE_SPECIAL	8

#define PARTY_MAP_SEC	3
#define PARTY_MAP_LENGTH	10

#ifdef __FL_PARTY_FINDER
struct PARTY_LIST
{
	// Leader info
	DWORD m_dwLeaderId;
	int m_nLeaderLevel;
	int m_nLeaderJob;
	CString m_strLeaderName;

	// Party info
	DWORD m_dwPartyId;
	int m_nPartyMembers;
	int m_nPartyLevel;
	int m_nPartyPoint;

	// Skills/Scrolls
	BOOL m_bLink;
	BOOL m_bDrop;
	BOOL m_bGift;
	BOOL m_bResting;
	BOOL m_bRedScroll;
	BOOL m_bOrangeScroll;
	BOOL m_bGreenScroll;

	PARTY_LIST() { Init(); };
	~PARTY_LIST() {};

	void Init()
	{
		m_dwLeaderId = NULL_ID;
		m_nLeaderLevel = 1;
		m_nLeaderJob = JOB_VAGRANT;
		m_strLeaderName = "";

		m_dwPartyId = NULL_ID;
		m_nPartyMembers = 0;
		m_nPartyLevel = 1;
		m_nPartyPoint = 0;

		m_bLink = FALSE;
		m_bDrop = FALSE;
		m_bGift = FALSE;
		m_bResting = FALSE;
		m_bRedScroll = FALSE;
		m_bOrangeScroll = FALSE;
		m_bGreenScroll = FALSE;
	};
};
#endif

class CParty;
extern	CParty		g_Party;

typedef	struct	_PartyMember
{
	u_long	m_uPlayerId;
	CTime	m_tTime;
	BOOL	m_bRemove;
#if defined( __WORLDSERVER ) || defined( __CLIENT )
	D3DXVECTOR3	m_vPos;
#endif // defined( __WORLDSERVER ) || defined( __CLIENT )
	_PartyMember()
	{
		m_uPlayerId = 0;
		m_tTime = CTime::GetCurrentTime();
		m_bRemove = FALSE;
#if defined( __WORLDSERVER ) || defined( __CLIENT )
		m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
#endif // #if defined( __WORLDSERVER ) || defined( __CLIENT )
	}
}	PartyMember, * PPartyMember;

class CParty
{
private:
public:
	u_long	m_uPartyId;
	TCHAR	m_sParty[33];
	PartyMember	m_aMember[MAX_PTMEMBER_SIZE];
	int		m_nSizeofMember;
	LONG	m_nLevel, m_nExp, m_nPoint;
	int		m_nTroupsShareExp, m_nTroupeShareItem;
#ifdef __FL_PARTY_FINDER
	BOOL	m_bAllowed;
#endif
	int		m_nKindTroup;
	int		m_nReferens;
	int		m_nModeTime[MAX_PARTYMODE];
	int		m_nGetItemPlayerId;

#ifdef __WORLDSERVER
	DWORD	m_dwWorldId;
#endif // __WORLDSERVER

public:
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	CRIT_SEC	m_AddRemoveLock;
#endif

public:
	//	Constructions
	CParty();
	~CParty();

	//	Operations
	void	InitParty();
	BOOL	IsMember(u_long uPlayerId) { return(FindMember(uPlayerId) >= 0); }
	int		IsLeader(u_long uPlayerId) { return(m_aMember[0].m_uPlayerId == uPlayerId); }
	int		GetSizeofMember() { return m_nSizeofMember; }
	int		GetLevel() { return m_nLevel; }
	int		GetExp() { return m_nExp; }
	int		GetPoint() { return m_nPoint; }
	u_long	GetPlayerId(int i) { return m_aMember[i].m_uPlayerId; }
#if defined( __WORLDSERVER ) || defined( __CLIENT )
	D3DXVECTOR3	GetPos(int i) { return m_aMember[i].m_vPos; }
	void	SetPos(int i, D3DXVECTOR3 vPos) { m_aMember[i].m_vPos = vPos; }
#endif // #if defined( __WORLDSERVER ) || defined( __CLIENT )
#ifndef __CORESERVER
	CMover* GetLeader(void);
#endif // __CORESERVER

	void	SetPartyId(u_long uPartyId) { m_uPartyId = uPartyId; }
	BOOL	NewMember(u_long uPlayerId);
	BOOL	DeleteMember(u_long uPlayerId);

#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	void	Lock(void) { m_AddRemoveLock.Enter(); }
	void	Unlock(void) { m_AddRemoveLock.Leave(); }
#endif

	void	ChangeLeader(u_long uLeaderId);

	void	Serialize(CAr& ar);

	void	SwapPartyMember(int first, int Second);

	int		GetPartyModeTime(int nMode);
	void	SetPartyMode(int nMode, DWORD dwSkillTime, int nCachMode);

	void	DoUsePartySkill(u_long uPartyId, u_long nLeaderid, int nSkill);
#ifdef __WORLDSERVER
	void	SetPartyLevel(CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp);
	void	DoUsePartyReCall(u_long uPartyId, u_long nLeaderid, int nSkill);
	void	Replace(DWORD dwWorldId, D3DXVECTOR3& vPos, BOOL bMasterAround);
	void	Replace(DWORD dwWorldId, LPCTSTR sKey);
	BOOL	ReplaceChkLv(int Lv);
	void	ReplaceLodestar(const CRect& rect);
#endif

	void	GetPoint(int nTotalLevel, int nMemberSize, int nDeadLeavel);
	//	Attributes
	int		GetSize(void);
	int		FindMember(u_long uPlayerId);
#ifdef __FL_PARTY_SORT
	BOOL	ChangePosition(u_long uSelectedMember, BOOL bUp);
#endif

private:


public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CParty>* m_pPool;
	void* operator new(size_t nSize) { return CParty::m_pPool->Alloc(); }
	void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine) { return CParty::m_pPool->Alloc(); }
	void	operator delete(void* lpMem) { CParty::m_pPool->Free((CParty*)lpMem); }
	void	operator delete(void* lpMem, LPCSTR lpszFileName, int nLine) { CParty::m_pPool->Free((CParty*)lpMem); }
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

using	namespace	std;
typedef	map< u_long, CParty*>	C2PartyPtr;
typedef map<u_long, string>	ULONG2STRING;
typedef map<string, u_long>	STRING2ULONG;

class CPlayer;

class CPartyMng
{
private:
	u_long		m_id;
//	CMapParty	m_2Party;
	C2PartyPtr	m_2PartyPtr;
#ifdef __WORLDSERVER
	DWORD		m_dwPartyMapInfoLastTick;
#endif // __WORLDSERVER
public:
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	CRIT_SEC	m_AddRemoveLock;
#endif
#ifdef __CORESERVER
	EXPPARTY	m_aExpParty[MAX_PARTYLEVEL];
#endif	// __CORESERVER

	ULONG2STRING	m_2PartyNameLongPtr;
	STRING2ULONG	m_2PartyNameStringPtr;

public:
	//	Constructions
	CPartyMng();
	~CPartyMng();
	void	Clear(void);
	//	Operations
	u_long	NewParty(u_long uLeaderId, u_long uMemberId, u_long uPartyId = 0);
	BOOL	DeleteParty(u_long uPartyId);
	CParty* GetParty(u_long uPartyId);
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	void	Lock(void) { m_AddRemoveLock.Enter(); }
	void	Unlock(void) { m_AddRemoveLock.Leave(); }
#endif

	void	Serialize(CAr& ar);

	//	Attributs
	int		GetSize(void);

#ifdef __CORESERVER
public:
	HANDLE	m_hWorker;
	HANDLE	m_hCloseWorker;

public:
	BOOL	IsPartyNameId(u_long uidPlayer);
	BOOL	IsPartyName(const char* szPartyName);
	LPCSTR  GetPartyString(u_long uidPlayer);
	u_long  GetPartyID(const char* szPartyName);
	void	AddPartyName(u_long uidPlayer, const char* szPartyName);
	BOOL	CreateWorkers(void);
	void	CloseWorkers(void);
	static	UINT	_Worker(LPVOID pParam);
	void	Worker(void);
	void	RemovePartyName(u_long uidPlayer, const char* szPartyName);

	void	AddConnection(CPlayer* pPlayer);
	void	RemoveConnection(CPlayer* pPlayer);
#endif // __CORESERVERE
#ifdef __WORLDSERVER
#ifdef __FL_PARTY_FINDER
	void	AddPartyList(CUser* pUser);
#endif
	void	PartyMapInfo(void);
	DWORD	CanInviteParty(const u_long uLeaderPlayerID, const u_long uMemberPlayerID);
	void	RequestAddPartyMemberToCS(const u_long uLeaderPlayerID, const u_long uMemberPlayerID) const;
#endif // __WORLDSERVER
};

#endif	//	_PARTY_H