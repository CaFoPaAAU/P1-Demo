
#include "fitness.h"
#include "sp_types.h"
#include "IO.h"
#include "search.h"
#include "tables.h"
#include <time.h>

/* Highlevel functions for handling fitness scoring */
void get_fitness_score(table_t *t, dict_t *d, bool use_weights) {
	int i = 0;
	
	table_fitness_t *ft = calloc(1, sizeof(table_fitness_t));

	if (!ft)
		error_handler(NO_MEMORY);

	/* Mapping the data from table_t to table_fitness_t*/
	for (i = 0; i < d->n_groups; i++) {

		/* Copy of date from table input */
		ft->seminar_fit[i].id = i;
		ft->seminar_fit[i].presenter = t->seminars[i].presenter;
		ft->seminar_fit[i].opponent = t->seminars[i].opponent;
		ft->seminar_fit[i].external_supervisor = t->seminars[i].external_supervisor;
		ft->seminar_fit[i].room = t->seminars[i].room;
		ft->seminar_fit[i].timeslot = t->seminars[i].timeslot;

		/* Copy data by looking up ID and switch on it based on the required return ID*/
		ft->seminar_fit[i].presenter_subject = id_to_id(d, t->seminars[i].presenter, GROUP_S, SUBJECT_S);
		ft->seminar_fit[i].opp_subject = id_to_id(d, t->seminars[i].opponent, GROUP_S, SUBJECT_S);
		ft->seminar_fit[i].supervisor = id_to_id(d, t->seminars[i].presenter, GROUP_S, SUPERVISOR_S);
		ft->seminar_fit[i].co_supervisor = id_to_id(d, t->seminars[i].presenter, GROUP_S, CO_SUPERVISOR_S);
		ft->seminar_fit[i].day = timeslot_to_x(t->seminars[i].timeslot, d, DATE_S);
		ft->seminar_fit[i].month = timeslot_to_x(t->seminars[i].timeslot, d, MONTH_S);
		ft->seminar_fit[i].year = timeslot_to_x(t->seminars[i].timeslot, d, YEAR_S);
	}

	get_fitness_definitions(d, ft, use_weights);
	calc_fitness_score(d, ft);

	memcpy(t->fitness_summary.fitness_definitions, ft->fitness_definitons, sizeof(ft->fitness_definitons));
	t->fitness_summary.total = ft->score;

	free(ft);
}

void calc_fitness_score(dict_t *d, table_fitness_t *ft) {
	int i;
	double score = 0;

	Room_selection_score(d, ft);
	Person_day_utilization_score(d, ft);
	Optimal_time_slot_score(d, ft);
	Person_cost_score(d, ft);
	Room_cost_score(d, ft);
	Different_subjects_score(d, ft);
	Group_booking_different_day_score(d, ft);
	group_booking_spread_score(d, ft);
	person_overall_booking_spread_score(d, ft);
	person_daily_booking_spread_score(d, ft);
	total_seminar_length_score(d, ft);

	for (i = 0; i < TOTAL_SCORE_S; i++) {
		score += ft->fitness_definitons[i][SCORE_S];
	}

	ft->score = score;
}

table_fitness_t *get_benchmark(dict_t *d, bool use_weights) {

	/* Importing authentic data from Dianas table and getting the fitness score */
	table_fitness_t *benchmark_data = get_benchmark_data();
	get_benchmark_score(d, benchmark_data, use_weights);

	return benchmark_data;
}

