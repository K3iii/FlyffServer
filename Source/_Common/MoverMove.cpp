#include "stdafx.h"
#include "defineText.h"
#include "defineSound.h"
#include "defineSkill.h"
#include "defineObj.h"
#include "party.h"

#ifdef __CLIENT
#include "Sfx.h"
#include "DPClient.h"
#include "DialogMsg.h"
extern	CDPClient	g_DPlay;
#else	// __CLIENT
#include "User.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "dpdatabaseclient.h"
extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPSrvr		g_DPSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __CLIENT

extern	CPartyMng	g_PartyMng;


#ifdef __CLIENT
void CMover::PlayCombatMusic()
{
	if (g_Option.m_bBattleBGM == FALSE)
		return;

	if (GetWorld()->GetID() == WI_WORLD_GUILDWAR)
		return;

	if (GetWorld()->GetID() == WI_DUNGEON_MUSCLE || GetWorld()->GetID() == WI_DUNGEON_KRRR || GetWorld()->GetID() == WI_DUNGEON_BEAR)
		return;

	if (GetWorld()->m_bIsIndoor == FALSE)
	{
		FLOAT x = GetPos().x;
		FLOAT z = GetPos().z;
		CRect rectSaintmorning(7535, 1846, 9283, 3134);

		if (rectSaintmorning.PtInRect(CPoint((int)(x), (int)(z))))
			PlayMusic(BGM_BA_SAINTMORNING);
		else
			if (x < 4609)
				PlayMusic(BGM_BA_DARKON);
			else
				if (x < 6309)
					PlayMusic(BGM_BA_CRISIS);
				else
					if (x < 8138)
						PlayMusic(BGM_BA_FLARIS);
					else
						PlayMusic(BGM_BA_SAINTMORNING);
	}
}

