#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include <iostream>

#include <ScreenGrab.h>
#include <wincodec.h>
#include <filesystem>

//#include <mutex>
#include <thread>

/*
extern "C" {
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/opt.h>
	#include <libswscale/swscale.h>

}
*/
//extern std::mutex system_call_mutex;

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_win32.h"
#include "IMGUI/imgui_impl_dx11.h"

class ScreenCapture
{	

	public:
		static const std::filesystem::path path;
		static const std::filesystem::path output;

	private:
		IDXGISwapChain* m_swap_chain;
		ID3D11Device* m_d3d_device;
		ID3D11DeviceContext* m_device_context;
		std::vector<ID3D11Texture2D*> tex;

	private:
		bool isRecording;
		bool isEncoding;
		bool recentlyEncoded;

	public:
		bool init(IDXGISwapChain* swap_chain, ID3D11Device* d3d_device, ID3D11DeviceContext* context);
		void Update();
		bool release();
		void CaptureScreen();

		bool CanPrompt(){return !isRecording && !recentlyEncoded && !isEncoding;}

	private:
		void EncodeVideo();
		void OnRecentlyEncoded();
};

