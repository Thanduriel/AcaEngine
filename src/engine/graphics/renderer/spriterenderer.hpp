#pragma once

#include "../core/texture.hpp"
#include "../core/shader.hpp"
#include "../camera.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace graphics {

	class Sprite
	{
	public:
		/// Create a new sprite definition.
		/// \details Creates a sprite of a certain size and positioning which covers a (region) of a
		///		texture.
		/// \param [in] _alignX Defines which point of the sprite is set to the position when
		///		instances are added.
		///		0: the sprite coordinate is on the left, 0.5: centered in X, 1: on the right side.
		/// \param [in] _alignY Defines which point of the sprite is set to the position when
		///		instances are added.
		///		0: the sprite coordinate is on the bottom, 0.5: centered in Y, 1: on the top side.
		/// \param [in] _textureHandle Bindless handle of a texture.
		/// \param [in] _texX Left pixel coordinate in the texture. The default is 0.
		/// \param [in] _texY Bottom pixel coordinate in the texture. The default is 0.
		/// \param [in] _texWidth The width of the texture region in pixels. The default -1 uses the
		///		full texture.
		/// \param [in] _texWidth The height of the texture region in pixels. The default -1 uses the 
		///		full texture.
		/// \param [in] _numX Number of tiles in X-direction. This can be used for animated sprites.
		/// \param [in] _numX Number of tiles in Y-direction. This can be used for animated sprites.
		Sprite(float _alignX, float _alignY,
			Texture2D::Handle _textureHandle, int _texX = 0, int _texY = 0, int _texWidth = -1, int _texHeight = -1,
			int _numX = 1, int _numY = 1);

#pragma pack(push, 4)
		struct SpriteData
		{
			glm::u16vec4 texCoords;
			uint64_t texture;
			glm::u16vec2 numTiles;
		};
#pragma pack(pop)
		SpriteData data;
		glm::vec2 offset;
		glm::ivec2 size;
	};

	/// Batch renderer for sprites.
	/// \details This renderer represents exactly one pipeline state and contains one vertex and index
	///		buffer, as well as a texture-atlas.
	class SpriteRenderer
	{
	public:
		/// Initialize the renderer once.
		SpriteRenderer();

		/// \param [in] _position Position in world (x,y) and z for the "layer". You may also use
		///		the sprites in a 3D environment as billboards. Dependent on the camera z is also
		///		used for perspective division (not if orthographic).
		/// \param [in] _rotation Angle (radiants) of a rotation around the z-axis).
		/// \param [in] _scale A relative scale where 1.0 renders the sprite pixel perfect
		///		(1 screen-pixel = 1 texture-pixel) at a distance of 1.0** or with orthographic projection.
		/// \param [in] _animX Choose a tile in X direction when the sprite was created with more than one.
		///		E.g. a 1.4f means, that the second and third tile are interpolated with factor 0.4.
		///		If necessary a modulo operation is applied automatically.
		/// \param [in] _animY Choose a tile in Y direction when the sprite was created with more than one.
		///		E.g. a 1.4f means, that the second and third tile are interpolated with factor 0.4.
		///		If necessary a modulo operation is applied automatically.
		void draw(const Sprite& _sprite, const glm::vec3& _position, float _rotation, const glm::vec2& _scale, float _animX = 0.0f, float _animY = 0.0f);

		/// Clear all existing instances (recommended for fully dynamic buffers)
		void clear();
		
		/// Single draw call for all instances.
		void present(const Camera& _camera);

		/// Check if there are any instances to draw
		bool isEmpty() const { return m_instances.empty(); }

	private:
#pragma pack(push, 4)

		struct SpriteInstance
		{
			Sprite::SpriteData sprite;
			glm::vec3 position;
			float rotation;
			glm::uvec2 scale; // 4 packed halfs
		//	glm::vec<half, 4> scale;
			glm::vec2 animation;
		};
#pragma pack(pop)

		unsigned m_vao;		///< OpenGL vertex array object
		unsigned m_vbo;		///< OpenGL vertex buffer for sprites

		std::vector<SpriteInstance> m_instances;
		mutable bool m_dirty;

		Program m_program; // todo: move outside to reuse
	};

}
