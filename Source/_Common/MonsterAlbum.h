#ifndef __MONSTER_ALBUM_H__
#define	__MONSTER_ALBUM_H__

#ifdef __FL_MONSTER_ALBUM
#define CARDS_NEEDED_TO_UNLOCK 100
#define MAX_BOSS_CARDS 6

class CMonsterAlbumMng
{
public:
	struct CARD_STRUC
	{
		DWORD dwWorldID;
		DWORD dwCardPieceID;
		DWORD dwChanceToObtain;

		CARD_STRUC()
		{
			dwWorldID = NULL_ID;
			dwCardPieceID = NULL_ID;
			dwChanceToObtain = 0;
		}
	};

	struct ITEM_REWARD_STRUC
	{
		DWORD dwItemID;
		int nItemCount;
		BOOL bBinds;

		ITEM_REWARD_STRUC()
		{
			dwItemID = NULL_ID;
			nItemCount = 0;
			bBinds = FALSE;
		}
	};

	struct DST_REWARD_STRUC
	{
		DWORD dwDest;
		int nVal;

		DST_REWARD_STRUC()
		{
			dwDest = NULL_ID;
			nVal = 0;
		}
	};

	struct BONUS_STRUC
	{
		vector<ITEM_REWARD_STRUC> vecItems;
		vector<DST_REWARD_STRUC> vecDst;
	};

private:
	unordered_map<DWORD, CARD_STRUC> m_mapMonsters;
	map<size_t, BONUS_STRUC> m_mapBonuses;

public:
	CMonsterAlbumMng();
	virtual ~CMonsterAlbumMng();
	static CMonsterAlbumMng* GetInstance();
	
	unordered_map<DWORD, CARD_STRUC>* GetMonsters() { return &m_mapMonsters; };
	map<size_t, BONUS_STRUC>* GetBonuses() { return &m_mapBonuses; };

#if defined (__WORLDSERVER) || defined (__CLIENT)
	void LoadMonsterAlbum();
#endif
	bool FindMonster(DWORD dwID);
};

class CMonsterAlbum
{
	CMover* m_pMover;
	unordered_map<DWORD, int> m_mapMonsterProgress;
	array<DWORD, MAX_BOSS_CARDS> m_arrayBossCards; // NULL_ID - slot locked, 0 - slot unlocked but empty, other value - slot unlocked and have card in it

public:
	CMonsterAlbum(CMover* pMover);
	virtual ~CMonsterAlbum();

	unordered_map<DWORD, int>* GetMonsterProgress() { return &m_mapMonsterProgress; };
	array<DWORD, MAX_BOSS_CARDS>* GetBossCards() { return &m_arrayBossCards; };

	void Serialize(CAr& ar);

	int GetMonsterProgress(DWORD dwID);
	size_t GetCompletedMonstersCount();
	void MergeBonuses(unordered_map<DWORD, int>* pMergedDst);
#ifdef __WORLDSERVER
	void SetApplyDST();
	void ResetApplyDST();
	void AddMonsterCardPiece(DWORD dwCardObjId, int nType);
	void BossCardSlotUnlock(DWORD dwKeyObjId, int nType);
	void BossCardSlotInsert(DWORD dwBossCardObjId, int nType);
	void BossCardSlotRestore(DWORD dwKeyObjId, int nType, int nSlot);
#endif
#ifdef __CLIENT
	void AddMonsterCardPiece(DWORD dwMobID, int nAddedPieces);
	void BossCardSlotUnlock(int nSlot);
	void BossCardSlotInsert(int nSlot, DWORD dwBossCardID);
	void BossCardSlotRestore(int nSlot);
#endif
};
#endif

#endif	// __MONSTER_ALBUM_H__