varying vec4 diffuse,amb_global,ambient,frag_pos,shadow_coord;
varying vec3 normal;
uniform sampler2D tex;
uniform sampler2D ShadowMap;
uniform int rg = 2;			 //determines the sampling numbers
uniform float offset = 4e-4; //determines the sampling distance

vec3 toVec3(vec4 x){
	return x.rgb/x.w;
}

//percentage closer filtering
float pcf(vec4 sc){
	int x, y;
	float sum = 0.0, dist, shadow;
	if (sc.w > 0.0){
		for(x=-rg;x<=rg;++x)
			for(y=-rg;y<=rg;++y){
				dist = texture2D(ShadowMap,vec2(sc.s+float(x)*offset,sc.t+float(y)*offset)).z;
 				shadow = 1.0;
				if (abs(dist-sc.z)>2e-4 && dist < sc.z)
					shadow = 0.2;
				sum += shadow;
			}
	}
	int rg2 = rg*2 + 1;
	return sum / float(rg2*rg2);
}

void main()
{
    vec3 N,L,R,V;
    float NdotL,att,dist,RdotV;
    vec4 color_amb = amb_global, color_diff = vec4(0.0), color_sp = vec4(0.0);
	vec4 colorTex = texture2D(tex,gl_TexCoord[0].st);
	float shadow = pcf(shadow_coord / shadow_coord.w);

	if(abs(colorTex.a)<1e-5)discard;

    N = normalize(normal); //normalize the interpolated normal
	V = toVec3(frag_pos);  //calculate the fragment's position in vec3

	//parallel light from infinitely far away
	if(abs(gl_LightSource[0].position.w)<1e-5){
		L = normalize(gl_LightSource[0].position.xyz);
		NdotL = max(dot(N,L),0.0);
		R = 2.0 * NdotL * N - L; //calculate the reflection light's direction
		RdotV = max(-dot(normalize(V),R),0.0);
		color_amb += ambient;
		if (NdotL > 0.0) {
			color_diff += diffuse * NdotL;
			color_sp += gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(RdotV,gl_FrontMaterial.shininess);
		}
	} else {
		vec3 light_pos = toVec3(gl_LightSource[0].position);
		shadow = 1.0; //disable shadow for point light, TODO: add this functionality
		L = light_pos - V;
		dist = length(L);
		L = normalize(L);
		NdotL = dot(N,L);
		R = 2.0 * NdotL * N - L; //calculate the reflection light's direction
		RdotV = max(-dot(normalize(V),R),0.0);
		if (NdotL > 0.0) {
			att = 1.0 / (gl_LightSource[0].constantAttenuation +
					gl_LightSource[0].linearAttenuation * dist +
					gl_LightSource[0].quadraticAttenuation * dist * dist);
			color_amb += att * ambient;
			color_diff += att * diffuse * NdotL;
			color_sp += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(RdotV,gl_FrontMaterial.shininess);
		}
	}
	vec3 color = (color_amb.rgb + color_diff.rgb * shadow) * colorTex.rgb + color_sp.rgb * shadow;
	//color = colorTex.aaa;
    gl_FragColor = vec4(color.rgb, 1.0);
}