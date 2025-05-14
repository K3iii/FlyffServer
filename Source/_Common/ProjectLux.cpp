// Project.cpp: implementation of the CProject class.

////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "defineItem.h"
#include "lang.h"

#include "Ship.h"
#ifdef __WORLDSERVER
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#endif	// __WORLDSERVER

#include "guild.h"


#ifdef __WORLDSERVER
#include "guildquest.h"
#endif	// __WORLDSERVER

#define SYNTAX_ERROR		Error( "%s(%d) : MoverID=%d syntax error %s", szFileName, script.GetLineNum(), nVal, script.token );
#define UNIDENTIFY_ERROR	Error( "%s(%d) : MoverID=%d 지정되지 않은 디파인 error %s", szFileName, script.GetLineNum(), nVal, script.token );
#define CUSTOM_ERROR(A)		Error( "%s(%d) : MoverID=%d %s %s", szFileName, script.GetLineNum(), nVal, A, script.token );

enum AICMD {
	AICMD_NONE = 0,
	AICMD_SCAN,
	AICMD_ATTACK,
	AICMD_ATK_HPCOND,
	AICMD_ATK_LVCOND,
	AICMD_RECOVERY,
	AICMD_RANGEATTACK,
	AICMD_KEEP_RANGEATTACK,
	AICMD_SUMMON,
	AICMD_EVADE,
	AICMD_HELPER,
	AICMD_BERSERK,
	AICMD_RANDOMTARGET,
	AICMD_LOOT,
};


BOOL CProject::LoadPropMoverEx_AI_SCAN(LPCTSTR szFileName, CScript& script, int nVal)
{
	AICMD nCommand = AICMD_NONE;

	script.GetToken();	// {
	if (script.token[0] != '{')
	{
		Error("%s(%d) : MoverID=%d SCAN 섹터블럭 { 빠졌음.", szFileName, script.GetLineNum(), nVal);
		return FALSE;
	}

	MoverProp* pProp = GetMoverProp(nVal);

	while (1)
	{
		script.GetToken();
		if (script.token[0] == '}')
			break;

		if (script.tokenType == IDENTIFIER)
		{
			if (nCommand == AICMD_SCAN)
			{
				if (strcmpi(script.token, "job") == 0)
					pProp->m_nScanJob = script.GetNumber();
				else
					if (strcmpi(script.token, "range") == 0)
						pProp->m_nAttackFirstRange = script.GetNumber();
					else
						if (strcmpi(script.token, "quest") == 0)
							pProp->m_dwScanQuestId = script.GetNumber();
						else
							if (strcmpi(script.token, "item") == 0)
								pProp->m_dwScanItemIdx = script.GetNumber();
							else
								if (strcmpi(script.token, "chao") == 0)
									pProp->m_nScanChao = script.GetNumber();
								else
								{

									SYNTAX_ERROR;
									return FALSE;
								}
			}
			if (strcmpi(script.Token, "scan") == 0)
			{
				if (nCommand)
				{
					SYNTAX_ERROR;
					return FALSE;
				}
				nCommand = AICMD_SCAN;
				//pProp->m_nAttackFirstRange = 3;
			}
		}
	}

	return TRUE;
}


