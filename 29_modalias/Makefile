#obj-m += sheet_5_q2.o Sheet_6_q2.o
#obj-m += sheet8_proc_read_write.o
obj-m += gpio_driver.o platform_dev.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) clean
