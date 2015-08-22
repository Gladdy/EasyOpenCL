__kernel void simplekernel(	__global int* input
                          , const int singlevalue )
{
	int i = get_global_id(0);
	output[i] = input[i] * singlevalue;
}
