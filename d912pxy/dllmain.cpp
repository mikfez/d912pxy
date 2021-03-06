/*
MIT License

Copyright(c) 2018-2019 megai2

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include "stdafx.h"

extern "C" HRESULT WINAPI CreateD912PXY(D3DPRESENT_PARAMETERS* presPars, IDirect3DDevice9** dev)
{
	d912pxy_device* proxy = new d912pxy_device(NULL, presPars);

	*dev = (IDirect3DDevice9*)proxy;

	return D3D_OK;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

	if (!d3d9_proxy_dll_main(hModule, ul_reason_for_call, lpReserved))
		return FALSE;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		//megai2: load config at dll load
		new d912pxy_config();		
		new d912pxy_log();

		D3D9ProxyCb_set_OnDevCreate(&app_cb_D3D9Dev_create);
		D3D9ProxyCb_set_OnDevDestroy(&app_cb_D3D9Dev_destroy);

		break;
	}
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}