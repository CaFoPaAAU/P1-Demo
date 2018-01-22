/**
*	tables.c
*
*	Functions for creating valid quasi-random seminar tables
*/

#include <stdlib.h>
#include "sp_types.h"
#include "search.h"
#include "IO.h"
#include "tables.h"


int supervisors_available(int group, int timeslot, dict_t *d) {

	int sup, co_sup, sup_booked, co_sup_booked;

	sup = id_to_id(d, group, GROUP_S, SUPERVISOR_S);
	co_sup = id_to_id(d, group, GROUP_S, CO_SUPERVISOR_S);

	sup_booked = d->person_bookings_slots[timeslot * d->n_persons + sup];
	co_sup_booked = d->person_bookings_slots[timeslot * d->n_persons + co_sup];

	return !sup_booked && !co_sup_booked;
}

int external_supervisor_available(int id, int timeslot, dict_t *d) {

	int is_booked;

	is_booked = d->person_bookings_slots[timeslot * d->n_persons + id];

	return !is_booked;

}

int room_available(int room_id, int timeslot, dict_t *d) {

	int is_booked;

	is_booked = d->room_bookings_slots[timeslot * d->n_rooms + room_id];

	return !is_booked;
}


slot_t *get_slots(dict_t *d, int *n_slots, bool use_bookings) {

	/* avail groups, rooms, external supervisor */
	slot_t *slots = (slot_t *)calloc(N_SLOTS, sizeof(slot_t));

	if (!slots)
		error_handler(NO_MEMORY);

	*n_slots = N_SLOTS;
	int s, i;
	/* ---- SETUP ARRAYS ----  */
	for (s = 0; s < N_SLOTS; s++) {
		/* id */
		slots[s].id = s + 1;
		/* groups */
		slots[s].n_groups = 0;
		for (i = 0; i < d->n_groups; i++) {
			/* check supervisor and co-supervisor bookings */
			if (!use_bookings || supervisors_available(d->groups[i].id, s, d)) {
				slots[s].groups[i] = d->groups[i].id;
				slots[s].n_groups += 1;
			}
			else {
				/* external supervisors available for slot */
				slots[s].groups[i] = -1;
			}
			slots[s].opps[i] = d->groups[i].id;
			slots[s].n_opps = slots[s].n_opps < 0 ? 1 : slots[s].n_opps + 1;
		}
		/* rooms */
		slots[s].n_rooms = 0;
		for (i = 0; i < d->n_rooms; i++) {
			/* check room bookings */
			if (!use_bookings || room_available(d->rooms[i].id, s, d)) {
				slots[s].rooms[i] = d->rooms[i].id;
				slots[s].n_rooms += 1;
			}
			else {
				slots[s].rooms[i] = -1;
			}
		}
		/* external supervisors */
		slots[s].n_pv = 0;

		for (i = 0; i < d->n_persons; i++) {
			if (d->persons[i].role & 4) {
				if (!use_bookings || external_supervisor_available(d->persons[i].id, s, d)) {
					slots[s].n_pv += 1;
					slots[s].pv[slots[s].n_pv - 1] = d->persons[i].id;
				}
			}
		}

		/* ---- REMOVE SLOT IF NO AVAILABLE PV CONSULTANTS OR ROOMS ---- */
		if (!slots[s].n_rooms || !slots[s].n_pv || slots[s].n_groups < 2) {
			slots[s] = slots[*n_slots - 1];
			*n_slots -= 1;
		}
		qsort(slots[s].groups, N_GRPS, sizeof(int), cmp_ints_reverse);
		qsort(slots[s].rooms, N_ROOMS, sizeof(int), cmp_ints_reverse);
	}

	return slots;
}