void CMover::ProcessMoveArrival(CCtrl* pObj)
{
	if (IsActiveMover())
	{
		BOOL bSendUseSkill = FALSE;
		switch (m_oaCmd)
		{
		case OBJACT_USESKILL:
			if (pObj->GetType() == OT_MOVER)
			{
				CWorld* pWorld = GetWorld();
				D3DXVECTOR3 vStart = GetPos();			vStart.y += 0.5f;
				D3DXVECTOR3 vEnd = pObj->GetPos();	vEnd.y += 0.5f;

				int nSkillIdx = GetCmdParam(0);
				LPSKILL pSkill = GetSkill(0, nSkillIdx);
				if (pSkill == NULL)
				{
					Error("CMD_SetUseSkill : %s skill(%d) not found", m_szName, nSkillIdx);
					return;
				}

				if (!IsIgnoreCollisionSkill(pSkill->dwSkill) && pWorld->IntersectObjLine(NULL, vStart, vEnd, FALSE, FALSE))
				{
					g_WndMng.PutString(prj.GetText(TID_GAME_BLOCKTARGETING), NULL, prj.GetTextColor(TID_GAME_BLOCKTARGETING));
					g_WndMng.m_pWndWorld->SetNextSkill(NEXTSKILL_NONE);
					break;
				}

				PlayCombatMusic();

				OBJID idTarget = (OBJID)GetCmdParam(1);
				SKILLUSETYPE sutType = (SKILLUSETYPE)GetCmdParam(2);

				if (pSkill->dwSkill == SI_MAG_MAG_BLINKPOOL)
				{
					CWndWorld* pWndWorld;
					pWndWorld = (CWndWorld*)g_WndMng.m_pWndWorld;
					{
						vStart = GetPos();		vStart.y += 1.0f;
						vEnd = pWndWorld->m_vTelePos;
						if (pWorld->IntersectObjLine(NULL, vStart, vEnd, FALSE, FALSE))
						{
							g_WndMng.m_pWndWorld->SetNextSkill(NEXTSKILL_NONE);
							g_WndMng.PutString(prj.GetText(TID_GAME_BLOCKTARGETING), NULL, prj.GetTextColor(TID_GAME_BLOCKTARGETING));
							break;
						}
					}

					if (g_pMoveMark != NULL) g_pMoveMark->m_pSfxObj->m_nCurFrame = 180;
					CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_MOVEMARK01, pWndWorld->m_vTelePos);
				}
				else if (pSkill->dwSkill == SI_WIN_YOYO_BACKSTEP)
				{
					if (pWorld->IntersectObjLine(NULL, vStart, vEnd, FALSE, FALSE) == TRUE)
					{
						g_WndMng.m_pWndWorld->SetNextSkill(NEXTSKILL_NONE);
						g_WndMng.PutString(prj.GetText(TID_GAME_BLOCKTARGETING), NULL, prj.GetTextColor(TID_GAME_BLOCKTARGETING));
						break;
					}
				}

				if (pSkill->GetProp() && pSkill->GetProp()->dwAtkStyle == AS_BACK)
				{
					D3DXVECTOR3 v3Pos;
					D3DXVECTOR3 v3PosSrc;
					D3DXVECTOR3 v3PosDest;


					v3PosSrc = pObj->GetPos() - GetPos();
					D3DXVec3Normalize(&v3PosSrc, &v3PosSrc);


					AngleToVectorXZ(&v3Pos, pObj->GetAngle(), 3.0f);
					v3PosDest = (pObj->GetPos() + v3Pos) - pObj->GetPos();
					D3DXVec3Normalize(&v3PosDest, &v3PosDest);

					FLOAT fDir = D3DXVec3Dot(&v3PosSrc, &v3PosDest);


					if (fDir < 0.3f)
					{
						g_WndMng.PutString(prj.GetText(TID_GAME_NEVERKILLSTOP));
						break;
					}
				}

				ClearDestObj(FALSE);
				g_DPlay.SendPlayerBehavior();

				BOOL bControl = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) ? TRUE : FALSE);
				g_DPlay.SendUseSkill(0, nSkillIdx, idTarget, sutType, bControl);
				bSendUseSkill = TRUE;
				m_dwReqFlag |= REQ_USESKILL;
			}
			break;
			//------------------------------------------
		case OBJACT_MELEE_ATTACK:
			if (pObj->GetType() == OT_MOVER)
			{
				ItemProp* pItemProp = GetActiveHandItemProp();
				if (pItemProp && pItemProp->dwItemKind3 == IK3_YOYO)
				{
					CWorld* pWorld = GetWorld();
					D3DXVECTOR3 vStart = GetPos();			vStart.y += 0.5f;
					D3DXVECTOR3 vEnd = pObj->GetPos();	vEnd.y += 0.5f;

					if (pWorld->IntersectObjLine(NULL, vStart, vEnd, FALSE, FALSE))
					{
						g_WndMng.PutString(prj.GetText(TID_GAME_BLOCKTARGETING), NULL, prj.GetTextColor(TID_GAME_BLOCKTARGETING));
						break;
					}
				}

				ClearDestObj();
				g_DPlay.SendPlayerBehavior();
				DoAttackMelee((CMover*)pObj);
			}
			break;
			//---------------------------------------------
		case OBJACT_MAGIC_ATTACK:
			if (pObj->GetType() == OT_MOVER)
			{
				PlayCombatMusic();

				OBJID	idTarget = GetCmdParam(0);
				int		nMagicPower = GetCmdParam(1);
				CMover* pTarget = prj.GetMover(idTarget);
				if (IsInvalidObj(pTarget))
					break;

				ClearDestObj();
				g_DPlay.SendPlayerBehavior();
				DoAttackMagic(pTarget, nMagicPower);
			}
			break;
		case OBJACT_RANGE_ATTACK:
		{
			if (pObj->GetType() == OT_MOVER)
			{
				PlayCombatMusic();

				OBJID	idTarget = GetCmdParam(0);
				int		nPower = GetCmdParam(1);
				CMover* pTarget = prj.GetMover(idTarget);
				if (IsInvalidObj(pTarget))
					break;

				ClearDestObj();
				g_DPlay.SendPlayerBehavior();
				DoAttackRange(pTarget, nPower, 0);
			}
		}
		break;
		//---------------------------------------------
		case OBJACT_USEOBJECT:
		{
			OBJID	idTarget = GetCmdParam(0);
			CCtrl* pTarget = prj.GetCtrl(idTarget);
			if (!IsValidObj((CObj*)pTarget))
				break;

			ClearDestObj();
			g_DPlay.SendPlayerBehavior();
			g_DPlay.SendUseObject(idTarget);
			m_dwReqFlag |= REQ_USEOBJECT;
		}
		break;
		//---------------------------------------------
		default:
		{
			if (g_WndMng.m_pWndTaskBar && g_WndMng.m_pWndTaskBar->m_nExecute == 2)
			{
				// Wait till worldserver will go to the ProcessMoveArrival and will cast next skill in action slot
			}
			else
			{
				ClearDestObj();
				g_DPlay.SendPlayerBehavior();
			}
		}
		break;

		}

		if (OBJACT_USESKILL == m_oaCmd && FALSE == bSendUseSkill)
		{
			if (g_WndMng.m_pWndTaskBar)
				g_WndMng.m_pWndTaskBar->m_nExecute = 0;
		}

		SetCmd(OBJACT_NONE);
	}
}
#else // Client
void CMover::ProcessMoveArrivalBySynchronizer()
{
	CCtrl* pObj = GetDestObj();
	if (::IsValidObj(pObj) == TRUE)
		ProcessMoveArrival(pObj);
}

