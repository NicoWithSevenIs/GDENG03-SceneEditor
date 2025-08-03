#pragma once


#include <unordered_map>
#include <string>

#include "../VertexShader/VertexShader.h"
#include "../PixelShader/PixelShader.h"

#include "../../../Library/Interfaces/Singleton.h"

class ShaderManager : public Singleton<ShaderManager> {

private:
	std::unordered_map<std::string, VertexShader*> m_map_vs;
	std::unordered_map<std::string, PixelShader*> m_map_ps;


public:
	~ShaderManager();
	bool Request(std::string name, VertexShader** vertex_shader, PixelShader** pixel_shader);

private:
	//bool compileShader();
};