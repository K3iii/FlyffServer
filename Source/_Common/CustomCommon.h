// Config
#define	__VER 18							        //| "Version"
#define __LOCAL_TESTS TRUE					        //| TRUE = Local | FALSE = Live 
#define __NEUZ_IPSET __LOCAL_TESTS == TRUE ? "127.0.0.1" : "123.123.123.123"		 
#define	__NEUZ_MSGVR "f14538dg1h26z"				//| Patch Version
#define	__NEUZ_TITLE "FlyFF"				        //| Neuz Wnd Title
#define __NEUZ_PHASH "kikugalanet"			        //| Password hash
#define	__NEUZ_BOOTS __LOCAL_TESTS == TRUE ? "" : "sunkist"		
#define	__MAX_SPEED	 200					        //| Max speed

// Systems & Features
#define __FL_TDM									//| Team vs Team pvp mode
#define __FL_MONSTER_ALBUM							//| Monster Album System
#define __FL_RECORD_BOOK							//| Recordbook
#define __FL_WHEEL_OF_FORTUNE						//| Wheel of fortune
#define __FL_PACKED_ITEMS							//| Ability to pack items into items 
#define __FL_FARM_DISPLAY							//| Remade old farm counter system
#define __FL_GUILD_FINDER							//| Guild Finder
#define __FL_SWITCH_EQ								//| Self made switch equipment system
#define __FL_CASH_SHOP								//| Donate/Vote Shop
#define __FL_PENYA_CONVERTER						//| "Smart" and manual penya -> perins converter
#define __FL_NEW_BOXES								//| Boxes with ability to select item
#define __FL_VENDORS 1								//| Offline Vendors v3
#define __FL_PARTY_FINDER							//| Party Finder
#define __FL_SOLO_DUNGEONS					        //| Solo dungeons
#define __FL_RECYCLE II_SYS_SYS_EVE_TICKECT01       //| System for convert items to specific currency
#define __FL_TELEPORT						        //| Teleporter System
#define __FL_FFA							        //| FFA system
#define __FL_DAILY_REWARDS 					        //| [TODO: Notify Online Players] Daily Rewards
#define __FL_GW_STATUE_V2					        //| Guild and MvP Statue v2
#define __FL_PET_FILTER						        //| New petfilter with more filters + possibility to loot items in bag + presets
#define __FL_SHOP_SEARCH					        //| Shop Search System
#define __FL_DPS_DISPLAY					        //| DPS Display
#define __FL_SAVE_MESSAGES					        //| Save all private messages while client is active
#define __FL_CONSUMABLES_STORAGE			        //| New item container for power-ups/consumables
#define __FL_DUNGEON_TREASURE				        //| Dungeon treasure system
#define __FL_BATTLE_PASS					        //| Battle Pass System
#define __FL_BOXES_V2						        //| Box Preview & Bulk Opening
#define __FL_PARTY_SORT						        //| Party Sorting
#define __FL_FAST_ELEMENT II_SYS_SYS_SCR_SOKCHANG	//| Fast Element Changer
#define __FL_EQUALIZED_MODE					        //| [TODO: rework for optimising memory] Equalized System
#define __MODEL_HIDEFASH					        //| [Main code only] Hide Fashion option
#define __MODEL_CHANGE						        //| [Main code only] Model Change
#ifdef __CLIENT
#define	__MODEL_CACHE 5						        //| NaNo Model Cache
#endif
#define __MODEL_VIEW						        //| Public model viewer
#define __CONTAINER_RCLICKS					        //| Right clicks transfer items between inventory <--> containers
#define __CONTAINER_SORTING							//| TODO: recode this system to avoid packets-spam + make it universal for all containers. v21 version - exploitable
#define __INVENTORY_168_SLOTS				        //| 168 Slots in inventory
#define __INVENTORY_RESIZE					        //| Inventory Resize
#define __WIKI_ITEMS_V2						        //| Based on public wiki, but re-written with more filters etc
#define __JOB_CHANGE						        //| Public job changer with some edits, dont use other job-changers if you dont know what you do

