#ifndef __DPCLIENT_H__
#define __DPCLIENT_H__

#include "DPMng.h"
#include "Ar.h"

#include "Obj.h"
#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 13 // __HOUSING
#include "Housing.h"
#endif // __HOUSING

#undef	theClass
#define	theClass	CDPClient
#undef theParameters
#define theParameters	CAr & ar

typedef	struct	tagPLAYERPOS
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3	vDelta;
	BOOL fValid;
}
PLAYERPOS, * LPPLAYERPOS;

typedef	struct	tagPLAYERDESTPOS
{
	D3DXVECTOR3 vPos;
	BYTE	fForward;
	BOOL	fValid;
	FLOAT	d;
}
PLAYERDESTPOS, * LPPLAYERDESTPOS;

typedef	struct	tagPLAYERMOVINGACTMSG
{
	D3DXVECTOR3 vPos;
	BYTE	fMoving;
	BYTE	fForward;
	DWORD	dwMsg;
	int		nParam1;
	int		nParam2;
	float	fAngle;
	BOOL	fValid;
}
PLAYERMOVINGACTMSG, * LPPLAYERMOVINGACTMSG;

typedef	struct	tagPLAYERANGLE
{
	BOOL	fValid;
	int		nCounter;
}
PLAYERANGLE, * PPLAYERANGLE;

typedef struct tagSNAPSHOT
{
	PLAYERPOS	playerpos;
	PLAYERDESTPOS	playerdestpos;
	PLAYERMOVINGACTMSG	playermovingactmsg;
	u_long	uFrameMove;
}
SNAPSHOT, * LPSNAPSHOT;

class CDPClient : public CDPMng
{
private:
	SNAPSHOT	m_ss;
	LONG		m_lError;
	PLAYERANGLE		m_pa;

public:
	BOOL	m_fConn;
	BYTE* m_pDump;
	int		m_nDumpSize;
	int		m_nMaxLoginGuild;
	u_long  m_uLoginPlayerIdGuild[200];
	u_long  m_uLoginGuildMulti[200];
	DWORD	m_dwReturnScroll;

public:

	//	Constructions
	CDPClient();
	virtual	~CDPClient();
	//	Override
	virtual	void SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom);
	virtual void UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom);
#ifdef __TEST_PACKET_CRYPT
	virtual BOOL Send(LPVOID lpData, DWORD dwDataSize, DPID dpidTo);
#endif

	//	Operations
	BOOL	Connect(LPSTR lpszAddr, USHORT uPort);
	//	Writes
	void	SendHdr(DWORD dwHdr);
	LONG	GetErrorCode() { return m_lError; }
	LONG	GetNetError();
	void	SendJoin(BYTE nSlot, DWORD dwWorldID, CMover* pMover, CRTMessenger* pRTMessenger, u_long uIdofMulti);

	void	PostPlayerAngle(BOOL f);
	void	FlushPlayerAngle(void);
	void	SendBlock(BYTE Gu, const char* szFrom);
	void	SendSkillFlag(int nSkill);
	void	SendChat(LPCSTR lpszChat);
	void	SendRemoveItem(CItemElem* pItemElem, int nNum);
	void	SendQueryPostMail(BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText);
	void	SendQueryRemoveMail(u_long nMail);
	void	SendQueryGetMailItem(u_long nMail);
	void	SendQueryGetMailGold(u_long nMail);
	void	SendQueryReadMail(u_long nMail);
	void	SendQueryMailBox(void);
	void	SendMoveItem(BYTE nItemType, BYTE nSrcIndex, BYTE nDestIndex);
	void	SendDropItem(DWORD dwItemType, DWORD dwItemId, short nITemNum, const D3DXVECTOR3& vPos);
	void	SendDropGold(DWORD dwGold, const D3DXVECTOR3& vPlayerPos, const D3DXVECTOR3& vPos);

	void	SendConfirmTrade(CMover* pTrader);
	void	SendConfirmTradeCancel(OBJID objid);
	void	SendTrade(CMover* pTrader);
	void	SendTradePut(BYTE i, DWORD dwItemObjID, int ItemNum = 1);
	void	SendTradePull(BYTE byNth);
	void	SendTradeOk(void);
	void	SendTradeCancel(int nMode = 0);
	void	SendTradeConfirm(void);
	void	SendTradePutGold(DWORD dwGold);

#ifndef __FL_VENDORS
	void	SendPVendorOpen(const char* szVendor);
	void	SendRegisterPVendorItem(BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost);
	void	SendUnregisterPVendorItem(BYTE iIndex);
#else
	void	SendPVendorOpen(const char* szVendor, BOOL bOwnShop, DWORD dwObjId);
	void	SendRegisterPVendorItem(BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost, int nPerinCost, BOOL bOwnShop = TRUE);
	void	SendUnregisterPVendorItem(BYTE iIndex, BOOL bOwnShop = TRUE);
	void    OnVendorActivate(CAr& ar);
	void    OnVendorDeactivate(CAr& ar);
	void	SendVendorAction(BYTE nType);
	void	OnVendorPreview(CAr& ar);
	void	OnVendorAddNewItem(OBJID objid, CAr& ar);
