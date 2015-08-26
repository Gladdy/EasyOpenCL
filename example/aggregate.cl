__kernel void aggregate(__global int* input_1, __global int* input_2, __global int* output)
{
  int i = get_global_id(0);
  output[i] = input_1[i] + input_2[i];
}