void CMover::ProcessMoveArrival(CCtrl* pObj)
{
	switch (m_oaCmd)
	{
	case OBJACT_USESKILL:
		if (pObj->GetType() == OT_MOVER)
		{
			int nSkillIdx = GetCmdParam(0);
			OBJID idTarget = (OBJID)GetCmdParam(1);
			SKILLUSETYPE sutType = (SKILLUSETYPE)GetCmdParam(2);

			LPSKILL pSkill = GetSkill(0, nSkillIdx);
			if (pSkill == NULL)
			{
				Error("ProcessMoveArrival mover:%s skill(%d) not found.", m_szName, nSkillIdx);
				return;
			}
			if (pSkill->dwSkill == SI_MAG_MAG_BLINKPOOL || pSkill->dwSkill == SI_RIG_HERO_RETURN)
				return;

			if (pSkill->dwSkill == SI_WIN_YOYO_BACKSTEP)
				return;

			CWorld* pWorld = GetWorld();
			D3DXVECTOR3 vStart = GetPos();			vStart.y += 0.5f;
			D3DXVECTOR3 vEnd = pObj->GetPos();	vEnd.y += 0.5f;

			if (!IsIgnoreCollisionSkill(pSkill->dwSkill) && pWorld->IntersectObjLine(NULL, vStart, vEnd, FALSE, FALSE))
			{
				if (IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_BLOCKTARGETING, "");
				break;
			}

			BOOL bSuccess = DoUseSkill(0, nSkillIdx, idTarget, sutType, FALSE);
			if (bSuccess == FALSE)
				if (IsPlayer())
					((CUser*)this)->m_playTaskBar.OnEndSkillQueue((CUser*)this);
		}
		break;

	default:
		OnArrive(pObj->GetId(), 0);
		break;
	} // switch

	SetCmd(OBJACT_NONE);
}
#endif // not CLIENT


