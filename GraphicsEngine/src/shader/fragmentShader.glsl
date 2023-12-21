// Variables re�ues du vertex shader
varying lowp vec4 v_color;    // Couleur du vertex
varying mediump vec3 v_normal; // Normale du vertex
varying mediump vec2 v_texCoord; // Coordonn�es de texture du vertex

// Uniforms suppl�mentaires (si n�cessaire)
// uniform sampler2D u_texture;  // Exemple d'uniforme pour une texture

void main() {
    // �ventuellement, effectuer des calculs bas�s sur la couleur, la normale et les coordonn�es de texture
    // Pour l'instant, utilisons simplement la couleur re�ue du vertex shader
    gl_FragColor = v_color;

    // Exemple d'utilisation de la texture (d�commentez si vous utilisez une texture)
    // gl_FragColor = texture2D(u_texture, v_texCoord);
}