void get_benchmark_score(dict_t *d, table_fitness_t *bd, bool use_weights) {
	int i;
	table_fitness_t *ft = calloc(1, sizeof(table_fitness_t));

	if (!ft)
		error_handler(NO_MEMORY);

	get_fitness_definitions(d, ft, use_weights);
	for (i = 0; i < d->n_groups; i++) {

		/* Copy of date from table input */
		ft->seminar_fit[i].id = i;
		ft->seminar_fit[i].presenter = bd->seminar_fit[i].presenter;
		ft->seminar_fit[i].opponent = bd->seminar_fit[i].opponent;
		ft->seminar_fit[i].external_supervisor = bd->seminar_fit[i].external_supervisor;
		ft->seminar_fit[i].room = bd->seminar_fit[i].room;
		ft->seminar_fit[i].timeslot = bd->seminar_fit[i].timeslot;

		/* Saving data by looking up referenced data (not coded yet due us of benchmark data) */
		ft->seminar_fit[i].presenter_subject = bd->seminar_fit[i].presenter_subject;
		ft->seminar_fit[i].opp_subject = bd->seminar_fit[i].opp_subject;
		ft->seminar_fit[i].supervisor = bd->seminar_fit[i].supervisor;
		ft->seminar_fit[i].co_supervisor = bd->seminar_fit[i].co_supervisor;
		ft->seminar_fit[i].day = bd->seminar_fit[i].day;
		ft->seminar_fit[i].month = bd->seminar_fit[i].month;
		ft->seminar_fit[i].year = bd->seminar_fit[i].year;
	}
	calc_fitness_score(d, ft);


	memcpy(bd->fitness_definitons, ft->fitness_definitons, sizeof(ft->fitness_definitons));
	bd->score = ft->score;

	free(ft);
}

table_fitness_t *get_benchmark_data() {
	int i;
	table_fitness_t *bd = calloc(1, sizeof(table_fitness_t));

	if (!bd)
		error_handler(NO_MEMORY);

	/* This the time table that was used for the actual seminar and serves as our benchmarking data */
	seminar_fitness_t input[N_GRPS] = {
		{ 1, 1, 3, 3, 1, 2, 1, 2, 1, 3, 13, 11, 2017 },{ 2, 2, 4, 3, 1, 3, 4, 2, 2, 3, 13, 11, 2017 },
	{ 3, 3, 5, 7, 2, 1, 5, 6, 3, 4, 13, 11, 2017 },{ 4, 4, 7, 7, 2, 2, 5, 6, 3, 1, 13, 11, 2017 },
	{ 5, 5, 1, 7, 2, 3, 5, 6, 4, 1, 13, 11, 2017 },{ 6, 6, 8, 7, 3, 4, 5, 3, 5, 1, 13, 11, 2017 },
	{ 7, 7, 2, 7, 3, 5, 8, 9, 1, 2, 13, 11, 2017 },{ 8, 8, 6, 7, 3, 6, 8, 9, 1, 5, 13, 11, 2017 },
	{ 9, 9, 14, 7, 4, 9, 10, 6, 6, 8, 14, 11, 2017 },{ 10, 10, 15, 7, 4, 10, 10, 6, 5, 7, 14, 11, 2017 },
	{ 11, 11, 13, 7, 4, 11, 10, 6, 5, 7, 14, 11, 2017 },{ 12, 12, 9, 3, 1, 12, 4, 9, 5, 6, 14, 11, 2017 },
	{ 13, 13, 10, 3, 1, 13, 4, 9, 7, 5, 14, 11, 2017 },{ 14, 14, 16, 7, 3, 12, 11, 12, 8, 9, 14, 11, 2017 },
	{ 15, 15, 17, 7, 3, 13, 13, 12, 7, 10, 14, 11, 2017 },{ 16, 16, 18, 7, 3, 14, 13, 12, 9, 10, 14, 11, 2017 },
	{ 17, 17, 12, 7, 5, 19, 14, 15, 10, 5, 15, 11, 2017 },{ 18, 18, 11, 7, 5, 20, 14, 15, 10, 5, 15, 11, 2017 },
	{ 19, 19, 22, 7, 1, 28, 16, 15, 8, 6, 16, 11, 2017 },{ 20, 20, 28, 7, 1, 29, 11, 17, 8, 2, 16, 11, 2017 },
	{ 21, 21, 23, 7, 1, 30, 16, 17, 8, 6, 16, 11, 2017 },{ 22, 22, 29, 7, 3, 27, 10, 6, 6, 3, 16, 11, 2017 },
	{ 23, 23, 26, 18, 3, 28, 10, 6, 6, 2, 16, 11, 2017 },{ 24, 24, 27, 18, 4, 33, 10, 9, 5, 2, 17, 11, 2017 },
	{ 25, 25, 31, 7, 4, 34, 13, 17, 2, 6, 17, 11, 2017 },{ 26, 26, 24, 7, 4, 35, 13, 17, 2, 5, 17, 11, 2017 },
	{ 27, 27, 19, 7, 4, 36, 13, 17, 2, 8, 17, 11, 2017 },{ 28, 28, 30, 7, 4, 37, 13, 17, 2, 5, 17, 11, 2017 },
	{ 29, 29, 21, 7, 4, 38, 19, 15, 3, 8, 17, 11, 2017 },{ 30, 30, 20, 21, 3, 41, 19, 20, 5, 8, 20, 11, 2017 },
	{ 31, 31, 25, 21, 3, 42, 1, 20, 6, 2, 20, 11, 2017 }
	};

	/* Saving the input array to the struct. The '-1' is for allingning the index correctly */
	for (i = 0; i < N_GRPS; i++) {
		bd->seminar_fit[i].id = input[i].id;
		bd->seminar_fit[i].presenter = input[i].presenter;
		bd->seminar_fit[i].opponent = input[i].opponent;
		bd->seminar_fit[i].external_supervisor = input[i].external_supervisor;
		bd->seminar_fit[i].room = input[i].room;
		bd->seminar_fit[i].timeslot = input[i].timeslot;
		bd->seminar_fit[i].supervisor = input[i].supervisor;
		bd->seminar_fit[i].co_supervisor = input[i].co_supervisor;
		bd->seminar_fit[i].presenter_subject = input[i].presenter_subject;
		bd->seminar_fit[i].opp_subject = input[i].opp_subject;
		bd->seminar_fit[i].day = input[i].day;
		bd->seminar_fit[i].month = input[i].month;
		bd->seminar_fit[i].year = input[i].year;
	}
	return bd;
}

