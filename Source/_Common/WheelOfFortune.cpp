#include "stdafx.h"
#include "WheelOfFortune.h"
#include "defineText.h"
#ifdef __WORLDSERVER
#include "User.h"
#include "DPDatabaseClient.h"
#include "DPCoreClient.h"
extern  CDPCoreClient g_DPCoreClient;
extern	CUserMng g_UserMng;
extern CDPDatabaseClient g_dpDBClient;

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
CWheelOfFortune::CWheelOfFortune()
	: m_bActive(false)
	, m_dwTicket(NULL_ID)
	, m_nTicketCount(0)
	, m_dwMostRareChance(MAX_CHANCE)
#ifdef __CLIENT
	, m_pTicketTexture(nullptr)
#endif
{

}

CWheelOfFortune::~CWheelOfFortune()
{
	m_vecItem.clear();
#ifdef __WORLDSERVER
	m_mapParticipants.clear();
#else
	m_listRecentRewards.clear();
#endif
}

CWheelOfFortune* CWheelOfFortune::GetInstance()
{
	static CWheelOfFortune	sProperty;
	return &sProperty;
}

#ifdef __WORLDSERVER
bool CWheelOfFortune::LoadScript(BOOL bWithNotify)
{
	CScript s;
	if (!s.Load("WheelOfFortune.inc"))
		return false;

	m_vecItem.clear();

	s.GetToken();
	while (s.tok != FINISHED)
	{
		if (s.Token == _T("IsActive"))
		{
			s.GetToken();
			m_bActive = s.GetNumber();
		}
		else if (s.Token == _T("Ticket"))
		{
			s.GetToken();
			m_dwTicket = s.GetNumber();
			m_nTicketCount = s.GetNumber();
		}
		else if (s.Token == _T("Item_List"))
		{
			WOF_ITEM item;
			DWORD dwTotal = 0;

			m_dwMostRareChance = MAX_CHANCE;

			s.GetToken();
			item.dwItemId = s.GetNumber();
			while (*s.token != '}')
			{
				item.nItemCount = s.GetNumber();
				item.bBinded = s.GetNumber();
				item.dwChance = s.GetNumber();
				dwTotal += item.dwChance;
				item.dwProb = dwTotal;
				item.dwColor = s.GetNumber();
				if (item.dwColor < COLOR_DEFAULT || item.dwColor >= COLOR_MAX)
					item.dwColor = COLOR_DEFAULT;

				if (item.dwChance < m_dwMostRareChance)
					m_dwMostRareChance = item.dwChance;

				m_vecItem.push_back(item);

				item.dwItemId = s.GetNumber();
			}

			if (dwTotal != MAX_CHANCE)
			{
				Error("Wheel Of Fortune total chances is %d, should be %d", dwTotal, MAX_CHANCE);
				m_bActive = false;
				m_vecItem.clear();
				break;
			}
		}

		s.GetToken();
	}

	if (bWithNotify)
	{
		for (auto& it : g_UserMng.m_users)
		{
			CUser* pUser = it.second;
			if (!pUser->IsValid())
				continue;

			pUser->AddWof();
		}
	}

	return true;
}
#endif

void CWheelOfFortune::Serialize(CAr& ar)
{
	if (ar.IsStoring())
	{
		ar << m_bActive;
		ar << m_dwTicket;
		ar << m_nTicketCount;

		ar << m_vecItem.size();
		for (auto& it : m_vecItem)
		{
			ar << it.dwItemId;
			ar << it.nItemCount;
			ar << it.bBinded;
			ar << it.dwChance;
			ar << it.dwColor;
		}
	}
	else
	{
		ar >> m_bActive;
		ar >> m_dwTicket;
		ar >> m_nTicketCount;

#ifdef __CLIENT
		m_pTicketTexture = nullptr;
#endif
		m_vecItem.clear();
		size_t vecSize;

		ar >> vecSize;
		for (size_t i = 0; i < vecSize; i++)
		{
			WOF_ITEM tmpStruc;
			ar >> tmpStruc.dwItemId;
			ar >> tmpStruc.nItemCount;
			ar >> tmpStruc.bBinded;
			ar >> tmpStruc.dwChance;
			ar >> tmpStruc.dwColor;

			m_vecItem.push_back(tmpStruc);
		}
	}
}

