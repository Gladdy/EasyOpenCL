__kernel void generate(__global int* array)
{
  int i = get_global_id(0);
  array[i] = i;
}