#endif

	void	SendPVendorClose(OBJID objidVendor);
	void	SendQueryPVendorItem(OBJID objidVendor);
	void	SendBuyPVendorItem(OBJID objidVendor, BYTE nItem, short nNum);
	void	OnUseObject(OBJID objid, CAr& ar);
	void	SendSetHair(BYTE nHair, float r, float g, float b);	//, int nCost );
	void	SendPartySkillUse(int nSkill);
	void	SendPartyMemberCancle(u_long uLeader, u_long uMember, int nMode = 0);
	void	SendPartyMemberRequest(u_long uMemberId, BOOL bTroup);
	void	SendAddPartyMember(u_long uLeader);
	void	SendRemovePartyMember(u_long MemberId);
	void	SendChangeShareItem(int nItemMode);
	void	SendChangeShareExp(int nExpMode);

	void	SendChangeTroup(BOOL bSendName, const char* szPartyName = "");
	void	SendChangePartyName(const char* szPartyName);
	void	SendPartyChangeLeader(u_long uChangerLeaderid);
	void	SendChangeFace(DWORD dwFaceNum);

	void	SendScriptDialogReq(OBJID objid, LPCTSTR lpKey, int nGlobal1, int nGlobal2, int nGlobal3, int nGlobal4);
	void	SendOpenShopWnd(OBJID objid);
	void	SendCloseShopWnd(void);
	void	SendBuyItem(CHAR cTab, DWORD dwItemObjID, short nNum);
	void	SendBuyChipItem(CHAR cTab, DWORD dwItemObjID, short nNum);
	void	SendSellItem(DWORD dwId, int nNum);
	void	SendMeleeAttack(OBJMSG dwAtkMsg, OBJID objid);
	void	SendMeleeAttack2(OBJID objid);
	void	SendMagicAttack(OBJID objid, int nMagicPower, int idSfxHit);
	void	SendRangeAttack(OBJID objid, DWORD dwItemID, int idSfxHit);

	void	SendUseSkill(WORD wType, WORD wId, OBJID objid, int nUseType = 0, int bControl = FALSE);
	void	SendUseObject(OBJID objid);
	void	SendSfxID(OBJID idTarget, int idSfxHit, DWORD dwType, DWORD dwSkill = NULL_ID, int nMaxDmgCnt = 1);
	void	SendSetTarget(OBJID idTarget, BYTE bClear);
	void	SendTeleSkill(OBJID objid, D3DXVECTOR3 vPos);

	void	SendExp(EXPINTEGER nExp);

	void	SendOpenBankWnd(DWORD dwId, DWORD dwItemId);

	void	SendFocusObj();

	void	SendOpenGuildBankWnd();
	void	SendCloseGuildBankWnd();
	void	SendCloseBankWnd(void);

	void	SendDoUseSkillPoint(SKILL aJobSkill[]);
	void	SendEnterChattingRoom(u_long uidChatting);
	void	SendChatting(char* pszChat);
	void	SendOpenChattingRoom(void);
	void	SendCloseChattingRoom(void);

	void	SendPutItemGuildBank(DWORD dwItemObjID, int ItemNum, BYTE p_Mode);
	void	SendGetItemGuildBank(DWORD dwItemObjID, int ItemNum, BYTE p_Mode);
	void	SendPutItemBank(BYTE nSlot, DWORD dwItemObjID, int ItemNum);
	void	SendGetItemBank(BYTE nSlot, DWORD dwItemObjID, int ItemNum);
	void	SendPutItemBankToBank(BYTE nPutSlot, BYTE nSlot, DWORD dwItemObjID, int ItemNum);
	void	SendPutGoldBank(BYTE nSlot, DWORD dwGold);
	void	SendGetGoldBank(BYTE nSlot, DWORD dwGold);
	void	SendPutGoldBankToBank(BYTE nPutSlot, BYTE nSlot, DWORD dwGold);
	void	SendStateModeCancel(DWORD dwStateMode, BYTE nFlag);


	void	SendMoveBankItem(BYTE nSrcIndex, BYTE nDestIndex);

	void	SendChangeBankPass(const char* szLastPass, const char* szNewPass, DWORD dwId, DWORD dwItemId);
	void	SendConfirmBank(const char* szPass, DWORD dwId, DWORD dwItemId);

	void	SendCorrReq(OBJID idObj);
	//________________________________________________________________________________
	void	SendPlayerBehavior(void);
	void	SendPlayerDestObj(OBJID objid, float fRange = 0.0f);
	void	SendSkillTaskBar(void);
	void	SendRemoveAppletTaskBar(BYTE nIndex);
	void	SendAddAppletTaskBar(BYTE nIndex, LPSHORTCUT pAppletShortcut);
	void	SendRemoveItemTaskBar(BYTE nSlotIndex, BYTE nIndex);
	void	SendAddItemTaskBar(BYTE nSlotIndex, BYTE nIndex, LPSHORTCUT pItemShortcut);
	void	SendAddFriend(u_long uidPlayer);
	void	SendAddFriendReqest(u_long uidPlayer);
	void	SendAddFriendNameReqest(const char* szName);
	void	SendFriendCancel(u_long uidLeader);

#ifdef __JEFF_11
	void	SendQuePetResurrection(int nItem);
