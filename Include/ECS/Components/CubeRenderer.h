#pragma once
#include "ECS/Components/Component.h"

#include "Rendering/GraphicsEngine.h"
#include "Rendering/DeviceContext.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/VertexShader.h"
#include "Rendering/PixelShader.h"
#include "Rendering/ConstantBuffer.h"

#include "Game Engine/EngineTime.h"

#include "filesystem"

class CubeRenderer : public Component {

	private:
		IndexBuffer* m_ib;
		VertexBuffer* m_vb;
		VertexShader* m_vs;
		PixelShader* m_ps;
		ConstantBuffer* m_cb;

		Texture* m_tex;

		std::vector<vertex> cube_list;
		std::vector<unsigned int> index_list;
	public:
		bool isTextured = false;

	public:
		inline CubeRenderer() : Component(ComponentType::RENDERER) {
			Load();
			unit = 2;
		}
		inline CubeRenderer(Entity* newOwner) : Component(ComponentType::RENDERER, newOwner) {
			Load();
			unit = 2;
		}
		inline void Update(constant cc) override {
			m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
			Draw();
		}
		inline Component* CreateSnapshot() override {
			return new CubeRenderer();
		}
		inline void Release() override {
			//this->m_ib->release();
			//this->m_vb->release();
			//this->m_vs->release();
			//this->m_ps->release();
			//this->m_cb->release();
			//delete this;
		}
		inline void ExportToUnity(std::string& output) override {
			std::stringstream s;

			// MeshRenderer block
			s << "--- !u!23 &" << component_id << "\n";
			s << "MeshRenderer:\n";
			s << "  m_ObjectHideFlags: 0\n";
			s << "  m_CorrespondingSourceObject: {fileID: 0}\n";
			s << "  m_PrefabInstance: {fileID: 0}\n";
			s << "  m_PrefabAsset: {fileID: 0}\n";
			s << "  m_GameObject: {fileID: " << owner->GUID << "}\n";
			s << "  m_Enabled: 1\n";
			s << "  m_CastShadows: 1\n";
			s << "  m_ReceiveShadows: 1\n";
			s << "  m_DynamicOccludee: 1\n";
			s << "  m_StaticShadowCaster: 0\n";
			s << "  m_MotionVectors: 1\n";
			s << "  m_LightProbeUsage: 1\n";
			s << "  m_ReflectionProbeUsage: 1\n";
			s << "  m_RayTracingMode: 2\n";
			s << "  m_RayTraceProcedural: 0\n";
			s << "  m_RayTracingAccelStructBuildFlagsOverride: 0\n";
			s << "  m_RayTracingAccelStructBuildFlags: 1\n";
			s << "  m_SmallMeshCulling: 1\n";
			s << "  m_RenderingLayerMask: 1\n";
			s << "  m_RendererPriority: 0\n";
			s << "  m_Materials:\n";
			s << "  - {fileID: 2100000, guid: 31321ba15b8f8eb4c954353edc038b1d, type: 2}\n";
			s << "  m_StaticBatchInfo:\n";
			s << "    firstSubMesh: 0\n";
			s << "    subMeshCount: 0\n";
			s << "  m_StaticBatchRoot: {fileID: 0}\n";
			s << "  m_ProbeAnchor: {fileID: 0}\n";
			s << "  m_LightProbeVolumeOverride: {fileID: 0}\n";
			s << "  m_ScaleInLightmap: 1\n";
			s << "  m_ReceiveGI: 1\n";
			s << "  m_PreserveUVs: 0\n";
			s << "  m_IgnoreNormalsForChartDetection: 0\n";
			s << "  m_ImportantGI: 0\n";
			s << "  m_StitchLightmapSeams: 1\n";
			s << "  m_SelectedEditorRenderState: 3\n";
			s << "  m_MinimumChartSize: 4\n";
			s << "  m_AutoUVMaxDistance: 0.5\n";
			s << "  m_AutoUVMaxAngle: 89\n";
			s << "  m_LightmapParameters: {fileID: 0}\n";
			s << "  m_SortingLayerID: 0\n";
			s << "  m_SortingLayer: 0\n";
			s << "  m_SortingOrder: 0\n";
			s << "  m_AdditionalVertexStreams: {fileID: 0}\n";

			// MeshFilter block

			s << "--- !u!33 &" << std::stoull(component_id) + 1 << "\n";
			s << "MeshFilter:\n";
			s << "  m_ObjectHideFlags: 0\n";
			s << "  m_CorrespondingSourceObject: {fileID: 0}\n";
			s << "  m_PrefabInstance: {fileID: 0}\n";
			s << "  m_PrefabAsset: {fileID: 0}\n";
			s << "  m_GameObject: {fileID: " << owner->GUID << "}\n";
			s << "  m_Mesh: {fileID: 10202, guid: 0000000000000000e000000000000000, type: 0}\n";

			output = s.str();
		}

