#include "jdm-string.h"
#include <jdm-alloc.h>
#include <jdm-log.h>
#include <stdlib.h>
#include <string.h>

struct jdm_string_t {
  jdm_alloc_t *alloc;
  sz length;
  sz capacity;
  char c_str[];
};

static jdm_string_status_t grow_string(jdm_string_t **str, sz additional) {
  jdm_alloc_t *alloc = (*str)->alloc;

  if ((*str)->length == 0) {
    additional++; // add one for null terminator
  }

  sz new_capacity = sizeof(jdm_string_t) + (*str)->capacity + additional;
  jdm_string_t *tmp = jdm_realloc(alloc, *str, new_capacity);
  if (tmp == JDM_NULL) {
    return JDM_STRING_ERR_ALLOC;
  } else {
    *str = tmp;
    JDM_ZERO_MEMORY((*str)->c_str + (*str)->capacity, additional);
    (*str)->capacity = new_capacity - sizeof(jdm_string_t);
    return JDM_STRING_OK;
  }
}

static jdm_string_status_t shrink_string(jdm_string_t **str, sz *reclaimed) {
  jdm_alloc_t *alloc = (*str)->alloc;
  sz new_capacity = sizeof(jdm_string_t) + (*str)->length + 1;
  sz reclaim = (*str)->capacity - ((*str)->length + 1);

  jdm_string_t *tmp = jdm_realloc(alloc, *str, new_capacity);
  if (tmp == JDM_NULL) {
    return JDM_STRING_ERR_ALLOC;
  } else {
    *str = tmp;
    *reclaimed = reclaim;
    (*str)->capacity = new_capacity - sizeof(jdm_string_t);
    return JDM_STRING_OK;
  }
}

jdm_string_t *jdm_string_create_empty(jdm_alloc_t *alloc) {
  jdm_string_t *s = jdm_calloc(alloc, 1, sizeof(jdm_string_t));
  s->alloc = alloc;
  return s;
}

jdm_string_t *jdm_string_create(jdm_alloc_t *alloc, const char *c_str) {
  sz len = strlen(c_str);
  sz cap = len + 1;
  sz total_size = sizeof(jdm_string_t) + cap;
  jdm_string_t *s = jdm_calloc(alloc, 1, total_size);
  s->alloc = alloc;
  s->length = len;
  s->capacity = cap;
  JDM_COPY_MEMORY(s->c_str, c_str, len);
  return s;
}

void jdm_string_destroy(jdm_string_t *str) {
  jdm_alloc_t *alloc = str->alloc;
  jdm_free(alloc, str);
}

sz jdm_string_get_length(jdm_string_t *str) { return str->length; }

sz jdm_string_get_capacity(jdm_string_t *str) { return str->capacity; }

char *jdm_string_get_c_str(jdm_string_t *str) {
  return str->length > 0 ? str->c_str : "";
}

jdm_string_status_t jdm_string_copy(const jdm_string_t *str,
                                    jdm_string_t **out) {
  jdm_alloc_t *alloc = str->alloc;
  jdm_string_t *copy = jdm_string_create(alloc, str->c_str);
  if (copy == JDM_NULL) {
    return JDM_STRING_ERR_ALLOC;
  }
  *out = copy;
  return JDM_STRING_OK;
}

void jdm_string_empty(jdm_string_t *str) {
  JDM_ZERO_MEMORY(str->c_str, str->capacity);
  str->length = 0;
}

void jdm_string_reverse(jdm_string_t *str) {
  for (sz i = 0, j = (str->length - 1); i < j; i++, j--) {
    char tmp = str->c_str[i];
    str->c_str[i] = str->c_str[j];
    str->c_str[j] = tmp;
  }
}

b8 jdm_string_is_empty(const jdm_string_t *str) { return str->length == 0; }

s8 jdm_string_compare(const jdm_string_t *str1, const jdm_string_t *str2) {
  return strcmp(str1->c_str, str2->c_str);
}

jdm_string_status_t jdm_string_trim_capacity(jdm_string_t **str, sz *out) {
  jdm_string_status_t s;
  jdm_string_t *sobj = *str;
  if (sobj->length < (sobj->capacity - 1)) {
    return shrink_string(str, out);
  }
  return JDM_STRING_OK;
}

jdm_string_status_t jdm_string_append_c_str(jdm_string_t **str,
                                            const char *append) {
  sz append_len = strlen(append);
  jdm_string_status_t s;
  if (((*str)->length + append_len) >= (*str)->capacity &&
      (s = grow_string(str, append_len)) != JDM_STRING_OK) {
    return s;
  }
  JDM_COPY_MEMORY((*str)->c_str + (*str)->length, append, append_len);
  (*str)->length += append_len;
  return JDM_STRING_OK;
}

