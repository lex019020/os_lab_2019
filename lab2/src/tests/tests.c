#include "suite1.h"

void testRevertString(void) {
  char simple_string[] = "Hello";
  char str_with_spaces[] = "String with spaces";
  char str_with_odd_chars_num[] = "abc";
  char str_with_even_chars_num[] = "abcd";

  RevertString(simple_string);
  CU_ASSERT_STRING_EQUAL_FATAL(simple_string, "eHllo");

  RevertString(str_with_spaces);
  CU_ASSERT_STRING_EQUAL_FATAL(str_with_spaces, "iw gnirtSth spaces");

  RevertString(str_with_odd_chars_num);
  CU_ASSERT_STRING_EQUAL_FATAL(str_with_odd_chars_num, "abc");

  RevertString(str_with_even_chars_num);
  CU_ASSERT_STRING_EQUAL_FATAL(str_with_even_chars_num, "bacd");
}



int main() {
  CU_pSuite pSuite = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

  /* add a suite to the registry */
  pSuite = CU_add_suite("Suite", NULL, NULL);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
  if ((NULL == CU_add_test(pSuite, "test of RevertString function",
                           testRevertString))) {
    CU_cleanup_registry();
    return CU_get_error();
  }


 CU_pSuite suite1 = NULL;

 suite1 = CU_add_suite("Suite1", NULL, NULL);
 if (NULL == suite1) {
    CU_cleanup_registry();
    return CU_get_error();
  }

if ((NULL == CU_add_test(suite1, "test of numbers",
                           test_numbers))) {
    CU_cleanup_registry();
    return CU_get_error();
  }


  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
