1992
{
	SetTitle
	(
		IDS_PROPQUEST_INC_000000
	);
}

1993
{
	SetTitle
	(
		IDS_PROPQUEST_INC_000001 
	);


}

1994
{
	SetTitle
	(
		IDS_PROPQUEST_INC_000002
	);


}

1995
{
	SetTitle
	(
		IDS_PROPQUEST_INC_000003
	);
	

}

1997
{
	SetTitle
	(
		IDS_PROPQUEST_INC_000004
	);


}

6000
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002612
	);
	
}

6001
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002613
	);
	
}

6002
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002614
	);
	
}

6003
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002615
	);
	
}

6004
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002608
	);
	Setting
	{
		SetHeadQuest( 6001 );
	}
}

6005
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002609
	);
	Setting
	{
		SetHeadQuest( 6001 );
	}
}

6006
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002610
	);
	Setting
	{
		SetHeadQuest( 6001 );
	}
}

6007
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002611
	);
	Setting
	{
		SetHeadQuest( 6001 );
	}
}

6050
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002615
	);
	Setting
	{
		SetHeadQuest( 6003 );
	}
}

6051
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002616
	);
	Setting
	{
		SetHeadQuest( 6001 );
	}
}

6052
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002617
	);
	Setting
	{
		SetHeadQuest( 6001 );
	}
}

6054
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002618
	);
	Setting
	{
		SetHeadQuest( 6001 );
	}
}

6060
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002647
	);
	Setting
	{
		SetHeadQuest( 6001 );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// GUILD CREATE ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

QUEST_CREGUILD
{
	SetTitle
	(
		IDS_PROPQUEST_INC_001800
	);
	
	setting
	{
		
		SetCharacter( "MaSa_Helena" );
		SetEndCondCharacter( "MaSa_Helena", 8463, 3600, QUEST_DESTINATION_ID_0000 );
		SetBeginCondLevel( 40, 199 );
		SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
		SetEndCondLevel( 40, 199 );
		SetEndCondParty( 2, 0, 3, 2 );
		SetHeadQuest( 6005 );
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_001801
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_001802
	);

		
	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_001803
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_001804
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_001805
	);

	SetDialog
	(
		QSAY_END_COMPLETE2, 
		IDS_PROPQUEST_INC_001806	
	);

	SetDialog
	(
		QSAY_END_COMPLETE3,
		IDS_PROPQUEST_INC_001807	
	);

	SetDialog
	(
		QSAY_END_FAILURE1,
		IDS_PROPQUEST_INC_001808	
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_001809
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_001810
		);
		
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// TAMER ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

QUEST_PETTAME01 // 알 -> D급
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002010
	);
	
	setting
	{
		
		SetRepeat(1);
		SetCharacter( "MaFl_PetTamer" );
		SetBeginCondLevel( 1, 199 );
		SetBeginCondParty( 0, 0, 0, 0 );
        SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
        SetBeginCondPetLevel( 0 );
		SetEndCondPetLevel( 0 );
		SetEndCondPetExp( 100 );
		SetEndRewardPetLevelup();
		SetHeadQuest( 6004 );
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_002011
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_002012
	);

	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_002013
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_002014
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_002015
	);

	SetDialog
	(
		QSAY_END_FAILURE1, 
		IDS_PROPQUEST_INC_002016
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002017
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002018
		);
		
	}
}

QUEST_PETTAME02 // D급 -> C급
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002019
	);
	
	setting
	{
		
		SetRepeat(1);
		SetCharacter( "MaFl_PetTamer" );
		SetBeginCondLevel( 1, 199 );
		SetBeginCondParty( 0, 0, 0, 0 );
        SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
		SetBeginCondPetLevel( 1 );
		SetEndCondPetLevel( 1 );
		SetEndCondPetExp( 100 );
		SetEndRewardPetLevelup();
		SetHeadQuest( 6004 );
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_002020
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_002021
	);

	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_002022
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_002023
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_002024
	);

	SetDialog
	(
		QSAY_END_FAILURE1, 
		IDS_PROPQUEST_INC_002025
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002026
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002027
		);
		
	}
}

