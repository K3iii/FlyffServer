#ifndef __DEFINEATTRIBUTE
#define __DEFINEATTRIBUTE

// ���� ������ Ÿ�� 
#define VT_ITEM                      1
#define VT_SKILL                     2
/*
#define CT_START     1
#define CT_CIRCLE    2
#define CT_FINISH    3 
#define CT_COUNTER   4
#define CT_BOOSTER   5 
#define CT_FG        6
*/

#define CT_STEP      1 // ���� CT_START
#define CT_CIRCLE    2
#define CT_FINISH    3
#define CT_GENERAL   4 // ���� CT_FG


// ItemSex
//#define IS_MALE                      1 // ����������
//#define IS_FEMALE                    2 // �����ܿ���

// Handed
#define HD_ONE                       1 // �Ѽ�����
#define HD_TWO                       2 // ������� 
#define HD_DUAL                      3 // Two-Weapon

// AtkType
#define AT_SLASH                     1 // ����
#define AT_BLOW                      2 // ġ��
#define AT_PIERCE                    3 // ���

// AtkStyle
#define AS_HORIZONTAL                0 // ���� �ֵθ���
#define AS_VERTICAL                  1 // ���� �ֵθ���
#define AS_DIAGONAL                  2 // �缱 �ֵθ���
#define AS_THRUST                    3 // ���
#define AS_HEAD                      4 // �Ӹ�(�����κ�)
#define AS_CHEST                     5 // ����(����)
#define AS_ARM                       6 // ��(���ݺ���)
#define AS_LEG                       7 // �ٸ�(�̵�����)
#define AS_BACK                      8 // �ڿ��� ����

#define MAX_AS                       9 //

// WeaponType
#define WT_MELEE_SWD                 1 // Į�� ����� �⺻ ���ϰ��ݼ���
#define WT_MELEE_AXE                 2 // ������ ����� �⺻ ���ϰ��ݼ���
#define WT_MELEE_STICK               3 // ġ�ƽ�� ����� �⺻ ���ϰ��ݼ���
#define WT_MELEE_KNUCKLE             4 // ��Ŭ�ظӸ� ����� �⺻ ���ϰ��ݼ���
#define WT_MELEE_STAFF               5 // �������� ����� �⺻ ���ϰ��ݼ���
#define WT_MAGIC_WAND                6 // �ϵ带 ����� �⺻ ���ϰ��ݼ���

/* 20050329 �����߰� */
#define WT_MELEE_YOYO               20 // ��並 ����� �⺻ ���ϰ��ݼ���
#define WT_RANGE_BOW                21 // Ȱ�� ����� �⺻ ���ϰ��ݼ���

#define WT_MELEE                     7 // �⺻ ���� �������ݿ� ����(Str)
#define WT_RANGE                     8 // �⺻ ���� ��Ÿ����ݿ� ����(Dex)
#define WT_MAGIC                     9 // �⺻ ���� �������ݿ� ����(Int)
#define WT_CHEER                    10 // �⺻ ���� ġ�ƽ ���� ����(*Sta, Int)
#define WT_GUN                      11 // �⺻ ���� ���� ����� ���ݿ� ����(Sta,*Dex)
#define WT_DOLL                     12 // �⺻ ���� �������ݿ� ����(*Sta,Dex)
#define WT_EQUIP                    13 // ������ ��� ���ؼ� ����� ��������(���)
#define WT_PROPERTY                 14 // �ҵ��� ��� ���� ��������(���, Dex)
#define WT_ACROBAT                  15 // ���� ���� ��������(���, Dex)
#define WT_THROWITEM                16 // ���¿��� ������ ������ ������� ��������(������ ����, Sta, Dex)
#define WT_THROWING                 17 // ���¿��� ����,�� ������ ������� ��������(���, Sta, Dex)
#define WT_SWING                    18 // ���¿��� �����ֵθ��� ������� ��������(���, Sta, Dex)
#define WT_KNUCKLE                  19 // ���¿��� �����ֵθ��� ������� ��������(���, Sta, Dex)

//�Ӽ� Ÿ��

