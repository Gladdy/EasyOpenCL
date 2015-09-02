__kernel void generate(__global int* output)
{
  int i = get_global_id(0);
  output[i] = i;
}
