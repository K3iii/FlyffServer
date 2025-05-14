#ifndef __CAMERA_H
#define __CAMERA_H

class CWorld;
class CObj;

#define CAMSTY_NONE    0
#define CAMSTY_HIT     1
#define CAMSTY_DAMAGE  2

extern D3DXMATRIX g_matView;
extern D3DXMATRIX g_matInvView;

class CCamera
{
//protected:
public:
	D3DXVECTOR3    m_vPos        ;


	D3DXMATRIX     m_matView     ;
	D3DXMATRIX     m_matInvView  ;

	D3DXVECTOR3    m_vLookAt     ;

	D3DXVECTOR3    m_vPosDest    ;
	D3DXVECTOR3    m_vLookAtDest ;

	D3DXVECTOR3    m_vOffset     ;
	D3DXVECTOR3    m_vOffsetDest ;

	DWORD          m_dwCamStyle  ;
	FLOAT m_fRotx,m_fRoty;
	FLOAT m_fCurRotx,m_fCurRoty;

	int		m_nQuakeSec;
	float	m_fQuakeSize;

	CCamera();

	void Reset();
	void SetPos( D3DXVECTOR3& vPos ) { m_vPos = vPos; }
	D3DXVECTOR3 GetPos() { return m_vPos; }

	void	SetQuake( int nSec, float fSize = 0.06f );

virtual	void Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor=15.0f );
virtual	void Transform( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld );
virtual void ControlCamera( DWORD dwStyle );
};

class CBackCamera : public CCamera
{
	BOOL m_bLock;

public:
	FLOAT m_fLength1;
	FLOAT m_fLength2;
	BOOL  m_bOld;
	BOOL  m_bStart;
	FLOAT m_fZoom;

#if __VER >= 13 // __HOUSING
#define CM_NORMAL	0
#define CM_MYROOM	1

	int m_nCamMode;
	BOOL SetCamMode(int nType);
#endif	// __HOUSING
	CBackCamera();

	BOOL IsLock() { return m_bLock; }
	void Lock();
	void Unlock();

	int  GetAnglePie( FLOAT fAngle );

	virtual	void Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor=15.0f );
	virtual void ControlCamera( DWORD dwStyle );

};




class CFlyCamera : public CCamera
{
	BOOL m_bLock;
	FLOAT	m_fAngle;
	FLOAT	m_fAngleY;

public:
	CObj* pObjTarget;
	FLOAT m_fZoom;

	CFlyCamera();

	BOOL IsLock() { return m_bLock; }
	void Lock();
	void Unlock();

	virtual	void Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor=15.0f );
	virtual void ControlCamera( DWORD dwStyle );
};

class CToolCamera : public CCamera
{
public:
	//D3DXVECTOR3        m_vPosition;
	D3DXVECTOR3        m_vVelocity;
	FLOAT              m_fYaw;
	FLOAT              m_fYawVelocity;
	FLOAT              m_fPitch;
	FLOAT              m_fPitchVelocity;
	//D3DXMATRIXA16         m_matView;
	D3DXMATRIXA16         m_matOrientation;
	CToolCamera();
	virtual	void Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor=15.0f );
	virtual	void Transform( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld );

};


#endif