#endif	// __JEFF_11
	void	SendGetFriendState();
	void	SendSetState(int state);
	void	SendFriendInterceptState(u_long uidPlayer);
	void	SendRemoveFriend(u_long uidPlayer);

	void	SendRandomScroll(OBJID objid, OBJID objid2);
	void	SendEnchant(OBJID objid, OBJID objMaterialId);
	void	SendRemoveAttribute(OBJID objid);
	void	SendItemTransy(OBJID objid0, OBJID objid1, DWORD dwChangeId = NULL_ID, BOOL bCash = TRUE);
	void	SendPiercing(OBJID objid1, OBJID objid2);
	void	SendPiercingRemove(OBJID objid, int nSlot);
	void	SendCreateSfxObj(DWORD dwSfxId, u_long idPlayer = NULL_ID, BOOL bFlag = FALSE);
	void	SendSetNaviPoint(const D3DXVECTOR3& Pos, OBJID objidTarget);
	void	OnSetNaviPoint(OBJID objid, CAr& ar);
	void	SendGuildInvite(OBJID objid);
	void	SendIgnoreGuildInvite(u_long idPlayer);
	void	SendDestroyGuild();
	void	SendAddGuildMember(u_long idMaster);
	void	SendRemoveGuildMember(u_long idPlayer);
	void	SendGuildContribution(BYTE cbPxpCount, int nGold, BYTE cbItemFlag = 0);
	void    SendGuildNotice(const char* szNotice);
	void	SendGuildAuthority(u_long uGuildId, DWORD dwAuthority[]);
	void	SendGuildSetName(LPCTSTR szName);
	void	SendGuildMemberLv(u_long idMaster, u_long idPlayer, int nMemberLv);
	void	UpdateGuildWnd();
	void	SendGuildClass(u_long idMaster, u_long idPlayer, BYTE nFlag);
	void	SendGuildNickName(u_long idPlayer, LPCTSTR strNickName);
	void	SendChgMaster(u_long idPlayer, u_long idPlayer2);
	void	OnChgMaster(CAr& ar);

	void	OnRemoveSkillInfluence(OBJID objid, CAr& ar);
	void	OnRemoveAllSkillInfluence(OBJID objid, CAr& ar);

	//________________________________________________________________________________

	void	SendDoUseItem(DWORD dwItemId, char* szInput = "", int nType = TYPE_INVENTORY);
	void	SendDoUseItemTarget(DWORD dwMaterial, DWORD dwTarget);
	void	SendRemoveItemLevelDown(DWORD dwId);
	void	SendBlessednessCancel(int nItem);

	void	SendSnapshot(BOOL fUnconditional = FALSE);
	void	PutPlayerDestPos(CONST D3DXVECTOR3& vPos, bool bForward, BYTE f = 0);
	void	SendSfxHit(int idSfxHit, DWORD dwSkill = NULL_ID, OBJID idAttacker = NULL_ID);
	void	SendSfxClear(int idSfxHit, OBJID idMover = NULL_ID);
	void	SendQuerySetPlayerName(DWORD dwData, const char* lpszPlayer);
	void	SendSummonFriend(DWORD dwData, const char* lpszPlayer);
	void	SendSummonFriendConfirm(OBJID objid, DWORD dwData);
	void	SendSummonFriendCancel(OBJID objid, DWORD dwData);
	void	SendSummonParty(DWORD dwData);
	void	SendSummonPartyConfirm(OBJID objid, DWORD dwData);
	void	SendPetRelease(void);
	void	SendUsePetFeed(DWORD dwFeedId);
	void	SendMakePetFeed(DWORD dwMaterialId, short nNum, DWORD dwToolId);
	void	SendPetTamerMistake(DWORD dwId);
	void	SendPetTamerMiracle(DWORD dwId);
	void	SendFeedPocketInactive(void);
	void	SendLegendSkillUp(OBJID* pdwItemId, int count);
	void	SendModifyStatus(int nStrCount, int nStaCount, int nDexCount, int nIntCount);
	void	SendRemoveQuest(DWORD dwQuest);
	void	SendReqLeave();
	void	SendResurrectionOK();
	void	SendResurrectionCancel();
	void	SendDoEscape(void);
	void	SendCheering(OBJID objid);
	void	OnSetCheerParam(OBJID objid, CAr& ar);
	void	SendQuerySetGuildName(LPCSTR pszGuild, BYTE nId);
	void	SendQueryEquip(OBJID objid);
	void	SendQueryEquipSetting(BOOL bAllow);
	void	SendReturnScroll(int nSelect);
	void	SendEndSkillQueue(void);
	void	SendGuildCombatWindow(void);
	void	SendGuildCombatApp(DWORD dwPenya);
	void	SendGuildCombatCancel(void);
	void	SendGCRequestStatusWindow(void);
	void	SendGCSelectPlayerWindow(void);
	void	SendGCSelectPlayer(vector<u_long> vecSelectPlayer, u_long uidDefender);
	void	SendGCJoin(void);
	void	SendGCTele(void);
	void	SendGCPlayerPoint(void);
	void	SendCreateMonster(DWORD dwItemId, D3DXVECTOR3 vPos);

#ifdef __EVE_MINIGAME
	void SendKawibawiboStart();
	void SendKawibawiboGetItem();
#endif // __EVE_MINIGAME

	void OnUltimateWeapon(OBJID objid, CAr& ar);
	void OnUltimateMakeItem(OBJID objid, CAr& ar);
	void OnUltimateMakeGem(OBJID objid, CAr& ar);
	void OnUltimateSetGem(OBJID objid, CAr& ar);
	void OnUltimateRemoveGem(OBJID objid, CAr& ar);
	void OnUltimateEnchantWeapon(OBJID objid, CAr& ar);

	void SendUltimateMakeItem(OBJID objSunId, OBJID objMoonId);
	void SendUltimateMakeGem(OBJID objItemId);
	void SendUltimateTransWeapon(OBJID objItemWeapon, OBJID objItemGem1, OBJID objItemGem2, OBJID objItemScroll);
	void SendUltimateSetGem(OBJID objItemWeapon, OBJID objItemGem);
	void SendUltimateRemoveGem(OBJID objItemWeapon, OBJID objItemGem, int nSlot2);
	void SendUltimateEnchantWeapon(OBJID objItemWeapon, OBJID objItemGem);


#ifdef __TRADESYS
	void OnExchange(OBJID objid, CAr& ar);
	void OnExchangeResult(CAr& ar);
	void SendExchange(int nMMIId, int nListNum);
