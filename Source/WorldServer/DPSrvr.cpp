#include "stdafx.h"
#include "defineText.h"
#include "defineItem.h"
#include "defineSkill.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "dpsrvr.h"
#include "definesound.h"
#include "defineObj.h"
#include "user.h"
#include "worldmng.h"
#include "misc.h"
#include "lang.h"
#include "npchecker.h"
#include "guild.h"
#include "..\_Common\Ship.h"
#include "Party.h"
#include "Chatting.h"
#include "post.h"
#include "..\_Network\ErrorCode.h"
#include "definequest.h"
#include "Exchange.h"

#ifdef __FL_TELEPORT
#include "Teleporter.h"
#endif

#ifdef __FL_CASH_SHOP
#include "FlyFFShop.h"
#endif

#if __VER >= 12 // __PET_0519
#include "pet.h"
#endif	// __PET_0519

#if __VER >= 12 // __LORD
#include "slord.h"
#include "lordskillexecutable.h"
#endif	// __LORD

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#endif	// __SYS_IDENTIFY

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 12 // __ITEMCREATEMON_S0602
#include "CreateMonster.h"
#endif // __ITEMCREATEMON_S0602

#if __VER >= 13
#include "honor.h"
#endif

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#if __VER >= 12 // __EXT_PIERCING
#include "ItemUpgrade.h"
#endif // __EXT_PIERCING

#include "tools.h"

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

#if __VER >= 15 // __GUILD_HOUSE 
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

#ifdef __FL_FFA
#include "FFAManager.h"
#endif
#ifdef __FL_TDM
#include "TDMManager.h"
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
#include "WheelOfFortune.h"
#endif

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

#define	MAX_RANGE_ASYNC			1024
#define	MAX_RANGE_NPC_MENU		1024

extern	CGuildMng			g_GuildMng;
extern	CPartyMng			g_PartyMng;
extern	CUserMng			g_UserMng;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;
extern	CWorldMng			g_WorldMng;
extern	CChattingMng		g_ChattingMng;
extern	CGuildCombat		g_GuildCombatMng;

CDPSrvr		g_DPSrvr;


BOOL CanAdd(DWORD dwGold, int nPlus)
{
	if (nPlus <= 0)
		return FALSE;

	__int64 n64Gold = (__int64)dwGold;
	if (n64Gold < 0 || (n64Gold + (__int64)nPlus) < 0 || (n64Gold + (__int64)nPlus) > INT_MAX)
		return FALSE;

	return TRUE;
}

CDPSrvr::CDPSrvr()
{
	BEGIN_MSG;

	ON_MSG(PACKETTYPE_JOIN, &CDPSrvr::OnAddUser);
	ON_MSG(PACKETTYPE_LEAVE, &CDPSrvr::OnRemoveUser);
	ON_MSG(PACKETTYPE_CORR_REQ, &CDPSrvr::OnCorrReq);
	ON_MSG(PACKETTYPE_SCRIPTDLG, &CDPSrvr::OnScriptDialogReq);
	ON_MSG(PACKETTYPE_TRADEPUT, &CDPSrvr::OnTradePut);
	ON_MSG(PACKETTYPE_TRADEPULL, &CDPSrvr::OnTradePull);
	ON_MSG(PACKETTYPE_TRADEPUTGOLD, &CDPSrvr::OnTradePutGold);
	ON_MSG(PACKETTYPE_SFX_ID, &CDPSrvr::OnSfxID);
	ON_MSG(PACKETTYPE_SFX_CLEAR, &CDPSrvr::OnSfxClear);
	ON_MSG(PACKETTYPE_CHAT, &CDPSrvr::OnChat);
	ON_MSG(PACKETTYPE_MOVEITEM, &CDPSrvr::OnMoveItem);
	ON_MSG(PACKETTYPE_SNAPSHOT, &CDPSrvr::OnSnapshot);
	ON_MSG(PACKETTYPE_MODIFYMODE, &CDPSrvr::OnModifyMode);
	ON_MSG(PACKETTYPE_REVIVAL, &CDPSrvr::OnRevival);
	ON_MSG(PACKETTYPE_REVIVAL_TO_LODESTAR, &CDPSrvr::OnRevivalLodestar);
	ON_MSG(PACKETTYPE_OPENSHOPWND, &CDPSrvr::OnOpenShopWnd);
	ON_MSG(PACKETTYPE_CLOSESHOPWND, &CDPSrvr::OnCloseShopWnd);
	ON_MSG(PACKETTYPE_SEND_TO_SERVER_EXP, &CDPSrvr::OnExpUp);
	ON_MSG(PACKETTYPE_OPENBANKWND, &CDPSrvr::OnOpenBankWnd);
	ON_MSG(PACKETTYPE_GUILD_BANK_WND, &CDPSrvr::OnOpenGuildBankWnd);
	ON_MSG(PACKETTYPE_PUTITEMGUILDBANK, &CDPSrvr::OnPutItemGuildBank);
	ON_MSG(PACKETTYPE_GETITEMGUILDBANK, &CDPSrvr::OnGetItemGuildBank);
	ON_MSG(PACKETTYPE_GUILD_BANK_WND_CLOSE, &CDPSrvr::OnCloseGuildBankWnd);
	ON_MSG(PACKETTYPE_GUILD_BANK_MOVEITEM, &CDPSrvr::OnGuildBankMoveItem);
	ON_MSG(PACKETTYPE_CLOSEBANKWND, &CDPSrvr::OnCloseBankWnd);
	ON_MSG(PACKETTYPE_PUTITEMBACK, &CDPSrvr::OnPutItemBank);
	ON_MSG(PACKETTYPE_DOUSESKILLPOINT, &CDPSrvr::OnDoUseSkillPoint);
	ON_MSG(PACKETTYPE_PUTBACKTOBANK, &CDPSrvr::OnBankToBank);
	ON_MSG(PACKETTYPE_GETITEMBACK, &CDPSrvr::OnGetItemBank);
	ON_MSG(PACKETTYPE_PUTGOLDBACK, &CDPSrvr::OnPutGoldBank);
	ON_MSG(PACKETTYPE_GETGOLDBACK, &CDPSrvr::OnGetGoldBank);
	ON_MSG(PACKETTYPE_MOVEBANKITEM, &CDPSrvr::OnMoveBankItem);
	ON_MSG(PACKETTYPE_CHANGEBANKPASS, &CDPSrvr::OnChangeBankPass);
	ON_MSG(PACKETTYPE_CONFIRMBANK, &CDPSrvr::OnConfirmBank);

	ON_MSG(PACKETTYPE_PLAYERBEHAVIOR, &CDPSrvr::OnPlayerBehavior);
	ON_MSG(PACKETTYPE_PLAYERSETDESTOBJ, &CDPSrvr::OnPlayerSetDestObj);

	ON_MSG(PACKETTYPE_TRADE, &CDPSrvr::OnTrade);
	ON_MSG(PACKETTYPE_CONFIRMTRADE, &CDPSrvr::OnConfirmTrade);
	ON_MSG(PACKETTYPE_CONFIRMTRADECANCEL, &CDPSrvr::OnConfirmTradeCancel);
	ON_MSG(PACKETTYPE_TRADECANCEL, &CDPSrvr::OnTradeCancel);
	ON_MSG(PACKETTYPE_DOUSEITEM, &CDPSrvr::OnDoUseItem);

	ON_MSG(PACKETTYPE_DO_USE_ITEM_TARGET, &CDPSrvr::OnDoUseItemTarget);
	ON_MSG(PACKETTYPE_REMOVE_ITEM_LEVEL_DOWN, &CDPSrvr::OnRemoveItemLevelDown);
	ON_MSG(PACKETTYPE_BLESSEDNESS_CANCEL, &CDPSrvr::OnBlessednessCancel);

#ifdef __PET_1024
	ON_MSG(PACKETTYPE_CLEAR_PET_NAME, &CDPSrvr::OnClearPetName);
#endif	// __PET_1024

#ifdef __JEFF_11
	ON_MSG(PACKETTYPE_QUE_PETRESURRECTION, &CDPSrvr::OnQuePetResurrection);
#endif	// __JEFF_11

#ifdef __JEFF_11_4
	ON_MSG(PACKETTYPE_ARENA_ENTER, &CDPSrvr::OnArenaEnter);
	ON_MSG(PACKETTYPE_ARENA_EXIT, &CDPSrvr::OnArenaExit);
#endif	// __JEFF_11_4

	ON_MSG(PACKETTYPE_TRADECONFIRM, &CDPSrvr::OnTradelastConfrim);
	ON_MSG(PACKETTYPE_DROPITEM, &CDPSrvr::OnDropItem);
	ON_MSG(PACKETTYPE_DROPGOLD, &CDPSrvr::OnDropGold);
	ON_MSG(PACKETTYPE_BUYITEM, &CDPSrvr::OnBuyItem);
	ON_MSG(PACKETTYPE_BUYCHIPITEM, &CDPSrvr::OnBuyChipItem);
	ON_MSG(PACKETTYPE_SELLITEM, &CDPSrvr::OnSellItem);
	ON_MSG(PACKETTYPE_TRADEOK, &CDPSrvr::OnTradeOk);

	ON_MSG(PACKETTYPE_MELEE_ATTACK, &CDPSrvr::OnMeleeAttack);
	ON_MSG(PACKETTYPE_MELEE_ATTACK2, &CDPSrvr::OnMeleeAttack2);
	ON_MSG(PACKETTYPE_MAGIC_ATTACK, &CDPSrvr::OnMagicAttack);
	ON_MSG(PACKETTYPE_RANGE_ATTACK, &CDPSrvr::OnRangeAttack);

	ON_MSG(PACKETTYPE_SFX_HIT, &CDPSrvr::OnSfxHit);
	ON_MSG(PACKETTYPE_USESKILL, &CDPSrvr::OnUseSkill);
	ON_MSG(PACKETTYPE_USEOBJECT, &CDPSrvr::OnUseObject);
	ON_MSG(PACKETTYPE_SETTARGET, &CDPSrvr::OnSetTarget);
	ON_MSG(PACKETTYPE_TELESKILL, &CDPSrvr::OnTeleSkill);
	ON_MSG(PACKETTYPE_SKILLTASKBAR, &CDPSrvr::OnSkillTaskBar);
	ON_MSG(PACKETTYPE_ADDAPPLETTASKBAR, &CDPSrvr::OnAddAppletTaskBar);
	ON_MSG(PACKETTYPE_REMOVEAPPLETTASKBAR, &CDPSrvr::OnRemoveAppletTaskBar);
	ON_MSG(PACKETTYPE_ADDITEMTASKBAR, &CDPSrvr::OnAddItemTaskBar);
	ON_MSG(PACKETTYPE_REMOVEITEMTASKBAR, &CDPSrvr::OnRemoveItemTaskBar);
	ON_MSG(PACKETTYPE_MEMBERREQUEST, &CDPSrvr::OnPartyRequest);
	ON_MSG(PACKETTYPE_MEMBERREQUESTCANCLE, &CDPSrvr::OnPartyRequestCancle);
	ON_MSG(PACKETTYPE_PARTYSKILLUSE, &CDPSrvr::OnPartySkillUse);
	ON_MSG(PACKETTYPE_ADDFRIENDREQEST, &CDPSrvr::OnAddFriendReqest);
	ON_MSG(PACKETTYPE_ADDFRIENDNAMEREQEST, &CDPSrvr::OnAddFriendNameReqest);
	ON_MSG(PACKETTYPE_ADDFRIENDCANCEL, &CDPSrvr::OnAddFriendCancel);

	ON_MSG(PACKETTYPE_QUERY_PLAYER_DATA, &CDPSrvr::OnQueryPlayerData);
	ON_MSG(PACKETTYPE_QUERY_PLAYER_DATA2, &CDPSrvr::OnQueryPlayerData2);
	ON_MSG(PACKETTYPE_GUILD_INVITE, &CDPSrvr::OnGuildInvite);
	ON_MSG(PACKETTYPE_IGNORE_GUILD_INVITE, &CDPSrvr::OnIgnoreGuildInvite);
	ON_MSG(PACKETTYPE_NW_GUILDCONTRIBUTION, &CDPSrvr::OnGuildContribution);
	ON_MSG(PACKETTYPE_NW_GUILDNOTICE, &CDPSrvr::OnGuildNotice);
	ON_MSG(PACKETTYPE_PVENDOR_OPEN, &CDPSrvr::OnPVendorOpen);
	ON_MSG(PACKETTYPE_PVENDOR_CLOSE, &CDPSrvr::OnPVendorClose);
	ON_MSG(PACKETTYPE_REGISTER_PVENDOR_ITEM, &CDPSrvr::OnRegisterPVendorItem);
	ON_MSG(PACKETTYPE_UNREGISTER_PVENDOR_ITEM, &CDPSrvr::OnUnregisterPVendorItem);
	ON_MSG(PACKETTYPE_QUERY_PVENDOR_ITEM, &CDPSrvr::OnQueryPVendorItem);
	ON_MSG(PACKETTYPE_BUY_PVENDOR_ITEM, &CDPSrvr::OnBuyPVendorItem);
	ON_MSG(PACKETTYPE_SET_HAIR, &CDPSrvr::OnSetHair);
	ON_MSG(PACKETTYPE_BLOCK, &CDPSrvr::OnBlock);

	ON_MSG(PACKETTYPE_ENCHANT, &CDPSrvr::OnEnchant);
	ON_MSG(PACKETTYPE_SMELT_SAFETY, &CDPSrvr::OnSmeltSafety);
	ON_MSG(PACKETTYPE_REMVOE_ATTRIBUTE, &CDPSrvr::OnRemoveAttribute);
	ON_MSG(PACKETTYPE_RANDOMSCROLL, &CDPSrvr::OnRandomScroll);
	ON_MSG(PACHETTYPE_ITEMTRANSY, &CDPSrvr::OnItemTransy);
	ON_MSG(PACKETTYPE_PIERCING, &CDPSrvr::OnPiercing);
	ON_MSG(PACKETTYPE_PIERCINGREMOVE, &CDPSrvr::OnPiercingRemove);
	ON_MSG(PACKETTYPE_ENTERCHTTING, &CDPSrvr::OnEnterChattingRoom);
	ON_MSG(PACKETTYPE_CHATTING, &CDPSrvr::OnChatting);
	ON_MSG(PACKETTYPE_OPENCHATTINGROOM, &CDPSrvr::OnOpenChattingRoom);
	ON_MSG(PACKETTYPE_CLOSECHATTINGROOM, &CDPSrvr::OnCloseChattingRoom);

	ON_MSG(PACKETTYPE_SETNAVIPOINT, &CDPSrvr::OnSetNaviPoint);
	ON_MSG(PACKETTYPE_DO_ESCAPE, &CDPSrvr::OnDoEscape);
	ON_MSG(PACKETTYPE_RETURNSCROLL, &CDPSrvr::OnReturnScroll);
	ON_MSG(PACKETTYPE_ENDSKILLQUEUE, &CDPSrvr::OnEndSkillQueue);
	ON_MSG(PACKETTYPE_FOCUSOBJ, &CDPSrvr::OnFoucusObj);

	ON_MSG(PACKETTYPE_REMOVEQUEST, &CDPSrvr::OnRemoveQuest);

	ON_MSG(PACKETTYPE_REQ_LEAVE, &CDPSrvr::OnReqLeave);
	ON_MSG(PACKETTYPE_RESURRECTION_OK, &CDPSrvr::OnResurrectionOK);
	ON_MSG(PACKETTYPE_RESURRECTION_CANCEL, &CDPSrvr::OnResurrectionCancel);

	ON_MSG(PACKETTYPE_STATEMODE, &CDPSrvr::OnStateMode);
	ON_MSG(PACKETTYPE_QUERYSETPLAYERNAME, &CDPSrvr::OnQuerySetPlayerName);
	ON_MSG(PACKETTYPE_QUERYSETGUILDNAME, &CDPSrvr::OnQuerySetGuildName);
	ON_MSG(PACKETTYPE_CHEERING, &CDPSrvr::OnCheering);
	ON_MSG(PACKETTYPE_QUERYEQUIP, &CDPSrvr::OnQueryEquip);
	ON_MSG(PACKETTYPE_QUERYEQUIPSETTING, &CDPSrvr::OnQueryEquipSetting);
	ON_MSG(PACKETTYPE_QUERYPOSTMAIL, &CDPSrvr::OnQueryPostMail);
	ON_MSG(PACKETTYPE_QUERYREMOVEMAIL, &CDPSrvr::OnQueryRemoveMail);
	ON_MSG(PACKETTYPE_QUERYGETMAILITEM, &CDPSrvr::OnQueryGetMailItem);
	ON_MSG(PACKETTYPE_QUERYGETMAILGOLD, &CDPSrvr::OnQueryGetMailGold);
	ON_MSG(PACKETTYPE_READMAIL, &CDPSrvr::OnQueryReadMail);
	ON_MSG(PACKETTYPE_QUERYMAILBOX, &CDPSrvr::OnQueryMailBox);
	ON_MSG(PACKETTYPE_CHANGEFACE, &CDPSrvr::OnChangeFace);

	ON_MSG(PACKETTYPE_CREATEMONSTER, &CDPSrvr::OnCreateMonster);

	ON_MSG(PACKETTYPE_IN_GUILDCOMBAT, &CDPSrvr::OnGCApp);
	ON_MSG(PACKETTYPE_OUT_GUILDCOMBAT, &CDPSrvr::OnGCCancel);
	ON_MSG(PACKETTYPE_REQUEST_STATUS, &CDPSrvr::OnGCRequestStatus);
	ON_MSG(PACKETTYPE_SELECTPLAYER_GUILDCOMBAT, &CDPSrvr::OnGCSelectPlayer);
	ON_MSG(PACKETTYPE_JOIN_GUILDCOMBAT, &CDPSrvr::OnGCJoin);
	ON_MSG(PACKETTYPE_TELE_GUILDCOMBAT, &CDPSrvr::OnGCTele);
	ON_MSG(PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, &CDPSrvr::OnGCPlayerPoint);
	ON_MSG(PACKETTYPE_SUMMON_FRIEND, &CDPSrvr::OnSummonFriend);
	ON_MSG(PACKETTYPE_SUMMON_FRIEND_CONFIRM, &CDPSrvr::OnSummonFriendConfirm);
	ON_MSG(PACKETTYPE_SUMMON_FRIEND_CANCEL, &CDPSrvr::OnSummonFriendCancel);
	ON_MSG(PACKETTYPE_SUMMON_PARTY, &CDPSrvr::OnSummonParty);
	ON_MSG(PACKETTYPE_SUMMON_PARTY_CONFIRM, &CDPSrvr::OnSummonPartyConfirm);

	ON_MSG(PACKETTYPE_REMOVEINVENITEM, &CDPSrvr::OnRemoveInvenItem);

#ifdef __EVE_MINIGAME
	ON_MSG(PACKETTYPE_KAWIBAWIBO_START, &CDPSrvr::OnKawibawiboStart);
	ON_MSG(PACKETTYPE_KAWIBAWIBO_GETITEM, &CDPSrvr::OnKawibawiboGetItem);
#endif // __EVE_MINIGAME

	ON_MSG(PACKETTYPE_ULTIMATE_MAKEITEM, &CDPSrvr::OnUltimateMakeItem);
	ON_MSG(PACKETTYPE_ULTIMATE_MAKEGEM, &CDPSrvr::OnUltimateMakeGem);
	ON_MSG(PACKETTYPE_ULTIMATE_TRANSWEAPON, &CDPSrvr::OnUltimateTransWeapon);
	ON_MSG(PACKETTYPE_ULTIMATE_SETGEM, &CDPSrvr::OnUltimateSetGem);
	ON_MSG(PACKETTYPE_ULTIMATE_REMOVEGEM, &CDPSrvr::OnUltimateRemoveGem);
	ON_MSG(PACKETTYPE_ULTIMATE_ENCHANTWEAPON, &CDPSrvr::OnUltimateEnchantWeapon);

#ifdef __TRADESYS
	ON_MSG(PACKETTYPE_EXCHANGE, &CDPSrvr::OnExchange);
#endif // __TRADESYS

	ON_MSG(PACKETTYPE_PET_RELEASE, &CDPSrvr::OnPetRelease);
	ON_MSG(PACKETTYPE_USE_PET_FEED, &CDPSrvr::OnUsePetFeed);
	ON_MSG(PACKETTYPE_MAKE_PET_FEED, &CDPSrvr::OnMakePetFeed);
	ON_MSG(PACKETTYPE_PET_TAMER_MISTAKE, &CDPSrvr::OnPetTamerMistake);
	ON_MSG(PACKETTYPE_PET_TAMER_MIRACLE, &CDPSrvr::OnPetTamerMiracle);
	ON_MSG(PACKETTYPE_FEED_POCKET_INACTIVE, &CDPSrvr::OnFeedPocketInactive);

	ON_MSG(PACKETTYPE_LEGENDSKILLUP_START, &CDPSrvr::OnLegendSkillStart);

	ON_MSG(PACKETTYPE_MODIFY_STATUS, &CDPSrvr::OnModifyStatus);

	ON_MSG(PACKETTYPE_GUILDLOG_VIEW, &CDPSrvr::OnQueryGuildBankLogList);

	ON_MSG(PACKETTYPE_HONOR_LIST_REQ, &CDPSrvr::OnHonorListReq);
	ON_MSG(PACKETTYPE_HONOR_CHANGE_REQ, &CDPSrvr::OnHonorChangeReq);

	ON_MSG(PACKETTYPE_QUERY_START_COLLECTING, &CDPSrvr::OnQueryStartCollecting);
	ON_MSG(PACKETTYPE_QUERY_STOP_COLLECTING, &CDPSrvr::OnQueryStopCollecting);

#ifdef __NPC_BUFF
	ON_MSG(PACKETTYPE_NPC_BUFF, &CDPSrvr::OnNPCBuff);
#endif // __NPC_BUFF

	ON_MSG(PACKETTYPE_SECRETROOM_TENDEROPENWND, &CDPSrvr::OnSecretRoomTenderOpenWnd);
	ON_MSG(PACKETTYPE_SECRETROOM_TENDER, &CDPSrvr::OnSecretRoomTender);
	ON_MSG(PACKETTYPE_SECRETROOM_TENDERCANCELRETURN, &CDPSrvr::OnSecretRoomTenderCancelReturn);
	ON_MSG(PACKETTYPE_SECRETROOM_LINEUPOPENWND, &CDPSrvr::OnSecretRoomLineUpOpenWnd);
	ON_MSG(PACKETTYPE_SECRETROOM_LINEUPMEMBER, &CDPSrvr::OnSecretRoomLineUpMember);
	ON_MSG(PACKETTYPE_SECRETROOM_ENTRANCE, &CDPSrvr::OnSecretRoomEntrance);
	ON_MSG(PACKETTYPE_SECRETROOM_TELEPORTTONPC, &CDPSrvr::OnSecretRoomTeleportToNPC);
	ON_MSG(PACKETTYPE_SECRETROOM_TENDERVIEW, &CDPSrvr::OnSecretRoomTenderView);

	ON_MSG(PACKETTYPE_ELECTION_ADD_DEPOSIT, &CDPSrvr::OnElectionAddDeposit);
	ON_MSG(PACKETTYPE_ELECTION_SET_PLEDGE, &CDPSrvr::OnElectionSetPledge);
	ON_MSG(PACKETTYPE_ELECTION_INC_VOTE, &CDPSrvr::OnElectionIncVote);
	ON_MSG(PACKETTYPE_L_EVENT_CREATE, &CDPSrvr::OnLEventCreate);
	ON_MSG(PACKETTYPE_LORD_SKILL_USE, &CDPSrvr::OnLordSkillUse);


	ON_MSG(PACKETTYPE_TAX_SET_TAXRATE, &CDPSrvr::OnSetTaxRate);

	ON_MSG(PACKETTYPE_HEAVENTOWER_TELEPORT, &CDPSrvr::OnTeleportToHeavenTower);

	ON_MSG(PACKETTYPE_PICKUP_PET_AWAKENING_CANCEL, &CDPSrvr::OnPickupPetAwakeningCancel);

	ON_MSG(PACKETTYPE_HOUSING_SETUPFURNITURE, &CDPSrvr::OnHousingSetupFurniture);
	ON_MSG(PACKETTYPE_HOUSING_SETVISITALLOW, &CDPSrvr::OnHousingSetVisitAllow);
	ON_MSG(PACKETTYPE_HOUSING_VISITROOM, &CDPSrvr::OnHousingVisitRoom);
	ON_MSG(PACKETTYPE_HOUSING_REQVISITABLELIST, &CDPSrvr::OnHousingVisitableList);
	ON_MSG(PACKETTYPE_HOUSING_GOOUT, &CDPSrvr::OnHousingGoOut);

	ON_MSG(PACKETTYPE_QUESTHELPER_REQNPCPOS, &CDPSrvr::OnReqQuestNPCPos);

	ON_MSG(PACKETTYPE_PROPOSE, &CDPSrvr::OnPropose);
	ON_MSG(PACKETTYPE_REFUSE, &CDPSrvr::OnRefuse);
	ON_MSG(PACKETTYPE_COUPLE, &CDPSrvr::OnCouple);
	ON_MSG(PACKETTYPE_DECOUPLE, &CDPSrvr::OnDecouple);

#ifdef __MAP_SECURITY
	ON_MSG(PACKETTYPE_MAP_KEY, &CDPSrvr::OnMapKey);
#endif // __MAP_SECURITY

	ON_MSG(PACKETTYPE_VISPET_REMOVEVIS, &CDPSrvr::OnRemoveVis);
	ON_MSG(PACKETTYPE_VISPET_SWAPVIS, &CDPSrvr::OnSwapVis);

	ON_MSG(PACKETTYPE_GUILDHOUSE_BUY, &CDPSrvr::OnBuyGuildHouse);
	ON_MSG(PACKETTYPE_GUILDHOUSE_PACKET, &CDPSrvr::OnGuildHousePacket);
	ON_MSG(PACKETTYPE_GUILDHOUSE_ENTER, &CDPSrvr::OnGuildHouseEnter);
	ON_MSG(PACKETTYPE_GUILDHOUSE_GOOUT, &CDPSrvr::OnGuildHouseGoOut);

	ON_MSG(PACKETTYPE_TELEPORTER, &CDPSrvr::OnTeleporterReq);

	ON_MSG(PACKETTYPE_QUEST_CHECK, &CDPSrvr::OnCheckedQuest);

	ON_MSG(PACKETTYPE_ITEM_SELECT_AWAKENING_VALUE, &CDPSrvr::OnItemSelectAwakeningValue);


#ifdef __FL_DUNGEON_TREASURE
	ON_MSG(PACKETTYPE_DUNGEON_TREASURE_SELECT, &CDPSrvr::OnDungeonTreasureSelect);
#endif

#ifdef __FL_CONSUMABLES_STORAGE
	ON_MSG(PACKETTYPE_MOVE_ITEM_CHEST, &CDPSrvr::OnMoveItemToChest);
	ON_MSG(PACKETTYPE_MOVE_ITEM_INSIDECHEST, &CDPSrvr::OnMoveItemInsideChest);
	ON_MSG(PACKETTYPE_MOVE_ALL_ITEMS_CHEST, &CDPSrvr::OnMoveAllItemsToChest);
#endif

#ifdef __FL_BOXES_V2
	ON_MSG(PACKETTYPE_BULKBOXOPENING, &CDPSrvr::OnBulkBoxOpen);
#endif

#ifdef __FL_FAST_ELEMENT
	ON_MSG(PACKETTYPE_SELECT_ELEMENT, &CDPSrvr::OnChangeAttributeNew);
#endif

#ifdef __MODEL_HIDEFASH
	ON_MSG(PACKETTYPE_HIDE_COAT, &CDPSrvr::OnHideCoat);
#endif

#ifdef __FL_SHOP_SEARCH
	ON_MSG(PACKETTYPE_REQUEST_SHOP_SEARCH, &CDPSrvr::OnRequestShopSearch);
	ON_MSG(PACKETTYPE_TELEPORT_TO_THIS_SHOP, &CDPSrvr::OnTeleportToShop);
#endif

#ifdef __FL_PET_FILTER
	ON_MSG(PACKETTYPE_PETLOOTTYPE_REQ, &CDPSrvr::OnPetLootOption);
#endif

#ifdef __FL_EQUALIZED_MODE
	ON_MSG(PACKETTYPE_SELECT_TMP_CLASS, &CDPSrvr::OnSelectTmpClass);
#endif

	ON_MSG(PACKETTYPE_ADDPARTYMEMBER, &CDPSrvr::OnConfirmPartyRequest);
	ON_MSG(PACKETTYPE_ADDFRIEND, &CDPSrvr::OnConfirmFriendRequest);
	ON_MSG(PACKETTYPE_ADD_GUILD_MEMBER, &CDPSrvr::OnConfirmGuildRequest);

#ifdef __FL_GW_STATUE_V2
	ON_MSG(PACKETTYPE_STATUE_SETTINGS, &CDPSrvr::OnConfirmStatueSettings);
	ON_MSG(PACKETTYPE_REQUEST_STATUE_PARAMS, &CDPSrvr::OnRequestStatueParams);
	ON_MSG(PACKETTYPE_REQUEST_BUFF_STATUE, &CDPSrvr::OnReceiveStatueBuff);
#endif

#ifdef __FL_FFA
	ON_MSG(PACKETTYPE_REQUEST_FFA, &CDPSrvr::OnRequestFFA);
	ON_MSG(PACKETTYPE_REQUEST_FFA_REGISTER, &CDPSrvr::OnRequestFFARegister);
	ON_MSG(PACKETTYPE_REQUEST_FFA_JOIN, &CDPSrvr::OnRequestFFAJoin);
#endif

#ifdef __FL_DAILY_REWARDS
	ON_MSG(PACKETTYPE_COLLECT_DAILY_GIFT, &CDPSrvr::OnCollectDailyGift);
#endif

#ifdef __WIKI_ITEMS_V2
	ON_MSG(PACKETTYPE_WIKI_CREATE, &CDPSrvr::OnWikiCreate);
#endif

#ifdef __MODEL_CHANGE
	ON_MSG(PACKETTYPE_LOOKCHANGE, &CDPSrvr::OnLookChange);
#endif

#ifdef __FL_TELEPORT
	ON_MSG(PACKETTYPE_TELEPORT_REQUEST, &CDPSrvr::OnTeleportRequest);
#endif

#ifdef __JOB_CHANGE
	ON_MSG(PACKETTYPE_SEND_JOB_CHANGE, &CDPSrvr::OnSendJobChange);
#endif

#ifdef __FL_RECYCLE
	ON_MSG(PACKETTYPE_SEND_RECYCLE, &CDPSrvr::OnRecycle);
#endif

#ifdef __FL_PARTY_FINDER
	ON_MSG(PACKETTYPE_REQUEST_PARTY_FINDER, &CDPSrvr::OnPartyFinderRequest);
	ON_MSG(PACKETTYPE_REQUEST_PARTY_FINDER_JOIN, &CDPSrvr::OnPartyFinderJoin);
#endif

#ifdef __FL_VENDORS
	ON_MSG(PACKETTYPE_VENDOR_ACTION, &CDPSrvr::OnVendorAction);
#endif

#ifdef __FL_CASH_SHOP
	ON_MSG(PACKETTYPE_CSHOP_BALANCE, &CDPSrvr::OnGetCShopBalance);
	ON_MSG(PACKETTYPE_CSHOP_ITEM, &CDPSrvr::OnBuyCShopItem);
#endif

#ifdef __FL_NEW_BOXES
	ON_MSG(PACKETTYPE_NEWBOX_OPEN, &CDPSrvr::OnNewBoxOpen);
#endif

	ON_MSG(PACKETTYPE_USE_TRANSY, &CDPSrvr::OnUseTransy);

#ifdef __FL_PENYA_CONVERTER
	ON_MSG(PACKETTYPE_CONVERT_PENYA, &CDPSrvr::OnConvertPenya);
#endif
#ifdef __FL_SWITCH_EQ
	ON_MSG(PACKETTYPE_SWITCH_ADD, &CDPSrvr::OnSwitchAdd);
	ON_MSG(PACKETTYPE_SWITCH_DELETE, &CDPSrvr::OnSwitchDelete);
	ON_MSG(PACKETTYPE_SWITCH_USE, &CDPSrvr::OnSwitchUse);
#endif
#ifdef __FL_GUILD_FINDER
	ON_MSG(PACKETTYPE_REQUEST_GUILD_FINDER, &CDPSrvr::OnGuildFinderRequest);
	ON_MSG(PACKETTYPE_GUILD_FINDERJOIN, &CDPSrvr::OnGuildFinderJoin);
	ON_MSG(PACKETTYPE_GUILD_FINDER_ADDON, &CDPSrvr::OnGuildFinderAddon);
#endif
#ifdef __FL_PACKED_ITEMS
	ON_MSG(PACKETTYPE_PACKAGE_OPEN, &CDPSrvr::OnPackageOpen);
	ON_MSG(PACKETTYPE_PACKAGE_ADD, &CDPSrvr::OnPackageAdd);
	ON_MSG(PACKETTYPE_PACKAGE_RESTORE, &CDPSrvr::OnPackageRestore);
#endif
#ifdef __FL_WHEEL_OF_FORTUNE
	ON_MSG(PACKETTYPE_WOF_INIT, &CDPSrvr::OnWofInit);
	ON_MSG(PACKETTYPE_WOF_FINISH, &CDPSrvr::OnWofFinish);
#endif
#ifdef __FL_RECORD_BOOK
	ON_MSG(PACKETTYPE_RECORD_BOOK, &CDPSrvr::OnRecordBook);
#endif
#ifdef __FL_MONSTER_ALBUM
	ON_MSG(PACKETTYPE_MA_BOSSCARDSLOT_RESTORE, &CDPSrvr::OnMABossCardSlotRestore);
#endif
#ifdef __FL_TDM
	ON_MSG(PACKETTYPE_REQUEST_TDM, &CDPSrvr::OnRequestTDM);
	ON_MSG(PACKETTYPE_REQUEST_TDM_REGISTER, &CDPSrvr::OnRequestTDMRegister);
	ON_MSG(PACKETTYPE_REQUEST_TDM_JOIN, &CDPSrvr::OnRequestTDMJoin);
#endif
}

CDPSrvr::~CDPSrvr()
{
	m_dpidCache = DPID_UNKNOWN;
}

void CDPSrvr::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	switch (lpMsg->dwType)
	{
	case DPSYS_CREATEPLAYERORGROUP:
	{
		LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer = (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
		OnAddConnection(lpCreatePlayer->dpId);
		break;
	}
	case DPSYS_DESTROYPLAYERORGROUP:
	{
		LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
		OnRemoveConnection(lpDestroyPlayer->dpId);
		break;
	}
	}
}

void CDPSrvr::UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	LPBYTE lpBuffer = (LPBYTE)lpMsg + sizeof(DPID);
	u_long uBufSize = dwMsgSize - sizeof(DPID);

	CAr ar(lpBuffer, uBufSize);
	GETTYPE(ar)

		void (theClass:: * pfn)(theParameters) = GetHandler(dw);

	if (pfn)
	{
		(this->*(pfn))(ar, idFrom, *(UNALIGNED LPDPID)lpMsg, lpBuffer, uBufSize);
	}

}

void CDPSrvr::OnAddConnection(DPID dpid)
{
}

void CDPSrvr::OnRemoveConnection(DPID dpid)
{
	if (dpid == m_dpidCache)
	{
		g_UserMng.RemoveAllUsers();
		m_dpidCache = DPID_UNKNOWN;
	}
}

void CDPSrvr::OnAddUser(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	m_dpidCache = dpidCache;

	TCHAR	lpszAccount[MAX_ACCOUNT], lpszpw[MAX_PASSWORD], lpAddr[16];
	DWORD	dwAuthKey;
	u_long	idPlayer;
	BYTE	nSlot;
	DPID	dpidSocket;

	ar >> dwAuthKey >> idPlayer >> nSlot;
	ar >> dpidSocket;
	ar.ReadString(lpszAccount, MAX_ACCOUNT);
	ar.ReadString(lpszpw, MAX_PASSWORD);
	ar.ReadString(lpAddr, 16);

	if (nSlot >= 3)
	{
		Error("CDPSrvr::OnAddUser: slot %d idplayer %d", nSlot, idPlayer);
		return;
	}

	CUser* pUser = (CUser*)prj.GetUserByID(idPlayer);
	if (pUser)
	{
		Error("CDPSrvr::OnAddUser exist user. account(%s), idPlayer(%07d), DPID(%d) ]", lpszAccount, idPlayer, dpidUser);
		QueryDestroyPlayer(pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer);
		QueryDestroyPlayer(dpidCache, dpidSocket, dpidUser, idPlayer);
		return;
	}

	pUser = g_UserMng.AddUser(dpidCache, dpidUser, dpidSocket);
	if (!pUser)
	{
		Error("CDPSrvr::OnAddUser: AddUser NULL err. New acc: %s, Playerid: %d", lpszAccount, idPlayer);
		return;
	}

	pUser->m_dwAuthKey = dwAuthKey;
	memcpy(pUser->m_playAccount.lpAddr, lpAddr, 16);

	//	TRANS
	BEFORESENDDUAL(arJoin, PACKETTYPE_JOIN, dpidCache, dpidUser);
	arJoin << dwAuthKey;
	arJoin.WriteString(lpszAccount);
	arJoin.WriteString(lpszpw);
	arJoin << nSlot << idPlayer;
	SEND(arJoin, &g_dpDBClient, DPID_SERVERPLAYER);
}


void CDPSrvr::OnRemoveUser(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	g_UserMng.RemoveUser((DWORD)dpidUser);
}

void CDPSrvr::OnChat(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	static	TCHAR	sChat[1024];
	if (uBufSize > 1031 || uBufSize < 0)
		return;

	DWORD dwAuth;
	ar >> dwAuth;

	ar.ReadString(sChat, 1024);
	CString strChat = sChat;
	strChat.Replace("\\n", " ");
	strChat.Replace("@", " ");
	strChat.Replace("@@", " ");

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_dwAuthorization != dwAuth)
		{
			QueryDestroyPlayer(pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer);
			return;
		}

#ifdef __FL_UPDATE_CHATTING
		if (IsChatSpamString(sChat))
		{
			if (pUser->GetLastChatMessage() + 500 > GetTickCount())
			{
				pUser->AddDefinedText(TID_GAME_ERROR_CHATTING_1);
				return;
			}
			else
				pUser->SetLastChatMessage(GetTickCount());
		}
#endif

		if (sChat[0] == '/' && ParsingCommand(strChat, (CMover*)pUser))
			return;

		if (pUser->IsMode(TALK_MODE))
			return;

		int nText = pUser->GetMuteText();
		if (nText)
		{
			pUser->AddDefinedText(nText);
			return;
		}

		if (!(pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
			ParsingEffect(sChat, strlen(sChat));

		strChat = sChat;
		strChat.Replace("\\n", " ");
		strChat.Replace("@", " ");
		strChat.Replace("@@", " ");

		g_UserMng.AddChat(pUser, strChat);
	}
}