	private:
		inline void Load() {

			auto  getRandom = [](int cap) {
				Vector3D r;
				r.m_x = rand() % cap;
				r.m_y = rand() % cap;
				r.m_z = rand() % cap;
				return r;
			};

			this->cube_list = {
				// FRONT
				{Vector3D(-0.5f, -0.5f, -0.5f), Vector2D(0.0f, 1.0f),  getRandom(2)},
				{Vector3D(-0.5f,  0.5f, -0.5f), Vector2D(0.0f, 0.0f),  getRandom(2)},
				{Vector3D(0.5f,  0.5f, -0.5f),  Vector2D(1.0f, 0.0f),  getRandom(2)},
				{Vector3D(0.5f, -0.5f, -0.5f),  Vector2D(1.0f, 1.0f),  getRandom(2)},

				// BACK face
				{Vector3D(0.5f, -0.5f, 0.5f),  Vector2D(0.0f, 1.0f),  getRandom(2)},
				{Vector3D(0.5f,  0.5f, 0.5f),  Vector2D(0.0f, 0.0f),  getRandom(2)},
				{Vector3D(-0.5f,  0.5f, 0.5f), Vector2D(1.0f, 0.0f),  getRandom(2)},
				{Vector3D(-0.5f, -0.5f, 0.5f), Vector2D(1.0f, 1.0f),  getRandom(2)},

				// LEFT
				{Vector3D(-0.5f, -0.5f,  0.5f), Vector2D(0.0f, 1.0f),  getRandom(2)},
				{Vector3D(-0.5f,  0.5f,  0.5f), Vector2D(0.0f, 0.0f),  getRandom(2)},
				{Vector3D(-0.5f,  0.5f, -0.5f), Vector2D(1.0f, 0.0f),  getRandom(2)},
				{Vector3D(-0.5f, -0.5f, -0.5f), Vector2D(1.0f, 1.0f),  getRandom(2)},

				// RIGHT
				{Vector3D(0.5f, -0.5f, -0.5f), Vector2D(0.0f, 1.0f),  getRandom(2)},
				{Vector3D(0.5f,  0.5f, -0.5f), Vector2D(0.0f, 0.0f),  getRandom(2)},
				{Vector3D(0.5f,  0.5f,  0.5f), Vector2D(1.0f, 0.0f),  getRandom(2)},
				{Vector3D(0.5f, -0.5f,  0.5f), Vector2D(1.0f, 1.0f),  getRandom(2)},

				// TOP face
				{Vector3D(-0.5f, 0.5f, -0.5f), Vector2D(0.0f, 1.0f),  getRandom(2)},
				{Vector3D(-0.5f, 0.5f,  0.5f), Vector2D(0.0f, 0.0f),  getRandom(2)},
				{Vector3D(0.5f, 0.5f,  0.5f),  Vector2D(1.0f, 0.0f),  getRandom(2)},
				{Vector3D(0.5f, 0.5f, -0.5f),  Vector2D(1.0f, 1.0f),  getRandom(2)},

				// BOTTOM
				{Vector3D(-0.5f, -0.5f,  0.5f), Vector2D(0.0f, 1.0f),  getRandom(2)},
				{Vector3D(-0.5f, -0.5f, -0.5f), Vector2D(0.0f, 0.0f),  getRandom(2)},
				{Vector3D(0.5f, -0.5f, -0.5f),  Vector2D(1.0f, 0.0f),  getRandom(2)},
				{Vector3D(0.5f, -0.5f,  0.5f),  Vector2D(1.0f, 1.0f),  getRandom(2)},
			};

			this->index_list = {
				// FRONT
				0, 1, 2, 0, 2, 3,

				// BACK
				4, 5, 6, 4, 6, 7,

				// LEFT
				8, 9,10, 8,10,11,

				// RIGHT
				12,13,14,12,14,15,

				// TOP
				16,17,18,16,18,19,

				// BOTTOM
				20,21,22,20,22,23
			};


			this->m_vb = GraphicsEngine::get()->createVertexBuffer();
			this->m_ib = GraphicsEngine::get()->createIndexBuffer();

			UINT size_list = cube_list.size();
			UINT size_index_list = index_list.size();

			void* shader_byte_code = nullptr;
			size_t size_shader = 0;

			GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
			this->m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);

			this->m_vb->load(cube_list.data(), sizeof(vertex), size_list, shader_byte_code, size_shader);
			this->m_ib->load(index_list.data(), size_index_list);

			GraphicsEngine::get()->releaseCompiledShader();

			GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
			this->m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);

			GraphicsEngine::get()->releaseCompiledShader();


			constant cc;
			std::string texture_path = "brick.png";
			m_tex = new Texture();
			if (!texture_path.empty()) {
				cc.hasTex = m_tex->load(texture_path);
				this->isTextured = cc.hasTex;
			}
			else {
				cc.hasTex = false;
			}

			m_cb = GraphicsEngine::get()->createConstantBuffer();
			m_cb->load(&cc, sizeof(constant));
		}
		inline void Draw() {
			GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(this->m_vs, m_cb);
			GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(this->m_ps, m_cb);

			GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(this->m_vs);
			GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(this->m_ps);

			if (this->owner->cc.hasTex) {
				GraphicsEngine::get()->getImmediateDeviceContext()->setTexture(m_vs, m_tex);
				GraphicsEngine::get()->getImmediateDeviceContext()->setTexture(m_ps, m_tex);
			}

			GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);
			GraphicsEngine::get()->getImmediateDeviceContext()->setIndexBuffer(m_ib);
			GraphicsEngine::get()->getImmediateDeviceContext()->drawIndexedList(m_ib->getSizeIndexList(), 0, 0);
		}
	public:
		inline void TextureChange(std::string texture_path) {
			if (std::filesystem::exists(texture_path)) {
				Texture* newTexture = new Texture();
				newTexture->load(texture_path);
				m_tex = newTexture;
				this->owner->cc.hasTex = true;
			}
		}

};