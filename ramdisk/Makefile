IMAGE=ramdisk
FW_NAME		=	eddy-fs-2.5.1.1.bin

FIRMWARE_DIR	=	../firmware

## Check environments
#include ../Make.check

install:
	#@echo "Making ramdisk image..."
	#$(TOOL) -b 8192 -d root -D device_table.txt ramdisk
	#../tool/genext2fs  -U -b 5110 -d root -D device_table.txt ramdisk
	#../tool/genext2fs  -U -b 7158 -d root -D device_table.txt ramdisk
	#../tool/mkcramfs   -q -D device_table.txt root ramdisk
	../tool/genext2fs  -U -b 10240 -N 1024 -d root -D device_table.txt ramdisk
	gzip -vf9 ramdisk
	test -f ramdisk.gz 
	../tool/mkimage -A arm -O linux -T ramdisk -C gzip -a 0 -e 0 -n $(FW_NAME) -d ./ramdisk.gz $(FW_NAME) 
	test -f $(FW_NAME) 
	mv $(FW_NAME) $(FIRMWARE_DIR)/ 

release:
	cp -f ../src/Eddy_APPs/eddy					root/sbin
	cp -f ../src/Eddy_APPs/wifi					root/sbin
	cp -f ../src/Eddy_APPs/def					root/sbin
	cp -f ../src/Eddy_APPs/ddns_agent			root/sbin
	cp -f ../src/Eddy_APPs/kt					root/sbin
	cp -f ../src/Eddy_APPs/pinetd				root/sbin
	cp -f ../src/Eddy_APPs/upgrade				root/sbin
	cp -f ../src/Eddy_APPs/upgradetftp			root/sbin
	cp -f ../src/Eddy_APPs/test_sio				root/sbin
	cp -f ../src/Eddy_APPs/test_adc				root/sbin
	cp -f ../src/Eddy_APPs/test_gpio_pin		root/sbin
	cp -f ../src/Eddy_APPs/test_gpio_led		root/sbin
	cp -f ../src/Eddy_APPs/test_rtc				root/sbin
	cp -f ../src/Eddy_APPs/test_nand			root/sbin
	cp -f ../src/Eddy_APPs/test_mmc				root/sbin
	cp -f ../src/Eddy_APPs/test_spi_eeprom		root/sbin
	cp -f ../src/Eddy_APPs/test_keypad			root/sbin
	cp -f ../src/Eddy_APPs/test_lcd				root/sbin
	cp -f ../src/Eddy_APPs/test_dio				root/sbin
	cp -f ../src/Eddy_APPs/testdk				root/sbin
	cp -f ../src/Eddy_APPs/test_bluetooth		root/sbin
	cp -f ../src/Eddy_APPs/test_read_config		root/sbin
	cp -f ../src/Eddy_APPs/test_serial			root/sbin
	cp -f ../src/Eddy_APPs/test_serial_to_lan-1	root/sbin
	cp -f ../src/Eddy_APPs/test_serial_to_lan-2	root/sbin
	cp -f ../src/Eddy_APPs/test_tcp_server		root/sbin
	cp -f ../src/Eddy_APPs/test_tcp_client		root/sbin
	cp -f ../src/Eddy_APPs/test_udp_server		root/sbin
	cp -f ../src/Eddy_APPs/test_udp_client		root/sbin
	
	cp -f ../src/RT73/Module/rt73.ko			root/etc/drivers
	cp -f ../src/RT73/Module/rt73.bin			root/etc/drivers
	cp -f ../src/RT73/Module/rt73sta.dat		root/etc/drivers
	cp -f ../src/wireless_tools.29/libiw.so.*	root/lib
	cp -f ../src/wireless_tools.29/ifrename		root/sbin
	cp -f ../src/wireless_tools.29/iwconfig		root/sbin
	cp -f ../src/wireless_tools.29/iwevent		root/sbin
	cp -f ../src/wireless_tools.29/iwgetid		root/sbin
	cp -f ../src/wireless_tools.29/iwlist		root/sbin
	cp -f ../src/wireless_tools.29/iwpriv		root/sbin
	cp -f ../src/wireless_tools.29/iwspy		root/sbin
	
	rm -f ../src/Eddy_APPs/web/htdocs/*.bak
	cp -f ../src/Eddy_APPs/web/htdocs/*.*		root/usr/local/www
	cp -f ../src/Eddy_APPs/web/cgi/*.cgi		root/usr/local/www/cgi-bin
	cp -f ../src/busybox-1.5.0/busybox			root/bin
	cp -f ../src/iptables-1.3.7/iptables		root/usr/local/sbin
	cp -f ../src/dropbear-0.50/dropbear			root/usr/local/sbin
	cp -f ../src/dropbear-0.50/dropbearkey		root/usr/local/sbin
	cp -f ../src/ethtool-6/ethtool				root/usr/local/sbin
	cp -f ../src/thttpd-2.25b/thttpd			root/usr/local/sbin
	cp -f ../src/net-snmp-5.4.1/agent/snmpd 		root/usr/snmp	
	cp -f ../src/net-snmp-5.4.1/local/snmpd.conf	root/usr/snmp
	cp -f ../src/netkit-ftp-0.18/ftp/ftp		root/usr/local/bin
	cp -f ../src/ddns-1.8/ddns3					root/usr/sbin
	cp -f ../src/vsftpd-2.0.5/vsftpd			root/usr/sbin
	cp -f ../src/target-agent/tae 				root/sbin
	cp -f ../src/gdbserver/gdbserver			root/sbin		