void CDPSrvr::OnMoveItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nItemType, nSrcIndex, nDstIndex;
	ar >> nItemType >> nSrcIndex >> nDstIndex;

	if (nSrcIndex == nDstIndex)
		return;
	if (nDstIndex >= MAX_INVENTORY || nSrcIndex >= MAX_INVENTORY)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CItemElem* pItemSrc = pUser->m_Inventory.GetAt(nSrcIndex);
	CItemElem* pItemDst = pUser->m_Inventory.GetAt(nDstIndex);
	if (pItemDst == NULL || IsUsableItem(pItemDst))
	{
		if (IsUsableItem(pItemSrc))
		{
			pUser->m_Inventory.Swap(nSrcIndex, nDstIndex);
			pUser->AddMoveItem(0, nSrcIndex, nDstIndex);
		}
	}
}

void CDPSrvr::OnDropItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwItemType;
	DWORD dwItemId;
	short nDropNum;
	D3DXVECTOR3 vPos;

	ar >> dwItemType >> dwItemId >> nDropNum >> vPos;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwItemId);
	if (!IsUsableItem(pItemElem))
		return;

	if (nDropNum > pItemElem->m_nItemNum)
		nDropNum = pItemElem->m_nItemNum;
	if (nDropNum < 1)
		nDropNum = 1;

	if (!g_eLocal.GetState(EVE_DROPITEMREMOVE))
	{
		static int min = ::GetTickCount() / 60000;
		static int count = 0;
		int min2 = ::GetTickCount() / 60000;
		if (min2 != min)
		{
			min = min2;
			count = 0;
		}
		if (count > 100)
		{
			pUser->AddDefinedText(TID_GAME_DIALOGNODROPITEM);
			return;
		}
		count++;
	}

	if (g_eLocal.GetState(EVE_DROPITEMREMOVE))
	{
		if (pUser->IsDropable(pItemElem, FALSE))
			pUser->RemoveItem((BYTE)dwItemId, nDropNum);
	}
	else
	{
		pUser->DropItem((DWORD)dwItemId, nDropNum, vPos);
	}
}

void CDPSrvr::OnDropGold(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	return;
}


void CDPSrvr::OnScriptDialogReq(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	static TCHAR lpKey[256];
	OBJID objid;
	int nGlobal1, nGlobal2, nGlobal3, nGlobal4;

	ar >> objid;
	ar.ReadString(lpKey, 256);

	ar >> nGlobal1 >> nGlobal2 >> nGlobal3 >> nGlobal4;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
#ifdef __QUEST_1208
		DWORD dwTickCount = GetTickCount();
		if (dwTickCount < pUser->m_tickScript + 400)
			return;
		pUser->m_tickScript = dwTickCount;
#endif	// __QUEST_1208
		CMover* pMover = prj.GetMover(objid);
		if (IsValidObj(pMover))
		{
#ifdef __HACK_1130
			char lpOutputString[512] = { 0, };
			sprintf(lpOutputString, "npc = %s, key = %s, n1 = %d, n2 = %d, n3 = %d, n4 = %d",
				pMover->GetName(), lpKey, nGlobal1, nGlobal2, nGlobal3, nGlobal4);
			OutputDebugString(lpOutputString);
#endif	// __HACK_1130

			D3DXVECTOR3 vOut = pUser->GetPos() - pMover->GetPos();
			if (fabs((double)D3DXVec3LengthSq(&vOut)) > MAX_LEN_MOVER_MENU)
			{
				return;
			}
#if !defined(__REMOVE_SCIRPT_060712)
			CScriptDialog::SetLatestDialog(pMover->GetName(), lpKey);
#endif
			if (pMover->m_pNpcProperty &&
				pMover->m_pNpcProperty->IsDialogLoaded())
			{
				if (lstrlen(lpKey) == 0)
					lstrcpy(lpKey, _T("#init"));
				if (nGlobal3 == 0)
					nGlobal3 = (int)pMover->GetId();
				if (nGlobal4 == 0)
					nGlobal4 = (int)pUser->GetId();

				pMover->m_pNpcProperty->RunDialog(lpKey, NULL, nGlobal1, (int)pMover->GetId(), (int)pUser->GetId(), nGlobal2);


				for (int i = 0; i < pUser->m_nQuestSize; i++)
				{
					LPQUEST lpQuest = &pUser->m_aQuest[i];
					QuestProp* pQuestProp = pUser->m_aQuest[i].GetProp();
					if (pQuestProp)
					{
						if (strcmp(pQuestProp->m_szEndCondDlgCharKey, pMover->m_szCharacterKey) == 0)
						{
							if (strcmp(pQuestProp->m_szEndCondDlgAddKey, lpKey) == 0)
							{
								lpQuest->m_bDialog = TRUE;
								pUser->AddSetQuest(lpQuest);
								break;
							}
						}
					}
				}
			}
		}
	}
}

void CDPSrvr::OnResurrectionCancel(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) && pUser->m_Resurrection_Data.bUseing)
	{
		pUser->m_Resurrection_Data.bUseing = FALSE;
	}
}


void CDPSrvr::OnResurrectionOK(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) && pUser->m_Resurrection_Data.bUseing)
	{
		if (!pUser->IsDie())
		{
			pUser->m_Resurrection_Data.bUseing = FALSE;
			return;
		}

		RESURRECTION_DATA* pData = &(((CMover*)pUser)->m_Resurrection_Data);
		CUser* pSrc = (CUser*)prj.GetUserByID(pData->dwPlayerID);

		if (IsValidObj(pSrc))
		{
			pUser->MakeCharacterAlive(TRUE, FALSE);
			pUser->m_Resurrection_Data.bUseing = FALSE;
		}
		else
			pUser->m_Resurrection_Data.bUseing = FALSE;
	}
}


void CDPSrvr::OnRevival(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->m_Resurrection_Data.bUseing = FALSE;
	pUser->MakeCharacterAlive(TRUE, TRUE);
}

void CDPSrvr::OnRevivalLodestar(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->m_Resurrection_Data.bUseing = FALSE;
	pUser->MakeCharacterAlive(FALSE, FALSE);
}

void CDPSrvr::OnCorrReq(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID idObj;
	ar >> idObj;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CMover* pMover = prj.GetMover(idObj);
		if (IsValidObj(pMover))
		{
			pUser->AddCorrReq(pMover);
		}
	}
}

void CDPSrvr::OnPartyRequest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long uMemberid;
	BOOL bTroup;
	ar >> uMemberid;
	ar >> bTroup;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		InviteParty(pUser->m_idPlayer, uMemberid, bTroup);
}

void CDPSrvr::OnPartyRequestCancle(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long uLeaderid, uMemberid;
	int nMode;
	ar >> uLeaderid >> uMemberid >> nMode;

	CUser* pUser = g_UserMng.GetUserByPlayerID(uLeaderid);
	if (IsValidObj(pUser))
		pUser->AddPartyRequestCancel(uLeaderid, uMemberid, nMode);
}

void CDPSrvr::OnPartySkillUse(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nSkill;
	ar >> nSkill;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	CParty* pParty = g_PartyMng.GetParty(pUser->m_idparty);
	if (pParty)
	{
		if (pParty->IsLeader(pUser->m_idPlayer))
			pParty->DoUsePartySkill(pUser->m_idparty, pUser->m_idPlayer, nSkill);
	}
}

void CDPSrvr::OnAddFriendReqest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long uMemberid;
	ar >> uMemberid;

	CUser* pLeader = g_UserMng.GetUser(dpidCache, dpidUser);
	CUser* pMember = g_UserMng.GetUserByPlayerID(uMemberid);

	if (!IsValidObj(pLeader))
		return;

	if (!IsValidObj(pMember))
		return;

	if (pLeader == pMember)
		return;

	if (!pLeader->m_RTMessenger.GetFriend(uMemberid))
	{
#ifdef __FL_FIX_IDCHANGE_EXPLOIT // Friend by ID
		BOOL bFound = FALSE;
		for (auto& it : pLeader->vecFriendInvites)
		{
			if (it.dwInvieMember == uMemberid)
			{
				it.dwTimeEnd = GetTickCount() + MIN(10);
				bFound = TRUE;
				break;
			}
		}

		if (!bFound)
		{
			INVITE_REQUESTS tmp;
			tmp.dwInvieMember = uMemberid;
			tmp.dwTimeEnd = GetTickCount() + MIN(10);
			pLeader->vecFriendInvites.push_back(tmp);
		}
#endif
		pMember->AddFriendReqest(pLeader->m_idPlayer, pLeader->m_nJob, (BYTE)pLeader->GetSex(), pLeader->GetName());
	}
}


void CDPSrvr::OnAddFriendNameReqest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long uMember;
	char szMemberName[64] = { 0, };
	ar.ReadString(szMemberName, 64);

	CUser* pLeader = g_UserMng.GetUser(dpidCache, dpidUser);
	uMember = CPlayerDataCenter::GetInstance()->GetPlayerId(szMemberName);
	CUser* pMember = g_UserMng.GetUserByPlayerID(uMember);

	if (!IsValidObj(pLeader))
		return;

	if (!IsValidObj(pMember))
	{
		pLeader->AddFriendError(2, szMemberName);
		return;
	}

	if (pLeader == pMember)
		return;

	if (!pLeader->m_RTMessenger.GetFriend(uMember))
	{
#ifdef __FL_FIX_IDCHANGE_EXPLOIT // Friend by name
		BOOL bFound = FALSE;
		for (auto& it : pLeader->vecFriendInvites)
		{
			if (it.dwInvieMember == uMember)
			{
				it.dwTimeEnd = GetTickCount() + MIN(10);
				bFound = TRUE;
				break;
			}
		}

		if (!bFound)
		{
			INVITE_REQUESTS tmp;
			tmp.dwInvieMember = uMember;
			tmp.dwTimeEnd = GetTickCount() + MIN(10);
			pLeader->vecFriendInvites.push_back(tmp);
		}
#endif
		pMember->AddFriendReqest(pLeader->m_idPlayer, pLeader->m_nJob, (BYTE)pLeader->GetSex(), pLeader->GetName());
	}
	else
		pLeader->AddFriendError(1, szMemberName);
}


void CDPSrvr::OnAddFriendCancel(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long uLeaderid;
	ar >> uLeaderid;

	CUser* pLeader = g_UserMng.GetUserByPlayerID(uLeaderid);
	if (IsValidObj(pLeader))
		pLeader->AddFriendCancel();	// uMemberid
}

void CDPSrvr::OnRemoveQuest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwQuestCancelID;
	ar >> dwQuestCancelID;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
#ifdef __QUEST_1208
		DWORD dwTickCount = GetTickCount();
		if (dwTickCount < pUser->m_tickScript + 400)
			return;
		pUser->m_tickScript = dwTickCount;
#endif	// __QUEST_1208
		LPQUEST lpQuest = pUser->FindQuest(dwQuestCancelID);
		if (lpQuest)
		{
			if (lpQuest->m_nState != QS_END)
			{
				QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(lpQuest->m_wId);
				if (pQuestProp && pQuestProp->m_bNoRemove == FALSE)
				{
					pUser->RemoveQuest(dwQuestCancelID);
					pUser->AddCancelQuest(dwQuestCancelID);
					g_dpDBClient.CalluspLoggingQuest(pUser->m_idPlayer, dwQuestCancelID, 30);

					if (pQuestProp->m_nBeginSetDisguiseMoverIndex)
					{
						pUser->NoDisguise();
						g_UserMng.AddNoDisguise(pUser);
					}
				}
			}
		}
	}
}

void CDPSrvr::OnQueryPlayerData(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long idPlayer;
	ar >> idPlayer;
	int nVer;
	ar >> nVer;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPlayer);
		if (pPlayerData && pPlayerData->data.nVer != nVer)
			pUser->AddQueryPlayerData(idPlayer, pPlayerData);
	}
}

void CDPSrvr::OnQueryPlayerData2(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nSize;
	//	u_long idPlayer;
	ar >> nSize;

	if (nSize > 1024)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		for (int i = 0; i < nSize; i++)
		{
			PDVer	pdv;
			ar.Read(&pdv, sizeof(PDVer));
			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pdv.idPlayer);
			if (pPlayerData && pPlayerData->data.nVer != pdv.nVer)
				pUser->AddQueryPlayerData(pdv.idPlayer, pPlayerData);
		}
	}
}

void CDPSrvr::OnGuildInvite(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objid;
	ar >> objid;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	InviteCompany(pUser, objid);
}

void CDPSrvr::OnIgnoreGuildInvite(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long idPlayer;
	ar >> idPlayer;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CUser* pPlayer = g_UserMng.GetUserByPlayerID(idPlayer);		// kingpin
		if (IsValidObj(pPlayer))
		{
			pPlayer->AddDefinedText(TID_GAME_COMACCEPTDENY, "%s", pUser->GetName(TRUE));
		}
	}
}


void CDPSrvr::OnGuildContribution(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE cbPxpCount, cbItemFlag;
	int nGold;

	cbItemFlag = 0;
	ar >> cbPxpCount >> nGold;
	ar >> cbItemFlag;

	if (g_eLocal.GetState(ENABLE_GUILD_INVENTORY) == FALSE)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	if (pUser->IsDie() == TRUE)
		return;

	if (nGold > 0)
	{
		if (pUser->CheckUserGold(nGold, false) == true)
		{
			if (g_DPCoreClient.SendGuildStat(pUser, GUILD_STAT_PENYA, nGold))
			{
				pUser->AddGold(-nGold);

				if (pUser->GetWorld())
					g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

				LogItemInfo aLogItem;
				aLogItem.Action = "W";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "GUILDBANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold() + nGold;
				aLogItem.Gold2 = pUser->GetGold();
				_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
				aLogItem.itemNumber = nGold;
				OnLogItem(aLogItem);
			}
		}
		else
		{
			pUser->AddDefinedText(TID_GAME_GUILDNOTENGGOLD, "");
		}
	}
	else if (cbItemFlag)
	{
		for (int i = 0; i < pUser->m_Inventory.GetMax(); ++i)
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId(i);
			if (IsUsableItem(pItemElem) == FALSE)
				continue;

			if (pItemElem->GetProp()->dwItemKind3 != IK3_GEM)
				continue;

			int nValue = 0;

			if (pItemElem->m_nItemNum > 0)
			{
				nValue = (pItemElem->GetProp()->dwItemLV + 1) / 2;
				nValue *= pItemElem->m_nItemNum;
			}

			if (nValue > 0)
			{
				if (g_DPCoreClient.SendGuildStat(pUser, GUILD_STAT_PXPCOUNT, nValue))
				{
#ifndef __FL_FIX_USELESS_LOGS
					LogItemInfo aLogItem;
					aLogItem.Action = "V";
					aLogItem.SendName = pUser->GetName();
					aLogItem.RecvName = "GUILDBANK";
					aLogItem.WorldId = pUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					OnLogItem(aLogItem, pItemElem, pItemElem->m_nItemNum);
#endif
					pUser->RemoveItem((BYTE)i, pItemElem->m_nItemNum);
				}
			}
		}

	}
}


void CDPSrvr::OnGuildNotice(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	char szNotice[MAX_BYTE_NOTICE];
	ar.ReadString(szNotice, MAX_BYTE_NOTICE);
	szNotice[MAX_BYTE_NOTICE - 1] = '\0';

	if (strlen(szNotice) == 0)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	g_DPCoreClient.SendGuildStat(pUser, GUILD_STAT_NOTICE, (DWORD)szNotice);
}

void CDPSrvr::OnSkillTaskBar(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nCount;
	ar >> nCount;

	if (nCount > MAX_SLOT_QUEUE)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		for (int i = 0; i < nCount; i++)
		{
			BYTE nIndex;
			ar >> nIndex;

			if (nIndex >= MAX_SLOT_QUEUE)
				return;

			ar >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwShortcut >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwId >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwType;
			ar >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwIndex >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwUserId >> pUser->m_playTaskBar.m_aSlotQueue[nIndex].m_dwData;
		}
	}
}
void CDPSrvr::OnAddAppletTaskBar(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nIndex;
	ar >> nIndex;

	if (nIndex >= MAX_SLOT_APPLET)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		ar >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwId >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwType;
		ar >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwIndex >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwUserId >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwData;
		ar >> pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_nLocation;

		if (pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT)
		{
			ar.ReadString(pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString, MAX_SHORTCUT_STRING);
			pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString[MAX_SHORTCUT_STRING - 1] = '\0';
		}
	}
}
void CDPSrvr::OnRemoveAppletTaskBar(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nIndex;
	ar >> nIndex;

	if (nIndex >= MAX_SLOT_APPLET)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT)
		{
			memset(pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString, 0, sizeof(pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_szString));
		}
		pUser->m_playTaskBar.m_aSlotApplet[nIndex].m_dwShortcut = SHORTCUT_NONE;
	}
}

void CDPSrvr::OnAddItemTaskBar(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nSlotIndex, nIndex;
	ar >> nSlotIndex >> nIndex;

	if (nSlotIndex >= MAX_SLOT_ITEM_COUNT || nIndex >= MAX_SLOT_ITEM)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		DWORD dwShortCut;
		ar >> dwShortCut;

		if (dwShortCut == SHORTCUT_CHAT)
		{
			int nchatshortcut = 0;
			for (int i = 0; i < MAX_SLOT_ITEM_COUNT; i++)
			{
				for (int j = 0; j < MAX_SLOT_ITEM; j++)
				{
					if (pUser->m_playTaskBar.m_aSlotItem[i][j].m_dwShortcut == SHORTCUT_CHAT)
						nchatshortcut++;
				}
			}

			if (nchatshortcut > 9)
			{
				pUser->AddDefinedText(TID_GAME_MAX_SHORTCUT_CHAT);
				return;
			}
		}

		pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut = dwShortCut;
		ar >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwId >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwType;
		ar >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwIndex >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwUserId >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwData;
		ar >> pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_nLocation;

		if (pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut == SHORTCUT_CHAT)
		{
			ar.ReadString(pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString, MAX_SHORTCUT_STRING);
			pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString[MAX_SHORTCUT_STRING - 1] = '\0';
		}
	}
}
void CDPSrvr::OnRemoveItemTaskBar(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nSlotIndex, nIndex;
	ar >> nSlotIndex >> nIndex;

	if (nSlotIndex >= MAX_SLOT_ITEM_COUNT || nIndex >= MAX_SLOT_ITEM)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut == SHORTCUT_CHAT)
		{
			memset(pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString, 0, sizeof(pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString));
		}
		pUser->m_playTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut = SHORTCUT_NONE;
	}
}

void CDPSrvr::OnPlayerBehavior(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	if (pUser->GetWorld() == NULL)
		return;

	if (pUser->IsDie() == TRUE)
		return;

	if (pUser->GetIndex() == 0)
		return;

	if (pUser->m_pActMover->IsActAttack())
		return;

	pUser->AutoSynchronizer()->Serialize(ar);
}

void CDPSrvr::OnPlayerSetDestObj(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->IsDie() == TRUE)
			return;

		OBJID objid;
		float fRange;
		ar >> objid >> fRange;

		CCtrl* pCtrl;

		pCtrl = prj.GetCtrl(objid);
		if (IsValidObj(pCtrl))
		{
			if (pUser->GetDestId() == objid && pUser->m_fArrivalRange == fRange)
				return;

			pUser->SetDestObj(objid, fRange, TRUE);
		}
	}
}

void CDPSrvr::OnDoUseItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwData;
	int nType;
	char szInput[MAX_INPUT_LEN] = { 0, };

	ar >> dwData >> nType;
	ar.ReadString(szInput, MAX_INPUT_LEN);

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->SetInput(szInput);
	pUser->DoUseItem(dwData, nType);
	pUser->ResetInput();
}

void CDPSrvr::OnOpenShopWnd(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	OBJID objid;
	ar >> objid;

	CMover* pVendor = prj.GetMover(objid);
	if (IsValidObj(pVendor) && pUser->m_vtInfo.GetOther() == NULL)
	{
		if (pVendor->IsNPC() == FALSE)
			return;

		if (pVendor->IsVendorNPC() == FALSE)
			return;

		if (pUser->m_bBank)
		{
			pUser->AddDefinedText(TID_GAME_TRADELIMITNPC, "");
			return;
		}

		if (pUser->m_vtInfo.VendorIsVendor())
			return;

#ifdef __S_SERVER_UNIFY
		if (pUser->m_bAllAction == FALSE)
			return;
#endif // __S_SERVER_UNIFY

		pUser->m_vtInfo.SetOther(pVendor);
		pUser->AddOpenShopWnd(pVendor);
	}
}

void CDPSrvr::OnCloseShopWnd(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CMover* pMover = pUser->m_vtInfo.GetOther();
	if (!IsValidObj(pMover))
		return;

	if (!pMover->IsNPC())
		return;

	pUser->m_vtInfo.SetOther(NULL);
}

void CDPSrvr::OnBuyItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CHAR cTab;
	DWORD dwItemObjID;
	short nNum;

	ar >> cTab >> dwItemObjID >> nNum;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_TRADE, pUser->GetWorld(), pUser->GetPos()))
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CMover* pVendor = pUser->m_vtInfo.GetOther();
	if (!IsValidObj(pVendor))
		return;

	if (!pVendor->IsNPC())
		return;

	LPCHARACTER lpChar = prj.GetCharacter(pVendor->m_szCharacterKey);
	if (!lpChar || lpChar->m_nVenderType != 0)
		return;

	if (cTab >= MAX_VENDOR_INVENTORY_TAB || cTab < 0)
		return;

	CItemElem* pItemElem = pVendor->m_ShopInventory[cTab]->GetAtId(dwItemObjID);
	if (!pItemElem)
		return;

	if (nNum > pItemElem->m_nItemNum)
		nNum = pItemElem->m_nItemNum;
	if (nNum < 1)
		nNum = 1;

#ifdef __OCCUPATION_SHOPITEM
	if (CTax::GetInstance()->IsOccupationShopItem(pItemElem->m_dwItemId) && !CTax::GetInstance()->IsOccupationGuildMember(pUser))
	{
		pUser->AddDefinedText(TID_GAME_SECRETROOM_STORE_BUY);
		return;
	}
#endif // 

	// Price for single item before tax
	__int64 n64TotalCost = pItemElem->GetCost();
	n64TotalCost *= prj.m_fShopCost;
	n64TotalCost *= prj.m_EventLua.GetShopBuyFactor(); //__SHOP_COST_RATE

	if (pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN)
		n64TotalCost = PERIN_VALUE;

	// Price for all items before tax
	n64TotalCost *= nNum;

	// Price for all items after tax
	__int64 n64Tax = 0;
	if (CTax::GetInstance()->IsApplyTaxRate(pVendor, pItemElem))
	{
		n64Tax = n64TotalCost * CTax::GetInstance()->GetPurchaseTaxRate(pVendor);
		n64TotalCost += n64Tax;
	}

	// If total price more than int limit
	if (n64TotalCost > INT_MAX)
		return;

	// If total price less than 1
	if (n64TotalCost < 1)
	{
		Error("CDPSrvr::OnBuyItem: n64TotalCost < 1 [User ID = %d], [Item ID = %d], [Item Number = %d]", pUser->m_idPlayer, pItemElem->m_dwItemId, nNum);
		return;
	}

	int nTotalCost = static_cast<int>(n64TotalCost);
	int nTax = static_cast<int>(n64Tax);

	if (pUser->CheckUserGold(nTotalCost, false) == false)
		return;

	CItemElem itemElem;
	itemElem = *pItemElem;
	itemElem.m_nItemNum = nNum;
	itemElem.SetSerialNumber();

	if (pUser->CreateItem(&itemElem))
	{
#ifndef __FL_FIX_USELESS_LOGS
		LogItemInfo aLogItem;
		aLogItem.Action = "B";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = pVendor->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold() - nTotalCost;
		aLogItem.Gold_1 = pVendor->GetGold();

		pItemElem->SetSerialNumber(itemElem.GetSerialNumber());
		OnLogItem(aLogItem, pItemElem, nNum);
		pItemElem->SetSerialNumber(0);
#endif

		pUser->AddGold(-nTotalCost);

		if (nTax > 0)
			CTax::GetInstance()->AddTax(CTax::GetInstance()->GetContinent(pUser), nTax, TAX_PURCHASE);
	}
	else
		pUser->AddDefinedText(TID_GAME_LACKSPACE);
}

void CDPSrvr::OnBuyChipItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CHAR cTab;
	DWORD dwItemObjID;
	short nNum;

	ar >> cTab >> dwItemObjID >> nNum;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_TRADE, pUser->GetWorld(), pUser->GetPos()))
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CMover* pVendor = pUser->m_vtInfo.GetOther();
	if (!IsValidObj(pVendor))
		return;

	if (!pVendor->IsNPC())
		return;

	LPCHARACTER lpChar = prj.GetCharacter(pVendor->m_szCharacterKey);
	if (!lpChar || lpChar->m_nVenderType == 0)
		return;

	if (cTab >= MAX_VENDOR_INVENTORY_TAB || cTab < 0)
		return;

	CItemElem* pItemElem = pVendor->m_ShopInventory[cTab]->GetAtId(dwItemObjID);
	if (!pItemElem)
		return;

	ItemProp* pItemProp = pItemElem->GetProp();
	if (!pItemProp)
		return;

	if (nNum > pItemElem->m_nItemNum)
		nNum = pItemElem->m_nItemNum;
	if (nNum < 1)
		nNum = 1;

#ifdef __OCCUPATION_SHOPITEM
	if (CTax::GetInstance()->IsOccupationShopItem(pItemElem->m_dwItemId) && !CTax::GetInstance()->IsOccupationGuildMember(pUser))
	{
		pUser->AddDefinedText(TID_GAME_SECRETROOM_STORE_BUY);
		return;
	}
#endif // 

	// Price for single item
	__int64 n64TotalCost = pItemElem->GetChipCost();

	// Price for all items
	n64TotalCost *= nNum;

	// If total price more than int limit
	if (n64TotalCost > INT_MAX)
		return;

	// If total price less than 1
	if (n64TotalCost < 1)
	{
		Error("CDPSrvr::OnBuyChipItem: n64TotalCost < 1 [User ID = %d], [Item ID = %d], [Item Number = %d]", pUser->m_idPlayer, pItemElem->m_dwItemId, nNum);
		return;
	}

#ifndef __NPC_SETVENDORTYPE
	int nTotalItems = pUser->GetAvailableItemNum(II_CHP_RED);
#else
	int nTotalItems = pUser->GetAvailableItemNum(lpChar->m_nVenderType);
#endif
	int nTotalCost = static_cast<int>(n64TotalCost);

	if (nTotalItems < nTotalCost)
	{
		pUser->AddDefinedText(TID_GAME_LACKCHIP);
		return;
	}

	if (pUser->m_Inventory.IsFull(pItemElem, pItemProp, nNum))
	{
		pUser->AddDefinedText(TID_GAME_LACKSPACE);
		return;
	}

	CItemElem itemElem;
	itemElem = *pItemElem;
	itemElem.m_nItemNum = nNum;
	itemElem.SetSerialNumber();

	if (pUser->CreateItem(&itemElem))
	{
#ifndef __FL_FIX_USELESS_LOGS
		LogItemInfo aLogItem;
		aLogItem.Action = "B";
		aLogItem.SendName = pUser->GetName();
		CString strTemp;
		strTemp.Format("%s_C", pVendor->GetName());
		aLogItem.RecvName = (LPCTSTR)strTemp;
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = nTotalItems + itemElem.GetChipCost() * nNum;
		aLogItem.Gold2 = nTotalItems;
		aLogItem.Gold_1 = (DWORD)((-1) * (int)((itemElem.GetChipCost() * nNum)));

		OnLogItem(aLogItem, &itemElem, nNum);
#endif

#ifndef __NPC_SETVENDORTYPE
		pUser->RemoveAvailableItem(II_CHP_RED, dwChipCost);
#else
		pUser->RemoveAvailableItem(lpChar->m_nVenderType, nTotalCost);
#endif
	}
}

void CDPSrvr::OnSellItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId;
	int nNum;

	ar >> dwId >> nNum;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_TRADE, pUser->GetWorld(), pUser->GetPos()))
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CMover* pVendor = pUser->m_vtInfo.GetOther();
	if (!IsValidObj(pVendor))
		return;

	if (!pVendor->IsNPC())
		return;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwId);
	if (!IsUsableItem(pItemElem))
		return;

	if (pUser->IsUsing(pItemElem))
	{
		pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
		return;
	}

	if (pUser->m_Inventory.IsEquip(dwId))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPTRADE, "");
		return;
	}

	if (pItemElem->IsQuest())
		return;

	if (pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN)
		return;

	if (nNum > pItemElem->m_nItemNum)
		nNum = pItemElem->m_nItemNum;
	if (nNum < 1)
		nNum = 1;

	ItemProp* pProp = pItemElem->GetProp();
	if (!pProp)
		return;

	if (pProp->dwItemKind3 == IK3_EVENTMAIN)
		return;

	// Selling price for single item before tax
	__int64 n64TotalPrice = pItemElem->GetCost() / 4;
	n64TotalPrice *= prj.m_EventLua.GetShopSellFactor(); //__SHOP_COST_RATE

	if (n64TotalPrice < 1)
		n64TotalPrice = 1;

	if (pItemElem->IsOnePenyaPrice())
		n64TotalPrice = 1;

	// Selling price for all items before tax
	n64TotalPrice *= nNum;

	// Selling price for all items after tax
	__int64 n64Tax = 0;
	if (CTax::GetInstance()->IsApplyTaxRate(pVendor, pItemElem))
	{
		n64Tax = n64TotalPrice * CTax::GetInstance()->GetSalesTaxRate(pVendor);
		n64TotalPrice -= n64Tax;
	}

	// If total selling price more than int limit, todo: convert perins and use them
	if (n64TotalPrice > INT_MAX)
		return;

	// If total selling price less than 1
	if (n64TotalPrice < 1)
	{
		Error("CDPSrvr::OnSellItem: n64TotalPrice < 1 [User ID = %d], [Item ID = %d], [Item Number = %d]", pUser->m_idPlayer, pItemElem->m_dwItemId, nNum);
		return;
	}

	int nTotalPrice = static_cast<int>(n64TotalPrice);
	int nTax = static_cast<int>(n64Tax);

	if (pUser->CheckUserGold(nTotalPrice, true) == FALSE)
		return;

#ifndef __FL_FIX_USELESS_LOGS
	LogItemInfo aLogItem;
	aLogItem.Action = "S";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = pUser->m_vtInfo.GetOther()->GetName();
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold() + nTotalPrice;
	OnLogItem(aLogItem, pItemElem, nNum);
#endif

	pUser->AddGold(nTotalPrice);

	if (nTax > 0)
		CTax::GetInstance()->AddTax(CTax::GetInstance()->GetContinent(pUser), nTax, TAX_SALES);

	pUser->RemoveItem((BYTE)dwId, nNum);
}

void CDPSrvr::OnOpenBankWnd(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD	dwId, dwItemId;
	ar >> dwId >> dwItemId;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (dwId == NULL_ID && !CNpcChecker::GetInstance()->IsCloseNpc(MMI_BANKING, pUser->GetWorld(), pUser->GetPos()))
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	if (0 == strcmp(pUser->m_szBankPass, "0000"))
		pUser->AddBankWindow(0, dwId, dwItemId);
	else
		pUser->AddBankWindow(1, dwId, dwItemId);
}

void CDPSrvr::OnOpenGuildBankWnd(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (g_eLocal.GetState(ENABLE_GUILD_INVENTORY) == FALSE)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif
	if (pUser->m_vtInfo.GetOther())
		return;
	if (pUser->m_vtInfo.VendorIsVendor())
		return;
	if (pUser->m_bBank)
		return;
#ifdef __S_SERVER_UNIFY
	if (pUser->m_bAllAction == FALSE)
		return;
#endif // 

	pUser->AddGuildBankWindow(0);
	pUser->m_bGuildBank = TRUE;
}

void CDPSrvr::OnCloseBankWnd(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->m_bBank = FALSE;
	pUser->m_bInstantBank = FALSE;
}

void CDPSrvr::OnDoUseSkillPoint(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	SKILL aJobSkill[MAX_SKILL_JOB] = { 0, };

	for (int i = 0; i < MAX_SKILL_JOB; i++)
		ar >> aJobSkill[i].dwSkill >> aJobSkill[i].dwLevel;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->IsDie())
			return;

		int nChangePoint = 0;
		for (int i = 0; i < MAX_SKILL_JOB; ++i)
		{
			LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
			if (aJobSkill[i].dwSkill == NULL_ID || lpSkill->dwSkill == NULL_ID)
				continue;

			if (aJobSkill[i].dwSkill != lpSkill->dwSkill)
			{
				Error("Skill index no match. src: %d, recv: %d", lpSkill->dwSkill, aJobSkill[i].dwSkill);
				return;
			}

			ItemProp* pSkillProp = lpSkill->GetProp();
			if (pSkillProp == NULL)
			{
				Error("Skill Prop not found?????: %d", aJobSkill[i].dwSkill);
				return;
			}

			if (aJobSkill[i].dwLevel == 0 || aJobSkill[i].dwLevel == lpSkill->dwLevel)
				continue;

			if (aJobSkill[i].dwLevel < lpSkill->dwLevel || aJobSkill[i].dwLevel > pSkillProp->dwExpertMax)
			{
				pUser->AddDefinedText(TID_RESKILLPOINT_ERROR);
				return;
			}

			if (pSkillProp->dwReqDisLV != NULL_ID)
			{
				if (pUser->IsMaster() == FALSE && pUser->IsHero() == FALSE)
				{
					if (pUser->GetLevel() < (int)(pSkillProp->dwReqDisLV))
						return;
				}
			}

			if (pSkillProp->dwReSkill1 != NULL_ID)
			{
				LPSKILL pReqSkill1 = NULL;
				pReqSkill1 = pUser->GetSkill(pSkillProp->dwReSkill1);
				if (pReqSkill1 == NULL || (pReqSkill1->dwLevel < pSkillProp->dwReSkillLevel1))
				{
					for (int j = 0; j < MAX_SKILL_JOB; ++j)
					{
						if (aJobSkill[j].dwSkill == pSkillProp->dwReSkill1)
						{
							pReqSkill1 = &aJobSkill[j];
							break;
						}
					}
				}

				if (pReqSkill1 == NULL)
				{
					int nIdx = pUser->GetSkillIdx(lpSkill->dwSkill);
					Error("%s NULL GetSkill %d = dwReSkill1(%d, %d)", pUser->GetName(), nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2);
					return;
				}

				if (pReqSkill1->dwLevel < pSkillProp->dwReSkillLevel1)
					return;
			}
			if (pSkillProp->dwReSkill2 != NULL_ID)
			{
				LPSKILL pReSkill2 = NULL;
				pReSkill2 = pUser->GetSkill(pSkillProp->dwReSkill2);
				if (pReSkill2 == NULL || (pReSkill2->dwLevel < pSkillProp->dwReSkillLevel2))
				{
					for (int j = 0; j < MAX_SKILL_JOB; ++j)
					{
						if (aJobSkill[j].dwSkill == pSkillProp->dwReSkill2)
						{
							pReSkill2 = &aJobSkill[j];
							break;
						}
					}
				}

				if (pReSkill2 == NULL)
				{
					int nIdx = pUser->GetSkillIdx(lpSkill->dwSkill);
					Error("%s NULL GetSkill %d = dwReSkill2(%d, %d)", pUser->GetName(), nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2);
					return;
				}

				if (pReSkill2->dwLevel < pSkillProp->dwReSkillLevel2)
					return;
			}

			if (aJobSkill[i].dwLevel > lpSkill->dwLevel)
			{
				int nPoint = (aJobSkill[i].dwLevel - lpSkill->dwLevel) * prj.GetSkillPoint(pSkillProp);
				nChangePoint += nPoint;
			}
		}

		if (nChangePoint <= 0)
			return;

		if (pUser->m_nSkillPoint < nChangePoint)
		{
			pUser->AddDefinedText(TID_RESKILLPOINT_ERROR);
			return;
		}

		pUser->m_nSkillPoint -= nChangePoint;
		for (int i = 0; i < MAX_SKILL_JOB; ++i)
		{
			LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
			if (aJobSkill[i].dwSkill == NULL_ID || lpSkill->dwSkill == NULL_ID)
				continue;

			if (aJobSkill[i].dwLevel > lpSkill->dwLevel)
				lpSkill->dwLevel = aJobSkill[i].dwLevel;
		}

		g_UserMng.AddCreateSfxObj(pUser, XI_SYS_EXCHAN01, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		pUser->AddDoUseSkillPoint(pUser->m_aJobSkill, pUser->m_nSkillPoint);
	}
}


void CDPSrvr::OnCloseGuildBankWnd(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (g_eLocal.GetState(ENABLE_GUILD_INVENTORY) == FALSE)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->m_bGuildBank = FALSE;
}

void CDPSrvr::OnBankToBank(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	BYTE nFlag, nPutSlot, nSlot;
	ar >> nFlag >> nPutSlot >> nSlot;

	if (nPutSlot >= 3 || nSlot >= 3)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif
	if (pUser->IsDie() == TRUE)
		return;

	if (pUser->m_idPlayerBank[nPutSlot] != 0 && pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank())
	{
		if (nFlag == 1)
		{
			DWORD dwItemObjID;
			int nItemNum;
			ar >> dwItemObjID >> nItemNum;
			CItemElem* pItemElem = pUser->m_Bank[nPutSlot].GetAtId(dwItemObjID);
			if (pItemElem == NULL)
				return;

			if (nItemNum > pItemElem->m_nItemNum)
				nItemNum = pItemElem->m_nItemNum;
			if (nItemNum < 1)
				nItemNum = 1;

			if (MAX_BANK > pUser->m_Bank[nSlot].GetCount())
			{
				CItemElem itemElem;
				itemElem = *pItemElem;
				itemElem.m_nItemNum = nItemNum;
				pUser->AddPutItemBank(nSlot, &itemElem);
				pUser->m_Bank[nSlot].Add(&itemElem);

				int nType = -1;
				switch (nPutSlot)
				{
				case 0: nType = TYPE_BANK_0; break;
				case 1: nType = TYPE_BANK_1; break;
				case 2: nType = TYPE_BANK_2; break;
				default: break;
				}

				pUser->RemoveItem((BYTE)dwItemObjID, nItemNum, nType);

#ifndef __FL_FIX_USELESS_LOGS
				LogItemInfo aLogItem;
				aLogItem.Action = "A";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "BANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot = nSlot;
				aLogItem.nSlot1 = nPutSlot;
				OnLogItem(aLogItem, &itemElem, itemElem.m_nItemNum);
#endif
			}
			else
				pUser->AddBankIsFull();
		}
		else
		{
			DWORD dwGold;
			ar >> dwGold;

			if (dwGold > pUser->m_dwGoldBank[nPutSlot])
				dwGold = pUser->m_dwGoldBank[nPutSlot];

			int nGold = dwGold;
			if (nGold < 0)
				return;

			if (CanAdd(pUser->m_dwGoldBank[nSlot], nGold))
			{
				pUser->m_dwGoldBank[nSlot] += nGold;
				pUser->m_dwGoldBank[nPutSlot] -= nGold;

				pUser->AddPutGoldBank(nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot]);
				pUser->AddPutGoldBank(nPutSlot, pUser->GetGold(), pUser->m_dwGoldBank[nPutSlot]);

#ifndef __FL_FIX_USELESS_LOGS
				LogItemInfo aLogItem;
				aLogItem.Action = "A";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "BANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
				aLogItem.itemNumber = nGold;
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot = nSlot;
				aLogItem.nSlot1 = nPutSlot;
				OnLogItem(aLogItem);
#endif
			}
		}
	}
}

