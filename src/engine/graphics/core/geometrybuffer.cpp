#include "opengl.hpp"
#include "geometrybuffer.hpp"
#include <glm/glm.hpp>

static void enableAttribute(unsigned _index, const graphics::VertexAttribute& _attribute, unsigned _vertexSize, std::size_t _offset)
{
	graphics::glCall(glEnableVertexAttribArray, _index);
	if( isFloatFormat(_attribute.type) || _attribute.normalize )
		graphics::glCall(glVertexAttribPointer,
			_index,
			_attribute.numComponents,
			GLenum(_attribute.type),
			GLboolean(_attribute.normalize),
			_vertexSize,
			(GLvoid*)(_offset));
	else
		graphics::glCall(glVertexAttribIPointer,
			_index,
			_attribute.numComponents,
			GLenum(_attribute.type),
			_vertexSize,
			(GLvoid*)(_offset));
}

namespace graphics {

	GeometryBuffer::GeometryBuffer(
		GLPrimitiveType _type,
		const VertexAttribute* _attributes,
		int _numAttributes,
		int _indexed,
		unsigned _initialSize,
		GLUsageHint _usageHint
	) :
		m_vboInstances(0),
		m_ibo(0),
		m_glType(_type),
		m_capacity(_initialSize),
		m_indexCapacity(_indexed ? _initialSize : 0),
		m_instanceCapacity(0),
		m_vertexSize(0),
		m_instanceVertexSize(0),
		m_indexSize(_indexed),
		m_indexFormat(PrimitiveFormat::UINT32),
		m_vertexCount(0),
		m_indexCount(0),
		m_instanceCount(0)
	{
		// Analyze vertex data (vertexsize, instancesize)
		for(int i = 0; i < _numAttributes; ++i)
		{
			if(_attributes[i].perInstance)
				m_instanceVertexSize += attributeSize(_attributes[i]);
			else
				m_vertexSize += attributeSize(_attributes[i]);
		}

		// Create OpenGL objects
		glCall(glGenVertexArrays, 1, &m_vao);
		glCall(glBindVertexArray, m_vao);

		glCall(glGenBuffers, 1, &m_vbo);
		glCall(glBindBuffer, GL_ARRAY_BUFFER, m_vbo);
		// Reserve the capacity
		const GLint usageHint = static_cast<GLint>(_usageHint);
		if (m_capacity)
			glCall(glBufferData, GL_ARRAY_BUFFER, m_capacity, nullptr, usageHint);
		
		// Bind Attributes
		unsigned offset = 0;
		unsigned attribIndex = 0;
		for(int i = 0; i < _numAttributes; ++i) if(!_attributes[i].perInstance)
		{
			enableAttribute(attribIndex++, _attributes[i], m_vertexSize, offset);
			offset += attributeSize(_attributes[i]);
		}

		if(m_instanceVertexSize > 0)
		{
			glCall(glGenBuffers, 1, &m_vboInstances);
			glCall(glBindBuffer, GL_ARRAY_BUFFER, m_vboInstances);
			// Reserve a capacity of 32 instances
			m_instanceCapacity = 32;
			glCall(glBufferData, GL_ARRAY_BUFFER, m_instanceCapacity, nullptr, usageHint);

			// Bind Attributes
			unsigned offset = 0;
			for(int i = 0; i < _numAttributes; ++i) if(_attributes[i].perInstance)
			{
				// Also set this attribute to be instanced
				glCall(glVertexAttribDivisor, attribIndex, 1);
				enableAttribute(attribIndex++, _attributes[i], m_instanceVertexSize, offset);
				offset += attributeSize(_attributes[i]);
			}
		}

		if(_indexed) 
		{
			glCall(glGenBuffers, 1, &m_ibo);
			glCall(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			// Reserve the capacity
			if (m_indexCapacity)
				glCall(glBufferData, GL_ELEMENT_ARRAY_BUFFER, m_indexCapacity, nullptr, usageHint);

			switch (m_indexSize)
			{
			case 1: m_indexFormat = PrimitiveFormat::UINT8; break;
			case 2: m_indexFormat = PrimitiveFormat::UINT16; break;
			case 4: m_indexFormat = PrimitiveFormat::UINT32; break;
			default:
				spdlog::error("GeomtryBuffer index size has to be one of 1,2 or 4.");
			}
		}
	}

	GeometryBuffer::GeometryBuffer(GLPrimitiveType _type, std::span<const VertexAttribute> _attributes, int _indexed, unsigned _initialSize, GLUsageHint _usageHint)
		: GeometryBuffer(_type, _attributes.data(), static_cast<int>(_attributes.size()), _indexed, _initialSize, _usageHint)
	{}

	GeometryBuffer::~GeometryBuffer()
	{
		// Delete all buffers in inverse order
		glCall(glDeleteBuffers, 1, &m_ibo);
		if(m_vboInstances)
			glCall(glDeleteBuffers, 1, &m_vboInstances);
		glCall(glDeleteBuffers, 1, &m_vbo);
		glCall(glDeleteVertexArrays, 1, &m_vao);
	}

	void GeometryBuffer::setData(const void* _data, unsigned _size, GLUsageHint _usageHint)
	{
		glCall(glBindBuffer, GL_ARRAY_BUFFER, m_vbo);
		// Use subdata, if capacity is greater or equal the required size.
		if(_size <= m_capacity)
		{
			glCall(glBufferSubData, GL_ARRAY_BUFFER, 0, _size, _data);
		} else {
			m_capacity = glm::max(_size, m_capacity * 3 / 2);
			if (m_capacity == _size)
				glCall(glBufferData, GL_ARRAY_BUFFER, m_capacity, _data, static_cast<GLint>(_usageHint));
			else
			{
				glCall(glBufferData, GL_ARRAY_BUFFER, m_capacity, nullptr, static_cast<GLint>(_usageHint));
				glCall(glBufferSubData, GL_ARRAY_BUFFER, 0, _size, _data);
			}
		}

		m_vertexCount = _size / m_vertexSize;
	}

	void GeometryBuffer::setIndexData(const void* _data, unsigned _size, GLUsageHint _usageHint)
	{
		glCall(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		if(_size <= m_indexCapacity)
		{
			glCall(glBufferSubData, GL_ELEMENT_ARRAY_BUFFER, 0, _size, _data);
		} else {
			m_indexCapacity = glm::max(_size, m_indexCapacity * 3 / 2);
			if (m_indexCapacity == _size)
				glCall(glBufferData, GL_ELEMENT_ARRAY_BUFFER, m_indexCapacity, _data, static_cast<GLint>(_usageHint));
			else 
			{
				glCall(glBufferData, GL_ELEMENT_ARRAY_BUFFER, m_indexCapacity, nullptr, static_cast<GLint>(_usageHint));
				glCall(glBufferSubData, GL_ELEMENT_ARRAY_BUFFER, 0, _size, _data);
			}
		}

		m_indexCount = _size / m_indexSize;
	}

	void GeometryBuffer::setInstanceData(const void* _data, unsigned _size, GLUsageHint _usageHint)
	{
		if(!m_vboInstances)
		{
			spdlog::error("The buffer was initialized without instance vertex attributes!");
			return;
		}

		glCall(glBindBuffer, GL_ARRAY_BUFFER, m_vboInstances);
		if(_size <= m_instanceCapacity)
		{
			glCall(glBufferSubData, GL_ARRAY_BUFFER, 0, _size, _data);
		} else {
			m_instanceCapacity = glm::max(_size, m_indexCapacity * 3 / 2);
			if (m_instanceCapacity == _size)
				glCall(glBufferData, GL_ARRAY_BUFFER, m_instanceCapacity, _data, static_cast<GLint>(_usageHint));
			else
			{
				glCall(glBufferData, GL_ARRAY_BUFFER, m_instanceCapacity, _data, static_cast<GLint>(_usageHint));
				glCall(glBufferSubData, GL_ARRAY_BUFFER, 0, _size, _data);
			}
		}

		m_instanceCount = _size / m_instanceVertexSize;
	}

	void GeometryBuffer::bind() const
	{
		// Vertex buffers do not need to be bound, they are both included through the
		// attribPointer calls of the vertex array.
		glCall(glBindVertexArray, m_vao);
		glCall(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	}

	void GeometryBuffer::draw() const
	{
		bind();

		if (m_ibo)
		{
			glCall(glDrawElementsInstanced, unsigned(m_glType), m_indexCount,
				static_cast<GLenum>(m_indexFormat), nullptr, glm::max(1u, m_instanceCount));
		}
		else
		{
			glCall(glDrawArraysInstanced, unsigned(m_glType), 0, m_vertexCount, 
				glm::max(1u, m_instanceCount));
		}
	}

}