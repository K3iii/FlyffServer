#include "stdafx.h"
#include "defineSkill.h"
#include "defineItem.h"
#include "defineSound.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndMessenger.h"
#include "WndCommand.h"
#include "WndNotice.h"
#include "WndNotice.h"
#include "WndMap.h"
#include "WndPartyConfirm.h"
#include "WndPartyChangeTroup.h"
#include "WndFriendConfirm.h"
#include "WndRegVend.h"
#include "WndText.h"
#include "party.h"
#include "DPClient.h"
#include "DPLoginClient.h"
#include "dpcertified.h"
#include "ClientMsg.h"
#include "yUtil.h"
#include "worldmap.h"
#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;
#include "post.h"
#if __VER >= 11 // __SYS_POCKET
#include "wndbagex.h"
#endif	// __SYS_POCKET

#include "ticket.h"

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

#include "WndSummonFriend.h"
#include "WndChangeFace.h"

#ifdef __Y_INTERFACE_VER3
#include "WorldMap.h"
#endif //__Y_INTERFACE_VER3

#ifdef __MODEL_VIEW
#include "WndModelView.h"
#endif

#include "WndSelectVillage.h"

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#endif	// __SYS_IDENTIFY

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 12 // __LORD
#include "definelordskill.h"
#endif
#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#include "webbox.h"

#define TMS_CLIENT   0x00000001
#define TMS_DIALOG   0x00000002
#define TMS_CAPTION  0x00000004
#define TMS_CHAT     0x00000008

extern	CParty	g_Party;

extern	CDPClient	g_DPlay;
extern	CDPLoginClient	g_dpLoginClient;
extern	CDPCertified	g_dpCertified;

#include "dpclient.h"
extern	CDPClient	g_DPlay;

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#include "WndGuildHouse.h"
#endif // __GUILD_HOUSE

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
#include "WndQuest.h"
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __CERTIFIER_COLLECTING_SYSTEM
#include "DPCollectClient.h"
#endif // __CERTIFIER_COLLECTING_SYSTEM

extern DWORD FULLSCREEN_WIDTH;
extern DWORD FULLSCREEN_HEIGHT;

#define CLIENT_WIDTH FULLSCREEN_WIDTH
#define CLIENT_HEIGHT FULLSCREEN_HEIGHT

#ifdef __VS2003
CPtrArray      m_wndOrder;
#endif

CWndMgr          g_WndMng;


const char* GetATKSpeedString(float fSpeed)
{
	if (fSpeed < 0.035)
		return prj.GetText(TID_GAME_VERYSLOW);
	else if (fSpeed < 0.050)
		return prj.GetText(TID_GAME_SLOW);
	else if (fSpeed < 0.070)
		return prj.GetText(TID_GAME_NORMALS);
	else if (fSpeed < 0.080)
		return prj.GetText(TID_GAME_FAST);
	else if (fSpeed < 0.17)
		return prj.GetText(TID_GAME_VERYFAST);
	else
		return prj.GetText(TID_GAME_FASTEST);
}

CTexture* g_pCoolTexArry[128];

CTexture* g_pEnchantTexArry[11];

void RenderEnchant(C2DRender* p2DRender, CPoint pt)
{
	static int i = 0;

	CPoint ptBack = pt;
	CPoint ptBack_size(32, 32);

	i++;

	if (i >= 11)
		i = 0;

	p2DRender->RenderTextureEx(ptBack, ptBack_size, g_pEnchantTexArry[i], 255, 1.0f, 1.0f);
}


void RenderRadar(C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor)
{
	pt.y += 1;
	pt.x += 15;

	CPoint pt_size(15, 15);
	const BYTE bAlpha = 200;

	int nDrawIndex = MulDiv(dwValue, 127, dwDivisor);

	CPoint ptBack = pt;
	CPoint ptBack_size(32, 32);
	ptBack.x -= 15;
	p2DRender->RenderFillRect(CRect(ptBack, ptBack + ptBack_size), D3DCOLOR_ARGB(120, 255, 255, 255));

	if (nDrawIndex >= 0 && nDrawIndex < 32)
	{
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f);

		pt.y += 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[32], bAlpha, 1.1f, 1.1f);

		pt.x -= 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[64], bAlpha, 1.1f, 1.1f);

		pt.y -= 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f);
	}
	else if (nDrawIndex >= 32 && nDrawIndex <= 63)
	{
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f);

		pt.y += 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f);

		pt.x -= 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[64], bAlpha, 1.1f, 1.1f);

		pt.y -= 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f);
	}
	else if (nDrawIndex >= 64 && nDrawIndex <= 95)
	{
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f);

		pt.y += 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[63], bAlpha, 1.1f, 1.1f);

		pt.x -= 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f);

		pt.y -= 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f);
	}
	else if (nDrawIndex >= 96 && nDrawIndex <= 128)
	{
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f);

		pt.y += 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[63], bAlpha, 1.1f, 1.1f);

		pt.x -= 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[95], bAlpha, 1.1f, 1.1f);

		pt.y -= 16;
		p2DRender->RenderTextureEx(pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f);
	}

}

CWndMgr::CWndMgr()
	: m_pWndWorld(NULL)
	, m_bTitle(FALSE)
	, m_clearFlag(FALSE)
	, m_bAllAction(FALSE)
	, m_bWaitRequestMail(FALSE)
	, m_bIsOpenLordSkill(false)
	, m_dwSavePlayerTime(0)
	, m_strChatBackup(_T(""))
	, m_bAutoRun(FALSE)
{
	ZeroMemory(m_szTimerChat, sizeof(TCHAR) * 128);
	ZeroMemory(&dwItemColor, sizeof(ToolTipItemTextColor));

	ZeroMemory(g_pCoolTexArry, sizeof(CTexture*) * 128);
	ZeroMemory(g_pEnchantTexArry, sizeof(CTexture*) * 11);

	m_timerChat.Set(0);

	m_nAlphaCount = m_nAlpha;

	InitSetItemTextColor();
	RegisterAllCustomChild();

	m_pWndRoot = this;

#ifdef __FL_UPDATE_CHATTING
	m_dwLastSentChatMessage = GetTickCount();
#endif
}


CWndMgr::~CWndMgr()
{
	AppletFunc* pApplet(NULL);
	DWORD			dwIdApplet(NULL);

	POSITION pos = m_mapAppletFunc.GetStartPosition();
	while (pos)
	{
		m_mapAppletFunc.GetNextAssoc(pos, dwIdApplet, (void*&)pApplet);
		SAFE_DELETE((AppletFunc*)pApplet);
	}

	Free();
	m_vecChildWnds.clear();
	DeleteDeviceObjects();
}

void CWndMgr::RegisterAllCustomChild()
{
	m_vecChildWnds.reserve(400);

	RegisterCustomChild((CWndBase*&)m_pWndBeautyShop);
	RegisterCustomChild((CWndBase*&)m_pWndFaceShop);
	RegisterCustomChild((CWndBase*&)m_pWndUseCouponConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndDialog);
	RegisterCustomChild((CWndBase*&)m_pWndShop);
	RegisterCustomChild((CWndBase*&)m_pWndTrade);
	RegisterCustomChild((CWndBase*&)m_pWndTradeGold);
	RegisterCustomChild((CWndBase*&)m_pWndDropItem);
	RegisterCustomChild((CWndBase*&)m_pWndConfirmTrade);
	RegisterCustomChild((CWndBase*&)m_pWndTradeConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndCommand);
	RegisterCustomChild((CWndBase*&)m_pWndBank);
	RegisterCustomChild((CWndBase*&)m_pPetRes);
	RegisterCustomChild((CWndBase*&)m_pWndKawiBawiBoGame);
	RegisterCustomChild((CWndBase*&)m_pWndKawiBawiBoGameConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndSmeltMixJewel);
	RegisterCustomChild((CWndBase*&)m_pWndSmeltJewel);
	RegisterCustomChild((CWndBase*&)m_pWndExtraction);
	RegisterCustomChild((CWndBase*&)m_pWndPetStatus);
	RegisterCustomChild((CWndBase*&)m_pWndFoodConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndPetMiracle);
	RegisterCustomChild((CWndBase*&)m_pWndPetFoodMill);
	RegisterCustomChild((CWndBase*&)m_pWndPetLifeConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndHeroSkillUp);
	RegisterCustomChild((CWndBase*&)m_pWndRemovePiercing);
	RegisterCustomChild((CWndBase*&)m_pWndDialogEvent);
	RegisterCustomChild((CWndBase*&)m_pWndHeavenTower);
	RegisterCustomChild((CWndBase*&)m_pWndChatLog);
	RegisterCustomChild((CWndBase*&)m_pWndRemoveAttribute);
	RegisterCustomChild((CWndBase*&)m_pWndPost);
	RegisterCustomChild((CWndBase*&)m_pWndQueryEquip);
	RegisterCustomChild((CWndBase*&)m_pWndSelectVillage);
	RegisterCustomChild((CWndBase*&)m_pWndQuestItemWarning);
	RegisterCustomChild((CWndBase*&)m_pWndChangeName);
	RegisterCustomChild((CWndBase*&)m_pWndDropConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndUpgradeBase);
	RegisterCustomChild((CWndBase*&)m_pWndInvenRemoveItem);
	RegisterCustomChild((CWndBase*&)m_pWndConfirmBank);
	RegisterCustomChild((CWndBase*&)m_pWndBankPassword);
	RegisterCustomChild((CWndBase*&)m_pWndRankGuild);
	RegisterCustomChild((CWndBase*&)m_pWndRankInfo);
	RegisterCustomChild((CWndBase*&)m_pWndGuildBank);
	RegisterCustomChild((CWndBase*&)m_pWndGuildConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndGuildWarPeaceConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndGuildWarRequest);
	RegisterCustomChild((CWndBase*&)m_pWndGuildNickName);
	RegisterCustomChild((CWndBase*&)m_pWndGuildCombatRanking);
	RegisterCustomChild((CWndBase*&)m_pWndGuildWarState);
	RegisterCustomChild((CWndBase*&)m_pWndGuildCombatOffer);
	RegisterCustomChild((CWndBase*&)m_pWndGuildCombatSelection);
	RegisterCustomChild((CWndBase*&)m_pWndGuildCombatJoinSelection);
	RegisterCustomChild((CWndBase*&)m_pWndGuildCombatInfoMessageBox2);
	RegisterCustomChild((CWndBase*&)m_pWndGuildCombatInfoMessageBox);
	RegisterCustomChild((CWndBase*&)n_pWndGuildCombatResult);
	RegisterCustomChild((CWndBase*&)m_pWndCollecting);
	RegisterCustomChild((CWndBase*&)m_pWndLvReqDown);
	RegisterCustomChild((CWndBase*&)m_pWndBlessingCancel);
	RegisterCustomChild((CWndBase*&)m_pWndLordTender);
	RegisterCustomChild((CWndBase*&)m_pWndLordVote);
	RegisterCustomChild((CWndBase*&)m_pWndLordState);
	RegisterCustomChild((CWndBase*&)m_pWndLordEvent);
	RegisterCustomChild((CWndBase*&)m_pWndLordSkillConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndLordInfo);
	RegisterCustomChild((CWndBase*&)m_pWndLordRPInfo);
	RegisterCustomChild((CWndBase*&)m_pWndSecretRoomMsg);
	RegisterCustomChild((CWndBase*&)m_pWndSecretRoomSelection);
	RegisterCustomChild((CWndBase*&)m_pWndSecretRoomOffer);
	RegisterCustomChild((CWndBase*&)m_pWndSecretRoomOfferState);
	RegisterCustomChild((CWndBase*&)m_pWndSecretRoomChangeTaxRate);
	RegisterCustomChild((CWndBase*&)m_pWndSecretRoomCheckTaxRate);
	RegisterCustomChild((CWndBase*&)m_pWndSecretRoomBoard);
	RegisterCustomChild((CWndBase*&)m_pWndSecretRoomQuick);
	RegisterCustomChild((CWndBase*&)m_pWndPetAwakCancel);
	RegisterCustomChild((CWndBase*&)m_pWndSelectCh);
	RegisterCustomChild((CWndBase*&)m_pWndCoupleMessage);
	RegisterCustomChild((CWndBase*&)m_pWndReSkillWarning);
	RegisterCustomChild((CWndBase*&)m_pWndFontEdit);
	RegisterCustomChild((CWndBase*&)m_pWndPartyChangeName);
	RegisterCustomChild((CWndBase*&)m_pWndPartyConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndPartyLeaveConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndFriendConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndAddFriend);
	RegisterCustomChild((CWndBase*&)m_pWndStateConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndCloseExistingConnection);
	RegisterCustomChild((CWndBase*&)m_pWndRandomScrollConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndHelpInstant);
	RegisterCustomChild((CWndBase*&)m_pWndRoomList);
	RegisterCustomChild((CWndBase*&)m_pWndQuitRoom);
	RegisterCustomChild((CWndBase*&)m_pWndChangePetName);
	RegisterCustomChild((CWndBase*&)m_pWndSmeltSafety);
	RegisterCustomChild((CWndBase*&)m_pWndSmeltSafetyConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndRestateConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndQuestQuickInfo);
	RegisterCustomChild((CWndBase*&)m_pWndQuestDetail);
	RegisterCustomChild((CWndBase*&)m_pWndBuffPetStatus);
	RegisterCustomChild((CWndBase*&)m_pWndConfirmVis);
	RegisterCustomChild((CWndBase*&)m_pWndGHMain);
	RegisterCustomChild((CWndBase*&)m_pWndUpkeep);
	RegisterCustomChild((CWndBase*&)m_pWndSelectAwakeCase);
	RegisterCustomChild((CWndBase*&)m_pWndChattingBlockingList);
	RegisterCustomChild((CWndBase*&)m_pWndMessageBox);
	RegisterCustomChild((CWndBase*&)m_pWndMessageBoxUpper);
	RegisterCustomChild((CWndBase*&)m_pWndTaskBar);
	RegisterCustomChild((CWndBase*&)m_pWndMenu);
	RegisterCustomChild((CWndBase*&)m_pWndRevival);
	RegisterCustomChild((CWndBase*&)m_pWndResurrectionConfirm);
	RegisterCustomChild((CWndBase*&)m_pLogOutWaitting);
	RegisterCustomChild((CWndBase*&)m_pWndCommItemDlg);

#ifdef __FL_DUNGEON_TREASURE
	RegisterCustomChild((CWndBase*&)m_bDungeonTreasure);
#endif
#ifdef __FL_EQUALIZED_MODE
	RegisterCustomChild((CWndBase*&)m_WndSelectTmpClass);
#endif
#ifdef __FL_NEW_BOXES
	RegisterCustomChild((CWndBase*&)m_pWndNewBoxOpen);
#endif
	RegisterCustomChild((CWndBase*&)m_pWndGuildName);
	RegisterCustomChild((CWndBase*&)m_pWndLookChange);
	RegisterCustomChild((CWndBase*&)m_pWndSecretRoomCancelConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndSummonPartyUse);
	RegisterCustomChild((CWndBase*&)m_pWndItemTransy);
	RegisterCustomChild((CWndBase*&)m_pWndGuildWarJoinConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndGuildWarCancelConfirm);
	RegisterCustomChild((CWndBase*&)m_pWndChangeSex);
	RegisterCustomChild((CWndBase*&)m_pWndSummonParty);
	RegisterCustomChild((CWndBase*&)m_pWndIndirectTalk);
	RegisterCustomChild((CWndBase*&)m_pWndSummonPartyMsg);
	RegisterCustomChild((CWndBase*&)m_pWndSummonFriendMsg);
	RegisterCustomChild((CWndBase*&)m_pWndSummonFriend);
#ifdef __FL_PACKED_ITEMS
	RegisterCustomChild((CWndBase*&)m_pWndPackagePreview);
#endif
}

void CWndMgr::InitSetItemTextColor()
{
	dwItemColor.dwName0 = D3DCOLOR_XRGB(46, 112, 169);
	dwItemColor.dwName1 = D3DCOLOR_XRGB(0, 93, 0);
	dwItemColor.dwName2 = D3DCOLOR_XRGB(128, 64, 0);
	dwItemColor.dwName3 = D3DCOLOR_XRGB(127, 0, 255);
	dwItemColor.dwName4 = D3DCOLOR_XRGB(0, 0, 255);
	dwItemColor.dwName5 = D3DCOLOR_XRGB(0, 0, 255);
	dwItemColor.dwGeneral = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwPlusOption = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwPiercing = D3DCOLOR_XRGB(178, 0, 255);
	dwItemColor.dwResist = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwResistFire = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwResistWater = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwResistEarth = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwResistElectricity = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwResistWind = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwResistSM = D3DCOLOR_XRGB(0, 158, 0);
	dwItemColor.dwResistSM1 = D3DCOLOR_XRGB(255, 0, 0);
	dwItemColor.dwTime = D3DCOLOR_XRGB(0, 200, 0);
	dwItemColor.dwEffective0 = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwEffective1 = D3DCOLOR_XRGB(0, 255, 0);
	dwItemColor.dwEffective2 = D3DCOLOR_XRGB(255, 0, 0);
	dwItemColor.dwEffective3 = D3DCOLOR_XRGB(128, 128, 128);
	dwItemColor.dwRandomOption = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwEnchantOption = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwSetName = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwSetItem0 = D3DCOLOR_XRGB(178, 178, 178);
	dwItemColor.dwSetItem1 = D3DCOLOR_XRGB(0, 0, 204);
	dwItemColor.dwSetEffect = D3DCOLOR_XRGB(255, 157, 0);
	dwItemColor.dwGold = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwCommand = D3DCOLOR_XRGB(0, 0, 0);
	dwItemColor.dwNotUse = D3DCOLOR_XRGB(255, 66, 0);
	dwItemColor.dwAddedOpt1 = D3DCOLOR_XRGB(127, 255, 0);
	dwItemColor.dwAddedOpt2 = D3DCOLOR_XRGB(0, 255, 255);
	dwItemColor.dwAddedOpt3 = D3DCOLOR_XRGB(140, 23, 23);
	dwItemColor.dwAddedOpt4 = D3DCOLOR_XRGB(255, 127, 0);
	dwItemColor.dwAddedOpt5 = D3DCOLOR_XRGB(255, 110, 199);
	dwItemColor.dwAddedOpt6 = D3DCOLOR_XRGB(205, 127, 50);
	dwItemColor.dwAddedOpt7 = D3DCOLOR_XRGB(255, 0, 0);
	dwItemColor.dwAddedOpt8 = D3DCOLOR_XRGB(50, 50, 255);
	dwItemColor.dwAddedOpt9 = D3DCOLOR_XRGB(147, 112, 219);
	dwItemColor.dwAwakening = D3DCOLOR_XRGB(0, 0, 255);
	dwItemColor.dwBlessing = D3DCOLOR_XRGB(0, 0, 255);
	dwItemColor.dwBlessingWarning = D3DCOLOR_XRGB(255, 0, 0);
}


void CWndMgr::AlighWindow(CRect rcBndOld, CRect rcBndNew, CRect rcOld, CRect rcNew)
{
	for (int i = 0; i < m_wndOrder.GetSize(); i++)
	{
		CWndBase* pWndBase = (CWndBase*)m_wndOrder.GetAt(i);
		if (!pWndBase)
			continue;

		if (pWndBase == this || pWndBase == m_pWndTaskBar)
			continue;

		CRect rcWnd = pWndBase->GetWindowRect(TRUE);

		if (pWndBase->m_nWinSize == WSIZE_MAX)
		{
			if (rcWnd != rcNew)
				pWndBase->SetWndRect(rcNew);
		}
		else
		{
			CPoint point = rcWnd.TopLeft();
			CPoint pointOld = point;

			if (rcBndOld != rcBndNew)
			{
				if (rcBndOld.left > rcBndNew.left)
					point.x += (rcBndOld.left - rcBndNew.left) / 2;
				else if (rcBndOld.left < rcBndNew.left)
					point.x -= (rcBndNew.left - rcBndOld.left) / 2;

				if (rcBndOld.top > rcBndNew.top)
					point.y += (rcBndOld.top - rcBndNew.top) / 2;
				else if (rcBndOld.top < rcBndNew.top)
					point.y -= (rcBndNew.top - rcBndOld.top) / 2;

				if (rcBndOld.right > rcBndNew.right)
					point.x -= (rcBndOld.right - rcBndNew.right) / 2;
				else if (rcBndOld.right < rcBndNew.right)
					point.x += (rcBndNew.right - rcBndOld.right) / 2;

				if (rcBndOld.bottom > rcBndNew.bottom)
					point.y -= (rcBndOld.bottom - rcBndNew.bottom) / 2;
				else if (rcBndOld.bottom < rcBndNew.bottom)
					point.y += (rcBndNew.bottom - rcBndOld.bottom) / 2;
			}

			if (rcWnd.left < rcNew.left)
				point.x = rcNew.left;
			if (rcWnd.top < rcNew.top)
				point.y = rcNew.top;
			if (rcWnd.right > rcNew.right)
				point.x = rcNew.right - rcWnd.Width();
			if (rcWnd.bottom > rcNew.bottom)
				point.y = rcNew.bottom - rcWnd.Height();

			if (rcWnd.left == rcOld.left)
				point.x = rcNew.left;
			if (rcWnd.top == rcOld.top)
				point.y = rcNew.top;
			if (rcWnd.right == rcOld.right)
				point.x = rcNew.right - rcWnd.Width();
			if (rcWnd.bottom == rcOld.bottom)
				point.y = rcNew.bottom - rcWnd.Height();

			if (pointOld != point)
				pWndBase->Move(point);
		}
	}
}


void CWndMgr::Free()
{
	POSITION		pos(m_mapWndRegInfo.GetStartPosition());
	DWORD			dwWndId(NULL);
	LPWNDREGINFO	pWndRegInfo;

	while (pos)
	{
		m_mapWndRegInfo.GetNextAssoc(pos, dwWndId, (void*&)pWndRegInfo);
		SAFE_DELETE(pWndRegInfo->lpArchive);
		SAFE_DELETE(pWndRegInfo);
	}
	m_mapWndRegInfo.RemoveAll();


	CString			strTemp(_T(""));
	CWndBase* pWndBaseTemp(NULL);

	pos = m_mapMessage.GetStartPosition();
	while (pos)
	{
		m_mapMessage.GetNextAssoc(pos, strTemp, (void*&)pWndBaseTemp);
		SAFE_DELETE(pWndBaseTemp);
	}
	m_mapMessage.RemoveAll();


	pos = m_mapInstantMsg.GetStartPosition();
	while (pos)
	{
		m_mapInstantMsg.GetNextAssoc(pos, strTemp, (void*&)pWndBaseTemp);
		SAFE_DELETE(pWndBaseTemp);
	}
	m_mapInstantMsg.RemoveAll();

	DeleteAllCustomChild();

	if (IsWndRoot())
	{
		RemoveDestroyWnd();
		DestroyAllWnd(this);
	}

	m_pWndWorld = NULL;
}

void CWndMgr::DestroyApplet()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	POSITION pos = m_mapWndApplet.GetStartPosition();
	while (pos)
	{
		m_mapWndApplet.GetNextAssoc(pos, dwIdApplet, (void*&)pWndBase);

		if (pWndBase->IsPutRegInfo())
			PutRegInfo((CWndNeuz*)pWndBase, TRUE);

		SAFE_DELETE(pWndBase);
	}

	if (m_bTitle == FALSE)
		SaveRegInfo("regInfo.dat");

	m_mapWndApplet.RemoveAll();
}

BOOL CWndMgr::Initialize(CWndBase* pParentWnd)
{
	CWndBase::m_resMng.Load("ResData.inc");
	Create(WBS_NOFRAME | WBS_MANAGER | WBS_NODRAWFRAME, CRect(0, 0, CLIENT_WIDTH, CLIENT_HEIGHT/* - TASKBAR_HEIGHT*/), this, 100);
	return TRUE;
}
void CWndMgr::OnInitialUpdate()
{
	m_timerDobe.Set(SEC(2));
	AddAllApplet();

	m_dwSavePlayerTime = 0;

	char filename[MAX_PATH];
	for (int i = 0; i < 128; i++)
	{
		sprintf(filename, "Icon_CoolTime_%.3d.tga", i);
		g_pCoolTexArry[i] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, filename), 0xffff00ff);
	}

	for (int i = 0; i < 11; i++)
	{
		sprintf(filename, "Icon_ImgIncAni_%.2d.tga", i);
		g_pEnchantTexArry[i] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, filename), 0xffffffff);
	}
}

void CWndMgr::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void CWndMgr::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void CWndMgr::RegisterCustomChild(CWndBase*& pWndChild)
{
	vecWndIter iter = std::find(m_vecChildWnds.begin(), m_vecChildWnds.end(), &pWndChild);
	if (iter != m_vecChildWnds.end())
	{
		CWndBase** ppWnd = *iter;
		ppWnd = &pWndChild;
	}
	else
	{
		pWndChild = NULL;
		m_vecChildWnds.push_back(&pWndChild);
	}
}

void CWndMgr::DeleteAllCustomChild()
{
	int nCount = 0;

	for (vecWndIter iter = m_vecChildWnds.begin(); iter != m_vecChildWnds.end(); ++iter)
	{
		CWndBase** ppWnd = *iter;
		SAFE_DELETE((*ppWnd));
		++nCount;
	}
}

void CWndMgr::OnDestroyChildWnd(CWndBase* pWndChild)
{
	BOOL bFocus = TRUE;
	if (pWndChild && pWndChild->IsWndStyle(WBS_NOFOCUS))
		bFocus = FALSE;

	for (vecWndIter iter = m_vecChildWnds.begin(); iter != m_vecChildWnds.end(); ++iter)
	{
		CWndBase** ppWnd = *iter;
		if (pWndChild == *ppWnd)
		{
			SAFE_DELETE(*ppWnd);
			pWndChild = NULL;
			break;
		}
	}

	if (pWndChild)
	{
		CWndNeuz* pWndBase;
		if (m_mapWndApplet.Lookup(pWndChild->GetWndId(), (void*&)pWndBase))
		{
			m_mapWndApplet.RemoveKey(pWndChild->GetWndId());
			if (pWndBase->IsPutRegInfo())
				PutRegInfo(pWndBase, FALSE);
			SAFE_DELETE(pWndBase);
			pWndChild = NULL;
		}
		else if (pWndChild->GetWndId() == APP_MESSAGE)
		{
			CWndMessage* pWndMessage = (CWndMessage*)pWndChild;
			pWndMessage = GetMessagge(pWndMessage->m_strPlayer);
			if (pWndMessage)
			{
				m_mapMessage.RemoveKey(pWndMessage->m_strPlayer);
				SAFE_DELETE(pWndChild);
			}
		}
		else if (pWndChild->GetWndId() == APP_INSTANTMSG)
		{
			CWndInstantMsg* pWndMessage = (CWndInstantMsg*)pWndChild;
			pWndMessage = GetInstantMsg(pWndMessage->m_strPlayer);
			if (pWndMessage)
			{
				m_mapInstantMsg.RemoveKey(pWndMessage->m_strPlayer);
				SAFE_DELETE(pWndChild);
			}
		}
	}

	if (pWndChild)
		Error("CWndMgr::OnDestroyChildWnd: %d not deleted", pWndChild->GetWndId());

	if (m_bTitle == FALSE && bFocus)
		SetFocus();
}

void CWndMgr::OpenTitle(BOOL bFirstTime)
{
	if (m_bTitle == TRUE)
		return;

#ifdef __DISCORD // Set default details (login page)
	g_Neuz.ManageDiscord(DIS_DEFAULT_DETAILS);
#endif

	if (bFirstTime == FALSE)
	{
		g_Option.Save("neuz.ini");
		DestroyApplet();
		Free();
		g_WorldMng.DestroyCurrentWorld();
		g_pPlayer = NULL;
		CMover::SetActiveObj(NULL);
		g_dpCertified.DeleteDPObject();
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();

		if (g_pBipedMesh)
		{
			g_pBipedMesh->DeleteDeviceObjects();
			SAFE_DELETE(g_pBipedMesh);
		}
		g_Party.InitParty();
		g_GuildMng.Clear();
		g_GuildCombatMng.GuildCombatClear(99);
		CPost::GetInstance()->Clear();
#if __VER >= 13 // __COUPLE_1117
		CCoupleHelper::Instance()->Clear();
#endif	// __COUPLE_1117
#if __VER >= 11 // __SYS_PLAYER_DATA
		CPlayerDataCenter::GetInstance()->Clear();
#endif // __SYS_PLAYER_DATA
#if __VER >= 15 // __GUILD_HOUSE
		GuildHouse->Clear();
#endif // __GUILD_HOUSE
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if (m_pWndQuestQuickInfo)
			SAFE_DELETE(m_pWndQuestQuickInfo);
		g_QuestTreeInfoManager.DeleteAllTreeInformation();
#endif // __IMPROVE_QUEST_INTERFACE
#ifdef __YS_CHATTING_BLOCKING_SYSTEM
		prj.m_setBlockedUserID.clear();
#endif // __YS_CHATTING_BLOCKING_SYSTEM
	}

	SetWndRect(CRect(0, 0, CLIENT_WIDTH, CLIENT_HEIGHT));
	m_bTitle = TRUE;

	ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);

	CWndBase::m_nAlpha = 255;
	m_bTitle = TRUE;
}
void CWndMgr::OpenField()
{
	m_bAutoRun = FALSE;
#if __VER >= 11 // __MOD_VENDOR
	memset(g_Neuz.m_aSavedInven, 0, sizeof(g_Neuz.m_aSavedInven));
#endif
	memset(g_bSlotSwitchAboutEquipItem, FALSE, sizeof(g_bSlotSwitchAboutEquipItem));
	if (m_bTitle == FALSE)
		return;
	StopMusic();
	for (int i = 0; i < MAX_CHARACTER_LIST; i++)
		SAFE_DELETE(g_Neuz.m_apPlayer[i]);

	g_DialogMsg.ClearAllMessage();
	SetWndRect(g_Neuz.GetDeviceRect());
	OpenTaskBar();

#ifdef __FL_PET_FILTER
	g_DPlay.SendPetLootOption(g_Option.m_nLootType, g_Option.m_nLootPlace);
#endif

	m_aChatString.RemoveAll();
	m_aChatColor.RemoveAll();
	m_aChatStyle.RemoveAll();

	LoadRegInfo("regInfo.dat");
	ObjectExecutor(SHORTCUT_APPLET, APP_COMMUNICATION_CHAT);
	ObjectExecutor(SHORTCUT_APPLET, APP_NAVIGATOR);
	ObjectExecutor(SHORTCUT_APPLET, APP_STATUS1);
	ObjectExecutor(SHORTCUT_APPLET, APP_WORLD);


	if (g_Option.m_bNotice)
		ObjectExecutor(SHORTCUT_APPLET, APP_INFO_NOTICE);

	CWndBase::m_nAlpha = g_Option.m_nWindowAlpha;

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	SAFE_DELETE(m_pWndQuestQuickInfo);
	m_pWndQuestQuickInfo = new CWndQuestQuickInfo;
	m_pWndQuestQuickInfo->Initialize();
	m_pWndQuestQuickInfo->SetVisible(FALSE);
#endif // __IMPROVE_QUEST_INTERFACE
	m_bTitle = FALSE;
}
BOOL CWndMgr::OnCommand(UINT nID, DWORD dwMessage)
{
	return TRUE;
}

