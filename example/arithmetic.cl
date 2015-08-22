__kernel void arithmetic(	__global float* in1,
							__global float* in2,
							__global float* out)
{
	int i = get_global_id(0);
	out[i] = in1[i] * in2[i];
}