void CDPSrvr::OnPutItemBank(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	BYTE nSlot;
	DWORD dwItemObjID;
	int nItemNum;

	ar >> nSlot >> dwItemObjID >> nItemNum;
	if (nSlot >= 3)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	if (pUser->IsDie() == TRUE)
		return;

	if (!pUser->m_bInstantBank)
	{
		if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_BANKING, pUser->GetWorld(), pUser->GetPos()))
			return;
	}

	if (pUser->m_nSlot == nSlot || (pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank()))
	{
		CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwItemObjID);
		if (IsUsableItem(pItemElem) == FALSE)
			return;

		if (nSlot != pUser->m_nSlot)
		{
			if (pItemElem->IsQuest())
				return;
		}

		if (pItemElem->IsBinds())
			return;

		ItemProp* pProp = pItemElem->GetProp();
		if (!pProp)
			return;

		if (pUser->IsUsing(pItemElem))
		{
			pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
			return;
		}

		if (pUser->m_Inventory.IsEquip(dwItemObjID))
		{
			pUser->AddDefinedText(TID_GAME_EQUIPTRADE, "");
			return;
		}

		if (nItemNum > pItemElem->m_nItemNum)
			nItemNum = pItemElem->m_nItemNum;
		if (nItemNum < 1)
			nItemNum = 1;

		if (pUser->m_Bank[nSlot].IsFull(pItemElem, pProp, nItemNum) == TRUE)
		{
			pUser->AddBankIsFull();
			return;
		}
		else
		{
			CItemElem itemElem;
			itemElem = *pItemElem;
			itemElem.m_nItemNum = nItemNum;

#ifndef __FL_FIX_USELESS_LOGS
			LogItemInfo aLogItem;
			aLogItem.Action = "P";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "BANK";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
			aLogItem.nSlot1 = nSlot;
			OnLogItem(aLogItem, pItemElem, nItemNum);
#endif

			pUser->RemoveItem((BYTE)dwItemObjID, nItemNum);
			pUser->m_Bank[nSlot].Add(&itemElem);
			pUser->AddPutItemBank(nSlot, &itemElem);
		}
	}
}

void CDPSrvr::OnPutItemGuildBank(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	if (g_eLocal.GetState(ENABLE_GUILD_INVENTORY) == FALSE)
		return;

	DWORD dwItemObjID;
	int nItemNum;
	BYTE mode;

	ar >> dwItemObjID >> nItemNum >> mode;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	if (pUser->IsDie() == TRUE)
		return;

	if (!pUser->GetWorld() || !GuildHouseMng->IsGuildHouse(pUser->GetWorld()->GetID()))
		if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_GUILDBANKING, pUser->GetWorld(), pUser->GetPos()))
			return;

	if (mode == 0)
		return;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwItemObjID);
	if (IsUsableItem(pItemElem) == FALSE)
		return;

	if (pItemElem->IsQuest())
		return;

	if (pItemElem->IsBinds())
		return;

	if (pUser->IsUsing(pItemElem))
	{
		pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
		return;
	}

	ItemProp* pItemProp = pItemElem->GetProp();
	if (pItemProp == NULL)
		return;

	if (pUser->m_Inventory.IsEquip(dwItemObjID))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPTRADE, "");
		return;
	}

	if (nItemNum > pItemElem->m_nItemNum)
		nItemNum = pItemElem->m_nItemNum;
	if (nItemNum < 1)
		nItemNum = 1;


	CGuild* pGuild = pUser->GetGuild();
	if (pGuild)
	{
		if (pGuild->m_GuildBank.IsFull(pItemElem, pItemProp, nItemNum))
		{
			pUser->AddDefinedText(TID_GAME_GUILDBANKFULL, "");
			return;
		}

		CItemElem itemElem;
		itemElem = *pItemElem;
		itemElem.m_nItemNum = nItemNum;

		pUser->RemoveItem((BYTE)dwItemObjID, nItemNum);
		pGuild->m_GuildBank.Add(&itemElem);

		UpdateGuildBank(pGuild, GUILD_PUT_ITEM, 0, pUser->m_idPlayer, &itemElem, 0, nItemNum);
		CWorld* pWorld = pUser->GetWorld();
		if (pWorld)
			g_dpDBClient.SavePlayer(pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer());

		LogItemInfo aLogItem;
		aLogItem.Action = "W";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "GUILDBANK";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		OnLogItem(aLogItem, &itemElem, nItemNum);

		pUser->AddPutItemGuildBank(&itemElem);

		CGuildMember* pMember;
		CUser* pUsertmp;
		map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		for (; i != pGuild->m_mapPMember.end(); ++i)
		{
			pMember = i->second;
			pUsertmp = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
			if (IsValidObj(pUsertmp) && pUsertmp != pUser && pUsertmp->m_bGuildBank)
				pUsertmp->AddPutItemGuildBank(&itemElem);
		}
	}
}

void CDPSrvr::OnGetItemGuildBank(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	if (g_eLocal.GetState(ENABLE_GUILD_INVENTORY) == FALSE)
		return;

	DWORD dwItemObjID = NULL_ID;
	int nItemNum = 0;
	BYTE byMode = 0;

	ar >> dwItemObjID >> nItemNum >> byMode;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CGuild* pGuild = pUser->GetGuild();
	if (pGuild == NULL)
		return;

	if (pGuild->IsMember(pUser->m_idPlayer) == FALSE)
		return;

	if (pUser->IsDie() == TRUE)
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (pWorld == NULL)
		return;

	if (GuildHouseMng->IsGuildHouse(pWorld->GetID()) == FALSE && CNpcChecker::GetInstance()->IsCloseNpc(MMI_GUILDBANKING, pWorld, pUser->GetPos()) == FALSE)
		return;

	if (byMode < 0 || byMode > 1)
		return;

	if (byMode == 0)
	{
		if (pGuild->IsGetPenya(pUser->m_idPlayer) == FALSE)
			return;

		const int nGetGold = nItemNum;
		if (nGetGold <= 0 || nGetGold > INT_MAX)
			return;

		if (static_cast<DWORD>(nGetGold) > pGuild->m_nGoldGuild)
			return;

		if (pUser->CheckUserGold(nGetGold, true) == false)
			return;

		pUser->AddGold(nGetGold, FALSE);
		pGuild->m_nGoldGuild -= nGetGold;
		pUser->AddGetGoldGuildBank(nGetGold, 0, pUser->m_idPlayer, 0);
		pGuild->DecrementMemberContribution(pUser->m_idPlayer, nGetGold, 0);
		UpdateGuildBank(pGuild, GUILD_GET_PENYA, 1, pUser->m_idPlayer, NULL, nGetGold);

		LogItemInfo aLogItem;
		aLogItem.Action = "Y";
		aLogItem.SendName = "GUILDBANK";
		aLogItem.RecvName = pUser->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold() - nGetGold;
		aLogItem.Gold2 = pUser->GetGold();
		_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
		aLogItem.itemNumber = nGetGold;
		OnLogItem(aLogItem);

		CGuildMember* pMember;
		CUser* pUsertmp;
		map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		for (; i != pGuild->m_mapPMember.end(); ++i)
		{
			pMember = i->second;
			pUsertmp = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
			if (IsValidObj(pUsertmp) && pUsertmp != pUser)
				pUsertmp->AddGetGoldGuildBank(nGetGold, 2, pUser->m_idPlayer, 0);
		}

		g_DPCoreClient.SendGuildMsgControl_Bank_Penya(pUser, nGetGold, 2, 0);
	}
	else if (byMode == 1)
	{
		if (pGuild->IsGetItem(pUser->m_idPlayer) == FALSE)
			return;

		CItemElem* const pItemElem = pGuild->m_GuildBank.GetAtId(dwItemObjID);
		if (NULL == pItemElem)
			return;

		ItemProp* pItemSpec = pItemElem->GetProp();
		if (pItemSpec == NULL)
			return;

		const int nGetItemQuantity = nItemNum;
		if (nGetItemQuantity <= 0 || static_cast<DWORD>(nGetItemQuantity) > pItemSpec->dwPackMax)
			return;

		if (nGetItemQuantity > pItemElem->m_nItemNum)
			return;

		if (pUser->m_Inventory.IsFull(pItemElem, pItemSpec, nGetItemQuantity) == TRUE)
		{
			pUser->AddBankIsFull();
			return;
		}

		CItemElem itemElem;
		itemElem = *pItemElem;
		itemElem.m_nItemNum = nGetItemQuantity;
		itemElem.m_dwObjId = pItemElem->m_dwObjId;

		if (pUser->m_Inventory.Add(&itemElem))
		{
			if (itemElem.m_nItemNum < pItemElem->m_nItemNum)
				pItemElem->m_nItemNum = pItemElem->m_nItemNum - nGetItemQuantity;
			else
				pGuild->m_GuildBank.RemoveAtId(dwItemObjID);

			UpdateGuildBank(pGuild, GUILD_GET_ITEM, 0, pUser->m_idPlayer, &itemElem, 0, (short)(nItemNum));
			LogItemInfo aLogItem;
			aLogItem.Action = "Y";
			aLogItem.SendName = "GUILDBANK";
			aLogItem.RecvName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			OnLogItem(aLogItem, &itemElem, nItemNum);

			pUser->AddGetItemGuildBank(&itemElem);

			CGuildMember* pMember;
			CUser* pUsertmp;
			map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
			for (; i != pGuild->m_mapPMember.end(); ++i)
			{
				pMember = i->second;
				pUsertmp = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
				if (IsValidObj(pUsertmp) && pUsertmp != pUser && pUsertmp->m_bGuildBank)
					pUsertmp->AddGetItemGuildBank(&itemElem, FALSE);
			}
		}
		else
			pUser->AddBankIsFull();
	}
	else
	{
		Error("[ INVALID GUILD BANK OUTPUT MODE. byMode(%d), PLAYER_ID(%07d) ]", byMode, pUser->m_idPlayer);
		return;
	}

	if (pUser->GetWorld())
		g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

}


void CDPSrvr::OnGuildBankMoveItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{

}

void CDPSrvr::UpdateGuildBank(CGuild* p_GuildBank, int p_Mode, BYTE cbUpdate, u_long idPlayer, CItemElem* pItemElem, DWORD dwPenya, short nItemCount)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GUILD_BANK_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN);

	if (p_GuildBank)
	{
		ar << p_GuildBank->m_idGuild;
		ar << p_GuildBank->m_nGoldGuild;
		p_GuildBank->m_GuildBank.Serialize(ar);
		ar << cbUpdate;
		ar << idPlayer;
		ar << p_Mode;
		if (pItemElem == NULL)
		{
			ar << (DWORD)0;
			ar << (int)0;
			ar << (SERIALNUMBER)0;
		}
		else
		{
			ar << pItemElem->m_dwItemId;
			ar << pItemElem->GetAbilityOption();
			ar << pItemElem->GetSerialNumber();
		}
		ar << nItemCount;
		ar << dwPenya;
		SEND(ar, &g_dpDBClient, DPID_SERVERPLAYER);
	}
}


void CDPSrvr::OnGetItemBank(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	BYTE nSlot;
	DWORD dwItemObjID;
	int nItemNum;

	ar >> nSlot >> dwItemObjID >> nItemNum;
	if (nSlot >= 3)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser))
	{
#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif

		if (pUser->IsDie() == TRUE)
			return;

		if (!pUser->m_bInstantBank)
		{
			if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_BANKING, pUser->GetWorld(), pUser->GetPos()))
				return;
		}

		if (pUser->m_nSlot == nSlot || (pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank()))
		{
			CItemElem* pItemElem = pUser->m_Bank[nSlot].GetAtId(dwItemObjID);
			if (NULL == pItemElem)
				return;

			ItemProp* pItemProp = pItemElem->GetProp();
			if (!pItemProp)
				return;

			if (nItemNum > pItemElem->m_nItemNum)
				nItemNum = pItemElem->m_nItemNum;
			if (nItemNum < 1)
				nItemNum = 1;

			if (pUser->m_Inventory.IsFull(pItemElem, pItemProp, nItemNum))
			{
				pUser->AddBankIsFull();
				return;
			}

			CItemElem itemElem;
			itemElem = *pItemElem;
			itemElem.m_nItemNum = nItemNum;

#ifndef __FL_FIX_USELESS_LOGS
			LogItemInfo aLogItem;
			aLogItem.Action = "G";
			aLogItem.SendName = "BANK";
			aLogItem.RecvName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
			aLogItem.nSlot = nSlot;
			OnLogItem(aLogItem, pItemElem, nItemNum);
#endif

			int nType = -1;
			switch (nSlot)
			{
			case 0: nType = TYPE_BANK_0; break;
			case 1: nType = TYPE_BANK_1; break;
			case 2: nType = TYPE_BANK_2; break;
			default: break;
			}

			pUser->RemoveItem((BYTE)dwItemObjID, nItemNum, nType);
			pUser->m_Inventory.Add(&itemElem);
			pUser->AddGetItemBank(&itemElem);
		}
	}
}

void CDPSrvr::OnPutGoldBank(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpbuf*/, u_long /*uBufSize*/)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	DWORD dwGold;
	BYTE nSlot;
	ar >> nSlot >> dwGold;

	int nGold = dwGold;
	if (nGold <= 0)
		return;

	if (nSlot >= 3)
		return;

	if (IsValidObj(pUser))
	{
		if (pUser->IsDie() == TRUE)
			return;

		if (!pUser->m_bInstantBank)
		{
			if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_BANKING, pUser->GetWorld(), pUser->GetPos()))
				return;
		}

		if (pUser->m_nSlot == nSlot || (pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank()))
		{
			if (pUser->CheckUserGold(nGold, false) == false)
				return;

			if (CanAdd(pUser->m_dwGoldBank[nSlot], nGold) == TRUE)
			{
#ifndef __FL_FIX_USELESS_LOGS
				LogItemInfo aLogItem;
				aLogItem.Action = "P";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "BANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold() - nGold;
				_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
				aLogItem.itemNumber = nGold;
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot1 = nSlot;
				OnLogItem(aLogItem);
#endif

				pUser->m_dwGoldBank[nSlot] += nGold;
				pUser->AddGold(-nGold, FALSE);

				pUser->AddPutGoldBank(nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot]);
			}
		}
	}
}


void CDPSrvr::OnGetGoldBank(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpbuf*/, u_long /*uBufSize*/)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	BYTE nSlot;
	DWORD dwMoveGold;
	ar >> nSlot >> dwMoveGold;

	int nMoveGold = dwMoveGold;
	if (nMoveGold <= 0)
		return;

	if (nSlot >= 3)
		return;

	if (IsValidObj(pUser) == FALSE)
		return;

	if (pUser->IsDie() == TRUE)
		return;

	if (!pUser->m_bInstantBank)
	{
		if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_BANKING, pUser->GetWorld(), pUser->GetPos()))
			return;
	}

	if (pUser->m_nSlot == nSlot || (pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank()))
	{
		int nBankGold = pUser->m_dwGoldBank[nSlot];
		if (nBankGold < 0)
			return;

#undef min
		nMoveGold = std::min(nBankGold, nMoveGold);

		if (pUser->CheckUserGold(nMoveGold, true) == false)
			return;

#ifndef __FL_FIX_USELESS_LOGS
		LogItemInfo aLogItem;
		aLogItem.Action = "G";
		aLogItem.SendName = "BANK";
		aLogItem.RecvName = pUser->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold() + nMoveGold;
		_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
		aLogItem.itemNumber = nMoveGold;
		aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
		aLogItem.nSlot = nSlot;
		OnLogItem(aLogItem);
#endif

		pUser->AddGold(nMoveGold, FALSE);
		pUser->m_dwGoldBank[nSlot] -= nMoveGold;
		pUser->AddPutGoldBank(nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot]);
	}
}

void CDPSrvr::OnMoveBankItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
}

void CDPSrvr::OnChangeBankPass(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	char szLastPass[10] = { 0, };
	char szNewPass[10] = { 0, };
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		ar.ReadString(szLastPass, 10);
		ar.ReadString(szNewPass, 10);

		if (strlen(szLastPass) > 4 || strlen(szNewPass) > 4)
		{
			WriteError("%s %d, %s, %s", __FILE__, __LINE__, szLastPass, szNewPass);
			return;
		}

		DWORD dwId, dwItemId;
		ar >> dwId >> dwItemId;


		if (0 == strcmp(szLastPass, pUser->m_szBankPass))
		{

			strcpy(pUser->m_szBankPass, szNewPass);
			g_dpDBClient.SendChangeBankPass(pUser->GetName(), szNewPass, pUser->m_idPlayer);
			pUser->AddChangeBankPass(1, dwId, dwItemId);
		}
		else
		{


			pUser->AddChangeBankPass(0, dwId, dwItemId);
		}
	}
}

void CDPSrvr::OnConfirmBank(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	char szPass[10] = { 0, };
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_vtInfo.GetOther())
			return;
		if (pUser->m_vtInfo.VendorIsVendor())
			return;
		if (pUser->m_bGuildBank)
			return;
#ifdef __S_SERVER_UNIFY
		if (pUser->m_bAllAction == FALSE)
			return;
#endif // __S_SERVER_UNIFY
#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif

		ar.ReadString(szPass, 10);

		DWORD dwId, dwItemId;
		ar >> dwId >> dwItemId;

		if (dwId == NULL_ID && !CNpcChecker::GetInstance()->IsCloseNpc(MMI_BANKING, pUser->GetWorld(), pUser->GetPos()))
			return;

		if (0 == strcmp(szPass, pUser->m_szBankPass))
		{
			if (dwId != NULL_ID)
			{
				CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwId);
				if (IsUsableItem(pItemElem) == FALSE || pItemElem->m_dwItemId != II_SYS_SYS_SCR_CUSTODY)
				{
					return;
				}
				else
				{
					pUser->m_bInstantBank = TRUE;
					pUser->RemoveItem((BYTE)(pItemElem->m_dwObjId), 1);
				}
			}
			pUser->m_bBank = TRUE;
			pUser->AddconfirmBankPass(1, dwId, dwItemId);
		}
		else
			pUser->AddconfirmBankPass(0, dwId, dwItemId);
	}
}

void CDPSrvr::OnSfxHit(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int idSfxHit;
	DWORD dwSkill;
	OBJID idAttacker;
	CMover* pAttacker = NULL;

	ar >> idSfxHit >> dwSkill >> idAttacker;

	if (idAttacker == NULL_ID)
		pAttacker = g_UserMng.GetUser(dpidCache, dpidUser);
	else
		pAttacker = prj.GetMover(idAttacker);

	if (IsValidObj(pAttacker) == FALSE)
		return;

	PSfxHit pSfxHit = pAttacker->m_sfxHitArray.GetSfxHit(idSfxHit);
	if (pSfxHit == NULL)
		return;

	CMover* pTarget = prj.GetMover(pSfxHit->objid);

	pAttacker->RemoveSFX(pSfxHit->objid, idSfxHit, (IsInvalidObj(pTarget) || pTarget->IsDie()), dwSkill);
	pAttacker->m_sfxHitArray.RemoveSfxHit(idSfxHit, TRUE);
}


void CDPSrvr::OnSfxID(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID	idTarget;
	int		idSfxHit;
	DWORD	dwType, dwSkill;
	int		nMaxDmgCnt;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		ar >> idTarget >> idSfxHit >> dwType >> dwSkill >> nMaxDmgCnt;
		pUser->m_sfxHitArray.Add(idSfxHit, idTarget, dwType, dwSkill, nMaxDmgCnt);
	}
}


void CDPSrvr::OnSfxClear(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID	idMover = NULL_ID;
	int	  idSfxHit;

	ar >> idSfxHit;
	ar >> idMover;

	CMover* pMover;

	if (idMover == NULL_ID)
		pMover = g_UserMng.GetUser(dpidCache, dpidUser);
	else
		pMover = prj.GetMover(idMover);

	if (IsValidObj(pMover))
	{
		PSfxHit pSfxHit = pMover->m_sfxHitArray.GetSfxHit(idSfxHit);
		pMover->m_sfxHitArray.RemoveSfxHit(idSfxHit, TRUE);
	}
}

void CDPSrvr::OnMeleeAttack(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	OBJMSG dwAtkMsg;
	OBJID objid;

	ar >> dwAtkMsg >> objid;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CMover* pTargetObj = prj.GetMover(objid);
	if (IsValidObj(pTargetObj) == FALSE)
		return;

	if (pUser->IsDisguise() == TRUE)
		return;

	if (pUser->IsDie() == TRUE || pTargetObj->IsDie() == TRUE)
		return;

	if (pTargetObj->IsMode(TRANSPARENT_MODE))
		return;

#ifdef __FL_FIX_BLADEHACK
#ifdef __WORLDSERVER
	bool bHack = false;
	switch (dwAtkMsg)
	{
	case OBJMSG_ATK1:
		pUser->SetLastAnimation(1);
		break;
	case OBJMSG_ATK2:
		if (pUser->GetLastAnimation() == 1)
			pUser->SetLastAnimation(2);
		else
			bHack = true;
		break;
	case OBJMSG_ATK3:
		if (pUser->GetLastAnimation() == 2)
			pUser->SetLastAnimation(3);
		else
			bHack = true;
		break;
	case OBJMSG_ATK4:
		if (pUser->GetLastAnimation() == 3)
			pUser->SetLastAnimation(4);
		else
			bHack = true;
		break;
	default: bHack = true; break;
	}

	if (bHack)
	{
		pUser->SetLastAnimation(0);
		return;
	}
#endif
#endif

	pUser->DoAttackMelee(pTargetObj, dwAtkMsg, 0, dwAtkMsg == OBJMSG_ATK1);
}

void CDPSrvr::OnMeleeAttack2(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objid;
	ar >> objid;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->GetIndex() == 0)
		{
			WriteError("PACKETTYPE_MELEE_ATTACK2");
			return;
		}

		CMover* pTargetObj = prj.GetMover(objid);
		if (IsValidObj(pTargetObj))
		{
			if (pUser->IsDie() == TRUE || pTargetObj->IsDie() == TRUE || pUser->IsFly() == FALSE)
				return;

			int nRet = pUser->SendActMsg(OBJMSG_ATK1, objid);

			if (nRet == 0)
				pUser->m_pActMover->m_qMeleeAtkMsg.AddTail(new ACTMSG(OBJMSG_ATK1, objid, 0, 0));

			if (nRet != -2)
				g_UserMng.AddMeleeAttack2(pUser, objid);
		}
	}
}


void CDPSrvr::OnMagicAttack(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	OBJID objid;
	int nMagicPower, idSfxHit;
	ar >> objid >> nMagicPower >> idSfxHit;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CMover* pTargetObj = prj.GetMover(objid);
	if (!IsValidObj(pTargetObj))
		return;

	if (pUser->IsDie() == TRUE || pTargetObj->IsDie() == TRUE)
		return;

	if (pTargetObj->IsMode(TRANSPARENT_MODE))
		return;

	int nRet = pUser->DoAttackMagic(pTargetObj, nMagicPower, idSfxHit);

	if (nRet == 0 && pUser->IsFly() == FALSE)
		pUser->m_pActMover->m_qMagicAtkMsg.AddTail(new MAGICATKMSG(OBJMSG_ATK_MAGIC1, objid, 0, 0, nMagicPower, idSfxHit));
}


void CDPSrvr::OnRangeAttack(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	OBJID objid = 0;
	DWORD dwItemID = 0;
	int idSfxHit = 0;

	ar >> objid >> dwItemID >> idSfxHit;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CMover* pTargetObj = prj.GetMover(objid);
	if (IsValidObj(pTargetObj) == FALSE)
		return;

	if (pUser->IsDisguise() == TRUE)
		return;

	if (pUser->IsDie() == TRUE || pTargetObj->IsDie() == TRUE)
		return;

	if (pTargetObj->IsMode(TRANSPARENT_MODE))
		return;

	ItemProp* pHandItemProp = pUser->GetActiveHandItemProp();
	if (pHandItemProp == NULL)
		return;

	if (pUser->DoAttackRange(pTargetObj, dwItemID, idSfxHit) == 0)
		pUser->m_pActMover->m_qMagicAtkMsg.AddTail(new MAGICATKMSG(OBJMSG_ATK_RANGE1, objid, 1, dwItemID, 0, idSfxHit));
}

void CDPSrvr::OnTeleSkill(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	D3DXVECTOR3 vPos;
	ar >> vPos;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (pUser->IsDie())
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return;

	LPSKILL pSkill = pUser->GetSkill(SI_MAG_MAG_BLINKPOOL);
	if (!pSkill || pSkill->dwLevel == 0)
		return;

	ItemProp* pPropSkill = pSkill->GetProp();
	if (!pPropSkill)
		return;

	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pPropSkill->dwSubDefine, pSkill->dwLevel);
	if (!pAddSkillProp)
		return;

	const float fMaxDistance = (float)pAddSkillProp->dwSkillRange;
	const D3DXVECTOR3 vPosDifference = vPos - pUser->GetPos();
	float fTotalDistance = ::sqrt(D3DXVec3LengthSq(&vPosDifference));
	if (fTotalDistance > fMaxDistance)
		fTotalDistance = fMaxDistance;

	const float fPower = fTotalDistance * 0.1f;
	pUser->pushedByAnotherPower(GetDegree(vPos, pUser->GetPos()), fPower);
}

void CDPSrvr::OnSetTarget(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID idTarget;
	BYTE bClear;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		ar >> idTarget >> bClear;

		pUser->ClearCastingSkills(TRUE);

		if (bClear == 2)
			pUser->m_idSetTarget = idTarget;

		if (bClear < 2)
		{
			CMover* pTarget = prj.GetMover(idTarget);
			if (IsValidObj(pTarget))
			{
				if (pUser->IsDie() || pTarget->IsDie())
					return;

				if (bClear)
				{
					if (pTarget->m_idTargeter == pUser->GetId())
						pTarget->m_idTargeter = NULL_ID;
				}
				else
				{
					if (pTarget->m_idTargeter == NULL_ID)
						pTarget->m_idTargeter = pUser->GetId();
				}
			}
		}
	}
}


void CDPSrvr::OnSnapshot(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		BYTE c;
		ar >> c;

		while (c--)
		{
			WORD wHdr;
			ar >> wHdr;
			switch (wHdr)
			{
			case SNAPSHOTTYPE_DESTPOS:	OnPlayerDestPos(ar, pUser);	break;
			default:
			{
				ASSERT(0);
				break;
			}
			}
		}
	}
}


void CDPSrvr::OnPlayerDestPos(CAr& ar, CUser* pUser)
{
	D3DXVECTOR3 vPos;
	BYTE fForward;
	ar >> vPos >> fForward;

	if (IsValidObj(pUser) == FALSE)
		return;
	if (pUser->IsDie() == TRUE)
		return;

	bool bForward = (fForward != 0);

	pUser->SetDestPos(vPos, bForward);
	g_UserMng.AddSetDestPos(pUser, vPos, bForward);
}

void CDPSrvr::OnModifyMode(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		DWORD dwMode;
		BYTE f;
		u_long idFrom;

		ar >> dwMode >> f >> idFrom;

		DPID dpid;
		ar >> dpid;
		if (pUser->m_Snapshot.dpidUser != dpid)
		{
			Error("[%s] try to hack : PACKETTYPE_MODIFYMODE", pUser->GetName());
			return;
		}

		if (f)
			pUser->m_dwMode |= dwMode;
		else
			pUser->m_dwMode &= ~dwMode;

		g_UserMng.AddModifyMode(pUser);
	}
}

void CDPSrvr::OnChangeFace(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwFaceNum;
	BOOL bUseCoupon;

	ar >> dwFaceNum >> bUseCoupon;

	if (dwFaceNum >= MAX_HEAD || dwFaceNum < 0)
		return;

	int cost = CHANGE_FACE_COST;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (dwFaceNum == pUser->m_dwHeadMesh)
			return;

		if (!bUseCoupon)
		{
			if (pUser->CheckUserGold(cost, false) == false)
			{
				pUser->AddDefinedText(TID_GAME_LACKMONEY, "");
				return;
			}
			pUser->AddGold(-(cost));
		}
		else
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtItemId(II_SYS_SYS_SCR_FACEOFFFREE);
			if (IsUsableItem(pItemElem) == FALSE)
			{
				pUser->AddDefinedText(TID_GAME_WARNNING_COUPON, "");
				return;
			}

			pUser->RemoveItem((BYTE)(pItemElem->m_dwObjId), 1);
		}
		pUser->SetHead(dwFaceNum);
		g_UserMng.AddChangeFace(pUser->m_idPlayer, dwFaceNum);
	}
}

void CDPSrvr::OnExpUp(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if ((DWORD)AUTH_GAMEMASTER <= pUser->m_dwAuthorization)
		{
			EXPINTEGER nExp;
			ar >> nExp;

			if (pUser->AddExperience(nExp, TRUE, TRUE, TRUE))
				pUser->LevelUpSetting();

			pUser->AddSetExperience(pUser->GetExp1(), (WORD)pUser->m_nLevel, pUser->m_nSkillPoint, pUser->m_nSkillLevel);
		}
	}
}

void	CDPSrvr::OnLogItem(LogItemInfo& info, CItemElem* pItemElem, int nItemCount)
{
	if (pItemElem != NULL)
	{
		info.ItemNo = pItemElem->GetSerialNumber();
		_stprintf(info.szItemName, "%d", pItemElem->GetProp()->dwID);
		info.itemNumber = nItemCount;
		info.nItemResist = pItemElem->m_bItemResist;
		info.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
		info.nAbilityOption = pItemElem->GetAbilityOption();
		info.Negudo = pItemElem->m_nHitPoint;
		info.m_dwKeepTime = pItemElem->m_dwKeepTime;
		info.nPiercedSize = pItemElem->GetPiercingSize();
		for (int i = 0; i < pItemElem->GetPiercingSize(); i++)
			info.adwItemId[i] = pItemElem->GetPiercingItem(i);
		info.nUMPiercedSize = pItemElem->GetUltimatePiercingSize();
		for (int i = 0; i < pItemElem->GetUltimatePiercingSize(); i++)
			info.adwUMItemId[i] = pItemElem->GetUltimatePiercingItem(i);
		info.m_iRandomOptItemId = pItemElem->GetRandomOptItemId();
		if (pItemElem->m_pPet)
		{
			CPet* pPet = pItemElem->m_pPet;

			info.nPetKind = pPet->GetKind();
			info.nPetLevel = pPet->GetLevel();
			info.dwPetExp = pPet->GetExp();
			info.wPetEnergy = pPet->GetEnergy();
			info.wPetLife = pPet->GetLife();
			info.nPetAL_D = pPet->GetAvailLevel(PL_D);
			info.nPetAL_C = pPet->GetAvailLevel(PL_C);
			info.nPetAL_B = pPet->GetAvailLevel(PL_B);
			info.nPetAL_A = pPet->GetAvailLevel(PL_A);
			info.nPetAL_S = pPet->GetAvailLevel(PL_S);
		}
		else if (pItemElem->IsTransformVisPet() == TRUE)
		{
			info.nPetKind = (BYTE)100;
		}
	}

	BEFORESENDDUAL(ar, PACKETTYPE_LOG_ALLITEM, DPID_UNKNOWN, DPID_UNKNOWN);
	ar.WriteString(info.Action);
	ar.WriteString(info.SendName);
	ar.WriteString(info.RecvName);
	ar << info.WorldId;
	ar << info.Gold;
	ar << info.Gold2;
	ar << info.ItemNo;
	ar << info.Negudo;
	if (_tcslen(info.szItemName) == 0)
	{
		_stprintf(info.szItemName, "%d", -1);
	}
	ar.WriteString(info.szItemName);
	ar << info.itemNumber;
	ar << info.nAbilityOption;
	ar << info.Gold_1;
	ar << info.nSlot;
	ar << info.nSlot1;
	ar << info.nItemResist;
	ar << info.nResistAbilityOption;
	ar << info.m_dwKeepTime;
	ar << info.nPiercedSize;
	for (int i = 0; i < info.nPiercedSize; i++)
		ar << info.adwItemId[i];
	ar << info.nUMPiercedSize;
	for (int i = 0; i < info.nUMPiercedSize; i++)
		ar << info.adwUMItemId[i];
	ar << info.m_iRandomOptItemId;
	ar << info.nPetKind;
	ar << info.nPetLevel;
	ar << info.dwPetExp;
	ar << info.wPetEnergy;
	ar << info.wPetLife;
	ar << info.nPetAL_D;
	ar << info.nPetAL_C;
	ar << info.nPetAL_B;
	ar << info.nPetAL_A;
	ar << info.nPetAL_S;

	SEND(ar, &g_dpDBClient, DPID_SERVERPLAYER);
}


void CDPSrvr::OnSetHair(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nHair, nR, nG, nB;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
#ifdef __NEWYEARDAY_EVENT_COUPON
		BOOL bUseCoupon;
		ar >> nHair >> nR >> nG >> nB >> bUseCoupon;
#else //__NEWYEARDAY_EVENT_COUPON
		ar >> nHair >> nR >> nG >> nB;// >> nCost;
#endif //__NEWYEARDAY_EVENT_COUPON

		int nCost;
		nCost = CMover::GetHairCost((CMover*)pUser, nR, nG, nB, nHair);

#ifdef __NEWYEARDAY_EVENT_COUPON
		if (pUser->CheckUserGold(nCost, false) == false && bUseCoupon == FALSE)
#else //__NEWYEARDAY_EVENT_COUPON
		if (pUser->CheckUserGold(nCost, false) == false)
#endif //__NEWYEARDAY_EVENT_COUPON
		{
			pUser->AddDefinedText(TID_GAME_LACKMONEY, "");
			return;
		}

		pUser->SetHair(nHair);
		float r, g, b;
		r = (float)nR / 255.0f;
		g = (float)nG / 255.0f;
		b = (float)nB / 255.0f;

#ifdef __NEWYEARDAY_EVENT_COUPON
		if (!bUseCoupon)
			pUser->AddGold(-(nCost));
		else
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtItemId(II_SYS_SYS_SCR_HAIRCHANGE);
			if (IsUsableItem(pItemElem) == FALSE)
			{
				pUser->AddDefinedText(TID_GAME_WARNNING_COUPON, "");
				return;
			}

			pUser->RemoveItem((BYTE)(pItemElem->m_dwObjId), 1);
		}
#else //__NEWYEARDAY_EVENT_COUPON
		pUser->AddGold(-(nCost));
#endif //__NEWYEARDAY_EVENT_COUPON

		pUser->SetHairColor(r, g, b);
		g_UserMng.AddSetHair(pUser, nHair, nR, nG, nB);
	}
}

void CDPSrvr::OnBlock(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nGu;
	char szNameFrom[MAX_NAME] = { 0, };

	ar >> nGu;
	ar.ReadString(szNameFrom, MAX_NAME);

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	u_long uidPlayerFrom = CPlayerDataCenter::GetInstance()->GetPlayerId(szNameFrom);
	if (uidPlayerFrom == 0)
		return;

	if (pUser->m_idPlayer == uidPlayerFrom)
		return;

	g_DPCoreClient.SendBlock(nGu, pUser->m_idPlayer, (char*)pUser->GetName(), uidPlayerFrom);
}


void CDPSrvr::OnItemTransy(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objidTarget, objidTransy;
	ar >> objidTarget;
	ar >> objidTransy;
#ifdef __SYS_ITEMTRANSY
	DWORD dwChangeId;
	BOOL bCash;
	ar >> dwChangeId;
	ar >> bCash;
#endif // __SYS_ITEMTRANSY
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		CItemUpgrade::GetInstance()->OnItemTransy(pUser, objidTarget, objidTransy, dwChangeId, bCash);

}


void CDPSrvr::OnPiercing(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId1, dwId2;

	ar >> dwId1;
	ar >> dwId2;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser))
		CItemUpgrade::GetInstance()->OnPiercing(pUser, dwId1, dwId2);
}


void CDPSrvr::OnPiercingRemove(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objId;
	ar >> objId;
	int nSlot;
	ar >> nSlot;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CItemUpgrade::GetInstance()->OnPiercingRemove(pUser, objId, nSlot);
}

void CDPSrvr::OnCreateSfxObj(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwSfxId;
	u_long uIdPlayer;
	BOOL bFlag;
	ar >> dwSfxId >> uIdPlayer >> bFlag;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (uIdPlayer == NULL_ID)
		{
			g_UserMng.AddCreateSfxObj((CMover*)pUser, dwSfxId, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z, bFlag);
		}
		else
		{
			CUser* pUsertmp = (CUser*)prj.GetUserByID(uIdPlayer);
			if (IsValidObj(pUsertmp))
				g_UserMng.AddCreateSfxObj((CMover*)pUsertmp, dwSfxId, pUsertmp->GetPos().x, pUsertmp->GetPos().y, pUsertmp->GetPos().z, bFlag);
		}
	}
}

void CDPSrvr::OnRemoveItemLevelDown(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId;
	ar >> dwId;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwId);
		if (!IsUsableItem(pItemElem))
			return;

		if (pUser->m_Inventory.IsEquip(pItemElem->m_dwObjId))
		{
			pUser->AddDiagText(prj.GetText(TID_GAME_EQUIPPUT));
			return;
		}

		if (pItemElem->GetLevelDown() == 0)
		{
			pUser->AddDefinedText(TID_GAME_INVALID_TARGET_ITEM);
			return;
		}
#ifndef __FL_FIX_USELESS_LOGS
		PutItemLog(pUser, "v", "OnRemoveItemLevelDown", pItemElem);
#endif
		pItemElem->SetLevelDown();
		pUser->UpdateItemEx((BYTE)(pItemElem->m_dwObjId), UI_RANDOMOPTITEMID, pItemElem->GetRandomOptItemId());
	}
}