#define _NONE			0 //���Ӽ�
#define _FIRE			1 //�ҼӼ�
#define _WATER			2 //���Ӽ�
#define _ELECTRICITY		3 //����Ӽ�
#define _WIND			4 //�ٶ��Ӽ�
#define _EARTH			5 //���Ӽ�

// AttackRange
#define AR_SHORT                     1 // �Ѽտ� ������ ����
#define AR_LONG                      2 // ��տ� ������ ����
#define AR_FAR                       3 // ä��, â�� ���� ����
#define AR_RANGE                     4 // ��� ����
#define AR_WAND                      5 // �ϵ� �Ÿ�
#define AR_HRANGE                    6 // ��� ����
#define AR_HWAND                      7 // ��ϵ� �Ÿ�

// SpellAttribute
#define SA_DIRECTDMG                 1 // ���� ������
#define SA_OBJCHGPARAMET             2 // ����� �Ķ���� ����
#define SA_SELFCHGPARAMET            3 // �ڽ��� �Ķ���� ����
#define SA_OTHERS                    4 // ��Ÿ

//	SpellType
#define ST_MAGIC                     1 // �Ϲ� ����
#define ST_MIND                      2 // ���� �迭
#define ST_POISON                    3 // �� �迭
#define ST_ELECTRICITY               4 // ���� �迭
#define ST_FIRE                      5 // �� �迭
#define ST_WIND                      6 // �ٶ� �迭
#define ST_WATER                     7 // �� �迭
#define ST_EARTH                     8 // �� �迭
#define ST_DARK                      9 // ��� �迭
#define ST_LIGHT                    10 // �� �迭
#define ST_FIREEARTH                11 // ��/�� �迭
#define ST_ELECWIND                 12 // ����/�ٶ� �迭
#define ST_EARTHWIND                13 // ��/�ٶ� �迭
#define ST_EARTHWATER               14 // ��/�� �迭

// Skill Ready ����غ�ð�
#define SR_AFTER                     1 // ��� ����Ŀ� ���ð� �ߵ�
#define SR_BEFORE                    2 // ��� ������� ���ð� �ߵ�  

//	Spell Region
#define SRO_DIRECT                   1 // ���� ��� 100%
#define SRO_REGION                   2 // ����� �߽�(��� 100%, ���� 100%)
#define SRO_EXTENT                   3 // ����� �߽�(��� 100%, ���� �ּ� ������)
#define SRO_SURROUND                 4 // ����� �߽�(��� �ּ�, ���� 100%)
#define SRO_DOUBLE                   5 // 100% ������
#define SRO_LINE                     6 // ���� ���� �Ÿ��� �ִ� ����(��� 100%, ���� 100%)
#define SRO_AROUND                   7 // ����� �߽�(��� 0%, ���� 100%)
#define SRO_TROUPE					 8 // ��Ƽ������ ���.

//	Skill Type
#define KT_MAGIC                     1 // 
#define KT_SKILL                     2 // 

// CardType
#define CT_TELEPORT                  1 // Ư�� �������� �̵�
#define CT_SUMMON                    2 // Ư�� ����� ��ȯ

// Race
#define RACE_HUMAN                   1 // �ΰ�
#define RACE_ANIMAL                  2 // ����
#define RACE_HUMANOID                3 // �ΰ� ����
#define RACE_MONSTER                 4 // �Ϲ� ����
#define RACE_UNDEAD                  5 // �𵥵�
#define RACE_GHOST                   6 // ��ü�� ���� ����
#define RACE_INSECT                  7 // ����
#define RACE_MECHANIC                8 // ���� �۵��ϴ� ����ü
#define RACE_ELEMENTAL               9 // ���Ұ� ����ü


// Size
#define SIZE_TINY                    1 // 
#define SIZE_SMALL                   2 // 
#define SIZE_MIDDLE                  3 // 
#define SIZE_NORMAL                  4 // 
#define SIZE_TALL                    5 // 
#define SIZE_BIG                     6 // 
#define SIZE_GAINT                   7 // 

// dwClass 
#define RANK_LOW                     1 //
#define RANK_NORMAL                  2 //
#define RANK_CAPTAIN                 3 //
#define RANK_BOSS                    4 //
#define RANK_MIDBOSS				 5 // ��ӽ�,ũ����,�ӽ���Ʈ �߰�������
#define RANK_MATERIAL                6 //
#define RANK_SUPER                   7 //
#define RANK_GUARD                   8 //

