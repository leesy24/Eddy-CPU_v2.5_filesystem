CROSS		= /opt/lemonix/cdt/bin/arm-linux-
LDFLAGS 	+= -L/opt/lemonix/cdt/lib -L/opt/lemonix/cdt/bin
IFLAGS 		+= -I/opt/lemonix/cdt/include  -I./include
CFLAGS 		=	-lm -O2 -g  -Wall -Wno-nonnull
DEST		= ../../ramdisk/root/sbin	
DEST_ETC	= ../../ramdisk/root/etc

CC		=	$(CROSS)gcc 
STRIP	=	$(CROSS)strip
AR		= 	$(CROSS)ar

TARGET		=	eddy			pinetd					def			\
			ddns_agent		upgrade					upgradetftp		\
			wifi			test_nand				test_mmc		\
			test_rtc		test_sio				test_adc		\
			test_gpio_led		test_keypad 				test_lcd		\
			test_dio		test_gpio_pin				test_bluetooth		\
			test_spi_eeprom		test_tcp_server				test_tcp_client		\
			test_serial		test_read_config			test_serial_to_lan-2	\
			test_serial_to_lan-1	test_udp_server				test_udp_client		\
			testdk 	    udp_DRFG  udp_DRFB  udp_DRFGmain  udp_DVSCPU  tcp16broadcast  udp_GDM_EC \
			testUDPhosts  tcp32broadcast  tcp_client  udp_DPSscan

LIBS		=   -lrt SB_APIs/SB_APIs.a

all : $(TARGET)

udp_DPSscan : udp_DPSscan.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

tcp_client : tcp_client.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

tcp32broadcast : tcp32broadcast.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

udp_GDM_EC : udp_GDM_EC.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

tcp16broadcast : tcp16broadcast.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@
	
udp_DVSCPU : udp_DVSCPU.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

udp_DRFGmain : udp_DRFGmain.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

udp_DRFB : udp_DRFB.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@
	
udp_DRFG : udp_DRFG.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

test_tcp_client : test_tcp_client.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

test_tcp_server : test_tcp_server.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

test_read_config : test_read_config.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

test_udp_server : test_udp_server.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

test_udp_client : test_udp_client.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

test_serial : test_serial.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

test_bluetooth : test_bluetooth.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@
	
test_serial_to_lan-1 : test_serial_to_lan-1.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS) 
	$(STRIP) $@

test_serial_to_lan-2 : test_serial_to_lan-2.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS)
	$(STRIP) $@

testdk : testdk.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o 
	$(STRIP) $@

test_dio : test_dio.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o 
	$(STRIP) $@

test_lcd : test_lcd.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o 
	$(STRIP) $@

test_keypad : test_keypad.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o 
	$(STRIP) $@

test_spi_eeprom : test_spi_eeprom.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o 
	$(STRIP) $@

test_gpio_led : test_gpio_led.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o  $(LIBS)
	$(STRIP) $@

test_nand : test_nand.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o
	$(STRIP) $@

test_mmc : test_mmc.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o
	$(STRIP) $@

test_rtc : test_rtc.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o
	$(STRIP) $@

test_gpio_pin : test_gpio_pin.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o 
	$(STRIP) $@

test_sio : test_sio.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o  $(LIBS)
	$(STRIP) $@

test_adc : test_adc.o
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o  $(LIBS)
	$(STRIP) $@

def : def.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS)  -o $@  $@.o  $(LIBS)
	$(STRIP) $@

wifi : wifi.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS)
	$(STRIP) $@

pinetd : pinetd.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o  $(LIBS)
	$(STRIP) $@

eddy : eddy.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o
	$(STRIP) $@

ddns_agent : ddns_agent.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o $(LIBS)
	$(STRIP) $@

upgrade : upgrade.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o $(LIBS)
	$(STRIP) $@
	
upgradetftp : upgradetftp.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o 
	$(STRIP) $@
		
rt_test : rt_test.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o $(LIBS)
	$(STRIP) $@

testUDPhosts : testUDPhosts.o 
	rm -f $@
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAGS) -o $@  $@.o $(LIBS)
	$(STRIP) $@

clean: 
	rm -f *.bak *.o


release:
	cp  -f $(TARGET) $(DEST)
	cp  -f sb_default_config $(DEST_ETC)

