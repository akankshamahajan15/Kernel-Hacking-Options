#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	int rc = 0, val = 10;

	if (argc < 2) {
		printf("system call number not specified\n");
		return -1;
	}

	if (!strcmp(argv[1], "1"))
		rc = syscall(__NR_call1, &val);
	else if (!strcmp(argv[1], "2"))
                rc = syscall(__NR_call2, &val);
	else if (!strcmp(argv[1], "3"))
                rc = syscall(__NR_call3, &val);
	else if (!strcmp(argv[1], "4"))
                rc = syscall(__NR_call4, &val);
	else if (!strcmp(argv[1], "5"))
                rc = syscall(__NR_call5, &val);
	else if (!strcmp(argv[1], "6"))
                rc = syscall(__NR_call6, &val);
	else if (!strcmp(argv[1], "7"))
                rc = syscall(__NR_call7, &val);
	else if (!strcmp(argv[1], "8"))
                rc = syscall(__NR_call8_1, &val);
	else if (!strcmp(argv[1], "9"))
                rc = syscall(__NR_call9, &val);
	else if (!strcmp(argv[1], "10"))
                rc = syscall(__NR_call10, &val);
	else {
		printf("Wrong system call number specified\n");
		return -1;
	}

	if (rc == 0)
		printf("syscall returned %d\n", rc);
	else
		printf("syscall returned %d (errno=%d)\n", rc, errno);

	exit(rc);
}