void CWndMgr::CloseMessageBox()
{
	//m_pWndMessageBox->Destroy();
	SAFE_DELETE(m_pWndMessageBox);
	SAFE_DELETE(m_pWndMessageBoxUpper);
}
BOOL CWndMgr::OpenCustomBox(LPCTSTR lpszMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent)
{
	SAFE_DELETE(m_pWndMessageBox);
	m_pWndMessageBox = pWndMessageBox;
	pWndParent = this;

	if (pWndMessageBox->Initialize(pWndParent) == FALSE)
	{
		SAFE_DELETE(m_pWndMessageBox);
		return TRUE;
	}
	return FALSE;
}
BOOL CWndMgr::OpenMessageBox(LPCTSTR lpszMessage, UINT nType, CWndBase* pWndParent)
{
	SAFE_DELETE(m_pWndMessageBox);
	m_pWndMessageBox = new CWndMessageBox;
	pWndParent = this;
	if (m_pWndMessageBox->Initialize(lpszMessage, pWndParent, nType) == FALSE)
	{
		SAFE_DELETE(m_pWndMessageBox);
		return TRUE;
	}
	return FALSE;
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
BOOL CWndMgr::OpenMessageBoxWithTitle(LPCTSTR lpszMessage, const CString& strTitle, UINT nType, CWndBase* pWndParent)
{
	SAFE_DELETE(m_pWndMessageBox);
	m_pWndMessageBox = new CWndMessageBox;
	pWndParent = this;
	if (m_pWndMessageBox->Initialize(lpszMessage, pWndParent, nType) == FALSE)
	{
		SAFE_DELETE(m_pWndMessageBox);
		return TRUE;
	}
	m_pWndMessageBox->SetTitle(strTitle);
	return FALSE;
}
#endif // __IMPROVE_QUEST_INTERFACE

BOOL CWndMgr::OpenMessageBoxUpper(LPCTSTR lpszMessage, UINT nType, BOOL bPostLogoutMsg)
{
	CWndBase* pWndParent = NULL;
	SAFE_DELETE(m_pWndMessageBoxUpper);
	m_pWndMessageBoxUpper = new CWndMessageBoxUpper;
	pWndParent = this;
	if (m_pWndMessageBoxUpper->Initialize(lpszMessage, pWndParent, nType, bPostLogoutMsg) == FALSE)
	{
		SAFE_DELETE(m_pWndMessageBoxUpper);
		return TRUE;
	}
	return FALSE;
}
BOOL CWndMgr::OpenTaskBar()
{
	CWndTaskBar* pTaskbarOld = m_pWndTaskBar;
	m_pWndTaskBar = new CWndTaskBar;
	if (m_pWndTaskBar->Initialize(this) == FALSE)
	{
		SAFE_DELETE(m_pWndTaskBar);
		SAFE_DELETE(pTaskbarOld);
		return TRUE;
	}

	m_pWndTaskBar->Copy(pTaskbarOld);
	SAFE_DELETE(pTaskbarOld);
	return TRUE;
}
BOOL CWndMgr::OpenMenu()
{
	if (m_pWndMenu)
	{
		if (m_pWndMenu->IsVisible())
			m_pWndMenu->SetVisible(FALSE);
		else
		{
			m_pWndMenu->SetVisible(TRUE);
			m_pWndMenu->SetFocus();
		}
		return FALSE;
	}

	m_pWndMenu = new CWndTaskMenu;
	if (m_pWndMenu->Initialize(this) == FALSE)
	{
		SAFE_DELETE(m_pWndMenu);
		return TRUE;
	}
	return TRUE;
}
void CWndMgr::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
}

HRESULT CWndMgr::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	return S_OK;
}

HRESULT CWndMgr::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return S_OK;
}

HRESULT CWndMgr::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	return 0;
}
AppletFunc* CWndMgr::GetAppletFunc(DWORD dwIdApplet)
{
	AppletFunc* pApplet;
	if (m_mapAppletFunc.Lookup(dwIdApplet, (void*&)pApplet) == TRUE)
		return pApplet;
	return NULL;
}
DWORD CWndMgr::GetAppletId(TCHAR* lpszAppletName)
{
	AppletFunc* pApplet;
	DWORD dwIdApplet;
	POSITION pos = m_mapAppletFunc.GetStartPosition();
	while (pos)
	{
		m_mapAppletFunc.GetNextAssoc(pos, dwIdApplet, (void*&)pApplet);
		if (!_tcscmp(pApplet->m_pAppletName, lpszAppletName))
			return pApplet->m_dwIdApplet;
	}
	return 0xffffffff;
}
CWndBase* CWndMgr::GetApplet(DWORD dwIdApplet)
{
	CWndBase* pWndBase;
	if (m_mapWndApplet.Lookup(dwIdApplet, (void*&)pWndBase) == FALSE)
		return NULL;
	return pWndBase;
}

void CWndMgr::AddAppletFunc(LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszAppletName, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc, CHAR cHotkey)
{
	AppletFunc* pApplet = new AppletFunc;
	pApplet->m_pFunc = (CWndBase * (*)()) pAppletFunc;
	pApplet->m_dwIdApplet = dwIdApplet;
	pApplet->m_pAppletName = lpszAppletName;
	pApplet->m_pAppletDesc = lpszAppletDesc;
	pApplet->m_pszIconName = pszIconName;
	pApplet->m_cHotkey = cHotkey;
	m_mapAppletFunc.SetAt(dwIdApplet, pApplet);
}
CWndBase* CWndMgr::CreateApplet(DWORD dwIdApplet)
{
	CWndNeuz* pWndBase = NULL;
	AppletFunc* pAppletFunc = NULL;
	if (m_mapAppletFunc.Lookup(dwIdApplet, (void*&)pAppletFunc) == FALSE)
	{

		pAppletFunc = GetAppletFunc(dwIdApplet);
	}
	if (pAppletFunc)
	{
		CWndWorld* pWndWorld = (CWndWorld*)GetWndBase(APP_WORLD);
		pWndBase = (CWndNeuz*)GetWndBase(pAppletFunc->m_dwIdApplet);
		if (pWndBase == NULL)
		{
			pWndBase = (CWndNeuz*)(*pAppletFunc->m_pFunc)();
			if (pWndBase)
			{
				if (pWndBase->Initialize(this, dwIdApplet) == FALSE)
				{
					SAFE_DELETE(pWndBase);
					return pWndBase;
				}
				LPWNDREGINFO pWndRegInfo;
				if (m_mapWndRegInfo.Lookup(dwIdApplet, (void*&)pWndRegInfo) == TRUE)
				{
					if (pWndRegInfo->dwSize)
					{
						// load
						CAr ar(pWndRegInfo->lpArchive, pWndRegInfo->dwSize);
						((CWndNeuz*)pWndBase)->SerializeRegInfo(ar, pWndRegInfo->dwVersion);
					}
					if (pWndBase->IsWndStyle(WBS_THICKFRAME))
					{
						if (pWndRegInfo->dwWndSize == WSIZE_WINDOW)
						{
							((CWndNeuz*)pWndBase)->SetSizeWnd();
							pWndBase->SetWndRect(pWndRegInfo->rect);
						}
						if (pWndRegInfo->dwWndSize == WSIZE_MAX)
							((CWndNeuz*)pWndBase)->SetSizeMax();
					}
					else
					{
						CRect wndRect = pWndBase->GetWindowRect();
						pWndBase->SetWndRect(
							CRect(
								pWndRegInfo->rect.left,
								pWndRegInfo->rect.top,
								pWndRegInfo->rect.left + wndRect.Width(),
								pWndRegInfo->rect.top + wndRect.Height()
							));
					}
					//pWndBase->SetWndRect( pWndRegInfo->rect );
				}
				//LPWNDAPPLET lpApplet = m_resMng.GetAt( dwIdApplet );
				//if( lpApplet )
				//	pWndBase->SetToolTip( m_resMng.GetAt( dwIdApplet )->strToolTip );
				//pWndBase->SetToolTip( pAppletFunc->m_pAppletDesc );
				m_mapWndApplet.SetAt(dwIdApplet, pWndBase);
			}
		}
		else
			pWndBase->SetFocus();
	}
	return pWndBase;
}
void CWndMgr::ObjectExecutor(DWORD dwShortcut, DWORD dwId, DWORD dwType)
{
	SHORTCUT shortcut;
	ZeroMemory(&shortcut, sizeof(shortcut));
	shortcut.m_dwShortcut = dwShortcut;
	shortcut.m_dwId = dwId;
	shortcut.m_dwType = dwType;
	ObjectExecutor(&shortcut);
}
void CWndMgr::ObjectExecutor(LPSHORTCUT pShortcut)
{
	if (pShortcut->m_dwShortcut == SHORTCUT_APPLET)
	{
		CWndBase* pWndBase = (CWndBase*)GetWndBase(APP_DIALOG_EX);
		if (pWndBase)
			return;

		if (pShortcut->m_dwId == APP_SKILL4)
		{
			CWndBase* pWndBase = (CWndBase*)GetWndBase(APP_QUEITMWARNING);
			if (pWndBase)
				return;

			CWndSkill_16* pWndBase1 = (CWndSkill_16*)GetWndBase(APP_SKILL4);
			if (pWndBase1 && ((CWndSkill_16*)pWndBase1)->GetCurrSkillPoint() != g_pPlayer->m_nSkillPoint)
			{
				if (pWndBase == NULL)
				{
					SAFE_DELETE(m_pWndReSkillWarning);
					m_pWndReSkillWarning = new CWndReSkillWarning;
					m_pWndReSkillWarning->Initialize(NULL);
					m_pWndReSkillWarning->InitItem(TRUE);
				}
				return;
			}
		}

		if (pShortcut->m_dwId == APP_QUIT)
		{
			OpenCustomBox(NULL, new CWndQuit);
		}
		else
			if (pShortcut->m_dwId == APP_LOGOUT)
			{
				if (g_pPlayer->IsAttackMode())
				{
					PutString(prj.GetText(TID_PK_COMBAT_END), NULL, 0xffff0000);//, CHATSTY_SYSTEM_CHAT );
					return;
				}
				else
				{
					OpenCustomBox(NULL, new CWndLogOut);
				}
			}
#if __VER >= 8 //__CSC_VER8_2
			else if (pShortcut->m_dwId == APP_PARTY)
			{
				CWndBase* pWndParty = (CWndBase*)GetApplet(APP_PARTY);
				CWndBase* pWndPartyQuick = (CWndBase*)GetApplet(APP_PARTY_QUICK);
				if (pWndParty == NULL && pWndPartyQuick == NULL)
				{
					CreateApplet(APP_PARTY);
					if (g_Party.GetSizeofMember() >= 2)
					{
						CreateApplet(APP_PARTY_QUICK);
					}
				}
				else if (pWndParty != NULL && pWndPartyQuick != NULL)
				{
					if (pWndParty && pWndPartyQuick)
					{
						pWndParty->Destroy();
						pWndPartyQuick->Destroy();
						SetFocus();
					}
				}
				else
				{
					if (pWndParty != NULL)
					{
						pWndParty->Destroy();
						SetFocus();
					}
					else if (pWndPartyQuick != NULL)
					{
						pWndPartyQuick->Destroy();
						SetFocus();
					}
				}
			}
#endif //__CSC_VER8_2
			else
			{
				CWndBase* pWndBase = (CWndBase*)GetApplet(pShortcut->m_dwId);
				if (pWndBase)
				{
					pWndBase->Destroy();
					SetFocus();
				}
#if __VER >= 8 //__CSC_VER8_5
				else if (pShortcut->m_dwId == APP_VENDOR_REVISION)
				{
					CWndBase* pWndBaseBuf = GetWndBase(APP_SUMMON_ANGEL);
					if (pWndBaseBuf)
					{
						PutString(prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING));
					}
					else
						CreateApplet(pShortcut->m_dwId);
				}
#endif //__CSC_VER8_5
				else if (CreateApplet(pShortcut->m_dwId) == FALSE)
				{
				}
			}
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_CHAT)
	{
		CWndWorld* pWndWorld = (CWndWorld*)GetWndBase(APP_WORLD);
		ParsingChat(pShortcut->m_szString);
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_EMOTICON)
	{
		TCHAR szCmd[1024] = { 0 };
		_tcscat(szCmd, "/");
		_tcscat(szCmd, g_DialogMsg.m_EmiticonCmd[pShortcut->m_dwId].m_szCommand);
		ParsingChat(szCmd);
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_ITEM)
	{
		CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(pShortcut->m_nLocation);
		if (!pContainer)
			return;

		CItemElem* pItemBase = pContainer->GetAtId(pShortcut->m_dwId);
		if (!pItemBase)
			return;

		UseItem(pItemBase, pShortcut->m_nLocation);
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_SKILL)
	{
		if (CMover::GetActiveMover()->m_vtInfo.VendorIsVendor())
			return;

		if (pShortcut->m_dwType == 2)
		{
			if (g_Party.m_nSizeofMember >= 2 && g_Party.m_nKindTroup == 1)
			{
				ItemProp* pItemProp = prj.GetPartySkill(pShortcut->m_dwId);
				if (pItemProp)
				{
					if (g_Party.IsLeader(g_pPlayer->m_idPlayer))
					{
						if (int(g_Party.GetLevel() - pItemProp->dwReqDisLV) >= 0)
						{
							if ((g_Party.GetPoint() - pItemProp->dwExp) >= 0)
							{
								ItemProp* pItemProp = prj.GetPartySkill(pShortcut->m_dwId);
								DWORD	dwSkillTimeTmp = 0;
								CWndWorld* pWndWorld = (CWndWorld*)m_pWndWorld;
								for (auto& i : pWndWorld->m_buffs.m_mapBuffs)
								{
									IBuff* ptr = i.second;
									if (ptr->GetId() == pShortcut->m_dwId)
										dwSkillTimeTmp = ptr->GetTotal();
								}
								if (dwSkillTimeTmp == 0
									|| g_pPlayer->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01)
									|| g_pPlayer->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02)
									|| g_pPlayer->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01_01))
								{
									g_DPlay.SendPartySkillUse(pShortcut->m_dwId);
								}
								else
								{
									if (pShortcut->m_dwId != ST_CALL &&
										pShortcut->m_dwId != ST_BLITZ &&
										pShortcut->m_dwId != ST_RETREAT)
									{
										CString str;
										str.Format(prj.GetText(TID_GAME_NOT_TIME), pItemProp->szName);
										PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_NOT_TIME));
									}
								}
							}
							else
								PutString(_T(prj.GetText(TID_DIAG_0049)));
						}
						else
							PutString(_T(prj.GetText(TID_DIAG_0019)));
					}
					else
						PutString(_T(prj.GetText(TID_DIAG_0047)));
				}
			}
		}
		else
		{
			UseSkillShortCut(pShortcut->m_dwType, pShortcut->m_dwId);
		}
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_LORDSKILL)
	{

		CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
		DWORD dwObjId = NULL_ID;
		CCLord* pLord = CCLord::Instance();
		CLordSkillComponentExecutable* pComponent = pLord->GetSkills()->GetSkill(pShortcut->m_dwId);

		switch (pShortcut->m_dwId)
		{
		case LI_SUMMON:
		case LI_TELEPORT:
			if (pComponent->GetTick() == 0)
			{
				SAFE_DELETE(m_pWndLordSkillConfirm);
				m_pWndLordSkillConfirm = new CWndLordSkillConfirm(pShortcut->m_dwId);
				m_pWndLordSkillConfirm->Initialize();
			}
			else
			{
				PutString(prj.GetText(TID_GAME_STILLNOTUSE), NULL, prj.GetTextColor(TID_GAME_STILLNOTUSE));
			}
			break;

		default:
			if (!pCtrl)
			{
				g_DPlay.SendLordSkillUse(pShortcut->m_dwId, g_pPlayer->GetName());
			}
			else if (pCtrl && pCtrl->GetType() == OT_MOVER)
			{
				if (((CMover*)pCtrl)->IsPlayer())
					g_DPlay.SendLordSkillUse(pShortcut->m_dwId, ((CMover*)pCtrl)->GetName());
			}
			break;
		};

	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_MOTION)
	{
		{
			CWndBase* pWndBase = GetWndBase(APP_VENDOR_REVISION);
			if (pWndBase)
				return;
		}
		MotionProp* pMotionProp = prj.GetMotionProp(pShortcut->m_dwId);
		if (pMotionProp->szName[0])
		{
			strcpy(m_szTimerChat, pMotionProp->szName);
			m_timerChat.Set(1000);
		}
		bool fBehavior = false;
		if (pShortcut->m_dwId == MOT_BASE_PICKUP)
			g_pPlayer->DoPickupItemAround();
		else
			if (pShortcut->m_dwId == MOT_BASE_BATTLE)
			{
				if (g_pPlayer->m_pActMover->IsStateFlag(OBJSTAF_COMBAT)) {
					if (g_pPlayer->SendActMsg(OBJMSG_MODE_PEACE) == 1) {
						fBehavior = true;
					}
				}
				else {
					if (g_pPlayer->SendActMsg(OBJMSG_MODE_COMBAT) == 1) {
						fBehavior = true;
					}
				}
			}
			else if (pShortcut->m_dwId == MOT_BASE_COLLECT)
			{
			}
			else if (pShortcut->m_dwId == MOT_BASE_ESCAPE)
			{
				g_DPlay.SendDoEscape();
			}
			else if (pShortcut->m_dwId == MOT_BASE_CHEER)
			{
				CObj* pFocus = g_pPlayer->GetWorld()->GetObjFocus();

				if (pFocus && pFocus->GetType() == OT_MOVER && ((CMover*)pFocus)->IsPlayer())
				{
					g_DPlay.SendCheering(((CMover*)pFocus)->GetId());
					fBehavior = false;
				}
				else
				{
					PutString(prj.GetText(TID_CHEER_NO2), NULL, prj.GetTextColor(TID_CHEER_NO2));
				}
			}
			else if (pShortcut->m_dwId == MOT_BASE_AUTOATTACK)
			{
				CObj* pFocus = g_pPlayer->GetWorld()->GetObjFocus();

				if (g_pPlayer->IsAttackAble(pFocus) != NULL)
				{
					if (pFocus && pFocus->GetType() == OT_MOVER)
					{
						ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp();
						if (pItemProp && pItemProp->dwItemKind3 == IK3_WAND)
						{
							CCtrl* pFocusObj = (CCtrl*)pFocus;
							g_pPlayer->CMD_SetMagicAttack(pFocusObj->GetId(), 0);
						}
						else
							((CWndWorld*)m_pWndWorld)->m_bAutoAttack = TRUE;
					}
					else
					{
						PutString(prj.GetText(TID_GAME_NOTTARGET), NULL, prj.GetTextColor(TID_GAME_NOTTARGET));
					}
				}
			}
			else if (pShortcut->m_dwId == MOT_BASE_COUPLE)
			{
				CObj* pFocus = g_pPlayer->GetWorld()->GetObjFocus();
				if (pFocus && pFocus->GetType() == OT_MOVER && ((CMover*)pFocus)->IsPlayer())
				{
					SAFE_DELETE(m_pWndCoupleMessage);
					m_pWndCoupleMessage = new CWndCoupleMessage;
					CString strText;
					strText.Format(prj.GetText(TID_GAME_PROPOSETO), ((CMover*)pFocus)->GetName());
					m_pWndCoupleMessage->SetMessageMod(strText, CWndCoupleMessage::CM_SENDPROPOSE, pFocus);
					m_pWndCoupleMessage->Initialize(NULL);
				}
				else
				{
					PutString(prj.GetText(TID_GAME_NOTCOUPLETARGET), NULL, prj.GetTextColor(TID_GAME_NOTCOUPLETARGET));
				}
			}
			else
			{

				MotionProp* pProp = prj.GetMotionProp(pShortcut->m_dwId);
				if (pProp)
				{
					int nMotion = pProp->dwMotion;
					int nPlayMode = 0;
					switch (pProp->dwPlay)
					{
					case 0:	nPlayMode = ANILOOP_1PLAY;	break;
					case 1:	nPlayMode = ANILOOP_CONT;	break;
					case 2:	nPlayMode = ANILOOP_LOOP;	break;
					default: ANILOOP_1PLAY;
					}

					if (g_pPlayer->SendActMsg(OBJMSG_MOTION, nMotion, nPlayMode) == 1)
					{
						switch (nMotion)
						{
						case MTI_POINTWAYNORTH:
							g_pPlayer->SetAngle(180.0f);
							break;
						case MTI_POINTWAYSOUTH:
							g_pPlayer->SetAngle(0.0f);
							break;
						case MTI_POINTWAYEAST:
							g_pPlayer->SetAngle(90.0f);
							break;
						case MTI_POINTWAYWEST:
							g_pPlayer->SetAngle(270.0f);
							break;
						}
						fBehavior = true;
					}
				}
			}

		if (fBehavior)
		{
			g_pPlayer->ClearDest();
			g_WorldMng()->SetObjFocus(0);
			g_DPlay.SendPlayerBehavior();
		}
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_OBJECT)
	{
		CMover* CMover = prj.GetMover(pShortcut->m_dwId);
		if (CMover)
			OpenMessage(pShortcut->m_szString);
	}
	else if (pShortcut->m_dwShortcut == SHORTCUT_SKILLFUN)
		m_pWndWorld->UseSkill();
#ifdef __FL_SWITCH_EQ
	else if (pShortcut->m_dwShortcut == SHORTCUT_EQ_SWITCH)
		g_DPlay.SendSwitchUse(pShortcut->m_dwId);
#endif

}
BOOL CWndMgr::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}
BOOL CWndMgr::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{

	}
	return TRUE;
}

BOOL CWndMgr::OnEraseBkgnd(C2DRender* p2DRender)
{
	p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	CWndWorld* pWndWorld = (CWndWorld*)GetApplet(APP_WORLD);

	if (pWndWorld == NULL && m_bTitle) //|| pWndWorld->m_nWinSize != WSIZE_MAX )
	{
		m_pTheme->RenderDesktop(&g_Neuz.m_2DRender);
	}
	else if (pWndWorld)
	{
		m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0);
	}
	else
		m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0);
	p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	p2DRender->m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return TRUE;
}
BOOL CWndMgr::Process()
{
	AlighWindow(g_Neuz.m_rcWindowBounds, g_Neuz.m_rcWindowBounds, g_Neuz.m_rcWindowClient, g_Neuz.m_rcWindowClient);

#ifdef __MAIL_REQUESTING_BOX
	static DWORD dwTargetTime = g_tmCurrent + 5000;
	if (m_bWaitRequestMail && dwTargetTime < g_tmCurrent)
	{
		g_DPlay.SendQueryMailBox();
		dwTargetTime = g_tmCurrent + 5000;
	}
#endif //__MAIL_REQUESTING_BOX

	CWndRegVend* pWndRegVend = (CWndRegVend*)GetWndBase(APP_VENDOREX_SELL);
	CWndRegVend* pWndVendorBuy = (CWndRegVend*)GetWndBase(APP_VENDOREX_BUY);

	if (pWndRegVend || pWndVendorBuy)
	{
		return CWndBase::Process();
	}

	if (g_pPlayer)
	{
		if (g_bKeyTable[VK_ESCAPE] && m_pLogOutWaitting == NULL)
		{
			g_bKeyTable[VK_ESCAPE] = 0;

			if (m_pWndMenu == NULL || m_pWndMenu->IsVisible() == FALSE)
			{
				CWorld* pWorld = g_WorldMng();
				CObj* pObj = pWorld->GetObjFocus();
				CWndWorld* pWndWorld = m_pWndWorld;
				if (pWndWorld && pWndWorld->m_pNextTargetObj)
				{
					pWndWorld->m_pNextTargetObj = NULL;
					CMover* pMover = (CMover*)pObj;
					if (pMover->IsPeaceful() == FALSE)
						pWndWorld->m_pRenderTargetObj = pObj;
				}
				else if (pObj)
				{
					pWorld->SetObjFocus(NULL);

					if (pWndWorld)
						pWndWorld->m_pRenderTargetObj = NULL;

					pWorld->m_idObjFocusOld = NULL_ID;

					g_pPlayer->ClearDest();

					if (pWndWorld)
						pWndWorld->m_bAutoAttack = FALSE;

					if (g_pPlayer->m_dwFlag & MVRF_TRACKING)
					{
						g_pPlayer->m_dwFlag &= (~MVRF_TRACKING);
						g_pPlayer->m_idTracking = NULL_ID;
					}
				}
				else if (pWndWorld->m_bSetQuestNPCDest)
					pWndWorld->m_bSetQuestNPCDest = FALSE;
				else
				{
					if (g_Neuz.m_pCreateMonItem)
						g_Neuz.m_pCreateMonItem = NULL;
					else
					{
						if (CWndBase::m_GlobalShortcut.IsEmpty())
						{
							CWndBase* pWndBase = GetWndBase();
							if (pWndBase)
							{
								LPWNDAPPLET lpWndApplet = pWndBase->GetWndApplet();
								if (pWndBase == &m_pWndWorld->m_wndMenuMover || pWndBase->GetWndId() == 10)
									m_pWndWorld->SetFocus();
								else if (lpWndApplet != NULL)
								{
									if (lpWndApplet->dwWndId != APP_MINIGAME_KAWIBAWIBO && lpWndApplet->dwWndId != APP_MINIGAME_KAWIBAWIBO_WIN
										&& lpWndApplet->dwWndId != APP_MINIGAME_DICE && lpWndApplet->dwWndId != APP_COLLECTING
										&& lpWndApplet->dwWndId != APP_RR_MINIGAME_KAWIBAWIBO && lpWndApplet->dwWndId != APP_RR_MINIGAME_DICE
										&& lpWndApplet->dwWndId != APP_RR_MINIGAME_ARITHMETIC && lpWndApplet->dwWndId != APP_RR_MINIGAME_STOPWATCH
										&& lpWndApplet->dwWndId != APP_RR_MINIGAME_TYPING && lpWndApplet->dwWndId != APP_RR_MINIGAME_CARD
										&& lpWndApplet->dwWndId != APP_RR_MINIGAME_LADDER)

										if (lpWndApplet->dwWndId != APP_GH_MAIN
											&& lpWndApplet->dwWndId != APP_CHAT_LOG
#ifdef __FL_DAILY_REWARDS
											&& lpWndApplet->dwWndId != APP_DAILY_REWARD
#endif
											&& lpWndApplet->dwWndId != APP_GUILDCOMBAT_SELSTART
#ifdef __FL_WHEEL_OF_FORTUNE
											&& lpWndApplet->dwWndId != APP_WOF
#endif

											)
											pWndBase->Destroy();
								}
							}
							else
							{
								OpenMenu();
								CRect rect = g_Neuz.GetDeviceRect();
#ifndef __FL_UPDATE_TASKBAR
								if (m_pWndMenu)
									m_pWndMenu->Move(CPoint(0, rect.bottom - 48 - m_pWndMenu->GetWndRect().Height()));
#else
								if (m_pWndMenu)
								{
									CWndTaskBar* pTaskBar = m_pWndTaskBar;
									if (pTaskBar)
									{
										int x, y;
										x = pTaskBar->GetWindowRect(TRUE).left;
										y = rect.bottom - 48 - m_pWndMenu->GetWndRect().Height();
										m_pWndMenu->Move(CPoint(x, y));
									}
									else
										m_pWndMenu->Move(CPoint(0, rect.bottom - 48 - m_pWndMenu->GetWndRect().Height()));
								}
#endif
							}
						}
					}
				}
			}
			if (m_wndRemove.GetSize() > 0)
			{
				CWndBase* pWndBase = (CWndBase*)m_wndRemove.GetAt(0);

				if (pWndBase && pWndBase->GetWndApplet()->dwWndId == APP_SECRETROOM_GUILDMEMBER)
					pWndBase->Destroy();
			}
		}
		if (m_szTimerChat[0] && m_timerChat.IsTimeOut())
		{
			g_DPlay.SendChat(m_szTimerChat);
			m_szTimerChat[0] = 0;
			m_timerChat.Set(0);
		}

		if (g_pPlayer && CSecretRoomMng::GetInstance()->m_nState >= SRCONT_WAR)
		{
			if (CSecretRoomMng::GetInstance()->IsInTheSecretRoom(g_pPlayer))
			{
				if (m_pWndSecretRoomQuick == NULL)
				{
					//Create GuildMemberMng Window
					int nGuildIndex = CSecretRoomMng::GetInstance()->m_pSRCont->GetTenderGuild(g_pPlayer->m_idGuild);
					if (nGuildIndex != NULL_ID)
					{
						int nGuildMemberCount = CSecretRoomMng::GetInstance()->m_pSRCont->m_vecSecretRoomTender[nGuildIndex].vecLineUpMember.size();
						SAFE_DELETE(m_pWndSecretRoomQuick);
						m_pWndSecretRoomQuick = new CWndSecretRoomQuick;
						m_pWndSecretRoomQuick->SetGuildMemCount(nGuildMemberCount);

						for (int k = 0; k < nGuildMemberCount; k++)
							m_pWndSecretRoomQuick->SetGuildMember(CSecretRoomMng::GetInstance()->m_pSRCont->m_vecSecretRoomTender[nGuildIndex].vecLineUpMember[k]);

						m_pWndSecretRoomQuick->Initialize(NULL);
					}
				}
			}
		}

		if (!CSecretRoomMng::GetInstance()->IsInTheSecretRoom(g_pPlayer))
		{
			if (m_pWndSecretRoomQuick)
				SAFE_DELETE(m_pWndSecretRoomQuick);
		}
	}


	if (m_pLogOutWaitting)
		return CWndBase::Process();


	if (m_pWndTaskBar)
	{
#ifndef __FL_UPDATE_TASKBAR
		if (g_bKeyTable['1'])
		{
			g_bKeyTable['1'] = 0;
			m_pWndTaskBar->SetItemSlot(0);
		}
		if (g_bKeyTable['2'])
		{
			g_bKeyTable['2'] = 0;
			m_pWndTaskBar->SetItemSlot(1);
		}
		if (g_bKeyTable['3'])
		{
			g_bKeyTable['3'] = 0;
			m_pWndTaskBar->SetItemSlot(2);
		}
		if (g_bKeyTable['4'])
		{
			g_bKeyTable['4'] = 0;
			m_pWndTaskBar->SetItemSlot(3);
		}
		if (g_bKeyTable['5'])
		{
			g_bKeyTable['5'] = 0;
			m_pWndTaskBar->SetItemSlot(4);
		}
		if (g_bKeyTable['6'])
		{
			g_bKeyTable['6'] = 0;
			m_pWndTaskBar->SetItemSlot(5);
		}
		if (g_bKeyTable['7'])
		{
			g_bKeyTable['7'] = 0;
			m_pWndTaskBar->SetItemSlot(6);
		}
		if (g_bKeyTable['8'])
		{
			g_bKeyTable['8'] = 0;
			m_pWndTaskBar->SetItemSlot(7);
		}

		if (g_bKeyTable['1'])
		{
			g_bKeyTable['1'] = 0;
			ObjectExecutor(SHORTCUT_APPLET, APP_HELPER_HELP);
		}

		if (g_bKeyTable['0'])
		{
			g_bKeyTable['0'] = 0;
			ScreenCapture();
		}

		if (g_bKeyTable['9'])
		{
			g_bKeyTable['9'] = 0;
			FILEOUT("Postion.txt", "Pos x = %f, z = %f", g_pPlayer->GetPos().x, g_pPlayer->GetPos().z);
			PutString(prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE));
		}

#else
		if (g_Option.m_bRekeyTask)
		{
			if (g_bKeyTable['1'])
			{
				g_bKeyTable['1'] = 0;
				m_pWndTaskBar->SetItemSlot(0);
			}
			if (g_bKeyTable['2'])
			{
				g_bKeyTable['2'] = 0;
				m_pWndTaskBar->SetItemSlot(1);
			}
			if (g_bKeyTable['3'])
			{
				g_bKeyTable['3'] = 0;
				m_pWndTaskBar->SetItemSlot(2);
			}
			if (g_bKeyTable['4'])
			{
				g_bKeyTable['4'] = 0;
				m_pWndTaskBar->SetItemSlot(3);
			}
			if (g_bKeyTable['5'])
			{
				g_bKeyTable['5'] = 0;
				m_pWndTaskBar->SetItemSlot(4);
			}
			if (g_bKeyTable['6'])
			{
				g_bKeyTable['6'] = 0;
				m_pWndTaskBar->SetItemSlot(5);
			}
			if (g_bKeyTable['7'])
			{
				g_bKeyTable['7'] = 0;
				m_pWndTaskBar->SetItemSlot(6);
			}
			if (g_bKeyTable['8'])
			{
				g_bKeyTable['8'] = 0;
				m_pWndTaskBar->SetItemSlot(7);
			}
			if (g_bKeyTable['9'])
			{
				g_bKeyTable['9'] = 0;
				FILEOUT("Postion.txt", "Pos x = %f, z = %f", g_pPlayer->GetPos().x, g_pPlayer->GetPos().z);
				PutString(prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE));
			}
		}
		else
		{
			if (g_bKeyTable[VK_F1])
			{
				g_bKeyTable[VK_F1] = 0;
				m_pWndTaskBar->SetItemSlot(0);
			}
			if (g_bKeyTable[VK_F2])
			{
				g_bKeyTable[VK_F2] = 0;
				m_pWndTaskBar->SetItemSlot(1);
			}
			if (g_bKeyTable[VK_F3])
			{
				g_bKeyTable[VK_F3] = 0;
				m_pWndTaskBar->SetItemSlot(2);
			}
			if (g_bKeyTable[VK_F4])
			{
				g_bKeyTable[VK_F4] = 0;
				m_pWndTaskBar->SetItemSlot(3);
			}
			if (g_bKeyTable[VK_F5])
			{
				g_bKeyTable[VK_F5] = 0;
				m_pWndTaskBar->SetItemSlot(4);
			}
			if (g_bKeyTable[VK_F6])
			{
				g_bKeyTable[VK_F6] = 0;
				m_pWndTaskBar->SetItemSlot(5);
			}
			if (g_bKeyTable[VK_F7])
			{
				g_bKeyTable[VK_F7] = 0;
				m_pWndTaskBar->SetItemSlot(6);
			}
			if (g_bKeyTable[VK_F8])
			{
				g_bKeyTable[VK_F8] = 0;
				m_pWndTaskBar->SetItemSlot(7);
			}
			if (g_bKeyTable[VK_F9])
			{
				g_bKeyTable[VK_F9] = 0;
				FILEOUT("Postion.txt", "Pos x = %f, z = %f", g_pPlayer->GetPos().x, g_pPlayer->GetPos().z);
				PutString(prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE));
			}
		}

		if (g_bKeyTable['0'])
		{
			g_bKeyTable['0'] = 0;
			ScreenCapture();
		}
