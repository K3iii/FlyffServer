// WndArcane.h: interface for the CWndNeuz class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rtmessenger.h"
#include "WndWorld.h"
#include "WndBank.h"
#include "WndGuildWarPeaceConfirm.h"
#include "WndGuildWarRequest.h"
#include "WndUpgradebase.h"
#include "WndCommItem.h"
#include "WndPvp.h"
#include "WndSummonFriend.h"
#include "WndPiercing.h"
#include "WndIndirectTalk.h"
#include "WndInvenRemoveItem.h"
#include "WndCollecting.h"
#include "WndChangeFace.h"
#if __VER >= 11 // __SYS_IDENTIFY
#include "WndAwakening.h"
#include "WndLvReqDown.h"
#include "WndBlessingCancel.h"
#endif

#if __VER >= 12 // __LORD
#include "WndLord.h"
#endif

#include "WndPetRes.h"

#if __VER >= 8 //__CSC_VER8_5
#include "WndSummonAngel.h"
#endif //__CSC_VER8_5

#ifdef __EVE_MINIGAME
#include "WndMiniGame.h"
#endif //__EVE_MINIGAME

#if __VER >= 9 // __CSC_VER9_1
#include "WndPetSys.h"
#endif //__CSC_VER9_1

#if __VER >= 12 // __SECRET_ROOM
#include "WndSecretRoom.h"
#endif //__SECRET_ROOM
#ifdef __AZRIA_1023
#include "WndSelectCh.h"
#endif
#ifdef __HELP_BUG_FIX
#include "WndHelp.h"
#endif //__HELP_BUG_FIX

#if __VER >= 13 // __HOUSING
#include "WndRoomList.h"
#include "WndQuitRoom.h"
#endif // __HOUSING
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
#include "WndQuestQuickInfo.h"
#include "WndQuest.h"
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#include "WndChattingBlockingList.h"
#endif // __YS_CHATTING_BLOCKING_SYSTEM

typedef CMap<DWORD, DWORD, void*, void*> CMapDWordToPtr;

struct VIEWPORT
{
	D3DXVECTOR3 vecLookAt;
	D3DXVECTOR3 vecPos;
	D3DXVECTOR3 vecMatrixScaling;
	D3DXVECTOR3 vecMatrixTranslation;
	float m_fRot;
	float m_fFov;

	VIEWPORT() : vecLookAt(0.0f, 0.0f, 3.0f), vecPos(0.0f, 0.7f, -3.5f), vecMatrixScaling(0.0f, 0.0f, 0.0f), vecMatrixTranslation(0.0f, 0.0f, 0.0f), m_fRot(0.0f), m_fFov(4.0f)
	{
	}
};


#define DECLAREAPPLET( AddMain_Func, AllocClass) CWndBase* AddMain_Func() { return AllocClass; }

class CWndMessage;
class CWndInstantMsg;
class CWndCommand;
class CWndMap;
class CWndPartyChangeName;
class CWndPartyConfirm;
class CWndPartyLeaveConfirm;
class CWndFriendConFirm;

class CWndAddFriend;
class CWndCloseExistingConnection;
class CWndMessengerNote;
class CWndSelectVillage;

#define REG_VERSION 1

#include "WndGuild.h"
#include "WndRankGuild.h"
#include "WndRankInfo.h"
#include "WndRankWar.h"
#include "WndGuildBank.h"

#include "WndChangeName.h"

#if __VER >= 11
#include "WndSealChar.h"
#endif

#define MAX_SKILL		  16

// chat style
#define CHATSTY_GENERAL   0x0001
#define CHATSTY_WHISPER   0x0002
#define CHATSTY_SHOUT     0x0004
#define CHATSTY_PARTY     0x0008
#define CHATSTY_GUILD     0x0010

#define CHATSTY_HELP          0x0060
#define CHATSTY_HELP_CLIENT   0x0020
#define CHATSTY_HELP_CHAT     0x0040
#define CHATSTY_SYSTEM        0x0180
#define CHATSTY_SYSTEM_CLIENT 0x0080
#define CHATSTY_SYSTEM_CHAT   0x0100
#define CHATSTY_GAME          0x0600
#define CHATSTY_GAME_CLIENT   0x0200
#define CHATSTY_GAME_CHAT     0x0400

