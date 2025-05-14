#include "stdafx.h"
#include "defineText.h"
#include "defineSound.h"

#include "defineItem.h"
#include "defineSkill.h"

#include "appdefine.h"
#include "dploginclient.h"
#include "dpCertified.h"
#include "dpclient.h"
#include "MsgHdr.h"
#include "dialogmsg.h"
#include "misc.h"
#include "defineObj.h"
#include "wndmessenger.h"
#include "WndQuest.h"
#include "mover.h"
#include "Ship.h"
#include "Sfx.h"

#include "WndMapEx.h"

#include "wndvendor.h"
#include "..\_Network\ErrorCode.h"
#include "wndchangeface.h"
#include "definequest.h"
#include "webbox.h"
#include "WndParty.h"

#ifdef __AZRIA_1023
#include "ticket.h"
#endif	// __AZRIA_1023

#ifdef __FL_BATTLE_PASS
#include "BattlePass.h"
#endif

#ifdef __EVE_MINIGAME
#include "WndMiniGame.h"
#endif //__EVE_MINIGAME

#include "path.h"

#ifdef __FL_CASH_SHOP
#include "FlyFFShop.h"
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
#include "WheelOfFortune.h"
#endif

#include "WndSummonFriend.h"

#include "tools.h"

#include "eveschool.h"
#include "CommonCtrl.h"
#include "yUtil.h"
extern CFLASH g_Flash;

#include "party.h"
extern	CParty g_Party;
#include "wndpartyconfirm.h"
#include "Environment.h"

#if __VER >= 12 // __LORD
#include "clord.h"
#endif	// __LORD

#include "guild.h"
#include "guildwar.h"
extern	CGuildMng	g_GuildMng;

#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;

#include "Chatting.h"
extern	CChatting g_Chatting;

#include "WndFriendConfirm.h"
#include "WndPartyChangeTroup.h"
#include "Network.h"
#include "WndSelectVillage.h"


extern	CDPCertified	g_dpCertified;
extern	CDPLoginClient	g_dpLoginClient;

#include "post.h"
#include "spevent.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 11 // __SYS_POCKET
#include "WndBagEx.h"
#endif	// __SYS_POCKET

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#if __VER >= 13 // __HOUSING
#include "Housing.h"
#include "WndHousing.h"
#endif // __HOUSING

#if __VER >= 13 // __HONORABLE_TITLE
#include "honor.h"
#endif	// __HONORABLE_TITLE
#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#include "WndGuildHouse.h"
#endif // __GUILD_HOUSE


BOOL	s_f = FALSE;
BOOL	fJoin = FALSE;

CDPClient::CDPClient()
{
	m_lError = 0;
	m_dwReturnScroll = 0;

	BEGIN_MSG;
	ON_MSG(PACKETTYPE_JOIN, &CDPClient::OnJoin);
	ON_MSG(PACKETTYPE_SNAPSHOT, &CDPClient::OnSnapshot);
	ON_MSG(PACKETTYPE_ERROR, &CDPClient::OnError);
	ON_MSG(PACKETTYPE_WHISPER, &CDPClient::OnWhisper);
	ON_MSG(PACKETTYPE_SAY, &CDPClient::OnSay);
	ON_MSG(PACKETTYPE_GMSAY, &CDPClient::OnGMSay);
	ON_MSG(PACKETTYPE_GETPLAYERADDR, &CDPClient::OnGetPlayerAddr);
	ON_MSG(PACKETTYPE_GETPLAYERCOUNT, &CDPClient::OnGetPlayerCount);
	ON_MSG(PACKETTYPE_GETCOREPLAYER, &CDPClient::OnGetCorePlayer);
	ON_MSG(PACKETTYPE_SYSTEM, &CDPClient::OnSystem);
	ON_MSG(PACKETTYPE_CAPTION, &CDPClient::OnCaption);

	ON_MSG(PACKETTYPE_DEFINEDTEXT, &CDPClient::OnDefinedText);
	ON_MSG(PACKETTYPE_GET_CLOCK, &CDPClient::OnGetClock);
	ON_MSG(PACKETTYPE_ADDFRIENDJOIN, &CDPClient::OnFriendJoin);
	ON_MSG(PACKETTYPE_ADDFRIENDLOGOUT, &CDPClient::OnFriendLogOut);
	ON_MSG(PACKETTYPE_FRIENDNOINTERCEPT, &CDPClient::OnFriendNoIntercept);
	ON_MSG(PACKETTYPE_FRIENDINTERCEPTSTATE, &CDPClient::OnFriendIntercept);
	ON_MSG(PACKETTYPE_GETFRIENDSTATE, &CDPClient::OnGetFriendState);
	ON_MSG(PACKETTYPE_SETFRIENDSTATE, &CDPClient::OnSetFriendState);
	ON_MSG(PACKETTYPE_ONEFRIEMDSTATE, &CDPClient::OnOneFriendState);
	ON_MSG(PACKETTYPE_GAMERATE, &CDPClient::OnGameRate);
	ON_MSG(PACKETTYPE_ERRORPARTY, &CDPClient::OnErrorParty);
	ON_MSG(PACKETTYPE_GUILD, &CDPClient::OnGuild);
	ON_MSG(PACKETTYPE_ADD_GUILD_MEMBER, &CDPClient::OnAddGuildMember);
	ON_MSG(PACKETTYPE_REMOVE_GUILD_MEMBER, &CDPClient::OnRemoveGuildMember);
	ON_MSG(PACKETTYPE_GUILD_CHAT, &CDPClient::OnGuildChat);
	ON_MSG(PACKETTYPE_GUILD_MEMBER_LEVEL, &CDPClient::OnGuildMemberLv);
	ON_MSG(PACKETTYPE_GUILD_CLASS, &CDPClient::OnGuildClass);
	ON_MSG(PACKETTYPE_GUILD_NICKNAME, &CDPClient::OnGuildNickName);
	ON_MSG(PACKETTYPE_CHG_MASTER, &CDPClient::OnChgMaster);
	ON_MSG(PACKETTYPE_GUILD_GAMELOGIN, &CDPClient::OnGuildMemberLogin);
	ON_MSG(PACKETTYPE_GUILD_GAMEJOIN, &CDPClient::OnGuldMyGameJoin);
	ON_MSG(PACKETTYPE_GUILD_SETNAME, &CDPClient::OnGuildSetName);
	ON_MSG(PACKETTYPE_GUILD_ERROR, &CDPClient::OnGuildError);
	ON_MSG(PACKETTYPE_ADDFRIENDNAMEREQEST, &CDPClient::OnAddFriendNameReqest);
	ON_MSG(PACKETTYPE_ADDFRIENDNAMENOTFOUND, &CDPClient::OnAddFriendNotConnect);
	ON_MSG(PACKETTYPE_REMOVEFRIENDSTATE, &CDPClient::OnRemoveFriendState);
	ON_MSG(PACKETTYPE_BLOCK, &CDPClient::OnBlock);
	ON_MSG(PACKETTYPE_KEEP_ALIVE, &CDPClient::OnKeepAlive);

	ON_MSG(PACKETTYPE_QUERYSETPLAYERNAME, &CDPClient::OnSetPlayerName);
	ON_MSG(PACKETTYPE_SNOOP, &CDPClient::OnSnoop);

#if __VER >= 11 // __SYS_PLAYER_DATA
	ON_MSG(PACKETTYPE_UPDATE_PLAYER_DATA, &CDPClient::OnUpdatePlayerData);
	ON_MSG(PACKETTYPE_LOGOUT, &CDPClient::OnLogout);
#endif	// __SYS_PLAYER_DATA

	memset((void*)&m_ss, 0, sizeof(SNAPSHOT));
	memset((void*)&m_pa, 0, sizeof(PLAYERANGLE));

	m_nMaxLoginGuild = 0;
	memset(m_uLoginPlayerIdGuild, 0, sizeof(m_uLoginPlayerIdGuild));
	memset(m_uLoginGuildMulti, 0, sizeof(m_uLoginGuildMulti));

	m_pDump = NULL;
	m_nDumpSize = 0;
}

CDPClient::~CDPClient()
{

}

LONG CDPClient::GetNetError()
{
	return m_lError;
}

void CDPClient::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	switch (lpMsg->dwType)
	{
	case DPSYS_DESTROYPLAYERORGROUP:
	{
#ifdef __INFO_SOCKLIB0516
		{
			LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
			m_lError = lpDestroyPlayer->dwFlags;
		}
#endif

		m_fConn = FALSE;
		if (s_f)
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0017)));
		s_f = FALSE;
		break;
	}
	}
}

void CDPClient::UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	m_pDump = (BYTE*)lpMsg;
	m_nDumpSize = dwMsgSize;

#ifdef __TEST_PACKET_CRYPT
	BYTE* pData = (BYTE*)lpMsg;
	for (DWORD i = 0; i < dwMsgSize; i++)
	{
		pData[i] = pData[i] ^ ((BYTE)(i % sizeof(BYTE)) ^ 171);
		pData[i] = pData[i] ^ ((sizeof(BYTE) - (BYTE)(i % sizeof(BYTE))) ^ 84);
	}
#endif

	CAr ar((LPBYTE)lpMsg, dwMsgSize);
	GETTYPE(ar);
	void (theClass:: * pfn)(theParameters) = GetHandler(dw);

	if (pfn) {
		(this->*(pfn))(ar);
	}

	m_pDump = NULL;
	m_nDumpSize = 0;
}

#ifdef __TEST_PACKET_CRYPT
BOOL CDPClient::Send(LPVOID lpData, DWORD dwDataSize, DPID dpidTo)
{
	BYTE* pData = (BYTE*)lpData;
	for (DWORD i = 0; i < dwDataSize; i++)
	{
		pData[i] = pData[i] ^ ((BYTE)(i % sizeof(BYTE)) ^ 171);
		pData[i] = pData[i] ^ ((sizeof(BYTE) - (BYTE)(i % sizeof(BYTE))) ^ 84);
	}
	return CDPMng::Send((LPVOID)pData, dwDataSize, dpidTo);
}
#endif

BOOL CDPClient::Connect(LPSTR lpszAddr, USHORT uPort)
{
	if (g_dpLoginClient.IsConnected() && CreateDPObject(TRUE /*Single threaded*/) == TRUE)
	{
		InitConnection((LPVOID)lpszAddr, uPort);
#ifdef __CRC
		return (m_fConn = JoinSession(NULL, CSock::crcWrite));
#else	// __CRC
		return (m_fConn = JoinSession(NULL));
#endif	// __CRC
	}
	return (m_fConn = FALSE);
}

BYTE g_hdr, g_Prev;

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// Receiver
void CDPClient::OnSnapshot(CAr& ar)
{
	OBJID objid, objidPlayer;
	short cb;
	//	BYTE hdr, prev	= 0x00;
	WORD	hdr, prev = 0x0000;

	ar >> objidPlayer >> cb;
	while (cb--)
	{
		ar >> objid >> hdr;
#ifdef __TRAFIC_1218
		m_traficLog.Add((BYTE)(hdr));
#endif	// __TRAFIC_1218
		g_hdr = (BYTE)(hdr);

#ifdef __AIO_ADMIN_THINGS
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
		if (pWndWorld && pWndWorld->m_bRenderFPS && g_pPlayer && g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR))
		{
			CString str;
			str.Format("%04x", hdr);
			if (strcmp(str, "0001") != 0 && strcmp(str, "00f0") != 0 && strcmp(str, "00c1") != 0 && strcmp(str, "00f1") != 0 && strcmp(str, "001b") != 0 && strcmp(str, "003c") != 0)
				g_WndMng.PutString(str);
		}
#endif

		switch (hdr)
		{
		case SNAPSHOTTYPE_MOVERBEHAVIOR:		OnMoverBehavior(objid, ar);	break;
		case SNAPSHOTTYPE_CREATESFXOBJ:		OnCreateSfxObj(objid, ar);	break;
		case SNAPSHOTTYPE_CREATESFXALLOW:	OnCreateSfxAllow(objid, ar);	break;
		case SNAPSHOTTYPE_DEFINEDTEXT:	OnDefinedText(ar);	break;
		case SNAPSHOTTYPE_CHATTEXT:	OnChatText(ar);	break;
		case SNAPSHOTTYPE_DEFINEDTEXT1: OnDefinedText1(ar);	break;
		case SNAPSHOTTYPE_GAMETIMER:	OnGameTimer(ar);	break;
		case SNAPSHOTTYPE_ADDGAMEJOIN:	OnGameJoin(ar); break;
		case SNAPSHOTTYPE_TASKBAR:		OnTaskBar(ar); break;
		case SNAPSHOTTYPE_ERRORPARTY:	OnErrorParty(ar); break;
		case SNAPSHOTTYPE_PARTYMEMBER:	OnAddPartyMember(ar); break;
		case SNAPSHOTTYPE_PARTYREQEST: OnPartyRequest(ar); break;
		case SNAPSHOTTYPE_PARTYREQESTCANCEL: OnPartyRequestCancel(ar); break;
		case SNAPSHOTTYPE_PARTYEXP: OnPartyExpLevel(ar); break;
		case SNAPSHOTTYPE_PARTYCHANGETROUP: OnPartyChangeTroup(ar); break;
		case SNAPSHOTTYPE_PARTYCHANGENAME: OnPartyChangeName(ar); break;
		case SNAPSHOTTYPE_PARTYSKILL_CALL:	OnPartySkillCall(objid, ar); break;
		case SNAPSHOTTYPE_PARTYSKILL_BLITZ:	OnPartySkillBlitz(ar); break;
		case SNAPSHOTTYPE_PARTYSKILL_RETREAT:	OnPartySkillRetreat(objid); break;
		case SNAPSHOTTYPE_PARTYSKILL_SPHERECIRCLE:	OnPartySkillSphereCircle(objid); break;
		case SNAPSHOTTYPE_SETPARTYMODE:	OnSetPartyMode(ar); break;
		case SNAPSHOTTYPE_PARTYCHANGEITEMMODE: OnPartyChangeItemMode(ar); break;
		case SNAPSHOTTYPE_PARTYCHANGEEXPMODE: OnPartyChangeExpMode(ar); break;

		case SNAPSHOTTYPE_ENVIRONMENTALL: OnEnvironmentSetting(ar); break;
		case SNAPSHOTTYPE_ENVIRONMENT: OnEnvironmentEffect(ar); break;

		case SNAPSHOTTYPE_PARTYCHAT: OnPartyChat(ar); break;
		case SNAPSHOTTYPE_PVENDOR_OPEN:	OnPVendorOpen(objid, ar);	break;
		case SNAPSHOTTYPE_PVENDOR_CLOSE:		OnPVendorClose(objid, ar);	break;
		case SNAPSHOTTYPE_REGISTER_PVENDOR_ITEM:	OnRegisterPVendorItem(objid, ar);	break;
		case SNAPSHOTTYPE_DOAPPLYUSESKILL:	OnDoApplySkill(objid, ar); break;
		case SNAPSHOTTYPE_COMMONSKILL:	OnCommonSkill(objid, ar); break;
		case SNAPSHOTTYPE_UNREGISTER_PVENDOR_ITEM:	OnUnregisterPVendorItem(objid, ar);	break;
		case SNAPSHOTTYPE_PVENDOR_ITEM:				OnPVendorItem(objid, ar);		break;
		case SNAPSHOTTYPE_PVENDOR_ITEM_NUM:			OnPVendorItemNum(objid, ar);	break;
		case SNAPSHOTTYPE_SET_HAIR:					OnSetHair(objid, ar);	break;
		case SNAPSHOTTYPE_ADDPARTYNAME:				OnMyPartyName(ar); break;
		case SNAPSHOTTYPE_SET_PARTY_MEMBER_PARAM:	OnSetPartyMemberParam(ar);	break;
		case SNAPSHOTTYPE_ADDFRIENDGAMEJOIN: OnFriendGameJoin(ar); break;
		case SNAPSHOTTYPE_ADDFRIEND: OnAddFriend(ar); break;
		case SNAPSHOTTYPE_ADDFRIENDREQEST: OnAddFriendReqest(ar); break;
		case SNAPSHOTTYPE_ADDFRIENDCANCEL: OnAddFriendCancel(ar); break;
		case SNAPSHOTTYPE_REMOVEFRIEND:	OnRemoveFriend(ar); break;
		case SNAPSHOTTYPE_ADDFRIENDERROR: OnAddFriendError(ar); break;
		case SNAPSHOTTYPE_SETSKILLSTATE:	OnSetSkillState(ar); break;
		case SNAPSHOTTYPE_ADDPARTYCHANGELEADER: OnPartyChangeLeader(ar); break;
		case SNAPSHOTTYPE_FLYFF_EVENT:	OnFlyffEvent(ar);	break;
		case SNAPSHOTTYPE_GAMERATE:	OnGameRate(ar); break;
#if __VER >= 9 // __EVENTLUA
		case SNAPSHOTTYPE_EVENTLUA_DESC: OnEventLuaDesc(ar); break;
#endif // __EVENTLUA
#if __VER >= 10 // __REMOVE_ATTRIBUTE
		case SNAPSHOTTYPE_REMOVE_ATTRIBUTE: OnRemoveAttributeResult(ar); break;
#endif // __REMOVE_ATTRIBUTE
		case SNAPSHOTTYPE_MOTION_ARRIVE: OnMotionArrive(objid, ar); break;

#ifdef __S1108_BACK_END_SYSTEM
		case SNAPSHOTTYPE_MONSTERPROP:	OnMonsterProp(ar); break;
		case SNAPSHOTTYPE_GMCHAT:		OnGMChat(ar); break;
#endif // __S1108_BACK_END_SYSTEM

		case SNAPSHOTTYPE_CHANGEFACE: OnChangeFace(ar); break;
		case SNAPSHOTTYPE_DEFINEDCAPTION: OnDefinedCaption(ar); break;
		case SNAPSHOTTYPE_SM_MODE:	OnSMMode(objid, ar); break;
		case SNAPSHOTTYPE_SM_MODE_ALL:	OnSMModeAll(ar); break;
		case SNAPSHOTTYPE_PARTYMAPINFO: OnPartyMapInfo(ar); break;
		case SNAPSHOTTYPE_MOVERDEATH:	OnMoverDeath(objid, ar);	break;
		case SNAPSHOTTYPE_ADD_OBJ:	OnAddObj(objid, ar);	break;
		case SNAPSHOTTYPE_DEL_OBJ:	OnRemoveObj(objid);	break;
		case SNAPSHOTTYPE_CHAT:		OnChat(objid, ar);	break;
		case SNAPSHOTTYPE_EVENTMESSAGE:		OnEventMessage(objid, ar); break;

		case SNAPSHOTTYPE_DAMAGE:	OnDamage(objid, ar);	break;
		case SNAPSHOTTYPE_CREATEITEM:	OnCreateItem(objid, ar);	break;
		case SNAPSHOTTYPE_MOVEITEM:		OnMoveItem(ar);	break;
		case SNAPSHOTTYPE_DOEQUIP:	OnDoEquip(objid, ar);	break;
		case SNAPSHOTTYPE_TRADE:	OnTrade(objid, ar);	break;
		case SNAPSHOTTYPE_CONFIRMTRADE: OnConfirmTrade(objid, ar); break;
		case SNAPSHOTTYPE_CONFIRMTRADECANCEL: OnConfirmTradeCancel(objid, ar); break;
		case SNAPSHOTTYPE_TRADEPUT:		OnTradePut(objid, ar);	break;
		case SNAPSHOTTYPE_TRADEPULL:	OnTradePull(objid, ar);	break;
		case SNAPSHOTTYPE_TRADEPUTGOLD:		OnTradePutGold(objid, ar);	break;

		case SNAPSHOTTYPE_TRADECANCEL:	OnTradeCancel(objid, ar);		break;
		case SNAPSHOTTYPE_TRADEOK:		OnTradeOk(objid, ar);		break;
		case SNAPSHOTTYPE_TRADECONSENT:		OnTradeConsent();	break;
		case SNAPSHOTTYPE_TRADELASTCONFIRM:	OnTradelastConfirm();	break;
		case SNAPSHOTTYPE_TRADELASTCONFIRMOK: OnTradelastConfirmOk(objid, ar); break;

		case SNAPSHOTTYPE_SETGUILDQUEST:	OnSetGuildQuest(ar);	break;
		case SNAPSHOTTYPE_REMOVEGUILDQUEST:	OnRemoveGuildQuest(ar);	break;
		case SNAPSHOTTYPE_SETQUEST:	OnSetQuest(objid, ar);		break;

		case SNAPSHOTTYPE_OPENSHOPWND:	OnOpenShopWnd(objid, ar);		break;
		case SNAPSHOTTYPE_PUTITEMBANK:	OnPutItemBank(objid, ar);		break;
		case SNAPSHOTTYPE_GETITEMBANK:	OnGetItemBank(objid, ar);		break;
		case SNAPSHOTTYPE_PUTGOLDBANK:	OnPutGoldBank(objid, ar);		break;
		case SNAPSHOTTYPE_MOVEBANKITEM:	OnMoveBankItem(objid, ar);	break;
		case SNAPSHOTTYPE_BANKISFULL:	OnErrorBankIsFull(objid, ar); break;
		case SNAPSHOTTYPE_BANKWINDOW:	OnBankWindow(objid, ar); break;
		case SNAPSHOTTYPE_REMOVESKILLINFULENCE: OnRemoveSkillInfluence(objid, ar); break;
		case SNAPSHOTTYPE_REMOVEALLSKILLINFULENCE: OnRemoveAllSkillInfluence(objid, ar); break;
		case SNAPSHOTTYPE_GUILD_BANK_WND:	OnGuildBankWindow(objid, ar); break;
		case SNAPSHOTTYPE_PUTITEMGUILDBANK:	OnPutItemGuildBank(objid, ar); break;
		case SNAPSHOTTYPE_GETITEMGUILDBANK: OnGetItemGuildBank(objid, ar); break;
		case SNAPSHOTTYPE_CHANGEBANKPASS: OnChangeBankPass(objid, ar); break;
		case SNAPSHOTTYPE_CONFIRMBANKPASS: OnConfirmBankPass(objid, ar); break;
		case SNAPSHOTTYPE_QUERY_PLAYER_DATA:	OnQueryPlayerData(ar);	break;
		case SNAPSHOTTYPE_FOCUSOBJ:				OnFocusObj(ar);	break;

		case SNAPSHOTTYPE_GUILD_INVITE:	OnGuildInvite(ar);	break;
		case SNAPSHOTTYPE_SET_GUILD:	OnSetGuild(objid, ar);	break;
		case SNAPSHOTTYPE_CREATE_GUILD:	OnCreateGuild(ar);	break;
		case SNAPSHOTTYPE_DESTROY_GUILD:	OnDestroyGuild(ar);	break;
		case SNAPSHOTTYPE_GUILD:			OnGuild(ar);	break;
		case SNAPSHOTTYPE_ALL_GUILDS:		OnAllGuilds(ar);	break;
		case SNAPSHOTTYPE_GUILD_CONTRIBUTION:		OnGuildContribution(ar); break;
		case SNAPSHOTTYPE_GUILD_NOTICE:		OnGuildNotice(ar); break;
		case SNAPSHOTTYPE_GUILD_AUTHORITY:	OnGuildAuthority(ar); break;
		case SNAPSHOTTYPE_SETNAVIPOINT: OnSetNaviPoint(objid, ar);	break;
		case SNAPSHOTTYPE_UPDATE_VENDOR:	OnUpdateVendor(objid, ar);	break;
		case SNAPSHOTTYPE_UPDATE_ITEM:	OnUpdateItem(objid, ar);	break;
		case SNAPSHOTTYPE_UPDATE_ITEM_EX:	OnUpdateItemEx(objid, ar);	break;
		case SNAPSHOTTYPE_SETDESTPARAM:		OnSetDestParam(objid, ar);	break;
		case SNAPSHOTTYPE_RESETDESTPARAM:	OnResetDestParam(objid, ar);	break;
		case SNAPSHOTTYPE_SETPOINTPARAM:	OnSetPointParam(objid, ar);	break;
		case SNAPSHOTTYPE_SETSCALE:		OnSetScale(objid, ar);	break;
#ifndef __S_SERVER_UNIFY
		case SNAPSHOTTYPE_DIAG_TEXT:		OnDiagText(ar);	break;
#endif // __S_SERVER_UNIFY
#ifdef __S_SERVER_UNIFY
		case SNAPSHOTTYPE_ALLACTION:		OnAllAction(ar);	break;
#endif // __S_SERVER_UNIFY
		case SNAPSHOTTYPE_TEXT:		OnText(ar);	break;
		case SNAPSHOTTYPE_SETPOS:	OnSetPos(objid, ar);	break;
		case SNAPSHOTTYPE_SETPOSANGLE:	OnSetPosAngle(objid, ar);	break;
		case SNAPSHOTTYPE_REPLACE:	OnReplace(ar);	break;
		case SNAPSHOTTYPE_SETEXPERIENCE:	OnSetExperience(objid, ar);	break;
		case SNAPSHOTTYPE_SETFXP:	OnSetFxp(objid, ar);	break;
		case SNAPSHOTTYPE_SETLEVEL:	OnSetLevel(objid, ar);	break;
		case SNAPSHOTTYPE_SETFLIGHTLEVEL:	OnSetFlightLevel(objid, ar);	break;
		case SNAPSHOTTYPE_SETSKILLLEVEL:	OnSetSkillLevel(objid, ar);	break;
		case SNAPSHOTTYPE_DESTPOS:	OnSetDestPos(objid, ar);	break;
		case SNAPSHOTTYPE_SETMOVEPATTERN:	OnSetMovePattern(objid, ar);	break;
		case SNAPSHOTTYPE_MELEE_ATTACK:		OnMeleeAttack(objid, ar);	break;
		case SNAPSHOTTYPE_MELEE_ATTACK2:	OnMeleeAttack2(objid, ar);	break;
		case SNAPSHOTTYPE_MAGIC_ATTACK:		OnMagicAttack(objid, ar);	 break;
		case SNAPSHOTTYPE_RANGE_ATTACK:		OnRangeAttack(objid, ar);	break;
		case SNAPSHOTTYPE_SP_ATTACK:		OnAttackSP(objid, ar);	break;
		case SNAPSHOTTYPE_MOVERSETDESTOBJ: OnMoverSetDestObj(objid, ar);	break;
		case SNAPSHOTTYPE_SHOUT:	OnShout(ar);	break;
		case SNAPSHOTTYPE_PLAYMUSIC:	OnPlayMusic(ar);	break;
		case SNAPSHOTTYPE_PLAYSOUND:	OnPlaySound(ar);	break;
		case SNAPSHOTTYPE_USESKILL:	OnUseSkill(objid, ar);	break;
		case SNAPSHOTTYPE_SET_GROWTH_LEARNING_POINT:	OnSetGrowthLearningPoint(objid, ar);	break;
		case SNAPSHOTTYPE_SET_JOB_SKILL:		OnSetChangeJob(objid, ar); break;
		case SNAPSHOTTYPE_SET_NEAR_JOB_SKILL:   OnSetNearChangeJob(objid, ar); break;
		case SNAPSHOTTYPE_MODIFYMODE:	OnModifyMode(objid, ar);	break;
		case SNAPSHOTTYPE_STATEMODE: 	OnStateMode(objid, ar);	break;
		case SNAPSHOTTYPE_RETURNSAY:	OnReturnSay(objid, ar); break;
		case SNAPSHOTTYPE_CLEAR_USESKILL:	OnClearUseSkill();	break;
		case SNAPSHOTTYPE_CLEAR_USEOBJECT:	OnClearUseObject();	break;
		case SNAPSHOTTYPE_SETFUEL:	OnSetFuel(objid, ar);	break;
		case SNAPSHOTTYPE_RESURRECTION:		OnResurrection(objid);	break;
		case SNAPSHOTTYPE_TRADEPUTERROR:	OnTradePutError(objid, ar);	break;
		case SNAPSHOTTYPE_CORRREQ:		OnCorrReq(objid, ar);	break;
		case SNAPSHOTTYPE_DISGUISE:		OnDisguise(objid, ar);	break;
		case SNAPSHOTTYPE_NODISGUISE:		OnNoDisguise(objid, ar);	break;
		case SNAPSHOTTYPE_CMDSETSKILLLEVEL:		OnCmdSetSkillLevel(ar); break;
		case SNAPSHOTTYPE_ACTIVESKILL:		OnCreateSkillEffect(objid, ar);	break;
		case SNAPSHOTTYPE_SETSTUN:			OnSetStun(objid, ar);	break;
		case SNAPSHOTTYPE_SENDACTMSG:		OnSendActMsg(objid, ar);	break;
		case SNAPSHOTTYPE_PUSHPOWER:		OnPushPower(objid, ar);	break;
		case SNAPSHOTTYPE_DO_ESCAPE:		OnEscape(objid, ar);	break;
		case SNAPSHOTTYPE_SETACTIONPOINT:	OnSetActionPoint(objid, ar);	break;
		case SNAPSHOTTYPE_ENDSKILLQUEUE:	OnEndSkillQueue(objid);	break;
		case SNAPSHOTTYPE_SETTARGET:	OnSetTarget(objid, ar);	break;


		case SNAPSHOTTYPE_USEOBJECT:	OnUseObject(objid, ar);	break;
		case SNAPSHOTTYPE_RUNSCRIPTFUNC:	OnRunScriptFunc(objid, ar);	break;
		case SNAPSHOTTYPE_SEX_CHANGE:	OnSexChange(objid, ar);	break;
		case SNAPSHOTTYPE_CHATTING: OnChatting(objid, ar); break;
		case SNAPSHOTTYPE_REMOVEQUEST: OnRemoveQuest(ar); break;
		case SNAPSHOTTYPE_INITSKILLPOINT: OnInitSkillPoint(ar); break;
		case SNAPSHOTTYPE_DOUSESKILLPOINT: OnDoUseSkillPoint(ar); break;

		case SNAPSHOTTYPE_COMMONPlACE: OnCommonPlace(objid, ar); break;
		case SNAPSHOTTYPE_RESETBUFFSKILL:	OnResetBuffSkill(objid, ar);	break;
		case SNAPSHOTTYPE_RESURRECTION_MESSAGE:	OnResurrectionMessage();	break;

		case SNAPSHOTTYPE_WORLDMSG: OnWorldMsg(objid, ar);	break;
		case SNAPSHOTTYPE_SNOOP:	OnSnoop(ar);	break;
		case SNAPSHOTTYPE_SETCHEERPARAM:	OnSetCheerParam(objid, ar);	break;
		case SNAPSHOTTYPE_QUERYEQUIP:	OnQueryEquip(objid, ar);	break;
		case SNAPSHOTTYPE_RETURNSCORLL: OnReturnScrollACK(ar); break;
		case SNAPSHOTTYPE_GUILDCOMBAT: OnGuildCombat(ar); break;
		case SNAPSHOTTYPE_QUEST_TEXT_TIME: OnQuestTextTime(ar); break;
		case SNAPSHOTTYPE_POSTMAIL:		OnPostMail(ar);	break;
		case SNAPSHOTTYPE_REMOVEMAIL:	OnRemoveMail(ar);		break;
		case SNAPSHOTTYPE_QUERYMAILBOX:		OnMailBox(ar);	break;
		case SNAPSHOTTYPE_QUERYMAILBOX_REQ:		OnMailBoxReq(ar);	break;
		case SNAPSHOTTYPE_SUMMON:	OnSummon(ar); break;
		case SNAPSHOTTYPE_REMOVE_GUILD_BANK_ITEM:	OnRemoveGuildBankItem(ar);	break;

#ifdef __EVE_MINIGAME
		case SNAPSHOTTYPE_MINIGAME:		OnMiniGame(objid, ar); break;
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
		case SNAPSHOTTYPE_ULTIMATE:		OnUltimateWeapon(objid, ar); break;
#endif // __ULTIMATE

#ifdef __TRADESYS
		case SNAPSHOTTYPE_EXCHANGE:		OnExchange(objid, ar); break;
#endif // __TRADESYS

#if __VER >= 9	// __PET_0410
		case SNAPSHOTTYPE_PET_CALL:		OnPetCall(objid, ar);	break;
		case SNAPSHOTTYPE_PET_RELEASE:	OnPetRelease(objid, ar);	break;
		case SNAPSHOTTYPE_PET_LEVELUP:	OnPetLevelup(objid, ar);	break;
		case SNAPSHOTTYPE_PET_SET_EXP:	OnPetSetExp(objid, ar);	break;
		case SNAPSHOTTYPE_PET:	OnPet(objid, ar);		break;
		case SNAPSHOTTYPE_PET_STATE:	OnPetState(objid, ar);	break;
		case SNAPSHOTTYPE_PET_FEED:		OnPetFeed(objid, ar);		break;
		case SNAPSHOTTYPE_PET_MILL:		OnPetFoodMill(objid, ar);	break;
#endif	// __PET_0410

#if __VER >= 9	// __AI_0509
		case SNAPSHOTTYPE_SET_SPEED_FACTOR:		OnSetSpeedFactor(objid, ar);	break;
#endif	// __AI_0509
#if __VER >= 10 // __CSC_VER9_1
		case SNAPSHOTTYPE_LEGENDSKILLUP_RESULT:	OnLegendSkillUp(ar); break;
#endif //__CSC_VER9_1

#if __VER >= 11 // __SYS_COLLECTING
		case SNAPSHOTTYPE_START_COLLECTING:		OnStartCollecting(objid);	break;
		case SNAPSHOTTYPE_STOP_COLLECTING:	OnStopCollecting(objid);	break;
		case SNAPSHOTTYPE_RESTART_COLLECTING:	OnRestartCollecting(objid, ar);	break;
#endif	// __SYS_COLLECTING

#ifdef __JEFF_11
		case SNAPSHOTTYPE_QUE_PETRESURRECTION_RESULT:	OnQuePetResurrectionResult(ar);	break;
#endif	// __JEFF_11

#if __VER >= 11
		case SNAPSHOTTYPE_GUILDLOG_VIEW: OnGuildBankLogList(ar); break;
#endif

#if __VER >= 13
		case SNAPSHOTTYPE_HONOR_LIST_ACK: OnHonorListAck(ar); break;
		case SNAPSHOTTYPE_HONOR_CHANGE_ACK: OnHonorChangeAck(objid, ar); break;
#endif

#if __VER >= 12 // __SECRET_ROOM
		case SNAPSHOTTYPE_SECRETROOM_MNG_STATE: OnSecretRoomMngState(ar); break;
		case SNAPSHOTTYPE_SECRETROOM_INFO: OnSecretRoomInfo(ar); break;
		case SNAPSHOTTYPE_SECRETROOM_TENDEROPENWND: OnSecretRoomTenderOpenWnd(ar); break;
		case SNAPSHOTTYPE_SECRETROOM_LINEUPOPENWND: OnSecretRoomLineUpOpenWnd(ar); break;
		case SNAPSHOTTYPE_SECRETROOM_TENDERVIEW: OnSecretRoomTenderView(ar); break;
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
		case SNAPSHOTTYPE_TAX_ALLINFO: OnTaxInfo(ar); break;
		case SNAPSHOTTYPE_TAX_SETTAXRATE_OPENWND: OnTaxSetTaxRateOpenWnd(ar); break;
#endif // __TAX
#if __VER >= 12 // __LORD

		case SNAPSHOTTYPE_ELECTION_ADD_DEPOSIT:	OnElectionAddDeposit(ar);		break;

		case SNAPSHOTTYPE_ELECTION_SET_PLEDGE:	OnElectionSetPledge(ar);	break;

		case SNAPSHOTTYPE_ELECTION_INC_VOTE:	OnElectionIncVote(ar);	break;

		case SNAPSHOTTYPE_ELECTION_BEGIN_CANDIDACY:		OnElectionBeginCandidacy(ar);		break;

		case SNAPSHOTTYPE_ELECTION_BEGIN_VOTE:		OnElectionBeginVote(ar);		break;

		case SNAPSHOTTYPE_ELECTION_END_VOTE:	OnElectionEndVote(ar);	break;

		case SNAPSHOTTYPE_LORD:	OnLord(ar);	break;

		case SNAPSHOTTYPE_L_EVENT_CREATE:	OnLEventCreate(ar);	break;

		case SNAPSHOTTYPE_L_EVENT_INITIALIZE:	OnLEventInitialize(ar);	break;

		case SNAPSHOTTYPE_LORD_SKILL_TICK:	OnLordSkillTick(ar);	break;

		case SNAPSHOTTYPE_L_EVENT_TICK:		OnLEventTick(ar);	break;

		case SNAPSHOTTYPE_LORD_SKILL_USE:	OnLordSkillUse(objid, ar);	break;
#endif	// __LORD
#ifdef __PET_1024
		case SNAPSHOTTYPE_SET_PET_NAME:		OnSetPetName(objid, ar);	break;
#endif	// __PET_1024
#if __VER >= 13 // __HOUSING
		case SNAPSHOTTYPE_HOUSING_ALLINFO: OnHousingAllInfo(ar); break;
		case SNAPSHOTTYPE_HOUSING_FURNITURELIST: OnHousingSetFunitureList(ar); break;
		case SNAPSHOTTYPE_HOUSING_SETUPFURNITURE: OnHousingSetupFurniture(ar); break;
		case SNAPSHOTTYPE_HOUSING_PAPERINGINFO: OnHousingPaperingInfo(ar); break;
		case SNAPSHOTTYPE_HOUSING_SETVISITALLOW: OnHousingSetVisitAllow(ar); break;
		case SNAPSHOTTYPE_HOUSING_VISITABLELIST: OnHousingVisitableList(ar); break;
#endif // __HOUSING

#if __VER >= 13 // __QUEST_HELPER
		case SNAPSHOTTYPE_QUESTHELPER_NPCPOS: OnNPCPos(ar); break;
#endif // __QUEST_HELPER
#if __VER >= 13 // __COUPLE_1117
		case SNAPSHOTTYPE_COUPLE:	OnCouple(ar);	break;
		case SNAPSHOTTYPE_PROPOSE_RESULT:	OnProposeResult(ar);	break;
		case SNAPSHOTTYPE_COUPLE_RESULT:	OnCoupleResult(ar);	break;
		case SNAPSHOTTYPE_DECOUPLE_RESULT:	OnDecoupleResult(ar);	break;
#if __VER >= 13 // __COUPLE_1202
		case SNAPSHOTTYPE_ADD_COUPLE_EXPERIENCE:	OnAddCoupleExperience(ar);	break;
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117

#ifdef __SPEED_SYNC_0108
		case SNAPSHOTTYPE_RESETDESTPARAM_SYNC:	OnResetDestParamSync(objid, ar);	break;
#endif
#if __VER >= 14 // __SMELT_SAFETY
		case SNAPSHOTTYPE_SMELT_SAFETY:OnSmeltSafety(ar); break;
#endif // __SMELT_SAFETY
#ifdef __MAP_SECURITY
		case SNAPSHOTTYPE_WORLD_READINFO:	OnWorldReadInfo(ar); break;
#endif // __MAP_SECURITY
#if __VER >= 15 // __PETVIS
		case SNAPSHOTTYPE_VISPET_ACTIVATE:		OnActivateVisPet(ar); break;
		case SNAPSHOTTYPE_MOVER_CHANGESFX:		OnChangeMoverSfx(objid, ar); break;
#endif // __PETVIS
#if __VER >= 15 // __GUILD_HOUSE
		case SNAPSHOTTYPE_GUILDHOUSE_PACKET:	OnGuildHousePacket(ar); break;
		case SNAPSHOTTYPE_GUILDHOUSE_ALLINFO:	OnGuildHouseAllInfo(ar); break;
		case SNAPSHOTTYPE_GUILDHOUSE_REMOVE:	OnGuildHouseRemove(ar); break;
#endif // __GUILD_HOUSE
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		case SNAPSHOTTYPE_QUEST_CHECKED:		OnCheckedQuest(ar); break;
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __PROTECT_AWAKE
		case SNAPSHOTTYPE_ITEM_SELECT_AWAKENING_VALUE: OnSafeAwakening(ar); break;
#endif	//__PROTECT_AWAKE

#ifdef __FL_DUNGEON_TREASURE
		case SNAPSHOTTYPE_DUNGEONTREASURE_WND: OnDungeonTreasureWnd(ar); break;
		case SNAPSHOTTYPE_DUNGEONTREASURE_RESULT: OnDungeonTreasureResult(ar); break;
#endif
#ifdef __FL_BATTLE_PASS
		case SNAPSHOTTYPE_BATTLE_PASS_TIME:	OnBattlePassTime(ar);	break;
		case SNAPSHOTTYPE_BATTLE_PASS:	OnBattlePassProgress(ar);	break;
#endif
#ifdef __FL_CONSUMABLES_STORAGE
		case SNAPSHOTTYPE_PUT_ITEM_TO_CHEST:	OnPutItemToChest(objid, ar);		break;
		case SNAPSHOTTYPE_MOVE_ITEM_INSIDECHEST:		OnMoveItemInsideChest(ar);	break;
		case SNAPSHOTTYPE_PUT_ALL_ITEMS_TO_CHEST:   OnMoveAllItemsToChest(); break;
#endif

		case SNAPSHOTTYPE_UPDATE_STATINFO:	OnUpdateStatInfo(objid, ar);	break;

#ifdef __FL_FAST_ELEMENT
		case SNAPSHOTTYPE_ADDREDOEQUIP:	OnElementChange(objid, ar);	break;
#endif
#ifdef __FL_SHOP_SEARCH
		case SNAPSHOTTYPE_ADD_SHOP_SEARCH:	OnShopSearch(ar);		break;
#endif
#ifdef __FL_EQUALIZED_MODE
		case SNAPSHOTTYPE_SERIALIZE_INVENTORY:   OnSerializeInvAndBuffs(ar); break;
		case SNAPSHOTTYPE_SELECT_TMP_CLASS:	OnOpenSelectTmpWnd(ar);	break;
		case SNAPSHOTTYPE_ADDUPDATEEQUIPFORNEAR:   OnSetNearEquipUpd(objid, ar); break;
#endif
#ifdef __FL_PARTY_SORT
		case SNAPSHOTTYPE_ADDPARTYCHANGEPOS: OnPartyChangePos(ar); break;
#endif
#ifdef __FL_GW_STATUE_V2
		case SNAPSHOTTYPE_STATUE_PARAMS: OnStatueParams(ar); break;
#endif
#ifdef __FL_FFA
		case SNAPSHOTTYPE_FFA_DATA: OnFFAData(ar); break;
		case SNAPSHOTTYPE_FFA_STATE: OnFFAState(ar); break;
		case SNAPSHOTTYPE_FFA_CLEAR: OnFFAClear();	break;
		case SNAPSHOTTYPE_FFA_KILL: OnFFAKill(ar); break;
#endif
#ifdef __FL_DAILY_REWARDS
		case SNAPSHOTTYPE_ADD_DAILY_REWARD: OnDailyReward(ar); break;
#endif
#ifdef __MODEL_HIDEFASH
		case SNAPSHOTTYPE_HIDE_FASH:   OnHideFashion(objid, ar); break;
#endif
#ifdef __MODEL_CHANGE
		case SNAPSHOTTYPE_LOOKCHANGE: OnLookChange(ar); break;
#endif
		case SNAPSHOTTYPE_CLEARCASTING: OnClearCastingSkills(objid, ar); break;

#ifdef __FL_PARTY_FINDER
		case SNAPSHOTTYPE_PARTYFINDER: OnPartyFinder(ar); break;
		case SNAPSHOTTYPE_PARTYJOINMODE: OnPartyChangeJoinMode(ar); break;
#endif

#ifdef __FL_VENDORS
		case SNAPSHOTTYPE_VENDOR_ACTIVATE: OnVendorActivate(ar); break;
		case SNAPSHOTTYPE_VENDOR_DEACTIVATE: OnVendorDeactivate(ar); break;
		case SNAPSHOTTYPE_VENDOR_PREVIEW: OnVendorPreview(ar); break;
		case SNAPSHOTTYPE_VENDOR_ADDNEWITEM: OnVendorAddNewItem(objid, ar); break;
#endif
#ifdef __FL_CASH_SHOP
		case SNAPSHOTTYPE_CSHOP_ADD: OnCShop(ar); break;
		case SNAPSHOTTYPE_CSHOP_BALANCE: OnCShopBalance(ar); break;
#endif
#ifdef __FL_SWITCH_EQ
		case SNAPSHOTTYPE_SWITCH_ADD: OnSwitchAdd(ar); break;
		case SNAPSHOTTYPE_SWITCH_DELETE: OnSwitchDelete(ar); break;
		case SNAPSHOTTYPE_SWITCHES_ALL: OnSwitchesAll(ar); break;
#endif
#ifdef __FL_GUILD_FINDER
		case SNAPSHOTTYPE_GUILDFINDER: OnGuildFinder(ar); break;
		case SNAPSHOTTYPE_GUILD_FINDERSTATE:	OnGuildFinderState(ar); break;
		case SNAPSHOTTYPE_GUILD_FINDERADDON:	OnGuildFinderAddon(ar); break;
#endif
#ifdef __FL_PACKED_ITEMS
		case SNAPSHOTTYPE_PACKAGE_OPEN:	OnPackageOpen(ar); break;
		case SNAPSHOTTYPE_PACKAGE_ADD:	OnPackageAdd(ar); break;
		case SNAPSHOTTYPE_PACKAGE_RESTORE:	OnPackageRestore(ar); break;
#endif
#ifdef __FL_WHEEL_OF_FORTUNE
		case SNAPSHOTTYPE_WOF: OnWof(ar); break;
		case SNAPSHOTTYPE_WOF_INIT: OnWofInit(ar); break;
		case SNAPSHOTTYPE_WOF_FINISH: OnWofFinish(ar); break;
#endif
#ifdef __FL_RECORD_BOOK
		case SNAPSHOTTYPE_RECORD_BOOK: OnRecordBook(ar); break;
#endif
#ifdef __FL_MONSTER_ALBUM
		case SNAPSHOTTYPE_MA_ADDCARDPIECE: OnMAAddCardPieces(ar); break;
		case SNAPSHOTTYPE_MA_BOSSCARDSLOT_UNLOCK: OnMABossCardSlotUnlock(ar); break;
		case SNAPSHOTTYPE_MA_BOSSCARDSLOT_INSERT: OnMABossCardSlotInsert(ar); break;
		case SNAPSHOTTYPE_MA_BOSSCARDSLOT_RESTORE: OnMABossCardSlotRestore(ar); break;
#endif
#ifdef __FL_TDM
		case SNAPSHOTTYPE_TDM_DATA: OnTDMData(ar); break;
		case SNAPSHOTTYPE_TDM_REGISTER: OnTDMRegister(ar); break;
		case SNAPSHOTTYPE_TDM_STATE: OnTDMState(ar); break;
		case SNAPSHOTTYPE_TDM_CLEAR: OnTDMClear();	break;
		case SNAPSHOTTYPE_TDM_KILL: OnTDMKill(ar); break;
#endif
		default:
		{
			ASSERT(0);
			break;
		}
		}
		prev = hdr;
		g_Prev = (BYTE)(prev);
	}
}

void CDPClient::OnWorldMsg(OBJID objid, CAr& ar)
{
	static CHAR	lpString[260];
	ar.ReadString(lpString, 260);
	g_WndMng.PutString(lpString, NULL, 0xffff99cc, CHATSTY_GAME);
}

void CDPClient::OnJoin(CAr& ar)
{
	g_Neuz.m_dwTimeOutDis = 0xffffffff;

	CWndBase* pWndBase = g_WndMng.GetWndBase(APP_SELECT_CHAR);
	if (pWndBase)
		((CWndSelectChar*)pWndBase)->Destroy();
	g_WndMng.CloseMessageBox();

	fJoin = TRUE;
	OnSnapshot(ar);
	fJoin = FALSE;

}

void CDPClient::OnKeepAlive(CAr& ar)
{
	BEFORESENDSOLE(ar2, PACKETTYPE_KEEP_ALIVE, DPID_UNKNOWN);
	SEND(ar2, this, DPID_SERVERPLAYER);
}


void CDPClient::OnUpdatePlayerData(CAr& ar)
{
	u_long idPlayer;
	ar >> idPlayer;
	sPlayerData	data;
	ar.Read(&data, sizeof(sPlayerData));

	PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPlayer);

#ifdef __FL_FIX_PLAYERDATA // this actually should not happen again, but in case if so...
	if (pPlayerData->data.nVer == 0)
		return;
#endif

	if (pPlayerData->data.nVer < data.nVer)
		memcpy(&pPlayerData->data, &data, sizeof(sPlayerData));

	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
	if (pWndMessengerEx)
	{
		pWndMessengerEx->UpdateFriendList();
		pWndMessengerEx->UpdateGuildMemberList();
	}

	UpdateGuildWnd();
}

void	CDPClient::OnLogout(CAr& ar)
{
	u_long idPlayer;
	ar >> idPlayer;
	PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPlayer);
	pPlayerData->data.uLogin = 0;

	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
	if (pWndMessengerEx)
	{
		pWndMessengerEx->UpdateFriendList();
		pWndMessengerEx->UpdateGuildMemberList();
	}

	UpdateGuildWnd();
}


void CDPClient::OnGetClock(CAr& ar)
{
	BYTE fsETbaseOfClient;
	ar >> fsETbaseOfClient;

	//	time_t tClient	= timeGetTime();
	DWORD tClient = timeGetTime();
	BEFORESENDSOLE(ar2, PACKETTYPE_GET_CLOCK, DPID_UNKNOWN);
	ar2 << fsETbaseOfClient << tClient;
	SEND(ar2, this, DPID_SERVERPLAYER);
}

void CDPClient::OnError(CAr& ar)
{
	ar >> m_lError;

	switch (m_lError)
	{
	case ERROR_INVALID_CLOCK:
	{
		g_dpLoginClient.Destroy(TRUE);
		s_f = TRUE;
		break;
	}
	default:
		break;
	}
}


void CDPClient::OnAddObj(OBJID objid, CAr& ar)
{
	//static
	DWORD	dwObjType, dwObjIndex;

	CObj::SetMethod((g_pPlayer && g_pPlayer->GetId() != objid) ? METHOD_EXCLUDE_ITEM : METHOD_NONE);

	dwObjType = 0;
	ar >> (BYTE&)dwObjType >> dwObjIndex;

	CObj* pObj = (CObj*)prj.GetCtrl(objid);
	if (pObj)
	{
		CWorld* pWorld = pObj->GetWorld();
		if (pWorld)
		{
			pObj->Delete();
			pWorld->DeleteObjects();
		}
		else
			SAFE_DELETE(pObj);
	}

	pObj = (CCtrl*)CreateObj(g_Neuz.m_pd3dDevice, dwObjType, dwObjIndex, dwObjType != OT_MOVER);
	if (pObj == NULL)
	{
		CString string;
		string.Format("CreateObj Error: Type = %d, Index = %d", dwObjType, dwObjIndex);
		return;
	}
	pObj->Serialize(ar);

	if (dwObjType == OT_MOVER)
	{
		if (((CMover*)pObj)->IsPlayer())
		{
			CMover* pPlayer = (CMover*)pObj;
#ifdef __FIX_SRLZ_STATS
			if (g_pPlayer && g_pPlayer->GetId() != objid)
				pPlayer->RedoEquip(CMover::GetActiveMover() != NULL, false);
			else
#endif
				pPlayer->RedoEquip(CMover::GetActiveMover() != NULL);

#ifndef __FL_FIX_PLAYERDATA // Can't see any reason to have in client memory player data of random players
			PlayerData pd;
			pd.data.nJob = pPlayer->GetJob();
			pd.data.nLevel = pPlayer->GetLevel();
			pd.data.nSex = pPlayer->GetSex();
			lstrcpy(pd.szPlayer, pPlayer->GetName());
			CPlayerDataCenter::GetInstance()->AddPlayerData(((CMover*)pObj)->m_idPlayer, pd);
#endif

		}
		else
		{

			((CMover*)pObj)->RedoEquip(TRUE);
			// npc
			if (!fJoin)
			{
				if (g_pPlayer)
				{
					D3DXVECTOR3 vTemp = pObj->GetPos() - g_pPlayer->GetPos();
					if (vTemp.x * vTemp.x + vTemp.z * vTemp.z < 32 * 32)
						CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_MONSTER_SPAWN01, pObj->GetPos(), ((CMover*)pObj)->GetId());
				}
			}
		}

		if (!CMover::GetActiveMover())
		{
			if (((CMover*)pObj)->m_idPlayer != g_Neuz.m_idPlayer)
			{
				Error("OnAddObj Prob: %d - first, %d - second", ((CMover*)pObj)->m_idPlayer, g_Neuz.m_idPlayer);
				ExitProcess(-1);
			}
			CMover::SetActiveObj(pObj);
			g_WndMng.SetPlayer(g_pPlayer = (CMover*)pObj);
			g_WndMng.OpenField();
			InitFST();

			g_Neuz.m_bTexLoadAlpha = TRUE;
		}
		if (((CMover*)pObj)->IsNPC() && ((CMover*)pObj)->GetProp()->dwClass == RANK_BOSS)
		{
			D3DXVECTOR3 vDist = pObj->GetPos() - g_pPlayer->GetPos();
			FLOAT fLength = D3DXVec3Length(&vDist);
			if (fLength < 30)
			{
				PlayMusic(BGM_IN_BOSS, 1);
				LockMusic();
				CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
				g_Caption1.RemoveAll();
				if (::GetLanguage() != LANG_JAP)
					g_Caption1.AddCaption(prj.GetText(TID_GAME_BIGMONSTER), pWndWorld->m_pFontAPITitle);
				else
					g_Caption1.AddCaption(prj.GetText(TID_GAME_BIGMONSTER), NULL);
			}
		}
		CMover* pPlayer = (CMover*)pObj;
		if (pPlayer->m_pActMover->IsFly())
		{
			CModel* pModel = prj.m_modelMng.LoadModel(D3DDEVICE, OT_ITEM, pPlayer->GetRideItemIdx());
			CModelObject* pModelObject = (CModelObject*)pModel;
			if (pModelObject->m_pBone)
			{
				CString strMotion = pModelObject->GetMotionFileName(_T("stand"));
				assert(strMotion != _T(""));
				pModelObject->LoadMotion(strMotion);
			}
			pPlayer->SetRide(pModel, pPlayer->GetRideItemIdx());

			ItemProp* pItemProp = prj.GetItemProp(pPlayer->GetRideItemIdx());
			if (pItemProp)
			{
				if (pPlayer->m_nFuel == -1)
					pPlayer->m_nFuel = (int)(pItemProp->dwFFuelReMax * 0.2f);
			}
		}
		pPlayer->InitMotion(pPlayer->m_dwMotion);
	}
	else if (pObj->GetType() == OT_ITEM)
	{
		ItemProp* pItemProp = ((CItem*)pObj)->GetProp();
		assert(pItemProp != NULL);
		PLAYSND(pItemProp->dwSubDefine, &(pObj->GetPos()));
	}
	else if (OT_CTRL == pObj->GetType())
	{
		CtrlProp* pProp = (CtrlProp*)pObj->GetProp();

		if (pProp && pProp->IsGuildHousingObj())
		{
			CModel* pModel = (CModel*)pObj->GetModel();
			assert(pModel);

			if (pModel->m_nFrameMax > 0)
			{
				((CCommonCtrl*)pObj)->m_bAniPlay = TRUE;
				pModel->SetLoop(ANILOOP_LOOP);
			}
		}
	}

	pObj->UpdateLocalMatrix();

	g_WorldMng.Get()->AddObj(pObj, TRUE);
	if (pObj->GetType() == OT_MOVER && ((CMover*)pObj)->m_vtInfo.IsVendorOpen())
	{
		g_DialogMsg.AddVendorMessage(pObj, ((CMover*)pObj)->m_vtInfo.GetTitle(), 0xffffffff);
	}

	if (pObj->GetType() == OT_MOVER)
	{
		CMover* pMover = (CMover*)pObj;

		////////////////////////////////
		D3DXVECTOR3 vDestPos(0.0f, 0.0f, 0.0f);
		DWORD dwDestId = NULL_ID;
		float fArrivalRange = 0.0f;

		ar >> vDestPos;
		ar >> dwDestId;
		ar >> fArrivalRange;

		if (vDestPos != D3DXVECTOR3(0.0f, 0.0f, 0.0f))
			pMover->SetDestPos(vDestPos, true, FALSE);

		if (dwDestId != NULL_ID)
			pMover->SetDestObj(dwDestId, fArrivalRange, FALSE);

		////////////////////////////////

		if (pMover->IsPlayer())
		{
			if (pMover->IsStateMode(STATE_BASEMOTION_MODE))
			{
				if (pMover->m_dwUseItemId != 0)
				{
					ItemProp* pItemProp = prj.GetItemProp(pMover->m_dwUseItemId);
					if (pItemProp)
					{
						CreateItemReadySfx(D3DDEVICE, pItemProp->dwSfxObj, pMover->GetPos(), pMover->GetId(),
							pMover->GetPos(), pMover->GetId(), -1);
					}
				}
			}

			DWORD dwDisquise = pMover->GetDisguise();
			if (dwDisquise != NULL_ID)
			{
				pMover->Disguise(D3DDEVICE, dwDisquise);
				CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
				if (pWndQuestDetail)
					pWndQuestDetail->UpdateQuestText();

				CWndQuestQuickInfo* pWndQuestQuickInfo = g_WndMng.m_pWndQuestQuickInfo;
				if (pWndQuestQuickInfo)
					pWndQuestQuickInfo->SetUpdateTextSwitch(TRUE);
			}
		}

	}

	if (pObj->GetType() == OT_ITEM)
	{
		CItem* pItem = (CItem*)pObj;
		if (pItem)
		{
			//todo
#ifdef __FL_UPDATE_GROUNDITEMS
			//DWORD dwSfx = NULL_ID;
			//dwSfx = XI_NAT_ROCKET01;

			//if (dwSfx != NULL_ID)
			//	CreateSfx(g_Neuz.m_pd3dDevice, XI_NAT_ROCKET01, pItem->GetPos(), NULL_ID, D3DXVECTOR3(0, 0, 0), pItem->GetId(), -1);
#endif

			CWorld* pWorld = g_WorldMng.Get();
			D3DXVECTOR3 vPos = pObj->GetPos();
			D3DXVECTOR3 vPosOrig = vPos;
			vPos.y += 1.5f;
			FLOAT fGroundY = pWorld->GetItemHeight(vPos);


			if (pItem->m_pModel)
			{
				float fHeight = pItem->m_pModel->GetMaxHeight();
				if (fHeight < 0)
				{
					Error("OnAddObj Height < 0 -%d", pItem->GetIndex());
					fHeight = -fHeight;
				}
				fGroundY += (fHeight / 2.0f);
				fGroundY += 0.12f;

			}

			if (fabs(fGroundY - vPosOrig.y) < 1.0f)
			{
				pObj->SetPos(vPos);
				D3DXVECTOR3 vDelta;
				FLOAT fAngXZ = (float)xRandom(360);
				vDelta.x = vDelta.z = 0;
				vDelta.y = 0.15f;
				pItem->SetDelta(fGroundY, vDelta);
			}
		}
	}
}

char szBuf[1024];
char szChat[1024];

void CDPClient::OnChat(OBJID objid, CAr& ar)
{
	ar.ReadString(szBuf, 1024);
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
#ifdef __YS_CHATTING_BLOCKING_SYSTEM
		if (pMover && pMover->IsPlayer() == TRUE)
		{
			set< CString >::iterator BlockedUserIterator = prj.m_setBlockedUserID.find(pMover->GetName(TRUE));
			if (BlockedUserIterator != prj.m_setBlockedUserID.end())
				return;
		}
#endif // __YS_CHATTING_BLOCKING_SYSTEM

		if (IsValidObj(pMover) && pMover->GetWorld() && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
		{
			if (pMover->IsMode(GCWAR_RENDER_SKIP_MODE))
				return;
		}
		if (strlen(szBuf) && (szBuf[0] == '/' || szBuf[0] == '!'))
			g_DialogMsg.AddMessage(pMover, szBuf);
		else
		{
			int nTextId = -1;
			sprintf(szChat, "%s : %s", pMover->GetName(TRUE), szBuf);
			g_WndMng.PutString((LPCTSTR)szChat, pMover, 0xffffffff, CHATSTY_GENERAL);
		}
	}
}

void CDPClient::OnEventMessage(OBJID objid, CAr& ar)
{
	int nFlag;
	DWORD dwItemID;
	ar.ReadString(szBuf, 1024);
	ar >> nFlag;
	ar >> dwItemID;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		CString strString;
		strString = szBuf;
		D3DXVECTOR3 vPos = pMover->GetPos();
		vPos.y += pMover->m_pModel->GetMaxHeight();

		switch (nFlag)
		{
		case 0:
		{
			CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_EVE_EVENT_FAIL, vPos);
			if (pSfx)
				pSfx->SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
		}
		break;
		case 1:
		{
			CSfx* pSfx = NULL;
			switch (dwItemID)
			{
			case II_SYS_SYS_EVE_0504M1NOTEBOOK:
				pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_EVE_EVENT_NOTEBOOK, vPos);
				break;
			case II_SYS_SYS_EVE_0504M2DCAMARA:
				pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_EVE_EVENT_DCAMARA, vPos);
				break;
			case II_SYS_SYS_EVE_0504M3AIRSHIP:
				pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_EVE_EVENT_AIRSHIP, vPos);
				break;
			case II_SYS_SYS_EVE_0504M4USBFANLIGHT:
				pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_EVE_EVENT_USBFANLIGHT, vPos);
				break;
			case II_SYS_SYS_EVE_0504M5BALLOON:
				pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_EVE_EVENT_BALLOON, vPos);
				break;
			case II_SYS_SYS_EVE_0504M6GIFTTICKET:
				pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_EVE_EVENT_GIFTTICKET, vPos);
				break;
			case II_SYS_SYS_EVE_0504M7MOVIETICKET:
				pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_EVE_EVENT_MOVIETICKET, vPos);
				break;
			case II_SYS_SYS_EVE_0504M8OST:
				pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_EVE_EVENT_OST, vPos);
				break;
			default:
				break;
			}

			if (pSfx)
				pSfx->SetScale(D3DXVECTOR3(1.5f, 1.5f, 1.5f));

			if (g_pPlayer == pMover)
				g_WndMng.PutString(strString, NULL, 0xff0000);
		}
		break;
		case 2:
		{
			g_WndMng.PutString(strString, pMover, 0xffffffff);
			if (g_pPlayer == pMover)
			{
				CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_EVE_EVENT_WIN, vPos);
				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
			}
		}
		break;
		case 3:
		{
			if (g_pPlayer == pMover)
				g_WndMng.PutString(prj.GetText(TID_GAME_LACKSPACE), NULL, prj.GetTextColor(TID_GAME_LACKSPACE)); //0xff0000 );
		}
		break;
		}
	}
}

void CDPClient::OnText(CAr& ar)
{
	static TCHAR szText[MAX_PATH]; // 260

#ifdef __S_SERVER_UNIFY
	BYTE nState;
	ar >> nState;
#endif // __S_SERVER_UNIFY
	ar.ReadString(szText, MAX_PATH);

	DWORD dwColor;
	ar >> dwColor;

#ifdef __S_SERVER_UNIFY
	switch (nState)
	{
	case TEXT_GENERAL:
		g_WndMng.PutString(szText, NULL, dwColor);
		break;
	case TEXT_DIAG:
		g_WndMng.OpenMessageBoxUpper(szText);
		break;
	}
#else // __S_SERVER_UNIFY
	g_WndMng.PutString(szText, NULL, 0xff00ff00);
#endif // __S_SERVER_UNIFY
}

#ifndef __S_SERVER_UNIFY
void CDPClient::OnDiagText(CAr& ar)
{
	static TCHAR szText[MAX_PATH]; // 260

	ar.ReadString(szText, MAX_PATH);
	g_WndMng.OpenMessageBoxUpper(szText);
}
#endif // __S_SERVER_UNIFY

#ifdef __S_SERVER_UNIFY
void CDPClient::OnAllAction(CAr& ar)
{
	BOOL bCharacter;
	ar >> g_WndMng.m_bAllAction;
	ar >> bCharacter;

	if (g_WndMng.m_bAllAction == FALSE)
	{
		if (bCharacter)
		{

			if (!g_WndMng.m_pWndChangeName)
			{
				g_WndMng.m_pWndChangeName = new CWndChangeName;
				g_WndMng.m_pWndChangeName->Initialize(&g_WndMng, 0);
			}
		}
		else
		{
			if (!g_WndMng.m_pWndGuildName)
			{
				g_WndMng.m_pWndGuildName = new CWndGuildName;
				g_WndMng.m_pWndGuildName->Initialize(&g_WndMng);
			}
			g_WndMng.m_pWndGuildName->SetData(0x7f);
		}
	}
}
#endif // __S_SERVER_UNIFY

void CDPClient::OnRemoveObj(OBJID objid)
{
	CCtrl* pCtrl = prj.GetCtrl(objid);
	if (IsValidObj(pCtrl) == FALSE)
		return;

	CWorld* pWorld = pCtrl->GetWorld();
	if (pWorld)
	{
		if (pCtrl->GetType() == OT_MOVER)
		{
			CMover* pMover = (CMover*)pCtrl;
			if (pMover->m_pet.GetObj())
			{
				pMover->m_pet.GetObj()->Delete();
				pMover->m_pet.SetObj(NULL);
			}
		}
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
		if (pWndWorld)
		{
			if (pCtrl == pWndWorld->m_pSelectRenderObj)
				pWndWorld->m_pSelectRenderObj = NULL;
		}

		if (pCtrl == g_pPlayer->m_vtInfo.GetOther())
		{
			if (pCtrl->GetType() == OT_MOVER)
			{
				CWndVendor* pWndVendor = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);
				if (pWndVendor)
				{
					if (pWndVendor == CWndBase::m_GlobalShortcut.m_pFromWnd->GetFrameWnd())
						CWndBase::m_GlobalShortcut.Empty();
					pWndVendor->Destroy();
				}

				CWndShop* pWndShop = (CWndShop*)g_WndMng.GetWndBase(APP_SHOP_);
				if (pWndShop)
					pWndShop->Destroy();

				CWndTrade* pWndTrade = (CWndTrade*)g_WndMng.GetWndBase(APP_TRADE);
				if (pWndTrade)
					pWndTrade->Destroy();

				CWndConfirmTrade* pWndConfirmTrade = (CWndConfirmTrade*)g_WndMng.GetApplet(APP_CONFIRM_TRADE);
				if (pWndConfirmTrade)
					pWndConfirmTrade->Destroy();

				CWndTradeConfirm* pWndTradeConfirm = (CWndTradeConfirm*)g_WndMng.GetWndBase(APP_TRADE_CONFIRM);
				if (pWndTradeConfirm)
					pWndTradeConfirm->Destroy();

				SAFE_DELETE(g_WndMng.m_pWndTradeGold);

				g_pPlayer->OnTradeRemoveUser();
				g_pPlayer->m_vtInfo.SetOther(NULL);
			}
		}

		pCtrl->Delete();
	}
	else
	{
		//safety
		if (g_pPlayer)
		{
			CWorld* pWorld = g_pPlayer->GetWorld();
			if (pWorld)
			{
				// cancel deletion
				for (int i = 0; i < pWorld->m_nDeleteObjs; i++)
				{
					if (pCtrl == pWorld->m_apDeleteObjs[i])
					{
						pWorld->m_apDeleteObjs[i] = NULL;
						pWorld->m_nDeleteObjs--;
						break;
					}
				}
			}
		}

		SAFE_DELETE(pCtrl);
	}
}

void CDPClient::OnDamage(OBJID objid, CAr& ar)
{
	OBJID objidAttacker;
	DWORD dwHit;
	ar >> objidAttacker >> dwHit;
	DWORD dwAtkFlags;
	ar >> dwAtkFlags;

	CMover* pMover = prj.GetMover(objid);

	D3DXVECTOR3 vPos;
	FLOAT		fAngle;

	if (dwAtkFlags & AF_FLYING)
	{
		ar >> vPos;
		ar >> fAngle;

		if (IsValidObj(pMover))
		{
			pMover->SetPos(vPos);
			pMover->SetAngle(fAngle);
		}
	}

	if (!IsValidObj(pMover))
		return;

	pMover->IncHitPoint((int)((-1) * (int)(dwHit)));
	if (pMover->m_nHitPoint == 0)
		pMover->m_nHitPoint = 1;


	CObj* pObj = g_pPlayer->GetWorld()->GetObjFocus();

	if (pMover->IsActiveMover())
	{
		CWndBase* pWndBase = g_WndMng.GetWndBase(APP_VENDOR_REVISION);
		if (pWndBase)
		{
			pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);
			if (pWndBase)
			{
				pWndBase->Destroy();
				pWndBase->m_GlobalShortcut.Empty();
			}
		}
	}

	CCtrl* pAttacker = prj.GetCtrl(objidAttacker);
	if (IsValidObj(pAttacker) == FALSE)
		goto g_PASS;

#ifdef __FL_FARM_DISPLAY
	CWndFarmDisplay* pWndBase = (CWndFarmDisplay*)g_WndMng.GetWndBase(APP_FARM_DISPLAY);
	if (pMover->IsNPC() && pAttacker == g_pPlayer && pWndBase)
		pWndBase->AddDamage(dwHit);
#endif

#ifdef __FL_DPS_DISPLAY
	CWndDpsDisplay* pWnd = (CWndDpsDisplay*)g_WndMng.GetWndBase(APP_DPS_DISPLAY);
	if (pWnd)
	{
		if (IsValidObj(pAttacker) && pAttacker->GetType() == OT_MOVER && ((CMover*)pAttacker)->IsPlayer() && pMover->GetId() == pWnd->GetTargetId())
			pWnd->Add(((CMover*)pAttacker)->GetName(), ((CMover*)pAttacker)->GetJob(), dwHit);
	}
#endif

	if (g_Option.m_bDamageRender)
	{
		if (g_pPlayer->IsRangeObj(pMover->GetPos(), 180))
			goto g_PASS;
	}
	else
	{
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER) && pObj)
		{
			if (pObj == (CObj*)pMover || pMover->IsActiveMover() || (pAttacker->GetType() == OT_MOVER && ((CMover*)pAttacker)->IsActiveMover()))
				goto g_PASS;
		}
	}

	if (pMover->IsActiveMover() || (pAttacker->GetType() == OT_MOVER && ((CMover*)pAttacker)->IsActiveMover()))
	{
	g_PASS:

		if (g_pPlayer->IsStateMode(STATE_BASEMOTION_MODE) && (pMover == g_pPlayer || pAttacker == g_pPlayer))
			SendStateModeCancel(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL);

		if (dwAtkFlags & AF_BLOCKING)
		{
			D3DXVECTOR3 vPos = pMover->GetPos();
			vPos.y += 1.5f;
			CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_HIT_HITBLOCK01, vPos);
			if (pSfx)
				pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
		}

		if (dwAtkFlags & AF_PARRY)
		{
			D3DXVECTOR3 vPos = pMover->GetPos();
			vPos.y += 1.5f;
			CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_HIT_PARRY01, vPos);
			if (pSfx)
				pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
		}
		else if (dwAtkFlags & AF_RESIST)
		{
			if (dwHit == 0)
			{
				D3DXVECTOR3 vPos = pMover->GetPos();
				vPos.y += 1.5f;
				CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_HIT_MISS01, vPos);
				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
			}
			else
				g_DamageNumMng.AddNumber(pMover->GetPos(), dwHit, (pMover->IsActiveMover() ? 0 : 1));
		}
		else if (dwAtkFlags & AF_MISS)
		{
			D3DXVECTOR3 vPos = pMover->GetPos();
			vPos.y += 1.5f;
			CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_HIT_MISS01, vPos);
			if (pSfx)
				pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
		}
		else
		{
			if (dwAtkFlags & AF_CRITICAL)
			{
				D3DXVECTOR3 vPos = pMover->GetPos();
				vPos.y += 1.5f;
				CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, XI_HIT_CRITICAL01, vPos);
				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(3.0f, 3.0f, 3.0f));
			}

			g_DamageNumMng.AddNumber(pMover->GetPos(), dwHit, (pMover->IsActiveMover() ? 0 : 1));
		}
	}

	if (IsValidObj(pAttacker))
	{
		if (pAttacker->GetType() == OT_MOVER)
		{
			if (dwAtkFlags & AF_CRITICAL)
			{
				ItemProp* pItemProp = ((CMover*)pAttacker)->GetActiveHandItemProp();
				if (pItemProp && pItemProp->dwSndAttack2 != NULL_ID)
					PLAYSND(pItemProp->dwSndAttack2, &pAttacker->GetPos());
			}
		}
		else
		{
			if (pMover->IsActiveMover())
				SendCorrReq(objidAttacker);
		}
	}

	if (dwAtkFlags & AF_FLYING)
		pMover->FlyByAttack();
}

void CDPClient::OnMoverDeath(OBJID objid, CAr& ar)
{
	DWORD dwMsg;
	OBJID objidAttacker;
	ar >> objidAttacker >> dwMsg;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		const int nHitPoint = pMover->GetHitPoint();
		pMover->IncHitPoint(-nHitPoint);

		CWorld* pMoverWorld = pMover->GetWorld();
		if (g_pPlayer == pMover)
		{
			if (pMoverWorld)
				pMoverWorld->SetObjFocus(NULL);

			if (g_WndMng.m_pWndWorld)
			{
				g_WndMng.m_pWndWorld->SetLastTarget(NULL_ID);
				g_WndMng.m_pWndWorld->m_bAutoAttack = false;
				g_WndMng.m_pWndWorld->ClearTracking();
			}
		}

		if (pMover->IsActiveMover())
		{
			bool bBossDie = false;
			bool guildovo = false;
			bool arena = false;

			if (pMoverWorld)
			{
				if (pMoverWorld->GetID() == WI_DUNGEON_MUSCLE || pMoverWorld->GetID() == WI_DUNGEON_KRRR || pMoverWorld->GetID() == WI_DUNGEON_BEAR)
					bBossDie = true;
				else if (pMover->m_nGuildCombatState != 0
#ifdef __FL_FFA
					|| pMoverWorld->IsFFA()
#endif
#ifdef __FL_TDM
					|| pMoverWorld->IsTDM()
#endif
					)
					guildovo = true;
				else if (pMoverWorld->IsArena())
					arena = true;
			}

			if (!guildovo)
				PlayMusic(BGM_IN_DEATH);

			g_DamageNumMng.AddNumber(pMover->GetPos(), nHitPoint, 0);

			if (pMover->m_nGuildCombatState == 1
#ifdef __FL_FFA
				|| pMoverWorld->IsFFA()
#endif
#ifdef __FL_TDM
				|| pMoverWorld->IsTDM()
#endif
				)
				SendHdr(PACKETTYPE_REVIVAL_TO_LODESTAR);
			else
			{
				SAFE_DELETE(g_WndMng.m_pWndRevival);
				g_WndMng.m_pWndRevival = new CWndRevival;
				g_WndMng.m_pWndRevival->Initialize(&g_WndMng, 0);

#ifdef __AIO_NEUZAPP
				if (g_Neuz.IsMinimised())
					g_Neuz.Flash();
#endif
			}
		}

		if (static_cast<CObj*>(pMover) == g_WndMng.m_pWndWorld->m_pNextTargetObj)
		{
			if (g_WndMng.m_pWndWorld->m_pNextTargetObj == g_WndMng.m_pWndWorld->m_pRenderTargetObj)
			{
				g_WndMng.m_pWndWorld->m_pNextTargetObj = NULL;
				g_WndMng.m_pWndWorld->m_pRenderTargetObj = g_WorldMng()->GetObjFocus();
			}
			else
				g_WndMng.m_pWndWorld->m_pNextTargetObj = NULL;
		}

		CMover* pAttacker = prj.GetMover(objidAttacker);
		if (pAttacker)
		{
			if (g_pPlayer == pAttacker)
			{
#ifdef __FL_FARM_DISPLAY
				CWndFarmDisplay* pWndBase = (CWndFarmDisplay*)g_WndMng.GetWndBase(APP_FARM_DISPLAY);
				if (pMover->IsNPC() && pWndBase)
				{
					pWndBase->AddDamage(nHitPoint);
					pWndBase->AddKills();
				}
#endif
				CWorld* playerWorld = g_pPlayer->GetWorld();
				if (playerWorld)
				{
					CObj* focusObj = playerWorld->GetObjFocus();
					if (static_cast<CMover*>(focusObj) == pMover)
					{
						if (g_WndMng.m_pWndWorld->m_pNextTargetObj)
						{
							g_WndMng.m_pWndWorld->SetNextTarget();
							g_WndMng.m_pWndWorld->m_bAutoAttack = true;
						}
						else
						{
							playerWorld->SetObjFocus(NULL);
							g_WndMng.m_pWndWorld->m_bAutoAttack = false;
						}
					}
				}
			}

			if (!pMover->IsActiveMover())
				g_DamageNumMng.AddNumber(pMover->GetPos(), nHitPoint, 1);

			if (pAttacker->m_pActMover->GetObjHit() == objid)
				pAttacker->m_pActMover->SetObjHit(NULL_ID);
		}

		if (g_WndMng.m_pWndWorld)
		{
			if (g_WndMng.m_pWndWorld->m_objidTracking == pMover->GetId())
				g_WndMng.m_pWndWorld->ClearTracking();
		}

		if (g_pPlayer->GetDestId() == objid)
		{
			g_pPlayer->SetStop();
			SendPlayerBehavior();
		}

		pMover->SendActMsg(OBJMSG_DIE, dwMsg, reinterpret_cast<int>(pAttacker));
	}
}

void CDPClient::OnCreateItem(OBJID objid, CAr& ar)
{
	CItemElem itemElem;
	int nType;

	itemElem.Serialize(ar);
	ar >> nType;

	CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(nType);
	if (!pContainer)
		return;

#ifdef __FL_FARM_DISPLAY
	CWndFarmDisplay* pWndBase = (CWndFarmDisplay*)g_WndMng.GetWndBase(APP_FARM_DISPLAY);
	if (pWndBase)
		pWndBase->AddItem(itemElem.m_dwItemId, itemElem.m_nItemNum);
#endif

	pContainer->Add(&itemElem);

	ItemProp* pProp = itemElem.GetProp();
	if (pProp)
	{
		CString str;
		str.Format("You received: %s (x%d) (%s).", pProp->szName, itemElem.m_nItemNum, g_WndMng.GetStringNameByContainerType(nType));
		g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_REAPITEM));
	}

	PLAYSND(SND_INF_GROUNDPICKUP, &g_pPlayer->GetPos());

	if (nType == TYPE_INVENTORY)
	{
		CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
		if (pWndQuestDetail)
			pWndQuestDetail->UpdateQuestText();

		CWndQuestQuickInfo* pWndQuestQuickInfo = g_WndMng.m_pWndQuestQuickInfo;
		if (pWndQuestQuickInfo)
			pWndQuestQuickInfo->SetUpdateTextSwitch(TRUE);
	}
}

void CDPClient::OnMoveItem(CAr& ar)
{
	BYTE nItemType, nSrcIndex, nDestIndex;
	ar >> nItemType >> nSrcIndex >> nDestIndex;

	if (g_pPlayer)
		g_pPlayer->m_Inventory.Swap(nSrcIndex, nDestIndex);
}

void CDPClient::OnSetPos(OBJID objid, CAr& ar)
{
	CWorld* pWorld;
	D3DXVECTOR3 vPos;
	ar >> vPos;

	CCtrl* pCtrl = prj.GetCtrl(objid);
	if (IsValidObj((CObj*)pCtrl) && (pWorld = pCtrl->GetWorld()))
	{
		((CWndWorld*)g_WndMng.m_pWndWorld)->m_pSelectRenderObj = NULL;

		if (pCtrl->IsActiveObj())
		{
			CMover* pMover = (CMover*)pCtrl;
			if (pMover->m_pet.GetObj())
			{
				pMover->m_pet.GetObj()->Delete();
				pMover->m_pet.SetObj(NULL);
			}

			pWorld->RemoveObj(pCtrl);
			pWorld->ReadWorld(vPos, TRUE);
			pCtrl->SetPos(vPos);
			pWorld->AddObj(pCtrl, FALSE);
			((CMover*)pCtrl)->SendActMsg(OBJMSG_STAND);
			pWorld->m_pCamera->m_vPos = vPos;
		}
		else
		{
			pCtrl->SetPos(vPos);
		}
		if (pCtrl->GetType() == OT_MOVER)
		{
			CMover* pMover = (CMover*)pCtrl;
			if (pMover->m_pet.GetObj())
			{
				pMover->m_pet.GetObj()->Delete();
				pMover->m_pet.SetObj(NULL);
			}

			pMover->ClearDest();
			pMover->SendActMsg(OBJMSG_ACC_STOP);
			pMover->SendActMsg(OBJMSG_STOP_TURN);
			pMover->SendActMsg(OBJMSG_STAND);

			pMover->AutoSynchronizer()->Initialize();
			pWorld->ProcessAllSfx();

			if (pMover->IsActiveMover())
			{
				CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
				if (pWndWorld)
					pWndWorld->SetWorldMusic();

				CWndMapEx* pWndMapEx = (CWndMapEx*)g_WndMng.GetApplet(APP_MAP_EX);
				if (pWndMapEx)
					pWndMapEx->Reload();

				SendPlayerBehavior();
			}
		}
	}
}

void CDPClient::OnSetPosAngle(OBJID objid, CAr& ar)
{
	CWorld* pWorld;
	D3DXVECTOR3 vPos;
	FLOAT fAngle;
	ar >> vPos >> fAngle;

	CCtrl* pCtrl = prj.GetCtrl(objid);
	if (IsValidObj((CObj*)pCtrl) && (pWorld = pCtrl->GetWorld()))
	{
		pCtrl->SetPos(vPos);
		pCtrl->SetAngle(fAngle);
		pCtrl->UpdateLocalMatrix();
	}
}

void CDPClient::OnReplace(CAr& ar)
{
	DWORD dwWorldID(NULL);
	D3DXVECTOR3 vPos(0.0f, 0.0f, 0.0f);

	ar >> dwWorldID;
	ar >> vPos;

#ifdef __WINDOW_INTERFACE_BUG
	g_WndMng.CloseBoundWindow();
#endif // __WINDOW_INTERFACE_BUG

	if (CDeployManager::GetInstance()->IsReady())
		CDeployManager::GetInstance()->EndDeploy();

	CWorld::m_nZoomLevel = 0;
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	if (pWndWorld)
		pWndWorld->m_pSelectRenderObj = NULL;

	char* szMapFile[] = { "MapLoading2.tga", "MapLoading3.tga", "MapLoading4.tga" };

	CString strPath = MakePath(DIR_THEME, szMapFile[xRandom(0, 3)]);
	if (dwWorldID == WI_WORLD_GUILDWAR || (dwWorldID >= WI_WORLD_GUILDWAR1TO1_0 && dwWorldID <= WI_WORLD_GUILDWAR1TO1_L))
		strPath = MakePath(DIR_THEME, "MapLoading_GuildWar.tga");
	else if (dwWorldID == WI_INSTANCE_OMINOUS)
		strPath = MakePath(DIR_THEME, "MapLoading_Ominous.tga");
	else if (dwWorldID == WI_INSTANCE_OMINOUS_1)
		strPath = MakePath(DIR_THEME, "MapLoading_Ominous_1.tga");
	else if (dwWorldID == WI_INSTANCE_DREADFULCAVE)
		strPath = MakePath(DIR_THEME, "MapLoading_DreadfulCave.tga");
	else if (GuildHouse->IsMyGuildHouse(dwWorldID))
		strPath = MakePath(DIR_THEME, "MapLoading_Guildhouse.tga");
	else if (dwWorldID == WI_DUNGEON_FL_MAS)
		strPath = MakePath(DIR_THEME, "MapLoading_Mas.tga");
	else if (dwWorldID == WI_DUNGEON_SA_TA)
		strPath = MakePath(DIR_THEME, "MapLoading_Ibless.tga");
	else if (dwWorldID == WI_DUNGEON_VOLCANE || dwWorldID == WI_DUNGEON_VOLCANERED)
		strPath = MakePath(DIR_THEME, "MapLoading_Volcane.tga");
	else if (dwWorldID >= WI_WORLD_HEAVEN01 && dwWorldID <= WI_WORLD_HEAVEN05)
		strPath = MakePath(DIR_THEME, "MapLoading_Heaven.tga");
	else if (dwWorldID == WI_WORLD_CISLAND)
		strPath = MakePath(DIR_THEME, "MapLoading_Cisland.tga");
	else if (dwWorldID == WI_WORLD_KEBARAS)
		strPath = MakePath(DIR_THEME, "MapLoading_Azria.tga");
	else if (dwWorldID == WI_WORLD_RARTESIA)
		strPath = MakePath(DIR_THEME, "MapLoading_Rartesia.tga");
	else if (dwWorldID == WI_WORLD_ARENA)
		strPath = MakePath(DIR_THEME, "MapLoading_Arena.tga");
	else if (dwWorldID >= WI_INSTANCE_RUSTIA && dwWorldID <= WI_INSTANCE_RUSTIA_1)
		strPath = MakePath(DIR_THEME, "MapLoading_Rustia.tga");
	else if (dwWorldID == WI_INSTANCE_HERNEOS || dwWorldID == WI_INSTANCE_HERNEOS_1)
		strPath = MakePath(DIR_THEME, "MapLoading_herneos.tga");
	else if (dwWorldID == WI_INSTANCE_UPRESIA || dwWorldID == WI_INSTANCE_UPRESIA_1)
		strPath = MakePath(DIR_THEME, "MapLoading_Uplesia.tga");
	else if (dwWorldID == WI_INSTANCE_SANPRES || dwWorldID == WI_INSTANCE_SANPRES_1)
		strPath = MakePath(DIR_THEME, "MapLoading_Flyship.tga");
	else if (dwWorldID == WI_INSTANCE_BEHAMAH)
		strPath = MakePath(DIR_THEME, "MapLoading_Behemoth.tga");
	else if (dwWorldID == WI_INSTANCE_KALGAS)
		strPath = MakePath(DIR_THEME, "MapLoading_kalgas.tga");
	else if (dwWorldID == WI_WORLD_DARKRARTESIA)
		strPath = MakePath(DIR_THEME, "MapLoading_DarkRartesia.tga");

	g_Neuz.m_TexLoading.LoadTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff, FALSE);

	if (NULL == g_Neuz.m_TexLoading.m_pTexture)
	{
		Error("MapLoading Texture error, filename %s", strPath);
		strPath = MakePath(DIR_THEME, "MapLoading2.tga");
		g_Neuz.m_TexLoading.LoadTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff, FALSE);
	}

	g_Neuz.m_bTexLoad = TRUE;
	g_Neuz.m_nTexAlpha = 255;

	if (SUCCEEDED(g_Neuz.m_pd3dDevice->BeginScene()))
	{
		g_Neuz.m_TexLoading.m_size = CSize(FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
		g_Neuz.m_2DRender.RenderTexture(CPoint(0, 0), &g_Neuz.m_TexLoading, g_Neuz.m_nTexAlpha);
	}
	g_Neuz.m_pd3dDevice->EndScene();
	g_Neuz.m_pd3dDevice->Present(NULL, NULL, NULL, NULL);

	Sleep(1000);

	if (dwWorldID == WI_WORLD_GUILDWAR)
	{
		g_GameTimer.SetFixed(TRUE);
		g_GameTimer.SetTime(0, 23, 0, 0);
	}
	else
		g_GameTimer.SetFixed(FALSE);

	CMover::SetActiveObj(NULL);
	g_WndMng.SetPlayer(g_pPlayer = NULL);
	g_DialogMsg.RemoveDeleteObjMsg();

	g_WorldMng.Open(g_Neuz.m_pd3dDevice, dwWorldID);
	g_WorldMng.Get()->RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
	g_WorldMng.Get()->SetCamera(&g_Neuz.m_camera);
	g_Neuz.m_camera.Transform(D3DDEVICE, g_WorldMng.Get());
	g_Neuz.m_camera.Reset();
	g_WorldMng.Get()->ReadWorld(vPos);
#ifdef __HOUSING
	if (dwWorldID == WI_WORLD_MINIROOM)
	{
		g_Neuz.m_camera.SetCamMode(CM_MYROOM);
	}
	else
	{
		g_Neuz.m_camera.SetCamMode(CM_NORMAL);
		CDeployManager::GetInstance()->SetMyRoom(FALSE);
	}
#endif

	D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
	rDestinationArrow = D3DXVECTOR3(-1.0F, 0.0F, -1.0F);

	if (pWndWorld)
		pWndWorld->SetWorldMusic(TRUE);

	CWndMapEx* pWndMapEx = (CWndMapEx*)g_WndMng.GetApplet(APP_MAP_EX);
	if (pWndMapEx)
		pWndMapEx->Reload();
}

void CDPClient::OnDoEquip(OBJID objid, CAr& ar)
{
	BOOL bEquip;
	DWORD dwItemId;
	DWORD dwPart;

	ar >> dwItemId >> bEquip;
	EQUIP_INFO equipInfo;
	ar.Read((void*)&equipInfo, sizeof(EQUIP_INFO));
	ar >> dwPart;

	CMover* pPlayer = prj.GetMover(objid);
	if (!IsValidObj(pPlayer))
		return;

	if (pPlayer->IsActiveMover())
	{
		CItemElem* pItemElem = pPlayer->m_Inventory.GetAtId(dwItemId);
		pPlayer->DoEquipItem(pItemElem);
		g_WndMng.UpdateWindowModels();
	}
	else
		pPlayer->DoEquipItemOther(equipInfo, bEquip, dwPart);

	pPlayer->UpdateParts(!pPlayer->IsActiveMover());
}

void CDPClient::OnTrade(OBJID objid, CAr& ar)
{
	u_long uidPlayer;
	ar >> uidPlayer;

	CMover* pTrader = prj.GetMover(objid);
	if (IsValidObj((CObj*)pTrader))
	{
		g_pPlayer->m_vtInfo.SetOther(pTrader);
		pTrader->m_vtInfo.SetOther(g_pPlayer);

		g_WndMng.CreateApplet(APP_INVENTORY);
		g_WndMng.CreateApplet(APP_TRADE);
	}
}

void CDPClient::OnConfirmTrade(OBJID objid, CAr& ar)
{
	CMover* pTrader = prj.GetMover(objid);
	if (pTrader != NULL)
	{
		if (g_Option.m_bTrade == FALSE)
		{
			SendBlock(3, pTrader->GetName());
			return;
		}

		CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_REPAIR);

		if (pWndBaseBuf)
		{
			SendBlock(4, pTrader->GetName());
			return;
		}

		pWndBaseBuf = g_WndMng.GetWndBase(APP_TRADE);
		if (pWndBaseBuf)
		{
			SendBlock(5, pTrader->GetName());
			return;
		}

		pWndBaseBuf = g_WndMng.GetWndBase(APP_CONFIRM_TRADE);
		if (pWndBaseBuf)
		{
			SendBlock(5, pTrader->GetName());
			return;
		}

		pWndBaseBuf = g_WndMng.GetWndBase(APP_GUILD_BANK);
		if (pWndBaseBuf)
		{
			SendBlock(8, pTrader->GetName());
			return;
		}
		pWndBaseBuf = g_WndMng.GetWndBase(APP_SUMMON_ANGEL);
		if (pWndBaseBuf)
		{
			SendBlock(8, pTrader->GetName());
			return;
		}

		SAFE_DELETE(g_WndMng.m_pWndConfirmTrade);
		g_WndMng.m_pWndConfirmTrade = new CWndConfirmTrade;
		g_WndMng.m_pWndConfirmTrade->Initialize(NULL, APP_CONFIRM_TRADE);
		g_WndMng.m_pWndConfirmTrade->OnSetName(pTrader->GetName(), objid);
	}
}

void CDPClient::OnConfirmTradeCancel(OBJID objid, CAr& ar)
{
	g_WndMng.PutString(prj.GetText(TID_GAME_TRADECANCEL), NULL, prj.GetTextColor(TID_GAME_TRADECANCEL));
}


void CDPClient::OnTradePut(OBJID objid, CAr& ar)
{
	BYTE byNth;
	int nItemNum;

	ar >> byNth >> nItemNum;

	CItemElem itemElem;
	itemElem.Serialize(ar);

	CMover* pOther = g_pPlayer->m_vtInfo.GetOther();
	if (pOther && pOther->m_vtInfo.GetOther() == g_pPlayer)
	{
		CMover* pPlayer = (objid == g_pPlayer->GetId() ? g_pPlayer : g_pPlayer->m_vtInfo.GetOther());
		pPlayer->m_Inventory.SetAtId(itemElem.m_dwObjId, &itemElem);
		pPlayer->m_vtInfo.TradeSetItem(itemElem.m_dwObjId, byNth, nItemNum);
	}
}

void CDPClient::OnTradePutError(OBJID objid, CAr& ar)
{
	g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0054)));

}

void CDPClient::OnTradePull(OBJID objid, CAr& ar)
{
	BYTE byNth;
	ar >> byNth;

	CMover* pOther = g_pPlayer->m_vtInfo.GetOther();
	if (pOther && pOther->m_vtInfo.GetOther() == g_pPlayer)
	{
		CMover* pPlayer = (objid == g_pPlayer->GetId() ? g_pPlayer : g_pPlayer->m_vtInfo.GetOther());
		pPlayer->m_vtInfo.TradeClearItem(byNth);
	}
}

void CDPClient::OnTradePutGold(OBJID objid, CAr& ar)
{
	DWORD dwGold;
	ar >> dwGold;

	CMover* pTrader = g_pPlayer->m_vtInfo.GetOther();
	ASSERT(pTrader);

	CWndTrade* pWndTrade = (CWndTrade*)g_WndMng.GetApplet(APP_TRADE);
	if (pWndTrade)
	{
		if (g_pPlayer->GetId() == objid)
		{
			g_pPlayer->AddGold((int)((-1) * (int)(dwGold)), FALSE);		//raiders.2006.11.28
			g_pPlayer->m_vtInfo.TradeSetGold(dwGold);
			pWndTrade->m_nGoldI = dwGold;
		}
		else
		{
			pTrader->AddGold((int)((-1) * (int)(dwGold)), FALSE);			//raiders.2006.11.28
			pTrader->m_vtInfo.TradeSetGold(dwGold);
			pWndTrade->m_nGoldYou = dwGold;
		}
	}
}

void CDPClient::SendTradeOk(void)
{
	SendHdr(PACKETTYPE_TRADEOK);
}

void CDPClient::SendChangeFace(DWORD dwFaceNum)
{
	if (g_pPlayer->HasBuffByIk3(IK3_TEXT_DISGUISE))
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_ERROR_BEAUTYSHOP_DISGUISE), NULL, prj.GetTextColor(TID_GAME_ERROR_BEAUTYSHOP_DISGUISE));
	}
	else
	{
		BEFORESENDSOLE(ar, PACKETTYPE_CHANGEFACE, DPID_UNKNOWN);
		BOOL bUseCoupon = FALSE;
		if (g_WndMng.m_pWndFaceShop != NULL)
		{
			bUseCoupon = g_WndMng.m_pWndFaceShop->m_bUseCoupon;
		}
		ar << dwFaceNum << bUseCoupon;

		SEND(ar, this, DPID_SERVERPLAYER);
	}
}

void CDPClient::OnTradeCancel(OBJID objid, CAr& ar)
{
	int nMode;
	u_long uidPlayer;
	ar >> uidPlayer;
	ar >> nMode;

	CMover* pTrader = g_pPlayer->m_vtInfo.GetOther();
	if (pTrader)
		pTrader->m_vtInfo.TradeClear();
	g_pPlayer->m_vtInfo.TradeClear();

	CWndTrade* pWndTrade = (CWndTrade*)g_WndMng.GetApplet(APP_TRADE);
	if (pWndTrade)
		pWndTrade->DoCancel();

	if (nMode)
	{
		if (uidPlayer != g_pPlayer->m_idPlayer)
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0002)));
	}
	else
	{
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0003)));
		CWndTradeConfirm* pWndTradeConfirm = (CWndTradeConfirm*)g_WndMng.GetWndBase(APP_TRADE_CONFIRM);
		if (pWndTradeConfirm)
			pWndTradeConfirm->Destroy();
	}

	SAFE_DELETE(g_WndMng.m_pWndTradeGold);
}

void CDPClient::OnTradelastConfirmOk(OBJID objid, CAr& ar)
{
	if (objid == g_pPlayer->GetId())
	{
		CWndTradeConfirm* pWndTradeConfirm = (CWndTradeConfirm*)g_WndMng.GetWndBase(APP_TRADE_CONFIRM);
		if (pWndTradeConfirm)
		{
			pWndTradeConfirm->bMsg = TRUE;
			CString str = prj.GetText(TID_GAME_WAITCOMFIRM);
			CWndStatic* pWndStatic = (CWndStatic*)pWndTradeConfirm->GetDlgItem(WIDC_STATIC1);
			pWndStatic->SetTitle(str);
			CWndButton* pWndButtonOk = (CWndButton*)pWndTradeConfirm->GetDlgItem(WIDC_YES);
			pWndButtonOk->SetVisible(FALSE);
			CWndButton* pWndButtonNO = (CWndButton*)pWndTradeConfirm->GetDlgItem(WIDC_NO);
			pWndButtonNO->SetVisible(FALSE);
		}
	}
	else
	{
		if (g_pPlayer->m_vtInfo.GetOther())
		{
			CString str;
			str.Format(prj.GetText(TID_GAME_FINALCOMFIRM), g_pPlayer->m_vtInfo.GetOther()->GetName());
			g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_TRADEACCPET));
		}
	}
}

void CDPClient::OnTradeOk(OBJID objid, CAr& ar)
{
	CWndTrade* pWndTrade = (CWndTrade*)g_WndMng.GetApplet(APP_TRADE);
	if (pWndTrade)
	{
		CWndButton* pWndOkI = (CWndButton*)pWndTrade->GetDlgItem(WIDC_OK_I);
		CWndButton* pWndOkYou = (CWndButton*)pWndTrade->GetDlgItem(WIDC_OK_YOU);
		if (objid == g_pPlayer->GetId())
		{
			pWndOkI->SetCheck(TRUE);
			pWndOkI->EnableWindow(FALSE);
		}
		else
		{
			pWndOkYou->SetCheck(TRUE);
			pWndOkYou->EnableWindow(FALSE);
			if (g_pPlayer->m_vtInfo.GetOther())
			{
				CString str;
				str.Format(prj.GetText(TID_GAME_TRADEACCPET), g_pPlayer->m_vtInfo.GetOther()->GetName());
				g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_TRADEACCPET));
			}
		}
	}
}

void CDPClient::OnTradeConsent(void)
{
	if (g_pPlayer->m_vtInfo.TradeConsent())
	{
		CWndTrade* pWndTrade = (CWndTrade*)g_WndMng.GetApplet(APP_TRADE);
		if (pWndTrade)
		{
			pWndTrade->Destroy();
		}

		CWndTradeConfirm* pWndTradeConfirm = (CWndTradeConfirm*)g_WndMng.GetWndBase(APP_TRADE_CONFIRM);
		if (pWndTradeConfirm)
		{
			pWndTradeConfirm->Destroy();
		}
	}
}

void CDPClient::OnTradelastConfirm(void)
{
	SAFE_DELETE(g_WndMng.m_pWndTradeConfirm);
	g_WndMng.m_pWndTradeConfirm = new CWndTradeConfirm;
	g_WndMng.m_pWndTradeConfirm->Initialize(NULL, APP_TRADE_CONFIRM);

	CWndTrade* pWndTrade = (CWndTrade*)g_WndMng.GetApplet(APP_TRADE);
	if (pWndTrade)
	{
		CWndButton* pWndOkI = (CWndButton*)pWndTrade->GetDlgItem(WIDC_OK_I);
		CWndButton* pWndOkYou = (CWndButton*)pWndTrade->GetDlgItem(WIDC_OK_YOU);
		pWndOkI->SetCheck(TRUE);
		pWndOkI->EnableWindow(FALSE);
		pWndOkYou->SetCheck(TRUE);
		pWndOkYou->EnableWindow(FALSE);
	}
}


void CDPClient::OnOpenShopWnd(OBJID objid, CAr& ar)
{
	CMover* pVendor = prj.GetMover(objid);
	if (IsValidObj((CObj*)pVendor))
	{
		for (int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++)
		{
			pVendor->m_ShopInventory[i]->Serialize(ar);

			// Serialize prices
			int nItemsCount;
			ar >> nItemsCount;

			BYTE nItem;
			int nShopCost;

			for (int z = 0; z < nItemsCount; z++)
			{
				ar >> nItem;
				ar >> nShopCost;

				CItemElem* pItemElem = pVendor->m_ShopInventory[i]->GetAtId(nItem);
				if (pItemElem)
					pItemElem->m_nShopCost = nShopCost;
			}

		}

		SAFE_DELETE(g_WndMng.m_pWndShop);
		g_WndMng.m_pWndShop = new CWndShop;
		g_WndMng.m_pWndShop->m_pMover = pVendor;
		g_pPlayer->m_vtInfo.SetOther(pVendor);

		g_WndMng.CreateApplet(APP_INVENTORY);
		g_WndMng.m_pWndShop->Initialize(NULL, APP_SHOP_);
	}
	else
	{
		CItemContainer<CItemElem> dump;
		dump.SetItemContainer(ITYPE_ITEM, MAX_VENDOR_INVENTORY);
		for (int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++)
			dump.Serialize(ar);	// waste
	}
}

void CDPClient::OnPutItemBank(OBJID objid, CAr& ar)
{
	CItemElem itemElem;
	BYTE nSlot;
	ar >> nSlot;
	itemElem.Serialize(ar);
	g_pPlayer->m_Bank[nSlot].Add(&itemElem);
}

void CDPClient::OnPutItemGuildBank(OBJID objid, CAr& ar)
{
	BYTE mode = 0;
	ar >> mode;

	CItemElem itemElem;
	itemElem.Serialize(ar);
	g_pPlayer->GetGuild()->m_GuildBank.Add(&itemElem);
}

void CDPClient::OnGetItemGuildBank(OBJID objid, CAr& ar)
{
	BYTE mode = 0;
	ar >> mode;

	switch (mode)
	{
	case	GUILD_PENYA_MINE_UPDATE:
	{
		DWORD	Gold = 0;
		u_long	playerID;
		BYTE	cbCloak;

		ar >> Gold;
		ar >> playerID;
		ar >> cbCloak;

		//g_pPlayer->m_dwGold += Gold;
		g_pPlayer->AddGold(Gold, FALSE);

		if (g_pPlayer->GetGuild()->m_nGoldGuild > Gold)
			g_pPlayer->GetGuild()->m_nGoldGuild -= Gold;
		else
			g_pPlayer->GetGuild()->m_nGoldGuild = 0;

		if (cbCloak != 1)
		{

			g_pPlayer->GetGuild()->DecrementMemberContribution(playerID, Gold, 0);
		}
		UpdateGuildWnd();

	}
	break;
	case	GUILD_PENYA_ALL_UPDATE:
	{
		DWORD	Gold = 0;
		u_long	playerID;
		BYTE	cbCloak;

		ar >> Gold;
		ar >> playerID;
		ar >> cbCloak;

		if (g_pPlayer->GetGuild()->m_nGoldGuild > Gold)
			g_pPlayer->GetGuild()->m_nGoldGuild -= Gold;
		else
			g_pPlayer->GetGuild()->m_nGoldGuild = 0;

		if (cbCloak != 1)
		{

			g_pPlayer->GetGuild()->DecrementMemberContribution(playerID, Gold, 0);
		}
		UpdateGuildWnd();
	}
	break;

	case	GUILD_ITEM_MINE_UPDATE:
	{
		CItemElem itemElem;
		itemElem.Serialize(ar);

		CItemElem* pTempElem = g_pPlayer->GetGuild()->m_GuildBank.GetAtId(itemElem.m_dwObjId);
		if (pTempElem)
		{
			g_pPlayer->m_Inventory.Add(&itemElem);

			if (pTempElem->m_nItemNum > itemElem.m_nItemNum)
				pTempElem->m_nItemNum -= itemElem.m_nItemNum;
			else
				g_pPlayer->GetGuild()->m_GuildBank.RemoveAtId(itemElem.m_dwObjId);
		}
	}
	break;
	case	GUILD_ITEM_ALL_UPDATE:
	{
		CItemElem itemElem;
		itemElem.Serialize(ar);

		CItemElem* pTempElem = g_pPlayer->GetGuild()->m_GuildBank.GetAtId(itemElem.m_dwObjId);
		if (pTempElem)
		{
			if (pTempElem->m_nItemNum > itemElem.m_nItemNum)
				pTempElem->m_nItemNum -= itemElem.m_nItemNum;
			else
				g_pPlayer->GetGuild()->m_GuildBank.RemoveAtId(itemElem.m_dwObjId);
		}
	}
	break;
	}
}

void CDPClient::OnGetItemBank(OBJID objid, CAr& ar)
{
	CItemElem itemElem;
	itemElem.Serialize(ar);
	g_pPlayer->m_Inventory.Add(&itemElem);
}

void CDPClient::OnPutGoldBank(OBJID objid, CAr& ar)
{
	BYTE nSlot;
	DWORD dwGold;

	ar >> nSlot;
	ar >> dwGold >> g_pPlayer->m_dwGoldBank[nSlot];
	g_pPlayer->SetGold(dwGold);
}
void CDPClient::OnMoveBankItem(OBJID objid, CAr& ar)
{
}

void CDPClient::OnErrorBankIsFull(OBJID objid, CAr& ar)
{
	g_WndMng.PutString(prj.GetText(TID_GAME_SLOTFULL), NULL, prj.GetTextColor(TID_GAME_SLOTFULL));
}

void CDPClient::OnBankWindow(OBJID objid, CAr& ar)
{
	int nMode;
	ar >> nMode;
	DWORD dwId, dwItemId;
	ar >> dwId >> dwItemId;

	SAFE_DELETE(g_WndMng.m_pWndBank);
	SAFE_DELETE(g_WndMng.m_pWndConfirmBank);
	SAFE_DELETE(g_WndMng.m_pWndBankPassword);
	if (nMode)
	{
		g_WndMng.m_pWndConfirmBank = new CWndConfirmBank;
		g_WndMng.m_pWndConfirmBank->SetItem(dwId, dwItemId);
		g_WndMng.m_pWndConfirmBank->Initialize(NULL, APP_CONFIRM_BANK);
	}
	else
	{
		g_WndMng.m_pWndBankPassword = new CWndBankPassword;
		g_WndMng.m_pWndBankPassword->SetItem(dwId, dwItemId);
		g_WndMng.m_pWndBankPassword->SetBankPassword(0);
		g_WndMng.m_pWndBankPassword->Initialize(NULL, APP_BANK_PASSWORD);

	}
}

void CDPClient::OnGuildBankWindow(OBJID objid, CAr& ar)
{
	int nMode;
	ar >> nMode;

	switch (nMode)
	{
	case 0:
	{
		SAFE_DELETE(g_WndMng.m_pWndGuildBank);
		g_WndMng.m_pWndGuildBank = new CWndGuildBank;

		int nGuildGold = 0;
		ar >> nGuildGold;

		g_pPlayer->GetGuild()->m_nGoldGuild = nGuildGold;
		g_pPlayer->GetGuild()->m_GuildBank.Serialize(ar);

		g_WndMng.CreateApplet(APP_INVENTORY);

		g_WndMng.m_pWndGuildBank->Initialize(NULL, APP_GUILD_BANK);
	}
	break;
	default:
		break;
	}
}

void CDPClient::OnChangeBankPass(OBJID objid, CAr& ar)
{
	int nMode;
	ar >> nMode;
	DWORD dwId, dwItemId;
	ar >> dwId >> dwItemId;

	SAFE_DELETE(g_WndMng.m_pWndBank);
	SAFE_DELETE(g_WndMng.m_pWndConfirmBank);
	SAFE_DELETE(g_WndMng.m_pWndBankPassword);
	if (nMode)
	{
		g_WndMng.m_pWndConfirmBank = new CWndConfirmBank;
		g_WndMng.m_pWndConfirmBank->SetItem(dwId, dwItemId);
		g_WndMng.m_pWndConfirmBank->Initialize(NULL, APP_CONFIRM_BANK);
	}
	else
	{
		g_WndMng.m_pWndBankPassword = new CWndBankPassword;
		g_WndMng.m_pWndBankPassword->SetItem(dwId, dwItemId);
		g_WndMng.m_pWndBankPassword->SetBankPassword(1);
		g_WndMng.m_pWndBankPassword->Initialize(NULL, APP_BANK_PASSWORD);

		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0028)));

	}
}

void CDPClient::OnConfirmBankPass(OBJID objid, CAr& ar)
{
	int nMode;
	ar >> nMode;
	DWORD dwId, dwItemId;
	ar >> dwId >> dwItemId;

	SAFE_DELETE(g_WndMng.m_pWndBank);
	SAFE_DELETE(g_WndMng.m_pWndConfirmBank);
	SAFE_DELETE(g_WndMng.m_pWndBankPassword);
	if (nMode)
	{
		g_WndMng.CreateApplet(APP_INVENTORY);
		g_WndMng.m_pWndBank = new CWndBank;
		g_WndMng.m_pWndBank->Initialize(&g_WndMng, APP_COMMON_BANK);
	}
	else
	{
		g_WndMng.m_pWndConfirmBank = new CWndConfirmBank;
		g_WndMng.m_pWndConfirmBank->SetItem(dwId, dwItemId);
		g_WndMng.m_pWndConfirmBank->Initialize(NULL, APP_CONFIRM_BANK);
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0028)));
	}
}

void CDPClient::OnUpdateVendor(OBJID objid, CAr& ar)
{
	CHAR cTab;
	BYTE nId;
	short nNum;

	ar >> cTab >> nId >> nNum;

	CMover* pVendor = prj.GetMover(objid);
	if (IsValidObj((CObj*)pVendor))
	{
		CItemElem* pItemElem = pVendor->m_ShopInventory[cTab]->GetAtId(nId);
		if (pItemElem)
		{
			if (nNum == 0)
			{
				pVendor->m_ShopInventory[cTab]->RemoveAtId(nId);
			}
			else
			{
				if (pItemElem)
					pItemElem->m_nItemNum = nNum;
			}

			CWndShop* pWndShop = (CWndShop*)g_WndMng.GetApplet(APP_SHOP_);
			if (pWndShop)
			{
				if (pWndShop->m_pWndConfirmSell->m_pItemElem == pItemElem)
					SAFE_DELETE(pWndShop->m_pWndConfirmSell);
			}
		}
	}
}

#if __VER >= 11 // __SYS_IDENTIFY
void CDPClient::OnUpdateItemEx(OBJID objid, CAr& ar)
{
	unsigned char id;
	char cParam;
	__int64 iValue;

	ar >> id >> cParam >> iValue;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover) == FALSE)
		return;

	pMover->UpdateItemEx(id, cParam, iValue);
}
#endif	// __SYS_IDENTIFY

void CDPClient::OnUpdateItem(OBJID objid, CAr& ar)
{
	CHAR cType, cParam;
	BYTE nId;
	DWORD dwValue;

	ar >> cType >> nId >> cParam >> dwValue;
	DWORD dwTime;
	ar >> dwTime;
	int nType;
	ar >> nType;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover) == FALSE)
		return;

	if (cParam == UI_COOLTIME)
	{
		CMover* pPlayer = CMover::GetActiveMover();
		if (!pPlayer)
			return;

		if (pMover == pPlayer)
		{
			CItemContainer<CItemElem>* pContainer = pPlayer->GetContainerByType(nType);
			if (!pContainer)
				return;

			CItemElem* pItemBase = pContainer->GetAtId(nId);
			if (!pItemBase)
				return;

			ItemProp* pItemProp = pItemBase->GetProp();
			if (!pItemProp)
				return;

			DWORD dwGroup = g_pPlayer->m_cooltimeMgr.GetGroup(pItemProp);
			if (dwGroup)
				pPlayer->m_cooltimeMgr.SetTime(dwGroup, pItemProp->GetCoolTime());
		}
	}


	pMover->UpdateItem(nId, cParam, dwValue, dwTime, nType);

	CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
	if (pWndQuestDetail)
		pWndQuestDetail->UpdateQuestText();

	CWndQuestQuickInfo* pWndQuestQuickInfo = g_WndMng.m_pWndQuestQuickInfo;
	if (pWndQuestQuickInfo)
		pWndQuestQuickInfo->SetUpdateTextSwitch(TRUE);
}


void CDPClient::OnSetDestParam(OBJID objid, CAr& ar)
{
	int nDstParameter, nAdjParameterValue, nChgParameterValue;
	ar >> nDstParameter >> nAdjParameterValue >> nChgParameterValue;

	CMover* pMover = prj.GetMover(objid);
	if (TRUE == IsValidObj((CObj*)pMover))
	{
		pMover->SetDestParam(nDstParameter, nAdjParameterValue, nChgParameterValue);

		if (pMover->IsSMMode(SM_MAINTAIN_MP))
			pMover->SetManaPoint(pMover->GetMaxManaPoint());

		if (pMover->IsSMMode(SM_MAINTAIN_FP))
			pMover->SetFatiguePoint(pMover->GetMaxFatiguePoint());
	}
}

void CDPClient::OnResetDestParam(OBJID objid, CAr& ar)
{
	int nDstParameter, nAdjParameterValue;
	ar >> nDstParameter >> nAdjParameterValue;

	CMover* pMover = prj.GetMover(objid);
	if (TRUE == IsValidObj((CObj*)pMover))
	{
		pMover->ResetDestParam(nDstParameter, nAdjParameterValue, 0);

		if (pMover->GetHitPoint() > pMover->GetMaxHitPoint())
			pMover->SetHitPoint(pMover->GetMaxHitPoint());

		if (pMover->GetManaPoint() > pMover->GetMaxManaPoint())
			pMover->SetManaPoint(pMover->GetMaxManaPoint());

		if (pMover->GetFatiguePoint() > pMover->GetMaxFatiguePoint())
			pMover->SetFatiguePoint(pMover->GetMaxFatiguePoint());
	}
}

#ifdef __SPEED_SYNC_0108
void CDPClient::OnResetDestParamSync(OBJID objid, CAr& ar)
{
	int nDstParameter, nAdjParameterValue, nParameterValue;
	ar >> nDstParameter >> nAdjParameterValue >> nParameterValue;

	CMover* pMover = prj.GetMover(objid);
	if (TRUE == IsValidObj((CObj*)pMover))
	{
		pMover->ResetDestParamSync(nDstParameter, nAdjParameterValue, nParameterValue, 0);
	}
}
#endif

void CDPClient::OnSetPointParam(OBJID objid, CAr& ar)
{
	int nDstParameter, nValue;
	ar >> nDstParameter >> nValue;

	CMover* pMover = prj.GetMover(objid);
	if (TRUE == IsValidObj((CObj*)pMover))
	{
		pMover->SetPointParam(nDstParameter, nValue);
	}
}


void CDPClient::OnResurrection(OBJID objid)
{
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		pMover->m_pActMover->SendActMsg(OBJMSG_RESURRECTION);
		CreateSfx(g_Neuz.m_pd3dDevice, XI_SKILL_ASS_HEAL_RESURRECTION01, pMover->GetPos(), objid, pMover->GetPos(), objid);

		if (g_WndMng.m_pWndRevival && pMover->IsActiveMover())
			g_WndMng.m_pWndRevival->Destroy();
	}
}

void CDPClient::OnResurrectionMessage()
{
	if (g_WndMng.m_pWndResurrectionConfirm == NULL)
	{
		g_WndMng.m_pWndResurrectionConfirm = new CWndResurrectionConfirm;
		g_WndMng.m_pWndResurrectionConfirm->Initialize();
	}
}


void CDPClient::OnSetLevel(OBJID objid, CAr& ar)
{
	WORD wLevel;
	ar >> wLevel;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		if ((int)wLevel > pMover->m_nLevel)
		{
			if (pMover->m_pActMover && (pMover->m_pActMover->IsState(OBJSTA_STAND) || pMover->m_pActMover->IsState(OBJSTA_STAND2)))
				pMover->SetMotion(MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED);

			CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_LEVEL_UP01, pMover->GetPos(), pMover->GetId());
		}


		pMover->m_nLevel = (int)wLevel;
		pMover->m_nHitPoint = pMover->GetMaxHitPoint();
		pMover->m_nManaPoint = pMover->GetMaxManaPoint();
	}
}

void CDPClient::OnSetFlightLevel(OBJID objid, CAr& ar)
{
	WORD wFlightLv;
	ar >> wFlightLv;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		if ((int)wFlightLv > pMover->GetFlightLv())
			CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_LEVEL_UP01, pMover->GetPos(), pMover->GetId());
	}
}


void CDPClient::OnSetExperience(OBJID objid, CAr& ar)
{
	EXPINTEGER nExp1;
	WORD wLevel;

	int nSkillLevel;
	int nSkillPoint;
	ar >> nExp1 >> wLevel;
	ar >> nSkillLevel >> nSkillPoint;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
#ifdef __DISCORD // Update when set new level
		int nOld = pMover->GetLevel();
#endif

		pMover->SetExperience(nExp1, (int)wLevel);

#ifdef __DISCORD // Update when set new level
		if (g_pPlayer == pMover && nOld != wLevel)
			g_Neuz.ManageDiscord(DIS_CHARACTER_DETAILS);
#endif

		int nSP = pMover->m_nSkillPoint;
		pMover->m_nSkillLevel = nSkillLevel;
		pMover->m_nSkillPoint = nSkillPoint;

		if (nSP < nSkillPoint)
		{
			CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_LEVEL_UP01, g_pPlayer->GetPos(), g_pPlayer->GetId());

			if (pMover == g_pPlayer)
			{
				PlayMusic(BGM_IN_LEVELUP);
				if (g_WndMng.m_pWndWorld)
				{
					if (::GetLanguage() != LANG_JAP)
						g_Caption1.AddCaption(prj.GetText(TID_GAME_SKILLPOINT_UP), g_WndMng.m_pWndWorld->m_pFontAPICaption);// CWndBase::m_Theme.m_pFontCaption );
					else
						g_Caption1.AddCaption(prj.GetText(TID_GAME_SKILLPOINT_UP), NULL);// CWndBase::m_Theme.m_pFontCaption );

					g_WndMng.m_pWndWorld->GetAdvMgr()->AddAdvButton(APP_SKILL4);
				}
			}
		}

		if (pMover == g_pPlayer)
		{
			CWndSkill_16* pSkillTree = (CWndSkill_16*)g_WndMng.GetWndBase(APP_SKILL4);
			if (pSkillTree && nSP != nSkillPoint)
				pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill);
		}
	}
}

void CDPClient::OnSetFxp(OBJID objid, CAr& ar)
{
	WORD wFxp, wFlightLv;
	ar >> wFxp >> wFlightLv;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		pMover->SetFxp((int)wFxp, (int)wFlightLv);
	}
}

void CDPClient::OnSetSkillLevel(OBJID objid, CAr& ar)
{
	DWORD dwSkill, dwLevel;
	ar >> dwSkill >> dwLevel;
}

void CDPClient::OnSetGrowthLearningPoint(OBJID objid, CAr& ar)
{
	long nRemainGP, nRemainLP;
	ar >> nRemainGP >> nRemainLP;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
		pMover->m_nRemainGP = nRemainGP;
}

void CDPClient::OnSetChangeJob(OBJID objid, CAr& ar)
{
	int nJob;
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		ar >> nJob;
		pMover->m_nJob = nJob;

		for (int i = MAX_JOB_SKILL; i < MAX_SKILL_JOB; i++)
		{
			pMover->m_aJobSkill[i].dwSkill = NULL_ID;
			pMover->m_aJobSkill[i].dwLevel = 0;
		}

		ar.Read((void*)&pMover->m_aJobSkill[0], sizeof(SKILL) * MAX_SKILL_JOB);

		DWORD dwJobLv[MAX_JOB];
		ar.Read((void*)dwJobLv, sizeof(DWORD) * MAX_JOB);

		CWndSkill_16* pSkillTree = (CWndSkill_16*)g_WndMng.GetWndBase(APP_SKILL4);
		if (pSkillTree)
			pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill);

		if (pMover->IsActiveMover())
		{

			if (pMover->m_pActMover && (pMover->m_pActMover->IsState(OBJSTA_STAND) || pMover->m_pActMover->IsState(OBJSTA_STAND2)))
				pMover->SetMotion(MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED);

			CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_LEVEL_UP01, pMover->GetPos(), pMover->GetId());

			PlayMusic(BGM_IN_LEVELUP);
			CString str;
			str.Format(prj.GetText(TID_EVE_CHGJOB), pMover->GetJobString());
			g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_EVE_CHGJOB));
		}
	}
	else
	{
		ar >> nJob;
		CMover TempMover;

		TempMover.m_nJob = nJob;
		if (TempMover.IsExpert())
			ar.Read((void*)&TempMover.m_aJobSkill[MAX_JOB_SKILL], sizeof(SKILL) * (MAX_EXPERT_SKILL));
		else if (TempMover.IsLegendHero())
			ar.Read((void*)&TempMover.m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL));
		else if (TempMover.IsHero())
			ar.Read((void*)&TempMover.m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL));
		else if (TempMover.IsMaster())
			ar.Read((void*)&TempMover.m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL));
		else
			ar.Read((void*)&TempMover.m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL));

		DWORD dwJobLv[MAX_JOB];
		ar.Read((void*)dwJobLv, sizeof(DWORD) * MAX_JOB);
	}
}

void CDPClient::OnSetNearChangeJob(OBJID objid, CAr& ar)
{
	int nJob;
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		ar >> nJob;
		pMover->m_nJob = nJob;

		if (pMover->IsExpert())
			ar.Read((void*)&pMover->m_aJobSkill[MAX_JOB_SKILL], sizeof(SKILL) * (MAX_EXPERT_SKILL));
		else if (pMover->IsLegendHero())
			ar.Read((void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL));
		else if (pMover->IsHero())
			ar.Read((void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL));
		else if (pMover->IsMaster())
			ar.Read((void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL));
		else
			ar.Read((void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL));


		CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_LEVEL_UP01, pMover->GetPos(), pMover->GetId());

		if (pMover->m_pActMover && (pMover->m_pActMover->IsState(OBJSTA_STAND) || pMover->m_pActMover->IsState(OBJSTA_STAND2)))
			pMover->SetMotion(MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED);
	}
	else
	{
		ar >> nJob;
		CMover TempMover;

		TempMover.m_nJob = nJob;
		if (TempMover.IsExpert())
			ar.Read((void*)&TempMover.m_aJobSkill[MAX_JOB_SKILL], sizeof(SKILL) * (MAX_EXPERT_SKILL));
		else if (TempMover.IsLegendHero())
			ar.Read((void*)&TempMover.m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL));
		else if (TempMover.IsHero())
			ar.Read((void*)&TempMover.m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL));
		else if (TempMover.IsMaster())
			ar.Read((void*)&TempMover.m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL));
		else
			ar.Read((void*)&TempMover.m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL));
	}
}

void CDPClient::OnSetDestPos(OBJID objid, CAr& ar)
{
	D3DXVECTOR3 vPos;
	BYTE fForward;
	ar >> vPos >> fForward;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
		pMover->SetDestPos(vPos, (fForward != 0), FALSE);
}

void CDPClient::OnSetMovePattern(OBJID objid, CAr& ar)
{
	FLOAT fAngle, fAngleX;
	D3DXVECTOR3 vPos;
	int		nPattern;

	ar >> nPattern >> vPos >> fAngle >> fAngleX;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		pMover->SetMovePattern(nPattern);
		pMover->SetPos(vPos);
		pMover->SetAngle(fAngle);
		pMover->SetAngleX(fAngleX);
	}
}

void CDPClient::OnMeleeAttack(OBJID objid, CAr& ar)
{
	OBJMSG dwAtkMsg;
	OBJID id;
	ar >> dwAtkMsg >> id;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
		pMover->DoAttackMelee(id, dwAtkMsg);
}

void CDPClient::OnMeleeAttack2(OBJID objid, CAr& ar)
{
	OBJID id;
	ar >> id;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		if (pMover->SendActMsg(OBJMSG_ATK1, id) == 0)
			pMover->m_pActMover->m_qMeleeAtkMsg.AddTail(new ACTMSG(OBJMSG_ATK1, id, 0, 0));
	}
}


void CDPClient::OnMagicAttack(OBJID objid, CAr& ar)
{
	OBJID id;
	int	nMagicPower, idSfxHit;
	ar >> id >> nMagicPower >> idSfxHit;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		CCtrl* pTarget = prj.GetCtrl(id);
		if (IsValidObj(pTarget))
		{
			if (pMover->DoAttackMagic(pTarget, nMagicPower, idSfxHit) == 0)
				pMover->m_pActMover->m_qMagicAtkMsg.AddTail(new MAGICATKMSG(OBJMSG_ATK_MAGIC1, id, 0, 0, nMagicPower, idSfxHit));
		}
	}
}

void CDPClient::OnRangeAttack(OBJID objid, CAr& ar)
{
	OBJID id;
	int nParam2;
	int	idSfxHit;

	ar >> id >> nParam2 >> idSfxHit;

	CMover* pMover = prj.GetMover(objid);
	CMover* pTarget = prj.GetMover(id);

	if (IsValidObj(pMover))
	{
		if (IsValidObj(pTarget))
		{
			if (pMover->DoAttackRange(pTarget, nParam2, idSfxHit) == 0)
				pMover->m_pActMover->m_qMagicAtkMsg.AddTail(new MAGICATKMSG(OBJMSG_ATK_RANGE1, id, 1, nParam2, 0, idSfxHit));
		}
	}
}

void CDPClient::OnAttackSP(OBJID objid, CAr& ar)
{
	DWORD dwAtkMsg;
	OBJID id;
	int nParam2, nParam3;
	DWORD dwItemID;

	ar >> dwAtkMsg >> id >> nParam2 >> nParam3;

	dwItemID = nParam2;
	CMover* pMover = prj.GetMover(objid);
	CMover* pTarget = prj.GetMover(id);
	if (IsValidObj(pMover))
		pMover->DoAttackSP(pTarget, dwItemID);
}

void CDPClient::OnUseSkill(OBJID objid, CAr& ar)
{
	OBJID id;
	int nUseType = 0;
	int nCastingTime = 0;
	DWORD dwSkill, dwLevel;
	ar >> dwSkill >> dwLevel;
	ar >> id >> nUseType >> nCastingTime;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		pMover->m_dwReqFlag &= (~REQ_USESKILL);
		BOOL bSuccess = pMover->DoUseSkill(0, -1, id, (SKILLUSETYPE)nUseType, FALSE, nCastingTime, dwSkill, dwLevel);
		if (bSuccess)
		{
			if (g_Option.m_bAutoAttack && pMover->IsActiveMover())
			{
				ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp();
				if (pItemProp)
				{
					if (pItemProp->dwItemKind3 != IK3_STAFF && pItemProp->dwItemKind3 != IK3_CHEERSTICK && pItemProp->dwItemKind3 != IK3_WAND)
					{
						ItemProp* pSkillProp;
						pSkillProp = prj.GetSkillProp(dwSkill);
						if (g_WndMng.m_pWndWorld && pSkillProp && pSkillProp->dwExeTarget != EXT_SELFCHGPARAMET &&
							dwSkill != SI_KNT_HERO_DRAWING)
						{
							g_WndMng.m_pWndWorld->m_bAutoAttack = TRUE;
						}
					}
				}
			}
		}
	}
}

void CDPClient::OnClearUseSkill()
{
	g_pPlayer->m_dwReqFlag &= (~REQ_USESKILL);

	if (g_WndMng.m_pWndWorld)
		g_WndMng.m_pWndWorld->SetNextSkill(NEXTSKILL_NONE);

	if (g_WndMng.m_pWndTaskBar && g_WndMng.m_pWndTaskBar->m_nExecute != 0)
	{
		g_WndMng.m_pWndTaskBar->m_nExecute = 0;
		g_WndMng.m_pWndTaskBar->OnCancelSkill();
	}
}

void CDPClient::OnClearUseObject()
{
	g_pPlayer->m_dwReqFlag &= (~REQ_USEOBJECT);
	g_WorldMng()->SetObjFocus(NULL);
}

void CDPClient::OnSetFuel(OBJID objid, CAr& ar)
{
	int nFuel;
	int	tmAccFuel;
	ar >> nFuel >> tmAccFuel;

	CMover* pPlayer = prj.GetMover(objid);
	if (IsValidObj((CObj*)pPlayer))
	{
		pPlayer->m_nFuel = nFuel;
		if (nFuel == 0)
			pPlayer->SendActMsg(OBJMSG_ACC_STOP);
		pPlayer->m_tmAccFuel = tmAccFuel;
	}
}



void CDPClient::OnCreateSfxObj(OBJID objid, CAr& ar)
{
	DWORD dwSfxObj;
	float	x, y, z;
	BOOL	bFlag;
	ar >> dwSfxObj >> x >> y >> z;
	ar >> bFlag;

	D3DXVECTOR3 vWorld(x, y, z);
	D3DXVECTOR3 vPos;

	CCtrl* pObj = prj.GetCtrl(objid);
	if (IsValidObj((CObj*)pObj))
	{
		OBJID idObj = pObj->GetId();
		if (x || y || z)
		{
			idObj = NULL_ID;
			vPos = vWorld;
		}
		else
			vPos = pObj->GetPos();
		CreateSfx(g_Neuz.m_pd3dDevice, dwSfxObj, vPos, idObj, vPos, idObj);
	}
	else
	{
		if (x || y || z)
			CreateSfx(g_Neuz.m_pd3dDevice, dwSfxObj, vWorld, NULL_ID, vWorld, NULL_ID);
	}
}

void CDPClient::OnCreateSfxAllow(OBJID objid, CAr& ar)
{
	DWORD dwSfxObjAllow;
	DWORD dwSfxObjHit;
	D3DXVECTOR3 v3DestPos;
	int   idTarget;
	ar >> dwSfxObjAllow >> dwSfxObjHit >> v3DestPos.x >> v3DestPos.y >> v3DestPos.z >> idTarget;

	CCtrl* pObj = prj.GetCtrl(objid);
	if (IsValidObj((CObj*)pObj))
	{
		CMover* pHit = prj.GetMover(idTarget);

		if (IsInvalidObj(pHit))
			return;

		CMover* pMover = (CMover*)pObj;

		D3DXVECTOR3 vPosDest = pHit->GetPos() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 vPosSrc = pMover->GetPos() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		CSfx* pSfx = NULL;

		pSfx = pMover->CreateSfxArrow(dwSfxObjAllow, dwSfxObjHit, vPosDest, idTarget);
	}
}


void CDPClient::OnDefinedText(CAr& ar)
{
	TCHAR lpString[1024], argv[32][256], * pargv[32];
	int argc = 0;

	DWORD dwText;
	ar >> dwText;
	ar.ReadString(lpString, 1024);

	CScanner s(lpString);
	s.GetToken();
	while (s.tok != FINISHED)
	{
#if __VER >= 9	// __ULTIMATE
		s.Token.Replace(" @", "");
#endif
		_tcscpy(argv[argc], s.Token);
		argc++;
		s.GetToken();
	}

	for (int i = 0; i < argc; i++)
	{
		pargv[i] = argv[i];
	}

	CString string;
	string.FormatV(prj.GetText(dwText), (va_list)pargv);


	g_WndMng.PutString((LPCTSTR)string, NULL, prj.GetTextColor(dwText));
}

void CDPClient::OnChatText(CAr& ar)
{
	TCHAR lpString[1024], * pargv[32];
	memset(pargv, 0, sizeof(pargv));
	int argc = 0;

	DWORD dwText;
	ar >> dwText;
	ar.ReadString(lpString, 1024);
}

void CDPClient::OnDefinedText1(CAr& ar)
{
	DWORD dwText;
	ar >> dwText;

	CString strText = GETTEXT(dwText);

	DWORD dwColor = prj.GetTextColor(dwText);
	g_WndMng.PutString((LPCTSTR)strText, NULL, dwColor);
}

void CDPClient::OnGameTimer(CAr& ar)
{
	double dCurrentTime;
	ar >> dCurrentTime;

	g_GameTimer.SetCurrentTime(dCurrentTime);
}

void CDPClient::OnGameJoin(CAr& ar)
{
	ar >> g_WndMng.m_dwSavePlayerTime;
	CString strMessage;
	strMessage.Format(prj.GetText(TID_GAME_TOOLTIP_PRELEVEL), g_pPlayer->GetLevel());
	g_WndMng.PutString(strMessage, NULL, 0xffff0000);

	CString strTitle;
	strTitle.Format("%s - %s", g_pPlayer->GetName(), __NEUZ_TITLE);
	SetWindowText(CWndBase::m_hWnd, strTitle);

#ifdef __FL_FIX_PLAYERDATA // Instead, let's add it for active player only when he join the game
	PlayerData pd;
	pd.data.nJob = g_pPlayer->GetJob();
	pd.data.nLevel = g_pPlayer->GetLevel();
	pd.data.nSex = g_pPlayer->GetSex();
	lstrcpy(pd.szPlayer, g_pPlayer->GetName());
	CPlayerDataCenter::GetInstance()->AddPlayerData(g_pPlayer->m_idPlayer, pd);
#endif

	CMover* pMover = CMover::GetActiveMover();
	if (pMover)
	{
#ifdef __FL_FARM_DISPLAY
		prj.LoadFarmDisplay();
#endif

#ifdef __JOB_CHANGE
		if (pMover->GetLevel() == MAX_JOB_LEVEL && pMover->IsBaseJob())
		{
			if (!g_WndMng.GetApplet(APP_CHANGE_JOB_01))
				g_WndMng.CreateApplet(APP_CHANGE_JOB_01);
		}
		if (pMover->GetLevel() == (MAX_JOB_LEVEL + MAX_EXP_LEVEL) && pMover->IsExpert())
		{
			if (!g_WndMng.GetApplet(APP_CHANGE_JOB_02))
				g_WndMng.CreateApplet(APP_CHANGE_JOB_02);
		}
		if ((pMover->GetLevel() == MAX_GENERAL_LEVEL && pMover->IsPro() && pMover->GetExpPercent() >= 9999)
			|| (g_pPlayer->GetLevel() == MAX_GENERAL_LEVEL && g_pPlayer->IsMaster() && g_pPlayer->GetExpPercent() >= 9999)
			|| (g_pPlayer->GetLevel() == MAX_LEGEND_LEVEL && g_pPlayer->IsHero())
			)
		{
			if (!g_WndMng.GetApplet(APP_CHANGE_JOB_03))
				g_WndMng.CreateApplet(APP_CHANGE_JOB_03);
		}
#endif 

	}
}

void CDPClient::OnTaskBar(CAr& ar)
{
	g_WndMng.m_pWndTaskBar->Serialize(ar);
}

void CDPClient::OnErrorParty(CAr& ar)
{
	DWORD dw;
	ar >> dw;
	switch (dw) {
	case ERROR_NOLEADER:
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_PARTYNOINVATE), NULL, prj.GetTextColor(TID_GAME_PARTYNOINVATE));

	}
	break;
	case ERROR_FULLPARTY:
	{
		CString szMessageBuf;
		if (1 <= g_Party.GetLevel() && g_Party.GetLevel() <= 5)
		{

			szMessageBuf = prj.GetText(TID_GAME_FULLPARTY1);


		}
		else if (6 <= g_Party.GetLevel() && g_Party.GetLevel() <= 9)
		{

			szMessageBuf = prj.GetText(TID_GAME_FULLPARTY2);

		}
		else
		{
			szMessageBuf = prj.GetText(TID_GAME_FULLPARTY3);

		}

		g_WndMng.PutString(szMessageBuf, NULL, 0xff99cc00);
	}
	break;
	case ERROR_NOPARTY:
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_NOPARTY), NULL, prj.GetTextColor(TID_GAME_NOPARTY));

	}
	break;
	case ERROR_DIFFRENTPARTY:
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_DIFFRENTPARTY), NULL, prj.GetTextColor(TID_GAME_DIFFRENTPARTY));

	}
	break;
	case ERROR_DIFFERNTUSERNAME:
	{
		SAFE_DELETE(g_WndMng.m_pWndPartyChangeName);
		g_WndMng.m_pWndPartyChangeName = new CWndPartyChangeName;
		g_WndMng.m_pWndPartyChangeName->Initialize();
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0033)));

	}
	break;
	case ERROR_NOTPARTYPOINT:
	{
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0046)));

	}
	break;
	case ERROR_NOTPARTYSKILL:
	{
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0019)));

	}
	break;
	case ERROR_NOTTARGET:
	{
		DWORD dwSkill;
		ar >> dwSkill;

		g_WndMng.PutString(prj.GetText(TID_GAME_NOTTARGET), NULL, prj.GetTextColor(TID_GAME_NOTTARGET));

	}
	break;
	case ERROR_SAMLEADER:
		g_WndMng.PutString(prj.GetText(TID_GAME_PARTY_ALREADYMASTER), NULL, prj.GetTextColor(TID_GAME_PARTY_ALREADYMASTER));
		break;
	case ERROR_NOTMAMBER:
		g_WndMng.PutString(prj.GetText(TID_GAME_PARTYNOTEXMEMBER), NULL, prj.GetTextColor(TID_GAME_PARTYNOTEXMEMBER));
		break;
	default:
		break;
	}
}

void CDPClient::OnAddPartyMember(CAr& ar)
{
	u_long uPlayerId;
	int nSizeofMember;
	ar >> uPlayerId;

	char pszLeader[MAX_PLAYER] = { 0, };
	char pszMember[MAX_PLAYER] = { 0, };
	ar.ReadString(pszLeader, MAX_PLAYER);
	ar.ReadString(pszMember, MAX_PLAYER);

	ar >> nSizeofMember;
	int nOldSize = g_Party.GetSizeofMember();

	if (nSizeofMember != 0)
	{
		g_Party.Serialize(ar);

		CString sMessage;
		if (nOldSize == 0 && nSizeofMember == 2)	// new
		{
			if (g_Party.IsLeader(g_pPlayer->m_idPlayer))	// leader
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_PARTYCREATE), NULL, prj.GetTextColor(TID_GAME_PARTYCREATE));
				sMessage.Format(prj.GetText(TID_GAME_PARTYJOIN1), pszMember);
			}
			else	// member
			{
				sMessage.Format(prj.GetText(TID_GAME_PARTYJOIN2), pszLeader);
			}
			PlayMusic(BGM_IN_FITUP);
		}
		else if (nOldSize < nSizeofMember)		// add
		{
			sMessage.Format(prj.GetText(TID_GAME_PARTYJOIN1), pszMember);
		}
		else if (nOldSize > nSizeofMember)		// remove
		{
			sMessage.Format(prj.GetText(TID_GAME_PARTYREMOVE), pszMember);
		}
		g_WndMng.PutString(sMessage, NULL, 0xff99cc00);
	}
	else
	{
		g_Party.InitParty();
		g_pPlayer->m_idparty = 0;
		if (g_pPlayer->m_idPlayer == uPlayerId)
			g_WndMng.PutString(prj.GetText(TID_GAME_PARTYEXIT), NULL, prj.GetTextColor(TID_GAME_PARTYEXIT));
		else
			g_WndMng.PutString(prj.GetText(TID_GAME_PARTYDESTROY), NULL, prj.GetTextColor(TID_GAME_PARTYDESTROY));

		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.m_pWndWorld;
		pWndWorld->m_buffs.Clear();
	}

}

void CDPClient::OnPartyRequest(CAr& ar)
{
	u_long uLeader, uMember;

	BOOL bTroup;
	char szLeaderName[MAX_PLAYER] = { 0, };
	ar >> uLeader;
	ar >> uMember;
	ar.ReadString(szLeaderName, MAX_PLAYER);
	ar >> bTroup;

	if (g_Option.m_bParty == FALSE)
	{
		SendPartyMemberCancle(uLeader, uMember, 3);
		return;
	}

#ifdef __S_SERVER_UNIFY
	if (g_WndMng.m_bAllAction == FALSE)
		return;
#endif // 

	CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_PARTY_CONFIRM);

	if (pWndBaseBuf)
	{
		SendBlock(7, szLeaderName);
		return;
	}

	SAFE_DELETE(g_WndMng.m_pWndPartyConfirm);
	g_WndMng.m_pWndPartyConfirm = new CWndPartyConfirm;
	g_WndMng.m_pWndPartyConfirm->SetMember(uLeader, uMember, szLeaderName, bTroup);
	g_WndMng.m_pWndPartyConfirm->Initialize();
}

void CDPClient::OnPartyRequestCancel(CAr& ar)
{
	int nMode;


	u_long uLeader, uMember;

	ar >> uLeader >> uMember;
	ar >> nMode;

	CString sMessage;
	switch (nMode) {
	case 0:
	{
		CMover* pMember = prj.GetUserByID(uMember);
		if (IsValidObj((CObj*)pMember))
		{
			sMessage.Format(prj.GetText(TID_GAME_PARTYFAILINVATE), pMember->GetName());
		}
		break;
	}
	case 1:
	{
		CMover* pMember = prj.GetUserByID(uMember);
		if (IsValidObj((CObj*)pMember))
		{
			//sMessage = pMember->GetName();
			if (g_Party.FindMember(uMember) == -1)
			{

				sMessage.Format(prj.GetText(TID_GAME_PARTYOTHER), pMember->GetName());
			}
			else
			{

				sMessage.Format(prj.GetText(TID_GAME_PARTYEXISTCHR), pMember->GetName());
			}
		}
		break;
	}
	case 2:
	{
		sMessage = prj.GetText(TID_GAME_PARTYFAILENTER);
		break;
	}
	case 3:
	{
		CMover* pMember = prj.GetUserByID(uMember);
		if (IsValidObj((CObj*)pMember))
		{
			sMessage.Format(prj.GetText(TID_GAME_PARTYDENY), pMember->GetName());
		}
		break;
	}
	case 4:
	{
		sMessage = prj.GetText(TID_GAME_PARTYINVATEOTHERSVR);
		break;
	}
	default:
		break;
	}

	if (!sMessage.IsEmpty())
		g_WndMng.PutString(sMessage, NULL, 0xff99cc00);
}

void CDPClient::OnPartyExpLevel(CAr& ar)
{
	int nLevel;
	ar >> g_Party.m_nExp >> nLevel >> g_Party.m_nPoint;
	if (nLevel > g_Party.m_nLevel)
	{
		if (nLevel == MAX_PARTYLEVEL)
			g_WndMng.PutString(prj.GetText(TID_GAME_TROUPEUP), NULL, prj.GetTextColor(TID_GAME_TROUPEUP));
		else
			g_WndMng.PutString(prj.GetText(TID_GAME_PARTYLUP), NULL, prj.GetTextColor(TID_GAME_PARTYLUP));
	}
	g_Party.m_nLevel = nLevel;
}

void CDPClient::OnPartyChangeLeader(CAr& ar)
{
	u_long uidChangeLeader;
	ar >> uidChangeLeader;
	g_Party.ChangeLeader(uidChangeLeader);
}

void CDPClient::OnSetPartyMode(CAr& ar)
{
	/*
		#define ST_CALL                            1
		#define ST_BLITZ                           2
		#define ST_RETREAT                         3
		#define ST_SCOUT                           4
		#define ST_SPHERECIRCLE                    5
		#define ST_LINKATTACK                      6
		#define ST_FORTUNECIRCLE                   7
		#define ST_STRETCHING                      8
		#define ST_GIFTBOX                         9
	 */
	int nMode;
	BOOL bOnOff;
#if __VER >= 12
	DWORD	dwSkillTime;
	ar >> nMode >> dwSkillTime >> bOnOff;
#else
	ar >> nMode >> bOnOff;
#endif
	if (bOnOff == TRUE)
	{
		ar >> g_Party.m_nPoint;
	}
	g_Party.m_nModeTime[nMode] = bOnOff;
	CString csBufString;

	ItemProp* pItemProp = NULL;

	switch (nMode) {
	case PARTY_LINKATTACK_MODE:
	{
		pItemProp = prj.GetPartySkill(ST_LINKATTACK);

		if (bOnOff)
		{
			if (g_Party.IsLeader(g_pPlayer->m_idPlayer))
			{
				D3DXVECTOR3 v = g_pPlayer->GetPos();

				CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, NULL_ID, D3DXVECTOR3(0, 0, 0), g_pPlayer->GetId());

				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(0.8f, 0.8f, 0.8f));
			}
			else
			{
				u_long idPlayer = g_Party.GetPlayerId(0);
				CMover* pMover = prj.GetUserByID(idPlayer);

				if (!IsValidObj(pMover))
					break;

				D3DXVECTOR3 v = pMover->GetPos();

				CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, pMover->GetId());

				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(0.8f, 0.8f, 0.8f));

			}
		}

		csBufString = pItemProp->szName;
	}
	break;
	case PARTY_FORTUNECIRCLE_MODE:
	{
		pItemProp = prj.GetPartySkill(ST_FORTUNECIRCLE);

		if (bOnOff)
		{

			if (g_Party.IsLeader(g_pPlayer->m_idPlayer))
			{
				D3DXVECTOR3 v = g_pPlayer->GetPos();

				CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, NULL_ID, D3DXVECTOR3(0, 0, 0), g_pPlayer->GetId());

				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(0.7f, 0.7f, 0.7f));
			}
			else
			{
				u_long idPlayer = g_Party.GetPlayerId(0);
				CMover* pMover = prj.GetUserByID(idPlayer);

				if (!IsValidObj(pMover))
					break;

				D3DXVECTOR3 v = pMover->GetPos();

				CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, pMover->GetId());

				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(0.7f, 0.7f, 0.7f));
			}
		}

		csBufString = pItemProp->szName;
	}
	break;
	case PARTY_STRETCHING_MODE:
	{
		pItemProp = prj.GetPartySkill(ST_STRETCHING);


		for (int i = 0; i < g_Party.GetSizeofMember(); i++)
		{
			u_long idPlayer = g_Party.GetPlayerId(i);
			CMover* pPlayerMover = prj.GetUserByID(idPlayer);

			if (!IsValidObj(pPlayerMover))
				continue;

			D3DXVECTOR3 v = pPlayerMover->GetPos();

			CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, pPlayerMover->GetId());

			if (pSfx)
				pSfx->SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
		}

		csBufString = pItemProp->szName;
	}
	break;
	case PARTY_GIFTBOX_MODE:
	{
		pItemProp = prj.GetPartySkill(ST_GIFTBOX);

		if (bOnOff)
		{
			if (g_Party.IsLeader(g_pPlayer->m_idPlayer))
			{
				D3DXVECTOR3 v = g_pPlayer->GetPos();

				CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, g_pPlayer->GetId());

				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
			}
			else
			{
				u_long idPlayer = g_Party.GetPlayerId(0);
				CMover* pMover = prj.GetUserByID(idPlayer);

				if (!IsValidObj(pMover))
					break;

				D3DXVECTOR3 v = pMover->GetPos();

				CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, pMover->GetId());

				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
			}
		}

		csBufString = pItemProp->szName;
	}
	break;
	default:
		break;
	}

	CString str;

	if (pItemProp)
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.m_pWndWorld;
		if (bOnOff)
		{
			pWndWorld->m_buffs.AddBuff(BUFF_SKILL, (WORD)(pItemProp->dwID), 1, dwSkillTime);
			str.Format(prj.GetText(TID_GAME_TROUPESKILLUSE), pItemProp->szName);
			g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_TROUPESKILLUSE));
		}
		else
		{
			pWndWorld->m_buffs.RemoveBuff(BUFF_SKILL, (WORD)(pItemProp->dwID));
			str.Format(prj.GetText(TID_GAME_TROUPESKILLSTOP), pItemProp->szName);
			g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_TROUPESKILLSTOP));
		}
	}
}

void CDPClient::OnPartyChangeItemMode(CAr& ar)
{
	ar >> g_Party.m_nTroupeShareItem;
}

void CDPClient::OnPartyChangeExpMode(CAr& ar)
{
	ar >> g_Party.m_nTroupsShareExp;
}

void CDPClient::OnPartyChangeName(CAr& ar)
{
	BOOL	bNoName = FALSE;
	TCHAR	sParty[33];
	ar.ReadString(sParty, 33);

	if (0 != strlen(sParty))
	{
		if (0 != strcmp(sParty, prj.GetText(TID_GAME_PARTY_NAME)))
		{
			strcpy(g_Party.m_sParty, sParty);
			CString str;
			str.Format(prj.GetText(TID_GAME_PARTY), g_Party.m_sParty);
			g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_PARTY));
			if (g_Party.IsLeader(g_pPlayer->m_idPlayer))
			{
				strcpy(g_pPlayer->m_szPartyName, g_Party.m_sParty);
			}
		}
		else
		{
			bNoName = TRUE;
		}
	}
	else
	{
		bNoName = TRUE;
	}


	if (bNoName && g_Party.IsLeader(g_pPlayer->m_idPlayer))
	{
		SAFE_DELETE(g_WndMng.m_pWndPartyChangeName);
		g_WndMng.m_pWndPartyChangeName = new CWndPartyChangeName;
		g_WndMng.m_pWndPartyChangeName->Initialize();
	}
}

void CDPClient::OnPartyChangeTroup(CAr& ar)
{
	BOOL	bNoName = FALSE;
	TCHAR	sParty[33];
	ar.ReadString(sParty, 33);

	g_Party.m_nKindTroup = 1;
	g_WndMng.PutString(prj.GetText(TID_GAME_PARTYCHANGE), NULL, prj.GetTextColor(TID_GAME_PARTYCHANGE));

	PlayMusic(BGM_IN_FITUP);

	if (0 != strlen(sParty))
	{
		strcpy(g_Party.m_sParty, sParty);
	}
}

void CDPClient::OnSetPartyMemberParam(CAr& ar)
{
	u_long idMember;
	BYTE nParam;
	int nVal;
	ar >> idMember >> nParam >> nVal;
	switch (nParam)
	{
	case PP_REMOVE:
	{
		int i = g_Party.FindMember(idMember);
		if (i >= 0)
			g_Party.m_aMember[i].m_bRemove = (BOOL)nVal;

		if (nVal == 1 && i == 0)
		{
			bool fRemoveParty = true;
			for (int j = 1; j < g_Party.m_nSizeofMember; j++)
			{
				if (g_Party.m_aMember[j].m_bRemove == FALSE)
				{
					fRemoveParty = false;
					g_Party.SwapPartyMember(0, j);
					break;
				}
			}
			if (fRemoveParty)
				g_Party.InitParty();
		}
		break;
	}
	}
}

void CDPClient::OnPartySkillCall(OBJID objid, CAr& ar)
{
	D3DXVECTOR3	vLeader;
	ar >> vLeader;

	CMover* pMover = CMover::GetActiveMover();
	if (IsInvalidObj(pMover))	return;

	D3DXVECTOR3 v = pMover->GetPos();
	v.y += pMover->m_pModel->GetMaxHeight();

	ItemProp* pItemProp = prj.GetPartySkill(ST_CALL);

	if (g_Party.IsLeader(g_pPlayer->m_idPlayer))
	{
		/*
			#define ST_CALL                            1
			#define ST_BLITZ                           2
			#define ST_RETREAT                         3
			#define ST_SCOUT                           4
			#define ST_SPHERECIRCLE                    5
			#define ST_LINKATTACK                      6
			#define ST_FORTUNECIRCLE                   7
			#define ST_STRETCHING                      8
			#define ST_GIFTBOX                         9
		 */
		CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj2, v, objid);

		if (pSfx)
			pSfx->SetScale(D3DXVECTOR3(1.1f, 1.1f, 1.1f));
	}
	else
	{

		u_long idPlayer = g_Party.GetPlayerId(0);
		CMover* pLeaderMover = prj.GetUserByID(idPlayer);

		if (IsValidObj(pLeaderMover))
		{
			D3DXVECTOR3 v = pLeaderMover->GetPos();

			CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj2, v, pLeaderMover->GetId());

			if (pSfx)
				pSfx->SetScale(D3DXVECTOR3(1.1f, 1.1f, 1.1f));
		}


		CSfx* pSfx2 = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, objid);
		if (pSfx2)
		{
			pSfx2->SetScale(D3DXVECTOR3(1.1f, 1.1f, 1.1f));

			float xDist = vLeader.x - v.x;
			float zDist = vLeader.z - v.z;
			float fr = (float)atan2(xDist, zDist);
			float fa = D3DXToDegree(fr);
			//fa += 90.0f;

			pSfx2->SetAngle(fa);
		}
	}
}

void CDPClient::OnPartySkillBlitz(CAr& ar)
{
	OBJID idTarget;
	ar >> idTarget;

	CMover* pTarget = prj.GetMover(idTarget);
	if (IsInvalidObj(pTarget))	return;

	D3DXVECTOR3 v = pTarget->GetPos();
	v.y += pTarget->m_pModel->GetMaxHeight();

	OBJID tagetID = pTarget->GetId();

	ItemProp* pItemProp = prj.GetPartySkill(ST_BLITZ);

	CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, NULL_ID, D3DXVECTOR3(0, 0, 0), tagetID);

	if (pSfx)
		pSfx->SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
}


void CDPClient::OnPartySkillRetreat(OBJID objid)
{
	CMover* pMover = CMover::GetActiveMover();
	if (IsInvalidObj(pMover))	return;

	ItemProp* pItemProp = prj.GetPartySkill(ST_RETREAT);

	for (int i = 0; i < g_Party.GetSizeofMember(); i++)
	{
		u_long idPlayer = g_Party.GetPlayerId(i);
		CMover* pPlayerMover = prj.GetUserByID(idPlayer);

		if (!IsValidObj(pPlayerMover))
			continue;

		D3DXVECTOR3 v = pPlayerMover->GetPos();

		CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, pPlayerMover->GetId());

		if (pSfx)
			pSfx->SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	}
}


void CDPClient::OnPartySkillSphereCircle(OBJID objid)
{
	CMover* pMover = CMover::GetActiveMover();
	if (IsInvalidObj(pMover))	return;

	//	pMover->m_dwFlag |= MVRF_CRITICAL;

	CMover* pTarget = prj.GetMover(objid);

	if (IsInvalidObj(pTarget))	return;

	D3DXVECTOR3 v = pTarget->GetPos();

	ItemProp* pItemProp = prj.GetPartySkill(ST_SPHERECIRCLE);

	CMover* pLeader = g_Party.GetLeader();
	if (IsValidObj((CObj*)pLeader))
	{
		CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, pLeader->GetId(), D3DXVECTOR3(0, 0, 0));

		if (pSfx)
			pSfx->SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	}
}

void CDPClient::OnEnvironmentSetting(CAr& ar)
{
	int nSeason = SEASON_NONE;
	ar >> nSeason;
	CEnvironment::GetInstance()->SetSeason(nSeason);

	if (CEnvironment::GetInstance()->GetSeason() == SEASON_SPRING || CEnvironment::GetInstance()->GetSeason() == SEASON_FALL)
	{
		CFixedArray< tagMODELELEM >* apModelElem = &(prj.m_modelMng.m_aaModelElem[OT_OBJ]);
		for (int i = 0; i < apModelElem->GetSize(); ++i)
		{
			LPMODELELEM pModelElem = (LPMODELELEM)apModelElem->GetAt(i);
			if (pModelElem)
			{
				if (_tcsicmp(pModelElem->m_szName, "MaCoPrTr01") == 0 || _tcsicmp(pModelElem->m_szName, "MaCoPrTr03") == 0 ||
					_tcsicmp(pModelElem->m_szName, "MaCoPrTr10") == 0 || _tcsicmp(pModelElem->m_szName, "MaCoPrTr20") == 0 ||
					_tcsicmp(pModelElem->m_szName, "MaCoPrTr14") == 0 || _tcsicmp(pModelElem->m_szName, "MaCoPrTr11") == 0)
				{
					pModelElem->m_fScale = 1.0f;

					if (CEnvironment::GetInstance()->GetSeason() == SEASON_SPRING)
					{
						_tcscpy(pModelElem->m_szName, "MaCoPrTr16");
					}
					else if (CEnvironment::GetInstance()->GetSeason() == SEASON_FALL)
					{
						_tcscpy(pModelElem->m_szName, "MapleTree01");
					}
				}

				else if (_tcsicmp(pModelElem->m_szName, "MaCoPrTr04") == 0 || _tcsicmp(pModelElem->m_szName, "MaCoPrTr05") == 0 ||
					_tcsicmp(pModelElem->m_szName, "MaCoPrTr12") == 0 || _tcsicmp(pModelElem->m_szName, "MaCoPrTr15") == 0 ||
					_tcsicmp(pModelElem->m_szName, "MaCoPrTr13") == 0 || _tcsicmp(pModelElem->m_szName, "MaCoPrTr02") == 0)
				{
					pModelElem->m_fScale = 1.0f;

					if (CEnvironment::GetInstance()->GetSeason() == SEASON_SPRING)
					{
						_tcscpy(pModelElem->m_szName, "MaCoPrTr17");
					}
					else if (CEnvironment::GetInstance()->GetSeason() == SEASON_FALL)
					{
						_tcscpy(pModelElem->m_szName, "MapleTree02");
					}
				}
			}
		}
	}
}

void CDPClient::OnEnvironmentEffect(CAr& ar)
{
	BOOL bEnvEffectOld = CEnvironment::GetInstance()->GetEnvironmentEffect();
	CEnvironment::GetInstance()->Serialize(ar);
	BOOL bEnvEffectNew = CEnvironment::GetInstance()->GetEnvironmentEffect();

	char szEffectTitle[256] = { 0, };
	::memset(szEffectTitle, 0, sizeof(szEffectTitle));
	ar.ReadString(szEffectTitle, 255);
	if (_tcslen(szEffectTitle) > 0 && bEnvEffectOld != bEnvEffectNew)
	{
		char lpszTitle[512] = { 0, };
		if (bEnvEffectNew)
			_stprintf(lpszTitle, GETTEXT(TID_GAME_START_EVENT), szEffectTitle);
		else
			_stprintf(lpszTitle, GETTEXT(TID_GAME_END_EVENT), szEffectTitle);

		g_WndMng.PutString(lpszTitle, NULL, bEnvEffectNew ? 0xffccffcc : 0xffffff99);

	}

	if (g_pPlayer)
	{
		CWorld* pWorld = g_pPlayer->GetWorld();
		if (pWorld)
		{
#ifdef __DISCORD // Update when enter the game or change location
			g_Neuz.ManageDiscord(DIS_CHARACTER_DETAILS);
#endif
			if (pWorld->m_bIsIndoor && pWorld->GetID() != WI_DUNGEON_KRRR)
				return;

			if (pWorld->GetID() == WI_DUNGEON_KRRR)
				pWorld->m_bViewWeather = TRUE;
		}
	}


	int nWeather = WEATHER_NONE;
	switch (CEnvironment::GetInstance()->GetSeason())
	{
	case SEASON_SPRING:
		nWeather = WEATHER_SAKURA;
		break;
	case SEASON_SUMMER:
		nWeather = WEATHER_RAIN;
		break;
	case SEASON_FALL:
		nWeather = WEATHER_LEAF;
		break;
	case SEASON_WINTER:
		nWeather = WEATHER_SNOW;
		break;
	default:
		nWeather = WEATHER_NONE;
		break;
	}

	CWorld::m_skyBox.SetWeather(nWeather, CEnvironment::GetInstance()->GetEnvironmentEffect());

	if (CEnvironment::GetInstance()->GetEnvironmentEffect())
		g_Neuz.m_nRealWeather = nWeather;
	else
		g_Neuz.m_nRealWeather = WEATHER_NONE;
}

void CDPClient::OnPartyChat(CAr& ar)
{
	static	\
		CHAR	lpName[MAX_PLAYER], lpString[260];
	OBJID objid;
	ar >> objid;
	ar.ReadString(lpName, MAX_PLAYER);
	ar.ReadString(lpString, 260);

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	CMover* pMover = prj.GetMover(objid);
	if (pMover && pMover->IsPlayer() == TRUE)
	{
		set< CString >::iterator BlockedUserIterator = prj.m_setBlockedUserID.find(lpName);
		if (BlockedUserIterator != prj.m_setBlockedUserID.end())
			return;
	}
#endif // __YS_CHATTING_BLOCKING_SYSTEM

	CString str;
	str.Format("[%s] %s", lpName, lpString);
	g_WndMng.PutString(str, NULL, 0xff99cc00, CHATSTY_PARTY);

#ifndef __YS_CHATTING_BLOCKING_SYSTEM
	CMover* pMover = prj.GetMover(objid);
#endif // __YS_CHATTING_BLOCKING_SYSTEM
	if (IsValidObj(pMover))
	{
		if (strlen(lpString))
			g_DialogMsg.AddMessage(pMover, lpString, 0xfffffff, CHAT_PARTY);
	}
}

void CDPClient::OnMyPartyName(CAr& ar)
{
	ar.ReadString(g_pPlayer->m_szPartyName, 32);
}

void CDPClient::OnSMMode(OBJID objid, CAr& ar)
{
	int nType;
	DWORD dwTime;
	ar >> nType;
	ar >> dwTime;

	CMover* pMover = prj.GetMover(objid);
	if (!IsValidObj(pMover))
		return;

	if (nType < 0 || SM_MAX <= nType)
	{
		Error("CDPClient::OnSMMode %d", nType);
		return;
	}

	pMover->m_dwSMTime[nType] = dwTime;

	if (g_pPlayer && g_pPlayer == pMover)
	{
		if (nType == SM_BANK15 || nType == SM_BANK30 || nType == SM_BANK001)
		{
			CWndBase* pWndBase = g_WndMng.GetWndBase(APP_COMMON_BANK);
			if (pWndBase)
				((CWndBank*)pWndBase)->ReSetBank();
		}
	}
}

void CDPClient::OnSMModeAll(CAr& ar)
{
	short nCount;
	ar >> nCount;

	DWORD dwTime;
	int	nType;
	for (int i = 0; i < nCount; ++i)
	{
		ar >> nType;
		ar >> dwTime;

		if (nType < 0 || SM_MAX <= nType)
			Error("CDPClient::OnSMModeAll %d", nType);

		if (g_pPlayer == NULL)
			Error("CDPClient::OnSMModeAll Player NULL");

		g_pPlayer->m_dwSMTime[nType] = dwTime;
	}

	if (g_pPlayer && (0 < g_pPlayer->m_dwSMTime[SM_BANK15] || 0 < g_pPlayer->m_dwSMTime[SM_BANK30] || 0 < g_pPlayer->m_dwSMTime[SM_BANK001]))
	{
		CWndBase* pWndBase = g_WndMng.GetWndBase(APP_COMMON_BANK);
		if (pWndBase)
			((CWndBank*)pWndBase)->ReSetBank();
	}
}

void CDPClient::OnPartyMapInfo(CAr& ar)
{
	int nPartyIndex;
	D3DXVECTOR3 vPos;

	ar >> nPartyIndex;
	ar >> vPos;

	g_Party.SetPos(nPartyIndex, vPos);
}

void CDPClient::OnFlyffEvent(CAr& ar)
{
	g_eLocal.Serialize(ar);
}

#if __VER >= 9 // __EVENTLUA
void CDPClient::OnEventLuaDesc(CAr& ar)
{
	int nState;
	char lpString[200];
	char strDesc[100];

	ar >> nState;
	ar.ReadString(strDesc);

	switch (nState)
	{
	case 0:
		sprintf(lpString, GETTEXT(TID_GAME_END_EVENT), strDesc);
		break;
	case 1:
		sprintf(lpString, GETTEXT(TID_GAME_START_EVENT), strDesc);
		break;
	case 2:
		sprintf(lpString, GETTEXT(TID_GAME_ON_EVENT), strDesc);
		break;
	}

	g_WndMng.PutString(lpString, NULL, 0xffffff99);
}
#endif // __EVENTLUA

#if __VER >= 10 // __REMOVE_ATTRIBUTE
void CDPClient::OnRemoveAttributeResult(CAr& ar)
{
	BOOL bSuccess;
	ar >> bSuccess;

	CWndRemoveAttribute* pWndRemoveAttribute = (CWndRemoveAttribute*)g_WndMng.GetWndBase(APP_REMOVE_ATTRIBUTE);
	if (pWndRemoveAttribute != NULL)
		pWndRemoveAttribute->ReceiveResult(bSuccess);
}
#endif // __REMOVE_ATTRIBUTE
void CDPClient::OnGameRate(CAr& ar)
{
	BYTE nFlag;
	FLOAT fRate;
	ar >> fRate;
	ar >> nFlag;

	char chMessage[128] = { 0, };
	BOOL bMessage = FALSE;
	switch (nFlag)
	{
	case GAME_RATE_SHOPCOST:
	{
		prj.m_fShopCost = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set ShopCost (0.5 ~ 2.0) : %f", prj.m_fShopCost);
			bMessage = TRUE;
		}
	}
	break;
	case GAME_RATE_ITEMDROP:
	{
		prj.m_fItemDropRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set ItemDropRate (0.1 ~ 20.0) : %f", prj.m_fItemDropRate);
			bMessage = TRUE;
		}
	}
	break;
	case GAME_RATE_GOLDDROP:
	{
		prj.m_fGoldDropRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set GoldDropRate (0.1 ~ 5.0) : %f", prj.m_fGoldDropRate);
			bMessage = TRUE;
		}
	}
	break;
	case GAME_RATE_MONSTEREXP:
	{
		prj.m_fMonsterExpRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set MonsterExpRate (0.1 ~ 5.0) : %f", prj.m_fMonsterExpRate);
			bMessage = TRUE;
		}
	}
	break;
	case GAME_RATE_MONSTERHIT:
	{
		prj.m_fMonsterHitRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set MonsterHitRate (0.1 ~ 5.0) : %f", prj.m_fMonsterHitRate);
			bMessage = TRUE;
		}
	}
	break;
	case GAME_RATE_X3:
	{
		prj.m_fItemDropRate = fRate;
		prj.m_fGoldDropRate = fRate;
		prj.m_fMonsterExpRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, prj.GetText(TID_GAME_APPLYGAMEENI), (int)fRate);
			bMessage = TRUE;
		}
	}
	break;
#ifdef __S1108_BACK_END_SYSTEM
	case GAME_RATE_REBIRTH:
	{
		prj.m_fMonsterRebirthRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set Monster Rebirth Rate (0.1 ~ 5.0) : %f", prj.m_fMonsterRebirthRate);
			bMessage = TRUE;
		}
	}
	break;
	case GAME_RATE_HITPOINT:
	{
		prj.m_fMonsterHitpointRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set Monster Hitpoint Rate (0.1 ~ 5.0) : %f", prj.m_fMonsterHitpointRate);
			bMessage = TRUE;
		}
	}
	break;
	case GAME_RATE_AGGRESSIVE:
	{
		prj.m_fMonsterAggressiveRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set Monster Aggressive Rate (0.1 ~ 5.0) : %f", prj.m_fMonsterAggressiveRate);
			bMessage = TRUE;
		}
	}
	break;
	case GAME_RATE_RESPAWN:
	{
		prj.m_fMonsterRespawnRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set Monster Aggressive Rate (0.1 ~ 5.0) : %f", prj.m_fMonsterRespawnRate);
			bMessage = TRUE;
		}
	}
	break;
#endif // __S1108_BACK_END_SYSTEM
	case GAME_SKILL_VAGSP:
	{
		prj.m_dwVagSP = (DWORD)fRate;
	}
	break;
	case GAME_SKILL_EXPERTSP:
	{
		prj.m_dwExpertSP = (DWORD)fRate;
	}
	break;
	case GAME_SKILL_PROSP:
	{
		prj.m_dwProSP = (DWORD)fRate;
	}
	break;
#ifdef __SHOP_COST_RATE
	case GAME_RATE_SHOP_BUY:
	{
		prj.m_fShopBuyRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set Shop Buy Rate : %f", prj.m_fShopBuyRate);
			bMessage = TRUE;
		}
	}
	break;
	case GAME_RATE_SHOP_SELL:
	{
		prj.m_fShopSellRate = fRate;
		if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			sprintf(chMessage, "Set Shop Sell Rate : %f", prj.m_fShopSellRate);
			bMessage = TRUE;
		}
	}
	break;
#endif // __SHOP_COST_RATE
	default:
		break;
	}

	if (bMessage)
	{
		g_WndMng.PutString(chMessage, NULL, 0xffff0000, CHATSTY_GENERAL);
	}
}

#ifdef __S1108_BACK_END_SYSTEM
void CDPClient::OnMonsterProp(CAr& ar)
{
	char	szMonsterName[32];
	int		nHitPoint;
	int		nAttackPower;
	int		nDefence;
	int		nExp;
	int		nItemDrop;
	int		nPenya;

	int		nMonsterPropSize;

	prj.m_nAddMonsterPropSize = 0;
	prj.m_nRemoveMonsterPropSize = 0;

	ar >> nMonsterPropSize;
	for (int i = 0; i < nMonsterPropSize; ++i)
	{
		ar.ReadString(szMonsterName, 32);
		ar >> nHitPoint;
		ar >> nAttackPower;
		ar >> nDefence;
		ar >> nExp;
		ar >> nItemDrop;
		ar >> nPenya;

		MoverProp* pMoverProp = NULL;
		pMoverProp = prj.GetMoverProp(szMonsterName);
		if (pMoverProp)
		{
			pMoverProp->m_fHitPoint_Rate = (float)nHitPoint / 100.0f;
			pMoverProp->m_fAttackPower_Rate = (float)nAttackPower / 100.0f;
			pMoverProp->m_fDefence_Rate = (float)nDefence / 100.0f;
			pMoverProp->m_fExp_Rate = (float)nExp / 100.0f;
			pMoverProp->m_fItemDrop_Rate = (float)nItemDrop / 100.0f;
			pMoverProp->m_fPenya_Rate = (float)nPenya / 100.0f;
			pMoverProp->m_bRate = TRUE;

			MONSTER_PROP MonsterProp;
			strcpy(MonsterProp.szMonsterName, szMonsterName);
			MonsterProp.nHitPoint = nHitPoint;
			MonsterProp.nAttackPower = nAttackPower;
			MonsterProp.nDefence = nDefence;
			MonsterProp.nExp = nExp;
			MonsterProp.nItemDrop = nItemDrop;
			MonsterProp.nPenya = nPenya;
			prj.AddMonsterProp(MonsterProp);
		}
	}

	int	nRemovePropSize;
	ar >> nRemovePropSize;
	for (int i = 0; i < nRemovePropSize; ++i)
	{
		ar.ReadString(szMonsterName, 32);
		MoverProp* pMoverProp = NULL;
		pMoverProp = prj.GetMoverProp(szMonsterName);
		if (pMoverProp)
		{
			pMoverProp->m_fHitPoint_Rate = 1.0f;
			pMoverProp->m_fAttackPower_Rate = 1.0f;
			pMoverProp->m_fDefence_Rate = 1.0f;
			pMoverProp->m_fExp_Rate = 1.0f;
			pMoverProp->m_fItemDrop_Rate = 1.0f;
			pMoverProp->m_fPenya_Rate = 1.0f;
			pMoverProp->m_bRate = FALSE;
			prj.RemoveMonsterProp(szMonsterName);
		}
	}
}

void CDPClient::OnGMChat(CAr& ar)
{
	int nSize;
	char chMessage[256];
	CString str;
	ar >> nSize;
	for (int i = 0; i < nSize; ++i)
	{
		ar.ReadString(chMessage, 256);
		str.Format(prj.GetText(TID_ADMIN_ANNOUNCE), chMessage);
		g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_ADMIN_ANNOUNCE), CHATSTY_SYSTEM);
	}
}
#endif // __S1108_BACK_END_SYSTEM

void CDPClient::OnChangeFace(CAr& ar)
{
	u_long uidPlayer;
	DWORD dwFace;

	ar >> uidPlayer >> dwFace;

	CMover* pMover = prj.GetUserByID(uidPlayer);
	if (IsValidObj(pMover))
	{
		pMover->m_dwHeadMesh = dwFace;
		if (g_pPlayer == pMover)
		{
			CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_aEquipInfo, (CModelObject*)g_pPlayer->m_pModel, &g_pPlayer->m_Inventory);
			g_WndMng.UpdateWindowModels();
		}
		else
			CMover::UpdateParts(pMover->GetSex(), pMover->m_dwHairMesh, pMover->m_dwHeadMesh, pMover->m_aEquipInfo, (CModelObject*)pMover->m_pModel, NULL);
	}
}

void CDPClient::OnQuestTextTime(CAr& ar)
{
	BOOL  bFlag;
	int   nState;
	DWORD dwTime;

	ar >> bFlag;
	ar >> nState;
	ar >> dwTime;

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	if (pWndWorld)
	{
		pWndWorld->m_QuestTime.bFlag = bFlag;
		pWndWorld->m_QuestTime.nState = nState;
		pWndWorld->m_QuestTime.dwQuestTime = GetTickCount() + dwTime;
	}
}

void CDPClient::OnGuildCombat(CAr& ar)
{
	BYTE nState;
	ar >> nState;
	switch (nState)
	{
	case GC_IN_WINDOW:
		OnGCInWindow(ar);
		break;
	case GC_SELECTPLAYER:
		OnGCSelectPlayer(ar);
		break;
	case GC_REQUEST_STATUS:
		OnGCRequestStatus(ar);
		break;
	case GC_USERSTATE:
		OnGCUserState(ar);
		break;
	case GC_GUILDSTATUS:
		OnGCGuildStatus(ar);
		break;
	case GC_GUILDPRECEDENCE:
		OnGCGuildPrecedence(ar);
		break;
	case GC_PLAYERPRECEDENCE:
		OnGCPlayerPrecedence(ar);
		break;
	case GC_SELECTWARPOS:
		OnGCJoinWarWindow(ar);
		break;
	case GC_GCSTATE:
		OnGuildCombatState(ar);
		break;
	case GC_NEXTTIMESTATE:
		OnGuildCombatNextTimeState(ar);
		break;
	case GC_ENTERTIME:
		OnGuildCombatEnterTime(ar);
		break;
	case GC_WINGUILD:
		OnGCWinGuild(ar);
		break;
	case GC_BESTPLAYER:
		OnGCBestPlayer(ar);
		break;
	case GC_DIAGMESSAGE:
		OnGCDiagMessage(ar);
		break;
	case GC_TELE:
		OnGCTele(ar);
		break;
	case GC_WARPLAYERLIST:
		OnGCWarPlayerList(ar);
		break;
	case GC_ISREQUEST:
		OnIsRequest(ar);
		break;
	case GC_LOG:
		OnGCLog(ar);
		break;
	case GC_LOG_REALTIME:
		OnGCLogRealTime(ar);
		break;
	case GC_PLAYERPOINT:
		OnGCPlayerPoint(ar);
		break;
	}
}
void CDPClient::OnGCInWindow(CAr& ar)
{
	__int64 nPrizePenya;
	DWORD dwRequestPenya;
	DWORD dwMinPenya;
	ar >> nPrizePenya >> dwRequestPenya >> dwMinPenya;

	if (g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE)
	{
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_REQUESTEND));
		return;
	}

	SAFE_DELETE(g_WndMng.m_pWndGuildCombatOffer);
	g_WndMng.m_pWndGuildCombatOffer = new CWndGuildCombatOffer;

	if (g_WndMng.m_pWndGuildCombatOffer)
	{
		g_WndMng.m_pWndGuildCombatOffer->Initialize(NULL);

		g_WndMng.m_pWndGuildCombatOffer->SetMinGold(dwMinPenya);

		if (dwRequestPenya == 0)
		{
			g_WndMng.m_pWndGuildCombatOffer->SetReqGold(dwRequestPenya);
			g_WndMng.m_pWndGuildCombatOffer->SetBackupGold(dwMinPenya);
			g_WndMng.m_pWndGuildCombatOffer->SetGold(dwMinPenya);
		}
		else
		{
			g_WndMng.m_pWndGuildCombatOffer->SetReqGold(dwRequestPenya);
			g_WndMng.m_pWndGuildCombatOffer->SetBackupGold(dwRequestPenya);
			g_WndMng.m_pWndGuildCombatOffer->SetGold(dwRequestPenya);
		}

		g_WndMng.m_pWndGuildCombatOffer->EnableAccept(TRUE);
	}
}

void CDPClient::OnGCRequestStatus(CAr& ar)
{
	__int64 nPrizePenya;
	DWORD dwRequstPenya;
	int nSize;
	char strGuildName[128];
	vector<CString> vecRequestGuild;
	time_t tNext;
	ar >> tNext;
	ar >> nPrizePenya;
	ar >> nSize;

	SAFE_DELETE(g_WndMng.m_pWndGuildWarState);
	g_WndMng.m_pWndGuildWarState = new CWndGuildCombatState;

	if (g_WndMng.m_pWndGuildWarState)
	{
		g_WndMng.m_pWndGuildWarState->Initialize();
		g_WndMng.m_pWndGuildWarState->SetTime(tNext);

		int nRate = -1;
		CGuild* pGuild = g_pPlayer->GetGuild();

		for (int i = 0; i < nSize; ++i)
		{
			ar.ReadString(strGuildName, 128);
			vecRequestGuild.push_back(strGuildName);

			if (pGuild && nRate == -1)
			{
				if (stricmp(pGuild->m_szGuild, strGuildName) == 0)
				{
					nRate = i + 1;
				}
			}

			g_WndMng.m_pWndGuildWarState->InsertGuild(strGuildName, "1", i + 1);
		}

		g_WndMng.m_pWndGuildWarState->SetRate(nRate);
	}

	ar >> dwRequstPenya;

	if (g_WndMng.m_pWndGuildWarState)
	{
		g_WndMng.m_pWndGuildWarState->SetGold(dwRequstPenya);
		g_WndMng.m_pWndGuildWarState->SetTime(tNext);
	}
}

void CDPClient::OnGCSelectPlayer(CAr& ar)
{
	BOOL bRequestWar, bWindow;
	u_long uidDefender, uidPlayer;
	int nSize;
	int nMaxJoinMember;
	int nMaxWarMember;
	vector<u_long> vecSelectPlayer;
	ar >> nMaxJoinMember;
	ar >> nMaxWarMember;

	ar >> bRequestWar;
	if (bRequestWar)
	{
		ar >> bWindow;

		if (bWindow)
		{
			ar >> uidDefender;
			ar >> nSize;

			SAFE_DELETE(g_WndMng.m_pWndGuildCombatSelection);
			g_WndMng.m_pWndGuildCombatSelection = new CWndGuildCombatSelection;
			if (g_WndMng.m_pWndGuildCombatSelection)
			{
				g_WndMng.m_pWndGuildCombatSelection->Initialize(NULL);
				g_WndMng.m_pWndGuildCombatSelection->SetMemberSize(nMaxJoinMember, nMaxWarMember);
			}

			for (int i = 0; i < nSize; ++i)
			{
				ar >> uidPlayer;
				vecSelectPlayer.push_back(uidPlayer);
				if (g_WndMng.m_pWndGuildCombatSelection)
				{
					g_WndMng.m_pWndGuildCombatSelection->AddCombatPlayer(uidPlayer);
				}
			}

			if (g_WndMng.m_pWndGuildCombatSelection)
			{
				g_WndMng.m_pWndGuildCombatSelection->SetDefender(uidDefender);
			}
		}
		else
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_GUILDCOMBAT_PLAYER_MAKEUP));
		}
	}
	else
	{

	}
}
void CDPClient::OnGCUserState(CAr& ar)
{
	int		nGuildCombatState;
	OBJID	idPlayer;
	ar >> idPlayer;
	ar >> nGuildCombatState;
	CMover* pMover = prj.GetMover(idPlayer);
	if (IsValidObj(pMover))
	{
		pMover->m_nGuildCombatState = nGuildCombatState;
	}
}

void CDPClient::OnGCGuildStatus(CAr& ar)
{
	int nSize;
	u_long uidPlayer;
	int nLife, nJoinCount;
	ar >> nJoinCount;
	ar >> nSize;

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	if (pWndWorld)
		pWndWorld->ClearGuildStatus();

	for (int i = 0; i < nSize; ++i)
	{
		ar >> uidPlayer;
		ar >> nLife;

		if (pWndWorld)
		{
			if (nJoinCount == i && nJoinCount != 0)
				pWndWorld->AddGuildStatus(uidPlayer, nLife, TRUE);
			else
				pWndWorld->AddGuildStatus(uidPlayer, nLife, FALSE);
		}
	}




}

void CDPClient::OnGCGuildPrecedence(CAr& ar)
{
	int nSize;
	char strGuildName[128];
	int nGuildPoint;

	ar >> nSize;

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	if (pWndWorld)
		pWndWorld->ClearGuildPrecedence();

	for (int i = 0; i < nSize; ++i)
	{
		BOOL bRecive;
		ar >> bRecive;
		if (bRecive)
		{
			ar.ReadString(strGuildName, 128);
			ar >> nGuildPoint;
			if (pWndWorld)
			{
				pWndWorld->AddGuildPrecedence(nGuildPoint, strGuildName);
			}
		}
	}




}

void CDPClient::OnGCPlayerPrecedence(CAr& ar)
{
	int nGuildSize, nPlayerSize;
	u_long uidPlayer;
	int nPoint;

	ar >> nGuildSize;
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	if (pWndWorld)
		pWndWorld->ClearPlayerPrecedence();

	for (int i = 0; i < nGuildSize; ++i)
	{
		ar >> nPlayerSize;
		for (int j = 0; j < nPlayerSize; ++j)
		{
			ar >> uidPlayer;
			ar >> nPoint;
			if (pWndWorld)
			{
				pWndWorld->AddPlayerPrecedence(nPoint, uidPlayer);
			}
		}
	}

}

void CDPClient::OnGCJoinWarWindow(CAr& ar)
{
	int nJoinWarTime;
	ar >> nJoinWarTime;

	SAFE_DELETE(g_WndMng.m_pWndGuildCombatJoinSelection);
	g_WndMng.m_pWndGuildCombatJoinSelection = new CWndGuildCombatJoinSelection;

	if (g_WndMng.m_pWndGuildCombatJoinSelection)
	{
		g_WndMng.m_pWndGuildCombatJoinSelection->Initialize();
		g_WndMng.m_pWndGuildCombatJoinSelection->SetTimeOut(nJoinWarTime * 1000);
	}
}
void CDPClient::OnGCDiagMessage(CAr& ar)
{
	char strMessage[512] = { 0, };
	ar.ReadString(strMessage, 512);

	SAFE_DELETE(g_WndMng.m_pWndGuildCombatInfoMessageBox2);
	g_WndMng.m_pWndGuildCombatInfoMessageBox2 = new CGuildCombatInfoMessageBox2;

	if (g_WndMng.m_pWndGuildCombatInfoMessageBox2)
	{
		g_WndMng.m_pWndGuildCombatInfoMessageBox2->Initialize(&g_WndMng);
		g_WndMng.m_pWndGuildCombatInfoMessageBox2->SetString(strMessage);
	}
}
void CDPClient::OnGCTele(CAr& ar)
{
	char strMessage[512] = { 0, };
	ar.ReadString(strMessage, 512);

	SAFE_DELETE(g_WndMng.m_pWndGuildCombatInfoMessageBox);
	g_WndMng.m_pWndGuildCombatInfoMessageBox = new CGuildCombatInfoMessageBox;

	if (g_WndMng.m_pWndGuildCombatInfoMessageBox)
	{
		g_WndMng.m_pWndGuildCombatInfoMessageBox->Initialize(&g_WndMng);
		g_WndMng.m_pWndGuildCombatInfoMessageBox->SetString(strMessage);
	}
}
void CDPClient::OnGCWarPlayerList(CAr& ar)
{
	int nSizeGuild, nSizeMember, nStatus;
	u_long uidDefender, uidPlayer;

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	if (pWndWorld)
	{
		pWndWorld->m_mapGC_GuildStatus.clear();

		ar >> nSizeGuild;

		for (int i = 0; i < nSizeGuild; ++i)
		{
			ar >> uidDefender;
			ar >> nSizeMember;

			for (int j = 0; j < nSizeMember; ++j)
			{
				ar >> uidPlayer;
				ar >> nStatus;

				pWndWorld->AddGCStatus(uidDefender, uidPlayer, nStatus);
			}
		}
	}
}
void CDPClient::OnIsRequest(CAr& ar)
{
	BOOL bRequest;
	ar >> bRequest;
	g_GuildCombatMng.m_bRequest = bRequest;
}
void CDPClient::OnGCLog(CAr& ar)
{
	g_GuildCombatMng.m_vecGCGetPoint.clear();
	u_long uSize;
	ar >> uSize;

	//#ifdef __INTERNALSERVER
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	SAFE_DELETE(g_WndMng.n_pWndGuildCombatResult);
	g_WndMng.n_pWndGuildCombatResult = new CWndGuildCombatResult;

	if (pWndWorld && g_WndMng.n_pWndGuildCombatResult)
	{
		g_WndMng.n_pWndGuildCombatResult->Initialize();

		CGuild* pPlayerGuild = g_pPlayer->GetGuild();


		multimap< int, CString > mmapGuildRate = pWndWorld->m_mmapGuildCombat_GuildPrecedence;

		int nRate = 0;
		int nPoint;
		CString str, strTemp;
		int nOldPoint = 0xffffffff;

		for (multimap<int, CString>::reverse_iterator i = mmapGuildRate.rbegin(); i != mmapGuildRate.rend(); ++i)
		{
			nPoint = i->first;
			str = i->second;
			g_WndMng.StringSize(str, 8);

			if (nOldPoint != nPoint)
				nRate++;

			if (nOldPoint != nPoint)
			{
				if (nRate == 1)
					strTemp.Format("%2d   %s\t (%d) WINNER", nRate, str, nPoint);
				else
					strTemp.Format("%2d   %s\t (%d)", nRate, str, nPoint);
			}
			else
				strTemp.Format("%s   %s\t (%d)", "  ", str, nPoint);

			g_WndMng.n_pWndGuildCombatResult->InsertGuildRate(strTemp);
			nOldPoint = nPoint;
		}


		nRate = 0;
		nOldPoint = 0xffffffff;
		u_long uiPlayer;

		multimap<int, u_long> mmapPersonRate = pWndWorld->m_mmapGuildCombat_PlayerPrecedence;
		for (multimap<int, u_long>::reverse_iterator j = mmapPersonRate.rbegin(); j != mmapPersonRate.rend(); ++j)
		{
			nPoint = j->first;
			uiPlayer = j->second;

			if (nOldPoint != nPoint)
				nRate++;

			str = CPlayerDataCenter::GetInstance()->GetPlayerString(uiPlayer);
			g_WndMng.StringSize(str, 10);

			if (nOldPoint != nPoint)
			{
				if (uiPlayer == g_GuildCombatMng.m_uBestPlayer)
					strTemp.Format("%2d   %s\t (%d) MVP", nRate, str, nPoint);
				else
					strTemp.Format("%2d   %s\t (%d)", nRate, str, nPoint);
			}
			else
				strTemp.Format("%s   %s\t (%d)", "  ", str, nPoint);

			g_WndMng.n_pWndGuildCombatResult->InsertPersonRate(strTemp);
			nOldPoint = nPoint;
		}

		CString strAtk, strDef;
		CString strGuild1, strGuild2;
		CString strTemp2;

		for (int k = 0; k < (int)(uSize); ++k)
		{
			CGuildCombat::__GCGETPOINT GCGetPoint;
			ar >> GCGetPoint.uidGuildAttack;
			ar >> GCGetPoint.uidGuildDefence;
			ar >> GCGetPoint.uidPlayerAttack;
			ar >> GCGetPoint.uidPlayerDefence;
			ar >> GCGetPoint.nPoint;
			ar >> GCGetPoint.bKillDiffernceGuild;
			ar >> GCGetPoint.bMaster;
			ar >> GCGetPoint.bDefender;
			ar >> GCGetPoint.bLastLife;
			g_GuildCombatMng.m_vecGCGetPoint.push_back(GCGetPoint);


			CGuild* pGuildAtk = g_GuildMng.GetGuild(GCGetPoint.uidGuildAttack);
			CGuild* pGuildDef = g_GuildMng.GetGuild(GCGetPoint.uidGuildDefence);

			BOOL bAtkMaster = pGuildAtk->IsMaster(GCGetPoint.uidPlayerAttack);
			BOOL bDefMaster = pGuildDef->IsMaster(GCGetPoint.uidPlayerDefence);

			if (bAtkMaster && bDefMaster)
			{
				strGuild1 = prj.GetText(TID_GAME_GC_LOG_MASTER);
				strGuild2 = strGuild1;
			}
			else if (!bAtkMaster && bDefMaster)
			{
				strGuild1 = "";
				strGuild2 = prj.GetText(TID_GAME_GC_LOG_MASTER);
			}
			else if (bAtkMaster && !bDefMaster)
			{
				strGuild1 = prj.GetText(TID_GAME_GC_LOG_MASTER);
				strGuild2 = "";
			}
			else if (!bAtkMaster && !bDefMaster)
			{
				strGuild1 = "";
				strGuild2 = "";
			}


			strAtk = CPlayerDataCenter::GetInstance()->GetPlayerString(GCGetPoint.uidPlayerAttack);
			strDef = CPlayerDataCenter::GetInstance()->GetPlayerString(GCGetPoint.uidPlayerDefence);

			if (GCGetPoint.bDefender)
			{
				strGuild2 = prj.GetText(TID_GAME_GC_LOG_DEFENDER);
			}

			CString szTempGuild;
			szTempGuild.Format(prj.GetText(TID_GAME_GC_LOG_GUILD), pGuildAtk->m_szGuild);
			CString szTempPoint;
			szTempPoint.Format(prj.GetText(TID_GAME_GC_LOG_POINT), GCGetPoint.nPoint);
			CString szTempGuildDef;
			szTempGuildDef.Format(prj.GetText(TID_GAME_GC_LOG_GUILD), pGuildDef->m_szGuild);
			CString szTempAttack;
			szTempAttack = prj.GetText(TID_GAME_ATTACK);

			if (GCGetPoint.uidPlayerAttack == g_pPlayer->m_idPlayer)
			{
				strTemp.Format("#cff009c00%s %s %s%s#nc %s → %s %s %s",
					szTempGuild, strGuild1, strAtk, szTempPoint, szTempAttack, szTempGuildDef, strGuild2, strDef);
			}
			else
			{
				if (pPlayerGuild && pPlayerGuild->GetGuildId() == GCGetPoint.uidGuildAttack)
				{
					strTemp.Format("#cff009c00%s %s %s%s#nc %s → %s %s %s",
						szTempGuild, strGuild1, strAtk, szTempPoint, szTempAttack, szTempGuildDef, strGuild2, strDef);
				}
				else
					if (pPlayerGuild && pPlayerGuild->GetGuildId() == GCGetPoint.uidGuildDefence)
					{
						strTemp.Format("%s %s %s%s %s → #cff9c0000%s %s %s#nc",
							szTempGuild, strGuild1, strAtk, szTempPoint, szTempAttack, szTempGuildDef, strGuild2, strDef);
					}
					else
					{
						strTemp.Format("%s %s %s%s %s → %s %s %s",
							szTempGuild, strGuild1, strAtk, szTempPoint, szTempAttack, szTempGuildDef, strGuild2, strDef);
					}
			}
			strTemp += "\n";

			g_WndMng.n_pWndGuildCombatResult->InsertLog(strTemp);

			strTemp.Empty();
			strTemp += prj.GetText(TID_GAME_GC_LOG1);

			if (GCGetPoint.bMaster)
			{
				strTemp += ", ";
				strTemp += prj.GetText(TID_GAME_GC_LOG2);
			}
			if (GCGetPoint.bDefender)
			{
				strTemp += ", ";
				strTemp += prj.GetText(TID_GAME_GC_LOG3);
			}
			if (GCGetPoint.bKillDiffernceGuild)
			{
				strTemp += ", ";
				strTemp += prj.GetText(TID_GAME_GC_LOG4);
			}
			if (GCGetPoint.bLastLife)
			{
				strTemp += ", ";
				strTemp += prj.GetText(TID_GAME_GC_LOG5);
			}

			strTemp2.Format("< %s >", strTemp);
			strTemp2 += "\n";

			g_WndMng.n_pWndGuildCombatResult->InsertLog(strTemp2);
			g_WndMng.n_pWndGuildCombatResult->InsertLog("\r\n");
		}
	}
	//#endif //__INTERNALSERVER
}
void CDPClient::OnGCLogRealTime(CAr& ar)
{
	char szAttacker[MAX_NAME], szDefender[MAX_NAME];
	CGuildCombat::__GCGETPOINT GCGetPoint;
	ar >> GCGetPoint.uidGuildAttack;
	ar >> GCGetPoint.uidGuildDefence;
	ar.ReadString(szAttacker, MAX_NAME);
	ar.ReadString(szDefender, MAX_NAME);
	ar >> GCGetPoint.uidPlayerAttack;
	ar >> GCGetPoint.uidPlayerDefence;
	ar >> GCGetPoint.nPoint;
	ar >> GCGetPoint.bKillDiffernceGuild;
	ar >> GCGetPoint.bMaster;
	ar >> GCGetPoint.bDefender;
	ar >> GCGetPoint.bLastLife;

	CString strAtk, strDef;
	CString strGuild1, strGuild2;
	CString strTemp2;

	CGuild* pGuildAtk = g_GuildMng.GetGuild(GCGetPoint.uidGuildAttack);
	CGuild* pGuildDef = g_GuildMng.GetGuild(GCGetPoint.uidGuildDefence);

	BOOL bAtkMaster = pGuildAtk->IsMaster(GCGetPoint.uidPlayerAttack);
	BOOL bDefMaster = pGuildDef->IsMaster(GCGetPoint.uidPlayerDefence);

	if (bAtkMaster && bDefMaster)
	{
		strGuild1 = prj.GetText(TID_GAME_GC_LOG_MASTER);
		strGuild2 = strGuild1;
	}
	else if (!bAtkMaster && bDefMaster)
	{
		strGuild1 = "";
		strGuild2 = prj.GetText(TID_GAME_GC_LOG_MASTER);
	}
	else if (bAtkMaster && !bDefMaster)
	{
		strGuild1 = prj.GetText(TID_GAME_GC_LOG_MASTER);
		strGuild2 = "";
	}
	else if (!bAtkMaster && !bDefMaster)
	{
		strGuild1 = "";
		strGuild2 = "";
	}

	strAtk.Format(prj.GetText(TID_GAME_GC_FROM3), szAttacker);
	strDef.Format(prj.GetText(TID_GAME_GC_FROM4), szDefender);

	if (GCGetPoint.bDefender)
	{
		strGuild2 = prj.GetText(TID_GAME_GC_LOG_DEFENDER);
	}

	CString szTempGuild;
	szTempGuild.Format(prj.GetText(TID_GAME_GC_LOG_GUILD), pGuildAtk->m_szGuild);
	CString szTempPoint;
	szTempPoint.Format(prj.GetText(TID_GAME_GC_ATTACK_POINT), GCGetPoint.nPoint);
	CString szTempGuildDef;
	szTempGuildDef.Format(prj.GetText(TID_GAME_GC_LOG_GUILD), pGuildDef->m_szGuild);
	CString szTempAttack;
	szTempAttack = prj.GetText(TID_GAME_ATTACK);

	sprintf(szChat, "[%s][%s] has killed [%s][%s] and has gained %d points.", pGuildAtk->m_szGuild, szAttacker, pGuildDef->m_szGuild, szDefender, GCGetPoint.nPoint);
	g_WndMng.PutString((LPCTSTR)szChat, NULL, 0xffffffff, CHATSTY_GENERAL);
	g_WndMng.PutString((LPCTSTR)szChat);
}

void CDPClient::OnGCPlayerPoint(CAr& ar)
{
	g_GuildCombatMng.m_vecGCPlayerPoint.clear();
	u_long uSize;
	ar >> uSize;

	SAFE_DELETE(g_WndMng.m_pWndGuildCombatRanking);
	g_WndMng.m_pWndGuildCombatRanking = new CWndGuildCombatRank_Person;

	if (g_WndMng.m_pWndGuildCombatRanking)
	{
		g_WndMng.m_pWndGuildCombatRanking->Initialize();

		CString str;
		CString strFormat;
		vector<PDVer>	vecPlayer;
		vecPlayer.clear();

		for (int i = 0; i < (int)(uSize); ++i)
		{
			CGuildCombat::__GCPLAYERPOINT GCPlayerPoint;
			ar >> GCPlayerPoint.uidPlayer;
			ar >> GCPlayerPoint.nJob;
			ar >> GCPlayerPoint.nPoint;
			g_GuildCombatMng.m_vecGCPlayerPoint.push_back(GCPlayerPoint);
			PDVer pdv(GCPlayerPoint.uidPlayer, CPlayerDataCenter::GetInstance()->GetPlayerData(GCPlayerPoint.uidPlayer, FALSE)->data.nVer);
			vecPlayer.push_back(pdv);

			g_WndMng.m_pWndGuildCombatRanking->InsertRank(GCPlayerPoint.nJob, GCPlayerPoint.uidPlayer, GCPlayerPoint.nPoint);
		}

		CWndGuildCombatRank_Person* pWndRank = (CWndGuildCombatRank_Person*)g_WndMng.GetWndBase(APP_GUILDCOMBAT_RANK_P);
		if (pWndRank)
			pWndRank->DivisionList();

		SendQueryPlayerData(vecPlayer);

	}
}
void CDPClient::OnGuildCombatState(CAr& ar)
{
	int		nState;
	OBJID	nWarState;
	ar >> nState >> nWarState;
	g_GuildCombatMng.m_nState = nState;
	g_GuildCombatMng.m_nGCState = nWarState;

	if (!g_pPlayer)
		return;

	if (g_GuildCombatMng.m_nGCState == CGuildCombat::NOTENTER_COUNT_STATE)
	{
		CGuild* pGuild = g_pPlayer->GetGuild();
		if (pGuild && g_GuildCombatMng.IsRequest())
		{
			if (pGuild->IsMaster(g_pPlayer->m_idPlayer))
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_GUILDCOMBAT_NOW_PLAYER_MAKEUP));
			}
		}
	}


	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	if (pWndWorld)
	{
		if (g_GuildCombatMng.m_nState == CGuildCombat::WAR_STATE &&
			g_GuildCombatMng.m_nGCState == CGuildCombat::MAINTENANCE_STATE)
		{
			CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
			g_Neuz.m_2DRender.SetFont(CWndBase::m_Theme.m_pFontText);
			g_WndMng.PutString(prj.GetText(TID_GAME_GUILDCOMBAT_MAINTENANCE2), NULL, 0xffffffff);
			g_Neuz.m_2DRender.SetFont(pOldFont);
		}


		if (g_GuildCombatMng.m_nState == CGuildCombat::WAR_STATE &&
			g_GuildCombatMng.m_nGCState == CGuildCombat::WAR_WAR_STATE)
		{
			if (g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
				PlayMusic(BGM_BA_CRISIS, 0);

			g_Caption1.AddCaption(prj.GetText(TID_GAME_GUILDCOMBAT_WAR), pWndWorld->m_pFontAPITitle);
		}


		if (g_GuildCombatMng.m_nState == CGuildCombat::COMPLET_CLOSE_STATE &&
			g_GuildCombatMng.m_nGCState == CGuildCombat::WAR_CLOSE_STATE)
		{
			g_Caption1.AddCaption(prj.GetText(TID_GAME_GUILDCOMBAT_WAR_END), pWndWorld->m_pFontAPITitle);
		}


		if (g_GuildCombatMng.m_nState == CGuildCombat::COMPLET_CLOSE_STATE &&
			g_GuildCombatMng.m_nGCState == CGuildCombat::WAR_CLOSE_WAIT_STATE)
		{
			g_GuildCombatMng.m_bRequest = FALSE;
		}


		if (g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE &&
			g_GuildCombatMng.m_nGCState == CGuildCombat::WAR_TELEPORT_STATE)
		{
			pWndWorld->m_dwGuildCombatTime = 0xffffffff;
			pWndWorld->m_mapGC_GuildStatus.clear();
		}
	}
}

void CDPClient::OnGuildCombatNextTimeState(CAr& ar)
{
	DWORD dwTime, dwGuildCombatState;
	ar >> dwTime >> dwGuildCombatState;

	if (!g_pPlayer)
		return;

	CWorld* pWorld = g_pPlayer->GetWorld();
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	if (!pWorld || pWndWorld == NULL)
	{
		Error("CDPClient::OnGuildCombatNextTimeState() err");
		return;
	}

	BOOL bTimeSet = FALSE;
	switch (dwGuildCombatState)
	{
	case CGuildCombat::WAR_WAR_STATE:
	{
		if (pWorld->GetID() == WI_WORLD_GUILDWAR)
		{
			bTimeSet = TRUE;
			sprintf(pWndWorld->m_szGuildCombatStr, prj.GetText(TID_GAME_GUILDCOMBAT_MAINTENANCE));
		}
	}
	break;
	case CGuildCombat::WAR_CLOSE_STATE:
	{
		if (pWorld->GetID() == WI_WORLD_GUILDWAR)
		{
			bTimeSet = TRUE;
			sprintf(pWndWorld->m_szGuildCombatStr, prj.GetText(TID_GAME_GUILDCOMBAT_WAR_END));
		}
	}
	break;
	case CGuildCombat::WAR_TELEPORT_STATE:
	{
		if (pWorld->GetID() == WI_WORLD_GUILDWAR)
		{
			bTimeSet = TRUE;
			sprintf(pWndWorld->m_szGuildCombatStr, prj.GetText(TID_GAME_GUILDCOMBAT_EXIT_MSG));
		}
	}
	break;
	case CGuildCombat::ENTER_STATE:
	{
		CGuild* pGuild = g_pPlayer->GetGuild();
		CGuildMember* pGuildMemberl;

		if (pGuild && g_GuildCombatMng.IsRequest())
		{
			pGuildMemberl = pGuild->GetMember(g_pPlayer->m_idPlayer);

			if (pGuildMemberl && (pGuildMemberl->m_nMemberLv == GUD_MASTER || pGuildMemberl->m_nMemberLv == GUD_KINGPIN))
			{
				bTimeSet = TRUE;
				sprintf(pWndWorld->m_szGuildCombatStr, prj.GetText(TID_GAME_GUILDCOMBAT_PLAYER_MAKEUP_FINISH));
			}
		}
	}
	break;
	}

	if (bTimeSet)
		pWndWorld->m_dwGuildCombatTime = GetTickCount() + dwTime;
}

void CDPClient::OnGuildCombatEnterTime(CAr& ar)
{
	DWORD dwEnterTime;
	ar >> dwEnterTime;

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	if (pWndWorld && g_GuildCombatMng.IsRequest())
	{
		pWndWorld->m_dwGuildCombatTime = GetTickCount() + dwEnterTime;

		sprintf(pWndWorld->m_szGuildCombatStr, prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_LIMIT_TIME));

		g_WndMng.PutString(prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_NOW), NULL, 0xffffffff);
	}

}
void CDPClient::OnGCWinGuild(CAr& ar)
{
	ar >> g_GuildCombatMng.m_nGuildCombatIndex >> g_GuildCombatMng.m_uWinGuildId >> g_GuildCombatMng.m_nWinGuildCount;
}
void CDPClient::OnGCBestPlayer(CAr& ar)
{
	ar >> g_GuildCombatMng.m_uBestPlayer;
}

void CDPClient::OnFriendGameJoin(CAr& ar)
{
	g_WndMng.m_RTMessenger.Serialize(ar);

	if (g_WndMng.m_RTMessenger.size())
		SendGetFriendState();

	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
	if (pWndMessengerEx)
		pWndMessengerEx->UpdateFriendList();
}

void CDPClient::OnAddFriend(CAr& ar)
{
	u_long uidPlayer;
	char lpName[MAX_PLAYER] = { 0, };

	ar >> uidPlayer;
	ar.ReadString(lpName, MAX_PLAYER);

	g_WndMng.m_RTMessenger.SetFriend(uidPlayer, NULL);
	SendGetFriendState();

	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
	if (pWndMessengerEx)
		pWndMessengerEx->UpdateFriendList();
}

void CDPClient::OnRemoveFriend(CAr& ar)
{
	u_long uidPlayer;
	ar >> uidPlayer;
	g_WndMng.m_RTMessenger.RemoveFriend(uidPlayer);

	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
	if (pWndMessengerEx)
		pWndMessengerEx->UpdateFriendList();
}


void CDPClient::OnAddFriendReqest(CAr& ar)
{
	char szLeaderName[MAX_NAME] = { 0, };
	u_long uLeaderid;
	LONG nLeaderJob;
	BYTE nLeaderSex;
	ar >> uLeaderid;
	ar >> nLeaderSex;
	ar >> nLeaderJob;
	ar.ReadString(szLeaderName, MAX_NAME);

	if (g_Option.m_bMessenger == FALSE)
	{
		SendBlock(2, szLeaderName);
		return;
	}

	CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_FRIEND_CONFIRM);

	if (pWndBaseBuf)
	{
		SendBlock(6, szLeaderName);
		return;
	}

	SAFE_DELETE(g_WndMng.m_pWndFriendConfirm);
	g_WndMng.m_pWndFriendConfirm = new CWndFriendConFirm;
	g_WndMng.m_pWndFriendConfirm->Initialize();
	g_WndMng.m_pWndFriendConfirm->SetMember(uLeaderid, g_pPlayer->m_idPlayer, nLeaderJob, nLeaderSex, szLeaderName);
}

void CDPClient::OnAddFriendCancel(CAr& ar)
{

	g_WndMng.PutString(prj.GetText(TID_GAME_MSGDENYINVATE), NULL, prj.GetTextColor(TID_GAME_MSGDENYINVATE));
}

void CDPClient::OnAddFriendError(CAr& ar)
{
	char szName[64] = { 0, };
	BYTE nError;
	ar >> nError;
	ar.ReadString(szName, 64);
	CString szMessage;
	if (nError == 1)
		szMessage.Format(prj.GetText(TID_DIAG_0059), szName);
	else if (nError == 2)
		szMessage.Format(prj.GetText(TID_DIAG_0060), szName);

	g_WndMng.OpenMessageBox(szMessage);
}

void CDPClient::OnAddFriendNotConnect(CAr& ar)
{
	char szName[64] = { 0, };
	ar.ReadString(szName, 64);
	CString szMessage;
	szMessage.Format(prj.GetText(TID_DIAG_0061), szName);

	g_WndMng.OpenMessageBox(szMessage);
}

void CDPClient::OnRemoveFriendState(CAr& ar)
{
	u_long uRemoveid;
	ar >> uRemoveid;

	g_WndMng.m_RTMessenger.SetFriend(uRemoveid, NULL);

#if __VER >= 11 // __CSC_VER11_4
	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
	if (pWndMessengerEx)
		pWndMessengerEx->UpdateFriendList();
#endif //__CSC_VER11_4
}

void CDPClient::OnFriendJoin(CAr& ar)
{
	u_long idFriend;
	DWORD dwState;
	u_long uLogin;

	ar >> idFriend >> dwState >> uLogin;

	Friend* pFriend = g_WndMng.m_RTMessenger.GetFriend(idFriend);
	if (pFriend)
	{
		pFriend->dwState = dwState;
		if (g_Option.m_bMessengerJoin && pFriend->dwState != FRS_OFFLINE)
		{
			CWndInstantMsg* pWndInstantMsg = g_WndMng.OpenInstantMsg(CPlayerDataCenter::GetInstance()->GetPlayerString(idFriend));
			if (pWndInstantMsg)
				pWndInstantMsg->AddMessageJoin(CPlayerDataCenter::GetInstance()->GetPlayerString(idFriend));
		}
#if __VER >= 11 // __CSC_VER11_4
		CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
		if (pWndMessengerEx)
			pWndMessengerEx->UpdateFriendList();
#endif //__CSC_VER11_4
	}
}

void CDPClient::OnFriendLogOut(CAr& ar)
{
	u_long idFriend;
	ar >> idFriend;
	Friend* pFriend = g_WndMng.m_RTMessenger.GetFriend(idFriend);
	if (pFriend)
	{
		pFriend->dwState = FRS_OFFLINE;
		CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
		if (pWndMessengerEx)
			pWndMessengerEx->UpdateFriendList();
	}
}

void CDPClient::OnFriendNoIntercept(CAr& ar)
{
	u_long idFriend;
	ar >> idFriend;
	g_WndMng.m_RTMessenger.SetBlock(idFriend, FALSE);
	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
	if (pWndMessengerEx)
		pWndMessengerEx->UpdateFriendList();
}

void CDPClient::OnFriendIntercept(CAr& ar)
{
	u_long idPlayer, idFriend;
	ar >> idPlayer >> idFriend;
	if (g_pPlayer)
	{
		if (g_pPlayer->m_idPlayer == idPlayer)
		{
			g_WndMng.m_RTMessenger.SetBlock(idFriend, TRUE);
		}
		else
		{
			Friend* pFriend = g_WndMng.m_RTMessenger.GetFriend(idPlayer);
			if (pFriend)
				pFriend->dwState = FRS_OFFLINE;
		}
#if __VER >= 11 // __CSC_VER11_4
		CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
		if (pWndMessengerEx)
			pWndMessengerEx->UpdateFriendList();
#endif //__CSC_VER11_4
	}
}

void CDPClient::OnGetFriendState(CAr& ar)
{
	int nCount;
	int nCountBlock;
	u_long uidPlayer;
	u_long uIdofMulti;
	DWORD dwState;

	ar >> nCount;
	ar >> nCountBlock;

	for (int i = 0; i < nCount; ++i)
	{
		ar >> uidPlayer;
		ar >> dwState;
		ar >> uIdofMulti;

		Friend* pFriend = g_WndMng.m_RTMessenger.GetFriend(uidPlayer);
		if (pFriend)
			pFriend->dwState = dwState;
		CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
		if (pWndMessengerEx)
			pWndMessengerEx->UpdateFriendList();
	}
	for (int i = 0; i < nCountBlock; ++i)
	{
		ar >> uidPlayer;
		ar >> dwState;
		ar >> uIdofMulti;

		Friend* pFriend = g_WndMng.m_RTMessenger.GetFriend(uidPlayer);
		if (pFriend)
			pFriend->dwState = dwState;
		CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
		if (pWndMessengerEx)
			pWndMessengerEx->UpdateFriendList();
	}
}

void CDPClient::OnSetFriendState(CAr& ar)
{
	u_long uidPlayer;
	DWORD dwState;
	ar >> uidPlayer;
	ar >> dwState;

	if (g_pPlayer)
	{
		if (g_pPlayer->m_idPlayer == uidPlayer)
		{
			g_WndMng.m_RTMessenger.SetState(dwState);
		}
		else
		{
			Friend* pFriend = g_WndMng.m_RTMessenger.GetFriend(uidPlayer);
			if (pFriend)
				pFriend->dwState = dwState;
			CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
			if (pWndMessengerEx)
				pWndMessengerEx->UpdateFriendList();
		}
	}
}

void CDPClient::OnOneFriendState(CAr& ar)
{
	u_long idFriend;
	DWORD dwState;
	ar >> idFriend >> dwState;
	Friend* pFriend = g_WndMng.m_RTMessenger.GetFriend(idFriend);
	if (pFriend)
		pFriend->dwState = dwState;
#if __VER >= 11 // __CSC_VER11_4
	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
	if (pWndMessengerEx)
		pWndMessengerEx->UpdateFriendList();
#endif //__CSC_VER11_4
}

void CDPClient::OnBlock(CAr& ar)
{
	BYTE nGu;
	char szName[MAX_NAME] = { 0, };
	ar >> nGu;
	ar.ReadString(szName, MAX_NAME);

	CString szMessage;
	if (nGu == 1)
	{
		CWndMessage* pWndMessage = g_WndMng.GetMessagge(szName);
		if (pWndMessage)
		{
			pWndMessage->Destroy();
		}
		szMessage.Format(prj.GetText(TID_GAME_MSGCHATDENY), szName);
		g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_MSGCHATDENY));
	}
	else
		if (nGu == 2)
		{
			szMessage.Format(prj.GetText(TID_GAME_MSGDENY), szName);
			g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_MSGDENY));
		}
		else
			if (nGu == 3)
			{
				szMessage.Format(prj.GetText(TID_GAME_TRADEDENY), szName);
				g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_TRADEDENY));
			}
			else
				if (nGu == 4)
				{
					szMessage.Format(prj.GetText(TID_GAME_REPAIR_NOTTRADE), szName);
					g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_TRADEDENY));
				}
				else
					if (nGu == 5)
					{

						g_WndMng.PutString(prj.GetText(TID_GAME_YETTRADE), NULL, prj.GetTextColor(TID_GAME_YETTRADE));
					}
					else
						if (nGu == 6)
						{

							g_WndMng.PutString(prj.GetText(TID_GAME_YETFRIEND), NULL, prj.GetTextColor(TID_GAME_YETFRIEND));
						}
						else
							if (nGu == 7)
							{

								g_WndMng.PutString(prj.GetText(TID_GAME_YETPARTY), NULL, prj.GetTextColor(TID_GAME_YETPARTY));
							}
							else
								if (nGu == 8)
								{

									g_WndMng.PutString(prj.GetText(TID_GAME_TRADELIMITPC), NULL, prj.GetTextColor(TID_GAME_TRADELIMITPC));
								}
}

void CDPClient::OnSetGuildQuest(CAr& ar)
{
	int nQuestId, nState;
	ar >> nQuestId >> nState;
	CMover* pPlayer;

	if (pPlayer = CMover::GetActiveMover())
	{
		CGuild* pGuild = pPlayer->GetGuild();
		if (pGuild)
		{
			pGuild->SetQuest(nQuestId, nState);
		}
	}
}

void CDPClient::OnRemoveGuildQuest(CAr& ar)
{
	int nQuestId;
	ar >> nQuestId;

	CMover* pPlayer;

	if (pPlayer = CMover::GetActiveMover())
	{
		CGuild* pGuild = pPlayer->GetGuild();
		if (pGuild)
		{
			pGuild->RemoveQuest(nQuestId);
		}
	}
}


void CDPClient::OnSetQuest(OBJID objid, CAr& ar)
{

	QUEST quest;
	ar.Read(&quest, sizeof(quest));

	CString strTemp;
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover) && pMover->IsActiveMover())
	{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
#else // __IMPROVE_QUEST_INTERFACE
		CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetApplet(APP_QUEST);
#endif // __IMPROVE_QUEST_INTERFACE
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

		LPQUEST lpCurQuest = pMover->FindQuest(quest.m_wId);
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(quest.m_wId);

		if (lpCurQuest && quest.m_nState != QS_END)
		{

			DWORD dwCurTime = lpCurQuest->m_wTime & 0x7fff;
			DWORD dwTime = quest.m_wTime & 0x7fff;
			if (dwCurTime != dwTime)
			{
				strTemp.Format(GETTEXT(TID_QUEST_LIMIT_TIME), dwTime);
				g_WndMng.PutString(strTemp);
				if (dwTime)
				{
					if (::GetLanguage() != LANG_JAP)
						g_CapTime.SetTime(dwTime, pWndWorld->m_pFontAPITime);
					else
						g_CapTime.SetTime(dwTime, NULL);
				}
				else
				{
					g_WndMng.PutString(GETTEXT(TID_QUEST_LIMIT_TIMEOUT));
					g_WndMng.PutString(GETTEXT(TID_QUEST_FAILURE));
					SendRemoveQuest(quest.m_wId);
				}
			}

			if (lpCurQuest->m_bPatrol != quest.m_bPatrol)
			{
				strTemp.Format(GETTEXT(TID_QUEST_PATROL), pQuestProp->m_szPatrolZoneName);
				g_WndMng.PutString(strTemp);
			}

			for (int i = 0; i < 2; i++)
			{
				if (lpCurQuest->m_nKillNPCNum[i] != quest.m_nKillNPCNum[i])
				{
					MoverProp* pMoverProp = prj.GetMoverProp(pQuestProp->m_nEndCondKillNPCIdx[i]);
					strTemp.Format(GETTEXT(TID_QUEST_KILL_NPC), pMoverProp->szName, quest.m_nKillNPCNum[i], pQuestProp->m_nEndCondKillNPCNum[i]);
					g_WndMng.PutString(strTemp);
				}
			}

			if (lpCurQuest->m_bDialog != quest.m_bDialog)
			{
				LPCHARACTER lpCharacter = prj.GetCharacter(pQuestProp->m_szEndCondDlgCharKey);
				strTemp.Format(GETTEXT(TID_QUEST_DIALOG), lpCharacter->m_strName);
				g_WndMng.PutString(strTemp);
			}
			pMover->SetQuest(&quest);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			if (pWndQuestDetail)
				pWndQuestDetail->UpdateQuestText();

			CWndQuestQuickInfo* pWndQuestQuickInfo = g_WndMng.m_pWndQuestQuickInfo;
			if (pWndQuestQuickInfo)
				pWndQuestQuickInfo->SetUpdateTextSwitch(TRUE);
#else // __IMPROVE_QUEST_INTERFACE
			if (pWndQuest)
				pWndQuest->UpdateText();
#endif // __IMPROVE_QUEST_INTERFACE
		}

		else
		{
			pMover->SetQuest(&quest);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			if (pWndQuestDetail)
				pWndQuestDetail->UpdateQuestText(quest.m_wId);

			CWndQuestQuickInfo* pWndQuestQuickInfo = g_WndMng.m_pWndQuestQuickInfo;
			if (pWndQuestQuickInfo)
				pWndQuestQuickInfo->SetUpdateTextSwitch(TRUE);
#else // __IMPROVE_QUEST_INTERFACE
			if (pWndQuest)
				pWndQuest->Update(quest.m_wId);
#endif // __IMPROVE_QUEST_INTERFACE
		}

		if (pQuestProp)
		{
			QuestState* pQuestState = pQuestProp->m_questState[quest.m_nState];
			if (quest.m_nState == 1)
				g_WndMng.PutDefinedString(TID_EVE_STARTJOB1, "");
			//g_WndMng.PutString( prj.GetText( TID_EVE_STARTJOB1 ), NULL, prj.GetTextColor( TID_EVE_STARTJOB1 ) );
			else
				if (quest.m_nState == QS_END) // VHOME
				{
					if (quest.m_wId != QUEST_CREGUILD)
						g_WndMng.PutDefinedString(TID_EVE_ENDJOB1, "");
				}
			//g_WndMng.PutString( prj.GetText( TID_EVE_ENDJOB1 ), NULL, prj.GetTextColor( TID_EVE_ENDJOB1 ) );
		}
		//}
	}
}

void CDPClient::OnMoverSetDestObj(OBJID objid, CAr& ar)
{
	CMover* pMover = prj.GetMover(objid);
	float fRange;
	ar >> objid >> fRange;

	//CCtrl* pCtrl = prj.GetCtrl(objid);
	if (IsValidObj((CObj*)pMover) /* && IsValidObj((CCtrl*)pCtrl)*/)
		pMover->SetDestObj(objid, fRange, FALSE);
}


void CDPClient::OnMoverBehavior(OBJID objid, CAr& ar)
{
	CMover* pMover = prj.GetMover(objid);
	if (::IsValidObj(pMover))
	{
		pMover->AutoSynchronizer()->Serialize(ar);
	}
	else
	{
		AutoSynchronizer temp(NULL);
		temp.Serialize(ar);
	}
}

// Sender
void CDPClient::SendJoin(BYTE nSlot, DWORD dwWorldID, CMover* pMover, CRTMessenger* pRTMessenger, u_long uIdofMulti)
{
#ifndef __MAP_SECURITY
	g_WorldMng.Open(g_Neuz.m_pd3dDevice, dwWorldID);
	g_WorldMng()->RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
	g_WorldMng()->SetCamera(&g_Neuz.m_camera);
	g_Neuz.m_camera.Transform(D3DDEVICE, g_WorldMng.Get());
	g_Neuz.m_camera.Reset();
	g_WorldMng()->ReadWorld(pMover->GetPos());
#endif // __MAP_SECURITY

	g_Neuz.m_idPlayer = pMover->m_idPlayer;

	BEFORESENDSOLE(ar, PACKETTYPE_JOIN, DPID_UNKNOWN);
	ar << dwWorldID;
	ar << pMover->m_idPlayer;
	ar << g_Neuz.m_dwAuthKey;
	ar << pMover->m_idparty;
	ar << pMover->m_idGuild;
	ar << uIdofMulti;
	ar << nSlot;
	ar.WriteString(pMover->GetName());
	ar.WriteString(g_Neuz.m_szAccount);
	ar.WriteString(g_Neuz.m_szPassword);
	pRTMessenger->Serialize(ar);

	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendChat(LPCSTR lpszChat)
{
#ifdef __FL_UPDATE_CHATTING
	if (IsChatSpamString(lpszChat))
	{
		if (g_WndMng.m_dwLastSentChatMessage + 500 > GetTickCount())
		{
			CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase(APP_COMMUNICATION_CHAT);
			if (pWndChat)
				pWndChat->PutString(prj.GetText(TID_GAME_ERROR_CHATTING_1), prj.GetTextColor(TID_GAME_ERROR_CHATTING_2));

			g_WndMng.PutString(prj.GetText(TID_GAME_ERROR_CHATTING_1));
			return;
		}
		else
			g_WndMng.m_dwLastSentChatMessage = GetTickCount();
	}
#endif

	BEFORESENDSOLE(ar, PACKETTYPE_CHAT, DPID_UNKNOWN);
	ar << g_pPlayer->m_dwAuthorization;
	ar.WriteString(lpszChat);
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPClient::SendGuildCombatWindow(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_IN_GUILDCOMBAT, DPID_UNKNOWN);
	ar << GC_IN_WINDOW;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGuildCombatApp(DWORD dwPenya)
{
	BEFORESENDSOLE(ar, PACKETTYPE_IN_GUILDCOMBAT, DPID_UNKNOWN);
	ar << GC_IN_APP;
	ar << dwPenya;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGuildCombatCancel(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_OUT_GUILDCOMBAT, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGCRequestStatusWindow(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REQUEST_STATUS, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGCSelectPlayerWindow(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SELECTPLAYER_GUILDCOMBAT, DPID_UNKNOWN);
	ar << (BOOL)TRUE;		// bWindow
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGCSelectPlayer(vector<u_long> vecSelectPlayer, u_long uidDefender)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SELECTPLAYER_GUILDCOMBAT, DPID_UNKNOWN);
	ar << (BOOL)FALSE;		// bWindow
	ar << uidDefender;
	int nSize = vecSelectPlayer.size();
	ar << nSize;
	for (int i = 0; i < nSize; ++i)
	{
		ar << vecSelectPlayer[i];
	}
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGCJoin(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_JOIN_GUILDCOMBAT, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGCTele(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_TELE_GUILDCOMBAT, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendGCPlayerPoint(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendRemoveItem(CItemElem* pItemElem, int nNum)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REMOVEINVENITEM, DPID_UNKNOWN);
	ar << pItemElem->m_dwObjId;
	ar << nNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendMoveItem(BYTE nItemType, BYTE nSrcIndex, BYTE nDestIndex)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MOVEITEM, DPID_UNKNOWN);
	ar << nItemType << nSrcIndex << nDestIndex;	// 3/12
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendMoveBankItem(BYTE nSrcIndex, BYTE nDestIndex)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MOVEBANKITEM, DPID_UNKNOWN);
	ar << nSrcIndex << nDestIndex;	// 3/12
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendDropItem(DWORD dwItemType, DWORD dwItemId, short nITemNum, const D3DXVECTOR3& vPos)
{
	if (g_WndMng.m_pWndDialog)
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_DIALOGNODROPITEM), NULL, prj.GetTextColor(TID_GAME_DIALOGNODROPITEM));
		return;
	}

	BEFORESENDSOLE(ar, PACKETTYPE_DROPITEM, DPID_UNKNOWN);
	ar << dwItemType << dwItemId << nITemNum << vPos;		// 14/20
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendDropGold(DWORD dwGold, const D3DXVECTOR3& vPlayerPos, const D3DXVECTOR3& vPos)
{
	D3DXVECTOR3 vOut, v = vPos - vPlayerPos;
	D3DXVec3Normalize(&vOut, &v);
	v = vPlayerPos + vOut;

	BEFORESENDSOLE(ar, PACKETTYPE_DROPGOLD, DPID_UNKNOWN);
	ar << dwGold << v;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendTrade(CMover* pTrader)
{
	if (IsValidObj(pTrader)/**/ && pTrader->GetType() == OT_MOVER && ((CMover*)pTrader)->IsPlayer())
	{
		BEFORESENDSOLE(ar, PACKETTYPE_TRADE, DPID_UNKNOWN);
		ar << pTrader->GetId();
		SEND(ar, this, DPID_SERVERPLAYER);
	}
}

void CDPClient::SendConfirmTrade(CMover* pTrader)
{
	if (IsValidObj(pTrader)/**/ && pTrader->GetType() == OT_MOVER && ((CMover*)pTrader)->IsPlayer())
	{
		BEFORESENDSOLE(ar, PACKETTYPE_CONFIRMTRADE, DPID_UNKNOWN);
		ar << pTrader->GetId();
		SEND(ar, this, DPID_SERVERPLAYER);
	}
}

void CDPClient::SendConfirmTradeCancel(OBJID objid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CONFIRMTRADECANCEL, DPID_UNKNOWN);
	ar << objid;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendTradePut(BYTE i, DWORD dwItemObjID, int ItemNum)
{
	if (ItemNum <= 0)
		return;

	BEFORESENDSOLE(ar, PACKETTYPE_TRADEPUT, DPID_UNKNOWN);
	ar << i << dwItemObjID << ItemNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendTradePull(BYTE byNth)
{
	BEFORESENDSOLE(ar, PACKETTYPE_TRADEPULL, DPID_UNKNOWN);
	ar << byNth;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendTradePutGold(DWORD dwGold)
{
	BEFORESENDSOLE(ar, PACKETTYPE_TRADEPUTGOLD, DPID_UNKNOWN);
	ar << dwGold;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendTradeCancel(int nMode)
{
	BEFORESENDSOLE(ar, PACKETTYPE_TRADECANCEL, DPID_UNKNOWN);
	ar << nMode;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendTradeConfirm()
{
	BEFORESENDSOLE(ar, PACKETTYPE_TRADECONFIRM, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendPartySkillUse(int nSkill)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PARTYSKILLUSE, DPID_UNKNOWN);
	ar << nSkill;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendChangeShareItem(int nItemMode)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PARTYCHANGEITEMMODE, DPID_UNKNOWN);
	ar << nItemMode;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendChangeShareExp(int nExpMode)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PARTYCHANGEEXPMODE, DPID_UNKNOWN);
	ar << nExpMode;
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPClient::SendPartyMemberRequest(u_long uMemberId, BOOL bTroup)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MEMBERREQUEST, DPID_UNKNOWN);
	ar << uMemberId << bTroup;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendPartyMemberCancle(u_long uLeader, u_long uMember, int nMode)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MEMBERREQUESTCANCLE, DPID_UNKNOWN);
	ar << uLeader << uMember << nMode;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendAddPartyMember(u_long uLeader)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADDPARTYMEMBER, DPID_UNKNOWN);
	ar << uLeader;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendRemovePartyMember(u_long MemberId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REMOVEPARTYMEMBER, DPID_UNKNOWN);
	ar << MemberId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendChangeTroup(BOOL bSendName, const char* szPartyName)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CHANGETROUP, DPID_UNKNOWN);
	ar << bSendName;
	if (bSendName)
	{
		ar.WriteString(szPartyName);
	}
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendChangePartyName(const char* szPartyName)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CHANPARTYNAME, DPID_UNKNOWN);
	ar.WriteString(szPartyName);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendPartyChangeLeader(u_long uChangerLeaderid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PARTYCHANGELEADER, DPID_UNKNOWN);
	ar << uChangerLeaderid;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendFocusObj()
{
	CObj* pFocusObj = g_WorldMng()->GetObjFocus();
	if (pFocusObj && IsValidObj(pFocusObj) && pFocusObj->GetType() == OT_MOVER)
	{
		CMover* pMover = ((CMover*)pFocusObj);
		if (pMover->GetWorld() && pMover->GetWorld()->GetObjFocus())
		{
			BEFORESENDSOLE(ar, PACKETTYPE_FOCUSOBJ, DPID_UNKNOWN);
			ar << ((CMover*)pMover->GetWorld()->GetObjFocus())->GetId();
			SEND(ar, this, DPID_SERVERPLAYER);
		}
	}
}

void CDPClient::OnFocusObj(CAr& ar)
{
	OBJID objid;
	ar >> objid;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		CWorld* pWorld = g_WorldMng.Get();

		if (pWorld)
			pWorld->SetObjFocus(pMover);
	}
}

void CDPClient::SendScriptDialogReq(OBJID objid, LPCTSTR lpKey, int nGlobal1, int nGlobal2, int nGlobal3, int nGlobal4)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SCRIPTDLG, DPID_UNKNOWN);
	ar << objid;
	ar.WriteString((lpKey ? lpKey : _T("")));
	ar << nGlobal1 << nGlobal2 << nGlobal3 << nGlobal4;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendOpenShopWnd(OBJID objid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_OPENSHOPWND, DPID_UNKNOWN);
	ar << objid;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendCloseShopWnd(void)
{
	SendHdr(PACKETTYPE_CLOSESHOPWND);
}

void CDPClient::SendOpenBankWnd(DWORD dwId, DWORD dwItemId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_OPENBANKWND, DPID_UNKNOWN);
	ar << dwId << dwItemId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendOpenGuildBankWnd()
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_BANK_WND, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendCloseGuildBankWnd()
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_BANK_WND_CLOSE, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendChangeBankPass(const char* szLastPass, const char* szNewPass, DWORD dwId, DWORD dwItemId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CHANGEBANKPASS, DPID_UNKNOWN);
	ar.WriteString(szLastPass);
	ar.WriteString(szNewPass);
	ar << dwId << dwItemId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendConfirmBank(const char* szPass, DWORD dwId, DWORD dwItemId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CONFIRMBANK, DPID_UNKNOWN);
	ar.WriteString(szPass);
	ar << dwId << dwItemId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendCloseBankWnd(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CLOSEBANKWND, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendDoUseSkillPoint(SKILL aJobSkill[])
{
	BEFORESENDSOLE(ar, PACKETTYPE_DOUSESKILLPOINT, DPID_UNKNOWN);

	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		LPSKILL lpSkill;
		lpSkill = &aJobSkill[i];

		ItemProp* pSkillProp;

		if (lpSkill->dwSkill != NULL_ID)
			pSkillProp = prj.GetSkillProp(lpSkill->dwSkill);

		ar << lpSkill->dwSkill << lpSkill->dwLevel;
	}

	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendStateModeCancel(DWORD dwStateMode, BYTE nFlag)
{
	BEFORESENDSOLE(ar, PACKETTYPE_STATEMODE, DPID_UNKNOWN);
	ar << dwStateMode;
	ar << nFlag;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendPutItemBank(BYTE nSlot, DWORD dwItemObjID, int ItemNum)
{
	if (ItemNum <= 0)
		return;

	BEFORESENDSOLE(ar, PACKETTYPE_PUTITEMBACK, DPID_UNKNOWN);
	ar << nSlot << dwItemObjID << ItemNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGetItemBank(BYTE nSlot, DWORD dwItemObjID, int ItemNum)
{
	if (ItemNum <= 0)
		return;

	BEFORESENDSOLE(ar, PACKETTYPE_GETITEMBACK, DPID_UNKNOWN);
	ar << nSlot << dwItemObjID << ItemNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendPutGoldBank(BYTE nSlot, DWORD dwGold)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PUTGOLDBACK, DPID_UNKNOWN);
	ar << nSlot << dwGold;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendGetGoldBank(BYTE nSlot, DWORD dwGold)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GETGOLDBACK, DPID_UNKNOWN);
	ar << nSlot << dwGold;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendPutItemBankToBank(BYTE nPutSlot, BYTE nSlot, DWORD dwItemObjID, int ItemNum)
{
	if (ItemNum <= 0)
		return;

	BEFORESENDSOLE(ar, PACKETTYPE_PUTBACKTOBANK, DPID_UNKNOWN);
	BYTE nFlag = 1;
	ar << nFlag << nPutSlot << nSlot << dwItemObjID << ItemNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendPutGoldBankToBank(BYTE nPutSlot, BYTE nSlot, DWORD dwGold)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PUTBACKTOBANK, DPID_UNKNOWN);
	BYTE nFlag = 0;
	ar << nFlag << nPutSlot << nSlot << dwGold;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendPutItemGuildBank(DWORD dwItemObjID, int ItemNum, BYTE p_Mode)
{
	if (ItemNum <= 0)
		return;

	BEFORESENDSOLE(ar, PACKETTYPE_PUTITEMGUILDBANK, DPID_UNKNOWN);
	ar << dwItemObjID << ItemNum << p_Mode;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGetItemGuildBank(DWORD dwItemObjID, int ItemNum, BYTE p_Mode)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GETITEMGUILDBANK, DPID_UNKNOWN);
	ar << dwItemObjID << ItemNum << p_Mode;
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPClient::SendBuyItem(CHAR cTab, DWORD dwItemObjID, short nNum)
{
	BEFORESENDSOLE(ar, PACKETTYPE_BUYITEM, DPID_UNKNOWN);
	ar << cTab << dwItemObjID << nNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendBuyChipItem(CHAR cTab, DWORD dwItemObjID, short nNum)
{
	BEFORESENDSOLE(ar, PACKETTYPE_BUYCHIPITEM, DPID_UNKNOWN);
	ar << cTab << dwItemObjID << nNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSellItem(DWORD dwId, int nNum)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SELLITEM, DPID_UNKNOWN);
	ar << dwId << nNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendMeleeAttack(OBJMSG dwAtkMsg, OBJID objid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MELEE_ATTACK, DPID_UNKNOWN);
	ar << dwAtkMsg << objid;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendMeleeAttack2(OBJID objid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MELEE_ATTACK2, DPID_UNKNOWN);
	ar << objid;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendMagicAttack(OBJID objid, int nMagicPower, int idSfxHit)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MAGIC_ATTACK, DPID_UNKNOWN);
	ar << objid << nMagicPower << idSfxHit;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendRangeAttack(OBJID objid, DWORD dwItemID, int idSfxHit)
{
	BEFORESENDSOLE(ar, PACKETTYPE_RANGE_ATTACK, DPID_UNKNOWN);
	ar << objid << dwItemID << idSfxHit;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendUseSkill(WORD wType, WORD wId, OBJID objid, int nUseSkill, BOOL bControl)
{
	BEFORESENDSOLE(ar, PACKETTYPE_USESKILL, DPID_UNKNOWN);
	ar << wType << wId << objid << nUseSkill;
	ar << bControl;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendUseObject(OBJID objid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_USEOBJECT, DPID_UNKNOWN);
	ar << objid;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendTeleSkill(OBJID objid, D3DXVECTOR3 vPos)
{
	BEFORESENDSOLE(ar, PACKETTYPE_TELESKILL, DPID_UNKNOWN);
	ar << vPos;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSfxID(OBJID idTarget, int idSfxHit, DWORD dwType, DWORD dwSkill, int nMaxDmgCnt)
{
	if (nMaxDmgCnt <= 0)
		nMaxDmgCnt = 1;
	BEFORESENDSOLE(ar, PACKETTYPE_SFX_ID, DPID_UNKNOWN);
	ar << idTarget << idSfxHit << dwType << dwSkill << nMaxDmgCnt;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSetTarget(OBJID idTarget, BYTE bClear)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SETTARGET, DPID_UNKNOWN);
	ar << idTarget << bClear;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendExp(EXPINTEGER nExp)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SEND_TO_SERVER_EXP, DPID_UNKNOWN);
	ar << nExp;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendCorrReq(OBJID idObj)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CORR_REQ, DPID_UNKNOWN);
	ar << idObj;
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPClient::OnUseObject(OBJID objid, CAr& ar)
{
	CMover* pMover = prj.GetMover(objid);
	if (!IsValidObj(pMover))
		return;

	if (pMover->IsActiveMover())
	{
		pMover->m_dwReqFlag &= (~REQ_USEOBJECT);
		g_WorldMng()->SetObjFocus(NULL);
	}

	pMover->SendActMsg(OBJMSG_PICKUP, 0, 0, 1);
}


void CDPClient::OnCorrReq(OBJID objid, CAr& ar)
{
	D3DXVECTOR3 vPos;
	OBJID idAttacker;
	ar >> vPos >> idAttacker;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		pMover->SetPos(vPos);
		pMover->m_idAttacker = idAttacker;
	}
}
void CDPClient::SendPlayerBehavior(void)
{
	CMover* pPlayer = CMover::GetActiveMover();
	if (::IsValidObj(pPlayer))
	{
		BEFORESENDSOLE(ar, PACKETTYPE_PLAYERBEHAVIOR, DPID_UNKNOWN);
		pPlayer->AutoSynchronizer()->Serialize(ar);
		SEND(ar, this, DPID_SERVERPLAYER);
	}
}

void CDPClient::SendRemoveItemLevelDown(DWORD dwId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REMOVE_ITEM_LEVEL_DOWN, DPID_UNKNOWN);
	ar << dwId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendDoUseItemTarget(DWORD dwMaterial, DWORD dwTarget)
{
	BEFORESENDSOLE(ar, PACKETTYPE_DO_USE_ITEM_TARGET, DPID_UNKNOWN);
	ar << dwMaterial << dwTarget;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendBlessednessCancel(int nItem)
{
	BEFORESENDSOLE(ar, PACKETTYPE_BLESSEDNESS_CANCEL, DPID_UNKNOWN);
	ar << nItem;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendDoUseItem(DWORD dwItemId, char* szInput, int nType)
{
	if (!g_pPlayer)
		return;

	if (!g_pPlayer->IsPossibleUseItem())
		return;

	CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(nType);
	if (!pContainer)
		return;

	CItemElem* pItemBase = pContainer->GetAtId(dwItemId);
	if (!pItemBase)
		return;

	ItemProp* pItemProp = pItemBase->GetProp();
	if (!pItemProp)
		return;

	if (pItemProp->dwParts == NULL_ID && pItemBase->IsFlag(CItemElem::expired))
	{
		if (pItemProp->dwItemKind3 == IK3_EGG)
			g_WndMng.PutString(prj.GetText(TID_GAME_PET_DEAD));
		else
			g_WndMng.PutString(prj.GetText(TID_GAME_ITEM_EXPIRED));

		return;
	}

	DWORD dwGroup = g_pPlayer->m_cooltimeMgr.GetGroup(pItemProp);
	if (dwGroup)
	{
		if (!g_pPlayer->m_cooltimeMgr.CanUse(dwGroup))
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_ATTENTIONCOOLTIME), NULL, prj.GetTextColor(TID_GAME_ATTENTIONCOOLTIME));
			return;
		}
	}

	if (!g_pPlayer->IsUseItemReadyTime(pItemProp, pItemBase->m_dwObjId, nType))
		return;

	if (nType == TYPE_INVENTORY)
	{
		if (pItemProp->dwID == II_SYS_SYS_SCR_CUSTODY)
		{
			SendOpenBankWnd(pItemBase->m_dwObjId, pItemBase->m_dwItemId);
			return;
		}

		if (pItemProp->dwID == II_SYS_SYS_FEED_01)
		{
			SendUsePetFeed(dwItemId);
			return;
		}
	}

	BEFORESENDSOLE(ar, PACKETTYPE_DOUSEITEM, DPID_UNKNOWN);
	ar << dwItemId;
	ar << nType;
	ar.WriteString(szInput);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendPlayerDestObj(OBJID objid, float fRange)
{
	CMover* pPlayer = CMover::GetActiveMover();
	if (IsValidObj((CObj*)pPlayer))
	{
		BEFORESENDSOLE(ar, PACKETTYPE_PLAYERSETDESTOBJ, DPID_UNKNOWN);
		ar << objid << fRange;
		SEND(ar, this, DPID_SERVERPLAYER);
	}
}

void CDPClient::SendSkillTaskBar(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SKILLTASKBAR, DPID_UNKNOWN);
	int nCount = 0;
	u_long uOffset = ar.GetOffset();
	ar << nCount;
	for (int i = 0; i < MAX_SLOT_QUEUE; i++)
	{
		ar << (BYTE)i << g_WndMng.m_pWndTaskBar->m_aSlotQueue[i].m_dwShortcut << g_WndMng.m_pWndTaskBar->m_aSlotQueue[i].m_dwId << g_WndMng.m_pWndTaskBar->m_aSlotQueue[i].m_dwType;
		ar << g_WndMng.m_pWndTaskBar->m_aSlotQueue[i].m_dwIndex << g_WndMng.m_pWndTaskBar->m_aSlotQueue[i].m_dwUserId << g_WndMng.m_pWndTaskBar->m_aSlotQueue[i].m_dwData;
	}
	nCount = MAX_SLOT_QUEUE;
	int nBufSize1;
	LPBYTE lpBuf1 = ar.GetBuffer(&nBufSize1);
	*(UNALIGNED int*)(lpBuf1 + uOffset) = nCount;

	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendRemoveAppletTaskBar(BYTE nIndex)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REMOVEAPPLETTASKBAR, DPID_UNKNOWN);
	ar << nIndex;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendAddAppletTaskBar(BYTE nIndex, LPSHORTCUT pAppletShortcut)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADDAPPLETTASKBAR, DPID_UNKNOWN);
	ar << nIndex;
	ar << pAppletShortcut->m_dwShortcut << pAppletShortcut->m_dwId << pAppletShortcut->m_dwType;
	ar << pAppletShortcut->m_dwIndex << pAppletShortcut->m_dwUserId << pAppletShortcut->m_dwData;
	ar << pAppletShortcut->m_nLocation;

	if (pAppletShortcut->m_dwShortcut == SHORTCUT_CHAT)
	{
		ar.WriteString(pAppletShortcut->m_szString);
	}
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendRemoveItemTaskBar(BYTE nSlotIndex, BYTE nIndex)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REMOVEITEMTASKBAR, DPID_UNKNOWN);
	ar << nSlotIndex << nIndex;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendAddItemTaskBar(BYTE nSlotIndex, BYTE nIndex, LPSHORTCUT pItemShortcut)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADDITEMTASKBAR, DPID_UNKNOWN);
	ar << nSlotIndex << nIndex;
	ar << pItemShortcut->m_dwShortcut << pItemShortcut->m_dwId << pItemShortcut->m_dwType;
	ar << pItemShortcut->m_dwIndex << pItemShortcut->m_dwUserId << pItemShortcut->m_dwData;
	ar << pItemShortcut->m_nLocation;

	if (pItemShortcut->m_dwShortcut == SHORTCUT_CHAT)
	{
		ar.WriteString(pItemShortcut->m_szString);
	}
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendAddFriend(u_long uidPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADDFRIEND, DPID_UNKNOWN);
	ar << uidPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendAddFriendReqest(u_long uidPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADDFRIENDREQEST, DPID_UNKNOWN);
	ar << uidPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendAddFriendNameReqest(const char* szName)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADDFRIENDNAMEREQEST, DPID_UNKNOWN);
	ar.WriteString(szName);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendFriendCancel(u_long uidLeader)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADDFRIENDCANCEL, DPID_UNKNOWN);
	ar << uidLeader;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGetFriendState()
{
	BEFORESENDSOLE(ar, PACKETTYPE_GETFRIENDSTATE, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSetState(int state)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SETFRIENDSTATE, DPID_UNKNOWN);
	ar << state;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendFriendInterceptState(u_long uidPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_FRIENDINTERCEPTSTATE, DPID_UNKNOWN);
	ar << uidPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendRemoveFriend(u_long uidPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REMOVEFRIEND, DPID_UNKNOWN);
	ar << uidPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendBlock(BYTE Gu, const char* szFrom)
{
	BEFORESENDSOLE(ar, PACKETTYPE_BLOCK, DPID_UNKNOWN);
	ar << Gu;
	ar.WriteString(szFrom);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::PutPlayerDestPos(CONST D3DXVECTOR3& vPos, bool bForward, BYTE f)
{
	m_ss.playerdestpos.fValid = TRUE;
	m_ss.playerdestpos.vPos = vPos;
	m_ss.playerdestpos.fForward = (bForward ? 1 : 0);
	if (f)
		m_ss.uFrameMove = 1;
}


void CDPClient::SendSnapshot(BOOL fUnconditional)
{
	m_ss.uFrameMove++;
	if (fUnconditional || m_ss.uFrameMove % 30 == 0)	// 5 / 1
	{
		BEFORESENDSOLE(ar, PACKETTYPE_SNAPSHOT, DPID_UNKNOWN);

		u_long uOffset = ar.GetOffset();
		BYTE c = 0;
		ar << c;

		if (m_ss.playerdestpos.fValid == TRUE)
		{
			ar << SNAPSHOTTYPE_DESTPOS;
			ar << m_ss.playerdestpos.vPos;	// 12
			ar << m_ss.playerdestpos.fForward;	// 1

			m_ss.playerdestpos.fValid = FALSE;
			c++;
		}

		if (c > 0)
		{
			BYTE* lpBuf = ar.GetBuffer(&nBufSize);
			*(lpBuf + uOffset) = c;
			Send((LPVOID)lpBuf, nBufSize, DPID_SERVERPLAYER);
		}

		m_ss.uFrameMove = 0;
	}
}

void CDPClient::SendSfxHit(int idSfxHit, DWORD dwSkill, OBJID idAttacker)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SFX_HIT, DPID_UNKNOWN);
	ar << idSfxHit << dwSkill << idAttacker;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSfxClear(int idSfxHit, OBJID idMover)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SFX_CLEAR, DPID_UNKNOWN);
	ar << idSfxHit;
	ar << idMover;

	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendRemoveQuest(DWORD dwQuest)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REMOVEQUEST, DPID_UNKNOWN);
	ar << dwQuest;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendHdr(DWORD dwHdr)
{
	BEFORESENDSOLE(ar, dwHdr, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

#ifdef __EVE_MINIGAME
void CDPClient::OnMiniGame(OBJID objid, CAr& ar)
{
	BYTE nState;
	ar >> nState;
	switch (nState)
	{
	case MINIGAME_KAWIBAWIBO_RESUTLT:
		OnKawibawibo_Result(ar);
		break;
	}
}

void CDPClient::OnKawibawibo_Result(CAr& ar)
{
	int nResult;

	int		nWinCount = 0;
	int		nMaxWinCount = 0;
	DWORD	dwItemId = (DWORD)(-1);
	DWORD	dwNextItemId = (DWORD)(-1);
	int		nItemCount = -1;
	int		nNextItemCount = -1;

	ar >> nResult;
	ar >> nWinCount;

	CWndKawiBawiBoGame* pWndGame = (CWndKawiBawiBoGame*)g_WndMng.GetWndBase(APP_MINIGAME_KAWIBAWIBO);

	if (nResult == CMiniGame::KAWIBAWIBO_WIN)
	{
		ar >> dwItemId >> dwNextItemId;
		ar >> nItemCount >> nNextItemCount;
		ar >> nMaxWinCount;

		pWndGame->m_nWinningMaxCount = nMaxWinCount;
		ItemProp* ITem = prj.GetItemProp(dwItemId);
		ItemProp* ITemNext = prj.GetItemProp(dwNextItemId);
	}

	else if (nResult == CMiniGame::KAWIBAWIBO_GETITEM)
	{
		ar >> dwItemId;
		ar >> nItemCount;
		ItemProp* ITem = prj.GetItemProp(dwItemId);

		CString message;

		if (pWndGame)
		{
			message.Format(prj.GetText(TID_GAME_KAWIBAWIBO_WIN), pWndGame->m_nWinningCount, ITem->szName, nItemCount);
			pWndGame->m_nWinningCount = 0;
			pWndGame->RefreshInfo();
		}
		else
			message.Format(prj.GetText(TID_GAME_KAWIBAWIBO_WIN), nWinCount, ITem->szName, nItemCount);

		g_WndMng.OpenMessageBox(message);
	}

	if (pWndGame != NULL)
	{
		pWndGame->ReceiveResult(nResult, nItemCount, nNextItemCount, dwItemId, dwNextItemId);
	}
}
#endif // 

void CDPClient::OnUltimateWeapon(OBJID objid, CAr& ar)
{
	BYTE nState;
	ar >> nState;
	switch (nState)
	{
	case ULTIMATE_MAKEITEM:
		OnUltimateMakeItem(objid, ar);
		break;
	case ULTIMATE_MAKEGEM:
		OnUltimateMakeGem(objid, ar);
		break;
	case ULTIMATE_SETGEM:
		OnUltimateSetGem(objid, ar);
		break;
	case ULTIMATE_REMOVEGEM:
		OnUltimateRemoveGem(objid, ar);
		break;
	case ULTIMATE_ENCHANTWEAPON:
		OnUltimateEnchantWeapon(objid, ar);
		break;
	}

}

void CDPClient::OnUltimateMakeItem(OBJID objid, CAr& ar)
{
	int nResult;
	ar >> nResult;

	CWndMixJewel* pWndMixJewel = (CWndMixJewel*)g_WndMng.GetWndBase(APP_SMELT_MIXJEWEL);
	if (pWndMixJewel != NULL)
		pWndMixJewel->ReceiveResult(nResult);
}

void CDPClient::OnUltimateMakeGem(OBJID objid, CAr& ar)
{
	int nResult, nCount;
	ar >> nResult;
	ar >> nCount;

	CWndExtraction* pWndExtraction = (CWndExtraction*)g_WndMng.GetWndBase(APP_SMELT_EXTRACTION);
	if (pWndExtraction != NULL)
		pWndExtraction->ReceiveResult(nResult, nCount);
}

void CDPClient::OnUltimateSetGem(OBJID objid, CAr& ar)
{
	int nResult;
	ar >> nResult;

	CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)g_WndMng.GetWndBase(APP_SMELT_JEWEL);
	if (pWndSmeltJewel != NULL)
		pWndSmeltJewel->ReceiveResult(nResult);
}

void CDPClient::OnUltimateRemoveGem(OBJID objid, CAr& ar)
{
	int nResult;
	ar >> nResult;

	CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)g_WndMng.GetWndBase(APP_SMELT_JEWEL);
	if (pWndSmeltJewel && pWndSmeltJewel->m_pItemElem)
	{
		if (pWndSmeltJewel->m_pMoonstone)
		{
			if (g_pPlayer && !g_pPlayer->m_vtInfo.IsTrading(pWndSmeltJewel->m_pMoonstone))
			{
				pWndSmeltJewel->m_pMoonstone->SetExtra(0);
				pWndSmeltJewel->m_pMoonstone = NULL;
			}
		}
		pWndSmeltJewel->InitializeJewel(pWndSmeltJewel->m_pItemElem);
	}
}

void CDPClient::OnUltimateEnchantWeapon(OBJID objid, CAr& ar)
{
	int nResult;
	ar >> nResult;
}

#ifdef __TRADESYS
void CDPClient::OnExchange(OBJID objid, CAr& ar)
{
	BYTE nState;
	ar >> nState;
	switch (nState)
	{
	case EXCHANGE_RESULT:
		OnExchangeResult(ar);
		break;
	}
}

void CDPClient::OnExchangeResult(CAr& ar)
{
	int nResult;
	ar >> nResult;

	CWndDialogEvent* pWndDialogEvent = (CWndDialogEvent*)g_WndMng.GetWndBase(APP_DIALOG_EVENT);
	if (pWndDialogEvent != NULL)
		pWndDialogEvent->ReceiveResult(nResult);
}
#endif // __TRADESYS

#if __VER >= 10 // __CSC_VER9_1
void CDPClient::OnLegendSkillUp(CAr& ar)
{
	int nResult;
	ar >> nResult;

	CWndHeroSkillUp* pWndHeroSkillUp = (CWndHeroSkillUp*)g_WndMng.GetWndBase(APP_HERO_SKILLUP);
	if (pWndHeroSkillUp != NULL)
		pWndHeroSkillUp->ReceiveResult(nResult);
}
#endif //__CSC_VER9_1

void CDPClient::OnShout(CAr& ar)
{
	static	\
		CHAR	lpszPlayer[MAX_PLAYER], lpString[260];

	OBJID objid;
	ar >> objid;
	ar.ReadString(lpszPlayer, MAX_PLAYER);
	ar.ReadString(lpString, 260);
	DWORD dwColor = 0xffff99cc;
#if __VER >= 12 // __LORD
	ar >> dwColor;
#endif	// __LORD

	CMover* pMover = prj.GetMover(objid);

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	if (pMover && pMover->IsPlayer() == TRUE)
	{
		set< CString >::iterator BlockedUserIterator = prj.m_setBlockedUserID.find(lpszPlayer);
		if (BlockedUserIterator != prj.m_setBlockedUserID.end())
			return;
	}
#endif // __YS_CHATTING_BLOCKING_SYSTEM


	if (IsValidObj(pMover) && pMover->GetWorld() && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
	{
		if (pMover->IsMode(GCWAR_RENDER_SKIP_MODE))
			return;
	}

	CString str;
	str.Format("[%s] %s", lpszPlayer, lpString);
	g_WndMng.PutString(str, NULL, dwColor, CHATSTY_SHOUT);

	if (IsValidObj(pMover))
	{
		if (strlen(lpString))
			g_DialogMsg.AddMessage(pMover, lpString, 0xfffffff, CHAT_SHOUT);
	}
}

void CDPClient::OnPlayMusic(CAr& ar)
{
	u_long idmusic;
	ar >> idmusic;
	PlayMusic(idmusic, 1);
}

void CDPClient::OnPlaySound(CAr& ar)
{
	char szSound[MAX_PATH] = { 0, };
	u_long idsound;

	BYTE nType;
	ar >> nType;
	if (nType == 0)
	{
		ar >> idsound;
		PLAYSND(idsound);
	}
	else if (nType == 1)
	{
		ar.ReadString(szSound, MAX_PATH);
		if (g_pPlayer)
			PLAYSND(szSound, NULL, TRUE);
	}
}

void CDPClient::OnWhisper(CAr& ar)
{
	if (g_pPlayer == NULL)
		return;
	static	\
		CHAR	sPlayerFrom[MAX_PLAYER], sPlayerTo[MAX_PLAYER], lpString[260];
	int nSearch;
	u_long idFrom, idTo;

	ar.ReadString(sPlayerFrom, MAX_PLAYER);
	ar.ReadString(sPlayerTo, MAX_PLAYER);
	ar.ReadString(lpString, 260);
	ar >> idFrom >> idTo;
	ar >> nSearch;

	Friend* pFriend = NULL;
	u_long idPlayer = 0;
	BOOL	bCheck = FALSE;
	if (strcmp(sPlayerFrom, g_pPlayer->GetName()) == 0)
	{
		idPlayer = idTo;
		pFriend = g_WndMng.m_RTMessenger.GetFriend(idTo);
		bCheck = TRUE;
	}
	else
	{
		idPlayer = idFrom;
		pFriend = g_WndMng.m_RTMessenger.GetFriend(idFrom);
	}

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	set< CString >::iterator BlockedUserIterator = prj.m_setBlockedUserID.find(sPlayerFrom);
	if ((pFriend && pFriend->bBlock) || (BlockedUserIterator != prj.m_setBlockedUserID.end()))
#else // 
	if (pFriend && pFriend->bBlock)
#endif // 
	{
		if (bCheck)
		{
			CString szMessage;
			szMessage.Format(prj.GetText(TID_GAME_MSGBLOCKCHR), sPlayerTo);
			g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN));
			CWndMessage* pWndMessage = g_WndMng.GetMessagge(sPlayerTo);
			if (pWndMessage)
			{
				CWndEdit* pWndText = (CWndEdit*)pWndMessage->GetDlgItem(WIDC_EDIT);
				CWndButton* pWndSend = (CWndButton*)pWndMessage->GetDlgItem(WIDC_SEND);
				if (pWndText && pWndSend)
				{
					pWndSend->EnableWindow(FALSE);
					pWndText->SetVisible(FALSE);
				}
			}
		}
		return;
	}
	if (nSearch)
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_NOTLOGIN), NULL, prj.GetTextColor(TID_GAME_NOTLOGIN));
		return;
	}

	BOOL bMySend = FALSE;

	CString str;
	if (strcmp(g_pPlayer->GetName(), sPlayerFrom) == 0)
	{
		str.Format("-> [%s] %s", sPlayerTo, lpString);
		bMySend = TRUE;
	}
	else
		str.Format("<- [%s] %s", sPlayerFrom, lpString);

	if (bMySend == FALSE && g_Option.m_bSay == FALSE)
	{
		SendBlock(1, sPlayerFrom);
		return;
	}

	g_WndMng.PutString(str, NULL, 0xffffff00, CHATSTY_WHISPER);
}

void CDPClient::OnSay(CAr& ar)
{
	if (g_pPlayer == NULL)
		return;
	static	\
		CHAR	sPlayerFrom[MAX_PLAYER], sPlayerTo[MAX_PLAYER], lpString[260];
	int nSearch;
	u_long idFrom, idTo;

	ar.ReadString(sPlayerFrom, MAX_PLAYER);
	ar.ReadString(sPlayerTo, MAX_PLAYER);
	ar.ReadString(lpString, 260);
	ar >> idFrom >> idTo;
	ar >> nSearch;

	u_long idPlayer = 0;
	DWORD dwState = 0xFFFFFFFF;
	BOOL bCheck = FALSE;
	if (strcmp(sPlayerFrom, g_pPlayer->GetName()) == 0)
	{
		idPlayer = idTo;
		bCheck = TRUE;
	}
	else
		idPlayer = idFrom;
	Friend* pFriend = g_WndMng.m_RTMessenger.GetFriend(idPlayer);

	if (pFriend)
	{
		if (pFriend->bBlock)
		{
			if (bCheck)
			{
				CString szMessage;
				szMessage.Format(prj.GetText(TID_GAME_MSGBLOCKCHR), sPlayerTo);

				g_WndMng.PutString(szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN));
				CWndMessage* pWndMessage = g_WndMng.GetMessagge(sPlayerTo);
				if (pWndMessage)
				{
					CWndEdit* pWndText = (CWndEdit*)pWndMessage->GetDlgItem(WIDC_EDIT);
					CWndButton* pWndSend = (CWndButton*)pWndMessage->GetDlgItem(WIDC_SEND);
					if (pWndText && pWndSend)
					{
						pWndSend->EnableWindow(FALSE);
						pWndText->SetVisible(FALSE);
					}
				}
			}
			return;
		}
	}
	if (nSearch)
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_NOTLOGIN), NULL, prj.GetTextColor(TID_GAME_NOTLOGIN));
		return;
	}

#ifndef __FL_SAVE_MESSAGES
	CString strFrom = sPlayerFrom;
	CString strTo = sPlayerTo;
	if (strcmp(sPlayerFrom, g_pPlayer->GetName()) == 0)
	{
		strFrom = sPlayerTo;
		strTo = sPlayerFrom;
		CWndMessage* pWndMessage = g_WndMng.GetMessage(strFrom);

		if (pWndMessage)
			pWndMessage->AddMessage(sPlayerFrom, lpString);
		else
		{
			pWndMessage = g_WndMng.OpenMessage(strFrom);
			pWndMessage->AddMessage(strTo, lpString);
		}
	}
	else
	{
		if (g_Option.m_bSay == FALSE)
		{
			SendBlock(1, g_pPlayer->GetName(), sPlayerFrom);
			return;
		}

		CWndMessage* pWndMessage = g_WndMng.GetMessage(strFrom);
		if (pWndMessage)
			pWndMessage->AddMessage(sPlayerFrom, lpString);
		else
		{
			CWndInstantMsg* pWndInstantMsg = g_WndMng.OpenInstantMsg(strFrom);
			if (pWndInstantMsg)
				pWndInstantMsg->AddMessage(sPlayerFrom, lpString);

			CString str;
			str.Format("[%s] %s", sPlayerFrom, lpString);
			g_WndMng.PutString(str, NULL, 0xffffff00);
		}
	}
#else
	CString strFrom = sPlayerFrom;
	CString strTo = sPlayerTo;

	if (g_pPlayer->m_idPlayer == idFrom) // If i sent message
	{
		CWndMessage* pWndMessage = g_WndMng.GetMessagge(strTo);
		if (!pWndMessage)
			pWndMessage = g_WndMng.OpenMessage(strTo);

		PRIVATE_MESSAGES messages;
		messages.str = lpString;
		messages.bIsFromMe = TRUE;
		messages.cTime = CTime::GetCurrentTime();

		auto i = g_Neuz.m_mapPrivateMessages.find((LPCSTR)strTo);
		if (i != g_Neuz.m_mapPrivateMessages.end())
		{
			if (i->second.size() >= 50)
				i->second.erase(i->second.begin());

			i->second.push_back(messages);
		}
		else
		{
			vector < PRIVATE_MESSAGES> vec;
			vec.push_back(messages);
			g_Neuz.m_mapPrivateMessages.insert({ (LPCSTR)strTo, vec });

			CWndMessengerEx* pWndList = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
			if (pWndList)
				pWndList->UpdateList();
		}

		pWndMessage->AddMessage(strFrom, lpString);
	}
	else // If someone else sent message to me
	{
		if (!g_Option.m_bSay)
		{
			SendBlock(1, sPlayerFrom);
			return;
		}

#ifdef __AIO_NEUZAPP
		if (g_Neuz.IsMinimised())
			g_Neuz.Flash();
#endif

		PRIVATE_MESSAGES messages;
		messages.str = lpString;
		messages.bIsFromMe = FALSE;
		messages.cTime = CTime::GetCurrentTime();

		auto i = g_Neuz.m_mapPrivateMessages.find((LPCSTR)strFrom);
		if (i != g_Neuz.m_mapPrivateMessages.end())
		{
			if (i->second.size() >= 50)
				i->second.erase(i->second.begin());

			i->second.push_back(messages);
		}
		else
		{
			vector < PRIVATE_MESSAGES> vec;
			vec.push_back(messages);
			g_Neuz.m_mapPrivateMessages.insert({ (LPCSTR)strFrom, vec });

			CWndMessengerEx* pWndList = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
			if (pWndList)
				pWndList->UpdateList();
		}

		CWndMessage* pWndMessage = g_WndMng.GetMessagge(strFrom);
		if (pWndMessage)
			pWndMessage->AddMessage(sPlayerFrom, lpString);
		else
		{
			CWndInstantMsg* pWndInstantMsg = g_WndMng.OpenInstantMsg(strFrom);
			if (pWndInstantMsg)
				pWndInstantMsg->AddMessage(sPlayerFrom, lpString);

			CString str;
			str.Format("[%s] %s", sPlayerFrom, lpString);
			g_WndMng.PutString(str, NULL, 0xffffff00);
		}
	}
#endif
}

void CDPClient::OnGMSay(CAr& ar)
{
	static	\
		CHAR	sPlayerFrom[MAX_PLAYER], sPlayerTo[MAX_PLAYER], lpString[260];

	ar.ReadString(sPlayerFrom, MAX_PLAYER);
	ar.ReadString(sPlayerTo, MAX_PLAYER);
	ar.ReadString(lpString, 260);

	CString str;
	str.Format("[%s] %s", sPlayerFrom, lpString);
	g_WndMng.PutString(str, NULL, 0xffffff00);
}

void CDPClient::OnGetPlayerAddr(CAr& ar)
{
	static	\
		CHAR	lpszPlayer[MAX_PLAYER], lpAddr[16];

	ar.ReadString(lpszPlayer, MAX_PLAYER);
	ar.ReadString(lpAddr, 16);
	CString str;
	str.Format("[IP] %s : %s", lpszPlayer, lpAddr);
	g_WndMng.PutString(str, NULL, 0xff0000ff);
}

void CDPClient::OnGetPlayerCount(CAr& ar)
{
	u_short	uCount;
	ar >> uCount;

	CString str;
	str.Format(prj.GetText(TID_ADMIN_WORLDCOUNT), uCount);
	g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_ADMIN_WORLDCOUNT), CHATSTY_GENERAL);
}

void CDPClient::OnGetCorePlayer(CAr& ar)
{
	static	\
		CHAR	lpszPlayer[64];
	u_long uCount;

	ar >> uCount;

	for (u_short i = 0; i < uCount; i++)
	{
		ar.ReadString(lpszPlayer, 64);
		CString str;
		str.Format("%s", lpszPlayer);

		g_WndMng.PutString(str, NULL, 0xffffff80, CHATSTY_GENERAL);
	}
}


void CDPClient::OnSystem(CAr& ar)
{
	CHAR	lpString[512];

	ar.ReadString(lpString, 512);

	CString str;
	str.Format(prj.GetText(TID_ADMIN_ANNOUNCE), lpString);
	g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_ADMIN_ANNOUNCE), CHATSTY_SYSTEM);
	if (::GetLanguage() != LANG_GER && ::GetLanguage() != LANG_FRE)
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
		if (pWndWorld)
			g_Caption1.AddCaption(str, pWndWorld->m_pFontAPITitle, FALSE, D3DCOLOR_ARGB(255, 255, 255, 153));
	}
}

void CDPClient::OnCaption(CAr& ar)
{
	static	\
		CHAR	lpString[260];

	BOOL bSmall;
	DWORD dwWorldID;
	ar >> bSmall;
	ar >> dwWorldID;
	ar.ReadString(lpString, 260);

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	if (pWndWorld)
	{
		BOOL bCaption = FALSE;
		if (dwWorldID == 0)
			bCaption = TRUE;
		else if (g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == dwWorldID)
			bCaption = TRUE;

		if (bCaption)
		{
			if (bSmall)
			{
				if (::GetLanguage() != LANG_JAP)
					g_Caption1.AddCaption(lpString, pWndWorld->m_pFontAPICaption);// CWndBase::m_Theme.m_pFontCaption );
				else
					g_Caption1.AddCaption(lpString, NULL);// CWndBase::m_Theme.m_pFontCaption );
			}
			else
			{
				if (::GetLanguage() != LANG_JAP)
					g_Caption1.AddCaption(lpString, pWndWorld->m_pFontAPITitle);// CWndBase::m_Theme.m_pFontCaption );
				else
					g_Caption1.AddCaption(lpString, NULL);// CWndBase::m_Theme.m_pFontCaption );
			}
		}
	}
}

void CDPClient::OnDefinedCaption(CAr& ar)
{
	TCHAR lpString[1024], argv[32][256], * pargv[32];
	int argc = 0;
	BOOL bSmall;

	DWORD dwText;
	ar >> bSmall;
	ar >> dwText;
	ar.ReadString(lpString, 1024);

	CScanner s(lpString);
	s.GetToken();
	while (s.tok != FINISHED)
	{
		_tcscpy(argv[argc], s.Token);
		argc++;
		s.GetToken();
	}

	for (int i = 0; i < argc; i++)
	{
		pargv[i] = argv[i];
	}

	CString string;
	string.FormatV(prj.GetText(dwText), (va_list)pargv);

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	if (pWndWorld)
	{
		if (bSmall)
		{
			if (::GetLanguage() != LANG_JAP)
				g_Caption1.AddCaption(string, pWndWorld->m_pFontAPICaption);// CWndBase::m_Theme.m_pFontCaption );
			else
				g_Caption1.AddCaption(string, NULL);// CWndBase::m_Theme.m_pFontCaption );
		}
		else
		{
			if (::GetLanguage() != LANG_JAP)
				g_Caption1.AddCaption(string, pWndWorld->m_pFontAPITitle);// CWndBase::m_Theme.m_pFontCaption );
			else
				g_Caption1.AddCaption(string, NULL);// CWndBase::m_Theme.m_pFontCaption );
		}
	}
}

void CDPClient::OnModifyMode(OBJID objid, CAr& ar)
{
	DWORD dwMode;
	ar >> dwMode;

	CMover* pPlayer = prj.GetMover(objid);
	if (IsValidObj((CObj*)pPlayer))
	{
		pPlayer->m_dwMode = dwMode;
		if (pPlayer->IsActiveMover() && g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
		{
			CString str;
			if (pPlayer->m_dwMode & MATCHLESS_MODE)
			{
				str += prj.GetText(TID_ADMIN_IMMORTAL);
			}
			if (pPlayer->m_dwMode & MATCHLESS2_MODE)
			{
				str += prj.GetText(TID_ADMIN_IMMORTALHALF);
			}
			if (pPlayer->IsMode(TRANSPARENT_MODE))
			{
				str += prj.GetText(TID_ADMIN_INVISIBLE);
			}
			if (pPlayer->IsMode(OBSERVE_MODE))
			{
				str += "관전";
			}
			if (pPlayer->IsMode(ITEM_MODE))
			{
				str += "아이템";
			}
			if (pPlayer->IsMode(NO_ATTACK_MODE))
			{
				str += "공격";
			}
			if (pPlayer->IsMode(COMMUNITY_MODE))
			{
				str += "커뮤니티";
			}
			if (pPlayer->m_dwMode & ONEKILL_MODE)
			{
				str += prj.GetText(TID_ADMIN_MIGHTY);
			}

			if (pPlayer->IsMode(SAYTALK_MODE))
			{
				str += "귓속말할수 없는";
			}
			if (pPlayer->IsMode(TALK_MODE))
			{
				str += "채팅할수 없는";
			}
			if (pPlayer->IsMode(SHOUTTALK_MODE))
			{
				str += "외치기할수 없는";
			}
			if (pPlayer->m_dwMode & DONMOVE_MODE)
			{
				str += prj.GetText(TID_ADMIN_NOMOVING);
			}
			if (pPlayer->m_dwMode != 0)
			{
				str += prj.GetText(TID_ADMIN_STATE);
			}
			else
			{
				str += prj.GetText(TID_ADMIN_NORMAL);
			}

			g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_ADMIN_NORMAL));
		}

		if (pPlayer->IsActiveMover())
		{
			if (pPlayer->IsMode(MODE_MAILBOX))
			{
				CString str;
				str += prj.GetText(TID_MAIL_RECEIVE);

				CWndInstantMsg* pWndInstantMsg = g_WndMng.OpenInstantMsg("Mail");
				if (pWndInstantMsg)
				{
					pWndInstantMsg->AddPostMessage(prj.GetText(TID_MAIL_RECEIVE));
					pWndInstantMsg->m_timer.Set(MIN(1));
				}

				g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_ADMIN_NORMAL));

			}
		}
	}
}

void CDPClient::OnStateMode(OBJID objid, CAr& ar)
{
	DWORD dwStateMode, dwOldStateMode;
	BYTE nFlag;
	OBJID dwItemId;
	ar >> dwStateMode;
	ar >> nFlag;
	if (nFlag == STATEMODE_BASEMOTION_ON)
	{
		ar >> dwItemId;
	}

	CMover* pPlayer = prj.GetMover(objid);
	if (IsValidObj((CObj*)pPlayer))
	{
		if (nFlag == STATEMODE_BASEMOTION_ON)
		{
			ItemProp* pItemProp = prj.GetItemProp(dwItemId);
			if (pItemProp)
			{
				CreateItemReadySfx(D3DDEVICE, pItemProp->dwSfxObj, pPlayer->GetPos(), pPlayer->GetId(),
					pPlayer->GetPos(), pPlayer->GetId(), -1);
			}
		}

		dwOldStateMode = pPlayer->m_dwStateMode;
		pPlayer->m_dwStateMode = dwStateMode;
		if (pPlayer->IsActiveMover())
		{
			CString strMessage;
			switch (nFlag)
			{
			case STATEMODE_BASEMOTION_ON:
			{
				g_pPlayer->m_nReadyTime = 0;
				g_pPlayer->m_dwUseItemId = dwItemId;
				g_pPlayer->m_nReadyTime = timeGetTime();
				//					g_WndMng.PutString( strMessage, NULL, prj.GetTextColor( TID_GAME_PARTYNOINVATE ) );
				if (g_pPlayer->m_pActMover->IsState(OBJSTA_STAND) == FALSE)
				{
					SendStateModeCancel(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL);
				}
			}
			break;
			case STATEMODE_BASEMOTION_OFF:
			{
				g_pPlayer->m_nReadyTime = 0;
				//					g_WndMng.PutString( strMessage, NULL, prj.GetTextColor( TID_GAME_PARTYNOINVATE ) );
			}
			break;
			case STATEMODE_BASEMOTION_CANCEL:
			{
				g_pPlayer->m_nReadyTime = 0;
				//					g_WndMng.PutString( strMessage, NULL, prj.GetTextColor( TID_GAME_PARTYNOINVATE ) );
			}
			break;
			}
		}
	}
}

void CDPClient::OnReturnSay(OBJID objid, CAr& ar)
{
	int ReturnFlag;
	CHAR	lpString[260];

	ar >> ReturnFlag;
	ar.ReadString(lpString, 260);
	CMover* pPlayer = prj.GetMover(objid);
	if (IsValidObj((CObj*)pPlayer))
	{
		CString Str;
		if (ReturnFlag == 1)
		{
			Str.Format("[%s] %s", pPlayer->GetName(), lpString);
		}
		else if (ReturnFlag == 2)
		{

			Str += prj.GetText(TID_GAME_MSGNOTTRANSELF);
		}
		else
		{

			Str.Format(prj.GetText(TID_GAME_MSGNOTEXIST), lpString);
		}
		//g_WndMng.PutString( Str, NULL, 0xffffff00 );
		g_WndMng.PutString(Str, NULL, prj.GetTextColor(TID_GAME_MSGNOTEXIST));
	}
}

void CDPClient::SendGuildInvite(OBJID objid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_INVITE, DPID_UNKNOWN);
	ar << objid;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendIgnoreGuildInvite(u_long idPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_IGNORE_GUILD_INVITE, DPID_UNKNOWN);
	ar << idPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPClient::SendDestroyGuild()
{
	BEFORESENDSOLE(ar, PACKETTYPE_DESTROY_GUILD, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendAddGuildMember(u_long idMaster)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADD_GUILD_MEMBER, DPID_UNKNOWN);
	ar << idMaster;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendRemoveGuildMember(u_long idPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REMOVE_GUILD_MEMBER, DPID_UNKNOWN);
	ar << idPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnCreateGuild(CAr& ar)
{
	u_long idPlayer, idGuild;
	char szGuild[MAX_G_NAME];
	char lpszPlayer[MAX_PLAYER];
	ar >> idPlayer >> idGuild;
	ar.ReadString(lpszPlayer, MAX_PLAYER);
	ar.ReadString(szGuild, MAX_G_NAME);

	CPlayerDataCenter::GetInstance()->AddPlayerData(idPlayer, lpszPlayer);

	CGuild* pGuild = new CGuild;
	pGuild->m_idGuild = idGuild;
	lstrcpy(pGuild->m_szGuild, szGuild);
	pGuild->m_idMaster = idPlayer;
	pGuild->m_adwPower[GUD_MASTER] = 0x000000FF;
	idGuild = g_GuildMng.AddGuild(pGuild);
	if (idGuild > 0)
	{
		CGuildMember* pMember = new CGuildMember;
		pMember->m_idPlayer = idPlayer;
		pMember->m_nMemberLv = GUD_MASTER;
		if (pGuild->AddMember(pMember))
		{
			CMover* pMover = prj.GetUserByID(idPlayer);
			if (pMover)
			{
				pMover->m_idGuild = idGuild;
				if (pMover->IsActiveMover())
				{
					// CWndX
				}
			}
			CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
			if (pGuild)
			{
				CString str;
				str.Format(prj.GetText(TID_GAME_COMCREATE), lpszPlayer, pGuild->m_szGuild);
				g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_COMCREATE));

#if __VER >= 11 // __CSC_VER11_4
				if (g_pPlayer->m_idPlayer == idPlayer)
				{
					CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
					if (pWndMessengerEx)
						pWndMessengerEx->UpdateGuildMemberList();
				}
#endif //__CSC_VER11_4
			}
		}
		else
		{
			SAFE_DELETE(pMember);
			g_GuildMng.RemoveGuild(idGuild);
		}
	}
	else
	{
		SAFE_DELETE(pGuild);
	}
}

void CDPClient::OnDestroyGuild(CAr& ar)
{
	char lpszPlayer[MAX_PLAYER];
	u_long idGuild;
	ar.ReadString(lpszPlayer, MAX_PLAYER);
	ar >> idGuild;

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		CString str;
		str.Format(prj.GetText(TID_GAME_COMDISMESS), lpszPlayer, pGuild->m_szGuild);
		if (idGuild == g_pPlayer->m_idGuild)
			g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_COMDISMESS));
		g_GuildMng.RemoveGuild(idGuild);

		CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
		if (pWndGuild)
		{
			pWndGuild->m_WndGuildTabApp.UpdateData();
			pWndGuild->m_WndGuildTabInfo.UpdateData();
			pWndGuild->m_WndGuildTabMember.UpdateData();
		}

		CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
		if (pWndMessengerEx)
			pWndMessengerEx->UpdateGuildMemberList();

	}
}

void CDPClient::OnGuild(CAr& ar)
{
	u_long idGuild;
	ar >> idGuild;
	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (!pGuild)
	{
		pGuild = new CGuild;
		pGuild->Serialize(ar, FALSE);
		g_GuildMng.AddGuild(pGuild);
	}
	else
		pGuild->Serialize(ar, FALSE);

	// This function called when we just join the game or just join the guild.
	// No reason to updatedata() in windows in this function, because  we send playerdata of all guildmembers
	// in AddPlayerData() when join the game and in OnAddGuildMember()/OnCreateGuild() when join the guild
	// And we also update guild/messenger window in OnQueryPlayerData() function when playerdata received by client
	// pWndGuild->m_WndGuildTabApp.UpdateData() updated in CDPClient::OnSetGuild()
#ifndef __FL_FIX_PLAYERDATA
	CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
	if (pWndGuild)
	{
		pWndGuild->m_WndGuildTabApp.UpdateData();
		pWndGuild->m_WndGuildTabInfo.UpdateData();
		pWndGuild->m_WndGuildTabMember.UpdateData();
	}

	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
	if (pWndMessengerEx)
		pWndMessengerEx->UpdateGuildMemberList();

	if (pGuild)
	{
		vector<PDVer>	aPDVer;
		CGuildMember* pMember;
		for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i)
		{
			pMember = i->second;
			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer, FALSE);
			PDVer pdv(pMember->m_idPlayer, pPlayerData->data.nVer);
			aPDVer.push_back(pdv);
		}

		SendQueryPlayerData(aPDVer);
	}
#endif
}

void CDPClient::OnSetGuild(OBJID objid, CAr& ar)
{
	u_long idGuild;
	ar >> idGuild;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		u_long idGuildOld = pMover->m_idGuild;
		pMover->m_idGuild = idGuild;

		if (g_pPlayer && g_pPlayer->GetId() == objid)
		{
			if (idGuild == 0)
			{
				CGuild* pGuild = g_GuildMng.GetGuild(idGuildOld);
				if (pGuild)
				{
					CString str;
					str.Format(prj.GetText(TID_GAME_COMLEAVE), pGuild->m_szGuild);
					g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_COMLEAVE));
				}
				GuildHouse->Clear();

			}
			else
			{
				CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
				if (pGuild)
				{
					PlayMusic(BGM_IN_COMPANY, 1);
					CString str;
					str.Format(prj.GetText(TID_GAME_COMACCEPT), pGuild->m_szGuild);
					g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_COMACCEPT));

					// This function called from worldserver when we create guild or join the guild
					// We have no reason to update other tabs/messenger now, because it will be updated as soon as playerdata will be recieved by client 
					// from CDPCoreClient::OnAddGuildMember or from CDPCoreClient::OnCreateGuild
#ifdef __FL_FIX_PLAYERDATA
					CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
					if (pWndGuild)
						pWndGuild->m_WndGuildTabApp.UpdateData();
#else
					CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
					if (pWndGuild)
					{
						pWndGuild->m_WndGuildTabApp.UpdateData();
						pWndGuild->m_WndGuildTabInfo.UpdateData();
						pWndGuild->m_WndGuildTabMember.UpdateData();
					}
					CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
					if (pWndMessengerEx)
					{
						pWndMessengerEx->UpdateFriendList();
						pWndMessengerEx->UpdateGuildMemberList();
					}
#endif
				}
			}
		}
	}
}

void CDPClient::OnAddGuildMember(CAr& ar)
{
	u_long idGuild;
	GUILD_MEMBER_INFO info;
	char lpszPlayer[MAX_PLAYER];
	ar.Read(&info, sizeof(GUILD_MEMBER_INFO));
	ar >> idGuild;
	ar.ReadString(lpszPlayer, MAX_PLAYER);

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		CGuildMember* pMember = new CGuildMember;
		pMember->m_idPlayer = info.idPlayer;
		pMember->m_nMemberLv = GUD_ROOKIE;
		if (pGuild->AddMember(pMember))
		{
			CString str;
			str.Format(prj.GetText(TID_GAME_COMACCEPTWELCOME), lpszPlayer);
			g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_COMACCEPTWELCOME));
		}

		// Same as OnSetGuild(). We wait from worldserver playerdata of new member and as soon as its received - we will update tabs.
#ifndef __FL_FIX_PLAYERDATA
		CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
		if (pWndGuild)
		{
			pWndGuild->m_WndGuildTabInfo.UpdateData();
			pWndGuild->m_WndGuildTabMember.UpdateData();
		}
		if (g_pPlayer->m_idGuild == idGuild)
		{
			CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
			if (pWndMessengerEx)
				pWndMessengerEx->UpdateGuildMemberList();
		}
#endif
	}
}

void CDPClient::OnRemoveGuildMember(CAr& ar)
{
	u_long idPlayer, idGuild;
	ar >> idPlayer >> idGuild;
	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		if (pGuild->RemoveMember(idPlayer))
		{
			LPCSTR lpszPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer);
			if (lpszPlayer)
			{
				CString str;
				str.Format(prj.GetText(TID_GAME_COMLEAVEBYE), lpszPlayer);
				g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_COMLEAVEBYE));
			}

			CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
			if (pWndGuild)
			{
				pWndGuild->m_WndGuildTabApp.UpdateData();
				pWndGuild->m_WndGuildTabInfo.UpdateData();
				pWndGuild->m_WndGuildTabMember.UpdateData();
			}

			CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
			if (pWndMessengerEx)
				pWndMessengerEx->UpdateGuildMemberList();

		}
	}
}

void CDPClient::OnGuildChat(CAr& ar)
{
	char sChat[1024];
	char sPlayer[MAX_PLAYER] = { 0, };
	OBJID objid;
	ar >> objid;

	ar.ReadString(sPlayer, MAX_PLAYER);
	ar.ReadString(sChat, 1024);

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	CMover* pMover = prj.GetMover(objid);
	if (pMover && pMover->IsPlayer() == TRUE)
	{
		set< CString >::iterator BlockedUserIterator = prj.m_setBlockedUserID.find(pMover->GetName(TRUE));
		if (BlockedUserIterator != prj.m_setBlockedUserID.end())
			return;
	}
#endif // __YS_CHATTING_BLOCKING_SYSTEM

	CString str;
	str.Format("[%s] %s", sPlayer, sChat);
	g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GUILD_CHAT), CHATSTY_GUILD);

#ifndef __YS_CHATTING_BLOCKING_SYSTEM
	CMover* pMover = prj.GetMover(objid);
#endif // __YS_CHATTING_BLOCKING_SYSTEM
	if (IsValidObj(pMover))
	{
		if (strlen(sChat))
			g_DialogMsg.AddMessage(pMover, sChat, 0xfffffff, CHAT_GUILD);
	}
}

void CDPClient::OnGuildMemberLv(CAr& ar)
{
	u_long idPlayer;
	int nMemberLv;
	ar >> idPlayer >> nMemberLv;

	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild && pGuild->IsMember(idPlayer))
	{
		CGuildMember* pMember = pGuild->GetMember(idPlayer);

		CString str;
		if (pMember->m_nMemberLv < nMemberLv)
		{
			if (pMember->GetMover())
				str.Format(prj.GetText(TID_GAME_GUILDMEM_APPUP), pMember->GetMover()->GetName());
		}
		else
		{
			if (pMember->GetMover())
				str.Format(prj.GetText(TID_GAME_GUILDMEM_APPDOWN), pMember->GetMover()->GetName());
		}

		pMember->m_nMemberLv = nMemberLv;

		pMember->m_nClass = 0;

		CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
		if (pWndGuild)
		{
			pWndGuild->m_WndGuildTabInfo.UpdateData();
			pWndGuild->m_WndGuildTabMember.UpdateData();
		}
#if __VER >= 11 // __CSC_VER11_4
		CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
		if (pWndMessengerEx)
			pWndMessengerEx->UpdateGuildMemberList();
#endif //__CSC_VER11_4
	}
}

void CDPClient::OnGuildClass(CAr& ar)
{
	u_long idPlayer;
	int nClass;
	ar >> idPlayer >> nClass;
	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild && pGuild->IsMember(idPlayer))
	{
		CGuildMember* pMember = pGuild->GetMember(idPlayer);

		if (pMember)
			pMember->m_nClass = nClass;

		CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
		if (pWndGuild)
		{
			pWndGuild->m_WndGuildTabInfo.UpdateData();
			pWndGuild->m_WndGuildTabMember.UpdateData();
		}
	}
}

void CDPClient::OnChgMaster(CAr& ar)
{
#ifndef __FL_GUILD_FINDER
	u_long idPlayer, idPlayer2;
	ar >> idPlayer >> idPlayer2;
	CGuild* pGuild = g_pPlayer->GetGuild();

	if (pGuild && pGuild->IsMember(idPlayer) && pGuild->IsMember(idPlayer2))
#else
	u_long idGuild, idPlayer, idPlayer2;
	ar >> idGuild >> idPlayer >> idPlayer2;
	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);

	if (pGuild)
#endif
	{
		pGuild->m_idMaster = idPlayer2;

		CGuildMember* pMember = pGuild->GetMember(idPlayer);
		if (pMember)
		{
			pMember->m_nMemberLv = GUD_ROOKIE;
			pMember->m_nClass = 0;
		}

		pMember = pGuild->GetMember(idPlayer2);
		if (pMember)
		{
			pMember->m_nMemberLv = GUD_MASTER;
			pMember->m_nClass = 0;
		}

		CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
		if (pWndGuild)
		{
			pWndGuild->m_WndGuildTabInfo.UpdateData();
			pWndGuild->m_WndGuildTabMember.UpdateData();
		}

#ifndef __FL_GUILD_FINDER
		if (pMember->GetMover())
#else
		if (pMember && pMember->GetMover())
#endif
		{
			CString szMsg;
			szMsg.Format(prj.GetText(TID_GAME_CHANGE_GUILDMASTER), pMember->GetMover()->GetName());
			g_WndMng.PutString(szMsg, NULL, prj.GetTextColor(TID_GAME_CHANGE_GUILDMASTER));
		}
		// output
	}
}

void CDPClient::OnGuildNickName(CAr& ar)
{
	u_long idPlayer;
	char strNickName[MAX_G_NAME] = { 0, };
	ar >> idPlayer;
	ar.ReadString(strNickName, MAX_G_NAME);
	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild && pGuild->IsMember(idPlayer))
	{
		CGuildMember* pMember = pGuild->GetMember(idPlayer);

		strcpy(pMember->m_szAlias, strNickName);

		CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
		if (pWndGuild)
		{
			pWndGuild->m_WndGuildTabInfo.UpdateData();
			pWndGuild->m_WndGuildTabMember.UpdateData();
		}
	}
}


void CDPClient::OnGuildMemberLogin(CAr& ar)
{
	BYTE nLogin;
	u_long uPlayerId, uMultiNo;
	ar >> nLogin >> uPlayerId >> uMultiNo;

	if (g_pPlayer)
	{
		CGuild* pGuild = g_pPlayer->GetGuild();
		if (pGuild && pGuild->IsMember(uPlayerId))
		{
			CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
			if (pWndGuild)
				pWndGuild->m_WndGuildTabMember.UpdateData();
			CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
			if (pWndMessengerEx)
				pWndMessengerEx->UpdateGuildMemberList();
		}
	}
}

void CDPClient::OnGuldMyGameJoin(CAr& ar)
{
	int		nMaxLoginGuild;
	u_long  uLoginPlayerIdGuild[200];
	u_long  uLoginGuildMulti[200];

	nMaxLoginGuild = 0;
	memset(uLoginPlayerIdGuild, 0, sizeof(uLoginPlayerIdGuild));
	memset(uLoginGuildMulti, 0, sizeof(uLoginGuildMulti));

	ar >> nMaxLoginGuild;
	ar.Read(uLoginPlayerIdGuild, sizeof(u_long) * nMaxLoginGuild);
	ar.Read(uLoginGuildMulti, sizeof(u_long) * nMaxLoginGuild);
	BOOL bGetInfo = FALSE;
	if (g_pPlayer)
	{
		CGuild* pGuild = g_pPlayer->GetGuild();
		if (pGuild)
		{
			bGetInfo = TRUE;

			CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
			if (pWndGuild)
			{
				pWndGuild->m_WndGuildTabInfo.UpdateData();
				pWndGuild->m_WndGuildTabMember.UpdateData();
			}

			CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
			if (pWndMessengerEx)
				pWndMessengerEx->UpdateGuildMemberList();

		}
	}

	if (bGetInfo == FALSE)
	{
		m_nMaxLoginGuild = nMaxLoginGuild;
		memcpy(m_uLoginPlayerIdGuild, uLoginPlayerIdGuild, sizeof(uLoginPlayerIdGuild));
		memcpy(m_uLoginGuildMulti, uLoginGuildMulti, sizeof(uLoginGuildMulti));
	}
}

void CDPClient::OnGuildError(CAr& ar)
{
	int nError;
	ar >> nError;

	switch (nError)
	{
	case 1:
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_COMOVERLAPNAME), NULL, prj.GetTextColor(TID_GAME_COMOVERLAPNAME));
		break;
	}
	case 2:
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_GUILDONLYNUMBER), NULL, prj.GetTextColor(TID_GAME_GUILDONLYNUMBER));
		break;
	}
	default:
		break;
	}
}

void CDPClient::OnGuildInvite(CAr& ar)
{
	u_long idGuild;
	ar >> idGuild;
	u_long idMaster;
	ar >> idMaster;


#ifdef __S_SERVER_UNIFY
	if (g_WndMng.m_bAllAction == FALSE)
		return;
#endif // __S_SERVER_UNIFY

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		SAFE_DELETE(g_WndMng.m_pWndGuildConfirm);
		g_WndMng.m_pWndGuildConfirm = new CWndGuildConfirm;
		g_WndMng.m_pWndGuildConfirm->SetGuildName(pGuild->m_szGuild);
		g_WndMng.m_pWndGuildConfirm->Initialize(NULL, APP_GUILD_INVATE);
		g_WndMng.m_pWndGuildConfirm->m_idMaster = idMaster;
	}
}

void CDPClient::OnAllGuilds(CAr& ar)
{
	g_GuildMng.Serialize(ar, TRUE);
}


void CDPClient::SendGuildMemberLv(u_long idMaster, u_long idPlayer, int nMemberLv)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_MEMBER_LEVEL, DPID_UNKNOWN);
	ar << idMaster << idPlayer << nMemberLv;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGuildClass(u_long idMaster, u_long idPlayer, BYTE nFlag)
{
	if (idMaster != idPlayer)
	{
		BEFORESENDSOLE(ar, PACKETTYPE_GUILD_CLASS, DPID_UNKNOWN);
		ar << nFlag;		// UP : 1, DOWN : 0
		ar << idMaster << idPlayer;
		SEND(ar, this, DPID_SERVERPLAYER);
	}
}

void CDPClient::SendGuildNickName(u_long idPlayer, LPCTSTR strNickName)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_NICKNAME, DPID_UNKNOWN);
	ar << idPlayer;
	ar.WriteString(strNickName);
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPClient::SendGuildContribution(BYTE cbPxpCount, int nGold, BYTE cbItemFlag)
{
	ASSERT(cbPxpCount == 0 || cbPxpCount == 1);
	ASSERT(nGold >= 0);

	BEFORESENDSOLE(ar, PACKETTYPE_NW_GUILDCONTRIBUTION, DPID_UNKNOWN);
	ar << cbPxpCount << nGold;
	ar << cbItemFlag;

	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPClient::SendGuildNotice(const char* szNotice)
{
	ASSERT(szNotice);

	BEFORESENDSOLE(ar, PACKETTYPE_NW_GUILDNOTICE, DPID_UNKNOWN);
	ar.WriteString(szNotice);
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPClient::SendGuildAuthority(u_long uGuildId, DWORD dwAuthority[])
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_AUTHORITY, DPID_UNKNOWN);
	ar << uGuildId;

	ar.Write(dwAuthority, sizeof(DWORD) * MAX_GM_LEVEL);

	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendGuildSetName(LPCTSTR szName)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_SETNAME, DPID_UNKNOWN);
	ar.WriteString(szName);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendChgMaster(DWORD idPlayer, u_long idPlayer2)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CHG_MASTER, DPID_UNKNOWN);
	ar << idPlayer << idPlayer2;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::UpdateGuildWnd()
{
	CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
	if (pWndGuild)
	{
		pWndGuild->m_WndGuildTabInfo.UpdateData();
		pWndGuild->m_WndGuildTabMember.UpdateData();
	}
}


void CDPClient::OnGuildContribution(CAr& ar)
{
	CONTRIBUTION_CHANGED_INFO info;
	ar >> info;

	CGuild* pGuild = g_GuildMng.GetGuild(info.idGuild);
	if (pGuild == NULL)
		return;

	if (info.nGuildLevel > pGuild->m_nLevel)
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_GUILDLEVELUP), NULL, prj.GetTextColor(TID_GAME_GUILDLEVELUP));

	}

	pGuild->SetContribution(info);
	UpdateGuildWnd();
}

void CDPClient::OnGuildNotice(CAr& ar)
{
	u_long idGuild;
	char szNotice[MAX_BYTE_NOTICE];
	ar >> idGuild;
	ar.ReadString(szNotice, MAX_BYTE_NOTICE);

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild == NULL)
		return;

	pGuild->SetNotice(szNotice);


	g_WndMng.PutString(prj.GetText(TID_GAME_GUILDNOTICEUPDATE), NULL, prj.GetTextColor(TID_GAME_GUILDNOTICEUPDATE));

	CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
	if (pWndGuild)
		pWndGuild->m_WndGuildTabInfo.UpdateData();
}

void CDPClient::OnGuildAuthority(CAr& ar)
{
	DWORD dwAuthority[MAX_GM_LEVEL];

	ar.Read(dwAuthority, sizeof(dwAuthority));

	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild)
	{
		memcpy(pGuild->m_adwPower, dwAuthority, sizeof(pGuild->m_adwPower));

		CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
		if (pWndGuild)
			pWndGuild->m_WndGuildTabApp.UpdateData();

		g_WndMng.PutString(prj.GetText(TID_GAME_GUILDCHGWARRANT), NULL, prj.GetTextColor(TID_GAME_GUILDCHGWARRANT));
	}
}



void CDPClient::OnGuildSetName(CAr& ar)
{
	u_long idGuild;
	char szName[MAX_G_NAME];
	ar >> idGuild;
	ar.ReadString(szName, MAX_G_NAME);

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		CString str;

		str.Format(prj.GetText(TID_GAME_GUILDNAMECHG), pGuild->m_szGuild, szName);

		g_GuildMng.SetName(pGuild, szName);

		g_WndMng.PutString(str);

		CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase(APP_GUILD);
		if (pWndGuild)
			pWndGuild->m_WndGuildTabInfo.UpdateData();
	}
}


void CDPClient::SendQueryPlayerData(u_long idPlayer, int nVer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERY_PLAYER_DATA, DPID_UNKNOWN);
	ar << idPlayer << nVer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendQueryPlayerData(const vector<PDVer>& vecPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERY_PLAYER_DATA2, DPID_UNKNOWN);
	ar << vecPlayer.size();
	for (int i = 0; i < (int)(vecPlayer.size()); i++)
		ar.Write(&vecPlayer[i], sizeof(PDVer));
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPClient::OnCmdSetSkillLevel(CAr& ar)
{
	DWORD dwSkill;
	DWORD dwSkillLevel;
	ar >> dwSkill >> dwSkillLevel;

	LPSKILL pSkill = g_pPlayer->GetSkill(dwSkill);
	if (pSkill)
	{
		pSkill->dwLevel = dwSkillLevel;
		ItemProp* pSkillProp = prj.GetSkillProp(pSkill->dwSkill);
		if (pSkillProp)
		{
			CString str;
			str.Format(prj.GetText(TID_GAME_SKILLLEVELUP), pSkillProp->szName, pSkill->dwLevel);
			g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_REAPSKILL));
		}

		g_pPlayer->PutLvUpSkillName_2(dwSkill);


		CWndSkill_16* pSkillTree = (CWndSkill_16*)g_WndMng.GetWndBase(APP_SKILL4);
		if (pSkillTree)
			pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill);

		CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
		if (pWndQuestDetail)
			pWndQuestDetail->UpdateQuestText();

		CWndQuestQuickInfo* pWndQuestQuickInfo = g_WndMng.m_pWndQuestQuickInfo;
		if (pWndQuestQuickInfo)
			pWndQuestQuickInfo->SetUpdateTextSwitch(TRUE);

	}
}

void CDPClient::OnQueryPlayerData(CAr& ar)
{
	u_long idPlayer;
	ar >> idPlayer;
	PlayerData pd;
	ar.ReadString(pd.szPlayer, MAX_PLAYER);
	ar.Read(&pd.data, sizeof(sPlayerData));
	PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPlayer, FALSE);

	if (pPlayerData->data.nVer < pd.data.nVer)
	{
		CPlayerDataCenter::GetInstance()->AddPlayerData(idPlayer, pd);
	}

	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetWndBase(APP_MESSENGER_);
	if (pWndMessengerEx)
	{
		pWndMessengerEx->UpdateFriendList();
		pWndMessengerEx->UpdateGuildMemberList();
	}

	UpdateGuildWnd();

}

void CDPClient::OnDisguise(OBJID objid, CAr& ar)
{
	DWORD dwMoverIdx;
	ar >> dwMoverIdx;

	CMover* pMover = prj.GetMover(objid);
	if (IsInvalidObj(pMover))
		return;

	if (pMover)
	{
		pMover->Disguise(D3DDEVICE, dwMoverIdx);
		CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
		if (pWndQuestDetail)
			pWndQuestDetail->UpdateQuestText();

		CWndQuestQuickInfo* pWndQuestQuickInfo = g_WndMng.m_pWndQuestQuickInfo;
		if (pWndQuestQuickInfo)
			pWndQuestQuickInfo->SetUpdateTextSwitch(TRUE);
	}
}
void CDPClient::OnNoDisguise(OBJID objid, CAr& ar)
{
	CMover* pMover = prj.GetMover(objid);
	if (IsInvalidObj(pMover))
		return;

	if (pMover)
	{
		pMover->NoDisguise(D3DDEVICE);
		CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
		if (pWndQuestDetail)
			pWndQuestDetail->UpdateQuestText();

		CWndQuestQuickInfo* pWndQuestQuickInfo = g_WndMng.m_pWndQuestQuickInfo;
		if (pWndQuestQuickInfo)
			pWndQuestQuickInfo->SetUpdateTextSwitch(TRUE);
	}
}

#ifndef __FL_VENDORS
void CDPClient::SendRegisterPVendorItem(BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REGISTER_PVENDOR_ITEM, DPID_UNKNOWN);
	ar << iIndex << nType << nId << nNum << nCost;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendUnregisterPVendorItem(BYTE iIndex)
{
	BEFORESENDSOLE(ar, PACKETTYPE_UNREGISTER_PVENDOR_ITEM, DPID_UNKNOWN);
	ar << iIndex;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnRegisterPVendorItem(OBJID objid, CAr& ar)
{
	BYTE iIndex, nType, nId;
	short nNum;
	int nCost;

	ar >> iIndex >> nType >> nId >> nNum >> nCost;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover) && pMover->IsActiveMover())
	{
		pMover->m_vtInfo.VendorSetItem(nId, iIndex, nNum, nCost);
	}
}
void CDPClient::SendPVendorOpen(const char* szVendor)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PVENDOR_OPEN, DPID_UNKNOWN);
	ar.WriteString(szVendor);
	SEND(ar, this, DPID_SERVERPLAYER);
}
#else 
void CDPClient::SendRegisterPVendorItem(BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost, int nPerinCost, BOOL bOwnShop)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REGISTER_PVENDOR_ITEM, DPID_UNKNOWN);
	ar << iIndex << nType << nId << nNum << nCost << nPerinCost;
	ar << bOwnShop;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendUnregisterPVendorItem(BYTE iIndex, BOOL bOwnShop)
{
	BEFORESENDSOLE(ar, PACKETTYPE_UNREGISTER_PVENDOR_ITEM, DPID_UNKNOWN);
	ar << iIndex;
	ar << bOwnShop;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnRegisterPVendorItem(OBJID objid, CAr& ar)
{
	BYTE iIndex, nType, nId;
	short nNum;
	int nCost;
	int nPerinCost;

	ar >> iIndex >> nType >> nId >> nNum >> nCost >> nPerinCost;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover) && pMover->IsActiveMover())
		pMover->m_vtInfo.VendorSetItem(nId, iIndex, nNum, nCost, nPerinCost);
}
void CDPClient::SendPVendorOpen(const char* szVendor, BOOL bOwnShop, DWORD dwObjId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PVENDOR_OPEN, DPID_UNKNOWN);
	ar << bOwnShop;
	ar << dwObjId;
	ar.WriteString(szVendor);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnVendorActivate(CAr& ar)
{
	DWORD dwVendorTime;
	DWORD dwVendorItem;

	ar >> dwVendorTime;
	ar >> dwVendorItem;

	g_pPlayer->m_buffs.AddBuff(BUFF_ITEM, (WORD)dwVendorItem, 1, dwVendorTime * 1000);
}
void CDPClient::OnVendorDeactivate(CAr& ar)
{
	g_pPlayer->RemoveIk3Buffs(IK3_VENDOR);
}
void CDPClient::SendVendorAction(BYTE nType)
{
	BEFORESENDSOLE(ar, PACKETTYPE_VENDOR_ACTION, DPID_UNKNOWN);
	ar << nType;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnVendorPreview(CAr& ar)
{
	CItemElem* apItemVd[MAX_VENDITEM] = { NULL, };
	BYTE nVendorItem, iIndex;
	int nExtra;
	char szPVendor[MAX_VENDORNAME] = { 0, };

	ar >> nVendorItem;
	for (int i = 0; i < nVendorItem; i++)
	{
		CItemElem* pItemElem = new CItemElem;
		ar >> iIndex;
		pItemElem->Serialize(ar);
		ar >> nExtra;
		ar >> pItemElem->m_nCost;
		ar >> pItemElem->m_nPerinCost;

		pItemElem->SetExtra(nExtra);
		pItemElem->SetTexture();
		apItemVd[iIndex] = pItemElem;
	}
	ar.ReadString(szPVendor, MAX_VENDORNAME);

	CWndVendorPreview* pWndVendor = (CWndVendorPreview*)g_WndMng.CreateApplet(APP_VENDOR_PREVIEW);
	if (!pWndVendor)
	{
		for (int i = 0; i < MAX_VENDITEM; i++)
			SAFE_DELETE(apItemVd[i]);
		return;
	}

	pWndVendor->m_pMover.m_vtInfo.VendorClose(FALSE);
	pWndVendor->m_pMover.m_vtInfo.VendorCopyItems(apItemVd);
	pWndVendor->SetTitle(szPVendor);
}
void CDPClient::OnVendorAddNewItem(OBJID objid, CAr& ar)
{
	BYTE iIndex;
	int nExtra;
	CItemElem* pItemElem = new CItemElem;

	ar >> iIndex;
	pItemElem->Serialize(ar);
	ar >> nExtra;
	ar >> pItemElem->m_nCost;
	ar >> pItemElem->m_nPerinCost;

	pItemElem->SetExtra(nExtra);
	pItemElem->SetTexture();

	CMover* pMover = prj.GetMover(objid);
	if (!IsValidObj(pMover))
	{
		SAFE_DELETE(pItemElem);
		return;
	}

	CWndVendor* pWndVendor = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);
	if (!pWndVendor)
	{
		SAFE_DELETE(pItemElem);
		return;
	}

	SAFE_DELETE(pWndVendor->m_pWndVendorBuy);
	pMover->m_vtInfo.SetItem(iIndex, pItemElem);
	g_WndMng.PutString("Item list was just updated by owner.");
}
#endif

void CDPClient::SendPVendorClose(OBJID objidVendor)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PVENDOR_CLOSE, DPID_UNKNOWN);
	ar << objidVendor;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendQueryPVendorItem(OBJID objidVendor)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERY_PVENDOR_ITEM, DPID_UNKNOWN);
	ar << objidVendor;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendBuyPVendorItem(OBJID objidVendor, BYTE nItem, short nNum)
{
	BEFORESENDSOLE(ar, PACKETTYPE_BUY_PVENDOR_ITEM, DPID_UNKNOWN);
	ar << objidVendor << nItem << nNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnPVendorOpen(OBJID objid, CAr& ar)
{
	char szPVendor[MAX_VENDORNAME] = { 0, };
	ar.ReadString(szPVendor, MAX_VENDORNAME);

	CMover* pMover = prj.GetMover(objid);
	if (!IsValidObj(pMover))
		return;

	pMover->m_vtInfo.SetTitle(szPVendor);
	g_DialogMsg.AddVendorMessage(pMover, pMover->m_vtInfo.GetTitle(), 0xffffffff);

	CWndVendor* pWnd = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);
	if (pWnd == NULL)
		return;

	if (pMover->IsActiveMover())
	{
		CWndEdit* pWndEdit = (CWndEdit*)pWnd->GetDlgItem(WIDC_EDIT1);
		pWndEdit->EnableWindow(FALSE);

		SendEnterChattingRoom(pMover->m_idPlayer);

		if (pWnd->m_pwndVenderMessage == NULL)
		{
			g_Chatting.m_bState = TRUE;

			pWnd->m_pwndVenderMessage = new CWndVendorMessage;

			if (pWnd->m_pVendor->IsActiveMover())
				pWnd->m_pwndVenderMessage->m_nIsOwner = TRUE;

			pWnd->m_pwndVenderMessage->Initialize(pWnd);
			pWnd->SetFocus();
		}
	}
}

void CDPClient::OnPVendorClose(OBJID objid, CAr& ar)
{
	BYTE byClearTitle;
	ar >> byClearTitle;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		if (pMover == CMover::GetActiveMover()->m_vtInfo.GetOther())
		{
			CMover::GetActiveMover()->m_vtInfo.SetOther(NULL);
			CWndVendor* pWndVendor = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);
			if (pWndVendor)
			{
				if (pWndVendor == CWndBase::m_GlobalShortcut.m_pFromWnd->GetFrameWnd())
					CWndBase::m_GlobalShortcut.Empty();
				pWndVendor->Destroy();
			}
		}

		BOOL bReSetVendor = FALSE;
		CString strVendorTitle;
		if (pMover == g_pPlayer)
		{
			if (pMover->m_vtInfo.IsVendorOpen())
			{
				bReSetVendor = TRUE;
				strVendorTitle = pMover->m_vtInfo.GetTitle();
			}
		}

		pMover->m_vtInfo.VendorClose(byClearTitle);
		if (byClearTitle)
			g_DialogMsg.RemoveVendorMessage(pMover);

		if (bReSetVendor)
		{
			CWndVendor* pWndVendor = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);
			if (!pWndVendor)
			{
				g_WndMng.CreateApplet(APP_VENDOR_REVISION);

				pWndVendor = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);
				if (pWndVendor)
				{
					CWndEdit* pWndEdit = (CWndEdit*)pWndVendor->GetDlgItem(WIDC_EDIT1);
					if (pWndEdit)
					{
						for (int j = 0; j < strVendorTitle.GetLength(); j++)
						{
							if (strVendorTitle.GetAt(j) == '\n')
								strVendorTitle.SetAt(j, ' ');
						}
						pWndEdit->SetString(strVendorTitle);
					}
				}
			}
		}
	}
}

void CDPClient::OnUnregisterPVendorItem(OBJID objid, CAr& ar)
{
	BYTE iIndex;
	ar >> iIndex;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover) && pMover->IsActiveMover())
	{
		pMover->m_vtInfo.VendorClearItem(iIndex);
	}
	g_Neuz.m_aSavedInven[iIndex].m_dwObjId = 0;
	g_Neuz.m_aSavedInven[iIndex].m_nCost = 0;
#ifdef __FL_VENDORS
	g_Neuz.m_aSavedInven[iIndex].m_nPerinCost = 0;
#endif
	g_Neuz.m_aSavedInven[iIndex].m_nExtra = 0;
	g_Neuz.m_aSavedInven[iIndex].m_dwItemId = 0;
}

void CDPClient::OnPVendorItemNum(OBJID objid, CAr& ar)
{
	BYTE nItem;
	short nVend;
	ar >> nItem >> nVend;
	char sBuyer[MAX_PLAYER] = { 0, };
	ar.ReadString(sBuyer, MAX_PLAYER);

	CMover* pPVendor = prj.GetMover(objid);
	if (IsValidObj((CObj*)pPVendor))
	{
		pPVendor->m_vtInfo.VendorItemNum(nItem, nVend);

		if (pPVendor->IsActiveMover())
		{
			g_Neuz.m_aSavedInven[nItem].m_dwObjId = 0;
			g_Neuz.m_aSavedInven[nItem].m_nCost = 0;
#ifdef __FL_VENDORS
			g_Neuz.m_aSavedInven[nItem].m_nPerinCost = 0;
#endif
			g_Neuz.m_aSavedInven[nItem].m_nExtra = 0;
			g_Neuz.m_aSavedInven[nItem].m_dwItemId = 0;
		}
	}
}

void CDPClient::OnPVendorItem(OBJID objid, CAr& ar)
{
	CItemElem* apItemVd[MAX_VENDITEM] = { NULL, };
	BYTE nVendorItem, iIndex;
	int nExtra;

	ar >> nVendorItem;
	for (int i = 0; i < nVendorItem; i++)
	{
		CItemElem* pItemElem = new CItemElem;

		ar >> iIndex;
		pItemElem->Serialize(ar);
		ar >> nExtra;
		ar >> pItemElem->m_nCost;
#ifdef __FL_VENDORS
		ar >> pItemElem->m_nPerinCost;
#endif

		pItemElem->SetExtra(nExtra);
		pItemElem->SetTexture();
		apItemVd[iIndex] = pItemElem;
	}
	ar >> g_Chatting.m_bState;

	CMover* pPVendor = prj.GetMover(objid);
#ifndef __FL_VENDORS
	if (IsValidObj((CObj*)pPVendor) && pPVendor->IsPlayer())
#else
	if (IsValidObj((CObj*)pPVendor))
#endif
	{
		CMover::GetActiveMover()->m_vtInfo.SetOther(pPVendor);
		pPVendor->m_vtInfo.VendorCopyItems(apItemVd);

		CWndVendor* pWndVendor = (CWndVendor*)g_WndMng.CreateApplet(APP_VENDOR_REVISION);
		if (pWndVendor)
		{
			SendEnterChattingRoom(pPVendor->m_idPlayer);

			if (g_Chatting.m_bState)
			{
				SAFE_DELETE(pWndVendor->m_pwndVenderMessage);
				pWndVendor->m_pwndVenderMessage = new CWndVendorMessage;
				pWndVendor->m_pwndVenderMessage->Initialize(pWndVendor);
			}

			pWndVendor->SetVendor(pPVendor);
		}
	}
	else
	{
		for (int i = 0; i < MAX_VENDITEM; i++)
			SAFE_DELETE(apItemVd[i]);
	}
}

void CDPClient::OnAddFriendNameReqest(CAr& ar)
{
	u_long uLeader, uMember;
	LONG nLeaderJob;
	BYTE nLeaderSex;
	char szLeaderName[64] = { 0, };
	ar >> uLeader >> uMember;
	ar >> nLeaderJob >> nLeaderSex;
	ar.ReadString(szLeaderName, 64);

	if (g_pPlayer == NULL)
		return;

#ifdef __S_SERVER_UNIFY
	if (g_WndMng.m_bAllAction == FALSE)
		return;
#endif // __S_SERVER_UNIFY

	if (g_Option.m_bMessenger == FALSE)
	{
		SendBlock(2, szLeaderName);
		return;
	}

	if (g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
		return;

	SAFE_DELETE(g_WndMng.m_pWndFriendConfirm);
	g_WndMng.m_pWndFriendConfirm = new CWndFriendConFirm;
	g_WndMng.m_pWndFriendConfirm->Initialize();
	g_WndMng.m_pWndFriendConfirm->SetMember(uLeader, g_pPlayer->m_idPlayer, nLeaderJob, nLeaderSex, szLeaderName);
}

void CDPClient::SendSetHair(BYTE nHair, float r, float g, float b)//, int nCost )
{
	if (g_pPlayer->HasBuffByIk3(IK3_TEXT_DISGUISE))
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_ERROR_BEAUTYSHOP_DISGUISE), NULL, prj.GetTextColor(TID_GAME_ERROR_BEAUTYSHOP_DISGUISE));
	}
	else
	{
		BYTE nR, nG, nB;
		nR = (BYTE)(r * 255.0f);
		nG = (BYTE)(g * 255.0f);
		nB = (BYTE)(b * 255.0f);

		BEFORESENDSOLE(ar, PACKETTYPE_SET_HAIR, DPID_UNKNOWN);
#ifdef __NEWYEARDAY_EVENT_COUPON
		BOOL bUseCoupon = FALSE;
		if (g_WndMng.m_pWndBeautyShop != NULL)
		{
			bUseCoupon = g_WndMng.m_pWndBeautyShop->m_bUseCoupon;
		}
		ar << nHair << nR << nG << nB << bUseCoupon;
#else //__NEWYEARDAY_EVENT_COUPON
		ar << nHair << nR << nG << nB;	// << nCost;
#endif //__NEWYEARDAY_EVENT_COUPON
		SEND(ar, this, DPID_SERVERPLAYER);
	}
}

void CDPClient::OnSetHair(OBJID objid, CAr& ar)
{
	BYTE nHair, nR, nG, nB;
	ar >> nHair >> nR >> nG >> nB;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover) && pMover->IsPlayer())
	{
		float r, g, b;
		r = (float)nR / 255.0f;
		g = (float)nG / 255.0f;
		b = (float)nB / 255.0f;

		pMover->SetHair(nHair);
		pMover->SetHairColor(r, g, b);

		if (g_pPlayer && g_pPlayer == pMover)
			g_WndMng.UpdateWindowModels();
	}
}

void CDPClient::OnSetSkillState(CAr& ar)
{
	WORD wType, wID;
	DWORD dwLevel, dwTime;
	OBJID	idTarget;
	ar >> idTarget >> wType >> wID;
	if (wType == BUFF_ITEM2)
	{
		time_t t;
		ar >> t;
		dwLevel = time_null() + t;
	}
	else
		ar >> dwLevel;
	ar >> dwTime;

	CMover* pTarget = prj.GetMover(idTarget);
	if (IsValidObj(pTarget))
		pTarget->AddBuff(wType, wID, dwLevel, dwTime);
}


void CDPClient::OnRunScriptFunc(OBJID objid, CAr& ar)
{
	CWndDialog* pWndDialog = (CWndDialog*)g_WndMng.GetWndBase(APP_DIALOG_EX);
	CMover* pMover = prj.GetMover(objid);

	RunScriptFunc	rsf;
	WORD wFuncType;
	ar >> wFuncType;
	switch (wFuncType)
	{
	case FUNCTYPE_ADDKEY:
	{
		ar.ReadString(rsf.lpszVal1, 1024);
		ar.ReadString(rsf.lpszVal2, 1024);
		ar >> rsf.dwVal1;
		ar >> rsf.dwVal2;
		if (pWndDialog)
			pWndDialog->AddKeyButton(rsf.lpszVal1, rsf.lpszVal2, rsf.dwVal1, rsf.dwVal2);
		break;
	}
	case FUNCTYPE_REMOVEKEY:
	{
		ar.ReadString(rsf.lpszVal1, 1024);
		if (pWndDialog)
			pWndDialog->RemoveKeyButton(rsf.lpszVal1);
		break;
	}
	case FUNCTYPE_REMOVEALLKEY:
	{
		if (pWndDialog)
			pWndDialog->RemoveAllKeyButton();
		break;
	}
	case FUNCTYPE_ADDANSWER:
	{
		ar.ReadString(rsf.lpszVal1, 1024);
		ar.ReadString(rsf.lpszVal2, 1024);
		ar >> rsf.dwVal1;
		ar >> rsf.dwVal2;
		if (pWndDialog)
			pWndDialog->AddAnswerButton(rsf.lpszVal1, rsf.lpszVal2, rsf.dwVal1, rsf.dwVal2);
		break;
	}
	case FUNCTYPE_SAY:
	{
		ar.ReadString(rsf.lpszVal1, 1024);
		ar >> rsf.dwVal2; // quest
		if (pWndDialog)
		{
			pWndDialog->m_bSay = TRUE;
			pWndDialog->Say(rsf.lpszVal1, rsf.dwVal2);
		}
		break;
	}

	case FUNCTYPE_NEWQUEST:
	{
		ar.ReadString(rsf.lpszVal1, 1024);
		ar.ReadString(rsf.lpszVal2, 1024);
		ar >> rsf.dwVal1;
		ar >> rsf.dwVal2;
		if (pWndDialog)
			pWndDialog->AddNewQuestList(rsf.lpszVal1, rsf.lpszVal2, rsf.dwVal1, rsf.dwVal2);
		break;
	}
	case FUNCTYPE_CURRQUEST:
	{
		ar.ReadString(rsf.lpszVal1, 1024);
		ar.ReadString(rsf.lpszVal2, 1024);
		ar >> rsf.dwVal1;
		ar >> rsf.dwVal2;
		if (pWndDialog)
			pWndDialog->AddCurrentQuestList(rsf.lpszVal1, rsf.lpszVal2, rsf.dwVal1, rsf.dwVal2);
		break;
	}

	case FUNCTYPE_SETMARK:
	case FUNCTYPE_GOMARK:
	{
		if (pWndDialog)
			pWndDialog->MakeKeyButton();
		break;
	}
	case FUNCTYPE_EXIT:
	{
		if (pWndDialog)
			pWndDialog->Destroy();
		break;
	}
	case FUNCTYPE_CREATEGUILD:
	{
		break;
	}
	case FUNCTYPE_DESTROYGUILD:
	{
		break;
	}
	case FUNCTYPE_ENDSAY:
		break;
	case FUNCTYPE_SETNAVIGATOR:
	{
		ar >> rsf.dwVal1;
		ar >> rsf.vPos;
		if (g_WndMng.m_pWndWorld)
		{
			g_WndMng.m_pWndWorld->m_stnv.dwWorldId = rsf.dwVal1;
			g_WndMng.m_pWndWorld->m_stnv.vPos = rsf.vPos;
		}
		break;
	}
	case FUNCTYPE_REMOVENAVIGATOR:
	{
		if (g_WndMng.m_pWndWorld)
		{
			g_WndMng.m_pWndWorld->m_stnv.dwWorldId = 0;
			//					g_WndMng.m_pWndWorld->m_stnv.vPos	= D3DXVECTOR3( 0, 0, 0 );
		}
		break;
	}
	case FUNCTYPE_QUERYSETPLAYERNAME:
	{
		if (NULL == g_WndMng.m_pWndChangeName)
		{
			g_WndMng.m_pWndChangeName = new CWndChangeName;
			g_WndMng.m_pWndChangeName->Initialize(&g_WndMng, 0);
		}
		g_WndMng.m_pWndChangeName->SetData(0xffff, 1);
		break;
	}

	case FUNCTYPE_QUERYSETGUILDNAME:
	{
		if (!g_WndMng.m_pWndGuildName)
		{
			g_WndMng.m_pWndGuildName = new CWndGuildName;
			g_WndMng.m_pWndGuildName->Initialize(&g_WndMng);
		}
		g_WndMng.m_pWndGuildName->SetData(0x7f);
		break;
	}
	default:
		break;
	}
}

void CDPClient::PostPlayerAngle(BOOL f)
{
	m_pa.fValid = f;
}

void CDPClient::FlushPlayerAngle(void)
{
	if (m_pa.fValid && (++m_pa.nCounter % 30) == 0)
	{
		m_pa.fValid = FALSE;
		CMover* pPlayer = CMover::GetActiveMover();
		if (pPlayer)
		{
			if (FALSE == pPlayer->m_pActMover->IsFly())
				return;

			SendPlayerBehavior();
		}
	}
}

void CDPClient::OnRemoveQuest(CAr& ar)
{
	int nRemoveType;
	DWORD dwQuestCancelID;
	ar >> nRemoveType >> dwQuestCancelID;

	if (nRemoveType == -1)
	{
		QuestProp* pQuestPorp = prj.m_aPropQuest.GetAt(dwQuestCancelID);

		CString str;
		str.Format(prj.GetText(TID_GAME_REMOVEQUEST), pQuestPorp->m_szTitle);
		g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_REMOVEQUEST));

		g_pPlayer->RemoveQuest(dwQuestCancelID);
		g_QuestTreeInfoManager.DeleteTreeInformation(dwQuestCancelID);
	}
	else
		if (nRemoveType == 0)
		{
			g_pPlayer->RemoveQuest(dwQuestCancelID);
			g_QuestTreeInfoManager.DeleteTreeInformation(dwQuestCancelID);
		}
		else
			if (nRemoveType == 1)
			{
				g_pPlayer->RemoveAllQuest();
				g_QuestTreeInfoManager.DeleteAllTreeInformation();
			}
			else
				if (nRemoveType == 2)
					g_pPlayer->RemoveCompleteQuest();

	CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetApplet(APP_QUEST_EX_LIST);
	if (pWndQuest)
	{
		pWndQuest->Update();
	}
}


void CDPClient::OnInitSkillPoint(CAr& ar)
{
	int nSkillPoint;
	ar >> nSkillPoint;
	if (g_pPlayer->m_nSkillPoint < nSkillPoint)
	{
		CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_LEVEL_UP01, g_pPlayer->GetPos(), g_pPlayer->GetId());
		PlayMusic(BGM_IN_LEVELUP);
	}
	g_pPlayer->m_nSkillPoint = nSkillPoint;

	for (int i = 0; i < MAX_SKILL_JOB; ++i)
	{
		LPSKILL pSkill = (LPSKILL)&g_pPlayer->m_aJobSkill[i];
		if (pSkill != NULL)
		{
			if (pSkill->dwLevel != 0)
			{
				ItemProp* pSkillProp = prj.GetSkillProp(pSkill->dwSkill);
				if (pSkillProp == NULL)
					return;

				if (pSkillProp->dwItemKind1 != JTYPE_MASTER && pSkillProp->dwItemKind1 != JTYPE_HERO)
					pSkill->dwLevel = 0;
			}
		}
	}

	CWndSkill_16* pSkillTree = (CWndSkill_16*)g_WndMng.GetWndBase(APP_SKILL4);
	if (pSkillTree)
		pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill);

	g_WndMng.PutString(prj.GetText(TID_GAME_RECCURENCE), NULL, prj.GetTextColor(TID_GAME_RECCURENCE));
}

void CDPClient::OnDoUseSkillPoint(CAr& ar)
{
	DWORD dwSkill, dwLevel;
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		ar >> dwSkill >> dwLevel;
		LPSKILL pSkill = g_pPlayer->GetSkill(dwSkill);
		if (pSkill)
			pSkill->dwLevel = dwLevel;
	}
	ar >> g_pPlayer->m_nSkillPoint;

	CWndSkill_16* pSkillTree = (CWndSkill_16*)g_WndMng.GetWndBase(APP_SKILL4);
	if (pSkillTree)
		pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill);
}

void CDPClient::SendRandomScroll(OBJID objid, OBJID objid2)
{
	BEFORESENDSOLE(ar, PACKETTYPE_RANDOMSCROLL, DPID_UNKNOWN);
	ar << objid;
	ar << objid2;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendEnchant(OBJID objid, OBJID objMaterialId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ENCHANT, DPID_UNKNOWN);
	ar << objid;
	ar << objMaterialId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendRemoveAttribute(OBJID objid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REMVOE_ATTRIBUTE, DPID_UNKNOWN);
	ar << objid;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendPiercing(OBJID objid1, OBJID objid2)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PIERCING, DPID_UNKNOWN);
	ar << objid1;
	ar << objid2;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendPiercingRemove(OBJID objid, int nSlot)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PIERCINGREMOVE, DPID_UNKNOWN);
	ar << objid;
	ar << nSlot;
	SEND(ar, this, DPID_SERVERPLAYER);
}

#ifdef __SYS_ITEMTRANSY
void CDPClient::SendItemTransy(OBJID objid0, OBJID objid1, DWORD dwChangeId, BOOL bCash)
#else // 
void CDPClient::SendItemTransy(OBJID objid0, OBJID objid1)
#endif // 
{
	BEFORESENDSOLE(ar, PACHETTYPE_ITEMTRANSY, DPID_UNKNOWN);
	ar << objid0;
	ar << objid1;
#ifdef __SYS_ITEMTRANSY
	ar << dwChangeId;
	ar << bCash;
#endif // __SYS_ITEMTRANSY
	SEND(ar, this, DPID_SERVERPLAYER);
}


void	CDPClient::OnCreateSkillEffect(OBJID objid, CAr& ar)
{
	OBJID idTarget;
	DWORD dwSkill, dwLevel;
	ar >> idTarget >> dwSkill >> dwLevel;

	CMover* pAttacker = prj.GetMover(objid);
	if (IsValidObj(pAttacker))
	{
		//
	}
}


void	CDPClient::OnSetStun(OBJID objid, CAr& ar)
{
	BOOL bTrue;
	ar >> bTrue;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
		pMover->SetStun(bTrue);
}


void	CDPClient::OnSendActMsg(OBJID objid, CAr& ar)
{
	int nParam1, nParam2, nParam3;
	OBJMSG dwMsg;
	BOOL	bFly;
	ar >> nParam1;	dwMsg = (OBJMSG)nParam1;
	ar >> bFly;
	ar >> nParam1 >> nParam2 >> nParam3;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		if (bFly == FALSE)
			pMover->m_pActMover->ProcessActMsg1(pMover, dwMsg, nParam1, nParam2, nParam3, 1, 0);
		else
			pMover->m_pActMover->ProcessActMsg2(pMover, dwMsg, nParam1, nParam2, nParam3, 1, 0);
	}
}

void CDPClient::OnSexChange(OBJID objid, CAr& ar)
{
	u_short uIndex;
	ar >> uIndex;
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		pMover->SetSex((uIndex == MI_MALE) ? SEX_MALE : SEX_FEMALE);
		if (pMover->m_pModel && pMover->m_pModel->IsAniable())
			SAFE_DELETE(pMover->m_pModel);
		pMover->SetTypeIndex(D3DDEVICE, OT_MOVER, (DWORD)uIndex);
		pMover->ResetScale();
		pMover->UpdateLocalMatrix();
		pMover->m_dwMotion = -1;
		pMover->SetMotion(MTI_STAND);
		pMover->UpdateParts(!pMover->IsActiveMover());

		if (pMover->IsActiveMover())
		{
			g_WndMng.UpdateWindowModels();
#ifdef __FL_SWITCH_EQ
			pMover->m_Switches.ClearAllSwitches();
#endif
		}
	}
}

void CDPClient::OnRemoveSkillInfluence(OBJID objid, CAr& ar)
{
	WORD	wType, wID;
	ar >> wType >> wID;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		pMover->RemoveBuff(wType, wID);
	}
}

void CDPClient::OnRemoveAllSkillInfluence(OBJID objid, CAr& ar)
{
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
		pMover->RemoveCommonBuffs();
}


void CDPClient::OnSetScale(OBJID objid, CAr& ar)
{
	float fScalePerscale;
	ar >> fScalePerscale;

	CMover* pMover = prj.GetMover(objid);
	if (TRUE == IsValidObj(pMover))
	{
		FLOAT fScale = pMover->m_pModel->m_pModelElem->m_fScale;
		fScale *= fScalePerscale;
		pMover->m_fDestScale = fScale;
		pMover->m_fDestScaleSlerp = 1.0f;
	}
}

void CDPClient::SendEnterChattingRoom(u_long uidChatting)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ENTERCHTTING, DPID_UNKNOWN);
	ar << uidChatting;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendChatting(char* pszChat)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CHATTING, DPID_UNKNOWN);
	ar.WriteString(pszChat);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendOpenChattingRoom()
{
	BEFORESENDSOLE(ar, PACKETTYPE_OPENCHATTINGROOM, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendCloseChattingRoom()
{
	BEFORESENDSOLE(ar, PACKETTYPE_CLOSECHATTINGROOM, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnChatting(OBJID objid, CAr& ar)
{
	BYTE nType;
	ar >> nType;

	u_long uidPlayer;
	TCHAR szName[MAX_NAME] = { 0, };

	switch (nType)
	{
	case CHATTING_NEWCHATTING:
	{
		g_Chatting.m_nSizeofMember = 0;
		int nSize;
		ar >> nSize;
		for (int i = 0; i < nSize; ++i)
		{
			ar >> uidPlayer;
			ar.ReadString(szName, MAX_NAME);
			g_Chatting.AddChattingMember(uidPlayer, szName);
		}
	}
	break;
	case CHATTING_ENTERMEMBER:
	{
		ar >> uidPlayer;
		ar.ReadString(szName, MAX_NAME);
		g_Chatting.AddChattingMember(uidPlayer, szName);

		CWndVendor* pWndVendor = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);

		if (pWndVendor)
		{
			for (int i = 0; i < g_Chatting.GetChattingMember(); ++i)
			{
				if (uidPlayer == g_Chatting.m_idMember[i])
				{
					if (pWndVendor->m_pwndVenderMessage)
						pWndVendor->m_pwndVenderMessage->AddChattingMemver(g_Chatting.m_szName[i]);
				}
			}
		}
	}
	break;
	case CHATTING_DELETECHATTING:
	{
		g_Chatting.m_nSizeofMember = 0;
	}
	break;
	case CHATTING_REMOVEMEMBER:
	{
		ar >> uidPlayer;

		CWndVendor* pWndVendor = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);

		if (pWndVendor)
		{
			for (int i = 0; i < g_Chatting.GetChattingMember(); ++i)
			{
				if (uidPlayer == g_Chatting.m_idMember[i])
				{
					if (pWndVendor->m_pwndVenderMessage)
						pWndVendor->m_pwndVenderMessage->RemoveChattingMemver(g_Chatting.m_szName[i]);
				}
			}
		}

		g_Chatting.RemoveChattingMember(uidPlayer);
	}
	break;
	case CHATTING_CHATTING:
	{
		static	TCHAR	sChat[1024];

		ar >> uidPlayer;
		ar.ReadString(sChat, 1024);

		CWndVendor* pWndVendor = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);

		if (pWndVendor)
		{
			for (int i = 0; i < g_Chatting.GetChattingMember(); ++i)
			{
				if (uidPlayer == g_Chatting.m_idMember[i])
				{
					if (pWndVendor->m_pwndVenderMessage)
						pWndVendor->m_pwndVenderMessage->AddMessage(g_Chatting.m_szName[i], sChat);
				}
			}

		}

	}
	break;
	case CHATTING_CHATTINGROOMSTATE:
	{
		BOOL bState;
		ar >> bState;
		g_Chatting.m_bState = bState;
		if (bState == FALSE)
		{
			CWndVendor* pVendor = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);
			if (pVendor)
			{
				if (pVendor->m_pwndVenderMessage != NULL)
					SAFE_DELETE(pVendor->m_pwndVenderMessage);
			}
		}
	}
	break;
	}
}


void CDPClient::OnCommonPlace(OBJID objid, CAr& ar)
{
	BYTE nType;

	ar >> nType;

	CCommonCtrl* pObj = (CCommonCtrl*)prj.GetCtrl(objid);

	if (pObj != NULL)
	{
		switch (nType)
		{
		case COMMONPLACE_ACTION:
			pObj->SetActionPlay();

			break;
		case COMMONPLACE_ALPHA:
			pObj->m_bAlpha = TRUE;
			break;
		case COMMONPLACE_QUAKE:
		{
			CWorld* pWorld = ((CMover*)pObj)->GetWorld();

			if (pWorld)
			{
				if (pWorld->m_pCamera)
				{
					pWorld->m_pCamera->SetQuake(1400, 1.0f);
				}
			}
		}
		break;
		//		case COMMONPLACE_ERROR:

		//			break;
		}
	}
}

void CDPClient::OnDoApplySkill(OBJID objid, CAr& ar)
{
	OBJID idTarget;
	DWORD dwSkill, dwLevel;
	ar >> idTarget >> dwSkill >> dwLevel;

	CCtrl* pCtrl = prj.GetCtrl(objid);
	if (IsValidObj((CObj*)pCtrl))
	{
		ItemProp* pSkillProp;
		AddSkillProp* pAddSkillProp;

		g_pPlayer->GetSkillProp(&pSkillProp, &pAddSkillProp, dwSkill, dwLevel, "OnDoApplySkill");

		CCtrl* pCtrlTarget = prj.GetCtrl(idTarget);

		if (IsValidObj((CObj*)pCtrlTarget))
		{
			if (pCtrlTarget->GetType() == OT_MOVER)
			{
				pCtrl->DoApplySkill(pCtrlTarget, pSkillProp, pAddSkillProp);
			}
		}
	}
}

void CDPClient::OnCommonSkill(OBJID objid, CAr& ar)
{
	DWORD dwSkill, dwLevel;
	ar >> dwSkill >> dwLevel;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		ItemProp* pSkill = prj.GetSkillProp(dwSkill);

		if (pSkill == NULL)
			return;

		AddSkillProp* pAddSkill = prj.GetAddSkillProp(pSkill->dwSubDefine, dwLevel);

		if (pAddSkill == NULL)
			return;

		g_Flash.SetTime(pAddSkill->dwCircleTime, pAddSkill->dwSkillTime, TRUE);
	}
}


void	CDPClient::OnPushPower(OBJID objid, CAr& ar)
{
	D3DXVECTOR3 vPos;
	FLOAT fPushAngle, fPower;
	ar >> vPos;
	ar >> fPushAngle >> fPower;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		pMover->SetPos(vPos);
		pMover->pushedByAnotherPower(fPushAngle, fPower);
	}
}


void	CDPClient::OnResetBuffSkill(OBJID objid, CAr& ar)
{
	CMover* pPlayer = prj.GetMover(objid);
	if (IsValidObj(pPlayer))
		pPlayer->ClearBuffInst();
}


void CDPClient::OnSetNaviPoint(OBJID objid, CAr& ar)
{
	NaviPoint nv;
	TCHAR szName[64] = { 0, };

	ar >> nv.Pos;
	ar.ReadString(szName, 64);

	nv.Name = szName;
	nv.objid = objid;
	nv.Time = 200;
	int tChk = 0;

	for (int i = 0; i < (int)(g_pPlayer->m_vOtherPoint.size()); i++)
	{
		if (g_pPlayer->m_vOtherPoint[i].objid == nv.objid)
		{
			tChk = 1;
			g_pPlayer->m_vOtherPoint[i] = nv;
			break;
		}
	}

	if (g_pPlayer->m_vOtherPoint.size() < 10 && tChk == 0)
		g_pPlayer->m_vOtherPoint.push_back(nv);
}

void CDPClient::SendSetNaviPoint(const D3DXVECTOR3& Pos, OBJID objidTarget)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SETNAVIPOINT, DPID_UNKNOWN);
	ar << Pos << objidTarget;
	SEND(ar, this, DPID_SERVERPLAYER);
}


// CreateSfx



void CDPClient::SendCreateSfxObj(DWORD dwSfxId, u_long idPlayer, BOOL bFlag)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CREATESFXOBJ, DPID_UNKNOWN);
	ar << dwSfxId;
	ar << idPlayer;
	ar << bFlag;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendReqLeave()
{
	SendHdr(PACKETTYPE_REQ_LEAVE);
}

void CDPClient::SendResurrectionOK()
{
	BEFORESENDSOLE(ar, PACKETTYPE_RESURRECTION_OK, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendResurrectionCancel()
{
	BEFORESENDSOLE(ar, PACKETTYPE_RESURRECTION_CANCEL, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSummonFriend(DWORD dwData, const char* lpszPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SUMMON_FRIEND, DPID_UNKNOWN);
	ar << dwData;
	ar.WriteString(lpszPlayer);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSummonFriendConfirm(OBJID objid, DWORD dwData)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SUMMON_FRIEND_CONFIRM, DPID_UNKNOWN);
	ar << objid << dwData;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSummonFriendCancel(OBJID objid, DWORD dwData)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SUMMON_FRIEND_CANCEL, DPID_UNKNOWN);
	ar << objid << dwData;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSummonParty(DWORD dwData)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SUMMON_PARTY, DPID_UNKNOWN);
	ar << dwData;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSummonPartyConfirm(OBJID objid, DWORD dwData)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SUMMON_PARTY_CONFIRM, DPID_UNKNOWN);
	ar << objid << dwData;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendQuerySetPlayerName(DWORD dwData, const char* lpszPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERYSETPLAYERNAME, DPID_UNKNOWN);
	ar << dwData;
	ar.WriteString(lpszPlayer);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnSetPlayerName(CAr& ar)
{
	u_long idPlayer;
	char lpszPlayer[MAX_PLAYER] = { 0, };
	ar >> idPlayer;
	ar.ReadString(lpszPlayer, MAX_PLAYER);

	char lpOutputString[128] = { 0, };
	sprintf(lpOutputString, "NEUZ.EXE\t// PAKCETTYPE_QUERYSETPLAYERNAME\t// %d\t// %s", idPlayer, lpszPlayer);
	OutputDebugString(lpOutputString);

	const char* pszOld = CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer);
	if (lstrlen(pszOld) > 0)
		g_WndMng.UpdateMessage(pszOld, lpszPlayer);

	CMover* pPlayer = prj.GetUserByID(idPlayer);
	if (IsValidObj((CObj*)pPlayer))
		pPlayer->SetName(lpszPlayer);

	CPlayerDataCenter* pPlayerDataCenter = CPlayerDataCenter::GetInstance();
	PlayerData* pPlayerData = pPlayerDataCenter->GetPlayerData(idPlayer);
	if (pPlayerData)
	{
		PlayerData pd;
		memcpy(&pd, pPlayerData, sizeof(PlayerData));
		pPlayerDataCenter->DeletePlayerData(idPlayer);
		lstrcpy(pd.szPlayer, lpszPlayer);
		pPlayerDataCenter->AddPlayerData(idPlayer, pd);
	}
	// Guild
	UpdateGuildWnd();
	// Messenger

	// Chat
	if (g_Chatting.RemoveChattingMember(idPlayer))
		g_Chatting.AddChattingMember(idPlayer, lpszPlayer);

	CMover* pMover = prj.GetUserByID(idPlayer);
	if (IsValidObj((CObj*)pMover))
	{
		if (pMover->IsActiveMover())
			g_WndMng.PutString(prj.GetText(TID_GAME_CHANGENAME), NULL, prj.GetTextColor(TID_GAME_CHANGENAME));
		CreateSfx(g_Neuz.m_pd3dDevice, XI_SYS_EXCHAN01, pMover->GetPos(), pMover->GetId(), pMover->GetPos(), pMover->GetId());

		if (pMover == g_pPlayer)
		{
			CString strTitle;
			strTitle.Format("%s - %s", pMover->GetName(), __NEUZ_TITLE);
			SetWindowText(CWndBase::m_hWnd, strTitle);

#ifdef __DISCORD // Update when set new name
			g_Neuz.ManageDiscord(DIS_CHARACTER_DETAILS);
#endif
		}

	}
}

void	CDPClient::SendDoEscape(void)
{
	if (g_pPlayer)
	{
		CWorld* pWorld = g_pPlayer->GetWorld();

		if (pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR)
		{
			g_WndMng.OpenMessageBoxUpper(prj.GetText(TID_GAME_GUILDCOMBAT_NO_OUTWAR));
			return;
		}

		SendHdr(PACKETTYPE_DO_ESCAPE);
	}
}


void	CDPClient::OnEscape(OBJID objid, CAr& ar)
{
}


void CDPClient::OnSnoop(CAr& ar)
{
	static char lpszString[2048] = { 0, };
	ar.ReadString(lpszString, 2048);
	g_WndMng.PutString(lpszString, NULL, 0xffffffff, CHATSTY_GENERAL);
}

void CDPClient::SendCheering(OBJID objid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CHEERING, DPID_UNKNOWN);
	ar << objid;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnSetCheerParam(OBJID objid, CAr& ar)
{
	int nCheerPoint;
	DWORD dwRest;
	ar >> nCheerPoint >> dwRest;
	BOOL bAdd;
	ar >> bAdd;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		if (bAdd)	// chipi_080411
			g_WndMng.PutString(prj.GetText(TID_CHEER_MESSAGE5));

		pMover->SetCheerParam(nCheerPoint, GetTickCount(), dwRest);

	}
}

void CDPClient::SendQueryEquip(OBJID objid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERYEQUIP, DPID_UNKNOWN);
	ar << objid;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendQueryEquipSetting(BOOL bAllow)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERYEQUIPSETTING, DPID_UNKNOWN);
	ar << bAllow;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnQueryEquip(OBJID objid, CAr& ar)
{
	CItemElem	aEquipInfoAdd[MAX_HUMAN_PARTS];

	int cbEquip;
	ar >> cbEquip;

	int nParts;
	for (int i = 0; i < cbEquip; i++)
	{
		ar >> nParts;
		aEquipInfoAdd[nParts].Serialize(ar);
	}

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
	{
		SAFE_DELETE(g_WndMng.m_pWndQueryEquip);
		g_WndMng.m_pWndQueryEquip = new CWndQueryEquip;
		g_WndMng.m_pWndQueryEquip->Initialize(&g_WndMng, APP_QUERYEQUIP);
		g_WndMng.m_pWndQueryEquip->SetModel(pMover);
		g_WndMng.m_pWndQueryEquip->SetEquipInfoAdd(aEquipInfoAdd);
	}
}

void CDPClient::SendReturnScroll(int nSelect)
{
	BEFORESENDSOLE(ar, PACKETTYPE_RETURNSCROLL, DPID_UNKNOWN);
	ar << nSelect;
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPClient::OnReturnScrollACK(CAr& ar)
{
	SendDoUseItem(m_dwReturnScroll);
}

void CDPClient::SendQuerySetGuildName(LPCSTR pszGuild, BYTE nId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERYSETGUILDNAME, DPID_UNKNOWN);
	ar << nId;
	ar.WriteString(pszGuild);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnEndSkillQueue(OBJID objid)
{
	CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
	pTaskBar->OnCancelSkill();
}


void	CDPClient::OnSetActionPoint(OBJID objid, CAr& ar)
{
	int nAP;
	ar >> nAP;
	if (IsInvalidObj(g_pPlayer))	return;

	g_WndMng.m_pWndTaskBar->m_nActionPoint = nAP;
}


void	CDPClient::OnSetTarget(OBJID objid, CAr& ar)
{
	OBJID idTarget;
	ar >> idTarget;
	if (IsInvalidObj(g_pPlayer))	return;

	CMover* pMover = prj.GetMover(idTarget);
	if (IsValidObj(pMover))
		g_WorldMng()->SetObjFocus(pMover);
}

void CDPClient::SendEndSkillQueue(void)
{
	SendHdr(PACKETTYPE_ENDSKILLQUEUE);
}

void CDPClient::SendCreateMonster(DWORD dwItemId, D3DXVECTOR3 vPos)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CREATEMONSTER, DPID_UNKNOWN);
	ar << dwItemId;
	ar << vPos;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendQueryPostMail(BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERYPOSTMAIL, DPID_UNKNOWN);
	ar << nItem << nItemNum;
	ar.WriteString(lpszReceiver);
	ar << nGold;
	ar.WriteString(lpszTitle);
	ar.WriteString(lpszText);
	SEND(ar, this, DPID_SERVERPLAYER);

}

void CDPClient::SendQueryRemoveMail(u_long nMail)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERYREMOVEMAIL, DPID_UNKNOWN);
	ar << nMail;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendQueryGetMailItem(u_long nMail)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERYGETMAILITEM, DPID_UNKNOWN);
	ar << nMail;
	SEND(ar, this, DPID_SERVERPLAYER);

}

void CDPClient::SendQueryGetMailGold(u_long nMail)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERYGETMAILGOLD, DPID_UNKNOWN);
	ar << nMail;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendQueryReadMail(u_long nMail)
{
	BEFORESENDSOLE(ar, PACKETTYPE_READMAIL, DPID_UNKNOWN);
	ar << nMail;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendQueryMailBox(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERYMAILBOX, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);

}

void CDPClient::OnPostMail(CAr& ar)
{
	CMail* pMail = new CMail;
	pMail->Serialize(ar);

	u_long nMail = CMailBox::GetInstance()->AddMail(pMail);

}

void CDPClient::OnRemoveMail(CAr& ar)
{
	u_long nMail;
	int nType;
	ar >> nMail >> nType;

	CMailBox* pMailBox = CMailBox::GetInstance();
	switch (nType)
	{
	case CMail::mail:
	{
		BOOL bResult = pMailBox->RemoveMail(nMail);
		break;
	}
	case CMail::item:
	{
		BOOL bResult = pMailBox->RemoveMailItem(nMail);
		break;
	}
	case CMail::gold:
	{
		BOOL bResult = pMailBox->RemoveMailGold(nMail);
		break;
	}
	case CMail::read:
	{
		BOOL bResult = pMailBox->ReadMail(nMail);
		break;
	}
	default:
		break;
	}
}

void CDPClient::OnMailBox(CAr& ar)
{

	CMailBox* pMailBox = CMailBox::GetInstance();
	pMailBox->Serialize(ar);
	CWndPost* pWndPost = (CWndPost*)g_WndMng.GetWndBase(APP_POST);
	if (pWndPost)
#ifdef __MAIL_REQUESTING_BOX
	{
		pWndPost->EnableWindow(TRUE);
		pWndPost->m_PostTabReceive.UpdateScroll();

		if (pWndPost->m_pWndMailRequestingBox)
			pWndPost->m_pWndMailRequestingBox->Destroy();

		//gmpbigsun
		g_WndMng.m_bWaitRequestMail = FALSE;
	}
#else // __MAIL_REQUESTING_BOX
		pWndPost->m_PostTabReceive.UpdateScroll();
#endif // __MAIL_REQUESTING_BOX
	}

void CDPClient::OnMailBoxReq(CAr& ar)
{
	BOOL bCheckTransMailBox = FALSE;
	ar >> bCheckTransMailBox;

	if (bCheckTransMailBox)
	{
#ifdef __MAIL_REQUESTING_BOX

		g_WndMng.m_bWaitRequestMail = TRUE;
#endif // __MAIL_REQUESTING_BOX
	}
	else
	{

#ifdef __MAIL_REQUESTING_BOX

		CWndPost* pWndPost = (CWndPost*)g_WndMng.GetWndBase(APP_POST);
		if (pWndPost)
		{
			pWndPost->EnableWindow(TRUE);
			if (pWndPost->m_pWndMailRequestingBox)
				pWndPost->m_pWndMailRequestingBox->Destroy();
		}

		CMailBox* pMailBox = CMailBox::GetInstance();
		if (pMailBox)
		{
			pMailBox->Clear();
		}


		if (pWndPost)
		{
			pWndPost->m_PostTabReceive.UpdateScroll();
		}


		CString strFileName = _T("");
		if (g_pPlayer == NULL)
		{
			return;
		}
		strFileName.Format("%s_MailData.Temp", g_pPlayer->GetName());
		if (DeleteFile(strFileName) == FALSE)
		{

		}
#endif //__MAIL_REQUESTING_BOX
	}

}

void CDPClient::OnSummon(CAr& ar)
{
	BYTE nState;
	ar >> nState;
	switch (nState)
	{
	case SOMMON_FRIEND:
		OnSummonFriend(ar);
		break;
	case SOMMON_FRIEND_CONFIRM:
		OnSummonFriendConfirm(ar);
		break;
	case SOMMON_PARTY_CONFIRM:
		OnSummonPartyConfirm(ar);
		break;
	}
}
void CDPClient::OnSummonFriend(CAr& ar)
{
	DWORD dwData;
	ar >> dwData;

	CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(dwData);
	if (pItemBase)
	{
		SAFE_DELETE(g_WndMng.m_pWndSummonFriend);
		g_WndMng.m_pWndSummonFriend = new CWndSummonFriend;
		g_WndMng.m_pWndSummonFriend->Initialize(&g_WndMng);
		g_WndMng.m_pWndSummonFriend->SetData((WORD)(pItemBase->m_dwObjId), 0);
	}
}
void CDPClient::OnSummonFriendConfirm(CAr& ar)
{
	OBJID objid;
	DWORD dwData;
	char szName[MAX_NAME] = { 0, };
	char szWorldName[128] = { 0, };
	ar >> objid >> dwData;
	ar.ReadString(szName, MAX_NAME);
	ar.ReadString(szWorldName, 128);

	if (g_pPlayer)
	{
		SAFE_DELETE(g_WndMng.m_pWndSummonFriendMsg);
		g_WndMng.m_pWndSummonFriendMsg = new CWndSummonFriendMsg;
		g_WndMng.m_pWndSummonFriendMsg->Initialize(&g_WndMng);
		g_WndMng.m_pWndSummonFriendMsg->SetData(objid, dwData, szName, szWorldName);
	}
}
void CDPClient::OnSummonPartyConfirm(CAr& ar)
{
	OBJID objid;
	DWORD dwData;
	char szWorldName[256] = { 0, };
	ar >> objid >> dwData;
	ar.ReadString(szWorldName, 256);

	if (g_pPlayer)
	{
		SAFE_DELETE(g_WndMng.m_pWndSummonPartyMsg);
		g_WndMng.m_pWndSummonPartyMsg = new CWndSummonPartyMsg;
		g_WndMng.m_pWndSummonPartyMsg->Initialize(&g_WndMng);
		g_WndMng.m_pWndSummonPartyMsg->SetData(objid, dwData, szWorldName);
	}
}

void CDPClient::OnRemoveGuildBankItem(CAr& ar)
{
	u_long idGuild;
	DWORD dwId;
	DWORD dwItemNum;
	ar >> idGuild >> dwId >> dwItemNum;
	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);

	if (pGuild)
	{
		if (dwId == NULL_ID)
			pGuild->m_nGoldGuild -= dwItemNum;
		else
			pGuild->RemoveItem(dwId, (short)(dwItemNum));
	}
}

void CDPClient::OnMotionArrive(OBJID objid, CAr& ar)
{
	DWORD objmsg;
	ar >> objmsg;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj((CObj*)pMover))
		pMover->m_dwMotionArrive = (OBJMSG)objmsg;
}

#ifdef __EVE_MINIGAME
void CDPClient::SendKawibawiboStart()
{
	BEFORESENDSOLE(ar, PACKETTYPE_KAWIBAWIBO_START, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendKawibawiboGetItem()
{
	BEFORESENDSOLE(ar, PACKETTYPE_KAWIBAWIBO_GETITEM, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __EVE_MINIGAME


#if __VER >= 9 // __ULTIMATE
void CDPClient::SendUltimateMakeItem(OBJID objSunId, OBJID objMoonId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ULTIMATE_MAKEITEM, DPID_UNKNOWN);
	ar << objSunId << objMoonId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendUltimateMakeGem(OBJID objItemId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ULTIMATE_MAKEGEM, DPID_UNKNOWN);
	ar << objItemId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendUltimateTransWeapon(OBJID objItemWeapon, OBJID objItemGem1, OBJID objItemGem2, OBJID objItemScroll)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ULTIMATE_TRANSWEAPON, DPID_UNKNOWN);
	ar << objItemWeapon;
	ar << objItemGem1;
	ar << objItemGem2;
	ar << objItemScroll;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendUltimateSetGem(OBJID objItemWeapon, OBJID objItemGem)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ULTIMATE_SETGEM, DPID_UNKNOWN);
	ar << objItemWeapon;
	ar << objItemGem;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendUltimateRemoveGem(OBJID objItemWeapon, OBJID objItemGem, int nSlot2)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ULTIMATE_REMOVEGEM, DPID_UNKNOWN);
	ar << objItemWeapon;
	ar << objItemGem;
	ar << nSlot2;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendUltimateEnchantWeapon(OBJID objItemWeapon, OBJID objItemGem)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ULTIMATE_ENCHANTWEAPON, DPID_UNKNOWN);
	ar << objItemWeapon;
	ar << objItemGem;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __ULTIMATE

void CDPClient::SendExchange(int nMMIId, int nListNum)
{
	BEFORESENDSOLE(ar, PACKETTYPE_EXCHANGE, DPID_UNKNOWN);
	ar << nMMIId;
	ar << nListNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnPetCall(OBJID objid, CAr& ar)
{
	DWORD dwPetId, dwIndex;
	BYTE nPetLevel;
	ar >> dwPetId >> dwIndex >> nPetLevel;
#ifdef __PET_1024
	char szPetName[MAX_PET_NAME] = { 0, };
	ar.ReadString(szPetName, MAX_PET_NAME);
#endif	// __PET_1024
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		if (pMover->IsActiveMover())
			pMover->SetPetId(dwPetId);
		else
			pMover->SetPetId(MAKELONG((WORD)dwIndex, (WORD)nPetLevel));
#ifdef __PET_1024
		pMover->m_pet.SetName(szPetName);
#endif	//__PET_1024
#ifdef __FL_SWITCH_EQ
		pMover->ProcessPet();
#endif
	}
}

void CDPClient::OnPetRelease(OBJID objid, CAr& ar)
{
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		pMover->PetRelease();

		if (pMover->IsActiveMover())
		{
			CWndPetStatus* pWndPetStatus = (CWndPetStatus*)g_WndMng.GetWndBase(APP_PET_STATUS);
			if (pWndPetStatus != NULL)
				pWndPetStatus->Destroy();

			CWndPetMiracle* pWndPetMiracle = (CWndPetMiracle*)g_WndMng.GetWndBase(APP_PET_MIRACLE);
			if (pWndPetMiracle != NULL)
				pWndPetMiracle->Destroy();

			if (g_WndMng.m_pWndFoodConfirm != NULL)
				g_WndMng.m_pWndFoodConfirm->Destroy();
		}
#ifdef __PET_1024
		pMover->m_pet.SetName("");
#endif	//
	}
}

void CDPClient::SendPetRelease(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PET_RELEASE, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendUsePetFeed(DWORD dwFeedId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_USE_PET_FEED, DPID_UNKNOWN);
	ar << dwFeedId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendMakePetFeed(DWORD dwMaterialId, short nNum, DWORD dwToolId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MAKE_PET_FEED, DPID_UNKNOWN);
	ar << dwMaterialId << dwToolId << nNum;
	SEND(ar, this, DPID_SERVERPLAYER);
}

// dwId : pItemElem->m_dwObjId
void CDPClient::SendPetTamerMistake(DWORD dwId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PET_TAMER_MISTAKE, DPID_UNKNOWN);
	ar << dwId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

// dwId : pItemElem->m_dwObjId
void CDPClient::SendPetTamerMiracle(DWORD dwId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PET_TAMER_MIRACLE, DPID_UNKNOWN);
	ar << dwId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendFeedPocketInactive(void)
{
	BEFORESENDSOLE(ar, PACKETTYPE_FEED_POCKET_INACTIVE, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnPetLevelup(OBJID objid, CAr& ar)
{

	DWORD dwPetId;
	ar >> dwPetId;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		pMover->SetPetId(dwPetId);
		pMover->PetLevelup();
	}
}

void CDPClient::OnPetSetExp(OBJID objid, CAr& ar)
{

	DWORD dwExp;
	ar >> dwExp;
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover) && pMover->IsActiveMover())
	{
		CPet* pPet = pMover->GetPet();
		if (pPet)
		{
			pPet->SetExp(dwExp);
			if (pPet->GetLevel() == PL_EGG)
			{
				if (pPet->GetExp() == MAX_PET_EGG_EXP)
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_PETEGG_HATCH)));
			}
			else
			{
#ifdef __AIO_NEUZAPP
				if (pPet->GetExp() == MAX_PET_EXP)
				{
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_PETEGG_HATCH)));
					if (g_Neuz.IsMinimised())
						g_Neuz.Flash();
				}
#else
				if (pPet->GetExp() == MAX_PET_EXP)
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_PETEGG_HATCH)));
#endif
				}
			}
		}
	}

void CDPClient::OnPet(OBJID objid, CAr& ar)
{
	CPet pet;
	BYTE nPetLevelup;

	pet.Serialize(ar);
	ar >> nPetLevelup;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover) && pMover->IsActiveMover())
	{
		CItemElem* pItemElem = pMover->GetPetItem();
		if (pItemElem)
		{
			SAFE_DELETE(pItemElem->m_pPet);
			pItemElem->m_pPet = new CPet;
			*(pItemElem->m_pPet) = pet;
			pMover->m_pet.SetLevelup(nPetLevelup);
			if (nPetLevelup != 0 && pMover->m_pet.GetObj())
			{
				pItemElem->m_dwItemId = pItemElem->m_pPet->GetItemId();
				pItemElem->SetTexture();
				pMover->m_pet.GetObj()->Delete();
				pMover->m_pet.SetObj(NULL);
			}
		}
	}
}

void CDPClient::OnPetState(OBJID objid, CAr& ar)
{

	DWORD dwPetId, dwExp;
	WORD wLife, wEnergy;
	ar >> dwPetId >> wLife >> wEnergy >> dwExp;
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover) && pMover->IsActiveMover())
	{
		CItemElem* pItemElem = pMover->m_Inventory.GetAtId(dwPetId);
		if (pItemElem && pItemElem->IsPet() && pItemElem->m_pPet)
		{
			WORD wOldLife = pItemElem->m_pPet->GetLife();

			pItemElem->m_pPet->SetLife(wLife);
			pItemElem->m_pPet->SetEnergy(wEnergy);
			pItemElem->m_pPet->SetExp(dwExp);

			if (wOldLife > wLife)
			{

			}
			else if (wOldLife < wLife)
			{

				CString strTemp;
				strTemp.Format(prj.GetText(TID_GAME_PET_LIFE), wLife - wOldLife);
				g_WndMng.OpenMessageBox(strTemp);
			}
		}
	}
}

void CDPClient::OnPetFeed(OBJID objid, CAr& ar)
{
	WORD wEnergy;
	ar >> wEnergy;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		CPet* pPet = pMover->GetPet();
		if (pPet)
		{
			pPet->SetEnergy(wEnergy);

		}
	}
}

void CDPClient::OnPetFoodMill(OBJID objid, CAr& ar)
{
	int nResult, nCount;
	ar >> nResult >> nCount;

	CWndPetFoodMill* pWndMill = (CWndPetFoodMill*)g_WndMng.GetWndBase(APP_PET_FOODMILL);
	if (pWndMill != NULL)
	{
		pWndMill->ReceiveResult(nResult, nCount);
		if (pWndMill->m_dwObjId != NULL_ID)
			pWndMill->Destroy();
	}
}

#if __VER >= 9	//__AI_0509
void CDPClient::OnSetSpeedFactor(OBJID objid, CAr& ar)
{
	FLOAT fSpeedFactor;
	ar >> fSpeedFactor;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
		pMover->SetSpeedFactor(fSpeedFactor);
}
#endif	// __AI_0509

#if __VER >= 10 // __CSC_VER9_1
void CDPClient::SendLegendSkillUp(OBJID* pdwItemId, int count)
{
	BEFORESENDSOLE(ar, PACKETTYPE_LEGENDSKILLUP_START, DPID_UNKNOWN);

	for (int i = 0; i < count; i++)
		ar << pdwItemId[i];

	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif //__CSC_VER9_1

#if __VER >= 9 // __CSC_VER9_2
void CDPClient::SendModifyStatus(int nStrCount, int nStaCount, int nDexCount, int nIntCount)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MODIFY_STATUS, DPID_UNKNOWN);

	ar << nStrCount << nStaCount << nDexCount << nIntCount;

	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif //__CSC_VER9_2
#if __VER >= 11 // __SYS_COLLECTING
void CDPClient::SendQueryStartCollecting(void)
{
	SendHdr(PACKETTYPE_QUERY_START_COLLECTING);
}

void CDPClient::SendQueryStopCollecting(void)
{
	SendHdr(PACKETTYPE_QUERY_STOP_COLLECTING);
}

void CDPClient::OnStartCollecting(OBJID objid)
{
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		pMover->StartCollecting();
		if (pMover->IsActiveMover())
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_START_COLLECTING), NULL, prj.GetTextColor(TID_GAME_START_COLLECTING));

			g_WndMng.m_pWndCollecting->SetButtonCaption(true);
		}
	}
}

void CDPClient::OnRestartCollecting(OBJID objid, CAr& ar)
{
	DWORD dwItemId;
	ar >> dwItemId;
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover) && pMover->IsActiveMover())
	{
		pMover->m_nCltTime = 0;
	}
}

void CDPClient::OnStopCollecting(OBJID objid)
{
	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		pMover->StopCollecting();
		if (pMover->IsActiveMover())
		{

			if (g_WndMng.m_pWndCollecting)
				g_WndMng.m_pWndCollecting->SetButtonCaption(false);
		}
	}
}
#endif	// __SYS_COLLECTING

#if __VER >= 11
void CDPClient::SendReqGuildBankLogList(BYTE byListType)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILDLOG_VIEW, DPID_UNKNOWN);
	ar << byListType;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnGuildBankLogList(CAr& ar)
{
	BYTE byListType;
	short nLogCount = 0;
	char szPlayer[42];
	long nDate = 0;
	long nItem = 0;
	long nAbilityOption = 0;
	long nItem_count = 0;
	long nPenya = 0;
	vector < CString >* pvString = NULL;

	CWndGuildBankLog* pWndGuildBankLog = (CWndGuildBankLog*)g_WndMng.GetWndBase(APP_GUILD_BANK_LOG);
	CGuild* pGuild = g_pPlayer->GetGuild();

	if (pWndGuildBankLog == NULL || pGuild == NULL)
		return;

	ar >> byListType;
	ar >> nLogCount;

	switch (byListType)
	{
	case GI_LOG_VIEW_ITEM_ADD:
	{
		pvString = &pWndGuildBankLog->m_wndAddItemLog.m_vLogList;
	}
	break;
	case GI_LOG_VIEW_ITEM_REMOVE:
	{
		pvString = &pWndGuildBankLog->m_wndRemoveItemLog.m_vLogList;
	}
	break;
	case GI_LOG_VIEW_MONEY_ADD:
	{
		pvString = &pWndGuildBankLog->m_wndReceivePenyaLog.m_vLogList;
	}
	break;
	case GI_LOG_VIEW_MONEY_REMOVE:
	{
		pvString = &pWndGuildBankLog->m_wndInvestPenyaLog.m_vLogList;
	}
	break;
	}

	char szDate[256] = { 0 };
	for (short i = 0; i < nLogCount; i++)
	{
		CString strLog;
		ar.ReadString(szPlayer, 42);
		ar >> nDate;
		if (byListType == GI_LOG_VIEW_ITEM_ADD || byListType == GI_LOG_VIEW_ITEM_REMOVE)
		{
			ar >> nItem;
			ar >> nAbilityOption;
			ar >> nItem_count;
			CTime cTime((time_t)nDate);
			sprintf(szDate, "%.2d%.2d%.2d %.2d:%.2d   ", cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute());

			CString strItemName, strCharName;
			ItemProp* pItemProp = prj.GetItemProp(nItem);

			strCharName.Format("%s", szPlayer);
			g_WndMng.StringSize(strCharName, 9);

			if (pItemProp->dwReferStat1 == WEAPON_ULTIMATE)
				strItemName.Format("(U)%s", pItemProp->szName);
			else
				strItemName.Format("%s", pItemProp->szName);

			BOOL bGuildCombatCloak = FALSE;

			if (nItem == II_ARM_S_CLO_CLO_DRAGON1 || nItem == II_ARM_S_CLO_CLO_DRAGON2 ||
				nItem == II_ARM_S_CLO_CLO_DRAGON3 || nItem == II_ARM_S_CLO_CLO_DRAGON4)
				bGuildCombatCloak = TRUE;

			if (pItemProp->dwItemKind3 == IK3_CLOAK)
			{
				CGuild* pGuild = g_GuildMng.GetGuild(g_pPlayer->m_idGuild);
				if (pGuild && bGuildCombatCloak == FALSE)
					strItemName.Format(prj.GetText(TID_GUILD_CLOAK), pGuild->m_szGuild);
				else
					strItemName.Format("%s", pItemProp->szName);
			}

			g_WndMng.StringSize(strItemName, 25);

			if (nAbilityOption > 0)
			{
				CString strAblility;
				strAblility.Format(" +%d", nAbilityOption);
				strItemName += strAblility;
			}

			if (pItemProp)
			{
				if (byListType == GI_LOG_VIEW_ITEM_ADD)
					strLog.Format(prj.GetText(TID_GAME_GUILDBANK_LOG_ITEM_ADD), szDate, strCharName, strItemName, nItem_count);
				else if (byListType == GI_LOG_VIEW_ITEM_REMOVE)
					strLog.Format(prj.GetText(TID_GAME_GUILDBANK_LOG_ITEM_REMOVE), szDate, strCharName, strItemName, nItem_count);
			}

			if (pvString)
				pvString->push_back(strLog);
		}
		else if (byListType == GI_LOG_VIEW_MONEY_ADD || byListType == GI_LOG_VIEW_MONEY_REMOVE)
		{
			ar >> nPenya;

			CTime cTime((time_t)nDate);
			sprintf(szDate, "%.2d%.2d%.2d %.2d:%.2d   ", cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute());

			if (byListType == GI_LOG_VIEW_MONEY_ADD)
				strLog.Format(prj.GetText(TID_GAME_GUILDBANK_LOG_PENYA_ADD), szDate, szPlayer, nPenya);
			else if (byListType == GI_LOG_VIEW_MONEY_REMOVE)
				strLog.Format(prj.GetText(TID_GAME_GUILDBANK_LOG_PENYA_REMOVE), szDate, szPlayer, nPenya);

			if (pvString)
				pvString->push_back(strLog);
		}
	}

	pWndGuildBankLog->UpdateScroll();
}
#endif

#if __VER >= 13
void CDPClient::SendReqHonorList()
{
	BEFORESENDSOLE(ar, PACKETTYPE_HONOR_LIST_REQ, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendReqHonorTitleChange(int nChange)
{
	BEFORESENDSOLE(ar, PACKETTYPE_HONOR_CHANGE_REQ, DPID_UNKNOWN);
	ar << nChange;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnHonorListAck(CAr& ar)
{
	int nHonorTitle[MAX_HONOR_TITLE];

	for (int i = 0; i < MAX_HONOR_TITLE; i++)
	{
		ar >> nHonorTitle[i];

		int nNeed = CTitleManager::Instance()->GetNeedCount(i, -1);
		if (nHonorTitle[i] >= nNeed && (0 < nNeed))
		{
			if (!CTitleManager::Instance()->IsEarned(i))
			{
				CTitleManager::Instance()->AddEarned(i);

				CString	strNotice;
				strNotice.Format(prj.GetText(TID_GAME_GET_TITLE), CTitleManager::Instance()->GetTitle(i));
				g_WndMng.PutString((LPCTSTR)strNotice, NULL, prj.GetTextColor(TID_GAME_GET_TITLE));
			}
		}
		else
		{

			if (CTitleManager::Instance()->IsEarned(i))
			{
				CTitleManager::Instance()->RemoveEarned(i);
			}
		}

	}

	CWndBase* pWndBase = g_WndMng.GetWndBase(APP_CHARACTER3);

	if (pWndBase)
		((CWndCharacter*)pWndBase)->m_wndHonor.RefreshList();

}

void CDPClient::OnHonorChangeAck(OBJID objid, CAr& ar)
{
	int nChange;
	ar >> nChange;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
	{
		pMover->m_nHonor = nChange;
		pMover->SetTitle(CTitleManager::Instance()->GetTitle(pMover->m_nHonor));
	}
}

#endif

#ifdef __NPC_BUFF
void CDPClient::SendNPCBuff(const char* szKey)
{
	BEFORESENDSOLE(ar, PACKETTYPE_NPC_BUFF, DPID_UNKNOWN);
	ar.WriteString(szKey);
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __NPC_BUFF

#ifdef __JEFF_11
void	CDPClient::SendQuePetResurrection(int nItem)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUE_PETRESURRECTION, DPID_UNKNOWN);
	ar << nItem;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void	CDPClient::OnQuePetResurrectionResult(CAr& ar)
{
	BOOL bResult;
	ar >> bResult;
	if (bResult)
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_PETTRADE_OK)));
	else
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_PETTRADE_FAIL)));
}
#endif	// __JEFF_11

#if __VER >= 12 // __SECRET_ROOM
void	CDPClient::SendSecretRoomTender(int nPenya)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SECRETROOM_TENDER, DPID_UNKNOWN);
	ar << nPenya;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void	CDPClient::SendSecretRoomTenderCancelReturn()
{
	BEFORESENDSOLE(ar, PACKETTYPE_SECRETROOM_TENDERCANCELRETURN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSecretRoomLineUpMember(vector<DWORD> vecLineUpMember)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SECRETROOM_LINEUPMEMBER, DPID_UNKNOWN);
	ar << vecLineUpMember.size();
	for (int i = 0; i < (int)(vecLineUpMember.size()); i++)
		ar << vecLineUpMember[i];
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnSecretRoomMngState(CAr& ar)
{
	DWORD dwTime;
	ar >> CSecretRoomMng::GetInstance()->m_nState;
	ar >> dwTime;

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	if (pWndWorld == NULL)
	{
		Error("CDPClient::OnGuildCombatNextTimeState()에 World가 없다");
		return;
	}

	BOOL bTimeSet = FALSE;

	CString strTemp;
	switch (CSecretRoomMng::GetInstance()->m_nState)
	{
	case SRMNG_LINEUP:
	{
		CGuild* pGuild = g_pPlayer->GetGuild();
		if (pGuild)
		{
			CGuildMember* pGuildMemberl = pGuild->GetMember(g_pPlayer->m_idPlayer);

			if (pGuildMemberl)
			{
				if (pGuildMemberl->m_nMemberLv == GUD_MASTER)
				{
					bTimeSet = TRUE;
					sprintf(pWndWorld->m_szSecretRoomStr, prj.GetText(TID_GAME_SECRETROOM_READY_TIME));
				}
			}
		}
		strTemp = prj.GetText(TID_GAME_SECRETROOM_MSG_SELECTION);
	}
	break;
	case SRMNG_ENTRANCE:
	{
		strTemp = prj.GetText(TID_GAME_SECRETROOM_MSG_ENTRANCE);

		SAFE_DELETE(g_WndMng.m_pWndSecretRoomMsg);
		g_WndMng.m_pWndSecretRoomMsg = new CWndSecretRoomInfoMsgBox();
		g_WndMng.m_pWndSecretRoomMsg->Initialize(&g_WndMng);
		CString strMsg;
		strMsg.Format("%s", prj.GetText(TID_GAME_SECRETROOM_TELEPORT_MSG));
		g_WndMng.m_pWndSecretRoomMsg->SetString(strMsg);

		bTimeSet = FALSE;
	}
	break;
	case SRMNG_WARWAIT:
	{
		bTimeSet = TRUE;
		sprintf(pWndWorld->m_szSecretRoomStr, prj.GetText(TID_GAME_SECRETROOM_START_TIME));

		strTemp = prj.GetText(TID_GAME_SECRETROOM_MSG_WARWAIT);
	}
	break;
	case SRCONT_WAR:
	{
		bTimeSet = TRUE;
		sprintf(pWndWorld->m_szSecretRoomStr, prj.GetText(TID_GAME_SECRETROOM_WAR_TIME));
		strTemp = prj.GetText(TID_GAME_SECRETROOM_MSG_START);
	}
	break;
	case SRCONT_CLOSEWAIT:
	{
		bTimeSet = TRUE;
		sprintf(pWndWorld->m_szSecretRoomStr, prj.GetText(TID_GAME_SECRETROOM_CLOSE_TIME));
		strTemp = prj.GetText(TID_GAME_SECRETROOM_MSG_END);
	}
	break;
	default:
	{
		sprintf(pWndWorld->m_szSecretRoomStr, "");
		CSecretRoomMng::GetInstance()->m_dwRemainTime = 0;
	}
	break;
	}

	if (bTimeSet)
		CSecretRoomMng::GetInstance()->m_dwRemainTime = dwTime + GetTickCount();

	if (!strTemp.IsEmpty())
		g_Caption1.AddCaption(strTemp, pWndWorld->m_pFontAPICaption);
}

void CDPClient::OnSecretRoomInfo(CAr& ar)
{
	BYTE nType, nContinent;
	int nSize;
	ar >> nContinent;
	ar >> nType;

	map<BYTE, CSecretRoomContinent*>::iterator it = CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.find(nContinent);
	CSecretRoomContinent* pSRCont = it->second;

	switch (nType)
	{

	case SECRETROOM_GUILDLIST:
	{
		if (pSRCont)
		{
			pSRCont->m_vecSecretRoomTender.clear();
			CSecretRoomMng::GetInstance()->m_pSRCont = pSRCont;
		}

		ar >> nSize;
		for (int i = 0; i < nSize; i++)
		{
			__SECRETROOM_TENDER srTender;
			ar >> srTender.dwGuildId;
			ar >> srTender.nWarState;
			ar >> srTender.nKillCount;
			if (pSRCont)
				pSRCont->m_vecSecretRoomTender.push_back(srTender);

			if (g_pPlayer->GetGuild() && srTender.dwGuildId == g_pPlayer->GetGuild()->m_idGuild)
			{
				int nLineupSize;
				DWORD dwLineupId;
				ar >> nLineupSize;

				for (int j = 0; j < nLineupSize; j++)
				{
					ar >> dwLineupId;
					pSRCont->m_vecSecretRoomTender[i].vecLineUpMember.push_back(dwLineupId);
				}
			}
		}
		ar >> nSize;
		for (int i = 0; i < nSize; i++)
		{
			int nMonsterType, nNum;
			ar >> nMonsterType;
			ar >> nNum;
			CSecretRoomMng::GetInstance()->m_mapMonsterNum.insert(make_pair(nMonsterType, nNum));
		}
	}
	break;
	case SECRETROOM_KILLCOUNT:
	{
		int nIndex, nKillConunt;
		ar >> nIndex;
		ar >> nKillConunt;
		if (pSRCont)
		{
			ASSERT(nIndex < (int)(pSRCont->m_vecSecretRoomTender.size()));
			pSRCont->m_vecSecretRoomTender[nIndex].nKillCount = nKillConunt;
		}
	}
	break;

	case SECRETROOM_WARSTATE:
	{
		int nIndex, nWarState;
		ar >> nIndex;
		ar >> nWarState;
		if (pSRCont)
		{
			ASSERT(nIndex < (int)(pSRCont->m_vecSecretRoomTender.size()));
			pSRCont->m_vecSecretRoomTender[nIndex].nWarState = nWarState;
			pSRCont->m_vecSecretRoomTender[nIndex].nKillCount = 0;
		}

		CString strTemp;
		switch (nWarState)
		{
		case MONSTER_WIN:
			strTemp.Empty();
			break;
		case MONSTER_FAILED:
			ASSERT(nIndex < (int)(pSRCont->m_vecSecretRoomTender.size()));
			strTemp.Empty();
			CGuild* pGuild = g_GuildMng.GetGuild(pSRCont->m_vecSecretRoomTender[nIndex].dwGuildId);
			if (pGuild)
				strTemp.Format(prj.GetText(TID_GAME_SECRETROOM_GUILD_FAILED), pGuild->m_szGuild);

			break;
		}

		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

		if (pWndWorld == NULL)
		{
			Error("CDPClient::OnGuildCombatNextTimeState()에 World가 없다");
			return;
		}

		if (!strTemp.IsEmpty())
		{
			if (::GetLanguage() != LANG_JAP)
				g_Caption1.AddCaption(strTemp, pWndWorld->m_pFontAPICaption);// CWndBase::m_Theme.m_pFontCaption );
			else
				g_Caption1.AddCaption(strTemp, NULL);// CWndBase::m_Theme.m_pFontCaption );
		}
	}
	break;
	}
}

void CDPClient::SendSecretRoomTenderOpenWnd()
{
	BEFORESENDSOLE(ar, PACKETTYPE_SECRETROOM_TENDEROPENWND, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnSecretRoomTenderOpenWnd(CAr& ar)
{
	int nTenderPenya;

	ar >> CSecretRoomMng::GetInstance()->m_nMinPenya;
	ar >> nTenderPenya;

	SAFE_DELETE(g_WndMng.m_pWndSecretRoomOffer);
	g_WndMng.m_pWndSecretRoomOffer = new CWndSecretRoomOffer();
	g_WndMng.m_pWndSecretRoomOffer->Initialize();

	if (nTenderPenya == 0)
	{
		g_WndMng.m_pWndSecretRoomOffer->SetReqGold(nTenderPenya);
		g_WndMng.m_pWndSecretRoomOffer->SetBackupGold(CSecretRoomMng::GetInstance()->m_nMinPenya);
		g_WndMng.m_pWndSecretRoomOffer->SetGold(CSecretRoomMng::GetInstance()->m_nMinPenya);
	}
	else
	{
		g_WndMng.m_pWndSecretRoomOffer->SetReqGold(nTenderPenya);
		g_WndMng.m_pWndSecretRoomOffer->SetBackupGold(nTenderPenya);
		g_WndMng.m_pWndSecretRoomOffer->SetGold(nTenderPenya);
	}
}

void CDPClient::SendSecretRoomLineUpOpenWnd()
{
	BEFORESENDSOLE(ar, PACKETTYPE_SECRETROOM_LINEUPOPENWND, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnSecretRoomLineUpOpenWnd(CAr& ar)
{
	int nSize;

	ar >> CSecretRoomMng::GetInstance()->m_nMinGuildMemberNum;
	ar >> CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum;
	ar >> nSize;

	SAFE_DELETE(g_WndMng.m_pWndSecretRoomSelection);
	g_WndMng.m_pWndSecretRoomSelection = new CWndSecretRoomSelection();
	g_WndMng.m_pWndSecretRoomSelection->Initialize();

	for (int i = 0; i < nSize; i++)
	{
		DWORD dwIdPlayer;
		ar >> dwIdPlayer;
		g_WndMng.m_pWndSecretRoomSelection->AddCombatPlayer(dwIdPlayer);
	}
}

void CDPClient::SendSecretRoomEntrance()
{
	BEFORESENDSOLE(ar, PACKETTYPE_SECRETROOM_ENTRANCE, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSecretRoomTeleportToNPC()
{
	BEFORESENDSOLE(ar, PACKETTYPE_SECRETROOM_TELEPORTTONPC, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSecretRoomTenderView()
{
	BEFORESENDSOLE(ar, PACKETTYPE_SECRETROOM_TENDERVIEW, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnSecretRoomTenderView(CAr& ar)
{
	int nTenderPenya;
	int nRanking;
	time_t t;
	int nSize;

	ar >> nTenderPenya;
	ar >> nRanking;
	ar >> t;
	ar >> nSize;

	SAFE_DELETE(g_WndMng.m_pWndSecretRoomOfferState);
	g_WndMng.m_pWndSecretRoomOfferState = new CWndSecretRoomOfferState();
	g_WndMng.m_pWndSecretRoomOfferState->Initialize(NULL);
	g_WndMng.m_pWndSecretRoomOfferState->SetTime(t);
	g_WndMng.m_pWndSecretRoomOfferState->SetRate(nRanking);
	g_WndMng.m_pWndSecretRoomOfferState->SetGold(nTenderPenya);

	DWORD dwGuildId;
	int nRank = 0;
	for (int i = 0; i < nSize; i++)
	{
		ar >> dwGuildId;

		if (!g_GuildMng.GetGuild(dwGuildId))
			continue;
		g_WndMng.m_pWndSecretRoomOfferState->InsertGuild(g_GuildMng.GetGuild(dwGuildId)->m_szGuild, "1", ++nRank);
	}
}
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
void CDPClient::OnTaxInfo(CAr& ar)
{
	CTax::GetInstance()->Serialize(ar);

	SAFE_DELETE(g_WndMng.m_pWndSecretRoomChangeTaxRate);
}

void CDPClient::OnTaxSetTaxRateOpenWnd(CAr& ar)
{
	BYTE nCont;
	int nMinTaxRate, nMaxTaxRate;

	ar >> nCont;
	ar >> nMinTaxRate >> nMaxTaxRate;

	SAFE_DELETE(g_WndMng.m_pWndSecretRoomChangeTaxRate);
	g_WndMng.m_pWndSecretRoomChangeTaxRate = new CWndSecretRoomChangeTaxRate;
	g_WndMng.m_pWndSecretRoomChangeTaxRate->SetDefaultTax(nMinTaxRate, nMaxTaxRate, nCont);
	g_WndMng.m_pWndSecretRoomChangeTaxRate->Initialize(NULL);
}

void CDPClient::SendTaxRate(BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate)
{
	BEFORESENDSOLE(ar, PACKETTYPE_TAX_SET_TAXRATE, DPID_UNKNOWN);
	ar << nCont << nSalesTaxRate << nPurchaseTaxRate;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __TAX

#if __VER >= 12 // __HEAVEN_TOWER
void CDPClient::SendTeleportToHeavenTower(int nFloor)
{
	BEFORESENDSOLE(ar, PACKETTYPE_HEAVENTOWER_TELEPORT, DPID_UNKNOWN);
	ar << nFloor;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __HEAVEN_TOWER

#if __VER >= 12 // __LORD
void CDPClient::OnElectionAddDeposit(CAr& ar)
{
	u_long idPlayer;
	__int64 iDeposit;
	time_t tCreate;
	ar >> idPlayer >> iDeposit >> tCreate;

	IElection* pElection = CCLord::Instance()->GetElection();
	pElection->AddDeposit(idPlayer, iDeposit, tCreate);
}

void CDPClient::OnElectionSetPledge(CAr& ar)
{
	u_long idPlayer;
	char szPledge[CCandidate::nMaxPledgeLen] = { 0, };
	ar >> idPlayer;
	ar.ReadString(szPledge, CCandidate::nMaxPledgeLen);

	IElection* pElection = CCLord::Instance()->GetElection();
	pElection->SetPledge(idPlayer, szPledge);
}

void CDPClient::OnElectionIncVote(CAr& ar)
{
	u_long idPlayer, idElector;
	ar >> idPlayer >> idElector;

	IElection* pElection = CCLord::Instance()->GetElection();
	pElection->IncVote(idPlayer, idElector);
}

void CDPClient::OnElectionBeginCandidacy(CAr& ar)
{
	IElection* pElection = CCLord::Instance()->GetElection();
	pElection->BeginCandidacy();
}

void CDPClient::OnElectionBeginVote(CAr& ar)
{
	int nRequirement;
	ar >> nRequirement;

	IElection* pElection = CCLord::Instance()->GetElection();
	pElection->BeginVote(nRequirement);
}

void CDPClient::OnElectionEndVote(CAr& ar)
{
	u_long idPlayer;
	ar >> idPlayer;

	IElection* pElection = CCLord::Instance()->GetElection();
	pElection->EndVote(idPlayer);
}

void CDPClient::OnLord(CAr& ar)
{
	CCLord::Instance()->Serialize(ar);
}

void CDPClient::OnLEventCreate(CAr& ar)
{
	CLEComponent* pComponent = new CLEComponent;
	pComponent->Serialize(ar);
	ILordEvent* pEvent = CCLord::Instance()->GetEvent();
	pEvent->AddComponent(pComponent);
}

void CDPClient::OnLEventInitialize(CAr& ar)
{
	ILordEvent* pEvent = CCLord::Instance()->GetEvent();
	pEvent->Initialize();
}

void CDPClient::OnLordSkillTick(CAr& ar)
{
	CLordSkill* pSkills = CCLord::Instance()->GetSkills();
	pSkills->SerializeTick(ar);
}

void CDPClient::OnLEventTick(CAr& ar)
{
	ILordEvent* pEvent = CCLord::Instance()->GetEvent();
	pEvent->SerializeTick(ar);
	pEvent->EraseExpiredComponents();
}

void CDPClient::OnLordSkillUse(OBJID objid, CAr& ar)
{
	u_long idTarget;
	int nSkill;
	ar >> idTarget >> nSkill;


	CLordSkillComponentExecutable* pSkill = CCLord::Instance()->GetSkills()->GetSkill(nSkill);
	if (!pSkill)
		return;

	pSkill->Execute(CCLord::Instance()->Get(), idTarget, NULL);

	CMover* pLord = prj.GetMover(objid);
	CMover* pTarget = prj.GetUserByID(idTarget);

	if (IsValidObj(pLord) && pSkill->GetSrcSfx() != NULL_ID)
	{

		if (pTarget)
		{
			g_pPlayer->SetAngle(GetDegree(pTarget->GetPos(), pLord->GetPos()));
		}
		g_pPlayer->SetMotion(MTI_ATK1, ANILOOP_1PLAY, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED);
		CreateSfx(g_Neuz.m_pd3dDevice, (int)pSkill->GetSrcSfx(), pLord->GetPos(), pLord->GetId());
	}
	if (IsValidObj(pTarget) && pSkill->GetDstSfx() != NULL_ID)
	{

		CreateSfx(g_Neuz.m_pd3dDevice, (int)pSkill->GetDstSfx(), pTarget->GetPos(), pTarget->GetId());
	}
}

void CDPClient::SendElectionAddDeposit(__int64 iDeposit)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ELECTION_ADD_DEPOSIT, DPID_UNKNOWN);
	ar << iDeposit;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendElectionSetPledge(const char* szPledge)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ELECTION_SET_PLEDGE, DPID_UNKNOWN);
	ar.WriteString(szPledge);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendElectionIncVote(u_long idPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ELECTION_INC_VOTE, DPID_UNKNOWN);
	ar << idPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

#ifndef __FL_UPDATE_LORDEVENT
void CDPClient::SendLEventCreate(int iEEvent, int iIEvent)
{
	BEFORESENDSOLE(ar, PACKETTYPE_L_EVENT_CREATE, DPID_UNKNOWN);
	ar << iEEvent << iIEvent;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#else
void CDPClient::SendLEventCreate(int iEEvent, int iIEvent, int iPEvent)
{
	BEFORESENDSOLE(ar, PACKETTYPE_L_EVENT_CREATE, DPID_UNKNOWN);
	ar << iEEvent << iIEvent;
	ar << iPEvent;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

void CDPClient::SendLordSkillUse(int nSkill, const char* szTarget)
{
	BEFORESENDSOLE(ar, PACKETTYPE_LORD_SKILL_USE, DPID_UNKNOWN);
	ar << nSkill;
	ar.WriteString(szTarget);
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif	// __LORD

#if __VER >= 12 // __PET_0519
void CDPClient::SendPickupPetAwakeningCancel(DWORD dwItem)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PICKUP_PET_AWAKENING_CANCEL, DPID_UNKNOWN);
	ar << dwItem;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif	// __PET_0519

#ifdef __PET_1024
void CDPClient::SendClearPetName()
{
	SendHdr(PACKETTYPE_CLEAR_PET_NAME);
}

void CDPClient::OnSetPetName(OBJID objid, CAr& ar)
{
	char szPetName[MAX_PET_NAME] = { 0, };
	ar.ReadString(szPetName, MAX_PET_NAME);

	CMover* pPlayer = prj.GetMover(objid);
	if (IsValidObj(pPlayer))
	{
		pPlayer->m_pet.SetName(szPetName);
		if (IsValidObj(pPlayer->m_pet.GetObj()))
		{
			if (strlen(szPetName) == 0)
				pPlayer->m_pet.GetObj()->SetName(pPlayer->m_pet.GetObj()->GetProp()->szName);
			else
				pPlayer->m_pet.GetObj()->SetName(szPetName);
		}
		if (pPlayer->IsActiveMover())
		{
			CPet* pPet = pPlayer->GetPet();
			pPet->SetName(szPetName);
			g_WndMng.PutString(prj.GetText(TID_GAME_NAME_PET_S00), NULL, prj.GetTextColor(TID_GAME_NAME_PET_S00));
		}
	}
}
#endif	// __PET_1024

#if __VER >= 13 // __HOUSING
void CDPClient::OnHousingAllInfo(CAr& ar)
{
	CHousing::GetInstance()->Serialize(ar);
}

void CDPClient::OnHousingSetFunitureList(CAr& ar)
{
	HOUSINGINFO housingInfo;
	BOOL bAdd;

	housingInfo.Serialize(ar);
	ar >> bAdd;

	CHousing::GetInstance()->SetFurnitureList(housingInfo, bAdd);

	CWndHousing* pWndHousing = (CWndHousing*)g_WndMng.GetApplet(APP_HOUSING);
	if (pWndHousing)	pWndHousing->RefreshItemList();

	ItemProp* pItemProp = prj.GetItemProp(housingInfo.dwItemId);
	if (pItemProp)
	{
		DWORD dwTextId;
		CString strTemp;
		if (bAdd)
			dwTextId = TID_GAME_HOUSING_ADDITION;
		else
			dwTextId = TID_GAME_HOUSING_TIMEOUT;

		strTemp.Format(prj.GetText(dwTextId), pItemProp->szName);
		g_WndMng.PutString(strTemp, NULL, prj.GetTextColor(dwTextId));
	}

}

void CDPClient::OnHousingSetupFurniture(CAr& ar)
{
	HOUSINGINFO housingInfo;
	housingInfo.Serialize(ar);

	CHousing::GetInstance()->SetupFurniture(housingInfo);

	CWndHousing* pWndHousing = (CWndHousing*)g_WndMng.GetApplet(APP_HOUSING);
	if (pWndHousing)	pWndHousing->RefreshItemList();

	ItemProp* pItemProp = prj.GetItemProp(housingInfo.dwItemId);
	if (pItemProp)
	{
		DWORD dwTextId;
		CString strTemp;
		if (housingInfo.bSetup)
			dwTextId = TID_GAME_HOUSING_INSTALL;
		else
			dwTextId = TID_GAME_HOUSING_DEMOUNT;

		strTemp.Format(prj.GetText(dwTextId), pItemProp->szName);
		g_WndMng.PutString(strTemp, NULL, prj.GetTextColor(dwTextId));
	}
}

void CDPClient::OnHousingPaperingInfo(CAr& ar)
{
	DWORD dwItemId;
	BOOL bSetup;
	ar >> dwItemId;
	ar >> bSetup;


	if (dwItemId == NULL_ID && !bSetup)
	{
		CDeployManager::GetInstance()->ChangeWallTex();
		CDeployManager::GetInstance()->ChangeTileTex();
		return;
	}

	ItemProp* pItemProp = prj.GetItemProp(dwItemId);
	if (pItemProp && pItemProp->dwItemKind2 == IK2_PAPERING)
	{
		switch (pItemProp->dwItemKind3)
		{
		case IK3_WALLPAPER:
			if (bSetup)
				CDeployManager::GetInstance()->ChangeWallTex(pItemProp->szTextFileName);
			else
				CDeployManager::GetInstance()->ChangeWallTex();
			break;
		case IK3_CARPET:
			if (bSetup)
				CDeployManager::GetInstance()->ChangeTileTex(pItemProp->szTextFileName);
			else
				CDeployManager::GetInstance()->ChangeTileTex();
			break;
		}
	}
}

void CDPClient::OnHousingSetVisitAllow(CAr& ar)
{
	DWORD dwTargetId;
	BOOL bAllow;

	ar >> dwTargetId >> bAllow;

	CHousing::GetInstance()->SetVisitAllow(dwTargetId, bAllow);

	DWORD dwTextId;
	CString strTemp;
	if (bAllow)
		dwTextId = TID_GAME_HOUSING_INVIATION;
	else
		dwTextId = TID_GAME_HOUSING_INVIATION_CANCEL;

	strTemp.Format(prj.GetText(dwTextId), CPlayerDataCenter::GetInstance()->GetPlayerString(dwTargetId));
	g_WndMng.PutString(strTemp, NULL, prj.GetTextColor(dwTextId));

	CWndMessengerEx* pWndMessengerEx = (CWndMessengerEx*)g_WndMng.GetApplet(APP_MESSENGER_EX);
	if (pWndMessengerEx) pWndMessengerEx->UpdateFriendList();
}

void CDPClient::OnHousingVisitableList(CAr& ar)
{
	int nSize;
	DWORD dwPlayerId;

	ar >> nSize;
	CHousing::GetInstance()->m_vecVisitable.clear();
	for (int i = 0; i < nSize; i++)
	{
		ar >> dwPlayerId;
		CHousing::GetInstance()->m_vecVisitable.push_back(dwPlayerId);
	}
	CWndRoomList* pWndRoomList = g_WndMng.m_pWndRoomList;
	if (pWndRoomList) pWndRoomList->Refresh();
}

void CDPClient::SendHousingReqSetupFurniture(HOUSINGINFO housingInfo)
{
	BEFORESENDSOLE(ar, PACKETTYPE_HOUSING_SETUPFURNITURE, DPID_UNKNOWN);
	housingInfo.Serialize(ar);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendHousingReqSetVisitAllow(DWORD dwPlayerId, BOOL bAllow)
{
	BEFORESENDSOLE(ar, PACKETTYPE_HOUSING_SETVISITALLOW, DPID_UNKNOWN);
	ar << dwPlayerId << bAllow;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendHousingVisitRoom(DWORD dwPlayerId)
{
	if (dwPlayerId == NULL_ID)
		CDeployManager::GetInstance()->SetMyRoom(TRUE);
	else
		CDeployManager::GetInstance()->SetMyRoom(FALSE);

	BEFORESENDSOLE(ar, PACKETTYPE_HOUSING_VISITROOM, DPID_UNKNOWN);
	ar << dwPlayerId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendHousingReqVisitableList()
{
	BEFORESENDSOLE(ar, PACKETTYPE_HOUSING_REQVISITABLELIST, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendHousingGoOut()
{
	BEFORESENDSOLE(ar, PACKETTYPE_HOUSING_GOOUT, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __HOUSING

#if __VER >= 13 // __QUEST_HELPER
void CDPClient::SendReqNPCPos(const char* szCharKey)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUESTHELPER_REQNPCPOS, DPID_UNKNOWN);
	ar.WriteString(szCharKey);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnNPCPos(CAr& ar)
{
	D3DXVECTOR3 vPos;
	ar >> vPos;

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	if (pWndWorld)
	{
		pWndWorld->m_bSetQuestNPCDest = TRUE;
		pWndWorld->m_vQuestNPCDest = vPos;
	}
}
#endif // __QUEST_HELPER

#if __VER >= 13 // __COUPLE_1117
void CDPClient::SendPropose(const char* pszTarget)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PROPOSE, DPID_UNKNOWN);
	ar.WriteString(pszTarget);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendRefuse()
{
	SendHdr(PACKETTYPE_REFUSE);
}

void CDPClient::SendCouple()
{
	SendHdr(PACKETTYPE_COUPLE);
}

void CDPClient::SendDecouple()
{
	SendHdr(PACKETTYPE_DECOUPLE);
}

void CDPClient::OnCouple(CAr& ar)
{
	CCoupleHelper::Instance()->OnCouple(ar);
}

void CDPClient::OnProposeResult(CAr& ar)
{
	CCoupleHelper::Instance()->OnProposeResult(ar);
}

void CDPClient::OnCoupleResult(CAr& ar)
{
	CCoupleHelper::Instance()->OnCoupleResult(ar);
}

void CDPClient::OnDecoupleResult(CAr& ar)
{
	CCoupleHelper::Instance()->OnDecoupleResult();
}

#if __VER >= 13 // __COUPLE_1202
void CDPClient::OnAddCoupleExperience(CAr& ar)
{
	CCoupleHelper::Instance()->OnAddCoupleExperience(ar);
}
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117


#if __VER >= 14 // __SMELT_SAFETY
void CDPClient::SendSmeltSafety(OBJID objid, OBJID objMaterialId, OBJID objProtScrId, OBJID objSmeltScrId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SMELT_SAFETY, DPID_UNKNOWN);
	ar << objid << objMaterialId << objProtScrId << objSmeltScrId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnSmeltSafety(CAr& ar)
{
	BYTE nResult = 0;
	ar >> nResult;

	CWndSmeltSafety* pWndSmeltSafety = g_WndMng.m_pWndSmeltSafety;
	if (pWndSmeltSafety == NULL)
		return;

	if (pWndSmeltSafety->m_eWndMode == CWndSmeltSafety::WND_UNIQUE || pWndSmeltSafety->m_eWndMode == CWndSmeltSafety::WND_ULTIMATE)
	{
		switch (nResult)
		{
		case 0:
		{
			pWndSmeltSafety->SetResult(1);
			pWndSmeltSafety->Destroy();
			g_WndMng.PutString(prj.GetText(TID_GAME_TRANSFORM_S00));
			break;
		}
		case 1:
		{
			pWndSmeltSafety->SetResult(2);
			pWndSmeltSafety->AddFailure();
			pWndSmeltSafety->RefreshText();
			break;
		}
		case 2:
		{
			pWndSmeltSafety->Destroy();
			g_WndMng.PutString(prj.GetText(TID_UPGRADE_ERROR_WRONGUPLEVEL));
			break;
		}
		}
	}
	else
	{
		switch (nResult)
		{
		case 1:
		{
			pWndSmeltSafety->DisableScroll2();
			pWndSmeltSafety->SetResult(1);
			pWndSmeltSafety->AddSuccess();
			pWndSmeltSafety->RefreshText();
			break;
		}
		case 2:
		{
			pWndSmeltSafety->SetResult(2);
			pWndSmeltSafety->AddFailure();
			pWndSmeltSafety->RefreshText();
			break;
		}
		}
	}
}
#endif // __SMELT_SAFETY

#ifdef __MAP_SECURITY
void CDPClient::OnWorldReadInfo(CAr& ar)
{
	DWORD dwWorldId;
	D3DXVECTOR3 vPos;
	ar >> dwWorldId >> vPos;

	g_WorldMng.Open(g_Neuz.m_pd3dDevice, dwWorldId);
	g_WorldMng.Get()->RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
	g_WorldMng.Get()->SetCamera(&g_Neuz.m_camera);
	g_Neuz.m_camera.Transform(D3DDEVICE, g_WorldMng.Get());
	g_Neuz.m_camera.Reset();
	g_WorldMng.Get()->ReadWorld(vPos);
}

void CDPClient::SendMapKey(const char* szFileName, const char* szMapKey)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MAP_KEY, DPID_UNKNOWN);
	ar.WriteString(szFileName);
	ar.WriteString(szMapKey);
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __MAP_SECURITY

#if __VER >= 15 // __PETVIS
void CDPClient::SendRemoveVis(int nPos)
{
	BEFORESENDSOLE(ar, PACKETTYPE_VISPET_REMOVEVIS, DPID_UNKNOWN);
	ar << nPos;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::SendSwapVis(int nPos1, int nPos2)
{
	BEFORESENDSOLE(ar, PACKETTYPE_VISPET_SWAPVIS, DPID_UNKNOWN);
	ar << nPos1 << nPos2;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnActivateVisPet(CAr& ar)
{
	OBJID objIdVisPetItem;
	OBJID objIdVisPetId;
	ar >> objIdVisPetItem >> objIdVisPetId;

	g_pPlayer->SetEatPetId(objIdVisPetId);
	g_pPlayer->SetVisPetItem(objIdVisPetItem);

	BOOL bActivate = g_pPlayer->HasActivatedVisPet();
	if (bActivate)
	{
#ifndef __AIO_PETS
		if (FALSE == g_WndMng.GetWndBase(APP_BUFFPET_STATUS))
#else
		if (!g_WndMng.GetWndBase(APP_BUFFPET_STATUS) && !g_Option.m_bHideWindowLooter)
#endif
		{
			SAFE_DELETE(g_WndMng.m_pWndBuffPetStatus);
			g_WndMng.m_pWndBuffPetStatus = new CWndBuffPetStatus;
			g_WndMng.m_pWndBuffPetStatus->Initialize();
		}

}
	else
		SAFE_DELETE(g_WndMng.m_pWndBuffPetStatus);
}

void CDPClient::OnChangeMoverSfx(OBJID objId, CAr& ar)
{
	DWORD dwSfxId;
	ar >> dwSfxId;
	CMover* pMover = prj.GetMover(objId);
	if (IsValidObj(pMover))
	{
		pMover->m_dwMoverSfxId = dwSfxId;
		pMover->SetSfxBuffPet(dwSfxId);
	}
}
#endif // __PETVIS

#if __VER >= 15 // __GUILD_HOUSE
void CDPClient::SendBuyGuildHouse() { SendHdr(PACKETTYPE_GUILDHOUSE_BUY); }
void CDPClient::SendGuildHouseEnter() { SendHdr(PACKETTYPE_GUILDHOUSE_ENTER); }
void CDPClient::SendGuildHouseGoOut() { SendHdr(PACKETTYPE_GUILDHOUSE_GOOUT); }
void CDPClient::OnGuildHousePacket(CAr& ar)
{
	int nPacketType, nIndex;
	GH_Fntr_Info gfi;

	ar >> nPacketType >> nIndex;
	gfi.Serialize(ar);

	GuildHouse->OnGuildHousePacket(nPacketType, gfi, nIndex);

	CWndGuildHousing* pWnd = (CWndGuildHousing*)g_WndMng.GetApplet(APP_GH_FURNITURE_STORAGE);
	if (pWnd)
		pWnd->RefreshItemList();
	else
		GuildHouse->ApplyEFTexture();
}

void CDPClient::OnGuildHouseAllInfo(CAr& ar)
{
	BOOL bSetFurnitureChannel, bHaveGuildHouse;
	ar >> bSetFurnitureChannel >> bHaveGuildHouse;

	GuildHouse->SetFurnitureChannel(bSetFurnitureChannel);
	if (bHaveGuildHouse)
		GuildHouse->SerializeAllInfo(ar);

	GuildHouse->ApplyEFTexture();
}

void CDPClient::OnGuildHouseRemove(CAr& ar)
{
	GuildHouse->Clear();
}
#endif // __GUILD_HOUSE

#if __VER >= 15 // __TELEPORTER
void CDPClient::SendTeleporterReq(const char* szCharKey, int nIndex)
{
	BEFORESENDSOLE(ar, PACKETTYPE_TELEPORTER, DPID_UNKNOWN);
	ar.WriteString(szCharKey);
	ar << nIndex;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __TELEPORTER

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CDPClient::OnCheckedQuest(CAr& ar)
{
	ar >> g_pPlayer->m_nCheckedQuestSize;
	if (g_pPlayer->m_nCheckedQuestSize)
		ar.Read(g_pPlayer->m_aCheckedQuest, sizeof(WORD) * g_pPlayer->m_nCheckedQuestSize);
}

void CDPClient::SendCheckedQuestId(int nQuestId, BOOL bCheck)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUEST_CHECK, DPID_UNKNOWN);
	ar << nQuestId << bCheck;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __PROTECT_AWAKE
void CDPClient::SendSelectedAwakeningValue(DWORD dwObjID, DWORD dwSerialNum, BYTE bySelectFlag)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ITEM_SELECT_AWAKENING_VALUE, DPID_UNKNOWN);
	ar << dwObjID << dwSerialNum << bySelectFlag;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnSafeAwakening(CAr& ar)
{
	BYTE byItemObjID = 0;
	DWORD dwSerialNumber = 0;
	__int64 n64NewRandomOption = 0;

	ar >> byItemObjID >> dwSerialNumber >> n64NewRandomOption;

	if (!g_pPlayer)
		return;

	SAFE_DELETE(g_WndMng.m_pWndSelectAwakeCase);
	g_WndMng.m_pWndSelectAwakeCase = new CWndSelectAwakeCase;
	g_WndMng.m_pWndSelectAwakeCase->SetData(byItemObjID, dwSerialNumber, n64NewRandomOption);
	g_WndMng.m_pWndSelectAwakeCase->Initialize(&g_WndMng, APP_AWAKE_SELECTCASE);
}

#endif //__PROTECT_AWAKE

#ifdef __FL_DUNGEON_TREASURE
void CDPClient::OnDungeonTreasureWnd(CAr& ar)
{
	BOOL bOpen;
	ar >> bOpen;

	if (!g_pPlayer)
		return;

	SAFE_DELETE(g_WndMng.m_bDungeonTreasure);

	if (bOpen)
	{
		g_WndMng.m_bDungeonTreasure = new CWndDungeonTreasure;
		g_WndMng.m_bDungeonTreasure->Initialize(&g_WndMng, APP_DUNGEON_TREASURE);
	}

}
void CDPClient::SendDungeonTreasureSelect(int nTreasure)
{
	BEFORESENDSOLE(ar, PACKETTYPE_DUNGEON_TREASURE_SELECT, DPID_UNKNOWN);
	ar << nTreasure;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnDungeonTreasureResult(CAr& ar)
{
	int nSelected;
	BOOL bResult;

	ar >> nSelected;
	ar >> bResult;

	if (!g_pPlayer)
		return;

	CWndDungeonTreasure* pWnd = (CWndDungeonTreasure*)g_WndMng.GetWndBase(APP_DUNGEON_TREASURE);
	if (pWnd)
		pWnd->ReceiveResult(nSelected, bResult);
}
#endif

#ifdef __FL_BATTLE_PASS
void CDPClient::OnBattlePassTime(CAr& ar)
{
	time_t tNext;
	ar >> tNext;
	CBattlePass::GetInstance()->SetTime(tNext);
}
void CDPClient::OnBattlePassProgress(CAr& ar)
{
	int nLevelTmp, nLevel, nExp;
	ar >> nLevel >> nExp;

	if (!g_pPlayer)
		return;

	nLevelTmp = g_pPlayer->GetBattlePassLevel();
	g_pPlayer->SetBattlePassLevel(nLevel);
	g_pPlayer->SetBattlePassExp(nExp);

	if (nLevelTmp != nLevel)
	{
		CWndBattlePass* pWindow = (CWndBattlePass*)g_WndMng.GetWndBase(APP_BATTLE_PASS);
		if (pWindow)
			pWindow->UpdateStaticParams();
	}
}
#endif

#ifdef __FL_CONSUMABLES_STORAGE
void	CDPClient::SendMoveItem_Chest(BYTE nItem, short nNum, BOOL bFromInventoryToChest)
{
	if (!g_pPlayer)
		return;

	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(nItem);
	if (!bFromInventoryToChest)
		pItemElem = g_pPlayer->m_Chest.GetAtId(nItem);

	if (!IsUsableItem(pItemElem))
		return;

	ItemProp* pProp = pItemElem->GetProp();
	if (!pProp)
		return;

	BEFORESENDSOLE(ar, PACKETTYPE_MOVE_ITEM_CHEST, DPID_UNKNOWN);
	ar << nItem << nNum << bFromInventoryToChest;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendMoveItemInsideChest(BYTE nSrcIndex, BYTE nDestIndex)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MOVE_ITEM_INSIDECHEST, DPID_UNKNOWN);
	ar << nSrcIndex << nDestIndex;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnPutItemToChest(OBJID objid, CAr& ar)
{
	CItemElem itemElem;
	itemElem.Serialize(ar);

	if (g_pPlayer)
		g_pPlayer->m_Chest.Add(&itemElem);
}
void CDPClient::OnMoveItemInsideChest(CAr& ar)
{
	BYTE nSrcIndex, nDestIndex;
	ar >> nSrcIndex >> nDestIndex;

	if (g_pPlayer)
		g_pPlayer->m_Chest.Swap(nSrcIndex, nDestIndex);
}
void CDPClient::OnMoveAllItemsToChest()
{
	int nNum = 0;
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAt(i);
		if (!pItemElem)
			continue;

		ItemProp* pProp = pItemElem->GetProp();
		if (!pProp)
			continue;

		if (!g_pPlayer->CanBeUsedOutsideInventory(pProp))
			continue;

		nNum = pItemElem->m_nItemNum;

		if (g_pPlayer->m_Chest.IsFull(pItemElem, pProp, nNum))
			continue;

		CItemElem itemElem;
		itemElem = *pItemElem;
		itemElem.m_nItemNum = nNum;

		g_pPlayer->m_Inventory.RemoveAtId(pItemElem->m_dwObjId); // Remove item from inventory in client side
		g_pPlayer->m_Chest.Add(&itemElem); // Add item in chest in client side
	}

	CWndChest* pWndChest = (CWndChest*)g_WndMng.GetApplet(APP_CHEST);
	if (pWndChest)
	{
		CWndButton* pButton = (CWndButton*)pWndChest->GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}

}
#endif

void CDPClient::OnUpdateStatInfo(OBJID objid, CAr& ar)
{
	int nStr, nSta, nDex, nInt;
	LONG nRemainGp;
	BOOL bRegen;

	ar >> nStr >> nSta >> nDex >> nInt;
	ar >> nRemainGp;
	ar >> bRegen;

	CMover* pMover = prj.GetMover(objid);
	if (!IsValidObj(pMover))
		return;

	pMover->SetStr(nStr);
	pMover->SetSta(nSta);
	pMover->SetDex(nDex);
	pMover->SetInt(nInt);
	pMover->m_nRemainGP = nRemainGp;

	if (bRegen)
		pMover->SetHitPoint(pMover->GetMaxHitPoint());

	if (bRegen || pMover->IsSMMode(SM_MAINTAIN_MP))
		pMover->SetManaPoint(pMover->GetMaxManaPoint());

	if (bRegen || pMover->IsSMMode(SM_MAINTAIN_FP))
		pMover->SetFatiguePoint(pMover->GetMaxFatiguePoint());
}

#ifdef __FL_BOXES_V2
void CDPClient::SendBulkOpenBox(DWORD dwObjID)
{
	BEFORESENDSOLE(ar, PACKETTYPE_BULKBOXOPENING, DPID_UNKNOWN);
	ar << dwObjID;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif
#ifdef __FL_FAST_ELEMENT
void CDPClient::SendSelectElement(OBJID objScroll, int nAttribute, int nPosition)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SELECT_ELEMENT, DPID_UNKNOWN);
	ar << objScroll;
	ar << nAttribute;
	ar << nPosition;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnElementChange(OBJID objid, CAr& ar)
{
	int nPosition, nOpt;
	ar >> nPosition >> nOpt;

	CMover* pObj = prj.GetMover(objid);
	if (!IsValidObj(pObj))
		return;

	pObj->m_aEquipInfo[nPosition].nOption = nOpt;
	pObj->RedoEquip(CMover::GetActiveMover() != NULL, false);
}

#endif

#ifdef __FL_SHOP_SEARCH
void CDPClient::OnShopSearch(CAr& ar)
{
	CWndShopSearch* pWnd = (CWndShopSearch*)g_WndMng.CreateApplet(APP_SHOP_SEARCH);
	if (pWnd)
		pWnd->AcceptResult(ar);
}
void CDPClient::SendTeleportToShop(OBJID objTarget)
{
	BEFORESENDSOLE(ar, PACKETTYPE_TELEPORT_TO_THIS_SHOP, DPID_UNKNOWN);
	ar << objTarget;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

#ifdef __FL_PET_FILTER
void CDPClient::SendPetLootOption(int nLootType, BYTE nLootPlace)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PETLOOTTYPE_REQ, DPID_UNKNOWN);
	ar << nLootType << nLootPlace;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

#ifdef __FL_EQUALIZED_MODE
void CDPClient::SendSelectTmpClass(int nTmpClass)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SELECT_TMP_CLASS, DPID_UNKNOWN);
	ar << nTmpClass;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnOpenSelectTmpWnd(CAr& ar)
{
	int nType;
	ar >> nType;

	SAFE_DELETE(g_WndMng.m_WndSelectTmpClass);
	g_WndMng.m_WndSelectTmpClass = new CWndSelectTmpClass;
	g_WndMng.m_WndSelectTmpClass->Initialize(NULL, APP_TMP_CLASS);
	g_WndMng.m_WndSelectTmpClass->SetClass(nType);
}
void CDPClient::OnSerializeInvAndBuffs(CAr& ar)
{
	if (!g_pPlayer)
		return;

	g_pPlayer->m_Inventory.Serialize(ar);

	bool bWithBuffs;
	ar >> bWithBuffs;
	if (bWithBuffs)
		g_pPlayer->m_buffs.Serialize(ar);

	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_aEquipInfo, (CModelObject*)g_pPlayer->m_pModel, &g_pPlayer->m_Inventory);
	g_WndMng.UpdateWindowModels();

#ifdef __INVENTORY_RESIZE
	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.GetWndBase(APP_INVENTORY);
	if (pWndInventory)
		pWndInventory->m_wndItemCtrl.SetMinScrollPos();
#endif

#ifdef __FL_SWITCH_EQ
	CWndSwitchEquip* pWnd = (CWndSwitchEquip*)g_WndMng.GetWndBase(APP_EQ_SWITCH);
	if (pWnd && pWnd->m_pWndNewSwitch)
		pWnd->m_pWndNewSwitch->Destroy();
#endif

	g_pPlayer->UpdateBoundBox();
	g_pPlayer->UpdateParam();
}
void CDPClient::OnSetNearEquipUpd(OBJID objid, CAr& ar)
{
	CMover* pMover = prj.GetMover(objid);
	if (!IsValidObj((CObj*)pMover))
		return;

	pMover->ClearEquipInfo();
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		ar >> pMover->m_aEquipInfo[i].dwId;
		ar >> pMover->m_aEquipInfo[i].nOption;
#ifdef __MODEL_CHANGE
		ar >> pMover->m_aEquipInfo[i].dwLook;
#endif
	}
	CMover::UpdateParts(pMover->GetSex(), pMover->m_dwHairMesh, pMover->m_dwHeadMesh, pMover->m_aEquipInfo, (CModelObject*)pMover->m_pModel, NULL);
}
#endif

#ifdef __FL_PARTY_SORT
void CDPClient::SendMoveMemberParty(u_long uMember, BOOL UpDown)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MOVE_PARTYMEMBERS, DPID_UNKNOWN);
	ar << uMember;
	ar << UpDown;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnPartyChangePos(CAr& ar)
{
	u_long uidChangeMember;
	BOOL bUpDown;
	ar >> uidChangeMember;
	ar >> bUpDown;

	if (g_Party.ChangePosition(uidChangeMember, bUpDown))
	{
		if (g_pPlayer && g_Party.IsLeader(g_pPlayer->m_idPlayer))
		{
			CWndParty* pWndParty = (CWndParty*)g_WndMng.GetApplet(APP_PARTY);
			if (pWndParty)
			{
				int nTarget = g_Party.FindMember(uidChangeMember);
				pWndParty->SelectTarget(nTarget);
			}
			g_WndMng.PutString("Members swapped!");
		}
	}
}
#endif

#ifdef __FL_GW_STATUE_V2
void CDPClient::SendStatueSettings(int nStatueType, int nStatuePrice)
{
	BEFORESENDSOLE(ar, PACKETTYPE_STATUE_SETTINGS, DPID_UNKNOWN);
	ar << nStatueType << nStatuePrice;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendRequestStatueParams(BOOL bIsMvp)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REQUEST_STATUE_PARAMS, DPID_UNKNOWN);
	ar << bIsMvp;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendReceiveStatueBuff(BOOL bIsMvp, int nPrice)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REQUEST_BUFF_STATUE, DPID_UNKNOWN);
	ar << bIsMvp;
	ar << nPrice;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnStatueParams(CAr& ar)
{
	int nCost;
	ar >> nCost;

	CWndStatuePurchase* pWnd = (CWndStatuePurchase*)g_WndMng.GetApplet(APP_STATUE_PURCHASE);
	if (pWnd)
		pWnd->SetBuff(nCost);
}
#endif

#ifdef __FL_FFA
void CDPClient::OnFFAData(CAr& ar)
{
	CWndActivities* pWnd = (CWndActivities*)g_WndMng.CreateApplet(APP_PVP_ACTIVITIES);
	if (pWnd)
		pWnd->m_wndFFA.AcceptData(ar);
}
void CDPClient::OnFFAState(CAr& ar)
{
	if (!g_WndMng.m_pWndWorld)
		return;

	BYTE nState;
	time_t time;
	BOOL bPlayerList;

	size_t nPlayerCount;
	u_long uPlayerID;
	int nKills;
	int nDeaths;

	ar >> nState;
	ar >> time;
	ar >> bPlayerList;

	if (bPlayerList)
	{
		g_WndMng.m_pWndWorld->vecPlayerFFA.clear();

		ar >> nPlayerCount;
		for (size_t i = 0; i < nPlayerCount; i++)
		{
			ar >> uPlayerID;
			ar >> nKills;
			ar >> nDeaths;

			FFA_PLAYER tmp;
			tmp.idPlayer = uPlayerID;
			tmp.nKills = nKills;
			tmp.nDeaths = nDeaths;
			g_WndMng.m_pWndWorld->vecPlayerFFA.push_back(tmp);
		}

		g_WndMng.m_pWndWorld->SortFFA();
	}

	g_WndMng.m_pWndWorld->SetStateFFA(nState);
	g_WndMng.m_pWndWorld->SetTimeFFA(time);
}
void CDPClient::OnFFAClear()
{
	CWndFFAResult* pWnd = (CWndFFAResult*)g_WndMng.GetApplet(APP_FFA_RESULT);
	if (!pWnd)
		g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_FFA_RESULT);

	if (g_WndMng.m_pWndWorld)
		g_WndMng.m_pWndWorld->ClearFFA();
}
void CDPClient::OnFFAKill(CAr& ar)
{
	u_long uAttackerID;
	u_long uDefenderID;

	ar >> uAttackerID;
	ar >> uDefenderID;

	if (!g_WndMng.m_pWndWorld)
		return;

	for (vector<FFA_PLAYER>::iterator it = g_WndMng.m_pWndWorld->vecPlayerFFA.begin(); it != g_WndMng.m_pWndWorld->vecPlayerFFA.end(); it++)
	{
		if (it->idPlayer == uDefenderID)
			it->nDeaths++;

		if (it->idPlayer == uAttackerID)
			it->nKills++;
	}

	g_WndMng.m_pWndWorld->SortFFA();

	CString str;
	str.Format("%s has killed %s", CPlayerDataCenter::GetInstance()->GetPlayerString(uAttackerID), CPlayerDataCenter::GetInstance()->GetPlayerString(uDefenderID));
	g_WndMng.PutString(str);
}
#endif

#ifdef __FL_DAILY_REWARDS
void CDPClient::OnDailyReward(CAr& ar)
{
	CWndDailyGift* pWndDailyGift = (CWndDailyGift*)g_WndMng.CreateApplet(APP_DAILY_REWARD);
	if (pWndDailyGift)
		pWndDailyGift->SetData(ar);
}
#endif

#ifdef __MODEL_HIDEFASH
void CDPClient::SendHideCoat(int nPosition, BOOL bHide)
{
	BEFORESENDSOLE(ar, PACKETTYPE_HIDE_COAT, DPID_UNKNOWN);
	ar << nPosition << bHide;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnHideFashion(OBJID objid, CAr& ar)
{
	int nPos;
	BOOL bState;
	ar >> nPos;
	ar >> bState;

	CMover* pMover = prj.GetMover(objid);
	if (IsValidObj(pMover))
		pMover->m_bHideFashion[nPos] = bState;
}
#endif

#ifdef __WIKI_ITEMS_V2
void CDPClient::SendWikiCreate(DWORD dwId, int nMode)
{
	BEFORESENDSOLE(ar, PACKETTYPE_WIKI_CREATE, DPID_UNKNOWN);
	ar << dwId << nMode;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

#ifdef __MODEL_CHANGE
void CDPClient::SendLookChange(DWORD dwIdSrc, DWORD dwIdDest)
{
	BEFORESENDSOLE(ar, PACKETTYPE_LOOKCHANGE, DPID_UNKNOWN);
	ar << dwIdSrc << dwIdDest;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPClient::OnLookChange(CAr& ar)
{
	DWORD dwID, dwLook;
	ar >> dwID >> dwLook;

	CItemElem* pItem = g_pPlayer->m_Inventory.GetAtId(dwID);
	if (!pItem)
		return;

	pItem->SetLook(dwLook);
}
#endif

#ifdef __FL_TELEPORT
void CDPClient::SendTeleportRequest(int nKey)
{
	BEFORESENDSOLE(ar, PACKETTYPE_TELEPORT_REQUEST, DPID_UNKNOWN);
	ar << nKey;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

#ifdef __JOB_CHANGE
void CDPClient::SendJobChange(int nJob)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SEND_JOB_CHANGE, DPID_UNKNOWN);
	ar << nJob;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

#ifdef __FL_RECYCLE
void CDPClient::SendRecycleRequest(DWORD dwItem)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SEND_RECYCLE, DPID_UNKNOWN);
	ar << dwItem;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

void CDPClient::OnClearCastingSkills(OBJID objid, CAr& ar)
{
	CMover* pMover = prj.GetMover(objid);
	if (!IsValidObj(pMover))
		return;

	pMover->ClearCastingSkills();
}

#ifdef __FL_PARTY_FINDER
void CDPClient::OnPartyFinder(CAr& ar)
{
	CWndPartyFinder* pWnd = (CWndPartyFinder*)g_WndMng.CreateApplet(APP_PARTY_FINDER);
	if (pWnd)
		pWnd->ReceiveList(ar);
}
void CDPClient::OnPartyChangeJoinMode(CAr& ar)
{
	ar >> g_Party.m_bAllowed;
}
void CDPClient::SendJoinPartyFinder(u_long uPartyId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_REQUEST_PARTY_FINDER_JOIN, DPID_UNKNOWN);
	ar << uPartyId;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

#ifdef __FL_CASH_SHOP
void CDPClient::OnCShop(CAr& ar)
{
	CFlyFFShop::GetInstance()->SerializeFlyffShop(ar);

	CWndFlyFFShop* pWndShop = (CWndFlyFFShop*)g_WndMng.GetWndBase(APP_CASH_SHOP);
	if (pWndShop)
	{
		pWndShop->UpdateContent();
		SAFE_DELETE(pWndShop->m_pWndPurchaseItem);
		g_WndMng.PutString("Shop was just updated.");
	}
}
void CDPClient::OnCShopBalance(CAr& ar)
{
	ar >> g_Neuz.m_nDonatePoints >> g_Neuz.m_nVotePoints;

	CWndFlyFFShop* pWndShop = (CWndFlyFFShop*)g_WndMng.GetWndBase(APP_CASH_SHOP);
	if (pWndShop)
		pWndShop->UpdatePoints();
}
void CDPClient::SendBuyCShopItem(int nItemUniqueID, int nItemAmount, BOOL bDonate, u_long idReceiver)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CSHOP_ITEM, DPID_UNKNOWN);
	ar << nItemUniqueID << nItemAmount << bDonate;
	ar << idReceiver;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

#ifdef __FL_NEW_BOXES
void CDPClient::SendUseNewBox(DWORD dwObjId, int nCurSel)
{
	BEFORESENDSOLE(ar, PACKETTYPE_NEWBOX_OPEN, DPID_UNKNOWN);
	ar << dwObjId << nCurSel;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

void CDPClient::SendUseTransy(DWORD dwObjId, int nSel)
{
	BEFORESENDSOLE(ar, PACKETTYPE_USE_TRANSY, DPID_UNKNOWN);
	ar << dwObjId << nSel;
	SEND(ar, this, DPID_SERVERPLAYER);
}

#ifdef __FL_SWITCH_EQ
void CDPClient::SendSwitchAdd(int nSel, const char* lpszName, DWORD dwLooter, DWORD dwPet, DWORD dwRegen)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SWITCH_ADD, DPID_UNKNOWN);
	ar << nSel;
	ar.WriteString(lpszName);
	ar << dwLooter;
	ar << dwPet;
	ar << dwRegen;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendSwitchDelete(int nSel)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SWITCH_DELETE, DPID_UNKNOWN);
	ar << nSel;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendSwitchUse(int nSel)
{
	CWndSwitchEquip* pWnd = (CWndSwitchEquip*)g_WndMng.GetWndBase(APP_EQ_SWITCH);
	if (pWnd)
		pWnd->SetUsedPreset(nSel);

	BEFORESENDSOLE(ar, PACKETTYPE_SWITCH_USE, DPID_UNKNOWN);
	ar << nSel;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnSwitchAdd(CAr& ar)
{
	int nSel;
	char lpszName[32] = { 0, };
	DWORD dwLooter, dwPet, dwRegen;

	ar >> nSel;
	ar.ReadString(lpszName, 32);
	ar >> dwLooter;
	ar >> dwPet;
	ar >> dwRegen;

	if (g_pPlayer->m_Switches.AddSwitch(nSel, lpszName, dwLooter, dwPet, dwRegen, TRUE))
	{
		CWndSwitchEquip* pWnd = (CWndSwitchEquip*)g_WndMng.GetWndBase(APP_EQ_SWITCH);
		if (nSel == -1)
		{
			g_WndMng.PutString("Preset added! Now you can add shortcut to the taskbar.");
			if (pWnd)
				pWnd->ResetListbox();
		}
		else
		{
			g_WndMng.PutString("Preset modified!");
			if (pWnd)
				pWnd->UpdateParts();
		}
	}
}
void CDPClient::OnSwitchDelete(CAr& ar)
{
	int nSel;
	ar >> nSel;

	if (g_pPlayer->m_Switches.DeleteSwitch(nSel))
	{
		g_WndMng.PutString("Preset deleted.");

		CWndSwitchEquip* pWnd = (CWndSwitchEquip*)g_WndMng.GetWndBase(APP_EQ_SWITCH);
		if (pWnd)
			pWnd->ResetListbox();
	}
}
void CDPClient::OnSwitchesAll(CAr& ar)
{
	g_pPlayer->m_Switches.Serialize(ar);

	CWndSwitchEquip* pWnd = (CWndSwitchEquip*)g_WndMng.GetWndBase(APP_EQ_SWITCH);
	if (pWnd)
		pWnd->ResetListbox();
}
#endif

#ifdef __FL_GUILD_FINDER
void CDPClient::OnGuildFinder(CAr& ar)
{
	CWndGuildFinder* pWnd = (CWndGuildFinder*)g_WndMng.CreateApplet(APP_GUILD_FINDER);
	if (pWnd)
		pWnd->ReceiveList(ar);
}
void CDPClient::SendGuildFinderState(u_long uGuildId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_FINDERSTATE, DPID_UNKNOWN);
	ar << uGuildId;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnGuildFinderState(CAr& ar)
{
	BOOL bFinder;
	ar >> bFinder;

	CGuild* pGuild = g_pPlayer->GetGuild();
	if (!pGuild)
		return;

	pGuild->m_bFinder = bFinder;
}
void CDPClient::SendGuildFinderJoin(u_long uGuildId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_FINDERJOIN, DPID_UNKNOWN);
	ar << uGuildId;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::SendRequestGuildFinderAddon(u_long uGuildId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_FINDER_ADDON, DPID_UNKNOWN);
	ar << uGuildId;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnGuildFinderAddon(CAr& ar)
{
	CWndGuildFinder* pWnd = (CWndGuildFinder*)g_WndMng.CreateApplet(APP_GUILD_FINDER);
	if (pWnd)
		pWnd->ReceiveAdditionalInfo(ar);
}
#endif

#ifdef __FL_PACKED_ITEMS
void CDPClient::SendPackageOpen(DWORD dwObjId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PACKAGE_OPEN, DPID_UNKNOWN);
	ar << dwObjId;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnPackageOpen(CAr& ar)
{
	DWORD dwObjId;
	ar >> dwObjId;

	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(dwObjId);
	if (!pItemElem)
		return;

	pItemElem->ClearPackedItems();

	CWndPackedItems* pWnd = (CWndPackedItems*)g_WndMng.GetWndBase(APP_PACKED_ITEMS);
	if (pWnd && pWnd->GetItem() == pItemElem)
		pWnd->RefreshListBox();
}
void CDPClient::SendPackageAdd(DWORD dwPackObjId, DWORD dwItemObjId)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PACKAGE_ADD, DPID_UNKNOWN);
	ar << dwPackObjId << dwItemObjId;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnPackageAdd(CAr& ar)
{
	DWORD dwPackObjId, dwItemObjId;
	ar >> dwPackObjId >> dwItemObjId;

	CItemElem* pPackageElem = g_pPlayer->m_Inventory.GetAtId(dwPackObjId);
	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(dwItemObjId);
	if (!pPackageElem || !pItemElem)
		return;

	pPackageElem->AddToPack(pItemElem);

	CWndPackedItems* pWnd = (CWndPackedItems*)g_WndMng.GetWndBase(APP_PACKED_ITEMS);
	if (pWnd && pWnd->GetItem() == pPackageElem)
		pWnd->RefreshListBox();
}
void CDPClient::SendPackageRestore(DWORD dwObjId, int nCurSel)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PACKAGE_RESTORE, DPID_UNKNOWN);
	ar << dwObjId << nCurSel;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnPackageRestore(CAr& ar)
{
	DWORD dwObjId;
	int nCurSel;

	ar >> dwObjId >> nCurSel;

	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(dwObjId);
	if (!pItemElem)
		return;

	pItemElem->DeleteFromPack(nCurSel);

	CWndPackedItems* pWnd = (CWndPackedItems*)g_WndMng.GetWndBase(APP_PACKED_ITEMS);
	if (pWnd && pWnd->GetItem() == pItemElem)
		pWnd->RefreshListBox();
}
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
void CDPClient::OnWof(CAr& ar)
{
	CWheelOfFortune::GetInstance()->Serialize(ar);

	bool bActive = CWheelOfFortune::GetInstance()->IsActive();

	if (g_WndMng.m_pWndWorld)
	{
		if (bActive)
		{
			g_WndMng.m_pWndWorld->GetAdvMgr()->AddAdvButton(APP_WOF);
			g_WndMng.PutString("Wheel of Fortune is available now.");
		}
		else
			g_WndMng.m_pWndWorld->GetAdvMgr()->DeleteButton(APP_WOF);
	}

	CWndWof* pWnd = (CWndWof*)g_WndMng.GetWndBase(APP_WOF);
	if (pWnd)
	{
		if (!bActive)
			pWnd->Destroy();
		else
			pWnd->ResetListChances();
	}
}
void CDPClient::OnWofInit(CAr& ar)
{
	int nPrize;
	ar >> nPrize;

	CWndWof* pWnd = (CWndWof*)g_WndMng.GetWndBase(APP_WOF);
	if (pWnd)
		pWnd->SetState(CWndWof::STATE_INIT, nPrize);
}
void CDPClient::OnWofFinish(CAr& ar)
{
	WOF_ITEM item;
	ar >> item.dwItemId;
	ar >> item.nItemCount;

	CWheelOfFortune::GetInstance()->AddRecentReward(item);

	CWndWof* pWnd = (CWndWof*)g_WndMng.GetWndBase(APP_WOF);
	if (pWnd)
		pWnd->SetState(CWndWof::STATE_FINISH);
}
#endif
#ifdef __FL_RECORD_BOOK
void CDPClient::OnRecordBook(CAr& ar)
{
	CRecordBook::GetInstance()->SerializeAllRecords(ar);

	CWndRecordBook* pWnd = (CWndRecordBook*)g_WndMng.GetWndBase(APP_RECORD_BOOK);
	if (pWnd)
		pWnd->RefreshListBox();
}
#endif
#ifdef __FL_MONSTER_ALBUM
void CDPClient::SendMABossCardSlotRestore(DWORD dwKeyObjId, int nKeyLocation, int nSlot)
{
	BEFORESENDSOLE(ar, PACKETTYPE_MA_BOSSCARDSLOT_RESTORE, DPID_UNKNOWN);
	ar << dwKeyObjId << nKeyLocation << nSlot;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPClient::OnMAAddCardPieces(CAr& ar)
{
	DWORD dwMobID;
	int nAddedPieces;

	ar >> dwMobID;
	ar >> nAddedPieces;

	g_pPlayer->m_MonsterAlbum.AddMonsterCardPiece(dwMobID, nAddedPieces);
}
void CDPClient::OnMABossCardSlotUnlock(CAr& ar)
{
	int nSlot;

	ar >> nSlot;

	g_pPlayer->m_MonsterAlbum.BossCardSlotUnlock(nSlot);
}
void CDPClient::OnMABossCardSlotInsert(CAr& ar)
{
	int nSlot;
	DWORD dwBossCardID;

	ar >> nSlot;
	ar >> dwBossCardID;

	g_pPlayer->m_MonsterAlbum.BossCardSlotInsert(nSlot, dwBossCardID);
}
void CDPClient::OnMABossCardSlotRestore(CAr& ar)
{
	int nSlot;

	ar >> nSlot;

	g_pPlayer->m_MonsterAlbum.BossCardSlotRestore(nSlot);
}
#endif

#ifdef __FL_TDM
void CDPClient::OnTDMData(CAr& ar)
{
	CWndActivities* pWnd = (CWndActivities*)g_WndMng.CreateApplet(APP_PVP_ACTIVITIES);
	if (pWnd)
		pWnd->m_wndTDM.AcceptData(ar);
}
void CDPClient::OnTDMRegister(CAr& ar)
{
	u_long idPlayer;
	char pszPlayer[MAX_PLAYER] = { 0, };

	ar >> idPlayer;
	ar.ReadString(pszPlayer, MAX_PLAYER);

	CWndActivities* pWnd = (CWndActivities*)g_WndMng.GetWndBase(APP_PVP_ACTIVITIES);
	if (pWnd)
		pWnd->m_wndTDM.RegisterPlayer(idPlayer);

	CString str;
	if (g_pPlayer && g_pPlayer->m_idPlayer == idPlayer)
		str.Format("You just registered on team deathmatch!");
	else
		str.Format("%s just registered on team deathmatch!", pszPlayer);
	g_WndMng.PutString(str);
}
void CDPClient::OnTDMState(CAr& ar)
{
	BYTE nState;
	time_t time;
	bool bPlayerList;

	ar >> nState;
	ar >> time;
	ar >> bPlayerList;

	if (bPlayerList)
	{
		g_WndMng.m_pWndWorld->m_vecPlayersTDM.clear();
		g_WndMng.m_pWndWorld->m_mapPlayersTDM.clear();
		g_WndMng.m_pWndWorld->m_mapTeamsTDM.clear();

		size_t nPlayerCount;
		u_long uPlayerID;
		TDM_TEAMS team;
		int nKills;
		int nDeaths;

		ar >> nPlayerCount;
		for (size_t i = 0; i < nPlayerCount; i++)
		{
			ar >> uPlayerID;
			ar >> team;
			ar >> nKills;
			ar >> nDeaths;

			TDM_PLAYER tmp;
			tmp.idPlayer = uPlayerID;
			tmp.team = team;
			tmp.nKills = nKills;
			tmp.nDeaths = nDeaths;

			g_WndMng.m_pWndWorld->m_vecPlayersTDM.push_back(tmp);
			g_WndMng.m_pWndWorld->m_mapPlayersTDM.insert(make_pair(uPlayerID, tmp));
		}

		g_WndMng.m_pWndWorld->SortTDM();

		size_t nTeamsCount;
		TDM_TEAMS tm;
		int nTeamPoints;

		ar >> nTeamsCount;
		for (size_t i = 0; i < nTeamsCount; i++)
		{
			ar >> tm;
			ar >> nTeamPoints;

			g_WndMng.m_pWndWorld->m_mapTeamsTDM.insert(make_pair(tm, nTeamPoints));
		}
	}

	g_WndMng.m_pWndWorld->SetStateTDM(nState);
	g_WndMng.m_pWndWorld->SetTimeTDM(time);
}
void CDPClient::OnTDMClear()
{
	CWndTDMResult* pWnd = (CWndTDMResult*)g_WndMng.CreateApplet(APP_TDM_RESULT);

	if (g_WndMng.m_pWndWorld)
		g_WndMng.m_pWndWorld->ClearTDM();
}
void CDPClient::OnTDMKill(CAr& ar)
{
	u_long uAttackerID;
	u_long uDefenderID;

	ar >> uAttackerID;
	ar >> uDefenderID;

	bool bFoundKiller = false;
	bool bFoundDefender = false;

	for (auto& it : g_WndMng.m_pWndWorld->m_vecPlayersTDM)
	{
		if (it.idPlayer == uAttackerID)
		{
			it.nKills++;
			g_WndMng.m_pWndWorld->m_mapTeamsTDM[it.team]++;
			bFoundKiller = true;
		}

		if (it.idPlayer == uDefenderID)
		{
			it.nDeaths++;
			bFoundDefender = true;
		}

		if (bFoundKiller && bFoundDefender)
			break;
	}

	g_WndMng.m_pWndWorld->SortTDM();

	auto itAttacker = g_WndMng.m_pWndWorld->m_mapPlayersTDM.find(uAttackerID);
	if (itAttacker != g_WndMng.m_pWndWorld->m_mapPlayersTDM.end())
		itAttacker->second.nKills++;

	auto itDefender = g_WndMng.m_pWndWorld->m_mapPlayersTDM.find(uDefenderID);
	if (itDefender != g_WndMng.m_pWndWorld->m_mapPlayersTDM.end())
		itDefender->second.nDeaths++;

	CString str;
	str.Format("%s has killed %s and gained for his team 1 point!", CPlayerDataCenter::GetInstance()->GetPlayerString(uAttackerID), CPlayerDataCenter::GetInstance()->GetPlayerString(uDefenderID));
	g_WndMng.PutString(str);
}
#endif

CDPClient	g_DPlay;