#ifndef __WNDPIERCING__H
#define __WNDPIERCING__H

class CPiercingMessageBox : public CWndMessageBox
{
public:
	CPiercingMessageBox() {};
	~CPiercingMessageBox() {};
	OBJID m_Objid[3];

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
};

#endif
