--------------------------------------------------------------------
-- Dungeons --------------------------------------------------------
--------------------------------------------------------------------
dofile( ".\\LuaFunc\\InstanceDungeonBase.lua" )
--------------------------------------------------------------------

AddDungeon( "WI_INSTANCE_RUSTIA" )
--{
	SetClass( CLASS_NORMAL, CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 60, 300 )
	SetCoolTime( MIN(60) )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE01", false, 513, 101, 953 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE02", false, 889, 101, 1121 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE03", false, 926, 101, 850 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE04", false, 1247, 101, 1272 )
	SetMonster( ID_BOSS, "MI_BESIBIGFOOT01", true, 1126, 103, 1505 )
--}

AddDungeon( "WI_INSTANCE_RUSTIA_1" )
--{
	SetClass( CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 60, 300 )
	SetCoolTime( MIN(60) )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE01", false, 513, 101, 953 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE02", false, 889, 101, 1121 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE03", false, 926, 101, 850 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE04", false, 1247, 101, 1272 )
	SetMonster( ID_BOSS, "MI_BESIBIGFOOT02", true, 1126, 103, 1505 )
--}


AddDungeon( "WI_INSTANCE_UPRESIA" )
--{
	SetClass( CLASS_NORMAL, CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 75, 300 )
	SetCoolTime( MIN(90) )
	SetMonster( ID_BOSS, "MI_DREAMQEEN01", true, 2151, 100, 1727 )
--}

AddDungeon( "WI_INSTANCE_UPRESIA_1" )
--{
	SetClass( CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO)
	SetLevel( 75, 300 )
	SetCoolTime( MIN(90) )
	SetMonster( ID_BOSS, "MI_DREAMQEEN01_1", true, 2151, 100, 1727 )	
--}


AddDungeon( "WI_INSTANCE_HERNEOS" )
--{
	SetClass( CLASS_NORMAL, CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 90, 300 )
	SetCoolTime( MIN(90) )
	SetMonster( ID_MIDBOSS, "MI_HERNSIREN01", true, 689, 100, 508 )
	SetMonster( ID_BOSS, "MI_HERNKRAKEN01", true, 1942, 100, 1424 )	
--}

AddDungeon( "WI_INSTANCE_HERNEOS_1" )
--{
	SetClass( CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 90, 300 )
	SetCoolTime( MIN(90) )
	SetMonster( ID_MIDBOSS, "MI_HERNSIREN01_1", true, 689, 100, 508 )
	SetMonster( ID_BOSS, "MI_HERNKRAKEN01_1", true, 1942, 100, 1424 )
--}


AddDungeon( "WI_INSTANCE_SANPRES" )
--{
	SetClass( CLASS_NORMAL, CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 105, 300 )
	SetCoolTime( MIN(90) )
	SetMonster( ID_MIDBOSS, "MI_SHIPHIPPOGRIPH", true, 1961, 101, 253 )
	SetMonster( ID_BOSS, "MI_SHIPHARPINEES", true, 1362, 109, 1730 )
--}

AddDungeon( "WI_INSTANCE_SANPRES_1" )
--{
	SetClass( CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO)
	SetLevel( 105, 300 )
	SetCoolTime( MIN(90) )
	SetMonster( ID_MIDBOSS, "MI_SHIPHIPPOGRIPH_1", true, 1961, 101, 253 )
	SetMonster( ID_BOSS, "MI_SHIPHARPINEES_1", true, 1362, 109, 1730 )
--}


AddDungeon( "WI_INSTANCE_OMINOUS" )
--{
	SetClass( CLASS_NORMAL, CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 80, 300 )
	SetCoolTime( MIN(120) )
	SetMonster( ID_NORMAL, "MI_PRICKANT04", true, 1280, 101, 1640 )
	SetMonster( ID_NORMAL, "MI_MAULMOUSE04", true, 1234, 101, 1393 )
	SetMonster( ID_NORMAL, "MI_CRIPESCENTIPEDE04", true, 1089, 101, 1590 )
	SetMonster( ID_MIDBOSS, "MI_LYCANOS01", true, 1078, 101, 1359 )
	SetMonster( ID_BOSS, "MI_VEMPAIN01", true, 1079, 101, 1457 )
--}

