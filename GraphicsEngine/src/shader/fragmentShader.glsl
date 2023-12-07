// Variables reçues du vertex shader
varying lowp vec4 v_color;    // Couleur du vertex
varying mediump vec3 v_normal; // Normale du vertex
varying mediump vec2 v_texCoord; // Coordonnées de texture du vertex

// Uniforms supplémentaires (si nécessaire)
// uniform sampler2D u_texture;  // Exemple d'uniforme pour une texture

void main() {
    // Éventuellement, effectuer des calculs basés sur la couleur, la normale et les coordonnées de texture
    // Pour l'instant, utilisons simplement la couleur reçue du vertex shader
    gl_FragColor = v_color;

    // Exemple d'utilisation de la texture (décommentez si vous utilisez une texture)
    // gl_FragColor = texture2D(u_texture, v_texCoord);
}