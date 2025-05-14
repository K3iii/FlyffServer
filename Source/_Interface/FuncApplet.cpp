#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndParty.h"
#include "WndNotice.h"
#include "WndMessenger.h"
#include "WndBank.h"
#include "WndDebugInfo.h"
#include "WndHelp.h"
#include "WndWebBox.h"
#include "WndVendor.h"
#include "WndCommItem.h"
#include "WndQuest.h"
#include "WndMotion.h"
#include "WndPvp.h"

#include "WndMapEx.h"
#include "WndBagEx.h"

#if __VER >= 8 //__CSC_VER8_2
#include "WndPartyQuick.h"
#endif //__CSC_VER8_2
#if __VER >= 13 // __HOUSING
#include "WndHousing.h"
#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
#include "WndGuildHouse.h"
#endif

#ifdef __FL_CASH_SHOP
#include "FlyFFShop.h"
#endif

#ifdef __WIKI_ITEMS_V2
#include "WndWikiItems.h"
#endif

#include "WndNavigator.h"

#ifdef __FL_TELEPORT
#include "Teleporter.h"
#endif

#ifdef __MODEL_VIEW
#include "WndModelView.h"
#endif

DECLAREAPPLET(AppMain_WndNavigator, new CWndNavigator);
DECLAREAPPLET(AppMain_WndCharacter, new CWndCharacter);
DECLAREAPPLET(AppMain_WndSkill2, new CWndSkill_16);
DECLAREAPPLET(AppMain_WndEmotion, new CWndEmotion);
DECLAREAPPLET(AppMain_WndMotion, new CWndMotion);
DECLAREAPPLET(AppMain_WndTrade, new CWndTrade);
DECLAREAPPLET(AppMain_WndVendor, new CWndVendor);
DECLAREAPPLET(AppMain_WndQuest, new CWndQuest);
DECLAREAPPLET(AppMain_WndInventory, new CWndInventory);
DECLAREAPPLET(AppMain_WndParty, new CWndParty);
DECLAREAPPLET(AppMain_WndGuild, new CWndGuild);
DECLAREAPPLET(AppMain_WndCommItem, new CWndCommItem);
DECLAREAPPLET(AppMain_WndUpgradeBase, new CWndUpgradeBase);
DECLAREAPPLET(AppMain_WndChat, new CWndChat);
#if __VER >= 11 // __CSC_VER11_4
DECLAREAPPLET(AppMain_WndMessenger, new CWndMessengerEx);
#else //__CSC_VER11_4
DECLAREAPPLET(AppMain_WndMessenger, new CWndMessenger);
#endif //__CSC_VER11_4
DECLAREAPPLET(AppMain_WndOptSound, new CWndOptSound);
DECLAREAPPLET(AppMain_WndOptWindow, new CWndOptWindow);
DECLAREAPPLET(AppMain_WndOptMyInfo, new CWndOptMyInfo);
DECLAREAPPLET(AppMain_WndOption, new CWndTotalOption);
DECLAREAPPLET(AppMain_WndInfoNotice, new CWndInfoNotice);
DECLAREAPPLET(AppMain_WndHelpHelp, new CWndHelp);
DECLAREAPPLET(AppMain_WndHelpTip, new CWndHelpTip);
DECLAREAPPLET(AppMain_WndHelpFAQ, new CWndHelpFAQ);
DECLAREAPPLET(AppMain_WndLogOut, new CWndLogOut);
DECLAREAPPLET(AppMain_WndQuit, new CWndQuit);
DECLAREAPPLET(AppMain_WndWorld, new CWndWorld);
DECLAREAPPLET(AppMain_WndDebugInfo, new CWndDebugInfo);
DECLAREAPPLET(AppMain_WndStatus, new CWndStatus);
DECLAREAPPLET(AppMain_WndLogin, new CWndLogin);
DECLAREAPPLET(AppMain_WndSelectServer, new CWndSelectServer);
DECLAREAPPLET(AppMain_WndCreateChar, new CWndCreateChar);
DECLAREAPPLET(AppMain_WndSelectChar, new CWndSelectChar);
DECLAREAPPLET(AppMain_WndMap, new CWndMapEx);
DECLAREAPPLET(AppMain_LordSkill, new CWndLordSkill);
DECLAREAPPLET(AppMain_WndPartyQuick, new CWndPartyQuick);
DECLAREAPPLET(AppMain_WndBuffStatus, new CWndBuffStatus);
DECLAREAPPLET(AppMain_WndInvenRemoveItem, new CWndInvenRemoveItem);
DECLAREAPPLET(AppMain_Housing, new CWndHousing);
DECLAREAPPLET(AppMain_GuildHousing, new CWndGuildHousing);
DECLAREAPPLET(AppMain_Couple, new CWndCoupleManager);

