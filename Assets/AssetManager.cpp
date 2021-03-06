#include "Assets/AssetManager.h"

#include "System/System.h"
#include "Graphics/Graphics.h"

AssetManager::AssetManager( System& sys ) : log( "AssetManager" ), graphics( sys.graphics )
{
}

AssetManager::~AssetManager( void )
{
	CleanUp();
}

void AssetManager::AccquireAllMeshes( void )
{
	std::map<std::string, Mesh>::iterator it;

	for(it = meshAssets.begin(); it != meshAssets.end(); it++)
	{
		AccquireMeshResources( it->second );
	}
}

void AssetManager::ReleaseAllMeshes( void )
{
	std::map<std::string, Mesh>::iterator it;

	for(it = meshAssets.begin(); it != meshAssets.end(); it++)
	{
		ReleaseMeshResources( it->second );
	}
}

void AssetManager::CleanUp( void )
{
	ReleaseAllMeshes();
}

void AssetManager::OnLost( void )
{
	ReleaseAllMeshes();
}

void AssetManager::OnRecover( void )
{
	AccquireAllMeshes();
}