void check_top_x(table_t * t, table_t * TopX, int n) {
	if (t->fitness_summary.total < TopX[n - 1].fitness_summary.total) {
		TopX[n - 1] = *t;
		qsort(TopX, n, sizeof(table_t), cmp_totalscore);
	}
}

/* Fitness scoring functions */
void Room_selection_score(dict_t *d, table_fitness_t *ft) {
	int i, j;
	double score = 0;

	for (i = 0; i < d->n_groups; i++) {
		int room_id = ft->seminar_fit[i].room;

		for (j = 0; j < d->n_rooms; j++) {
			if ((room_id == d->rooms[j].id) && d->rooms[j].type == AUDITORIUM) {
				score++;
			}
		}
	}
	ft->fitness_definitons[ROOM_SELECTION_SCORE_S][VALUE_S] = score;

	normalize_weight_score(ROOM_SELECTION_SCORE_S, ft);
}

void Person_day_utilization_score(dict_t *d, table_fitness_t *ft) {
	int i, j, person_id, booked_count, days_count, date;
	double avg = 0, a, b, c, score = 0;

	int person_booked_count[sizeof(d->n_persons) * 8];	
	int person_days_booked_count[sizeof(d->n_persons) * 8]; 

															
	for (j = 0; j < d->n_persons; j++) {
		booked_count = 0;
		days_count = 0;
		date = 0;
		person_id = d->persons[j].id;
		/* Inner loop on time table to count how many timeslots a persons has been booked */
		for (i = 0; i < d->n_groups; i++) {
			if ((person_id == ft->seminar_fit[i].supervisor || person_id == ft->seminar_fit[i].co_supervisor || person_id == ft->seminar_fit[i].external_supervisor)) {
				booked_count++;
				/* Counting how many days the person has been booked */
				if (ft->seminar_fit[i].day != date) {
					days_count++;
					date = ft->seminar_fit[i].day;
				}

			}
		}
		person_booked_count[j] = booked_count;
		person_days_booked_count[j] = days_count;
	}

	/* Calculation the score by taking the overall avg of each persons timeslot / days_booked utilization */
	for (i = 0; i < d->n_persons; i++) {
		a = person_booked_count[i];
		b = person_days_booked_count[i] * N_TIMESLOTS_DAY_S;
		if (b != 0) {
			c = (1 - a / b);
			avg += c;
		}
		else {
			avg = 0;
		}
	}
	score = avg / d->n_persons;

	ft->fitness_definitons[PERSON_DAY_UTILIZATION_S][VALUE_S] = score;

	normalize_weight_score(PERSON_DAY_UTILIZATION_S, ft);

}