#ifdef __WIKI_ITEMS_V2
DECLAREAPPLET(AppMain_WikiItems, new CWndWikiItems);
#endif

#ifdef __FL_BATTLE_PASS
DECLAREAPPLET(AppMain_BattlePass, new CWndBattlePass);
#endif

#ifdef __FL_CONSUMABLES_STORAGE
DECLAREAPPLET(AppMain_Chest, new CWndChest);
#endif

#ifdef __FL_DPS_DISPLAY
DECLAREAPPLET(AppMain_DpsDisplay, new CWndDpsDisplay);
#endif

#ifdef __FL_FAST_ELEMENT
DECLAREAPPLET(AppMain_ElementChanger, new CWndChangeAttributeNew);
#endif

#ifdef __FL_SHOP_SEARCH
DECLAREAPPLET(AppMain_ShopSearch, new CWndShopSearch);
#endif

#ifdef __FL_PET_FILTER
DECLAREAPPLET(AppMain_PetFilter, new CWndPetLootOption);
#endif

DECLAREAPPLET(AppMain_CollectInfo, new CWndCollectingItems);

#ifdef __FL_GW_STATUE_V2
DECLAREAPPLET(AppMain_StatueConfiguration, new CWndStatueConfiguration);
DECLAREAPPLET(AppMain_StatuePurchase, new CWndStatuePurchase);
#endif

#ifdef __FL_FFA
DECLAREAPPLET(AppMain_FFAResult, new CWndFFAResult);
#endif

#ifdef __FL_DAILY_REWARDS
DECLAREAPPLET(AppMain_DailyGift, new CWndDailyGift);
#endif

#ifdef __FL_TELEPORT
DECLAREAPPLET(AppMain_Teleport, new CWndTeleport);
#endif

#ifdef __FL_RECYCLE
DECLAREAPPLET(AppMain_Recycle, new CWndRecycle);
#endif

#ifdef __JOB_CHANGE
DECLAREAPPLET(AppMain_WndNewChangeJob, new CWndNewChangeJob);
DECLAREAPPLET(AppMain_WndNewChangeJob2, new CWndNewChangeJob2);
DECLAREAPPLET(AppMain_WndNewChangeJob3, new CWndNewChangeJob3);
#endif

#ifdef __MODEL_VIEW
DECLAREAPPLET(AppMain_ModelView, new CWndModelView);
#endif

#ifdef __FL_PARTY_FINDER
DECLAREAPPLET(AppMain_PartyFinder, new CWndPartyFinder);
#endif

#ifdef __FL_VENDORS
DECLAREAPPLET(AppMain_VendorManagement, new CWndVendorManagement);
DECLAREAPPLET(AppMain_VendorPreview, new CWndVendorPreview);
#endif

#ifdef __FL_CASH_SHOP
DECLAREAPPLET(AppMain_FlyFFShop, new CWndFlyFFShop);
#endif

#ifdef __FL_SWITCH_EQ
DECLAREAPPLET(AppMain_SwitchEq, new CWndSwitchEquip);
#endif

#ifdef __FL_GUILD_FINDER
DECLAREAPPLET(AppMain_GuildFinder, new CWndGuildFinder);
#endif

#ifdef __FL_FARM_DISPLAY
DECLAREAPPLET(AppMain_FarmDisplay, new CWndFarmDisplay);
#endif