jdm_string_status_t jdm_string_append(jdm_string_t **str,
                                      const jdm_string_t *append) {
  return jdm_string_append_c_str(str, append->c_str);
}

jdm_string_status_t jdm_string_prepend_c_str(jdm_string_t **str,
                                             const char *prepend) {
  sz prepend_len = strlen(prepend);
  jdm_string_status_t s;
  if (((*str)->length + prepend_len) >= (*str)->capacity &&
      (s = grow_string(str, prepend_len)) != JDM_STRING_OK) {
    return s;
  }
  JDM_MOVE_MEMORY((*str)->c_str + prepend_len, (*str)->c_str, (*str)->length);
  JDM_COPY_MEMORY((*str)->c_str, prepend, prepend_len);
  (*str)->length += prepend_len;
  return JDM_STRING_OK;
}

jdm_string_status_t jdm_string_prepend(jdm_string_t **str,
                                       const jdm_string_t *prepend) {
  return jdm_string_prepend_c_str(str, prepend->c_str);
}

jdm_string_status_t jdm_string_insert_c_str_at(jdm_string_t **str, sz idx,
                                               const char *insert) {
  sz insert_len = strlen(insert);
  jdm_string_status_t s;
  if (((*str)->length + insert_len) >= (*str)->capacity &&
      (s = grow_string(str, insert_len)) != JDM_STRING_OK) {
    return s;
  }
  if (idx == 0) {
    return jdm_string_prepend_c_str(str, insert);
  } else if (idx == (*str)->length + 1) {
    return jdm_string_append_c_str(str, insert);
  } else {
    JDM_MOVE_MEMORY((*str)->c_str + idx + insert_len, (*str)->c_str + idx,
                    (*str)->length - idx);
    JDM_COPY_MEMORY((*str)->c_str + idx, insert, insert_len);
    (*str)->length += insert_len;
  }
  return JDM_STRING_OK;
}

jdm_string_status_t jdm_string_insert_at(jdm_string_t **str, sz idx,
                                         const jdm_string_t *insert) {
  return jdm_string_insert_c_str_at(str, idx, insert->c_str);
}

jdm_string_status_t jdm_string_replace_c_str_at(jdm_string_t **str, sz idx,
                                                sz len_to_replace,
                                                const char *replace) {
  jdm_string_status_t s;
  jdm_string_t *sobj = *str;

  sz replace_len = strlen(replace);

  if (idx > sobj->length || len_to_replace > sobj->length - idx) {
    return JDM_STRING_ERR_OUT_OF_RANGE;
  }

  sz new_length;

  if (replace_len >= len_to_replace) {
    sz growth = replace_len - len_to_replace;

    if (sobj->length > SIZE_MAX - growth) {
      return JDM_STRING_ERR_OVERFLOW;
    }

    new_length = sobj->length + growth;

    if (new_length >= sobj->capacity) {
      if ((s = grow_string(str, growth)) != JDM_STRING_OK) {
        return s;
      }
      sobj = *str;
    }
  } else {
    new_length = sobj->length - (len_to_replace - replace_len);
  }

  sz after_replace_idx = idx + len_to_replace;
  sz len_after_replace = sobj->length - after_replace_idx;

  JDM_MOVE_MEMORY(sobj->c_str + idx + replace_len,
                  sobj->c_str + after_replace_idx, len_after_replace);

  JDM_COPY_MEMORY(sobj->c_str + idx, replace, replace_len);

  if (new_length < sobj->length) {
    sz removed = sobj->length - new_length;

    JDM_ZERO_MEMORY(sobj->c_str + new_length, removed);
  }

  sobj->length = new_length;

  return JDM_STRING_OK;
}

jdm_string_status_t jdm_string_replace_at(jdm_string_t **str, sz idx,
                                          sz len_to_replace,
                                          const jdm_string_t *replace) {
  return jdm_string_replace_c_str_at(str, idx, len_to_replace, replace->c_str);
}

jdm_string_status_t jdm_string_get_at(const jdm_string_t *str, sz idx,
                                      char *out) {
  if (idx >= str->length) {
    return JDM_STRING_ERR_BAD_IDX;
  }
  *out = str->c_str[idx];
  return JDM_STRING_OK;
}

static jdm_string_status_t c_str_find_idx_char(const char *c_str, const sz len,
                                               const char elem, sz *out) {
  for (sz i = 0; i < len; i++) {
    if (c_str[i] == elem) {
      if (out) {
        *out = i;
      }
      return JDM_STRING_OK;
    }
  }
  return JDM_STRING_ERR_NOT_FOUND;
}