void Optimal_time_slot_score(dict_t *d, table_fitness_t *ft) {
	int i, j = 0, k = 0;
	double score = 0;

	int off_timeslots[MAX_TIMESLOTS];

	/* Identifying all possible timeslots outside normal hours (08-16) */
	for (i = 0; i < MAX_TIMESLOTS; i++) {
		if (d->timeslots[i].begin_hour > OFF_HOUR_START_S) {
			off_timeslots[j++] = d->timeslots[i].id;
		}
	}

	/* Looping on each seminar and counting each booked timeslot outside normal hours */
	for (i = 0; i < d->n_groups; i++) {
		for (k = 0; k < j; k++) {
			if (off_timeslots[k] == ft->seminar_fit[i].timeslot) {
				score++;
			}
		}
	}
	ft->fitness_definitons[OPTIMAL_TIME_SLOT_SCORE_S][VALUE_S] = score;

	normalize_weight_score(OPTIMAL_TIME_SLOT_SCORE_S, ft);
}

void Person_cost_score(dict_t *d, table_fitness_t *ft) {
	int i, j, person_id, booked_count;
	double score = 0;

	int person_booked_count[sizeof(d->n_persons) * 8]; 

	/* Outer loop on each person */
	for (j = 0; j < d->n_persons; j++) {
		booked_count = 0;
		person_id = d->persons[j].id;
		/* Inner loop on time table to count how many timeslots a persons has been booked */
		for (i = 0; i < d->n_groups; i++) {
			if ((person_id == ft->seminar_fit[i].supervisor ||
				person_id == ft->seminar_fit[i].co_supervisor ||
				person_id == ft->seminar_fit[i].external_supervisor)) {
				booked_count++;
			}
		}
		person_booked_count[j] = booked_count;
	}

	/* Calculation the score by taking the overall avg of each persons timeslot / days_booked utilization  */
	for (i = 0; i < d->n_persons; i++) {
		score += d->persons[i].salery_per_timeslot * person_booked_count[i];
	}
	ft->fitness_definitons[PERSON_COST_S][VALUE_S] = score;

	normalize_weight_score(PERSON_COST_S, ft);
}

void Room_cost_score(dict_t *d, table_fitness_t *ft) {
	int i, j, score = 0;

	for (i = 0; i < d->n_groups; i++) {
		int room_id = ft->seminar_fit[i].room;

		for (j = 0; j < d->n_rooms; j++) {
			if ((room_id == d->rooms[j].id) && d->rooms[j].type == AUDITORIUM) {
				score += AUD_COST_S;
			}
			else if ((room_id == d->rooms[j].id) && d->rooms[j].type != AUDITORIUM) {
				score += SEMINAR_ROOM_COST_S;
			}
		}
	}

	ft->fitness_definitons[ROOM_COST_S][VALUE_S] = score;

	normalize_weight_score(ROOM_COST_S, ft);
}

void Different_subjects_score(dict_t *d, table_fitness_t *ft) {
	int i, score = 0;

	/* Outer loop on each presenter group */
	for (i = 0; i < d->n_groups; i++) {
		if (ft->seminar_fit[i].presenter_subject == ft->seminar_fit[i].opp_subject) {
			score++;
		}
	}

	ft->fitness_definitons[DIFFERENT_SUBJECTS_SCORE_S][VALUE_S] = score;

	normalize_weight_score(DIFFERENT_SUBJECTS_SCORE_S, ft);
}