void CDPSrvr::OnDoUseItemTarget(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwMaterial, dwTarget;
	ar >> dwMaterial >> dwTarget;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!pUser->IsPossibleUseItem())
		return;

	CItemElem* pMaterial = pUser->m_Inventory.GetAtId(dwMaterial);
	CItemElem* pTarget = pUser->m_Inventory.GetAtId(dwTarget);
	if (!IsUsableItem(pMaterial) || !IsUsableItem(pTarget))
		return;

	if (pUser->m_Inventory.IsEquip(dwMaterial) || pUser->m_Inventory.IsEquip(dwTarget))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPPUT);
		return;
	}
	BOOL	b = FALSE;
	switch (pMaterial->m_dwItemId)
	{
	case II_SYS_SYS_QUE_PETRESURRECTION02_S:
	case II_SYS_SYS_QUE_PETRESURRECTION02_A:
	case II_SYS_SYS_QUE_PETRESURRECTION02_B:
	{
		CPet* pPet = pTarget->m_pPet;
		if (!pPet || !pTarget->IsFlag(CItemElem::expired))
		{
			pUser->AddDefinedText(TID_GAME_PETRESURRECTION_WRONG_TARGET_01);
		}
		else
		{
			BYTE nLevel = pPet->GetLevel();
			if ((nLevel == PL_B && pMaterial->m_dwItemId != II_SYS_SYS_QUE_PETRESURRECTION02_B)
				|| (nLevel == PL_A && pMaterial->m_dwItemId != II_SYS_SYS_QUE_PETRESURRECTION02_A)
				|| (nLevel == PL_S && pMaterial->m_dwItemId != II_SYS_SYS_QUE_PETRESURRECTION02_S)
				|| (nLevel < PL_B)
				)
			{
				pUser->AddDefinedText(TID_GAME_PETRESURRECTION_WRONG_TARGET_02);
			}
			else
			{
#ifndef __FL_FIX_USELESS_LOGS
				PutItemLog(pUser, "r", "::PetResurrection", pTarget);
#endif
				pTarget->ResetFlag(CItemElem::expired);
				pUser->UpdateItem((BYTE)(pTarget->m_dwObjId), UI_FLAG, MAKELONG(pTarget->m_dwObjIndex, pTarget->m_byFlag));
				pPet->SetLife(0);
				pPet->SetEnergy(pPet->GetMaxEnergy() / 2);
				pUser->AddPetState(pTarget->m_dwObjId, pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp());
				pUser->AddDefinedText(TID_GAME_PETRESURRECTION_SUCCESS);
				b = TRUE;
			}
		}
		break;
	}
	case II_SYS_SYS_SCR_AWAKECANCEL:
	case II_SYS_SYS_SCR_AWAKECANCEL02:
		b = DoUseItemTarget_InitializeRandomOption(pUser, pTarget, CRandomOptionProperty::eAwakening,
			TID_GAME_AWAKECANCEL_INFO, TID_GAME_AWAKECANCEL,
			"r", "::AwakeCancel");
		break;
	case II_SYS_SYS_SCR_AWAKE:
		b = DoUseItemTarget_GenRandomOption(pUser, pTarget, CRandomOptionProperty::eAwakening,
			0, TID_GAME_INVALID_TARGET_ITEM, TID_GAME_AWAKE_OR_BLESSEDNESS01,
			"r", "::Awake");
		break;
	case II_SYS_SYS_SCR_BLESSEDNESS:
	case II_SYS_SYS_SCR_BLESSEDNESS02:
		b = DoUseItemTarget_GenRandomOption(pUser, pTarget, CRandomOptionProperty::eBlessing,
			0, TID_GAME_USE_BLESSEDNESS_INFO, TID_GAME_BLESSEDNESS_INVALID_ITEM,
			"r", "::Blessedness");
		break;
	case II_SYS_SYS_SCR_EATPETAWAKE:
		b = DoUseItemTarget_GenRandomOption(pUser, pTarget, CRandomOptionProperty::eEatPet,
			TID_GAME_PETAWAKE_S00, TID_GAME_PETAWAKE_E00, TID_GAME_PETAWAKE_E00,
			"r", "EATPETAWAKE");
		break;
	case II_SYS_SYS_SCR_PETAWAKE:
		b = DoUseItemTarget_GenRandomOption(pUser, pTarget, CRandomOptionProperty::eSystemPet,
			TID_GAME_PETAWAKE_S00, TID_GAME_PETAWAKE_E00, TID_GAME_PETAWAKE_E00,
			"r", "PETAWAKE");
		break;
	case II_SYS_SYS_SCR_PETAWAKECANCEL:
		b = DoUseItemTarget_InitializeRandomOption(pUser, pTarget, CRandomOptionProperty::eSystemPet,
			TID_GAME_PETAWAKECANCEL_S00, TID_GAME_PETAWAKECANCEL_E00,
			"r", "PETAWAKECANCEL");
		break;
	case II_SYS_SYS_SCR_LEVELDOWN01:
	case II_SYS_SYS_SCR_LEVELDOWN02:
		b = DoUseItemTarget_ItemLevelDown(pUser, pMaterial, pTarget);
		break;
	case II_SYS_SYS_SCR_CANCELLATION:
		b = UnbindScroll(pUser, pMaterial, pTarget);
		break;
#ifdef __MODEL_CHANGE
	case II_SYS_SYS_ITEMMODELRETURN:
		if (b = (pTarget->GetLook() != NULL_ID))
		{
			if (pUser->IsUsingEatPet(pTarget))
			{
				b = FALSE;
				pUser->AddDefinedText(TID_MMI_PET_RELEASE);
				return;
			}
			pTarget->SetLook(NULL_ID);
			pUser->AddLookChange(pTarget->m_dwObjId, NULL_ID);
			pUser->AddDefinedText(TID_MMI_LOOKCHANGE05);
		}
		else
			pUser->AddDefinedText(TID_MMI_LOOKCHANGE02);

		break;
#endif
	default:
		break;
	}
	if (b)
	{
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
		if (pUser->IsMode(TRANSPARENT_MODE) == 0)
			g_UserMng.AddCreateSfxObj(pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

#ifndef __FL_FIX_USELESS_LOGS
		PutItemLog(pUser, "u", "OnDoUseItemTarget", pMaterial);
#endif

		pUser->RemoveItem((BYTE)(pMaterial->m_dwObjId), 1);
	}
}

void CDPSrvr::OnSmeltSafety(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID dwItemId, dwItemMaterialId, dwItemProtScrId, dwItemSmeltScrId;

	ar >> dwItemId >> dwItemMaterialId >> dwItemProtScrId >> dwItemSmeltScrId;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser))
	{
		if (pUser->m_vtInfo.GetOther() || pUser->m_vtInfo.VendorIsVendor())
		{
			pUser->AddSmeltSafety(0);
			return;
		}

		CItemElem* pItemElem0 = pUser->m_Inventory.GetAtId(dwItemId);
		CItemElem* pItemElem1 = pUser->m_Inventory.GetAtId(dwItemMaterialId);
		CItemElem* pItemElem2 = pUser->m_Inventory.GetAtId(dwItemProtScrId);
		CItemElem* pItemElem3 = NULL;
		if (dwItemSmeltScrId != NULL_ID)
		{
			pItemElem3 = pUser->m_Inventory.GetAtId(dwItemSmeltScrId);
			if (!IsUsableItem(pItemElem3))
				return;
		}

		if (IsUsableItem(pItemElem0) == FALSE || IsUsableItem(pItemElem1) == FALSE || IsUsableItem(pItemElem2) == FALSE)
		{
			pUser->AddSmeltSafety(0);
			return;
		}


		if (pUser->m_Inventory.IsEquip(dwItemId))
		{
			pUser->AddSmeltSafety(0);
			return;
		}

#ifdef __AIO_PETS
		if (pItemElem0->IsEatPet())
		{
			pUser->AddSmeltSafety(0);
			return;
		}
#endif

		BYTE nResult = CItemUpgrade::GetInstance()->OnSmeltSafety(pUser, pItemElem0, pItemElem1, pItemElem2, pItemElem3);

		pUser->AddSmeltSafety(nResult);
	}
}

void CDPSrvr::OnEnchant(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwItemId, dwItemMaterialId;

	ar >> dwItemId;
	ar >> dwItemMaterialId;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_vtInfo.GetOther())
			return;
		if (pUser->m_vtInfo.VendorIsVendor())
			return;

		CItemElem* pItemElem0 = pUser->m_Inventory.GetAtId(dwItemId);
		CItemElem* pItemElem1 = pUser->m_Inventory.GetAtId(dwItemMaterialId);

		if (IsUsableItem(pItemElem0) == FALSE || IsUsableItem(pItemElem1) == FALSE)
			return;

		CItemUpgrade::GetInstance()->OnEnchant(pUser, pItemElem0, pItemElem1);
	}
}

#if __VER >= 10 // __REMOVE_ATTRIBUTE
void CDPSrvr::OnRemoveAttribute(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nPayPenya = 100000;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	OBJID objItemId;
	ar >> objItemId;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(objItemId);
	if (IsUsableItem(pItemElem) == FALSE)
	{
		pUser->AddRemoveAttribute(FALSE);
		return;
	}

	ItemProp* pProp = pItemElem->GetProp();
	if (!pProp)
		return;

	if (!pProp->IsEleRefineryAble())
	{
		pUser->AddRemoveAttribute(FALSE);
		pUser->AddDefinedText(TID_GAME_NOTEQUALITEM, "");
		return;
	}

	if (pUser->m_Inventory.IsEquip(objItemId))
	{
		pUser->AddRemoveAttribute(FALSE);
		pUser->AddDefinedText(TID_GAME_EQUIPPUT, "");
		return;
	}

	if (pUser->CheckUserGold(nPayPenya, false) == false)
	{
		pUser->AddRemoveAttribute(FALSE);
		pUser->AddDefinedText(TID_GAME_LACKMONEY, "");
		return;
	}


	if ((pItemElem->m_bItemResist != SAI79::NO_PROP) && (pItemElem->m_nResistAbilityOption > 0))
	{
		pUser->AddGold(-nPayPenya);
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem((BYTE)pItemElem->m_dwObjId, UI_IR, SAI79::NO_PROP);
		pUser->UpdateItem((BYTE)pItemElem->m_dwObjId, UI_RAO, 0);
		pUser->AddRemoveAttribute(TRUE);

#ifndef __FL_FIX_USELESS_LOGS
		LogItemInfo aLogItem;
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "REMOVE_ATTRIBUTE";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold() + nPayPenya;
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Gold_1 = -nPayPenya;
		aLogItem.Action = "O";
		OnLogItem(aLogItem, pItemElem, 1);
#endif
	}
	else
		pUser->AddRemoveAttribute(FALSE);

}
#endif // __REMOVE_ATTRIBUTE


void CDPSrvr::OnRandomScroll(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize)
{
	DWORD dwId1, dwId2;

	ar >> dwId1;
	ar >> dwId2;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CItemElem* pItemElem0 = pUser->m_Inventory.GetAtId(dwId1);
		CItemElem* pItemElem1 = pUser->m_Inventory.GetAtId(dwId2);
		if (IsUsableItem(pItemElem0) == FALSE || IsUsableItem(pItemElem1) == FALSE)
		{
			return;
		}

		if (pUser->m_Inventory.IsEquip(dwId1))
		{
			pUser->AddDefinedText(TID_GAME_EQUIPPUT, "");
			return;
		}

		if (pItemElem1->GetProp()->dwItemKind3 != IK3_RANDOM_SCROLL)
		{
			return;
		}

		if (!(pItemElem0->GetProp()->dwItemKind1 == IK1_WEAPON || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMOR || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMORETC))
		{
			pUser->AddDefinedText(TID_GAME_RANDOMSCROLL_ERROR, "");
			return;
		}

		int nSTR[4] = { 1,  9, 21, 37 };
		int nDEX[4] = { 2, 10, 22, 38 };
		int nINT[4] = { 3, 11, 23, 39 };
		int nSTA[4] = { 4, 12, 24, 40 };

		int nValue = 0;
		int nRandom = xRandom(100);
		if (nRandom < 64)
			nValue = 1;
		else if (nRandom < 94)
			nValue = 2;
		else if (nRandom < 99)
			nValue = 3;
		else if (nRandom < 100)
			nValue = 4;

		if (0 < nValue)
		{
			int nKind = 0;
			int nToolKind = 0;
			if (pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMSTR)
			{
				nKind = nSTR[nValue - 1];
				nToolKind = DST_STR;
			}
			else if (pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMDEX)
			{
				nKind = nDEX[nValue - 1];
				nToolKind = DST_DEX;
			}
			else if (pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMINT)
			{
				nKind = nINT[nValue - 1];
				nToolKind = DST_INT;
			}
			else if (pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMSTA)
			{
				nKind = nSTA[nValue - 1];
				nToolKind = DST_STA;
			}

			pUser->UpdateItem((BYTE)pItemElem0->m_dwObjId, UI_RANDOMOPTITEMID, nKind);

#ifndef __FL_FIX_USELESS_LOGS
			LogItemInfo aLogItem;
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "RANDOMSCROLL";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.Action = "(";
			OnLogItem(aLogItem, pItemElem0, pItemElem0->m_nItemNum);
			aLogItem.Action = ")";
			OnLogItem(aLogItem, pItemElem1, pItemElem1->m_nItemNum);
#endif

			pUser->RemoveItem((BYTE)(dwId2), (short)1);


			pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

			DWORD dwStringNum = 0;
			switch (nToolKind)
			{
			case DST_STR:
				dwStringNum = TID_TOOLTIP_STR;
				break;
			case DST_DEX:
				dwStringNum = TID_TOOLTIP_DEX;
				break;
			case DST_STA:
				dwStringNum = TID_TOOLTIP_STA;
				break;
			default: //case DST_INT:
				dwStringNum = TID_TOOLTIP_INT;
				break;
			}
			CString strMessage;
			strMessage.Format(prj.GetText(TID_GAME_RANDOMSCROLL_SUCCESS), pItemElem0->GetProp()->szName, prj.GetText(dwStringNum), nValue);
			pUser->AddText(strMessage);
		}
	}
}

void CDPSrvr::OnEnterChattingRoom(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long uidChattingRoom;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		BOOL bChatting = FALSE;
		if (pUser->m_idChatting == 0)
		{
			ar >> uidChattingRoom;

			CChatting* pChatting = g_ChattingMng.GetChttingRoom(uidChattingRoom);
			if (pChatting)
			{
				if (pChatting->AddChattingMember(pUser->m_idPlayer))
				{
					CUser* pUsertmp;
					pUser->m_idChatting = uidChattingRoom;

					for (int i = 0; i < pChatting->GetChattingMember() - 1; ++i)
					{
						pUsertmp = (CUser*)prj.GetUserByID(pChatting->m_idMember[i]);
						if (IsValidObj(pUsertmp))
							pUsertmp->AddEnterChatting(pUser);
					}

					pUser->AddNewChatting(pChatting);
				}
				else
				{
					bChatting = TRUE;
				}
			}

		}
		else
		{
			bChatting = TRUE;
		}
	}
}

void CDPSrvr::OnChatting(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	static	TCHAR	sChat[1024];
	ar.ReadString(sChat, 1024);

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
#ifdef __JEFF_9_20
#if __VER >= 12 // __LORD
		int nText = pUser->GetMuteText();
		if (nText)
		{
			pUser->AddDefinedText(nText);
			return;
		}
#else	// __LORD
		if (pUser->IsMute())
			return;
#endif	// __LORD
#endif	// __JEFF_9_20

		if (!(pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
			ParsingEffect(sChat, strlen(sChat));

		RemoveCRLF(sChat);

		CChatting* pChatting = g_ChattingMng.GetChttingRoom(pUser->m_idChatting);
		if (pChatting)
		{
			int nFind = pChatting->FindChattingMember(pUser->m_idPlayer);
			if (nFind != -1)
			{
				CUser* pUsertmp;
				for (int i = 0; i < pChatting->GetChattingMember(); ++i)
				{
					pUsertmp = (CUser*)prj.GetUserByID(pChatting->m_idMember[i]);
					if (IsValidObj(pUsertmp))
						pUsertmp->AddChatting(pUser->m_idPlayer, sChat);
				}
			}
			else
			{

				pUser->m_idChatting = 0;
				pUser->AddDeleteChatting();
			}
		}
		else
		{

			pUser->m_idChatting = 0;
			pUser->AddDeleteChatting();
		}

	}
}


void CDPSrvr::OnOpenChattingRoom(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{

		CChatting* pChatting = g_ChattingMng.GetChttingRoom(pUser->m_idChatting);
		if (pChatting)
		{
			pChatting->m_bState = TRUE;
			CUser* pUsertmp;
			for (int i = 0; i < pChatting->GetChattingMember(); ++i)
			{
				pUsertmp = (CUser*)prj.GetUserByID(pChatting->m_idMember[i]);
				if (IsValidObj(pUsertmp) && pUser->m_idPlayer != pUsertmp->m_idPlayer)
					pUsertmp->AddChttingRoomState(pChatting->m_bState);
			}
		}
	}
}

void CDPSrvr::OnCloseChattingRoom(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{

		CChatting* pChatting = g_ChattingMng.GetChttingRoom(pUser->m_idChatting);
		if (pChatting)
		{
			pChatting->m_bState = FALSE;
			CUser* pUsertmp;

			for (int i = 0; i < pChatting->GetChattingMember(); ++i)
			{
				pUsertmp = (CUser*)prj.GetUserByID(pChatting->m_idMember[i]);
				if (IsValidObj(pUsertmp) && pUser->m_idPlayer != pUsertmp->m_idPlayer)
				{
					pUsertmp->AddChttingRoomState(pChatting->m_bState);
				}
			}
		}

	}
}


void CDPSrvr::QueryDestroyPlayer(DPID dpidCache, DPID dpidSocket, DWORD dwSerial, u_long idPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERY_DESTROY_PLAYER, dpidSocket);
	ar << dwSerial;
	ar << idPlayer;
	SEND(ar, this, dpidCache);
}

void CDPSrvr::OnSetNaviPoint(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	NaviPoint nv;
	OBJID objidTarget;
	ar >> nv.Pos >> objidTarget;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (objidTarget == NULL_ID)
		{
			CParty* pParty = g_PartyMng.GetParty(pUser->m_idparty);
			if (pParty)
			{
				for (int i = 0; i < pParty->GetSizeofMember(); ++i)
				{
					CUser* pUsertmp = (CUser*)prj.GetUserByID(pParty->GetPlayerId(i));
					if (IsValidObj(pUsertmp))
					{
						pUsertmp->AddSetNaviPoint(nv, pUser->GetId(), pUser->GetName(TRUE));
					}
				}
			}
		}
		else
		{
			CUser* pUsertmp = prj.GetUser(objidTarget);
			if (IsValidObj(pUsertmp))
			{
				pUser->AddSetNaviPoint(nv, pUser->GetId(), pUser->GetName(TRUE));
				pUsertmp->AddSetNaviPoint(nv, pUser->GetId(), pUser->GetName(TRUE));
			}
		}
	}
}


void CDPSrvr::OnReqLeave(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (pUser->m_dwLeavePenatyTime == 0)
		pUser->m_dwLeavePenatyTime = ::timeGetTime() + TIMEWAIT_CLOSE * 1000;
}

void CDPSrvr::OnStateMode(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwStateMode;
	BYTE nFlag;
	ar >> dwStateMode;
	ar >> nFlag;


	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->IsStateMode(dwStateMode))
		{
			if (nFlag == STATEMODE_BASEMOTION_CANCEL)
			{
				pUser->SetStateNotMode(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL);
				pUser->m_nReadyTime = 0;
				pUser->m_dwUseItemId = 0;
			}
		}
	}
}

void CDPSrvr::OnQuerySetPlayerName(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwData;
	char lpszPlayer[MAX_PLAYER] = { 0, };

	ar >> dwData;
	ar.ReadString(lpszPlayer, MAX_PLAYER);

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
		{
			pUser->AddDiagText(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE));
			return;
		}

		if (IsInvalidGameName(lpszPlayer))
		{
			pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
			return;
		}

		prj.Formalize(lpszPlayer);

#ifdef __S_SERVER_UNIFY
		if (pUser->m_bAllAction)
#endif
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwData);
			if (IsUsableItem(pItemElem) && pItemElem->m_dwItemId == II_SYS_SYS_SCR_CHANAM && pItemElem->m_bQuery == FALSE)
			{
				pItemElem->m_bQuery = TRUE;
				g_dpDBClient.SendQuerySetPlayerName(pUser->m_idPlayer, lpszPlayer, dwData);
				pUser->RemoveItem((BYTE)dwData, 1);
			}
		}
#ifdef __S_SERVER_UNIFY
		else
			g_dpDBClient.SendQuerySetPlayerName(pUser->m_idPlayer, lpszPlayer, dwData);
#endif // __S_SERVER_UNIFY
	}
}

void CDPSrvr::OnQuerySetGuildName(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nId;
	char lpszGuild[MAX_G_NAME] = { 0, };

	ar >> nId;
	ar.ReadString(lpszGuild, MAX_G_NAME);

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (!IsInviteAbleGuild(pUser->m_idGuild, pUser))
			return;

		//__FIX_SQL_INJECTS
		if (IsInvalidGameName(lpszGuild))
		{
			pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
			return;
		}

#ifdef __S_SERVER_UNIFY
		if (pUser->m_bAllAction)
#endif
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId(nId);
			if (IsUsableItem(pItemElem) && pItemElem->m_bQuery == FALSE && pItemElem->m_dwItemId == II_SYS_SYS_SCR_GCHANAM)
			{
				pItemElem->m_bQuery = TRUE;
				g_DPCoreClient.SendQuerySetGuildName(pUser->m_idPlayer, pUser->m_idGuild, lpszGuild, nId);
				pUser->RemoveItem(nId, 1);
			}
		}
#ifdef __S_SERVER_UNIFY
		else
			g_DPCoreClient.SendQuerySetGuildName(pUser->m_idPlayer, pUser->m_idGuild, lpszGuild, nId);
#endif
	}
}

#ifndef __FL_FIX_USELESS_LOGS
void CDPSrvr::PutCreateItemLog(CUser* pUser, CItemElem* pItemElem, const char* szAction, const char* recv)
{
	LogItemInfo logitem;
	logitem.Action = szAction;
	logitem.SendName = pUser->GetName();
	logitem.RecvName = recv;
	logitem.WorldId = pUser->GetWorld()->GetID();
	logitem.Gold = pUser->GetGold();
	logitem.Gold2 = 0;
	logitem.ItemNo = pItemElem->GetSerialNumber();
	//logitem.ItemName = pItemElem->GetProp()->szName;
	_stprintf(logitem.szItemName, "%d", pItemElem->GetProp()->dwID);
	logitem.itemNumber = pItemElem->m_nItemNum;
	logitem.nAbilityOption = 0;
	logitem.Gold_1 = 0;
	logitem.nItemResist = pItemElem->m_bItemResist;
	logitem.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
	logitem.Negudo = 100;
	OnLogItem(logitem);
}
#endif

void CDPSrvr::OnDoEscape(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		ItemProp* pItemProp = prj.GetItemProp(II_CHR_SYS_SCR_ESCAPEBLINKWING);
		if (pItemProp)
		{
			if (!pUser->IsSMMode(SM_ESCAPE))
				pUser->DoUseItemVirtual(II_CHR_SYS_SCR_ESCAPEBLINKWING, FALSE);
			else
				pUser->AddDefinedText(TID_GAME_STILLNOTUSE);
		}
	}
}

void CDPSrvr::OnCheering(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objid;
	ar >> objid;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->GetId() == objid)
			return;

		CMover* pTarget = prj.GetMover(objid);
		if (IsValidObj(pTarget) && pTarget->GetType() == OT_MOVER && pTarget->IsPlayer())
		{
			if (pTarget->HasBuff(BUFF_ITEM, II_CHEERUP))
			{
				pUser->AddText("This player already have cheer-effect. Please wait for the effect to finish.");
				return;
			}

			DWORD dwTickCount = GetTickCount();
			if (pUser->m_nCheerPoint <= 0)
			{
				pUser->AddDefinedText(TID_CHEER_NO1, "%d", (pUser->m_dwTickCheer - dwTickCount) / 60000);
				return;
			}

			if (pUser->m_nCheerPoint == MAX_CHEERPOINT)
				pUser->SetCheerParam(pUser->m_nCheerPoint - 1, dwTickCount, TICK_CHEERPOINT);
			else
				pUser->SetCheerParam(pUser->m_nCheerPoint - 1, dwTickCount, pUser->m_dwTickCheer - dwTickCount);

			FLOAT fAngle = GetDegree(pTarget->GetPos(), pUser->GetPos());
			pUser->SetAngle(fAngle);

			if (pTarget->GetSex() == pUser->GetSex())
			{
				((CUser*)pTarget)->AddDefinedText(TID_CHEER_MESSAGE3, "%s", pUser->GetName());
				pUser->SendActMsg(OBJMSG_MOTION, MTI_CHEERSAME, ANILOOP_1PLAY);
			}
			else
			{
				((CUser*)pTarget)->AddDefinedText(TID_CHEER_MESSAGE4, "%s", pUser->GetName());
				pUser->SendActMsg(OBJMSG_MOTION, MTI_CHEEROTHER, ANILOOP_1PLAY);
			}

			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_CHEERSENDEFFECT);
			g_UserMng.AddCreateSfxObj(pTarget, XI_CHEERRECEIVEEFFECT);

			CMover* pSrc = (CMover*)pUser;
			g_UserMng.AddMoverBehavior(pSrc, TRUE);

#ifdef __FL_RECORD_BOOK
			CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::CHEERS_GIVEN, 1, true);
			CRecordBook::GetInstance()->AddPlayerRecord(pTarget, RecordType::CHEERS_RECEIVED, 1, true);
#endif

			ItemProp* pItemProp = prj.GetItemProp(II_CHEERUP);
			if (pItemProp)
				pTarget->DoApplySkill(pTarget, pItemProp, NULL);
		}
		else
		{
			pUser->AddDefinedText(TID_CHEER_NO2, "");
		}
	}
}

void CDPSrvr::OnQueryEquip(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objid;
	ar >> objid;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CUser* pUsertmp = prj.GetUser(objid);
		if (IsValidObj(pUsertmp))
		{

			if (pUsertmp->IsMode(EQUIP_DENIAL_MODE) && pUser->IsAuthHigher(AUTH_GAMEMASTER) == FALSE)
			{
				pUser->AddDefinedText(TID_DIAG_0088);
				return;
			}
			pUser->AddQueryEquip(pUsertmp);
		}
	}
}

void CDPSrvr::OnQueryEquipSetting(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BOOL bAllow;
	ar >> bAllow;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (bAllow)
			pUser->SetNotMode(EQUIP_DENIAL_MODE);
		else
			pUser->SetMode(EQUIP_DENIAL_MODE);
		g_UserMng.AddModifyMode(pUser);
	}
}

void CDPSrvr::OnReturnScroll(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nSelect;
	ar >> nSelect;
	if (nSelect < -1 || nSelect > 2)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	if (nSelect >= 0)
	{
		static char* szPos[] = { "flaris",	"saintmorning",	"darkon" };
		pUser->m_lpszVillage = szPos[nSelect];
		pUser->AddReturnScroll();
	}
	else
	{

		if (pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_RETURN))
			pUser->DoUseItemVirtual(II_SYS_SYS_SCR_RETURN, TRUE);
	}
}

void CDPSrvr::OnEndSkillQueue(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		pUser->m_playTaskBar.OnEndSkillQueue(pUser);
}

void CDPSrvr::OnQueryPostMail(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	char lpszReceiver[MAX_PLAYER] = { 0, };
	char lpszTitle[MAX_MAILTITLE] = { 0, };
	char lpszText[MAX_MAILTEXT] = { 0, };
	BYTE nItem;
	short nItemNum;
	int nGold;

	ar >> nItem >> nItemNum;
	ar.ReadString(lpszReceiver, MAX_PLAYER);

	ar >> nGold;
	ar.ReadString(lpszTitle, MAX_MAILTITLE);
	ar.ReadString(lpszText, MAX_MAILTEXT);

	const int nCommissionGold = 100;
	const int nTotGold = nCommissionGold + nGold;
	if (nGold < 0 || nTotGold <= 0)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_vtInfo.GetOther())
			return;
		if (pUser->m_vtInfo.VendorIsVendor())
			return;
		if (pUser->m_bBank)
			return;
#ifdef __S_SERVER_UNIFY
		if (pUser->m_bAllAction == FALSE)
			return;
#endif // __S_SERVER_UNIFY
#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif

		u_long idReceiver = CPlayerDataCenter::GetInstance()->GetPlayerId(lpszReceiver);
		if (idReceiver > 0)
		{
			if (CPlayerDataCenter::GetInstance()->GetPlayerId((char*)pUser->GetName()) == idReceiver)
			{
				pUser->AddDiagText(prj.GetText(TID_GAME_MSGSELFSENDERROR));
				return;
			}

			CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(idReceiver);
			if (pMailBox && pMailBox->size() >= MAX_MAIL)
			{
				pUser->AddDefinedText(TID_GAME_MAILBOX_FULL, "%s", lpszReceiver);
				return;
			}

			CItemElem* pItemElem = pUser->m_Inventory.GetAtId(nItem);
			if (pItemElem)
			{
				if (IsUsableItem(pItemElem) == FALSE)
				{
					pUser->AddDiagText(prj.GetText(TID_GAME_CANNOT_POST));
					return;
				}
				if (pUser->m_Inventory.IsEquip(pItemElem->m_dwObjId))
				{
					pUser->AddDiagText(prj.GetText(TID_GAME_CANNOT_POST));
					return;
				}
				if (pItemElem->IsQuest())
				{
					pUser->AddDiagText(prj.GetText(TID_GAME_CANNOT_POST));
					return;
				}
				if (pItemElem->IsBinds())
				{
					pUser->AddDiagText(prj.GetText(TID_GAME_CANNOT_POST));
					return;
				}
				if (pUser->IsUsing(pItemElem))
				{
					pUser->AddDiagText(prj.GetText(TID_GAME_CANNOT_DO_USINGITEM));
					return;
				}
				ItemProp* pProp = pItemElem->GetProp();

				if (nItemNum > pItemElem->m_nItemNum)
					nItemNum = pItemElem->m_nItemNum;
				if (nItemNum < 1)
					nItemNum = 1;
			}

			if (pUser->CheckUserGold(nTotGold, false) == false)
			{
				pUser->AddDiagText(prj.GetText(TID_GAME_LACKMONEY));
				return;
			}

			pUser->AddGold(-nTotGold, TRUE);

			CItemElem	itemElem;
			if (pItemElem)
			{
				itemElem = *pItemElem;
				itemElem.m_nItemNum = nItemNum;
				pUser->RemoveItem((BYTE)pItemElem->m_dwObjId, nItemNum);
			}

			CWorld* pWorld = pUser->GetWorld();
			if (pWorld)
				g_dpDBClient.SavePlayer(pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer());

			g_dpDBClient.SendQueryPostMail(idReceiver, pUser->m_idPlayer, itemElem, nGold, lpszTitle, lpszText);
		}
		else
			pUser->AddDiagText(prj.GetText(TID_MAIL_UNKNOW));
	}

}

void CDPSrvr::OnQueryRemoveMail(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif
		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(pUser->m_idPlayer);
		if (pMailBox != NULL)
		{
			CMail* pMail = pMailBox->GetMail(nMail);
			if (pMail != NULL)
			{
				g_dpDBClient.SendQueryRemoveMail(pUser->m_idPlayer, nMail);
			}
			else
			{
				Error("CDPSrvr::OnQueryRemoveMail - Invalid nMail. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail);
			}
		}
		else
		{
			Error("CDPSrvr::OnQueryRemoveMail - Invalid pMailBox. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail);
		}
	}
}

void CDPSrvr::OnQueryGetMailItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_Inventory.GetEmptyCount() < 1)
		{
			pUser->AddDiagText(prj.GetText(TID_GAME_LACKSPACE));
			return;
		}

#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif

		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(pUser->m_idPlayer);
		if (pMailBox)
		{
			CMail* pMail = pMailBox->GetMail(nMail);
			if (pMail && pMail->m_pItemElem)
			{
				int nDay = 0;
				DWORD dwTime = 0;
				pMail->GetMailInfo(&nDay, &dwTime);

				if ((MAX_KEEP_MAX_DAY * 24) - dwTime > (MAX_KEEP_BASIC_DAY * 24))
				{
					FLOAT fCustody = 0.0f;
					FLOAT fPay = 0.0f;
					fCustody = (FLOAT)((FLOAT)(MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY - nDay) / (FLOAT)(MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY));
					fPay = pMail->m_pItemElem->GetCost() * fCustody;
					if (fPay < 0.0f)
						fPay = 0.0f;

					const int pay = (int)fPay;
					if (pay < 0 || pUser->CheckUserGold(pay, false) == false)
					{
						pUser->AddDiagText(prj.GetText(TID_GAME_LACKMONEY));
						return;
					}
					pUser->AddGold(-pay);
				}
			}
		}
		g_dpDBClient.SendQueryGetMailItem(pUser->m_idPlayer, nMail);
	}
}

void CDPSrvr::OnQueryGetMailGold(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(pUser->m_idPlayer);
		if (pMailBox != NULL)
		{
			CMail* pMail = pMailBox->GetMail(nMail);
			if (pMail != NULL)
			{
				if (pMail->m_nGold <= INT_MAX && pMail->m_nGold > 0 && pUser->CheckUserGold(pMail->m_nGold, true) == true)
					g_dpDBClient.SendQueryGetMailGold(pUser->m_idPlayer, nMail);
				else
					return;
			}
		}
	}
}

void CDPSrvr::OnQueryReadMail(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(pUser->m_idPlayer);
		if (pMailBox != NULL)
		{
			CMail* pMail = pMailBox->GetMail(nMail);
			if (pMail != NULL)
			{
				g_dpDBClient.SendQueryReadMail(pUser->m_idPlayer, nMail);
			}
		}
	}
}

void CDPSrvr::OnQueryMailBox(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	int	nClientReqCount = 1;

	if (IsValidObj(pUser))
	{
		if (pUser->CheckClientReq() == false)
		{
			nClientReqCount = 1;
		}
		else
		{
			nClientReqCount = pUser->GetCountClientReq();
		}

		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(pUser->m_idPlayer);
		if (pMailBox)
		{
			switch (pMailBox->m_nStatus)
			{
			case CMailBox::data:
			{
				if (nClientReqCount <= 1)
				{
					pUser->AddMailBox(pMailBox);
				}
				else
				{
					g_dpDBClient.SendQueryMailBoxCount(pUser->m_idPlayer, pUser->GetCountClientReq());
				}
			}
			break;
			case CMailBox::nodata:
			{
				if (nClientReqCount >= 2)
				{
					g_dpDBClient.SendQueryMailBoxCount(pUser->m_idPlayer, pUser->GetCountClientReq());
				}
				else
				{
					g_dpDBClient.SendQueryMailBox(pUser->m_idPlayer);
				}

				pMailBox->m_nStatus = CMailBox::read;
			}
			break;
			case CMailBox::read:
			{
				if (nClientReqCount >= 2)
				{
					g_dpDBClient.SendQueryMailBoxCount(pUser->m_idPlayer, pUser->GetCountClientReq());
				}
			}
			break;
			default:
			{
			}
			break;
			}
		}
		else
		{
			if (pUser->GetCheckTransMailBox() == FALSE)
			{
				g_dpDBClient.SendQueryMailBoxReq(pUser->m_idPlayer);
			}
			else
			{
				pUser->SendCheckMailBoxReq(FALSE);
			}
		}
	}
}

void CDPSrvr::OnGCApp(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (g_eLocal.GetState(EVE_GUILDCOMBAT) == 0)
		return;

	BYTE nState;
	ar >> nState;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CGuild* pGuild = pUser->GetGuild();
		if (pGuild == NULL || pGuild->IsMaster(pUser->m_idPlayer) == FALSE)
		{
			pUser->AddDiagText(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER));
			return;
		}

		if (nState == GC_IN_WINDOW)
		{
			CGuild* pGuild = g_GuildMng.GetGuild(pUser->m_idGuild);
			if (pGuild && pGuild->IsMaster(pUser->m_idPlayer))
			{
				if (strlen(pGuild->m_szGuild) < 3)
				{
					pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
					return;
				}
				pUser->AddGCWindow(g_GuildCombatMng.GetPrizePenya(2), g_GuildCombatMng.GetRequstPenya(pUser->m_idGuild), g_GuildCombatMng.m_nJoinPanya);
			}
		}
		else if (nState == GC_IN_APP)
		{
			DWORD dwPenya;
			ar >> dwPenya;
			g_GuildCombatMng.GuildCombatRequest(pUser, dwPenya);
		}
	}
}
void CDPSrvr::OnGCCancel(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (g_eLocal.GetState(EVE_GUILDCOMBAT) == 0)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		g_GuildCombatMng.GuildCombatCancel(pUser);
}
void CDPSrvr::OnGCRequestStatus(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (g_eLocal.GetState(EVE_GUILDCOMBAT) == 0)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		pUser->AddGCRequestStatus(g_GuildCombatMng.GetPrizePenya(2), g_GuildCombatMng.vecRequestRanking);
}
void CDPSrvr::OnGCSelectPlayer(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (g_eLocal.GetState(EVE_GUILDCOMBAT) == 0)
		return;

	BOOL bWindow;
	ar >> bWindow;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{

		if (g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE)
		{
			pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP));
			return;
		}


		if (g_GuildCombatMng.IsRequestWarGuild(pUser->m_idGuild, FALSE) == FALSE)
		{
			pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP_FAIL));
			return;
		}

		CGuild* pGuild = g_GuildMng.GetGuild(pUser->m_idGuild);

		BOOL bMK = FALSE;
		if (pGuild)
		{
			CGuildMember* pGuildMember = pGuild->GetMember(pUser->m_idPlayer);
			if (pGuildMember)
			{
				if (pGuildMember->m_nMemberLv == GUD_KINGPIN || pGuildMember->m_nMemberLv == GUD_MASTER)
					bMK = TRUE;
			}
		}


		if (pGuild && bMK)
		{
			if (bWindow == FALSE)
			{

				int nSize;
				u_long uidPlayer, uidDefender;
				vector< u_long > vecSelectPlayer;
				BOOL bMasterOrKinpin = FALSE;
				BOOL bDefender = FALSE;
				BOOL bLevel = FALSE;
				BOOL bLogOut = FALSE;
				BOOL bGuildMember = FALSE;
				BOOL bMastertoDefender = FALSE;
				vecSelectPlayer.clear();
				ar >> uidDefender;
				ar >> nSize;
				if (nSize > g_GuildCombatMng.m_nMaxJoinMember)
					return;

				for (int i = 0; i < nSize; ++i)
				{
					ar >> uidPlayer;
					vecSelectPlayer.push_back(uidPlayer);
					CUser* pUsertmp = g_UserMng.GetUserByPlayerID(uidPlayer);
					if (IsValidObj(pUsertmp))
					{
						CGuildMember* pGuildMember = pGuild->GetMember(uidPlayer);

						if (pGuildMember)
						{

							if (pGuild->IsMaster(pUsertmp->m_idPlayer))
								bMasterOrKinpin = TRUE;

							if (pGuildMember->m_nMemberLv == GUD_KINGPIN)
								bMasterOrKinpin = TRUE;

							if (pUsertmp->m_idPlayer == uidDefender)
								bDefender = TRUE;
						}
					}
				}

				if (1 < nSize && pGuild->IsMaster(uidDefender))
					bMastertoDefender = TRUE;

				if (bMasterOrKinpin && bMastertoDefender == FALSE
#ifndef _DEBUG
					&& bDefender
#endif // _DEBUG
					)
				{
					g_GuildCombatMng.SelectPlayerClear(pUser->m_idGuild);
					for (int veci = 0; veci < (int)(vecSelectPlayer.size()); ++veci)
					{

						if (veci >= g_GuildCombatMng.m_nMaxJoinMember)
							break;

						u_long uidSelectPlayer = vecSelectPlayer[veci];
						CUser* pUsertmp = g_UserMng.GetUserByPlayerID(uidSelectPlayer);
						{
							if (IsValidObj(pUsertmp))
							{
								CGuildMember* pGuildMember = pGuild->GetMember(uidPlayer);

								if (pGuildMember)
								{

									if (30 <= pUsertmp->GetLevel())
									{
										g_GuildCombatMng.AddSelectPlayer(pUser->m_idGuild, uidSelectPlayer);
									}
								}
							}
						}
					}
					g_GuildCombatMng.SetDefender(pUser->m_idGuild, uidDefender);
				}
			}

			vector<CGuildCombat::__JOINPLAYER> vecSelectList;
			g_GuildCombatMng.GetSelectPlayer(pUser->m_idGuild, vecSelectList);
			pUser->AddGCSelectPlayerWindow(vecSelectList, g_GuildCombatMng.GetDefender(pUser->m_idGuild), bWindow, g_GuildCombatMng.IsRequestWarGuild(pUser->m_idGuild, FALSE));
			if (bWindow == FALSE)
			{
				g_UserMng.AddGCGuildStatus(pUser->m_idGuild);
				g_UserMng.AddGCWarPlayerlist(pUser->m_idGuild);
			}
		}
	}
}
void CDPSrvr::OnGCJoin(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (g_eLocal.GetState(EVE_GUILDCOMBAT) == 0)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_GUILDWAR_JOIN, pUser->GetWorld(), pUser->GetPos()))
			return;
		g_GuildCombatMng.GuildCombatEnter(pUser);
	}
}
void CDPSrvr::OnGCTele(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (g_eLocal.GetState(EVE_GUILDCOMBAT) == 0)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		PRegionElem pRgnElem = g_WorldMng.GetRevivalPos(WI_WORLD_MADRIGAL, "flaris");
		if (pRgnElem)
			((CMover*)pUser)->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6983.0f, 0.0f, 3330.0f), REPLACE_NORMAL, nDefaultLayer);
	}
}
void CDPSrvr::OnGCPlayerPoint(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (g_eLocal.GetState(EVE_GUILDCOMBAT) == 0)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		pUser->AddGCPlayerPoint();
}

