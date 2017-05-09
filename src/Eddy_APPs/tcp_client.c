/*****************************************************************************
Copyright (c) 2007 SystemBase Co., LTD  All Rights Reserved.

Tcp_client 		09, 31, 2007. yhlee
******************************************************************************/
#include "include/sb_include.h"
#include "include/sb_define.h"
#include "include/sb_shared.h"
#include "include/sb_config.h"
#include "include/sb_extern.h"
#include "include/sb_ioctl.h"

#include "include/sb_misc.h"  ////...config_access...Eddy_2.5


#define MAX_BUFFER_SIZE 		2048  //4096  //2048
#define MAX_SOCKET_READ_SIZE 	256   //1024  //512
#define MAX_SERIAL_READ_SIZE 	256   //1024  //512


//-----------------------------------------------------(Config for S4M filesystem 2.5.1.2)
//struct SB_SIO_CONFIG			cfg [SB_MAX_SIO_PORT];
//
struct SB_SYSTEM_CONFIG			CFG_SYS;						// /flash/eddy_system.cfg
struct SB_SIO_CONFIG			CFG_SIO [SB_MAX_SIO_PORT];		// /flash/eddy_sio.cfg
struct SB_SNMP_CONFIG			CFG_SNMP;						// /flash/eddy_snmp.cfg
struct SB_GPIO_CONFIG			CFG_GPIO;						// /flash/eddy_gpio.cfg
struct SB_DIO_CONFIG			CFG_DIO;						// /flash/eddy_dio.cfg
struct SB_WIFI_CONFIG			CFG_WIFI;						// /flash/eddy_wifi.cfg
//-----------------------------------------------------(Config for S4M filesystem 2.5.1.2)


struct sys_info  
{
	int				sfd;         //!LinkGate_LCD_OUT_Port~~!(Com1)==>:mRIO~COM2
	int				lfd;
	int				port_no;
	int				wait_msec;
	char  			bps;
	char 			dps;
	char 			flow;
	char			device;
	char 			server_ip [100];
	int				server_port;
	char			interface;
	unsigned long 	alivetime;
	unsigned long 	rcv_timer;
	
	
	//------------------------------!LinkGate_LCD_OUT_Port~~!(Com2)==>:mRIO~COM1
	int				sfd2;
	char  			bps2;
	char 			dps2;
	char 			flow2;
	char			interface2;
	//------------------------------!LinkGate_LCD_OUT_Port~~!
	
};	

struct sys_info  SYS;

int port_no;
int portview;
int snmp;
struct SB_PORTVIEW_STRUCT *PSM;
struct SB_SNMP_STRUCT	  *SSM;
char WORK [MAX_BUFFER_SIZE];

char Remainder [MAX_BUFFER_SIZE];
int  Remainder_len;

int	SB_DEBUG	=	0;


char RingCH [4] = { '|','/','-', 0x5C };  // ~'\\'~ };
//char RingCH [4] = { '|','*','-', '*' };  // ~'\\'~ };
//char RingCH [4] = { '^','>','v', '<' };  // ~'\\'~ };
//char RingCH [4] = { '\'',';',',', '.' };  // ~'\\'~ };
//char RingCH [4] = { '.','o','O', ' ' };  // ~'\\'~ };
int  RingCnt=0;

int lpCNT;  //..LoopCNT..WDT..
int lpCNTx;  //..LoopCNT..WDT..for-CONN-Fail!


////struct SB_CONFIG cfg;  ////<--------OLD(Eddy_22a)

void get_snmp_memory ();
void get_portview_memory ();
void SSM_write ();
void PSM_write (char sw, char *data, int len);

void close_init ();
void mainloop(void);
int receive_from_lan(void);
int receive_from_port(void);

//<----------------------------------------------------
void uif_clr ();
void uif_xy (int x, int y);
void uif_xyPut (int x, int y, char ch);
void uif_xySTR (int x, int y, char *str);
//<----------------------------------------------------


int receive_from_UIF(void);

char LinkGate_name [4][10];
char LinkGate_ip [4][30];
int LinkGate_id;