#define PS_USE_MACRO     0x00000001
#define PS_NOT_MACRO     0x00000002


typedef struct tagWNDREGINFO
{
	DWORD dwWndId;
	CRect rect;
	BOOL  bOpen;
	DWORD dwVersion;
	DWORD dwWndSize;
	DWORD dwSize;
	BYTE* lpArchive;
} WNDREGINFO, * LPWNDREGINFO;

struct AppletFunc
{
	CWndBase* (*m_pFunc)();
	DWORD m_dwIdApplet;
	LPCTSTR m_pAppletName;
	LPCTSTR m_pAppletDesc;
	LPCTSTR m_pszIconName;
	CHAR m_cHotkey;
};

struct ToolTipItemTextColor
{
	DWORD dwName0;
	DWORD dwName1;
	DWORD dwName2;
	DWORD dwName3;
	DWORD dwName4;
	DWORD dwName5;
	DWORD dwGeneral;
	DWORD dwPiercing;
	DWORD dwPlusOption;
	DWORD dwResist;
	DWORD dwResistFire;
	DWORD dwResistWater;
	DWORD dwResistEarth;
	DWORD dwResistElectricity;
	DWORD dwResistWind;
	DWORD dwResistSM;
	DWORD dwResistSM1;
	DWORD dwTime;
	DWORD dwEffective0;
	DWORD dwEffective1;
	DWORD dwEffective2;
	DWORD dwEffective3;
	DWORD dwRandomOption;
	DWORD dwEnchantOption;
	DWORD dwSetName;
	DWORD dwSetItem0;
	DWORD dwSetItem1;
	DWORD dwSetEffect;
	DWORD dwGold;
	DWORD dwCommand;
	DWORD dwNotUse;
#if __VER >= 9 // __CSC_VER9_1
	DWORD dwAddedOpt1;
	DWORD dwAddedOpt2;
	DWORD dwAddedOpt3;
	DWORD dwAddedOpt4;
	DWORD dwAddedOpt5;
	DWORD dwAddedOpt6;
	DWORD dwAddedOpt7;
	DWORD dwAddedOpt8;
	DWORD dwAddedOpt9;
#endif //__CSC_VER9_1
#if __VER >= 11 // __SYS_IDENTIFY
	DWORD dwAwakening;
	DWORD dwBlessing;
	DWORD dwBlessingWarning;
#endif	// __SYS_IDENTIFY
};


#if __VER >= 15 // __GUILD_HOUSE
class CWndGHMainMenu;
class CWndGHUpkeep;
#endif

class CWndMgr : public CWndBase
{
	CString m_strChatBackup;
	CTimer m_timerDobe;

public:
	CStringArray m_aChatString;
	CUIntArray   m_aChatColor;
	CUIntArray   m_aChatStyle;
	CRTMessenger	m_RTMessenger;
	CMapStringToPtr m_mapInstantMsg;
	CMapStringToPtr m_mapMessage;
	CEditString m_ChatString;

	DWORD m_dwSavePlayerTime;
	BOOL m_bAutoRun;

	TCHAR m_szTimerChat[128];
	CTimer m_timerChat;

	ToolTipItemTextColor	dwItemColor;

	typedef vector< CWndBase** >				vecWndContainer;
	typedef vecWndContainer::iterator			vecWndIter;
	vecWndContainer								m_vecChildWnds;

	void RegisterAllCustomChild(void);
	void RegisterCustomChild(CWndBase*& pWndChild);
	void DeleteAllCustomChild(void);

	CPtrArray	m_awndShortCut;
	BOOL	m_bTitle;
	CMapDWordToPtr	m_mapAppletFunc;
#if __VER >= 8 // __CSC_VER8_1
	list<int> m_tempWndId;
	BOOL m_clearFlag;
#endif //__CSC_VER8_1