#endif
	}
#if __VER >= 8 //__CSC_VER8_2
	CWndBase* pWndPartyQuick = (CWndBase*)GetApplet(APP_PARTY_QUICK);
	if (pWndPartyQuick)
	{
		if (g_Party.GetSizeofMember() < 2)
		{
			pWndPartyQuick->Destroy();
			SetFocus();
		}
	}
#endif //__CSC_VER8_2
	return CWndBase::Process();
}
void CWndMgr::SetPlayer(CMover* pMover)
{
	CWndBase* pWndBase;

	if (pMover)
	{
		if (pWndBase = GetApplet(APP_GAUGE))
			pWndBase->SetTitle(pMover->GetName());

		if (pWndBase = GetApplet(APP_SKILL4))
			((CWndSkill_16*)pWndBase)->InitItem(pMover->GetJob(), pMover->m_aJobSkill);

		if (pWndBase = GetApplet(APP_INVENTORY))
		{
			((CWndInventory*)pWndBase)->m_dwEnchantWaitTime = 0xffffffff;
			((CWndInventory*)pWndBase)->m_wndItemCtrl.InitItem(&pMover->m_Inventory, APP_INVENTORY);
		}

#ifdef __FL_CONSUMABLES_STORAGE
		if (pWndBase = GetApplet(APP_CHEST))
			((CWndChest*)pWndBase)->m_wndItemCtrl.InitItem(&pMover->m_Chest, APP_CHEST);
#endif

		g_Neuz.m_camera.SetPos(pMover->GetPos());

		if (CMover::GetActiveMover() == NULL)
			Error("SetPlayer : ActiveMover ����");

		if (CMover::GetActiveMover()->m_pActMover == NULL)
			Error("SetPlayer : ActionMover ����");

		if (pMover->m_pActMover->IsFly())
			g_Neuz.m_camera.Unlock();
		else
			g_Neuz.m_camera.Lock();

		g_Neuz.m_camera.Reset();
		g_Neuz.m_camera.Process(D3DDEVICE);
		g_Neuz.m_camera.Transform(D3DDEVICE, g_WorldMng.Get());

		CWorld* pWorld = g_WorldMng.Get();
		if (pWorld)
		{
			pWorld->ReadWorld(g_pPlayer->GetPos());
			GuildHouse->ApplyEFTexture();
		}

	}
	else
	{
		if (pWndBase = GetApplet(APP_SKILL4))
			if (pWndBase = GetApplet(APP_GAUGE))
				SAFE_DELETE(((CWndStatus*)pWndBase)->m_pModel);

		if (pWndBase = GetApplet(APP_INVENTORY))
			((CWndInventory*)pWndBase)->m_wndItemCtrl.InitItem(NULL, APP_INVENTORY);

#ifdef __FL_CONSUMABLES_STORAGE
		if (pWndBase = GetApplet(APP_CHEST))
			((CWndChest*)pWndBase)->m_wndItemCtrl.InitItem(NULL, APP_CHEST);
#endif

	}
}
void CWndMgr::WordChange(CString& rString)
{
	CString strResult;

	CScanner s;
	s.SetProg((LPVOID)(LPCSTR)rString);

	s.GetToken();
	BOOL bFirst = (*s.m_pProg == ' ');

	while (s.tok != FINISHED)
	{
		CString str = s.Token;
		//		str.MakeLower();

		CScanner s1;
		s1.SetProg((LPVOID)(LPCSTR)str);
		s1.GetToken();
		BOOL bSecond = (*s1.m_pProg == ' ');
		BOOL bTerminate = (*s1.m_pProg == '\0');

		while (s1.tok != FINISHED)
		{
			CString str1 = s1.Token;
			str1.MakeLower();
			strResult += s1.Token;

			if ((bTerminate && bFirst) ||
				(!bTerminate && bSecond))
				strResult += ' ';

			s1.GetToken();
			bSecond = (*s1.m_pProg == ' ');
			bTerminate = (*s1.m_pProg == '\0');
		}
		s.GetToken();
		bFirst = (*s.m_pProg == ' ');
	}

	rString = strResult;
}

void CWndMgr::ParsingChat(CString string)
{
	string.TrimLeft(' ');
	if (string.IsEmpty() == TRUE)
		return;

	SetStrNull(string, 120);

	if (string.Find("#l") != -1)
	{
		int mCut = 0;
		mCut = string.Find('/');

		if (mCut != -1)
		{
			string = string.Right(string.GetLength() - mCut);
		}
	}

	FormalizeChatString(string);

	if (string.GetLength() > 0)
	{
		if (string[0] == '/')
		{
			ParsingCommand(string.LockBuffer(), g_pPlayer);
			string.UnlockBuffer();
		}
		else
		{
			m_strChatBackup = string;
			WordChange(string);
			SetStrNull(string, 120);
			g_DPlay.SendChat((LPCSTR)string);

		}
	}
}
void CWndMgr::PutDefinedString(DWORD dwText, ...)
{
	TCHAR szBuffer[512];
	TCHAR szFormat[512];
	strcpy(szFormat, prj.GetText(dwText));
	va_list args;
	va_start(args, szFormat);
	int nBuf = _vsntprintf(szBuffer, 512, szFormat, args);
	va_end(args);

	DWORD dwColor = prj.GetTextColor(dwText);
	PutString((LPCTSTR)szBuffer, NULL, dwColor);
}

void CWndMgr::PutString(LPCTSTR lpszString, CObj* pObj, DWORD dwColor, DWORD dwChatStyle, DWORD dwPStyle)
{
	CMover* pMover = (CMover*)pObj;
	CString str = lpszString;
	DWORD dwStyle = 0;
	dwPStyle = PS_USE_MACRO;

	int nChannel = 0;
	switch (CWndChat::m_nChatChannel)
	{
	case CHATSTY_GENERAL: nChannel = 0; break;
	case CHATSTY_WHISPER: nChannel = 1; break;
	case CHATSTY_SHOUT: nChannel = 2; break;
	case CHATSTY_PARTY: nChannel = 3; break;
	case CHATSTY_GUILD: nChannel = 4; break;
	}
	BOOL  bChatLog = TRUE;
	DWORD dwChatFilter = g_Option.m_dwChatFilter[nChannel];
	if ((dwChatFilter & CHATSTY_GENERAL) == dwChatStyle)
	{
		dwStyle |= TMS_CHAT; bChatLog = FALSE;
	}
	if ((dwChatFilter & CHATSTY_WHISPER) == dwChatStyle)
	{
		dwStyle |= TMS_CHAT; bChatLog = FALSE;
	}
	if ((dwChatFilter & CHATSTY_SHOUT) == dwChatStyle)
	{
		dwStyle |= TMS_CHAT; bChatLog = FALSE;
	}
	if ((dwChatFilter & CHATSTY_PARTY) == dwChatStyle)
	{
		dwStyle |= TMS_CHAT; bChatLog = FALSE;
	}
	if ((dwChatFilter & CHATSTY_GUILD) == dwChatStyle)
	{
		dwStyle |= TMS_CHAT; bChatLog = FALSE;
	}

	DWORD dwChatFilterEtc = g_Option.m_dwChatFilterEtc;
	if ((dwChatFilterEtc & CHATSTY_SYSTEM_CLIENT) && (dwChatStyle == CHATSTY_SYSTEM))
	{
		dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_SYSTEM_CHAT;
	}
	if ((dwChatFilterEtc & CHATSTY_SYSTEM_CHAT) && (dwChatStyle == CHATSTY_SYSTEM))
	{
		dwStyle |= TMS_CHAT; dwChatStyle = CHATSTY_SYSTEM_CHAT;
	}
	if ((dwChatFilterEtc & CHATSTY_HELP_CLIENT) && (dwChatStyle == CHATSTY_HELP))
	{
		dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_HELP_CHAT;
	}
	if ((dwChatFilterEtc & CHATSTY_HELP_CHAT) && (dwChatStyle == CHATSTY_HELP))
	{
		dwStyle |= TMS_CHAT; dwChatStyle = CHATSTY_HELP_CHAT;
	}
	if ((dwChatFilterEtc & CHATSTY_GAME_CLIENT) && (dwChatStyle == CHATSTY_GAME))
	{
		dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_GAME_CHAT;
	}
	if ((dwChatFilterEtc & CHATSTY_GAME_CHAT) && (dwChatStyle == CHATSTY_GAME))
	{
		dwStyle |= TMS_CHAT; dwChatStyle = CHATSTY_GAME_CHAT;
	}
	BOOL bRecord = TRUE;

	CWndChat* pWndChat = (CWndChat*)GetWndBase(APP_COMMUNICATION_CHAT);
	CEditString* pChatString = &m_ChatString;




	if (pObj == NULL)
		dwStyle &= ~TMS_DIALOG;
	else
		if (pObj->GetType() == OT_MOVER)
		{
			if (dwChatStyle & CHATSTY_GENERAL)
				dwStyle |= TMS_DIALOG;
			if (((CMover*)pObj)->IsPlayer() == FALSE)
			{
				dwStyle &= ~TMS_CHAT;
				bRecord = FALSE;
			}
			if (((CMover*)pObj)->m_vtInfo.IsVendorOpen())
				dwStyle &= ~TMS_DIALOG;


			if (((CMover*)pObj)->m_dwAuthorization >= AUTH_GAMEMASTER && ((CMover*)pObj)->IsMode(TRANSPARENT_MODE))
				dwStyle &= ~TMS_DIALOG;
		}


	if (dwStyle & TMS_CLIENT)
		g_ClientMsg.AddMessage(lpszString, dwColor);

	CString strChat = lpszString;
	strChat.Replace("\n", " ");
	strChat.Replace('\n', ' ');


	if (bRecord)
	{

		if (m_aChatString.GetSize() > 500)
		{
			m_aChatString.RemoveAt(0, 250);
			m_aChatColor.RemoveAt(0, 250);
			m_aChatStyle.RemoveAt(0, 250);
		}

		m_aChatString.Add(strChat);
		//		m_aChatString.Add( lpszString );
		m_aChatColor.Add(dwColor);
		m_aChatStyle.Add(dwChatStyle);
	}
	if (dwStyle & TMS_CHAT)
	{
		if (pWndChat)
			pWndChat->PutString(strChat, dwColor, dwPStyle);
		//else
		//{
		//	m_ChatString.AddString( lpszString, dwColor );
		//	m_ChatString.AddString( "\n", dwColor );// += '\n';
		//}
	}

	if (dwStyle & TMS_CAPTION)
	{
		CWndWorld* pWndWorld = (CWndWorld*)GetWndBase(APP_WORLD);
		if (::GetLanguage() != LANG_JAP)
			g_Caption1.AddCaption(lpszString, pWndWorld->m_pFontAPITitle);
		else
			g_Caption1.AddCaption(lpszString, NULL);
	}


	if (dwStyle & TMS_DIALOG)
	{
		if (pObj->GetType() == OT_MOVER)// && ((CMover*)pObj)->m_nQuestEmoticonIndex == -1 )
		{
			TCHAR* lpszChat = (TCHAR*)_tcschr(lpszString, _T(':'));
			lpszChat += sizeof(TCHAR) * 2;	// Skip ": "
			g_DialogMsg.AddMessage(pObj, lpszChat, 0xffffffff, 0, dwPStyle);
		}
	}

#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	if (m_pWndChatLog)
	{
		if (bChatLog && !(dwStyle & TMS_DIALOG) && dwChatStyle != CHATSTY_SHOUT)
			m_pWndChatLog->PutString(lpszString, dwColor, dwPStyle);
	}
#endif //__Y_CHAT_SYSTEM_8
}

void CWndMgr::PutToolTip_Skill(DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive)
{
	ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
	if (pSkillProp == NULL)
	{
		Error("CWndMgr::PutToolTip_Skill : ��ų(%d)�� ������Ƽ�� ����.", dwSkill);
		return;	// property not found
	}

	DWORD dwSkillLevel = dwLevel;

	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, dwSkillLevel);
	if (pAddSkillProp == NULL)
	{
		Error("CWndMgr::PutToolTip_Skill : add��ų(%d)�� ������Ƽ�� ����.", dwSkill);
		return;	// property not found
	}

	DWORD dwaf = D3DCOLOR_XRGB(199, 155, 0);
	CString str;
	CString strstr;
	CString string;
	if (bActive)
		string.Format("#b#cff2fbe6d%s   Lv %d#nb#nc", pSkillProp->szName, dwSkillLevel);
	else
		string.Format("#b#cff2fbe6d%s#nb#nc", pSkillProp->szName);
	char statebuf[30] = { 0, };
	if (pAddSkillProp->nReqMp != 0xffffffff)
	{
		sprintf(statebuf, "\nMP : %d", g_pPlayer->GetReqMp(pAddSkillProp->nReqMp));
		string += statebuf;
	}
	if (pAddSkillProp->nReqFp != 0xffffffff)
	{
		sprintf(statebuf, "\nFP : %d", g_pPlayer->GetReqFp(pAddSkillProp->nReqFp));
		string += statebuf;
	}


	if (pSkillProp->dwReSkill1 != -1)
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill1);

		if (pskill)
		{
			strstr.Format(prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel1);
			if (pskill->dwLevel < pSkillProp->dwReSkillLevel1)
				str.Format("\n#cffff0000%s#nc", strstr);
			else
				str.Format("\n%s#nc", strstr);
			string += str;
		}
	}
	if (pSkillProp->dwReSkill2 != -1)
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill2);

		if (pskill)
		{
			strstr.Format(prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel2);
			if (pskill->dwLevel < pSkillProp->dwReSkillLevel2)
				str.Format("\n#cffff0000%s#nc", strstr);
			else
				str.Format("\n%s#nc", strstr);
			string += str;
		}
	}

	strstr.Format(prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV);

	if (g_pPlayer->GetLevel() < (int)(pSkillProp->dwReqDisLV) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() && !g_pPlayer->IsLegendHero())
		str.Format("\n#cffff0000%s#nc", strstr);
	else
		str.Format("\n%s", strstr);

	string += str;

	if (strcmp(pSkillProp->szCommand, "="))
	{
		string += "\n";
		string += pSkillProp->szCommand;
	}

	int nSkillIdx = g_pPlayer->GetSkillIdx(dwSkill);

	if (nSkillIdx != -1)
	{
		DWORD dwDelay = g_pPlayer->GetReuseDelay(nSkillIdx);
		if (dwDelay > 0)
		{
			CTimeSpan ct((long)(dwDelay / 1000.0f));
			str.Format(prj.GetText(TID_TOOLTIP_COOLTIME), ct.GetMinutes(), ct.GetSeconds());
			string += "\n";
			string += str;
		}
	}

	CEditString strEdit;
	strEdit.SetParsingString(string);

	g_toolTip.PutToolTip(dwSkill, strEdit, *pRect, point, 0);
}
void CWndMgr::PutToolTip_Troupe(DWORD dwSkill, CPoint point, CRect* pRect)
{
	ItemProp* pSkillProp = prj.GetPartySkill(dwSkill);

	if (pSkillProp == NULL)
		return;

	CString string;
	string.Format("#b#cff2fbe6d%s#nb#nc", pSkillProp->szName);

	//string = pSkillProp->szName;
	char statebuf[30] = { 0, };
	if (pSkillProp->dwReqMp != 0xffffffff)
	{
		sprintf(statebuf, "\nMP : %d", pSkillProp->dwReqMp);
		string += statebuf;
	}
	if (pSkillProp->dwReqFp != 0xffffffff)
	{
		sprintf(statebuf, "\nFP : %d", pSkillProp->dwReqFp);
		string += statebuf;
	}
	if (strcmp(pSkillProp->szCommand, "="))
	{
		CString str;
		string += "\n";
		string += pSkillProp->szCommand;
	}
	CEditString strEdit;
	strEdit.SetParsingString(string);

	g_toolTip.PutToolTip(dwSkill, strEdit, *pRect, point, 0);
}

void CWndMgr::PutToolTip_Item(DWORD dwType, DWORD dwId, int nLocation, CPoint point, CRect* pRect, int flag)
{
	CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(nLocation);
	if (!pContainer)
		return;

	CItemElem* pItemBase = pContainer->GetAtId(dwId);
	if (!pItemBase)
		return;

	PutToolTip_Item(pItemBase, point, pRect, flag);
}

struct DST_STRING
{
	int nDstParam;		// ex) DST_STR
	int nNameIndex;
};

static DST_STRING g_DstString[] =
{
	DST_STR                     , TID_TOOLTIP_STR,
	DST_DEX                     , TID_TOOLTIP_DEX,
	DST_INT                     , TID_TOOLTIP_INT,
	DST_STA                     , TID_TOOLTIP_STA,
	DST_SPEED                   , TID_TOOLTIP_SPEED,
	DST_ABILITY_MIN             , TID_TOOLTIP_ABILITYMIN,
	DST_ABILITY_MAX             , TID_TOOLTIP_ABILITYMAX,
	DST_ATTACKSPEED             , TID_TOOLTIP_ATKSPEED,
	DST_ADJDEF                  , TID_TOOLTIP_DEFENCE,
	DST_RESIST_MAGIC            , TID_TOOLTIP_DEFMAGIC,
	DST_RESIST_ELECTRICITY      , TID_TOOLTIP_DEFELECTRICITY,
	DST_RESIST_ALL              , TID_TOOLTIP_DEFALLELEMETAL,
	DST_RESIST_FIRE             , TID_TOOLTIP_DEFFIFE,
	DST_RESIST_WIND             , TID_TOOLTIP_DEFWIND,
	DST_RESIST_WATER            , TID_TOOLTIP_DEFWATER,
	DST_RESIST_EARTH            , TID_TOOLTIP_DEFEARTH,
	DST_HP_MAX                  , TID_TOOLTIP_MAXHP,
	DST_MP_MAX                  , TID_TOOLTIP_MAXMP,
	DST_FP_MAX                  , TID_TOOLTIP_MAXFP,
	DST_HP                      , TID_TOOLTIP_HP,
	DST_MP                      , TID_TOOLTIP_MP,
	DST_FP                      , TID_TOOLTIP_FP,
	DST_HP_RECOVERY             , TID_TOOLTIP_HPRECOVERY,
	DST_MP_RECOVERY             , TID_TOOLTIP_MPRECOVERY,
	DST_FP_RECOVERY             , TID_TOOLTIP_FPRECOVERY,
	DST_HP_RECOVERY_RATE        , TID_TOOLTIP_HPRECOVERYRATE,
	DST_MP_RECOVERY_RATE        , TID_TOOLTIP_MPRECOVERYRATE,
	DST_FP_RECOVERY_RATE        , TID_TOOLTIP_FPRECOVERYRATE,
	DST_ALL_RECOVERY			, TID_TOOLTIP_ALL_RECOVERY,
	DST_ALL_RECOVERY_RATE		, TID_TOOLTIP_ALL_RECOVERY_RATE,
	DST_KILL_HP					, TID_TOOLTIP_KILL_HP,
	DST_KILL_MP					, TID_TOOLTIP_KILL_MP,
	DST_KILL_FP					, TID_TOOLTIP_KILL_FP,
	DST_KILL_ALL				, TID_TOOLTIP_KILL_ALL,
	DST_KILL_HP_RATE			, TID_TOOLTIP_KILL_HP_RATE,
	DST_KILL_MP_RATE			, TID_TOOLTIP_KILL_MP_RATE,
	DST_KILL_FP_RATE			, TID_TOOLTIP_KILL_FP_RATE,
	DST_KILL_ALL_RATE			, TID_TOOLTIP_KILL_ALL_RATE,
	DST_ALL_DEC_RATE			, TID_TOOLTIP_ALL_DEC_RATE,
	DST_ADJ_HITRATE             , TID_TOOLTIP_HITRATE,
	DST_CHR_DMG                 , TID_TOOLTIP_CHRDMG,
	DST_CHRSTATE                , TID_TOOLTIP_CHRSTATE,
	DST_PARRY                   , TID_TOOLTIP_PARRY,
	DST_ATKPOWER_RATE           , TID_TOOLTIP_ATKPOWER,
#ifdef __FL_DST_ATKPOWER_RATE_MAGIC
	DST_ATKPOWER_RATE_MAGIC     , TID_TOOLTIP_ATKPOWER_MAGIC,
#endif
	DST_JUMPING                 , TID_TOOLTIP_JUMPING,
	DST_BLOCK_MELEE				, TID_GAME_TOOLTIP_BLOCK,
	DST_BLOCK_RANGE				, TID_GAME_TOOLTIP_BLOCKRANGE,
	DST_STAT_ALLUP				, TID_GAME_TOOLTIPALLSTAT,
	DST_HP_MAX_RATE				, TID_TOOLTIP_DST_HP_MAX_RATE,
	DST_ADDMAGIC				, TID_GAME_TOOLTIPCONDITIONRATE,
	DST_ADJDEF_RATE				, TID_TOOLTIP_DST_ADJDEF_RATE,
	DST_HP_MAX_RATE				, TID_TOOLTIP_DST_HP_MAX_RATE,
	DST_MP_MAX_RATE				, TID_TOOLTIP_DST_MP_MAX_RATE,
	DST_FP_MAX_RATE				, TID_TOOLTIP_DST_FP_RATE,
	DST_CHR_CHANCECRITICAL		, TID_TOOLTIP_DST_CRITICAL_RATE,
	DST_MASTRY_EARTH			, TID_TOOLTIP_DST_MASTRY_EARTH,
	DST_MASTRY_FIRE				, TID_TOOLTIP_DST_MASTRY_FIRE,
	DST_MASTRY_WATER			, TID_TOOLTIP_DST_MASTRY_WATER,
	DST_MASTRY_ELECTRICITY		, TID_TOOLTIP_DST_MASTRY_ELECTRICITY,
	DST_MASTRY_WIND				, TID_TOOLTIP_DST_MASTRY_WIND,
	DST_REFLECT_DAMAGE			, TID_TOOLTIP_DST_REFLECT_DAMAGE,
	DST_MP_DEC_RATE				, TID_TOOLTIP_DST_MP_DEC_RATE,
	DST_FP_DEC_RATE				, TID_TOOLTIP_DST_FP_DEC_RATE,
	DST_SPELL_RATE				, TID_TOOLTIP_DST_SPELL_RATE,
	DST_CAST_CRITICAL_RATE		, TID_TOOLTIP_DST_CAST_CRITICAL_RATE,
	DST_CRITICAL_BONUS			, TID_TOOLTIP_DST_CRITICAL_BONUS,
	DST_YOY_DMG,				  TID_TOOLTIP_DST_YOY_DMG,
	DST_BOW_DMG,				  TID_TOOLTIP_DST_BOW_DMG,
	DST_KNUCKLE_DMG,			  TID_TOOLTIP_DST_KNUCKLE_DMG,
	DST_SWD_DMG,				  TID_TOOLTIP_DST_SWD_DMG,
	DST_AXE_DMG,				  TID_TOOLTIP_DST_AXE_DMG,
#ifdef __NEWWPN1024
	DST_MASTRY_ALL,				  TID_TOOLTIP_DST_MASTRY_ALL,
#endif	// __NEWWPN1024
	DST_ATTACKSPEED_RATE,		  TID_TOOLTIP_ATTACKSPEED_RATE,
	DST_CHR_STEALHP,			  TID_TOOLTIP_DST_DMG_GET,
#if __VER >= 8 // __GUILDCOMBAT_85
	DST_PVP_DMG_RATE,				TID_TOOLTIP_DST_DMG_GET,
#endif // __VER >= 8
#if __VER >= 10 // __CSC_VER9_1
	DST_EXPERIENCE,					TID_TOOLTIP_DST_EXPERIENCE,
	DST_MELEE_STEALHP,				TID_TOOLTIP_DST_MELEE_STEALHP,
	DST_MONSTER_DMG,				TID_TOOLTIP_DST_MONSTER_DMG,
	DST_PVP_DMG,					TID_TOOLTIP_DST_PVP_DMG,
#endif //__CSC_VER9_1
#if __VER >= 9	// __PET_0410
	DST_HEAL,	TID_TOOLTIP_DST_HEAL,
#endif	// __PET_0410
	DST_ATKPOWER,	TID_TOOLTIP_ATKPOWER_VALUE,
#if __VER >= 10
	DST_ONEHANDMASTER_DMG,			TID_TOOLTIP_DST_ONEHANDMASTER_DMG,
	DST_TWOHANDMASTER_DMG,			TID_TOOLTIP_DST_TWOHANDMASTER_DMG,
	DST_YOYOMASTER_DMG,				TID_TOOLTIP_DST_YOYOMASTER_DMG,
	DST_BOWMASTER_DMG,				TID_TOOLTIP_DST_BOWMASTER_DMG,
	DST_KNUCKLEMASTER_DMG,			TID_TOOLTIP_DST_KNUCKLEMASTER_DMG,
	DST_HAWKEYE_RATE,				TID_TOOLTIP_DST_HAWKEYE,
	DST_RESIST_MAGIC_RATE,			TID_TOOLTIP_DEFMAGIC_RATE,
#endif
#ifdef __DST_GIFTBOX
	DST_GIFTBOX,					TID_TOOLTIP_DST_GIFTBOX,
#endif // 
	DST_HPDMG_UP,					TID_TOOLTIP_DST_HPDMG_UP,
	DST_DEFHITRATE_DOWN,			TID_TOOLTIP_DST_DEFHITRATE_DOWN,
	DST_CHR_RANGE,					TID_GAME_TOOLTIP_ATTACKRANGE4,
	DST_STOP_MOVEMENT,				TID_GAME_TOOLTIP_MOVEMENT1,
	DST_IMMUNITY,					TID_GAME_TOOLTIP_IMMUNITY1,
	DST_IGNORE_DMG_PVP, TID_MMI_ABSOLUTEBERRIER01,

	0,  0
};

