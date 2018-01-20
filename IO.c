/**
*	IO.c
*
*	Input & Output functions
*
*/

#include <stdio.h>
#include "sp_types.h"
#include "search.h"
#include "IO.h"
#include "fitness.h"
#include <stdbool.h>
#include <stdlib.h>

const char *DAY_STRS[] = { "MON", "TUE", "WED", "THU", "FRI" };

/* Input function */
void error_handler(error_type_t error_type) {

	switch (error_type) {

	case INVALID_ID:
		printf("Error: ID not found.");
		break;
	
	case ACCESS_VIOLATION:
		printf("Error: Reading/writing to wrong memory!");
		break;
	
	case NO_MEMORY:
		printf("Error: Out of memory");
		break;

	case UNEXPECTED_VALUE:
		printf("Error: Unexpected value of variable.");
		break;
	
	case BAD_INPUT:
		printf("Error: Wrong input");
		break;
	
	case OPEN_FILE:
		printf("Error: Opening file.");
		break;
	}

	printf("\n\nClosing program.");
	
	exit(EXIT_FAILURE);
}

void print_table_full(table_t *table, dict_t *d, int n) {

	int i,
		opp_subject,
		pres_subject,
		supervisor,
		co_supervisor,
		external_supervisor,
		weekday,
		date, 
		month;

	seminar_t *seminars = table->seminars;

	hyphen_chain(10);

	printf("%-9s %-35s %-9s %-35s %-25s %-25s %-25s %-9s %-15s %-9s %-9s\n",
		"Presenter",
		"Subject",
		"Opponent",
		"Oopponent subject",
		"Supervisor",
		"Co-supervisor",
		"PV consultant",
		"Room",
		"Time ",
		"Day", 
		"Date"
	);

	hyphen_chain(10);

	for (i = 0; i < n; i++) {

		opp_subject = id_to_id(d, seminars[i].opponent, GROUP_S, SUBJECT_S);
		pres_subject = id_to_id(d, seminars[i].presenter, GROUP_S, SUBJECT_S);
		supervisor = id_to_id(d, seminars[i].presenter, GROUP_S, SUPERVISOR_S);
		co_supervisor = id_to_id(d, seminars[i].presenter, GROUP_S, CO_SUPERVISOR_S);
		external_supervisor = seminars[i].external_supervisor;
		weekday = timeslot_to_x(seminars[i].timeslot, d, WEEKDAY_S);
		date = timeslot_to_x(seminars[i].timeslot, d, DATE_S);
		month = timeslot_to_x(seminars[i].timeslot, d, MONTH_S);

		 char *WEEKDAY_STRS[] = { "Mon", "Tue", "Wed", "Thu", "Fri" };

		printf(	"%-9s "		/* presenter */
				"%-35s "	/* subject */
				"%-9s "		/* opponent */
				"%-35s "	/* opponent subject */
				"%-25s "	/* supervisor */
				"%-25s "	/*	co-supervisor	*/
				"%-25s "	/* external supervisor */
				"%-9s "		/* room */
				"%-15s "	/* timeslot */
				"%-9s "		/* weekday */
				"%-d/"		/* date */ 
				"%-d \n",	/* month */
				id_to_name(seminars[i].presenter, d, GROUP_S),
				id_to_name(pres_subject, d, SUBJECT_S),
				id_to_name(seminars[i].opponent, d, GROUP_S),
				id_to_name(opp_subject, d, SUBJECT_S),
				id_to_name(supervisor, d, PERSON_S),
				id_to_name(co_supervisor, d, PERSON_S),
				id_to_name(external_supervisor, d, PERSON_S),
				id_to_name(seminars[i].room, d, ROOM_S),
				id_to_name(seminars[i].timeslot, d, TIMESLOT_S),
				WEEKDAY_STRS[weekday],
				date,
				month
		);
		
	}
	printf("\nScore: %0.2lf \n\n", table->fitness_summary.total);

}

void print_table(table_t *table, dict_t *d, int n, bool debug) {

	int i;

	if(debug) { 
		printf("\nseminar_t seminars[] = {\n");
		
		for (i = 0; i < n; i++) {
			printf("{ %-2d, %-2d, %-2d, %-2d, %-2d, %-2d },  ", 
				table->seminars[i].id,
				table->seminars[i].presenter,
				table->seminars[i].opponent,
				table->seminars[i].external_supervisor,
				table->seminars[i].room,
				table->seminars[i].timeslot
			);
		
			if (i % 2 == 0)
				printf("\n");
		}
		printf("};\n");
	}

	else {
		printf("%-8s %-12s %-15s %-15s %-30s %-10s \n%s \n",
			"ID", "Timeslot", "Group", "Opponent", "PV-consultant", "Room",
			"---------------------------------------------"
			"---------------------------------------------"
		);

		for (i = 0; i < n; i++) {

			printf("%-8d %-12d %-15s %-15s %-30s %-10s \n",

				table->seminars[i].id,
				table->seminars[i].timeslot,
				id_to_name(table->seminars[i].presenter, d, GROUP_S),
				id_to_name(table->seminars[i].opponent, d, GROUP_S),
				id_to_name(table->seminars[i].external_supervisor, d, PERSON_S),
				id_to_name(table->seminars[i].room, d, ROOM_S)
			);
		}
	}

	printf("\nScore: %.2lf\n\n", table->fitness_summary.total);
}

void print_groups(const dict_t *d) {
	group_t g;
	int i;

	hyphen_chain(3);

	printf("%-15s %-30s %-30s %-20s \n",
		"Group",
		"Supervisor",
		"Co-supervisor",
		"Subject"
	);

	hyphen_chain(3);

	for (i = 0; i < d->n_groups; i++) {
		g = d->groups[i];

		printf("%-15s %-30s %-30s %-20s\n",
			g.name,
			d->persons[g.supervisor].name,
			d->persons[g.co_supervisor].name,
			d->subjects[g.subject].name
		);
	}
}

void hyphen_chain(int count) {
	int i;

	for (i = 0; i < count; i++) {

		printf(
			"---------------------"
		);
	}
	printf("\n");
}