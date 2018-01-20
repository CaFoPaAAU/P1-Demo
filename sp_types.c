#include <limits.h>
#include "sp_types.h"
#include "IO.h"

#define MAX_LINE_LEN 200

/* Data import */
/* dictionary to easily pass data */
dict_t *get_dictionary() {
	dict_t *d = malloc(sizeof(dict_t));

	d->subjects = get_subjects(&d->n_subjects);
	d->persons = get_persons(&d->n_persons);
	d->groups = get_groups(&d->n_groups);
	d->timeslots = get_timeslots(&d->n_timeslots);
	d->rooms = get_rooms(&d->n_rooms);
	d->room_bookings = get_room_bookings(&d->n_room_bookings);
	d->person_bookings = get_person_bookings(&d->n_person_bookings);
	d->room_bookings_slots = room_bookings_slots(d);
	d->person_bookings_slots = person_bookings_slots(d);

	return d;
}

timeslot_t *get_timeslots(int *n_timeslots) {
	FILE *file;
	timeslot_t *timeslots, *t;
	char buff[MAX_LINE_LEN], weekday[SIZE_TMP];
	int i = 0, allocated = 100, matched;

	timeslots = malloc(allocated * sizeof(timeslot_t));
	file = fopen("input_files/Timeslot.txt", "r");
	check_file(file, "input_files/Timeslot.txt");

	while (fgets(buff, MAX_LINE_LEN, file)) {
		if (i == allocated) {
			allocated *= 2;
			timeslots = realloc(timeslots, allocated * sizeof(timeslot_t));
		}

		t = &timeslots[i];
		matched = sscanf(buff, "%2d/%2d/%4d %s %2d:%2d-%2d:%2d %d",
			&t->month, &t->date, &t->year, weekday,
			&t->begin_hour, &t->begin_min, &t->end_hour, &t->end_min,
			&t->id);

		if (matched == 9) {
			t->weekday = weekday_from_str(weekday);
			sprintf(t->name, "%d:%.2d-%d:%.2d", t->begin_hour, t->begin_min,
				t->end_hour, t->end_min);
			i++;
		}
	}

	*n_timeslots = i;

	return timeslots;
}

room_t *get_rooms(int *n_rooms) {
	FILE *file;
	room_t *rooms, *r;
	char buff[MAX_LINE_LEN], name[SIZE_TMP], temp[SIZE_TMP];
	int i = 0, allocated = 10, matched;

	rooms = calloc(allocated, sizeof(room_t));
	
	if (!rooms)
		error_handler(NO_MEMORY);
	
	file = fopen("input_files/Room.txt", "r");
	check_file(file, "input_files/Room.txt");

	while (fgets(buff, MAX_LINE_LEN, file)) {
		if (i == allocated)
			rooms = realloc(rooms, (allocated *= 2) * sizeof(room_t));

		r = &rooms[i];
		matched = sscanf(buff, " %[^\t\n] %d %[^\t\n] %d", name, &r->id, temp , &r->type);

		if (matched == 4) {
			strcpy(rooms[i].name, name);
			i++;
		}
	}

	*n_rooms = i;

	return rooms;
}

subject_t *get_subjects(int *n_subjects) {
	FILE *file;
	char buff[MAX_LINE_LEN], name[SIZE_TMP];
	subject_t *subjects;
	int i = 0, allocated = 10, matched;

	subjects = calloc(allocated, sizeof(subject_t));
	
	if (!subjects)
		error_handler(NO_MEMORY);
	
	file = fopen("input_files/Subject.txt", "r");
	check_file(file, "input_files/Subject.txt");
	fgets(buff, MAX_LINE_LEN, file);

	while (fgets(buff, MAX_LINE_LEN, file)) {
		if (i == allocated)
			subjects = realloc(subjects, (allocated *= 2) * sizeof(subject_t));

		matched = sscanf(buff, " %[^\t\n] %d", name, &subjects[i].id);

		if (matched == 2) {
			strcpy(subjects[i].name, name);
			i++;
		}
	}

	fclose(file);
	*n_subjects = i;

	return subjects;
}

person_t *get_persons(int *n_persons) {
	FILE *file;
	char buff[MAX_LINE_LEN];
	person_t *persons, *p;
	int i = 0, allocated = 20, matched;

	file = fopen("input_files/Person.txt", "r");
	check_file(file, "input_files/Person.txt");
	persons = calloc(allocated, sizeof(person_t));
	if (!persons)
		error_handler(NO_MEMORY);

	/* salary is not in file: */
	while (fgets(buff, MAX_LINE_LEN, file)) {
		if (i == allocated)
			persons = realloc(persons, (allocated *= 2) * sizeof(person_t));

		p = &persons[i];
		matched = sscanf(buff, "%[^\t\n] %d %u %d",
			p->name, &p->id, &p->role, &p->salery_per_timeslot);
		
		if (!matched)
			error_handler(OPEN_FILE);
		i++;
	}

	fclose(file);
	*n_persons = i;

	return persons;
}