BOOL CWndMgr::IsDst_Rate(int nDstParam)
{
	static int nDstRate[] = {
			0,
			0,
			DST_ADJ_HITRATE,
			0,
			DST_ATKPOWER_RATE,
#ifdef __FL_DST_ATKPOWER_RATE_MAGIC
			DST_ATKPOWER_RATE_MAGIC,
#endif
			DST_ADJDEF_RATE,
			DST_DEFHITRATE_DOWN,
			DST_HP_MAX_RATE,
			DST_MP_MAX_RATE,
			DST_FP_MAX_RATE,
			DST_HP_RECOVERY_RATE,
			DST_MP_RECOVERY_RATE,
			DST_FP_RECOVERY_RATE,
			DST_CHR_CHANCECRITICAL,
			DST_MASTRY_EARTH,
			DST_MASTRY_FIRE,
			DST_MASTRY_WATER,
			DST_MASTRY_ELECTRICITY,
			DST_MASTRY_WIND,
			DST_ATTACKSPEED,
			DST_MP_DEC_RATE,
			DST_FP_DEC_RATE,
			DST_SPELL_RATE,
			DST_CAST_CRITICAL_RATE,
			DST_CRITICAL_BONUS,
#ifdef __NEWWPN1024
			DST_MASTRY_ALL,
#endif

			DST_ALL_RECOVERY_RATE,
			DST_KILL_HP_RATE,
			DST_KILL_MP_RATE,
			DST_KILL_FP_RATE,
			DST_KILL_ALL_RATE,
			DST_ALL_DEC_RATE,
			DST_BLOCK_MELEE,
			DST_BLOCK_RANGE,
			DST_ATTACKSPEED_RATE,
			DST_CHR_STEALHP,
#if __VER >= 10
			DST_EXPERIENCE,
			DST_HAWKEYE_RATE,
			DST_RESIST_MAGIC_RATE,
#endif
			DST_SPEED,
			DST_REFLECT_DAMAGE
#if __VER >= 15 // __GUILD_HOUSE
			, DST_MONSTER_DMG
			, DST_PVP_DMG
#endif // __GUILD_HOUSE
	};

	int n = sizeof(nDstRate) / sizeof(nDstRate[0]);

	for (int i = 0; i < n; ++i)
	{
		if (nDstRate[i] == nDstParam)
			return TRUE;
	}

	return FALSE;
}



DWORD CWndMgr::PutItemName(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	CString str;
	CString strTemp;
	DWORD dwColorbuf = dwItemColor.dwName0;

	if (pItemProp->dwReferStat1 == WEAPON_GENERAL)
		dwColorbuf = dwItemColor.dwName0;
	else if (pItemProp->dwReferStat1 == WEAPON_UNIQUE)
		dwColorbuf = dwItemColor.dwName1;
	else if (pItemProp->dwReferStat1 == WEAPON_ULTIMATE)
		dwColorbuf = dwItemColor.dwName3;
	else if (pItemProp->dwReferStat1 == ARMOR_SET)
		dwColorbuf = dwItemColor.dwName1;

	if (pItemElem->IsFlag(CItemElem::binds) && pItemProp->dwFlag != IP_FLAG_EQUIP_BIND)
		strTemp.Format("%s ", prj.GetText(TID_GAME_TOOLTIP_ITEM_BINDS));
	BOOL bCheckBonus = FALSE;
	if (pItemProp->dwParts != (DWORD)-1)
	{
		PRANDOMOPTITEM pRandomOptItem = CRandomOptItemGen::GetInstance()->GetRandomOptItem(pItemElem->GetRandomOpt());
		if (pRandomOptItem)
			strTemp.Format("%s ", pRandomOptItem->pszString);
	}

	BOOL bGuildCombatCloak = FALSE;

	if (pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON1 || pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON2 ||
		pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON3 || pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON4)
		bGuildCombatCloak = TRUE;


	str = pItemElem->GetName();
	CString stredit;
	strTemp += str;

#ifdef __WEAPON_RARITY
	if (pItemElem->m_nWeaponRarity > 0)
	{
		CString strWeapon;
		DWORD dwRarityColor = 0xff199bd5;
		if (pItemElem->m_nWeaponRarity == 5)
		{
			strWeapon.Format("[Legendary] %s", strTemp);
			dwRarityColor = 0xffb28a33;
		}
		else if (pItemElem->m_nWeaponRarity == 4)
		{
			strWeapon.Format("[Mythical] %s", strTemp);
			dwRarityColor = 0xff8847ff;
		}
		else if (pItemElem->m_nWeaponRarity == 3)
		{
			strWeapon.Format("[Rare] %s", strTemp);
			dwRarityColor = 0xff4b69ff;
		}
		else if (pItemElem->m_nWeaponRarity == 2)
		{
			strWeapon.Format("[Uncommon] %s", strTemp);
			dwRarityColor = 0xff5e98d9;
		}
		else
			strWeapon.Format("[Common] %s", strTemp);
		if (bCheckBonus)
			stredit.Format("%s", strWeapon);
		else
		{
			if (pItemElem->GetProp()->IsUltimate())
				stredit.Format("                 %s", strWeapon);
			else
				stredit.Format("%s", strWeapon);
		}

		pEdit->AddString(stredit, dwRarityColor, ESSTY_BOLD);
	}
	else
	{
#endif // __WEAPON_RARITY

	if (pItemProp->IsUltimate())
		stredit.Format("             %s", strTemp);
	else
		stredit.Format("%s", strTemp);

	pEdit->AddString(stredit, dwColorbuf, ESSTY_BOLD);

#ifdef __WEAPON_RARITY
	}
#endif // __WEAPON_RARITY
	return dwColorbuf;
}

void CWndMgr::PutItemAbilityPiercing(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit, DWORD dwColorBuf)
{
	CString strTemp;
	if (pItemElem->GetAbilityOption())
	{
		strTemp.Format(" %+d", pItemElem->GetAbilityOption());
#ifdef __WEAPON_RARITY
		if (pItemElem->m_nWeaponRarity > 0)
		{
			DWORD dwRarityColor = 0xff199bd5;
			if (pItemElem->m_nWeaponRarity == 5)
				dwRarityColor = 0xffb28a33;
			else if (pItemElem->m_nWeaponRarity == 4)
				dwRarityColor = 0xff8847ff;
			else if (pItemElem->m_nWeaponRarity == 3)
				dwRarityColor = 0xff4b69ff;
			else if (pItemElem->m_nWeaponRarity == 2)
				dwRarityColor = 0xff5e98d9;

			pEdit->AddString(strTemp, dwRarityColor, ESSTY_BOLD);
		}
		else
#endif // __WEAPON_RARITY
		pEdit->AddString(strTemp, dwColorBuf, ESSTY_BOLD);
	}

	int nCount = 0;
	for (int j = 0; j < pItemElem->GetPiercingSize(); j++)
	{
		if (pItemElem->GetPiercingItem(j) != 0)
			nCount++;
	}
	if (pItemElem->GetPiercingSize() > 0)
	{
		strTemp.Format("    (%d/%d)", nCount, pItemElem->GetPiercingSize());
		pEdit->AddString(strTemp, dwItemColor.dwPiercing);
	}
}

void CWndMgr::PutItemResist(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	CString str;
	CString strTemp;
	DWORD dwResistColor = dwItemColor.dwResist;
	str = "Element:�";
	switch (pItemElem->m_bItemResist)
	{
	case SAI79::FIRE:
	{
		str += prj.GetText(TID_UPGRADE_FIRE);
		dwResistColor = dwItemColor.dwResistFire;
	}
	break;
	case SAI79::WATER:
	{
		str += prj.GetText(TID_UPGRADE_WATER);
		dwResistColor = dwItemColor.dwResistWater;
	}
	break;
	case SAI79::EARTH:
	{
		str += prj.GetText(TID_UPGRADE_EARTH);
		dwResistColor = dwItemColor.dwResistEarth;
	}
	break;
	case SAI79::ELECTRICITY:
	{
		str += prj.GetText(TID_UPGRADE_ELECTRICITY);
		dwResistColor = dwItemColor.dwResistElectricity;
	}
	break;
	case SAI79::WIND:
	{
		str += prj.GetText(TID_UPGRADE_WIND);
		dwResistColor = dwItemColor.dwResistWind;
	}
	break;
	}

	if (pItemProp->IsEleRefineryAble())
	{
		if (!str.IsEmpty())
		{
			BOOL bBold = FALSE;
			strTemp.Format("\n%s %+d", str, pItemElem->m_nResistAbilityOption);
			pEdit->AddString(strTemp, dwResistColor);
		}
	}
	else
	{
		strTemp.Format("\n%s Lv%d", str, pItemProp->dwItemLV);
		pEdit->AddString(strTemp, dwResistColor);
	}
}

void CWndMgr::PutItemSpeed(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	CString strTemp;
	if (pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC)
	{
		if (pItemProp->fAttackSpeed != 0xffffffff)
		{
			pEdit->AddString("\n");
			strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_ATTACKSPEED), GetATKSpeedString(pItemProp->fAttackSpeed));
			pEdit->AddString(strTemp, dwItemColor.dwGeneral);
		}
	}
}

#if __VER >= 11 // __SYS_IDENTIFY
void CWndMgr::PutAwakeningBlessing(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	int nKind = g_xRandomOptionProperty->GetRandomOptionKind(pItemElem);
	if (nKind < 0)
		return;
	int nSize = g_xRandomOptionProperty->GetRandomOptionSize(pItemElem->GetRandomOptItemId());

#ifdef __PROTECT_AWAKE

	bool bSafe = g_xRandomOptionProperty->IsCheckedSafeFlag(pItemElem->GetRandomOptItemId());
	if (bSafe)
		nSize = g_xRandomOptionProperty->GetViewRandomOptionSize(pItemElem->GetRandomOptItemId());

	assert(nSize >= 0 && nSize < 4);
#endif //__PROTECT_AWAKE

	CString	str;

	BOOL bBlessing = FALSE;
	// title
	if (nKind == CRandomOptionProperty::eAwakening)
	{
		if (nSize == 0)
		{
			str.Format("\n\"%s\"", prj.GetText(TID_GAME_AWAKENING));
			pEdit->AddString(str, dwItemColor.dwAwakening);
		}
	}
	else if (nKind == CRandomOptionProperty::eBlessing)
	{
		if (nSize > 0)
		{
			str.Format("\n%s", prj.GetText(TID_GAME_BLESSING_CAPTION));
			pEdit->AddString(str, dwItemColor.dwBlessing);
			bBlessing = TRUE;
		}
		else if (nSize == 0)
		{
			str.Format("\n\"%s\"", prj.GetText(TID_GAME_BLESSING));
			pEdit->AddString(str, dwItemColor.dwAwakening);
		}
	}
	else if (nKind == CRandomOptionProperty::eSystemPet || nKind == CRandomOptionProperty::eEatPet)
	{
		if (nSize == 0)
		{
			str.Format("\n\"%s\"", prj.GetText(TID_GAME_AWAKENNIG_PET_00));
			pEdit->AddString(str, dwItemColor.dwAwakening);
		}
	}

	for (int i = 0; i < nSize; i++)
	{
		int nDst, nAdj;
		if (!g_xRandomOptionProperty->GetParam(pItemElem->GetRandomOptItemId(), i, &nDst, &nAdj))
			continue;
		if (IsDst_Rate(nDst))
		{
			if (nDst == DST_ATTACKSPEED)
				str.Format("\n%s %c%d%% ", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj / 2 / 10));
			else
				str.Format("\n%s %c%d%%", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj));
		}
		else
		{
			str.Format("\n%s %c%d", FindDstString(nDst), (nAdj > 0 ? '+' : '-'), ::abs(nAdj));
		}

		if (nKind == CRandomOptionProperty::eAwakening)
		{
			DWORD dwStyle = 0;
#ifdef __PROTECT_AWAKE
			dwStyle = (bSafe ? ESSTY_STRIKETHROUGH : 0);
#endif //__PROTECT_AWAKE
			pEdit->AddString(str, dwItemColor.dwAwakening, dwStyle);
		}
		else
			pEdit->AddString(str, dwItemColor.dwBlessing);
	}
	if (bBlessing)
	{
		str.Format("\n%s", prj.GetText(TID_GAME_BLESSING_WARNING));
		pEdit->AddString(str, dwItemColor.dwBlessingWarning);
	}
}
#endif	// __SYS_IDENTIFY

#ifdef __MODEL_CHANGE
void CWndMgr::PutLookText(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	if (!pItemElem->IsChangedLook())
		return;

	ItemProp* pLookProp = prj.GetItemProp(pItemElem->GetLook());
	if (!pLookProp)
		return;

	CString tmp;
	tmp.Format(prj.GetText(TID_MMI_LOOKCHANGE10), pLookProp->szName);
	pEdit->AddString('\n');
	pEdit->AddString(tmp, 0xffff0000);
}
#endif

void CWndMgr::PutRandomOpt(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	if (pItemProp->dwParts == (DWORD)-1)
		return;

	CString strTemp;
	PRANDOMOPTITEM pRandomOptItem = CRandomOptItemGen::GetInstance()->GetRandomOptItem(pItemElem->GetRandomOpt());
	if (pRandomOptItem)
	{
		for (int i = 0; i < pRandomOptItem->ia.nSize; i++)
		{
			int nDst = (int)pRandomOptItem->ia.anDstParam[i];
			int nAdj = (int)pRandomOptItem->ia.anAdjParam[i];

			if (IsDst_Rate(nDst))
			{
				if (nDst == DST_ATTACKSPEED)
					strTemp.Format("\n%s %+d%%", FindDstString(nDst), nAdj / 2 / 10);
				else
					strTemp.Format("\n%s %+d%%", FindDstString(nDst), nAdj);
			}
			else
			{
				strTemp.Format("\n%s +%d", FindDstString(nDst), nAdj);
			}

			pEdit->AddString(strTemp, dwItemColor.dwRandomOption);
		}
	}
}
void CWndMgr::PutPiercingOpt(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	CString strTemp;
	PIERCINGAVAIL pa;
	memset(&pa, 0, sizeof(PIERCINGAVAIL));
	pItemElem->GetPiercingAvail(&pa);
	for (int i = 0; i < pa.nSize; i++)
	{
		int nDst = (int)pa.anDstParam[i];
		int nAdj = (int)pa.anAdjParam[i];

		if (IsDst_Rate(nDst))
		{
			if (nDst == DST_ATTACKSPEED)
				strTemp.Format("\n%s %+d%%", FindDstString(nDst), nAdj / 2 / 10);
			else
				strTemp.Format("\n%s %+d%%", FindDstString(nDst), nAdj);
		}
		else
		{
			strTemp.Format("\n%s +%d", FindDstString(nDst), nAdj);
		}

		pEdit->AddString(strTemp, dwItemColor.dwPiercing);
	}
}

void CWndMgr::PutEnchantOpt(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit, int flag)
{
	CString str;
	CString strTemp;
	BOOL bPSetItem = FALSE;
	int nAbilityOption = pMover->GetSetItemClient();
	if (pMover->IsActiveMover())
	{
		if (flag == APP_INVENTORY && pMover->m_Inventory.IsEquip(pItemElem->m_dwObjId) && pMover->IsSetItemPart(pItemProp->dwParts)
			&& nAbilityOption > 0)
			bPSetItem = TRUE;
	}
	else
	{
		if (flag == APP_QUERYEQUIP && pMover->IsSetItemPart(pItemProp->dwParts) && nAbilityOption > 0)
			bPSetItem = TRUE;
	}
	if (bPSetItem)
	{
		PSETITEMAVAIL psa = prj.GetSetItemAvail(nAbilityOption);
		if (psa) // DST_MAX_HITRATE
		{
			if (psa->nHitRate > 0)
			{
				strTemp.Format("\n%s +%d%%", FindDstString((int)DST_ADJ_HITRATE), (int)psa->nHitRate);
				pEdit->AddString(strTemp, dwItemColor.dwEnchantOption);
			}
			if (psa->nBlock > 0)
			{
				str = prj.GetText(TID_GAME_TOOLTIPBLOCKRATE);
				strTemp.Format("\n%s +%d%%", str, (int)psa->nBlock);
				pEdit->AddString(strTemp, dwItemColor.dwEnchantOption);
			}
			if (psa->nMaxHitPointRate > 0)
			{
				strTemp.Format("\n%s +%d%%", FindDstString((int)DST_HP_MAX_RATE), (int)psa->nMaxHitPointRate);
				pEdit->AddString(strTemp, dwItemColor.dwEnchantOption);
			}
			if (psa->nAddMagic > 0)
			{
				strTemp.Format("\n%s +%d", FindDstString((int)DST_ADDMAGIC), (int)psa->nAddMagic);
				pEdit->AddString(strTemp, dwItemColor.dwEnchantOption);
			}
			if (psa->nAdded > 0)
			{
				strTemp.Format("\n%s +%d", FindDstString((int)DST_STAT_ALLUP), (int)psa->nAdded);
				pEdit->AddString(strTemp, dwItemColor.dwEnchantOption);
			}
		}
	}
}

void CWndMgr::PutSetItemOpt(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	CString strTemp;

	CSetItem* pSetItem = CSetItemFinder::GetInstance()->GetSetItemByItemId(pItemElem->m_dwItemId);
	if (pSetItem)
	{

		BOOL pbEquiped[MAX_HUMAN_PARTS];
		memset(pbEquiped, 0, sizeof(pbEquiped));
		int nEquiped;
		pMover->GetEquipedSetItem(pSetItem->m_nId, pbEquiped, &nEquiped);

		strTemp.Format("\n\n%s (%d/%d)", pSetItem->GetString(), pMover->GetEquipedSetItemNumber(pSetItem), pSetItem->m_nElemSize);
		//		pEdit->AddString( strTemp, dwItemColor.dwSetName, ESSTY_BOLD );
		pEdit->AddString(strTemp, dwItemColor.dwSetName);

		for (int i = 0; i < pSetItem->m_nElemSize; i++)
		{
			ItemProp* pItemProp = prj.GetItemProp(pSetItem->m_adwItemId[i]);
			if (pItemProp)
			{
				strTemp.Format("\n   %s", pItemProp->szName);
				if (pbEquiped[i])
					pEdit->AddString(strTemp, dwItemColor.dwSetItem1);
				else
					pEdit->AddString(strTemp, dwItemColor.dwSetItem0);
			}
		}

		nEquiped = pSetItem->m_nElemSize;

		ITEMAVAIL itemAvail;
		memset(&itemAvail, 0, sizeof(itemAvail));

		int nEqui[MAX_ITEMAVAIL] = { 0 };
		pSetItem->GetItemEquiped(*nEqui, &itemAvail, nEquiped);

		DWORD dwColor = 0xff000000;

		for (int i = 0; i < itemAvail.nSize; i++)
		{
			int nDst = (int)itemAvail.anDstParam[i];
			int nAdj = (int)itemAvail.anAdjParam[i];

			if (IsDst_Rate(nDst))
			{
				if (nDst == DST_ATTACKSPEED)
					strTemp.Format("\n%s(%d/%d): %s %+d%%", prj.GetText(TID_TOOLTIP_SET), nEqui[i], pSetItem->m_nElemSize, FindDstString(nDst), nAdj / 2 / 10);
				else
					strTemp.Format("\n%s(%d/%d): %s %+d%%", prj.GetText(TID_TOOLTIP_SET), nEqui[i], pSetItem->m_nElemSize, FindDstString(nDst), nAdj);
			}
			else
			{
				strTemp.Format("\n%s(%d/%d): %s +%d", prj.GetText(TID_TOOLTIP_SET), nEqui[i], pSetItem->m_nElemSize, FindDstString(nDst), nAdj);
			}
			dwColor = nEqui[i] <= pMover->GetEquipedSetItemNumber(pSetItem) ? D3DCOLOR_XRGB(255, 157, 0) : D3DCOLOR_XRGB(178, 178, 178);
			pEdit->AddString(strTemp, dwColor);
		}
	}
}

void CWndMgr::PutItemMinMax(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	pEdit->AddString("\n");
	CString strTemp;
	if (pItemProp->dwAbilityMin != 0xffffffff && pItemProp->dwAbilityMax != 0xffffffff
		&& pItemProp->dwEndurance != 0xffffffff)
	{
		DWORD dwColorMinMax = dwItemColor.dwEffective3;
		float f = pMover->GetItemMultiplier(pItemElem);

		int nMin = (int)(pMover->GetItemAbilityMin(pItemProp->dwID) * f);
		int nMax = (int)(pMover->GetItemAbilityMax(pItemProp->dwID) * f);
		if (1.0f <= f)
			dwColorMinMax = dwItemColor.dwEffective0;
		else if (0.8f <= f)
			dwColorMinMax = dwItemColor.dwEffective1;
		else if (0.6f <= f)
			dwColorMinMax = dwItemColor.dwEffective2;

		if (pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC)
		{
			int nOpt = 0;
			if (pItemElem->GetAbilityOption() > 0)
			{
				int nAdd = (int)pow((float)(pItemElem->GetAbilityOption()), 1.5f);

				nMin += nAdd;
				nMax += nAdd;
			}
			if (nOpt)
			{
				strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE1));
				pEdit->AddString(strTemp, dwItemColor.dwGeneral);
				strTemp.Format(" (%d ~ %d)+%d", nMin, nMax, nOpt);
			}
			else
			{
				strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE2));
				pEdit->AddString(strTemp, dwItemColor.dwGeneral);
				strTemp.Format(" %d ~ %d", nMin, nMax);
			}
		}
		else
		{
			int nOpt = 0;
			if (pItemElem->GetAbilityOption() > 0)
			{
				int nAdd = (int)pow((float)(pItemElem->GetAbilityOption()), 1.5f);

				nMin += nAdd;
				nMax += nAdd;
			}
			if (nOpt)
			{
				strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_DEFENSE_A));
				pEdit->AddString(strTemp, dwItemColor.dwGeneral);
				strTemp.Format(" (%d ~ %d)+%d", nMin, nMax, nOpt);
			}
			else
			{
				strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_DEFENSE_B));
				pEdit->AddString(strTemp, dwItemColor.dwGeneral);
				strTemp.Format(" %d ~ %d", nMin, nMax);
			}
		}
		pEdit->AddString(strTemp, dwColorMinMax);
		#ifdef __WEAPON_RARITY
		if (pItemElem->m_nWeaponRarity > 0)
		{
			DWORD dwRarityColor = 0xff199bd5;
			switch (pItemElem->m_nWeaponRarity)
			{
			case 5:
			{
				nMin = (int)(nMin * 0.25f);
				nMax = (int)(nMax * 0.25f);
				dwRarityColor = 0xffb28a33;
			}
			break;
			case 4:
			{
				nMin = (int)(nMin * 0.2f);
				nMax = (int)(nMax * 0.2f);
				dwRarityColor = 0xff8847ff;
			}
			break;
			case 3:
			{
				nMin = (int)(nMin * 0.15f);
				nMax = (int)(nMax * 0.15f);
				dwRarityColor = 0xff4b69ff;
			}
			break;
			case 2:
			{
				nMin = (int)(nMin * 0.1f);
				nMax = (int)(nMax * 0.1f);
				dwRarityColor = 0xff5e98d9;
			}
			break;
			case 1:
			{
				nMin = (int)(nMin * 0.05f);
				nMax = (int)(nMax * 0.05f);
			}
			break;
			}

			if (nMin < 1 && nMax > 0)
				strTemp.Format(" + (1 ~ %d)", nMax);
			else if (nMin > 0 && nMax < 1)
				strTemp.Format(" + (%d ~ 1)", nMin);
			else if (nMin < 1 && nMax < 1)
				strTemp.Format(" + (1 ~ 1)");
			else
				strTemp.Format(" + (%d ~ %d)", nMin, nMax);
			pEdit->AddString(strTemp, dwRarityColor);

			CString strAllStats;
        int nFlatBonus = 0;
        switch (pItemElem->m_nWeaponRarity)
        {
            case 5: nFlatBonus = 50; 
					if(pItemProp->dwItemKind1 == IK1_WEAPON)
					{
						strAllStats.Format("\nIncrease Attack +20%%\nIncrease Speed +20%%\n Increase Spell Cast +20%%\nIncrease Max HP +20%%\nIncrease Max MP +20%%");
						pEdit->AddString(strAllStats, dwRarityColor, ESSTY_BOLD);
					}
					switch(pItemProp->dwItemKind3)
					{
						case IK3_SUIT:
							strAllStats.Format("\nIncrease Max HP +20%%\nIncrease Max MP +20%%\nIncrease Attack +20%%\nIncrease Speed +20%%\nIncrease Spell Cast +20%%\nIncrease Critical Damage +20%%\nIncrease Magic Damage +20%%");
							break;
						case IK3_BOOTS:
							strAllStats.Format("\nIncrease Speed +20%%\nIncrease Critical Damage +20%%\nIncrease Magic Damage +20%%");
							break;
						case IK3_HELMET:
							strAllStats.Format("\nIncrease Max HP +20%%\nIncrease Max MP +20%%");
							break;
						case IK3_GAUNTLET:
							strAllStats.Format("\nIncrease Spell Cast +20%%\nIncrease Attack +20%%");
							break;
						default:
							strAllStats.Empty();
							break;
					}
					pEdit->AddString(strAllStats, dwRarityColor, ESSTY_BOLD);
					break;
            case 4: nFlatBonus = 40; 
					if(pItemProp->dwItemKind1 == IK1_WEAPON)
					{
						strAllStats.Format("\nIncrease Attack +10%%\nIncrease Speed +10%%\n Increase Spell Cast +10%%\nIncrease Max HP +10%%\nIncrease Max MP +10%%");
						pEdit->AddString(strAllStats, dwRarityColor, ESSTY_BOLD);
					}
					switch(pItemProp->dwItemKind3)
					{
						case IK3_SUIT:
							strAllStats.Format("\nIncrease Max HP +10%%\nIncrease Max MP +10%%\nIncrease Attack +10%%\nIncrease Speed +10%%\nIncrease Spell Cast +10%%\nIncrease Critical Damage +10%%\nIncrease Magic Damage +10%%");
							break;
						case IK3_BOOTS:
							strAllStats.Format("\nIncrease Speed +10%%\nIncrease Critical Damage +10%%\nIncrease Magic Damage +10%%");
							break;
						case IK3_HELMET:
							strAllStats.Format("\nIncrease Max HP +10%%\nIncrease Max MP +10%%");
							break;
						case IK3_GAUNTLET:
							strAllStats.Format("\nIncrease Spell Cast +10%%\nIncrease Attack +10%%");
							break;
						default:
							strAllStats.Empty();
							break;
					}
					pEdit->AddString(strAllStats, dwRarityColor, ESSTY_BOLD);	
					break;
            case 3: nFlatBonus = 30; break;
            case 2: nFlatBonus = 20; break;
            case 1: nFlatBonus = 10; break;
        }
        if (nFlatBonus > 0)
        {
            strAllStats.Format("\nAll Stats +%d", nFlatBonus);
            pEdit->AddString(strAllStats, dwRarityColor, ESSTY_BOLD);
        }
		}
#endif // __WEAPON_RARITY
	}
}
void CWndMgr::PutBaseItemOpt(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	PutDestParam(pItemProp, NULL, pEdit);

	CString str;
	CString strTemp;

	CAccessoryProperty* pProperty = CAccessoryProperty::GetInstance();
	if (pItemElem->IsAccessory())
	{
		BOOL bFirstString = TRUE;
		vector<SINGLE_DST>* pDst = pProperty->GetDst(pItemElem->m_dwItemId, pItemElem->GetAbilityOption());
		for (int i = 0; i < (int)(pDst->size()); i++)
		{
			if (bFirstString)
			{
				pEdit->AddString("\nBonus for upgrading:", dwItemColor.dwGeneral);
				bFirstString = FALSE;
			}

			int nDst = (*pDst)[i].nDst;
			int nAdj = (*pDst)[i].nAdj;
			if (IsDst_Rate(nDst))
			{
				if (nDst == DST_ATTACKSPEED)
					strTemp.Format("\n%s %+d%%", FindDstString(nDst), nAdj / 2 / 10);
				else
					strTemp.Format("\n%s %+d%%", FindDstString(nDst), nAdj);
			}
			else
				strTemp.Format("\n%s %+d", FindDstString(nDst), nAdj);

			pEdit->AddString(strTemp, 0xff0000ff);
		}
	}
}

#if __VER >= 9 // __CSC_VER9_1
void CWndMgr::PutAddedOpt(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	CString str;
	CString strTemp;
	int i;
	int nCount = 0;

	int nAddedOptDST[9];
	int nAddedOpt[5];
	int nAddedValue[5];

	for (i = 0; i < 5; i++)
	{
		nAddedOpt[i] = -1;
		nAddedValue[i] = -1;
	}


	nAddedOptDST[0] = DST_HP_MAX;
	nAddedOptDST[1] = DST_ATKPOWER;
	nAddedOptDST[2] = DST_ADJDEF;
	nAddedOptDST[3] = DST_MELEE_STEALHP;
	nAddedOptDST[4] = DST_PVP_DMG;
	nAddedOptDST[5] = DST_STR;
	nAddedOptDST[6] = DST_STA;
	nAddedOptDST[7] = DST_DEX;
	nAddedOptDST[8] = DST_INT;

	map<int, int> mapDst = prj.m_UltimateWeapon.GetDestParamUltimate(pItemElem);

	for (map<int, int>::iterator it = mapDst.begin(); it != mapDst.end(); it++)
	{
		int a = it->first;
		for (i = 0; i < 9; i++)
		{
			if (nAddedOptDST[i] == it->first)
			{
				nAddedOpt[nCount] = i;
				nAddedValue[nCount] = it->second;
				nCount++;
			}
		}
	}

	for (i = 0; i < nCount; i++)
	{

		if (nAddedOpt[i] >= 0)
		{
			if (FindDstString(nAddedOptDST[nAddedOpt[i]]))
			{
				if (nAddedOpt[i] == 3 || nAddedOpt[i] == 4)
					str.Format("\n%s +%d%%", FindDstString(nAddedOptDST[nAddedOpt[i]]), nAddedValue[i]);
				else
					str.Format("\n%s +%d", FindDstString(nAddedOptDST[nAddedOpt[i]]), nAddedValue[i]);
			}

			strTemp = str;
			pEdit->AddString(strTemp, dwItemColor.dwAddedOpt7);
		}
	}
}

