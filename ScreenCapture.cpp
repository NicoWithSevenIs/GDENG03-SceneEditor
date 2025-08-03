#include "ScreenCapture.h"

const std::filesystem::path ScreenCapture::path = "PATH";
const std::filesystem::path ScreenCapture::output = "OUTPUT";

bool ScreenCapture::init(IDXGISwapChain* swap_chain, ID3D11Device* d3d_device, ID3D11DeviceContext* context)
{
	isRecording = false;
	isEncoding = false;
	recentlyEncoded = false;

	m_swap_chain = swap_chain;
	m_d3d_device = d3d_device;
	m_device_context = context;

	
	std::filesystem::create_directory(ScreenCapture::output);

	return true;
}

void ScreenCapture::Update()
{

	OnRecentlyEncoded();

	if (isEncoding) {
		ImGui::SetNextWindowSize(ImVec2(200, 50));
		ImGui::Begin("Screen Capture");
		ImGui::Text("Enconding. Please Hold...");
		ImGui::End();
		return;
	}

	if(!isRecording)
		return;

	ImGui::SetNextWindowSize(ImVec2(200, 80));
	ImGui::Begin("Screen Capture");
	ImGui::Text("Recording...");
	if (ImGui::Button("Stop Recording")) {
		CaptureScreen();
	}
	ImGui::End();

	ID3D11Texture2D* buffer = NULL;
	HRESULT hr = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

	if (FAILED(hr)) {
		std::cout << "Failed to retrieve backbuffer" << std::endl;
		return;
	}

	D3D11_TEXTURE2D_DESC desc = {};
	buffer->GetDesc(&desc);

	ID3D11Texture2D* pStaging;

	desc.BindFlags = 0;
	desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;

	hr = m_d3d_device->CreateTexture2D(&desc, nullptr, &pStaging);

	if (FAILED(hr)) {
		std::cout << "Failed to create Staging Texture" << std::endl;
		return;
	}

	m_device_context->CopyResource(pStaging, buffer);
	tex.push_back(pStaging);

	buffer->Release();
}

bool ScreenCapture::release()
{
	for(auto t: tex)
		t->Release();
	delete this;
	return true;
}

void ScreenCapture::CaptureScreen()
{
	if (isEncoding) {
		std::cout << "Can't capture screen, currently Encoding" << std::endl;
		return;
	}

	if (!isRecording) {
		std::cout << "Recording Started" << std::endl;
		isRecording = true;
		recentlyEncoded = false;
	}
	else {
		std::cout << "Recording Ended. Encoding Video" << std::endl;
		auto wrapper = [this]() { EncodeVideo(); };
		std::thread(wrapper).detach();
		isRecording = false;
	}
}



void ScreenCapture::EncodeVideo()
{

	isEncoding = true;

	std::cout << "Frame Count: " << tex.size() << std::endl;

	std::filesystem::remove_all(ScreenCapture::path);
	std::filesystem::create_directory(ScreenCapture::path);

	for (int i =0; i < tex.size(); i++) {

		std::string file_name = ScreenCapture::path.string() + "\\" + std::to_string(i) + ".png";
		std::wstring p(file_name.begin(), file_name.end());

		HRESULT hr = DirectX::SaveWICTextureToFile(m_device_context, tex[i], GUID_ContainerFormatPng, p.c_str(), nullptr, nullptr, true);

		if (FAILED(hr)) {
			std::cout << "Failed to Create" << std::endl;
		}

	}


	/*
		-y overwrite
		-i batch filename conversion
		-c codex
		-pix_fmt picture format
	*/

	system("FFMPEG\\bin\\ffmpeg.exe -framerate 60 -y -i PATH\\%d.png -c:v libx264 -pix_fmt yuv420p OUTPUT\\out.mp4");

	std::filesystem::remove_all(ScreenCapture::path);

	isEncoding = false;
	recentlyEncoded = true;

}

void ScreenCapture::OnRecentlyEncoded()
{

	if(!recentlyEncoded)
		return;
	
	ImGui::SetNextWindowSize(ImVec2(200, 60));
	ImGui::Begin("Encoding Successful");
	
	if (ImGui::Button("Open")) {
		system("start OUTPUT\\out.mp4");
		recentlyEncoded = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Dismiss")) {
		recentlyEncoded = false;
	}
	
	ImGui::End();

}


