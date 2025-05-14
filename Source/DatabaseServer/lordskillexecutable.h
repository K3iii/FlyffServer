#ifndef __LORD_SKILL_EXECUTABLE_H__
#define	__LORD_SKILL_EXECUTABLE_H__

#include "lordskill.h"

class CLordSkillComponentODBC
	: public CLordSkillComponentExecutable
{
public:
	CLordSkillComponentODBC();
	virtual ~CLordSkillComponentODBC();
public:
	virtual	void	Execute( u_long idPlayer, u_long idTarget, VOID* pParam );
};

#endif	// __LORD_SKILL_EXECUTABLE_H__