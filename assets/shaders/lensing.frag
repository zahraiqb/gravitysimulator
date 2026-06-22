// Gravitational lensing — bends background UVs toward massive bodies

uniform sampler2D u_texture;
uniform vec2      u_resolution;
uniform vec2      u_bodyPos[8];    // screen-space positions (pixels)
uniform float     u_bodyMass[8];   // normalised mass 0..1
uniform int       u_numBodies;

void main() {
    vec2 uv          = gl_TexCoord[0].xy;
    vec2 fragScreen  = vec2(uv.x, 1.0 - uv.y) * u_resolution; // flip Y: GL vs SFML

    vec2 disp = vec2(0.0);

    for (int i = 0; i < 8; i++) {
        if (i >= u_numBodies) break;
        vec2  dir  = fragScreen - u_bodyPos[i];
        float dist = length(dir);
        if (dist < 2.0) continue;

        // Light bends toward mass, falling off with distance²
        float str = u_bodyMass[i] * 700.0 / (dist * dist * 0.0008 + 1.0);
        disp -= normalize(dir) * str;
    }

    // Convert pixel displacement to UV displacement
    vec2 uvDisp   = clamp(disp / u_resolution, -0.08, 0.08);
    vec2 sampleUV = clamp(uv + vec2(uvDisp.x, -uvDisp.y), 0.001, 0.999);

    gl_FragColor  = texture2D(u_texture, sampleUV) * gl_Color;
}
