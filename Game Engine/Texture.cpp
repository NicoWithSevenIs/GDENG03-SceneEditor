#include "Rendering/Texture.h"
#include "Rendering/GraphicsEngine.h"


#include <iostream>
#include <comdef.h>
#include <filesystem>

Texture::Texture()
{	
}

Texture::~Texture()
{
	m_texture->Release();
}

bool Texture::load(std::string path)
{
	std::wstring wide(path.begin(), path.end());

	DirectX::ScratchImage image_data;

	std::cout << std::filesystem::exists(path)<<std::endl;


	HRESULT res = DirectX::LoadFromWICFile(wide.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image_data);

	
	if (SUCCEEDED(res))
	{
		res = DirectX::CreateTexture(GraphicsEngine::get()->getDevice(), image_data.GetImages(),
			image_data.GetImageCount(), image_data.GetMetadata(), &m_texture);


		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = image_data.GetMetadata().format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = image_data.GetMetadata().mipLevels;
		desc.Texture2D.MostDetailedMip = 0;


		GraphicsEngine::get()->getDevice()->CreateShaderResourceView(m_texture, &desc, &m_shader_res_view);
	}
	else {
		std::cout << "Texture error" << std::endl;
	}

	if (FAILED(res)) {
		_com_error err(res);
		std::wcout << L"[WIC] Failed to load: " << wide << L"\nReason: " << err.ErrorMessage() << std::endl;
	}

	return SUCCEEDED(res);
}
