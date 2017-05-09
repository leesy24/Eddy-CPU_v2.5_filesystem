/*****************************************************************************
Copyright (c) 2013 DASAN-Infotech, jslee...

UDP Server/Clinet		from 09, 31, 2007. yhlee@sysbas
******************************************************************************/
#include "include/sb_include.h"
#include "include/sb_define.h"
#include "include/sb_shared.h"
#include "include/sb_config.h"
#include "include/sb_extern.h"
#include "include/sb_ioctl.h"
#include "include/sb_misc.h"  ////-DRF-

#define MAX_BUFFER_SIZE 		3000
#define MAX_SOCKET_READ_SIZE 	512
#define MAX_SERIAL_READ_SIZE 	512
//
#define MAX_rfWORK_SIZE 		128



//-----------------------------------------------------(Config for S4M filesystem 2.5.1.2)
//struct SB_SIO_CONFIG			cfg [SB_MAX_SIO_PORT];
//
struct SB_SYSTEM_CONFIG			CFG_SYS;						// /flash/eddy_system.cfg
struct SB_SIO_CONFIG			CFG_SIO [SB_MAX_SIO_PORT];		// /flash/eddy_sio.cfg
struct SB_SNMP_CONFIG			CFG_SNMP;						// /flash/eddy_snmp.cfg
struct SB_GPIO_CONFIG			CFG_GPIO;						// /flash/eddy_gpio.cfg
struct SB_DIO_CONFIG			CFG_DIO;						// /flash/eddy_dio.cfg
struct SB_WIFI_CONFIG			CFG_WIFI;						// /flash/eddy_wifi.cfg


struct sys_info  
{
	int				sfd;
	int				lfd;
	int				port_no;
	int				protocol;
	int				wait_msec;
	int				socket;
	char  			bps;
	char 			dps;
	char 			flow;
	char			interface;
	char 			server_ip [100];
	int				server_port;	
	
	
	//-----------------------------------!DRFG200R_0X!   (Com-1/2)
	int				sfd2;
	char  			bps2;
	char 			dps2;
	char 			flow2;
	char			interface2;
	//-----------------------------------!!
	
	
};	

struct sys_info  SYS;

int port_no;
int portview;
int snmp;
struct SB_PORTVIEW_STRUCT *PSM;
struct SB_SNMP_STRUCT	  *SSM;

char WORK [MAX_BUFFER_SIZE];
char WORK2 [MAX_BUFFER_SIZE];
//
char rfWORK [MAX_rfWORK_SIZE];
char rfWORK2 [MAX_rfWORK_SIZE];
int rfLEN2;

char IDs[32][10];

int	SB_DEBUG	=	0;

////struct SB_CONFIG cfg;

int main (int argc, char *argv[]);

void mainloop(int protocol);
void close_init ();
int receive_from_lan(void);
int receive_from_port (int protocol);
//
int receive_from_DRFC200 (void);



/****
void get_portview_memory ();
void get_snmp_memory ();
//int PA_login();

void SSM_write ();
void PSM_write (char sw, char *data, int len);
****/