void CDPSrvr::OnSummonFriend(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwData;
	char lpszPlayer[MAX_PLAYER] = { 0, };

	ar >> dwData;
	ar.ReadString(lpszPlayer, MAX_PLAYER);

	if (strlen(lpszPlayer) >= MAX_NAME)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwData);
	if (!IsUsableItem(pItemElem))
		return;

	if (pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_A && pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_B)
		return;

	if (pItemElem->m_bQuery)
		return;

	CUser* pUsertmp = g_UserMng.GetUserByPlayerID(CPlayerDataCenter::GetInstance()->GetPlayerId(lpszPlayer));
	if (!IsValidObj((CObj*)pUsertmp))
	{
		pUser->AddDefinedText(TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", lpszPlayer);
		return;
	}

	if (pUser->m_idPlayer == pUsertmp->m_idPlayer)
		pUser->AddDefinedText(TID_GAME_SUMMON_FRIEND_MY_NOUSE);
	else if (prj.IsGuildQuestRegion(pUser->GetPos()))
		pUser->AddDefinedText(TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText(TID_GAME_EVENT_WORLD_NOTUSE));
	else if (prj.IsGuildQuestRegion(pUsertmp->GetPos()))
		pUser->AddDefinedText(TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText(TID_GAME_EVENT_WORLD_NOTUSE1));
	else if (pUser->GetWorld()->GetID() != pUsertmp->GetWorld()->GetID() || pUser->GetLayer() != pUsertmp->GetLayer())
	{
		CString strtmp;
		strtmp.Format(prj.GetText(TID_GAME_WORLD_NOTUSE), pUser->GetWorld()->m_szWorldName, pUsertmp->GetWorld()->m_szWorldName);
		pUser->AddDefinedText(TID_GAME_STATE_NOTUSE, "\"%s\"", strtmp);
	}
	else
	{
		if (pUsertmp->m_RTMessenger.IsBlock(pUser->m_idPlayer))
		{
			pUser->AddDefinedText(TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", lpszPlayer);
			return;
		}

		pItemElem->m_bQuery = TRUE;
		pUsertmp->AddSummonFriendConfirm(pUser->GetId(), dwData, pUser->GetName(), pUser->GetWorld()->m_szWorldName);
		pUser->AddDefinedText(TID_GAME_SUMMONFRIEND_CONFIRM, "\"%s\"", lpszPlayer);
	}
}
void CDPSrvr::OnSummonFriendConfirm(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objid;
	DWORD dwData;

	ar >> objid >> dwData;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CUser* pUsertmp = prj.GetUser(objid);
	if (!IsValidObj((CObj*)pUsertmp))
		return;

	CItemElem* pItemElem = pUsertmp->m_Inventory.GetAtId(dwData);
	if (!pItemElem)
		return;

	if (pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_A && pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_B)
		return;

	if (pItemElem->m_bQuery == FALSE)
		return;

	pItemElem->m_bQuery = FALSE;

	if (!IsUsableItem(pItemElem))
		return;

	DWORD dwState = pUser->GetSummonState(FALSE);
	if (dwState != 0)
	{
		pUser->AddDefinedText(dwState);
		pUsertmp->AddDefinedText(dwState);
		return;
	}

	dwState = pUsertmp->GetSummonState(TRUE);
	if (dwState != 0)
	{
		pUser->AddDefinedText(dwState);
		pUsertmp->AddDefinedText(dwState);
		return;
	}

	if (pUser->GetWorld()->GetID() != pUsertmp->GetWorld()->GetID() || pUser->GetLayer() != pUsertmp->GetLayer())
	{
		CString strtmp;
		strtmp.Format(prj.GetText(TID_GAME_WORLD_NOTUSE), pUser->GetWorld()->m_szWorldName, pUsertmp->GetWorld()->m_szWorldName);
		pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp);
		strtmp.Format(prj.GetText(TID_GAME_WORLD_NOTUSE), pUsertmp->GetWorld()->m_szWorldName, pUser->GetWorld()->m_szWorldName);
		pUsertmp->AddDefinedText(TID_GAME_STATE_NOTSUMMON, "\"%s\"", strtmp);
	}
	else
	{
		pUser->REPLACE(g_uIdofMulti, pUsertmp->GetWorld()->GetID(), pUsertmp->GetPos(), REPLACE_FORCE, pUsertmp->GetLayer());
		pUsertmp->RemoveItem((BYTE)(dwData), (short)1);
		pUser->AddDefinedText(TID_GAME_SUMMON_SUCCESS1, "\"%s\"", pUsertmp->GetName());
		pUsertmp->AddDefinedText(TID_GAME_SUMMON_SUCCESS, "\"%s\"", pUser->GetName());
	}
}

void CDPSrvr::OnSummonFriendCancel(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objid;
	DWORD dwData;
	ar >> objid >> dwData;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CUser* pUsertmp = prj.GetUser(objid);
	if (!IsValidObj((CObj*)pUsertmp))
		return;

	WORD wId = LOWORD(dwData);
	CItemElem* pItemElem = pUsertmp->m_Inventory.GetAtId(wId);
	if (!pItemElem)
		return;

	if (pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_A && pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_B)
		return;

	if (pItemElem->m_bQuery == FALSE)
		return;

	pItemElem->m_bQuery = FALSE;

	pUsertmp->AddDefinedText(TID_GAME_SUMMON_FRIEND_CANCEL, "\"%s\"", pUser->GetName());
}

void CDPSrvr::OnSummonParty(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwData;

	ar >> dwData;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	DWORD dwState = pUser->GetSummonState(TRUE);
	if (dwState != 0)
	{
		pUser->AddDefinedText(dwState);
		return;
	}

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwData);
	if (!IsUsableItem(pItemElem))
		return;

	if (pItemElem->GetProp()->dwID != II_SYS_SYS_SCR_PARTYSUMMON)
		return;

	CParty* pParty = g_PartyMng.GetParty(pUser->GetPartyId());
	if (!pParty)
		return;

	if (pParty->IsLeader(pUser->m_idPlayer))
	{
		if (!pUser->HasBuff(BUFF_ITEM, (WORD)(pItemElem->GetProp()->dwID)))
		{
			pParty->m_dwWorldId = pUser->GetWorld()->GetID();

			ItemProp* pItemProptmp = prj.GetItemProp(II_SYS_SYS_SCR_PARTYSUMMON);
			for (int i = 1; i < pParty->m_nSizeofMember; i++)
			{
				CUser* pUsertmp = g_UserMng.GetUserByPlayerID(pParty->GetPlayerId(i));
				if (IsValidObj((CObj*)pUsertmp))
				{
					pUser->DoApplySkill((CCtrl*)pUsertmp, pItemElem->GetProp(), NULL);
					pUsertmp->AddSummonPartyConfirm(pUser->GetId(), dwData, pUser->GetWorld()->m_szWorldName);
					if (pItemProptmp)
						g_UserMng.AddCreateSfxObj((CMover*)pUsertmp, pItemProptmp->dwSfxObj3);
					pUser->AddDefinedText(TID_GAME_SUMMONFRIEND_CONFIRM, "\"%s\"", pUsertmp->GetName());
				}
				else
					pUser->AddDefinedText(TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString(pParty->GetPlayerId(i)));
			}

			pUser->DoApplySkill((CCtrl*)pUser, pItemElem->GetProp(), NULL);
			pUser->RemoveItem((BYTE)(dwData), (short)1);

			if (pItemProptmp)
				g_UserMng.AddCreateSfxObj((CMover*)pUser, pItemProptmp->dwSfxObj3);
		}
		else
			pUser->AddDefinedText(TID_GAME_LIMITED_USE);
	}
}

void CDPSrvr::OnSummonPartyConfirm(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objid;
	DWORD dwData;
	ar >> objid;
	ar >> dwData;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CParty* pParty = g_PartyMng.GetParty(pUser->GetPartyId());
	if (!pParty)
	{
		pUser->AddDefinedText(TID_GAME_NOPARTY);
		return;
	}

	CMover* pLeader = prj.GetUser(objid);
	if (!IsValidObj(pLeader))
	{
		pUser->AddDefinedText(TID_ERROR_SUMMONPARTY_NOTTIME);
		return;
	}

	if (pParty->IsLeader(pLeader->m_idPlayer) && pLeader->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PARTYSUMMON))
	{
		if (pUser == pLeader)
			return;

		DWORD dwState = pUser->GetSummonState(FALSE);
		if (dwState != 0)
		{
			pUser->AddDefinedText(dwState);
			return;
		}

		dwState = pLeader->GetSummonState(TRUE);
		if (dwState != 0)
		{
			pUser->AddDefinedText(dwState);
			return;
		}

		if (pUser->GetWorld()->GetID() != pParty->m_dwWorldId)
		{
			CWorld* pWorld = g_WorldMng.GetWorld(pParty->m_dwWorldId);
			if (pWorld)
			{
				CString strtmp;
				strtmp.Format(prj.GetText(TID_GAME_WORLD_NOTUSE), pUser->GetWorld()->m_szWorldName, pWorld->m_szWorldName);
				pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp);
			}
		}
		else if (pParty->m_dwWorldId != pLeader->GetWorld()->GetID())
		{
			CWorld* pWorld = g_WorldMng.GetWorld(pParty->m_dwWorldId);
			if (pWorld)
			{
				CString strtmp;
				strtmp.Format(prj.GetText(TID_GAME_WORLDLEADER_NOTUSE));
				pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp);
			}
		}
		else if (pLeader->GetLayer() != pUser->GetLayer())
		{
			CString strtmp;
			strtmp.Format(prj.GetText(TID_GAME_WORLD_NOTUSE));
			pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp);
		}
		else
		{
			pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_PARTYSUMMON);
			pUser->REPLACE(g_uIdofMulti, pLeader->GetWorld()->GetID(), pLeader->GetPos(), REPLACE_FORCE, pLeader->GetLayer());
			pUser->AddDefinedText(TID_GAME_SUMMON_SUCCESS1, "\"%s\"", pLeader->GetName());
			((CUser*)pLeader)->AddDefinedText(TID_GAME_SUMMON_SUCCESS, "\"%s\"", pUser->GetName());
			ItemProp* pItemProptmp = prj.GetItemProp(II_SYS_SYS_SCR_PARTYSUMMON);
			if (pItemProptmp)
				g_UserMng.AddCreateSfxObj((CMover*)pUser, pItemProptmp->dwSfxObj3);
		}
	}
	else
		pUser->AddDefinedText(TID_ERROR_SUMMONPARTY_NOTTIME);
}

void CDPSrvr::OnRemoveInvenItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD	dwId;
	int		nNum;
	ar >> dwId;
	ar >> nNum;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode && pUser->m_dwAuthorization == AUTH_GENERAL)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwId);
	if (IsUsableItem(pItemElem) == FALSE)
		return;

	if (pUser->m_Inventory.IsEquip(dwId))
		return;

	if (pUser->IsUsing(pItemElem))
	{
		pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
		return;
	}

	if (nNum > pItemElem->m_nItemNum)
		nNum = pItemElem->m_nItemNum;
	if (nNum < 1)
		nNum = 1;

	if (pItemElem->IsUndestructable())
		return;

	CString strNum;
	strNum.Format("%d", nNum);
	pUser->AddDefinedText(TID_GAME_SUCCESS_REMOVE_ITEM, "\"%s\" \"%s\"", pItemElem->GetProp()->szName, strNum);

#ifndef __FL_FIX_USELESS_LOGS
	LogItemInfo aLogItem;
	aLogItem.Action = "*";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "GARBAGE";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	OnLogItem(aLogItem, pItemElem, nNum);
#endif

	pUser->RemoveItem((BYTE)(dwId), (short)nNum);
}

void CDPSrvr::OnCreateMonster(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwItemIdRec;
	D3DXVECTOR3 vPos;
	ar >> dwItemIdRec;
	ar >> vPos;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!pUser->IsPossibleUseItem())
		return;

	CCreateMonster::GetInstance()->CreateMonster(pUser, dwItemIdRec, vPos);
}

void CDPSrvr::OnFoucusObj(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objid;
	ar >> objid;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		pUser->AddFocusObj(objid);
}

void CDPSrvr::OnTrade(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objidTrader;
	ar >> objidTrader;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return;

	if (pUser->m_vtInfo.GetOther() == NULL)
	{
		CMover* pTrader = prj.GetMover(objidTrader);
		if (IsValidObj(pTrader) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL)
		{
			if (pTrader->IsPlayer())
			{
				if (pUser->IsDie() || pTrader->IsDie())
					return;

#ifdef __FL_EQUALIZED_MODE
				if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
				{
					pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
					return;
				}
#endif
				if (pUser->m_vtInfo.VendorIsVendor() || pUser->m_vtInfo.IsVendorOpen() ||
					pTrader->m_vtInfo.VendorIsVendor() || pTrader->m_vtInfo.IsVendorOpen())
				{
					return;
				}
				if (pUser->m_bAllAction == FALSE || ((CUser*)pTrader)->m_bAllAction == FALSE)
					return;

				if (pUser->m_bBank || pTrader->m_bBank)
				{
					pUser->AddDefinedText(TID_GAME_TRADELIMITPC, "");
					return;
				}

				if (pUser->m_bGuildBank || pTrader->m_bGuildBank)
				{
					pUser->AddDefinedText(TID_GAME_TRADELIMITPC, "");
					return;
				}

				pUser->m_vtInfo.SetOther(pTrader);
				pTrader->m_vtInfo.SetOther(pUser);

				pUser->AddTrade((CUser*)pTrader, pUser->m_idPlayer);
				((CUser*)pTrader)->AddTrade(pUser, pUser->m_idPlayer);
			}
		}
	}
}

void CDPSrvr::OnConfirmTrade(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objidTrader;
	ar >> objidTrader;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return;

	if (pUser->m_vtInfo.GetOther() == NULL)
	{
		CMover* pTrader = prj.GetMover(objidTrader);
		if (IsValidObj(pTrader) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL)
		{
			if (pUser->IsDie() || pTrader->IsDie())
				return;

			if (pTrader->IsPlayer() == FALSE)
				return;

#ifdef __FL_EQUALIZED_MODE
			if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
			{
				pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
				return;
			}
#endif

			if (pTrader->IsAttackMode())
				pUser->AddDefinedText(TID_GAME_BATTLE_NOTTRADE, "");
			else
				((CUser*)pTrader)->AddComfirmTrade(pUser->GetId());
		}

		pUser->RemoveInvisibleSkill();
	}
}

void CDPSrvr::OnConfirmTradeCancel(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objidTrader;
	ar >> objidTrader;

	CMover* pTrader = prj.GetMover(objidTrader);
	if (IsValidObj(pTrader) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL)
		((CUser*)pTrader)->AddComfirmTradeCancel(objidTrader);
}

void CDPSrvr::OnTradePut(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE byNth;
	DWORD dwItemObjID;
	int nItemNum;

	ar >> byNth >> dwItemObjID >> nItemNum;

	if (byNth >= MAX_TRADE)
		return;

	if (nItemNum < 1)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CMover* pTrader = pUser->m_vtInfo.GetOther();
	if (IsValidObj(pTrader) == FALSE)
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return;

	if (pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser)
	{
		if (pUser->IsDie() || pTrader->IsDie())
			return;

#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif

		CItemElem* pItem = pUser->m_Inventory.GetAtId(dwItemObjID);
		if (pItem && pItem->IsFlag(CItemElem::expired))
			return;

		if (pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM)
		{
			int nItemNumResult = nItemNum;
			DWORD dwText = pUser->m_vtInfo.TradeSetItem2(dwItemObjID, byNth, nItemNumResult);
			if (dwText == 0)
			{
				pUser->AddTradePut(pUser->GetId(), byNth, pItem, nItemNumResult);
				((CUser*)pTrader)->AddTradePut(pUser->GetId(), byNth, pItem, nItemNumResult);
			}
			else
				pUser->AddDefinedText(dwText);
		}
		else
			pUser->AddTradePutError();
	}
}

void CDPSrvr::OnTradePull(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE byNth;
	ar >> byNth;

	if (byNth >= MAX_TRADE)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CMover* pTrader = pUser->m_vtInfo.GetOther();
	if (IsValidObj(pTrader) == FALSE)
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return;

	if (pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser)
	{
		if (pUser->IsDie() || pTrader->IsDie())
			return;

		if (pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM)
		{
			if (pUser->m_vtInfo.TradeClearItem(byNth))
			{
				pUser->AddTradePull(pUser->GetId(), byNth);
				((CUser*)pUser->m_vtInfo.GetOther())->AddTradePull(pUser->GetId(), byNth);
			}
		}
	}
}

void CDPSrvr::OnTradePutGold(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwGold;
	ar >> dwGold;

	int nGold = (int)(dwGold);
	if (nGold <= 0)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CMover* pTrader = pUser->m_vtInfo.GetOther();
	if (IsValidObj(pTrader) == FALSE)
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return;

	if (pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser)
	{
		if (pUser->IsDie() || pTrader->IsDie())
			return;

		if (pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM)
		{
			if (nGold > pUser->GetGold())
				nGold = pUser->GetGold();

			if (pUser->CheckUserGold(nGold, false) == false)
				return;

			pUser->m_vtInfo.TradeSetGold(nGold);
			pUser->AddGold(-nGold, FALSE);

			pUser->AddTradePutGold(pUser->GetId(), nGold);
			((CUser*)pTrader)->AddTradePutGold(pUser->GetId(), nGold);
		}
	}
}

void CDPSrvr::OnTradeCancel(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nMode;
	ar >> nMode;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CMover* pTrader = pUser->m_vtInfo.GetOther();
	if (IsValidObj(pTrader) == FALSE)
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return;

	if (pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser)
	{
		pUser->m_vtInfo.TradeClear();
		pTrader->m_vtInfo.TradeClear();

		pUser->AddTradeCancel(pUser->GetId(), pUser->m_idPlayer, nMode);
		((CUser*)pTrader)->AddTradeCancel(pUser->GetId(), pUser->m_idPlayer, nMode);
	}
}

void CDPSrvr::OnTradelastConfrim(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CMover* pTrader = pUser->m_vtInfo.GetOther();
	if (IsValidObj(pTrader) == FALSE)
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return;

	if (pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser)
	{
		if (pUser->IsDie() || pTrader->IsDie())
			return;

		if (pUser->m_vtInfo.TradeGetState() != TRADE_STEP_OK)
			return;

#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif

		switch (pTrader->m_vtInfo.TradeGetState())
		{
		case TRADE_STEP_OK:
			pUser->m_vtInfo.TradeSetState(TRADE_STEP_CONFIRM);

			pUser->AddTradelastConfirmOk(pUser->GetId());
			((CUser*)pTrader)->AddTradelastConfirmOk(pUser->GetId());
			break;

		case TRADE_STEP_CONFIRM:
		{
			BEFORESENDDUAL(out, PACKETTYPE_LOG_ALLITEM, DPID_UNKNOWN, DPID_UNKNOWN);
			TRADE_CONFIRM_TYPE type = pUser->m_vtInfo.TradeLastConfirm(out);
			switch (type)
			{
			case TRADE_CONFIRM_ERROR:
				pUser->AddTradeCancel(NULL_ID, pUser->m_idPlayer);
				((CUser*)pTrader)->AddTradeCancel(NULL_ID, pUser->m_idPlayer);
				break;
			case TRADE_CONFIRM_OK:
				SEND(out, &g_dpDBClient, DPID_SERVERPLAYER);

				pUser->AddTradeConsent();
				((CUser*)pTrader)->AddTradeConsent();
				break;
			}
		}
		break;
		}
	}
}

void CDPSrvr::OnTradeOk(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	CMover* pTrader = pUser->m_vtInfo.GetOther();
	if (IsValidObj(pTrader) == FALSE)
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return;

	if (pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser)
	{
		if (pUser->IsDie() || pTrader->IsDie())
			return;

#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode || ((CUser*)pTrader)->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif

		if (pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM)
		{
			pUser->m_vtInfo.TradeSetState(TRADE_STEP_OK);

			if (pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_OK)
			{
				pUser->AddTradelastConfirm();
				((CUser*)pTrader)->AddTradelastConfirm();
			}
			else
			{
				pUser->AddTradeOk(pUser->GetId());
				((CUser*)pTrader)->AddTradeOk(pUser->GetId());
			}
		}
	}
}

void CDPSrvr::OnPVendorClose(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objidVendor;
	ar >> objidVendor;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		ClosePVendor(pUser, objidVendor);
	}
}

BOOL CDPSrvr::ClosePVendor(CUser* pUser, OBJID objidVendor)
{
	if (pUser->GetId() == objidVendor)
	{
		if (pUser->m_vtInfo.IsVendorOpen() == FALSE &&
			pUser->m_vtInfo.VendorIsVendor() == FALSE)
		{
			return FALSE;
		}

#ifdef __FL_SHOP_SEARCH
		prj.RemoveFromShopSearch(pUser->GetId());
#endif

		CMover* pTrader = pUser->m_vtInfo.GetOther();   //raiders.2006.11.27

		pUser->m_vtInfo.VendorClose();

		//raiders.2006.11.27
		pUser->m_vtInfo.TradeClear();
		if (pTrader)
			pTrader->m_vtInfo.TradeClear();
		//--

		g_UserMng.AddPVendorClose(pUser);

		CChatting* pChatting = g_ChattingMng.GetChttingRoom(pUser->m_idChatting);
		if (pChatting)
		{
			for (int i = 0; i < pChatting->GetChattingMember(); ++i)
			{
				CUser* pUserBuf = (CUser*)prj.GetUserByID(pChatting->m_idMember[i]);
				if (IsValidObj(pUserBuf))
				{

					pUserBuf->AddDeleteChatting();
					pUserBuf->m_idChatting = 0;
				}
			}
		}
		g_ChattingMng.DeleteChattingRoom(pUser->m_idPlayer);
		pUser->m_idChatting = 0;
	}
	else
	{
		if (IsValidObj(pUser->m_vtInfo.GetOther()))
		{
			pUser->m_vtInfo.SetOther(NULL);
			pUser->AddPVendorClose(objidVendor);

			CChatting* pChatting = g_ChattingMng.GetChttingRoom(pUser->m_idChatting);
			if (pChatting)
			{
				for (int i = 0; i < pChatting->GetChattingMember(); ++i)
				{
					CUser* pUserBuf = (CUser*)prj.GetUserByID(pChatting->m_idMember[i]);
					if (IsValidObj(pUserBuf))
					{
						pUserBuf->AddRemoveChatting(pUser->m_idPlayer);
					}
				}
				pChatting->RemoveChattingMember(pUser->m_idPlayer);
				pUser->m_idChatting = 0;
			}
		}
#ifdef __FL_VENDORS
		else
		{
			pUser->m_vtInfo.SetOther(NULL);

			CChatting* pChatting = g_ChattingMng.GetChttingRoom(pUser->m_idChatting);
			if (pChatting)
			{
				for (int i = 0; i < pChatting->GetChattingMember(); ++i)
				{
					CUser* pUserBuf = (CUser*)prj.GetUserByID(pChatting->m_idMember[i]);
					if (IsValidObj(pUserBuf))
					{
						pUserBuf->AddRemoveChatting(pUser->m_idPlayer);
					}
				}
				pChatting->RemoveChattingMember(pUser->m_idPlayer);
				pUser->m_idChatting = 0;
			}
		}
#endif
	}

	return TRUE;
}

void CDPSrvr::OnBuyPVendorItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objidVendor;
	BYTE nItem;
	short nNum;

	ar >> objidVendor >> nItem >> nNum;

	if (nItem >= MAX_VENDITEM || nNum <= 0)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

#ifndef __FL_VENDORS
	CUser* pPVendor = prj.GetUser(objidVendor);
#else
	CMover* pPVendor = prj.GetMover(objidVendor);
#endif
	if (IsValidObj(pPVendor))
	{
		VENDOR_SELL_RESULT result;
		BOOL bOK = pPVendor->m_vtInfo.VendorSellItem(pUser, nItem, nNum, result);
		if (bOK)
		{
			LogItemInfo info;

			info.Action = "Z";
			info.SendName = pUser->GetName();
			info.RecvName = pPVendor->GetName();
			info.WorldId = pUser->GetWorld()->GetID();
			info.Gold = pUser->GetGold() + (result.item.m_nCost * nNum);
			info.Gold2 = pUser->GetGold();
			info.Gold_1 = pPVendor->GetGold();
			OnLogItem(info, &result.item, nNum);

			info.Action = "X";
			info.SendName = pPVendor->GetName();
			info.RecvName = pUser->GetName();
			info.WorldId = pPVendor->GetWorld()->GetID();
			info.Gold = pPVendor->GetGold() - (result.item.m_nCost * nNum);
			info.Gold2 = pPVendor->GetGold();
			info.Gold_1 = pUser->GetGold();
			OnLogItem(info, &result.item, nNum);
		}
		else
		{
			if (result.nErrorCode)
				pUser->AddDefinedText(result.nErrorCode, "");
		}
	}
}

void CDPSrvr::OnQueryPVendorItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objidVendor;
	ar >> objidVendor;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_vtInfo.GetOther())
			return;
		if (pUser->m_pActMover->IsFly())
			return;

#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif

		CUser* pPVendor = prj.GetUser(objidVendor);
		if (IsValidObj(pPVendor))
		{
			BOOL bChatting = TRUE;
			CChatting* pChatting = g_ChattingMng.GetChttingRoom(pPVendor->m_idChatting);
			if (pChatting)
				bChatting = pChatting->m_bState;

			if (pPVendor->m_vtInfo.IsVendorOpen())
			{
				pUser->m_vtInfo.SetOther(pPVendor);
				pUser->AddPVendorItem(pPVendor, bChatting);
			}
		}
#ifdef __FL_VENDORS
		else
		{
			CMover* pPVendor = prj.GetMover(objidVendor);
			if (IsValidObj(pPVendor))
			{
				if (pPVendor->m_vtInfo.IsVendorOpen())
				{
					pUser->m_vtInfo.SetOther(pPVendor);
					pUser->AddPVendorItem((CUser*)pPVendor, FALSE);
				}
			}
		}
#endif

	}
}

#ifdef __EVE_MINIGAME
void CDPSrvr::OnKawibawiboStart(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	if (CNpcChecker::GetInstance()->IsCloseNpc(MMI_KAWIBAWIBO, pUser->GetWorld(), pUser->GetPos()) == FALSE)
		return;

	pUser->m_nKawibawiboState = prj.m_MiniGame.Result_Kawibawibo(pUser);
	if (pUser->m_nKawibawiboState == CMiniGame::KAWIBAWIBO_WIN)
	{
		CMiniGame::__KAWIBAWIBO Kawibawibo = prj.m_MiniGame.FindKawibawibo(pUser->m_nKawibawiboWin);
		CMiniGame::__KAWIBAWIBO KawibawiboNext = prj.m_MiniGame.FindKawibawibo(pUser->m_nKawibawiboWin + 1);
		pUser->AddKawibawiboResult(pUser->m_nKawibawiboState, pUser->m_nKawibawiboWin, Kawibawibo.dwItemId, Kawibawibo.nItemCount, KawibawiboNext.dwItemId, KawibawiboNext.nItemCount);
	}
	else
		pUser->AddKawibawiboResult(pUser->m_nKawibawiboState, pUser->m_nKawibawiboWin);
}
void CDPSrvr::OnKawibawiboGetItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	prj.m_MiniGame.ResultItem_Kawibawibo(pUser);
}
#endif // __EVE_MINIGAME

void CDPSrvr::OnUltimateMakeItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	OBJID objSunId, objMoonId;
	ar >> objSunId >> objMoonId;

	CItemElem* pItemSun = pUser->m_Inventory.GetAtId(objSunId);
	CItemElem* pItemMoon = pUser->m_Inventory.GetAtId(objMoonId);

	if (!IsUsableItem(pItemSun) || !IsUsableItem(pItemMoon))
	{
		pUser->AddUltimateMakeItem(FALSE);
		return;
	}

	if (pItemSun->m_dwItemId != II_GEN_MAT_ORICHALCUM01 || pItemMoon->m_dwItemId != II_GEN_MAT_MOONSTONE)
	{
		pUser->AddUltimateMakeItem(FALSE);
		return;
	}

	if (pItemSun->m_nItemNum < 5 || pItemMoon->m_nItemNum < 5)
	{
		pUser->AddUltimateMakeItem(FALSE);
		return;
	}

	CItemElem itemElem;
	itemElem.m_dwItemId = II_GEN_MAT_ORICHALCUM02;
	itemElem.m_nItemNum = 1;
	itemElem.SetSerialNumber();
	itemElem.m_nHitPoint = 0;

	if (pUser->CreateItem(&itemElem))
	{
		pUser->RemoveItem((BYTE)objSunId, 5);
		pUser->RemoveItem((BYTE)objMoonId, 5);
		pUser->AddUltimateMakeItem(TRUE);
	}
	else
	{
		pUser->AddDefinedText(TID_GAME_EXTRACTION_ERROR);
		pUser->AddUltimateMakeItem(FALSE);
	}
}
void CDPSrvr::OnUltimateMakeGem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	OBJID objItemId;
	ar >> objItemId;

	int nNum;
	int nResult = prj.m_UltimateWeapon.MakeGem(pUser, objItemId, nNum);
	pUser->AddUltimateMakeGem(nResult, nNum);
}

void CDPSrvr::OnUltimateTransWeapon(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	OBJID objItemWeapon;
	OBJID objItemGem1;
	OBJID objItemGem2;
	OBJID objItemScroll;

	ar >> objItemWeapon;
	ar >> objItemGem1;
	ar >> objItemGem2;
	ar >> objItemScroll;

	int nResult = prj.m_UltimateWeapon.TransWeapon(pUser, objItemWeapon, objItemGem1, objItemGem2, objItemScroll);
	pUser->AddSmeltSafety(nResult);
}

void CDPSrvr::OnUltimateSetGem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	OBJID objItemWeapon;
	OBJID objItemGem;

	ar >> objItemWeapon;
	ar >> objItemGem;

	int nResult = prj.m_UltimateWeapon.SetGem(pUser, objItemWeapon, objItemGem);
	pUser->AddUltimateWeapon(ULTIMATE_SETGEM, nResult);
}

void CDPSrvr::OnUltimateRemoveGem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	OBJID objItemWeapon;
	OBJID objItemGem;

	ar >> objItemWeapon;
	ar >> objItemGem;

	int nSlot2;
	ar >> nSlot2;

	if (nSlot2 < 0 || nSlot2 > 4)
		return;

	int nResult = prj.m_UltimateWeapon.RemoveGem(pUser, objItemWeapon, objItemGem, nSlot2);

	if (nResult == CUltimateWeapon::ULTIMATE_SUCCESS)
	{

		pUser->AddDefinedText(TID_GAME_REMOVEGEM_SUCCESS, "");
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
	}
	else if (nResult == CUltimateWeapon::ULTIMATE_FAILED)
	{

		pUser->AddDefinedText(TID_GAME_REMOVEGEM_FAILED, "");
		pUser->AddPlaySound(SND_INF_UPGRADEFAIL);
		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
	}
	pUser->AddUltimateWeapon(ULTIMATE_REMOVEGEM, nResult);
}

void CDPSrvr::OnUltimateEnchantWeapon(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	OBJID objItemWeapon;
	OBJID objItemGem;

	ar >> objItemWeapon;
	ar >> objItemGem;

	int nResult = prj.m_UltimateWeapon.EnchantWeapon(pUser, objItemWeapon, objItemGem);

	if (nResult == CUltimateWeapon::ULTIMATE_SUCCESS)
	{

		pUser->AddDefinedText(TID_UPGRADE_SUCCEEFUL, "");
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
	}
	else if (nResult == CUltimateWeapon::ULTIMATE_FAILED)
	{

		pUser->AddDefinedText(TID_UPGRADE_FAIL, "");
		pUser->AddPlaySound(SND_INF_UPGRADEFAIL);
		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
	}
	pUser->AddUltimateWeapon(ULTIMATE_ENCHANTWEAPON, nResult);
}

void CDPSrvr::InviteParty(u_long uLeaderid, u_long uMemberid, BOOL bTroup)
{
	CUser* pLeader = g_UserMng.GetUserByPlayerID(uLeaderid);
	CUser* pMember = g_UserMng.GetUserByPlayerID(uMemberid);

	if (IsValidObj(pLeader) == FALSE)
	{
		return;
	}

	if (IsValidObj(pMember) == FALSE)
	{
		pLeader->AddDefinedText(TID_GAME_PARTYINVATEOTHERSVR);
		return;
	}

	if (pLeader == pMember)
		return;

	const DWORD dwResult = g_PartyMng.CanInviteParty(uLeaderid, uMemberid);
	if (dwResult == 5)
	{
#ifdef __FL_FIX_IDCHANGE_EXPLOIT // Party
		BOOL bFound = FALSE;
		for (auto& it : pLeader->vecPartyInvites)
		{
			if (it.dwInvieMember == uMemberid)
			{
				it.dwTimeEnd = GetTickCount() + MIN(10);
				bFound = TRUE;
				break;
			}
		}

		if (!bFound)
		{
			INVITE_REQUESTS tmp;
			tmp.dwInvieMember = uMemberid;
			tmp.dwTimeEnd = GetTickCount() + MIN(10);
			pLeader->vecPartyInvites.push_back(tmp);
		}
#endif

		pMember->AddPartyRequest(pLeader, pMember, bTroup);
	}
	else
	{
		switch (dwResult)
		{
		case 1:
			pLeader->AddDefinedText(TID_GAME_PARTYEXISTCHR, "\"%s\"", pMember->GetName());
			break;

		case 2:
			pLeader->AddDefinedText(TID_GAME_PARTYNOINVATE);
			break;

		case 3:
			pLeader->AddDefinedText(TID_GAME_FULLPARTY3);
			break;

		case 4:
			pLeader->AddDefinedText(TID_GAME_GUILDCOMBAT_CANNOT_PARTY);
			break;

		default:
			break;
		}
	}
}

void CDPSrvr::InviteCompany(CUser* pUser, OBJID objid)
{
#if __VER >= 12 // __SECRET_ROOM
	if (!IsInviteAbleGuild(pUser ? pUser->m_idGuild : NULL, pUser))
	{
		return;
	}
#endif // __SECRET_ROOM

	if (IsValidObj(pUser))
	{
		CUser* pUsertmp = prj.GetUser(objid);
		if (IsValidObj(pUsertmp))
		{
			if (pUser == pUsertmp)
				return;

			CGuild* pGuild = g_GuildMng.GetGuild(pUser->m_idGuild);
			if (pGuild)
			{
				CGuildMember* pMember = pGuild->GetMember(pUser->m_idPlayer);
				if (!pMember)
				{
					// is not member
					return;
				}
				if (!pGuild->IsCmdCap(pMember->m_nMemberLv, PF_INVITATION))
				{
					// have no power
					pUser->AddDefinedText(TID_GAME_GUILDINVAITNOTWARR);
					return;
				}
				CGuild* pGuildtmp = g_GuildMng.GetGuild(pUsertmp->m_idGuild);
				if (pGuildtmp && pGuildtmp->IsMember(pUsertmp->m_idPlayer))
				{
					// is already guild member
					pUser->AddDefinedText(TID_GAME_COMACCEPTHAVECOM, "%s", pUsertmp->GetName(TRUE));
				}
				else
				{

#ifdef __FL_FIX_IDCHANGE_EXPLOIT // Guild
					BOOL bFound = FALSE;
					for (auto& it : pUser->vecGuildInvites)
					{
						if (it.dwInvieMember == pUsertmp->m_idPlayer)
						{
							it.dwTimeEnd = GetTickCount() + MIN(10);
							bFound = TRUE;
							break;
						}
					}

					if (!bFound)
					{
						INVITE_REQUESTS tmp;
						tmp.dwInvieMember = pUsertmp->m_idPlayer;
						tmp.dwTimeEnd = GetTickCount() + MIN(10);
						pUser->vecGuildInvites.push_back(tmp);
					}
#endif

					pUsertmp->m_idGuild = 0;
					pUsertmp->AddGuildInvite(pGuild->m_idGuild, pUser->m_idPlayer);
					pUser->AddDefinedText(TID_GAME_COMACCEPTKINGPIN, "%s", pUsertmp->GetName(TRUE));
				}
			}
		}
	}
}

#if __VER >= 12 // __SECRET_ROOM
#ifndef __FL_GUILD_FINDER
BOOL CDPSrvr::IsInviteAbleGuild(CUser* pUser)
{
	CGuild* pGuild = pUser->GetGuild();
	if (!pGuild)
		return FALSE;


	if (g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE)
	{
		if (g_GuildCombatMng.FindGuildCombatMember(pUser->m_idGuild) &&
			g_GuildCombatMng.FindGuildCombatMember(pUser->m_idGuild)->bRequest)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_INVITATION_GUILD);
			return FALSE;
		}
	}


	if (CSecretRoomMng::GetInstance()->m_nState != SRMNG_CLOSE)
	{
		map<BYTE, CSecretRoomContinent*>::iterator it = CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.begin();
		for (; it != CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.end(); it++)
		{
			CSecretRoomContinent* pSRCont = it->second;
			if (pSRCont && (pSRCont->GetTenderGuild(pGuild->GetGuildId()) != NULL_ID))
			{
				pUser->AddDefinedText(TID_GAME_SECRETROOM_NOT_INVITATION_GUILD);
				return FALSE;
			}
		}
	}

	return TRUE;
}
#else
BOOL CDPSrvr::IsInviteAbleGuild(u_long idGuild, CUser* pUser)
{
	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (!pGuild)
		return FALSE;

	if (g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE)
	{
		if (g_GuildCombatMng.FindGuildCombatMember(idGuild) &&
			g_GuildCombatMng.FindGuildCombatMember(idGuild)->bRequest)
		{
			if (IsValidObj(pUser))
				pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_INVITATION_GUILD);

			return FALSE;
		}
	}


	if (CSecretRoomMng::GetInstance()->m_nState != SRMNG_CLOSE)
	{
		map<BYTE, CSecretRoomContinent*>::iterator it = CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.begin();
		for (; it != CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.end(); it++)
		{
			CSecretRoomContinent* pSRCont = it->second;
			if (pSRCont && (pSRCont->GetTenderGuild(idGuild) != NULL_ID))
			{
				if (IsValidObj(pUser))
					pUser->AddDefinedText(TID_GAME_SECRETROOM_NOT_INVITATION_GUILD);

				return FALSE;
			}
		}
	}

	return TRUE;
}
#endif
#endif // __SECRET_ROOM

