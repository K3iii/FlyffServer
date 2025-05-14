#pragma once
#include "ar.h"
#ifdef __ITEM_LINK
#define PROCESS_TICK MIN(1)
#define LINK_KEEPTIME MIN(15)

struct LinkedItem
{
	CItemElem item;
	time_t timeCreated;

	void Serialize(CAr& ar)
	{
		if (ar.IsStoring())
		{
			ar << timeCreated;
			item.Serialize(ar);
		}
		else
		{
			ar >> timeCreated;
			item.Serialize(ar);
		}
	}
};

class CLinkedItemMgr
{
private:
	CLinkedItemMgr() = default;
	~CLinkedItemMgr() = default;
	CLinkedItemMgr(const CLinkedItemMgr&) = delete;
	CLinkedItemMgr& operator=(CLinkedItemMgr&) = delete;

	void ProcessTimestamps();

	int m_nCount = 0;
	std::map<int, LinkedItem> m_mapLinkedItems;
	time_t m_LastProcessTick = GetTickCount();
public:
	static CLinkedItemMgr& GetInstance()
	{
		static CLinkedItemMgr instance;
		return instance;
	}

	void Serialize(CAr& ar, int& nIndex
#ifdef __CLIENT
		, int nLinkId
#endif
	);

#ifdef __CLIENT
	bool SerializeFromWorld(char* input, int nLinkId);
#else
	void SerializeToClient(CItemElem* item, char* pszTarget);
	void AddItem(CItemElem* pItemElem, int& nIndex);
#endif

	CItemElem* GetLinkedItem(int nIndex);
};

#define ItemLink CLinkedItemMgr::GetInstance()
#endif