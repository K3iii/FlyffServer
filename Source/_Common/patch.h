#ifndef PATCH_H
#define PATCH_H

class CLandscape;





class CPatch
{
protected:
	FLOAT *m_pHeightMap;

	BOOL m_bDirty;

	BOOL m_bVisible;
	D3DXVECTOR3  m_avBounds[8];
	D3DXPLANE    m_aplaneBounds[6];

	D3DXVECTOR3 m_vCenter;

public:
	int m_nWorldX,m_nWorldY;

	int m_nLevel;
	int m_nTopLevel;
	int m_nLeftLevel;
	int m_nRightLevel;
	int m_nBottomLevel;

	CPatch() { m_pHeightMap = NULL; m_nBottomLevel = m_nRightLevel = m_nLeftLevel = m_nTopLevel = m_nLevel = 0; }
	~CPatch();



	void Init( int heightX, int heightY, int worldX, int worldY, FLOAT *hMap );
	BOOL isVisibile( ) { return m_bVisible; }
	BOOL isDirty( ) { return m_bDirty; }
	void SetDirty( BOOL bDirty ) { m_bDirty=bDirty; }
	void Render(LPDIRECT3DDEVICE9 pd3dDevice,int X,int Y);

	void CalculateBound();
	void Cull();

	void CalculateLevel();
};

#endif