void CDPSrvr::OnPetRelease(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (!pUser->HasActivatedSystemPet())
			pUser->AddDefinedText(TID_GAME_PET_NOT_FOUND);
		else
		{
			CItemElem* pItemElem = pUser->GetPetItem();
			if (pItemElem && pItemElem->m_pPet)
				pUser->PetRelease();
		}
	}
}

void CDPSrvr::OnUsePetFeed(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CPet* pPet = pUser->GetPet();
		if (pPet == NULL)
		{
			pUser->AddDefinedText(TID_GAME_PET_NOT_FOUND);
			return;
		}

		DWORD dwFeedId;
		ar >> dwFeedId;

		CItemElem* pFeed = static_cast<CItemElem*>(pUser->m_Inventory.GetAtId(dwFeedId));
		if (IsUsableItem(pFeed) == FALSE)
			return;

		if (!pFeed->IsFeed())
			return;

		short nNum = pFeed->m_nItemNum;

		int nMaxNum = 0;
		if (pPet->GetLevel() == PL_EGG)
			nMaxNum = MAX_PET_EGG_EXP - pPet->GetExp();
		else
		{
			nMaxNum = pPet->GetMaxEnergy() - pPet->GetEnergy();
			nMaxNum /= 2;
		}

		if (nNum > nMaxNum)
			nNum = nMaxNum;
		if (nNum < 1)
			return;

		if (pPet->GetLevel() == PL_EGG)
		{
			pPet->SetExp(pPet->GetExp() + nNum);
			pUser->AddPetSetExp(pPet->GetExp());
		}
		else
		{
			pPet->SetEnergy(pPet->GetEnergy() + nNum * 2);
			g_UserMng.AddPetFeed(pUser, pPet->GetEnergy());
		}

		pUser->RemoveItem((BYTE)(pFeed->m_dwObjId), nNum);
		pUser->AddDefinedText(TID_GAME_PETFEED_S01, "%d", nNum);
	}
}

void CDPSrvr::OnMakePetFeed(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		DWORD dwMaterialId, dwToolId;
		short nNum;

		ar >> dwMaterialId >> dwToolId >> nNum;

		BOOL bTool = FALSE;
		CItemElem* pTool = NULL;
		if (dwToolId != NULL_ID)
		{
			pTool = pUser->m_Inventory.GetAtId(dwToolId);
			if (IsUsableItem(pTool))
			{
				if (pTool->m_dwItemId == II_SYS_SYS_FEED_MAKER)
					bTool = TRUE;
			}

			if (!bTool)	// error
				return;
		}

		if (bTool == FALSE && CNpcChecker::GetInstance()->IsCloseNpc(MMI_PET_FOODMILL, pUser->GetWorld(), pUser->GetPos()) == FALSE)
			return;

		CItemElem* pMaterial = pUser->m_Inventory.GetAtId(dwMaterialId);
		if (!IsUsableItem(pMaterial))
			return;

		ItemProp* pProp = pMaterial->GetProp();
		if (pProp == NULL)
			return;

		if (pProp->dwItemKind3 != IK3_GEM)
		{
			pUser->AddDefinedText(TID_GAME_PET_IS_NOT_FEED);
			return;
		}

		if (nNum < 1)
			nNum = 1;
		if (nNum > pMaterial->m_nItemNum)
			nNum = pMaterial->m_nItemNum;

		if (pUser->m_Inventory.IsEquip(dwMaterialId))
		{
			pUser->AddDefinedText(TID_GAME_PET_FEED_EQUIPED);
			return;
		}

		CItemElem itemElem;
		itemElem.m_dwItemId = II_SYS_SYS_FEED_01;
		int nTotalFeed = 0;
		int nPackMax = itemElem.GetProp()->dwPackMax;
		for (int i = 0; i < nNum; i++)
		{
			int nFeed = CPetProperty::GetInstance()->GetFeedEnergy(pProp->dwCost, (int)bTool);
			if (nTotalFeed + nFeed > nPackMax)
			{
				nNum = i;
				break;
			}
			nTotalFeed += nFeed;
		}
		itemElem.m_nItemNum = nTotalFeed;
		itemElem.m_nHitPoint = -1;

		int nResult = pUser->CreateItem(&itemElem);
		if (nResult)
		{
#ifndef __FL_FIX_USELESS_LOGS
			LogItemInfo aLogItem;
			aLogItem.Action = "~";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "PET_FOOD_MATERIAL_REMOVE";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			OnLogItem(aLogItem, pMaterial, nNum);

			aLogItem.RecvName = "PET_FOOD_CREATE";
			OnLogItem(aLogItem, &itemElem, itemElem.m_nItemNum);
#endif

			pUser->RemoveItem((BYTE)(pMaterial->m_dwObjId), nNum);
			if (bTool)
				pUser->RemoveItem((BYTE)(pTool->m_dwObjId), 1);
		}
		pUser->AddPetFoodMill(nResult, itemElem.m_nItemNum);
	}
}

void CDPSrvr::OnPetTamerMistake(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		ar >> dwId;
		CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwId);
		if (IsUsableItem(pItemElem) && pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TAMER_MISTAKE) // m_dwItemId
		{
			CPet* pPet = pUser->GetPet();
			if (pPet)
			{
				if (pPet->GetLevel() >= PL_C && pPet->GetLevel() <= PL_S)
				{
					pPet->SetAvailLevel(pPet->GetLevel(), 0);
					pPet->SetLevel(pPet->GetLevel() - 1);
					pPet->SetExp(MAX_PET_EXP);

					if (pUser->HasPet())
						pUser->RemovePet();
					pUser->AddPet(pPet, PF_PET_LEVEL_DOWN);
					g_UserMng.AddPetLevelup(pUser, MAKELONG((WORD)pPet->GetIndex(), (WORD)pPet->GetLevel()));
					pUser->RemoveItem((BYTE)(pItemElem->m_dwObjId), 1);
				}
				else
					pUser->AddDefinedText(TID_GAME_PET_BETWEEN_C_TO_S);
			}
			else
				pUser->AddDefinedText(TID_GAME_PET_NOT_FOUND);
		}
	}
}

void CDPSrvr::OnPetTamerMiracle(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		ar >> dwId;
		CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwId);
		if (IsUsableItem(pItemElem) && pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TAMER_MIRACLE)
		{
			CPet* pPet = pUser->GetPet();
			if (pPet)
			{
				if (pPet->GetLevel() >= PL_B && pPet->GetLevel() <= PL_S)
				{
					pPet->SetAvailLevel(pPet->GetLevel() - 1, 0);
					pPet->SetAvailLevel(pPet->GetLevel(), 0);

					BYTE nAvailLevel = CPetProperty::GetInstance()->GetLevelupAvailLevel(pPet->GetLevel() - 1);
					pPet->SetAvailLevel(pPet->GetLevel() - 1, nAvailLevel);
					nAvailLevel = CPetProperty::GetInstance()->GetLevelupAvailLevel(pPet->GetLevel());
					pPet->SetAvailLevel(pPet->GetLevel(), nAvailLevel);

					if (pUser->HasPet())
						pUser->RemovePet();
					pUser->AddPet(pPet, PF_PET_GET_AVAIL);
					g_UserMng.AddPetLevelup(pUser, MAKELONG((WORD)pPet->GetIndex(), (WORD)pPet->GetLevel()));
					pUser->RemoveItem((BYTE)(pItemElem->m_dwObjId), 1);
				}
				else
					pUser->AddDefinedText(TID_GAME_PET_BETWEEN_B_TO_S);
			}
			else
				pUser->AddDefinedText(TID_GAME_PET_NOT_FOUND);
		}
	}
}

void CDPSrvr::OnFeedPocketInactive(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET))
			pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET);
	}
}

void CDPSrvr::OnModifyStatus(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nStrCount, nStaCount, nDexCount, nIntCount;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	ar >> nStrCount >> nStaCount >> nDexCount >> nIntCount;

	const int remainStatPoint = pUser->m_nRemainGP;

	if ((nStrCount < 0 || nStaCount < 0 || nDexCount < 0 || nIntCount < 0)
		|| (remainStatPoint < nStrCount || remainStatPoint < nStaCount || remainStatPoint < nDexCount || remainStatPoint < nIntCount))
	{
		return;
	}

	const __int64 sumModifyStatPoint = static_cast<__int64>(nStrCount) + static_cast<__int64>(nStaCount) + static_cast<__int64>(nDexCount) + static_cast<__int64>(nIntCount);
	if (sumModifyStatPoint <= 0 || remainStatPoint < sumModifyStatPoint)
	{
		return;
	}

	if (remainStatPoint >= (nStrCount + nStaCount + nDexCount + nIntCount))
	{
		pUser->m_nStr += nStrCount;
		pUser->m_nSta += nStaCount;
		pUser->m_nDex += nDexCount;
		pUser->m_nInt += nIntCount;
		pUser->m_nRemainGP = remainStatPoint - (nStrCount + nStaCount + nDexCount + nIntCount);

		pUser->RefreshGrilledEel();
		g_UserMng.AddStatInfo(pUser, FALSE);

		pUser->CheckHonorStat();
		pUser->AddHonorListAck();
		g_UserMng.AddHonorTitleChange(pUser, pUser->m_nHonor);

#ifdef __FL_RECORD_BOOK
		if (nStrCount > 0)
			CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::STR, pUser->GetStr(), false);

		if (nStaCount > 0)
		{
			CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::STA, pUser->GetSta(), false);
			CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::HP, pUser->GetMaxHitPoint(), false);
		}

		if (nDexCount > 0)
			CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::DEX, pUser->GetDex(), false);

		if (nIntCount > 0)
			CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::INTT, pUser->GetInt(), false);
#endif
	}
}

void CDPSrvr::OnLegendSkillStart(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	OBJID objItemId[5];
	for (int i = 0; i < 5; ++i)
		ar >> objItemId[i];

	if ((pUser->IsHero() == FALSE) && (pUser->IsLegendHero() == FALSE))
		return;

	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
		if (lpSkill && lpSkill->dwSkill != NULL_ID)
		{
			ItemProp* pSkillProp = prj.GetSkillProp(lpSkill->dwSkill);
			if (pSkillProp == NULL)
				continue;
			if (pSkillProp->dwItemKind1 != JTYPE_HERO)
				continue;
			if (lpSkill->dwLevel > 4)
			{
				pUser->AddLegendSkillResult(-1);
				return;
			}
		}
	}

	CItemElem* pItemElem[5];
	for (int i = 0; i < 5; i++)
	{
		pItemElem[i] = pUser->m_Inventory.GetAtId(objItemId[i]);
		if (IsUsableItem(pItemElem[i]) == FALSE)
			return;
	}

	if (pItemElem[0]->m_dwItemId != II_GEN_MAT_DIAMOND ||
		pItemElem[1]->m_dwItemId != II_GEN_MAT_EMERALD ||
		pItemElem[2]->m_dwItemId != II_GEN_MAT_SAPPHIRE ||
		pItemElem[3]->m_dwItemId != II_GEN_MAT_RUBY ||
		pItemElem[4]->m_dwItemId != II_GEN_MAT_TOPAZ)
		return;


	for (int i = 0; i < 5; i++)
	{
#ifndef __FL_FIX_USELESS_LOGS
		LogItemInfo aLogItem;
		aLogItem.Action = "+";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "LEGENDSKILL_USE";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		OnLogItem(aLogItem, pItemElem[i], 1);
#endif
		pUser->RemoveItem((BYTE)(objItemId[i]), (short)1);
	}


	if (xRandom(1000) > 766)
	{
		for (int i = 0; i < MAX_SKILL_JOB; i++)
		{
			LPSKILL lpSkill = &(pUser->m_aJobSkill[i]);
			if (lpSkill && lpSkill->dwSkill != NULL_ID)
			{
				ItemProp* pSkillProp = prj.GetSkillProp(lpSkill->dwSkill);
				if (pSkillProp == NULL)
					continue;
				if (pSkillProp->dwItemKind1 != JTYPE_HERO)
					continue;
				lpSkill->dwLevel++;
			}
		}
		g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_SYS_EXCHAN01, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		pUser->AddDoUseSkillPoint(&(pUser->m_aJobSkill[0]), pUser->m_nSkillPoint);
		pUser->AddLegendSkillResult(TRUE);
	}
	else
	{
		pUser->AddLegendSkillResult(FALSE);
	}
}

void CDPSrvr::OnQueryStartCollecting(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->IsDisguise())
			return;

		if (!pUser->GetWorld() || pUser->GetWorld()->GetID() != WI_WORLD_MADRIGAL)
			return;

		if (pUser->IsDie())
			return;

		pUser->StartCollecting();
	}
}

void CDPSrvr::OnQueryStopCollecting(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		pUser->StopCollecting();
}

#if __VER >= 11
void CDPSrvr::OnQueryGuildBankLogList(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	bool bMK = FALSE;
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		BYTE byListType;
		ar >> byListType;
		CGuild* pGuild = pUser->GetGuild();

		if (pGuild)
		{
			CGuildMember* pGuildMember = pGuild->GetMember(pUser->m_idPlayer);
			if (pGuildMember)
			{
				if (pGuildMember->m_nMemberLv == GUD_KINGPIN || pGuildMember->m_nMemberLv == GUD_MASTER)
					g_dpDBClient.SendQueryGetGuildBankLogList(pUser->m_idPlayer, pGuild->m_idGuild, byListType);
			}
		}

	}
}
#endif
#if __VER >= 13
void CDPSrvr::OnHonorListReq(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		pUser->AddHonorListAck();
	}
}
void CDPSrvr::OnHonorChangeReq(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		int nChange;
		ar >> nChange;
		if (nChange > -1 && nChange < MAX_HONOR_TITLE)
		{
			int nNeed = CTitleManager::Instance()->GetNeedCount(nChange, -1);
			if (nNeed < 0)
				return;
			if (pUser->GetHonorTitle(nChange) >= nNeed)
			{
				pUser->m_nHonor = nChange;
				g_UserMng.AddHonorTitleChange(pUser, nChange);
			}
		}
		else if (nChange == -1)
		{
			pUser->m_nHonor = nChange;
			g_UserMng.AddHonorTitleChange(pUser, nChange);
		}
	}
}
#endif

void	CDPSrvr::OnBlessednessCancel(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_BLESSING_CANCEL, pUser->GetWorld(), pUser->GetPos()))
			return;

		int nItem;
		ar >> nItem;
		CItemElem* pItem = pUser->m_Inventory.GetAtId(nItem);
		if (IsUsableItem(pItem))
		{
			if (pUser->m_Inventory.IsEquip(pItem->m_dwObjId))
				return;

			if (g_xRandomOptionProperty->GetRandomOptionKind(pItem) == CRandomOptionProperty::eBlessing
				&& g_xRandomOptionProperty->GetRandomOptionSize(pItem->GetRandomOptItemId()) > 0)
			{
				g_xRandomOptionProperty->InitializeRandomOption(pItem->GetRandomOptItemIdPtr());
				pUser->UpdateItemEx((BYTE)(pItem->m_dwObjId), UI_RANDOMOPTITEMID, pItem->GetRandomOptItemId());
				pUser->AddDiagText(prj.GetText(TID_GAME_BLESSEDNESS_CANCEL_INFO));

#ifndef __FL_FIX_USELESS_LOGS
				// log
				LogItemInfo	log;
				log.Action = "r";
				log.SendName = pUser->GetName();
				log.RecvName = "OnBlessednessCancel";
				log.WorldId = pUser->GetWorld()->GetID();
				log.Gold = pUser->GetGold();
				log.Gold2 = pUser->GetGold();
				OnLogItem(log, pItem, 1);
#endif
			}
			else
			{
				pUser->AddDefinedText(TID_GAME_BLESSEDNESS_CANCEL);
			}
		}
	}
}

#ifdef __NPC_BUFF
void	CDPSrvr::OnNPCBuff(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CHAR	m_szKey[64];
		ar.ReadString(m_szKey, 64);
		LPCHARACTER lpChar = prj.GetCharacter(m_szKey);

		if (lpChar)
		{
			if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_NPC_BUFF, pUser->GetWorld(), pUser->GetPos()))
				return;

			vector<NPC_BUFF_SKILL> vecNPCBuff = lpChar->m_vecNPCBuffSkill;
			for (int i = 0; i < (int)(vecNPCBuff.size()); i++)
			{
				if (pUser->GetLevel() >= vecNPCBuff[i].nMinPlayerLV && pUser->GetLevel() <= vecNPCBuff[i].nMaxPlayerLV)
				{
					ItemProp* pSkillProp = prj.GetSkillProp(vecNPCBuff[i].dwSkillID);
					if (pSkillProp)
					{
						if (vecNPCBuff[i].dwSkillLV < pSkillProp->dwExpertLV || vecNPCBuff[i].dwSkillLV > pSkillProp->dwExpertMax)
							continue;

						if ((pSkillProp->dwID == SI_GEN_EVE_QUICKSTEP && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_QUICKSTEP))
							|| (pSkillProp->dwID == SI_GEN_EVE_HASTE && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_HASTE))
							|| (pSkillProp->dwID == SI_GEN_EVE_HEAPUP && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_HEAPUP))
							|| (pSkillProp->dwID == SI_GEN_EVE_ACCURACY && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_ACCURACY)))
						{
							pUser->AddDefinedText(TID_GAME_NPCBUFF_FAILED, "\"%s\"", pSkillProp->szName);
							continue;
						}

						AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine + vecNPCBuff[i].dwSkillLV - 1);
						if (pAddSkillProp)
						{


							DWORD dwReferTarget1Backup = pSkillProp->dwReferTarget1;
							pSkillProp->dwReferTarget1 = NULL_ID;
							DWORD dwReferTarget2Backup = pSkillProp->dwReferTarget2;
							pSkillProp->dwReferTarget2 = NULL_ID;
							DWORD dwSkillTimeBackup = pAddSkillProp->dwSkillTime;
							pAddSkillProp->dwSkillTime = vecNPCBuff[i].dwSkillTime;

							pUser->DoApplySkill(pUser, pSkillProp, pAddSkillProp);
							g_UserMng.AddDoApplySkill(pUser, pUser->GetId(), vecNPCBuff[i].dwSkillID, vecNPCBuff[i].dwSkillLV);

							pSkillProp->dwReferTarget1 = dwReferTarget1Backup;
							pSkillProp->dwReferTarget2 = dwReferTarget2Backup;
							pAddSkillProp->dwSkillTime = dwSkillTimeBackup;
						}
					}
				}
				else
				{
					ItemProp* pSkillProp = prj.GetSkillProp(vecNPCBuff[i].dwSkillID);
					if (pSkillProp)
						pUser->AddDefinedText(TID_GAME_NPCBUFF_LEVELLIMIT, "%d %d \"%s\"", vecNPCBuff[i].nMinPlayerLV, vecNPCBuff[i].nMaxPlayerLV, pSkillProp->szName);
				}
			}
		}
	}
}
#endif // __NPC_BUFF

#ifdef __JEFF_11_4
void	CDPSrvr::OnArenaEnter(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		pUser->SetMarkingPos();
		pUser->REPLACE(g_uIdofMulti, WI_WORLD_ARENA, D3DXVECTOR3(540.0F, 140.0F, 485.0F), REPLACE_NORMAL, nDefaultLayer);
	}
}

void	CDPSrvr::OnArenaExit(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->GetWorld() && pUser->GetWorld()->IsArena())
		{
			if (pUser->m_idMarkingWorld)
				pUser->REPLACE(g_uIdofMulti, pUser->m_idMarkingWorld, pUser->m_vMarkingPos, REPLACE_NORMAL, nTempLayer);
			else
				pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6968.0f, 0.0f, 3328.8f), REPLACE_NORMAL, nDefaultLayer);
		}
	}
}
#endif	// __JEFF_11_4

void	CDPSrvr::OnQuePetResurrection(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		int nItem;
		ar >> nItem;
		CItemElem* pItem = pUser->m_Inventory.GetAtId(nItem);
		if (!IsUsableItem(pItem))
			return;

		CPet* pPet = pItem->m_pPet;
		if (!pPet || pUser->GetPetId() == pItem->m_dwObjId || pItem->IsFlag(CItemElem::expired))
		{
			pUser->AddQuePetResurrectionResult(FALSE);
			return;
		}

		BYTE nLevel = pPet->GetLevel();
		if (nLevel < PL_B || nLevel > PL_S)
		{
			pUser->AddQuePetResurrectionResult(FALSE);
			return;
		}

		CItemElem itemElem;
		itemElem.m_nItemNum = 1;
		switch (nLevel)
		{
		case PL_B:	itemElem.m_dwItemId = II_SYS_SYS_QUE_PETRESURRECTION01_B;	break;
		case PL_A:	itemElem.m_dwItemId = II_SYS_SYS_QUE_PETRESURRECTION01_A;	break;
		case PL_S:	itemElem.m_dwItemId = II_SYS_SYS_QUE_PETRESURRECTION01_S;	break;
		}

#ifndef __FL_FIX_USELESS_LOGS
		// log
		LogItemInfo	log;
		log.Action = "x";
		log.SendName = pUser->GetName();
		log.RecvName = "OnQuePetResurrection";
		OnLogItem(log, pItem, 1);
#endif

		pUser->RemoveItem(nItem, 1);
		pUser->CreateItem(&itemElem);
		pUser->AddQuePetResurrectionResult(TRUE);
	}
}

#if __VER >= 12 // __SECRET_ROOM
void	CDPSrvr::OnSecretRoomTenderOpenWnd(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	CSecretRoomMng::GetInstance()->SetTenderOpenWnd(pUser);
}

void	CDPSrvr::OnSecretRoomTender(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	int nPenya;
	ar >> nPenya;

	if (nPenya <= 0)
		return;

	CSecretRoomMng::GetInstance()->SetTender(pUser, nPenya);
}

void	CDPSrvr::OnSecretRoomTenderCancelReturn(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	CSecretRoomMng::GetInstance()->SetTenderCancelReturn(pUser);
}

void	CDPSrvr::OnSecretRoomLineUpOpenWnd(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	CSecretRoomMng::GetInstance()->SetLineUpOpenWnd(pUser);
}

void	CDPSrvr::OnSecretRoomLineUpMember(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	set<DWORD> checker;
	vector<DWORD> vecLineUpMember;
	int nSize;
	ar >> nSize;

	if (nSize > CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum)
		return;

	for (int i = 0; i < nSize; i++)
	{
		DWORD dwIdPlayer;
		ar >> dwIdPlayer;


		PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(dwIdPlayer);
		if (!pData)
			return;


		if (!checker.insert(dwIdPlayer).second)
			return;

		vecLineUpMember.push_back(dwIdPlayer);
	}

	CSecretRoomMng::GetInstance()->SetLineUp(pUser, vecLineUpMember);
}

void	CDPSrvr::OnSecretRoomEntrance(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	CSecretRoomMng::GetInstance()->SetTeleportSecretRoom(pUser);
}

void	CDPSrvr::OnSecretRoomTeleportToNPC(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;
	pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, CSecretRoomMng::GetInstance()->GetRevivalPos(pUser), REPLACE_NORMAL, nDefaultLayer);
}

void CDPSrvr::OnSecretRoomTenderView(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	CSecretRoomMng::GetInstance()->GetTenderView(pUser);
}

#endif // __SECRET_ROOM

#if __VER >= 12 // __LORD
void CDPSrvr::OnElectionAddDeposit(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		__int64 iTotal;
		ar >> iTotal;

		__int64 iDeposit;
		int nRet = election::AddDepositRequirements(pUser, iTotal, iDeposit);
		if (nRet)
		{
			pUser->AddDefinedText(nRet);
			return;
		}
		pUser->AddGold(-static_cast<int>(iDeposit));
		g_dpDBClient.SendElectionAddDeposit(pUser->m_idPlayer, iDeposit);

		PutPenyaLog(pUser, "d", "DEPOSIT", static_cast<int>(iDeposit));
	}
}

void CDPSrvr::PutItemLog(CUser* pUser, const char* szAction, const char* szContext, CItemElem* pItem, int nNum)
{
	if (!IsValidObj(pUser))
		return;

	LogItemInfo	log;
	log.Action = szAction;
	log.SendName = pUser->GetName();
	log.RecvName = szContext;
	log.WorldId = pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;
	log.Gold = pUser->GetGold();
	log.Gold2 = pUser->GetGold();
	if (nNum == 0)
		nNum = pItem->m_nItemNum;
	OnLogItem(log, pItem, nNum);
}

void CDPSrvr::PutPenyaLog(CUser* pUser, const char* szAction, const char* szContext, int nCost)
{
	LogItemInfo	log;
	log.Action = szAction;
	log.SendName = pUser->GetName();
	log.RecvName = szContext;
	log.WorldId = pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;
	//log.ItemName	= "SEED";
	_stprintf(log.szItemName, "%d", II_GOLD_SEED1);
	log.Gold = pUser->GetGold() + nCost;
	log.Gold2 = pUser->GetGold();
	log.Gold_1 = -nCost;
	OnLogItem(log);
}

void CDPSrvr::OnElectionSetPledge(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	char szPledge[CCandidate::nMaxPledgeLen] = { 0, };
	ar.ReadString(szPledge, CCandidate::nMaxPledgeLen);
	if (strlen(szPledge) == 0)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{

#ifdef __FIX_SQL_INJECTS // test thingy
		CString str = szPledge;
		if (str.Find("--") >= 0 || str.Find("'") >= 0 || str.Find("/*") >= 0 || str.Find("*/") >= 0 || str.Find(";") >= 0
			|| str.Find("--") >= 0 || str.Find("'") >= 0 || str.Find("/*") >= 0 || str.Find("*/") >= 0 || str.Find(";") >= 0)
		{
			pUser->AddDefinedText(TID_DIAG_0013);
			return;
		}
#endif

		int nCost;
		int nRet = election::SetPledgeRequirements(pUser, nCost);
		if (nRet)
		{
			pUser->AddDefinedText(nRet);
			return;
		}
		pUser->AddGold(-nCost);
		g_dpDBClient.SendElectionSetPledge(pUser->m_idPlayer, szPledge);

		PutPenyaLog(pUser, "p", "PLEDGE", nCost);
	}
}

void CDPSrvr::OnElectionIncVote(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		u_long idPlayer;
		ar >> idPlayer;

		int nRet = election::IncVoteRequirements(pUser, idPlayer);
		if (nRet)
		{
			pUser->AddDefinedText(nRet);
			return;
		}
		g_dpDBClient.SendElectionIncVote(idPlayer, pUser->m_idPlayer);
	}
}

#ifndef __FL_UPDATE_LORDEVENT
void CDPSrvr::OnLEventCreate(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		int iEEvent, iIEvent;
		ar >> iEEvent >> iIEvent;
		int nRet = lordevent::CreateRequirements(pUser, iEEvent, iIEvent);
		if (nRet)
		{
			pUser->AddDefinedText(nRet);
			return;
		}
		ILordEvent* pEvent = CSLord::Instance()->GetEvent();

		int nPerin = pUser->RemoveTotalGold(pEvent->GetCost(iEEvent, iIEvent));
		char szContext[100] = { 0, };
		sprintf(szContext, "OnLEventCreate: cost: %d(perin), %I64d(penya)", nPerin, pEvent->GetCost(iEEvent, iIEvent) - (nPerin * PERIN_VALUE));
		PutPenyaLog(pUser, "e", szContext, 0);

		g_dpDBClient.SendLEventCreate(pUser->m_idPlayer, iEEvent, iIEvent);
	}
}
#else
void CDPSrvr::OnLEventCreate(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		int iEEvent, iIEvent;
		ar >> iEEvent >> iIEvent;
		int iPEvent;
		ar >> iPEvent;

		int nRet = lordevent::CreateRequirements(pUser, iEEvent, iIEvent, iPEvent);
		if (nRet)
		{
			pUser->AddDefinedText(nRet);
			return;
		}
		ILordEvent* pEvent = CSLord::Instance()->GetEvent();

		int nPerin = pUser->RemoveTotalGold(pEvent->GetCost(iEEvent, iIEvent, iPEvent));
		char szContext[100] = { 0, };
		sprintf(szContext, "OnLEventCreate: cost: %d(perin), %I64d(penya)", nPerin, pEvent->GetCost(iEEvent, iIEvent, iPEvent) - (nPerin * PERIN_VALUE));
		PutPenyaLog(pUser, "e", szContext, 0);

		g_dpDBClient.SendLEventCreate(pUser->m_idPlayer, iEEvent, iIEvent, iPEvent);
	}
}
#endif

void CDPSrvr::OnLordSkillUse(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		int nSkill;
		char szTarget[MAX_PLAYER] = { 0, };
		ar >> nSkill;
		ar.ReadString(szTarget, MAX_PLAYER);
		u_long idTarget;
		int nRet = lordskill::UseRequirements(pUser, szTarget, nSkill, idTarget);
		if (nRet)
		{
			pUser->AddDefinedText(nRet);
			return;
		}
		g_dpDBClient.SendLordSkillUse(pUser->m_idPlayer, idTarget, nSkill);
	}
}
#endif	// __LORD

#if __VER >= 12 // __TAX
void CDPSrvr::OnSetTaxRate(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	BYTE nCont;
	int nSalesTaxRate, nPurchaseTaxRate;
	ar >> nCont;
	ar >> nSalesTaxRate >> nPurchaseTaxRate;

	__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo(nCont);
	if (pTaxInfo && pUser->m_idGuild == pTaxInfo->dwNextId && pUser->IsGuildMaster())
		CTax::GetInstance()->SetNextTaxRate(nCont, nSalesTaxRate, nPurchaseTaxRate);
}
#endif // __TAX

#if __VER >= 12 // __HEAVEN_TOWER
void CDPSrvr::OnTeleportToHeavenTower(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_HEAVEN_TOWER, pUser->GetWorld(), pUser->GetPos()))
		return;

	int nFloor;
	ar >> nFloor;

	int nCost = 0;
	DWORD dwWorldId = NULL_ID;
	float fAngle = 0.0f;
	D3DXVECTOR3 vPos;

	switch (nFloor)
	{
	case 1:
		nCost = 10000;	dwWorldId = WI_WORLD_HEAVEN01; vPos = D3DXVECTOR3(253, 102, 78); fAngle = 183.0f;
		break;
	case 2:
		nCost = 30000;	dwWorldId = WI_WORLD_HEAVEN02; vPos = D3DXVECTOR3(251, 102, 95); fAngle = 183.0f;
		break;
	case 3:
		nCost = 50000;	dwWorldId = WI_WORLD_HEAVEN03; vPos = D3DXVECTOR3(264, 102, 227); fAngle = 183.0f;
		break;
	case 4:
		nCost = 70000;	dwWorldId = WI_WORLD_HEAVEN04; vPos = D3DXVECTOR3(253, 102, 86); fAngle = 174.0f;
		break;
	case 5:
		nCost = 100000;	dwWorldId = WI_WORLD_HEAVEN05; vPos = D3DXVECTOR3(218, 102, 101); fAngle = 176.0f;
		break;

	default:
		Error("CDPSrvr::OnTeleportToHeavenTower() - Àß¸øµÈ Ãþ : %d, Name = %s", nFloor, pUser->GetName());
		return;
	}

	if (pUser->CheckUserGold(nCost, false) == false)
	{
		pUser->AddDefinedText(TID_GAME_LACKMONEY);
		return;
	}


	BYTE nCont = CTax::GetInstance()->GetContinent(pUser);
	if (pUser->REPLACE(g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, nDefaultLayer))
	{
		pUser->AddGold(-nCost);
		pUser->SetAngle(fAngle);

		__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo(nCont);
		if (pTaxInfo && pTaxInfo->dwId != NULL_ID)
			CTax::GetInstance()->AddTax(nCont, nCost, TAX_ADMISSION);

#ifndef __FL_FIX_USELESS_LOGS
		CString strFloor;
		strFloor.Format("HEAVEN_TOWER_%2d", nFloor);
		PutPenyaLog(pUser, "h", strFloor, nCost);
#endif
	}
	else
		return;
}
#endif // __HEAVEN_TOWER

BOOL CDPSrvr::DoUseItemTarget_GenRandomOption(
	CUser* pUser, CItemElem* pTarget, int nKind,
	int nOk, int nMismatchKind, int nHasOption,
	const char* szOperation, const char* szReceive)
{
	int nRandomOptionKind = g_xRandomOptionProperty->GetRandomOptionKind(pTarget);
	if (nRandomOptionKind != nKind || pTarget->IsFlag(CItemElem::expired))
	{
		pUser->AddDefinedText(nMismatchKind);
		return FALSE;
	}

	if (
#if __VER >= 12 // __J12_0

		nKind != CRandomOptionProperty::eBlessing && nKind != CRandomOptionProperty::eEatPet &&
#endif	// __J12_0
		g_xRandomOptionProperty->GetRandomOptionSize(pTarget->GetRandomOptItemId()) > 0
		)
	{
		pUser->AddDefinedText(nHasOption);
		return FALSE;
	}

	if (nOk > 0)
		pUser->AddDefinedText(nOk);


	if (pUser->IsUsing(pTarget) && nKind == CRandomOptionProperty::eEatPet)
		pUser->ResetDestParamRandomOptExtension(pTarget);



	//g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
	//g_xRandomOptionProperty->GenRandomOption( pTarget->GetRandomOptItemIdPtr(), nRandomOptionKind, pTarget->GetProp()->dwParts );
	bool	bCheckedSafeFlag = false;
	bCheckedSafeFlag = g_xRandomOptionProperty->IsCheckedSafeFlag(pTarget->GetRandomOptItemId());

	if (bCheckedSafeFlag == true)
	{
		g_xRandomOptionProperty->ResetSafeFlag(pTarget->GetRandomOptItemIdPtr());

		g_xRandomOptionProperty->InitializeRandomOption(pTarget->GetNewRandomOptionPtr());
		g_xRandomOptionProperty->GenRandomOption(pTarget->GetNewRandomOptionPtr(), nRandomOptionKind, pTarget->GetProp()->dwParts, true);


		// 		WriteLog( "UserID [%d] : ItemSerialNumber[%d] Protect GenNewRandomOption [%I64d]", (int)( pUser->GetId() ), (int)( pTarget->GetSerialNumber() ), pTarget->GetNewRandomOption() );

#ifndef __FL_FIX_USELESS_LOGS
		TCHAR	szNewOption[128];
		::memset(szNewOption, 0, sizeof(szNewOption));
		::_stprintf(szNewOption, "%I64d", pTarget->GetNewRandomOption());
		g_DPSrvr.PutItemLog(pUser, "z", szNewOption, pTarget, 1);
#endif

		pUser->SendNewRandomOption((BYTE)(pTarget->m_dwObjId), (DWORD)(pTarget->GetSerialNumber()), pTarget->GetNewRandomOption());

	}
	else
	{
		g_xRandomOptionProperty->InitializeRandomOption(pTarget->GetRandomOptItemIdPtr());
		g_xRandomOptionProperty->GenRandomOption(pTarget->GetRandomOptItemIdPtr(), nRandomOptionKind, pTarget->GetProp()->dwParts);
	}


	pUser->UpdateItemEx((BYTE)(pTarget->m_dwObjId), UI_RANDOMOPTITEMID, pTarget->GetRandomOptItemId());

#if __VER >= 12 // __PET_0519

	if (pUser->IsUsing(pTarget)
		&& (nKind == CRandomOptionProperty::eSystemPet || nKind == CRandomOptionProperty::eEatPet))
		pUser->SetDestParamRandomOptExtension(pTarget);
#endif	// __PET_0519

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::AWAKES_COUNT, 1, true);
#endif

#ifndef __FL_FIX_USELESS_LOGS
	// log
	LogItemInfo	log;
	log.Action = szOperation;
	log.SendName = pUser->GetName();
	log.RecvName = szReceive;
	log.WorldId = pUser->GetWorld()->GetID();
	log.Gold = pUser->GetGold();
	log.Gold2 = pUser->GetGold();
	OnLogItem(log, pTarget, 1);
#endif

	return TRUE;
}

BOOL CDPSrvr::DoUseItemTarget_InitializeRandomOption(
	CUser* pUser, CItemElem* pTarget, int nKind,
	int nOk, int nError,
	const char* szOperation, const char* szRecv)
{
	int nRandomOptionKind = g_xRandomOptionProperty->GetRandomOptionKind(pTarget);
	if (nRandomOptionKind == nKind && g_xRandomOptionProperty->GetRandomOptionSize(pTarget->GetRandomOptItemId()) > 0)
	{
#if __VER >= 12 // __PET_0519

		if (pUser->IsUsing(pTarget)
			&& (nKind == CRandomOptionProperty::eSystemPet || nKind == CRandomOptionProperty::eEatPet))
			pUser->ResetDestParamRandomOptExtension(pTarget);
#endif	// __PET_0519


		//g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
#ifdef __PROTECT_AWAKE
		if (pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_AWAKESAFE) == TRUE && nKind == CRandomOptionProperty::eAwakening)
		{
			nOk = TID_GAME_REGARDLESS_USE04;
			g_xRandomOptionProperty->SetSafeFlag(pTarget->GetRandomOptItemIdPtr());

			pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_AWAKESAFE);

#ifndef __FL_FIX_USELESS_LOGS
			g_DPSrvr.PutItemLog(pUser, "z", "USE_AWAKESAVE_ITEM", pTarget, 1);
#endif


		}
		else
		{
			g_xRandomOptionProperty->InitializeRandomOption(pTarget->GetRandomOptItemIdPtr());
		}
#else	//__PROTECT_AWAKE
		g_xRandomOptionProperty->InitializeRandomOption(pTarget->GetRandomOptItemIdPtr());
#endif	//__PROTECT_AWAKE


		pUser->UpdateItemEx((BYTE)(pTarget->m_dwObjId), UI_RANDOMOPTITEMID, pTarget->GetRandomOptItemId());
		pUser->AddText(prj.GetText(nOk), 0xffffffff);

#ifndef __FL_FIX_USELESS_LOGS
		// log
		LogItemInfo	log;
		log.Action = szOperation;
		log.SendName = pUser->GetName();
		log.RecvName = szRecv;
		log.WorldId = pUser->GetWorld()->GetID();
		log.Gold = pUser->GetGold();
		log.Gold2 = pUser->GetGold();
		OnLogItem(log, pTarget, 1);
#endif

		return TRUE;
	}
	pUser->AddDefinedText(nError);
	return FALSE;
}