#endif // __TRADESYS

	void	SendReqGuildBankLogList(BYTE byListType);
	void	OnGuildBankLogList(CAr& ar);

	void	SendReqHonorList();
	void	SendReqHonorTitleChange(int nChange);
	void	OnHonorListAck(CAr& ar);
	void	OnHonorChangeAck(OBJID objid, CAr& ar);


	USES_PFNENTRIES;
	void	OnGetClock(CAr& ar);
	void	OnKeepAlive(CAr& ar);
	void	OnError(CAr& ar);
	void	OnSnapshot(CAr& ar);
	void	OnJoin(CAr& ar);
	void	OnReplace(CAr& ar);
	void	OnWhisper(CAr& ar);
	void	OnSay(CAr& ar);
	void	OnGMSay(CAr& ar);
	//	Snapshot handlers
	void	OnAddObj(OBJID objid, CAr& ar);
	void	OnRemoveObj(OBJID objid);
	void	OnChat(OBJID objid, CAr& ar);
	void	OnEventMessage(OBJID objid, CAr& ar);
	void	OnDamage(OBJID objid, CAr& ar);
	void	OnMoverDeath(OBJID objid, CAr& ar);
	void	OnCreateItem(OBJID objid, CAr& ar);
	void	OnMoveItem(CAr& ar);
	void	OnDoEquip(OBJID objid, CAr& ar);

	void	OnTrade(OBJID objid, CAr& ar);
	void	OnConfirmTrade(OBJID objid, CAr& ar);
	void	OnConfirmTradeCancel(OBJID objid, CAr& ar);
	void	OnTradePut(OBJID objid, CAr& ar);
	void	OnTradePutError(OBJID objid, CAr& ar);
	void	OnTradePull(OBJID objid, CAr& ar);
	void	OnTradePutGold(OBJID objid, CAr& ar);

	void	OnTradeCancel(OBJID objid, CAr& ar);
	void	OnTradeOk(OBJID objid, CAr& ar);
	void	OnTradeConsent(void);
	void	OnTradelastConfirm(void);
	void	OnTradelastConfirmOk(OBJID objid, CAr& ar);

	void	OnOpenShopWnd(OBJID objid, CAr& ar);
	void	OnPutItemBank(OBJID objid, CAr& ar);
	void	OnGetItemBank(OBJID objid, CAr& ar);
	void	OnPutGoldBank(OBJID objid, CAr& ar);
	void	OnMoveBankItem(OBJID objid, CAr& ar);
	void	OnErrorBankIsFull(OBJID objid, CAr& ar);
	void	OnBankWindow(OBJID objid, CAr& ar);

	void    OnFocusObj(CAr& ar);

	void	OnGuildBankWindow(OBJID objid, CAr& ar);
	void	OnPutItemGuildBank(OBJID objid, CAr& ar);
	void	OnGetItemGuildBank(OBJID objid, CAr& ar);
	void	OnChangeBankPass(OBJID objid, CAr& ar);
	void	OnConfirmBankPass(OBJID objid, CAr& ar);
	void	OnUpdateVendor(OBJID objid, CAr& ar);
	void	OnUpdateItem(OBJID objid, CAr& ar);

	void	OnUpdateItemEx(OBJID objid, CAr& ar);

#ifdef __JEFF_11
	void	OnQuePetResurrectionResult(CAr& ar);
#endif	// __JEFF_11
	void	OnSetDestParam(OBJID objid, CAr& ar);
	void	OnResetDestParam(OBJID objid, CAr& ar);
#ifdef __SPEED_SYNC_0108
	void	OnResetDestParamSync(OBJID objid, CAr& ar);
#endif
	void	OnSetPointParam(OBJID objid, CAr& ar);
	void	OnSetScale(OBJID objid, CAr& ar);

	void	OnSetPos(OBJID objid, CAr& ar);
	void	OnSetPosAngle(OBJID objid, CAr& ar);
	void	OnSetLevel(OBJID objid, CAr& ar);
	void	OnSetFlightLevel(OBJID objid, CAr& ar);
	void	OnSetExperience(OBJID objid, CAr& ar);
	void	OnSetFxp(OBJID objid, CAr& ar);
	void	OnSetSkillLevel(OBJID objid, CAr& ar);
	void	OnSetSkillExp(OBJID objid, CAr& ar);
	void	OnText(CAr& ar);
#ifndef __S_SERVER_UNIFY
	void	OnDiagText(CAr& ar);
#endif // __S_SERVER_UNIFY
#ifdef __S_SERVER_UNIFY
	void	OnAllAction(CAr& ar);
#endif // __S_SERVER_UNIFY

	void	OnSetGrowthLearningPoint(OBJID objid, CAr& ar);

	void	OnSetDestPos(OBJID objid, CAr& ar);
	void	OnSetMovePattern(OBJID objid, CAr& ar);
	void	OnMeleeAttack(OBJID objid, CAr& ar);
	void	OnMeleeAttack2(OBJID objid, CAr& ar);
	void	OnMagicAttack(OBJID objid, CAr& ar);
	void	OnRangeAttack(OBJID objid, CAr& ar);
	void	OnAttackSP(OBJID objid, CAr& ar);
	void	OnMoverSetDestObj(OBJID objid, CAr& ar);
	void	OnUseSkill(OBJID objid, CAr& ar);
	void	OnCreateSfxObj(OBJID objid, CAr& ar);
	void	OnCreateSfxAllow(OBJID objid, CAr& ar);
	void	OnDefinedText(CAr& ar);
	void	OnChatText(CAr& ar);
	void	OnDefinedText1(CAr& ar);
	void	OnDefinedCaption(CAr& ar);

	void	OnGameTimer(CAr& ar);
	void	OnGameJoin(CAr& ar);
	void	OnResurrection(OBJID objid);
	void	OnTaskBar(CAr& ar);
	void	OnErrorParty(CAr& ar);
	void	OnAddPartyMember(CAr& ar);
	void	OnPartyRequest(CAr& ar);
	void	OnPartyRequestCancel(CAr& ar);
	void	OnPartyExpLevel(CAr& ar);
	void	OnPartyChangeTroup(CAr& ar);
	void	OnPartyChangeName(CAr& ar);
	void	OnPartySkillCall(OBJID objid, CAr& ar);
	void	OnPartySkillBlitz(CAr& ar);
	void	OnPartySkillRetreat(OBJID objid);
	void	OnPartySkillSphereCircle(OBJID objid);
	void	OnSetPartyMode(CAr& ar);
	void	OnPartyChangeItemMode(CAr& ar);
	void	OnPartyChangeExpMode(CAr& ar);

	void	OnSetPartyMemberParam(CAr& ar);

	void	OnEnvironmentSetting(CAr& ar);
	void	OnEnvironmentEffect(CAr& ar);

	void	OnPartyChat(CAr& ar);
	void	OnMyPartyName(CAr& ar);
	void	OnPartyChangeLeader(CAr& ar);
	void	OnSMMode(OBJID objid, CAr& ar);
	void	OnSMModeAll(CAr& ar);
	void	OnCommercialElem(CAr& ar);
	void	OnPartyMapInfo(CAr& ar);

	void	OnChatting(OBJID objid, CAr& ar);

	void	OnCommonPlace(OBJID objid, CAr& ar);
	void	OnDoApplySkill(OBJID objid, CAr& ar);
	void	OnCommonSkill(OBJID objid, CAr& ar);
	void	OnFlyffEvent(CAr& ar);
	void	OnGameRate(CAr& ar);