	AppletFunc* GetAppletFunc(DWORD dwIdApplet);
	void	AddAllApplet();
	BOOL ScreenCapture();
#if __VER >= 11 // __CAPTURE_JPG
	BOOL SaveJPG(LPCTSTR lpszName);
#endif
	void InitSetItemTextColor();
	void Free();

public:
#ifdef __FL_UPDATE_CHATTING
	DWORD m_dwLastSentChatMessage;
#endif

	CWndBeautyShop* m_pWndBeautyShop;
#if __VER >= 8 //__CSC_VER8_4
	CWndFaceShop* m_pWndFaceShop;
#ifdef __NEWYEARDAY_EVENT_COUPON
	CWndUseCouponConfirm* m_pWndUseCouponConfirm;
#endif //__NEWYEARDAY_EVENT_COUPON

#endif //__CSC_VER8_4
	CWndDialog* m_pWndDialog;
	CWndShop* m_pWndShop;
	CWndTrade* m_pWndTrade;
	CWndTradeGold* m_pWndTradeGold;
	CWndDropItem* m_pWndDropItem;
	CWndConfirmTrade* m_pWndConfirmTrade;
	CWndTradeConfirm* m_pWndTradeConfirm;
	CWndCommand* m_pWndCommand;
	CWndBank* m_pWndBank;
	CWndWorld* m_pWndWorld;
#ifdef __JEFF_11
	CWndPetRes* m_pPetRes;
#endif
#ifdef __EVE_MINIGAME
	CWndKawiBawiBoGame* m_pWndKawiBawiBoGame;
	CWndKawiBawiBoGameConfirm* m_pWndKawiBawiBoGameConfirm;
#endif //__EVE_MINIGAME

#if __VER >= 9 // __CSC_VER9_1
	CWndMixJewel* m_pWndSmeltMixJewel;
	CWndSmeltJewel* m_pWndSmeltJewel;
	CWndExtraction* m_pWndExtraction;
	CWndPetStatus* m_pWndPetStatus;

	CWndFoodConfirm* m_pWndFoodConfirm;
	CWndPetMiracle* m_pWndPetMiracle;
	CWndPetFoodMill* m_pWndPetFoodMill;
	CWndPetLifeConfirm* m_pWndPetLifeConfirm;
#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND
	CWndHeroSkillUp* m_pWndHeroSkillUp;
#endif //__CSC_VER9_1 -> __LEGEND
#endif //__CSC_VER9_1
#if __VER >= 11 // __PIERCING_REMOVE
	CWndRemovePiercing* m_pWndRemovePiercing;
#endif //__PIERCING_REMOVE
#ifdef __TRADESYS
	CWndDialogEvent* m_pWndDialogEvent;
#endif //__TRADESYS
#if __VER >= 12 // __HEAVEN_TOWER
	CWndHeavenTower* m_pWndHeavenTower;
#endif //__HEAVEN_TOWER
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	CWndChatLog* m_pWndChatLog;
#endif //__Y_CHAT_SYSTEM_8

#if __VER >= 10 // __REMOVE_ATTRIBUTE
	CWndRemoveAttribute* m_pWndRemoveAttribute;
#endif //__REMOVE_ATTRIBUTE
	CWndPost* m_pWndPost;
	CWndQueryEquip* m_pWndQueryEquip;
	CWndSelectVillage* m_pWndSelectVillage;
	CWndQuestItemWarning* m_pWndQuestItemWarning;
	CWndChangeName* m_pWndChangeName;
	CWndDropConfirm* m_pWndDropConfirm;
	CWndUpgradeBase* m_pWndUpgradeBase;

	CWndInvenRemoveItem* m_pWndInvenRemoveItem;

	CWndConfirmBank* m_pWndConfirmBank;
	CWndBankPassword* m_pWndBankPassword;
	CWndRankGuild* m_pWndRankGuild;
	CWndRankInfo* m_pWndRankInfo;
	CWndGuildBank* m_pWndGuildBank;

#ifdef __MAIL_REQUESTING_BOX
	BOOL	m_bWaitRequestMail;
#endif

