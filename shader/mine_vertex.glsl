#version 120

varying vec4 diffuse,amb_global,ambient,frag_pos,shadow_coord;
varying vec3 normal;
void main()
{   
    /* transform the normal into eye space and normalize the result */
    normal = normalize(gl_NormalMatrix * gl_Normal);
	
    /* compute the vertex position  in camera space. */
    frag_pos = gl_ModelViewMatrix * gl_Vertex;

	/* calculate the texture coordinates at the shadow map */
	shadow_coord = gl_TextureMatrix[7] * gl_Vertex;

    /* compute the diffuse, ambient and global ambient terms */
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    amb_global = gl_LightModel.ambient * gl_FrontMaterial.ambient;
    
	/* compute the texture coordinates for Texture0 */
    gl_TexCoord[0] = gl_MultiTexCoord0;

    gl_Position = ftransform();
}