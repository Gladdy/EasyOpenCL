__kernel void aggregatefloat(__global float* in1, __global float* in2, __global float* output)
{
  int i = get_global_id(0);
  output[i] = in1[i] + in2[i];
}