void Group_booking_different_day_score(dict_t *d, table_fitness_t *ft) {
	int i, j, score = 0;

	int presenter_day[50]; 
	int opponent_day[50]; 

	/* Outer loop on each seminar */
	for (i = 0; i < d->n_groups; i++) {
		/* Inner loop on each group */
		for (j = 0; j < d->n_groups; j++) {
			if (ft->seminar_fit[i].opponent == d->groups[j].id) {
				opponent_day[j] = ft->seminar_fit[i].day;
			}
		}
	}

	/* Outer loop on each seminar */
	for (i = 0; i < d->n_groups; i++) {
		/* Inner loop on each group */
		for (j = 0; j < d->n_groups; j++) {
			if (ft->seminar_fit[i].presenter == d->groups[j].id) {
				presenter_day[j] = ft->seminar_fit[i].day;
			}
		}
	}

	for (i = 0; i < d->n_groups; i++) {
		if (presenter_day[i] == opponent_day[i]) {
			score++;
		}
	}
	ft->fitness_definitons[GROUP_BOOKING_DIFFERENT_DAY_SCORE_S][VALUE_S] = score;

	normalize_weight_score(GROUP_BOOKING_DIFFERENT_DAY_SCORE_S, ft);
}

void group_booking_spread_score(dict_t *d, table_fitness_t *ft) {
	int i, j, score = 0;

	int presenter_day[50]; 
	int opponent_day[50]; 

	/* Outer loop on each seminar */
	for (i = 0; i < d->n_groups; i++) {
		/* Inner loop on each group */
		for (j = 0; j < d->n_groups; j++) {
			if (ft->seminar_fit[i].opponent == d->groups[j].id) {
				opponent_day[j] = ft->seminar_fit[i].day;
			}
		}
	}

	/* Outer loop on each seminar */
	for (i = 0; i < d->n_groups; i++) {
		/* Inner loop on each group */
		for (j = 0; j < d->n_groups; j++) {
			if (ft->seminar_fit[i].presenter == d->groups[j].id) {
				presenter_day[j] = ft->seminar_fit[i].day;
			}
		}
	}

	for (i = 0; i < d->n_groups; i++) {
		if (abs(presenter_day[i] - opponent_day[i]) > 1) {
			score += abs(presenter_day[i] - opponent_day[i]);
		}
		else if (abs(presenter_day[i] - opponent_day[i]) == 0) {
			score++;
		}
	}

	ft->fitness_definitons[GROUP_BOOKING_SPREAD_S][VALUE_S] = score;

	normalize_weight_score(GROUP_BOOKING_SPREAD_S, ft);
}

void person_daily_booking_spread_score(dict_t *d, table_fitness_t *ft) {
	int i, j, days_count, date;
	double score = 0;
	int persons[40];

	/* Outer loop on each person */
	for (j = 0; j < d->n_persons; j++) {
		days_count = 0;
		date = 0;

		/* Inner loop on time table to count how many timeslots a persons has been booked. */
		for (i = 0; i < d->n_groups; i++) {
			if ((d->persons[j].id == ft->seminar_fit[i].supervisor ||
				d->persons[j].id == ft->seminar_fit[i].co_supervisor || 
				d->persons[j].id == ft->seminar_fit[i].external_supervisor)) {
				
				if (ft->seminar_fit[i].day != date) {
					days_count++;
					date = ft->seminar_fit[i].day;
				}

			}
		}
		persons[j] = days_count;
	}

	for (i = 0; i < d->n_persons; i++) {
		score += persons[i];
	}

	ft->fitness_definitons[PERSON_DAILY_BOOKING_SPREAD_S][VALUE_S] = score;

	normalize_weight_score(PERSON_DAILY_BOOKING_SPREAD_S, ft);
}

