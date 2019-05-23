obj-m += sys_call1.o sys_call2.o sys_call3.o sys_call4.o sys_call5.o sys_call6.o sys_call7.o sys_call8.o sys_call8_1.o sys_call9.o sys_call10.o

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all: call_user call1 call2 call3 call4 call5 call6 call7 call8 call8_1 call9 call10

call_user: call_user.c
	gcc -Wall -Werror -I$(INC)/generated/uapi -I$(INC)/uapi call_user.c -o call_user

call1:
	make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call2:
	 make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call3:
	 make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call4:
	 make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call5:
	make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call6:
	make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call7:
	make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call8:
	 make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call8_1:
	 make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call9:
	 make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call10:
	make -Wall -Werroe -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f call_user
