#pragma once


#include "Math/Vector3D.h"
#include "Math/Matrix4x4.h"

#include "GameObject/IGUID.h"
#include <string>


class Transform
{
	public:
		Vector3D m_translation;
		Vector3D m_scale;
		Vector3D m_rotation;
		std::string component_id;

	private:
		Matrix4x4 m_transformation_matrix;

	private:
		IGUID* m_guid;
		

	public:
		Transform(IGUID* guid): m_translation(Vector3D()), m_scale(Vector3D(1,1,1)), m_rotation(Vector3D()), m_guid(guid) 
		{
			std::stringstream s;
			s << m_guid->GUID;
			s << 0;
			component_id = s.str();
		}

		IGUID* Parent() {return m_guid;}

		void BuildTransform() {

			Matrix4x4 m;
			m.SetIdentity();
	
			Matrix4x4 sm;
			sm.SetScale(m_scale);
			m *= sm;

			Matrix4x4 rm;

			Matrix4x4 temp;
			temp.SetIdentity();

			rm.SetIdentity();
			rm.setRotationX(m_rotation.m_x);
			temp *= rm;


			rm.SetIdentity();
			rm.setRotationY(m_rotation.m_y);
			temp *= rm;


			rm.SetIdentity();
			rm.setRotationZ(m_rotation.m_z);
			temp *= rm;

			m *= temp;
		

			Matrix4x4 tm;
			tm.SetTranslation(m_translation);
			m *= tm;
	
		
			this->m_transformation_matrix = m;

		}

		Matrix4x4 GetTransformationMatrix() {
			BuildTransform();
			return m_transformation_matrix;
		}

		static Matrix4x4 CalculateTransform(Vector3D translate, Vector3D scale, Vector3D rotation) {

			Matrix4x4 m;
			m.SetIdentity();

			Matrix4x4 sm;
			sm.SetScale(scale);
			m *= sm;

			Matrix4x4 rm;

			Matrix4x4 temp;
			temp.SetIdentity();

			rm.SetIdentity();
			rm.setRotationX(rotation.m_x);
			temp *= rm;


			rm.SetIdentity();
			rm.setRotationY(rotation.m_y);
			temp *= rm;


			rm.SetIdentity();
			rm.setRotationZ(rotation.m_z);
			temp *= rm;

			m *= temp;


			Matrix4x4 tm;
			tm.SetTranslation(translate);
			m *= tm;


			return m;
		}

		inline std::string ExportToUnity() {


			float rot_x_deg = m_rotation.m_x * 180.f / 3.14159265358979323846f;
			float rot_y_deg = m_rotation.m_y * 180.f / 3.14159265358979323846f;
			float rot_z_deg = m_rotation.m_z * 180.f / 3.14159265358979323846f;

			float cx = cos(m_rotation.m_x * 0.5f);
			float sx = sin(m_rotation.m_x * 0.5f);
			float cy = cos(m_rotation.m_y * 0.5f);
			float sy = sin(m_rotation.m_y * 0.5f);
			float cz = cos(m_rotation.m_z * 0.5f);
			float sz = sin(m_rotation.m_z * 0.5f);

			float x = sx * cy * cz + cx * sy * sz;
			float y = cx * sy * cz - sx * cy * sz;
			float z = cx * cy * sz + sx * sy * cz;
			float w = cx * cy * cz - sx * sy * sz;

			std::stringstream os;
			std::cout << "Transform ID: " << component_id << std::endl;
			os << "--- !u!4 &" << component_id << "\n";
			os << "Transform:\n";
			os << "  m_ObjectHideFlags: 0\n";
			os << "  m_CorrespondingSourceObject: {fileID: 0}\n";
			os << "  m_PrefabInstance: {fileID: 0}\n";
			os << "  m_PrefabAsset: {fileID: 0}\n";
			os << "  m_GameObject: {fileID: " << m_guid->GUID << "}\n";
			os << "  serializedVersion: 2\n";
			os << "  m_LocalRotation: " << "{x: " <<  x << ", y: " << y <<", z: " << z << ", w: " <<  w <<  "}\n";
			os << "  m_LocalPosition: {x: " << m_translation.m_x  << ", y: " << m_translation.m_y  << ", z: " << m_translation.m_z << "}\n";
			os << "  m_LocalScale: {x: " << m_scale.m_x << ", y: " << m_scale.m_y << ", z: " << m_scale.m_z << "}\n";
			os << "  m_ConstrainProportionsScale: 0\n";
			os << "  m_Children: []\n";
			os << "  m_Father: {fileID: 0}\n";
			os << "  m_LocalEulerAnglesHint: {x: " << rot_x_deg << ", y: " << rot_y_deg << ", z: " << rot_z_deg << "}\n";


			return os.str();
		}
};