	CWndGuildConfirm* m_pWndGuildConfirm;
	CWndGuildWarPeaceConfirm* m_pWndGuildWarPeaceConfirm;
	CWndGuildWarRequest* m_pWndGuildWarRequest;
	CWndGuildNickName* m_pWndGuildNickName;
	CWndGuildCombatRank_Person* m_pWndGuildCombatRanking;
	CWndGuildCombatState* m_pWndGuildWarState;
	CWndGuildCombatOffer* m_pWndGuildCombatOffer;
	CWndGuildCombatSelection* m_pWndGuildCombatSelection;
	CWndGuildCombatJoinSelection* m_pWndGuildCombatJoinSelection;
	CGuildCombatInfoMessageBox2* m_pWndGuildCombatInfoMessageBox2;
	CGuildCombatInfoMessageBox* m_pWndGuildCombatInfoMessageBox;
	CWndGuildCombatResult* n_pWndGuildCombatResult;

	CCollectingWnd* m_pWndCollecting;
#if __VER >= 11 // __SYS_IDENTIFY
	CWndLvReqDown* m_pWndLvReqDown;
	CWndBlessingCancel* m_pWndBlessingCancel;
#endif
#if __VER >= 12 // __LORD
	CWndLordTender* m_pWndLordTender;
	CWndLordVote* m_pWndLordVote;
	CWndLordState* m_pWndLordState;
	CWndLordEvent* m_pWndLordEvent;
	CWndLordSkillConfirm* m_pWndLordSkillConfirm;
	CWndLordInfo* m_pWndLordInfo;
	CWndLordRPInfo* m_pWndLordRPInfo;
	bool						m_bIsOpenLordSkill;
#endif
#if __VER >= 12 // __SECRET_ROOM
	CWndSecretRoomInfoMsgBox* m_pWndSecretRoomMsg;
	CWndSecretRoomSelection* m_pWndSecretRoomSelection;
	CWndSecretRoomOffer* m_pWndSecretRoomOffer;
	CWndSecretRoomOfferState* m_pWndSecretRoomOfferState;
	CWndSecretRoomChangeTaxRate* m_pWndSecretRoomChangeTaxRate;
	CWndSecretRoomCheckTaxRate* m_pWndSecretRoomCheckTaxRate;
	CWndSecretRoomBoard* m_pWndSecretRoomBoard;
	CWndSecretRoomQuick* m_pWndSecretRoomQuick;
#endif //__SECRET_ROOM
#if __VER >= 12 // __PET_0519
	CWndPetAwakCancel* m_pWndPetAwakCancel;
#endif
#ifdef __AZRIA_1023
	CWndSelectCh* m_pWndSelectCh;
#endif
#if __VER >= 13 // __CSC_VER13_2
	CWndCoupleMessage* m_pWndCoupleMessage;
#endif //__CSC_VER13_2

	CWndReSkillWarning* m_pWndReSkillWarning;
	CWndFontEdit* m_pWndFontEdit;

	CWndPartyChangeName* m_pWndPartyChangeName;
	CWndPartyConfirm* m_pWndPartyConfirm;

	CWndPartyLeaveConfirm* m_pWndPartyLeaveConfirm;
	CWndFriendConFirm* m_pWndFriendConfirm;

	CWndAddFriend* m_pWndAddFriend;
	CWndStateConfirm* m_pWndStateConfirm;
	CWndCloseExistingConnection* m_pWndCloseExistingConnection;