QUEST_PETTAME03 // C급 -> B급
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002028
	);
	
	setting
	{
		
		SetRepeat(1);
		SetCharacter( "MaFl_PetTamer" );
		SetBeginCondLevel( 1, 199 );
		SetBeginCondParty( 0, 0, 0, 0 );
        SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
		SetBeginCondPetLevel( 2 );
		SetEndCondPetLevel( 2 );
		SetEndCondPetExp( 100 );
		SetEndRewardPetLevelup();
		SetHeadQuest( 6004 );
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_002029
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_002030
	);

	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_002031
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_002032
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_002033
	);

	SetDialog
	(
		QSAY_END_FAILURE1, 
		IDS_PROPQUEST_INC_002034
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002035
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002036
		);
		
	}
}

QUEST_PETTAME04 // B급 -> A급
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002037
	);
	
	setting
	{
	
		SetRepeat(1);
		SetCharacter( "MaFl_PetTamer" );
		SetBeginCondLevel( 1, 199 );
		SetBeginCondParty( 0, 0, 0, 0 );
        SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
		SetBeginCondPetLevel( 3 );
		SetEndCondPetLevel( 3 );
		SetEndCondPetExp( 100 );
		SetEndRewardPetLevelup();
		SetHeadQuest( 6004 );
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_002038
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_002039
	);

	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_002040
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_002041
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_002042
	);

	SetDialog
	(
		QSAY_END_FAILURE1, 
		IDS_PROPQUEST_INC_002043
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002044
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002045
		);
		
	}
}

QUEST_PETTAME05 // A급 -> S급
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002046
	);
	
	setting
	{
		
		SetRepeat(1);
		SetCharacter( "MaFl_PetTamer" );
		SetBeginCondLevel( 1, 199 );
		SetBeginCondParty( 0, 0, 0, 0 );
        SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
		SetBeginCondPetLevel( 4 );
		SetEndCondPetLevel( 4 );
		SetEndCondPetExp( 100 );
		SetEndRewardPetLevelup();
		SetHeadQuest( 6004 );
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_002047
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_002048
	);

	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_002049
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_002050
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_002051
	);

	SetDialog
	(
		QSAY_END_FAILURE1, 
		IDS_PROPQUEST_INC_002052
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002053
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002054
		);
		
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// VOLCANE TELEPORT //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

QUEST_VOLCANETELEPORT
{
	SetTitle
	(
		IDS_PROPQUEST_INC_002001
	);
	
	setting
	{
		SetHeadQuest( 1992 );
		SetRepeat(1);
		SetCharacter( "MaDa_RedRobeMan" );
		SetBeginCondLevel( 1, 199 );
		SetBeginCondParty( 0, 0, 0, 0 );
        SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
		SetEndCondItem( -1, 0, -1, II_GEN_GEM_GEM_PHANBUBLE, 1 );
		SetEndRemoveItem( 0, II_GEN_GEM_GEM_PHANBUBLE, 1 );
        SetEndRewardTeleport( 203, 1394, 150, 544)
		
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_002002
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_002003
	);

	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_002004
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_002005
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_002006
	);

	SetDialog
	(
		QSAY_END_FAILURE1, 
		IDS_PROPQUEST_INC_002007
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002008
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_002009
		);
		
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// CLOCKWORKS /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

QUEST_EXISTDESIRE
{
	SetTitle
	(
		IDS_PROPQUEST_INC_001756
	);
	
	setting
	{
		
		SetCharacter( "MaDa_DarMayor" );
		SetEndCondCharacter( "MaDa_DrEst", 5541, 4352, QUEST_DESTINATION_ID_0000 );
		SetBeginCondLevel( 70, 199 );
        SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
		SetEndCondLevel( 70, 199 );
		SetEndRewardItem( -1, 0, -1, II_SYS_SYS_QUE_MONBOOK1, 1 );
		SetHeadQuest( 6006 );
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_001757
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_001758
	);

	
	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_001759	
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_001760
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_001761
	);

	SetDialog
	(
		QSAY_END_COMPLETE2, 
		IDS_PROPQUEST_INC_001762
	);
		
	SetDialog
	(
		QSAY_END_FAILURE1,
		IDS_PROPQUEST_INC_001763
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_001764	
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_001765
		);
		
	}
}

