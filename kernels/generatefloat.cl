__kernel void generatefloat(__global float* array)
{
  int i = get_global_id(0);
  array[i] = 1.0f;
}
