__kernel void aggregate(__global int* in1, __global int* in2, __global int* output)
{
  int i = get_global_id(0);
  output[i] = in1[i] + in2[i];
}
