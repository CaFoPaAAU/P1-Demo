
#include "fitness.h"
#include "tables.h"
#include "time.h"
#include "sp_types.h"

int main() {
	
	/* dictionary to easily pass data */
	dict_t *d = get_dictionary();

	/* seed rand */
	time_t t;
	srand((unsigned)time(&t));

	char selection;
	char is_weighted;
	int population;

	do {
		printf("\n *** Welcome to Seminar Planner 9000! ***\n\n");
		printf("1) Generate X random valid time tables and print them.\n");
		printf("2) Calculate fitness score for X time tables and find the best one.\n");
		printf("3) Test Seminar Planner 9000 against the time table from the seminar in Nov '17.\n>> ");
		scanf(" %c", &selection);

		switch (selection) {
		case '1':
			printf("Enter number of time tables >> ");
			scanf(" %d", &population);
			generate_n_rand_tables(d, population);
			break;
		
		case '2':
			printf("Use weighted fitness score to create time tables? (y/n) >> ");
			scanf(" %c", &is_weighted);
			printf("Select population size (min. 1000) >> ");
			scanf(" %d", &population);
			create_table_population(d, population > 999 ? population : 1000, is_weighted == 'y' ? true : false, true);
			printf("Press 'q' to exit program, or any other key to continue \n");
			scanf(" %c", &selection);
			break;

		case '3':
			printf("\nInfo: Booking data will not be in included when generating tables.\n\n");
			printf("Use weighted fitness score to create time tables? (y/n) >> ");
			scanf(" %c", &is_weighted);
			printf("Select population size: \n - Recommented test size: > 100.000.\n - A size of 60.000 ~ 1 minute runtime. \n\n>> ");
			scanf(" %d", &population);
			create_table_population(d, population > 999 ? population : 1000, is_weighted == 'y' ? true : false, false);
			printf("Press 'q' to exit program, or any other key to continue \n");
			scanf(" %c", &selection);
			break;

		default:
			break;
		}
		
	} while (selection != 'q');

	/* exit */
	free(d->groups);
	free(d->persons);
	free(d->timeslots);
	free(d->subjects);
	free(d->room_bookings);
	free(d->rooms);
	free(d);

	return EXIT_SUCCESS;
}



