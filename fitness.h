#pragma once
#include "sp_types.h"



/* Enums */
typedef enum fitness_criterias {
	ROOM_SELECTION_SCORE_S,
	PERSON_DAY_UTILIZATION_S,
	OPTIMAL_TIME_SLOT_SCORE_S,
	PERSON_COST_S,
	ROOM_COST_S,
	DIFFERENT_SUBJECTS_SCORE_S,
	GROUP_BOOKING_DIFFERENT_DAY_SCORE_S,
	GROUP_BOOKING_SPREAD_S,
	PERSON_DAILY_BOOKING_SPREAD_S,
	PERSON_OVERALL_BOOKING_SPREAD_S,
	TOTAL_SEMINAR_LENGTH_S,
	TOTAL_SCORE_S
} fitness_criterias_t;
typedef enum fitness_cat {
	MIN_RANGE_S = 0,
	MAX_RANGE_S = 1,
	WEIGHT_S = 2,
	VALUE_S = 3,
	SCORE_S = 4
} fitness_cat_t;
typedef enum ressource_cost {
	SEMINAR_ROOM_COST_S = 50,
	AUD_COST_S = 100
} ressource_cost_t;
typedef enum time_name_numbers {
	MIN_DAYS = 4,
	MAX_DAYS = 7,
	MAX_TIMESLOTS = 56,
	N_TIMESLOTS_DAY_S = 8,
	N_OFF_HOUR_TIMESLOTS_S = 14,
	OFF_HOUR_START_S = 16
} time_name_number_t;


/* Structs */
typedef struct seminar_fitness_t {
	int id;
	int presenter;
	int opponent;
	int external_supervisor;
	int room;
	int timeslot;
	int supervisor;
	int co_supervisor;
	int presenter_subject;
	int opp_subject;
	int day;
	int month;
	int year;
} seminar_fitness_t;

typedef struct table_fitness_t {
	int id;
	seminar_fitness_t seminar_fit[31];
	double fitness_definitons[11][5];
	double score;
} table_fitness_t;


/* Prototypes  */
table_fitness_t *get_benchmark_data();
void check_top_x(table_t *t, table_t *TopX, int n);
table_fitness_t *get_benchmark(dict_t *d, bool use_weights);
void get_benchmark_score(dict_t *d, table_fitness_t *benchmark_data, bool use_weights);
void get_fitness_score(table_t *t, dict_t *d, bool use_weights);
void Room_selection_score(dict_t *d, table_fitness_t *fitness_table);
void Person_day_utilization_score(dict_t *d, table_fitness_t *fitness_table);
void Optimal_time_slot_score(dict_t *d, table_fitness_t *fitness_table);
void Person_cost_score(dict_t *d, table_fitness_t *fitness_table);
void calc_fitness_score(dict_t *d, table_fitness_t *fitness_table);
void Room_cost_score(dict_t *d, table_fitness_t *fitness_table);
void Different_subjects_score(dict_t *d, table_fitness_t *fitness_table);
void Group_booking_different_day_score(dict_t *d, table_fitness_t *fitness_table);
void group_booking_spread_score(dict_t *d, table_fitness_t *fitness_table);
void person_daily_booking_spread_score(dict_t *d, table_fitness_t *fitness_table);
void person_overall_booking_spread_score(dict_t *d, table_fitness_t *fitness_table);
void total_seminar_length_score(dict_t *d, table_fitness_t *fitness_table);
void normalize_weight_score(int i, table_fitness_t *ft);
table_t *prepare_top_x(int n);
int cmp_totalscore(const void *v1, const void *v2);
void create_table_population(dict_t *d, int population, bool is_weighted, bool use_bookings);
void print_fitness_details(table_t *table, table_fitness_t *bd);
void get_fitness_definitions(dict_t *d, table_fitness_t *ft, bool use_weights);