#include "Graphics/Graphics.h"
#include "System/System.h"
#include "Window/WindowMS.h"

#include <iostream>
#include <sstream>

using std::string;
using std::ostringstream;

//TODO: Replace window references with message system

Graphics::Graphics( System &s ) : 	log("Graphics"), system( s ), mInterface( NULL ), mDevice( NULL ),
									presentParameters( NULL ), adapterCount( 0 ), modeCount( 0 ), adapter( D3DADAPTER_DEFAULT ), mode( 0 ),
									deviceType( D3DDEVTYPE_HAL ), backbufferFormat( D3DFMT_A8R8G8B8 ), depthFormat( D3DFMT_D16 ),
									behaviourFlags( D3DCREATE_HARDWARE_VERTEXPROCESSING ), qualityAA( 0 ), bufferCount( 1 ), AA( D3DMULTISAMPLE_NONE ), modes( NULL ),
									refresh( 0 ), aspect( FourThree ), showCursorFullscreen( false ), dModes(), adapterList()
{
}

Graphics::~Graphics( void )
{
	ReleaseTextures();
	
	WriteSettings();
}

LPDIRECT3D9 const Graphics::Interface( void ) const
{
	return mInterface;
}

LPDIRECT3DDEVICE9 const Graphics::Device( void ) const
{
	return mDevice;
}

bool Graphics::IsDeviceLost( void )
{
	CheckDevice();

	return ( deviceState != D3D_OK );
}

void Graphics::Init( void )
{
	CreateInterface();

	GetAdaptersFromInterface();

	ReadSettings();

	ApplySettings();

	GetModesFromInterface();

	SetParameters();

	CreateDevice();
}

void Graphics::SetDebugStates( void )
{
	ErrorCheck( mDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME ), TEXT( "Setting Wireframe mode" ) );
	ErrorCheck( mDevice->SetRenderState( D3DRS_LIGHTING, false ), TEXT( "Setting Lighting State Failed" ) );
	ErrorCheck( mDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ), TEXT( "Setting Culling Mode Failed" ) );
}

void Graphics::CleanUp( void )
{
	ReleaseTextures();

	if( modes !=  NULL )
	{
		delete[] modes;
		modes = NULL;
	}

    if( mDevice != NULL )
	{
		mDevice->Release();
		mDevice = NULL;
	}

    if( mInterface != NULL )
	{
		mInterface->Release();
		mInterface = NULL;
	}
}

void Graphics::CheckDevice( void )
{
	deviceState = ( mDevice->TestCooperativeLevel() );
}

bool Graphics::Reset( void )
{
	TRACE( "Attempting Reset" );
	HRESULT result = mDevice->Reset( presentParameters );

	if( result == D3D_OK )
	{
		TRACE( "Device reset" );
	}
	else
	{
		TRACE( "Device not ok" );
	}

	if( result == D3DERR_DEVICELOST )
	{
		TRACE( "Device Lost" );
	}

	if( result == D3DERR_DEVICENOTRESET )
	{
		TRACE( "Device not reset" );
	}

	return ( D3D_OK == result );
}

void Graphics::Refresh( void )
{
	ReadSettings();

	ApplySettings();

	SetParameters();

	//TODO: Replace with message system?
	system.assets.OnLost();
	system.sceneManager.OnLost();

	if( Reset() )
	{
		TRACE( "Recovering" );
		
		system.assets.OnRecover();
		system.sceneManager.OnRecover();
	}
}

void Graphics::ReadSettings( void )
{
	fullscreen = system.GlobalSettings["display"]["fullscreen"];
	xResolution = system.GlobalSettings["display"]["xResolution"];
	yResolution = system.GlobalSettings["display"]["yResolution"];
	aspect = (ASPECT)system.GlobalSettings["display"]["aspect"];
	refresh = system.GlobalSettings["display"]["refresh"];
	AA = (D3DMULTISAMPLE_TYPE)system.GlobalSettings["display"]["multisample"];
	mode = system.GlobalSettings["display"]["displayMode"];
	backbufferFormat = (D3DFORMAT)system.GlobalSettings["display"]["bufferFormat"];
	depthFormat = (D3DFORMAT)system.GlobalSettings["display"]["depthFormat"];
	adapter = system.GlobalSettings["display"]["adapter"];
}

