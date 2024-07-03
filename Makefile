obj-m += my_module.o
KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules
	gcc -o my_app my_app.c

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf my_app
