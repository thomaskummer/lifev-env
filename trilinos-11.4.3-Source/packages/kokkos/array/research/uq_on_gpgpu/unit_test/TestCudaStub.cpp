
template <typename Scalar>
int mainCuda(bool test_flat, bool test_orig, bool test_block, bool check, 
	     int device_id)
{
  return 0 ;
}

template int mainCuda<float>(bool, bool, bool, bool, int);
template int mainCuda<double>(bool, bool, bool, bool, int);