BOOL CProject::LoadPropMoverEx_AI_BATTLE(LPCTSTR szFileName, CScript& script, int nVal)
{
	AICMD nCommand = AICMD_NONE;

	script.GetToken();	// {
	if (script.token[0] != '{')
	{
		Error("%s(%d) : MoverID=%d BATTLE 섹터블럭 { 빠졌음.", szFileName, script.GetLineNum(), nVal);
		return FALSE;
	}

	MoverProp* pProp = GetMoverProp(nVal);

	while (1)
	{
		script.GetToken();
		if (script.token[0] == '}')
			break;

		if (script.tokenType == IDENTIFIER)
		{
			//---- Attack
			if (strcmpi(script.token, "Attack") == 0)
			{
				nCommand = AICMD_ATTACK;
				pProp->m_bMeleeAttack = 1;
			}
			else
				//--- cunning
				if (strcmpi(script.token, "cunning") == 0)
				{
					if (nCommand == 0)
					{
						SYNTAX_ERROR;
						return FALSE;
					}
					else
						if (nCommand == AICMD_ATTACK)
						{
							script.GetToken();
							if (strcmpi(script.token, "low") == 0)
								pProp->m_nLvCond = 1;
							else if (strcmpi(script.token, "Sam") == 0)
								pProp->m_nLvCond = 2;
							else if (strcmpi(script.token, "Hi") == 0)
								pProp->m_nLvCond = 3;
							else
							{
								SYNTAX_ERROR;
								return FALSE;
							}
						}
				}
				else
					//--- Recovery
					if (strcmpi(script.token, "Recovery") == 0)
					{
						nCommand = AICMD_RECOVERY;
						pProp->m_bRecvCond = 1;
						pProp->m_nRecvCondMe = 0;
						pProp->m_nRecvCondHow = 100;
						pProp->m_nRecvCondMP = 0;
					}
					else

						if (strcmpi(script.token, "u") == 0 ||
							strcmpi(script.token, "m") == 0 ||
							strcmpi(script.token, "a") == 0)
						{
							if (nCommand == 0)
							{
								SYNTAX_ERROR;
								return FALSE;
							}
							else
								if (nCommand == AICMD_RECOVERY)
								{
									//					script.GetToken();
									if (strcmpi(script.token, "u") == 0)
										pProp->m_bRecvCondWho = 1;
									else if (strcmpi(script.token, "m") == 0)
										pProp->m_bRecvCondWho = 2;
									else if (strcmpi(script.token, "a") == 0)
										pProp->m_bRecvCondWho = 3;

									pProp->m_bRecvCond = 2;
								}
						}
						else
							if (strcmpi(script.token, "RangeAttack") == 0)
							{
								nCommand = AICMD_RANGEATTACK;
							}
							else
								if (strcmpi(script.token, "KeepRangeAttack") == 0)
								{
									nCommand = AICMD_KEEP_RANGEATTACK;
								}
								else
									if (strcmpi(script.token, "Summon") == 0)
									{
										nCommand = AICMD_SUMMON;
									}
									else
										if (strcmpi(script.token, "Evade") == 0)
										{
											nCommand = AICMD_EVADE;
										}
										else
											if (strcmpi(script.token, "Helper") == 0)
											{
												nCommand = AICMD_HELPER;
												pProp->m_tmUnitHelp = 0;
												pProp->m_nHelpRangeMul = 2;
												pProp->m_bHelpWho = 1;
												pProp->m_nCallHelperMax = 5;
											}
											else

												if (strcmpi(script.token, "all") == 0 ||
													strcmpi(script.token, "sam") == 0)
												{
													if (nCommand == 0)
													{
														SYNTAX_ERROR;
														return FALSE;
													}
													else
														if (nCommand == AICMD_HELPER)
														{
															if (strcmpi(script.token, "all") == 0)
																pProp->m_bHelpWho = 1;
															else if (strcmpi(script.token, "sam") == 0)
																pProp->m_bHelpWho = 2;
														}
												}
												else
													if (strcmpi(script.token, "Berserk") == 0)
													{
														nCommand = AICMD_BERSERK;
													}
													else
														if (strcmpi(script.token, "Randomtarget") == 0)
														{
														}
														else

															//			{
															//				nCommand = AICMD_LOOT;
															//				pProp->m_nLoot = 1;
															//			} else
														{
															SYNTAX_ERROR;
															return FALSE;
														}
		}
		else

			if (script.tokenType == NUMBER)
			{
				if (nCommand == 0)
				{
					SYNTAX_ERROR;
					return FALSE;
				}
				else
					if (nCommand == AICMD_ATTACK)
					{
						int nNum = atoi(script.token);

						pProp->m_nHPCond = nNum;
					}
					else
						if (nCommand == AICMD_RECOVERY)
						{
							int nNum = atoi(script.token);
							if (pProp->m_nRecvCondMe == 0)
							{
								pProp->m_nRecvCondMe = nNum;
							}
							else
								if (pProp->m_nRecvCondHow == 100)
								{
									pProp->m_nRecvCondHow = nNum;
								}
								else
									if (pProp->m_nRecvCondMP == 0)
									{
										pProp->m_nRecvCondMP = nNum;
									}
						}
						else
							if (nCommand == AICMD_RANGEATTACK || nCommand == AICMD_KEEP_RANGEATTACK)
							{
								int nRange = atoi(script.token);

								if (nCommand == AICMD_KEEP_RANGEATTACK)
									nRange |= 0x80;

								pProp->m_bRangeAttack = nRange;

							}
							else
								if (nCommand == AICMD_SUMMON)
								{
									pProp->m_nSummProb = atoi(script.token);
									pProp->m_nSummNum = script.GetNumber();
									if (pProp->m_nSummNum > MAX_SUMMON)
									{
										CUSTOM_ERROR("소환할수 있는 개수가 너무 많다");
#if __VER >= 12 // __NEW_SUMMON_RULE
										pProp->m_nSummNum = MAX_SUMMON;
#endif // __NEW_SUMMON_RULE
									}

									script.GetToken();
									if (script.tokenType != NUMBER)
										UNIDENTIFY_ERROR;
									pProp->m_nSummID = atoi(script.token);
									if (GetMoverProp(pProp->m_nSummID) == NULL)
										CUSTOM_ERROR("지정된 몬스터 아이디가 프로퍼티에 없음");
								}
								else
									if (nCommand == AICMD_EVADE)
									{
										pProp->m_nRunawayHP = atoi(script.token);
									}
									else
										if (nCommand == AICMD_HELPER)
										{
											int nNum = atoi(script.token);
											if (pProp->m_tmUnitHelp == 0)
												pProp->m_tmUnitHelp = (DWORD)nNum;
											else if (pProp->m_nHelpRangeMul == 2)
												pProp->m_nHelpRangeMul = nNum;
										}
										else
											if (nCommand == AICMD_BERSERK)
											{
												pProp->m_nBerserkHP = atoi(script.token);
												pProp->m_fBerserkDmgMul = script.GetFloat();
												if (pProp->m_fBerserkDmgMul <= 0 || pProp->m_fBerserkDmgMul >= 20.0f)
												{
													CString str;
													str.Format("Berserk명령의 데미지배수의 범위가 이상한거 같은디? %f", pProp->m_fBerserkDmgMul);
													CUSTOM_ERROR((LPCTSTR)str);
												}
											}
			}

	}

	return TRUE;
}