void Graphics::WriteSettings( void )
{
	system.GlobalSettings["display"]["fullscreen"] = fullscreen;
	system.GlobalSettings["display"]["xResolution"] = xResolution;
	system.GlobalSettings["display"]["yResolution"] = yResolution;
	system.GlobalSettings["display"]["aspect"] = aspect;
	system.GlobalSettings["display"]["refresh"] = refresh;
	system.GlobalSettings["display"]["multisample"] = AA;
	system.GlobalSettings["display"]["displayMode"] = mode;
	system.GlobalSettings["display"]["bufferFormat"] = backbufferFormat;
	system.GlobalSettings["display"]["depthFormat"] = depthFormat;
	system.GlobalSettings["display"]["bufferCount"] = bufferCount;
	system.GlobalSettings["display"]["adapter"] = adapter;
}

void Graphics::ApplySettings( void )
{
	SelectAdapter( adapter );

	if( bufferCount < 0 || bufferCount > 2 ){ DoubleBuffer(); }

	SelectBufferFormat( backbufferFormat );

	SelectDepthFormat( depthFormat );

	SelectMultisample( AA );
}

void Graphics::SetFullscreen( void )
{
	if( !fullscreen )
	{
		fullscreen = true;

		WriteSettings();

		Refresh();

		if( showCursorFullscreen )
		{
			system.window.CursorVisible( true );
		}
		else
		{
			system.window.CursorVisible( false );
		}
	}
	else
	{
		TRACE( "Already Fullscreen, command ignored" );
	}
}

void Graphics::SetWindowed( void )
{
	if( fullscreen )
	{
		fullscreen = false;

		WriteSettings();

		Refresh();

		system.window.Update();

		system.window.CursorVisible( true );
	}
	else
	{
		TRACE( "Already Windowed, command ignored" );
	}
}

void Graphics::SetClientSize( const int& width, const int& height )
{
	if( !fullscreen )
	{
		system.window.SetClientSize( width, height );
		system.window.Update();
	}
}

void Graphics::ToggleFullscreen( void )
{
	if( !fullscreen )
	{
		SetFullscreen();
	}
	else
	{
		SetWindowed();
	}
}

//TODO: Move to free function (ErrorCheck_DX9 ?)
void Graphics::ErrorCheck( HRESULT result, const std::string& info )
{
	if( result == D3D_OK )
		return;
	
	string text("");

	switch( result )
	{
		case D3DERR_INVALIDCALL:
			text = "D3DERR_INVALIDCALL: Invalid Call or Parameter!";
			break;
		case D3DERR_NOTAVAILABLE:
			text = "D3DERR_NOTAVAILABLE: Parameter not supported!";
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			text = "D3DERR_OUTOFVIDEOMEMORY: Out of Video Memory!";
			break;
		case  D3DXERR_INVALIDDATA:
			text = "D3DXERR_INVALIDDATA: Invalid data passed to D3DX function!";
			break;
		case D3DERR_DEVICELOST:
			text = "D3DERR_DEVICELOST: The device is lost!";
			break;
		case D3DERR_DRIVERINTERNALERROR:
			text = "D3DERR_DRIVERINTERNALERROR: Internal Driver Error!";
			break;
		case E_OUTOFMEMORY:
			text = "E_OUTOFMEMORY: Out of Memory";
		default:
			text = "Unknown Error: " + result;
			break;
	}

	string error = "Graphics: " + text + "\n" + info;

	throw std::exception( error.c_str() );
}