#include"LightShader.h"

LightShader::LightShader()
{
	this->vertexShader = nullptr;
	this->pixelShader = nullptr;
	this->geometryShader = nullptr;
	this->vertexLayout = nullptr;
	this->depthStencilState = nullptr;
	this->disDepthStencilState = nullptr;
	this->rasState = nullptr;
}

LightShader::~LightShader()
{
}

bool LightShader::Initialize(ID3D11Device * device, HWND hwnd)
{

	bool result = false;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd);//, L"../Engine/color.vs", L"../Engine/color.ps");
	if (!result)
	{
		result = false;
	}
	else
		result = true;


	this->data = (MatrixBuffers*)_aligned_malloc(sizeof(MatrixBuffers), 16);
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(MatrixBuffers);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA pData;
	ZeroMemory(&pData, sizeof(pData));
	pData.pSysMem = this->data;
	pData.SysMemPitch = 0;
	pData.SysMemSlicePitch = 0;

	HRESULT hr;
	hr = device->CreateBuffer(&desc, &pData, &MatrixPerFrameBuffer);
	if (FAILED(hr))
	{
		// deal with error...
		result = false;
	}
	//D3D11_SUBRESOURCE_DATA pDataSpec;
	D3D11_BUFFER_DESC lbDesc;
	lbDesc.ByteWidth = sizeof(AnyLight);
	lbDesc.Usage = D3D11_USAGE_DYNAMIC;
	lbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lbDesc.MiscFlags = 0;
	lbDesc.StructureByteStride = 0;

	this->dataSpec = (AnyLight*)_aligned_malloc(sizeof(AnyLight), 16);

	hr = device->CreateBuffer(&lbDesc, nullptr, &LightPerFrameBuffer);
	// Setup the description of the dynamic constant buffer that is in the vertex shader.
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(AnyLight);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	//hr = device->CreateBuffer(&desc, NULL, &LightPerFrameBuffer);
	//if (FAILED(hr))
	//{
	//	// deal with error...
	//	result = false;
	//}
	//first pass
	D3D11_DEPTH_STENCIL_DESC depthStencilDescL;
	ZeroMemory(&depthStencilDescL, sizeof(depthStencilDescL));
	// Depth test parameters

// Depth test parameters
	depthStencilDescL.DepthEnable = true;
	depthStencilDescL.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;//ALL
	depthStencilDescL.DepthFunc = D3D11_COMPARISON_GREATER;

	// Stencil test parameters
	depthStencilDescL.StencilEnable = true;
	depthStencilDescL.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDescL.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// Stencil operations if pixel is front-facing
	depthStencilDescL.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescL.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//decrement the value in the stencil buffer when the depth test fails
	depthStencilDescL.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;
	depthStencilDescL.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	depthStencilDescL.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescL.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//increment the value in the stencil buffer when the depth test fails
	depthStencilDescL.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;
	depthStencilDescL.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Create depth stencil state
	hr = device->CreateDepthStencilState(&depthStencilDescL, &depthStencilState);
	if (FAILED(hr))
	{
		// deal with error...
		return false;
	}
	//second pass
	D3D11_DEPTH_STENCIL_DESC disDepthStencilDescL;
	ZeroMemory(&disDepthStencilDescL, sizeof(disDepthStencilDescL));
	// Depth test parameters

// Depth test parameters
	disDepthStencilDescL.DepthEnable = true;
	disDepthStencilDescL.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;//ALL
	disDepthStencilDescL.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

	disDepthStencilDescL.StencilEnable = true;
	disDepthStencilDescL.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	disDepthStencilDescL.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// Stencil operations if pixel is front-facing
	disDepthStencilDescL.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	disDepthStencilDescL.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//decrement the value in the stencil buffer when the depth test fails
	disDepthStencilDescL.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	disDepthStencilDescL.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	// Stencil operations if pixel is back-facing
	disDepthStencilDescL.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	disDepthStencilDescL.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//increment the value in the stencil buffer when the depth test fails
	disDepthStencilDescL.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	disDepthStencilDescL.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	hr = device->CreateDepthStencilState(&disDepthStencilDescL, &disDepthStencilState);
	if (FAILED(hr))
	{
		// deal with error...
		return false;
	}
	//First pass
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

	rasterizerDesc.DepthClipEnable = true;
	hr = device->CreateRasterizerState(&rasterizerDesc, &rasState);
	if (FAILED(hr)) //If error occurred
	{
		MessageBox(NULL, "Failed to create rasterizer state.",
			"D3D11 Initialisation Error", MB_OK);
		return false;
	}
	//Second pass
	//D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;

	rasterizerDesc.DepthClipEnable = false;
	hr = device->CreateRasterizerState(&rasterizerDesc, &fRasState);
	if (FAILED(hr)) //If error occurred
	{
		MessageBox(NULL, "Failed to create rasterizer state.",
			"D3D11 Initialisation Error", MB_OK);
		return false;
	}


	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = true;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create the blend state using the description.
	hr = device->CreateBlendState(&blendStateDescription, &blendState);
	if (FAILED(hr))
	{
		return false;
	}


	return result;
}

