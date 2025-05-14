#ifndef __CREATEOBJ_H__
#define __CREATEOBJ_H__

#ifdef __CLIENT

#include "Sfx.h"


CSfx* CreateSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, D3DXVECTOR3& vPosSrc, OBJID idSrc = NULL_ID, D3DXVECTOR3& vPosDest = D3DXVECTOR3( 0, 0, 0), OBJID idDest = NULL_ID, int nSec = 0, BOOL bManageWorld = TRUE);
CSfx* CreateItemReadySfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, D3DXVECTOR3& vPosSrc, OBJID idSrc = NULL_ID, D3DXVECTOR3& vPosDest = D3DXVECTOR3( 0, 0, 0), OBJID idDest = NULL_ID, int nSec = 0 );
CSfxShoot* CreateShootSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, D3DXVECTOR3& vPosSrc, OBJID idSrc = NULL_ID, D3DXVECTOR3& vPosDest = D3DXVECTOR3( 0, 0, 0), OBJID idDest = NULL_ID, int nSec = 0 );
CSfx* CreateSfxYoYo( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, D3DXVECTOR3& vPosSrc, OBJID idSrc = NULL_ID, D3DXVECTOR3& vPosDest = D3DXVECTOR3( 0, 0, 0), OBJID idDest = NULL_ID, int nSec = 0 );

#endif // __CLIENT

#ifdef __LAYER_1015
CMover *CreateMover( CWorld *pWorld, DWORD dwID, const D3DXVECTOR3 &vPos, int nLayer );
#else	// __LAYER_1015
CMover *CreateMover( CWorld *pWorld, DWORD dwID, D3DXVECTOR3 vPos );
#endif	// __LAYER_1015
CObj* CreateObj( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwObjType, DWORD dwObjIndex, BOOL bInitProp = TRUE );
CObj* ReadObj( CFileIO* pFile );

BOOL CheckVersion( FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion );
BOOL WriteVersion( FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion );
BOOL CheckVersion( FILE* fp, DWORD dwVersion );
BOOL WriteVersion( FILE* fp, DWORD dwVersion );

#endif // __CREATEOBJ_H__