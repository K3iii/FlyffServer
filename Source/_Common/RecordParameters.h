#pragma once
#ifdef __FL_RECORD_BOOK
enum RecordType
{
	STR,
	STA,
	DEX,
	INTT,
	ADOCH,
	HP,
	PVP_DMG,
	PVP_ASAL,
	PVP_HOP,
	PVP_CHARGEBOW,
	PVE_DMG,
	PVE_ASAL,
	PVE_HOP,
	PVE_CHARGEBOW,
	AWAKES_COUNT,
	NORMAL_UPGRADES,
	ACCE_UPGRADES,
	PIERCING_UPGRADES,
	ELEMENT_UPGRADES,
	MOBS_KILLED,
	BOSSES_KILLED,
	PLAYERS_KILLED,
	DEATHS,
	MVP,
	GW_WIN,
	SR_WIN,
	FFA_KILLS,
	TOTAL_PLAYTIME,
	COLLECTOR_ITEMS,
	QUESTS,
	DUNGEON_RUNS,
	CHEERS_GIVEN,
	CHEERS_RECEIVED,
	WOF_ATTEMPTS,
	TDM_KILLS,
	MAX_RECORDTYPE
};

struct PlayerRecord_Own
{
	bool bUpdated;
	int nLevel;
	int nJob;
	__int64 n64Value;

	PlayerRecord_Own()
	{
		bUpdated = false;
		nLevel = 0;
		nJob = 0;
		n64Value = 0;
	}

	void PlayerRecord_Own::SerializeAll(CAr& ar)
	{
		if (ar.IsStoring())
		{
			ar << nLevel;
			ar << nJob;
			ar << n64Value;
		}
		else
		{
			ar >> nLevel;
			ar >> nJob;
			ar >> n64Value;
		}
	}

	void PlayerRecord_Own::SerializeUpdated(CAr& ar)
	{
		if (ar.IsStoring())
		{
			ar << bUpdated;
			if (bUpdated)
			{
				ar << nLevel;
				ar << nJob;
				ar << n64Value;
			}
		}
		else
		{
			ar >> bUpdated;
			if (bUpdated)
			{
				ar >> nLevel;
				ar >> nJob;
				ar >> n64Value;
			}
		}
	}
};

struct PlayerRecord
{
	u_long idPlayer;
	int nLevel;
	int nJob;
	__int64 n64Value;

	PlayerRecord()
	{
		idPlayer = 0;
		nLevel = 0;
		nJob = 0;
		n64Value = 0;
	}

	void PlayerRecord::Serialize(CAr& ar)
	{
		if (ar.IsStoring())
		{
			ar << idPlayer;
			ar << nLevel;
			ar << nJob;
			ar << n64Value;
		}
		else
		{
			ar >> idPlayer;
			ar >> nLevel;
			ar >> nJob;
			ar >> n64Value;
		}
	}
};

struct PlayerEquipment
{
	int nJob;
	int nSex;
	DWORD dwHairMesh;
	DWORD dwHairColor;
	DWORD dwHeadMesh;
	EQUIP_INFO	aEquipInfo[MAX_HUMAN_PARTS];

	PlayerEquipment()
	{
		nJob = 0;
		nSex = 0;
		dwHairMesh = 0;
		dwHairColor = 0;
		dwHeadMesh = 0;
	}

	void PlayerEquipment::Serialize(CAr& ar)
	{
		if (ar.IsStoring())
		{
			ar << nJob;
			ar << nSex;
			ar << dwHairMesh;
			ar << dwHairColor;
			ar << dwHeadMesh;

			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				ar << aEquipInfo[i].dwId;
				ar << aEquipInfo[i].nOption;
#ifdef __MODEL_CHANGE
				ar << aEquipInfo[i].dwLook;
#endif
			}
		}
		else
		{
			ar >> nJob;
			ar >> nSex;
			ar >> dwHairMesh;
			ar >> dwHairColor;
			ar >> dwHeadMesh;

			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				ar >> aEquipInfo[i].dwId;
				ar >> aEquipInfo[i].nOption;
#ifdef __MODEL_CHANGE
				ar >> aEquipInfo[i].dwLook;
#endif
			}
		}
	}
};
#endif