BOOL CProject::LoadPropMoverEx_AI_MOVE(LPCTSTR szFileName, CScript& script, int nVal)
{
	AICMD nCommand = AICMD_NONE;

	script.GetToken();	// {
	if (script.token[0] != '{')
	{
		Error("%s(%d) : MoverID=%d MOVE 섹터블럭 { 빠졌음.", szFileName, script.GetLineNum(), nVal);
		return FALSE;
	}

	MoverProp* pProp = GetMoverProp(nVal);

	while (1)
	{
		script.GetToken();
		if (script.token[0] == '}')
			break;

		if (script.tokenType == IDENTIFIER)
		{
			if (strcmpi(script.token, "Loot") == 0)
			{
				nCommand = AICMD_LOOT;
			}
			else
				if (strcmpi(script.token, "d") == 0)
				{
				}
				else
				{
					SYNTAX_ERROR;
					return FALSE;
				}
		}
		else

			if (script.tokenType == NUMBER)
			{
				if (nCommand == 0)
				{
					SYNTAX_ERROR;
					return FALSE;
				}
				else if (nCommand == AICMD_LOOT)
				{
					if (script.tokenType != NUMBER)
						UNIDENTIFY_ERROR;
				}
			}
	}

	return TRUE;
}



BOOL CProject::LoadPropMoverEx_AI(LPCTSTR szFileName, CScript& script, int nVal)
{
	script.GetToken();		// {

	if (script.token[0] != '{')
	{
		Error("%s(%d) : MoverID=%d AI 다음에 { 없음.", szFileName, script.GetLineNum(), nVal);
		return FALSE;
	}

	while (1)
	{
		script.GetToken();

		if (script.token[0] == '}')
			break;


		if (script.token[0] == '#')
		{
			//			script.GetToken();	// SCAN/BATTLE/MOVING
			if (strcmpi(script.Token, "#SCAN") == 0)
			{
				if (LoadPropMoverEx_AI_SCAN(szFileName, script, nVal) == FALSE)
					return FALSE;
			}
			else
				if (strcmpi(script.Token, "#BATTLE") == 0)
				{
					if (LoadPropMoverEx_AI_BATTLE(szFileName, script, nVal) == FALSE)
						return FALSE;
				}
				else
					if (strcmpi(script.Token, "#MOVE") == 0)
					{
						if (LoadPropMoverEx_AI_MOVE(szFileName, script, nVal) == FALSE)
							return FALSE;
					}
					else
					{
						Error("%s(%d) : MoverID=%d 섹터블럭이름이 이상함(%s) ", szFileName, script.GetLineNum(), nVal, script.Token);
						return FALSE;
					}
		}
		else
		{

			Error("%s(%d) : MoverID=%d 섹터블럭(<)이 없음.", szFileName, script.GetLineNum(), nVal);
			return FALSE;
		}
	}

	return TRUE;
}

