// AttackArbiter.h: interface for the CAttackArbiter class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTACKARBITER_H__AC3D1A9E_3828_4E22_94B4_DACA62D3AE0E__INCLUDED_)
#define AFX_ATTACKARBITER_H__AC3D1A9E_3828_4E22_94B4_DACA62D3AE0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CActionMover;



class CAttackArbiter
{
public:
	CAttackArbiter( DWORD dwMsg, CMover* pAttacker, CMover* pDefender, DWORD dwAtkFlags, int nParam, BOOL bTarget, int nReflect );
	virtual ~CAttackArbiter();

private:
	DWORD			m_dwMsg;
	CMover*			m_pAttacker;
	CMover*			m_pDefender;
	DWORD			m_dwAtkFlags;
	int				m_nParam;
	BOOL			m_bTarget;
	int				m_nReflect;
#if __VER >= 11 // __REFLECTDMG_AFTER
	int m_nReflectDmg;
#endif // __REFLECTDMG_AFTER

public:
	int				OnDamageMsgW();

private:
	BOOL			CheckValidDamageMsg();
	int				CalcDamage( ATTACK_INFO* pInfo );
	void			ChanceSkill( int nDestParam, CCtrl *m_pAttacker, CCtrl *m_pDefender );
	void			ProcessAbnormal( int nDamage, ATTACK_INFO* pInfo );
	int				MinusHP( int *pnDamage );
	void			StealHP( int nDamage, ATK_TYPE type );
	void			OnDied();

	int				PostAsalraalaikum();
	BYTE			GetHandFlag( void );
	int				CalcATK( ATTACK_INFO* pInfo );
	int				PostCalcDamage( int nATK, ATTACK_INFO* pInfo );
	int				OnAfterDamage( ATTACK_INFO* pInfo, int nDamage );
};


#endif // !defined(AFX_ATTACKARBITER_H__AC3D1A9E_3828_4E22_94B4_DACA62D3AE0E__INCLUDED_)