//===============================================================================	
int main (int argc, char *argv[])
{
int i;

	//SB_ReadConfig (CFGFILE_ETC_SIO, (char *)&cfg[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);
	//
	//------------------------- Read /etc Config Data
	SB_ReadConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, 		sizeof (struct SB_SYSTEM_CONFIG));
	SB_ReadConfig  (CFGFILE_ETC_SIO,     (char *)&CFG_SIO[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
	SB_ReadConfig  (CFGFILE_ETC_SNMP,    (char *)&CFG_SNMP,		sizeof(struct SB_SNMP_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_GPIO,    (char *)&CFG_GPIO,		sizeof(struct SB_GPIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_DIO,     (char *)&CFG_DIO,		sizeof(struct SB_DIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_WIFI,    (char *)&CFG_WIFI,		sizeof(struct SB_WIFI_CONFIG));



	//Not-USE@2.5  //SB_SetPriority (5);						// Min 1 ~ Max 99 

	port_no = atoi (argv[1]) - 1;			// port no 1 ~ 16
	
	signal(SIGPIPE, SIG_IGN);	//---??
	
	//get_portview_memory ();
	//get_snmp_memory ();

	SYS.lfd = 0;
	SYS.sfd = 0;

	while (1) 
		{
		////SB_ReadConfig (SB_CFG_FILE, &cfg);
		SYS.protocol	= CFG_SIO[port_no].protocol;
		SYS.bps		 	= CFG_SIO[port_no].speed;
		SYS.dps		 	= CFG_SIO[port_no].dps;
		SYS.flow		= CFG_SIO[port_no].flow;
		SYS.wait_msec	= CFG_SIO[port_no].packet_latency_time;
		SYS.socket		= CFG_SIO[port_no].socket_no;
		SYS.interface	= CFG_SIO[port_no].interface;
		SYS.server_port	= CFG_SIO[port_no].remote_socket_no;
		SB_Hex2Ip ((char *)CFG_SIO[port_no].remote_ip, (char *)SYS.server_ip);


		if ( (port_no > 1) && (CFG_SIO[port_no -2 ].protocol == SB_DISABLE_MODE) &&
					!strncmp( CFG_SYS.device_name, "DRFB", 4) ) 
		{
				//-------(for SYS.sfd2)------------------------!!
				SYS.bps2		= CFG_SIO[port_no - 2].speed;
				SYS.dps2		= CFG_SIO[port_no - 2].dps;
				SYS.flow2		= CFG_SIO[port_no - 2].flow;
				SYS.interface2	= CFG_SIO[port_no - 2].interface;
		}



		/****
		if (SYS.wait_msec == SB_ENABLE)
			SYS.wait_msec = 0;
		else	
			SYS.wait_msec = SB_GetDelaySerial (SYS.bps);
		****/
		
		/**
		if (portview) 
			{
			PSM_PORT.reset_request = SB_DISABLE;	// Reset init
			PSM_PORT.flag		   = SB_ENABLE;		// Scope init
			PSM_PORT.status 	   = SB_ENABLE;		// Wait Stat
			}
		if (snmp) 
			{
			SSM_CONNECT_STAT = SB_DISABLE;
			SSM_PORTRESET = SB_DISABLE;
			}
		**/
		
		if (SB_DEBUG) 
			SB_LogMsgPrint ("UDP port=%d, Speed = %d, dps = %d, flow = %d, MRU=%d\n",
			port_no+1, SYS.bps, SYS.dps, SYS.flow, SYS.wait_msec);
		
		mainloop(SYS.protocol);
		}
}

/****
//===============================================================================
void get_snmp_memory ()
{
void *shared_memory = (void *)0;

	snmp = SB_DISABLE;
	shared_memory = SB_GetSharedMemory (SB_SNMP_KEY, sizeof (struct SB_SNMP_STRUCT));
	if (shared_memory == (void *)-1)	return;
	SSM = (struct SB_SNMP_STRUCT *)shared_memory;
	snmp = SB_ENABLE;
}
//===============================================================================
void get_portview_memory (void)
{
void *shared_memory = (void *)0;

	portview = SB_DISABLE;
	shared_memory = SB_GetSharedMemory (SB_PORTVIEW_KEY, sizeof (struct SB_PORTVIEW_STRUCT));
	if (shared_memory == (void *)-1)	return;
	PSM = (struct SB_PORTVIEW_STRUCT *)shared_memory;
	portview = SB_ENABLE;
}
****/

//===============================================================================
void mainloop(int protocol)
{
unsigned long CTimer=0, BTimer;
unsigned long DTimer = 0;
unsigned long STimer = 0;
////
int interface;
unsigned long t1; 	////SB_GetTick (Void);
int len1;
int mID;

	close_init ();
	SB_msleep (1000);
	SYS.lfd = SB_BindUdp (SYS.socket);
	if (SYS.lfd <= 0) return;

	if (SYS.sfd <= 0)
	{
		SYS.sfd = SB_OpenSerial (port_no);
		////----------------------------------------S4M
		interface = SB_RS485_NONE_ECHO;
		ioctl (SYS.sfd, INTERFACESEL, &interface);
		////
		if (SYS.sfd <= 0) return;
	}
	SB_InitSerial (SYS.sfd, SYS.bps, SYS.dps, SYS.flow);
	SB_ReadSerial (SYS.sfd, WORK, MAX_BUFFER_SIZE, 0);
	if (SYS.interface == SB_RS232)
	{
		SB_SetDtr (SYS.sfd, SB_ENABLE);
		SB_SetRts (SYS.sfd, SB_ENABLE);
	}


	if ( (port_no > 1) && (CFG_SIO[port_no -2 ].protocol == SB_DISABLE_MODE) &&
				!strncmp( CFG_SYS.device_name, "DRFB", 4) ) 
	{
		if (SYS.sfd2 <= 0)
		{
			SYS.sfd2 = SB_OpenSerial (port_no - 2);
			if (SYS.sfd2 <= 0) return;
		}
		SB_InitSerial (SYS.sfd2, SYS.bps2, SYS.dps2, SYS.flow2);
		SB_ReadSerial (SYS.sfd2, rfWORK, MAX_rfWORK_SIZE, 0);
		if (SYS.interface2 == SB_RS232)
		{
			SB_SetDtr (SYS.sfd2, SB_ENABLE);
			SB_SetRts (SYS.sfd2, SB_ENABLE);
		}
	}


	/****
	if (snmp)
		{
		SSM_CONNECT_STAT = SB_ENABLE;
		SSM_CONNECT_COUNT ++;
		}
	****/
	
	t1 = SB_GetTick ();  ////
	mID=0;
	
	while (1)
	{
		
		if ( (SB_GetTick() - t1) > 300)  ////
		{
			t1 = SB_GetTick ();

			//SB_msleep (20);
			
			//t1 = SB_GetTick ();
		}
		
		if (SYS.bps != 13) SB_msleep (1);
		BTimer = SB_GetTick ();
		if (BTimer < CTimer)						// Check over Tick Counter;  0xffffffff (4 bytes) 
			{
			DTimer = BTimer + 7000;
			STimer = BTimer + 1000;
			}	
		CTimer = BTimer;
		
		if (DTimer < CTimer) 		
			{
			DTimer = CTimer + 7000;		// 7 sec
			sprintf (WORK, "/var/run/debug-%d", port_no);
    		if (access(WORK, F_OK) == 0)  SB_DEBUG = 1; else SB_DEBUG = 0;
    		}	
    		
		if (receive_from_lan () < 0) return;
		if (receive_from_port(protocol) < 0) return;
		//
		if (receive_from_DRFC200 () < 0) return;

		/****
		if (portview)
			{
			if (PSM_PORT.reset_request == SB_ENABLE) return;		// Portview Reset request
			}
		//
		if (snmp)
			{
			if (STimer > CTimer) continue;
			STimer = CTimer + 1000;
			SSM_write ();
			if (SSM_PORTRESET) return;			// SNMP set (reset)				
			}
		****/
	}
}

//===============================================================================
void close_init ()
{
	if (SYS.lfd > 0)
		{
		shutdown(SYS.lfd, SHUT_RDWR);	
		close (SYS.lfd);
		SYS.lfd = 0;
		}

	if (SB_DEBUG) SB_LogMsgPrint ("Close Socket & Serial Handle\n");
}

//===============================================================================
int receive_from_lan(void)
{
int len, sio_len; 

	sio_len = ioctl (SYS.sfd, TIOTGTXCNT);
	if (sio_len < 32) return 0;	
	if (sio_len > MAX_SOCKET_READ_SIZE) sio_len = MAX_SOCKET_READ_SIZE;
	len = SB_ReadUdp (SYS.lfd, WORK, sio_len);
	if (len <= 0) return len;
	
	////~~Not-USE! //SB_SendSerial (SYS.sfd, WORK, len);		
	
	if (SB_DEBUG) SB_LogDataPrint ("L->S", WORK, len); 

	////if (portview) PSM_PORT.status = SB_ACTIVE;	
	////if (portview)  PSM_write ('S', WORK, len);
	
	return 0;
}

//===============================================================================
int receive_from_port (int protocol)  ////<------RS-485
{
int len;
//
int idx;

char s1[256];
//
char *cp1, *cp2;
int p1, p2;  //


	len = SB_ReadSerial (SYS.sfd, WORK, MAX_SERIAL_READ_SIZE, SYS.wait_msec);
	if (len <= 0) return 0;


	WORK[len] = 0;
	
	if ( (WORK[len-1] == '\r') || (WORK[len-1] == '\n') )
	{
		;	//--Nomal--//
	}
	else
	{
		SB_msleep (20);  // wait(~200byte@115200bps)....for...Line-CRLF
		//
		idx = SB_ReadSerial (SYS.sfd, WORK2, MAX_SERIAL_READ_SIZE, SYS.wait_msec);
		//
		if ( idx )	{
			memcpy (&WORK[len], WORK2, idx);
			len = len + idx;
			WORK[len]=0;
		}			
	}


	cp1 = strstr(WORK, CFG_SYS.device_name + 1 );  //cp1 = strstr(WORK,"\n");  *(cp1) = 0;
	//
	sprintf(s1,"%d",SYS.socket);
	cp2 = strstr(WORK, s1 );
	//
	p1 = cp1 - WORK;
	p2 = cp2 - WORK;
	//
	//................"DRFB200_01_4003"......" RFB200_01_4003"..........
	if ( ((p1 >= 0) && (p1 < 2)) && ((p2 >= 10) && (p2 < 12)) )
	{
	  ////sprintf(s1,"[%s_%d]:",CFG_SYS.device_name,SYS.socket );
	  //
	  s1[0]='[';  //sprintf(s1,"[");
	  strncpy(&s1[1], &(CFG_SYS.device_name[strlen(CFG_SYS.device_name)-2]),2);
	  sprintf(&s1[3], "_%02d]", SYS.socket % 100 );
	  
	  
	  //SB_SendSerial (SYS.sfd, s1, strlen(s1));
	  ////
	  //if (rfLEN2 > 0) SB_SendSerial (SYS.sfd, rfWORK2, rfLEN2);
	  
	  memcpy ( &s1[7], rfWORK2, rfLEN2 );
	  //
	  if (rfLEN2 > 0) 	SB_SendSerial (SYS.sfd, s1, rfLEN2 + 7 );
	  else 				SB_SendSerial (SYS.sfd, s1, 7 );
	  
	  ////================>Mon__udp_DRFB~~
	  if (rfLEN2 > 0) 	SB_SendUdpServer (SYS.lfd, s1, rfLEN2 + 7 );
	  else 				SB_SendUdpServer (SYS.lfd, s1, 7 );
	  
	  
	  SB_msleep (30);
	  rfWORK2[0]=0; //_clear_
	  rfLEN2=0;
	}




	//if (1)  ////(protocol == SB_UDP_SERVER_MODE)	// server
	//	SB_SendUdpServer (SYS.lfd, WORK, len);   ////=====================>Mon__udp_DRFB~~
	//// else   //// <-----SB_DISABLE_MODE------------------{udp_DRFG}!!
	////	SB_SendUdpClient (SYS.lfd, WORK, len, SYS.server_ip, SYS.server_port);


	//memcpy (WORK2, WORK, len);
	//SB_SendSerial (SYS.sfd2, WORK2, len);
	//=======================================>> To-RF-Modem!!!
	//SB_msleep (20);
	

	if (SB_DEBUG) SB_LogDataPrint ("S->L", WORK, len); 
	
	////if (portview) PSM_PORT.status = SB_ACTIVE;
	////if (portview) PSM_write ('R', WORK, len);
	
	return 0;
}



//===============================================================================
int receive_from_DRFC200 (void)  ////<------DRFC200 
{
int len;
//
int idx;

char s1[128];
//
char *cp1, *cp2;
int p1, p2;  //


	len = SB_ReadSerial (SYS.sfd2, rfWORK, MAX_rfWORK_SIZE, SYS.wait_msec);
	if (len <= 0) return 0;


	rfWORK[len] = 0;
	
	if ( (rfWORK[len-1] == '\r') || (rfWORK[len-1] == '\n') )
	{
		;	//--Nomal--//
	}
	else
	{
		SB_msleep (20);  // wait(~200byte@115200bps)....for...Line-CRLF
		//
		idx = SB_ReadSerial (SYS.sfd2, WORK2, MAX_rfWORK_SIZE, SYS.wait_msec);
		//
		if ( idx )	{
			memcpy (&rfWORK[len], WORK2, idx);
			len = len + idx;
			rfWORK[len]=0;
		}			
	}

	if (len > 10)
	{
		memcpy ( rfWORK2, rfWORK, len );  rfLEN2=len;
	
		//SB_SendUdpServer (SYS.lfd, rfWORK2, len);   ////=====================>Mon__udp_DRFB~~
		SB_LogDataPrint ("RFin", rfWORK2, len);
	}





	/****
	
	cp1 = strstr(WORK, CFG_SYS.device_name + 1 );  //cp1 = strstr(WORK,"\n");  *(cp1) = 0;
	//
	sprintf(s1,"%d",SYS.socket);
	cp2 = strstr(WORK, s1 );
	//
	p1 = cp1 - WORK;
	p2 = cp2 - WORK;
	//
	//................"DRFB200_01_4003"......" RFB200_01_4003"..........
	if ( ((p1 >= 0) && (p1 < 2)) && ((p2 >= 10) && (p2 < 12)) )
	{
	  sprintf(s1,"\nID=(%s) Sno=(%d) \n\r",CFG_SYS.device_name,SYS.socket);
	  SB_SendSerial (SYS.sfd, s1, strlen(s1));
	  
	  SB_SendUdpServer (SYS.lfd, WORK, len);   ////=====================>Mon__udp_DRFB~~
	}

	****/



	//memcpy (WORK2, WORK, len);
	//SB_SendSerial (SYS.sfd2, WORK2, len);
	//=======================================>> To-RF-Modem!!!
	//SB_msleep (20);
	

	if (SB_DEBUG) SB_LogDataPrint ("RFin", WORK, len); 
	
	////if (portview) PSM_PORT.status = SB_ACTIVE;
	////if (portview) PSM_write ('R', WORK, len);
	
	return 0;
}




/****
//===============================================================================
void PSM_write (char sw, char *data, int len)
{
static int f_err=0, o_err=0, p_err=0;		//    get first error
struct serial_icounter_struct   err_cnt;
int cplen, f_len, l_len;

	if (sw == 'R')
		{
		switch ( PSM_PORT.flag) {
		case 0 : return;
		case 2 :
			if (RX_GET <= RX_PUT)
				cplen = SB_RING_BUFFER_SIZE - RX_PUT + RX_GET - 1;	
			else
				cplen = RX_GET - RX_PUT - 1;
			if (cplen == 0) return;
			if (cplen < len) len = cplen;
			
			if ((RX_PUT+len) < SB_RING_BUFFER_SIZE) 
				{ f_len = len; l_len = 0; }
			else
				{ f_len = SB_RING_BUFFER_SIZE-RX_PUT;  l_len = len - f_len; }

			if (f_len) memcpy (&PSM->rx_buff[RX_PUT], data, f_len);
			if (l_len) 
				{
				memcpy (&PSM->rx_buff[0], &data[f_len], l_len);	
				RX_PUT = l_len; 
				}
			else
				RX_PUT += f_len;	
			
			PSM->rx_lastputtime = SB_GetTick();
		case 1 :		// error count
			ioctl (SYS.sfd, TIOCGICOUNT, &err_cnt);
			PSM_PORT.rx_bytes += len;
			PSM_PORT.frame_err += (err_cnt.frame - f_err);
			f_err = err_cnt.frame;
			PSM_PORT.parity_err += (err_cnt.parity - p_err);
			p_err = err_cnt.parity;
			PSM_PORT.overrun_err += (err_cnt.overrun - o_err);
			o_err = err_cnt.overrun;
			break;
			}	
		}
	else
		{			
		switch ( PSM_PORT.flag) {
		case 0 : return;
		case 2 :
			if (TX_GET <= TX_PUT)
				cplen = SB_RING_BUFFER_SIZE - TX_PUT + TX_GET - 1;	
			else
				cplen = TX_GET - TX_PUT - 1;
			if (cplen == 0) return;
			if (cplen < len) len = cplen;
			
			if ((TX_PUT+len) < SB_RING_BUFFER_SIZE) 
				{ f_len = len; l_len = 0; }
			else
				{ f_len = SB_RING_BUFFER_SIZE-TX_PUT;  l_len = len - f_len; }

			if (f_len) memcpy (&PSM->tx_buff[TX_PUT], data, f_len);
			if (l_len) 
				{
				memcpy (&PSM->tx_buff[0], &data[f_len], l_len);	
				TX_PUT = l_len; 
				}
			else
				TX_PUT += f_len;	
			
			PSM->tx_lastputtime = SB_GetTick();
		case 1 :		// error count
			PSM_PORT.tx_bytes += len;
			break;			
			}	
		}
}
//===============================================================================
void SSM_write ()
{
unsigned char msr;
struct serial_icounter_struct   err_cnt;
static int f_err=0, o_err=0, p_err=0;		//    get first error
static unsigned char msr_shadow = 0;

	msr = ioctl(SYS.sfd, TIOTSMSR) & 0xf0;
	if (msr != msr_shadow) 
		{
		if ((msr&0x10) != (msr_shadow&0x10)) SSM_CTS_CHANGE ++;
		if ((msr&0x20) != (msr_shadow&0x20)) SSM_DSR_CHANGE ++;
		if ((msr&0x80) != (msr_shadow&0x80)) SSM_DCD_CHANGE ++;
		if (msr&0x10) SSM_CTS_STAT = 1; else SSM_CTS_STAT = 0;
		if (msr&0x20) SSM_DSR_STAT = 1; else SSM_DSR_STAT = 0;
		if (msr&0x80) SSM_DCD_STAT = 1; else SSM_DCD_STAT = 0;		
		msr_shadow = msr;
		}

	ioctl (SYS.sfd, TIOCGICOUNT, &err_cnt);
	SSM_FRAMING_ERRS += (err_cnt.frame   - f_err);  f_err = err_cnt.frame;
	SSM_PARITY_ERRS  += (err_cnt.parity  - p_err);  p_err = err_cnt.parity;
	SSM_OVERRUN_ERRS += (err_cnt.overrun - o_err);  o_err = err_cnt.overrun;
}		
****/