#ifdef __FL_PACKED_ITEMS
DECLAREAPPLET(AppMain_PackedItems, new CWndPackedItems);
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
DECLAREAPPLET(AppMain_Wof, new CWndWof);
#endif

#ifdef __FL_RECORD_BOOK
DECLAREAPPLET(AppMain_RecordBook, new CWndRecordBook);
#endif

#ifdef __FL_MONSTER_ALBUM
DECLAREAPPLET(AppMain_MonsterAlbum, new CWndMonsterAlbum);
#endif

#ifdef __FL_TDM
DECLAREAPPLET(AppMain_Activities, new CWndActivities);
DECLAREAPPLET(AppMain_TDMResult, new CWndTDMResult);
#endif

#ifdef __AI_SYSTEM
DECLAREAPPLET(AppMain_WndRefresher, new CWndRefresher);
#endif


void CWndMgr::AddAllApplet()
{
#ifdef __FL_TDM
	AddAppletFunc(AppMain_TDMResult, APP_TDM_RESULT, "WndTDMResult", "Icon_Hunter_FFA.tga", "TDM Result", 0);
	AddAppletFunc(AppMain_Activities, APP_PVP_ACTIVITIES, "WndActivities", "Icon_Hunter_FFA.tga", "PvP & PvE Activities", 0);
#endif
#ifdef __FL_MONSTER_ALBUM
	AddAppletFunc(AppMain_MonsterAlbum, APP_MONSTER_ALBUM, "WndMonsterAlbum", "Icon_Applet.dds", "Monster Album", 0);
#endif
#ifdef __FL_RECORD_BOOK
	AddAppletFunc(AppMain_RecordBook, APP_RECORD_BOOK, "WndRecordBook", "Icon_Applet.dds", "Record Book", 0);
#endif
#ifdef __FL_WHEEL_OF_FORTUNE
	AddAppletFunc(AppMain_Wof, APP_WOF, "WndWof", "Icon_Applet.dds", "Wheel of Fortune", 0);
#endif

#ifdef __FL_PACKED_ITEMS
	AddAppletFunc(AppMain_PackedItems, APP_PACKED_ITEMS, "WndPackedItems", "Icon_Applet.dds", "Preview Custom Package", 0);
#endif

#ifdef __FL_FARM_DISPLAY
	AddAppletFunc(AppMain_FarmDisplay, APP_FARM_DISPLAY, "WndFarmDisplay", "Icon_Applet.dds", "Farm Display", 0);
#endif

#ifdef __FL_GUILD_FINDER
	AddAppletFunc(AppMain_GuildFinder, APP_GUILD_FINDER, "WndGuildFinder", "Icon_Applet.dds", "Guild-Finder", 0);
#endif

#ifdef __AI_SYSTEM
	AddAppletFunc(AppMain_WndRefresher, APP_REFRESHER, _T("WndRefresher"), _T("Icon_Motion.dds"), ("AI System"), 0);
#endif

#ifdef __FL_SWITCH_EQ
	AddAppletFunc(AppMain_SwitchEq, APP_EQ_SWITCH, "WndSwitchEquip", "Icon_Applet.dds", "Switch Equipment", 0);
#endif

#ifdef __FL_CASH_SHOP
	AddAppletFunc(AppMain_FlyFFShop, APP_CASH_SHOP, "WndFlyFFShop", "Icon_Applet.dds", "FlyFF Cash Shop", 0);
#endif

#ifdef __FL_VENDORS
	AddAppletFunc(AppMain_VendorManagement, APP_VENDOR_MANAGE, "WndVendorManagement", "Icon_Applet.dds", "Offline Vendor Menu", 0);
	AddAppletFunc(AppMain_VendorPreview, APP_VENDOR_PREVIEW, "WndVendorPreview", "Icon_Applet.dds", "Offline Vendor Preview", 0);
#endif

#ifdef __FL_PARTY_FINDER
	AddAppletFunc(AppMain_PartyFinder, APP_PARTY_FINDER, "WndPartyFinder", "Icon_Hunter_Partyfinder.tga", "Party-Finder", 'L');
#endif

#ifdef __FL_RECYCLE
	AddAppletFunc(AppMain_Recycle, APP_RECYCLE, "WndRecycle", "Icon_Hunter_Recycle.tga", "Recycle", 0);
#endif

#ifdef __MODEL_VIEW
	AddAppletFunc(AppMain_ModelView, APP_MODEL_VIEW, "WndModelView", "Icon_Applet.dds", "ModelView", 0);
#endif

#ifdef __JOB_CHANGE
	AddAppletFunc(AppMain_WndNewChangeJob, APP_CHANGE_JOB_01, _T("WndNewChangeJob"), _T("Icon_Applet.dds"), "Job Changer", 0);
	AddAppletFunc(AppMain_WndNewChangeJob2, APP_CHANGE_JOB_02, _T("WndNewChangeJob2"), _T("Icon_Applet.dds"), "Job Changer", 0);
	AddAppletFunc(AppMain_WndNewChangeJob3, APP_CHANGE_JOB_03, _T("WndNewChangeJob3"), _T("Icon_Applet.dds"), "Job Changer", 0);
#endif

#ifdef __FL_TELEPORT
	AddAppletFunc(AppMain_Teleport, APP_TELEPORT, "WndTeleport", "Icon_Hunter_Teleporter.tga", "Teleporter", 'V');
#endif

#ifdef __FL_DAILY_REWARDS
	AddAppletFunc(AppMain_DailyGift, APP_DAILY_REWARD, "WndDailyGift", "Icon_Applet.dds", "Daily Reward", 0);
#endif

#ifdef __FL_FFA
	AddAppletFunc(AppMain_FFAResult, APP_FFA_RESULT, "WndFFAResult", "Icon_Hunter_FFA.tga", "FFA Result", 0);
#endif

#ifdef __FL_GW_STATUE_V2
	AddAppletFunc(AppMain_StatueConfiguration, APP_STATUE_CONFIGURATION, "WndStatueConfiguration", "Icon_Applet.dds", "Statue Configuration", 0);
	AddAppletFunc(AppMain_StatuePurchase, APP_STATUE_PURCHASE, "WndStatuePurchase", "Icon_Applet.dds", "Statue Buff Purchase", 0);
#endif

	AddAppletFunc(AppMain_CollectInfo, APP_COLLECTING_INFO, "WndCollectingItems", "Icon_Applet.dds", "Collector Info", 0);

#ifdef __FL_PET_FILTER
	AddAppletFunc(AppMain_PetFilter, APP_PET_LOOT_OPTION, "WndPetLootOption", "Icon_Hunter_PetFilter.tga", "Pet Filter", 'U');
#endif

#ifdef __FL_SHOP_SEARCH
	AddAppletFunc(AppMain_ShopSearch, APP_SHOP_SEARCH, "WndShopSearch", "Icon_Hunter_ShopSearch.tga", "Shop Search", 0);
#endif

#ifdef __FL_FAST_ELEMENT
	AddAppletFunc(AppMain_ElementChanger, APP_FAST_ELEMENT, "WndChangeAttributeNew", "Icon_Hunter_ElementChanger.tga", "Element Changer", 'R');
#endif

#ifdef __FL_DPS_DISPLAY
	AddAppletFunc(AppMain_DpsDisplay, APP_DPS_DISPLAY, _T("WndDpsDisplay"), _T("Icon_Hunter_DpsMeter.tga"), "DPS Display", 0);
#endif

#ifdef __FL_CONSUMABLES_STORAGE
	AddAppletFunc(AppMain_Chest, APP_CHEST, _T("WndChest"), _T("Icon_Hunter_Chest.tga"), "Chest", 'B');
#endif
#ifdef __FL_BATTLE_PASS
	AddAppletFunc(AppMain_BattlePass, APP_BATTLE_PASS, "WndBattlePass", "Icon_Hunter_BattlePass.tga", "Battle Pass", 0);
#endif

#ifdef __WIKI_ITEMS_V2
	AddAppletFunc(AppMain_WikiItems, APP_WIKI_ITEMS, "WndWikiItems", "Icon_Hunter_Wiki.tga", "Wiki Items", 0);
#endif

	AddAppletFunc(AppMain_WndNavigator, APP_NAVIGATOR, _T("WndNavigator"), _T("Icon_Navigator.dds"), GETTEXT(TID_TIP_NAVIGATOR), 'N');
	AddAppletFunc(AppMain_WndStatus, APP_STATUS1, _T("WndStatus"), _T("Icon_Status.dds"), GETTEXT(TID_TIP_STATUS), 'T');
	AddAppletFunc(AppMain_WndMap, APP_MAP_EX, _T("WndMap"), _T("Icon_Applet.dds"), GETTEXT(TID_TIP_MAP), 'M');
	AddAppletFunc(AppMain_LordSkill, APP_LORD_SKILL, _T("WndLordSkill"), _T("Icon_Infopang.dds"), GETTEXT(TID_TIP_INFOPANG), 0);
	AddAppletFunc(AppMain_Housing, APP_HOUSING, _T("WndHousing"), _T("Icon_Housing.dds"), GETTEXT(TID_GAME_HOUSING_BOX), 'Y');
	AddAppletFunc(AppMain_GuildHousing, APP_GH_FURNITURE_STORAGE, _T("WndGuildHousing"), _T("Icon_Housing.dds"), GETTEXT(TID_GAME_HOUSING_BOX), 0);
	AddAppletFunc(AppMain_WndCharacter, APP_CHARACTER3, _T("WndCharacter"), _T("Icon_Character.dds"), GETTEXT(TID_TIP_CHARACTER), 'H');
	AddAppletFunc(AppMain_WndInventory, APP_INVENTORY, _T("WndInventory"), _T("Icon_Inventory.dds"), GETTEXT(TID_TIP_INVENTORY), 'I');
	AddAppletFunc(AppMain_WndSkill2, APP_SKILL4, _T("WndSkill"), _T("Icon_Skill.dds"), GETTEXT(TID_TIP_SKILL), 'K');
	AddAppletFunc(AppMain_WndMotion, APP_MOTION, _T("WndMotion"), _T("Icon_Motion.dds"), GETTEXT(TID_TIP_MOTION), 'O');
	AddAppletFunc(AppMain_WndTrade, APP_TRADE, _T("WndTrade"), _T("Icon_Trade.dds"), GETTEXT(TID_TIP_TRADE), 0);
	AddAppletFunc(AppMain_WndVendor, APP_VENDOR_REVISION, _T("WndVendor"), _T("Icon_Hunter_PrivateShop.tga"), GETTEXT(TID_TIP_VENDOR), 0);
	AddAppletFunc(AppMain_WndQuest, APP_QUEST_EX_LIST, _T("WndQuest"), _T("Icon_Quest.dds"), GETTEXT(TID_TIP_QUEST), g_Neuz.Key.chQuest);
	AddAppletFunc(AppMain_WndParty, APP_PARTY, _T("WndParty"), _T("Icon_Troupe.dds"), GETTEXT(TID_TIP_PARTY), 'P');
	AddAppletFunc(AppMain_WndGuild, APP_GUILD, _T("WndGuild"), _T("Icon_Troupe.dds"), GETTEXT(TID_TIP_COMPANY), 'G');
	AddAppletFunc(AppMain_WndCommItem, APP_COMM_ITEM, _T("WndCommItem"), _T("Icon_CItemTime.dds"), GETTEXT(TID_TIP_ITEMTIME), 'J');
	AddAppletFunc(AppMain_WndUpgradeBase, APP_TEST, _T("WndUpgradeBase"), _T("Icon_Troupe.dds"), GETTEXT(TID_TIP_PARTY), 0);
	AddAppletFunc(AppMain_WndChat, APP_COMMUNICATION_CHAT, _T("WndChat"), _T("Icon_Chat.dds"), GETTEXT(TID_TIP_COMMUNICATION_CHAT), 0);
	AddAppletFunc(AppMain_WndMessenger, APP_MESSENGER_, _T("WndMessenger"), _T("Icon_Messenger.dds"), GETTEXT(TID_TIP_MESSENGER), 'E');
	AddAppletFunc(AppMain_WndOptSound, APP_OPTION_SOUND, _T("WndOptSound"), _T("Icon_Hunter_Settings.tga"), GETTEXT(TID_TIP_OPTION_SOUND), 0);
	AddAppletFunc(AppMain_WndOptWindow, APP_OPTION_WINDOW, _T("WndOptWindow"), _T("Icon_Hunter_Settings.tga"), GETTEXT(TID_TIP_OPTION_WINDOW), 0);
	AddAppletFunc(AppMain_WndOptMyInfo, APP_OPTION_MYINFO, _T("WndOptMyInfo"), _T("Icon_Hunter_Settings.tga"), GETTEXT(TID_TIP_OPTION_MYINFO), 0);
	AddAppletFunc(AppMain_WndOption, APP_OPTIONEX, _T("WndOption"), _T("Icon_Hunter_Settings.tga"), GETTEXT(TID_APP_OPTION), 0);
	AddAppletFunc(AppMain_WndInfoNotice, APP_INFO_NOTICE, _T("WndInfoNotice"), _T("Icon_Applet.dds"), GETTEXT(TID_TIP_INFO_NOTICE), 0);
	AddAppletFunc(AppMain_WndHelpHelp, APP_HELPER_HELP, _T("WndHelpHelp"), _T("Icon_HelperHelp.dds"), GETTEXT(TID_TIP_HELPER_HELP), 0);
	AddAppletFunc(AppMain_WndHelpTip, APP_HELPER_TIP, _T("WndHelpTip"), _T("Icon_HelperTip.dds"), GETTEXT(TID_TIP_HELPER_TIP), 0);
	AddAppletFunc(AppMain_WndHelpFAQ, APP_HELPER_FAQ, _T("WndHelpFAQ"), _T("Icon_HelperFAQ.dds"), GETTEXT(TID_TIP_HELPER_FAQ), 0);
	AddAppletFunc(AppMain_WndLogOut, APP_LOGOUT, _T("WndLogout"), _T("Icon_Logout.dds"), GETTEXT(TID_TIP_LOGOUT), 0);
	AddAppletFunc(AppMain_WndQuit, APP_QUIT, _T("WndQuit"), _T("Icon_Quit.dds"), GETTEXT(TID_TIP_QUIT), 0);
	AddAppletFunc(AppMain_WndWorld, APP_WORLD, _T("WndWorld"), _T("Icon_Applet.dds"), GETTEXT(TID_TIP_WORLD), 0);
	AddAppletFunc(AppMain_WndDebugInfo, APP_DEBUGINFO, _T("WndDebugInfo"), _T("Icon_Hunter_Debug.tga"), GETTEXT(TID_TIP_DEBUGINFO), 0);
	AddAppletFunc(AppMain_WndLogin, APP_LOGIN, _T("WndLogin"), _T("Icon_Login.dds"), GETTEXT(TID_TIP_APPLET), 0);
	AddAppletFunc(AppMain_WndSelectServer, APP_SELECT_SERVER, _T("WndSelectServer"), _T("Icon_SelectServer.dds"), GETTEXT(TID_TIP_WORLD), 0);
	AddAppletFunc(AppMain_WndCreateChar, APP_CREATE_CHAR, _T("WndCreateChar"), _T("Icon_CreateChar.dds"), GETTEXT(TID_TIP_WORLD), 0);
	AddAppletFunc(AppMain_WndSelectChar, APP_SELECT_CHAR, _T("WndSelectChar"), _T("Icon_SelectChar.dds"), GETTEXT(TID_TIP_DIALOG), 0);
	AddAppletFunc(AppMain_WndPartyQuick, APP_PARTY_QUICK, _T("WndPartyQuick"), NULL, GETTEXT(TID_TIP_DIALOG), 0);
	AddAppletFunc(AppMain_WndBuffStatus, APP_BUFF_STATUS, _T("WndBuffStatus"), NULL, GETTEXT(TID_TIP_DIALOG), 0);
	AddAppletFunc(AppMain_Couple, APP_COUPLE_MAIN, _T("WndCoupleManager"), _T("Icon_Couple.dds"), GETTEXT(TID_GAME_COUPLE), 'F');
}
