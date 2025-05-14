#ifndef __TLORD_H__
#define	__TLORD_H__

#if __VER >= 12 // __LORD

#include "lord.h"
#include "dbcontroller.h"
#include "post.h"

//////////////////////////////////////////////////////////////////////////
class CLController;

class CTLord : public CLord
{
public:
	enum { eBeginCandidacy, eBeginVote, eEndVote, eAddDeposit, eSetPledge, eIncVote, eInit, eLEventCreate, eLEventInitialize, eLordSkillUse };
public:
	CTLord();
	virtual	~CTLord();
	static	CTLord* Instance(void);

	virtual	void	CreateColleagues(void);

	virtual void	DestroyColleagues(void);

	BOOL	RestoreAll(void);

	BOOL	Restore(CQuery* pQuery);

	void	Handler(LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey);

	void	OnTimer(void);

	BOOL	PostRequest(int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0);
	virtual	CLController* GetController(void) { return m_pController; }
private:
	CLController* m_pController;
};

//////////////////////////////////////////////////////////////////////////
typedef	struct	_PostItemStruct
{
	int	nItem;
	DWORD dwKeepTime;
	_PostItemStruct() : nItem(0), dwKeepTime(0)
	{
	}
}	PostItemStruct;

class CQuery;

class CTElection :
	public IElection
{
public:
	CTElection(CLord* pLord);
	virtual	~CTElection();

	BOOL	Initialize(const char* szFile);

	float	GetRetRate(u_long idPlayer);

	void	SetRun(BOOL bRun) { m_bRun = bRun; }

	BOOL	IsRunable(void) { return m_bRun; }

	void	OnTimer(void);

	BOOL	Restore(CQuery* pQuery);

public:

	static	CString	ttos(time_t t);
	static	time_t	stot(const char* sz);

protected:


	virtual	BOOL	DoTestBeginCandidacy(void);

	virtual	void	DoReturnDeposit(void);

	virtual	BOOL	DoTestBeginVote(int& nRequirement);

	virtual	BOOL	DoTestEndVote(u_long idPlayer);

	virtual	BOOL	DoTestAddDeposit(u_long idPlayer, __int64 iDeposit, time_t tCreate);
	virtual	void	DoAddDepositComplete(u_long idPlayer, __int64 iDeposit, time_t tCreate);

	virtual	BOOL	DoTestSetPledge(u_long idPlayer, const char* szPledge);

	virtual	BOOL	DoTestIncVote(u_long idPlayer, u_long idElector);

private:

	BOOL	ReadIni(const char* szFile);

	void	PostItem(u_long idPlayer, PostItemStruct& pi);

	void	PostDepositToAllCandidate(void);

	void	PostLordItems(u_long idLord);

private:

	BOOL	m_bRun;
};


class CTLordSkill
	: public CLordSkill
{
public:
	CTLordSkill(CLord* pLord);
	virtual	~CTLordSkill();
	virtual	CLordSkillComponentExecutable* CreateSkillComponent(int nType);	// template method

	BOOL	Restore(CQuery* pQuery);

	void	OnTimer(void);
};

//////////////////////////////////////////////////////////////////////////
class CQuery;

class CLEvent : public ILordEvent
{
public:
	CLEvent(CLord* pLord);
	virtual	~CLEvent();

	BOOL	Restore(CQuery* pQuery);

	void	OnTimer(void);
protected:


	virtual	BOOL	DoTestAddComponent(CLEComponent* pComponent);

	virtual	BOOL	DoTestInitialize(void);

	BOOL	DecrementAllComponentTick(void);
};


//////////////////////////////////////////////////////////////////////////
class CLordSkillComponentODBC;



class CLController : public CDbController
{
public:
	CLController(CTLord* pLord);
	virtual	~CLController();


	BOOL	Restore(void);

	BOOL	BeginCandidacy(void);

	int		BeginVote(void);

	BOOL	EndVote(u_long idPlayer);

	BOOL	AddDeposit(u_long idPlayer, __int64 iDeposit, time_t tCreate);

	BOOL	SetPledge(u_long idPlayer, const char* szPledge);

	BOOL	IncVote(u_long idPlayer, u_long idElector);

	BOOL	AddLEComponent(CLEComponent* pComponent);

	BOOL	InitializeLEvent(void);

	BOOL	UpdateLordSkillTick(CLordSkillComponent* pSkill, int nTick);

	BOOL	UpdateLordEventTick(CLEComponent* pComponent);

	virtual	void	Handler(LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey);

	virtual	void	OnTimer(void);

private:
	CTLord* m_pLord;
};

#endif	// __LORD

#endif	// __TLORD_H__
