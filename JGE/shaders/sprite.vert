precision mediump float;

attribute vec2 vertex;

uniform vec4 spriteRect;      // [spriteSourceX, spriteSourceY, spriteWidth, spriteHeight]
uniform vec2 textureSize;     // Texture width and height
uniform mat4 model; 
uniform mat4 projection;
uniform vec2 flipped;         // [flipX, flipY], where 0.0 = no flip, 1.0 = flip

varying vec2 TexCoords;

void main()
{
    // Transform the vertex position
    gl_Position = projection * (model * vec4(vertex, 0.0, 1.0)); 

    // Flip the coordinates
    vec2 v = vertex;
    v = v * (1.0 - flipped) + (1.0 - v) * flipped; // Avoid branching

    // Calculate texture coordinates
    //const float epsilon = 0.5; // To fix potential border artifacts
    TexCoords.x = (v.x * (spriteRect.z) + spriteRect.x) / textureSize.x;
    TexCoords.y = (v.y * (spriteRect.w) + spriteRect.y) / textureSize.y;
}