static jdm_string_status_t c_str_find_idx_c_str(const char *c_str,
                                                const char *c_substr, sz *out) {
  const char *tmp;
  if ((tmp = strstr(c_str, c_substr)) != JDM_NULL) {
    if (out) {
      *out = (tmp - c_str);
    }
    return JDM_STRING_OK;
  } else {
    return JDM_STRING_ERR_NOT_FOUND;
  }
}

jdm_string_status_t jdm_string_find_idx_char(const jdm_string_t *str,
                                             const char elem, sz *out) {
  return c_str_find_idx_char(str->c_str, str->length, elem, out);
}

jdm_string_status_t jdm_string_find_idx_c_str(const jdm_string_t *str,
                                              const char *c_substr, sz *out) {
  return c_str_find_idx_c_str(str->c_str, c_substr, out);
}

jdm_string_status_t jdm_string_find_idx(const jdm_string_t *str,
                                        const jdm_string_t *substr, sz *out) {
  return c_str_find_idx_c_str(str->c_str, substr->c_str, out);
}

static b8 c_str_contains_char(const char *c_str, const sz len,
                              const char elem) {
  jdm_string_status_t s = c_str_find_idx_char(c_str, len, elem, NULL);
  if (s == JDM_STRING_OK) {
    return JDM_TRUE;
  }
  return JDM_FALSE;
}

static b8 c_str_contains_c_str(const char *c_str, const char *c_substr) {
  jdm_string_status_t s = c_str_find_idx_c_str(c_str, c_substr, NULL);
  if (s == JDM_STRING_OK) {
    return JDM_TRUE;
  }
  return JDM_FALSE;
}

b8 jdm_string_contains_char(const jdm_string_t *str, const char elem) {
  return c_str_contains_char(str->c_str, str->length, elem);
}

b8 jdm_string_contains_c_str(const jdm_string_t *str, const char *c_substr) {
  return c_str_contains_c_str(str->c_str, c_substr);
}
b8 jdm_string_contains(const jdm_string_t *str, const jdm_string_t *substr) {
  return jdm_string_contains_c_str(str, substr->c_str);
}

static sz c_str_count_occurences_char(const char *c_str, const sz len,
                                      const char elem) {
  sz count = 0;
  for (sz i = 0; i < len; i++) {
    if (c_str[i] == elem)
      count++;
  }
  return count;
}

sz jdm_string_count_occurences_char(const jdm_string_t *str, const char elem) {
  return c_str_count_occurences_char(str->c_str, str->length, elem);
}

static sz c_str_count_occurences_c_str(const char *c_str,
                                       const char *c_substr) {
  sz count = 0;
  const char *tmp = c_str;
  while (tmp = strstr(tmp, c_substr)) {
    count++;
    tmp++;
  }
  return count;
}

sz jdm_string_count_occurences_c_str(const jdm_string_t *str,
                                     const char *c_substr) {
  return c_str_count_occurences_c_str(str->c_str, c_substr);
}

sz jdm_string_count_occurences(const jdm_string_t *str,
                               const jdm_string_t *substr) {
  return jdm_string_count_occurences_c_str(str, substr->c_str);
}

jdm_string_status_t jdm_string_replace_first_c_str(jdm_string_t **str,
                                                   const char *c_substr,
                                                   const char *replace) {
  sz idx;
  jdm_string_status_t s;
  s = jdm_string_find_idx_c_str(*str, c_substr, &idx);
  if (s != JDM_STRING_OK) {
    return s;
  }
  return jdm_string_replace_c_str_at(str, idx, strlen(c_substr), replace);
}

jdm_string_status_t jdm_string_replace_first(jdm_string_t **str,
                                             const jdm_string_t *substr,
                                             const jdm_string_t *replace) {
  return jdm_string_replace_first_c_str(str, substr->c_str, replace->c_str);
}

jdm_string_status_t jdm_string_replace_all_c_str(jdm_string_t **str,
                                                 const char *c_substr,
                                                 const char *replace) {
  sz idx = 0;
  sz step = 0;
  char *c_str = (*str)->c_str;
  jdm_string_status_t s;
  while ((s = c_str_find_idx_c_str(c_str + step, c_substr, &idx)) ==
         JDM_STRING_OK) {
    jdm_string_status_t rs =
        jdm_string_replace_c_str_at(str, step + idx, strlen(c_substr), replace);
    if (rs != JDM_STRING_OK) {
      return rs;
    }
    step += idx + strlen(replace);
    c_str = (*str)->c_str; // need to refresh pointer since it could have moved
  }
  return JDM_STRING_OK;
}
jdm_string_status_t jdm_string_replace_all(jdm_string_t **str,
                                           const jdm_string_t *substr,
                                           const jdm_string_t *replace) {
  return jdm_string_replace_all_c_str(str, substr->c_str, replace->c_str);
}

