varying vec4 diffuse,ambientGlobal,ambient,ecPos;
varying vec3 normal,halfVector;
uniform sampler2D tex;
uniform sampler2D ShadowMap;
varying vec4 ShadowCoord;
void main()
{
    vec3 n,halfV,viewV,lightDir;
    float NdotL,NdotHV;
    vec4 color = vec4(0);
    float att, dist;
    
	vec4 shadowC = ShadowCoord / ShadowCoord.w ;
	
	// Used to lower moir¨¦ pattern and self-shadowing
	
	float distanceFromLight = texture2D(ShadowMap,shadowC.st).z;
 	float shadow = 1.0;
 	if (ShadowCoord.w > 0.0){
		if (abs(distanceFromLight-shadowC.z)>1e-6 && distanceFromLight < shadowC.z)
			shadow = 0.2;
	}
    /* a fragment shader can't write a verying variable, hence we need
    a new variable to store the normalized interpolated normal */
    n = normalize(normal);
	
    // Compute the light direction
	vec3 ecPos3 = ecPos.xyz/ecPos.w;
	if (shadow>0.25) {
		halfV = normalize(halfVector);
		NdotHV = max(dot(n,halfV),0.0);
		if(abs(gl_LightSource[0].position.w)<1e-5){
			lightDir = gl_LightSource[0].position.xyz;
			NdotL = dot(n,lightDir);
			if (NdotL > 0.0) {
				color = max(color, (diffuse * NdotL + ambient));
				color = max(color, gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess));
			}
		} else {
			vec3 light3 = gl_LightSource[0].position.xyz/gl_LightSource[0].position.w;
			lightDir = light3 - ecPos3;
			dist = length(lightDir);
			lightDir = normalize(lightDir);
			NdotL = dot(n,lightDir);
			if (NdotL > 0.0) {
				att = 1.0 / (gl_LightSource[0].constantAttenuation +
						gl_LightSource[0].linearAttenuation * dist +
						gl_LightSource[0].quadraticAttenuation * dist * dist);
				color += att * (diffuse * NdotL + ambient);
				color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess);
			}
		}
	}
	vec4 colorTex = texture2D(tex,gl_TexCoord[0].st);
	//color = vec4(n.xyz,1.0);
	//color = vec4(vec3(dot(lightDir,n)),1.0);
	//color = vec4(ecPos3.xyz,1.0);
	//color = vec4(lightDir.xyz/10.0,1.0);
	//color = vec4(vec3(dist),1.0);
	//color = vec4(1);
	color.rgb = color.rgb*shadow + ambientGlobal.rgb;
    gl_FragColor = vec4(colorTex.rgb * color.rgb, 1.0);
}