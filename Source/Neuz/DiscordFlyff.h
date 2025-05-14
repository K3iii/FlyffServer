#ifdef __DISCORD
#include <discord_register.h>
#include <discord_rpc.h>

class DiscordFlyff 
{
	static DiscordRichPresence* GetInstance();

public:
	void Initialize();
	void ShutDown();
	void Update();

	void SetStartTime();
	void SetDefaultDetails();
	void SetCharacterDetails();
};
#endif