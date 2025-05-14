#ifndef __COMMONCTRL_H__
#define __COMMONCTRL_H__

#include "Ctrl.h"
//#include "Respawn.h"
#include "ProjectCmn.h"


#define			MAX_CTRLDROPITEM		4
#define			MAX_CTRLDROPMOB			3
#define			MAX_TRAP                3

#define			MAX_KEY					64

typedef struct CCtrlElem
{
	DWORD		m_dwSet;
	DWORD 		m_dwSetItem;
	DWORD		m_dwSetLevel;
	DWORD		m_dwSetQuestNum;
	DWORD		m_dwSetFlagNum;
	DWORD		m_dwSetGender;

	BOOL		m_bSetJob[32];

	DWORD		m_dwSetEndu;

	DWORD		m_dwMinItemNum;
	DWORD		m_dwMaxiItemNum;
	DWORD		m_dwInsideItemKind[MAX_CTRLDROPITEM];
	DWORD		m_dwInsideItemPer [MAX_CTRLDROPITEM];

	DWORD       m_dwMonResKind  [MAX_CTRLDROPMOB];
	DWORD		m_dwMonResNum   [MAX_CTRLDROPMOB];
	DWORD		m_dwMonActAttack[MAX_CTRLDROPMOB];


	DWORD		m_dwTrapOperType;
	DWORD		m_dwTrapRandomPer;
	DWORD		m_dwTrapDelay;

	DWORD		m_dwTrapKind	[MAX_TRAP];
 	DWORD		m_dwTrapLevel	[MAX_TRAP];
	TCHAR		m_strLinkCtrlKey[MAX_KEY];
	TCHAR		m_strCtrlKey[MAX_KEY];
	DWORD		m_dwSetQuestNum1;
	DWORD		m_dwSetFlagNum1;
	DWORD		m_dwSetQuestNum2;
	DWORD		m_dwSetFlagNum2;
	DWORD 		m_dwSetItemCount;
	DWORD		m_dwTeleWorldId;
	DWORD		m_dwTeleX;
	DWORD		m_dwTeleY;
	DWORD		m_dwTeleZ;

} CCtrlElem, *LPCtrlElem;


class CCommonCtrl : public CCtrl
{
public:
	CCommonCtrl();
	virtual ~CCommonCtrl();

public:
	OBJID		m_nMoverID;
	CCtrlElem	m_CtrlElem;

	BOOL		 m_bTrap;
	BOOL		 m_bAniPlay;
	BOOL		 m_bAlpha;
	int			 m_nBlendFactorLocal;
	DWORD		 m_dwTrapProcessTime;

	float		 m_fFrameCount;
	BOOL		 m_bAniDelay;
	DWORD		 m_dwAniDelay;

public:
	void Init();
	ItemProp *m_pSkillProp;
	AddSkillProp *m_pAddSkillProp;
#ifdef __CLIENT
	CSfxModel *m_pSfxModel;
	CSfxModel *m_pSfxModel2;
#endif // __CLIENT
	OBJID	m_idAttacker;
	BOOL	m_bControl;
	int		m_nCount;
	int		m_nEvent;
	DWORD	m_tmStart;
	int		m_nLife;


	void DestroyWall( void );
	void _ProcessWall( void );
	void _CreateTrapSkill();
	void _ProcessTrap();
	void _ProcessAction();
	void DropNPC();

	void			DropItem();
	void			SetActionPlay();
	CtrlProp*		GetProp() { return prj.GetCtrlProp( GetIndex() ); }

	virtual void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual void RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor = 0xffffffff );
	virtual BOOL Read( CFileIO* pFile );
};

#endif //__COMMONCTRL_H__
