#ifndef __FSMSTATE_H__
#define __FSMSTATE_H__

struct FSM_STATE
{
	int		nInput;
	int		nOutput;
	DWORD	dwExtra;
};

class CFSMstate
{
private:
	vector< FSM_STATE* >	m_array;
	int						m_nStateID;
	int						m_nPolling;

public:
	CFSMstate( int nStateID );
	~CFSMstate();

	int			GetID() { return m_nStateID; }
	void		AddTransition( int nInput, int nOutputID, DWORD dwExtra );
	int			GetSize() { return (int)m_array.size(); }
	void		SetPolling( int nPolling );
	int			GetPolling();
	FSM_STATE*	GetEntry( int nIndex ) { return m_array[nIndex]; }
	BOOL		GetOutput( int nInput, int& nOutputID );
};

#endif // __FSMSTATE_H__