void person_overall_booking_spread_score(dict_t *d, table_fitness_t *ft) {
	int i, j, booked_count, days_count, date;
	double score = 0;

	double persons[50][3]; 

	/* Outer loop on each person */
	for (j = 0; j < d->n_persons; j++) {
		booked_count = 0;
		days_count = 0;
		date = 0;

		/* Inner loop on time table to count how many timeslots a persons has been booked */
		for (i = 0; i < d->n_groups; i++) {
			if ((d->persons[j].id == ft->seminar_fit[i].supervisor || 
				d->persons[j].id == ft->seminar_fit[i].co_supervisor || 
				d->persons[j].id == ft->seminar_fit[i].external_supervisor)) {

				booked_count++;
				persons[j][2] = ft->seminar_fit[i].timeslot;
				/* Counting how many days the person has been booked */
				if (ft->seminar_fit[i].day != date) {
					if (booked_count == 1) {
						persons[j][1] = ft->seminar_fit[i].timeslot;
					}
					days_count++;
					date = ft->seminar_fit[i].day;
				}

			}
		}
		persons[j][0] = booked_count;
	}

	for (i = 0; i < d->n_persons; i++) {
		if (persons[i][2] / N_TIMESLOTS_DAY_S != persons[i][1] / N_TIMESLOTS_DAY_S && persons[i][0] != 0) {
			score += ((persons[i][2] - persons[i][1]) / (N_TIMESLOTS_DAY_S * persons[i][0]));
		}
	}
	ft->fitness_definitons[PERSON_OVERALL_BOOKING_SPREAD_S][VALUE_S] = score;

	normalize_weight_score(PERSON_OVERALL_BOOKING_SPREAD_S, ft);

}

void total_seminar_length_score(dict_t *d, table_fitness_t *ft) {
	int j, date = 0, days_count = 0;

	/* Outer loop on each person */
	for (j = 0; j < d->n_groups; j++) {
		if (ft->seminar_fit[j].day != date) {
			days_count++;
			date = ft->seminar_fit[j].day;
		}
	}

	ft->fitness_definitons[TOTAL_SEMINAR_LENGTH_S][VALUE_S] = days_count;

	normalize_weight_score(TOTAL_SEMINAR_LENGTH_S, ft);
}

/* Supporting functions */
void get_fitness_definitions(dict_t *d, table_fitness_t *ft, bool use_weigths) {
	/* This array contain the defined range (min, max) of values  for each score can get and its weighting
	(Format: min, max, weight, actual value and score (it is preset to 9999) */
	int i;

	double fitness_definitions[11][5] = {
		{ 0, d->n_groups, 0.5, 0, 9999 },
	{ 0, 1 , 0.75, 0, 9999 },
	{ 0, N_OFF_HOUR_TIMESLOTS_S, 1, 0, 9999 },
	{ d->n_persons * 120 * 3,  d->n_persons * 200 * 3, 0.1, 0, 9999 },
	{ SEMINAR_ROOM_COST_S * d->n_persons, AUD_COST_S * d->n_persons, 0.1, 0, 9999 },
	{ 0, d->n_groups, 0.85, 0, 9999 },
	{ 0, d->n_groups, 0.4, 0, 9999 },
	{ 0, 110, 0.25, 0, 9999 },
	{ 20, 150, 0.8, 0, 9999 },
	{ 0, 40, 0.3, 0, 9999 },
	{ 4, 7, 1, 0, 9999 }
	};

	memcpy(ft->fitness_definitons, fitness_definitions, sizeof(fitness_definitions));

	if (use_weigths == 0) {
		for (i = 0; i < TOTAL_SCORE_S; i++) {
			ft->fitness_definitons[i][WEIGHT_S] = 1;
		}
	}
}

