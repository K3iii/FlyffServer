#ifndef __RAINBOW_NPC_INFORMATION_PACK_H__
#define __RAINBOW_NPC_INFORMATION_PACK_H__

#include "RainbowNPCInformation.h"

#ifdef __CLIENT

class CRainbowNPCInformationPack : public CTexturePack
{
public:
	CRainbowNPCInformationPack( void );
	~CRainbowNPCInformationPack( void );

public:
	BOOL LoadScript( const CString& strFileName );

public:
	const CRainbowNPCInformation* GetRainbowNPCInformation( int nIndex ) const;

private:
	void DeleteAllRainbowNPCInformation( void );

private:
	RainbowNPCInformationVector m_RainbowNPCInformationVector;
};

#endif // __CLIENT

#endif // __RAINBOW_NPC_INFORMATION_PACK_H__