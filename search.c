/**
*	search.c
*	
*	Search and delete operations related to arrays
*/

#include "sp_types.h"
#include "search.h"
#include "IO.h"

int timeslot_to_x(int id, dict_t *d, search_mode_t mode) {

	int index, res = -1;

	index = id_to_index(id, d, TIMESLOT_S);

	if (index == -1)
		error_handler(INVALID_ID);

	switch (mode) {
	case MONTH_S:
		res = d->timeslots[index].month;
		break;
	case YEAR_S:
		res = d->timeslots[index].year;
		break;
	case WEEKDAY_S:
		res = d->timeslots[index].weekday;
		break;
	case DATE_S:
		res = d->timeslots[index].date;
		break;
	default:
		error_handler(BAD_INPUT);
		break;
	}

	return res;
}

char *id_to_name(int id, dict_t *d, search_mode_t mode) {

	int i;

	switch (mode) {
	case EXTERNAL_SUPERVISOR_S:
	case CO_SUPERVISOR_S:
	case SUPERVISOR_S:
	case PERSON_S:
		for (i = 0; i < d->n_persons; i++)
			if (d->persons[i].id == id)
				return d->persons[i].name;
		break;
	case ROOM_S:
		for (i = 0; i < d->n_rooms; i++)
			if (d->rooms[i].id == id)
				return d->rooms[i].name;
		break;
	case GROUP_S:
		for (i = 0; i < d->n_groups; i++)
			if (d->groups[i].id == id)
				return d->groups[i].name;
		break;
	case SUBJECT_S:
		for (i = 0; i < d->n_subjects; i++)
			if (d->subjects[i].id == id)
				return d->subjects[i].name;
		break;
	case TIMESLOT_S:
		for (i = 0; i < d->n_timeslots; i++)
			if (d->timeslots[i].id == id)
				return d->timeslots[i].name;
	
	default:
		break;
	}

	return "None";
}

int id_to_index(int id, dict_t *d, search_mode_t mode) {

	int i;

	switch (mode) {
	
	case EXTERNAL_SUPERVISOR_S:
	case CO_SUPERVISOR_S:
	case SUPERVISOR_S:
	case PERSON_S:
		for (i = 0; i < d->n_persons; i++)
			if (d->persons[i].id == id)
				return i;
		break;
	case ROOM_S:
		for (i = 0; i < d->n_rooms; i++)
			if (d->rooms[i].id == id)
				return i;
		break;
	case GROUP_S:
		for (i = 0; i < d->n_groups; i++)
			if (d->groups[i].id == id)
				return i;
		break;
	case SUBJECT_S:
		for (i = 0; i < d->n_subjects; i++)
			if (d->subjects[i].id == id)
				return i;
		break;
	case TIMESLOT_S:
		for (i = 0; i < d->n_timeslots; i++)
			if (d->timeslots[i].id == id)
				return i;
		break;
	
	default:
		break;	
	}
	return -1;
}

int index_of_int(int target, int *array, int array_len ) {

	int i;

	if(target < 0)
		error_handler(UNEXPECTED_VALUE);

	for (i = 0; i < array_len; i++)
		if (target == array[i])
			return i;

	return -1;
}

void shuffle_array(int array[], int len) {
	int i, j, tmp;

	for (i = 0; i < len; i++) {

		j = rand() % len;
		tmp = array[j];
		array[j] = array[i];
		array[i] = tmp;
	}
}

int cmp_seminar_timeslot(const void *v1, const void *v2) {
	seminar_t *s1 = (seminar_t *)v1;
	seminar_t *s2 = (seminar_t *)v2;

	return s1->timeslot - s2->timeslot;
}

void sort_seminars(seminar_t *seminars, int n) {
	qsort(seminars, n, sizeof(seminar_t), cmp_seminar_timeslot);
}

int cmp_ints_reverse(const void *v1, const void *v2) {
	return *(int *)v2 - *(int *)v1;
}

void remove_index(int *array, int index, int *p_len) {

	if (index < 0)
		error_handler(ACCESS_VIOLATION);

	array[index] = array[*p_len - 1];
	array[*p_len - 1] = -1;
	/* set index to -1 for debugging purposes */
	*p_len -= 1;
}

void remove_id(int id, int *array, int *p_len) {

	int i;

	i = index_of_int(id, array, *p_len);

	if (i != -1) {
		array[i] = array[*p_len - 1];
		array[*p_len - 1] = -1;
		*p_len -= 1;
	}
}

int binary_search(int target, int array[], int len) {

	int low, mid, high;

	high = len - 1;
	low = 0;

	while (low <= high) {

		mid = (high + low) / 2;

		if (array[mid] == target)
			return mid;

		else if (array[mid] < target)
			low = mid + 1;

		else
			high = mid - 1;
	}

	return -1;
}

int id_to_id(dict_t *d, int id, search_mode_t IN_ID, search_mode_t OUT_ID) {

	int out_id, index;

	if (IN_ID == GROUP_S) {

		index = id_to_index(id, d, GROUP_S);

		switch (OUT_ID) {
		case SUBJECT_S:
			out_id = d->groups[index].subject;
			break;
		case SUPERVISOR_S:
			out_id = d->groups[index].supervisor;
			break;
		case CO_SUPERVISOR_S:
			out_id = d->groups[index].co_supervisor;
			break;
		default:
			error_handler(BAD_INPUT);
			break;
		}
	}
	return out_id;
}