#if __VER >= 9 // __EVENTLUA
	void	OnEventLuaDesc(CAr& ar);
#endif // __EVENTLUA
#if __VER >= 10 // __REMOVE_ATTRIBUTE
	void	OnRemoveAttributeResult(CAr& ar);
#endif // __REMOVE_ATTRIBUTE
	void    OnMotionArrive(OBJID objid, CAr& ar);
#ifdef __S1108_BACK_END_SYSTEM
	void	OnMonsterProp(CAr& ar);
	void	OnGMChat(CAr& ar);
#endif // __S1108_BACK_END_SYSTEM

	void	OnChangeFace(CAr& ar);

	void	OnGuildCombat(CAr& ar);
	void	OnQuestTextTime(CAr& ar);

	void	OnGCInWindow(CAr& ar);
	void	OnGCRequestStatus(CAr& ar);
	void	OnGCSelectPlayer(CAr& ar);
	void	OnGuildCombatEnterTime(CAr& ar);
	void	OnGuildCombatNextTimeState(CAr& ar);
	void	OnGuildCombatState(CAr& ar);
	void	OnGCUserState(CAr& ar);
	void	OnGCGuildStatus(CAr& ar);
	void	OnGCGuildPrecedence(CAr& ar);
	void	OnGCPlayerPrecedence(CAr& ar);
	void	OnGCJoinWarWindow(CAr& ar);
	void	OnGCWinGuild(CAr& ar);
	void	OnGCBestPlayer(CAr& ar);
	void	OnGCWarPlayerList(CAr& ar);
	void	OnGCTele(CAr& ar);
	void	OnGCDiagMessage(CAr& ar);
	void	OnIsRequest(CAr& ar);
	void	OnGCLog(CAr& ar);
	void	OnGCLogRealTime(CAr& ar);
	void	OnGCPlayerPoint(CAr& ar);

#ifdef __EVE_MINIGAME
	void	OnMiniGame(OBJID objid, CAr& ar);
	void	OnKawibawibo_Result(CAr& ar);
#endif // __EVE_MINIGAME

	void	OnFriendGameJoin(CAr& ar);
	void	OnAddFriend(CAr& ar);
	void	OnRemoveFriend(CAr& ar);
	void	OnAddFriendReqest(CAr& ar);
	void	OnAddFriendCancel(CAr& ar);
	void	OnFriendJoin(CAr& ar);
	void	OnFriendLogOut(CAr& ar);
	void	OnFriendNoIntercept(CAr& ar);
	void	OnFriendIntercept(CAr& ar);
	void	OnGetFriendState(CAr& ar);
	void	OnSetFriendState(CAr& ar);
	void	OnAddFriendError(CAr& ar);
	void	OnAddFriendNameReqest(CAr& ar);
	void	OnAddFriendNotConnect(CAr& ar);
	void	OnOneFriendState(CAr& ar);
	void	OnRemoveFriendState(CAr& ar);
	void	OnBlock(CAr& ar);
	void	OnSkillFlag(CAr& ar);
	void	OnSetSkillState(CAr& ar);
	void	OnChangeShopCost(CAr& ar);
	void	OnItemDropRate(CAr& ar);
	void	OnSetGuildQuest(CAr& ar);
	void	OnRemoveGuildQuest(CAr& ar);
	void	OnSetQuest(OBJID objid, CAr& ar);
	void	OnScriptRemoveQuest(OBJID objid, CAr& ar);
	void	OnSetChangeJob(OBJID objid, CAr& ar);
	void	OnSetNearChangeJob(OBJID objid, CAr& ar);
	void	OnModifyMode(OBJID objid, CAr& ar);
	void	OnStateMode(OBJID objid, CAr& ar);
	void	OnReturnSay(OBJID objid, CAr& ar);
	void	OnClearUseSkill();
	void	OnClearUseObject();

	void	OnSetFuel(OBJID objid, CAr& ar);

	//________________________________________________________________________________
	void	OnMoverBehavior(OBJID objid, CAr& ar);
	void	OnQueryPlayerData(CAr& ar);
	void	OnLogout(CAr& ar);
	void	OnCreateGuild(CAr& ar);
	void	OnDestroyGuild(CAr& ar);
	void	OnGuild(CAr& ar);
	void	OnSetGuild(OBJID objid, CAr& ar);
	void	OnAddGuildMember(CAr& ar);
	void	OnRemoveGuildMember(CAr& ar);
	void	OnGuildInvite(CAr& ar);
	void	OnAllGuilds(CAr& ar);
	void	OnGuildChat(CAr& ar);
	void	OnGuildMemberLv(CAr& ar);
	void	OnGuildClass(CAr& ar);
	void	OnGuildNickName(CAr& ar);
	void	OnGuildMemberLogin(CAr& ar);
	void	OnGuldMyGameJoin(CAr& ar);
	void	OnGuildError(CAr& ar);
	void	OnGuildContribution(CAr& ar);
	void	OnGuildNotice(CAr& ar);
	void	OnGuildAuthority(CAr& ar);
	void	OnGuildSetName(CAr& ar);

	void	OnCorrReq(OBJID objid, CAr& ar);
	void	OnPVendorOpen(OBJID objid, CAr& ar);
	void	OnPVendorClose(OBJID objid, CAr& ar);
	void	OnRegisterPVendorItem(OBJID objid, CAr& ar);
	void	OnUnregisterPVendorItem(OBJID objid, CAr& ar);
	void	OnPVendorItem(OBJID objid, CAr& ar);
	void	OnPVendorItemNum(OBJID objid, CAr& ar);

	void	OnSetHair(OBJID objid, CAr& ar);
	void	OnCmdSetSkillLevel(CAr& ar);
	void	OnCreateSkillEffect(OBJID objid, CAr& ar);
	void	OnSetStun(OBJID objid, CAr& ar);
	void	OnSendActMsg(OBJID objid, CAr& ar);
	void	OnPushPower(OBJID objid, CAr& ar);