//===============================================================================	
int main (int argc, char *argv[])
{
	//Not-USE@2.5  //SB_SetPriority (41); //(5)~!			// Min 1 ~ Max 99 

	////-----------------------------------------------------(Config for S4M filesystem 2.5.1.2)
	//
	//SB_ReadConfig (CFGFILE_ETC_SIO, (char *)&cfg[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);
	//
	//------------------------- Read /etc Config Data
	SB_ReadConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, 		sizeof (struct SB_SYSTEM_CONFIG));
	SB_ReadConfig  (CFGFILE_ETC_SIO,     (char *)&CFG_SIO[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
	SB_ReadConfig  (CFGFILE_ETC_SNMP,    (char *)&CFG_SNMP,		sizeof(struct SB_SNMP_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_GPIO,    (char *)&CFG_GPIO,		sizeof(struct SB_GPIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_DIO,     (char *)&CFG_DIO,		sizeof(struct SB_DIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_WIFI,    (char *)&CFG_WIFI,		sizeof(struct SB_WIFI_CONFIG));
	////-----------------------------------------------------(Config for S4M filesystem 2.5.1.2)
	

	port_no = atoi (argv[1]) - 1;			// port no 1 ~ 16
	
	signal(SIGPIPE, SIG_IGN);
	
	get_portview_memory ();
	get_snmp_memory ();

	SYS.lfd = 0;
	SYS.sfd = 0;
	
	//<------------
	SYS.sfd2 = 0;
	//<------------
	

	while (1) 
	{
		////SB_ReadConfig (SB_CFG_FILE, &cfg);
		SB_ReadConfig (CFGFILE_ETC_SIO, (char *)&CFG_SIO[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
		
		/****(OLD)
		SYS.bps		 	= cfg.sio[port_no].speed;
		SYS.dps		 	= cfg.sio[port_no].dps;
		SYS.flow		= cfg.sio[port_no].flow;
		SYS.wait_msec	= cfg.sio[port_no].bypass;
		SYS.alivetime	= cfg.sio[port_no].keepalive * 1000; 	
		SYS.server_port	= cfg.sio[port_no].remote_socket_no;
		SYS.interface	= cfg.sio[port_no].interface;
		SYS.device		= cfg.sio[port_no].device; 
		if (SYS.wait_msec == SB_ENABLE)
			SYS.wait_msec = 0;
		else
			SYS.wait_msec = SB_GetDelaySerial (SYS.bps);
		****/
		SYS.bps		 	= CFG_SIO[port_no].speed;
		SYS.dps		 	= CFG_SIO[port_no].dps;
		SYS.flow		= CFG_SIO[port_no].flow;
		SYS.interface	= CFG_SIO[port_no].interface;
		SYS.wait_msec	= CFG_SIO[port_no].packet_latency_time;  ////cfg.sio[port_no].bypass;
		SYS.alivetime	= CFG_SIO[port_no].keepalive * 1000; 	
		////SYS.socket		= CFG_SIO[port_no].socket_no;
		//
		SYS.device		= CFG_SIO[port_no].device;
		SYS.server_port	= CFG_SIO[port_no].remote_socket_no;  //..for..tcp_client..
		//
		sprintf (SYS.server_ip, "%d.%d.%d.%d", 
		CFG_SIO[port_no].remote_ip[0],CFG_SIO[port_no].remote_ip[1], CFG_SIO[port_no].remote_ip[2], CFG_SIO[port_no].remote_ip[3]);

		//<-----------------------------------------------------------------------------------------------!
		if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOslave", 8) )
		{
			//-------(for SYS.sfd2)-----------------!LinkGate_LCD_OUT_Port~~!(Com2)==>:mRIO~COM1
			SYS.bps2		= CFG_SIO[0].speed;
			SYS.dps2		= CFG_SIO[0].dps;
			SYS.flow2		= CFG_SIO[0].flow;
			SYS.interface2	= CFG_SIO[0].interface;
		}
		//<-----------------------------------------------------------------------------------------------!
		
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

		if (SB_DEBUG) 
			SB_LogMsgPrint ("TCP_Client port=%d, Speed = %d, dps = %d, flow = %d, MRU=%d, Alive=%d, IP=%s(%d), device=%d\n",
			port_no+1, SYS.bps, SYS.dps, SYS.flow, SYS.wait_msec, SYS.alivetime, SYS.server_ip, SYS.server_port, SYS.device);
		
		mainloop();
	}
}
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
//===============================================================================
void mainloop(void)
{
int  cnt, ret;
unsigned long CTimer=0, BTimer;
unsigned long DTimer = 0;
unsigned long STimer = 0;

//<----------------------------------------------------
struct in_addr addr;

//@
unsigned long lpTimer;
unsigned long lpTimerC;

//char *endp;  //...for strtod( ,&endp)
char endp[2];

	close_init ();
	
	SB_msleep (100);
	
	if (SYS.sfd <= 0)
		{
		SYS.sfd = SB_OpenSerial (port_no);
		if (SYS.sfd <= 0) return;
		}
	SB_InitSerial (SYS.sfd, SYS.bps, SYS.dps, SYS.flow);
	SB_ReadSerial (SYS.sfd, WORK, MAX_BUFFER_SIZE, 0);
	
	/****
	if (SYS.device == SB_MODEM_TYPE)
		{
		SB_SetDtr (SYS.sfd, SB_DISABLE);
		SB_SetRts (SYS.sfd, SB_DISABLE);
		SB_msleep (30);
		}
	****/
	
	if (SYS.interface == SB_RS232)
		{
		SB_SetDtr (SYS.sfd, SB_ENABLE);
		SB_SetRts (SYS.sfd, SB_ENABLE);
		}

	/****
	if (SYS.device == SB_MODEM_TYPE)
		{
		while (1)
			{
			SB_msleep (1000);
			if (SB_GetMsr (SYS.sfd) & 0x80) break;
			}
		}
	****/

	//<-----------------------------------------------------------------------------------------------!
	if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOslave", 8) )
	{
		//-------(for SYS.sfd2)-----------------mRIO::COM1
		if (SYS.sfd2 <= 0)
		{
			SYS.sfd2 = SB_OpenSerial (0);  //<--mRIO::COM1...UIF-LCD~
			if (SYS.sfd2 <= 0) return;
		}
		SB_InitSerial (SYS.sfd2, SYS.bps2, SYS.dps2, SYS.flow2);
		SB_ReadSerial (SYS.sfd2, WORK, MAX_BUFFER_SIZE, 0);
		if (SYS.interface2 == SB_RS232)
		{
			SB_SetDtr (SYS.sfd2, SB_ENABLE);
			SB_SetRts (SYS.sfd2, SB_ENABLE);
		}
		
		addr.s_addr = SB_GetIp ("eth0");
		//@@@@@@@@@@@@@@init_UIF
		uif_clr ();
		uif_clr ();
		uif_xy (0,0);
		sprintf (WORK, " MultiLink_RIO V1.0 ");
		SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));	
		uif_xy (0,1);
		sprintf (WORK, "myIP:%s ", inet_ntoa(addr) );
		SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));
		uif_xy (0,2);
		sprintf (WORK, "OUTPUT#1234567890 " );
		///////////////" OnOff:0101010101
		SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));
		//
		uif_xy (0,3);
		sprintf (WORK, "Init..:" );
		SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));		
		//@@@@@@@@@@@@@
	}
	//<-----------------------------------------------------------------------------------------------!
	

	for (cnt=0; cnt<10; cnt++)
	{
		ret = SB_ConnectTcp (SYS.server_ip, SYS.server_port, 3, 8, 8);
		if (ret > 0) break;
		SB_msleep (2000);
		
		if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOslave", 8) )
		{	//..retry..
			uif_xy (0,3);
			sprintf (WORK, "Retry_Connect...." );
			SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));
		}
	}
	
	if (ret <= 0)
	{
		if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOslave", 8) )
		{	//..CannotCONN..
			uif_xy (0,3);
			sprintf (WORK, "Cannot_Connect..." );
			SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));
		}
		return;
	}
	
	SYS.lfd = ret;		

	//...Connect-OK..!

	//<----------------------------------------------------------------------------------------!
	if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOslave", 8) )
	{	
		uif_xy (0,3);
		sprintf (WORK, "LinkOK:          " );
		SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));
	}	//<------------------------------------------------

	if (portview) PSM_PORT.status = SB_ACTIVE;
	if (snmp)	  
		{
		SSM_CONNECT_STAT = SB_ENABLE;
		SSM_CONNECT_COUNT ++;
		}

	SYS.rcv_timer = SB_GetTick() + SYS.alivetime;
	CTimer = 0;
	Remainder_len = 0;

	lpCNT=0;  lpCNTx=0;
	lpTimerC = SB_GetTick ();
	lpTimer = lpTimerC + 100;
	
	while (1)
	{	
		if (SYS.bps != 13) SB_msleep (1);
		BTimer = SB_GetTick ();
		if (BTimer < CTimer)						// Check over Tick Counter;  0xffffffff (4 bytes) 
			{
			DTimer = BTimer + 7000;
			STimer = BTimer + 1000;
			SYS.rcv_timer = BTimer + SYS.alivetime;
			}	
		CTimer = BTimer;

		if (DTimer < CTimer) 		
			{
			DTimer = CTimer + 7000;		// 7 sec
			sprintf (WORK, "/var/run/debug-%d", port_no);
    		if (access(WORK, F_OK) == 0)  SB_DEBUG = 1; else SB_DEBUG = 0;
    		}	
    		
		if (SYS.alivetime)
			{
			if (SYS.rcv_timer < CTimer)	return;
			}

		//@ loop-Timer @//
		if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOslave", 8) )
		{
			if ( lpTimerC > SB_GetTick () ) // Check over Tick Counter;  0xffffffff (4 bytes)
			{
				lpTimerC = SB_GetTick ();
				lpTimer = lpTimerC + 100;
			}
			else {
				lpTimerC = SB_GetTick ();
			}
			
			if ( lpTimer < lpTimerC )
			{
				lpTimerC = SB_GetTick ();
				lpTimer = lpTimerC + 100;
				//
				lpCNT++;  //--->WDT~~
				//
				if( lpCNT > 4 ) //...0.5sec...//
				{
					uif_xy (7,3);
					SB_SendSerial (SYS.sfd2, "No_DATA_RX",10); //---COM1---
					lpCNT=0;
					//
					lpCNTx++;
					if( lpCNTx > 15 ) return; //-----(Conn_Fail_Exit~)!!
				}
				
				uif_xy (19,3);
				//*endp = RingCH[RingCnt];
				endp[0] = RingCH[RingCnt];
				SB_SendSerial (SYS.sfd2, endp, 1);  fflush(0);
				if ( RingCnt++ >= 3 ) RingCnt=0;				
			}
		}
		//@

		if (receive_from_lan () < 0) return;
		if (receive_from_port() < 0) return;

		//<-----------------------------------------------------------------------------------------------!
		if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOslave", 8) )
		{
			if (receive_from_UIF() < 0) return;
		}	//<------------------------------------------------

		if (portview)
			{
			if (PSM_PORT.reset_request == SB_ENABLE) return;		// Portview Reset request
			}

		if (snmp)
			{
			if (STimer > CTimer) continue;
			STimer = CTimer + 1000;
			SSM_write ();
			if (SSM_PORTRESET) return;			// SNMP set (reset)				
			}
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

//
//int len2;
//char WORK2 [32];  //MAX_BUFFER_SIZE];
//

	sio_len = ioctl (SYS.sfd, TIOTGTXCNT);
	if (sio_len < 32) return 0;	 //<--------------??
	if (sio_len > MAX_SOCKET_READ_SIZE) sio_len = MAX_SOCKET_READ_SIZE;
	len = SB_ReadTcp (SYS.lfd, WORK, sio_len);
	if (len <= 0) return len;


	//<-----------------------------------------------------------------------------------------------!
	if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOslave", 8) )
	{
		if ( (WORK[0] == ':') && ((WORK[13]=='\n')||(WORK[13]=='\r')) )
		{
			//Do::Check-SUM~~~
			
			//sprintf (WORK, "INPUT:12345678901s## " );
			/////////////////"OnOff:0101010101
			uif_xy (7,3);
			SB_SendSerial (SYS.sfd2, &WORK[1], 10); //---COM1---
			
			lpCNT=0;  //<---Clear!~~WDT~~
			lpCNTx=0; //<---Con Fail Clear~~
		}
	}	//<------------------------------------------------
	


	SB_SendSerial (SYS.sfd, WORK, len);		
	if (SB_DEBUG) SB_LogDataPrint ("L->S", WORK, len); 
	if (portview)  PSM_write ('S', WORK, len);
	SYS.rcv_timer = SB_GetTick()+SYS.alivetime;
	return 0;
} 
//===============================================================================
int receive_from_port(void)
{
int len, ret;

//
////int len2;
//char WORK2 [32];  //MAX_BUFFER_SIZE];
//

	/****
	if (SYS.device == SB_MODEM_TYPE)
		{
		if ((SB_GetMsr (SYS.sfd) & 0x80) == 0x00) return -1;
		}
	****/
	
	if (Remainder_len)
		{
		memcpy (WORK, Remainder, Remainder_len);
		len = Remainder_len;
		Remainder_len = 0;
		}
	else
		{
		len = SB_ReadSerial (SYS.sfd, WORK, MAX_SERIAL_READ_SIZE, SYS.wait_msec);
		if (len <= 0) return 0;		
		}
	
	ret = write (SYS.lfd, WORK, len);
	if (ret != len)
		{
		if (ret < 0) ret = 0;
		Remainder_len = len - ret;	
		memcpy (Remainder, &WORK[ret], Remainder_len);
		if (SB_DEBUG) SB_LogMsgPrint ("S->L Remainder Len=%d\n", Remainder_len); 
		}

	if (ret)
		{
		if (SB_DEBUG) SB_LogDataPrint ("S->L", WORK, ret); 
		if (portview) PSM_write ('R', WORK, ret);
		}
	return 0;
}