table_t *create_table(dict_t *d, bool sort, bool print, bool use_bookings) {

	int s, i, start,
		n_slots, last_grp;

	bool valid = true;

	/* get time slots with arrays of available groups,
	rooms, external supervisor */
	slot_t *slots = get_slots(d, &n_slots, use_bookings);

	table_t *table = (table_t *)calloc(1, sizeof(table_t));

	if (!table)
		error_handler(NO_MEMORY);

	seminar_t *seminars = table->seminars;

	int opp_pool[N_GRPS];
	int n_opp_pool = 0;
	/* array for overall opponent pool */
	for (i = 0; i < N_GRPS; i++) {
		opp_pool[i] = d->groups[i].id;
		n_opp_pool += 1;
	}
	/* random starting point */
	i = rand() % N_GRPS;
	start = i;
	/* determine last index/group of iteration */
	last_grp = i - 1 >= 0 ? i - 1 : N_GRPS - 1;
	/* ---- MAIN LOOP ----
	*/
	do {
		/* control variables */
		int found_slot = false,	/* breaks inner loop if true */
			i_pres = 0,			/* index of presenter */
			tgt_opp = 0,		/* if > 0 will do targeted search for opponent */
			i_tgt_slot = -1,	/* index of target opponent in current slot */
			opp_id = -1,		/* opponent id */
			pres_id = 0,		/* presenter id */
			stuck = 0;			/* variable to keep track of # of failed attempts */

		pres_id = d->groups[i].id;
		/* s is random slot index */
		s = rand() % n_slots;

		while (!found_slot && valid) {

			stuck += 1;
			/* break loop if max number of attempts exceeded */
			if (stuck > MAX_ATTEMPTS)
				valid = false;
			/* id of presenter */
			pres_id = d->groups[i].id;
			/* check if presenter is available this slot */
			i_pres = index_of_int(pres_id, slots[s].groups, slots[s].n_groups);
			/*  if presenter is available, proceed with this slot  */
			if (i_pres != -1 && is_valid_slot(&slots[s])) {
				/*make sure the last group does not get stuck without opp */
				if (n_opp_pool == 2) {
					/* locals for scope */
					int index_last_grp;
					/* check if last group is in opponent pool */
					index_last_grp = index_of_int(last_grp, opp_pool, n_opp_pool);

					if (index_last_grp != -1) {
						tgt_opp = index_last_grp == 1 ? opp_pool[1] : opp_pool[0];
						/* get index of target in the current slot */
						i_tgt_slot = index_of_int(tgt_opp, slots[s].opps, slots[s].n_opps);
						/* check if oponent is available */
						found_slot = -1 < i_tgt_slot;
					}
				}
			
			}

			if (i_pres != -1) {
				opp_id = find_opponent(pres_id, &slots[s], i_tgt_slot, opp_pool, &n_opp_pool);
			}

			if (opp_id != -1) {
				found_slot = true;
			}
			else {
				/* try next slot */
				s = s == n_slots - 1 ? 0 : s + 1;
			}
		}
		/* save opponent to table */
		seminars[i].opponent = opp_id;
		/* save presenter to table */
		seminars[i].presenter = d->groups[i].id;
		/* pick room and remove from current slot */
		int rnd_i = rand() % slots[s].n_rooms;
		/* save room to table */
		seminars[i].room = slots[s].rooms[rnd_i];
		/* remove room from slot */
		remove_index(slots[s].rooms, rnd_i, &slots[s].n_rooms);
		/* pick random pv consultant and remove from this slot */
		rnd_i = rand() % slots[s].n_pv;
		seminars[i].external_supervisor = slots[s].pv[rnd_i];

		remove_index(slots[s].pv, rnd_i, &slots[s].n_pv);
		/* ---- REMOVE SLOT IF NO AVAILABLE GROUPS, PV CONSULTANTS OR ROOMS ---- */
		if (!slots[s].n_rooms || !slots[s].n_pv || !slots[s].n_groups || !slots[s].n_opps) {
			slots[s] = slots[n_slots - 1];
			n_slots -= 1;
		}

		seminars[i].id = i;
		seminars[i].timeslot = slots[s].id + 0;

		/* make seminar for next group */
		i = i == N_GRPS - 1 ? 0 : i + 1;

	} while (i != start && valid);

	/* exit sequence, sort and print table (if rquested) */
	if (sort && valid) 
		sort_seminars(seminars, N_GRPS);

	/* double check that table is valid */
	if (valid) 
		valid = table_is_valid(table, d);

	if (print && valid) 
		print_table_full(table, d, N_GRPS);
	
	free(slots);

	if (!valid)
		free(table);

	return valid ? table : NULL;
}