void CWndMgr::PutPetInfo(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	CString strTemp;
	pEdit->Empty();
	strTemp = pItemElem->GetName();
	pEdit->AddString(strTemp, dwItemColor.dwName0, ESSTY_BOLD);

	PutPetKind(pItemElem, pItemProp, pEdit);

	if (pItemElem->IsFlag(CItemElem::expired))
	{
		strTemp.Format(" %s", prj.GetText(TID_GAME_PETINFO_DEAD));
		pEdit->AddString(strTemp, dwItemColor.dwResistSM1, ESSTY_BOLD);
	}

	//Level
	int nLevel = PL_EGG;
	if (pItemElem->m_pPet)
	{
		nLevel = pItemElem->m_pPet->GetLevel();

		DWORD dwLevelText;
		switch (nLevel)
		{
		case PL_EGG:
			dwLevelText = TID_GAME_PETGRADE_E;
			break;
		case PL_D:
			dwLevelText = TID_GAME_PETGRADE_D;
			break;
		case PL_C:
			dwLevelText = TID_GAME_PETGRADE_C;
			break;
		case PL_B:
			dwLevelText = TID_GAME_PETGRADE_B;
			break;
		case PL_A:
			dwLevelText = TID_GAME_PETGRADE_A;
			break;
		case PL_S:
			dwLevelText = TID_GAME_PETGRADE_S;
			break;
		}
		strTemp.Format("%s : %s", prj.GetText(TID_GAME_CHARACTER_02), prj.GetText(dwLevelText));
		pEdit->AddString("\n");
		pEdit->AddString(strTemp, dwItemColor.dwResistSM);

		//Ability value
		if (nLevel != PL_EGG)
		{
			DWORD dwDstParam;
			int nParam;
			DWORD dwTooltip;
			pItemElem->m_pPet->GetAvailDestParam(dwDstParam, nParam);

			if (IsDst_Rate(dwDstParam))
			{
				if (dwDstParam == DST_ATTACKSPEED)
					strTemp.Format("\nBonus: %s %+d%%", FindDstString(dwDstParam), nParam / 2 / 10);
				else
					strTemp.Format("\nBonus: %s %+d%%", FindDstString(dwDstParam), nParam);
			}
			else
			{
				strTemp.Format("\nBonus: %s %+d", FindDstString(dwDstParam), nParam);
			}

			pEdit->AddString(strTemp, dwItemColor.dwResistSM1);
		}
		//Level History
		int nLevel = pItemElem->m_pPet->GetLevel();
		if (nLevel > PL_EGG)
		{
			pEdit->AddString("\n");
			pEdit->AddString("(", D3DCOLOR_XRGB(0, 200, 255));
			for (int i = PL_D; i <= nLevel; i++)
			{
				BYTE bLevel = pItemElem->m_pPet->GetAvailLevel(i);
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_LEVEL), bLevel);
				pEdit->AddString(strTemp, D3DCOLOR_XRGB(0, 127, 255));
				if (i != nLevel)
					pEdit->AddString("/", D3DCOLOR_XRGB(0, 200, 255));
			}
			pEdit->AddString(")", D3DCOLOR_XRGB(0, 200, 255));

			//Pet Experience
			EXPINTEGER	nExpResult = pItemElem->m_pPet->GetExp() * (EXPINTEGER)10000 / pItemElem->m_pPet->GetMaxExp();
			float fExp = (float)nExpResult / 100.0f;

			if (fExp >= 99.99f)
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP_MAX));
			else
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP), fExp);
			pEdit->AddString("\n");
			pEdit->AddString(strTemp, D3DCOLOR_XRGB(120, 120, 220));

#ifndef __AIO_PETS
			//Pet Energy
			int nMaxEnergy = pItemElem->m_pPet->GetMaxEnergy();
			int nEnergy = pItemElem->m_pPet->GetEnergy();
			int nLife = pItemElem->m_pPet->GetLife();
			pEdit->AddString("\n");
			strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_LIFE), nLife);
			pEdit->AddString(strTemp, D3DCOLOR_XRGB(255, 100, 100));
			pEdit->AddString("\n");
			strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_HP), nEnergy, nMaxEnergy);
			pEdit->AddString(strTemp, D3DCOLOR_XRGB(255, 10, 10));
#else
			int nLife = pItemElem->m_pPet->GetLife();
			pEdit->AddString("\n");
			strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_LIFE), nLife);
			pEdit->AddString(strTemp, D3DCOLOR_XRGB(255, 100, 100));
#endif
		}
		else
		{
			//Pet Experience
			EXPINTEGER	nExpResult = pItemElem->m_pPet->GetExp() * (EXPINTEGER)10000 / pItemElem->m_pPet->GetMaxExp();
			float fExp = (float)nExpResult / 100.0f;

			if (fExp >= 99.99f)
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP_MAX));
			else
				strTemp.Format(prj.GetText(TID_GAME_PET_TOOLTIP_EXP), fExp);
			pEdit->AddString("\n");
			pEdit->AddString(strTemp, D3DCOLOR_XRGB(120, 120, 220));
		}

		//Description
		strTemp.Format("%s", pItemProp->szCommand);
		pEdit->AddString("\n");
		pEdit->AddString(strTemp, dwItemColor.dwPiercing);
	}
}

void CWndMgr::PutPetFeedPocket(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	CString strTemp, str;
	pEdit->Empty();

	if (pItemElem->m_dwKeepTime > 0)
	{
		//Name
		strTemp.Format("%s", pItemProp->szName);
		pEdit->AddString(strTemp, dwItemColor.dwName0, ESSTY_BOLD);

		if (g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET))
		{
			strTemp.Format(" %s", prj.GetText(TID_GAME_POCKETUSING));
			pEdit->AddString(strTemp, D3DCOLOR_XRGB(255, 0, 0));


			time_t t = pItemElem->m_dwKeepTime - time_null();
			if (pItemElem->m_dwKeepTime && !pItemElem->IsFlag(CItemElem::expired))
			{
				if (t > 0)
				{
					CTimeSpan time(t);
					if (time.GetDays())
						str.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays() + 1));
					else if (time.GetHours())
						str.Format(prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours());
					else if (time.GetMinutes())
						str.Format(prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes());
					else
						str.Format(prj.GetText(TID_PK_LIMIT_SECOND), time.GetSeconds());
				}
				strTemp = str;
				pEdit->AddString("\n");
				pEdit->AddString(strTemp, D3DCOLOR_XRGB(255, 20, 20));
			}


			pEdit->AddString("\n");
			strTemp.Format("%s %d", prj.GetText(TID_GAME_PET_FEED_COUNT), g_pPlayer->m_Inventory.GetItemCount(II_SYS_SYS_FEED_01));
			pEdit->AddString(strTemp, D3DCOLOR_XRGB(50, 50, 205));

		}
		else
		{
			strTemp.Format(" %s", prj.GetText(TID_GAME_POCKET_NOTUSING));
			pEdit->AddString(strTemp, D3DCOLOR_XRGB(255, 0, 0));


			time_t t = pItemElem->m_dwKeepTime - time_null();
			if (pItemElem->m_dwKeepTime && !pItemElem->IsFlag(CItemElem::expired))
			{
				if (t > 0)
				{
					CTimeSpan time(t);
					if (time.GetDays())
						str.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays() + 1));
					else if (time.GetHours())
						str.Format(prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours());
					else if (time.GetMinutes())
						str.Format(prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes());
					else
						str.Format(prj.GetText(TID_PK_LIMIT_SECOND), time.GetSeconds());
				}
				strTemp = str;
				pEdit->AddString("\n");
				pEdit->AddString(strTemp, dwItemColor.dwTime);
			}
		}

		//Description
		strTemp.Format("%s", prj.GetText(TID_GAME_PET_FEEDPOCKET));
		pEdit->AddString("\n");
		pEdit->AddString(strTemp, dwItemColor.dwPiercing);
	}
	else
	{
		//Name
		strTemp.Format("%s", pItemProp->szName);
		pEdit->AddString(strTemp, dwItemColor.dwName0, ESSTY_BOLD);
		//Description
		strTemp.Format("%s", prj.GetText(TID_GAME_PET_FEEDPOCKET_USE));
		pEdit->AddString("\n");
		pEdit->AddString(strTemp, dwItemColor.dwPiercing);
	}
}
#endif //__CSC_VER9_1

void CWndMgr::PutItemGold(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit, int flag)
{
	CString str;
	CString strTemp;
	if (flag == APP_SHOP_)
	{
		if (pItemElem->GetCost() != 0xffffffff)
		{
			CWndShop* pWndShop = (CWndShop*)GetWndBase(APP_SHOP_);
			LPCHARACTER lpCharacter = pWndShop->m_pMover->GetCharacter();
			int nCost = 0;
			int nBeforeTax = 0;
			if (lpCharacter)
			{
				if (lpCharacter->m_nVenderType == 0)
				{
					nCost = pItemElem->GetCost();
#ifdef __SHOP_COST_RATE
					nCost = (int)(prj.m_fShopBuyRate * nCost);
#endif // __SHOP_COST_RATE
					nBeforeTax = nCost;
					if (CTax::GetInstance()->IsApplyTaxRate(g_pPlayer, pItemElem))
						nCost += (static_cast<int>(nCost * CTax::GetInstance()->GetPurchaseTaxRate(g_pPlayer)));
				}
#ifndef __NPC_SETVENDORTYPE
				else if (lpCharacter->m_nVenderType == 1)
					nCost = pItemElem->GetChipCost();
#else
				else
					nCost = pItemElem->GetChipCost();
#endif
			}
			nCost = (int)(nCost * prj.m_fShopCost);
			if (pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN)
				nCost = PERIN_VALUE;

			if (nCost <= 0)
			{
				nCost = 1;
				nBeforeTax = 1;
			}

			char szbuffer[1024];
			_itoa(nCost, szbuffer, 10);
			str = GetNumberFormatEx(szbuffer);
			strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_COST2), str);

#ifndef __NPC_SETVENDORTYPE
			if (lpCharacter && lpCharacter->m_nVenderType == 1)
			{
				CString temp;
				temp.Format(" %s", prj.GetText(TID_GAME_REDCHIP));
				strTemp += temp;
			}
#else
			if (lpCharacter && lpCharacter->m_nVenderType != 0)
			{
				CString temp;
				temp.Format(" %s", prj.GetItemProp(lpCharacter->m_nVenderType)->szName);
				strTemp += temp;
			}
#endif

			pEdit->AddString("\n");
			pEdit->AddString(strTemp, dwItemColor.dwGold);
			if (CTax::GetInstance()->IsApplyTaxRate(g_pPlayer, pItemElem)
				&& lpCharacter->m_nVenderType == 0
				)
			{
				_itoa(nBeforeTax, szbuffer, 10);
				str = GetNumberFormatEx(szbuffer);
				strTemp.Format(prj.GetText(TID_GAME_TAX_RATE), str);
				pEdit->AddString("\n");
				pEdit->AddString(strTemp, prj.GetTextColor(TID_GAME_TAX_RATE));
			}
		}
	}
	else if (flag == APP_INVENTORY)
	{
		CWndShop* pWndShop = (CWndShop*)GetWndBase(APP_SHOP_);
		if (pWndShop)
		{
			if (pItemElem->GetCost() != 0xffffffff)
			{
				char szbuffer[1024];
				DWORD dwCostTem = pItemElem->GetCost() / 4;
				if (pItemElem->IsOnePenyaPrice())
					dwCostTem = 1;

#ifdef __SHOP_COST_RATE
				dwCostTem = (DWORD)(prj.m_fShopSellRate * dwCostTem);
#endif // __SHOP_COST_RATE
				DWORD dwBeforeTax = dwCostTem;
				if (CTax::GetInstance()->IsApplyTaxRate(g_pPlayer, pItemElem))
					dwCostTem -= (static_cast<DWORD>(dwCostTem * CTax::GetInstance()->GetSalesTaxRate(g_pPlayer)));

				if (dwCostTem < 1)
				{
					dwCostTem = 1;
#if __VER >= 12 // __TAX
					dwBeforeTax = 1;
#endif // __TAX
				}

				_itoa(dwCostTem, szbuffer, 10);
				str = GetNumberFormatEx(szbuffer);
				strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str);
				pEdit->AddString("\n");
				pEdit->AddString(strTemp, dwItemColor.dwGold);
#if __VER >= 12 // __TAX
				if (CTax::GetInstance()->IsApplyTaxRate(g_pPlayer, pItemElem))
				{
					_itoa(dwBeforeTax, szbuffer, 10);
					str = GetNumberFormatEx(szbuffer);
					strTemp.Format(prj.GetText(TID_GAME_TAX_RATE), str);
					pEdit->AddString("\n");
					pEdit->AddString(strTemp, prj.GetTextColor(TID_GAME_TAX_RATE));
				}
#endif // __TAX
			}
		}
	}
	else if (flag == APP_VENDOR_REVISION)
	{
		if (pItemElem->m_nCost != 0xffffffff)
		{
			char szbuffer[1024];
			DWORD dwCostTem = 0;
			dwCostTem = pItemElem->m_nCost;
			_itoa(dwCostTem, szbuffer, 10);
			str = GetNumberFormatEx(szbuffer);

			strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str);
			pEdit->AddString("\n");
			pEdit->AddString(strTemp, dwItemColor.dwGold);
		}
#ifdef __FL_VENDORS
		if (pItemElem->m_nPerinCost != 0xffffffff)
		{
			char szbuffer[1024];
			DWORD dwCostTem = 0;
			dwCostTem = pItemElem->m_nPerinCost;
			_itoa(dwCostTem, szbuffer, 10);
			str = GetNumberFormatEx(szbuffer);

			strTemp.Format(("Perins: %s"), str);
			pEdit->AddString("\n");
			pEdit->AddString(strTemp, dwItemColor.dwGold);
		}
#endif
	}
}

void CWndMgr::PutCoolTime(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	CString strTemp;
	DWORD dwGroup = pMover->m_cooltimeMgr.GetGroup(pItemProp);
	if (dwGroup)
	{
		DWORD dwCur = g_tmCurrent;
		DWORD dwEnd = pMover->m_cooltimeMgr.GetTime(dwGroup);
		if (dwEnd != 0 && dwEnd > dwCur)
		{
			DWORD dwBase = pMover->m_cooltimeMgr.GetBase(dwGroup);

			CTimeSpan ct((dwEnd - dwCur + 500) / 1000);
			strTemp.Format(prj.GetText(TID_TOOLTIP_COOLTIME), ct.GetMinutes(), ct.GetSeconds());
			pEdit->AddString("\n");
			pEdit->AddString(strTemp, dwItemColor.dwGeneral);
		}
	}
}

void CWndMgr::PutKeepTime(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	if (pItemProp->IsVis())
		return;

#ifdef __FL_VENDORS
	if (pItemProp->dwItemKind3 == IK3_VENDOR)
		return;
#endif

	CString str, strTemp;
	if (pItemProp->dwCircleTime != 0xffffffff)
	{
		pEdit->AddString("\n");
		if (pItemProp->dwCircleTime == 1)
		{
			pEdit->AddString(prj.GetText(TID_GAME_COND_USE), dwItemColor.dwTime);
		}
		else
		{
			CTimeSpan ct(pItemProp->dwCircleTime);
			strTemp.Format("%s : ", prj.GetText(TID_TOOLTIP_ITEMTIME));
			pEdit->AddString(strTemp, dwItemColor.dwTime);
			strTemp.Format(prj.GetText(TID_TOOLTIP_DATE), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
			pEdit->AddString(strTemp, dwItemColor.dwTime);
		}
	}

	time_t t = pItemElem->m_dwKeepTime - time_null();
	if (pItemElem->m_dwKeepTime)
	{
		if (!pItemElem->IsFlag(CItemElem::expired))
		{
			if (t > 0)
			{
				CTimeSpan time(t);
				str.Format(prj.GetText(TID_TOOLTIP_DATE), static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes(), time.GetSeconds());
			}
			strTemp = str;
			pEdit->AddString("\n");
			pEdit->AddString(strTemp, dwItemColor.dwTime);
		}
		else
		{
			pEdit->AddString("\n");
			strTemp.Format(prj.GetText(TID_GAME_ITEM_EXPIRED));
			pEdit->AddString(strTemp, 0xfff02222);
		}
	}
}
void CWndMgr::PutSex(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	if (pItemProp->dwItemSex != 0xffffffff)
	{
		CString strTemp;
		if (pItemProp->dwItemSex == 0)
			strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_SEXMALE));
		else
			strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_SEXFEMALE));
		pEdit->AddString("\n");
		if (pItemProp->dwItemSex == pMover->GetSex())
			pEdit->AddString(strTemp, dwItemColor.dwGeneral);
		else
			pEdit->AddString(strTemp, dwItemColor.dwNotUse);
	}
}

void CWndMgr::PutJob(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	if (pItemProp->dwItemJob != 0xffffffff)
	{
		CString strTemp;
		strTemp.Format("Req job: %s", prj.m_aJob[pItemProp->dwItemJob].szName);

		pEdit->AddString("\n");
		if (pMover->IsInteriorityJob(pItemProp->dwItemJob))
			pEdit->AddString(strTemp, dwItemColor.dwGeneral);
		else
			pEdit->AddString(strTemp, dwItemColor.dwNotUse);
	}
}

void CWndMgr::PutLevel(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	if (pItemProp->dwLimitLevel1 != 0xffffffff)
	{
		CString strTemp;
		strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_REQLEVEL), pItemProp->dwLimitLevel1);
		pEdit->AddString("\n");

		if (pItemElem->IsLimitLevel(pMover))
			pEdit->AddString(strTemp, dwItemColor.dwNotUse);
		else
			pEdit->AddString(strTemp, dwItemColor.dwGeneral);

		if (pItemElem->GetLevelDown() != 0)
		{
			strTemp.Format("(%d)", pItemElem->GetLevelDown());
			pEdit->AddString(strTemp, dwItemColor.dwNotUse);
		}
	}
}

void CWndMgr::PutWeapon(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	if (pItemProp->dwItemKind3 != IK3_SHIELD
#if __VER >= 16
		&& pItemProp->dwItemKind3 != IK3_MAGICBARUNA
		&& pItemProp->dwItemKind3 != IK3_ZEMBARUNA
#endif
		)
	{
		CString strTemp;
		if (pItemProp->dwHanded != 0xffffffff)
		{
			if (HD_ONE == pItemProp->dwHanded)
			{
				strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_ONEHANDWEAPON));
			}
			else if (HD_TWO == pItemProp->dwHanded)
			{
				strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_TWOHANDWEAPON));
			}
			pEdit->AddString("\n");
			pEdit->AddString(strTemp, dwItemColor.dwGeneral);
		}

		if (pItemProp->dwActiveSkill != NULL_ID && pItemProp->dwActiveSkillRate != NULL_ID)
		{
			ItemProp* pSkillProp = prj.GetSkillProp(pItemProp->dwActiveSkill);
			if (pSkillProp)
			{
				strTemp.Format("Skill Chance: %s (%d%%)", pSkillProp->szName, pItemProp->dwActiveSkillRate);
				pEdit->AddString("\n");
				pEdit->AddString(strTemp, dwItemColor.dwGeneral);
			}
		}
	}
}

void CWndMgr::PutCommand(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit, int nFlag)
{
	CString strTemp;

	BOOL bPutDescription = TRUE;
	if (pItemProp->dwItemKind2 == IK2_BUFF || pItemProp->dwItemKind2 == IK2_BUFF2 || pItemProp->dwItemKind2 == IK2_BUFF_TOGIFT || pItemProp->dwItemKind3 == IK3_BALLOON)
	{
		if (PutDestParam(pItemProp, NULL, pEdit))
			bPutDescription = FALSE;
	}
	else if (pItemProp->dwItemKind2 == IK2_REFRESHER || pItemProp->dwItemKind2 == IK2_FOOD || pItemProp->dwItemKind2 == IK2_POTION)
	{
		if (PutMedicine(pItemProp, pEdit))
			bPutDescription = FALSE;
	}
	else if (pItemProp->dwItemKind3 == IK3_SOCKETCARD || pItemProp->dwItemKind3 == IK3_SOCKETCARD2)
	{
		if (PutSocketCard(pItemProp, pEdit))
			bPutDescription = FALSE;
	}
	else if (pItemProp->dwItemKind3 == IK3_VIS)
	{
		if (PutBeadInfo(pItemElem, pItemProp, pEdit))
			bPutDescription = FALSE;
	}
	else if (pItemProp->dwItemKind2 == IK2_FURNITURE || pItemProp->dwItemKind2 == IK2_GUILDHOUSE_FURNITURE || pItemProp->dwItemKind2 == IK2_GUILDHOUSE_PAPERING)
	{
		if (PutFurniture(pItemProp, pEdit))
			bPutDescription = FALSE;
	}
#ifdef __FL_VENDORS
	else if (pItemProp->dwItemKind3 == IK3_VENDOR)
	{
		if (PutVendorItem(pItemElem, pItemProp, pEdit))
			bPutDescription = FALSE;
	}
#endif
#ifdef __FL_BOXES_V2
	else if (pItemProp->nBoxType != BOX_TYPE_NOTBOX)
	{
		PutBoxDetails(pItemProp, pEdit, nFlag);
		bPutDescription = FALSE;
	}
#endif
#ifdef __FL_PACKED_ITEMS
	else if (pItemElem->IsPackage())
	{
		PutPackage(pItemElem, pItemProp, pEdit, nFlag);
		bPutDescription = FALSE;
	}
#endif
#ifdef __FL_MONSTER_ALBUM
	else if (pItemProp->dwItemKind3 == IK3_MONSTER_ALBUM)
	{
		PutMonsterCard(pItemProp, pEdit);
		bPutDescription = FALSE;
	}
	else if (pItemProp->dwItemKind3 == IK3_MONSTER_ALBUM_BOSS_CARD)
	{
		PutBossCard(pItemProp, pEdit);
		bPutDescription = FALSE;
	}
#endif

	if (bPutDescription && strlen(pItemProp->szCommand))
	{
		pEdit->AddString("\n");

		if ((pItemElem->m_dwItemId == II_GEN_WARP_COUPLERING || pItemElem->m_dwItemId == II_GEN_WARP_WEDDING_BAND || pItemElem->m_dwItemId == II_GEN_WARP_COUPLERING01)
			&& pItemElem->GetRandomOptItemId() > 0)
		{
			u_long idPlayer = (u_long)(pItemElem->GetRandomOptItemId());
			const char* pszPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer);
			CString strDesc;
			strDesc.Format(prj.GetText(TID_ITEM_COUPLERING_DESC), pszPlayer ? pszPlayer : "");
			strTemp.Format(prj.GetText(TID_ITEM_INFO), strDesc);
		}
		else
			strTemp.Format(prj.GetText(TID_ITEM_INFO), pItemProp->szCommand);

		pEdit->AddString(strTemp, dwItemColor.dwCommand);
	}
}

void CWndMgr::PutToolTip_Item(CItemElem* pItemBase, CPoint point, CRect* pRect, int flag)
{
	if (!pRect)
		return;

	if (!pItemBase)
		return;

	ItemProp* pItemProp = pItemBase->GetProp();
	if (!pItemProp)
		return;

	CEditString strItem = _T("");
	MakeToolTipText(pItemBase, pItemProp, strItem, flag);
	g_toolTip.PutToolTip(pItemBase->m_dwItemId, strItem, *pRect, point, 0);
	if (pItemProp->IsUltimate())
		g_toolTip.SetUltimateToolTip(pItemBase);
#ifdef __FL_PACKED_ITEMS
	if (pItemBase->IsPackage())
		g_toolTip.SetPackageToolTip(pItemBase);
#endif
}

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
void CWndMgr::MakeToolTipText(CItemElem* pItemBase, ItemProp* pItemProp, CEditString& strEdit, int flag)
{
	CMover* pMover = g_pPlayer;
	if (flag == APP_QUERYEQUIP)
	{
		CWndQueryEquip* pWndQueryEquip = (CWndQueryEquip*)GetWndBase(APP_QUERYEQUIP);
		if (pWndQueryEquip)
			pMover = pWndQueryEquip->GetMover();
	}

	if (!pMover)
		return;

	CItemElem* pItemElem = pItemBase;

	CString str = _T("");
	CString strTemp = _T("");
	CString strEnter = '\n';

	DWORD dwColorBuf;
	dwColorBuf = PutItemName(pItemElem, pItemProp, &strEdit);
	PutItemAbilityPiercing(pItemElem, pItemProp, &strEdit, dwColorBuf);
#ifdef __MODEL_CHANGE
	PutLookText(pItemElem, pItemProp, &strEdit);
#endif
	PutPetKind(pItemElem, pItemProp, &strEdit);
	if (pItemProp->dwFlag & IP_FLAG_EQUIP_BIND)
	{
		strEdit.AddString("\n");
		if (pItemElem->IsFlag(CItemElem::binds))
			strEdit.AddString(GETTEXT(TID_TOOLTIP_EQUIPBIND_AFTER), GETTEXTCOLOR(TID_TOOLTIP_EQUIPBIND_AFTER));
		else
			strEdit.AddString(GETTEXT(TID_TOOLTIP_EQUIPBIND_BEFORE), GETTEXTCOLOR(TID_TOOLTIP_EQUIPBIND_BEFORE), ESSTY_BOLD);
	}
	PutWeapon(pItemElem, pItemProp, &strEdit);
	PutSex(pMover, pItemElem, pItemProp, &strEdit);

	switch (pItemProp->dwItemKind2)
	{
	case IK2_WEAPON_DIRECT:
	case IK2_WEAPON_MAGIC:
	case IK2_ARMORETC:
	case IK2_CLOTHETC:
	case IK2_ARMOR:
	case IK2_CLOTH:
	case IK2_BLINKWING:
	{
		PutItemMinMax(pMover, pItemElem, pItemProp, &strEdit);
		PutItemSpeed(pItemElem, pItemProp, &strEdit);
		if (pItemProp->dwItemKind3 == IK3_ELECARD)
			PutItemResist(pItemElem, pItemProp, &strEdit);
		else if (pItemElem->m_nResistAbilityOption && pItemProp->IsEleRefineryAble())
			PutItemResist(pItemElem, pItemProp, &strEdit);

		PutDestParam(pItemProp, NULL, &strEdit);

		PutRandomOpt(pItemElem, pItemProp, &strEdit);
		PutEnchantOpt(pMover, pItemElem, pItemProp, &strEdit, flag);
		break;
	}
	case IK2_JEWELRY:
	case IK2_CHARM:
	case IK2_BULLET:
	{
		PutBaseItemOpt(pItemElem, pItemProp, &strEdit);
		break;
	}
	case IK2_GMTEXT:
	{
		if (pItemProp->dwItemKind3 == IK3_TEXT_DISGUISE)
		{
			PutDestParam(pItemProp, NULL, &strEdit);
			if (pItemProp->dwActiveSkill != NULL_ID)
			{
				ItemProp* pSkillProp;
				AddSkillProp* pAddSkillProp;
				pMover->GetSkillProp(&pSkillProp, &pAddSkillProp, pItemProp->dwActiveSkill, 1, "CWndMgr::PutToolTip_Item");

				if (pAddSkillProp)
				{
					PutDestParam(NULL, pAddSkillProp, &strEdit);
				}
			}
		}
		break;
	}
	default:
	{
		break;
	}
	}

	if (pItemElem->IsEatPet())
	{
		PutBaseItemOpt(pItemElem, pItemProp, &strEdit);

		if (pItemProp->dwActiveSkill != NULL_ID)
		{
			ItemProp* pSkillProp;
			AddSkillProp* pAddSkillProp;
			pMover->GetSkillProp(&pSkillProp, &pAddSkillProp, pItemProp->dwActiveSkill, 1, "CWndMgr::PutToolTip_Item");

			if (pAddSkillProp)
				PutDestParam(NULL, pAddSkillProp, &strEdit);
		}
	}

	if (pItemElem->IsVisPet())
		PutVisPetInfo(pItemElem, pItemProp, &strEdit);

	PutCoolTime(pMover, pItemElem, pItemProp, &strEdit);

	PutKeepTime(pItemElem, pItemProp, &strEdit);

	PutJob(pMover, pItemElem, pItemProp, &strEdit);
	PutLevel(pMover, pItemElem, pItemProp, &strEdit);
	PutCommand(pItemElem, pItemProp, &strEdit, flag);
#ifdef __MODEL_VIEW
	PutModelView(pItemProp, &strEdit, flag);
#endif
	PutItemGold(pMover, pItemElem, pItemProp, &strEdit, flag);
	PutSetItemOpt(pMover, pItemElem, pItemProp, &strEdit);
	if (pItemProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet)
		PutPetInfo(pItemElem, pItemProp, &strEdit);
	if (pItemProp->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET)
		PutPetFeedPocket(pItemElem, pItemProp, &strEdit);
	PutPiercingOpt(pItemElem, pItemProp, &strEdit);
	PutAwakeningBlessing(pItemElem, pItemProp, &strEdit);
	if ((pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) &&
		pItemProp->dwReferStat1 == WEAPON_ULTIMATE)
		PutAddedOpt(pItemElem, pItemProp, &strEdit);
}
#endif // __IMPROVE_SYSTEM_VER15

void CWndMgr::PutToolTip_Character(int SelectCharacter, CPoint point, CRect* pRect)
{
	CString string;
	char statebuf[30] = { 0, };

	if (g_Neuz.m_nCharacterBlock[SelectCharacter])
		string.Format("#b#cff0000ff%s#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName());
	else
		string.Format("#b#cffff0000%s\nConnet Cut#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName());

	if (g_Neuz.m_apPlayer[SelectCharacter]->GetSex())
		sprintf(statebuf, prj.GetText(TID_GAME_TOOLTIP_SEXFEMALE));
	else
		sprintf(statebuf, prj.GetText(TID_GAME_TOOLTIP_SEXMALE));
	string += '\n'; string += statebuf;


	if (g_Neuz.m_apPlayer[SelectCharacter]->IsMaster())
	{
		sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel);
		string += '\n'; string += statebuf;
		string += prj.GetText(TID_GAME_TOOLTIP_MARK_MASTER);
	}
	else if (g_Neuz.m_apPlayer[SelectCharacter]->IsHero() || g_Neuz.m_apPlayer[SelectCharacter]->IsLegendHero())
	{
		sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel);
		string += '\n'; string += statebuf;
		string += prj.GetText(TID_GAME_TOOLTIP_MARK_HERO);
	}
	else
	{
		sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel);
		string += '\n'; string += statebuf;
	}

	// Str
	sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_STR), g_Neuz.m_apPlayer[SelectCharacter]->m_nStr);
	string += '\n'; string += statebuf;

	// Sta
	sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_STA), g_Neuz.m_apPlayer[SelectCharacter]->m_nSta);
	string += '\n'; string += statebuf;

	// Dex
	sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_DEX), g_Neuz.m_apPlayer[SelectCharacter]->m_nDex);
	string += '\n'; string += statebuf;

	// Int
	sprintf(statebuf, prj.GetText(TID_GAME_TOOL_EX_INT), g_Neuz.m_apPlayer[SelectCharacter]->m_nInt);
	string += '\n'; string += statebuf;


	sprintf(statebuf, "%s : %s", prj.GetText(TID_APP_CHARACTER_JOB), prj.m_aJob[g_Neuz.m_apPlayer[SelectCharacter]->m_nJob].szName);
	string += '\n'; string += statebuf;

	CEditString strEdit;
	strEdit.SetParsingString(string);
	g_toolTip.PutToolTip(SelectCharacter, strEdit, *pRect, point, 3);
}


