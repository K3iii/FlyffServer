MaFl_Player
{
	setting
	{
		AddMenu( MMI_DIALOG );
		SetEquip( II_ARM_F_CLO_KOREAN01HAT, II_ARM_F_CLO_KOREAN01SUIT, II_ARM_F_CLO_KOREAN01SHOES, II_WEA_SWO_OUTLAW );
		SetFigure( MI_FEMALE, 0, 0xffff0000, 0 );
		SetMusic( BGM_NPC_HORROR2 );
		SetImage
		(
		IDS_CHARACTER_ETC_INC_000000
		);
		m_szDialog= "MaFl_Player.txt";
	}
	SetName
	(
	IDS_CHARACTER_ETC_INC_000001
	);
}
MaFl_Player2
{
	setting
	{
		AddMenu( MMI_DIALOG );
		//SetEquip( II_ARM_F_CLO_KOREAN01HAT );
		SetEquip( II_ARM_F_VAG_HELMET02 );
		SetFigure( MI_FEMALE, 0, 0xffff0000, 0 );
		SetMusic( BGM_NPC_SAD );
		SetImage
		(
		IDS_CHARACTER_ETC_INC_000002
		);
		m_szDialog= "MaFl_Player2.txt";
	}
	SetName
	(
	IDS_CHARACTER_ETC_INC_000003
	);
}
MaFl_Series1
{
	setting
	{
		AddMenu( MMI_DIALOG );
		m_nStructure= SRT_QUESTOFFICE;
		
		
		SetMusic( BGM_NPC_HORROR );
		SetImage
		(
		IDS_CHARACTER_ETC_INC_000004
		);
		m_szDialog= "MaFl_Series1.txt";
	}
	SetName
	(
	IDS_CHARACTER_ETC_INC_000005
	);
}
MaFl_Series2
{
	setting
	{
		AddMenu( MMI_DIALOG );
		m_nStructure= SRT_QUESTOFFICE;
		SetMusic( BGM_NPC_SECRET );		
		
		SetImage
		(
		IDS_CHARACTER_ETC_INC_000006
		);
		m_szDialog= "MaFl_Series2.txt";
	}
	SetName
	(
	IDS_CHARACTER_ETC_INC_000007
	);
}


MaFl_MultiQuest1
{
	setting
	{
		AddMenu( MMI_DIALOG );
		m_nStructure= SRT_QUESTOFFICE;
		SetMusic( BGM_NPC_SECRET );		
		
		SetImage
		(
		IDS_CHARACTER_ETC_INC_000008
		);
		m_szDialog= "MaFl_MultiQuest1.txt";
	}
	SetName
	(
	IDS_CHARACTER_ETC_INC_000009
	);
}
MaFl_MultiQuest2
{
	setting
	{
		AddMenu( MMI_DIALOG );
		m_nStructure= SRT_QUESTOFFICE;
		SetMusic( BGM_NPC_SECRET );		
		
		SetImage
		(
		IDS_CHARACTER_ETC_INC_000010
		);
		m_szDialog= "MaFl_MultiQuest2.txt";
	}
	SetName
	(
	IDS_CHARACTER_ETC_INC_000011
	);
}
MaFl_MultiQuest3
{
	setting
	{
		AddMenu( MMI_DIALOG );
		m_nStructure= SRT_QUESTOFFICE;
		SetMusic( BGM_NPC_SECRET );		
		
		SetImage
		(
		IDS_CHARACTER_ETC_INC_000012
		);
		m_szDialog= "MaFl_MultiQuest3.txt";
	}
	SetName
	(
	IDS_CHARACTER_ETC_INC_000013
	);
}