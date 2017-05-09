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

// #include <math.h>    ////-DVSCPU- VisualSpeedSensor


#define MAX_BUFFER_SIZE 		2058
#define MAX_SOCKET_READ_SIZE 	512
#define MAX_SERIAL_READ_SIZE 	512
#define MAX_CONNECT				32    //....5@org
//
#define MAX_CONNECT_UDP			10    //---tcp32udp9--(1~9)



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
	
	//--------------------------------tcp32udp9--
	int 			UDP_comPort;
	int 			UDP_hosts;
	char 			UDP_ip [MAX_CONNECT_UDP][32];
	int				UDP_port [MAX_CONNECT_UDP];
	int				UDP_Lport [MAX_CONNECT_UDP];
	//
	int				UDP_lfd[MAX_CONNECT_UDP];
	//
	char 			msg[130];
	//------------------------------(PJK2GDC)--
	int 			PJK2GDCcomPort ;
	
	
	//-------------------------------!tcp32udp9_mRIO_UIF_ComPort~~!   (Com1)
	//-------------------------------!tcp32udp9_mRIO_I/O_ComPort~~!   (Com2) => "sfd"
	int				sfd2;  //// => "Com1"~~~UIF-LCD!!!
	char  			bps2;
	char 			dps2;
	char 			flow2;
	char			interface2;
	
};	

struct sys_info  SYS;

int port_no;
int portview;
int snmp;
struct SB_PORTVIEW_STRUCT *PSM;
struct SB_SNMP_STRUCT	  *SSM;
char WORK [MAX_BUFFER_SIZE];

int	SB_DEBUG	=	0;


char RingCH [4] = { '|','/','-', 0x5C };  // ~'\\'~ };
//char RingCH [4] = { '|','*','-', '*' };  // ~'\\'~ };
//char RingCH [4] = { '^','>','v', '<' };  // ~'\\'~ };
//char RingCH [4] = { '\'',';',',', '.' };  // ~'\\'~ };
//char RingCH [4] = { '.','o','O', ' ' };  // ~'\\'~ };
int  RingCnt=0;

int lpCNT;  //..LoopCNT..WDT..

//-------------------------------------(PJK2GDC)--//
char WORK_2 [MAX_BUFFER_SIZE];

static char gdcA[17][15];

static double Xcur1;
static double Ycur1;

//static char gdcTime[15];
static char gdcX[15];
static char gdcY[15];

static double Height1;
static char pjkYEAR[5];
static char pjkDATE[5];

int gdcPass(char *p);

int pjk2gdc(int len);
//-------------------------------------(PJK2GDC)--//



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


void uif_clr ();
void uif_xy (int x, int y);
void uif_xyPut (int x, int y, char ch);
void uif_xySTR (int x, int y, char *str);


//void UDP_hosts_init(void);

