#include <stdio.h>
#include "wrappers.h"

/*
 * FIFO check: queue an older Chihuahua (floor 3 to 1) and then a younger Pug
 * (floor 3 to 5). Run it while the elevator is OFFLINE, then start the elevator.
 */
int main(void)
{
	printf("Issue (3, 1, 0) returned %d\n", issue_request(3, 1, 0));
	printf("Issue (3, 5, 1) returned %d\n", issue_request(3, 5, 1));
	return 0;
}