	CWndRandomScrollConfirm* m_pWndRandomScrollConfirm;

#ifdef __HELP_BUG_FIX
	CWndHelpInstant* m_pWndHelpInstant;
#endif //__HELP_BUG_FIX
#if __VER >= 13 // __HOUSING
	CWndRoomList* m_pWndRoomList;
	CWndQuitRoom* m_pWndQuitRoom;
#endif // __HOUSING
#ifdef __PET_1024
	CWndChangePetName* m_pWndChangePetName;
#endif
#ifdef __S_SERVER_UNIFY
	BOOL							m_bAllAction;
#endif // __S_SERVER_UNIFY

#if __VER >= 14 // __SMELT_SAFETY
	CWndSmeltSafety* m_pWndSmeltSafety;
	CWndSmeltSafetyConfirm* m_pWndSmeltSafetyConfirm;
#endif //__SMELT_SAFETY

#if __VER >= 14 // __RESTATE_CONFIRM
	CWndRestateConfirm* m_pWndRestateConfirm;
#endif // __RESTATE_CONFIRM

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CWndQuestQuickInfo* m_pWndQuestQuickInfo;
	CWndQuestDetail* m_pWndQuestDetail;
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __PETVIS
	CWndBuffPetStatus* m_pWndBuffPetStatus;
	CWndConfirmVis* m_pWndConfirmVis;
#endif

#if __VER >= 15 // __GUILD_HOUSE
	CWndGHMainMenu* m_pWndGHMain;
	CWndGHUpkeep* m_pWndUpkeep;
#endif

#ifdef __PROTECT_AWAKE
	CWndSelectAwakeCase* m_pWndSelectAwakeCase;
#endif

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	CWndChattingBlockingList* m_pWndChattingBlockingList;
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __FL_DUNGEON_TREASURE
	CWndDungeonTreasure* m_bDungeonTreasure;
#endif

#ifdef __FL_EQUALIZED_MODE
	CWndSelectTmpClass* m_WndSelectTmpClass;
#endif

#ifdef __FL_NEW_BOXES
	CWndNewBoxOpen* m_pWndNewBoxOpen;
#endif
	CWndGuildName* m_pWndGuildName;
	CWndLookChange* m_pWndLookChange;
	CWndSecretRoomCancelConfirm* m_pWndSecretRoomCancelConfirm;
	CWndSummonPartyUse* m_pWndSummonPartyUse;
	CWndItemTransy* m_pWndItemTransy;
	CWndGuildWarJoinConfirm* m_pWndGuildWarJoinConfirm;
	CWndGuildWarCancelConfirm* m_pWndGuildWarCancelConfirm;
	CWndChangeSex* m_pWndChangeSex;
	CWndSummonParty* m_pWndSummonParty;
	CWndIndirectTalk* m_pWndIndirectTalk;
	CWndSummonPartyMsg* m_pWndSummonPartyMsg;
	CWndSummonFriendMsg* m_pWndSummonFriendMsg;
	CWndSummonFriend* m_pWndSummonFriend;
#ifdef __FL_PACKED_ITEMS
	CWndPackedItemsPreview* m_pWndPackagePreview;
#endif

	CTexturePack  m_texture;
	CTexturePack  m_texCommand;
	CTexturePack  m_texIcon;
	CTexturePack  m_texWnd;


	CWndMessageBox* m_pWndMessageBox;
	CWndMessageBoxUpper* m_pWndMessageBoxUpper;

	CWndTaskBar* m_pWndTaskBar;
	CWndTaskMenu* m_pWndMenu;
	CWndRevival* m_pWndRevival;
	CWndResurrectionConfirm* m_pWndResurrectionConfirm;
	CWndLogOutWaitting* m_pLogOutWaitting;
	CWndCommItemDlg* m_pWndCommItemDlg;

	CMapDWordToPtr	m_mapWndApplet;
	CMapDWordToPtr  m_mapWndRegInfo;

	void PutDefinedString(DWORD dwText, ...);
	void PutString(LPCTSTR lpszString, CObj* pObj = NULL, DWORD dwColor = 0xffffffff, DWORD dwChatStyle = CHATSTY_GAME, DWORD dwPStyle = 0x00000001);
	void ParsingChat(CString string);
	void WordChange(CString& rString);

	BOOL	IsTitle() { return m_bTitle; }

	void	OpenTitle(BOOL bFirstTime = FALSE);
	void	OpenField();

	BOOL	OpenTaskBar();
	BOOL	OpenMenu();

	void	ObjectExecutor(DWORD dwShortcut, DWORD dwId, DWORD dwType = 0);
	void	ObjectExecutor(LPSHORTCUT lpShortcut);
	void	UseSkillShortCut(DWORD dwType, DWORD dwSkillIdx);

