#include "jdm-string.h"
#include <jdm-alloc.h>
#include <jdm-log.h>
#include <assert.h>
#include <string.h>

typedef struct {
  jdm_alloc_t *alloc;
  jdm_string_t *str;
} test_ctx;

static test_ctx get_test_ctx(char *c_str) {
  jdm_alloc_t *alloc =
      jdm_alloc_create((jdm_alloc_config_t){.type = JDM_ALLOC_SYSTEM});
  jdm_string_t *str = JDM_NULL;
  if (c_str != JDM_NULL) {
    str = jdm_string_create(alloc, c_str);
  } else {
    str = jdm_string_create_empty(alloc);
  }
  return (test_ctx){.alloc = alloc, .str = str};
}

static void destroy_test_ctx(test_ctx ctx) {
  jdm_string_destroy(ctx.str);
  jdm_alloc_destroy(ctx.alloc);
}

static b8 jdm_string_create_empty_destroy(void) {
  test_ctx ctx = get_test_ctx(JDM_NULL);
  if (0 != jdm_string_get_length(ctx.str))
    return JDM_FALSE;
  if (0 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  if (strcmp(jdm_string_get_c_str(ctx.str), "") != 0)
    return JDM_FALSE;
  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_create_destroy(void) {
  test_ctx ctx = get_test_ctx("test");
  if (4 != jdm_string_get_length(ctx.str))
    return JDM_FALSE;
  if (5 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  if (strcmp(jdm_string_get_c_str(ctx.str), "test") != 0)
    return JDM_FALSE;
  char c;
  jdm_string_get_at(ctx.str, 1, &c);
  if (c != 'e')
    return JDM_FALSE;
  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_copy_compare(void) {
  test_ctx ctx = get_test_ctx("test");
  jdm_string_t *copy = JDM_NULL;
  jdm_string_copy(ctx.str, &copy);
  if (jdm_string_compare(ctx.str, copy) != 0) {
    jdm_string_destroy(copy);
    return JDM_FALSE;
  }
  jdm_string_destroy(copy);
  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_reverse_ops(void) {
  test_ctx ctx = get_test_ctx("test");
  jdm_string_t *reversed = jdm_string_create(ctx.alloc, "tset");
  jdm_string_reverse(ctx.str);
  if (jdm_string_compare(ctx.str, reversed) != 0) {
    JDM_LOG_ERROR("%s", jdm_string_get_c_str(ctx.str));
    jdm_string_destroy(reversed);
    return JDM_FALSE;
  }
  jdm_string_destroy(reversed);
  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_append_ops(void) {
  test_ctx ctx = get_test_ctx("foo");
  jdm_string_status_t s = jdm_string_append_c_str(&ctx.str, "bar");
  if (6 != jdm_string_get_length(ctx.str))
    return JDM_FALSE;
  if (7 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  if (strcmp(jdm_string_get_c_str(ctx.str), "foobar") != 0)
    return JDM_FALSE;

  jdm_string_t *str2 = jdm_string_create(ctx.alloc, "baz");
  s = jdm_string_append(&ctx.str, str2);
  if (9 != jdm_string_get_length(ctx.str))
    return JDM_FALSE;
  if (10 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  if (strcmp(jdm_string_get_c_str(ctx.str), "foobarbaz") != 0)
    return JDM_FALSE;
  jdm_string_destroy(str2);

  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_prepend_ops(void) {
  test_ctx ctx = get_test_ctx("foo");
  jdm_string_prepend_c_str(&ctx.str, "bar");
  if (6 != jdm_string_get_length(ctx.str))
    return JDM_FALSE;
  if (7 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  if (strcmp(jdm_string_get_c_str(ctx.str), "barfoo") != 0)
    return JDM_FALSE;

  jdm_string_t *str2 = jdm_string_create(ctx.alloc, "baz");
  jdm_string_prepend(&ctx.str, str2);
  if (9 != jdm_string_get_length(ctx.str))
    return JDM_FALSE;
  if (10 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  if (strcmp(jdm_string_get_c_str(ctx.str), "bazbarfoo") != 0)
    return JDM_FALSE;
  jdm_string_destroy(str2);

  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_lower_upper(void) {
  test_ctx ctx = get_test_ctx("AbCdE  !%3");
  jdm_string_to_upper(ctx.str);
  if (strcmp(jdm_string_get_c_str(ctx.str), "ABCDE  !%3") != 0)
    return JDM_FALSE;
  jdm_string_to_lower(ctx.str);
  if (strcmp(jdm_string_get_c_str(ctx.str), "abcde  !%3") != 0)
    return JDM_FALSE;

  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_strip_null(void) {
  test_ctx ctx = get_test_ctx("  12345  ");
  jdm_string_strip_left(ctx.str, JDM_NULL);
  jdm_string_strip_right(ctx.str, JDM_NULL);
  if (5 != jdm_string_get_length(ctx.str))
    return JDM_FALSE;
  if (10 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  if (strcmp(jdm_string_get_c_str(ctx.str), "12345") != 0)
    return JDM_FALSE;

  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_strip_non_null(void) {
  test_ctx ctx = get_test_ctx("/+- 12345 -+/");
  jdm_string_strip_left_c_str(ctx.str, "/+- ");
  jdm_string_strip_right_c_str(ctx.str, "/+- ");
  if (5 != jdm_string_get_length(ctx.str))
    return JDM_FALSE;
  if (14 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  if (strcmp(jdm_string_get_c_str(ctx.str), "12345") != 0)
    return JDM_FALSE;

  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_substring_counts(void) {
  test_ctx ctx = get_test_ctx("ababbaabab");
  jdm_string_t *substr = jdm_string_create(ctx.alloc, "ab");
  if (!jdm_string_contains(ctx.str, substr))
    return JDM_FALSE;
  if (4 != jdm_string_count_occurences(ctx.str, substr))
    return JDM_FALSE;
  jdm_string_destroy(substr);
  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_substring_counts_overlap(void) {
  test_ctx ctx = get_test_ctx("banana");
  jdm_string_t *substr = jdm_string_create(ctx.alloc, "ana");
  if (!jdm_string_contains(ctx.str, substr))
    return JDM_FALSE;
  if (2 != jdm_string_count_occurences(ctx.str, substr))
    return JDM_FALSE;
  jdm_string_destroy(substr);
  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_validation_checks(void) {
  test_ctx ctx = get_test_ctx("123");
  if (!jdm_string_is_digit(ctx.str))
    return JDM_FALSE;
  jdm_string_empty(ctx.str);
  if (!jdm_string_is_empty(ctx.str))
    return JDM_FALSE;
  jdm_string_append_c_str(&ctx.str, "abc");
  if (!jdm_string_is_alpha(ctx.str))
    return JDM_FALSE;
  jdm_string_append_c_str(&ctx.str, "123");
  if (!jdm_string_is_alphanum(ctx.str))
    return JDM_FALSE;
  jdm_string_append_c_str(&ctx.str, "!");
  if (jdm_string_is_digit(ctx.str))
    return JDM_FALSE;
  if (jdm_string_is_alpha(ctx.str))
    return JDM_FALSE;
  if (jdm_string_is_alphanum(ctx.str))
    return JDM_FALSE;
  jdm_string_t* substr = jdm_string_create(ctx.alloc, "ab");
  if (!jdm_string_starts_with(ctx.str, substr))
    return JDM_FALSE;
  jdm_string_destroy(substr);
  substr = jdm_string_create(ctx.alloc, "3!");
  if (!jdm_string_ends_with(ctx.str, substr))
    return JDM_FALSE;
  jdm_string_destroy(substr);
  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_insert_ops(void) {
  test_ctx ctx = get_test_ctx("123");
  jdm_string_t *insert = jdm_string_create(ctx.alloc, "456");
  jdm_string_insert_at(&ctx.str, 1, insert);
  if (strcmp(jdm_string_get_c_str(ctx.str), "145623") != 0)
    return JDM_FALSE;
  jdm_string_destroy(insert);
  insert = jdm_string_create(ctx.alloc, "7");
  jdm_string_insert_at(&ctx.str, 0, insert);
  if (strcmp(jdm_string_get_c_str(ctx.str), "7145623") != 0)
    return JDM_FALSE;
  jdm_string_destroy(insert);
  insert = jdm_string_create(ctx.alloc, "8");
  jdm_string_insert_at(&ctx.str, 8, insert);
  if (strcmp(jdm_string_get_c_str(ctx.str), "71456238") != 0)
    return JDM_FALSE;
  jdm_string_destroy(insert);
  if (8 != jdm_string_get_length(ctx.str))
    return JDM_FALSE;
  if (9 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_replace_ops(void) {
  test_ctx ctx = get_test_ctx("foobar");
  jdm_string_t *replace = jdm_string_create(ctx.alloc, "izz");
  jdm_string_replace_at(&ctx.str, 1, 2, replace);
  if (strcmp(jdm_string_get_c_str(ctx.str), "fizzbar") != 0)
    return JDM_FALSE;
  jdm_string_destroy(replace);
  jdm_string_t *target = jdm_string_create(ctx.alloc, "zb");
  replace = jdm_string_create(ctx.alloc, "zbuzzb");
  jdm_string_replace_first(&ctx.str, target, replace);
  if (strcmp(jdm_string_get_c_str(ctx.str), "fizzbuzzbar") != 0)
    return JDM_FALSE;
  jdm_string_destroy(replace);
  jdm_string_destroy(target);
  target = jdm_string_create(ctx.alloc, "z");
  replace = jdm_string_create(ctx.alloc, "zz");
  jdm_string_replace_all(&ctx.str, target, replace);
  if (strcmp(jdm_string_get_c_str(ctx.str), "fizzzzbuzzzzbar") != 0)
    return JDM_FALSE;
  jdm_string_destroy(replace);
  jdm_string_destroy(target);
  jdm_string_t *delete = jdm_string_create(ctx.alloc, "f");
  jdm_string_remove_first(&ctx.str, delete);
  if (strcmp(jdm_string_get_c_str(ctx.str), "izzzzbuzzzzbar") != 0)
    return JDM_FALSE;
  jdm_string_destroy(delete);
  delete = jdm_string_create(ctx.alloc, "b");
  jdm_string_remove_first(&ctx.str, delete);
  if (strcmp(jdm_string_get_c_str(ctx.str), "izzzzuzzzzbar") != 0)
    return JDM_FALSE;
  jdm_string_destroy(delete);
  delete = jdm_string_create(ctx.alloc, "zzzz");
  jdm_string_remove_all(&ctx.str, delete);
  if (strcmp(jdm_string_get_c_str(ctx.str), "iubar") != 0)
    return JDM_FALSE;
  if (5 != jdm_string_get_length(ctx.str))
    return JDM_FALSE;
  if (16 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  sz reclaimed = 0;
  jdm_string_trim_capacity(&ctx.str, &reclaimed);
  if (6 != jdm_string_get_capacity(ctx.str))
    return JDM_FALSE;
  if (10 != reclaimed)
    return JDM_FALSE;
  jdm_string_destroy(delete);
  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

static b8 jdm_string_split_join_ops(void) {
  test_ctx ctx = get_test_ctx("000-000-000");
  jdm_string_status_t s;
  jdm_array_t* split_res = JDM_NULL;
  sz arr_size;

  jdm_string_t* split_on = jdm_string_create(ctx.alloc, "-");
  s = jdm_string_split(ctx.str, &split_res, split_on);
  if(s != JDM_STRING_OK) {
    JDM_LOG_ERROR("String split failed.");
    return JDM_FALSE;
  }
  jdm_free(ctx.alloc, split_on);

  arr_size = jdm_array_size(split_res); 
  if(arr_size != 3) {
    JDM_LOG_ERROR("Incorrect array size.");
    return JDM_FALSE;
  }

  jdm_string_t** actual = jdm_alloc(ctx.alloc, sizeof(jdm_string_t*));
  for(sz i = 0; i < arr_size; i++) {
    jdm_string_t* expected = jdm_string_create(ctx.alloc, "000");
    jdm_array_get_at(split_res, (u8**)&actual, i);
    if(jdm_string_compare(*actual, expected) != 0) {
      JDM_LOG_ERROR("Split element not expected.");
      JDM_LOG_ERROR("Expected: %s", jdm_string_get_c_str(expected));
      JDM_LOG_ERROR("actual (%p): %s", *actual, jdm_string_get_c_str(*actual));
      jdm_string_destroy(*actual);
      return JDM_FALSE;
    }
    jdm_string_destroy(expected);
  }

  jdm_string_t* join_with = jdm_string_create(ctx.alloc, "~");
  jdm_string_t* joined_str = jdm_string_create_empty(ctx.alloc);
  jdm_string_t* expected = jdm_string_create(ctx.alloc, "000~000~000");
  s = jdm_string_join(&joined_str, split_res, join_with);
  if(s != JDM_STRING_OK) {
    JDM_LOG_ERROR("String split join.");
    return JDM_FALSE;
  }

  if(0 != jdm_string_compare(joined_str, expected)) {
    JDM_LOG_ERROR("joined string does not meet expected.");
    JDM_LOG_ERROR("joined_str: %s", jdm_string_get_c_str(joined_str));
    JDM_LOG_ERROR("expected: %s", jdm_string_get_c_str(expected));
    JDM_LOG_ERROR("joined string does not meet expected.");
    return JDM_FALSE;
  }
  jdm_string_destroy(join_with);
  jdm_string_destroy(joined_str);
  jdm_string_destroy(expected);

  for(sz i = 0; i < arr_size; i++) {
    jdm_array_get_at(split_res, (u8**)&actual, i);
    jdm_string_destroy(*actual); 
  }
  jdm_free(ctx.alloc, actual);

  jdm_array_destroy(split_res);
  destroy_test_ctx(ctx);
  return JDM_TRUE;
}

int main() {
  jdm_log_init();
  if (jdm_string_create_empty_destroy() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_create_empty_destroy()");
    return 1;
  }
  if (jdm_string_create_destroy() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_create_destroy()");
    return 1;
  }
  if (jdm_string_copy_compare() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_copy_compare()");
    return 1;
  }
  if (jdm_string_reverse_ops() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_reverse_ops()");
    return 1;
  }
  if (jdm_string_append_ops() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_append_ops()");
    return 1;
  }
  if (jdm_string_prepend_ops() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_prepend_ops()");
    return 1;
  }
  if (jdm_string_lower_upper() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_lower_upper()");
    return 1;
  }
  if (jdm_string_strip_null() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_strip_null()");
    return 1;
  }
  if (jdm_string_strip_non_null() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_strip_non_null()");
    return 1;
  }
  if (jdm_string_substring_counts() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_substring_counts()");
    return 1;
  }
  if (jdm_string_substring_counts_overlap() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_substring_counts_overlap()");
    return 1;
  }
  if (jdm_string_validation_checks() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_digit_alpha_checks()");
    return 1;
  }
  if (jdm_string_insert_ops() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_insert_ops()");
    return 1;
  }
  if (jdm_string_replace_ops() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_replace_ops()");
    return 1;
  }
  if (jdm_string_split_join_ops() != JDM_TRUE) {
    JDM_LOG_ERROR("Failure in jdm_string_split_join_ops()");
    return 1;
  }
  jdm_log_cleanup();
  return 0;
}
