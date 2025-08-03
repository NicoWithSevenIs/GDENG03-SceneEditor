#include "ShaderManager.h"




ShaderManager::~ShaderManager()
{
	for (auto i : this->m_map_vs)
		i.second->release();

	for (auto i : this->m_map_ps)
		i.second->release();
}

bool ShaderManager::Request(std::string name, VertexShader** vertex_shader, PixelShader** pixel_shader)
{
	if (!m_map_vs.contains(name) || !m_map_ps.contains(name)) {
		return false;
	}
		




	return true;
}