	// map
	CCollectingWnd* OpenCollecting();
	BOOL CloseCollecting();

	// massenger
	CWndMessage* GetMessagge(LPCTSTR lpszFrom);
	CWndMessage* OpenMessage(LPCTSTR lpszFrom);
	BOOL	UpdateMessage(LPCTSTR pszOld, LPCTSTR pszNew);
	CWndInstantMsg* GetInstantMsg(LPCTSTR lpszFrom);
	CWndInstantMsg* OpenInstantMsg(LPCTSTR lpszFrom);

	// MessageBox
	BOOL	OpenCustomBox(LPCTSTR strMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent = NULL);
	BOOL	OpenMessageBox(LPCTSTR strMessage, UINT nType = MB_OK, CWndBase* pWndParent = NULL);
	BOOL	OpenMessageBoxWithTitle(LPCTSTR lpszMessage, const CString& strTitle, UINT nType = MB_OK, CWndBase* pWndParent = NULL);
	BOOL    OpenMessageBoxUpper(LPCTSTR lpszMessage, UINT nType = MB_OK, BOOL bPostLogoutMsg = FALSE);

	void	CloseMessageBox();

	// tooltip
	void PutToolTip_Troupe(DWORD dwSkill, CPoint point, CRect* pRect);
	void PutToolTip_Skill(DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive = TRUE);
	void PutToolTip_Item(DWORD dwType, DWORD dwId, int nLocation, CPoint point, CRect* pRect, int flag = 0);
	void PutToolTip_Item(CItemElem* pItemBase, CPoint point, CRect* pRect, int flag = 0);
	enum { TOOL_TIP_SWITCH_MAIN = 0, TOOL_TIP_SWITCH_SUB1 = 1, TOOL_TIP_SWITCH_SUB2 = 2 };
	void MakeToolTipText(CItemElem* pItemBase, ItemProp* pItemProp, CEditString& strEdit, int flag = 0);
	void PutToolTip_Character(int SelectCharacter, CPoint point, CRect* pRect);

	BOOL IsDst_Rate(int nDstParam);