void LightShader::Shutdown()
{
		// Release the vertex shader.
	if (vertexShader)
	{
		vertexShader->Release();
		vertexShader = 0;
	}
	// Release the pixel shader.
	if (pixelShader)
	{
		pixelShader->Release();
		pixelShader = 0;
	}
	if (this->geometryShader)
	{
		geometryShader->Release();
		geometryShader = 0;
	}
	// Release the layout.
	if (this->vertexLayout)
	{
		vertexLayout->Release();
		vertexLayout = 0;
	}

	if (this->ConstantBuffer)
	{
		this->ConstantBuffer->Release();
	}
	if (this->geometryShader)
	{
		this->geometryShader->Release();
	}
	if (this->LightPerFrameBuffer)
	{
		this->LightPerFrameBuffer->Release();
	}
	if (this->MatrixPerFrameBuffer)
	{
		this->MatrixPerFrameBuffer->Release();
	}
	if (this->pixelShader)
	{
		this->pixelShader->Release();
	}
	if (this->sampler)
	{
		this->sampler->Release();
	}
	if (this->vertexLayout)
	{
		this->vertexLayout->Release();
	}
	if (this->vertexShader)
	{
		this->vertexShader->Release();
	}
	if (this->data)
	{
		_aligned_free(data);
	}
	if (this->dataSpec)
	{
		_aligned_free(dataSpec);
	}
	if (this->gConstantBufferData)
	{
		_aligned_free(gConstantBufferData);
	}

	if (depthStencilState)
	{
		depthStencilState->Release();
		depthStencilState = nullptr;
	}

	if (disDepthStencilState)
	{
		disDepthStencilState->Release();
		disDepthStencilState = nullptr;
	}

	if (this->blendState)
	{
		blendState->Release();
		blendState = nullptr;
	}

	if (rasState)
	{
		rasState->Release();
		rasState = nullptr;
	}

	if (this->fRasState)
	{
		this->fRasState->Release();
		this->fRasState = nullptr;
	}
	
}

void LightShader::render(ID3D11DeviceContext * deviceContext, int count, ID3D11DepthStencilView* view)
{
	float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	
	deviceContext->OMSetDepthStencilState(depthStencilState, 0);
	deviceContext->ClearDepthStencilView(view, D3D11_CLEAR_STENCIL,1.0f, 1);
	deviceContext->RSSetState(rasState);
	this->renderUnmark(deviceContext, count);//unmark all of the pixels that are in front of the front faces of the light�s geometric volume
	deviceContext->OMSetBlendState(blendState, blendFactor, 1);
	deviceContext->OMSetDepthStencilState(disDepthStencilState, 1);
	deviceContext->RSSetState(fRasState);
	this->RenderShader(deviceContext, count);//shade the pixels
}



