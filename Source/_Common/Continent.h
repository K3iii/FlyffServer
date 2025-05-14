// Continent.h: interface for the CContinent class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTINENT_H__90911CD3_CA89_45BF_8632_B274DDDFE707__INCLUDED_)
#define AFX_CONTINENT_H__90911CD3_CA89_45BF_8632_B274DDDFE707__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define min_cont(x,y) (x < y ? x : y)
#define max_cont(x,y) (x > y ? x : y)
/*
#define CONT_NODATA			(BYTE)0x00
#define CONT_FLARIS			(BYTE)0x01
#define CONT_SAINTMORNING	(BYTE)0x02
#define CONT_RICIS			(BYTE)0x04
#define CONT_DARKON12		(BYTE)0x10
#define CONT_DARKON3		(BYTE)0x20

#define CONT_EAST			(BYTE)0x0f
#define CONT_WEST			(BYTE)0xf0

#define CONT_ALL			(BYTE)0xff
*/

#if __VER >= 15 // __USING_CONTINENT_DATA
#include "ContinentDef.h"
#else // __USING_CONTINENT_DATA
const BYTE CONT_NODATA			= 0x00;
const BYTE CONT_FLARIS			= 0x01;
const BYTE CONT_SAINTMORNING	= 0x02;
const BYTE CONT_RICIS			= 0x04;
const BYTE CONT_EAST			= 0x0f;

const BYTE CONT_DARKON12		= 0x10;
const BYTE CONT_DARKON3			= 0x20;
#if __VER >= 14 // __NEW_CONTINENT
const BYTE CONT_HARMONIN		= 0x40;
#endif // __NEW_CONTINENT
const BYTE CONT_WEST			= 0xf0;

const BYTE CONT_ALL				= 0xff;
#endif	//__USING_CONTINENT_DATA

#if __VER >= 12 // __SECRET_ROOM

class CContinent
{
public:
	CContinent();
	virtual ~CContinent();

	static CContinent*	GetInstance( void );
#ifndef __DBSERVER
	BYTE GetContinent( CMover* pMover );
	BYTE GetArea( CMover* pMover );
	static BOOL	IsValidObj( CMover* pMover );
#endif // !__DBSERVER
	CString GetContinentName( BYTE nCont );
#ifdef __WORLDSERVER
	D3DXVECTOR3 GetRevivalPos( BYTE nContinent );
#endif // __WORLDSERVER

#if __VER >= 14 // __NEW_CONTINENT
#ifdef __CLIENT
	int	GetMapNo( CMover* pMover );
	int	GetMapNo( const D3DXVECTOR3& vPos );		//gmpbigsun : const
#endif // __CLIENT
#endif // __NEW_CONTINENT

	int GetContinentIndex( const char* continentName );

public:
	BYTE GetContinent( const D3DXVECTOR3& vPos );

private:
	BYTE GetArea( BYTE nCont );

private:
	void Init();
	BOOL Point_In_Poly( vector<CPoint> vecMap, CPoint test_point );
	map< BYTE, vector<CPoint> > m_MapCont;
	map< BYTE, vector<CPoint> > m_MapContTown;
public:
	BYTE GetTown( const D3DXVECTOR3& vPos );
};

#endif // __SECRET_ROOM

#endif // !defined(AFX_CONTINENT_H__90911CD3_CA89_45BF_8632_B274DDDFE707__INCLUDED_)
