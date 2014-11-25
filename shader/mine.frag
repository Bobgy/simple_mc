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
    lightDir = light.xyz/light.w - ecPos3;
    /* compute the distance to the light source to a varying variable*/
    dist = length(lightDir);
    lightDir = normalize(lightDir);
	vec4 lightPos = light;
    /* compute the dot product between normal and ldir */
    NdotL = max(dot(n,lightDir), -0.0000000001);
	
    if (NdotL > 0.0) {
     
        att = 1.0 / (gl_LightSource[0].constantAttenuation +
                gl_LightSource[0].linearAttenuation * dist +
                gl_LightSource[0].quadraticAttenuation * dist * dist);
        color += att * (diffuse * NdotL + ambient);
        //halfV = normalize(halfVector);
		vec3 R = reflect(lightDir, n);
        //NdotHV = max(dot(n,halfV),0.0);
		NdotHV = max(dot(-R, lightDir),0.0);
        color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess);
    } else color = vec4(vec2(1.00,1.00).xxxx);
	vec4 colorTex = texture2D(tex,gl_TexCoord[0].st);
	//float dep = gl_FragCoord.z;
	//color = vec4(n.xyz,1.0);
	//color = vec4(light.xyz/light.w,1.0);
	//color = vec4(ecPos3.xyz,1.0);
	//float col = distance(ecPos.xy,light.xy);
	//color = vec4(col,col,col,1.0);
	//color = vec4(lightPos.xyz/10.0,1.0);
	color = vec4(vec3(distance(ecPos3,light.xyz/light.w)),1.0);
    gl_FragColor = vec4(color.rgb * colorTex.rgb, 1.0);
}