void CWndMgr::UseSkillShortCut(DWORD dwType, DWORD dwSkillIdx)
{
	LPSKILL lpSkill = g_pPlayer->GetSkill(dwType, dwSkillIdx);
	if (lpSkill)
	{
		CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
		DWORD dwObjId = NULL_ID;
		if (pCtrl && pCtrl->GetType() != OT_OBJ)
			dwObjId = pCtrl->GetId();

		{

			if (((CWndTaskBar*)m_pWndTaskBar)->m_nExecute == 0 && (pCtrl == NULL || (pCtrl && pCtrl->GetType() == OT_MOVER)))
			{

				//					((CWndTaskBar *)m_pWndTaskBar)->OnCancelSkill();
				int nResult = 0;

				if (g_pPlayer->IsBullet(lpSkill->GetProp()) == FALSE)
				{
					return;
				}

				nResult = g_pPlayer->CMD_SetUseSkill(dwObjId, dwSkillIdx);

				if (nResult == 0)
				{
					m_pWndWorld->SetNextSkill(NEXTSKILL_NONE);
				}

				if (nResult)
				{
					m_pWndWorld->SetNextSkill(NEXTSKILL_NONE);
				}
				else

					if (g_pPlayer->m_pActMover->IsActAttack())
					{

						m_pWndWorld->SetNextSkill(dwSkillIdx);
					}
			}

		}
	}
}

BOOL CWndMgr::ScreenCapture()
{
	static long	bmpcnt = 0;
	char	filename[_MAX_PATH] = "";
	FILE* fp;

	{
		CreateDirectory("Capture", NULL);
		while (1)
		{
			sprintf(filename, "Capture\\flyff%05d.jpg", bmpcnt);
			if ((fp = fopen(filename, "r")) == NULL) break; else fclose(fp);
			bmpcnt++;
		}
	}
	sprintf(filename, "Capture\\flyff%05d.jpg", bmpcnt);
	SaveJPG(filename);
	CString string;
	string.Format(prj.GetText(TID_ADMIN_CAPTUREJPG), bmpcnt++);
	PutString(string, NULL, prj.GetTextColor(TID_ADMIN_CAPTUREJPG));

	return TRUE;
}


BOOL CWndMgr::SaveJPG(LPCTSTR lpszName)
{
	IDirect3DSurface9* pDestSurface;

	CTime ctTime = CTime::GetCurrentTime();
	char lpString[260] = { 0, };
	sprintf(lpString, prj.GetText(TID_GAME_SCREENSHOT_TIME), ctTime.GetYear(), ctTime.GetMonth(), ctTime.GetDay(), ctTime.GetHour(), ctTime.GetMinute(), ctTime.GetSecond());
	CRect rect = g_Neuz.GetDeviceRect();
	CSize size = m_p2DRender->m_pFont->GetTextExtent(lpString);

	CRect bg;
	bg.left = rect.right - size.cx - 30;
	bg.top = rect.bottom - 80;
	bg.right = rect.right - 10;
	bg.bottom = bg.top + size.cy + 20;
	g_Neuz.m_2DRender.RenderFillRect(bg, D3DCOLOR_ARGB(140, 0, 0, 0));

	g_Neuz.m_2DRender.TextOut(bg.left + 10, bg.top + 10, lpString);

	m_pApp->m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface);
	BOOL bResult = D3DXSaveSurfaceToFile(lpszName, D3DXIFF_JPG, pDestSurface, NULL, NULL);
	pDestSurface->Release();

	return bResult;
}

CCollectingWnd* CWndMgr::OpenCollecting()
{

	if (!m_pWndCollecting)
	{
		m_pWndCollecting = new CCollectingWnd;
		m_pWndCollecting->Initialize();
	}

	return m_pWndCollecting;
}


BOOL CWndMgr::CloseCollecting()
{

	if (m_pWndCollecting)
	{
		m_pWndCollecting->Destroy();
		SAFE_DELETE(m_pWndCollecting);
		m_pWndCollecting = NULL;
		return TRUE;
	}

	return FALSE;
}


CWndMessage* CWndMgr::GetMessagge(LPCTSTR lpszFrom)
{
	CString string;
	CWndMessage* pWndMessage = NULL;
	m_mapMessage.Lookup(lpszFrom, (void*&)pWndMessage);
	return pWndMessage;
}
CWndMessage* CWndMgr::OpenMessage(LPCTSTR lpszFrom)
{
	CString string;
	CWndMessage* pWndMessage = GetMessagge(lpszFrom);
	if (!pWndMessage)
	{
		pWndMessage = new CWndMessage;
		pWndMessage->Initialize();
		m_mapMessage.SetAt(lpszFrom, pWndMessage);
		string = pWndMessage->GetTitle();
		string += " - ";
		string += lpszFrom;
		pWndMessage->SetTitle(string);
		pWndMessage->m_strPlayer = lpszFrom;

#ifdef __FL_SAVE_MESSAGES
		if (g_pPlayer)
		{
			auto i = g_Neuz.m_mapPrivateMessages.find(lpszFrom);
			if (i != g_Neuz.m_mapPrivateMessages.end())
			{
				for (auto& it : i->second)
					pWndMessage->AddMessage(it.bIsFromMe ? g_pPlayer->GetName() : lpszFrom, it.str, it.cTime);
			}
		}
#endif

	}
	else
		pWndMessage->InitSize();

	return pWndMessage;
}


BOOL CWndMgr::UpdateMessage(LPCTSTR pszOld, LPCTSTR pszNew)
{
	CWndMessage* pWndMessage = GetMessagge(pszOld);
	if (pWndMessage)
	{
		m_mapMessage.RemoveKey(pWndMessage->m_strPlayer);
		m_mapMessage.SetAt(pszNew, pWndMessage);
		CString string = pWndMessage->GetTitle();
		string.Replace(pszOld, pszNew);
		pWndMessage->SetTitle(string);
		pWndMessage->m_strPlayer = pszNew;
		return TRUE;
	}
	return FALSE;
}

CWndInstantMsg* CWndMgr::GetInstantMsg(LPCTSTR lpszFrom)
{
	CString string;
	CWndInstantMsg* pWndMessage = NULL;
	m_mapInstantMsg.Lookup(lpszFrom, (void*&)pWndMessage);
	return pWndMessage;
}

CWndInstantMsg* CWndMgr::OpenInstantMsg(LPCTSTR lpszFrom)
{
	CString string;
	CWndInstantMsg* pWndMessage = GetInstantMsg(lpszFrom);
	if (!pWndMessage)
	{
		CWndBase* pWndFocus = GetFocusWnd();
		pWndMessage = new CWndInstantMsg;
		pWndMessage->Initialize();
		pWndFocus->SetFocus();
		pWndMessage->m_strPlayer = lpszFrom;
		int nNumber = m_mapInstantMsg.GetCount();
		CRect rcMsg = pWndMessage->GetWindowRect();
		CRect rcWnd = GetLayoutRect();

		int nNumHeight = rcWnd.Height() / rcMsg.Height();
		int nNumWidth = rcWnd.Width() / rcMsg.Width();

		int nCount = (nNumber / nNumHeight) + 1;
		int nCount2 = (nNumber % nNumHeight) + 1;

		CPoint pt(rcWnd.right - (rcMsg.Width() * nCount), rcWnd.bottom - (rcMsg.Height() * nCount2));
		pWndMessage->Move(pt);
		m_mapInstantMsg.SetAt(lpszFrom, pWndMessage);

	}
	return pWndMessage;
}

LPWNDREGINFO CWndMgr::GetRegInfo(DWORD dwWndId)
{
	LPWNDREGINFO pWndRegInfo = NULL;
	m_mapWndRegInfo.Lookup(dwWndId, (void*&)pWndRegInfo);
	return pWndRegInfo;
}

BOOL CWndMgr::PutRegInfo(CWndNeuz* pWndNeuz, BOOL bOpen)
{
	LPWNDREGINFO pWndRegInfo = NULL;
	BOOL bLookup = FALSE;
	if (m_mapWndRegInfo.Lookup(pWndNeuz->GetWndId(), (void*&)pWndRegInfo) == TRUE)
	{
		SAFE_DELETE(pWndRegInfo->lpArchive);
		pWndRegInfo->dwSize = 0;
		bLookup = TRUE;
	}
	else
		pWndRegInfo = new WNDREGINFO;
	pWndRegInfo->dwWndId = pWndNeuz->GetWndId();
	pWndRegInfo->rect = pWndNeuz->GetWindowRect(TRUE);
	pWndRegInfo->bOpen = bOpen;
	pWndRegInfo->dwWndSize = pWndNeuz->m_nWinSize;
	pWndRegInfo->dwVersion = 0;
	CAr ar;
	int nSize;
	LPBYTE lpData;
	// Write
	pWndNeuz->SerializeRegInfo(ar, pWndRegInfo->dwVersion);
	lpData = ar.GetBuffer(&nSize);
	if (nSize)
	{
		pWndRegInfo->lpArchive = new BYTE[nSize];
		pWndRegInfo->dwSize = nSize;
		memcpy(pWndRegInfo->lpArchive, lpData, nSize);
	}
	else
	{
		pWndRegInfo->lpArchive = NULL;
		pWndRegInfo->dwSize = 0;
	}
	if (bLookup == FALSE)
		m_mapWndRegInfo.SetAt(pWndNeuz->GetWndId(), pWndRegInfo);
	return TRUE;
}

BOOL CWndMgr::PutRegInfo(LPWNDREGINFO lpRegInfo)
{
	LPWNDREGINFO pWndRegInfo = NULL;
	if (m_mapWndRegInfo.Lookup(lpRegInfo->dwWndId, (void*&)pWndRegInfo) == TRUE)
	{

		SAFE_DELETE(pWndRegInfo->lpArchive);
		memcpy(pWndRegInfo, lpRegInfo, sizeof(WNDREGINFO));
		if (lpRegInfo->dwSize)
		{
			pWndRegInfo->lpArchive = new BYTE[lpRegInfo->dwSize];
			memcpy(pWndRegInfo->lpArchive, lpRegInfo->lpArchive, lpRegInfo->dwSize);
		}
		else
			pWndRegInfo->lpArchive = NULL;
		return FALSE;
	}
	pWndRegInfo = new WNDREGINFO;
	memcpy(pWndRegInfo, lpRegInfo, sizeof(WNDREGINFO));
	if (lpRegInfo->dwSize)
	{
		pWndRegInfo->lpArchive = new BYTE[lpRegInfo->dwSize];
		memcpy(pWndRegInfo->lpArchive, lpRegInfo->lpArchive, lpRegInfo->dwSize);
	}
	else
		pWndRegInfo->lpArchive = NULL;
	m_mapWndRegInfo.SetAt(lpRegInfo->dwWndId, pWndRegInfo);
	return TRUE;
}
BOOL CWndMgr::SaveRegInfo(LPCTSTR lpszFileName)
{
	CFileIO file;
	if (file.Open(lpszFileName, "wb") == FALSE)
		return FALSE;
	POSITION pos = m_mapWndRegInfo.GetStartPosition();
	LPWNDREGINFO pWndRegInfo = NULL;
	//CRect* pRect;
	DWORD dwWndId;
	file.PutDW(REG_VERSION);
	file.PutDW(m_mapWndRegInfo.GetCount());
	while (pos)
	{
		m_mapWndRegInfo.GetNextAssoc(pos, dwWndId, (void*&)pWndRegInfo);
		file.Write(&pWndRegInfo->dwWndId, sizeof(pWndRegInfo->dwWndId));
		file.Write(&pWndRegInfo->rect, sizeof(pWndRegInfo->rect));
		file.Write(&pWndRegInfo->dwWndSize, sizeof(pWndRegInfo->dwWndSize));
		file.Write(&pWndRegInfo->bOpen, sizeof(pWndRegInfo->bOpen));
		file.Write(&pWndRegInfo->dwVersion, sizeof(pWndRegInfo->dwVersion));
		file.Write(&pWndRegInfo->dwSize, sizeof(pWndRegInfo->dwSize));
		if (pWndRegInfo->dwSize)
			file.Write(pWndRegInfo->lpArchive, pWndRegInfo->dwSize);
	}
	file.Close();

	if (file.Open("resolution", "wb") == FALSE)
		return FALSE;
	file.PutDW(g_Option.m_nResWidth);
	file.PutDW(g_Option.m_nResHeight);
	file.Close();
	return TRUE;
}
BOOL CWndMgr::LoadRegInfo(LPCTSTR lpszFileName)
{
	CFileIO file;
	BOOL bResult = FALSE;
	if (file.Open("resolution", "rb"))
	{
		int nWidth = file.GetDW();
		int nHeight = file.GetDW();
		file.Close();

		if (g_Option.m_nResWidth == nWidth && g_Option.m_nResHeight == nHeight)
		{
			bResult = TRUE;

			if (file.Open(lpszFileName, "rb") == FALSE)
				return FALSE;
			DWORD dwRegVersion = file.GetDW();
			if (dwRegVersion != REG_VERSION)
				return FALSE;
			WNDREGINFO wndRegInfo;
			//CWndBase* pWndBase;
			int nNum = file.GetDW();
			for (int i = 0; i < nNum; i++)
			{
				file.Read(&wndRegInfo.dwWndId, sizeof(wndRegInfo.dwWndId));
				file.Read(&wndRegInfo.rect, sizeof(wndRegInfo.rect));
				file.Read(&wndRegInfo.dwWndSize, sizeof(wndRegInfo.dwWndSize));
				file.Read(&wndRegInfo.bOpen, sizeof(wndRegInfo.bOpen));
				file.Read(&wndRegInfo.dwVersion, sizeof(wndRegInfo.dwVersion));
				file.Read(&wndRegInfo.dwSize, sizeof(wndRegInfo.dwSize));
				if (wndRegInfo.dwSize)
				{
					wndRegInfo.lpArchive = new BYTE[wndRegInfo.dwSize];
					file.Read(wndRegInfo.lpArchive, wndRegInfo.dwSize);
				}
				else
					wndRegInfo.lpArchive = NULL;
#if __VER >= 13 // __RENEW_CHARINFO
				if (wndRegInfo.rect.left < 0)
					wndRegInfo.rect.left = 0;
				if (wndRegInfo.rect.top < 0)
					wndRegInfo.rect.top = 0;
#endif // __RENEW_CHARINFO

				PutRegInfo(&wndRegInfo);//.dwWndId, wndRegInfo.rect, wndRegInfo.bOpen );
				SAFE_DELETE(wndRegInfo.lpArchive);
				//PutRegInfo( wndRegInfo.dwWndId, wndRegInfo.rect, wndRegInfo.bOpen );
			}
			file.Close();
		}
	}
	return bResult;
}

BOOL CWndMgr::PutDestParam(ItemProp* pItemProp, AddSkillProp* pAddSkillProp, CEditString* pEdit, CItemElem* pItemElem)
{
	if (!pItemProp && !pAddSkillProp)
		return FALSE;

	if (!pEdit)
		return FALSE;

	int nBonuses = PROP_BONUSES;
	if (pAddSkillProp)
		nBonuses = 2;

	CString strTemp;
	CString strSignAdj;
	DWORD dwParam;
	int nValue;
	BOOL bFirstString = TRUE;

	DWORD dwTextColor = 0xff0000ff;

	for (int i = 0; i < nBonuses; i++)
	{
		dwParam = pItemProp ? pItemProp->dwDestParam[i] : pAddSkillProp->dwDestParam[i];
		nValue = pItemProp ? pItemProp->nAdjParamVal[i] : pAddSkillProp->nAdjParamVal[i];

		if (dwParam == 0 || dwParam == NULL_ID || dwParam == DST_CHRSTATE)
			continue;

		if (bFirstString)
		{
			//			pEdit->AddString("\nGives following stats:", dwItemColor.dwGeneral);
			bFirstString = FALSE;
		}

		if (nValue < 0)
			strSignAdj = "";
		else
			strSignAdj = "+";

		if (IsDst_Rate(dwParam))
		{
			if (dwParam == DST_ATTACKSPEED)
				strTemp.Format("\n%s %s%d%%", FindDstString(dwParam), strSignAdj, static_cast<int>(nValue) / 2 / 10);
			else
				strTemp.Format("\n%s %s%d%%", FindDstString(dwParam), strSignAdj, nValue);
		}
		else
			strTemp.Format("\n%s %s%d", FindDstString(dwParam), strSignAdj, nValue);
		#ifdef __WEAPON_RARITY
		if (pItemElem && pItemProp)
		{
			if (pItemElem->m_nWeaponRarity > 0 && IsValidRarityItem(pItemProp->dwItemKind3))
			{
				CString str;

				float fFactor = 0.0f;
				DWORD dwRarityColor = 0xff199bd5;
				if (pItemElem->m_nWeaponRarity == 5)
				{
					dwRarityColor = 0xffb28a33;
					fFactor = 0.25f;
				}
				else if (pItemElem->m_nWeaponRarity == 4)
				{
					dwRarityColor = 0xff8847ff;
					fFactor = 0.2f;
				}
				else if (pItemElem->m_nWeaponRarity == 3)
				{
					dwRarityColor = 0xff4b69ff;
					fFactor = 0.15f;
				}
				else if (pItemElem->m_nWeaponRarity == 2)
				{
					dwRarityColor = 0xff5e98d9;
					fFactor = 0.1f;
				}
				else
					fFactor = 0.05f;

				if (pItemProp->dwDestParam[i] != 0xffffffff)
				{
					int nStat = 0;
					if (pItemProp->dwDestParam[i] == DST_STAT_ALLUP)
					{
						nStat = (int)(pItemProp->nAdjParamVal[i] * fFactor);
						str.Format(" (%+d)", nStat);
						strTemp += str;
						str.Format(" (%+d)", nStat);
						strTemp += str;
						str.Format(" (%+d)", nStat);
						strTemp += str;
						str.Format(" (%+d)", nStat);
						strTemp += str;
					}
					else
					{
						int nDst = (int)pItemProp->dwDestParam[i];
						if (IsDst_Rate(nDst))
						{
							if (nDst == DST_ATTACKSPEED)
							{
								nStat = (int)((pItemProp->nAdjParamVal[i] / 2 / 10) * fFactor);
								str.Format(" (%+d%%)", nStat);
								strTemp += str;
							}
							else
							{
								nStat = (int)(pItemProp->nAdjParamVal[i] * fFactor);
								str.Format(" (%+d%%)", nStat);
								strTemp += str;
							}
						}
						else
						{
							nStat = (int)(pItemProp->nAdjParamVal[i] * fFactor);
							str.Format(" (%+d)", nStat);
							strTemp += str;
						}
					}

					dwTextColor = dwRarityColor;
				}
			}
		}
#endif // __WEAPON_RARITY

		pEdit->AddString(strTemp, 0xff0000ff);
	}

	return !bFirstString;
}

BOOL CWndMgr::PutMedicine(ItemProp* pItemProp, CEditString* pEdit)
{
	if (!pItemProp || !pEdit)
		return FALSE;

	BOOL bFirstString = TRUE;
	DWORD dwParam;
	int nValue;
	CString strTemp;

	for (int i = 0; i < PROP_BONUSES; i++)
	{
		dwParam = pItemProp->dwDestParam[i];
		nValue = pItemProp->nAdjParamVal[i];

		if (dwParam != DST_MP && dwParam != DST_HP && dwParam != DST_FP)
			continue;

		if (nValue < 1)
			continue;

		if (bFirstString)
		{
			pEdit->AddString("\nRestores the following attributes:", dwItemColor.dwGeneral);
			bFirstString = FALSE;
		}

		switch (dwParam)
		{
		case DST_MP: strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_RECOVMP), nValue); break;
		case DST_HP: strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_RECOVHP), nValue); break;
		case DST_FP: strTemp.Format(prj.GetText(TID_GAME_TOOLTIP_RECOVFP), nValue); break;
		default: break;
		}

		pEdit->AddString("\n");
		pEdit->AddString(strTemp, dwItemColor.dwGeneral);
	}

	return !bFirstString;
}

BOOL CWndMgr::PutSocketCard(ItemProp* pItemProp, CEditString* pEdit)
{
	if (!pItemProp || !pEdit)
		return FALSE;

	if (pItemProp->dwItemKind3 != IK3_SOCKETCARD && pItemProp->dwItemKind3 != IK3_SOCKETCARD2)
		return FALSE;

	PPIERCINGAVAIL ptr = CPiercingAvail::GetInstance()->GetPiercingAvail(pItemProp->dwID);
	if (!ptr)
		return FALSE;

	CString str;
	if (pItemProp->dwItemKind3 == IK3_SOCKETCARD)
		str = "\nArmor Piercing Card.";
	else
		str = "\nWeapon Piercing Card.";
	pEdit->AddString(str, dwItemColor.dwGeneral);

	for (short i = 0; i < ptr->nSize; i++)
	{
		int nDst = ptr->anDstParam[i];
		int nAdj = ptr->anAdjParam[i];
		if (IsDst_Rate(nDst))
			str.Format("\nEffect: %s %+d%%", FindDstString(nDst), nDst == DST_ATTACKSPEED ? nAdj / 20 : nAdj);
		else
			str.Format("\nEffect: %s %+d", FindDstString(nDst), nAdj);
		pEdit->AddString(str, dwItemColor.dwGeneral);
	}

	return TRUE;
}

BOOL CWndMgr::PutFurniture(ItemProp* pItemProp, CEditString* pEdit)
{
	if (!pItemProp || !pEdit)
		return FALSE;

	if (pItemProp->dwItemKind2 != IK2_FURNITURE && pItemProp->dwItemKind2 != IK2_GUILDHOUSE_FURNITURE && pItemProp->dwItemKind2 != IK2_GUILDHOUSE_PAPERING)
		return FALSE;

	CString str;
	if (pItemProp->dwItemKind2 == IK2_FURNITURE)
		str = "\nPersonal house furniture.";
	else
		str = "\nGuild house furniture.";
	pEdit->AddString(str, dwItemColor.dwGeneral);

	PutDestParam(pItemProp, NULL, pEdit);
	return TRUE;
}

BOOL CWndMgr::PutBeadInfo(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	if (!pItemProp || !pEdit)
		return FALSE;

	if (pItemProp->dwItemKind2 != IK2_SYSTEM || pItemProp->dwItemKind3 != IK3_VIS)
		return FALSE;

	CString str;

	// Required bead
	DWORD dwNeed[2] = { pItemProp->dwReferTarget1, pItemProp->dwReferTarget2 };
	BOOL bAddRequiredText = FALSE;
	for (int i = 0; i < 2; i++)
	{
		if (dwNeed[i] != NULL_ID && dwNeed != NULL)
		{
			bAddRequiredText = TRUE;
			break;
		}
	}

	if (bAddRequiredText)
	{
		pEdit->AddString("\nRequired beads:", dwItemColor.dwGeneral);

		BYTE byState = 0;
		CItemElem* pPetItem = g_pPlayer->GetVisPetItem();
		if (pPetItem)
			byState = g_pPlayer->IsSatisfyNeedVis(pPetItem, pItemProp);
		else
			byState = FAILED_BOTH_NEEDVIS;

		for (int i = 0; i < 2; i++)
		{
			if (dwNeed[i] == NULL_ID || dwNeed == NULL)
				continue;

			ItemProp* pProp = prj.GetItemProp(dwNeed[i]);
			if (!pProp)
				continue;

			DWORD dwColorText = dwItemColor.dwGeneral;

			if (byState == FAILED_BOTH_NEEDVIS)
				dwColorText = 0xffff0000;

			if (i == 0 && byState == FAILED_1ST_NEEDVIS)
				dwColorText = 0xffff0000;

			if (i == 1 && byState == FAILED_2ND_NEEDVIS)
				dwColorText = 0xffff0000;

			str.Format("\n%s", pProp->szName);
			pEdit->AddString(str, dwColorText);
		}
	}

	// Stats
	PutDestParam(pItemProp, NULL, pEdit);

	// Time
	pEdit->AddString("\nRemaining time:", dwItemColor.dwGeneral);

	time_t t = pItemElem->m_dwKeepTime - time_null();
	if (t <= 0)
		t = (time_t)(pItemProp->dwAbilityMin * 60.0f);

	if (!pItemElem->IsFlag(CItemElem::expired))
	{
		if (t > 0)
		{
			CTimeSpan time(t);
			str.Format("%02d:%02d:%02d:%02d", static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes(), time.GetSeconds());
			pEdit->AddString("\n");
			pEdit->AddString(str, dwItemColor.dwGeneral);
		}
	}
	else
	{
		pEdit->AddString("\n");
		pEdit->AddString(prj.GetText(TID_GAME_ITEM_EXPIRED), 0xfff02222);
	}

	return TRUE;
}

#ifdef __FL_VENDORS
BOOL CWndMgr::PutVendorItem(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	if (!pItemElem || !pItemProp || !pEdit)
		return FALSE;

	if (pItemProp->dwItemKind3 != IK3_VENDOR)
		return FALSE;

	CString str = "\nOffline Vendor.";
	pEdit->AddString(str, dwItemColor.dwGeneral);

	time_t t = pItemElem->m_dwKeepTime;
	if (t <= 0)
		t = (time_t)(pItemProp->dwSkillTime * 60.0f);

	if (t > 0)
	{
		CTimeSpan time(t);
		str.Format("Remaining Time: %02d:%02d:%02d:%02d", static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes(), time.GetSeconds());
		pEdit->AddString("\n");
		pEdit->AddString(str, dwItemColor.dwGeneral);
	}

	return TRUE;
}
#endif

#if __VER >= 15 // __PETVIS
void CWndMgr::PutVisPetInfo(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{
	map< int, int > cTotalOpt;
	map< int, int >::iterator iter;
	CString strTemp, strTemp2;

	int availableSlot = pItemElem->GetPiercingSize();

	for (int ia = 0; ia < availableSlot; ++ia)
	{
		DWORD index = pItemElem->GetPiercingItem(ia);
		ItemProp* pProp = prj.GetItemProp(index);
		if (!pProp)
			continue;

		if (time_null() >= pItemElem->GetVisKeepTime(ia))
			continue;

		BOOL bOK = g_pPlayer->IsSatisfyNeedVis(pItemElem, pProp);

		if (SUCCSESS_NEEDVIS == bOK)
		{
			for (int iaa = 0; iaa < PROP_BONUSES; ++iaa)
			{
				int nDst = (int)pProp->dwDestParam[iaa];
				if (NULL_ID == nDst)
					continue;

				int nVal = (int)pProp->nAdjParamVal[iaa];
				iter = cTotalOpt.find(nDst);
				if (iter == cTotalOpt.end())
					cTotalOpt.insert(map< int, int >::value_type(nDst, nVal));
				else
					cTotalOpt[nDst] += nVal;
			}
		}
	}


	for (iter = cTotalOpt.begin(); iter != cTotalOpt.end(); ++iter)
	{
		int nDst = iter->first;
		int nVal = iter->second;

		if (IsDst_Rate(nDst))
		{
			if (nDst == DST_ATTACKSPEED)
				strTemp.Format("\n%s %+d%%", FindDstString(nDst), nVal / 2 / 10);
			else
				strTemp.Format("\n%s %+d%%", FindDstString(nDst), nVal);
		}
		else
		{
			strTemp.Format("\n%s %+d", FindDstString(nDst), nVal);
		}

		pEdit->AddString(strTemp, dwItemColor.dwPiercing);
	}


	for (int ib = 0; ib < MAX_VIS; ++ib)
	{
		DWORD index = pItemElem->GetPiercingItem(ib);
		if (0 != index && NULL_ID != index)
		{
			ItemProp* pProp = prj.GetItemProp(index);
			if (!pProp)
				continue;

			DWORD dwKeepTime = pItemElem->GetVisKeepTime(ib);

			time_t t = dwKeepTime - time_null();
			CTimeSpan ct(t);

			LONG nDay = (LONG)(ct.GetDays());
			LONG nHour = ct.GetHours();
			LONG nMin = ct.GetMinutes();
			LONG nSec = ct.GetSeconds();

			CString strDays, strHours, strMinutes, strSeconds;
			if (nDay > 0)
			{
				strDays.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(nDay));
				strHours.Format(prj.GetText(TID_PK_LIMIT_HOUR), nHour);
				strMinutes.Format(prj.GetText(TID_PK_LIMIT_MINUTE), nMin);

				strTemp2 = strDays + strHours + strMinutes;
			}
			else if (nHour > 0)
			{
				strHours.Format(prj.GetText(TID_PK_LIMIT_HOUR), nHour);
				strMinutes.Format(prj.GetText(TID_PK_LIMIT_MINUTE), nMin);

				strTemp2 = strHours + strMinutes;
			}
			else if (nMin > 0)
			{
				strMinutes.Format(prj.GetText(TID_PK_LIMIT_MINUTE), nMin);
				strSeconds.Format(prj.GetText(TID_PK_LIMIT_SECOND), nSec);

				strTemp2 = strMinutes + strSeconds;
			}
			else if (nSec >= 0)
			{
				strTemp2.Format(prj.GetText(TID_PK_LIMIT_SECOND), nSec);
			}
			else
			{
				strTemp2.Format(GETTEXT(TID_GAME_TOOLTIP_TIMEOUT));
			}

			strTemp = pProp->szName + CString(" (") + strTemp2 + CString(")");

			pEdit->AddString("\n");

			if (t > 0)
				pEdit->AddString(strTemp, dwItemColor.dwTime);
			else
				pEdit->AddString(strTemp, 0xffff0000);

		}
	}
}

