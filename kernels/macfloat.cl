struct MAC {
  float mult;
  float add;
};

__kernel void macfloat(__global float* input, __global float* output, const struct MAC mac)
{
  int i = get_global_id(0);
  output[i] = input[i] * mac.mult + mac.add;
}