// Character State(ĳ���� ���º���)
#define CHS_NORMAL				0			// �Ϲ�
#define CHS_GUARDARROW				0x00000001 // ȭ���� ���� ����
#define CHS_GUARDBULLET				0x00000002 // �Ѿ��� ���� ����
#define CHS_GROGGY				0x00000004 // Ÿ�ݹ��� ����(��!!)
#define CHS_STUN				0x00000008 // ������ ���� - nProbability:Ȯ�� dwSkillTime:���ӽð�
#define CHS_ANOMY				0x00000010 // ������ ȥ������(��!!)
#define CHS_STARVE				0x00000020 // ���ָ� ����
#define CHS_PLASYARM				0x00000040 // �ն������� ���� ���ݺҴɻ���
#define CHS_MISSING				0x00000080 // Ư���� ��Ȳ�� ���� ������� ����
#define CHS_DARK				0x00000100 // ���߷� ���� nProbability:Ȯ�� dwDestData2:������ ����ġ.
#define CHS_LITHOSKIN				0x00000200 // ����ϴ� ���� ���� �Դ´�. ���� �ɰ��� ���ظ� �ش�.
#define CHS_INVISIBILITY			0x00000400 // ����ϴ� ���� ���� �Դ´�. ���� �ɰ��� ���ظ� �ش�.
#define CHS_POISON				0x00000800 // ���ɸ� ����. nProbability:Ȯ��(CHANCE�� ����Ѱ��̸� =����)  dwPainTime:���ʸ���?  nDestData1 : ƽ�絥����: dwSkilltime:���ӽð�
#define CHS_SLOW				0x00001000 // ������ ����.
#define CHS_DMGREFLECT				0x00002000 // ������ �ݻ�. ���÷���Ʈ�� �̰ɷ� ���Ѵ�. �������°� �ƴϱ� ����
#define CHS_DOUBLE				0x00004000 // 2�� ������ .
#define CHS_BLEEDING			0x00008000 // ���� : ������ ���� ����
#define CHS_SILENT				0x00010000 // ħ��
#define CHS_DMG_COUNTERATTACK	0x00020000 // �ݰ� ����� : ���ݴ������� ������Ȯ��(destData1)�� �����Ƚ�ų(destData2)�� �ߵ���.
#define CHS_ATK_COUNTERATTACK	0x00040000 // �ݰ� ����� : ���ݴ������� ������Ȯ��(destData1)�� �����Ƚ�ų(destData2)�� �ߵ���.
#define CHS_LOOT				0x00080000	// �̵��Ұ� ����.

#define CHS_SETSTONE				0x00100000	// ��ȭ ����.

#define CHS_DEBUFFALL				0x00200000	// ��� ���¿��� ����.


// ������ ������ ����...
#define CHS_DARK_POISON				0x00000900	// ���� ����
#define CHS_DARK_POISON_STUN		0x00000908	// ���� ����� ���� 
#define CHS_LOOT_SLOW				0x00081000	// �̵���ֻ��� ������(��,���׾�,���ο��)
#define	CHS_DARK_POISON_STUN_BLEEDING	0x00008908	// ��, ����, ����, ����
#define CHS_DARK_POISON_STUN_BLEEDING_DEBUFFALL				0x0031d908	// ��� ���¿��� ����.��, ����, ����, ����,CHS_SILENT,CHS_SETSTONE,CHS_DOUBLE,CHS_SLOW