jdm_string_status_t jdm_string_remove_first_c_str(jdm_string_t **str,
                                                  const char *c_substr) {
  return jdm_string_replace_first_c_str(str, c_substr, "");
}

jdm_string_status_t jdm_string_remove_first(jdm_string_t **str,
                                            const jdm_string_t *substr) {
  return jdm_string_replace_first_c_str(str, substr->c_str, "");
}

jdm_string_status_t jdm_string_remove_all_c_str(jdm_string_t **str,
                                                const char *c_substr) {
  return jdm_string_replace_all_c_str(str, c_substr, "");
}

jdm_string_status_t jdm_string_remove_all(jdm_string_t **str,
                                          const jdm_string_t *substr) {
  return jdm_string_replace_all_c_str(str, substr->c_str, "");
}

b8 jdm_string_is_digit(const jdm_string_t *str) {
  if (str->length == 0)
    return JDM_FALSE;
  for (sz i = 0; i < str->length; i++) {
    if ((str->c_str[i]) < '0' || (str->c_str[i]) > '9') {
      return JDM_FALSE;
    }
  }
  return JDM_TRUE;
}

b8 jdm_string_is_alpha(const jdm_string_t *str) {
  if (str->length == 0)
    return JDM_FALSE;
  for (sz i = 0; i < str->length; i++) {
    if (!((str->c_str[i] >= 'A' && str->c_str[i] <= 'Z') ||
          (str->c_str[i] >= 'a' && str->c_str[i] <= 'z'))) {
      return JDM_FALSE;
    }
  }
  return JDM_TRUE;
}

b8 jdm_string_is_alphanum(const jdm_string_t *str) {
  if (str->length == 0)
    return JDM_FALSE;
  for (sz i = 0; i < str->length; i++) {
    if (!((str->c_str[i] >= 'A' && str->c_str[i] <= 'Z') ||
          (str->c_str[i] >= 'a' && str->c_str[i] <= 'z') ||
          (str->c_str[i] >= '0' && str->c_str[i] <= '9'))) {
      return JDM_FALSE;
    }
  }
  return JDM_TRUE;
}

b8 jdm_string_starts_with_c_str(const jdm_string_t *str, const char *c_substr) {
  sz sublen = strlen(c_substr);
  if (sublen > str->length) {
    return JDM_FALSE;
  }
  for (sz i = 0; i < sublen; i++) {
    if (str->c_str[i] != c_substr[i]) {
      return JDM_FALSE;
    }
  }
  return JDM_TRUE;
}

b8 jdm_string_starts_with(const jdm_string_t *str, const jdm_string_t *substr) {
  return jdm_string_starts_with_c_str(str, substr->c_str);
}

b8 jdm_string_ends_with_c_str(const jdm_string_t *str, const char *c_substr) {
  sz sublen = strlen(c_substr);
  if (sublen > str->length) {
    return JDM_FALSE;
  }
  sz back_idx = str->length - sublen;
  for (sz i = 0; i < sublen; i++) {
    if (str->c_str[back_idx + i] != c_substr[i]) {
      return JDM_FALSE;
    }
  }
  return JDM_TRUE;
}

b8 jdm_string_ends_with(const jdm_string_t *str, const jdm_string_t *substr) {
  return jdm_string_ends_with_c_str(str, substr->c_str);
}

void jdm_string_to_upper(jdm_string_t *str) {
  for (sz i = 0; i < str->length; i++) {
    if ((str->c_str[i]) >= 'a' && (str->c_str[i]) <= 'z') {
      str->c_str[i] = str->c_str[i] - 'a' + 'A';
    }
  }
}

void jdm_string_to_lower(jdm_string_t *str) {
  for (sz i = 0; i < str->length; i++) {
    if ((str->c_str[i]) >= 'A' && (str->c_str[i]) <= 'Z') {
      str->c_str[i] = str->c_str[i] - 'A' + 'a';
    }
  }
}

void jdm_string_trim_left(jdm_string_t *str, sz n) {
  JDM_MOVE_MEMORY(str->c_str, str->c_str + n, (str->length - n));
  JDM_ZERO_MEMORY((str->c_str + str->length - n), n);
  str->length -= n;
}

