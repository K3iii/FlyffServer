#include "StdAfx.h"
#include "BetterCache.h"

#ifdef __MODEL_CACHE
CBetterCache::~CBetterCache()
{
	Clear();
	cacheEntities.clear();
	cacheObjects.clear();
	cacheOthers.clear();
}

void CBetterCache::Clear()
{
	for (map<string, pair<CObject3D*, DWORD>>::iterator i = cacheEntities.begin(); i != cacheEntities.end(); ++i)
	{
		if (i->second.first)
		{
			i->second.first->DeleteDeviceObjects();
			SAFE_DELETE(i->second.first);
		}
	}
	for (map<string, pair<CObject3D*, DWORD>>::iterator i = cacheObjects.begin(); i != cacheObjects.end(); ++i)
	{
		if (i->second.first)
		{
			i->second.first->DeleteDeviceObjects();
			SAFE_DELETE(i->second.first);
		}
	}
	for (map<string, pair<CObject3D*, DWORD>>::iterator i = cacheOthers.begin(); i != cacheOthers.end(); ++i)
	{
		if (i->second.first)
		{
			i->second.first->DeleteDeviceObjects();
			SAFE_DELETE(i->second.first);
		}
	}
}

void CBetterCache::DeleteDeviceObjects()
{
	for (map<string, pair<CObject3D*, DWORD>>::iterator i = cacheEntities.begin(); i != cacheEntities.end(); ++i)
	{
		if (i->second.first)
			i->second.first->DeleteDeviceObjects();
	}
	for (map<string, pair<CObject3D*, DWORD>>::iterator i = cacheObjects.begin(); i != cacheObjects.end(); ++i)
	{
		if (i->second.first)
			i->second.first->DeleteDeviceObjects();
	}
	for (map<string, pair<CObject3D*, DWORD>>::iterator i = cacheOthers.begin(); i != cacheOthers.end(); ++i)
	{
		if (i->second.first)
			i->second.first->DeleteDeviceObjects();
	}
}

CObject3D* CBetterCache::GetCacheModel(string szFileName, int nType)
{
	map<string, pair<CObject3D*, DWORD>>::iterator itCacheObject;
	if (nType == MODELTYPE_ANIMATED_MESH)
	{
		itCacheObject = cacheEntities.find(szFileName);
		if (itCacheObject != cacheEntities.end())
		{
			if (itCacheObject->second.first)
				itCacheObject->second.first->m_nUseCnt++;
			itCacheObject->second.second = GetTickCount() + MIN(__MODEL_CACHE);
			return itCacheObject->second.first;
		}
	}
	else if (nType == MODELTYPE_MESH)
	{
		itCacheObject = cacheObjects.find(szFileName);
		if (itCacheObject != cacheObjects.end())
		{
			if (itCacheObject->second.first)
				itCacheObject->second.first->m_nUseCnt++;
			itCacheObject->second.second = GetTickCount() + MIN(__MODEL_CACHE);
			return itCacheObject->second.first;
		}
	}
	else
	{
		itCacheObject = cacheOthers.find(szFileName);
		if (itCacheObject != cacheOthers.end())
		{
			if (itCacheObject->second.first)
				itCacheObject->second.first->m_nUseCnt++;
			itCacheObject->second.second = GetTickCount() + MIN(__MODEL_CACHE);
			return itCacheObject->second.first;
		}
	}
	return NULL;
}


void CBetterCache::InsertCacheModel(string szFileName, CObject3D* pObject3D, int nType)
{
	if (pObject3D == NULL)
		return;

	switch (nType)
	{
	case MODELTYPE_ANIMATED_MESH:
		cacheEntities[szFileName] = { pObject3D, GetTickCount() + MIN(__MODEL_CACHE) };
		break;
	case MODELTYPE_MESH:
		cacheObjects[szFileName] = { pObject3D, GetTickCount() + MIN(__MODEL_CACHE) };
		break;
	default:
		cacheOthers[szFileName] = { pObject3D, GetTickCount() + MIN(__MODEL_CACHE) };
		break;
	}
}
void CBetterCache::Process()
{
	map<string, pair<CObject3D*, DWORD>>::iterator itCacheObject;
	itCacheObject = cacheObjects.begin();
	while (itCacheObject != cacheObjects.end())
	{
		if (itCacheObject->second.second < GetTickCount())
		{
			CObject3D* pObject3D = itCacheObject->second.first;
			if (pObject3D)
			{
				if (pObject3D->m_nUseCnt < 1)
				{
					SAFE_DELETE(pObject3D);
					itCacheObject = cacheObjects.erase(itCacheObject);
				}
				else
					++itCacheObject;
			}
			else
				++itCacheObject;
		}
		else
			++itCacheObject;
	}
	itCacheObject = cacheEntities.begin();
	while (itCacheObject != cacheEntities.end())
	{
		if (itCacheObject->second.second < GetTickCount())
		{
			CObject3D* pObject3D = itCacheObject->second.first;
			if (pObject3D)
			{
				if (pObject3D->m_nUseCnt < 1)
				{
					SAFE_DELETE(pObject3D);
					itCacheObject = cacheEntities.erase(itCacheObject);
				}
				else
					++itCacheObject;
			}
			else
				++itCacheObject;
		}
		else
			++itCacheObject;
	}
	itCacheObject = cacheOthers.begin();
	while (itCacheObject != cacheOthers.end())
	{
		if (itCacheObject->second.second < GetTickCount())
		{
			CObject3D* pObject3D = itCacheObject->second.first;
			if (pObject3D)
			{
				if (pObject3D->m_nUseCnt < 1)
				{
					SAFE_DELETE(pObject3D);
					itCacheObject = cacheOthers.erase(itCacheObject);
				}
				else
					++itCacheObject;
			}
			else
				++itCacheObject;
		}
		else
			++itCacheObject;
	}
}
void CBetterCache::DecreaseObjectCount(CObject3D* pObject3D, int nType)
{
	if (nType == MODELTYPE_ANIMATED_MESH)
	{
		for (auto& itObject : cacheEntities)
		{
			if (pObject3D == itObject.second.first && itObject.second.first->m_nUseCnt > 0)
				itObject.second.first->m_nUseCnt--;
		}
	}
	else if (nType == MODELTYPE_MESH)
	{
		for (auto& itObject : cacheObjects)
		{
			if (pObject3D == itObject.second.first && itObject.second.first->m_nUseCnt > 0)
				itObject.second.first->m_nUseCnt--;
		}
	}
	else
	{
		for (auto& itObject : cacheOthers)
		{
			if (pObject3D == itObject.second.first && itObject.second.first->m_nUseCnt > 0)
				itObject.second.first->m_nUseCnt--;
		}
	}
}
#endif