void normalize_weight_score(int i, table_fitness_t *ft) {
	/* The 'VALUE_S' is normalizied and weighted and then saved to 'SCORE_S' */
	ft->fitness_definitons[i][SCORE_S] = ft->fitness_definitons[i][WEIGHT_S] * 
		((ft->fitness_definitons[i][VALUE_S] - ft->fitness_definitons[i][MIN_RANGE_S]) / 
		(ft->fitness_definitons[i][MAX_RANGE_S] - ft->fitness_definitons[i][MIN_RANGE_S]));
}

table_t *prepare_top_x(int n) {
	int i;
	table_t *TopX = calloc(n, sizeof(table_t));

	if (!TopX)
		error_handler(NO_MEMORY);

	for (i = 0; i < n; i++) {
		TopX[i].fitness_summary.total = i + 10;
	}
	return TopX;
}

int cmp_totalscore(const void *v1, const void *v2) {
	table_t * t1 = (table_t *)v1;
	table_t * t2 = (table_t *)v2;

	if (t2->fitness_summary.total > t1->fitness_summary.total) {
		return -1;
	}

	return 1;
}

void create_table_population(dict_t *d, int population, bool is_weighted, bool use_bookings) {
	table_t *top_x = prepare_top_x(10);
	table_fitness_t *benchmark = get_benchmark(d, is_weighted);

	int end_t = 0, start_t = clock();
	double diff = 0, besttime = 0;
	int tabels = 0, gen = 0;

	int x;

	for (x = 0; x < 10; x++) {
		int i, n_tables = population / 10;
		double min_score = 500, max_score = 0;

		for (i = 0; i < n_tables; i++) {
			table_t *t = create_table(d, true, false, use_bookings);
			if (t != NULL) {

				get_fitness_score(t, d, is_weighted);

				tabels++;
				if (t->fitness_summary.total > max_score) {
					max_score = t->fitness_summary.total;
				}

				if (t->fitness_summary.total < min_score) {
					min_score = t->fitness_summary.total;
					end_t = clock();
					besttime = (double)end_t - start_t;
					gen = tabels;

				}
				if (min_score > 100) {
					print_fitness_details(top_x, benchmark);
				}

				check_top_x(t, top_x, 10);
				free(t);
			}
		}
		end_t = clock();
		diff = (double)end_t - start_t;

		printf(" %-12d tables took \t%5.1lf min \tWorst batch score: \t%0.2lf \t Best batch" 
			"score: %0.2lf\tBenchmark: %0.2lf | It was found after %5.1lf min. as table # %d  \n", 
			tabels, 
			diff / 60000, 
			max_score > 12 ? 12.0 : max_score, 
			min_score, 
			benchmark->score, 
			besttime / 60000, 
			gen
		);

	}
	printf("\n The best fit was found after %5.1lf min. as table # %d  \n", besttime / 60000, gen);

	print_table_full(top_x, d, d->n_groups);
	print_fitness_details(top_x, benchmark);


	free(benchmark);
	free(top_x);
}