// Belligerence
#define BELLI_PEACEFUL                1 // ��ȭ - ���� �޾Ƶ� �ݰ� ���� 
#define BELLI_CAUTIOUSATTACK          2 // ���� ������ �ݰ�
#define BELLI_ACTIVEATTACK            3 // ������ ���� 
#define BELLI_ALLIANCE                4
#define BELLI_ACTIVEATTACK_MELEE2X    5 // ������ �����ϴ� �������ݸ� ���� ĳ��(������ ����)
#define BELLI_ACTIVEATTACK_MELEE      6 // ������ �����ϴ� ����/ ��Ÿ��� ���� ĳ��(���� �߽�)
#define BELLI_ACTIVEATTACK_RANGE      7 // ������ �����ϴ� ��Ÿ�/ ������ ���� ĳ��(��Ÿ� �߽�)
#define BELLI_CAUTIOUSATTACK_MELEE2X  8 // ���ݹ����� �ݰ��ϴ� �������ݸ� ���� ĳ��(������ ����)
#define BELLI_CAUTIOUSATTACK_MELEE    9 // ���ݹ����� �ݰ��ϴ� ����/ ��Ÿ��� ���� ĳ��(���� �߽�)
#define BELLI_CAUTIOUSATTACK_RANGE   10 // ���ݹ����� �ݰ��ϴ� ��Ÿ�/ ������ ���� ĳ��(��Ÿ� �߽�)
#define BELLI_MELEE2X                11 // ������ �����ϴ� �������ݸ� ���� ĳ��(������ ����)
#define BELLI_MELEE                  12 // ������ �����ϴ� ����/ ��Ÿ��� ���� ĳ��(���� �߽�)
#define BELLI_RANGE                  13 // ������ �����ϴ� ��Ÿ�/ ������ ���� ĳ��(��Ÿ� �߽�)

// BloodColor
#define BLOOD_RED                    1
#define BLOOD_GREEN                  2
#define BLOOD_BLUE                   3
#define BLOOD_WHITE                  4
#define BLOOD_BLACK                  5

// Shelter
#define SHELTER_BASICZONE            1
#define SHELTER_TESTZONE             2
#define SHELTER_GRASS                3

// dwSfxElemental
#define ELEMENTAL_FIRE               1
#define ELEMENTAL_WATER              2
#define ELEMENTAL_WIND               3
#define ELEMENTAL_EARTH              4
#define ELEMENTAL_LASER              5
#define ELEMENTAL_DARK               6
#define ELEMENTAL_ELEC               7
#define ELEMENTAL_ANGEL_BLUE         8
#define ELEMENTAL_ANGEL_RED          9
#define ELEMENTAL_ANGEL_WHITE        10
#define ELEMENTAL_ANGEL_GREEN        11

// dwReferTarget
#define RT_ATTACK                    1
#define RT_TIME                      2
#define RT_HEAL                      3