private:
	void	OnRunScriptFunc(OBJID objid, CAr& ar);
	void	OnSexChange(OBJID objid, CAr& ar);
	void	OnRemoveQuest(CAr& ar);
	void	OnInitSkillPoint(CAr& ar);
	void	OnDoUseSkillPoint(CAr& ar);

	void	OnResetBuffSkill(OBJID objid, CAr& ar);
	void	OnResurrectionMessage();
	void	OnWorldMsg(OBJID objid, CAr& ar);
	void	OnSetPlayerName(CAr& ar);
	void	OnUpdatePlayerData(CAr& ar);
	void	OnEscape(OBJID objid, CAr& ar);
	void	OnSetActionPoint(OBJID objid, CAr& ar);
	void	OnEndSkillQueue(OBJID objid);
	void	OnSnoop(CAr& ar);
	void	OnQueryEquip(OBJID objid, CAr& ar);
	void	OnReturnScrollACK(CAr& ar);
	void	OnSetTarget(OBJID objid, CAr& ar);
	void	OnPostMail(CAr& ar);
	void	OnRemoveMail(CAr& ar);
	void	OnRemoveMailItem(CAr& ar);
	void	OnMailBox(CAr& ar);


	////////////////////////////////////////////////////////////////////
	void	OnMailBoxReq(CAr& ar);
	////////////////////////////////////////////////////////////////////


	void	OnSummon(CAr& ar);
	void	OnSummonFriend(CAr& ar);
	void	OnSummonFriendConfirm(CAr& ar);
	void	OnSummonPartyConfirm(CAr& ar);
	void	OnRemoveGuildBankItem(CAr& ar);

	void	OnPetCall(OBJID objid, CAr& ar);
	void	OnPetRelease(OBJID objid, CAr& ar);
	void	OnPetSetExp(OBJID objid, CAr& ar);
	void	OnPet(OBJID objid, CAr& ar);
	void	OnPetLevelup(OBJID objid, CAr& ar);
	void	OnPetState(OBJID objid, CAr& ar);
	void	OnPetFeed(OBJID objid, CAr& ar);
	void	OnPetFoodMill(OBJID objid, CAr& ar);

	void	OnSetSpeedFactor(OBJID objid, CAr& ar);

	void OnLegendSkillUp(CAr& ar);

public:
#if __VER >= 11 // __SYS_COLLECTING
	void	SendQueryStartCollecting(void);
	void	SendQueryStopCollecting(void);
	void	OnStartCollecting(OBJID objid);
	void	OnStopCollecting(OBJID objid);
	void	OnRestartCollecting(OBJID objid, CAr& ar);
#endif	// __SYS_COLLECTING

#if __VER >= 12 // __UPDATE_OPT
	void	SendOptionEnableRenderMask(BOOL bEnable);
#endif	// __UPDATE_OPT

#ifdef __NPC_BUFF
	void	SendNPCBuff(const char* szKey);
#endif // __NPC_BUFF

	void	SendQueryPlayerData(u_long idPlayer, int nVer = 0);
	void	SendQueryPlayerData(const vector<PDVer>& vecPlayer);

#if __VER >= 12 // __SECRET_ROOM
public:
	void	SendSecretRoomTender(int nPenya);
	void	SendSecretRoomTenderCancelReturn();
	void	SendSecretRoomLineUpMember(vector<DWORD> vecLineUpMember);
	void	OnSecretRoomMngState(CAr& ar);
	void	OnSecretRoomInfo(CAr& ar);
	void	SendSecretRoomTenderOpenWnd();
	void 	OnSecretRoomTenderOpenWnd(CAr& ar);
	void	SendSecretRoomLineUpOpenWnd();
	void	OnSecretRoomLineUpOpenWnd(CAr& ar);
	void	SendSecretRoomEntrance();
	void	SendSecretRoomTeleportToNPC();
	void	SendSecretRoomTenderView();
	void	OnSecretRoomTenderView(CAr& ar);
private:
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
public:
	void	OnTaxInfo(CAr& ar);
	void	OnTaxSetTaxRateOpenWnd(CAr& ar);
	void	SendTaxRate(BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate);
private:
#endif // __TAX

#if __VER >= 12 // __HEAVEN_TOWER
public:
	void	SendTeleportToHeavenTower(int nFloor);
private:
#endif // __HEAVEN_TOWER

