#pragma once

#ifdef __MODEL_CACHE
class CBetterCache
{
	std::map<string, pair<CObject3D*, DWORD>> cacheEntities;
	std::map<string, pair<CObject3D*, DWORD>> cacheObjects;
	std::map<string, pair<CObject3D*, DWORD>> cacheOthers;
public:
	~CBetterCache();

	void Clear();
	void DeleteDeviceObjects();
	CObject3D* GetCacheModel(string szFileName, int nType);

	void InsertCacheModel(string szFileName, CObject3D* pObject3D, int nType);
	void Process();
	void DecreaseObjectCount(CObject3D* pObject3D, int nType);

	void ClearCacheEntities() { cacheEntities.clear(); }
	void ClearCacheObjects() { cacheObjects.clear(); }
	void ClearCacheOthers() { cacheOthers.clear(); }

	static CBetterCache* CBetterCache::GetInstance()
	{
		static CBetterCache sGetInstance;
		return &sGetInstance;
	}
};
#endif
