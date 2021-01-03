#version 450
#extension GL_ARB_bindless_texture : enable

layout(location = 0) in vec2 in_texCoord;
layout(location = 1) in flat uvec2 in_textureHandle;
layout(location = 2) in flat vec4 in_anim;
layout(location = 3) in flat vec2 in_numTiles;
layout(location = 4) in flat vec4 in_color;

layout(location = 0, index = 0) out vec4 out_color;

vec2 tileOffset(vec2 _first, float _x, float _y)
{
	return mod(_first + vec2(_x, _y), in_numTiles) * in_anim.zw;
}

void main()
{
	sampler2D tex = sampler2D(in_textureHandle);
	vec4 color;
	
	// animation
	if(in_anim.x == 0.0 && in_anim.y == 0.0)
	{
		color = texture(tex, in_texCoord);
	} else {
		vec2 firstTile = floor(in_anim.xy);
		vec2 tileInterpolation = in_anim.xy - firstTile;
		color  = texture(tex, in_texCoord + firstTile * in_anim.zw) * (1 - tileInterpolation.x) * (1 - tileInterpolation.y);
		color += texture(tex, in_texCoord + tileOffset(firstTile, 1.0, 0.0)) * (    tileInterpolation.x) * (1 - tileInterpolation.y);
		color += texture(tex, in_texCoord + tileOffset(firstTile, 0.0, 1.0)) * (1 - tileInterpolation.x) * (    tileInterpolation.y);
		color += texture(tex, in_texCoord + tileOffset(firstTile, 1.0, 1.0)) * (    tileInterpolation.x) * (    tileInterpolation.y);
	}
	
	if(color.a < 0.05) discard;

	// color replacement
	if(in_color.a == 0.0)
	{
		out_color = color;
	} else {
		float replaceBrightness = color.z * 1.35;
		vec3 replaceColor = replaceBrightness * in_color.xyz;
		
		float blueish = clamp(2.0*color.z - color.x - color.y, 0.0, 1.0);
		float mask = clamp(blueish*2.0,0.0,1.0);

		// Final mix.
		color.z = max(0.0,color.z - mask);    // remove the blue from the original color
		out_color = vec4(mix(color.xyz, replaceColor, mask), color.a);
	}
}