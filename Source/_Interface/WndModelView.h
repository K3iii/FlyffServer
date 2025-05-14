#pragma once

#ifdef __MODEL_VIEW
class CWndModelView : public CWndNeuz
{
private:
	CMover m_Mover;
	FLOAT m_fRot;
	FLOAT m_fWheel;

	CModelObject* m_pPet;
	CModelObject* m_pEgg;

	CPoint m_ptOld;
	CPoint m_ptOld1;
	BOOL m_bLButtonDownRot;
public:
	CWndModelView(void);
	~CWndModelView(void);

public:
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnInitialUpdate();
	virtual BOOL Process();
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize);

	void UpdMover();
	void SetSex(BOOL bSex);
	bool SetPart(CItemElem* pItemElem);
	void UpdItem(DWORD dwParts, DWORD dwID);
};
#endif