#include "Mesh.hpp"

namespace cbn
{
    //-------------------------------------------------------------------------------------

    void DynamicMesh::generate_mesh(const std::vector<glm::vec2>& vertices)
    {
        CBN_Assert(vertices.size() >= 3, "A mesh cannot have less than three vertices");

        m_Vertices.reserve(vertices.size());
        m_Normals.reserve(vertices.size());
        m_Edges.reserve(vertices.size());

        // Use the vertices to find the normals and edges
        glm::vec2 last_vertex = vertices.back();
        for(const auto& vertex : vertices)
        {
            m_Vertices.emplace_back(vertex);
            m_Edges.emplace_back(last_vertex, vertex);
            m_Normals.emplace_back(m_Edges.back().normal());

            last_vertex = vertex;
        }
    }
    
    //-------------------------------------------------------------------------------------

    DynamicMesh::DynamicMesh()
    {}

    //-------------------------------------------------------------------------------------

    DynamicMesh::DynamicMesh(const std::vector<glm::vec2>& vertices)
    {
        generate_mesh(vertices);
    }

    //-------------------------------------------------------------------------------------

    DynamicMesh::DynamicMesh(const Transform& transform, const std::vector<glm::vec2>& vertices)
    {
        std::vector<glm::vec2> transformed_vertices(vertices.size());
        std::transform(vertices.begin(), vertices.end(), transformed_vertices.begin(), [&](const auto& v)
        {
            return transform.apply(v);
        });

        generate_mesh(transformed_vertices);
    }

    //-------------------------------------------------------------------------------------

    const uint64_t DynamicMesh::sides() const
    {
        return m_Vertices.size();
    }

    //-------------------------------------------------------------------------------------

    const std::vector<glm::vec2>& DynamicMesh::vertices() const
    {
        return m_Vertices;
    }

    //-------------------------------------------------------------------------------------

    const std::vector<glm::vec2>& DynamicMesh::normals() const
    {
        return m_Normals;
    }

    //-------------------------------------------------------------------------------------

    const std::vector<Segment>& DynamicMesh::edges() const
    {
        return m_Edges;
    }

    //-------------------------------------------------------------------------------------

}