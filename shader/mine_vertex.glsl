varying vec4 diffuse,ambientGlobal,ambient,ecPos;
varying vec3 normal,halfVector;
void main()
{   
    vec3 aux;
    
    /* first transform the normal into eye space and normalize the result */
    normal = normalize(gl_NormalMatrix * gl_Normal);
	
    /* compute the vertex position  in camera space. */
    ecPos = gl_ModelViewMatrix * gl_Vertex;
 
    /* Normalize the halfVector to pass it to the fragment shader */
    halfVector = normalize(gl_LightSource[0].halfVector.xyz / gl_LightSource[0].halfVector.w);
    
    /* Compute the diffuse, ambient and globalAmbient terms */
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = ftransform();
}