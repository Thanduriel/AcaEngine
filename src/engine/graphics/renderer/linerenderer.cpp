#include "linerenderer.hpp"
#include <engine/graphics/resources.hpp>
#include <engine/graphics/core/device.hpp>
#include <array>

namespace graphics {

	static constexpr std::array<VertexAttribute, 4> vertexAttributes = { {
		{PrimitiveFormat::FLOAT, 3, false, false},
		{PrimitiveFormat::FLOAT, 3, false, false},
		{PrimitiveFormat::FLOAT, 4, false, false},
		{PrimitiveFormat::FLOAT, 1, false, false},
	} };

	LineRenderer::LineRenderer()
		: m_buffer(GLPrimitiveType::POINTS, vertexAttributes.data(), static_cast<int>(vertexAttributes.size()), 0)
	{
		m_program.attach(ShaderManager::get("shader/ray.vert", ShaderType::VERTEX));
		m_program.attach(ShaderManager::get("shader/ray.geom", ShaderType::GEOMETRY));
		m_program.attach(ShaderManager::get("shader/ray.frag", ShaderType::FRAGMENT));
		m_program.link();
	}

	void LineRenderer::draw(const glm::vec3& _start
		, const glm::vec3& _end
		,const glm::vec4& _color, 
		float _thickness)
	{
		m_vertices.emplace_back(_start, _end, _color, _thickness);
	}

	void LineRenderer::clear()
	{
		m_vertices.clear();
	}

	void LineRenderer::present(const Camera& _camera)
	{
		Device::setZFunc(ComparisonFunc::LESS);
		Device::setZWrite(false);
		Device::enableBlending(true);
		Device::setBlendOp(BlendOp::ADD);
		Device::setBlendFactor(BlendFactor::SRC_ALPHA, BlendFactor::ONE);

		m_program.use();
		m_program.setUniform(0, _camera.getViewProjection());

		m_buffer.setData(m_vertices.data(), static_cast<unsigned>(m_vertices.size() * sizeof(Vertex)));
		m_buffer.draw();
	}
}