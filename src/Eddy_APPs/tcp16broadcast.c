/*****************************************************************************
Copyright (c) 2007 SystemBase Co., LTD  All Rights Reserved.

Serial (1 port) to LAN (5 Sessions) Broadcast		09, 31, 2007. yhlee
******************************************************************************/
#include "include/sb_include.h"
#include "include/sb_define.h"
#include "include/sb_shared.h"
#include "include/sb_config.h"
#include "include/sb_extern.h"
#include "include/sb_ioctl.h"

#include "include/sb_misc.h"  ////...config_access...Eddy_2.5


#define MAX_BUFFER_SIZE 		2058
#define MAX_SOCKET_READ_SIZE 	512
#define MAX_SERIAL_READ_SIZE 	512
#define MAX_CONNECT				16    //....5@org



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
	int				sfd;
	int				listen_fd;
	int				lfd[MAX_CONNECT];
	int				port_no;
	int				wait_msec;
	int				interface;
	int				socket;	
	char  			bps;
	char 			dps;
	char 			flow;
	char			rs_type;
	unsigned long 	alivetime;
	unsigned long 	rcv_timer[MAX_CONNECT];
	int				connect_cnt;
};	

struct sys_info  SYS;

int port_no;
int portview;
int snmp;
struct SB_PORTVIEW_STRUCT *PSM;
struct SB_SNMP_STRUCT	  *SSM;
char WORK [MAX_BUFFER_SIZE];

int	SB_DEBUG	=	0;

////struct SB_CONFIG cfg;  ////<--------OLD(Eddy_22a)

void mainloop(void);
int receive_from_lan(int no);
int receive_from_port();

void close_init (int no);


/****
void get_snmp_memory ();
void get_portview_memory ();
int PA_login();

void SSM_write ();
void PSM_write (char sw, char *data, int len);
****/

