#pragma once

/**
*	sp_types.h	
*
*  Types for seminar planner project
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef unsigned int uint;

typedef enum search_mode {
	PERSON_S, ROOM_S, GROUP_S,
	SUBJECT_S, TIMESLOT_S, SUPERVISOR_S,
	CO_SUPERVISOR_S, EXTERNAL_SUPERVISOR_S,
	MONTH_S, YEAR_S, DAY_S, WEEKDAY_S, DATE_S
}search_mode_t;


typedef enum days {
	MON, TUE, WED, THU, FRI
} day_t;

#define N_ROOMS 5
#define N_PV 4
#define N_GRPS 31
#define true 1
#define false 0
#define TABLE_SIZE 31

typedef enum buffers {
	SIZE_TMP = 70,
	ROOM_NAME_MAX = 8,
	DAY_MAX = 4,
	TIMESLOT = 10
}buffers;

typedef enum role {
    SUPERVISOR = 1, CO_SUPERVISOR = 2, EXTERNAL_SUPERVISOR = 4
} role_t;

typedef enum booking_type {
    PERSON,
	SEMINAR_ROOM,
    AUDITORIUM
} room_type_t;

typedef enum special {
	NO_OPPONENT,
}special;

typedef struct timeslot {
    int id;
	int date;
	int month;
	int year;
	day_t weekday;
	int begin_hour;
	int begin_min;
	int end_hour;
	int end_min;
	char name[TIMESLOT]; /* time str */
} timeslot_t;

typedef struct room {
    int id;
    char name[ROOM_NAME_MAX];
	int type;
} room_t;

typedef struct person {
    int id;
    char name[SIZE_TMP];
    uint role;
	int salery_per_timeslot;
} person_t;

typedef struct subject {
    int id;
    char name[SIZE_TMP];
} subject_t;

typedef struct group {
    int id;
    char name[SIZE_TMP];
    int subject;
    int supervisor;
    int co_supervisor;
	int special;
} group_t;

typedef struct booking {
    int id;
    int timeslot;
	int type; 
	int resource_id; 
} booking_t;

typedef struct seminar {
    int id;
    int presenter;
    int opponent;
    int external_supervisor; /* Has to be renamed */
    int room;
    int timeslot;
} seminar_t;

typedef struct dictionary{ 
	group_t *groups;
	int n_groups;

	person_t *persons;
	int n_persons;

	room_t *rooms;
	int n_rooms;

	timeslot_t *timeslots;
	int n_timeslots;

	subject_t *subjects;
	int n_subjects;

	booking_t *room_bookings;
	int n_room_bookings;

	booking_t *person_bookings;
	int n_person_bookings;

	int *person_bookings_slots;
	int *room_bookings_slots;
}dict_t;

typedef struct fitness_t {
	double fitness_definitions[11][5];
	double total;
} fitness_t;

typedef struct table_t {
	seminar_t seminars[N_GRPS];
	int id;
	fitness_t fitness_summary;
} table_t;

typedef struct group_test_t {
    char name[SIZE_TMP];
    char subject[SIZE_TMP];
    char supervisor[SIZE_TMP];
    char co_supervisor[SIZE_TMP];
} group_test_t;

timeslot_t *get_timeslots(int *n_timeslots);
room_t *get_rooms(int *n_rooms);
person_t *get_persons(int *n_persons);
subject_t *get_subjects(int *n_subjects);
group_t *get_groups(int *n_groups);
booking_t *get_person_bookings(int *n_bookings);
booking_t *get_room_bookings(int *n_bookings);
void shuffle_array(int array[], int len);
dict_t *get_dictionary();
timeslot_t *get_timeslots(int *n_timeslots);
room_t *get_rooms(int *n_rooms);
person_t *get_persons(int *n_persons);
subject_t *get_subjects(int *n_subjects);
group_t *get_groups(int *n_groups);
booking_t *get_bookings(int *n_bookings);
booking_t *get_room_bookings(int *n_bookings);
booking_t *get_person_bookings(int *n_bookings);
void shuffle_array(int array[], int len);
void check_file(FILE *file, char *msg);
day_t weekday_from_str(char *s);
int *room_bookings_slots(dict_t *d);
int *person_bookings_slots(dict_t *d);