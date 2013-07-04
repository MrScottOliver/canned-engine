#include "Game\Scenes\TestScene.h"
#include "SceneManager\SceneManager.h"

TestScene::TestScene( void ) : IScene( "TestScene" ), cubeBuffer( NULL ), cubeTex( NULL ), font( NULL )
{
	D3DXMatrixTranslation( &matWorld, 0, 0, 0 );
	D3DXMatrixLookAtLH( &matView, &(D3DXVECTOR3( 3.0f, 3.0f, -3.0f )), &(D3DXVECTOR3( 0.0f, 0.0f, 0.0f )), &(D3DXVECTOR3( 0.0f, 1.0f, 0.0f )) );
	D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( 90.0f ), (float)settings->client.xResolution/(float)settings->client.yResolution, 1.0f, 100.0f );

	FontPosition.top = 0;
	FontPosition.left = 0;
	FontPosition.right = settings->client.xResolution;
	FontPosition.bottom = settings->client.yResolution;
}

TestScene::~TestScene( void )
{
}

void TestScene::Load( void )
{
	log.Message( "Loading", true );
	Vertex3dTx verts[] = { 
	// Front Face (1-2-3-4)
	{ -1.0f, 1.0f, -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, -1.0f, 1.0f, 0.0f }, { -1.0f, -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
	// Right Face (2-6-4-8)
	{ 1.0f, 1.0f, -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f, 0.0f }, { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, -1.0f, 1.0f, 1.0f, 1.0f },
	// Top Face (5-6-1-2)
	{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, -1.0f, 1.0f, 1.0f },
	// Back Face (6-5-8-7)
	{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, { -1.0f, 1.0f, 1.0f, 1.0f, 0.0f }, { 1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f },
	// Left Face (5-1-7-3)
	{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, { -1.0f, 1.0f, -1.0f, 1.0f, 0.0f }, { -1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
	// Bottom Face (3-4-7-8)
	{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f }, { 1.0f, -1.0f, -1.0f, 1.0f, 0.0f }, { -1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, -1.0f, 1.0f, 1.0f, 1.0f }
	};

	graphics->ErrorCheck( device->CreateVertexBuffer( 24*sizeof(Vertex3dTx), D3DUSAGE_WRITEONLY, Vertex3dTx::format, D3DPOOL_DEFAULT, &cubeBuffer, NULL ), TEXT( "cubeBuffer creation failed" ) );

	VOID* pVoid;
	graphics->ErrorCheck( cubeBuffer->Lock( 0, 0, (void**)&pVoid, 0 ), TEXT( "SplashScreen: Error locking cube buffer" ) );
	memcpy( pVoid, verts, sizeof(verts) );
	graphics->ErrorCheck( cubeBuffer->Unlock(), TEXT( "SplashScreen: Error unlocking cube buffer" ) );

	graphics->ErrorCheck( D3DXCreateTextureFromFile( device, TEXT( "C:/Users/Scott/Programming/Projects/GitHub/canned-engine/Game/Textures/qMark.bmp" ), &cubeTex ), TEXT( "Failed creating texture" ) );

	D3DXFONT_DESC FontDesc = { 24, 0, 400, 0, false, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_PITCH, TEXT( "Arial" ) };
	D3DXCreateFontIndirect( device, &FontDesc, &font );

	//Always set state and report
	loaded = true;
	manager->ReportFinishedLoading( this );
}

void TestScene::OnLost( void )
{
	if( !lost )
	{
		log.Message( "Lost, releasing resources", true );
		if( cubeBuffer != NULL )
		{
			log.Message( "Releasing cube buffer", true );
			cubeBuffer->Release();
			cubeBuffer = NULL;
		}
		if( font != NULL )
		{
			graphics->ErrorCheck( font->OnLostDevice(), TEXT( "Error Font, lost device" ) );
		
		}

		lost = true;
	}
}

void TestScene::OnRecover( void )
{
	if( lost )
	{
		log.Message( "ReLoading", true );
		graphics->SetRenderStates();
		Vertex3dTx verts[] = { 
		// Front Face (1-2-3-4)
		{ -1.0f, 1.0f, -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, -1.0f, 1.0f, 0.0f }, { -1.0f, -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
		// Right Face (2-6-4-8)
		{ 1.0f, 1.0f, -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f, 0.0f }, { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, -1.0f, 1.0f, 1.0f, 1.0f },
		// Top Face (5-6-1-2)
		{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, -1.0f, 1.0f, 1.0f },
		// Back Face (6-5-8-7)
		{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, { -1.0f, 1.0f, 1.0f, 1.0f, 0.0f }, { 1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f },
		// Left Face (5-1-7-3)
		{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, { -1.0f, 1.0f, -1.0f, 1.0f, 0.0f }, { -1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
		// Bottom Face (3-4-7-8)
		{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f }, { 1.0f, -1.0f, -1.0f, 1.0f, 0.0f }, { -1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, -1.0f, 1.0f, 1.0f, 1.0f }
		};

		graphics->ErrorCheck( device->CreateVertexBuffer( 24*sizeof(Vertex3dTx), D3DUSAGE_WRITEONLY, Vertex3dTx::format, D3DPOOL_DEFAULT, &cubeBuffer, NULL ), TEXT( "cubeBuffer creation failed" ) );

		VOID* pVoid;
		graphics->ErrorCheck( cubeBuffer->Lock( 0, 0, (void**)&pVoid, 0 ), TEXT( "SplashScreen: Error locking cube buffer" ) );
		memcpy( pVoid, verts, sizeof(verts) );
		graphics->ErrorCheck( cubeBuffer->Unlock(), TEXT( "SplashScreen: Error unlocking cube buffer" ) );

		//D3DXFONT_DESC FontDesc = { 24, 0, 400, 0, false, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_PITCH, TEXT( "Arial" ) };
		//D3DXCreateFontIndirect( device, &FontDesc, &font );
		graphics->ErrorCheck( font->OnResetDevice(), TEXT( "Error recovering font" ) );

		lost = false;
	}
}

void TestScene::Unload( void )
{
	if( cubeBuffer != NULL )
	{
		cubeBuffer->Release();
		cubeBuffer = NULL;
	}
	if( cubeTex != NULL )
	{
		cubeTex->Release();
		cubeTex = NULL;
	}
	if( font != NULL )
	{
		font->Release();
		font = NULL;
	}
	//Always set state and report
	loaded = false;
	manager->ReportFinishedUnloading( this );
}

void TestScene::FadeIn( void )
{
	SetState( update );
}

void TestScene::MainLoop( void )
{
}

void TestScene::FadeOut( void )
{
	Unload();
}

void TestScene::RenderIn( void )
{
}

void TestScene::RenderMain( void )
{
	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 150, 150, 150 ), 1.0f, 0 );

	device->BeginScene();

	graphics->ErrorCheck( device->SetRenderState( D3DRS_LIGHTING, false ), TEXT( "Setting Lighting False" ) );

	device->SetFVF(Vertex3dTx::format);

	device->SetTransform( D3DTS_WORLD, &matWorld );
	device->SetTransform( D3DTS_VIEW, &matView );
	device->SetTransform( D3DTS_PROJECTION, &matProj );

	graphics->ErrorCheck( device->SetStreamSource( 0, cubeBuffer, 0, sizeof(Vertex3dTx) ), TEXT( "Setting stream source" ) );
	device->SetTexture( 0, cubeTex );

	graphics->ErrorCheck( device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 ), TEXT( "Drawing cube" ) );
	graphics->ErrorCheck( device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 4, 2 ), TEXT( "Drawing cube" ) );
	graphics->ErrorCheck( device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 8, 2 ), TEXT( "Drawing cube" ) );
	graphics->ErrorCheck( device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 12, 2 ), TEXT( "Drawing cube" ) );
	graphics->ErrorCheck( device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 16, 2 ), TEXT( "Drawing cube" ) );
	graphics->ErrorCheck( device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 20, 2 ), TEXT( "Drawing cube" ) );

	font->DrawText( NULL, TEXT( "Text Sample Using D3DXFont" ), -1, &FontPosition, DT_CENTER | DT_BOTTOM, 0xffffffff );

	device->EndScene();

	device->Present( NULL, NULL, NULL, NULL );
}

void TestScene::RenderOut( void )
{
}

//device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
//device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);	//Use the diffuse component of the vertex
//device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
//device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
//device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
//device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);