//===============================================================================

int receive_from_UIF(void)
{
int len;
//int ret;

//
//int len2;
char WORK2 [32];  //MAX_BUFFER_SIZE];
//
//char dd;


	len = SB_ReadSerial (SYS.sfd2, WORK2, 1, 0);
	if (len <= 0) return 0;

/***	
	dd = WORK2[0];
	
	if ( (dd == 0x01) || (dd == 0x06) || (dd == 0x0b) )
	{
		if (dd == 0x01)  LinkGate_id = 0;  //=F1
		if (dd == 0x06)  LinkGate_id = 1;  //=F2
		if (dd == 0x0b)  LinkGate_id = 2;  //=F3
		//if (dd == 0x0b)  //=F3
		//if (dd == 0x10)  //=F4

		uif_clr();
		
		uif_xy (0,0);
		sprintf (WORK2, "%2X", dd );
		SB_SendSerial (SYS.sfd2, WORK2, strlen(WORK2));
		
		uif_xy (3,0); 	sprintf (WORK2, "F%1d--Selected!", LinkGate_id + 1 );
						SB_SendSerial (SYS.sfd2, WORK2, strlen(WORK2));
		uif_xy (0,2); 	sprintf (WORK2, ">>Wait for ReCONNECT" );
						SB_SendSerial (SYS.sfd2, WORK2, strlen(WORK2));

		SB_msleep (2000);

		return -1;  //====>>RECONNECT!!
	}
***/

	return 0;
}

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



