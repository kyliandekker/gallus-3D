#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

// external
#include <vector>
#include <cstdint>
#include <memory>

// graphics
#include "graphics/dx12/IndexBuffer.h"
#include "graphics/dx12/VertexBuffer.h"
#include "graphics/dx12/ShaderDefs.h"

namespace gallus::core
{
	class Data;
}
namespace gallus::graphics::dx12
{
	//---------------------------------------------------------------------
	// MeshPartData
	//---------------------------------------------------------------------
	/// <summary>
	/// Represents a part of a mesh, containing vertices, indices, and GPU buffers.
	/// Useful for meshes that are split into sub-meshes or materials.
	/// </summary>
	class MeshPartData
	{
	public:
		MeshPartData() = default;

		MeshPartData(
			std::vector<VSInput> a_aVertices,
			std::vector<uint16_t> a_aIndices) :
			m_aVertices(a_aVertices), m_aIndices(a_aIndices)
		{}

		std::vector<VSInput> m_aVertices;
		std::vector<uint16_t> m_aIndices;

		VertexBuffer m_VertexBuffer;
		IndexBuffer m_IndexBuffer;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_pIntermediateVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pIntermediateIndexBuffer;
	};

	enum class PRIMITIVES
	{
		SQUARE,
		CUBE,
		CYLINDER,
		SPHERE
	};

	extern std::vector<MeshPartData> s_PRIMITIVES;

	class CommandList;
	class CommandQueue;

	//---------------------------------------------------------------------
	// Mesh
	//---------------------------------------------------------------------
	/// <summary>
	/// Represents a renderable mesh resource in the engine.
	/// Contains vertex and index buffers, transformation data, and supports rendering with a command list.
	/// </summary>
	class Mesh : public resources::EngineResource
	{
	public:
		~Mesh();

		/// <summary>
		/// Loads the mesh data.
		/// </summary>
		/// <param name="a_Data">A vector containing data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data)
		{
			return false;
		}
		
		/// <summary>
		/// Loads the mesh data.
		/// </summary>
		/// <param name="a_Data">A vector containing data in bytes.</param>
		/// <param name="a_pCommandList">The command list used for updating resources.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data, std::shared_ptr<CommandQueue> a_pCommandQueue);
		
		/// <summary>
		/// Loads the mesh data.
		/// </summary>
		/// <param name="a_Data">MeshPartData that will be used by this model.</param>
		/// <param name="a_pCommandList">The command list used for updating resources.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const MeshPartData& a_Data, std::shared_ptr<CommandQueue> a_pCommandQueue);

		/// <summary>
		/// Checks whether the mesh contains valid data and is ready for rendering.
		/// </summary>
		/// <returns>True if the mesh is valid, false otherwise.</returns>
		bool IsValid() const override;

		/// <summary>
		/// Uploads the mesh data.
		/// </summary>
		/// <param name="a_pCommandQueue">The command queue used for uploading.</param>
		void UploadMeshData(const std::shared_ptr<CommandQueue> a_pCommandQueue);

		std::vector<MeshPartData>& GetMeshData()
		{
			return m_aMeshData;
		}

		void RenderMeshData(MeshPartData& a_MeshData, std::shared_ptr<CommandList> a_pCommandList);
	private:
		/// <summary>
		/// Loads the mesh data.
		/// </summary>
		/// <param name="a_Data">The file data of the model.</param>
		/// <param name="a_aOutData">A vector containing mesh parts that got extracted.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool GetMeshDataFromModel(const core::Data& a_Data, std::vector<MeshPartData>& a_aOutData);

		/// <summary>
		/// Sets the mesh data.
		/// </summary>
		/// <param name="a_aData">Array of mesh part data.</param>
		/// <param name="a_pCommandQueue">The command queue used for uploading.</param>
		void SetMeshData(const std::vector<MeshPartData>& a_aData, const std::shared_ptr<CommandQueue> a_pCommandQueue);

		std::vector<MeshPartData> m_aMeshData;
		std::vector<std::string> m_aBoneInfo;

		BEGIN_SERIALIZE_PARENT(Mesh, resources::EngineResource)
		END_SERIALIZE(Mesh)
	};
}