QUEST_CLOCK1ST
{
	SetTitle
	(
		IDS_PROPQUEST_INC_001766
	);
	
	setting
	{
		
		SetCharacter( "MaDa_DrEst" );
		SetEndCondCharacter( "MaFl_DrEstern", 7940, 2406, QUEST_DESTINATION_ID_0000 );
		SetBeginCondLevel( 70, 199 );
		SetBeginCondPreviousQuest( 1, QUEST_EXISTDESIRE );
        SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
		SetEndCondLevel( 70, 199 );
		SetEndCondItem( -1, 0, -1, II_SYS_SYS_QUE_MONBOOK1, 1, -1, -1, QUEST_DESTINATION_ID_0043 );
		SetEndCondItem( -1, 0, -1, II_SYS_SYS_QUE_WARMARK, 5, 4041, 4559, QUEST_DESTINATION_ID_0000 );
		SetEndRemoveItem( 0, II_SYS_SYS_QUE_WARMARK, -1 );	
		SetEndRewardItem( -1, 0, -1, II_SYS_SYS_QUE_MONBOOK2, 1 );
		SetHeadQuest( 6006 );
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_001767
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_001768
	);

	SetDialog
	(
		QSAY_BEGIN3, 
		IDS_PROPQUEST_INC_001769
	);

	
	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_001770	
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_001771
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_001772
	);

	SetDialog
	(
		QSAY_END_COMPLETE2, 
		IDS_PROPQUEST_INC_001773
	);
		
	SetDialog
	(
		QSAY_END_FAILURE1,
		IDS_PROPQUEST_INC_001774
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_001775	
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_001776
		);
		
	}
}

QUEST_CLOCK2ND
{
	SetTitle
	(
		IDS_PROPQUEST_INC_001777
	);
	
	setting
	{
		
		SetCharacter( "MaFl_DrEstern" );
		SetEndCondCharacter( "MaFl_DrEstly", 7187, 4090, QUEST_DESTINATION_ID_0000 );
		SetBeginCondLevel( 70, 199 );
		SetBeginCondPreviousQuest( 1, QUEST_CLOCK1ST );
        SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
		SetEndCondLevel( 70, 199 );
		SetEndCondItem( -1, 0, -1, II_SYS_SYS_QUE_MONBOOK1, 1, -1, -1, QUEST_DESTINATION_ID_0044 );
		SetEndCondItem( -1, 0, -1, II_SYS_SYS_QUE_MONBOOK2, 1, -1, -1, QUEST_DESTINATION_ID_0044 );
		SetEndCondItem( -1, 0, -1, II_SYS_SYS_QUE_WOUNDWAR, 5, 6538, 3665, QUEST_DESTINATION_ID_0000 );
		SetEndRemoveItem( 0, II_SYS_SYS_QUE_WARMARK, -1 );	
		SetEndRemoveItem( 1, II_SYS_SYS_QUE_WOUNDWAR, -1 );
		SetEndRewardItem( -1, 0, -1, II_SYS_SYS_QUE_MONBOOK3, 1 );
		SetHeadQuest( 6004 );
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_001778
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_001779	
	);

	SetDialog
	(
		QSAY_BEGIN3, 
		IDS_PROPQUEST_INC_001780
	);

	SetDialog
	(
		QSAY_BEGIN4, 
		IDS_PROPQUEST_INC_001781
	);

	
	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_001782	
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_001783
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_001784
	);

	SetDialog
	(
		QSAY_END_COMPLETE2, 
		IDS_PROPQUEST_INC_001785
	);
		
	SetDialog
	(
		QSAY_END_FAILURE1,
		IDS_PROPQUEST_INC_001786
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_001787	
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_001788
		);
		
	}
}