// Fixes & Addons
#define __ATTR_HIT_FIX								//| Experimental fix from old source to not skip attack in case if animation speed is too big + increased range attacks speed
#define __FL_DST_ATKPOWER_RATE_MAGIC				//| DST_ATKPOWER_RATE_MAGIC first and experimental dst which mostly added to balance psykeepers/mentakists
#define __FL_FIX_LINKMAP_LEVEL						//| Experimental fix
#define __FL_FIX_USELESS_LOGS						//| Disable useless logs
#define __FL_FIX_SHORTCUTCRASH						//| Fix related to unknown itemelem in globalshortcut data
#define __FL_FIX_LOADING							//| Not allow to start new instance while old window not loaded completely
#define __FL_FIX_PLAYERDATA							//| Several fixes for playerdata
#define __FL_FIX_IK2BUFF2					        //| Now possible to add stats on IK2_BUFF2 items
#define __FL_FIX_PROPER_GUILD_COOLDOWN		        //| Proper guild cooldown removal
#define __FL_FIX_IDCHANGE_EXPLOIT			        //| Defined fixes for exploit which allow to change ID of character and be invited to any messenger/guild/party without invite
#define __FL_FIX_MEMORY_LEAKS				        //| Some code to prevent possible client memory leaks
#define __FL_FIX_BLADEHACK					        //| Blade hack fix + other code for test
#define __FL_FIX_MAILVALUES					        //| Added missing values (+ MAIL_STR/TBL)
#define __FL_UPDATE_TASKBAR					        //| Static taskbar at centre + switch F1-F9 keys + click effects etc
#define __FL_UPDATE_REAGGRO					        //| Re-aggro by any kind of damage + other updates
#define __FL_UPDATE_CHATTING				        //| CD on worldserver side + possibility to use DOWN button to see next message in history
#define __FL_UPDATE_LORDEVENT				        //| Penya event in lord system
#define __FL_UPDATE_READSTAFF				        //| Improved reading admin/gm characters from file.
#define __FL_UPDATE_PROJECTILES				        //| Increase most projectiles speed
#define __FL_UPDATE_GROUNDITEMS				        //| Name rendering for ground items with some additional updates
#define __AIO										//| Random snippets for quick access
#define __AIO_NEUZAPP						        //| Random snippets for quick access (Neuz)
#define __AIO_PETS							        //| Random snippets for quick access (Pets)
#define __AIO_ADMIN_THINGS					        //| Random snippets for quick access (Staff)
#define __AIO_POSI_RELATED					        //| All test modifications which somehow related to movement/position/casting/etc
#define __FIX_SQL_INJECTS					        //| Protections against broken text which can cause sql inject or just sql related problems
#define __FIX_SRLZ_STATS					        //| Serialize some DST world->client when object added in view
#define __FIX_NPC_GENDER					        //| Fix for male custom NPC
#define __FIX_STUNBUG								//| [Main code only] Defined fixes against stun-bug
#define __NPC_SETVENDORTYPE					        //| SetVenderType for NPC
#define __NPC_ADDSHOPITEM					        //| AddShopItem for NPC
#define __NPC_ADDMENU2						        //| AddMenu2 for NPC
#define __TEST_ANISOTROPIC					        //| Some random Anisotropy
#define __TEST_ANTIALIASING					        //| Some random Antialiasing
#define __TEST_PACKET_CRYPT					        //| Some random Packet Cryptage

// Classic Content & Other
#define __V21_CLOSE_EXISTING_CONNECTION				//| Actually should fix rare problem when you cant disconnect your own account
#define __MINIDUMP									//| .dmp generator (neuz/world)
//#define __BUGSPLAT								//| Bugsplat
//#define __DISCORD							        //| Discord RPC (credits to kotori for base)
//#define __VALID_CONNECTION_CONFORMATION			//| From Lykan src
//#define __AI_SYSTEM								//| For testing purposes ONLY