void CWndMgr::PutPetKind(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit)
{

	CString strTemp;

	if (pItemElem->IsEatPet())
	{
		if (pItemElem->IsVisPet())
		{
			strTemp.Format("\n%s", GETTEXT(TID_TOOLTIP_PET_BUFF));
			//	pEdit->AddString( strTemp, GETTEXTCOLOR(TID_TOOLTIP_PET_BUFF) );
		}
		else
		{
			strTemp.Format("\n%s", GETTEXT(TID_TOOLTIP_PET_PICKUP));
			//	pEdit->AddString( strTemp, GETTEXTCOLOR(TID_TOOLTIP_PET_PICKUP) );
		}
	}

	if (pItemElem->IsEgg())
	{
		strTemp.Format("\n%s", GETTEXT(TID_TOOLTIP_PET_REAR));
		//pEdit->AddString( strTemp, GETTEXTCOLOR(TID_TOOLTIP_PET_REAR) );
	}

	pEdit->AddString(strTemp);

#ifdef __AIO_ADMIN_THINGS
	if (g_pPlayer && g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR))
	{
		CString nID;
		nID.Format("\nItem ID: %d", pItemProp->dwID);
		pEdit->AddString(nID, D3DCOLOR_XRGB(255, 102, 0));
		nID.Format("\nObj ID: %u", pItemElem->m_dwObjId);
		pEdit->AddString(nID, D3DCOLOR_XRGB(255, 102, 0));
		nID.Format("\nObj Index: %u", pItemElem->m_dwObjIndex);
		pEdit->AddString(nID, D3DCOLOR_XRGB(255, 102, 0));
		nID.Format("\nSerial: %d", pItemElem->GetSerialNumber());
		pEdit->AddString(nID, D3DCOLOR_XRGB(255, 102, 0));

		if (pItemElem->m_pPet)
		{
			nID.Format("\nPet Kind: %d", pItemElem->m_pPet->GetKind());
			pEdit->AddString(nID, D3DCOLOR_XRGB(255, 102, 0));
			nID.Format("\nPet level: %d", pItemElem->m_pPet->GetLevel());
			pEdit->AddString(nID, D3DCOLOR_XRGB(255, 102, 0));
			nID.Format("\nPet energy: %d", pItemElem->m_pPet->GetEnergy());
			pEdit->AddString(nID, D3DCOLOR_XRGB(255, 102, 0));
			nID.Format("\nPet exp: %d", pItemElem->m_pPet->GetExp());
			pEdit->AddString(nID, D3DCOLOR_XRGB(255, 102, 0));
		}
	}
#endif
}
#endif 

void CWndMgr::PutEquipItemText(CEditString* pEdit)
{
	CString strEdit = _T("");
	strEdit.Format("[ %s ]\n", prj.GetText(TID_GAME_EQUIPED_ITEM));
	pEdit->AddString(strEdit, 0xff000000, ESSTY_BOLD);
}

#if __VER >= 8 // __CSC_VER8_1
void CWndMgr::ClearAllWnd()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	if (!m_clearFlag)
	{
		POSITION pos = m_mapWndApplet.GetStartPosition();
		while (pos)
		{
			m_mapWndApplet.GetNextAssoc(pos, dwIdApplet, (void*&)pWndBase);
			if (pWndBase && pWndBase->GetWndId() != APP_WORLD)
			{
				m_tempWndId.push_back(pWndBase->GetWndId());
				pWndBase->SetVisible(FALSE);
			}
		}
		CWndChat* pWndChat = (CWndChat*)GetApplet(APP_COMMUNICATION_CHAT);
		if (pWndChat != NULL && pWndChat->m_bChatLog)
			m_pWndChatLog->SetVisible(FALSE);

#if __VER >= 9 // __CSC_VER9_1
		CWndPetStatus* pWndPetStatus = (CWndPetStatus*)GetWndBase(APP_PET_STATUS);
		if (pWndPetStatus != NULL)
			pWndPetStatus->SetVisible(FALSE);

#if __VER >= 15 // __PETVIS
		CWndBuffPetStatus* pWndBuffPetStatus = (CWndBuffPetStatus*)GetWndBase(APP_BUFFPET_STATUS);
		if (pWndBuffPetStatus != NULL)
			pWndBuffPetStatus->SetVisible(FALSE);
#endif

#if __VER >= 15 // __GUILD_HOUSE
		CWndGHMainMenu* pWndGHMain = (CWndGHMainMenu*)GetWndBase(APP_GH_MAIN);
		if (pWndGHMain)
			pWndGHMain->SetVisible(FALSE);
#endif //__GUILD_HOUSE

#endif //__CSC_VER9_1

#if __VER >= 12 // __SECRET_ROOM
		CWndSecretRoomQuick* pWndSecretRoomQuick = (CWndSecretRoomQuick*)GetWndBase(APP_SECRETROOM_QUICK);
		if (pWndSecretRoomQuick != NULL)
			pWndSecretRoomQuick->SetVisible(FALSE);
#endif //__SECRET_ROOM

		if (m_pWndTaskBar)
			m_pWndTaskBar->SetVisible(FALSE);

		if (m_pWndWorld)
			m_pWndWorld->GetAdvMgr()->SetVisible(FALSE);

		SetFocus();
	}
	m_clearFlag = TRUE;
}

void CWndMgr::RestoreWnd()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	BOOL stopFlag = TRUE;
	int wndId;

	if (m_clearFlag)
	{
		std::list<int>::iterator it = m_tempWndId.begin();
		while (it != m_tempWndId.end())
		{
			POSITION pos = m_mapWndApplet.GetStartPosition();
			while (pos && stopFlag)
			{
				m_mapWndApplet.GetNextAssoc(pos, dwIdApplet, (void*&)pWndBase);
				wndId = *it;
				if (wndId == pWndBase->GetWndId())
				{
					pWndBase->SetVisible(TRUE);
					stopFlag = FALSE;
				}
			}
			it++;
			stopFlag = TRUE;
		}
		m_tempWndId.clear();
		CWndChat* pWndChat = (CWndChat*)GetApplet(APP_COMMUNICATION_CHAT);
		if (pWndChat != NULL && pWndChat->m_bChatLog)
			m_pWndChatLog->SetVisible(TRUE);

#if __VER >= 9 // __CSC_VER9_1
		CWndPetStatus* pWndPetStatus = (CWndPetStatus*)GetWndBase(APP_PET_STATUS);
		if (pWndPetStatus != NULL)
			pWndPetStatus->SetVisible(TRUE);

#if __VER >= 15 // __PETVIS
		CWndBuffPetStatus* pWndBuffPetStatus = (CWndBuffPetStatus*)GetWndBase(APP_BUFFPET_STATUS);
		if (pWndBuffPetStatus != NULL)
			pWndBuffPetStatus->SetVisible(TRUE);
#endif
#endif //__CSC_VER9_1

#if __VER >= 12 // __SECRET_ROOM
		CWndSecretRoomQuick* pWndSecretRoomQuick = (CWndSecretRoomQuick*)GetWndBase(APP_SECRETROOM_QUICK);
		if (pWndSecretRoomQuick != NULL)
			pWndSecretRoomQuick->SetVisible(TRUE);
#endif //__SECRET_ROOM

#if __VER >= 15 // __GUILD_HOUSE
		CWndGHMainMenu* pWndGHMain = (CWndGHMainMenu*)GetWndBase(APP_GH_MAIN);
		if (pWndGHMain != NULL)
			pWndGHMain->SetVisible(TRUE);
#endif //__GUILD_HOUSE

		if (m_pWndTaskBar)
			m_pWndTaskBar->SetVisible(TRUE);

		if (m_pWndWorld)
			m_pWndWorld->GetAdvMgr()->SetVisible(TRUE);
	}
	m_clearFlag = FALSE;
}
#endif //__CSC_VER8_1

BOOL CWndMgr::IsDstRate(int nDstParam)
{
	return IsDst_Rate(nDstParam);
}
char* CWndMgr::FindDstString(int nDstParam)
{
	for (int i = 0; ; ++i)
	{
		if (g_DstString[i].nDstParam == 0)
			break;

		if (g_DstString[i].nDstParam == nDstParam)
			return (char*)prj.GetText(g_DstString[i].nNameIndex);
	}
	return "";
}


void CWndMgr::FormalizeChatString(CString& str)
{

	str.TrimLeft();
	str.TrimRight();

	while (str.GetLength() > 0 && str.GetAt(0) == '\"')
	{
		str.Delete(0);

		if (str.GetLength() > 1)
		{
			if (str.GetAt(str.GetLength() - 1) == '\"')
				str.Delete(str.GetLength() - 1);
			str.TrimLeft();
			str.TrimRight();
		}
	}
}

#ifdef __WINDOW_INTERFACE_BUG
void CWndMgr::CloseBoundWindow(void)
{
	SAFE_DELETE(m_pWndRemoveAttribute);
	SAFE_DELETE(m_pWndRemovePiercing);
	SAFE_DELETE(m_pWndSmeltMixJewel);
	SAFE_DELETE(m_pWndSmeltJewel);
	SAFE_DELETE(m_pWndExtraction);
	SAFE_DELETE(m_pWndLvReqDown);
	SAFE_DELETE(m_pWndBlessingCancel);

	SAFE_DELETE(m_pWndBank);
	SAFE_DELETE(m_pWndGuildBank);

	SAFE_DELETE(m_pWndPost);
	SAFE_DELETE(m_pWndUpgradeBase);
	SAFE_DELETE(m_pWndShop);
	SAFE_DELETE(m_pWndSmeltSafety);
	SAFE_DELETE(m_pWndSmeltSafetyConfirm);

#ifdef __FL_PACKED_ITEMS
	CWndPackedItems* pWnd = (CWndPackedItems*)GetWndBase(APP_PACKED_ITEMS);
	if (pWnd)
		pWnd->ClearItem();
#endif
#ifdef __FL_RECYCLE
	CWndRecycle* pWndRecycle = (CWndRecycle*)GetWndBase(APP_RECYCLE);
	if (pWndRecycle)
		pWndRecycle->ClearItem();
#endif
#ifdef __FL_FAST_ELEMENT
	CWndChangeAttributeNew* pWndElement = (CWndChangeAttributeNew*)GetWndBase(APP_FAST_ELEMENT);
	if (pWndElement)
	{
		pWndElement->ClearMainItem();
		pWndElement->ClearScroll();
	}
#endif
}
#endif // __WINDOW_INTERFACE_BUG

#ifdef __FL_BOXES_V2
void CWndMgr::PutBoxDetails(ItemProp* pProp, CEditString* pEdit, int nFlag)
{
	if (!pProp)
		return;

	if (pProp->nBoxType == BOX_TYPE_NOTBOX)
		return;

	if (pProp->nBoxType != BOX_TYPE_SELECTBOX)
	{
		PGIFTBOX giftboxelem = NULL;
		PPACKITEMELEM pPackItemElem = CPackItem::GetInstance()->Open(pProp->dwID);
		if (!pPackItemElem)
			giftboxelem = CGiftboxMan::GetInstance()->TakeInfo(pProp->dwID);

		if (!pPackItemElem && !giftboxelem)
			return;

		if (nFlag == APP_INVENTORY)
			pEdit->AddString("\nRight mouse click for mass opening", 0xFF000000);

		if (giftboxelem)
			pEdit->AddString("\nPossible drop:", dwItemColor.dwGeneral, ESSTY_BOLD);
		else
			pEdit->AddString("\nGuaranteed drop:", dwItemColor.dwGeneral, ESSTY_BOLD);

		int nSize = giftboxelem ? giftboxelem->nSize : pPackItemElem->nSize;
		CString str;
		ItemProp* pReward;
		int nNum;
		float fChance;

		for (int i = 0; i < nSize; i++)
		{
			pReward = prj.GetItemProp(giftboxelem ? giftboxelem->adwItem[i] : pPackItemElem->adwItem[i]);
			nNum = giftboxelem ? giftboxelem->anNum[i] : pPackItemElem->anNum[i];
			fChance = giftboxelem ? (giftboxelem->dwChance[i] / 10000.0f) : 0.0f;

			if (!pReward)
				continue;

			if (giftboxelem)
				str.Format("\n- [%.2f%%] %s [x%d]", fChance, pReward->szName, nNum);
			else
				str.Format("\n- %s [x%d]", pReward->szName, nNum);

			pEdit->AddString(str, 0xFF333333);
		}
	}
	else
	{
		auto vecItems = CNewBoxes::GetInstance()->GetBoxElements(pProp->dwID);
		if (!vecItems)
			return;

		pEdit->AddString("\nAvailable drop:", dwItemColor.dwGeneral, ESSTY_BOLD);

		CString str;
		ItemProp* pProp;

		for (auto& it : *vecItems)
		{
			pProp = prj.GetItemProp(it.dwReward);
			if (!pProp)
				continue;

			str.Format("\n- %s [x%d]", pProp->szName, it.nItemCount);
			pEdit->AddString(str, 0xFF333333);
		}
	}
}
#endif

#ifdef __MODEL_VIEW
BOOL CWndMgr::IsModelView(ItemProp* pProp)
{
	if (!pProp)
		return FALSE;

	if (pProp->dwParts > 1 && pProp->dwParts < MAX_HUMAN_PARTS)
	{
		if (pProp->dwParts >= PARTS_NECKLACE1 && pProp->dwParts <= PARTS_BULLET)
			return FALSE;
		else if (pProp->dwParts == PARTS_RIDE)
			return FALSE;
		else
			return TRUE;
	}

	if (pProp->dwItemKind3 == IK3_PET || pProp->dwItemKind3 == IK3_EGG)
		return TRUE;

	return FALSE;
}
void CWndMgr::PutModelView(ItemProp* pProp, CEditString* pEdit, int nFlag)
{
	if (!pProp)
		return;

	if (!IsModelView(pProp))
		return;

	if (nFlag != APP_TELEPORT && nFlag != APP_WIKI_ITEMS
		&& nFlag != APP_TRADE && nFlag != APP_SHOPEX
		&& nFlag != APP_SHOP_SEARCH && nFlag != APP_VENDOR_REVISION
		)
		return;

	pEdit->AddString("\nSHIFT + Double Click to view model", 0xFF000000);
}
bool CWndMgr::PreviewModel(CItemElem* pItemElem)
{
	if (!pItemElem)
		return false;

	bool bOk = false;

	CWndModelView* pWndViewer = (CWndModelView*)GetApplet(APP_MODEL_VIEW);
	if (!pWndViewer)
	{
		pWndViewer = (CWndModelView*)CreateApplet(APP_MODEL_VIEW);
		if (pWndViewer)
		{
			bOk = pWndViewer->SetPart(pItemElem);

			if (!bOk)
				pWndViewer->Destroy();
		}
	}
	else
		bOk = pWndViewer->SetPart(pItemElem);

	return bOk;
}
#endif

void CWndMgr::StringSize(CString& strSrc, int nLetters)
{
	if (strSrc.GetLength() > nLetters)
	{
		int	nReduceCount = 0;

		for (nReduceCount = 0; nReduceCount < nLetters; )
		{
			if (IsDBCSLeadByte(strSrc[nReduceCount]))
				nReduceCount += 2;
			else
				nReduceCount++;
		}

		strSrc = strSrc.Left(nReduceCount);
		strSrc += "...";
	}
}

void CWndMgr::UseItem(CItemElem* pItemBase, int nType)
{
	if (!g_pPlayer)
		return;

	if (!g_pPlayer->IsPossibleUseItem())
		return;

	CItemElem* pItemElem = pItemBase;
	if (!IsUsableItem(pItemElem))
		return;

	ItemProp* pProp = pItemElem->GetProp();
	if (!pProp)
		return;

	if (nType == TYPE_INVENTORY)
	{
		// Check if this item can be used as material in one of the opened windows
		if (UseItem_MaterialWnd(pItemElem, pProp))
			return;

		// If not, then check if player can use items when one of this windows opened (have no idea why)
		if (UseItem_RestrictedWnd())
			return;

		// If all ok, then check if item can't be used directly and should be confirmed by player firstly
		if (UseItem_MaterialWnd2(pItemElem, pProp))
			return;
	}
	else
	{
		// Items from other containers cant be used as material, so we just check if player can use items when one of this windows opened (have no idea why)
		if (UseItem_RestrictedWnd())
			return;

		if (!g_pPlayer->CanBeUsedOutsideInventory(pProp))
		{
			PutString("This item can be used only from inventory!");
			return;
		}
	}

	// Check some specific cases
	if (UseItem_SpecificSituations(pItemElem, pProp))
		return;

	g_DPlay.SendDoUseItem(pItemElem->m_dwObjId, "", nType);
}

BOOL CWndMgr::UseItem_RestrictedWnd()
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	if ((pWndInventory && pWndInventory->m_dwEnchantWaitTime != 0xffffffff) || GetWndBase(APP_SMELT_SAFETY_CONFIRM))
	{
		PutString(prj.GetText(TID_MMI_NOTUPGRADE), NULL, prj.GetTextColor(TID_MMI_NOTUPGRADE));
		return TRUE;
	}
	else if (GetWndBase(APP_EQUIP_BIND_CONFIRM))
	{
		PutString(prj.GetText(TID_TOOLTIP_EQUIPBIND_ERROR01), NULL, prj.GetTextColor(TID_TOOLTIP_EQUIPBIND_ERROR01));
		return TRUE;
	}
	else if (GetWndBase(APP_COMMITEM_DIALOG))
	{
		PutString(prj.GetText(TID_TOOLTIP_ITEM_USING_ERROR), NULL, prj.GetTextColor(TID_TOOLTIP_ITEM_USING_ERROR));
		return TRUE;
	}
	else if (GetWndBase(APP_SHOP_) ||
		GetWndBase(APP_BANK) ||
		GetWndBase(APP_TRADE))
	{
		PutString(prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING));
		return TRUE;
	}
	else if (GetWndBase(APP_SUMMON_ANGEL))
	{
		PutString(prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING));
		return TRUE;
	}
#ifdef __EVE_MINIGAME
	else if (GetWndBase(APP_MINIGAME_KAWIBAWIBO))
	{
		PutString(prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM));
		return TRUE;
	}
	else if (GetWndBase(APP_MINIGAME_KAWIBAWIBO_WIN))
	{
		PutString(prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM));
		return TRUE;
	}
#endif //
	else if (GetWndBase(APP_SMELT_EXTRACTION))
	{
		PutString(prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM));
		return TRUE;
	}
	else if (GetWndBase(APP_SMELT_JEWEL))
	{
		PutString(prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM));
		return TRUE;
	}
	else if (GetWndBase(APP_SMELT_MIXJEWEL))
	{
		PutString(prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM));
		return TRUE;
	}
	else if (GetWndBase(APP_PET_FOODMILL))
	{
		PutString(prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM));
		return TRUE;
	}
	else if (GetWndBase(APP_SMELT_SAFETY))
	{
		PutString(prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM));
		return TRUE;
	}
	else if (GetWndBase(APP_REPAIR))
	{
		PutString(prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING));
		return TRUE;
	}
#ifdef __FL_EQUALIZED_MODE
	else if (GetWndBase(APP_TMP_CLASS))
		return TRUE;
#endif
	else if (GetWndBase(APP_VENDOR_REVISION))
		return TRUE;

	return FALSE;
}