#ifdef __WORLDSERVER
int CWheelOfFortune::GetPrize()
{
	const DWORD dwRand = xRandom(MAX_CHANCE);
	for (int i = 0; i < m_vecItem.size(); i++)
	{
		const WOF_ITEM* item = &m_vecItem[i];
		if (dwRand < item->dwProb)
			return i;
	}

	return -1;
}
void CWheelOfFortune::InitProcess(CUser* pUser)
{
	if (!IsActive())
		return;

	if (!IsValidObj(pUser))
		return;

	if (m_mapParticipants.find(pUser->m_idPlayer) != m_mapParticipants.end())
		return;

	if (pUser->GetAvailableItemNum(m_dwTicket) < m_nTicketCount)
	{
		pUser->AddDefinedText(TID_UPGRADE_ERROR_NOSUPSTON);
		return;
	}

	const int nItem = GetPrize();
	if (nItem == -1)
		return;

	WOF_ITEM item = m_vecItem[nItem];

	pUser->RemoveAvailableItem(m_dwTicket, m_nTicketCount);
	m_mapParticipants.insert(make_pair(pUser->m_idPlayer, item));

	pUser->AddWofInit(nItem);

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::WOF_ATTEMPTS, 1, true);
#endif
}
void CWheelOfFortune::FinishProcess(CUser* pUser)
{
	if (!IsValidObj(pUser))
		return;

	auto it = m_mapParticipants.find(pUser->m_idPlayer);
	if (it == m_mapParticipants.end())
		return;

	CItemElem itemElem;
	itemElem.m_dwItemId = it->second.dwItemId;
	itemElem.m_nItemNum = it->second.nItemCount;
	if (it->second.bBinded)
		itemElem.SetFlag(CItemElem::binds);
	itemElem.SetSerialNumber();

	if (!pUser->CreateItem(&itemElem))
		g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, itemElem, 0, "Fortune Wheel", "");

	pUser->AddWofFinish(&it->second);

	if (it->second.dwChance == m_dwMostRareChance)
	{
		ItemProp* pRewardProp = prj.GetItemProp(it->second.dwItemId);
		if (pRewardProp)
		{
			CString str; str.Format("%s received %s (x%d) from the Wheel of Fortune!", pUser->GetName(), pRewardProp->szName, it->second.nItemCount);
			g_DPCoreClient.SendCaption(str);
		}
	}

	m_mapParticipants.erase(it);
}
#else
DWORD CWheelOfFortune::GetColor(bool bLowAlpha, int nColor)
{
	const DWORD dwAlpha = bLowAlpha ? 0 : 200;
	switch (nColor)
	{
	case COLOR_YELLOW: return D3DCOLOR_ARGB(dwAlpha, 255, 223, 13);
	case COLOR_ORANGE: return D3DCOLOR_ARGB(dwAlpha, 255, 138, 13);
	case COLOR_BLUE: return D3DCOLOR_ARGB(dwAlpha, 12, 119, 250);
	case COLOR_GREEN: return D3DCOLOR_ARGB(dwAlpha, 0, 255, 123);
	case COLOR_VIOLET: return D3DCOLOR_ARGB(dwAlpha, 76, 19, 156);
	default: return D3DCOLOR_ARGB(dwAlpha, 146, 146, 146);
	}
}
void CWheelOfFortune::AddRecentReward(const WOF_ITEM& item)
{
	if (m_listRecentRewards.size() >= 24)
		m_listRecentRewards.pop_back();

	m_listRecentRewards.push_front(item);
}
#endif
#endif