/*
isEncoding = true;
	//Setup Codec
	const AVCodec* codec = avcodec_find_encoder_by_name("libx264");
	AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);

	DXGI_SWAP_CHAIN_DESC sc_desc = {};
	m_swap_chain->GetDesc(&sc_desc);

	UINT width = sc_desc.BufferDesc.Width;
	UINT height = sc_desc.BufferDesc.Height;

	codec_ctx->width = width;
	codec_ctx->height = height;
	codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	codec_ctx->time_base = { 1, 60 };
	codec_ctx->framerate = { 60, 1 };

	avcodec_open2(codec_ctx, codec, nullptr);


	//Configure Output
	AVFormatContext* fmt_ctx;
	avformat_alloc_output_context2(&fmt_ctx, nullptr, nullptr, "output.mp4");
	AVStream* stream = avformat_new_stream(fmt_ctx, nullptr);
	avcodec_parameters_from_context(stream->codecpar, codec_ctx);
	avio_open(&fmt_ctx->pb, "output.mp4", AVIO_FLAG_WRITE);
	avformat_write_header(fmt_ctx, nullptr);


	//Format Conversion Context

	SwsContext* sws_ctx = sws_getContext(
		sc_desc.BufferDesc.Width, sc_desc.BufferDesc.Height, AV_PIX_FMT_BGRA,
		sc_desc.BufferDesc.Width, sc_desc.BufferDesc.Height, AV_PIX_FMT_YUV420P,
		SWS_BILINEAR, nullptr, nullptr, nullptr
	);

	//Stage Textures
	std::vector<ID3D11Texture2D*> staging_tex;

	for (auto i : tex) {

		D3D11_TEXTURE2D_DESC desc = {};
		i->GetDesc(&desc);

		ID3D11Texture2D* pStaging;

		desc.BindFlags = 0;
		desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;

		HRESULT hr = m_d3d_device->CreateTexture2D(&desc, nullptr, &pStaging);

		if (FAILED(hr)) {
			std::cout << "Failed to create Staging Texture" << std::endl;
			continue;
		}

		staging_tex.push_back(pStaging);
		m_device_context->CopyResource(pStaging, i);

	}
	std::cout << "Staging Frames: " << staging_tex.size() << std::endl;

	int64_t frame_index = 0;

	//Encode Staged Textures

	for (auto i : staging_tex) {

		D3D11_MAPPED_SUBRESOURCE mapped;

		D3D11_TEXTURE2D_DESC desc = {};
		i->GetDesc(&desc);
		HRESULT hr = m_device_context->Map(i, 0, D3D11_MAP_READ, 0, &mapped);

		BYTE* b = new BYTE[desc.Width * desc.Height * 4];
		memcpy(b, mapped.pData, desc.Width * desc.Height * 4);
		UINT row_pitch = mapped.RowPitch;

		m_device_context->Unmap(i, 0);

		//figure out how to convert from 48b8g8a8 to AV_PIX_FMT_YUV420P

		uint8_t* src_data[4] = { b, nullptr, nullptr, nullptr };
		int src_linesize[4] = { int(desc.Width * 4), 0, 0, 0 };

		AVFrame* frame = av_frame_alloc();
		frame->width = codec_ctx->width;
		frame->height = codec_ctx->height;
		frame->format = codec_ctx->pix_fmt;
		av_frame_get_buffer(frame, 32);
		av_frame_make_writable(frame);

		sws_scale(sws_ctx, src_data, src_linesize, 0, codec_ctx->height, frame->data, frame->linesize);

		frame->pts = frame_index++;
		avcodec_send_frame(codec_ctx, frame);

		AVPacket* pkt = av_packet_alloc();
		while (avcodec_receive_packet(codec_ctx, pkt) == 0) {
			av_interleaved_write_frame(fmt_ctx, pkt);
			av_packet_unref(pkt);
		}
		av_packet_free(&pkt);
		av_frame_free(&frame);

		delete[] b;

	}

	avcodec_send_frame(codec_ctx, nullptr);
	AVPacket* pkt = av_packet_alloc();
	while (avcodec_receive_packet(codec_ctx, pkt) == 0) {
		av_interleaved_write_frame(fmt_ctx, pkt);
		av_packet_unref(pkt);
	}
	av_packet_free(&pkt);

	// Finalize file
	av_write_trailer(fmt_ctx);

	for(auto i: tex)
		i->Release();

	for(auto i: staging_tex)
		i->Release();


	// Free resources
	avcodec_free_context(&codec_ctx);
	avio_closep(&fmt_ctx->pb);
	avformat_free_context(fmt_ctx);
	sws_freeContext(sws_ctx);
	isEncoding = false;
	return true;

*/