void CMover::ProcessMove()
{
	if (IsEmptyDest())
		return;
	if (m_pActMover->IsActAttack())
		return;

	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDestPos = m_vDestPos;

	if (!IsEmptyDestPos())
	{
		bool bPositiveX = ((vPos.x - vDestPos.x) > 0.0f);
		bool bPositiveZ = ((vPos.z - vDestPos.z) > 0.0f);

		if (bPositiveX != m_bPositiveX || bPositiveZ != m_bPositiveZ)
		{
			D3DXVECTOR3 kDir = vPos - vDestPos;
			D3DXVec3Normalize(&kDir, &kDir);

			D3DXVECTOR3 kMyDir;
			AngleToVectorXZ(&kMyDir, GetAngle(), 1.0f);
			D3DXVec3Normalize(&kMyDir, &kMyDir);

			float fAngle = D3DXVec3Dot(&kDir, &kMyDir);
			if (fAngle > 0.0f)
			{
				OnArriveAtPos();
#ifdef __CLIENT
				if (IsActiveMover())
					g_DPlay.SendPlayerBehavior();
#endif
				return;
			}
		}
	}
	else
	{
		CCtrl* pObj = prj.GetCtrl(m_idDest);
		if (!IsValidObj(pObj))
		{
			SendActMsg(OBJMSG_STAND);
#ifdef __CLIENT
			if (IsActiveMover())
				ClearDestObj();
#endif
			return;
		}

		if (IsPlayer() && pObj->GetType() == OT_MOVER)
		{
			CMover* pMover = (CMover*)pObj;
			if (pMover->IsMode(TRANSPARENT_MODE) && pMover != this)
			{
				SendActMsg(OBJMSG_STAND);
#ifdef __CLIENT
				if (IsActiveMover())
					ClearDestObj();
#endif
				return;
			}
		}

		vDestPos = pObj->GetPos();
		BOOL bRangeObj = pObj->IsRangeObj(this, m_fArrivalRange);
		if (m_pActMover->IsFly())
		{
			if (bRangeObj)
			{
				ClearDestObj();
#ifdef __WORLDSERVER
				OnArrive(pObj->GetId(), 0);
#endif	// __WORLDSERVER
			}
		}
		else
		{
			if (bRangeObj)
			{
				SendActMsg(OBJMSG_STOP);
				ProcessMoveArrival(pObj);
				return;
			}
		}
			}

	if (m_pActMover->IsFly())
		SendActMsg(OBJMSG_TURNMOVE, (int)GetDegree(vDestPos, vPos), (int)GetDegreeX(vDestPos, vPos), 0);
	else
	{
		if (m_bForward)
			SendActMsg(OBJMSG_TURNMOVE, (int)GetDegree(vDestPos, vPos), (int)GetDegreeX(vDestPos, vPos));
		else
			SendActMsg(OBJMSG_TURNMOVE2, (int)GetDegree(vPos, vDestPos), (int)GetDegreeX(vPos, vDestPos));
	}
		}