AddDungeon( "WI_INSTANCE_OMINOUS_1" )
--{
	SetClass( CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 80, 300 )
	SetCoolTime( MIN(120) )
	SetMonster( ID_NORMAL, "MI_PRICKANT04_1", true, 1280, 101, 1640 )
	SetMonster( ID_NORMAL, "MI_MAULMOUSE04_1", true, 1234, 101, 1393 )
	SetMonster( ID_NORMAL, "MI_CRIPESCENTIPEDE04_1", true, 1089, 101, 1590 )
	SetMonster( ID_MIDBOSS, "MI_LYCANOS01_1", true, 1078, 101, 1359 )
	SetMonster( ID_BOSS, "MI_VEMPAIN01_1", true, 1079, 101, 1457 )
--}

AddDungeon( "WI_INSTANCE_DREADFULCAVE" )
--{
	SetClass( CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 121, 300)
	SetCoolTime( MIN(180) )
	SetMonster( ID_MIDBOSS, "MI_DREADSTONE01", false, 928, 101, 198 )
	SetMonster( ID_MIDBOSS, "MI_DREADSTONE02", false, 1504, 101, 419 )
	SetMonster( ID_MIDBOSS, "MI_DREADSTONE03", false, 1397, 101, 893 )
	SetMonster( ID_MIDBOSS, "MI_DREADSTONE04", false, 764, 101, 867 )
	SetMonster( ID_MIDBOSS, "MI_DREADSTONE05", false, 675, 101, 560 )
	SetMonster( ID_BOSS, "MI_SKELDEVIL", true, 1809, 101, 1395 )
--}

AddDungeon( "WI_INSTANCE_BEHAMAH" )
--{
	SetClass( CLASS_LEGEND_HERO )
	SetLevel( 130, 300 )
	SetCoolTime( MIN(360) )
	SetMonster( ID_MIDBOSS, "MI_BEHESTATUE01", false, 697, 100, 375 )
	SetMonster( ID_MIDBOSS, "MI_BEHESTATUE01", false, 624, 100, 494 )
	SetMonster( ID_MIDBOSS, "MI_BEHESTATUE01", false, 918, 100, 643 )
	SetMonster( ID_MIDBOSS, "MI_BEHESTATUE01", false, 898, 100, 785 )
	SetMonster( ID_MIDBOSS, "MI_BEHESTATUE01", false, 580, 100, 748 )
	SetMonster( ID_BOSS, "MI_BEHEMOTH", true, 1261, 101, 1392 )
--}

AddDungeon( "WI_INSTANCE_KALGAS" )
--{
	SetClass( CLASS_LEGEND_HERO )
	SetLevel( 135, 300 )
	SetCoolTime( MIN(360) )
	SetMonster( ID_MIDBOSS, "MI_KALGASSTELE01", false, 1382, 130, 820 )
	SetMonster( ID_MIDBOSS, "MI_KALGASSTELE02", false, 877, 135, 610 )
	SetMonster( ID_MIDBOSS, "MI_KALGASEGG01", false, 1332, 113, 259 )
	SetMonster( ID_MIDBOSS, "MI_KALGASEGG01", false, 1347, 113, 257 )
	SetMonster( ID_MIDBOSS, "MI_KALGASEGG01", false, 1352, 113, 276 )
	SetMonster( ID_MIDBOSS, "MI_KALGASEGG01", false, 1331, 113, 276 )
	SetMonster( ID_BOSS, "MI_KALGASBOSS", true, 1339, 135, 275 )
--}
