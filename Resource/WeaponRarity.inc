Configuration
{
	dwUpgradePrice = 250000;
}

Add_Weapon_Rarity
{
	nRarityLevel 	= 0;
	szName			= "Common";
	dwColor			= 0xFF3fd527;
	fUpgradeRate	= 100;
	fAttackRate		= 1.00f;
	fStatsRate		= 1.00f;
}

Add_Weapon_Rarity
{
	nRarityLevel	= 1;
	szName			= "Uncommon";
	dwColor			= 0xFF30b9e0;
	fUpgradeRate	= 100;
	fAttackRate		= 1.04f;
	fStatsRate		= 1.10f;
}

Add_Weapon_Rarity
{
	nRarityLevel	= 2;
	szName			= "Rare";
	dwColor			= 0xFF9d35b0;
	fUpgradeRate	= 100;
	fAttackRate		= 1.07f;
	fStatsRate		= 1.20f;
}

Add_Weapon_Rarity
{
	nRarityLevel	= 3;
	szName			= "Epic";
	dwColor			= 0xFFab9d24;
	fUpgradeRate	= 100;
	fAttackRate		= 1.10f;
	fStatsRate		= 1.30f;
}

Add_Weapon_Rarity
{
	nRarityLevel	= 4;
	szName			= "Legendary";
	dwColor			= 0xFFE06230;
	fUpgradeRate	= 100;
	fAttackRate		= 1.12f;
	fStatsRate		= 1.40f;
}

Add_Weapon_Rarity
{
	nRarityLevel	= 5;
	szName			= "Mythic";
	dwColor			= 0xFFFE1248;
	fUpgradeRate	= 100;
	fAttackRate		= 1.15f;
	fStatsRate		= 1.50f;
	
	SetEffect( reflect );
}

Drop
{
	// nRarityLevel nDropLuck(1/100)
	0	60
	1	16
	2	11
	3	7
	4	5
	5	1
}