void LightShader::renderUnmark(ID3D11DeviceContext * deviceContext, int count)
{

	deviceContext->VSSetShader(this->vertexShader, nullptr, 0);
	deviceContext->HSSetShader(nullptr, nullptr, 0);
	deviceContext->DSSetShader(nullptr, nullptr, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	//deviceContext->PSSetShader(this->pixelShader, nullptr, 0);
	deviceContext->PSSetShader(nullptr, nullptr, 0);
	deviceContext->IASetInputLayout(this->vertexLayout);
	deviceContext->PSSetSamplers(0, 1, &sampler);
	//deviceContext->DrawIndexed(count, 0, 0);
	deviceContext->Draw(count, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
}
void LightShader::RenderShader(ID3D11DeviceContext * deviceContext, int count)
{
	

	deviceContext->VSSetShader(this->vertexShader, nullptr, 0);
	deviceContext->HSSetShader(nullptr, nullptr, 0);
	deviceContext->DSSetShader(nullptr, nullptr, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->PSSetShader(this->pixelShader, nullptr, 0);
	//deviceContext->PSSetShader(nullptr, nullptr, 0);
	deviceContext->IASetInputLayout(this->vertexLayout);
	deviceContext->PSSetSamplers(0, 1, &sampler);
	//deviceContext->DrawIndexed(count, 0, 0);
	deviceContext->Draw(count, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
}
bool LightShader::SetShaderParameters(ID3D11DeviceContext *& deviceContext, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix,
		 AnyLight light, DirectX::XMFLOAT3 camPos,int choice)
{
	/*PointLight *temp = new PointLight[4];
	for (int i = 0; i < 4; i++)
	{
		temp[i] = light[i];
	}*/
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedMemory;
	//	D3D11_MAPPED_SUBRESOURCE mappedMemorySpec;
		//	MatrixBufferType* dataPtr;
			//PerFrameMatrices* matricesPerFrame;
			//unsigned int bufferNumber;

			//Make sure to transpose matrices before sending them into the shader, this is a requirement for DirectX 11.

				// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
	DirectX::XMMATRIX WorldView = DirectX::XMMatrixMultiply(viewMatrix, worldMatrix);
	DirectX::XMMATRIX WorldViewProj = DirectX::XMMatrixMultiply(projectionMatrix, WorldView);
	WorldViewProj = DirectX::XMMatrixTranspose(WorldViewProj);
	data->world = worldMatrix;
	data->view = XMMatrixTranspose(WorldView);
	data->projection = WorldViewProj;
	data->choice = float(choice);
	data->camPos = camPos;
	dataSpec->position.x = light.position.x;//lightPos.x;//temp[0].x; 
	dataSpec->position.y = light.position.y;//temp[0].y;
	dataSpec->position.z = light.position.z;//temp[0].z;

	dataSpec->position.w = light.position.w;//temp[0].w;
	dataSpec->color.x = light.color.x;//lightColor.x;//temp[0].r;
	dataSpec->color.y = light.color.y;//temp[0].g;
	dataSpec->color.z = light.color.z;
	dataSpec->color.w= light.color.w;//lightIntensity;
	dataSpec->world = worldMatrix;
	dataSpec->cameraPos.x = camPos.x;//temp[0].x; 
	dataSpec->cameraPos.y = camPos.y;//temp[0].y;
	dataSpec->cameraPos.z = camPos.z;//temp[0].z;
	dataSpec->cameraPos.w = 10.0f;//temp[0].w;
	dataSpec->direction = light.direction;

	// Lock the m_matrixBuffer, set the new matrices inside it, and then unlock it.

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(MatrixPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMemory);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	memcpy(mappedMemory.pData, data, sizeof(MatrixBuffers));

	// Unlock the constant buffer.
	deviceContext->Unmap(MatrixPerFrameBuffer, 0);

	result = deviceContext->Map(LightPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMemory);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	memcpy(mappedMemory.pData, dataSpec, sizeof(AnyLight));

	// Unlock the constant buffer.
	deviceContext->Unmap(LightPerFrameBuffer, 0);

	//Now set the updated matrix buffer in the HLSL vertex shader.

	// Set the position of the constant buffer in the vertex shader.
	//bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	//deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	//deviceContext->PSSetConstantBuffers(0, 1, &ConstantBuffer);
	//Insert posTexture 
	 //Insert normalTexture
	deviceContext->PSSetConstantBuffers(0, 1, &LightPerFrameBuffer);//Insert Light Properties Pointlight
	deviceContext->PSSetConstantBuffers(1, 1, &MatrixPerFrameBuffer);
	deviceContext->VSSetConstantBuffers(0, 1, &MatrixPerFrameBuffer); //Insert world,view,proj to VS

	return true;
}

bool LightShader::InitializeShader(ID3D11Device *& device, HWND hwnd)
{
	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT result = D3DCompileFromFile(
		L"LSVertexShader.hlsl", // filename vsFilename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			//OutputShaderErrorMessage(errorBlob, hwnd, vsFilename); //able when parameter active
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		else
		{
			//MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK); //able when parameter active
		}
		if (pVS)
			pVS->Release();
		return result;
	}

	device->CreateVertexShader(
		pVS->GetBufferPointer(),
		pVS->GetBufferSize(),
		nullptr,
		&vertexShader
	);

	// create input layout (verified using vertex shader)
	// Press F1 in Visual Studio with the cursor over the datatype to jump
	// to the documentation online!
	// please read:
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb205117(v=vs.85).aspx
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{
			"POSITION",		// "semantic" name in shader
			0,				// "semantic" index (not used)
			DXGI_FORMAT_R32G32B32_FLOAT, // size of ONE element (3 floats)
			0,							 // input slot
			0, // offset of first element
			D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
			0							 // used for INSTANCING (ignore)
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT, //2 values
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};
	device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &vertexLayout);

	// we do not need anymore this COM object, so we release it.
	pVS->Release();
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//CLAMP in Rastertek
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;//CLAMP
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;//CLAMP
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &sampler);
	if (FAILED(result))
	{
		return false;
	}


	//create pixel shader
	ID3DBlob* pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"LSFragmentShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);


	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (pPS)
			pPS->Release();
		return result;
	}

	result = device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &pixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();

	ID3DBlob* pGS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;
	result = D3DCompileFromFile(
		L"LSGeometryShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"GS_main",		// entry point
		"gs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&pGS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (pGS)
			pGS->Release();
		return result;
	}

	/*device->CreateGeometryShader(
		pGS->GetBufferPointer(),
		pGS->GetBufferSize(),
		nullptr,
		&geometryShader
	);*/
	pGS->Release();

	return true;
}

void LightShader::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	std::ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", LPCSTR(shaderFilename), MB_OK);

	//return;
}