//===============================================================================	
int main (int argc, char *argv[])
{
int no;

	//Not-USE@2.5  //SB_SetPriority (5);	// Min 1 ~ Max 99 

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

	//get_portview_memory ();
	//get_snmp_memory ();
	
	SYS.sfd = 0;
	SYS.connect_cnt = 0;
	for (no=0; no<MAX_CONNECT; no++) SYS.lfd[no] = 0;
	
	while (1) 
		{
		////SB_ReadConfig (SB_CFG_FILE, &cfg);
		SB_ReadConfig (CFGFILE_ETC_SIO, (char *)&CFG_SIO[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
		
		/****(OLD)
		SYS.bps		 	= cfg.sio[port_no].speed;
		SYS.dps		 	= cfg.sio[port_no].dps;
		SYS.flow		= cfg.sio[port_no].flow;
		SYS.interface	= cfg.sio[port_no].interface;
		SYS.wait_msec	= cfg.sio[port_no].bypass;
		SYS.alivetime	= cfg.sio[port_no].keepalive * 1000; 	
		SYS.socket		= cfg.sio[port_no].socket_no;
		****/
		SYS.bps		 	= CFG_SIO[port_no].speed;
		SYS.dps		 	= CFG_SIO[port_no].dps;
		SYS.flow		= CFG_SIO[port_no].flow;
		SYS.interface	= CFG_SIO[port_no].interface;
		SYS.wait_msec	= CFG_SIO[port_no].packet_latency_time;  ////cfg.sio[port_no].bypass;
		SYS.alivetime	= CFG_SIO[port_no].keepalive * 1000; 	
		SYS.socket		= CFG_SIO[port_no].socket_no;
		
		/****
		if (SYS.wait_msec == SB_ENABLE)
			SYS.wait_msec = 0;
		else	
			SYS.wait_msec = SB_GetDelaySerial (SYS.bps);
		****/
		
		/****
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
		****/

		if (SB_DEBUG) 
			SB_LogMsgPrint ("Tcp_Server port=%d, Speed = %d, dps = %d, flow = %d, MRU=%d, Alive=%d\n",
			port_no+1, SYS.bps, SYS.dps, SYS.flow, SYS.wait_msec, SYS.alivetime);
		
		mainloop();
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
void mainloop(void)
{
int no, ret;
unsigned long CTimer=0, BTimer;
unsigned long DTimer = 0;
unsigned long STimer = 0;

	SB_msleep (100);
	SYS.listen_fd = SB_ListenTcp (SYS.socket,4, 4);
	if (SYS.listen_fd <= 0) return;

	if (SYS.sfd <= 0)
		{
		SYS.sfd = SB_OpenSerial (port_no);
		if (SYS.sfd <= 0) return;
		}
	SB_InitSerial (SYS.sfd, SYS.bps, SYS.dps, SYS.flow);
	SB_ReadSerial (SYS.sfd, WORK, MAX_BUFFER_SIZE, 0);
	if (SYS.interface == SB_RS232)
		{
		SB_SetDtr (SYS.sfd, SB_ENABLE);
		SB_SetRts (SYS.sfd, SB_ENABLE);
		}
	
	while (1)
		{	
		if (SYS.bps != 13) SB_msleep (1);
		BTimer = SB_GetTick ();
		if (BTimer < CTimer)						// Check over Tick Counter;  0xffffffff (4 bytes) 
			{
			DTimer = BTimer + 7000;
			STimer = BTimer + 1000;
			for (no=0; no<MAX_CONNECT; no++)
				SYS.rcv_timer[no] = BTimer + SYS.alivetime;
			}	
		CTimer = BTimer;
		
		if (DTimer < CTimer) 		
			{
			DTimer = CTimer + 7000;		// 7 sec
			sprintf (WORK, "/var/run/debug-%d", port_no);
    		if (access(WORK, F_OK) == 0)  SB_DEBUG = 1; else SB_DEBUG = 0;
    		}	
   		
		ret = SB_AcceptTcpMulti (SYS.listen_fd, 1);
		if (ret > 0)
			{
			if (++SYS.connect_cnt > MAX_CONNECT) 
				{
				--SYS.connect_cnt;
				close (ret);
				
					////<---------------------------Add:CLOSE-All!!
					for (no=0; no<MAX_CONNECT; no++)
					{
						close_init (no);
					}
					////<---------------------------Add:CLOSE-All!!
				}
			else
				{
				for (no=0; no<MAX_CONNECT; no++)
					{
					if (SYS.lfd[no] == 0) 
						{
						SYS.lfd[no] = ret;
						SYS.rcv_timer[no] = CTimer + SYS.alivetime;
						
						/****
						if (portview) PSM_PORT.status = SB_ACTIVE;
						if (snmp)	  
							{
							SSM_CONNECT_STAT = SB_ENABLE;
							SSM_CONNECT_COUNT ++;
							}
						****/
						break;
						}		
					}
				}
			}

		for (no=0; no<MAX_CONNECT; no++)
			{
			if (SYS.lfd[no] <= 0) continue;
			if (SYS.alivetime) 
				{
				if (SYS.rcv_timer[no] < CTimer) 
					{
					close_init (no);
					continue;
					}
				}

			if (receive_from_lan (no) < 0) 
				{
				close_init (no);
				continue;
				}

			receive_from_port();
			
			/****
			if (portview)
				{
				if (PSM_PORT.reset_request == SB_ENABLE) 		// Portview Reset request
					{
					shutdown (SYS.listen_fd, SHUT_RDWR);
					close (SYS.listen_fd);
					for (no=0; no<MAX_CONNECT; no++) close_init (no);
					return;
					}
				}
			****/
			
			}
			
		/****
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
void close_init (int no)
{
	if (SYS.lfd[no] > 0)
		{
		shutdown(SYS.lfd[no], SHUT_RDWR);	
		close (SYS.lfd[no]);
		SYS.lfd[no] = 0;
		if (--SYS.connect_cnt < 0)  SYS.connect_cnt = 0;
		}
	
	/****
	if (SYS.connect_cnt==0)
		{
		if (portview) PSM_PORT.status = SB_ENABLE;
		if (snmp) SSM_CONNECT_STAT = SB_DISABLE;	
		}		
	****/
	
	if (SB_DEBUG) SB_LogMsgPrint ("Close Socket no=%d\n", no);
}	
//===============================================================================
int receive_from_lan(int no)
{
int len, sio_len; 

	sio_len = ioctl (SYS.sfd, TIOTGTXCNT);
	if (sio_len < 32) return 0;	
	if (sio_len > MAX_SOCKET_READ_SIZE) sio_len = MAX_SOCKET_READ_SIZE;
	len = SB_ReadTcp (SYS.lfd[no], WORK, sio_len);
	if (len <= 0) return len;
	
	SB_SendSerial (SYS.sfd, WORK, len);		
	if (SB_DEBUG) SB_LogDataPrint ("L->S", WORK, len); 
	////if (portview && no==0)  PSM_write ('S', WORK, len);
	SYS.rcv_timer[no] = SB_GetTick()+SYS.alivetime;
	return 0;
} 
//===============================================================================
int receive_from_port()
{
int len, no;

//<------
int idx;
char WORK2 [MAX_BUFFER_SIZE];
char *s1;
char *s2;
//<------

////_test_
/****
	sprintf(WORK2,"12345678901234567890abcdefghijklmnopqrstuvwxyz");
	len = strlen(WORK2);
	for (no=0; no<MAX_CONNECT; no++)
	{
		if (SYS.lfd[no] <= 0) continue;	
		write (SYS.lfd[no], WORK2, len);
	}
****/
////_test_


	len = SB_ReadSerial (SYS.sfd, WORK, MAX_SERIAL_READ_SIZE, SYS.wait_msec);
	if (len <= 0) return 0;

  //<------
	if(1)
	{
		WORK[len]=0;
		
		s1 = strchr(WORK, '\r');
		s2 = strchr(WORK, '\n');
		
		//if ( (WORK[len-1] == '\r') || (WORK[len-1] == '\n') )
		if ( ( s1 > (WORK + 1) ) || ( s2 > (WORK + 1) ) )
		{
			;	//--Nomal--//
		}
		else
		{
			SB_msleep (20);  // wait(~200byte@115200bps)....for...Line-CRLF
			//
			idx = SB_ReadSerial (SYS.sfd, WORK2, MAX_SERIAL_READ_SIZE, SYS.wait_msec);
			//
			if ( idx )
			{
				memcpy (&WORK[len], WORK2, idx);
				len = len + idx;
				//
				WORK[len]=0;
			}	
			
		}
		
	}
  //<------
	
	
	for (no=0; no<MAX_CONNECT; no++)
		{
		if (SYS.lfd[no] <= 0) continue;	
		write (SYS.lfd[no], WORK, len);
		////if (portview && no==0) PSM_write ('R', WORK, len);
		}
	if (SB_DEBUG) SB_LogDataPrint ("S->L", WORK, len); 
	return 0;
}

//===============================================================================
/****
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
****/
//===============================================================================
/****
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