QUEST_ABILITYYE
{
	SetTitle
	(
		IDS_PROPQUEST_INC_001789
	);
	
	setting
	{
		
		SetCharacter( "MaFl_DrEstly" );
		SetEndCondCharacter( "MaDa_DarMayor", 3831, 4457, QUEST_DESTINATION_ID_0000 );
		SetBeginCondLevel( 70, 199 );
		SetBeginCondPreviousQuest( 1, QUEST_CLOCK2ND );
		SetBeginCondJob( JOB_VAGRANT, JOB_MERCENARY, JOB_ACROBAT, JOB_ASSIST, JOB_MAGICIAN, JOB_KNIGHT, JOB_BLADE, JOB_JESTER, JOB_RANGER, JOB_RINGMASTER, JOB_BILLPOSTER, JOB_PSYCHIKEEPER, JOB_ELEMENTOR, JOB_KNIGHT_MASTER, JOB_BLADE_MASTER, JOB_JESTER_MASTER, JOB_RANGER_MASTER, JOB_RINGMASTER_MASTER, JOB_BILLPOSTER_MASTER, JOB_PSYCHIKEEPER_MASTER, JOB_ELEMENTOR_MASTER, JOB_KNIGHT_HERO, JOB_BLADE_HERO, JOB_JESTER_HERO, JOB_RANGER_HERO, JOB_RINGMASTER_HERO, JOB_BILLPOSTER_HERO, JOB_PSYCHIKEEPER_HERO, JOB_ELEMENTOR_HERO, JOB_LORDTEMPLER_HERO, JOB_STORMBLADE_HERO, JOB_WINDLURKER_HERO, JOB_CRACKSHOOTER_HERO, JOB_FORCEMASTER_HERO, JOB_FLORIST_HERO, JOB_MENTALIST_HERO, JOB_ELEMENTORLORD_HERO );
		SetEndCondLevel( 70, 199 );
		SetEndCondKillNPC( 0, MI_CLOCKS, 1, 3695, 4549, QUEST_DESTINATION_ID_0000 );
		SetEndCondItem( -1, 0, -1, II_SYS_SYS_QUE_MONBOOK1, 1, -1, -1, QUEST_DESTINATION_ID_0045 );
		SetEndCondItem( -1, 0, -1, II_SYS_SYS_QUE_MONBOOK2, 1, -1, -1, QUEST_DESTINATION_ID_0045 );
		SetEndCondItem( -1, 0, -1, II_SYS_SYS_QUE_MONBOOK3, 1, -1, -1, QUEST_DESTINATION_ID_0045 );
		SetEndRemoveItem( 0, II_SYS_SYS_QUE_MONBOOK1, -1 );
		SetEndRemoveItem( 1, II_SYS_SYS_QUE_MONBOOK3, -1 );
		SetEndRemoveItem( 2, II_SYS_SYS_QUE_MONBOOK2, -1 );
		SetEndRewardExp( 36778455, 36778455 );
		SetHeadQuest( 6004 );
	}

	SetDialog
	(
		QSAY_BEGIN1, 
		IDS_PROPQUEST_INC_001790
	);
	
	SetDialog
	(
		QSAY_BEGIN2, 
		IDS_PROPQUEST_INC_001791	
	);

	SetDialog
	(
		QSAY_BEGIN3, 
		IDS_PROPQUEST_INC_001792
	);

	SetDialog
	(
		QSAY_BEGIN4, 
		IDS_PROPQUEST_INC_001793
	);

	
	SetDialog
	(
		QSAY_BEGIN_YES,
		IDS_PROPQUEST_INC_001794
	);

	SetDialog
	(
		QSAY_BEGIN_NO, 
		IDS_PROPQUEST_INC_001795
	);

	SetDialog
	(
		QSAY_END_COMPLETE1, 
		IDS_PROPQUEST_INC_001796
	);

	SetDialog
	(
		QSAY_END_FAILURE1,
		IDS_PROPQUEST_INC_001797
	);

	state 0
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_001798		
		);
		
	}

	state 14
	{

		SetDesc
		(
			IDS_PROPQUEST_INC_001799
		);
		
	}
}