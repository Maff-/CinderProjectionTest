#version 120
		
uniform mat4 TexGenMat;
uniform mat4 InvViewMat;
varying vec3 normal, lightDir, eyeVec;

void main()
{	
	normal = gl_NormalMatrix * gl_Normal;
	
	vec4 posEye =  gl_ModelViewMatrix * gl_Vertex;;
	vec4 posWorld = InvViewMat * posEye;
	gl_TexCoord[0] = TexGenMat * posWorld;

	lightDir = vec3(gl_LightSource[0].position.xyz - posEye.xyz);
	eyeVec = -posEye.xyz;

	gl_Position = ftransform();		
}