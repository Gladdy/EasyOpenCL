__kernel void simplekernel(	__global int* array, const int singlevalue )
{
	int i = get_global_id(0);
	array[i] = array[i] * array[i];
}
