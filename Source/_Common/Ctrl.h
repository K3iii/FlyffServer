#if !defined(AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)
#define AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Obj.h"
#include "ProjectCmn.h"

#ifdef __WORLDSERVER
	class		CUser;
	class		CCtrl;
#endif	// __WORLDSERVER

struct		CtrlProp;
class		CWorld;
class       CSfx;


class CCtrl : public CObj
{
public:
	CCtrl();
	virtual ~CCtrl();

protected:
	OBJID			m_objid;
	D3DXMATRIX		m_mInvTM;

public:

#ifdef __WORLDSERVER
	LONG			m_lRespawn;
	int	            m_nRespawnType;
	map< DWORD, CUser* > m_2pc;
	int				m_nOldCenter[MAX_LINKLEVEL];
#endif	// __WORLDSERVER

public:
	virtual BOOL	GetSkillProp( ItemProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr );
	virtual int		SendDamage( DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE ) { return 0; }
	virtual int		SendDamageForce( DWORD dwAtkFlags, OBJID idSender, int nParam = 0, BOOL bTarget = TRUE ) { return 0; }
	virtual void	AddItToGlobalId();
	virtual	void	RemoveItFromGlobalId();
	virtual void	Process();
	virtual void	Render(LPDIRECT3DDEVICE9 pd3dDevice);
	virtual BOOL	Read( CFileIO* pFile );
	virtual	void	Serialize( CAr & ar );

	void			SetId( OBJID objid )	{ m_objid = objid;  }
	OBJID			GetId()					{ return m_objid;   }
	int				TargetSelecter( CCtrl *pTarget );
	int				ShootSkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
	void			CreateSkillSfx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
	int				ApplyDamage( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, int nParam, BOOL bTarget ) ;
	BOOL			IsPossibleApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
	void			ApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb = false, int nParam = 0, BOOL bOnlyDmg = FALSE, BOOL bTarget = TRUE );
	int				DoApplySkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, int nParam = 0, BOOL bOnlyDmg = FALSE, BOOL bControl = FALSE );
	void			ApplySkillRegion( const D3DXVECTOR3 &vPos, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, BOOL bOnlyDmg = FALSE, CCtrl* pCenter = NULL, BOOL bControl = FALSE );
	void			ApplySkillAround( CCtrl *pSrc, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, BOOL bOnlyDmg = FALSE, FLOAT fRangeCustom = 0.0f, BOOL bControl = FALSE );
	void			ApplySkillLine( int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, float fWidth, float fHeight, float fDepth, bool bIgnoreProp = false, BOOL bControl = FALSE );
	int				ApplySkillAroundTroupe( u_long idParty, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false );
	BOOL			ApplySkillHardCoding( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
	void			DoActiveSkill( DWORD dwSkill, int nLevel, CCtrl *pTarget, bool bIgnoreProb = false, BOOL bControl = FALSE );
	D3DXMATRIX*		GetInvTM()				{ return &m_mInvTM; }

#ifdef __WORLDSERVER
	void			PCSetAt( OBJID objid, CCtrl* pCtrl );
	BOOL			PCRemoveKey( OBJID objid );
	void			RemoveItFromView( BOOL bRemoveall = FALSE );
	BOOL			IsNearPC( OBJID objid );
	BOOL			IsNearPC( CUser* pUser );
	LONG			GetRespawn()			{ return m_lRespawn; }
	void			SetRespawn( LONG n, BYTE nType )	{ m_lRespawn = n; m_nRespawnType = nType; }
	BOOL			ProcessDeleteRespawn();
	int				GetRespawnType( )		{ return m_nRespawnType; }
#else
	void			CreateYoyoSkill( CSfx* pSfx, CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
#endif	//__WORLDSERVER

private:
	int				DoApplySkillEx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp, int nParam, BOOL bOnlyDmg, BOOL bControl );
};

#ifdef __WORLDSERVER

inline void CCtrl::PCSetAt( OBJID objid, CCtrl* pCtrl )
{
	m_2pc[ objid ] = (CUser *)pCtrl;
}

inline BOOL CCtrl::PCRemoveKey( OBJID objid )
{
	return m_2pc.erase( objid ) > 0;
}

#endif	// __WORLDSERVER


#endif // !defined(AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)