void UDP_hosts_init(void)
{
FILE *fd;
char cmd[130];
int i,n;

	/*****************************************(UDPhosts.conf)!
	UDPcomPort 1  ##-1-2-3-4!
	UDPhosts 3    ##-Max~~~9!
	UDPhost1 192.168.0.220 6001 61001  ##SR001
	UDPhost2 192.168.0.220 6002 61002  ##SR002
	UDPhost3 192.168.0.220 6003 61003  ##SR003
	##----iD --IPaddress- Rport Lport #<======
	##----------------------------------------
	PJK2GDCcomPort 1  ##-1-2-3-4! 			//(tcp32broadcastU9_201505_GDM1000R-PJK2GDC_A04.c)//
	##----------------------------------------
	******************************************/

	sprintf(cmd, "/bin/cp /flash/UDPhosts.conf /etc/UDPhosts.conf "); 	system(cmd);
	sprintf(cmd, "/usr/bin/dos2unix /etc/UDPhosts.conf "); 				system(cmd);
	sprintf(cmd, "/bin/chmod 777 /etc/UDPhosts.conf "); 				system(cmd);
	sprintf(cmd, "/bin/cp /etc/UDPhosts.conf /tmp/UDPhosts.conf"); 		system(cmd);

	SYS.UDP_comPort = 0;
	SYS.PJK2GDCcomPort = 0;

	if ((fd = fopen("/etc/UDPhosts.conf", "r")) != NULL)
	{
		fclose(fd);
		
		sprintf(cmd, "/bin/grep UDPhosts /etc/UDPhosts.conf | cut -f2 -d' ' > /tmp/udpHosts "); 
		system(cmd);
		if ((fd = fopen("/tmp/udpHosts", "r")) != NULL)
		{	if (fgets(cmd, 10, fd) != NULL)	 SYS.UDP_hosts = atoi(cmd); }
		fclose(fd);
		
		sprintf(cmd, "/bin/grep UDPcomPort /etc/UDPhosts.conf | cut -f2 -d' ' > /tmp/udpComPort "); 
		system(cmd);
		if ((fd = fopen("/tmp/udpComPort", "r")) != NULL)
		{	if (fgets(cmd, 10, fd) != NULL)	 SYS.UDP_comPort = atoi(cmd); }
		fclose(fd);
		//
		if ( port_no != (SYS.UDP_comPort - 1)  )  SYS.UDP_hosts = 0;  //--------------------->>NotUSE!!
		
		sprintf(cmd, "/bin/grep PJK2GDCcomPort /etc/UDPhosts.conf | cut -f2 -d' ' > /tmp/PJK2GDCcomPort "); 
		system(cmd);
		if ((fd = fopen("/tmp/PJK2GDCcomPort", "r")) != NULL)
		{	if (fgets(cmd, 10, fd) != NULL)	 SYS.PJK2GDCcomPort = atoi(cmd); }
		fclose(fd);
		//
		if ( port_no != (SYS.PJK2GDCcomPort - 1)  )  SYS.PJK2GDCcomPort = 0;  //------------->>NotUSE!!
		
		
		if( (SYS.UDP_hosts>0) && (SYS.UDP_hosts<MAX_CONNECT_UDP) )
		{
			sprintf(cmd,"/bin/rm -f /tmp/udpList.txt");
			system(cmd);
			sprintf(cmd,"/bin/touch /tmp/udpList.txt");
			system(cmd);
			
			for(i=1 ; i<(SYS.UDP_hosts + 1) ; i++)
			{
			sprintf(cmd, "/bin/grep UDPhost%d /etc/UDPhosts.conf | cut -f2 -d' ' > /tmp/udpHostIP%d ", i, i);
			system(cmd);
			
			sprintf(cmd,"/tmp/udpHostIP%d",i);  //// UDP_ip [MAX_CONNECT_UDP][32] ////
			if ((fd = fopen(cmd, "r")) != NULL) {	fgets(SYS.UDP_ip[i], 30, fd);  }
			fclose(fd);
			//
			n = strlen(SYS.UDP_ip[i]);
			if(SYS.UDP_ip[i][n-1]=='\n') SYS.UDP_ip[i][n-1]=0;
			
			sprintf(cmd, "/bin/grep UDPhost%d /etc/UDPhosts.conf | cut -f3 -d' ' > /tmp/udpHostPORT%d ", i, i);
			system(cmd);
			
			sprintf(cmd,"/tmp/udpHostPORT%d",i);
			if ((fd = fopen(cmd, "r")) != NULL) {	fgets(cmd, 10, fd);  SYS.UDP_port[i]=atoi(cmd);  }
			fclose(fd);
			
			sprintf(cmd, "/bin/grep UDPhost%d /etc/UDPhosts.conf | cut -f4 -d' ' > /tmp/udpMyPORT%d ", i, i);
			system(cmd);
			
			sprintf(cmd,"/tmp/udpMyPORT%d",i);
			if ((fd = fopen(cmd, "r")) != NULL) {	fgets(cmd, 10, fd);  SYS.UDP_Lport[i]=atoi(cmd);  }
			fclose(fd);
			
			sprintf(cmd,"/bin/echo [%d]%s:%d::%d >> /tmp/udpList.txt", i, SYS.UDP_ip[i], SYS.UDP_port[i], SYS.UDP_Lport[i]);
			system(cmd);
			}
		}
	}
	else { fclose(fd); }
}


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
	
	SYS.sfd2 = 0;
	
	SYS.UDP_comPort = -1;
	SYS.UDP_hosts = 0;
	for (no=0; no<MAX_CONNECT_UDP; no++) SYS.UDP_lfd[no] = 0;
	//
	UDP_hosts_init();

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
		
		if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOmaster", 9) )
		{
			//-------(for SYS.sfd2)-----------------!UIF--LCD_OUT_Port~~! --COM1--
			SYS.bps2		= CFG_SIO[0].speed;
			SYS.dps2		= CFG_SIO[0].dps;
			SYS.flow2		= CFG_SIO[0].flow;
			SYS.interface2	= CFG_SIO[0].interface;
		}
		
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
int no, no2, ret;
unsigned long CTimer=0, BTimer;
unsigned long DTimer = 0;
unsigned long STimer = 0;

