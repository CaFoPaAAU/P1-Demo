#pragma once

#include <stdbool.h>
#include "sp_types.h"

typedef enum CREATE_TABLE_ARGS {
	USE_BOOKINGS = 1,
	DO_PRINT = 1,
	DO_SORT = 1,
}CREATE_TABLE_ARGS;

typedef enum TABLE_CONSTS {
	MAX_ATTEMPTS = 31,
	N_SLOTS = 56
}TABLE_CONSTS;

typedef struct slot_t {
	int id;

	int pv[N_PV];
	int n_pv;

	int rooms[N_ROOMS];
	int n_rooms;

	int groups[N_GRPS];
	int n_groups;

	int opps[N_GRPS];
	int n_opps;
}slot_t;

table_t *create_table(dict_t *d, bool sort, bool print, bool use_bookings);
int find_opponent(int pres_id, slot_t *p_slot, int tgt, int opp_pool[], int *p_n_opps);
slot_t *get_slots(dict_t *d, int *n_slots, bool use_bookings);
int external_supervisor_available(int person_id, int timeslot, dict_t *d);
int supervisors_available(int group, int timeslot, dict_t *d);
int room_available(int room_id, int timeslot, dict_t *d);
int is_valid_slot(slot_t *sp);
int table_is_valid(table_t *table, dict_t *d);
void generate_n_rand_tables(dict_t *d, int n);