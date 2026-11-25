#ifndef JDM_STRING_H
#define JDM_STRING_H

#include <jdm-alloc.h>
#include <jdm-array.h>
#include <jdm-defs.h>

typedef enum {
  JDM_STRING_OK = 0,
  JDM_STRING_ERR_ALLOC,
  JDM_STRING_ERR_BAD_IDX,
  JDM_STRING_ERR_NOT_FOUND,
  JDM_STRING_ERR_OUT_OF_RANGE,
  JDM_STRING_ERR_OVERFLOW,
  JDM_STRING_ERR_ARRAY,
} jdm_string_status_t;

typedef struct jdm_string_t jdm_string_t;

jdm_string_t *jdm_string_create_empty(jdm_alloc_t *alloc);
jdm_string_t *jdm_string_create(jdm_alloc_t *alloc, const char *c_str);
void jdm_string_destroy(jdm_string_t *str);
sz jdm_string_get_length(jdm_string_t *str);
sz jdm_string_get_capacity(jdm_string_t *str);
char *jdm_string_get_c_str(jdm_string_t *str);
jdm_string_status_t jdm_string_copy(const jdm_string_t *str,
                                    jdm_string_t **out);
void jdm_string_empty(jdm_string_t *str);
void jdm_string_reverse(jdm_string_t *str);
b8 jdm_string_is_empty(const jdm_string_t *str);
s8 jdm_string_compare(const jdm_string_t *str1, const jdm_string_t *str2);
jdm_string_status_t jdm_string_trim_capacity(jdm_string_t **str, sz *out);
jdm_string_status_t jdm_string_append_c_str(jdm_string_t **str,
                                            const char *append);
jdm_string_status_t jdm_string_append(jdm_string_t **str,
                                      const jdm_string_t *append);
jdm_string_status_t jdm_string_prepend_c_str(jdm_string_t **str,
                                             const char *prepend);
jdm_string_status_t jdm_string_prepend(jdm_string_t **str,
                                       const jdm_string_t *prepend);
jdm_string_status_t jdm_string_insert_c_str_at(jdm_string_t **str, sz idx,
                                               const char *insert);
jdm_string_status_t jdm_string_insert_at(jdm_string_t **str, sz idx,
                                         const jdm_string_t *insert);
jdm_string_status_t jdm_string_replace_c_str_at(jdm_string_t **str, sz idx,
                                                sz len_to_replace,
                                                const char *replace);
jdm_string_status_t jdm_string_replace_at(jdm_string_t **str, sz idx,
                                          sz len_to_replace,
                                          const jdm_string_t *replace);
jdm_string_status_t jdm_string_get_at(const jdm_string_t *str, sz idx,
                                      char *out);
jdm_string_status_t jdm_string_find_idx_char(const jdm_string_t *str,
                                             const char elem, sz *out);
jdm_string_status_t jdm_string_find_idx_c_str(const jdm_string_t *str,
                                              const char *c_substr, sz *out);
jdm_string_status_t jdm_string_find_idx(const jdm_string_t *str,
                                        const jdm_string_t *substr, sz *out);
b8 jdm_string_contains_char(const jdm_string_t *str, const char elem);
b8 jdm_string_contains_c_str(const jdm_string_t *str, const char *c_substr);
b8 jdm_string_contains(const jdm_string_t *str, const jdm_string_t *substr);
sz jdm_string_count_occurences_char(const jdm_string_t *str, const char elem);
sz jdm_string_count_occurences_c_str(const jdm_string_t *str,
                                     const char *c_substr);
sz jdm_string_count_occurences(const jdm_string_t *str,
                               const jdm_string_t *substr);
jdm_string_status_t jdm_string_replace_first_c_str(jdm_string_t **str,
                                                   const char *c_substr,
                                                   const char *replace);
jdm_string_status_t jdm_string_replace_first(jdm_string_t **str,
                                             const jdm_string_t *substr,
                                             const jdm_string_t *replace);
jdm_string_status_t jdm_string_replace_all_c_str(jdm_string_t **str,
                                                 const char *c_substr,
                                                 const char *replace);
jdm_string_status_t jdm_string_replace_all(jdm_string_t **str,
                                           const jdm_string_t *substr,
                                           const jdm_string_t *replace);
jdm_string_status_t jdm_string_remove_first_c_str(jdm_string_t **str,
                                                  const char *c_substr);
jdm_string_status_t jdm_string_remove_first(jdm_string_t **str,
                                            const jdm_string_t *substr);
jdm_string_status_t jdm_string_remove_all_c_str(jdm_string_t **str,
                                                const char *c_substr);
jdm_string_status_t jdm_string_remove_all(jdm_string_t **str,
                                           const jdm_string_t *substr);
b8 jdm_string_is_digit(const jdm_string_t *str);
b8 jdm_string_is_alpha(const jdm_string_t *str);
b8 jdm_string_is_alphanum(const jdm_string_t *str);
b8 jdm_string_starts_with_c_str(const jdm_string_t *str, const char* c_substr);
b8 jdm_string_starts_with(const jdm_string_t *str, const jdm_string_t* substr);
b8 jdm_string_ends_with_c_str(const jdm_string_t *str, const char* c_substr);
b8 jdm_string_ends_with(const jdm_string_t *str, const jdm_string_t* substr);
void jdm_string_to_upper(jdm_string_t *str);
void jdm_string_to_lower(jdm_string_t *str);
void jdm_string_trim_left(jdm_string_t *str, sz n);
void jdm_string_trim_right(jdm_string_t *str, sz n);
void jdm_string_strip_left_c_str(jdm_string_t *str, const char *strip);
void jdm_string_strip_left(jdm_string_t *str, const jdm_string_t *strip);
void jdm_string_strip_right_c_str(jdm_string_t *str, const char *strip);
void jdm_string_strip_right(jdm_string_t *str, const jdm_string_t *strip);

#endif // JDM_STRING_H
