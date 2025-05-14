// WndArcane.cpp: implementation of the CWndNeuz class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resdata.h"
#include "defineText.h"
#include "defineSound.h"
#include "defineObj.h"
#include "Sfx.h"

#include "Ship.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;


//int		__bTestLOD = 1;

int CWndWorld::ControlPlayer(DWORD dwMessage, CPoint point)
{
	if (!IsValidObj(g_pPlayer) || !g_pPlayer->IsPossibleSetCmd(TRUE))
		return 0;

	if (g_pPlayer->m_pActMover->IsFly())
		return ControlFlying(dwMessage, point);
	return ControlGround(dwMessage, point);
}

int		CWndWorld::ControlGround(DWORD dwMessage, CPoint point)
{
	bool	fCastCancel = false;

	BOOL	bUp, bDown, bLeft, bRight, bSpace, bLForward = FALSE, bRForward = FALSE;
	BOOL	bWalk;
	static int	s_bWalk2 = 0;

	CMover* pMover = CMover::GetActiveMover();
	if (!pMover)
		return 0;

	bUp = g_bKeyTable[g_Neuz.Key.chUp];
	bDown = g_bKeyTable['S'];
	bLeft = g_bKeyTable[g_Neuz.Key.chLeft];
	bRight = g_bKeyTable['D'];
	bSpace = g_bKeyTable[VK_SPACE];

	if (pMover->m_pActMover->GetState() & OBJSTA_ATK_ALL)
		m_bLButtonDown = FALSE;

#ifdef __BS_ADJUST_SYNC
	if (bUp || bDown)
		m_bLButtonDown = FALSE;
#endif

	if (bDown)
	{
		g_WndMng.m_bAutoRun = FALSE;
	}
	if (bUp)
	{
		m_timerAutoRunPush.Reset();
		if (m_nDubleUp == 2 && m_timerAutoRun.TimeOut() == FALSE)
		{
			m_nDubleUp = 3;
			g_WndMng.m_bAutoRun = TRUE;
			//g_WndMng.PutString("Auto run activated! Click 'W' or 'S' to cancel.");
			m_timerAutoRun.Reset();
			m_timerAutoRunBlock.Reset();
		}
		else
		{
			m_nDubleUp = 1;
			m_timerAutoRun.Reset();
		}
		if (m_timerAutoRunBlock.TimeOut())
			g_WndMng.m_bAutoRun = FALSE;
	}
	else
	{
		if (m_timerAutoRunPush.TimeOut() == FALSE)
		{
			if (m_nDubleUp == 1)
				m_nDubleUp = 2;
		}
		else
		{
			m_nDubleUp = 0;
		}
	}

	if (g_WndMng.m_bAutoRun)
		bUp = TRUE;

	CActionMover* pAct = pMover->m_pActMover;
	pAct->m_dwCtrlMsg = 0;

	if (m_bLButtonDown)
		pAct->m_dwCtrlMsg |= CTRLMSG_LDOWN;

	CWorld* pWorld = g_WorldMng.Get();
	CRect rect = GetClientRect();
	D3DXVECTOR3 vRayEnd;
	CObj* pFocusObj = pWorld->GetObjFocus();

#if __VER >= 12
	D3DXVECTOR3 vec3Tri[3];
	pWorld->ClientPointToVector(vec3Tri, rect, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, &vRayEnd, TRUE);
	g_Neuz.m_vCursorPos = vRayEnd;

	if (g_Neuz.m_pCreateMonItem)
	{
		if (bUp || bDown || bLeft || bRight || bSpace || m_bLButtonDown)
		{
			BOOL bSendCM = TRUE;
			if (m_bLButtonDown)
			{
				D3DXVECTOR3 vDist2 = g_pPlayer->GetPos() - g_Neuz.m_vCursorPos;
				float fDist = D3DXVec3Length(&vDist2);
				if (15.f < fDist)
				{
					g_WndMng.PutString(prj.GetText(TID_GAME_CREATEMON_F_15), NULL, prj.GetTextColor(TID_GAME_CREATEMON_F_15));
					bSendCM = FALSE;
				}
				if (bSendCM)
				{
					int nAttr = g_pPlayer->GetWorld()->GetHeightAttribute(g_Neuz.m_vCursorPos.x, g_Neuz.m_vCursorPos.z);
					if (nAttr == HATTR_NOWALK || nAttr == HATTR_NOMOVE
						|| g_pPlayer->IsRegionAttr(RA_SAFETY) || g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
					{
						g_WndMng.PutString(prj.GetText(TID_GAME_CREATEMON_F_AREA), NULL, prj.GetTextColor(TID_GAME_CREATEMON_F_AREA));
						bSendCM = FALSE;
					}
					else if (g_pPlayer->GetWorld()->GetID() != WI_WORLD_MADRIGAL)
					{
						g_WndMng.PutString(prj.GetText(TID_GAME_CREATEMON_F_AREA), NULL, prj.GetTextColor(TID_GAME_CREATEMON_F_AREA));
						bSendCM = FALSE;
					}

					if (bSendCM)
					{
						g_DPlay.SendCreateMonster(g_Neuz.m_pCreateMonItem->m_dwObjId, g_Neuz.m_vCursorPos);
					}
				}
				m_bLButtonDown = FALSE;
			}
			if (bSendCM)
				g_Neuz.m_pCreateMonItem = NULL;
		}
	}
#endif // __ITEMCREATEMON_S0602

	if (m_bLButtonDown)
	{
		D3DXVECTOR3 vec3Tri[3];
		if (pWorld->ClientPointToVector(vec3Tri, rect, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, &vRayEnd, TRUE))
		{
			if (m_bFreeMove)
			{
				pMover->SetDestPos(vRayEnd);
				m_bFreeMove = FALSE;

				g_DPlay.SendPlayerBehavior();
				g_DPlay.SendSnapshot(TRUE);
				fCastCancel = true;

				if (g_pMoveMark && g_pMoveMark->m_pSfxObj)
					g_pMoveMark->m_pSfxObj->m_nCurFrame = 180;
				CSfx* pObj = CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_MOVEMARK01, vRayEnd);

				D3DXVECTOR3 vVector1 = vec3Tri[2] - vec3Tri[0];
				D3DXVECTOR3 vVector2 = vec3Tri[1] - vec3Tri[0];
				D3DXVECTOR3 vNormal;
				D3DXVec3Cross(&vNormal, &vVector1, &vVector2);
				D3DXVec3Normalize(&vNormal, &vNormal);

				D3DXVECTOR3 v3Up = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
				D3DXVECTOR3 v3Cross;
				FLOAT fDot;
				FLOAT fTheta;
				D3DXVec3Cross(&v3Cross, &v3Up, &vNormal);
				fDot = D3DXVec3Dot(&v3Up, &vNormal);
				fTheta = acos(fDot);

				D3DXQUATERNION qDirMap;
				D3DXQuaternionRotationAxis(&qDirMap, &v3Cross, fTheta);

				D3DXVECTOR3 vYPW;
				QuaternionRotationToYPW(qDirMap, vYPW);

				pObj->m_pSfxObj->m_vRotate.x = D3DXToDegree(vYPW.x);
				pObj->m_pSfxObj->m_vRotate.y = D3DXToDegree(vYPW.y);
				pObj->m_pSfxObj->m_vRotate.z = D3DXToDegree(vYPW.z);

				ClearTracking();
			}
		}
	}

	if (bUp || bDown || bLeft || bRight || bSpace)
	{
		m_bAutoAttack = FALSE;
		g_pPlayer->ClearCmd();
		if (!bSpace)
			ClearTracking();
	}


	if (m_objidTracking != NULL_ID)
	{
		CMover* pObjTracking = prj.GetMover(m_objidTracking);
		if (pObjTracking)
		{
			D3DXVECTOR3 vDis = pMover->GetPos() - pObjTracking->GetPos();
			if (D3DXVec3LengthSq(&vDis) > 4.0f * 4.0f)
			{
				pMover->SetDestObj(m_objidTracking);
				static DWORD tick = GetTickCount();
				if (tick + SEC(3) < GetTickCount())
				{
					g_DPlay.SendPlayerBehavior();
					tick = GetTickCount();
				}
			}
		}
		else
			ClearTracking();
	}

	bool fMoved = false;
	bool fBehavior = false;

	if (bUp)
	{
		if (pMover->SendActMsg(OBJMSG_FORWARD) == 1)
		{
			fMoved = true;
			fCastCancel = true;
		}
	}
	else if (bDown)
	{
		if (pMover->SendActMsg(OBJMSG_BACKWARD) == 1)
		{
			fMoved = true;
			fCastCancel = true;
		}
	}
	else if (bUp == FALSE || bDown == FALSE)
	{
		if (pMover->IsEmptyDest())
		{
			if (pMover->m_pActMover->IsActJump() == FALSE)
			{
				if (pMover->SendActMsg(OBJMSG_STAND) == 1)
					fMoved = true;
			}
		}
	}

	if (bLeft)
	{
		if (pMover->SendActMsg(OBJMSG_LTURN) == 1)
			fMoved = true;
	}
	else if (bRight)
	{
		if (pMover->SendActMsg(OBJMSG_RTURN) == 1)
			fMoved = true;
	}
	else
	{
		if (pMover->SendActMsg(OBJMSG_STOP_TURN) == 1)
			fMoved = true;
	}


	//	jump
	if (bSpace && pMover->m_pActMover->m_bGround)
	{
		if (pMover->SendActMsg(OBJMSG_JUMP) == 1)
		{
			fBehavior = true;
			fCastCancel = true;
		}
	}
	if (m_bLButtonDown == TRUE && m_bRButtonDown == TRUE && pMover->m_pActMover->m_bGround)
	{
		if (m_timerLButtonDown.GetLeftTime() < 500 && m_timerRButtonDown.GetLeftTime() < 500)
		{
			if (g_pPlayer->SendActMsg(OBJMSG_JUMP) == 1)
			{
				fBehavior = true;
				fCastCancel = true;
			}
		}
	}

	bWalk = g_bKeyTable[g_Neuz.Key.chWalk];
	if (bWalk && !s_bWalk2)
	{
		if (pMover->m_pActMover->IsStateFlag(OBJSTAF_WALK))
		{
			if (pMover->SendActMsg(OBJMSG_MODE_RUN) == 1) {
				g_WndMng.PutString(prj.GetText(TID_GAME_RUN), NULL, prj.GetTextColor(TID_GAME_RUN), CHATSTY_SYSTEM_CLIENT);
				fBehavior = true;
			}
		}
		else
		{
			if (pMover->SendActMsg(OBJMSG_MODE_WALK) == 1) {
				g_WndMng.PutString(prj.GetText(TID_GAME_WALK), NULL, prj.GetTextColor(TID_GAME_WALK), CHATSTY_SYSTEM_CLIENT);
				fBehavior = true;
			}
		}
	}
	s_bWalk2 = bWalk;

	if (fMoved || fBehavior)
	{
		g_pPlayer->ClearDest();
		g_DPlay.SendPlayerBehavior();
	}

	if (g_pPlayer->IsStateMode(STATE_BASEMOTION_MODE) && fCastCancel)
		g_DPlay.SendStateModeCancel(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL);

	return 0;
}

