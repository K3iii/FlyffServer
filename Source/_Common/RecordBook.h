#ifndef __RECORDBOOK_H__
#define	__RECORDBOOK_H__

#ifdef __FL_RECORD_BOOK
#include "RecordParameters.h"

#define MAX_TOP_RECORDS 15

class CRecordBook
{
	struct CompareByValueDescending
	{
		bool operator() (const PlayerRecord& lhs, const PlayerRecord& rhs) const
		{
			return lhs.n64Value > rhs.n64Value;
		}
	};

	std::array<std::vector<PlayerRecord>, MAX_RECORDTYPE> m_arrayRecords;
	std::unordered_map<u_long, PlayerEquipment> m_mapPlayerEquipments;

public:
	CRecordBook();
	virtual ~CRecordBook();
	void Clear();
	static CRecordBook* GetInstance();

	vector<PlayerRecord>* GetVectorRecords(int nType);
	std::unordered_map<u_long, PlayerEquipment>* GetPlayerEquipments() { return &m_mapPlayerEquipments; };

	void SerializeAllRecords(CAr& ar);

#ifdef __WORLDSERVER
	void AddPlayerRecord(CMover* pMover, int nRecordType, __int64 n64Value, bool bAddToCurrent);
#endif
#ifdef __DBSERVER
	void UpdateGlobalRecords(CMover* pMover);
	void InsertPlayerEquipment(CMover* pMover);
	void RemovePlayerEquipment(u_long idPlayer);
	bool FindPlayer(u_long idPlayer);
	int FindPlayerIndex(u_long idPlayer, vector<PlayerRecord>* pVec);

	void LoadRecordBook();
	void LoadPlayerEquipments(std::unordered_set<u_long>* setPlayers);
	void SaveRecordBook();
	void SortAllRecords();
	void SendRecordBook(DPID dpId);

	// So as i not really sure how threads can conflict between each other, ill use it
	// There is 2 threads using record book now: first thread is main thread (databaseserver.cpp), and second thread (dptrans.cpp)
	// I not really sure what can happen in case if send/save will be used at the same time with update records/update equipment, for example
	CMclCritSec	m_AccessRb;
#endif
};
#endif

#endif	// __RECORDBOOK_H__