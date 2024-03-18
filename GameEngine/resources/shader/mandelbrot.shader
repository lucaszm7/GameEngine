#version 440 core

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D img_output;

layout (location = 0) uniform int MAX_IT_MULTIPLAYER;

uniform double real_x;
uniform double real_y;
uniform double imag_x;
uniform double imag_y;

void main()
{
	vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	double real = real_x + (real_y - real_x) * double(texelCoord.x) / double(1000);
	double imag = imag_x + (imag_y - imag_x) * double(texelCoord.y) / double(1000);

	double ca = real;
	double cb = imag;

	int it = 0;
	int max_it = MAX_IT_MULTIPLAYER;
	while (it < max_it && (real*real + imag*imag) < 4)
	{
		double aa = real * real - imag * imag;
		double bb = 2 * real * imag;
		
		real = aa + ca;
		imag = bb + cb;

		++it;
	}

	float t = float(it) / float(max_it);
	float rr = (9*(1-t)*t*t*t);
	float rg = (15*(1-t)*(1-t)*t*t);
    float rb =  (8.5*(1-t)*(1-t)*(1-t)*t);
	value = vec4(rr, rg, rb, 1.0);

	imageStore(img_output, texelCoord, value);
}
