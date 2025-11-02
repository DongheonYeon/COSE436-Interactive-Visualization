//
// Passing example with per-vertex coloring
//
#version 150

out vec3 perVertexColor;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
 
void main() 
{
  for(int i = 0; i < gl_in.length(); i++) 
  {
    gl_Position = gl_in[i].gl_Position;
   
    // Assign vertex color
    perVertexColor = vec3(0,0,1);
	    
    EmitVertex();    
  }
  EndPrimitive();
}
