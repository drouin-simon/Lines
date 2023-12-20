// Attributs
attribute vec4 a_position;   // Position du vertex
attribute vec4 a_color;      // Couleur du vertex
attribute vec3 a_normal;     // Normale du vertex
attribute vec2 a_texCoord;   // Coordonnées de texture du vertex

// Matrices de transformation
uniform mat4 u_modelViewProjection; // Matrice de modèle-vue-projection

// Variables à transmettre au fragment shader
varying vec4 v_color;      // Couleur à transmettre au fragment shader
varying vec3 v_normal;     // Normale à transmettre au fragment shader
varying vec2 v_texCoord;   // Coordonnées de texture à transmettre au fragment shader

void main() {
    // Transformation de la position du vertex
    gl_Position = u_modelViewProjection * a_position;

    // Transmission des attributs au fragment shader
    v_color = a_color;
    v_normal = a_normal;
    v_texCoord = a_texCoord;
}