BOOL CWndMgr::UseItem_MaterialWnd(CItemElem* pItemElem, ItemProp* pProp)
{
#ifdef __MODEL_CHANGE
	if (m_pWndLookChange)
	{
		if (pItemElem->GetExtra() < pItemElem->m_nItemNum)
		{
			m_pWndLookChange->SetItem(pItemElem);
			return TRUE;
		}
	}
#endif

#ifdef __FL_RECYCLE
	if (GetWndBase(APP_RECYCLE))
	{
		CWndRecycle* pWnd = (CWndRecycle*)GetWndBase(APP_RECYCLE);
		if (pWnd && pWnd->SetItem(pItemElem))
			return TRUE;
	}
#endif

	if (GetWndBase(APP_REMOVE_ATTRIBUTE))
	{
		if (pProp->IsEleRefineryAble())
		{
			if (pItemElem->GetExtra() < pItemElem->m_nItemNum)
			{
				CWndRemoveAttribute* pWndRemoveAttribute = (CWndRemoveAttribute*)GetWndBase(APP_REMOVE_ATTRIBUTE);
				pWndRemoveAttribute->SetWeapon(pItemElem);
				return TRUE;
			}
		}
	}
	if (GetWndBase(APP_SMELT_REMOVE_PIERCING_EX))
	{
		if (pProp->IsEleRefineryAble())
		{
			CWndRemovePiercing* pWndRemovePiercing = (CWndRemovePiercing*)GetWndBase(APP_SMELT_REMOVE_PIERCING_EX);
			pWndRemovePiercing->SetItem(pItemElem);
			return TRUE;
		}
	}
	if (GetWndBase(APP_SMELT_JEWEL))
	{
		if (pProp->dwItemKind2 == IK2_MATERIAL)
		{
			CWndSmeltJewel* pWndRemoveJewel = (CWndSmeltJewel*)GetWndBase(APP_SMELT_JEWEL);
			pWndRemoveJewel->SetItem(pItemElem);
			pWndRemoveJewel->SetJewel(pItemElem);
			return TRUE;
		}
	}

	if (GetWndBase(APP_SMELT_MIXJEWEL))
	{
		if (pProp->dwID == II_GEN_MAT_ORICHALCUM01 || pProp->dwID == II_GEN_MAT_MOONSTONE)
		{
			if (pItemElem->GetExtra() < pItemElem->m_nItemNum)
			{
				CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase(APP_SMELT_MIXJEWEL);
				pWndMixJewel->SetJewel(pItemElem);
				return TRUE;
			}
		}
	}

	if (GetWndBase(APP_SMELT_SAFETY))
	{
		if (pItemElem->GetExtra() < pItemElem->m_nItemNum)
		{
			CWndSmeltSafety* pWndSmeltSafety = (CWndSmeltSafety*)GetWndBase(APP_SMELT_SAFETY);
			pWndSmeltSafety->SetItem(pItemElem);
			return TRUE;
		}
	}
	if (GetWndBase(APP_SMELT_JEWEL))
	{
		if (pProp->dwID == II_GEN_MAT_DIAMOND ||
			pProp->dwID == II_GEN_MAT_EMERALD ||
			pProp->dwID == II_GEN_MAT_SAPPHIRE ||
			pProp->dwID == II_GEN_MAT_RUBY ||
			pProp->dwID == II_GEN_MAT_TOPAZ)
		{
			if (pItemElem->GetExtra() < pItemElem->m_nItemNum)
			{
				CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)GetWndBase(APP_SMELT_JEWEL);
				pWndSmeltJewel->SetJewel(pItemElem);
				return TRUE;
			}
		}
	}
	if (GetWndBase(APP_HERO_SKILLUP))
	{
		if (pProp->dwID == II_GEN_MAT_DIAMOND ||
			pProp->dwID == II_GEN_MAT_EMERALD ||
			pProp->dwID == II_GEN_MAT_SAPPHIRE ||
			pProp->dwID == II_GEN_MAT_RUBY ||
			pProp->dwID == II_GEN_MAT_TOPAZ)
		{
			if (pItemElem->GetExtra() < pItemElem->m_nItemNum)
			{
				CWndHeroSkillUp* pWndHeroSkillUp = (CWndHeroSkillUp*)GetWndBase(APP_HERO_SKILLUP);
				pWndHeroSkillUp->SetJewel(pItemElem);
				return TRUE;
			}
		}
	}
	if (GetWndBase(APP_SMELT_EXTRACTION))
	{
		if (pProp->dwItemKind1 == IK1_WEAPON)
		{
			if (pItemElem->GetExtra() < pItemElem->m_nItemNum)
			{
				CWndExtraction* pWndExtraction = (CWndExtraction*)GetWndBase(APP_SMELT_EXTRACTION);
				pWndExtraction->SetWeapon(pItemElem);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CWndMgr::UseItem_MaterialWnd2(CItemElem* pItemElem, ItemProp* pProp)
{
	switch (pProp->dwID)
	{
#ifdef __FL_FAST_ELEMENT
	case __FL_FAST_ELEMENT:
	{
		CWndChangeAttributeNew* pWnd = (CWndChangeAttributeNew*)CreateApplet(APP_FAST_ELEMENT);
		if (pWnd)
		{
			pWnd->SetScroll(pItemElem);
			return TRUE;
		}
	}
	break;
#endif
	case II_SYS_SYS_SCR_PET_NAMING:
	{
		SAFE_DELETE(m_pWndChangePetName);
		m_pWndChangePetName = new CWndChangePetName;
		m_pWndChangePetName->Initialize(&g_WndMng);
		m_pWndChangePetName->SetItemId(pItemElem->m_dwObjId);
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_PARTYSUMMON:
	{
		if (!g_Party.IsLeader(g_pPlayer->m_idPlayer))
		{
			PutString(prj.GetText(TID_GAME_TROUPECREATE), NULL, prj.GetTextColor(TID_GAME_TROUPECREATE));
			return TRUE;
		}

		SAFE_DELETE(m_pWndSummonParty);
		m_pWndSummonParty = new CWndSummonParty;
		m_pWndSummonParty->Initialize(&g_WndMng);
		m_pWndSummonParty->SetData((WORD)(pItemElem->m_dwObjId), 0);
		return TRUE;
	}
	break;
	case II_CHR_SYS_SCR_ITEMTRANSY_A:
	case II_CHR_SYS_SCR_ITEMTRANSY_B:
	{
		SAFE_DELETE(m_pWndItemTransy);
		m_pWndItemTransy = new CWndItemTransy;
		m_pWndItemTransy->Initialize(&g_WndMng);
		m_pWndItemTransy->Init(pItemElem);
		return TRUE;
	}
	break;
	case II_CHR_SYS_SCR_TRANSY:
	{
		for (DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; dwParts++)
		{
			if (dwParts == PARTS_HEAD || dwParts == PARTS_HAIR || dwParts == PARTS_LWEAPON
				|| dwParts == PARTS_RWEAPON || dwParts == PARTS_SHIELD || dwParts == PARTS_RIDE
				|| (dwParts >= PARTS_NECKLACE1 && dwParts <= PARTS_BULLET))
				continue;

			CItemElem* pArmor = g_pPlayer->m_Inventory.GetEquip(dwParts);
			if (pArmor)
			{
				PutString(prj.GetText(TID_GAME_CHECK_EQUIP), NULL, prj.GetTextColor(TID_GAME_CHECK_EQUIP));
				return TRUE;
			}
		}

		SAFE_DELETE(m_pWndChangeSex);
		m_pWndChangeSex = new CWndChangeSex;
		m_pWndChangeSex->Initialize();
		m_pWndChangeSex->SetData(pItemElem->m_dwObjId);

		CWndStatic* pWndStatic = (CWndStatic*)m_pWndChangeSex->GetDlgItem(WIDC_STATIC1);
		if (pWndStatic)
			pWndStatic->SetVisible(FALSE);

		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_GCHANAM:
	{
		if (!m_pWndGuildName)
		{
			m_pWndGuildName = new CWndGuildName;
			m_pWndGuildName->Initialize(&g_WndMng);
		}
		m_pWndGuildName->SetData((BYTE)(pItemElem->m_dwObjId));
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_CHANAM:
	{
		if (!m_pWndChangeName)
		{
			m_pWndChangeName = new CWndChangeName;
			m_pWndChangeName->Initialize(&g_WndMng, 0);
		}
		m_pWndChangeName->SetData((WORD)(pItemElem->m_dwObjId), 0);
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_FACEOFFFREE:
	{
		if (!m_pWndUseCouponConfirm)
		{
			m_pWndUseCouponConfirm = new CWndUseCouponConfirm;
			m_pWndUseCouponConfirm->SetInfo(APP_BEAUTY_SHOP_SKIN, 0);
			m_pWndUseCouponConfirm->Initialize(NULL);
		}
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_HAIRCHANGE:
	{
		if (!m_pWndUseCouponConfirm)
		{
			m_pWndUseCouponConfirm = new CWndUseCouponConfirm;
			m_pWndUseCouponConfirm->SetInfo(APP_BEAUTY_SHOP_EX, 0);
			m_pWndUseCouponConfirm->Initialize(NULL);
		}
		return TRUE;
	}
	break;
	case II_SYS_SYS_VIS_KEY01:
	{
		if (g_pPlayer->HasActivatedVisPet())
		{
			if (!m_pWndBuffPetStatus)
			{
#ifdef __AIO_PETS
				g_Option.m_bHideWindowLooter = FALSE;
				PutString("Window activated. You can deactivate it again by clicking on the pet -> Toggle Buff Pet Status, or on close button in window.");
#endif

				m_pWndBuffPetStatus = new CWndBuffPetStatus;
				m_pWndBuffPetStatus->Initialize();
			}

			m_pWndBuffPetStatus->DoModal_ConfirmQuestion(pItemElem->m_dwObjId, g_pPlayer->GetId(), pProp->dwID, 0, CWndConfirmVis::CVS_EQUIP_VISKEY);
		}
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_PET_MAGIC:
	{
		SAFE_DELETE(m_pWndConfirmVis);
		m_pWndConfirmVis = new CWndConfirmVis;

		m_pWndConfirmVis->m_dwItemId = pItemElem->m_dwObjId;
		m_pWndConfirmVis->m_objid = g_pPlayer->GetId();
		m_pWndConfirmVis->m_dwItemIndex = pProp->dwID;

		m_pWndConfirmVis->m_eSection = CWndConfirmVis::CVS_PICKUP_TO_BUFF;
		m_pWndConfirmVis->Initialize();
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_AWAKESAFE:
	{
		SAFE_DELETE(m_pWndConfirmVis);
		m_pWndConfirmVis = new CWndConfirmVis;

		m_pWndConfirmVis->m_dwItemId = pItemElem->m_dwObjId;
		m_pWndConfirmVis->m_objid = g_pPlayer->GetId();
		m_pWndConfirmVis->m_dwItemIndex = pProp->dwID;

		m_pWndConfirmVis->m_eSection = CWndConfirmVis::ETC_PROTECT_AWAKE;
		m_pWndConfirmVis->Initialize();

		return TRUE;
	}
	break;
	case II_CHR_SYS_SCR_RESTATE:
	case II_CHR_SYS_SCR_RESTATE_STR:
	case II_CHR_SYS_SCR_RESTATE_STA:
	case II_CHR_SYS_SCR_RESTATE_DEX:
	case II_CHR_SYS_SCR_RESTATE_INT:
	case II_CHR_SYS_SCR_RESTATE_STR_LOW:
	case II_CHR_SYS_SCR_RESTATE_STA_LOW:
	case II_CHR_SYS_SCR_RESTATE_DEX_LOW:
	case II_CHR_SYS_SCR_RESTATE_INT_LOW:
	{
		SAFE_DELETE(m_pWndRestateConfirm);
		m_pWndRestateConfirm = new CWndRestateConfirm(pProp->dwID);
		m_pWndRestateConfirm->SetInformation(pItemElem->m_dwObjId);
		m_pWndRestateConfirm->Initialize(NULL);
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_RECCURENCE:
	{
		if (g_pPlayer->m_nSkillPoint < g_pPlayer->GetCurrentMaxSkillPoint())
		{
			SAFE_DELETE(m_pWndCommItemDlg);
			m_pWndCommItemDlg = new CWndCommItemDlg;
			m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
			m_pWndCommItemDlg->SetItem(TID_GAME_SKILLINIT, pItemElem->m_dwObjId);
		}
		else
			PutString(prj.GetText(TID_GAME_ERROR_SKILLRECCURENCE), NULL, prj.GetTextColor(TID_GAME_ERROR_SKILLRECCURENCE));

		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_CHACLA:
	{
		if (g_pPlayer->IsBaseJob())
		{
			PutString(prj.GetText(TID_GAME_NOTUSEVAG), NULL, prj.GetTextColor(TID_GAME_NOTUSEVAG));
			return TRUE;
		}
		else
		{
			for (DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; dwParts++)
			{
				if (dwParts == PARTS_HEAD || dwParts == PARTS_HAIR || dwParts == PARTS_RIDE)
					continue;
				CItemElem* pArmor = g_pPlayer->m_Inventory.GetEquip(dwParts);
				if (pArmor)
				{
					PutString(prj.GetText(TID_GAME_CHECK_EQUIP), NULL, prj.GetTextColor(TID_GAME_CHECK_EQUIP));
					return TRUE;
				}
			}
		}

		SAFE_DELETE(m_pWndCommItemDlg);
		m_pWndCommItemDlg = new CWndCommItemDlg;
		m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
		m_pWndCommItemDlg->SetItem(TID_GAME_WARNINGCCLS, pProp->dwID);
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_PET_TAMER_MIRACLE:
		SAFE_DELETE(m_pWndPetMiracle);
		m_pWndPetMiracle = new CWndPetMiracle;
		m_pWndPetMiracle->Initialize(&g_WndMng);
		m_pWndPetMiracle->SetItem(pItemElem->m_dwObjId);
		return TRUE;
	case II_SYS_SYS_SCR_PET_TAMER_MISTAKE:
		SAFE_DELETE(m_pWndCommItemDlg);
		m_pWndCommItemDlg = new CWndCommItemDlg;
		m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
		m_pWndCommItemDlg->SetItem(TID_GAME_PET_MISTAKE_DESC, pItemElem->m_dwObjId);
		return TRUE;
	case II_SYS_SYS_SCR_PET_HATCH:
		SAFE_DELETE(m_pWndCommItemDlg);
		m_pWndCommItemDlg = new CWndCommItemDlg;
		m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
		m_pWndCommItemDlg->SetItem(TID_GAME_PET_HATCH_DESC, pItemElem->m_dwObjId);
		return TRUE;
	case II_SYS_SYS_FEED_MAKER:
		if (m_pWndPetFoodMill)
			PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
		else
		{
			SAFE_DELETE(m_pWndPetFoodMill);
			m_pWndPetFoodMill = new CWndPetFoodMill;
			m_pWndPetFoodMill->Initialize(&g_WndMng, APP_PET_FOOD);
			m_pWndPetFoodMill->SetItem(pItemElem->m_dwObjId);
		}
		return TRUE;
	case II_SYS_SYS_SCR_PET_LIFE:
		SAFE_DELETE(m_pWndPetLifeConfirm);
		m_pWndPetLifeConfirm = new CWndPetLifeConfirm;
		m_pWndPetLifeConfirm->SetItem(pItemElem->m_dwObjId);
		m_pWndPetLifeConfirm->Initialize(&g_WndMng, APP_MESSAGEBOX);
		return TRUE;
	case II_GEN_TOO_COL_NORMALBATTERY:
		SAFE_DELETE(m_pWndCommItemDlg);
		m_pWndCommItemDlg = new CWndCommItemDlg;
		m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
		m_pWndCommItemDlg->SetItem(TID_GAME_BETTERY_DESC, pItemElem->m_dwObjId);
		return TRUE;
	case II_GEN_TOO_COL_BATTERY001:
	case II_GEN_TOO_COL_SILVERBATTERY:
	case II_GEN_TOO_COL_GOLDBATTERY:
		SAFE_DELETE(m_pWndCommItemDlg);
		m_pWndCommItemDlg = new CWndCommItemDlg;
		m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
		m_pWndCommItemDlg->SetItem(TID_GAME_CACHE_BETTERY_DESC, pItemElem->m_dwObjId);
		return TRUE;
	case II_SYS_SYS_SCR_SUPERSMELTING:
	{
		SAFE_DELETE(m_pWndCommItemDlg);
		m_pWndCommItemDlg = new CWndCommItemDlg;
		m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
		m_pWndCommItemDlg->SetItem(TID_GAME_LV7OVER_NOTUSE, pItemElem->m_dwObjId);
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_SMELPROT:
	case II_SYS_SYS_SCR_SMELPROT3:
	case II_SYS_SYS_SCR_SMELPROT4:
	{
		SAFE_DELETE(m_pWndCommItemDlg);
		m_pWndCommItemDlg = new CWndCommItemDlg;
		m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
		m_pWndCommItemDlg->SetItem(TID_GAME_REGARDLESS_USE, pItemElem->m_dwObjId);
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_SMELTING:
	{
		SAFE_DELETE(m_pWndCommItemDlg);
		m_pWndCommItemDlg = new CWndCommItemDlg;
		m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
		m_pWndCommItemDlg->SetItem(TID_GAME_LV7OVER_NOTUSE, pItemElem->m_dwObjId);
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_SMELTING2:
	{
		SAFE_DELETE(m_pWndCommItemDlg);
		m_pWndCommItemDlg = new CWndCommItemDlg;
		m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
		m_pWndCommItemDlg->SetItem(TID_GAME_LV10OVER_NOTUSE, pItemElem->m_dwObjId);
		return TRUE;
	}
	break;
	case II_SYS_SYS_SCR_RETURN:
	{
		if (g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_RETURN))
		{
			PutString(prj.GetText(TID_GAME_LIMITED_USE), NULL, prj.GetTextColor(TID_GAME_LIMITED_USE));
			return TRUE;
		}

		if (!m_pWndMessageBox)
		{
			CReturnScrollMsgBox* pMsgBox = new CReturnScrollMsgBox;
			g_DPlay.m_dwReturnScroll = pItemElem->m_dwObjId;
			OpenCustomBox("", pMsgBox);
		}
		return TRUE;
	}
	break;
	}

	if (pProp->IsVis() && g_pPlayer->HasActivatedVisPet())
	{
		if (!m_pWndBuffPetStatus)
		{
#ifdef __AIO_PETS
			g_Option.m_bHideWindowLooter = FALSE;
			PutString("Window activated. You can deactivate it again by clicking on the pet -> Toggle Buff Pet Status, or on close button in window.");
#endif
			m_pWndBuffPetStatus = new CWndBuffPetStatus;
			m_pWndBuffPetStatus->Initialize();
		}

		m_pWndBuffPetStatus->DoModal_ConfirmQuestion(pItemElem->m_dwObjId, g_pPlayer->GetId(), pProp->dwID);
		return TRUE;
	}

#ifdef __AZRIA_1023
	if (!pItemElem->IsFlag(CItemElem::expired))
	{
		TicketProp* pTicketProp = CTicketProperty::GetInstance()->GetTicketProp(pProp->dwID);
		if (pTicketProp && g_pPlayer->GetWorld())
		{
			if (g_pPlayer->GetWorld()->GetID() == pTicketProp->dwWorldId)
			{
				SAFE_DELETE(m_pWndCommItemDlg);
				m_pWndCommItemDlg = new CWndCommItemDlg;
				m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
				m_pWndCommItemDlg->SetItem(TID_GAME_TICKET_DESC, pItemElem->m_dwObjId);
			}
			else
			{
				int nExpand = CTicketProperty::GetInstance()->GetExpanedLayer(pTicketProp->dwWorldId);
				SAFE_DELETE(m_pWndSelectCh);
				m_pWndSelectCh = new CWndSelectCh(pItemElem->m_dwObjId, nExpand);
				m_pWndSelectCh->Initialize(&g_WndMng);
			}
			return TRUE;
		}
	}
#endif	// __AZRIA_1023

	if (pProp->dwItemKind3 == IK3_ELECARD
		|| pProp->dwItemKind3 == IK3_SOCKETCARD
		|| pProp->dwItemKind3 == IK3_SOCKETCARD2
		|| pProp->dwItemKind3 == IK3_ENCHANT
		|| pProp->dwItemKind3 == IK3_RANDOM_SCROLL
		|| pProp->dwItemKind3 == IK3_PIERDICE
		|| pProp->IsNeedTarget()
		)
	{
		CWndInventory* pWndInventory = (CWndInventory*)CreateApplet(APP_INVENTORY);
		if (pWndInventory)
		{
			pWndInventory->m_bIsUpgradeMode = TRUE;
			pWndInventory->m_pUpgradeMaterialItem = pItemElem;

			pWndInventory->DeleteUpgradeSfx();
			pWndInventory->m_pSfxUpgrade = CreateSfx(g_Neuz.m_pd3dDevice, XI_INT_INCHANT, g_pPlayer->GetPos(), g_pPlayer->GetId(), g_pPlayer->GetPos(), g_pPlayer->GetId(), -1);
		}
		return TRUE;
	}

	if (pProp->dwItemKind3 == IK3_CREATE_MONSTER)
	{
		if (!g_Neuz.m_pCreateMonItem)
		{
			g_Neuz.m_pCreateMonItem = pItemElem;
			CreateSfx(g_Neuz.m_pd3dDevice, XI_CHR_CURSOR1, g_pPlayer->GetPos(), g_pPlayer->GetId(), g_pPlayer->GetPos(), g_pPlayer->GetId(), -1);
		}
		return TRUE;
	}

#ifdef __FL_VENDORS
	if (pProp->dwItemKind3 == IK3_VENDOR)
	{
		ObjectExecutor(SHORTCUT_APPLET, APP_VENDOR_REVISION);
		return TRUE;
	}
#endif

#ifdef __FL_NEW_BOXES
	auto vecItems = CNewBoxes::GetInstance()->GetBoxElements(pProp->dwID);
	if (vecItems)
	{
		SAFE_DELETE(m_pWndNewBoxOpen);
		m_pWndNewBoxOpen = new CWndNewBoxOpen(pItemElem, vecItems);
		m_pWndNewBoxOpen->Initialize(&g_WndMng);
		return TRUE;
	}
#endif

#ifdef __FL_PACKED_ITEMS
	if (pItemElem->IsPackage())
	{
		CWndPackedItems* pWnd = (CWndPackedItems*)CreateApplet(APP_PACKED_ITEMS);
		if (pWnd)
			pWnd->SetItem(pItemElem);

		return TRUE;
	}
#endif

	return FALSE;
}

BOOL CWndMgr::UseItem_SpecificSituations(CItemElem* pItemElem, ItemProp* pProp)
{
	if (pProp->dwParts == NULL_ID && pItemElem->IsFlag(CItemElem::expired))
	{
		if (pProp->dwItemKind3 == IK3_EGG)
			PutString(prj.GetText(TID_GAME_PET_DEAD), NULL, prj.GetTextColor(TID_GAME_PET_DEAD));
		else
			PutString(prj.GetText(TID_GAME_ITEM_EXPIRED), NULL, prj.GetTextColor(TID_GAME_ITEM_EXPIRED));

		return TRUE;
	}

	if (pProp->dwParts != NULL_ID)
	{
		if (GetWndBase(APP_CHANGESEX))
		{
			PutString(prj.GetText(TID_GAME_DONOTUSE_SHOP), NULL, prj.GetTextColor(TID_GAME_DONOTUSE_SHOP));
			return TRUE;
		}
	}

	if (pProp->dwID == II_SYS_SYS_SCR_PIEPROT)
	{
		PutString(prj.GetText(TID_GAME_ONLYPIER_USE), NULL, prj.GetTextColor(TID_GAME_ONLYPIER_USE));
		return TRUE;
	}

	if (g_pPlayer->HasBuffByIk3(IK3_TEXT_DISGUISE))
	{
		if (pProp->dwParts == PARTS_RIDE)
		{
			PutString(prj.GetText(TID_QUEST_DISQUISE_NOTFLY), NULL, prj.GetTextColor(TID_QUEST_DISQUISE_NOTFLY));
			return TRUE;
		}
		else if (pProp->dwID == II_CHR_MAG_TRI_ANGELWING || pProp->dwID == II_SYS_SYS_EVE_WINGS)
		{
			PutString(prj.GetText(TID_GAME_DISQUISE_DNT_WING), NULL, prj.GetTextColor(TID_GAME_DISQUISE_DNT_WING));
			return TRUE;
		}
	}

	if (g_pPlayer->HasBuff(BUFF_ITEM, II_CHR_MAG_TRI_ANGELWING) || g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_EVE_WINGS))
	{
		if (pProp->dwItemKind3 == IK3_TEXT_DISGUISE)
		{
			PutString(prj.GetText(TID_GAME_DISQUISE_DNT_WING), NULL, prj.GetTextColor(TID_GAME_DISQUISE_DNT_WING));
			return TRUE;
		}

		BOOL check = FALSE;
		if (g_pPlayer->HasBuff(BUFF_ITEM, II_CHR_MAG_TRI_ANGELWING))
		{
			if (pProp->dwID == II_SYS_SYS_EVE_WINGS)
				check = TRUE;
		}
		else if (g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_EVE_WINGS))
		{
			if (pProp->dwID == II_CHR_MAG_TRI_ANGELWING)
				check = TRUE;
		}

		if (check)
		{
			PutString(prj.GetText(TID_GAME_NOTUSESKILL_ITEMUSE), NULL, prj.GetTextColor(TID_GAME_NOTUSESKILL_ITEMUSE));
			return TRUE;
		}
	}

	if (pProp->dwExeTarget == EXT_PET)
	{
		CPet* pPet = g_pPlayer->GetPet();
		if (!pPet)
		{
			PutString(prj.GetText(TID_GAME_PET_NOT_FOUND), NULL, prj.GetTextColor(TID_GAME_PET_NOT_FOUND));
			return TRUE;
		}
	}

	if (pProp->dwItemKind2 == IK2_BLINKWING)
	{
		if (g_pPlayer->GetWorld()->m_dwWorldID == WI_WORLD_KEBARAS
			|| g_pPlayer->GetWorld()->m_dwWorldID == WI_WORLD_GUILDWAR
#ifdef __FL_FFA
			|| g_pPlayer->GetWorld()->m_dwWorldID == WI_WORLD_FFA
#endif
#ifdef __FL_TDM
			|| g_pPlayer->GetWorld()->m_dwWorldID == WI_WORLD_TDM
#endif
			|| g_pPlayer->GetWorld()->m_dwWorldID == WI_WORLD_MINIROOM
			)
		{
			PutString(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE));
			return TRUE;
		}
	}

	return FALSE;
}

void CWndMgr::RenderObjectViewport(C2DRender* p2DRender, CModelObject* m_pModel, CMover* pMover, LPWNDCTRL lpFace, VIEWPORT* data, int nWindowID)
{
	if (!p2DRender)
		return;

	if (!m_pModel)
		return;

	if (!lpFace)
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	if (!pd3dDevice)
		return;

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
#ifndef __TEST_ANISOTROPIC
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255, 255, 255, 255));

	D3DVIEWPORT9 viewport;
	viewport.X = p2DRender->m_ptOrigin.x + lpFace->rect.left;
	viewport.Y = p2DRender->m_ptOrigin.y + lpFace->rect.top;
	viewport.Width = lpFace->rect.Width();
	viewport.Height = lpFace->rect.Height();
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	pd3dDevice->SetViewport(&viewport);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0);

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;
	D3DXMATRIXA16 matProj;

	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matProj);

	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt = data->vecLookAt;
	D3DXVECTOR3 vecPos = data->vecPos;
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

	D3DXMatrixLookAtLH(&matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	if (nWindowID == APP_SMELT_JEWEL)
	{
		D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane);
		D3DXMatrixRotationX(&matRot, D3DXToRadian(data->m_fRot));
	}
	else
	{
		FLOAT fov = D3DX_PI / data->m_fFov;
		FLOAT h = cos(fov / 2) / sin(fov / 2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH(&matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane);
		D3DXMatrixRotationY(&matRot, D3DXToRadian(data->m_fRot));
	}


	pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixScaling(&matScale, data->vecMatrixScaling.x, data->vecMatrixScaling.y, data->vecMatrixScaling.z);
	D3DXMatrixTranslation(&matTrans, data->vecMatrixTranslation.x, data->vecMatrixTranslation.y, data->vecMatrixTranslation.z);

	D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matRot);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	::SetLight(FALSE);
	::SetFog(FALSE);
	SetDiffuse(1.0f, 1.0f, 1.0f);
	SetAmbient(1.0f, 1.0f, 1.0f);

	D3DXVECTOR4 vConst(1.0f, 1.0f, 1.0f, 1.0f);
	pd3dDevice->SetVertexShaderConstantF(95, (float*)&vConst, 1);
	::SetTransformView(matView);
	::SetTransformProj(matProj);

	if (pMover)
	{
		if (nWindowID != APP_BEAUTY_SHOP_EX)
		{
			O3D_ELEMENT* pElem = m_pModel->GetParts(PARTS_HAIR);
			if (pElem && pElem->m_pObject3D)
			{
				pElem->m_pObject3D->m_fAmbient[0] = pMover->m_fHairColorR;
				pElem->m_pObject3D->m_fAmbient[1] = pMover->m_fHairColorG;
				pElem->m_pObject3D->m_fAmbient[2] = pMover->m_fHairColorB;
			}
		}

		PrepareMoverModel(pMover, m_pModel);
	}

	m_pModel->SetGroup(0);
	m_pModel->Render(p2DRender->m_pd3dDevice, &matWorld);

	viewport.X = p2DRender->m_ptOrigin.x;
	viewport.Y = p2DRender->m_ptOrigin.y;
	viewport.Width = p2DRender->m_clipRect.Width();
	viewport.Height = p2DRender->m_clipRect.Height();
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
}

void CWndMgr::PrepareMoverModel(CMover* pMover, CModelObject* pModel)
{
	// todo: move this function to other place 
	if (!pMover || !pModel)
		return;

	if (pMover->IsDisguise())
		return;

	pMover->OverCoatItemRenderCheck(pModel);

	int nVisibleParts = PARTS_HAT;
	int nInVisibleParts = PARTS_CAP;
	if (!pMover->IsOptionRenderCostume(0))
	{
		nVisibleParts = PARTS_CAP;
		nInVisibleParts = 0;
	}

	ItemProp* pItemProp = NULL;
	if (pMover->IsActiveMover())
	{
		CItemElem* pItemElem = pMover->GetEquipItem(nInVisibleParts);
		O3D_ELEMENT* pElement = pModel->GetParts(PARTS_HAIR);
		if (pElement)
			pElement->m_nEffect &= ~XE_HIDE;

		if (pItemElem)
		{
			CItemElem* pItemElemOvercoat = pMover->GetEquipItem(nVisibleParts);
			ItemProp* pItemProp = pItemElem->GetProp();
			if (pItemProp && pItemProp->dwBasePartsIgnore != -1)
			{
				if (pItemElem->GetLook() == NULL_ID && pItemElemOvercoat == NULL)
					pElement = pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

			if (pItemElemOvercoat == NULL)
				pItemElemOvercoat = pItemElem;

			if (pItemElemOvercoat)
			{
				if (!pItemElemOvercoat->IsFlag(CItemElem::expired))
				{
					ItemProp* pItemPropOC = NULL;
					if (pItemElemOvercoat->GetLook() != NULL_ID)
						pItemPropOC = prj.GetItemProp(pItemElemOvercoat->GetLook());
					else
						pItemPropOC = pItemElemOvercoat->GetProp();

					if (pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1)
					{
						if (pItemPropOC->dwBasePartsIgnore == PARTS_HEAD)
							pModel->SetEffect(PARTS_HAIR, XE_HIDE);

						pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
					else
					{
						if (pElement)
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}

		}
		else
		{
			CItemElem* pItemElemOvercoat = pMover->GetEquipItem(nVisibleParts);
			if (pItemElemOvercoat)
			{
				if (!pItemElemOvercoat->IsFlag(CItemElem::expired))
				{
					ItemProp* pItemPropOC = NULL;
					if (pItemElemOvercoat->GetLook() != NULL_ID)
						pItemPropOC = prj.GetItemProp(pItemElemOvercoat->GetLook());
					else
						pItemPropOC = pItemElemOvercoat->GetProp();

					if (pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1)
					{
						if (pItemPropOC->dwBasePartsIgnore == PARTS_HEAD)
							pModel->SetEffect(PARTS_HAIR, XE_HIDE);

						pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
				}
			}
		}
	}
	else
	{
		DWORD dwId = pMover->m_aEquipInfo[nInVisibleParts].dwId;
		O3D_ELEMENT* pElement = NULL;

		if (dwId != NULL_ID)
		{
			ItemProp* pItemProp = prj.GetItemProp(dwId);
			if (pItemProp && pItemProp->dwBasePartsIgnore != NULL_ID)
			{
				if (pMover->m_aEquipInfo[nInVisibleParts].dwLook == NULL_ID && pMover->m_aEquipInfo[nVisibleParts].dwId == NULL_ID)
					pElement = pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

			DWORD dwParts = nVisibleParts;
			dwId = pMover->m_aEquipInfo[dwParts].dwId;

			if (dwId == NULL_ID)
			{
				dwParts = nInVisibleParts;
				dwId = pMover->m_aEquipInfo[dwParts].dwId;

				if (pMover->m_aEquipInfo[dwParts].dwLook != NULL_ID)
					dwId = pMover->m_aEquipInfo[dwParts].dwLook;
			}
			else
			{
				if (pMover->m_aEquipInfo[dwParts].dwLook != NULL_ID)
					dwId = pMover->m_aEquipInfo[dwParts].dwLook;
			}

			if (dwId != NULL_ID)
			{
				if (!(pMover->m_aEquipInfo[dwParts].byFlag & CItemElem::expired))
				{
					pItemProp = prj.GetItemProp(dwId);
					if (pItemProp && pItemProp->dwBasePartsIgnore != NULL_ID)
					{
						if (pItemProp->dwBasePartsIgnore == PARTS_HEAD)
							pModel->SetEffect(PARTS_HAIR, XE_HIDE);

						pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
					else
					{
						if (pElement)
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}
		}
		else
		{
			dwId = pMover->m_aEquipInfo[nVisibleParts].dwId;
			if (dwId != NULL_ID)
			{
				if (!(pMover->m_aEquipInfo[nVisibleParts].byFlag & CItemElem::expired))
				{
					if (pMover->m_aEquipInfo[nVisibleParts].dwLook != NULL_ID)
						dwId = pMover->m_aEquipInfo[nVisibleParts].dwLook;

					pItemProp = prj.GetItemProp(dwId);
					if (pItemProp && pItemProp->dwBasePartsIgnore != NULL_ID)
					{
						if (pItemProp->dwBasePartsIgnore == PARTS_HEAD)
							pModel->SetEffect(PARTS_HAIR, XE_HIDE);

						pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
					else
					{
						if (pElement)
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}
		}
	}
}

void CWndMgr::UpdateWindowModels()
{
	if (!g_pPlayer)
		return;

	if (g_pBipedMesh)
		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_aEquipInfo, g_pBipedMesh, &g_pPlayer->m_Inventory);

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	if (pWndInventory)
		pWndInventory->UpdateParts();

	CWndBeautyShop* pWndBeautyShop = (CWndBeautyShop*)GetWndBase(APP_BEAUTY_SHOP_EX);
	if (pWndBeautyShop)
		pWndBeautyShop->UpdateModels();

	CWndFaceShop* pWndFaceShop = (CWndFaceShop*)GetWndBase(APP_BEAUTY_SHOP_SKIN);
	if (pWndFaceShop)
		pWndFaceShop->UpdateModels();

	CWndChangeSex* pWndChangeSex = (CWndChangeSex*)GetWndBase(APP_CHANGESEX);
	if (pWndChangeSex)
		pWndChangeSex->UpdateModel();

#ifdef __FL_SWITCH_EQ
	CWndSwitchEquip* pWndSwitch = (CWndSwitchEquip*)GetWndBase(APP_EQ_SWITCH);
	if (pWndSwitch)
		pWndSwitch->UpdateParts();
#endif

}

BOOL CWndMgr::IsWindowProcessState()
{
	if (g_pPlayer)
		return TRUE;

	if (GetWndBase(APP_LOGIN)
		|| GetWndBase(APP_SELECT_SERVER)
		|| GetWndBase(APP_SELECT_CHAR)
		|| GetWndBase(APP_CREATE_CHAR)
		)
		return TRUE;

	return FALSE;
}

#ifdef __FL_FIX_SHORTCUTCRASH
void CWndMgr::CleanGlobalShortcut(CItemElem* pItemElem)
{
	if (m_GlobalShortcut.m_dwData == 0)
		return;

	if (m_GlobalShortcut.m_dwType != ITYPE_ITEM)
		return;

	CItemElem* pItemShortcut = (CItemElem*)m_GlobalShortcut.m_dwData;
	if (!pItemShortcut)
		return;

	if (pItemShortcut == pItemElem)
		m_GlobalShortcut.Empty();
}
#endif

int CWndMgr::GetContainerTypeByWindowId(int nWindowId, int nZoneId)
{
	switch (nWindowId)
	{
	case APP_INVENTORY: return TYPE_INVENTORY;
	case APP_CHEST: return TYPE_CHEST;
	case APP_COMMON_BANK:
	{
		switch (nZoneId)
		{
		case WIDC_BANK_0:  return TYPE_BANK_0;
		case WIDC_BANK_1:  return TYPE_BANK_1;
		case WIDC_BANK_3:  return TYPE_BANK_2;
		}
	}
	break;
	default: return -1;
	}

	return -1;
}

CString CWndMgr::GetStringNameByContainerType(int nType)
{
	switch (nType)
	{
	case TYPE_INVENTORY: return "Inventory";
	case TYPE_CHEST: return "Chest";
	case TYPE_BANK_0: return "Bank 1";
	case TYPE_BANK_1: return "Bank 2";
	case TYPE_BANK_2: return "Bank 3";
	default: return "Error";
	}

	return "Error";
}

void CWndMgr::MakeNumberSmooth(int* nCurVal, int* nOldVal)
{
	if (*nOldVal < *nCurVal)
	{
		int nWidthMinusOld = *nCurVal - *nOldVal;
		nWidthMinusOld = nWidthMinusOld / 5;
		if (nWidthMinusOld <= 1)
			nWidthMinusOld = 1;

		*nCurVal = *nOldVal + (*nCurVal - *nOldVal > nWidthMinusOld ? nWidthMinusOld : *nCurVal - *nOldVal);
		*nOldVal = *nCurVal;
	}
	else if (*nOldVal > *nCurVal)
	{
		int nWidthMinusOld = *nOldVal - *nCurVal;
		nWidthMinusOld = nWidthMinusOld / 5;
		if (nWidthMinusOld <= 1)
			nWidthMinusOld = 1;

		*nCurVal = *nOldVal - (*nOldVal - *nCurVal > nWidthMinusOld ? nWidthMinusOld : *nOldVal - *nCurVal);
		*nOldVal = *nCurVal;
	}
}

void CWndMgr::MakeNumberSmooth64(__int64* nCurVal, __int64* nOldVal)
{
	if (*nOldVal < *nCurVal)
	{
		__int64 nWidthMinusOld = *nCurVal - *nOldVal;
		nWidthMinusOld = nWidthMinusOld / 5;
		if (nWidthMinusOld <= 1)
			nWidthMinusOld = 1;

		*nCurVal = *nOldVal + (*nCurVal - *nOldVal > nWidthMinusOld ? nWidthMinusOld : *nCurVal - *nOldVal);
		*nOldVal = *nCurVal;
	}
	else if (*nOldVal > *nCurVal)
	{
		__int64 nWidthMinusOld = *nOldVal - *nCurVal;
		nWidthMinusOld = nWidthMinusOld / 5;
		if (nWidthMinusOld <= 1)
			nWidthMinusOld = 1;

		*nCurVal = *nOldVal - (*nOldVal - *nCurVal > nWidthMinusOld ? nWidthMinusOld : *nOldVal - *nCurVal);
		*nOldVal = *nCurVal;
	}
}

#ifdef __FL_PACKED_ITEMS
void CWndMgr::PutPackage(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit, int nFlag)
{
	if (!pItemElem)
		return;

	pEdit->AddString("\nSHIFT + Double Click to see details.", dwItemColor.dwGeneral);

	auto vec = pItemElem->GetItemList();
	if (!vec->size())
	{
		pEdit->AddString("\nPackage is empty.", dwItemColor.dwGeneral);
		return;
	}
}
void CWndMgr::PreviewPackage(CItemElem* pItemElem)
{
	if (!pItemElem || !pItemElem->IsPackage())
		return;

	SAFE_DELETE(m_pWndPackagePreview);
	m_pWndPackagePreview = new CWndPackedItemsPreview;
	m_pWndPackagePreview->Initialize();
	m_pWndPackagePreview->SetItem(pItemElem);
}
#endif

#ifdef __FL_MONSTER_ALBUM
void CWndMgr::PutMonsterCard(ItemProp* pItemProp, CEditString* pEdit)
{
	if (!pItemProp || !pEdit)
		return;

	if (pItemProp->dwItemKind3 != IK3_MONSTER_ALBUM)
		return;

	pEdit->AddString("\nMonster card piece.", dwItemColor.dwGeneral);

	DWORD dwLinkedMobID = pItemProp->dwAbilityMin;
	if (dwLinkedMobID == NULL_ID)
		return;

	MoverProp* pMobProp = prj.GetMoverProp(dwLinkedMobID);
	if (!pMobProp)
		return;

	CString str;

	pEdit->AddString("\nMonster: ", dwItemColor.dwGeneral, ESSTY_BOLD);
	str.Format("%s (Lvl: %d)", pMobProp->szName, pMobProp->dwLevel);
	pEdit->AddString(str);

	const int nProgress = g_pPlayer->m_MonsterAlbum.GetMonsterProgress(dwLinkedMobID);

	pEdit->AddString("\nProgress: ", dwItemColor.dwGeneral, ESSTY_BOLD);
	str.Format("%d / %d", nProgress, CARDS_NEEDED_TO_UNLOCK);
	pEdit->AddString(str);
}
void CWndMgr::PutBossCard(ItemProp* pItemProp, CEditString* pEdit)
{
	if (!pItemProp || !pEdit)
		return;

	if (pItemProp->dwItemKind3 != IK3_MONSTER_ALBUM_BOSS_CARD)
		return;

	pEdit->AddString("\nBoss card.", dwItemColor.dwGeneral);

	PutDestParam(pItemProp, NULL, pEdit);
}
#endif