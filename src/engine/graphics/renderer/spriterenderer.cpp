#include "spriterenderer.hpp"
#include "../core/opengl.hpp"
#include "../core/vertexformat.hpp"
#include <spdlog/spdlog.h>

using namespace glm;

namespace graphics {

SpriteRenderer::SpriteRenderer()
{
	m_program.attach(ShaderManager::get("../resources/shader/sprite.vert", ShaderType::VERTEX));
	m_program.attach(ShaderManager::get("../resources/shader/sprite.geom", ShaderType::GEOMETRY));
	m_program.attach(ShaderManager::get("../resources/shader/sprite.frag", ShaderType::FRAGMENT));
	m_program.link();

	glCall(glGenVertexArrays, 1, &m_vao);
	glCall(glBindVertexArray, m_vao);
	// Create buffer without data for now
	glCall(glGenBuffers, 1, &m_vbo);
	glCall(glBindBuffer, GL_ARRAY_BUFFER, m_vbo);

	// 4 x uint16 for texture l, u, r and b
	glCall(glEnableVertexAttribArray, 0);
	glCall(glVertexAttribPointer, 0, 4, GLenum(PrimitiveFormat::UINT16), GL_TRUE, 52, (GLvoid*)(0));
	// 1 x uint64 bindless handle
	glCall(glEnableVertexAttribArray, 1);
	glCall(glVertexAttribIPointer, 1, 2, GLenum(PrimitiveFormat::UINT32), 52, (GLvoid*)(8));
	// 2 x int16 for num-tiles
	glCall(glEnableVertexAttribArray, 2);
	glCall(glVertexAttribIPointer, 2, 2, GLenum(PrimitiveFormat::UINT16), 52, (GLvoid*)(16));
	// 3 x float position in world space
	glCall(glEnableVertexAttribArray, 3);
	glCall(glVertexAttribPointer, 3, 3, GLenum(PrimitiveFormat::FLOAT), GL_FALSE, 52, (GLvoid*)(20));
	// 1 x float rotation
	glCall(glEnableVertexAttribArray, 4);
	glCall(glVertexAttribPointer, 4, 1, GLenum(PrimitiveFormat::FLOAT), GL_FALSE, 52, (GLvoid*)(32));
	// 4 x half float scale
	glCall(glEnableVertexAttribArray, 5);
	glCall(glVertexAttribPointer, 5, 4, GLenum(PrimitiveFormat::HALF), GL_FALSE, 52, (GLvoid*)(36));
	// 2 x float animation
	glCall(glEnableVertexAttribArray, 6);
	glCall(glVertexAttribPointer, 6, 2, GLenum(PrimitiveFormat::FLOAT), GL_FALSE, 52, (GLvoid*)(44));
}

Sprite::Sprite(float _alignX, float _alignY,
			  Texture2D::Handle _textureHandle, int _texX, int _texY, int _texWidth, int _texHeight,
			  int _numX, int _numY)
{
	// TODO: error handling for parameters
	const float w = _textureHandle ? (float)_textureHandle->getWidth() : 1.0f;
	const float h = _textureHandle ? (float)_textureHandle->getHeight() : 1.0f;

	if(_texWidth == -1) _texWidth = _textureHandle ? _textureHandle->getWidth() : 1;
	if(_texHeight == -1) _texHeight = _textureHandle ? _textureHandle->getHeight() : 1;

	data.texCoords.x = uint16((_texX + 0.0f) / w * 65535);
	data.texCoords.w = uint16((_texY + 0.0f) / h * 65535);
	data.texCoords.z = uint16((_texX - 0.0f + _texWidth) / w * 65535);
	data.texCoords.y = uint16((_texY - 0.0f + _texHeight) / h * 65535);
	data.texture = _textureHandle ? _textureHandle->getGPUHandle() : 0;
	data.numTiles.x = _numX;
	data.numTiles.y = _numY;
	offset.x = -_texWidth * _alignX;
	offset.y = -_texHeight * _alignY;
	size.x = _texWidth;
	size.y = _texHeight;
}

void SpriteRenderer::draw(const Sprite& _sprite, const vec3& _position, float _rotation, const vec2& _scale, float _animX, float _animY)
{
	SpriteInstance instance;
	const Sprite& sp = _sprite;
	instance.sprite = sp.data;
	instance.position = _position;// + Vec3(sp.offset * _scale, 0.0f);
	//instance.position.z = -1.0f + instance.position.z;
	instance.rotation = _rotation;
	vec2 minPos = _scale * (sp.offset);
	vec2 maxPos = _scale * (vec2(sp.size) + sp.offset);
	instance.scale = glm::uvec2(packHalf2x16(minPos), packHalf2x16(maxPos));
	if(sp.data.numTiles.x > 1) instance.animation.x = fmod(_animX, (float)sp.data.numTiles.x);
	else instance.animation.x = 0.0f;
	if(sp.data.numTiles.y > 1) instance.animation.y = fmod(_animY, (float)sp.data.numTiles.y);
	else instance.animation.y = 0.0f;

	m_instances.push_back(instance);
	m_dirty = true;
}

void SpriteRenderer::clear()
{
	m_instances.clear();
	m_dirty = true;
}

void SpriteRenderer::present(const Camera& _camera)
{
	m_program.use();
	m_program.setUniform(0, _camera.getViewProjection()); //todo: move this into an uniform buffer object?

	// Update instance data each frame - it could be dynamic
	if(m_dirty)
	{
		glCall(glBindBuffer, GL_ARRAY_BUFFER, m_vbo);
		glCall(glBufferData, GL_ARRAY_BUFFER, m_instances.size() * sizeof(SpriteInstance), m_instances.data(), GL_DYNAMIC_DRAW);
		m_dirty = false;
	}

	glCall(glBindVertexArray, m_vao);
	glCall(glDrawArrays, GL_POINTS, 0, m_instances.size());
}

}