BOOL CDPSrvr::DoUseItemTarget_ItemLevelDown(CUser* pUser, CItemElem* pMaterial, CItemElem* pTarget)
{
	ItemProp* pProp = pTarget->GetProp();
	if (pProp->dwParts == NULL_ID || pProp->dwLimitLevel1 == 0xFFFFFFFF)
	{
		pUser->AddDefinedText(TID_GAME_INVALID_TARGET_ITEM);
		return FALSE;
	}

	int nLevelDown = pTarget->GetLevelDown();
	if (nLevelDown == 0 || (nLevelDown == -5 && pMaterial->m_dwItemId == II_SYS_SYS_SCR_LEVELDOWN02))
	{
		pTarget->SetLevelDown(pMaterial->m_dwItemId == II_SYS_SYS_SCR_LEVELDOWN01 ? CItemElem::e5LevelDown : CItemElem::e10LevelDown);
		pUser->UpdateItemEx((BYTE)(pTarget->m_dwObjId), UI_RANDOMOPTITEMID, pTarget->GetRandomOptItemId());

#ifndef __FL_FIX_USELESS_LOGS
		// log
		LogItemInfo	log;
		log.Action = "r";
		log.SendName = pUser->GetName();
		log.RecvName = "::ItemLevelDown";
		log.WorldId = pUser->GetWorld()->GetID();
		log.Gold = pUser->GetGold();
		log.Gold2 = pUser->GetGold();
		OnLogItem(log, pTarget, 1);
#endif

		return TRUE;
	}
	else
		pUser->AddDefinedText(TID_GAME_ITEM_LEVELDOWN01);

	return FALSE;
}

void CDPSrvr::OnPickupPetAwakeningCancel(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;
	if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_PET_AWAK_CANCEL, pUser->GetWorld(), pUser->GetPos()))
		return;

	DWORD dwItem;
	ar >> dwItem;
	CItemElem* pItem = pUser->m_Inventory.GetAtId(dwItem);
	if (!IsUsableItem(pItem))
		return;

	if (DoUseItemTarget_InitializeRandomOption(pUser, pItem, CRandomOptionProperty::eEatPet,
		TID_GAME_PICKUP_PET_AWAKENING_CANCEL_S001, TID_GAME_PICKUP_PET_AWAKENING_CANCEL_E001,
		"k", "PPAC"))
	{
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
		if (pUser->IsMode(TRANSPARENT_MODE) == 0)
			g_UserMng.AddCreateSfxObj(pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
	}
}

#ifdef __PET_1024
void CDPSrvr::OnClearPetName(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CPet* pPet = pUser->GetPet();
		if (!pPet)
		{
			pUser->AddDefinedText(TID_GAME_NAME_PET_E00);
			return;
		}
		pPet->SetName("");
		g_UserMng.AddSetPetName(pUser, pPet->GetName());
	}
}
#endif	// __PET_1024

#if __VER >= 13 // __HOUSING
void CDPSrvr::OnHousingSetupFurniture(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	HOUSINGINFO housingInfo;
	housingInfo.Serialize(ar);


	CHousingMng::GetInstance()->ReqSetupFurniture(pUser, housingInfo);
}

void CDPSrvr::OnHousingSetVisitAllow(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	DWORD dwPlayerId;
	BOOL  bAllow;

	ar >> dwPlayerId >> bAllow;

	CHousingMng::GetInstance()->ReqSetAllowVisit(pUser, dwPlayerId, bAllow);
}

void CDPSrvr::OnHousingVisitRoom(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	DWORD dwPlayerId;
	ar >> dwPlayerId;

	if (dwPlayerId == NULL_ID)
		dwPlayerId = pUser->m_idPlayer;

	if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_VISIT_MYROOM, pUser->GetWorld(), pUser->GetPos()))
		return;

	CHousingMng::GetInstance()->SetVisitRoom(pUser, dwPlayerId);
}

void CDPSrvr::OnHousingVisitableList(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CHousingMng::GetInstance()->OnReqVisitableList(pUser);
}

void CDPSrvr::OnHousingGoOut(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CHousingMng::GetInstance()->GoOut(pUser);
}
#endif // __HOUSING

#if __VER >= 13 // __QUEST_HELPER
void CDPSrvr::OnReqQuestNPCPos(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	char szCharKey[64] = { 0, };
	ar.ReadString(szCharKey, 64);

	LPCHARACTER lpChar = prj.GetCharacter(szCharKey);
	if (lpChar)
	{
		if (pUser->GetWorld() && pUser->GetWorld()->GetID() == lpChar->m_dwWorldId)
			pUser->AddNPCPos(lpChar->m_vPos);
		else
			pUser->AddDefinedText(TID_GAME_QUESTINFO_FAIL);
	}
}
#endif // __QUEST_HELPER

#if __VER >= 13 // __COUPLE_1117
void CDPSrvr::OnPropose(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		char szPlayer[MAX_PLAYER] = { 0, };
		ar.ReadString(szPlayer, MAX_PLAYER);
		CCoupleHelper::Instance()->OnPropose(pUser, szPlayer);
	}
}

void CDPSrvr::OnRefuse(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		CCoupleHelper::Instance()->OnRefuse(pUser);
}

void CDPSrvr::OnCouple(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		CCoupleHelper::Instance()->OnCouple(pUser);
}

void CDPSrvr::OnDecouple(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		CCoupleHelper::Instance()->OnDecouple(pUser);
}
#endif	// __COUPLE_1117

#ifdef __MAP_SECURITY
void CDPSrvr::OnMapKey(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		char szFileName[33] = { 0, }, szMapKey[33] = { 0, };
		ar.ReadString(szFileName, 33);
		ar.ReadString(szMapKey, 33);
		g_WorldMng.CheckMapKey(pUser, szFileName, szMapKey);
	}
}
#endif // __MAP_SECURITY

#if __VER >= 15 // __PETVIS
void CDPSrvr::OnRemoveVis(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	int nPos;
	ar >> nPos;

	CItemUpgrade::GetInstance()->RemovePetVisItem(pUser, nPos);
}

void CDPSrvr::OnSwapVis(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	int nPos1, nPos2;
	ar >> nPos1 >> nPos2;

	CItemUpgrade::GetInstance()->SwapVis(pUser, nPos1, nPos2);
}
#endif // __PETVIS

#if __VER >= 15 // __GUILD_HOUSE
void CDPSrvr::OnBuyGuildHouse(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	GuildHouseMng->ReqBuyGuildHouse(pUser);
}

void CDPSrvr::OnGuildHousePacket(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	int nPacketType, nIndex;
	GH_Fntr_Info gfi;

	ar >> nPacketType >> nIndex;
	gfi.Serialize(ar);

	if (nPacketType == GUILDHOUSE_PCKTTYPE_LISTUP)
		return;

	GuildHouseMng->SendWorldToDatabase(pUser, nPacketType, gfi, nIndex);
}

void CDPSrvr::OnGuildHouseEnter(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	GuildHouseMng->EnteranceGuildHouse(pUser);
}

void CDPSrvr::OnGuildHouseGoOut(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	GuildHouseMng->GoOutGuildHouse(pUser);
}
#endif // __GUILD_HOUSE

#if __VER >= 15 // __TELEPORTER
void CDPSrvr::OnTeleporterReq(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		CHAR	m_szKey[64];
		int		nIndex;
		ar.ReadString(m_szKey, 64);
		ar >> nIndex;

		LPCHARACTER lpChar = prj.GetCharacter(m_szKey);
		if (lpChar)
		{
			if (!CNpcChecker::GetInstance()->IsCloseNpc(MMI_TELEPORTER, pUser->GetWorld(), pUser->GetPos()))
				return;

			if ((int)(lpChar->m_vecTeleportPos.size()) <= nIndex)
				return;

			pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, lpChar->m_vecTeleportPos[nIndex], REPLACE_NORMAL, nDefaultLayer);
		}
	}
}
#endif // __TELEPORTER

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CDPSrvr::OnCheckedQuest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		int nQuestid;
		BOOL bCheck;
		ar >> nQuestid >> bCheck;
		if (bCheck)
		{
			if (pUser->m_nCheckedQuestSize >= MAX_CHECKED_QUEST)
				return;

			for (int i = 0; i < pUser->m_nCheckedQuestSize; ++i)
			{
				if (pUser->m_aCheckedQuest[i] == nQuestid)
				{
					for (int j = i; j < pUser->m_nCheckedQuestSize - 1; ++j)
						pUser->m_aCheckedQuest[j] = pUser->m_aCheckedQuest[j + 1];
					pUser->m_aCheckedQuest[--pUser->m_nCheckedQuestSize] = 0;
					break;
				}
			}
			pUser->m_aCheckedQuest[pUser->m_nCheckedQuestSize++] = nQuestid;
		}
		else
		{
			if (pUser->m_nCheckedQuestSize <= 0)
				return;

			for (int i = 0; i < pUser->m_nCheckedQuestSize; ++i)
			{
				if (pUser->m_aCheckedQuest[i] == nQuestid)
				{
					for (int j = i; j < pUser->m_nCheckedQuestSize - 1; ++j)
						pUser->m_aCheckedQuest[j] = pUser->m_aCheckedQuest[j + 1];
					pUser->m_aCheckedQuest[--pUser->m_nCheckedQuestSize] = 0;
					break;
				}
			}
		}
		pUser->AddCheckedQuest();
	}
}
#endif // __IMPROVE_QUEST_INTERFACE


void	CDPSrvr::OnItemSelectAwakeningValue(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pRequest = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pRequest) == TRUE)
	{
		DWORD	dwItemObjID = 0;
		DWORD	dwSerialNumber = 0;
		BYTE	bySelectFlag = 0;

		ar >> dwItemObjID;
		ar >> dwSerialNumber;
		ar >> bySelectFlag;

		pRequest->SelectAwakeningValue(dwItemObjID, dwSerialNumber, bySelectFlag);
	}
	else
	{
		WriteLog("pUser is invalid in OnItemSelectAwakeningValue function.");
	}
}

#ifdef __FL_DUNGEON_TREASURE
void CDPSrvr::OnDungeonTreasureSelect(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld)
		return;

	int nSelected;
	ar >> nSelected;

	if (nSelected < 1 || nSelected > 4)
		return;

	if (pUser->GetTreasureOpenState() == FALSE)
	{
		pUser->AddText("You cant open treasure right now.");
		return;
	}

	for (auto& it : prj.m_vecDungeonTreasures)
	{
		if (it.dwWorldID != pWorld->GetID())
			continue;

		int nRandom = rand() % 4 + 1;
		if (nRandom == nSelected)
		{
			ItemProp* pItemProp = prj.GetItemProp(it.dwItemID);
			if (!pItemProp)
				break;

			CItemElem pItemElem;
			pItemElem.m_dwItemId = pItemProp->dwID;
			pItemElem.m_nItemNum = it.nItemAmount;
			pItemElem.m_nHitPoint = pItemProp->dwEndurance;
			pItemElem.SetSerialNumber();

			if (!pUser->CreateItem(&pItemElem))
				g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, pItemElem, 0, "Dungeon Treasure", "Your dungeon treasure reward!");

			pUser->AddDungeonTreasureResult(nRandom, TRUE);
		}
		else
			pUser->AddDungeonTreasureResult(nRandom, FALSE);

		break;
	}

	pUser->SetTreasureOpenState(FALSE);
}
#endif

#ifdef __FL_CONSUMABLES_STORAGE
void	CDPSrvr::OnMoveItemToChest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	BYTE nItem;
	short nNum;
	BOOL bFromInventoryToChest;
	ar >> nItem >> nNum >> bFromInventoryToChest;

	if (!pUser->m_bAllAction)
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	if (bFromInventoryToChest) // Inventory -> Chest
	{
		CItemElem* pItemElem = pUser->m_Inventory.GetAtId(nItem);
		if (!IsUsableItem(pItemElem))
			return;

		ItemProp* pProp = pItemElem->GetProp();
		if (!pProp)
			return;

		if (pUser->IsUsing(pItemElem))
		{
			pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
			return;
		}

		if (pUser->m_Inventory.IsEquip(nItem))
		{
			pUser->AddDefinedText(TID_GAME_EQUIPTRADE, "");
			return;
		}

		if (nNum > pItemElem->m_nItemNum)
			nNum = pItemElem->m_nItemNum;
		if (nNum < 1)
			nNum = 1;

		if (pUser->m_Chest.IsFull(pItemElem, pProp, nNum) == FALSE)
		{
			CItemElem itemElem;
			itemElem = *pItemElem;
			itemElem.m_nItemNum = nNum;

			pUser->m_Chest.Add(&itemElem); // Add item in chest in server side
			pUser->AddPutItemToChest(&itemElem); // Add item in chest in client side
			pUser->RemoveItem(nItem, nNum); // Remove item from inventory in server & client side
		}
		else
		{
			pUser->AddText("Your chest is full... Not possible to add item!", 0xffffffff);
			return;
		}
	}
	else // Chest -> Inventory
	{
		CItemElem* pItemElem = pUser->m_Chest.GetAtId(nItem);
		if (IsUsableItem(pItemElem) == FALSE)
			return;

		ItemProp* pProp = pItemElem->GetProp();
		if (!pProp)
			return;

		if (nNum > pItemElem->m_nItemNum)
			nNum = pItemElem->m_nItemNum;
		if (nNum < 1)
			nNum = 1;

		if (pUser->m_Inventory.IsFull(pItemElem, pProp, nNum) == FALSE)
		{
			CItemElem itemElem;
			itemElem = *pItemElem;
			itemElem.m_nItemNum = nNum;

			pUser->m_Inventory.Add(&itemElem); // Add item in inventory in server side
			pUser->AddGetItemBank(&itemElem); // Add item in inventory in client side
			pUser->RemoveItem(nItem, nNum, TYPE_CHEST);
		}
		else
		{
			pUser->AddText("Your inventory is full... Not possible to add item!", 0xffffffff);
			return;
		}
	}
}
void CDPSrvr::OnMoveItemInsideChest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nSrcIndex, nDstIndex;
	ar >> nSrcIndex >> nDstIndex;

	if (nSrcIndex == nDstIndex)
		return;
	if (nDstIndex >= MAX_CHEST || nSrcIndex >= MAX_CHEST)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	if (!pUser->m_bAllAction)
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CItemElem* pItemSrc = pUser->m_Chest.GetAt(nSrcIndex);
	CItemElem* pItemDst = pUser->m_Chest.GetAt(nDstIndex);
	if (pItemDst == NULL || IsUsableItem(pItemDst))
	{
		if (IsUsableItem(pItemSrc))
		{
			pUser->m_Chest.Swap(nSrcIndex, nDstIndex);
			pUser->AddMoveItemInsideChest(nSrcIndex, nDstIndex);
		}
	}
}
void CDPSrvr::OnMoveAllItemsToChest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!pUser->m_bAllAction)
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	BOOL bWasSomethingToAdd = FALSE;

	// Step 1: need to be sure what server + client both have "Not in use" state for all suitable items. Client ok, now server:
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		CItemElem* pItemElem = pUser->m_Inventory.GetAt(i);
		if (!pItemElem)
			continue;

		ItemProp* pProp = pItemElem->GetProp();
		if (!pProp)
			continue;

		if (!pUser->CanBeUsedOutsideInventory(pProp))
			continue;

		// UsableItem different for world and client, if user currently on trade/vendor, we should return it
		if (!IsUsableItem(pItemElem) || pUser->IsUsing(pItemElem))
		{
			pUser->AddDefinedText(TID_GAME_BANKNOTRADE);
			return;
		}

		bWasSomethingToAdd = TRUE;
	}

	// Step 2: Its ok, all suitable items is ready to move, thats why we here.
	if (bWasSomethingToAdd)
	{
		bWasSomethingToAdd = FALSE;

		int nNum = 0;
		for (int i = 0; i < MAX_INVENTORY; i++)
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAt(i);
			if (!pItemElem)
				continue;

			ItemProp* pProp = pItemElem->GetProp();
			if (!pProp)
				continue;

			if (!pUser->CanBeUsedOutsideInventory(pProp))
				continue;

			nNum = pItemElem->m_nItemNum;

			if (pUser->m_Chest.IsFull(pItemElem, pProp, nNum))
				continue;

			CItemElem itemElem;
			itemElem = *pItemElem;
			itemElem.m_nItemNum = nNum;

			pUser->m_Inventory.RemoveAtId(pItemElem->m_dwObjId); // Remove item from inventory in server side
			pUser->m_Chest.Add(&itemElem); // Add item in chest in server side

			bWasSomethingToAdd = TRUE;
		}

		if (bWasSomethingToAdd)
			pUser->AddHdr(NULL_ID, SNAPSHOTTYPE_PUT_ALL_ITEMS_TO_CHEST);
	}
}

#endif


#ifdef __TRADESYS
void CDPSrvr::OnExchange(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	int nMMIid;
	int nListNum;

	ar >> nMMIid;
	ar >> nListNum;

	int	nResult = prj.m_Exchange.ResultExchange(pUser, nMMIid, nListNum);
	pUser->AddExchangeResult(EXCHANGE_RESULT, nResult);
}
#endif // __TRADESYS

#ifdef __FL_BOXES_V2
void CDPSrvr::OnBulkBoxOpen(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId;
	ar >> dwId;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwId);
	if (!IsUsableItem(pItemElem))
		return;

	int nBoxNumber = pItemElem->m_nItemNum;
	int nTotalOpenedBoxes = 0;

	PPACKITEMELEM pPackItemElem = CPackItem::GetInstance()->Open(pItemElem->m_dwItemId);
	if (pPackItemElem)
	{
		for (int i = 0; i < nBoxNumber; i++)
		{
			if (pUser->DoUsePackItem(pItemElem, pPackItemElem, TYPE_INVENTORY, true) == FALSE)
				break;
			else
				nTotalOpenedBoxes++;
		}
	}
	else
	{
		for (int i = 0; i < nBoxNumber; i++)
		{
			if (pUser->DoUseGiftbox(pItemElem, TYPE_INVENTORY, true) == FALSE)
				break;
			else
				nTotalOpenedBoxes++;
		}
	}

	if (nTotalOpenedBoxes > 0)
	{
		pUser->RemoveItem((BYTE)(pItemElem->m_dwObjId), nTotalOpenedBoxes);

		CString str; str.Format("Bulk processing completed! Check your inventory. Total - %d", nTotalOpenedBoxes);
		pUser->AddText(str, 0xffffffff);
	}
}
#endif

BOOL CDPSrvr::UnbindScroll(CUser* pUser, CItemElem* pMaterial, CItemElem* pTarget)
{
	if (pUser)
	{
		if (!pTarget || !pTarget->GetProp() || !pMaterial || !pMaterial->GetProp())
			return FALSE;

		if (pTarget->IsFlag(CItemElem::binds) && pMaterial->m_dwItemId == II_SYS_SYS_SCR_CANCELLATION && (pTarget->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND))
		{
			pTarget->ResetFlag(CItemElem::binds);
			pUser->UpdateItem((BYTE)(pTarget->m_dwObjId), UI_FLAG, MAKELONG(pTarget->m_dwObjIndex, pTarget->m_byFlag));
			return TRUE;
		}
		else
			pUser->AddDefinedText(TID_GAME_LORD_SKILL_USE_E001);
	}

	return FALSE;
}

#ifdef __FL_FAST_ELEMENT
void CDPSrvr::OnChangeAttributeNew(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize)
{
	OBJID objScroll;
	int nAttribute, nPosition;

	ar >> objScroll;
	ar >> nAttribute;
	ar >> nPosition;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (nPosition != PARTS_RWEAPON && nPosition != PARTS_LWEAPON && nPosition != PARTS_SHIELD && nPosition != PARTS_UPPER_BODY)
		return;

	CItemElem* pTargetItemElem = pUser->GetEquipItem(nPosition);
	CItemElem* pScrollItemElem = pUser->m_Inventory.GetAtId(objScroll);
	if (!IsUsableItem(pTargetItemElem) || !IsUsableItem(pScrollItemElem))
		return;

	if (pScrollItemElem->m_dwItemId != __FL_FAST_ELEMENT)
		return;

	ItemProp* pTargetProp = pTargetItemElem->GetProp();
	if (!pTargetProp)
		return;

	if (!pTargetProp->IsEleRefineryAble())
		return;

	if (nAttribute >= SAI79::END_PROP || nAttribute <= SAI79::NO_PROP)
		return;

	if (pTargetItemElem->m_bItemResist == SAI79::NO_PROP || pTargetItemElem->m_bItemResist == nAttribute)
	{
		pUser->AddDefinedText(TID_GAME_NOCHANGE_SAME_ATTRIBUTE);
		return;
	}

	if (pTargetItemElem->IsFlag(CItemElem::expired))
	{
		pUser->AddDefinedText(TID_GAME_ITEM_EXPIRED);
		return;
	}

	if (pUser->m_Inventory.IsEquip(pTargetItemElem->m_dwObjId) == FALSE)
	{
		pUser->AddText("Item should be equipped for this action!");
		return;
	}

	if (pTargetItemElem->m_nResistAbilityOption <= 0)
	{
		pUser->AddDefinedText(TID_GAME_NOTELEMENT);
		return;
	}

	pUser->RemoveItem((BYTE)(pScrollItemElem->m_dwObjId), 1);

	((CMover*)pUser)->UpdateElementResist(pTargetItemElem, FALSE);

	pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
	if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
		g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
	pUser->UpdateItem((BYTE)pTargetItemElem->m_dwObjId, UI_IR, nAttribute);

	((CMover*)pUser)->UpdateElementResist(pTargetItemElem, TRUE);

	pUser->RedoEquip(FALSE, FALSE);
	g_UserMng.AddElementChange((CMover*)pUser, nPosition, pTargetItemElem->GetAttrOption());
}
#endif

#ifdef __MODEL_HIDEFASH
void CDPSrvr::OnHideCoat(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	int nPosition;
	BOOL bHide;
	ar >> nPosition >> bHide;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (nPosition < 0 || nPosition > 5)
		return;

	pUser->m_bHideFashion[nPosition] = bHide;
	g_UserMng.AddHideFashionUpdate(pUser, nPosition, bHide);
}
#endif

#ifdef __FL_SHOP_SEARCH
void CDPSrvr::OnRequestShopSearch(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
		pUser->AddShopSearch();
}
void CDPSrvr::OnTeleportToShop(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize)
{
	OBJID objTarget;
	ar >> objTarget;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	DWORD dwState = pUser->GetSummonState(FALSE);
	if (dwState != 0)
	{
		pUser->AddDefinedText(dwState);
		return;
	}

	CMover* pMoverShop = prj.GetMover(objTarget);
	if (!IsValidObj(pMoverShop))
		return;

	CWorld* pMoverShopWorld = pMoverShop->GetWorld();
	if (!pMoverShopWorld)
		return;

	if (pMoverShopWorld->GetID() != WI_WORLD_MADRIGAL)
	{
		pUser->AddText("You can teleport only if selected player located in Madrigal");
		return;
	}

	if (pMoverShop->IsPlayer() && pUser == ((CUser*)pMoverShop))
		return;

	if (!pMoverShop->m_vtInfo.VendorIsVendor() || !pMoverShop->m_vtInfo.IsVendorOpen())
	{
		pUser->AddText("This user not have opened shop. Please, refresh the list.");
		return;
	}

	pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, pMoverShop->GetPos(), REPLACE_FORCE, nDefaultLayer);
}
#endif


#ifdef __FL_PET_FILTER
void	CDPSrvr::OnPetLootOption(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser) == FALSE)
		return;

	int nLootType;
	BYTE nLootPlace;

	ar >> nLootType >> nLootPlace;
	if (nLootPlace >= TYPE_MAX)
		nLootPlace = TYPE_INVENTORY;

	pUser->m_nPetLootType = nLootType;
	pUser->m_nLootPlace = nLootPlace;
}
#endif

#ifdef __FL_EQUALIZED_MODE
void CDPSrvr::OnSelectTmpClass(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (IsValidObj(pUser) == FALSE)
		return;

	if (!pUser->GetWorld())
		return;

	int nTmpClass;
	ar >> nTmpClass;

	DWORD dwWorldID = pUser->GetWorld()->GetID();
	int nTypeOfClass = -1;

	for (auto& it : prj.m_vecTmpWorlds)
	{
		if (it.dwWorldID == dwWorldID)
		{
			if (pUser->TemporalPreCheck(dwWorldID))
				nTypeOfClass = it.nMode;

			break;
		}
	}

	if (nTypeOfClass == -1)
		return;

	if (pUser->bTmpMode)
		return;

	if (pUser->m_vtInfo.GetOther() || pUser->m_vtInfo.VendorIsVendor() || pUser->m_vtInfo.IsVendorOpen() || pUser->m_bBank || pUser->m_bGuildBank)
	{
		pUser->AddText("Please, close bank, guildbank, shop and cancel trade. After that try again..");
		pUser->AddOpenTempWindow(nTypeOfClass);
		return;
	}

	if (nTmpClass >= 0 && nTmpClass <= 7)
	{
		int nJob = 0;
		switch (nTypeOfClass)
		{
		case 0:
		{
			switch (nTmpClass)
			{
			case 0:	nJob = JOB_BLADE;	break;
			case 1:	nJob = JOB_KNIGHT;	break;
			case 2:	nJob = JOB_JESTER;	break;
			case 3:	nJob = JOB_RANGER;	break;
			case 4:	nJob = JOB_PSYCHIKEEPER;	break;
			case 5: nJob = JOB_ELEMENTOR;	break;
			case 6:	nJob = JOB_BILLPOSTER;	break;
			case 7:	nJob = JOB_RINGMASTER;	break;
			}
			break;
		}
		case 1:
		{
			switch (nTmpClass)
			{
			case 0:	nJob = JOB_STORMBLADE_HERO;	break;
			case 1:	nJob = JOB_LORDTEMPLER_HERO;	break;
			case 2:	nJob = JOB_WINDLURKER_HERO;	break;
			case 3:	nJob = JOB_CRACKSHOOTER_HERO;	break;
			case 4:	nJob = JOB_MENTALIST_HERO;	break;
			case 5: nJob = JOB_ELEMENTORLORD_HERO;	break;
			case 6:	nJob = JOB_FORCEMASTER_HERO;	break;
			case 7:	nJob = JOB_FLORIST_HERO;	break;
			}
			break;
		}
		default: Error("Something wrong in CDPSrvr::OnSelectTmpClass: %s", pUser->GetName()); pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6968.0f, 0.0f, 3328.8f), REPLACE_NORMAL, nDefaultLayer); return;
		}

		BOOL bAllowToEnter = FALSE;

		map<u_long, vector<TEMPORAL_LIMITS>>::iterator it = prj.map_DynamicTemporalCounter.find(dwWorldID);
		if (it != prj.map_DynamicTemporalCounter.end())
		{
			for (auto& it2 : it->second) // check "dynamic" vector inside map
			{
				if (it2.nJob == nTmpClass)
				{
					for (auto& it3 : prj.m_vecTmpWorlds) // check "static" vector to know limit
					{
						if (it3.dwWorldID == dwWorldID)
						{
							if (it2.nCurrentAmountOfPlayers >= it3.nMaxJob[nTmpClass])
								bAllowToEnter = FALSE;
							else
								bAllowToEnter = TRUE;

							break;
						}
					}

					break;
				}
			}
		}

		if (!bAllowToEnter)
		{
			pUser->AddText("Too much same jobs for this world, please, select another job.");
			pUser->AddOpenTempWindow(nTypeOfClass);
			return;
		}

		((CMover*)pUser)->TemporalEnter(nTypeOfClass, nJob);
	}
}
#endif

void	CDPSrvr::OnConfirmPartyRequest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	u_long uLeaderPlayerID;
	ar >> uLeaderPlayerID;

	CUser* pMember = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pMember))
		return;

	CUser* pLeader = g_UserMng.GetUserByPlayerID(uLeaderPlayerID);
	if (!IsValidObj(pLeader))
		return;

	if (pLeader == pMember)
		return;

#ifdef __FL_FIX_IDCHANGE_EXPLOIT
	BOOL bAllowed = FALSE;
	for (auto it = pLeader->vecPartyInvites.begin(); it != pLeader->vecPartyInvites.end(); it++)
	{
		if (it->dwInvieMember == pMember->m_idPlayer)
		{
			pLeader->vecPartyInvites.erase(it);
			bAllowed = TRUE;
			break;
		}
	}
	if (!bAllowed)
	{
		pMember->AddText("This invite was expired or you have not permissions to join.", 0xffffffff);
		return;
	}
#endif

	const DWORD dwResult = g_PartyMng.CanInviteParty(uLeaderPlayerID, pMember->m_idPlayer);
	if (dwResult == 5)
	{
		g_PartyMng.RequestAddPartyMemberToCS(uLeaderPlayerID, pMember->m_idPlayer);
	}
	else
	{
		switch (dwResult)
		{
		case 1:
			pLeader->AddDefinedText(TID_GAME_PARTYEXISTCHR, "\"%s\"", pMember->GetName());
			break;

		case 2:
			pLeader->AddDefinedText(TID_GAME_PARTYNOINVATE);
			break;

		case 3:
			pLeader->AddDefinedText(TID_GAME_FULLPARTY3);
			break;

		case 4:
			pLeader->AddDefinedText(TID_GAME_GUILDCOMBAT_CANNOT_PARTY);
			break;

		default:
			break;
		}
	}
}


void	CDPSrvr::OnConfirmFriendRequest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	u_long uLeaderPlayerID;
	ar >> uLeaderPlayerID;

	CUser* pMember = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pMember))
		return;

	CUser* pLeader = g_UserMng.GetUserByPlayerID(uLeaderPlayerID);
	if (!IsValidObj(pLeader))
		return;

	if (pLeader == pMember)
		return;

	if (pLeader->m_RTMessenger.GetFriend(pMember->m_idPlayer))
		return;

	BOOL bFullA = pMember->m_RTMessenger.size() >= MAX_FRIEND;
	BOOL bFullB = pLeader->m_RTMessenger.size() >= MAX_FRIEND;
	if (bFullA)
		return;

	if (bFullB)
		return;

#ifdef __FL_FIX_IDCHANGE_EXPLOIT
	BOOL bAllowed = FALSE;
	for (auto it = pLeader->vecFriendInvites.begin(); it != pLeader->vecFriendInvites.end(); it++)
	{
		if (it->dwInvieMember == pMember->m_idPlayer)
		{
			pLeader->vecFriendInvites.erase(it);
			bAllowed = TRUE;
			break;
		}
	}
	if (!bAllowed)
	{
		pMember->AddText("This invite was expired or you have not permissions to add.", 0xffffffff);
		return;
	}
#endif

	BEFORESENDDUAL(out, PACKETTYPE_ADDFRIEND, DPID_UNKNOWN, DPID_UNKNOWN);
	out << uLeaderPlayerID;
	out << pMember->m_idPlayer;
	SEND(out, &g_DPCoreClient, DPID_SERVERPLAYER);

}

void	CDPSrvr::OnConfirmGuildRequest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	u_long idMaster;
	ar >> idMaster;

	CUser* pMember = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pMember))
		return;

	CUser* pLeader = g_UserMng.GetUserByPlayerID(idMaster);
	if (!IsValidObj(pLeader))
		return;

	if (pLeader == pMember)
		return;

	CGuild* pGuild = g_GuildMng.GetGuild(pLeader->m_idGuild);
	if (!pGuild || !pGuild->IsMember(idMaster))
		return;

	if (!IsInviteAbleGuild(pLeader->m_idGuild, pLeader))
		return;

	CGuild* pGuildtmp = g_GuildMng.GetGuild(pMember->m_idGuild);
	if (pGuildtmp && pGuildtmp->IsMember(pMember->m_idPlayer))
		return;

	if (pGuild->GetSize() >= pGuild->GetMaxMemberSize())
		return;

#ifdef __FL_FIX_IDCHANGE_EXPLOIT
	BOOL bAllowed = FALSE;
	for (auto it = pLeader->vecGuildInvites.begin(); it != pLeader->vecGuildInvites.end(); it++)
	{
		if (it->dwInvieMember == pMember->m_idPlayer)
		{
			pLeader->vecGuildInvites.erase(it);
			bAllowed = TRUE;
			break;
		}
	}
	if (!bAllowed)
	{
		pMember->AddText("This invite was expired or you have not permissions to join.", 0xffffffff);
		return;
	}
#endif


	BEFORESENDDUAL(out, PACKETTYPE_ADD_GUILD_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN);
	out << pMember->m_idPlayer << pGuild->m_idGuild; //__FL_GUILD_FINDER
	SEND(out, &g_DPCoreClient, DPID_SERVERPLAYER);

}

#ifdef __FL_GW_STATUE_V2
void	CDPSrvr::OnConfirmStatueSettings(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	int nStatueType;
	int nStatuePrice;

	ar >> nStatueType;
	ar >> nStatuePrice;

	CMover* pStatue = NULL;
	if (nStatueType == NPC_TYPE_MVPSTATUE)
		pStatue = prj.GetMover(prj.GetCurrentMvpStatueId());
	else if (nStatueType == NPC_TYPE_GUILDLEADERSTATUE)
		pStatue = prj.GetMover(prj.GetCurrentLeaderStatueId());

	if (!IsValidObj(pStatue))
	{
		pUser->AddText("Statue not exists!", 0xffffffff);
		return;
	}

	if (pUser->m_idPlayer != pStatue->GetNpcOwner())
	{
		pUser->AddText("It's not your statue!", 0xffffffff);
		return;
	}

	int nMaxCost = 2000000000;
	if (nStatuePrice < 1 || nStatuePrice > nMaxCost)
	{
		pUser->AddText("Incorrect price!", 0xffffffff);
		return;
	}

	BEFORESENDDUAL(out, PACKETTYPE_STATUE_SETTINGS, DPID_UNKNOWN, DPID_UNKNOWN);
	out << nStatueType;
	out << nStatuePrice;
	out << pUser->m_idPlayer;
	SEND(out, &g_DPCoreClient, DPID_SERVERPLAYER);

}
void CDPSrvr::OnRequestStatueParams(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	BOOL bIsMvp;
	ar >> bIsMvp;

	int nCost = 0;
	if (bIsMvp)
		nCost = prj.GetCurrentMvpStatuePrice();
	else
		nCost = prj.GetCurrentLeaderStatuePrice();

	pUser->AddStatueParams(nCost);
}
void CDPSrvr::OnReceiveStatueBuff(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	BOOL bIsMvp;
	int nClientSidedPrice;

	ar >> bIsMvp;
	ar >> nClientSidedPrice;

	g_GuildCombatMng.ApplyStatueBuff(pUser, bIsMvp, nClientSidedPrice);
}
#endif

#ifdef __FL_FFA
void CDPSrvr::OnRequestFFA(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	pUser->AddFFAData();
}
void CDPSrvr::OnRequestFFARegister(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	CFFAManager::GetInstance()->RegisterPlayer(pUser);
}
void CDPSrvr::OnRequestFFAJoin(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	if (!CFFAManager::GetInstance()->JoinFFAWorld(pUser))
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT1TO1_NOENTRANCETIME);
}
#endif

#ifdef __FL_DAILY_REWARDS
void CDPSrvr::OnCollectDailyGift(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	if (pUser->m_nLastDailyReward < 0)
		return;

	auto it = prj.mapDailyRewards.find(pUser->m_nLastDailyReward);
	if (it == prj.mapDailyRewards.end())
		return;

	ItemProp* pRewardProp = prj.GetItemProp(it->second.dwItemID);
	if (!pRewardProp)
		return;

	CItemElem pItemElem;
	pItemElem.m_dwItemId = it->second.dwItemID;
	pItemElem.m_nItemNum = it->second.nItemNum;
	pItemElem.m_nHitPoint = pRewardProp->dwEndurance;
	if (it->second.bItemBind)
		pItemElem.SetFlag(CItemElem::binds);
	pItemElem.SetSerialNumber();

	if (pUser->CreateItem(&pItemElem) == FALSE)
		g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, pItemElem, 0, "Daily Gift", "Your daily gift!");

	CString str;
	str.Format("You've successfully received reward for Day %d!", pUser->m_nLastDailyReward + 1);
	pUser->AddText(str, 0xffffffff);

	g_dpDBClient.SendUpdateDailyGift(pUser->m_idPlayer, pUser->m_nLastDailyReward + 1);

	pUser->m_nLastDailyReward = -1;
}
#endif

#ifdef __WIKI_ITEMS_V2
void CDPSrvr::OnWikiCreate(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	if (pUser->IsAuthHigher(AUTH_ADMINISTRATOR) == FALSE)
		return;

	BOOL dwId;
	int nMode;

	ar >> dwId >> nMode;

	ItemProp* pProp = prj.GetItemProp(dwId);
	if (!pProp)
		return;

	int nAmount = 1;
	switch (nMode)
	{
	case 0: nAmount = 1; break;
	case 1: nAmount = 100; break;
	case 2: nAmount = 1000; break;
	case 3: nAmount = pProp->dwPackMax; break;
	default: return;
	}

	if (nAmount > pProp->dwPackMax)
		nAmount = pProp->dwPackMax;

	CItemElem itemElem;
	itemElem.m_dwItemId = dwId;
	itemElem.m_nItemNum = nAmount;
	itemElem.SetSerialNumber();
	itemElem.m_nHitPoint = 0;

	if (!pUser->CreateItem(&itemElem))
		pUser->AddDefinedText(TID_GAME_LACKSPACE);
}
#endif

#ifdef __MODEL_CHANGE
void CDPSrvr::OnLookChange(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	CItemElem* pDest, * pSrc;
	DWORD dwIdSrc, dwIdDest;
	ar >> dwIdSrc >> dwIdDest;

	pSrc = pUser->m_Inventory.GetAtId(dwIdSrc);
	pDest = pUser->m_Inventory.GetAtId(dwIdDest);

	if (!IsUsableItem(pSrc) || !IsUsableItem(pDest))
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE06);
		return;
	}

	if (pUser->IsUsing(pSrc) || pUser->IsUsing(pDest))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPPUT);
		return;
	}

	if (pSrc->IsChangedLook() || pDest->IsChangedLook())
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE01);
		return;
	}
	if (pUser->m_Inventory.IsEquip(pSrc->m_dwObjId) || pUser->m_Inventory.IsEquip(pDest->m_dwObjId))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPPUT);
		return;
	}

	if (pSrc->m_dwItemId == pDest->m_dwItemId)
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE08);
		return;
	}

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	ItemProp* iPropSrc = pSrc->GetProp();
	ItemProp* iPropDest = pDest->GetProp();
	if (!iPropSrc || !iPropDest)
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE07);
		return;
	}

	switch (pSrc->m_dwItemId)
	{
	case II_ARM_M_CHR_MAJESTY01HAT:
	case II_ARM_M_CHR_MAJESTY01SUIT:
	case II_ARM_M_CHR_MAJESTY01GLOVES:
	case II_ARM_M_CHR_MAJESTY01SHOES:
	case II_ARM_S_CLO_CLO_MAJESTY:
	case II_ARM_F_CHR_MAJESTY01HAT:
	case II_ARM_F_CHR_MAJESTY01SUIT:
	case II_ARM_F_CHR_MAJESTY01GLOVES:
	case II_ARM_F_CHR_MAJESTY01SHOES:
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE08);
		return;
	}
	}
	switch (pDest->m_dwItemId)
	{
	case II_ARM_M_CHR_MAJESTY01HAT:
	case II_ARM_M_CHR_MAJESTY01SUIT:
	case II_ARM_M_CHR_MAJESTY01GLOVES:
	case II_ARM_M_CHR_MAJESTY01SHOES:
	case II_ARM_S_CLO_CLO_MAJESTY:
	case II_ARM_F_CHR_MAJESTY01HAT:
	case II_ARM_F_CHR_MAJESTY01SUIT:
	case II_ARM_F_CHR_MAJESTY01GLOVES:
	case II_ARM_F_CHR_MAJESTY01SHOES:
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE08);
		return;
	}
	}

	const bool bSexSame = iPropDest->dwItemSex != NULL_ID
		? (iPropDest->dwItemSex == iPropSrc->dwItemSex || iPropSrc->dwItemSex == NULL_ID)
		: (iPropDest->dwItemSex == iPropSrc->dwItemSex);

	if (bSexSame == false)
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE07);
		return;
	}

	if (iPropDest->dwItemKind3 != IK3_SHIELD && iPropDest->dwItemKind3 != IK3_MAGICBARUNA && iPropDest->dwItemKind3 != IK3_ZEMBARUNA &&
		iPropSrc->dwItemKind3 != IK3_SHIELD && iPropSrc->dwItemKind3 != IK3_MAGICBARUNA && iPropSrc->dwItemKind3 != IK3_ZEMBARUNA)
	{
		if ((iPropDest->dwItemKind3 != iPropSrc->dwItemKind3) || (iPropDest->dwHanded != iPropSrc->dwHanded))
		{
			pUser->AddDefinedText(TID_MMI_LOOKCHANGE07);
			return;
		}
	}

	if (pSrc->m_dwKeepTime != 0 || pDest->m_dwKeepTime != 0)
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE08);
		return;
	}

	pDest->SetLook(pSrc->m_dwItemId);
	pUser->RemoveItem((BYTE)pSrc->m_dwObjId, 1);
	pUser->AddLookChange(pDest->m_dwObjId, pDest->GetLook());
	pUser->AddDefinedText(TID_MMI_LOOKCHANGE04);
}
#endif

