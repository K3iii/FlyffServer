#include "stdafx.h"

#if __VER >= 13 // __COUPLE_1117

#include "couplehelper.h"
#include "playerdata.h"
#include "definetext.h"

CCoupleHelper::CCoupleHelper()
	:
	m_pCouple(NULL)
{
}

CCoupleHelper::~CCoupleHelper()
{
	Clear();
}

void CCoupleHelper::Clear()
{
	SAFE_DELETE(m_pCouple);
}

CCoupleHelper* CCoupleHelper::Instance()
{
	static CCoupleHelper sCoupleHelper;
	return &sCoupleHelper;
}

BOOL CCoupleHelper::Initialize()
{
	return CCoupleProperty::Instance()->Initialize();
}

void CCoupleHelper::OnCouple(CAr& ar)
{
	SAFE_DELETE(m_pCouple);
	m_pCouple = new CCouple;
	m_pCouple->Serialize(ar);
}

void CCoupleHelper::OnProposeResult(CAr& ar)
{
	u_long idProposer;
	char szProposer[MAX_PLAYER] = { 0, };
	ar >> idProposer;
	ar.ReadString(szProposer, MAX_PLAYER);
	char szText[200] = { 0, };
	sprintf(szText, prj.GetText(TID_GAME_RECEIVEPROPOSAL), szProposer);
	CPlayerDataCenter::GetInstance()->GetPlayerString(idProposer);

	SAFE_DELETE(g_WndMng.m_pWndCoupleMessage);
	g_WndMng.m_pWndCoupleMessage = new CWndCoupleMessage;
	g_WndMng.m_pWndCoupleMessage->SetMessageMod(szText, CWndCoupleMessage::CM_RECEIVEPROPOSE);
	g_WndMng.m_pWndCoupleMessage->Initialize(NULL);
}

void CCoupleHelper::OnCoupleResult(CAr& ar)
{
	u_long idPartner;
	char szPartner[MAX_PLAYER] = { 0, };
	ar >> idPartner;
	ar.ReadString(szPartner, MAX_PLAYER);

	if (!g_pPlayer)
		return;

	SAFE_DELETE(m_pCouple);
	m_pCouple = new CCouple(g_pPlayer->m_idPlayer, idPartner);
	CPlayerDataCenter::GetInstance()->GetPlayerString(idPartner);
	char szText[200] = { 0, };
	sprintf(szText, prj.GetText(TID_GAME_COUPLE_S02), szPartner);
	g_WndMng.PutString(szText, NULL, prj.GetTextColor(TID_GAME_COUPLE_S02));
}

void CCoupleHelper::OnDecoupleResult()
{
	CCouple* pCouple = GetCouple();
	if (pCouple)
	{
		Clear();
		g_WndMng.PutString(prj.GetText(TID_GAME_COUPLE_S03), NULL, prj.GetTextColor(TID_GAME_COUPLE_S03));
	}
}

#if __VER >= 13 // __COUPLE_1202
void CCoupleHelper::OnAddCoupleExperience(CAr& ar)
{
	int nExperience;
	ar >> nExperience;
	CCouple* pCouple = GetCouple();
	if (pCouple)
	{
		BOOL bLevelup = pCouple->AddExperience(nExperience);
		if (bLevelup)
		{
			char szString[200] = { 0, };
			sprintf(szString, prj.GetText(TID_GAME_COUPLE_LEVELUP), pCouple->GetLevel());
			g_WndMng.PutString(szString, NULL, prj.GetTextColor(TID_GAME_COUPLE_LEVELUP));
		}
	}
}
#endif	// __COUPLE_1202

#endif	// __COUPLE_1117