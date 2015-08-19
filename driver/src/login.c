/*
 * Login access for PHP users
 * Victor Manuel Fernadez Castro
 * 3 August 2015
 */

#include <crypt.h>
#include <stdlib.h>
#include <string.h>
#include <shadow.h>

int main(int argc, char **argv) {
	struct spwd *spwd;
	char buffer[12];

	if (argc < 3)
		return EXIT_FAILURE;

	spwd = getspnam(argv[1]);

	if (spwd == NULL)
		return EXIT_FAILURE;

	strncpy(buffer, spwd->sp_pwdp, 11);
	return strcmp(crypt(argv[2], buffer), spwd->sp_pwdp) != 0;
}