	DWORD PutItemName(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutItemAbilityPiercing(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit, DWORD dwColorBuf);
	void PutItemResist(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutItemSpeed(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutItemMinMax(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutRandomOpt(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutAwakeningBlessing(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutPiercingOpt(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutEnchantOpt(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit, int flag);
	void PutSetItemOpt(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutBaseItemOpt(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	BOOL PutMedicine(ItemProp* pItemProp, CEditString* pEdit);
	void PutItemGold(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit, int flag);
	void PutCoolTime(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutKeepTime(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutCommand(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit, int nFlag);
	void PutSex(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutJob(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutLevel(CMover* pMover, CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutWeapon(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);

	void PutAddedOpt(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutPetInfo(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutPetFeedPocket(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);

	void PutVisPetInfo(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
	void PutPetKind(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);

	void PutEquipItemText(CEditString* pEdit);

	BOOL PutDestParam(ItemProp* pItemProp, AddSkillProp* pAddSkillProp, CEditString* pEdit);
	BOOL PutSocketCard(ItemProp* pItemProp, CEditString* pEdit);
	BOOL PutFurniture(ItemProp* pItemProp, CEditString* pEdit);
	BOOL PutBeadInfo(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);

#ifdef __MODEL_CHANGE
	void PutLookText(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
#endif
#ifdef __FL_BOXES_V2
	void PutBoxDetails(ItemProp* pProp, CEditString* pEdit, int nFlag);
#endif
#ifdef __FL_PACKED_ITEMS
	void PutPackage(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit, int nFlag);
	void PreviewPackage(CItemElem* pItemElem);
#endif
#ifdef __FL_MONSTER_ALBUM
	void PutMonsterCard(ItemProp* pItemProp, CEditString* pEdit);
	void PutBossCard(ItemProp* pItemProp, CEditString* pEdit);
#endif
#ifdef __MODEL_VIEW
	BOOL IsModelView(ItemProp* pProp);
	void PutModelView(ItemProp* pProp, CEditString* pEdit, int nFlag);
	bool PreviewModel(CItemElem* pItemElem);
#endif
#ifdef __FL_VENDORS
	BOOL PutVendorItem(CItemElem* pItemElem, ItemProp* pItemProp, CEditString* pEdit);
#endif
	void StringSize(CString& strSrc, int nLetters);
	void UseItem(CItemElem* pItemBase, int nType);
	BOOL UseItem_RestrictedWnd();
	BOOL UseItem_MaterialWnd(CItemElem* pItemElem, ItemProp* pProp);
	BOOL UseItem_MaterialWnd2(CItemElem* pItemElem, ItemProp* pProp);
	BOOL UseItem_SpecificSituations(CItemElem* pItemElem, ItemProp* pProp);
	void RenderObjectViewport(C2DRender* p2DRender, CModelObject* m_pModel, CMover* pMover, LPWNDCTRL lpFace, VIEWPORT* data, int nWindowID);
	void PrepareMoverModel(CMover* pMover, CModelObject* pModel);
	void UpdateWindowModels();
	BOOL IsWindowProcessState();
#ifdef __FL_FIX_SHORTCUTCRASH
	void CleanGlobalShortcut(CItemElem* pItemElem);
#endif
	int GetContainerTypeByWindowId(int nWindowId, int nZoneId);
	CString GetStringNameByContainerType(int nType);
	void MakeNumberSmooth(int* nCurVal, int* nOldVal);
	void MakeNumberSmooth64(__int64* nCurVal, __int64* nOldVal);

	CWndMgr();
	virtual	~CWndMgr();

	void	DestroyApplet();
	void	AlighWindow(CRect rcBndOld, CRect rcBndNew, CRect rcOld, CRect rcNew);

	//virtual CItem* GetFocusItem();
	virtual	BOOL    Process();
	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL);
	virtual	void	OnInitialUpdate();
	virtual	void	OnDestroyChildWnd(CWndBase* pWndChild);
	virtual	void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void	OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL	OnCommand(UINT nID, DWORD dwMessage);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual BOOL    OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual	BOOL	OnEraseBkgnd(C2DRender* p2DRender);
	virtual	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	DWORD	GetAppletId(TCHAR* lpszAppletName);
	CWndBase* GetApplet(DWORD dwIdApplet);
	CWndBase* CreateApplet(DWORD dwIdApplet);
	void	AddAppletFunc(LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszKeyword, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc = NULL, CHAR cHotkey = 0);

	HRESULT	RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

	//	Initialize player
	void SetPlayer(CMover* pMover);

	BOOL LoadRegInfo(LPCTSTR lpszFileName);
	BOOL SaveRegInfo(LPCTSTR lpszFileName);
	LPWNDREGINFO GetRegInfo(DWORD dwWndId);
	BOOL PutRegInfo(DWORD dwWndId, CRect rect, BOOL bOpen);
	BOOL PutRegInfo(LPWNDREGINFO lpRegInfo);
	BOOL PutRegInfo(CWndNeuz* pWndNeuz, BOOL bOpen);

#if __VER >= 8 //__CSC_VER8_1
	void ClearAllWnd();
	void RestoreWnd();
#endif //__CSC_VER8_1

#if __VER >= 12 // __CSC_VER12_4
	BOOL IsDstRate(int nDstParam);
	char* CWndMgr::FindDstString(int nDstParam);
#endif //__CSC_VER12_4

#ifdef __WINDOW_INTERFACE_BUG
	void CloseBoundWindow(void);
#endif // __WINDOW_INTERFACE_BUG

	friend CWndApplet;
protected:
	void	FormalizeChatString(CString& str);
};

#ifdef __VS2003
extern CPtrArray      m_wndOrder;
#endif

extern CWndMgr          g_WndMng;

extern void RenderEnchant(C2DRender* p2DRender, CPoint pt);
extern void RenderRadar(C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor);
#define WID_MESSAGEBOX        10

#endif // !defined(AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
