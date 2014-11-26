varying vec4 diffuse,ambientGlobal,ambient,ecPos;
varying vec3 normal,halfVector;
uniform sampler2D tex;
void main()
{
    vec3 n,halfV,viewV,lightDir;
    float NdotL,NdotHV;
    vec4 color = ambientGlobal;
    float att, dist;
    
    /* a fragment shader can't write a verying variable, hence we need
    a new variable to store the normalized interpolated normal */
    n = normalize(normal);
	
    // Compute the light direction
	vec3 ecPos3 = ecPos.xyz/ecPos.w;
	vec4 light = gl_LightSource[0].position;
	vec3 light3 = light.xyz/light.w;
    lightDir = light3 - ecPos3;
    /* compute the distance to the light source to a varying variable*/
    dist = length(lightDir);
    lightDir = normalize(lightDir);
    /* compute the dot product between normal and ldir */
    NdotL = dot(n,lightDir);
	
    if (NdotL > 0.0) {
     
        att = 1.0 / (gl_LightSource[0].constantAttenuation +
                gl_LightSource[0].linearAttenuation * dist +
                gl_LightSource[0].quadraticAttenuation * dist * dist);
        color += att * (diffuse * NdotL + ambient);
        halfV = normalize(halfVector);
        NdotHV = max(dot(n,halfV),0.0);
        color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess);
    }
	vec4 colorTex = texture2D(tex,gl_TexCoord[0].st);
	//color = vec4(n.xyz,1.0);
	//color = vec4(vec3(dot(lightDir,n)),1.0);
	//color = vec4(ecPos3.xyz,1.0);
	//color = vec4(lightDir.xyz/10.0,1.0);
	//color = vec4(vec3(dist),1.0);
    gl_FragColor = vec4(color.rgb * colorTex.rgb, 1.0);
}