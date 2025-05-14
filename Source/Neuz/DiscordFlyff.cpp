#include "StdAfx.h"
#ifdef __DISCORD
#include "DiscordFlyff.h"
#include "Continent.h"
#include "mover.h"
#include "worldmng.h"
#include "ResData.h"

DiscordRichPresence* DiscordFlyff::GetInstance()
{
	static DiscordRichPresence sRPC;
	return &sRPC;
}
void DiscordFlyff::Initialize()
{
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));
	Discord_Initialize("806653452792496148", &Handle, 1, NULL);

	SetStartTime();
}

void DiscordFlyff::ShutDown()
{
	Discord_ClearPresence();
	Discord_Shutdown();
}

void DiscordFlyff::Update()
{
	Discord_UpdatePresence(GetInstance());
}

void DiscordFlyff::SetStartTime()
{
	DiscordRichPresence* rpc = GetInstance();
	rpc->startTimestamp = time(NULL);
	Update();
}

void DiscordFlyff::SetDefaultDetails()
{
	DiscordRichPresence* rpc = GetInstance();
	rpc->largeImageKey = "bigflyff";
	rpc->largeImageText = "google.com";
	rpc->state = "Login stage";
	rpc->details = "";
	Update();
}

void DiscordFlyff::SetCharacterDetails()
{
	if (!g_pPlayer)
		return;

	DiscordRichPresence* rpc = GetInstance();

	// state (name & level)
	CString strDiscordName = g_pPlayer->GetName();
	strDiscordName.Format("%s | lv. %d", g_pPlayer->GetName(), g_pPlayer->GetLevel());
	rpc->state = strDiscordName;

	// details (world/continent)
	CString strDiscordWorld = "";
	CWorld* pWorld = g_pPlayer->GetWorld();
	if (pWorld)
		strDiscordWorld.Format("%s", g_WorldMng.GetWorldStruct(pWorld->GetID())->m_szWorldName);
	rpc->details = strDiscordWorld;

	Update();
}
#endif