#if __VER >= 12 // __LORD
private:

	void	OnElectionAddDeposit(CAr& ar);

	void	OnElectionSetPledge(CAr& ar);

	void	OnElectionIncVote(CAr& ar);

	void	OnElectionBeginCandidacy(CAr& ar);

	void	OnElectionBeginVote(CAr& ar);

	void	OnElectionEndVote(CAr& ar);

	void	OnLord(CAr& ar);

	void	OnLEventCreate(CAr& ar);

	void	OnLEventInitialize(CAr& ar);

	void	OnLordSkillTick(CAr& ar);

	void	OnLEventTick(CAr& ar);

	void	OnLordSkillUse(OBJID objid, CAr& ar);
public:

	void	SendElectionAddDeposit(__int64 iDeposit);

	void	SendElectionSetPledge(const char* szPledge);

	void	SendElectionIncVote(u_long idPlayer);

#ifndef __FL_UPDATE_LORDEVENT
	void	SendLEventCreate(int iEEvent, int iIEvent);
#else
	void	SendLEventCreate(int iEEvent, int iIEvent, int iPEvent);
#endif

	void	SendLordSkillUse(int nSkill, const char* szTarget = "");
#endif	// __LORD
#if __VER >= 12 // __PET_0519
	void	SendPickupPetAwakeningCancel(DWORD dwItem);
#endif	// __PET_0519

#ifdef __PET_1024
	void	SendClearPetName();
	void	OnSetPetName(OBJID objid, CAr& ar);
#endif	// __PET_1024

#if __VER >= 13 // __HOUSING
private:
	void	OnHousingAllInfo(CAr& ar);
	void	OnHousingSetFunitureList(CAr& ar);
	void	OnHousingSetupFurniture(CAr& ar);
	void	OnHousingPaperingInfo(CAr& ar);
	void	OnHousingSetVisitAllow(CAr& ar);
	void	OnHousingVisitableList(CAr& ar);
public:
	void	SendHousingReqSetupFurniture(HOUSINGINFO housingInfo);
	void	SendHousingReqSetVisitAllow(DWORD dwPlayerId, BOOL bAllow);
	void	SendHousingVisitRoom(DWORD dwPlayerId);
	void	SendHousingReqVisitableList();
	void	SendHousingGoOut();
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
	void	SendPropose(const char* pszTarget);
	void	SendRefuse();
	void	SendCouple();
	void	SendDecouple();
	void	OnCouple(CAr& ar);
	void	OnProposeResult(CAr& ar);
	void	OnCoupleResult(CAr& ar);
	void	OnDecoupleResult(CAr& ar);
#if __VER >= 13 // __COUPLE_1202
	void	OnAddCoupleExperience(CAr& ar);
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117

#if __VER >= 13 // __QUEST_HELPER
private:
	void	OnNPCPos(CAr& ar);
public:
	void	SendReqNPCPos(const char* szCharKey);
#endif // __QUEST_HELPER
#if __VER >= 14 // __SMELT_SAFETY
	void	SendSmeltSafety(OBJID objid, OBJID objMaterialId, OBJID objProtScrId, OBJID objSmeltScrId = NULL_ID);
	void	OnSmeltSafety(CAr& ar);
#endif // __SMELT_SAFETY
#ifdef __MAP_SECURITY
	void	OnWorldReadInfo(CAr& ar);
	void	SendMapKey(const char* szFileName, const char* szMapKey);
#endif // __MAP_SECURITY

#if __VER >= 15 // __PETVIS
	void	SendRemoveVis(int nPos);
	void	SendSwapVis(int nPos1, int nPos2);
	void	OnActivateVisPet(CAr& ar);
	void	OnChangeMoverSfx(OBJID objId, CAr& ar);
#endif // __PETVIS

#if __VER >= 15 // __GUILD_HOUSE
	void	SendBuyGuildHouse();
	void	SendGuildHouseEnter();
	void	SendGuildHouseGoOut();
	void	OnGuildHousePacket(CAr& ar);
	void	OnGuildHouseAllInfo(CAr& ar);
	void	OnGuildHouseRemove(CAr& ar);
#endif // __GUILD_HOUSE
#if __VER >= 15 // __TELEPORTER
	void	SendTeleporterReq(const char* szCharKey, int nIndex);
#endif // __TELEPORTER

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void	OnCheckedQuest(CAr& ar);
	void	SendCheckedQuestId(int nQuestId, BOOL bCheck);
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __PROTECT_AWAKE
	void	SendSelectedAwakeningValue(DWORD dwObjID, DWORD dwSerialNum, BYTE bySelectFlag);
	void	OnSafeAwakening(CAr& ar);
#endif


#ifdef __FL_DUNGEON_TREASURE
	void OnDungeonTreasureWnd(CAr& ar);
	void SendDungeonTreasureSelect(int nSelect);
	void OnDungeonTreasureResult(CAr& ar);
#endif

#ifdef __FL_BATTLE_PASS
	void	OnBattlePassTime(CAr& ar);
	void	OnBattlePassProgress(CAr& ar);
#endif

#ifdef __FL_CONSUMABLES_STORAGE
	void	SendMoveItem_Chest(BYTE nItem, short nNum, BOOL bFromInventoryToChest);
	void	SendMoveItemInsideChest(BYTE nSrcIndex, BYTE nDestIndex);
	void	OnPutItemToChest(OBJID objid, CAr& ar);
	void	OnMoveItemInsideChest(CAr& ar);
	void	OnMoveAllItemsToChest();
#endif

	void	OnUpdateStatInfo(OBJID objid, CAr& ar);

#ifdef __FL_BOXES_V2
	void SendBulkOpenBox(DWORD dwItemID);
#endif
#ifdef __FL_FAST_ELEMENT
	void SendSelectElement(OBJID objScroll, int nAttribute, int nPosition);
	void OnElementChange(OBJID objid, CAr& ar);