int		CWndWorld::ControlFlying(DWORD dwMessage, CPoint point)
{
	static	float	fTurnAngle = 0.0f;
	static BOOL	s_bTraceKeyed = 0, s_bSelectKeyed = 0, s_bTurbo2 = 0;

	int		nMsg = 0;

	BOOL	bUp, bDown, bLeft, bRight;
	BOOL	bAcc = FALSE;
	BOOL	bTurbo;

	BYTE nFrame = MAX_CORR_SIZE_150;
	CMover* pMover = CMover::GetActiveMover();

	bUp = g_bKeyTable[g_Neuz.Key.chUp];
	bDown = g_bKeyTable['S'];
	bLeft = g_bKeyTable[g_Neuz.Key.chLeft];
	bRight = g_bKeyTable['D'];

	bool fMoved = false;
	bool fBehavior = false;

	if (pMover->m_pActMover->IsStateFlag(OBJSTAF_ACC))
	{
		if (pMover->SendActMsg(OBJMSG_FORWARD) == 1)
			fMoved = true;
	}
	else
	{
		if (pMover->SendActMsg(OBJMSG_STAND) == 1)
			fMoved = true;
	}

	bAcc = g_bKeyTable[VK_SPACE];
	if (bAcc && !s_bAccKeyed)
	{
		if (pMover->m_pActMover->IsStateFlag(OBJSTAF_ACC))
		{
			pMover->SendActMsg(OBJMSG_ACC_STOP);
			if (pMover->m_pActMover->IsActTurn())
			{
				fMoved = true;
			}
		}
		else
		{
			if (pMover->SendActMsg(OBJMSG_ACC_START) == 0)
				g_WndMng.PutString(prj.GetText(TID_GAME_AIRFUELEMPTY));
			else
			{
				if (pMover->SendActMsg(OBJMSG_FORWARD) == 1)
					fMoved = true;
			}
		}
	}
	s_bAccKeyed = bAcc;

	bTurbo = g_bKeyTable[g_Neuz.Key.chWalk];
	if (bTurbo && !s_bTurbo2)
	{
		if (pMover->m_pActMover->IsStateFlag(OBJSTAF_TURBO))
		{
			if (pMover->SendActMsg(OBJMSG_MODE_TURBO_OFF) == 1)
				fMoved = true;
		}
		else
		{
			if (pMover->SendActMsg(OBJMSG_MODE_TURBO_ON) == 1)
				fMoved = true;
		}
	}
	s_bTurbo2 = bTurbo;

	if (pMover->m_pActMover->IsFly())
	{
		if (g_bKeyTable[g_Neuz.Key.chTrace] && !s_bTraceKeyed)
		{
			CCtrl* pFocusObj = (CCtrl*)(pMover->GetWorld()->GetObjFocus());
			if (pFocusObj && pFocusObj->GetType() == OT_MOVER)
			{
				CMover* pFocusMover = (CMover*)pFocusObj;
				if (pMover->m_dwFlag & MVRF_TRACKING)
				{
					pMover->m_dwFlag &= (~MVRF_TRACKING);
					pMover->m_idTracking = NULL_ID;
				}
				else
				{
					pMover->m_dwFlag |= MVRF_TRACKING;
					pMover->m_idTracking = pFocusMover->GetId();
				}
			}
			else
			{
				pMover->m_dwFlag &= (~MVRF_TRACKING);
				pMover->m_idTracking = NULL_ID;
			}
		}
		s_bTraceKeyed = g_bKeyTable[g_Neuz.Key.chTrace];


		if (g_bKeyTable[VK_TAB] && !s_bSelectKeyed)
		{
			if (m_aFlyTarget.GetSize() > 0)
			{
				if (m_nSelect >= m_aFlyTarget.GetSize())
					m_nSelect = 0;
				OBJID idSelect = m_aFlyTarget.GetAt(m_nSelect++);
				CMover* pSelectMover = prj.GetMover(idSelect);
				if (IsValidObj(pSelectMover))
				{
					CWorld* pWorld = pMover->GetWorld();
					if (pWorld)
					{
						pWorld->SetObjFocus(pSelectMover);
						pMover->m_idTracking = pSelectMover->GetId();
					}
				}
			}
		}
		s_bSelectKeyed = g_bKeyTable[VK_TAB];
	}



	if ( /*m_bFlyMove &&*/ m_bLButtonDown || g_bKeyTable[VK_INSERT])	// 192 = `
	{
		CObj* pObj = pMover->GetWorld()->GetObjFocus();
		if (pObj && pObj->GetType() == OT_MOVER)
		{
			if (pMover->IsAttackAble(pObj))
			{
				OBJID	idTarget = ((CMover*)pObj)->GetId();
				ItemProp* pWeapon = pMover->GetActiveHandItemProp();
				if (pWeapon && pMover->IsRangeObj(pObj, pMover->GetAttackRange(pWeapon->dwAttackRange)))
				{
					g_pPlayer->PlayCombatMusic();

					if (pWeapon->dwItemKind3 == IK3_WAND)
					{
						D3DXVECTOR3 vFront, vTarget;
						AngleToVector(&vFront, g_pPlayer->GetAngle(), -g_pPlayer->GetAngleX(), 1.0f);
						vTarget = pObj->GetPos() - g_pPlayer->GetPos();
						D3DXVec3Normalize(&vTarget, &vTarget);
						FLOAT fDot = D3DXVec3Dot(&vFront, &vTarget);
						if (fDot >= cosf(D3DXToRadian(60.0f)))
						{
							if (pMover->IsRangeObj(pObj, 64.0f))
							{
								pMover->DoAttackMagic(pObj, 0);
							}
						}
					}
					else
					{
						pMover->SendActMsg(OBJMSG_ATK1, idTarget);
					}
				}
			}
		}
	}

	ItemProp* pItemProp = prj.GetItemProp(g_pPlayer->GetRideItemIdx());
	if (pItemProp)
	{
		fTurnAngle = pItemProp->fFlightLRAngle;
	}
	else
	{
		Error("빗자루정보 읽기 실패 %d", g_pPlayer->GetRideItemIdx());
		fTurnAngle = 0.6f;
	}

	if (bUp) {
		if (g_WorldMng.Get()->GetFullHeight(pMover->GetPos()) < pMover->GetPos().y) {
			if (pMover->SendActMsg(OBJMSG_LOOKDOWN) == 1) {
				fMoved = true;
			}
		}
	}
	else if (bDown) {
		if (pMover->SendActMsg(OBJMSG_LOOKUP) == 1) {
			fMoved = true;
		}
	}
	else {
		if (pMover->SendActMsg(OBJMSG_STOP_LOOK) == 1) {
			fMoved = true;
		}
	}

	if (bLeft) {
		m_fRollAng -= 1.0f;
		if (m_fRollAng < -45.0f)
			m_fRollAng = -45.0f;
		if (pMover->SendActMsg(OBJMSG_LTURN, (int)(fTurnAngle * 100.0f)) == 1) {
			fMoved = true;
		}
	}
	else if (bRight) {
		m_fRollAng += 1.0f;
		if (m_fRollAng > 45.0f)
			m_fRollAng = 45.0f;
		if (pMover->SendActMsg(OBJMSG_RTURN, (int)(fTurnAngle * 100.0f)) == 1) {
			fMoved = true;
		}
	}
	else {
		if (m_fRollAng < 0)
		{
			m_fRollAng += 2.0f;
			if (m_fRollAng > 0)	m_fRollAng = 0;
		}
		else
			if (m_fRollAng > 0)
			{
				m_fRollAng -= 2.0f;
				if (m_fRollAng < 0)	m_fRollAng = 0;
			}
		if (pMover->SendActMsg(OBJMSG_STOP_TURN) == 1) {
			fMoved = true;
			//			fBehavior	= true;
		}
	}

	// 오른쪽 버튼 드래그는 빗자루 움직임
	if (dwMessage == WM_MOUSEMOVE)
	{
		float fAng = pMover->GetAngle();
		float fAdd = (point.x - m_ptMouseOld.x) / 2.0f;
		fAng -= fAdd;
		pMover->SetAngle(fAng);

		float fAngX = pMover->GetAngleX();
		float fAddX = (point.y - m_ptMouseOld.y) / 4.0f;
		fAngX += fAddX;
		if (fAddX > 0 && fAngX > 45.0f)
			fAngX = 45.0f;
		else
			if (fAddX < 0 && fAngX < -45.0f)
				fAngX = -45.0f;

		pMover->SetAngleX(fAngX);

		if (fAdd || fAddX)
			g_DPlay.PostPlayerAngle(TRUE);
	}

	if (fBehavior)
	{
		pMover->ClearDest();
	}

	if (fMoved || fBehavior)
	{
		g_DPlay.SendPlayerBehavior();
	}

	return nMsg;
}
