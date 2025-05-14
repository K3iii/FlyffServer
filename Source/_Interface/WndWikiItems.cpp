#include "stdafx.h"

#ifdef __WIKI_ITEMS_V2
#include "WndWikiItems.h"
#include "resdata.h"

#include "DPClient.h"
extern CDPClient g_DPlay;

bool SortByItemLevelHighest(ItemProp* s1, ItemProp* s2)
{
	return (s1->dwLimitLevel1 > s2->dwLimitLevel1);
}
bool SortByItemLevelLowest(ItemProp* s1, ItemProp* s2)
{
	return (s1->dwLimitLevel1 < s2->dwLimitLevel1);
}
void CWndWikiItems::SortByItemLevel(BOOL bHighest)
{
	if (bHighest)
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByItemLevelHighest);
	else
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByItemLevelLowest);
}

CWndWikiItems::CWndWikiItems()
#ifdef __FL_FARM_DISPLAY
	: m_nMode(0)
#endif
	, m_pWndComboFilter(nullptr)
{

}

CWndWikiItems::~CWndWikiItems()
{
	m_vecItems.clear();
}

BOOL CWndWikiItems::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_WIKI_ITEMS, 0, 0, pWndParent);
}
void CWndWikiItems::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndTreeCtrl* pWndTreeCtrl = (CWndTreeCtrl*)GetDlgItem(WIDC_TREE1);
	pWndTreeCtrl->LoadTreeScript(MakePath(DIR_CLIENT, "WikiItems.inc"));

	m_pListItems = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_pNameFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pMinLevelFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	m_pMaxLevelFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	m_pComboSex = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	m_pComboJob = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);

	m_pMinLevelFilter->AddWndStyle(EBS_NUMBER);
	m_pMaxLevelFilter->AddWndStyle(EBS_NUMBER);
	m_pMinLevelFilter->SetString("1");
	m_pMaxLevelFilter->SetString("200");

	m_pComboSex->AddString("~");
	m_pComboSex->AddString("Male");
	m_pComboSex->AddString("Female");
	m_pComboSex->SetCurSel(0);
	m_pComboSex->AddWndStyle(EBS_READONLY);

	m_pComboJob->AddString("~");
	for (int i = 0; i < MAX_JOB; ++i)
	{
		if (i == 5 || i == 14 || i == 15)
			continue;
		m_pComboJob->AddString(prj.m_aJob[i].szName);
	}
	m_pComboJob->SetCurSel(0);
	m_pComboJob->AddWndStyle(EBS_READONLY);

	m_pWndComboFilter = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX4);
	m_pWndComboFilter->AddWndStyle(EBS_READONLY);
	m_pWndComboFilter->AddString("Sort by item level (Highest)");
	m_pWndComboFilter->AddString("Sort by item level (Lowest)");
	m_pWndComboFilter->SetCurSel(1);

	ApplyFilters();
	m_Menu.CreateMenu(this);
	MoveParentCenter();
}
BOOL CWndWikiItems::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_TREE1)
		{
			LPTREEELEM lpTreeElem = (LPTREEELEM)pLResult;
			if (lpTreeElem)
			{
				if (lpTreeElem->m_strKeyword == m_strKeyword)
					return FALSE;

				m_strKeyword = lpTreeElem->m_strKeyword;
				ApplyFilters();
			}
		}
		else if (nID == WIDC_BUTTON1)
		{
			CWndTreeCtrl* pWndTreeCtrl = (CWndTreeCtrl*)GetDlgItem(WIDC_TREE1);
			pWndTreeCtrl->SetCurSel("All Items");
			m_strKeyword = "All Items";

			m_pNameFilter->SetString("");
			m_pMinLevelFilter->SetString("1");
			m_pMaxLevelFilter->SetString("200");
			m_pComboSex->SetCurSel(0);
			m_pComboJob->SetCurSel(0);
			ApplyFilters();
		}
	}
	else if (message == WNM_SELCHANGE)
	{
		if (nID == WIDC_COMBOBOX1 || nID == WIDC_COMBOBOX2 || nID == WIDC_COMBOBOX4)
			ApplyFilters();

	}
	else if (message == EN_CHANGE)
	{
		if (nID == WIDC_EDIT1 || nID == WIDC_EDIT2 || nID == WIDC_EDIT3)
			ApplyFilters();
	}
	else if (message == WNM_DBLCLK)
	{
		if (nID == WIDC_LISTBOX1)
		{
			if (m_pListItems->GetCurSel() != -1)
			{
				ItemProp* pProp = m_vecItems[m_pListItems->GetCurSel()];
				if (pProp)
				{
#ifdef __FL_FARM_DISPLAY
					if (m_nMode == 1)
					{
						CWndFarmDisplay* pWndFarm = (CWndFarmDisplay*)g_WndMng.GetWndBase(APP_FARM_DISPLAY);
						if (pWndFarm)
							pWndFarm->AddFromWiki(pProp->dwID);
						else
							m_nMode = 0;
					}
#endif
#ifdef __MODEL_VIEW
					else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
					{
						CItemElem pItemElem;
						pItemElem.m_dwItemId = pProp->dwID;
						g_WndMng.PreviewModel(&pItemElem);
					}
#endif
					else if (g_pPlayer && g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR))
					{
						CPoint point = GetMousePoint();
						ClientToScreen(&point);
						m_Menu.DeleteAllMenu();
						m_Menu.AppendMenu(0, 0, "Create 1");
						m_Menu.AppendMenu(0, 1, "Create 100");
						m_Menu.AppendMenu(0, 2, "Create 1000");
						m_Menu.AppendMenu(0, 3, "Create max");
						m_Menu.Move(point);
						m_Menu.SetVisible(TRUE);
						m_Menu.SetFocus();
					}
				}
			}
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndWikiItems::ApplyFilters()
{
	m_vecItems.clear();

	for (int i = 0; i < prj.m_aPropItem.GetSize(); ++i)
	{
		// Prop Check
		ItemProp* pItemProp = prj.m_aPropItem.GetAt(i);
		if (!pItemProp)
			continue;

		// Etc
		if (pItemProp->dwItemKind2 == IK2_MOB)
			continue;

		// Type Filter
		if (m_strKeyword != "" && m_strKeyword != "All Items")
		{
			if (m_strKeyword == "Weapon")
			{
				if (pItemProp->dwItemKind1 != IK1_WEAPON && pItemProp->dwItemKind3 != IK3_SHIELD && pItemProp->dwItemKind3 != IK3_MAGICBARUNA && pItemProp->dwItemKind3 != IK3_ZEMBARUNA)
					continue;
			}
			else if (m_strKeyword == "1-H Sword")
			{
				if (pItemProp->dwItemKind3 != IK3_SWD || pItemProp->dwHanded != HD_ONE)
					continue;
			}
			else if (m_strKeyword == "2-H Sword")
			{
				if (pItemProp->dwItemKind3 != IK3_SWD || pItemProp->dwHanded != HD_TWO)
					continue;
			}
			else if (m_strKeyword == "1-H Axe")
			{
				if (pItemProp->dwItemKind3 != IK3_AXE)
					continue;
				if (pItemProp->dwHanded != HD_ONE)
					continue;
			}
			else if (m_strKeyword == "2-H Axe")
			{
				if (pItemProp->dwItemKind3 != IK3_AXE)
					continue;
				if (pItemProp->dwHanded != HD_TWO)
					continue;
			}
			else if (m_strKeyword == "Knuckle")
			{
				if (pItemProp->dwItemKind3 != IK3_KNUCKLEHAMMER)
					continue;
			}
			else if (m_strKeyword == "Stick")
			{
				if (pItemProp->dwItemKind3 != IK3_CHEERSTICK)
					continue;
			}
			else if (m_strKeyword == "Bow")
			{
				if (pItemProp->dwItemKind3 != IK3_BOW
#ifdef __CROSSBOW
					&& pItemProp->dwItemKind3 != IK3_CROSSBOW
#endif
					)
					continue;
			}
			else if (m_strKeyword == "Yo-Yo")
			{
				if (pItemProp->dwItemKind3 != IK3_YOYO)
					continue;
			}
			else if (m_strKeyword == "Wand")
			{
				if (pItemProp->dwItemKind3 != IK3_WAND)
					continue;
			}
			else if (m_strKeyword == "Staff")
			{
				if (pItemProp->dwItemKind3 != IK3_STAFF)
					continue;
			}
			else if (m_strKeyword == "Shield")
			{
				if (pItemProp->dwItemKind3 != IK3_SHIELD && pItemProp->dwItemKind3 != IK3_MAGICBARUNA && pItemProp->dwItemKind3 != IK3_ZEMBARUNA)
					continue;
			}
			else if (m_strKeyword == "Armor")
			{
				if ((pItemProp->dwItemKind1 != IK1_ARMOR || (pItemProp->dwItemKind2 != IK2_ARMORETC && pItemProp->dwItemKind2 != IK2_ARMOR)) &&
					(pItemProp->dwItemKind3 != IK3_HELMET || pItemProp->dwItemKind3 != IK3_SUIT || pItemProp->dwItemKind3 != IK3_GAUNTLET ||
						pItemProp->dwItemKind3 != IK3_BOOTS))
					continue;
			}
			else if (m_strKeyword == "Helmet")
			{
				if (pItemProp->dwItemKind3 != IK3_HELMET)
					continue;
			}
			else if (m_strKeyword == "Suit")
			{
				if (pItemProp->dwItemKind3 != IK3_SUIT)
					continue;
			}
			else if (m_strKeyword == "Gauntlets")
			{
				if (pItemProp->dwItemKind3 != IK3_GAUNTLET)
					continue;
			}
			else if (m_strKeyword == "Boots")
			{
				if (pItemProp->dwItemKind3 != IK3_BOOTS)
					continue;
			}
			else if (m_strKeyword == "Jewelry")
			{
				if (pItemProp->dwItemKind2 != IK2_JEWELRY)
					continue;
			}
			else if (m_strKeyword == "Ring")
			{
				if (pItemProp->dwItemKind3 != IK3_RING)
					continue;
			}
			else if (m_strKeyword == "Earring")
			{
				if (pItemProp->dwItemKind3 != IK3_EARRING)
					continue;
			}
			else if (m_strKeyword == "Necklace")
			{
				if (pItemProp->dwItemKind3 != IK3_NECKLACE)
					continue;
			}
			else if (m_strKeyword == "Fashion")
			{
				if ((pItemProp->dwItemKind2 != IK2_CLOTH && pItemProp->dwItemKind2 != IK2_CLOTHETC) &&
					(pItemProp->dwItemKind3 != IK3_HAT || pItemProp->dwItemKind3 != IK3_CLOTH || pItemProp->dwItemKind3 != IK3_GLOVE ||
						pItemProp->dwItemKind3 != IK3_SHOES || pItemProp->dwItemKind3 != IK3_CLOAK || pItemProp->dwItemKind3 != IK3_MASK || pItemProp->dwItemKind3 != IK3_WING))
					continue;
			}
			else if (m_strKeyword == "Hat")
			{
				if (pItemProp->dwItemKind3 != IK3_HAT && pItemProp->dwParts != PARTS_HAT)
					continue;
			}
			else if (m_strKeyword == "Suits")
			{
				if (pItemProp->dwItemKind3 != IK3_CLOTH && pItemProp->dwParts != PARTS_CLOTH)
					continue;
			}
			else if (m_strKeyword == "Gloves")
			{
				if (pItemProp->dwItemKind3 != IK3_GLOVE && pItemProp->dwParts != PARTS_GLOVE)
					continue;
			}
			else if (m_strKeyword == "Shoes")
			{
				if (pItemProp->dwItemKind3 != IK3_SHOES && pItemProp->dwParts != PARTS_BOOTS)
					continue;
			}
			else if (m_strKeyword == "Cloak")
			{
				if (pItemProp->dwItemKind3 != IK3_CLOAK && pItemProp->dwParts != PARTS_CLOAK)
					continue;
			}
			else if (m_strKeyword == "Mask")
			{
				if (pItemProp->dwItemKind3 != IK3_MASK && pItemProp->dwParts != PARTS_MASK)
					continue;
			}
			else if (m_strKeyword == "Pet")
			{
				if (pItemProp->dwItemKind3 != IK3_PET && pItemProp->dwItemKind3 != IK3_EGG)
					continue;
			}
			else if (m_strKeyword == "Egg")
			{
				if (pItemProp->dwItemKind3 != IK3_EGG)
					continue;
			}
			else if (m_strKeyword == "Looter")
			{
				if (pItemProp->dwItemKind3 != IK3_PET || pItemProp->dwReferStat1 != NULL_ID)
					continue;
			}
			else if (m_strKeyword == "Vis-Looter")
			{
				if (pItemProp->dwItemKind3 != IK3_PET || pItemProp->dwReferStat1 != PET_VIS)
					continue;
			}
			else if (m_strKeyword == "Furniture")
			{
				if (pItemProp->dwItemKind1 != IK1_HOUSING)
					continue;
			}
			else if (m_strKeyword == "House")
			{
				if (pItemProp->dwItemKind2 != IK2_FURNITURE && pItemProp->dwItemKind2 != IK2_PAPERING)
					continue;
			}
			else if (m_strKeyword == "Guild House")
			{
				if (pItemProp->dwItemKind2 != IK2_GUILDHOUSE_FURNITURE && pItemProp->dwItemKind2 != IK2_GUILDHOUSE_PAPERING)
					continue;
			}
			else if (m_strKeyword == "Other")
			{
				if (pItemProp->dwID == II_GEN_MAT_ELE_TOUCH || pItemProp->dwID == II_GEN_MAT_ELE_LAKE || pItemProp->dwID == II_GEN_MAT_ELE_VOLTAGE ||
					pItemProp->dwID == II_GEN_MAT_ELE_STONE || pItemProp->dwID == II_GEN_MAT_ELE_GALE)
					continue;
				if (pItemProp->dwItemKind3 != IK3_SOCKETCARD && pItemProp->dwItemKind3 != IK3_SOCKETCARD2 && pItemProp->dwItemKind3 != IK3_ELECARD &&
					pItemProp->dwItemKind3 != IK3_VIS && pItemProp->dwItemKind2 != IK2_GEM && pItemProp->dwItemKind3 != IK3_TICKET)
					continue;
			}
			else if (m_strKeyword == "Card")
			{
				if (pItemProp->dwID == II_GEN_MAT_ELE_TOUCH || pItemProp->dwID == II_GEN_MAT_ELE_LAKE || pItemProp->dwID == II_GEN_MAT_ELE_VOLTAGE ||
					pItemProp->dwID == II_GEN_MAT_ELE_STONE || pItemProp->dwID == II_GEN_MAT_ELE_GALE)
					continue;
				if (pItemProp->dwItemKind3 != IK3_SOCKETCARD && pItemProp->dwItemKind3 != IK3_SOCKETCARD2 && pItemProp->dwItemKind3 != IK3_ELECARD)
					continue;
			}
			else if (m_strKeyword == "Vis")
			{
				if (pItemProp->dwItemKind3 != IK3_VIS)
					continue;
			}
			else if (m_strKeyword == "Gem")
			{
				if (pItemProp->dwItemKind2 != IK2_GEM)
					continue;
			}
			else if (m_strKeyword == "Ticket")
			{
				if (pItemProp->dwItemKind3 != IK3_TICKET)
					continue;
			}
			else if (m_strKeyword == "Ride")
			{
				if (pItemProp->dwItemKind1 != IK1_RIDE)
					continue;
			}
		}

		// Name Filter
		CString strFilter = m_pNameFilter->GetString();
		if (!strFilter.IsEmpty())
		{
			CString strName = pItemProp->szName;
			if (!strName.IsEmpty())
			{
				if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
					continue;
			}
		}

		// Level Filters
		DWORD dwLimitLevel = 1;
		if (pItemProp->dwLimitLevel1 != 0xFFFFFFFF)
			dwLimitLevel = pItemProp->dwLimitLevel1;

		int nMinLevel = atoi(m_pMinLevelFilter->GetString());
		if (nMinLevel <= 0)
			nMinLevel = 1;

		if ((int)dwLimitLevel < nMinLevel)
			continue;

		int nMaxLevel = atoi(m_pMaxLevelFilter->GetString());
		if ((int)dwLimitLevel > nMaxLevel)
			continue;

		// Sex Filters
		if (m_pComboSex->GetCurSel() != 0)
		{
			if (pItemProp->dwItemSex != SEX_FEMALE && m_pComboSex->GetCurSel() == 2)
				continue;

			if (pItemProp->dwItemSex != SEX_MALE && m_pComboSex->GetCurSel() == 1)
				continue;
		}

		// Job Filter
		if (m_pComboJob->GetCurSel() >= 1)
		{
			DWORD dwJob = JOB_VAGRANT;
			switch (m_pComboJob->GetCurSel())
			{
			case 2: dwJob = JOB_MERCENARY; break;
			case 3: dwJob = JOB_ACROBAT; break;
			case 4: dwJob = JOB_ASSIST; break;
			case 5: dwJob = JOB_MAGICIAN; break;
			case 6: dwJob = JOB_KNIGHT; break;
			case 7: dwJob = JOB_BLADE; break;
			case 8: dwJob = JOB_JESTER; break;
			case 9: dwJob = JOB_RANGER; break;
			case 10: dwJob = JOB_RINGMASTER; break;
			case 11: dwJob = JOB_BILLPOSTER; break;
			case 12: dwJob = JOB_PSYCHIKEEPER; break;
			case 13: dwJob = JOB_ELEMENTOR; break;
			case 14: dwJob = JOB_KNIGHT_MASTER; break;
			case 15: dwJob = JOB_BLADE_MASTER; break;
			case 16: dwJob = JOB_JESTER_MASTER; break;
			case 17: dwJob = JOB_RANGER_MASTER; break;
			case 18: dwJob = JOB_RINGMASTER_MASTER; break;
			case 19: dwJob = JOB_BILLPOSTER_MASTER; break;
			case 20: dwJob = JOB_PSYCHIKEEPER_MASTER; break;
			case 21: dwJob = JOB_ELEMENTOR_MASTER; break;
			case 22: dwJob = JOB_KNIGHT_HERO; break;
			case 23: dwJob = JOB_BLADE_HERO; break;
			case 24: dwJob = JOB_JESTER_HERO; break;
			case 25: dwJob = JOB_RANGER_HERO; break;
			case 26: dwJob = JOB_RINGMASTER_HERO; break;
			case 27: dwJob = JOB_BILLPOSTER_HERO; break;
			case 28: dwJob = JOB_PSYCHIKEEPER_HERO; break;
			case 29: dwJob = JOB_ELEMENTOR_HERO; break;
			case 30: dwJob = JOB_LORDTEMPLER_HERO; break;
			case 31: dwJob = JOB_STORMBLADE_HERO; break;
			case 32: dwJob = JOB_WINDLURKER_HERO; break;
			case 33: dwJob = JOB_CRACKSHOOTER_HERO; break;
			case 34: dwJob = JOB_FLORIST_HERO; break;
			case 35: dwJob = JOB_FORCEMASTER_HERO; break;
			case 36: dwJob = JOB_MENTALIST_HERO; break;
			case 37: dwJob = JOB_ELEMENTORLORD_HERO; break;
			default: dwJob = JOB_VAGRANT; break;
			}

			if (pItemProp->dwItemJob != dwJob)
				continue;
		}

		m_vecItems.push_back(pItemProp);
	}

	// Sort
	switch (m_pWndComboFilter->GetCurSel())
	{
	case 0: SortByItemLevel(TRUE); break;
	case 1: SortByItemLevel(FALSE); break;
	default: break;
	}

	m_pListItems->ResetContent();
	for (auto& it : m_vecItems)
		m_pListItems->AddString("");
}
BOOL CWndWikiItems::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	if (pWndBase == &m_Menu)
	{
		if (m_pListItems->GetCurSel() != -1)
		{
			ItemProp* pProp = m_vecItems[m_pListItems->GetCurSel()];
			if (pProp)
			{
				g_DPlay.SendWikiCreate(pProp->dwID, nID);
				m_Menu.SetVisible(FALSE);
			}

		}
	}
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
#endif