//===============================================================================
//==(LinkGate)==>


//Const Byte SCANCODETB = (&HFF,19,15,1,2,3,20,16,4,5,6,21,17,7,8,9,22,18,10,0,11,&HFF,&HFF,12,13,14)

/**
		01234567890123456789
		--------------------
		Dist:999.99 * SK:12
		X:123456.1234 RT:0
		Y:123456.1234 RQ:4
		
		--------------------
**/


void uif_clr ()  
{
char chD[8];
		
		chD[0] = 0x1B;  //27;  //ESC
		chD[1] = 0x43;  //"C";  //clr
		SB_SendSerial (SYS.sfd2, chD, 2);  fflush(0);  SB_msleep (30);  //20
}

void uif_xy (int x, int y)  // x:0~19 y:0~3 //
{
char chD[8];

		chD[0] = 0x1B;  //27;  //ESC
		chD[1] = 0x4C;  //"L";  //Locate
		chD[2] = x; //X
		chD[3] = y; //Y
		
		SB_SendSerial (SYS.sfd2, chD, 4);  fflush(0);  //SB_msleep (1);  //fflush(SYS.sfd);
}

void uif_xyPut (int x, int y, char ch)  // x:0~19 y:0~3 //
{
char chD[8];

		chD[0] = 0x1B;  //27;  //ESC
		chD[1] = 0x4C;  //"L";  //Locate
		chD[2] = x; //X
		chD[3] = y; //Y
		chD[4] = ch;
		
		SB_SendSerial (SYS.sfd2, chD, 5);  fflush(0);  //SB_msleep (1);  //fflush(SYS.sfd);
}

void uif_xySTR (int x, int y, char *str)  // x:0~19 y:0~3 //
{
		uif_xy (x,y);
		SB_SendSerial (SYS.sfd2, str, strlen(str));  fflush(0);
}

//===============================================================================
//==(((END)))==//


	