int is_valid_slot(slot_t *sp) {

	return	sp->n_groups > 0 && sp->n_opps > 0 &&
			sp->n_pv > 0 && sp->n_rooms > 0;
}

int find_opponent(int pres_id, slot_t *p_slot, int i_target, int opp_pool[], int *p_n_opps) {

	int opp_id, i_opp_pool, g, start;

	bool found_opp = false;

	/*	g is starting point for group index
		if no specific target choose a random
		starting point
	*/
	g = i_target > -1 ? i_target : rand() % p_slot->n_groups;
	start = g;

	do {
		opp_id = p_slot->opps[g];
		/*  check if opponent id is same as presenter */
		if (opp_id > -1 && opp_id != pres_id) {

			i_opp_pool = index_of_int(opp_id, opp_pool, *p_n_opps);

			if (i_opp_pool != -1) {
				/* remove IDs from arrays */
				remove_id(opp_id, opp_pool, p_n_opps);
				remove_id(pres_id, p_slot->groups, &(p_slot->n_groups));
				remove_id(pres_id, p_slot->opps, &(p_slot->n_opps));
				remove_id(opp_id, p_slot->groups, &(p_slot->n_groups));
				remove_id(opp_id, p_slot->opps, &(p_slot->n_opps));
				/* found opponent */
				found_opp = true;
			}
		}
		g = g == p_slot->n_opps - 1 ? 0 : g + 1;
		/* loop while no opponent or we are not back to the start */
	} while (!found_opp && start != g);

	return found_opp ? opp_id : -1;
}

int table_is_valid(table_t *table, dict_t *d) {

	seminar_t *sp = table->seminars;

	int i, j, k, m,
		sup1, co_sup1, 
		ext_sup1, sup2, 
		co_sup2, ext_sup2;

	int persons_1[3];
	int persons_2[3];

	for (j = 0; j < d->n_groups; j++) {
		
		for (i = 0; i < d->n_groups; i++) {
			
			if (sp[i].timeslot == sp[j].timeslot && i != j) {
				
				sup1 = id_to_id(d, sp[j].presenter, GROUP_S, SUPERVISOR_S);
				co_sup1 = id_to_id(d, sp[j].presenter, GROUP_S, CO_SUPERVISOR_S);
				ext_sup1 = sp[j].external_supervisor;
				
				if (sup1 == co_sup1 || sup1 == ext_sup1 || co_sup1 == ext_sup1)
					return false;

				persons_1[0] = sup1;
				persons_1[1] = co_sup1;
				persons_1[2] = ext_sup1;

				sup2 = id_to_id(d, sp[i].presenter, GROUP_S, SUPERVISOR_S);
				co_sup2 = id_to_id(d, sp[i].presenter, GROUP_S, CO_SUPERVISOR_S);
				ext_sup2 = sp[i].external_supervisor;

				persons_2[0] = sup2;
				persons_2[1] = co_sup2;
				persons_2[2] = ext_sup2;

				for (k = 0; k < 3; k++) {
					for (m = 0; m < 3; m++) {
						if (persons_1[k] == persons_2[m]) {
							return false;
						}
					}
				}
				if (sp[i].room == sp[j].room || sp[i].opponent == sp[j].presenter || 
					sp[i].presenter == sp[j].opponent || sp[i].opponent == sp[j].opponent || 
					sp[i].presenter == sp[j].presenter) {
						return false;
				}
			}
		}
	}

	return true;
}



void generate_n_rand_tables(dict_t *d, int n) {

	table_t *t;

	while (n > 0) {

		t = create_table(d, DO_SORT, !DO_PRINT, USE_BOOKINGS);
		if (t) {
			print_table_full(t, d, N_GRPS);
			free(t);
			n--;
		}
	}
}