#ifdef __FL_TELEPORT
void CDPSrvr::OnTeleportRequest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	int nKey;
	ar >> nKey;

	CTeleporter::GetInstance()->DoTeleport(nKey, pUser);
}
#endif

#ifdef __JOB_CHANGE
void CDPSrvr::OnSendJobChange(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	int nJob;
	ar >> nJob;

	if (!IsValidObj(pUser))
		return;

	if (pUser->GetLevel() == MAX_JOB_LEVEL && pUser->IsBaseJob())
	{
		switch (nJob)
		{
		case JOB_MERCENARY: pUser->ChangeJob(JOB_MERCENARY); break;
		case JOB_ASSIST: pUser->ChangeJob(JOB_ASSIST); break;
		case JOB_ACROBAT: pUser->ChangeJob(JOB_ACROBAT); break;
		case JOB_MAGICIAN: pUser->ChangeJob(JOB_MAGICIAN); break;
		default: Error("[Warning] CDPSrvr::OnSendJobChange (1) - %s", pUser->GetName()); break;
		}
	}
	else if (pUser->GetLevel() == (MAX_JOB_LEVEL + MAX_EXP_LEVEL) && pUser->IsExpert())
	{
		switch (nJob)
		{
		case JOB_KNIGHT: if (pUser->GetJob() == JOB_MERCENARY) pUser->ChangeJob(JOB_KNIGHT); break;
		case JOB_BLADE: if (pUser->GetJob() == JOB_MERCENARY) pUser->ChangeJob(JOB_BLADE); break;
		case JOB_JESTER: if (pUser->GetJob() == JOB_ACROBAT) pUser->ChangeJob(JOB_JESTER); break;
		case JOB_RANGER: if (pUser->GetJob() == JOB_ACROBAT) pUser->ChangeJob(JOB_RANGER); break;
		case JOB_RINGMASTER: if (pUser->GetJob() == JOB_ASSIST) pUser->ChangeJob(JOB_RINGMASTER); break;
		case JOB_BILLPOSTER: if (pUser->GetJob() == JOB_ASSIST) pUser->ChangeJob(JOB_BILLPOSTER); break;
		case JOB_PSYCHIKEEPER: if (pUser->GetJob() == JOB_MAGICIAN) pUser->ChangeJob(JOB_PSYCHIKEEPER); break;
		case JOB_ELEMENTOR: if (pUser->GetJob() == JOB_MAGICIAN) pUser->ChangeJob(JOB_ELEMENTOR); break;
		default: Error("[Warning] CDPSrvr::OnSendJobChange (2) - %s", pUser->GetName()); break;
		}
	}
	else if (pUser->GetLevel() == MAX_GENERAL_LEVEL && pUser->IsPro() && pUser->GetExpPercent() >= 9999)
	{
		switch (nJob)
		{
		case JOB_KNIGHT_MASTER: if (pUser->GetJob() == JOB_KNIGHT) pUser->ChangeJob(JOB_KNIGHT_MASTER); break;
		case JOB_BLADE_MASTER: if (pUser->GetJob() == JOB_BLADE) pUser->ChangeJob(JOB_BLADE_MASTER); break;
		case JOB_JESTER_MASTER: if (pUser->GetJob() == JOB_JESTER) pUser->ChangeJob(JOB_JESTER_MASTER); break;
		case JOB_RANGER_MASTER: if (pUser->GetJob() == JOB_RANGER) pUser->ChangeJob(JOB_RANGER_MASTER); break;
		case JOB_RINGMASTER_MASTER: if (pUser->GetJob() == JOB_RINGMASTER) pUser->ChangeJob(JOB_RINGMASTER_MASTER); break;
		case JOB_BILLPOSTER_MASTER: if (pUser->GetJob() == JOB_BILLPOSTER) pUser->ChangeJob(JOB_BILLPOSTER_MASTER); break;
		case JOB_PSYCHIKEEPER_MASTER: if (pUser->GetJob() == JOB_PSYCHIKEEPER) pUser->ChangeJob(JOB_PSYCHIKEEPER_MASTER); break;
		case JOB_ELEMENTOR_MASTER: if (pUser->GetJob() == JOB_ELEMENTOR) pUser->ChangeJob(JOB_ELEMENTOR_MASTER); break;
		default: Error("[Warning] CDPSrvr::OnSendJobChange (3) - %s", pUser->GetName()); break;
		}
	}
	else if (pUser->GetLevel() == MAX_GENERAL_LEVEL && pUser->IsMaster() && pUser->GetExpPercent() >= 9999)
	{
		switch (nJob)
		{
		case JOB_KNIGHT_HERO: if (pUser->GetJob() == JOB_KNIGHT_MASTER) pUser->ChangeJob(JOB_KNIGHT_HERO); break;
		case JOB_BLADE_HERO: if (pUser->GetJob() == JOB_BLADE_MASTER) pUser->ChangeJob(JOB_BLADE_HERO); break;
		case JOB_JESTER_HERO: if (pUser->GetJob() == JOB_JESTER_MASTER) pUser->ChangeJob(JOB_JESTER_HERO); break;
		case JOB_RANGER_HERO: if (pUser->GetJob() == JOB_RANGER_MASTER) pUser->ChangeJob(JOB_RANGER_HERO); break;
		case JOB_RINGMASTER_HERO: if (pUser->GetJob() == JOB_RINGMASTER_MASTER) pUser->ChangeJob(JOB_RINGMASTER_HERO); break;
		case JOB_BILLPOSTER_HERO: if (pUser->GetJob() == JOB_BILLPOSTER_MASTER) pUser->ChangeJob(JOB_BILLPOSTER_HERO); break;
		case JOB_PSYCHIKEEPER_HERO: if (pUser->GetJob() == JOB_PSYCHIKEEPER_MASTER) pUser->ChangeJob(JOB_PSYCHIKEEPER_HERO); break;
		case JOB_ELEMENTOR_HERO: if (pUser->GetJob() == JOB_ELEMENTOR_MASTER) pUser->ChangeJob(JOB_ELEMENTOR_HERO); break;
		default: Error("[Warning] CDPSrvr::OnSendJobChange (4) - %s", pUser->GetName()); break;
		}
	}
	else if (pUser->GetLevel() == MAX_LEGEND_LEVEL && pUser->IsHero())
	{
		switch (nJob)
		{
		case JOB_LORDTEMPLER_HERO: if (pUser->GetJob() == JOB_KNIGHT_HERO) pUser->ChangeJob(JOB_LORDTEMPLER_HERO); break;
		case JOB_STORMBLADE_HERO: if (pUser->GetJob() == JOB_BLADE_HERO) pUser->ChangeJob(JOB_STORMBLADE_HERO); break;
		case JOB_WINDLURKER_HERO: if (pUser->GetJob() == JOB_JESTER_HERO) pUser->ChangeJob(JOB_WINDLURKER_HERO); break;
		case JOB_CRACKSHOOTER_HERO: if (pUser->GetJob() == JOB_RANGER_HERO) pUser->ChangeJob(JOB_CRACKSHOOTER_HERO); break;
		case JOB_FLORIST_HERO: if (pUser->GetJob() == JOB_RINGMASTER_HERO) pUser->ChangeJob(JOB_FLORIST_HERO); break;
		case JOB_FORCEMASTER_HERO: if (pUser->GetJob() == JOB_BILLPOSTER_HERO) pUser->ChangeJob(JOB_FORCEMASTER_HERO); break;
		case JOB_MENTALIST_HERO: if (pUser->GetJob() == JOB_PSYCHIKEEPER_HERO) pUser->ChangeJob(JOB_MENTALIST_HERO); break;
		case JOB_ELEMENTORLORD_HERO: if (pUser->GetJob() == JOB_ELEMENTOR_HERO) pUser->ChangeJob(JOB_ELEMENTORLORD_HERO); break;
		default: Error("[Warning] CDPSrvr::OnSendJobChange (5) - %s", pUser->GetName()); break;
		}
	}
}
#endif

#ifdef __FL_RECYCLE
void CDPSrvr::OnRecycle(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwItem;
	ar >> dwItem;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwItem);
	if (!IsUsableItem(pItemElem))
		return;

	auto it = prj.mapRecycle.find(pItemElem->m_dwItemId);
	if (it == prj.mapRecycle.end())
		return;

	if (pUser->IsUsing(pItemElem))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPPUT);
		return;
	}

	if (pUser->m_Inventory.IsEquip(dwItem))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPPUT);
		return;
	}

	ItemProp* pRewardProp = prj.GetItemProp(__FL_RECYCLE);
	if (!pRewardProp)
		return;

	CItemElem pItemReward;
	pItemReward.m_dwItemId = pRewardProp->dwID;
	pItemReward.m_nItemNum = it->second;
	pItemReward.m_nHitPoint = pRewardProp->dwEndurance;
	pItemReward.SetSerialNumber();

	if (pUser->CreateItem(&pItemReward))
	{
		pUser->RemoveItem((BYTE)dwItem, 1);
		pUser->AddText("Recycling is done, check your inventory!");
	}
	else
		pUser->AddDefinedText(TID_GAME_LACKSPACE);
}
#endif

#ifdef __FL_PARTY_FINDER
void CDPSrvr::OnPartyFinderRequest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	g_PartyMng.AddPartyList(pUser);
}
void CDPSrvr::OnPartyFinderJoin(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long uPartyId;
	ar >> uPartyId;

	CUser* pMember = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pMember))
		return;

	CParty* pParty = g_PartyMng.GetParty(uPartyId);
	if (!pParty || !pParty->m_bAllowed)
	{
		pMember->AddDefinedText(TID_GAME_NOPARTY);
		return;
	}

	CParty* pMemberParty = g_PartyMng.GetParty(pMember->m_idparty);
	if (pMemberParty)
	{
		pMember->AddText("You already in party.", 0xffffffff);
		return;
	}

	if (pParty->GetSizeofMember() >= MAX_PTMEMBER_SIZE_SPECIAL)
	{
		pMember->AddDefinedText(TID_DIAG_0007);
		return;
	}

	g_PartyMng.RequestAddPartyMemberToCS(pParty->m_aMember[0].m_uPlayerId, pMember->m_idPlayer);
}
#endif

#ifndef __FL_VENDORS
void CDPSrvr::OnRegisterPVendorItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE iIndex, nType, nId;
	short nNum;
	int nCost;
	ar >> iIndex >> nType >> nId >> nNum >> nCost;

	if (iIndex >= MAX_VENDOR_REVISION)
		return;

	if (nCost < 1)
		nCost = 1;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_vtInfo.GetOther())
			return;
		if (pUser->m_vtInfo.IsVendorOpen())
			return;
#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif
		if (pUser->IsRegionAttr(RA_FIGHT))
		{
			pUser->AddDefinedText(TID_GAME_FAIL_TO_OPEN_SHOP);
			return;
		}

		if (pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM)
			return;

		if (CNpcChecker::GetInstance()->IsCloseNpc(pUser->GetWorld(), pUser->GetPos()))
		{
			pUser->AddDiagText(prj.GetText(TID_GAME_NPC_RADIUS));
			return;
		}

		CItemElem* pItemElem = pUser->m_Inventory.GetAtId(nId);
		if (IsUsableItem(pItemElem))
		{
			if (pItemElem->IsQuest())
				return;

			if (pItemElem->IsBinds())
				return;
			if (pUser->IsUsing(pItemElem))
			{
				pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
				return;
			}

			ItemProp* pProp = pItemElem->GetProp();

			if (pUser->m_Inventory.IsEquip(pItemElem->m_dwObjId))
				return;

			if (pItemElem->IsFlag(CItemElem::expired))
				return;

			if (nNum > pItemElem->m_nItemNum)
				nNum = pItemElem->m_nItemNum;
			if (nNum < 1)
				nNum = 1;

			pUser->m_vtInfo.VendorSetItem(nId, iIndex, nNum, nCost);
			pUser->AddRegisterPVendorItem(iIndex, 0, nId, nNum, nCost);
		}
	}
}

void CDPSrvr::OnUnregisterPVendorItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE i;
	ar >> i;

	if (i >= MAX_VENDITEM)
		return;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (pUser->m_vtInfo.GetOther())
			return;
		if (pUser->m_vtInfo.IsVendorOpen())
			return;

		if (pUser->m_vtInfo.VendorClearItem(i))
			pUser->AddUnregisterPVendorItem(i);
	}
}
void CDPSrvr::OnPVendorOpen(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (uBufSize > 55)
		return;

	char szPVendor[MAX_VENDORNAME];
	ar.ReadString(szPVendor, MAX_VENDORNAME);

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsValidObj(pUser))
	{
		if (_tcslen(szPVendor) < 6 || _tcslen(szPVendor) > 32)
		{
			pUser->AddDefinedText(TID_DIAG_0011);
			return;
		}

		if (!(pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
			ParsingEffect(szPVendor, strlen(szPVendor));

		CWorld* pWorld = pUser->GetWorld();
		if (!pWorld)
			return;

		if (pWorld->GetID() == WI_WORLD_GUILDWAR)
		{
			pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE));
			return;
		}

		if (pWorld->GetID() == WI_WORLD_MINIROOM)
			return;

		if (pUser->m_vtInfo.GetOther())
			return;

		if (pUser->IsAttackMode())
			return;

#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode)
		{
			pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
			return;
		}
#endif
		if (pUser->m_pActMover->IsFly())
			return;

#ifdef __S_SERVER_UNIFY
		if (pUser->m_bAllAction == FALSE)
			return;
#endif // __S_SERVER_UNIFY

		pUser->m_vtInfo.SetTitle(szPVendor);

		if (pUser->m_vtInfo.IsVendorOpen())
		{
			if (pUser->m_vtInfo.VendorIsVendor())
			{
#ifdef __FL_SHOP_SEARCH
				prj.AddToShopSearch(pUser->GetId());
#endif

				g_ChattingMng.NewChattingRoom(pUser->m_idPlayer);
				CChatting* pChatting = g_ChattingMng.GetChttingRoom(pUser->m_idPlayer);

				g_UserMng.AddPVendorOpen(pUser);
				if (pChatting)
				{
					pChatting->m_bState = TRUE;
					if (pChatting->AddChattingMember(pUser->m_idPlayer))
						pUser->m_idChatting = pUser->m_idPlayer;
					pUser->AddNewChatting(pChatting);
				}
				pUser->m_dwHonorCheckTime = GetTickCount();

			}
		}
	}
}
#else 
void CDPSrvr::OnRegisterPVendorItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE iIndex, nType, nId;
	short nNum;
	int nCost;
	int nPerinCost;
	BOOL bOwnShop;

	ar >> iIndex >> nType >> nId >> nNum >> nCost >> nPerinCost;
	ar >> bOwnShop;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!pUser->GetWorld())
		return;

	if (bOwnShop && (pUser->m_vtInfo.IsVendorOpen() || pUser->m_vtInfo.GetOther()))
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(nId);
	if (!IsUsableItem(pItemElem))
		return;

	if (pUser->m_Inventory.IsEquip(pItemElem->m_dwObjId) || pUser->IsUsing(pItemElem))
		return;

	if (pItemElem->IsFlag(CItemElem::expired))
		return;

	if (pItemElem->IsQuest() || pItemElem->IsBinds())
		return;

	if (iIndex >= MAX_VENDOR_REVISION)
		return;

	if (nPerinCost < 0)
		nPerinCost = 0;
	if (nPerinCost > 9999)
		nPerinCost = 9999;

	if (nCost > PERIN_VALUE - 1)
		nCost = PERIN_VALUE - 1;
	if (nCost < 1 && nPerinCost < 1)
		nCost = 1;

	if (nNum > pItemElem->m_nItemNum)
		nNum = pItemElem->m_nItemNum;
	if (nNum < 1)
		nNum = 1;

	if (bOwnShop) // Own private shop
	{
		if (pUser->m_vtInfo.GetItem(iIndex))
			return;

		pUser->m_vtInfo.VendorSetItem(nId, iIndex, nNum, nCost, nPerinCost);
		pUser->AddRegisterPVendorItem(iIndex, 0, nId, nNum, nCost, nPerinCost);
	}
	else
	{
		auto it = prj.m_mapPlayerShops.find(pUser->m_idPlayer);
		if (it == prj.m_mapPlayerShops.end())
		{
			pUser->AddText("You have not activated vendor.", 0xffffffff);
			return;
		}

		CMover* pMoverVendor = prj.GetMover(it->second);
		if (!IsValidObj(pMoverVendor))
		{
			pUser->AddText("Invalid vendor.", 0xffffffff);
			return;
		}

		if (!pMoverVendor->m_vtInfo.IsVendorOpen())
			return;

		if (pMoverVendor->m_vtInfo.GetItem(iIndex))
			return;

		// Remove from player, add to vendor
		pMoverVendor->m_vtInfo.PlayerToVendorSingle(pUser, pItemElem, iIndex, nNum, nCost, nPerinCost);

		// Save vendor, save player (TODO: delayed saving)
		g_dpDBClient.SendUpdateVendor(pMoverVendor);
		g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

		// Add in clients side
		g_UserMng.AddNewItemToLiveVendor(pMoverVendor, iIndex);
		pUser->AddVendorPreview(pMoverVendor);
	}
}

void CDPSrvr::OnUnregisterPVendorItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE i;
	BOOL bOwnShop;

	ar >> i;
	ar >> bOwnShop;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!pUser->GetWorld())
		return;

	if (bOwnShop && (pUser->m_vtInfo.IsVendorOpen() || pUser->m_vtInfo.GetOther()))
		return;

	if (i >= MAX_VENDITEM)
		return;

	if (bOwnShop) // Own private shop
	{
		if (pUser->m_vtInfo.VendorClearItem(i))
			pUser->AddUnregisterPVendorItem(i);
	}
	else // Own vendor shop
	{
		auto it = prj.m_mapPlayerShops.find(pUser->m_idPlayer);
		if (it == prj.m_mapPlayerShops.end())
		{
			pUser->AddText("You have not activated vendor.", 0xffffffff);
			return;
		}

		CMover* pMoverVendor = prj.GetMover(it->second);
		if (!IsValidObj(pMoverVendor))
		{
			pUser->AddText("Invalid vendor.", 0xffffffff);
			return;
		}

		if (!pMoverVendor->m_vtInfo.IsVendorOpen())
			return;

		if (!pMoverVendor->m_vtInfo.GetItem(i))
		{
			pUser->AddVendorPreview(pMoverVendor);
			return;
		}

		// Remove from vendor, add to player
		pMoverVendor->m_vtInfo.VendorToPlayerSingle(pUser, i);

		// Save vendor, save player (TODO: delayed saving)
		g_dpDBClient.SendUpdateVendor(pMoverVendor);
		g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

		// Remove in clients side
		g_UserMng.AddPVendorItemNum((CUser*)pMoverVendor, i, 0, "");
		pUser->AddVendorPreview(pMoverVendor);
	}
}
void CDPSrvr::OnPVendorOpen(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if (uBufSize > 55)
		return;

	BOOL bOwnShop;
	DWORD dwObjId;

	ar >> bOwnShop;
	ar >> dwObjId;

	char szPVendor[MAX_VENDORNAME];
	ar.ReadString(szPVendor, MAX_VENDORNAME);

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (_tcslen(szPVendor) < 6 || _tcslen(szPVendor) > 32)
	{
		pUser->AddDefinedText(TID_DIAG_0011);
		return;
	}

	if (!(pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
		ParsingEffect(szPVendor, strlen(szPVendor));

	CWorld* pWorld = pUser->GetWorld();
	if (!pWorld || pWorld->GetID() != WI_WORLD_MADRIGAL)
	{
		pUser->AddText("You can open private shop only in Madrigal!", 0xffffffff);
		return;
	}

	/*	const BYTE nCont = CContinent::GetInstance()->GetContinent(pUser);
		if (nCont != CONT_SAINTMORNING)
		{
			pUser->AddText("You can open private shop only in Saint Morning!", 0xffffffff);
			return;
		}*/

	if (pUser->m_vtInfo.GetOther() || pUser->m_vtInfo.IsVendorOpen())
		return;

	if (!pUser->m_vtInfo.VendorIsVendor())
		return;

	if (pUser->IsAttackMode() || pUser->m_pActMover->IsFly())
		return;

#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return;
	}
#endif

#ifdef __S_SERVER_UNIFY
	if (pUser->m_bAllAction == FALSE)
		return;
#endif // 

	if (bOwnShop) // Own private shop
	{
		pUser->m_vtInfo.SetTitle(szPVendor);
		g_UserMng.AddPVendorOpen(pUser);

#ifdef __FL_SHOP_SEARCH
		prj.AddToShopSearch(pUser->GetId());
#endif
		// Chat
		g_ChattingMng.NewChattingRoom(pUser->m_idPlayer);
		CChatting* pChatting = g_ChattingMng.GetChttingRoom(pUser->m_idPlayer);
		if (pChatting)
		{
			pChatting->m_bState = TRUE;
			if (pChatting->AddChattingMember(pUser->m_idPlayer))
				pUser->m_idChatting = pUser->m_idPlayer;
			pUser->AddNewChatting(pChatting);
		}
		pUser->m_dwHonorCheckTime = GetTickCount();
	}
	else // Own private vendor
	{
		if (dwObjId == NULL_ID)
			return;

		CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwObjId);
		if (!IsUsableItem(pItemElem))
		{
			pUser->AddDefinedText(TID_GAME_HOUSING_ADDITION_FAIL);
			return;
		}

		ItemProp* pProp = pItemElem->GetProp();
		if (!pProp || pProp->dwItemKind3 != IK3_VENDOR)
			return;

		DWORD dwSkillTime = pItemElem->m_dwKeepTime ? pItemElem->m_dwKeepTime : pProp->dwSkillTime;
		if (dwSkillTime == NULL || dwSkillTime == NULL_ID)
			return;

		if (g_uIdofMulti != __FL_VENDORS)
		{
			pUser->AddText("Offline Vendors not allowed at this channel.", 0xffffffff);
			return;
		}

		auto it = prj.m_mapPlayerShops.find(pUser->m_idPlayer);
		if (it != prj.m_mapPlayerShops.end())
		{
			pUser->AddText("You already have vendor.", 0xffffffff);
			return;
		}

		CMover* pMoverVendor = NULL;

		PLAYER_COPY tmpStruc;
		tmpStruc.idPlayer = pUser->m_idPlayer;
		tmpStruc.strName = pUser->GetName();
		tmpStruc.nJob = pUser->GetJob();
		tmpStruc.nLevel = pUser->GetLevel();
		tmpStruc.dwHairId = pUser->m_dwHairMesh;
		tmpStruc.dwHairColor = pUser->m_dwHairColor;
		tmpStruc.dwHeadId = pUser->m_dwHeadMesh;
		tmpStruc.vPos = pUser->GetPos();
		tmpStruc.dwWorld = pUser->GetWorld()->GetID();
		tmpStruc.fAngle = pUser->GetAngle();

		if (pProp->dwAbilityMin != NULL_ID)
		{
			MoverProp* pMoverProp = prj.GetMoverProp(pProp->dwAbilityMin);
			if (!pMoverProp)
				return;

			tmpStruc.dwIndex = pMoverProp->dwID;
			tmpStruc.tmpInventoryOwner = NULL;
			pMoverVendor = g_GuildCombatMng.CreatePlayerCopy(NPC_TYPE_VENDOR_NPC, tmpStruc);
		}
		else
		{
			tmpStruc.dwIndex = pUser->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE;
			tmpStruc.tmpInventoryOwner = &pUser->m_Inventory;
			pMoverVendor = g_GuildCombatMng.CreatePlayerCopy(NPC_TYPE_VENDOR_HUMAN, tmpStruc);
		}

		if (!pMoverVendor)
			return;

		pMoverVendor->SetVendorTime(pItemElem->m_dwKeepTime ? pItemElem->m_dwKeepTime + time(0) : pProp->dwSkillTime * 60 + time(0));
		pMoverVendor->SetVendorItemId(pProp->dwID);
		pMoverVendor->m_Inventory.SetItemContainer(ITYPE_ITEM, MAX_VENDITEM);

		pMoverVendor->m_vtInfo.PlayerToVendorAll(pUser);
		pMoverVendor->m_vtInfo.SetTitle(szPVendor);
		g_UserMng.AddPVendorOpen((CUser*)pMoverVendor);

#ifdef __FL_SHOP_SEARCH
		prj.AddToShopSearch(pMoverVendor->GetId());
#endif
		pUser->RemoveItem((BYTE)dwObjId, 1);
		pUser->ActivateClientVendor();
		g_dpDBClient.SendAddVendor(pMoverVendor);
		g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
	}
}

void CDPSrvr::OnVendorAction(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	BYTE nAction;
	ar >> nAction;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	auto it = prj.m_mapPlayerShops.find(pUser->m_idPlayer);
	if (it == prj.m_mapPlayerShops.end())
	{
		pUser->AddText("You have not activated vendor.", 0xffffffff);
		return;
	}

	CMover* pMoverVendor = prj.GetMover(it->second);
	if (!IsValidObj(pMoverVendor))
	{
		pUser->AddText("Invalid vendor.", 0xffffffff);
		return;
	}

	switch (nAction)
	{
	case 1: pMoverVendor->ShutdownVendor(); break;
	case 2: pUser->AddVendorPreview(pMoverVendor); break;
	default: return;
	}
}
#endif

#ifdef __FL_CASH_SHOP
void CDPSrvr::OnGetCShopBalance(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	g_dpDBClient.SendGetCShopBalance(pUser->m_idPlayer, pUser->m_playAccount.lpszAccount);
}
void CDPSrvr::OnBuyCShopItem(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nItemUniqueID;
	int nItemAmount;
	BOOL bDonate;
	u_long idReceiver;

	ar >> nItemUniqueID >> nItemAmount >> bDonate;
	ar >> idReceiver;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	DWORD dwItemID = NULL_ID;
	int nItemNum = 0;
	__int64 n64TotalPrice = 0;

	for (auto& it : *CFlyFFShop::GetInstance()->GetItemVector())
	{
		if (nItemUniqueID != it.nItemIndex)
			continue;

		dwItemID = it.dwItemID;
		nItemNum = it.nAmount * nItemAmount;

		if (bDonate)
		{
			if (it.nPurchaseMethod == CS_METHOD_DP || it.nPurchaseMethod == CS_METHOD_BOTH)
				n64TotalPrice = it.nDonatePrice;
		}
		else
		{
			if (it.nPurchaseMethod == CS_METHOD_VP || it.nPurchaseMethod == CS_METHOD_BOTH)
				n64TotalPrice = it.nVotePrice;
		}

		break;
	}

	if (n64TotalPrice == 0)
	{
		pUser->AddText("Invalid item or currency.", 0xffffffff);
		return;
	}

	if (nItemAmount < 1 || nItemAmount > 99)
	{
		pUser->AddText("Invalid item amount.", 0xffffffff);
		return;
	}

	// FIX_031223 BEGIN
	if (nItemNum > SHRT_MAX)
	{
		pUser->AddText("Cannot buy. Decrease amount of items and try again.", 0xffffffff);
		return;
	}
	// FIX_031223 END

	ItemProp* pProp = prj.GetItemProp(dwItemID);
	if (!pProp)
	{
		pUser->AddText("Unexpected item problem. Unable to purchase.", 0xffffffff);
		Error("CShop Error: itemprop is null [%d]", nItemUniqueID);
		return;
	}

	n64TotalPrice *= nItemAmount;
	if (n64TotalPrice < 1 || n64TotalPrice > INT_MAX)
	{
		pUser->AddText("Unexpected price limit problem. Unable to purchase.", 0xffffffff);
		Error("CShop Error: price < 1 or > INT_MAX for item id [%d]", nItemUniqueID);
		return;
	}

	int nTotalPrice = static_cast<int>(n64TotalPrice);
	g_dpDBClient.SendBuyCShopItem(pUser->m_idPlayer, pUser->m_playAccount.lpszAccount, dwItemID, nItemNum, nTotalPrice, bDonate, idReceiver);
}
#endif

#ifdef __FL_NEW_BOXES
void CDPSrvr::OnNewBoxOpen(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwObjId;
	int nCurSel;

	ar >> dwObjId >> nCurSel;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!pUser->IsPossibleUseItem())
		return;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwObjId);
	if (!IsUsableItem(pItemElem))
		return;

	ItemProp* pItemPropBox = pItemElem->GetProp();
	if (!pItemPropBox)
		return;

	if (pUser->IsUsing(pItemElem))
	{
		pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
		return;
	}

	if (pUser->m_Inventory.IsEquip(dwObjId))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPTRADE, "");
		return;
	}

	auto vecItems = CNewBoxes::GetInstance()->GetBoxElements(pItemElem->m_dwItemId);
	if (vecItems == NULL)
		return;

	if (nCurSel < 0 || nCurSel >= vecItems->size())
		return;

	CItemElem itemElem;
	itemElem.m_dwItemId = vecItems->at(nCurSel).dwReward;
	itemElem.m_nItemNum = vecItems->at(nCurSel).nItemCount;
	if (vecItems->at(nCurSel).bBinded)
		itemElem.SetFlag(CItemElem::binds);

	ItemProp* pItemPropReward = itemElem.GetProp();
	if (!pItemPropReward)
		return;

	if (!pUser->CreateItem(&itemElem))
	{
		pUser->AddDefinedText(TID_GAME_LACKSPACE);
		return;
	}

	pUser->RemoveItem((BYTE)(pItemElem->m_dwObjId), 1);

	// TODO: Move text/sound/sfx to one separate packet and do it in client side instead
	CString str; str.Format("You successfully opened '%s' and received '%s' (x%d)!", pItemPropBox->szName, pItemPropReward->szName, vecItems->at(nCurSel).nItemCount);
	pUser->AddText(str, 0xffffff00);
	if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
		g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
	pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
}
#endif

void CDPSrvr::OnUseTransy(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwObjId;
	int nSel;

	ar >> dwObjId >> nSel;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (!pUser->IsPossibleUseItem())
		return;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwObjId);
	if (!IsUsableItem(pItemElem))
		return;

	ItemProp* pItemProp = pItemElem->GetProp();
	if (!pItemProp)
		return;

	if (pItemProp->dwID != II_CHR_SYS_SCR_TRANSY)
		return;

	if (pUser->IsUsing(pItemElem))
	{
		pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
		return;
	}

	if (!pUser->DoUseItemSexChange(nSel))
		return;

	if (pUser->HasBuffByIk3(IK3_TEXT_DISGUISE))
		pUser->RemoveIk3Buffs(IK3_TEXT_DISGUISE);

	pUser->RemoveItem((BYTE)(pItemElem->m_dwObjId), 1);
}

#ifdef __FL_PENYA_CONVERTER
void CDPSrvr::OnConvertPenya(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (pUser->IsDie())
		return;

	pUser->PenyaToPerins();
}
#endif

#ifdef __FL_SWITCH_EQ
void CDPSrvr::OnSwitchAdd(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nSel;
	char lpszName[32] = { 0, };
	DWORD dwLooter, dwPet, dwRegen;

	ar >> nSel;
	ar.ReadString(lpszName, 32);
	ar >> dwLooter;
	ar >> dwPet;
	ar >> dwRegen;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (pUser->m_Switches.AddSwitch(nSel, lpszName, dwLooter, dwPet, dwRegen, TRUE))
		pUser->AddSwitchAdd(nSel, lpszName, dwLooter, dwPet, dwRegen);
}
void CDPSrvr::OnSwitchDelete(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nSel;
	ar >> nSel;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	if (pUser->m_Switches.DeleteSwitch(nSel))
		pUser->AddSwitchDelete(nSel);
}
void CDPSrvr::OnSwitchUse(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	int nSel;
	ar >> nSel;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->m_Switches.UseSwitch(nSel);
}
#endif

#ifdef __FL_GUILD_FINDER
void CDPSrvr::OnGuildFinderRequest(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->AddGuildFinder();
}
void CDPSrvr::OnGuildFinderJoin(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	u_long idGuild;
	ar >> idGuild;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CGuild* pUserGuild = g_GuildMng.GetGuild(pUser->m_idGuild);
	if (pUserGuild && pUserGuild->IsMember(pUser->m_idPlayer))
		return;

	CGuild* pTargetGuild = g_GuildMng.GetGuild(idGuild);
	if (!pTargetGuild || !pTargetGuild->m_bFinder)
		return;

	if (pTargetGuild->GetSize() >= pTargetGuild->GetMaxMemberSize())
		return;

	if (!IsInviteAbleGuild(idGuild, NULL))
	{
		pUser->AddText("You cannot join this guild right now.", 0xffffffff);
		return;
	}

	BEFORESENDDUAL(out, PACKETTYPE_ADD_GUILD_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN);
	out << pUser->m_idPlayer << idGuild;
	SEND(out, &g_DPCoreClient, DPID_SERVERPLAYER);
}
void CDPSrvr::OnGuildFinderAddon(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	u_long idGuild;
	ar >> idGuild;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CGuild* pTargetGuild = g_GuildMng.GetGuild(idGuild);
	if (!pTargetGuild || !pTargetGuild->m_bFinder)
		return;

	pUser->AddGuildFinderAddon(pTargetGuild);
}
#endif

#ifdef __FL_PACKED_ITEMS
void CDPSrvr::OnPackageOpen(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	DWORD dwObjId;
	ar >> dwObjId;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwObjId);
	if (!IsUsableItem(pItemElem))
		return;

	if (pUser->IsUsing(pItemElem))
	{
		pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
		return;
	}

	if (!pItemElem->IsPackage())
		return;

	auto pVecItems = pItemElem->GetItemList();
	if (pVecItems->empty())
		return;

	if (pUser->m_Inventory.GetEmptyCount() < pVecItems->size())
	{
		pUser->AddText(prj.GetText(TID_GAME_LACKSPACE), 0xffffffff);
		return;
	}

	for (auto& it : *pVecItems)
		pUser->CreateItem(it);

	pItemElem->ClearPackedItems();
	pUser->AddPackageOpen(dwObjId);
}
void CDPSrvr::OnPackageAdd(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	DWORD dwPackObjId, dwItemObjId;
	ar >> dwPackObjId >> dwItemObjId;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	// Check package and item
	CItemElem* pPackageElem = pUser->m_Inventory.GetAtId(dwPackObjId);
	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwItemObjId);
	if (!IsUsableItem(pPackageElem) || !IsUsableItem(pItemElem))
		return;

	if (!pPackageElem->IsPackage() || pItemElem->IsPackage())
		return;

	if (pUser->IsUsing(pPackageElem) || pUser->IsUsing(pItemElem))
	{
		pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
		return;
	}

	// Check item
	if (pUser->m_Inventory.IsEquip(pItemElem->m_dwObjId))
	{
		pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
		return;
	}

	if (pItemElem->IsFlag(CItemElem::expired) || pItemElem->IsQuest() || pItemElem->IsBinds())
		return;

	// Add
	if (pPackageElem->AddToPack(pItemElem))
	{
		pUser->AddPackageAdd(dwPackObjId, dwItemObjId);
		pUser->RemoveItem((BYTE)dwItemObjId, pItemElem->m_nItemNum);
	}
}
void CDPSrvr::OnPackageRestore(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	DWORD dwObjId;
	int nCurSel;

	ar >> dwObjId >> nCurSel;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwObjId);
	if (!IsUsableItem(pItemElem))
		return;

	if (pUser->IsUsing(pItemElem))
	{
		pUser->AddDefinedText(TID_GAME_CANNOT_DO_USINGITEM);
		return;
	}

	if (pUser->m_Inventory.GetEmptyCount() < 1)
	{
		pUser->AddText(prj.GetText(TID_GAME_LACKSPACE), 0xffffffff);
		return;
	}

	if (!pItemElem->IsPackage())
		return;

	auto pVecItems = pItemElem->GetItemList();
	if (nCurSel < 0 || nCurSel >= static_cast<int>(pVecItems->size()))
		return;

	CItemElem* pItemInside = pVecItems->at(nCurSel);

	CItemElem itemElem;
	itemElem = *pItemInside;

	if (pItemElem->DeleteFromPack(nCurSel))
	{
		pUser->CreateItem(&itemElem);
		pUser->AddPackageRestore(dwObjId, nCurSel);
	}
}
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
void CDPSrvr::OnWofInit(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CWheelOfFortune::GetInstance()->InitProcess(pUser);
}
void CDPSrvr::OnWofFinish(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	CWheelOfFortune::GetInstance()->FinishProcess(pUser);
}
#endif

#ifdef __FL_RECORD_BOOK
void CDPSrvr::OnRecordBook(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->AddRecordBook();
}
#endif
#ifdef __FL_MONSTER_ALBUM
void CDPSrvr::OnMABossCardSlotRestore(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	DWORD dwKeyObjId;
	int nKeyLocation;
	int nSlot;

	ar >> dwKeyObjId >> nKeyLocation >> nSlot;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser))
		return;

	pUser->m_MonsterAlbum.BossCardSlotRestore(dwKeyObjId, nKeyLocation, nSlot);
}
#endif
#ifdef __FL_TDM
void CDPSrvr::OnRequestTDM(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	pUser->AddTDMData();
}
void CDPSrvr::OnRequestTDMRegister(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	CTDMManager::GetInstance()->RegisterPlayer(pUser);
}
void CDPSrvr::OnRequestTDMJoin(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long)
{
	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (IsInvalidObj(pUser))
		return;

	if (!CTDMManager::GetInstance()->JoinTDMWorld(pUser))
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT1TO1_NOENTRANCETIME);
}
#endif