group_t *get_groups(int *n_groups) {
	group_t *groups;
	FILE *file;
	char buff[MAX_LINE_LEN], ignore[SIZE_TMP], name[SIZE_TMP];
	int i = 0, allocated = 32;

	groups = calloc(allocated, sizeof(group_t));
	if (!groups)
		error_handler(NO_MEMORY);
	file = fopen("input_files/Group.txt", "r");
	check_file(file, "input_files/Group.txt");
	fgets(buff, MAX_LINE_LEN, file); /* skip header */

	while (fgets(buff, MAX_LINE_LEN, file)) {
		if (allocated == i) {
			allocated *= 2;
			groups = realloc(groups, allocated * sizeof(group_t));
		}

		sscanf(buff, "%d %[^\t\n] %[^\t\n] %d %[^\t\n] %d %[^\t\n] %d",
			&groups[i].id, name, ignore,
			&groups[i].subject, ignore, &groups[i].supervisor,
			ignore, &groups[i].co_supervisor);
		strcpy(groups[i].name, name);
		groups[i].special = 0; /* ? */
		i++;
	}

	fclose(file);
	*n_groups = i;

	return groups;
}

booking_t *get_person_bookings(int *n_bookings) {
	char *file_name = "input_files/Booking_Persons.txt";
	booking_t *bookings, *b;
	FILE *file;
	char buff[MAX_LINE_LEN], ignore[SIZE_TMP];
	int i = 0, allocated = 100, matched;

	bookings = malloc(allocated * sizeof(booking_t));
	file = fopen(file_name, "r");
	check_file(file, file_name);

	while (fgets(buff, MAX_LINE_LEN, file)) {
		if (allocated == i) {
			allocated *= 2;
			bookings = realloc(bookings, allocated * sizeof(booking_t));
		}

		b = &bookings[i];
		matched = sscanf(buff, "%d %[^\t\n] %[^\t\n] %d %[^\t\n] %d",
			&b->id, ignore, ignore, &b->timeslot, ignore, &b->resource_id);
		b->type = 0; /* ? */

		if (matched == 6)
			i++;
	}

	fclose(file);
	*n_bookings = i;

	return bookings;
}

booking_t *get_room_bookings(int *n_bookings) {
	char *file_name = "input_files/Booking_Rooms.txt";
	booking_t *bookings, *b;
	FILE *file;
	char buff[MAX_LINE_LEN], ignore[SIZE_TMP];
	int i = 0, allocated = 100, matched;

	bookings = malloc(allocated * sizeof(booking_t));
	file = fopen(file_name, "r");
	check_file(file, file_name);

	while (fgets(buff, MAX_LINE_LEN, file)) {
		if (allocated == i) {
			allocated *= 2;
			bookings = realloc(bookings, allocated * sizeof(booking_t));
		}

		b = &bookings[i];
		matched = sscanf(buff, "%d %[^\t\n] %[^\t\n] %d %[^\t\n] %d",
			&b->id, ignore, ignore, &b->timeslot, ignore, &b->resource_id);

		if (matched == 6)
			i++;
	}

	fclose(file);
	*n_bookings = i;

	return bookings;
}

booking_t *get_bookings(int *n_bookings) {
	char *file_name = "input_files/bookings.txt";
	booking_t *bookings, *b;
	FILE *file;
	char buff[MAX_LINE_LEN];
	int i = 0, allocated = 100, matched;

	bookings = malloc(allocated * sizeof(booking_t));
	file = fopen(file_name, "r");
	check_file(file, file_name);

	while (fgets(buff, MAX_LINE_LEN, file)) {
		if (allocated == i) {
			allocated *= 2;
			bookings = realloc(bookings, allocated * sizeof(booking_t));
		}

		b = &bookings[i];
		matched = sscanf(buff, "%d %d %d %d", &b->id, &b->timeslot,
			&b->resource_id, &b->type);

		if (matched == 4)
			i++;
	}

	fclose(file);
	*n_bookings = i;

	return bookings;
}

void check_file(FILE *f, char *msg) {
	if (f == NULL) {
		fprintf(stderr, "Could not open >>%s<<!\n", msg);
		exit(1);
	}
}

/* this assumes s is in correct format */
day_t weekday_from_str(char *s) {
	const char *day_code[] = { "MON", "TUE", "WED", "THU", "FRI" };
	day_t day = MON;

	while (day < FRI && strcmp(day_code[day], s) != 0)
		day++;

	return day;
}

int *person_bookings_slots(dict_t *d) {

	int i, slot, id;
	/* create a 1d matrix of ID of booked person 1 axis and timeslot on the other */

	int *pb = (int *)calloc(d->n_timeslots * d->n_persons * 2, sizeof(int));

	if (!pb)
		error_handler(NO_MEMORY);

	for (i = 0; i < d->n_person_bookings; i++) {

		slot = d->person_bookings[i].timeslot;
		id = d->person_bookings[i].resource_id;

		pb[slot * d->n_persons + id] = 1;
	}

	return pb;
}


int *room_bookings_slots(dict_t *d) {

	int i, slot, id;
	/* create a mapping of timeslots in relation to booking IDs of person */ 

	int *rb = (int *)calloc(d->n_timeslots * d->n_rooms * 2, sizeof(int));

	if (!rb)
		error_handler(NO_MEMORY);

	for (i = 0; i < d->n_room_bookings; i++) {
		slot = d->room_bookings[i].timeslot;
		id = d->room_bookings[i].resource_id;

		rb[slot * d->n_rooms + id] = 1;
	}

	return rb;
}