// DestParam1 �߰�
// ĳ���� ����
#define DST_STR                      1   // Str			<-�����
#define DST_DEX                      2   // Dex			<-�����
#define DST_INT                      3   // Int			<-�����
#define DST_STA                      4   // Sta			<-�����
#define DST_YOY_DMG		             5   // ��� ������
#define DST_BOW_DMG		             6   // ���� ������
#define DST_CHR_RANGE                7   // ���� ����
#define DST_BLOCK_RANGE              8   // ���������(���Ÿ����ݿ� ����)
#define DST_CHR_CHANCECRITICAL       9   // 
#define DST_CHR_BLEEDING            10	//
#define DST_SPEED                   11  // 		<-�����
#define DST_ABILITY_MIN             12  // AbilityMin
#define DST_ABILITY_MAX             13  // AbilityMax
#define DST_BLOCK_MELEE             14  // ���������(�������ݿ� ����)
#define DST_MASTRY_EARTH            15  // 
#define DST_STOP_MOVEMENT           16  // ��� �̵� �Ұ�(������ ����)
#define DST_MASTRY_FIRE             17  // 
#define DST_MASTRY_WATER            18  // 
#define DST_MASTRY_ELECTRICITY      19  // 
#define DST_MASTRY_WIND             20  //
#define DST_KNUCKLE_DMG             21  // ��Ŭ ���ݷ� ����
#define DST_PVP_DMG_RATE			22	// PVP�� ���� ���� ������ ���� Rate //#define DST_ARM_RATE                22  // Diagonal Rate
//#define DST_LEG_RATE                23  // Thrust Rate
#define DST_ATTACKSPEED             24  // Attack Speed		<-�����
#define DST_SWD_DMG                 25  // �߰� ������			<-�����
#define DST_ADJDEF                  26  // Adjestment Defense	<-�����
#define DST_RESIST_MAGIC            27  // Resist Magic ������ ��ü���׼�ġ
#define DST_RESIST_ELECTRICITY      28  // Resist Electticity
#define DST_REFLECT_DAMAGE          29  // �����ڿ��� ������ �ݻ�(%), nAdj �����ִ� �������� ����(100%=100) dwDestData1 Ȯ��(=��100%)
#define DST_RESIST_FIRE             30  // Resist Fire
#define DST_RESIST_WIND             31  // Resist Wind
#define DST_RESIST_WATER            32  // Resist Water
#define DST_RESIST_EARTH            33  // Resist Earth
#define DST_AXE_DMG                 34  // �߰� ������			<-�����									
#define DST_HP_MAX                  35  // HP			<-�����
#define DST_MP_MAX                  36  // MP			
#define DST_FP_MAX                  37  // FP
#define DST_HP                      38  // HP			<-�����
#define DST_MP                      39  // MP			<-�����
#define DST_FP                      40  // FP			<-�����
#define DST_HP_RECOVERY             41  // HPRecovery
#define DST_MP_RECOVERY             42  // MPRecovery
#define DST_FP_RECOVERY             43  // FPRecovery
#define DST_KILL_HP					44	// ���� �׿����� HP ȸ����
#define DST_KILL_MP					45	// ���� �׿����� MP ȸ����
#define DST_KILL_FP					46	// ���� �׿����� FP ȸ����
#define DST_ADJ_HITRATE             47  // AdjHitRate			<-�����
//#define DST_ADJ_SPELLRATE           48  // AdjSpellRate
#define DST_CLEARBUFF				49  // ���� �ı�.
#define DST_CHR_STEALHP_IMM         50  // ������κ��� ������ ���� hp�� ����
#define DST_ATTACKSPEED_RATE		51	// ���ݼӵ� ������ //#define DST_ABRASION                51  // Abrasion
#define DST_HP_MAX_RATE				52		// DST_HP_MAX �ۼ�Ʈ ����		//#define DST_ADJROLL                 52  // AdjRoll
#define DST_MP_MAX_RATE				53		// ����							//#define DST_AP                      53  // �ִ�ġ���� ����ϰ� �Ǹ� ������ �����ȴ�.(���Ӱ����� �����Ǹ� ReAttackDelay ����)	<-�����
#define DST_FP_MAX_RATE				54		// ����							//#define DST_GROGGY                  54  // ���ص��� �߻��� �ʿ��� ��ġ(������ �����Ǹ鼭 ���ص����� ���ϰ� �ȴ�. ReAttackDelay ���� ����)
#define DST_CHR_WEAEATKCHANGE	    55	// �ɸ����� �Ӽ� ���ݷ��� �÷���(���⿡ �Ӽ� ���ݷ��� ���ų� ���⸦ �ٲܽ� ������)
#define DST_CHR_STEALHP				56  // ������ �������� ����������HP�� ����(����)
#define DST_CHR_CHANCESTUN			57  // ȿ���� ���ӵǴµ��� ���ݽ� (����Ȯ����Adj) ������ ��ų(Chg) �ߵ�.
#define DST_AUTOHP					58  // �ɸ��Ͱ� ������ �̻� HP�� �پ��� HP�� �ڵ����� ȸ���� Adj:���� HP�� �ۼ������� Chg:ȸ�� HP�� �ۼ�������
#define DST_CHR_CHANCEDARK			59  // ȿ���� ���ӵǴµ��� ���ݽ� (����Ȯ����Adj) ������ ��ų(Chg) �ߵ�.
//#define DST_CHR_CHANCESKILL	    60  // ȿ���� ���ӵǴµ��� ���ݽ� (����Ȯ����Adj) ������ ��ų(Chg) �ߵ�.
#define DST_CHR_CHANCEPOISON	    60  // ȿ���� ���ӵǴµ��� ���ݽ� (����Ȯ����Adj) ������ ��ų(Chg) �ߵ�.
#define DST_IMMUNITY	 			61  // �鿪
#define DST_ADDMAGIC				62 	// ���� �߰�
#define DST_CHR_DMG                 63  // �߰� ������			<-�����
#define DST_CHRSTATE                64  // ĳ������ ���¸� ����		<-����� Adj:CHR_�ø��� Chg:������.  nProbability:Ȯ�� dwSkillTime:�ð�.
#define DST_PARRY                   65  // ���� Ȯ���� �߰��Ѵ�.	<-�����
#define DST_ATKPOWER_RATE			66  // ���ݷ�(%�� ���)
#define DST_EXPERIENCE				67  // ����ġ //#define DST_REATTACKDELAY           67  // ����� �ӵ��� ����
#define DST_JUMPING                 68  // ĳ������ ���� ���� ����
#define DST_CHR_CHANCESTEALHP		69  // ȿ���� ���ӵǴµ��� ���ݽ� (����Ȯ����Adj) ������ ��ų(Chg) �ߵ�.
#define DST_CHR_CHANCEBLEEDING      70  // ȿ���� ���ӵǴµ��� ���ݽ� (����Ȯ����Adj) ������ ��ų(Chg) �ߵ�.
#define DST_RECOVERY_EXP            71  // ĳ���� ����ÿ� ��Ȱ�� ���ؼ� ��Ƴ� ��� Exp �ս� �κ��� ȸ�� �ۼ�Ʈ	<-�����
#define DST_ADJDEF_RATE				72	// ADJDEF �ۼ�Ʈ�� ���� ����.

