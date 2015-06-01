__kernel void kernel(__global int* in1,
					__global int* in2,
					__global int* in3, 
					__global int* out)
{
	int i = get_global_id(0);
	out[i] = in1[i] * in2[i] + in3[i] + 1;
}