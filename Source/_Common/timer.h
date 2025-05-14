#ifndef _CTimer_H_
#define _CTimer_H_

#define	MIN(x)	(1000*60*((int)x))
#define	SEC(x)	(1000*((int)x))

class CTimer
{
protected:
	double	m_curTime;
	double	m_endTime;
	float	m_fInterval;

public:
	// Constructions
	CTimer();
	CTimer( float fInterval	);
	CTimer( float fInterval, BOOL bFirstTimeOut );
	virtual ~CTimer()	{}

	// Operations
	void	Set( float fInterval, BOOL bFirstTimeOut = FALSE );
	virtual	void	Reset( void );

	virtual	BOOL	Over( void );

	BOOL	IsOver( void )	{	return Over();	}
	BOOL	IsTimeOut( void )	{	return Over();	}
	BOOL	TimeOut( void )		{	return Over();	}

	float	GetInterval( void );
	double	GetLeftTime( void );
	double	GetTime( void );

	void	operator()( void );
	void	operator()( double val );
};

#define SKIPPING_PARAM 6
class CSkipTimer : public CTimer
{
protected:
	int		m_bMaxSkip;

public:
	// Constructions
	CSkipTimer() : CTimer()		{}
	CSkipTimer( float fInterval ) : CTimer( fInterval )	{	m_bMaxSkip = 0;	}
	virtual	~CSkipTimer()	{}

	// Operations
	void	Set( float fInterval );

	virtual	BOOL	Over( void );

	BOOL	IsNeedSkip( void );
	BOOL	IsMaxSkip( void );

	void	Skip( void );
};

class CGameTimer
{
	double m_dCurrentTime;
	double m_dBeginTime;
public:
	BOOL m_bFixed;
	int m_nSec;
	int m_nMin;
	int m_nHour;
	int m_nDay;

	CGameTimer();
	CGameTimer( double dCurrentime );
	~CGameTimer();

	void SetCurrentTime( int nDay, int nHour );
	void SetCurrentTime( double dCurrentime );
	double GetCurrentTime();

	void SetTime( int nDay, int nHour, int nMin, int nSec );
	void SetFixed( BOOL bFixed ) { m_bFixed = bFixed; }
	BOOL IsFixed( ) { return m_bFixed; }

	int GetSunPercent();
	int GetMoonPercent();

	void Compute();
};

typedef struct tagTIMECOLOR
{
	int  m_nDarkAlpha;
	char m_nR;
	char m_nG;
	char m_nB;
} TIMECOLOR,* LPTIMECOLOR;

class CDate //: public CObject
{
	//DECLARE_SERIAL(CDate)
	//void Serialize (CArchive& ar);
	CSkipTimer m_delay;

public:
	int m_nDelay;
	int m_nYear;
	int m_nMonth;
	int m_nDay;
	int m_nHour;
	int m_nMinute;
	int m_nSecond;

	CDate();
	CDate(int nYear,int nMonth,int nData,int nHour,int nMinute = 0,int nSecond = 0,int m_nDelay = 125);
	~CDate();
	BOOL IsNight();
	int	 GetDay( void );
	int  ComputeShade(int nMax = 8);
	void GetTimeColor(LPTIMECOLOR lpTileColor);

	void SetDelay(int nDelay);
	void SetDate(int nYear,int nMonth,int nDay);
	void SetTime(int nHour,int nMinute,int nSecond);
	void SetTime(int nYear,int nMonth,int nDay,int nHour,int nMinute,int nSecond);
	void Process();
};

#define		FRAME_PER_SEC		60
#define		MAX_SKIP_FRAME		4
#define		SEC1				PROCESS_COUNT

#ifdef __WORLDSERVER
#define		PROCESS_COUNT		(FRAME_PER_SEC / 4)
#else
#define		PROCESS_COUNT		FRAME_PER_SEC
#endif

BOOL	GetFrameSkip( void );
void	SetFrameSkip( BOOL bFlag );
void	InitFST( void );
BOOL	IsDrawTiming( void );
void	SetFST( void );


#endif 