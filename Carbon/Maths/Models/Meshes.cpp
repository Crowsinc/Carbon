#include "Meshes.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	TriangleMesh::TriangleMesh() {}
	
	//-------------------------------------------------------------------------------------

	TriangleMesh::TriangleMesh(const TriangleMesh& mesh)
	{
		vertices = mesh.vertices;
		normals = mesh.normals;
		edges = mesh.edges;
	}
	
	//-------------------------------------------------------------------------------------

	TriangleMesh::TriangleMesh(const PolygonMesh<3>& polygon)
		: PolygonMesh(polygon) {}
	
	//-------------------------------------------------------------------------------------

	void TriangleMesh::operator=(const TriangleMesh& mesh)
	{
		vertices = mesh.vertices;
		normals = mesh.normals;
		edges = mesh.edges;
	}
	
	//-------------------------------------------------------------------------------------

	void TriangleMesh::operator=(const PolygonMesh<3>&polygon)
	{
		vertices = polygon.vertices;
		normals = polygon.normals;
		edges = polygon.edges;
	}

	//-------------------------------------------------------------------------------------

	QuadMesh QuadMesh::Create(const Transform& transform, const glm::vec2& size, const glm::vec2& local_origin)
	{
		const auto half_size = size * 0.5f;

		// Use the size and origin to define the local vertices and then use the normal polygon functions

		// The local origin is an offset from the centre of the local box which specifies the box's origin.
		// The coordinate (0,0) of the local mesh is the origin of the box, so we need to offset all the vertices 
		// so the origin of the box has the given offset from the centre while still being at (0,0).

		std::array<glm::vec2, QuadMesh::Sides> local_vertices{};
	
		// Vertex 1
		local_vertices[0].x = -half_size.x - local_origin.x;
		local_vertices[0].y =  half_size.y - local_origin.y;

		// Vertex 2
		local_vertices[1].x = -half_size.x - local_origin.x;
		local_vertices[1].y = -half_size.y - local_origin.y;
		
		// Vertex 3
		local_vertices[2].x =  half_size.x - local_origin.x;
		local_vertices[2].y = -half_size.y - local_origin.y;
		
		// Vertex 4
		local_vertices[3].x =  half_size.x - local_origin.x;
		local_vertices[3].y =  half_size.y - local_origin.y;
		
		return PolygonMesh<4>::Create(transform, local_vertices);
	}
	
	//-------------------------------------------------------------------------------------

	QuadMesh QuadMesh::Create(const Extent& extent)
	{
		std::array<glm::vec2, QuadMesh::Sides> vertices{};

		// Vertex 1
		vertices[0].x = extent.min.x;
		vertices[0].y = extent.max.y;

		// Vertex 2
		vertices[1].x = extent.min.x;
		vertices[1].y = extent.min.y;

		// Vertex 3
		vertices[2].x = extent.max.x;
		vertices[2].y = extent.min.y;

		// Vertex 4
		vertices[3].x = extent.max.x;
		vertices[3].y = extent.max.y;

		return PolygonMesh<4>::Create(vertices);
	}

	//-------------------------------------------------------------------------------------

	QuadMesh::QuadMesh() {}
	
	//-------------------------------------------------------------------------------------

	QuadMesh::QuadMesh(const QuadMesh& mesh)
	{
		vertices = mesh.vertices;
		normals = mesh.normals;
		edges = mesh.edges;
	}
	
	//-------------------------------------------------------------------------------------

	QuadMesh::QuadMesh(const PolygonMesh<4>& polygon)
		: PolygonMesh(polygon) {}
	
	//-------------------------------------------------------------------------------------

	void QuadMesh::operator=(const QuadMesh& mesh)
	{
		vertices = mesh.vertices;
		normals = mesh.normals;
		edges = mesh.edges;
	}
	
	//-------------------------------------------------------------------------------------

	void QuadMesh::operator=(const PolygonMesh<4>& polygon)
	{
		vertices = polygon.vertices;
		normals = polygon.normals;
		edges = polygon.edges;
	}

	//-------------------------------------------------------------------------------------

	
}