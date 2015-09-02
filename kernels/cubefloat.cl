struct orientation {
  float x;
  float y;
  float z;
};

__kernel void cubefloat(__global float* input, __global float* output, const struct orientation o)
{
  int i = get_global_id(0);
  output[i] = input[i] * input[i] * input[i] + o.x;
}
