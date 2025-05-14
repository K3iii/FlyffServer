#ifndef __TREE_INFORMATION_H__
#define __TREE_INFORMATION_H__

#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */ && defined( __CLIENT )
//-----------------------------------------------------------------------------
class CTreeInformation
{
public:
	CTreeInformation( void );
	~CTreeInformation( void );

public:
	void SetData( DWORD dwData );
	DWORD GetData( void ) const;
	void SetOpen( BOOL bOpen );
	BOOL GetOpen( void ) const;

private:
	DWORD m_dwData;
	BOOL m_bOpen;
};
//-----------------------------------------------------------------------------
typedef list< CTreeInformation* > TreeInformationList;

class CTreeInformationManager
{
public:
	CTreeInformationManager( void );
	~CTreeInformationManager( void );

public:
	enum QuestListGroup { ALL_QUEST_LIST, CURRENT_QUEST_LIST, COMPLETE_QUEST_LIST };

public:
	TreeInformationList* GetTreeInformationList( void );
	CTreeInformation* GetTreeInformation( DWORD dwData ) const;
	void InsertTreeInformation( DWORD dwData, BOOL bOpen );
	void DeleteTreeInformation( DWORD dwData );
	void DeleteAllTreeInformation( void );
	int GetTreeInformationListSize( void ) const;

public:
	static int m_nSelectedTabNumber;
	static QuestListGroup m_eQuestListGroup;

private:
	TreeInformationList m_TreeInformationList;
};
//-----------------------------------------------------------------------------
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

#endif // __TREE_INFORMATION_H__