void	CMover::ProcessMovePattern(void)
{
	if (m_nMovePattern == 1)
	{
		switch (m_nMoveEvent)
		{
		case 0:
			m_nMoveEvent++;
			m_nMoveEventCnt = 0;

		case 1:
			SendActMsg(OBJMSG_FORWARD);
			if ((m_nMoveEventCnt & 3) == 0)
				SendActMsg(OBJMSG_LTURN);
			else
				SendActMsg(OBJMSG_STOP_TURN);

			if (++m_nMoveEventCnt > SEC1 * 2)
			{
				m_nMoveEventCnt = 0;
				m_nMoveEvent++;
				//				SendActMsg( OBJMSG_STOP );
			}
			break;
		case 2:
			SendActMsg(OBJMSG_FORWARD);
			if ((m_nMoveEventCnt & 3) == 0)
				SendActMsg(OBJMSG_RTURN);
			else
				SendActMsg(OBJMSG_STOP_TURN);
			if (m_nMoveEventCnt < (SEC1 * 5) / 2)
			{
				SendActMsg(OBJMSG_LOOKUP);
			}
			else
				if (m_nMoveEventCnt == (SEC1 * 5) / 2)
				{
					SendActMsg(OBJMSG_STOP_LOOK);
					SetAngleX(0);
				}
				else
				{
					SendActMsg(OBJMSG_LOOKDOWN);
				}

			if (++m_nMoveEventCnt > SEC1 * 5)
			{
				m_nMoveEventCnt = 0;
				m_nMoveEvent++;
				SendActMsg(OBJMSG_STOP_LOOK);
				SendActMsg(OBJMSG_STOP_TURN);
				SendActMsg(OBJMSG_STOP);
				SetAngleX(0);
			}
			break;
		case 3:
			SendActMsg(OBJMSG_FORWARD);
			if (m_nMoveEventCnt < (SEC1 * 5) / 2)
			{
				if ((m_nMoveEventCnt & 3) == 0)
					SendActMsg(OBJMSG_RTURN);
				else
					SendActMsg(OBJMSG_STOP_TURN);
			}
			else
			{
				if ((m_nMoveEventCnt & 3) == 0)
					SendActMsg(OBJMSG_LTURN);
				else
					SendActMsg(OBJMSG_STOP_TURN);
			}

			if (++m_nMoveEventCnt > SEC1 * 5)
			{
				m_nMoveEventCnt = 0;
				m_nMoveEvent++;
				SendActMsg(OBJMSG_STOP_LOOK);
				SendActMsg(OBJMSG_STOP_TURN);
				SetAngleX(0);
			}
			break;
		case 4:
			SendActMsg(OBJMSG_FORWARD);
			if ((m_nMoveEventCnt & 3) == 0)
				SendActMsg(OBJMSG_LTURN);
			else
				SendActMsg(OBJMSG_STOP_TURN);
			if (m_nMoveEventCnt < (SEC1 * 5) / 2)
			{
				SendActMsg(OBJMSG_LOOKDOWN);
			}
			else
				if (m_nMoveEventCnt == (SEC1 * 5) / 2)
				{
					SendActMsg(OBJMSG_STOP_LOOK);
					SetAngleX(0);
				}
				else
				{
					SendActMsg(OBJMSG_LOOKUP);
				}

			if (++m_nMoveEventCnt > SEC1 * 5)
			{
				m_nMoveEventCnt = 0;
				m_nMoveEvent = 99;
				SendActMsg(OBJMSG_STOP_LOOK);
				SendActMsg(OBJMSG_STOP_TURN);
				SendActMsg(OBJMSG_STOP);
				SetAngleX(0);
			}
			break;

		case 99:
			break;
		}
	}
	else	// movePattern 1
		if (m_nMovePattern == 2)
		{
			switch (m_nMoveEvent)
			{
			case 0:
				m_nMoveEvent++;
				m_nMoveEventCnt = 0;

			case 1:
				SendActMsg(OBJMSG_FORWARD);

				if (++m_nMoveEventCnt > SEC1)
				{
					FLOAT fAngle = GetAngle();
					SetAngle(fAngle + 135.0f);
					m_nMoveEventCnt = 0;
					m_nMoveEvent++;
				}
				break;
			case 2:
				SendActMsg(OBJMSG_FORWARD);

				if (++m_nMoveEventCnt > SEC1 * 2)
				{
					FLOAT fAngle = GetAngle();
					SetAngle(fAngle - 90.0f);
					m_nMoveEventCnt = 0;
					m_nMoveEvent++;
					SendActMsg(OBJMSG_STOP_LOOK);
					SendActMsg(OBJMSG_STOP_TURN);
					SendActMsg(OBJMSG_STOP);
					SetAngleX(0);
				}
				break;
			case 3:
				SendActMsg(OBJMSG_FORWARD);

				if (++m_nMoveEventCnt > SEC1 * 2)
				{
					FLOAT fAngle = GetAngle();
					SetAngle(fAngle - 45.0f);
					m_nMoveEventCnt = 0;
					m_nMoveEvent++;
					SendActMsg(OBJMSG_STOP_LOOK);
					SendActMsg(OBJMSG_STOP_TURN);
					SetAngleX(0);
				}
				break;
			case 4:
				SendActMsg(OBJMSG_FORWARD);

				if (++m_nMoveEventCnt > SEC1 * 1)
				{
					FLOAT fAngle = GetAngle();
					SetAngle(fAngle - 45.0f);
					m_nMoveEventCnt = 0;
					m_nMoveEvent++;
					SendActMsg(OBJMSG_STOP_LOOK);
					SendActMsg(OBJMSG_STOP_TURN);
					SendActMsg(OBJMSG_STOP);
					SetAngleX(0);
				}
				break;
			case 5:
				SendActMsg(OBJMSG_FORWARD);

				if (++m_nMoveEventCnt > SEC1 * 3)
				{
					FLOAT fAngle = GetAngle();
					SetAngle(fAngle + 45.0f);
					m_nMoveEventCnt = 0;
					m_nMoveEvent = 99;
					SendActMsg(OBJMSG_STOP_LOOK);
					SendActMsg(OBJMSG_STOP_TURN);
					SendActMsg(OBJMSG_STOP);
					SetAngleX(0);
				}
				break;

			case 99:
				break;
			}
		} // MovePattern 2
}
