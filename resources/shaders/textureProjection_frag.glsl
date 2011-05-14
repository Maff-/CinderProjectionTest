//warning: blending code is pretty terrible; but the idea is there ...
//built using some samples from http://www.lighthouse3d.com/

#version 120

uniform sampler2D projMap;
uniform float texScaling;
varying vec3 normal, lightDir, eyeVec;

void main (void)
{
  vec4 final_color = 
  (gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + 
  (gl_LightSource[0].ambient * gl_FrontMaterial.ambient);
	
  vec3 N = normalize(normal);
  vec3 L = normalize(lightDir);
	
  float lambertTerm = dot(N,L);
	
  if(lambertTerm > 0.0)
  {	
    final_color += gl_LightSource[0].diffuse * 
	gl_FrontMaterial.diffuse * lambertTerm;	
		
    vec3 E = normalize(eyeVec);
    vec3 R = reflect(-L, N);
    float specular = pow( max(dot(R, E), 0.0), 
	                     gl_FrontMaterial.shininess );
    final_color += gl_LightSource[0].specular * 
	               gl_FrontMaterial.specular * 
				   specular;	
		
		
    // Suppress the reverse projection.
    if( gl_TexCoord[0].q>0.0 )
    {
      //vec4 ProjMapColor = texture2DProj(projMap, gl_TexCoord[0]);
	  vec4 ProjMapColor = texture2D(projMap, fract(gl_TexCoord[0].st / texScaling ) );
      final_color += ProjMapColor;
    }
  }
  
  if( lambertTerm > 0.0 )
	final_color = vec4(1.0, 0.0, 0.0, 1.0);
  else
	final_color = vec4(0.0, 0.0, 1.0, 1.0);
  
  gl_FragColor = texture2D(projMap, fract(gl_TexCoord[0].st / texScaling) );	
}