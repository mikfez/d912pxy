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

#define API_OVERHEAD_TRACK_LOCAL_ID_DEFINE PXY_METRICS_API_OVERHEAD_DEVICE_SHADERS

HRESULT WINAPI d912pxy_device::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	LOG_DBG_DTDM(__FUNCTION__);

	API_OVERHEAD_TRACK_START(0)

	d912pxy_vshader* shd = (d912pxy_vshader*)pShader;

	d912pxy_s(psoCache)->VShader(shd);

	API_OVERHEAD_TRACK_END(0)

	return D3D_OK;
}

HRESULT WINAPI d912pxy_device::SetPixelShader(IDirect3DPixelShader9* pShader) 
{
	LOG_DBG_DTDM(__FUNCTION__);

	API_OVERHEAD_TRACK_START(0)

	d912pxy_pshader* shd = (d912pxy_pshader*)pShader;

	d912pxy_s(psoCache)->PShader(shd);

	API_OVERHEAD_TRACK_END(0)

	return D3D_OK;
}

HRESULT WINAPI d912pxy_device::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{ 
	LOG_DBG_DTDM(__FUNCTION__);

	API_OVERHEAD_TRACK_START(0)

#ifdef _DEBUG
	if (PXY_INNER_MAX_SHADER_CONSTS <= ((StartRegister + Vector4fCount) * 4))
	{
		LOG_DBG_DTDM("too many shader consts, trimming");
		Vector4fCount = PXY_INNER_MAX_SHADER_CONSTS/4 - StartRegister;
	}
#endif

	d912pxy_s(batch)->SetShaderConstF(0, StartRegister, Vector4fCount, (float*)pConstantData);

	API_OVERHEAD_TRACK_END(0)

	return D3D_OK;
}

HRESULT WINAPI d912pxy_device::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) 
{ 
	LOG_DBG_DTDM(__FUNCTION__);

	API_OVERHEAD_TRACK_START(0)

#ifdef _DEBUG
	if (PXY_INNER_MAX_SHADER_CONSTS <= ((StartRegister + Vector4fCount) * 4))
	{
		LOG_DBG_DTDM3("too many shader consts, trimming");
		Vector4fCount = PXY_INNER_MAX_SHADER_CONSTS/4 - StartRegister;
	}
#endif

	d912pxy_s(batch)->SetShaderConstF(1, StartRegister, Vector4fCount, (float*)pConstantData);

	API_OVERHEAD_TRACK_END(0)

	return D3D_OK;
}

ID3D12RootSignature * d912pxy_device::ConstructRootSignature(D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc)
{
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	ID3D12RootSignature* rsObj;

	HRESULT ret;

	ret = D3D12SerializeRootSignature(rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);

	if (FAILED(ret))
	{
		if ((error != NULL) && (error->GetBufferSize()))
		{
			LOG_ERR_DTDM("error: %S", error->GetBufferPointer());
		}
		LOG_ERR_THROW2(ret, "SerializeRootSignature failed");
	}

	LOG_ERR_THROW2(d912pxy_s(DXDev)->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rsObj)), "CreateRootSignature failed");

	return rsObj;
}

#ifdef TRACK_SHADER_BUGS_PROFILE

void d912pxy_device::TrackShaderCodeBugs(UINT type, UINT val, d912pxy_shader_uid faultyId)
{
	UINT32 size;
	UINT32* data = (UINT32*)d912pxy_s(vfs)->LoadFileH(faultyId, &size, PXY_VFS_BID_SHADER_PROFILE);

	if (data == NULL)
	{
		data = (UINT32*)malloc(PXY_INNER_SHDR_BUG_FILE_SIZE);
		ZeroMemory(data, PXY_INNER_SHDR_BUG_FILE_SIZE);
		data[type] = val;

		d912pxy_s(vfs)->WriteFileH(faultyId, data, PXY_INNER_SHDR_BUG_FILE_SIZE, PXY_VFS_BID_SHADER_PROFILE);
	}
	else {

		if (size != PXY_INNER_SHDR_BUG_FILE_SIZE)
		{
			LOG_ERR_THROW2(-1, "wrong shader profile file size");
		}

		if (data[type] != val)
		{
			data[type] = val;

			d912pxy_s(vfs)->ReWriteFileH(faultyId, data, PXY_INNER_SHDR_BUG_FILE_SIZE, PXY_VFS_BID_SHADER_PROFILE);
		}
	}

	free(data);
}

#endif

#undef API_OVERHEAD_TRACK_LOCAL_ID_DEFINE 