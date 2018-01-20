#pragma once

int timeslot_to_x(int id, dict_t *d, search_mode_t mode);
char *id_to_name(int id, dict_t *d, search_mode_t mode);
int id_to_index(int id, dict_t *d, search_mode_t mode);
int index_of_int(int target, int *array, int array_len);
void shuffle_array(int array[], int len);
void remove_index(int *array, int index, int *p_len);
void sort_seminars(seminar_t *seminars, int n);
int binary_search(int target, int array[], int len);
int id_to_id(dict_t *d, int id, search_mode_t IN_ID, search_mode_t OUT_ID);
int cmp_ints_reverse(const void *v1, const void *v2);
void remove_id(int id, int *array, int *p_len);