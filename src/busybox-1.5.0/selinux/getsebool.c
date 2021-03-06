/*
 * getsebool
 *
 * Based on libselinux 1.33.1
 * Port to BusyBox  Hiroshi Shinji <shiroshi@my.email.ne.jp>
 *
 */

#include "busybox.h"

int getsebool_main(int argc, char **argv);
int getsebool_main(int argc, char **argv)
{
	int i, rc = 0, active, pending, len = 0;
	char **names;
	unsigned opt;

	selinux_or_die();
	opt = getopt32(argc, argv, "a");

	if (opt) { /* -a */
		if (argc > 2)
			bb_show_usage();

		rc = security_get_boolean_names(&names, &len);
		if (rc)
			bb_perror_msg_and_die("cannot get boolean names");

		if (!len) {
			puts("No booleans");
			return 0;
		}
	}

	if (!len) {
		if (argc < 2)
			bb_show_usage();
		len = argc - 1;
		names = xmalloc(sizeof(char *) * len);
		for (i = 0; i < len; i++)
			names[i] = xstrdup(argv[i + 1]);
	}

	for (i = 0; i < len; i++) {
		active = security_get_boolean_active(names[i]);
		if (active < 0) {
			bb_error_msg_and_die("error getting active value for %s", names[i]);
		}
		pending = security_get_boolean_pending(names[i]);
		if (pending < 0) {
			bb_error_msg_and_die("error getting pending value for %s", names[i]);
		}
		printf("%s --> %s", names[i], (active ? "on" : "off"));
		if (pending != active)
			printf(" pending: %s", (pending ? "on" : "off"));
		putchar('\n');
	}

	if (ENABLE_FEATURE_CLEAN_UP) {
		for (i = 0; i < len; i++)
			free(names[i]);
		free(names);
	}

	return rc;
}