void print_fitness_details(table_t *table, table_fitness_t *bd) {

	printf("\n\n--------------------------------------------------------------------------\n");
	printf(" SCORES                              Benchmark       Result       Weight");
	printf("\n--------------------------------------------------------------------------\n");
	printf("ROOM SELECTION SCORE                %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[0][SCORE_S], 
		table->fitness_summary.fitness_definitions[0][SCORE_S], 
		table->fitness_summary.fitness_definitions[0][WEIGHT_S]);
	printf("PERSON DAY UTILIZATION              %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[1][SCORE_S], 
		table->fitness_summary.fitness_definitions[1][SCORE_S], 
		table->fitness_summary.fitness_definitions[1][WEIGHT_S]);
	printf("OPTIMAL TIME SLOT SCORE             %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[2][SCORE_S], 
		table->fitness_summary.fitness_definitions[2][SCORE_S], 
		table->fitness_summary.fitness_definitions[2][WEIGHT_S]);
	printf("PERSON COST                         %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[3][SCORE_S], 
		table->fitness_summary.fitness_definitions[3][SCORE_S], 
		table->fitness_summary.fitness_definitions[3][WEIGHT_S]);
	printf("ROOM COST                           %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[4][SCORE_S], 
		table->fitness_summary.fitness_definitions[4][SCORE_S], 
		table->fitness_summary.fitness_definitions[4][WEIGHT_S]);
	printf("DIFFERENT SUBJECTS SCORE            %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[5][SCORE_S], 
		table->fitness_summary.fitness_definitions[5][SCORE_S], 
		table->fitness_summary.fitness_definitions[5][WEIGHT_S]);
	printf("GROUP BOOKING DIFFERENT DAY SCORE   %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[6][SCORE_S], 
		table->fitness_summary.fitness_definitions[6][SCORE_S], 
		table->fitness_summary.fitness_definitions[6][WEIGHT_S]);
	printf("GROUP BOOKING SPREAD                %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[7][SCORE_S], 
		table->fitness_summary.fitness_definitions[7][SCORE_S], 
		table->fitness_summary.fitness_definitions[7][WEIGHT_S]);
	printf("PERSON DAILY BOOKING SPREAD         %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[8][SCORE_S], 
		table->fitness_summary.fitness_definitions[8][SCORE_S], 
		table->fitness_summary.fitness_definitions[8][WEIGHT_S]);
	printf("PERSON OVERALL BOOKING SPREAD       %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[9][SCORE_S], 
		table->fitness_summary.fitness_definitions[9][SCORE_S], 
		table->fitness_summary.fitness_definitions[8][WEIGHT_S]);
	printf("TOTAL SEMINAR LENGTH                %8.2lf      %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[10][SCORE_S], 
		table->fitness_summary.fitness_definitions[10][SCORE_S], 
		table->fitness_summary.fitness_definitions[10][WEIGHT_S]);
	printf("--------------------------------------------------------------------------\n");
	printf("TOTAL SCORE                         %8.2lf      %8.2lf\n", 
		bd->score, table->fitness_summary.total);
	printf("--------------------------------------------------------------------------\n");
	printf("\n ACTUAL VALUES                       Benchmark       Result");
	printf("\n------------------------------------------------------------\n");
	printf("ROOM SELECTION SCORE                %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[0][VALUE_S], 
		table->fitness_summary.fitness_definitions[0][VALUE_S]);
	printf("PERSON DAY UTILIZATION              %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[1][VALUE_S], 
		table->fitness_summary.fitness_definitions[1][VALUE_S]);
	printf("OPTIMAL TIME SLOT SCORE             %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[2][VALUE_S], 
		table->fitness_summary.fitness_definitions[2][VALUE_S]);
	printf("PERSON COST                         %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[3][VALUE_S], 
		table->fitness_summary.fitness_definitions[3][VALUE_S]);
	printf("ROOM COST                           %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[4][VALUE_S], 
		table->fitness_summary.fitness_definitions[4][VALUE_S]);
	printf("DIFFERENT SUBJECTS SCORE            %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[5][VALUE_S], 
		table->fitness_summary.fitness_definitions[5][VALUE_S]);
	printf("GROUP BOOKING DIFFERENT DAY SCORE   %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[6][VALUE_S], 
		table->fitness_summary.fitness_definitions[6][VALUE_S]);
	printf("GROUP BOOKING SPREAD                %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[7][VALUE_S], 
		table->fitness_summary.fitness_definitions[7][VALUE_S]);
	printf("PERSON DAILY BOOKING SPREAD         %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[8][VALUE_S], 
		table->fitness_summary.fitness_definitions[8][VALUE_S]);
	printf("PERSON OVERALL BOOKING SPREAD       %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[9][VALUE_S], 
		table->fitness_summary.fitness_definitions[9][VALUE_S]);
	printf("TOTAL SEMINAR LENGTH                %8.2lf      %8.2lf\n", 
		bd->fitness_definitons[10][VALUE_S], 
		table->fitness_summary.fitness_definitions[10][VALUE_S]);
	printf("------------------------------------------------------------\n");
}