void jdm_string_trim_right(jdm_string_t *str, sz n) {
  JDM_ZERO_MEMORY((str->c_str + str->length - n), n);
  str->length -= n;
}

void jdm_string_strip_left_c_str(jdm_string_t *str, const char *strip) {
  const char *_strip = strip != JDM_NULL ? strip : " ";
  sz striplen = strlen(_strip);
  sz to_strip = 0;
  for (sz i = 0; i < str->length; i++) {
    char c = str->c_str[i];
    if (c_str_contains_char(_strip, striplen, c)) {
      to_strip++;
    } else {
      break;
    }
  }
  jdm_string_trim_left(str, to_strip);
}

void jdm_string_strip_left(jdm_string_t *str, const jdm_string_t *strip) {
  if (strip != JDM_NULL)
    jdm_string_strip_left_c_str(str, strip->c_str);
  else
    jdm_string_strip_left_c_str(str, JDM_NULL);
}

void jdm_string_strip_right_c_str(jdm_string_t *str, const char *strip) {
  const char *_strip = strip ? strip : " ";
  sz striplen = strlen(_strip);
  sz to_strip = 0;
  for (sz i = 0; i < str->length; i++) {
    sz idx = str->length - 1 - i;
    char c = str->c_str[idx];
    if (c_str_contains_char(_strip, striplen, c)) {
      to_strip++;
    } else {
      break;
    }
  }
  jdm_string_trim_right(str, to_strip);
}

void jdm_string_strip_right(jdm_string_t *str, const jdm_string_t *strip) {
  if (strip != JDM_NULL)
    jdm_string_strip_right_c_str(str, strip->c_str);
  else
    jdm_string_strip_right_c_str(str, JDM_NULL);
}

static void free_str_elem(u8 *elem) {
  jdm_string_destroy((jdm_string_t *)elem);
}

jdm_string_status_t jdm_string_split_c_str(const jdm_string_t *str,
                                           jdm_array_t **out,
                                           const char *split_on) {
  jdm_alloc_t *alloc = str->alloc;
  jdm_array_status_t as;
  jdm_string_status_t s;
  sz split_occurences;
  sz out_size;
  jdm_string_t *elem;
  jdm_array_t *res;

  split_occurences = jdm_string_count_occurences_c_str(str, split_on);
  out_size = split_occurences + 1;
  res = jdm_array_create((jdm_array_config_t){
      .alloc = alloc,
      .capacity = out_size,
      .element_size = sizeof(jdm_string_t *),
      .exp_factor = 2,
  });
  if (res == JDM_NULL) {
    return JDM_STRING_ERR_ARRAY;
  }

  jdm_string_t *copy = JDM_NULL;
  s = jdm_string_copy(str, &copy);
  char *token = strtok(copy->c_str, split_on);
  while (token) {
    elem = jdm_string_create(alloc, token);
    if (elem == JDM_NULL) {
      break;
    }
    as = jdm_array_add(res, (u8 *)&elem);
    if (as != JDM_ARRAY_OK) {
      jdm_string_destroy(elem);
      break;
    }
    token = strtok(JDM_NULL, split_on);
  }
  jdm_string_destroy(copy);

  if (elem == JDM_NULL || as != JDM_ARRAY_OK) {
    jdm_array_map(res, free_str_elem);
    jdm_array_destroy(res);
    res = JDM_NULL;
    return JDM_STRING_ERR_ARRAY;
  }

  *out = res;
  return JDM_STRING_OK;
}

jdm_string_status_t jdm_string_split(const jdm_string_t *str, jdm_array_t **out,
                                     const jdm_string_t *split_on) {
  return jdm_string_split_c_str(str, out, split_on->c_str);
}

jdm_string_status_t jdm_string_join_c_str(jdm_string_t **str,
                                          const jdm_array_t *in,
                                          const char *join_on) {
  if ((*str)->length > 0) {
    jdm_string_empty(*str);
  }

  sz in_size = jdm_array_size(in);
  jdm_string_t **s = jdm_alloc((*str)->alloc, sizeof(jdm_string_t *));
  for (sz i = 0; i < (in_size - 1); i++) {
    jdm_array_get_at(in, (u8 **)&s, i);
    jdm_string_append(str, *s);
    jdm_string_append_c_str(str, join_on);
  }
  jdm_string_append(str, *s);

  jdm_free((*str)->alloc, s);
  return JDM_STRING_OK;
}

jdm_string_status_t jdm_string_join(jdm_string_t **str, const jdm_array_t *in,
                                    const jdm_string_t *join_on) {
  return jdm_string_join_c_str(str, in, join_on->c_str);
}
