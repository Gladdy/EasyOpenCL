__kernel void sum(	__global int* input, 
					__global int* output,
					int width )
{
	int tid = get_global_id(0);
	output[tid] = input[tid];

	barrier(CLK_GLOBAL_MEM_FENCE);

	int offset = 1;

	while(offset <= width) {

		if(tid % 2*offset == 0) {
			output[tid] += output[tid + offset];
		}
		
		offset *= 2;
		barrier(CLK_GLOBAL_MEM_FENCE);
	}

}