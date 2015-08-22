__kernel void sum( __global int* array, const int length)
{
  int i = get_global_id(0);

  uint dist = 1;
  uint l = length / 2;

  while(l > 0)
  {
    if(i % (dist*2) == 0) {
      array[i] = array[i] + array[i + dist];
    }

    dist = dist * 2;
    l = l / 2;

    barrier(CLK_LOCAL_MEM_FENCE);
  }

}