#define	DST_MP_DEC_RATE				73	// MP �Ҹ� ������
#define	DST_FP_DEC_RATE				74	// FP �Ҹ� ������
#define	DST_SPELL_RATE				75	// ���� �ӵ� ������
#define	DST_CAST_CRITICAL_RATE		76	// �ֹ� �ش�ȭ Ȯ�� ����
#define	DST_CRITICAL_BONUS			77	// ġ��Ÿ ���߽� �߰� Ÿ��
#define	DST_ATKPOWER_RATE_MAGIC				78	// ��ų���� ��
#define DST_MONSTER_DMG				79	// ���� ��ɽ� ������ ����
#define DST_PVP_DMG					80	// PVP�� ������ ����
#define DST_MELEE_STEALHP			81	// ��Ÿ�� ����
#define	DST_HEAL						82		// �ڵ� ġ��
#define DST_ATKPOWER			83	// ���ݷ�

//	10�� ���� ������ ��ų 
#define DST_ONEHANDMASTER_DMG             85	
#define DST_TWOHANDMASTER_DMG             86
#define DST_YOYOMASTER_DMG             87
#define DST_BOWMASTER_DMG             88
#define DST_KNUCKLEMASTER_DMG             89
#define DST_HAWKEYE_RATE             90
#define DST_RESIST_MAGIC_RATE             91
#define DST_GIFTBOX					92

#define	DST_RESTPOINT_RATE			93	// 15�� �޽��� ��� ������(%)
#define DST_IGNORE_DMG_PVP			94

#define MAX_ADJPARAMARY             95

// ADJPARAMARRAY���� �߰����� �ʰ�, ���簪�� ������ ��ȯ( GetPointParam, SetPointParam )�� ����ϱ� ���� �ĺ���
#define	DST_GOLD				10000
#define	DST_PXP					10001
#define DST_RESIST_ALL			10002	// Resist ALL - (MAX_ADJPARAMARY + 3)�� ���� ����� ������Ƽ���� �ν����� ����.
#define DST_STAT_ALLUP			10003	//��� Status�� +1��ŭ �÷��ش�.
#define DST_HPDMG_UP			10004
#define DST_DEFHITRATE_DOWN		10005
#define DST_CURECHR				10006		// DST_CHRSTATE ���� ����
#define DST_HP_RECOVERY_RATE	10007		// HPRecovery Rate
#define DST_MP_RECOVERY_RATE	10008		// MPRecovery Rate
#define DST_FP_RECOVERY_RATE	10009		// FPRecovery Rate
#define	DST_LOCOMOTION			10010		// JUMP & SPEED
#define	DST_MASTRY_ALL			10011
#define DST_ALL_RECOVERY		10012
#define DST_ALL_RECOVERY_RATE	10013
#define DST_KILL_ALL			10014
#define DST_KILL_HP_RATE		10015
#define DST_KILL_MP_RATE		10016
#define DST_KILL_FP_RATE		10017
#define DST_KILL_ALL_RATE		10018
#define DST_ALL_DEC_RATE		10019

#endif