#endif
#ifdef __FL_SHOP_SEARCH
	void OnShopSearch(CAr& ar);
	void SendTeleportToShop(OBJID objTarget);
#endif
#ifdef __FL_PET_FILTER
	void SendPetLootOption(int nLootType, BYTE nLootPlace);
#endif
#ifdef __FL_EQUALIZED_MODE
	void SendSelectTmpClass(int nTmpClass);
	void OnOpenSelectTmpWnd(CAr& ar);
	void OnSerializeInvAndBuffs(CAr& ar);
	void OnSetNearEquipUpd(OBJID objid, CAr& ar);
#endif
#ifdef __FL_PARTY_SORT
	void	SendMoveMemberParty(u_long uMember, BOOL UpDown);
	void	OnPartyChangePos(CAr& ar);
#endif

#ifdef __FL_GW_STATUE_V2
	void SendStatueSettings(int nStatueType, int nStatuePrice);
	void SendRequestStatueParams(BOOL bIsMvp);
	void SendReceiveStatueBuff(BOOL bIsMvp, int nPrice);
	void OnStatueParams(CAr& ar);
#endif

#ifdef __FL_FFA
	void OnFFAData(CAr& ar);
	void OnFFAState(CAr& ar);
	void OnFFAClear();
	void OnFFAKill(CAr& ar);
#endif

#ifdef __FL_DAILY_REWARDS
	void OnDailyReward(CAr& ar);
#endif

#ifdef __MODEL_HIDEFASH
	void SendHideCoat(int nPos, BOOL bHide);
	void OnHideFashion(OBJID objid, CAr& ar);
#endif

#ifdef __WIKI_ITEMS_V2
	void SendWikiCreate(DWORD dwId, int nMode);
#endif
#ifdef __MODEL_CHANGE
	void	SendLookChange(DWORD dwIdSrc, DWORD dwIdDest);
	void	OnLookChange(CAr& ar);
#endif
#ifdef __FL_TELEPORT
	void	SendTeleportRequest(int nKey);
#endif
#ifdef __JOB_CHANGE
	void	SendJobChange(int nJob);
#endif
#ifdef __FL_RECYCLE
	void	SendRecycleRequest(DWORD dwItem);
#endif
	void OnClearCastingSkills(OBJID objid, CAr& ar);
#ifdef __FL_PARTY_FINDER
	void	OnPartyFinder(CAr& ar);
	void	OnPartyChangeJoinMode(CAr& ar);
	void	SendJoinPartyFinder(u_long uPartyId);
#endif
#ifdef __FL_CASH_SHOP
private:
	void	OnCShop(CAr& ar);
	void	OnCShopBalance(CAr& ar);
public:
	void	SendBuyCShopItem(int nItemUniqueID, int nItemAmount, BOOL bDonate, u_long idReceiver);
#endif
#ifdef __FL_NEW_BOXES
	void SendUseNewBox(DWORD dwObjId, int nCurSel);
#endif
	void	SendUseTransy(DWORD dwObjId, int nSel);
#ifdef __FL_SWITCH_EQ
	void SendSwitchAdd(int nSel, const char* lpszName, DWORD dwLooter, DWORD dwPet, DWORD dwRegen);
	void SendSwitchDelete(int nSel);
	void SendSwitchUse(int nSel);
	void OnSwitchAdd(CAr& ar);
	void OnSwitchDelete(CAr& ar);
	void OnSwitchesAll(CAr& ar);
#endif
#ifdef __FL_GUILD_FINDER
	void	OnGuildFinder(CAr& ar);
	void	SendGuildFinderState(u_long uGuildId);
	void	OnGuildFinderState(CAr& ar);
	void	SendGuildFinderJoin(u_long uGuildId);
	void	SendRequestGuildFinderAddon(u_long uGuildId);
	void	OnGuildFinderAddon(CAr& ar);
#endif
#ifdef __FL_PACKED_ITEMS
	void SendPackageOpen(DWORD dwObjId);
	void OnPackageOpen(CAr& ar);
	void SendPackageAdd(DWORD dwPackObjId, DWORD dwItemObjId);
	void OnPackageAdd(CAr& ar);
	void SendPackageRestore(DWORD dwObjId, int nCurSel);
	void OnPackageRestore(CAr& ar);
#endif
#ifdef __FL_WHEEL_OF_FORTUNE
	void OnWof(CAr& ar);
	void OnWofInit(CAr& ar);
	void OnWofFinish(CAr& ar);
#endif
#ifdef __FL_RECORD_BOOK
	void OnRecordBook(CAr& ar);
#endif
#ifdef __FL_MONSTER_ALBUM
	void SendMABossCardSlotRestore(DWORD dwKeyObjId, int nKeyLocation, int nSlot);
	void OnMAAddCardPieces(CAr& ar);
	void OnMABossCardSlotUnlock(CAr& ar);
	void OnMABossCardSlotInsert(CAr& ar);
	void OnMABossCardSlotRestore(CAr& ar);
#endif
#ifdef __FL_TDM
	void OnTDMData(CAr& ar);
	void OnTDMRegister(CAr& ar);
	void OnTDMState(CAr& ar);
	void OnTDMClear();
	void OnTDMKill(CAr& ar);
#endif

	//________________________________________________________________________________
	//	Operator commands
	void	OnShout(CAr& ar);
	void	OnPlayMusic(CAr& ar);
	void	OnPlaySound(CAr& ar);
	void	OnGetPlayerAddr(CAr& ar);
	void	OnGetPlayerCount(CAr& ar);
	void	OnGetCorePlayer(CAr& ar);
	void	OnSystem(CAr& ar);
	void	OnCaption(CAr& ar);

	void    OnDisguise(OBJID objid, CAr& ar);
	void    OnNoDisguise(OBJID objid, CAr& ar);
};

#endif	// __DPCLIENT_H__