struct in_addr addr;

//@
unsigned long lpTimer;
unsigned long lpTimerC;


//char *endp;  //...for strtod( ,&endp)
char endp[2];

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
	
	
	if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOmaster", 9) )
	{	
		if (SYS.sfd2 <= 0)
			{
			SYS.sfd2 = SB_OpenSerial (0);
			if (SYS.sfd2 <= 0) return;
			}
		SB_InitSerial (SYS.sfd2, SYS.bps2, SYS.dps2, SYS.flow2);
		SB_ReadSerial (SYS.sfd2, WORK, MAX_BUFFER_SIZE, 0);
		if (SYS.interface == SB_RS232)
			{
			SB_SetDtr (SYS.sfd2, SB_ENABLE);
			SB_SetRts (SYS.sfd2, SB_ENABLE);
			}
	}
	
	
	if ( SYS.UDP_hosts > 0 )
	{
		for (no=1; no<(SYS.UDP_hosts + 1); no++)    //---UDP_close_init //
		{
			if ( SYS.UDP_lfd[no] > 0 )
			{
				shutdown(SYS.UDP_lfd[no], SHUT_RDWR);	
				close (SYS.UDP_lfd[no]);
				SYS.UDP_lfd[no] = 0;
			}
		}
		for (no=1; no<(SYS.UDP_hosts + 1); no++)    //---UDP_Bind~ //
		{
			SYS.UDP_lfd[no] = SB_BindUdp( SYS.UDP_Lport[no] );
			////if (SYS.UDP_lfd[no] <= 0) return; ////
		}
	}


	sprintf(SYS.msg,"/bin/echo ===Cnt:%02d::EAof::tcp32=== > /tmp/tcp32s.txt", SYS.connect_cnt);  system(SYS.msg);
	for (no2=0; no2<MAX_CONNECT; no2++)
	{
	sprintf(SYS.msg,"/bin/echo -:%02d::%05d >> /tmp/tcp32s.txt", no2, SYS.lfd[no2] );  system(SYS.msg);
	}
	sprintf(SYS.msg,"/bin/echo ===Inited~~:ConnCNT::%02d=== >> /tmp/tcp32s.txt", SYS.connect_cnt );  system(SYS.msg);


	if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOmaster", 9) )
	{
		addr.s_addr = SB_GetIp ("eth0");
		//@init_UIF
		uif_clr ();
		uif_clr ();
		uif_xy (0,0);
		sprintf (WORK, " MultiLink_RIO V1.0 ");
		SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));	
		uif_xy (0,1);
		sprintf (WORK, "myIP:%s ", inet_ntoa(addr) );
		SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));
		uif_xy (0,2);
		sprintf (WORK, "INPUT#1234567890 " );
		///////////////"OnOff:0101010101
		SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));
		//
		uif_xy (0,3);
		sprintf (WORK, " STAT:" );
		SB_SendSerial (SYS.sfd2, WORK, strlen(WORK));		
		//@
	}
	
	lpCNT=0;
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


		//-------MultiLink-RIO-Master-----//
		if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOmaster", 9) )
		{
		//@ loop-Timer @//
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
					uif_xy (6,3);
					SB_SendSerial (SYS.sfd2, " -NoInput-",10); //---COM1---
					lpCNT=0;
				}
				
				uif_xy (19,3);
				//*endp = RingCH[RingCnt];
				endp[0] = RingCH[RingCnt];
				SB_SendSerial (SYS.sfd2, endp, 1);  fflush(0);
				if ( RingCnt++ >= 3 ) RingCnt=0;				
			}
		//@
		}
   		
		ret = SB_AcceptTcpMulti (SYS.listen_fd, 1);
		if (ret > 0)
		{
			if (++SYS.connect_cnt > MAX_CONNECT) 
			{
				--SYS.connect_cnt;
				close (ret);
				
				////<---------------------------Add:CLOSE-All!!
				//for (no=0; no<MAX_CONNECT; no++)
				//	close_init (no);
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
						
			sprintf(SYS.msg,"/bin/echo ===Cnt:%02d::EAof::tcp32=== > /tmp/tcp32s.txt", SYS.connect_cnt);  system(SYS.msg);
			for (no2=0; no2<MAX_CONNECT; no2++)
			{
			sprintf(SYS.msg,"/bin/echo ~:%02d::%05d >> /tmp/tcp32s.txt", no2, SYS.lfd[no2] );  system(SYS.msg);
			}
			sprintf(SYS.msg,"/bin/echo ===Accetp~1:ConnCNT::%02d=== >> /tmp/tcp32s.txt", SYS.connect_cnt );  system(SYS.msg);
						
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
		
		receive_from_port();  //<---tcp32---
		
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
int no2;

	if (SYS.lfd[no] > 0)
	{
		shutdown(SYS.lfd[no], SHUT_RDWR);	
		close (SYS.lfd[no]);
		SYS.lfd[no] = 0;
		if (--SYS.connect_cnt < 0)  SYS.connect_cnt = 0;
	}

	sprintf(SYS.msg,"/bin/echo ===Cnt:%02d::EAof::tcp32=== > /tmp/tcp32s.txt", SYS.connect_cnt);  system(SYS.msg);
	for (no2=0; no2<MAX_CONNECT; no2++)
	{
	sprintf(SYS.msg,"/bin/echo -:%02d::%05d >> /tmp/tcp32s.txt", no2, SYS.lfd[no2] );  system(SYS.msg);
	}
	sprintf(SYS.msg,"/bin/echo ===Close~~1:ConnCNT::%02d=== >> /tmp/tcp32s.txt", SYS.connect_cnt );  system(SYS.msg);
	
	
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

char *endp;  //...for strtod( ,&endp)

int gMode;


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

	for(no=0;no<20;no++) WORK[no]=0;  //...Dumy-Clear~~
	
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
			//SB_msleep (20);  // wait(~200byte@115200bps)....for...Line-CRLF
			SB_msleep (SYS.wait_msec);    //<--tcp32--
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
	
	//-------MultiLink-RIO-Master-----//
	if ( (port_no == 1) && !strncmp( CFG_SYS.device_name, "RIOmaster", 9) )
	{
		if ( (WORK[0] == ':') && ((WORK[13]=='\n')||(WORK[13]=='\r')) )
		{
			//Do::Check-SUM~~~
			
			//sprintf (WORK, "INPUT:12345678901s## " );
			/////////////////"OnOff:0101010101
			uif_xy (6,3);
			SB_SendSerial (SYS.sfd2, &WORK[1], 10); //---COM1---
			
			lpCNT=0;  //<---Clear!~~WDT~~
		}
	}
	//-------MultiLink-RIO-Master-----//
	

	//---------------------------------------------------------------------(PJK2GDC)--//
	if ( (SYS.PJK2GDCcomPort > 0) && !strncmp( CFG_SYS.device_name, "GDM_ROVER", 9) )
	{
		if ( !strncmp( WORK, "$PTNL,PJK,", 10) )
		{
			if (strlen(WORK) > 70)
			{
				memcpy (WORK_2, WORK, len);
				WORK_2[len]=0;
				
				//cp1 = strstr(WORK_2,"\n");  *(cp1) = 0;
				//cp1 = strstr(WORK_2,"\r");  *(cp1) = 0; 		//-------// Trim_CrLF //
				
				idx = gdcPass( WORK_2 );
				//-----------------------------//
					
				if ( idx < 12 ) return (0) ; 	//==>Error~
					
				strcpy(&gdcX[0], &gdcA[6][0]);
				strcpy(&gdcY[0], &gdcA[4][0]);	
				//
				Xcur1 = (double) strtod (&gdcX[0],&endp);
				Ycur1 = (double) strtod (&gdcY[0],&endp);
				
				if ( gdcA[8][0] == '1' )  gMode = 1;
				else if ( gdcA[8][0] == '2' )  gMode = 5;		
						else if ( gdcA[8][0] == '3' )  gMode = 4;
								else gMode = 0;				
				
				if ( Xcur1 < 0) Xcur1=0;
				if ( Ycur1 < 0) Ycur1=0;
				
				// if ( (gMode > 0) && (Xcur1 > 0) && (Ycur1 > 0) )  //...Mask_for_Init...//
				if ( (gMode >= 0) && (Xcur1 >= 0) && (Ycur1 >= 0) )  //...Mask_for_Init...//
				{
					//	$PTNL,PJK,085714.40,082414,+443954.037,N,+366903.406,E,3,15,3.8,EHT+89.962,M*49
				strcpy(&gdcY[0], &gdcA[11][3]);
				Height1 = ((double) strtod (&gdcY[0],&endp)) - ((double) 20);
				if ( Height1 < 0) Height1=0; //<---Min!
				//
				strncpy(&pjkYEAR[0], &gdcA[3][4], 2);   pjkYEAR[2]=0;
				strncpy(&pjkDATE[0], &gdcA[3][0], 4);   pjkDATE[4]=0;
				
				// Header++Time++XXXXXX.XXX,YYYYYY.YYY,g,SK,DoP,,,
				//sprintf (WORK_2, "$PASHR,GDC,%s,TM83,%6.4f,%6.4f,%d,%s,%s,+00034.56,M,+0023.45,M,0,12,W84*00\n\r", &gdcA[2][0], Xcur1, Ycur1, gMode, &gdcA[9][0], &gdcA[10][0] );
				
				//sprintf (WORK_2, "$PASHR,GDC,%s,TM83,%6.4f,%6.4f,%d,%s,%s,%+5.2f,M,+0020.00,M,0,12,W84*00\n\r", &gdcA[2][0], Xcur1, Ycur1, gMode, &gdcA[9][0], &gdcA[10][0], Height1 );
				
				//---With-DATE---//
				sprintf (WORK_2, "$PASHR,GDC,%s,TM83,%6.4f,%6.4f,%d,%s,%s,%+09.2f,M,+0020.00,M,%s,%s,W84*00\n\r", &gdcA[2][0], Xcur1, Ycur1, gMode, &gdcA[9][0], &gdcA[10][0], Height1, &pjkYEAR[0], &pjkDATE[0] );
				
				
				len = strlen( WORK_2 ); 
				memcpy (WORK, WORK_2, len);
				WORK[len]=0;
				
				}
				else 
				{
					return 0;  		 //===>Cancle~~~!!! 
				}
				
				//<====RUN!!
			}
			else { 	return 0;  		} //===>Cancle~~~!!! 
		}
	}
	//---------------------------------------------------------------------(PJK2GDC)--//


	for (no=0; no<MAX_CONNECT; no++)
	{
		if (SYS.lfd[no] <= 0) continue;	
		
		// write (SYS.lfd[no], WORK, len);
		if ( write (SYS.lfd[no], WORK, len) < len )  close_init (no);  ////<===tcp32==!!
		
		////if (portview && no==0) PSM_write ('R', WORK, len);
	}
	
	if ( SYS.UDP_hosts > 0 )
	{	
		for (no=1; no<(SYS.UDP_hosts + 1); no++)
		{
			if (SYS.UDP_lfd[no] <= 0) continue;
			SB_SendUdpClient (SYS.UDP_lfd[no], WORK, len, SYS.UDP_ip[no], SYS.UDP_port[no]);
		}
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



//===============================================================================
//===============================================================================
//==(--UIF-420A/_5K)==>


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



//----Rover-DATA::GDC-&-PJK-----//
//	$PASHR,GDC,100046.60,TM83,132270.1447,378936.2864,4,9,00.8,+00018.03,M,+0020.38,M,0,11,W84*45
//	$PASHR,GDC,100046.80,TM83,132270.1436,378936.2871,4,9,00.8,+00018.03,M,+0020.38,M,0,11,W84*49
//	$PASHR,GDC,100047.00,TM83,132270.1448,378936.2875,4,10,00.8,+00018.03,M,+0020.38,M,0,11,W84*75
//	$PASHR,GDC,100047.20,TM83,132270.1441,378936.2868,4,10,00.8,+00018.03,M,+0020.38,M,0,11,W84*72

//	$PTNL,PJK,085714.20,082414,+443954.039,N,+366903.407,E,3,15,3.8,EHT+89.954,M*45
//	$PTNL,PJK,085714.40,082414,+443954.037,N,+366903.406,E,3,15,3.8,EHT+89.962,M*49

/**
00000000011111111112222222222333333333344444444445555555555666666666677777777778
12345678901234567890123456789012345678901234567890123456789012345678901234567890

$PTNL,PJK,085427.80,082414,+443955.179,N,+366902.627,E,3,15,1.8,EHT+89.983,M*41
$PTNL,PJK,085428.00,082414,+443955.186,N,+366902.614,E,3,15,1.8,EHT+89.973,M*49
$PTNL,PJK,085428.20,082414,+443955.184,N,+366902.607,E,3,14,2.0,EHT+89.968,M*4B
$PTNL,PJK,085428.40,082414,+443955.185,N,+366902.611,E,3,14,2.0,EHT+89.978,M*4A
$PTNL,PJK,085428.60,082414,+443955.185,N,+366902.608,E,3,14,2.0,EHT+89.974,M*4C

$PASHR,GDC,072144.20,TM83,132209.2521,379078.7690,1,8,01.0,+00035.54,M,+0020.38,M,,,W84*78
$PASHR,GDC,072144.40,TM83,132209.2330,379078.7177,1,8,01.0,+00035.51,M,+0020.38,M,,,W84*73
$PASHR,GDC,072144.40,TM83,132215.9696,379073.8866,5,7,01.1,+00016.68,M,+0020.38,M,0,11,W84*40
$PASHR,GDC,072144.60,TM83,132215.7171,379073.9572,5,7,01.1,+00016.56,M,+0020.38,M,0,11,W84*46

$GPGGA,085127.40,3728.7990030,N,12653.2141385,E,4,15,1.8,67.1724,M,22.804,M,00,0004*61
$GPGGA,085127.60,3728.7990038,N,12653.2141385,E,4,15,1.8,67.1667,M,22.804,M,00,0004*6D

**/





//===>CAPed~
/**
 94:$PASHR,GDC,092412.40,TM83,207212.3005,446481.3327,4,9,01.0,+00015.90,M,+0021.60,M,4,5,W84*7C
j==16
j=>$PASHR
j=>GDC
j=>092412.40
j=>TM83
j=>207212.3005
j=>446481.3327
j=>4
j=>9
j=>01.0
j=>+00015.90
j=>M
j=>+0021.60
j=>M
j=>4
j=>5
j=>W84*7C
**/

/**
	//LCD_clr();  //<---sleep(20)
	sprintf(str1, "X:%9s       ",&gdcX[0] ); 			LCD_line(1, str1);
	sprintf(str1, "Y:%9s       ",&gdcY[0] ); 			LCD_line(2, str1);
	sprintf(str1, "SK:%2s  RT:%2s  RQ:%2s ",&gdcA[7][0], &gdcA[13][0], &gdcA[6][0] );
	LCD_line(3, str1);
**/

//===============================================================================

int gdcPass(char *p)
{
int i=0;
int j=0;

char *cp;

	cp=p;
	
	//for( ; *cp ; ++cp )  
	for( ; *cp && (*cp != '*') ; ++cp )
	{
		if ( *cp != ',' )
		{
			gdcA[j][i++] = *cp;
		}
		else
		{
			gdcA[j][i]=0;  
			i=0;
			j++;
		}
		
		if ( j>16) return 0; //error
	}
	
	gdcA[j][i]=0;  
	j++;

	return j;
}

//===============================================================================

int pjk2gdc(int len)
{

//Segment...fault// char WORK2 [MAX_SOCKET_READ_SIZE];  //[MAX_BUFFER_SIZE];

int idx;
//---------------------------
//char *cp1;
	
char *endp;  //...for strtod( ,&endp)

int gMode;


	memcpy (WORK_2, WORK, len);
	WORK_2[len]=0;


	//cp1 = strstr(WORK_2,"\n");  *(cp1) = 0;
	//cp1 = strstr(WORK_2,"\r");  *(cp1) = 0;
	//---------------------------------------------// Trim_CrLF //

	idx = gdcPass( WORK_2 );
	//-----------------------------//
		
	if ( idx < 12 ) return (-1) ; //==>Error~
		
		
	strcpy(&gdcX[0], &gdcA[6][0]);
	strcpy(&gdcY[0], &gdcA[4][0]);	
	//
	Xcur1 = (double) strtod (&gdcX[0],&endp);
	Ycur1 = (double) strtod (&gdcY[0],&endp);
		
		
	if ( gdcA[8][0] == '1' )  gMode = 1;
	else if ( gdcA[8][0] == '2' )  gMode = 5;		
			else if ( gdcA[8][0] == '3' )  gMode = 4;
					else gMode = 0;
		
		
	// Header++Time++XXXXXX.XXX,YYYYYY.YYY,g,SK,DoP,,,
	sprintf (WORK_2, "$PASHR,GDC,%s,TM83,%6.4f,%6.4f,%d,%s,%s,+00034.56,M,+0023.45,M,0,12,W84*00\n\r", &gdcA[2][0], Xcur1, Ycur1, gMode, &gdcA[9][0], &gdcA[10][0] );
	
	
	len = strlen( WORK_2 ); 
	memcpy (WORK, WORK_2, len);
	WORK[len]=0;
	
	return len;
}

//===============================================================================

/**((Opset_Logic)**

Private Sub RADOFFB(W2, W3, W4, W5, e, F)
 
    m_listBX = Sin(CALAZ(W2, W3, e, F) - CALAZ(W2, W3, W4, W5)) * DIST(W2, W3, e, F)
 '   m_listAY = Sin(CALAZ(W2, W3, e, f) - CALAZ(W2, W3, W4, W5)) * DIST(W2, W3, e, f)

End Sub

Private Sub RADOFFC(W2, W3, W4, W5, e, F)

    m_listCX = Cos(CALAZ(W2, W3, e, F) - CALAZ(W2, W3, W4, W5)) * DIST(W2, W3, e, F)

End Sub

'''두점의 방위각'''
Public Function CALAZ(ByVal m_x1 As Double, ByVal m_y1 As Double, ByVal m_x2 As Double, ByVal m_y2 As Double) As Double 
    On Error GoTo errmanager
       If (m_y2 - m_y1) = 0 And (m_x2 - m_x1) = 0 Then
             CALAZ = 0
       ElseIf (m_y2 - m_y1) = 0 And (m_x2 - m_x1) > 0 Then
             CALAZ = 0
       ElseIf (m_y2 - m_y1) = 0 And (m_x2 - m_x1) < 0 Then
             CALAZ = PI
       ElseIf (m_y2 - m_y1) > 0 And (m_x2 - m_x1) = 0 Then
             CALAZ = PI / 2
       ElseIf (m_y2 - m_y1) < 0 And (m_x2 - m_x1) = 0 Then
             CALAZ = PI * 3 / 2
       ElseIf (m_y2 - m_y1) > 0 And (m_x2 - m_x1) > 0 Then
             CALAZ = Atn(Abs(m_y2 - m_y1) / Abs(m_x2 - m_x1))
       ElseIf (m_y2 - m_y1) > 0 And (m_x2 - m_x1) < 0 Then
             CALAZ = PI - Atn(Abs(m_y2 - m_y1) / Abs(m_x2 - m_x1))
       ElseIf (m_y2 - m_y1) < 0 And (m_x2 - m_x1) < 0 Then
             CALAZ = PI + Atn(Abs(m_y2 - m_y1) / Abs(m_x2 - m_x1))
       ElseIf (m_y2 - m_y1) < 0 And (m_x2 - m_x1) > 0 Then
             CALAZ = 2 * PI - Atn(Abs(m_y2 - m_y1) / Abs(m_x2 - m_x1))
       End If
       Exit Function

End Function

**((Opset_Logic)**/

//===============================================================================


