/*****************************************************************************
Copyright (c) 2007 SystemBase Co., LTD  All Rights Reserved.

UDP Server/Clinet		09, 31, 2007. yhlee
******************************************************************************/

#include "include/sb_include.h"
#include "include/sb_define.h"
#include "include/sb_shared.h"
#include "include/sb_config.h"
#include "include/sb_extern.h"
#include "include/sb_ioctl.h"

//---
#include "include/sb_misc.h"  ////-DRF-  //====(eddy v2.5)==<


#include <math.h>    ////-DVSCPU- VisualSpeedSensor



#define MAX_BUFFER_SIZE 		3000
#define MAX_SOCKET_READ_SIZE 	512
#define MAX_SERIAL_READ_SIZE 	512


//===========================(eddy v2.5)==<




//===========================(eddy v2.5)==<




//==(GDM_EC)==>
//char * a_data		[MAX_DATA]	= { "5", "6", "7", "8" };
char uifKeyTable [26] = { 0xFF,19,15,1,2,3,20,16,4,5,6,21,17,7,8,9,22,18,10,0,11,0xFF,0xFF,12,13,14 };
/**
[0..9] = [0..9]
10 = "-" 	11 = "."
12 = B/S	13 = Alarm	14 = ACK
15 = Left	16 = Home	17 = Right	18 = Enter
19 = F1		20 = F2		21 = F3		22 = F4
**/


int ec_RunMode;		// 0:none 
int GDM_type;		// 0:none 1:Base 2:Repeater 3:Rover 4:EConverter

int GDM_RXwdtCnt;		// 100msec++
int GDM_RXwdtMode;		// 0:none 1:>ON



//char RingCH [4] = { '|','/','-', 0x5C };  // ~'\\'~ };
//char RingCH [4] = { '|','*','-', '*' };  // ~'\\'~ };
//char RingCH [4] = { '^','>','v', '<' };  // ~'\\'~ };
//char RingCH [4] = { '\'',';',',', '.' };  // ~'\\'~ };
char RingCH [4] = { '.','o','O', ' ' };  // ~'\\'~ };
int  RingCnt=0;


static char gdcA[17][15];

static char gdcTime[15];
static char gdcX[15];
static char gdcY[15];

static double Xcur1;
static double Ycur1;

static double Xzero1;
static double Yzero1;
static double Dzero1;
//
static char useGPS;

//static  double Xcur1L;
//static  double Ycur1L;

//static  double tmpD;
//static  long distTMP;
static  double distTMP;

static  double distOUT;


static unsigned long distXY;
static unsigned long distXYout;
//
static unsigned long distWcnt;


int pjk2gdc(int len);

int gdcPass(char *p);

void uif_clr ();
void uif_xy (int x, int y);
void uif_xyPut (int x, int y, char ch);
void uif_xySTR (int x, int y, char *str);

/****
static int M_isqrt(int n);
static long M_llsqrt(long long a);

static double DDsqrt(double k);
static double DDsqrt3f(double k);
****/


int mCnt = 0;

//==(GDM_EC)==<


struct sys_info  
{
	int				sfd;
	int				lfd;
	int				port_no; 		//---!GDM_EC_UIF_IN/OUT_Port~~! (Com2)
	int				protocol;
	int				wait_msec;
	int				socket;
	char  			bps;
	char 			dps;
	char 			flow;
	char			interface;
	char 			server_ip [100];
	int				server_port;

	//-----------------------------------!GDM_EC_Dist_OUT_Port~~!   (Com1)
	int				sfd2;
	char  			bps2;
	char 			dps2;
	char 			flow2;
	char			interface2;
	//-----------------------------------!GDM_EC_Dist_OUT_Port~~!
	
	//-------------------------------// GDM_EC MON //
	int				mon_lfd;
	int				mon_Lport;  //=0
	int				mon_Rport;
	char 			mon_Rip [100];
	
	//-------------------------------// GDM_EC PLC //  (Yaskawa CP218 Send)
	int				PLC_lfd;
	int				PLC_Lport; 		// 4001
	int				PLC_Rport; 		// 3001
	char 			PLC_Rip [100];  // 10.1.228.2

// GDM_EC_
	char			GDM_XXzero[16];			//16	// "GDM_EC:: zero_position!
	char			GDM_YYzero[16];			//16	// "GDM_EC:: zero_position!
	char			GDM_DDzero[16];			//16	// "GDM_EC:: zero_position!
	//...++[48]byte...
	char			GDM_USEmode;			//		// "GDM_EC:: USE-GPS/Encoder! {-1:none 1:ON 0:OFF(encoder)}
	//...++[49]byte...

	int			monTXport;   //<----(7608),(7611),,,<=="Port-1" ~ "RC608" "ST611" ,,, ???multi...???

};	

struct sys_info  SYS;

int port_no;
int snmp;

/****
int portview;
struct SB_PORTVIEW_STRUCT *PSM;
struct SB_SNMP_STRUCT	  *SSM;
****/

char WORK [MAX_BUFFER_SIZE];

char WORK_2 [MAX_BUFFER_SIZE];

int	SB_DEBUG	=	0;   ///////===0


int main (int argc, char *argv[]);

void close_init ();

void mainloop(int protocol);
int receive_from_lan(void);
int receive_from_port (int protocol);


/****
int PA_login();

void get_snmp_memory ();
void get_portview_memory ();

void SSM_write ();
void PSM_write (char sw, char *data, int len);
****/



//struct SB_CONFIG cfg;

//-----------------------------------------------------(Config for S4M filesystem 2.5.1.2)
//struct SB_SIO_CONFIG			cfg [SB_MAX_SIO_PORT];
//
struct SB_SYSTEM_CONFIG			CFG_SYS;						// /flash/eddy_system.cfg
struct SB_SIO_CONFIG			CFG_SIO [SB_MAX_SIO_PORT];		// /flash/eddy_sio.cfg
struct SB_SNMP_CONFIG			CFG_SNMP;						// /flash/eddy_snmp.cfg
struct SB_GPIO_CONFIG			CFG_GPIO;						// /flash/eddy_gpio.cfg
struct SB_DIO_CONFIG			CFG_DIO;						// /flash/eddy_dio.cfg
struct SB_WIFI_CONFIG			CFG_WIFI;						// /flash/eddy_wifi.cfg


//===============================================================================	
int main (int argc, char *argv[])
{
char *endp;  //...for strtod( ,&endp)

char strT[16];

	//Not-USE@2.5  //SB_SetPriority (77);	//95			// Min 1 ~ Max 99 

	port_no = atoi (argv[1]) - 1;			// port no 1 ~ 16

	signal(SIGPIPE, SIG_IGN);		//---??

	//get_portview_memory ();
	//get_snmp_memory ();


	SYS.lfd = 0;
	SYS.sfd = 0;
	
	//------------
	SYS.sfd2 = 0;
	SYS.mon_lfd = 0;
	SYS.PLC_lfd = 0;
	//------------

	distXY = 0;
	distXYout = 0;
	distWcnt = 0;


	while (1) 
	{
		//SB_ReadConfig (SB_CFG_FILE, &cfg);

		//------------------------- Read /etc Config Data
		SB_ReadConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, 		sizeof (struct SB_SYSTEM_CONFIG));
		SB_ReadConfig  (CFGFILE_ETC_SIO,     (char *)&CFG_SIO[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
		SB_ReadConfig  (CFGFILE_ETC_SNMP,    (char *)&CFG_SNMP,		sizeof(struct SB_SNMP_CONFIG));	
		SB_ReadConfig  (CFGFILE_ETC_GPIO,    (char *)&CFG_GPIO,		sizeof(struct SB_GPIO_CONFIG));	
		SB_ReadConfig  (CFGFILE_ETC_DIO,     (char *)&CFG_DIO,		sizeof(struct SB_DIO_CONFIG));	
		SB_ReadConfig  (CFGFILE_ETC_WIFI,    (char *)&CFG_WIFI,		sizeof(struct SB_WIFI_CONFIG));


		SYS.protocol	= CFG_SIO[port_no].protocol;
		SYS.bps		 	= CFG_SIO[port_no].speed;
		SYS.dps		 	= CFG_SIO[port_no].dps;
		SYS.flow		= CFG_SIO[port_no].flow;
		SYS.wait_msec	= CFG_SIO[port_no].packet_latency_time;   /// .bypass;
		SYS.socket		= CFG_SIO[port_no].socket_no;
		SYS.interface	= CFG_SIO[port_no].interface;
		
		strncpy(strT, CFG_SIO[port_no].name, sizeof(CFG_SIO[port_no].name));
		SYS.monTXport = atoi( strT ); 
		///////////////////////////// "7608" <-- "Port-1"


		//SYS.server_port	= cfg.sio[port_no].remote_socket_no;
		//SB_Hex2Ip ((char *)cfg.sio[port_no].remote_ip, (char *)SYS.server_ip);
		//
		if (  (port_no == 1) && !strncmp( CFG_SYS.device_name, "GDM_EC", 6) )
		{
			SYS.server_port	= 0;
			strcpy((char *)SYS.server_ip, "0.0.0.0"); //SB_Hex2Ip ((char *)cfg.sio[port_no].remote_ip, (char *)SYS.server_ip);
			
			//---------------------------------------------------------------// GDM_EC PLC_lfd //
			SYS.PLC_Lport 	= CFG_SIO[0].socket_no; 							// 4001
			SYS.PLC_Rport 	= CFG_SIO[1].remote_socket_no; 					// 3001
			SB_Hex2Ip ((char *)CFG_SIO[1].remote_ip, (char *)SYS.PLC_Rip); 	// 10.1.223.2
		}
		else {
			SYS.server_port	= CFG_SIO[port_no].remote_socket_no;
			SB_Hex2Ip ((char *)CFG_SIO[port_no].remote_ip, (char *)SYS.server_ip);
		}
		
		if (  (port_no == 1) && !strncmp( CFG_SYS.device_name, "GDM_EC", 6) )
		{
		
			//-------------------------------// GDM_EC MON //
			//SYS.mon_lfd
			SYS.mon_Lport 	= SYS.monTXport;  //1234;  //cfg.sio[0].socket_no;
			SYS.mon_Rport 	= CFG_SIO[0].remote_socket_no;
			SB_Hex2Ip ((char *)CFG_SIO[0].remote_ip, (char *)SYS.mon_Rip);
			
		
			//-------(for SYS.sfd2)-----------------!GDM_EC_Dist_OUT_Port~~!
			SYS.bps2		= CFG_SIO[0].speed;
			SYS.dps2		= CFG_SIO[0].dps;
			SYS.flow2		= CFG_SIO[0].flow;
			SYS.interface2	= CFG_SIO[0].interface;
			
			strncpy(SYS.GDM_XXzero, CFG_SYS.GDM_XXzero, sizeof(CFG_SYS.GDM_XXzero));
			strncpy(SYS.GDM_YYzero, CFG_SYS.GDM_YYzero, sizeof(CFG_SYS.GDM_YYzero));
			strncpy(SYS.GDM_DDzero, CFG_SYS.GDM_DDzero, sizeof(CFG_SYS.GDM_DDzero));
			//
			SYS.GDM_USEmode = CFG_SYS.GDM_USEmode;
			useGPS = SYS.GDM_USEmode;
			
			Xzero1 = strtod(SYS.GDM_XXzero, &endp);
			Yzero1 = strtod(SYS.GDM_YYzero, &endp);
			Dzero1 = strtod(SYS.GDM_DDzero, &endp);
			
		}

		/****  //SYS.wait_msec	= CFG_SIO[port_no].packet_latency_time;   /// .bypass;
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
			SB_LogMsgPrint ("UDP port=%d, Speed = %d, dps = %d, flow = %d, MRU=%d\n",
			port_no+1, SYS.bps, SYS.dps, SYS.flow, SYS.wait_msec);
		
		mainloop(SYS.protocol);
	}
}
//===============================================================================
/**
void get_snmp_memory ()
{
void *shared_memory = (void *)0;

	snmp = SB_DISABLE;
	shared_memory = SB_GetSharedMemory (SB_SNMP_KEY, sizeof (struct SB_SNMP_STRUCT));
	if (shared_memory == (void *)-1)	return;
	SSM = (struct SB_SNMP_STRUCT *)shared_memory;
	snmp = SB_ENABLE;
}
**/
//===============================================================================
/**
void get_portview_memory (void)
{
void *shared_memory = (void *)0;

	portview = SB_DISABLE;
	shared_memory = SB_GetSharedMemory (SB_PORTVIEW_KEY, sizeof (struct SB_PORTVIEW_STRUCT));
	if (shared_memory == (void *)-1)	return;
	PSM = (struct SB_PORTVIEW_STRUCT *)shared_memory;
	portview = SB_ENABLE;
}
**/

//===============================================================================
void mainloop(int protocol)
{
unsigned long CTimer=0, BTimer;
unsigned long DTimer = 0;
unsigned long STimer = 0;

//@
unsigned long lpTimer;
unsigned long lpTimerC;
//int lpCNT=0;

char *endp;  //...for strtod( ,&endp)

char WORKt [100];


	close_init ();
	SB_msleep (1000);
	SYS.lfd = SB_BindUdp (SYS.socket);
	if (SYS.lfd <= 0) return;

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


	if (  (port_no ==1) && !strncmp( CFG_SYS.device_name, "GDM_EC", 6) ) 
	{
		//-----------------------------------------// GDM_EC PLC //
		SYS.PLC_lfd = SB_BindUdp (SYS.PLC_Lport);
		if (SYS.PLC_lfd <= 0) return;
		
		//-----------------------------------------// GDM_EC MON //
		SYS.mon_lfd = SB_BindUdp (SYS.mon_Lport);
		if (SYS.mon_lfd <= 0) return;
		
		
		//-------(for SYS.sfd2)-----------------!GDM_EC_Dist_OUT_Port~~!		
		
		if (SYS.sfd2 <= 0)
			{
			SYS.sfd2 = SB_OpenSerial (0);
			if (SYS.sfd2 <= 0) return;		//<====================(when...com1_ACT???)
			}
		SB_InitSerial (SYS.sfd2, SYS.bps2, SYS.dps2, SYS.flow2);
		SB_ReadSerial (SYS.sfd2, WORK, MAX_BUFFER_SIZE, 0);
		if (SYS.interface2 == SB_RS232)
			{
			SB_SetDtr (SYS.sfd2, SB_ENABLE);
			SB_SetRts (SYS.sfd2, SB_ENABLE);
			}
		
		//@init_UIF
		uif_clr ();
		uif_clr ();
		uif_xy (0,0);
		sprintf (WORK, "[GDM-1000R]ECstart.. ");
		SB_SendSerial (SYS.sfd, WORK, strlen(WORK));	
		//@
		
		
		Xzero1 = strtod(SYS.GDM_XXzero, &endp);
		Yzero1 = strtod(SYS.GDM_YYzero, &endp);
		Dzero1 = strtod(SYS.GDM_DDzero, &endp);
		
		if ( (Xzero1 < 10000.0 ) | (Xzero1 > 500000.0) ) Xzero1 = 123456.1234;
		if ( (Yzero1 < 10000.0 ) | (Yzero1 > 500000.0) ) Yzero1 = 123456.1234;
		if ( (Dzero1 < 0 ) | (Dzero1 > 100000.0) ) Dzero1 = 0;		
		
		sprintf (WORK,"X:%f", Xzero1 );		uif_xySTR ( 0, 1, WORK );
		sprintf (WORK,"Y:%f", Yzero1 );		uif_xySTR ( 0, 2, WORK );
		sprintf (WORK,"D:%f", Dzero1 );		uif_xySTR ( 0, 3, WORK );
		
		
		//Xzero1 = 203000.0;
		//Yzero1 = 445000.0;
		//Dzero1 = 500; //200.0;	
		
		
		if ( (useGPS != 0) && (useGPS != 1) )  useGPS=1;  //==ON!
		
		SB_msleep (2000);
		uif_clr ();
	}


	/****
	if (snmp)	  
		{
		SSM_CONNECT_STAT = SB_ENABLE;
		SSM_CONNECT_COUNT ++;
		}
	****/

	lpTimerC = SB_GetTick ();
	lpTimer = lpTimerC + 500;
	
	while (1)
	{
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
		
			
		//@ loop-Timer @//
			if ( lpTimerC > SB_GetTick () ) // Check over Tick Counter;  0xffffffff (4 bytes)
			{
				lpTimerC = SB_GetTick ();
				lpTimer = lpTimerC + 500;
			}
			else {
				lpTimerC = SB_GetTick ();
			}
			
			if ( lpTimer < lpTimerC )
			{
				lpTimerC = SB_GetTick ();
				lpTimer = lpTimerC + 500;
				
				/**(LCD-DIAG)**
					uif_xy (lpCNT,1); sprintf (WORK, "%d", lpCNT % 10);
															   SB_SendSerial (SYS.sfd, WORK, strlen(WORK));
					uif_xy (lpCNT,2); WORK[0] = 0x61 + lpCNT;  SB_SendSerial (SYS.sfd, WORK, 1);
					uif_xy (lpCNT,3); WORK[0] = 0x41 + lpCNT;  SB_SendSerial (SYS.sfd, WORK, 1);
					if ( lpCNT++ > 19 ) { lpCNT=0;  uif_clr (); }
				**/
				
				uif_xy (19,3);
				*endp = RingCH[RingCnt];
				SB_SendSerial (SYS.sfd, endp, 1);  fflush(0);
				if ( RingCnt++ >= 3 ) RingCnt=0;
				
				
				distWcnt++;
				//
				if ( (distWcnt > 5) && (distWcnt < 20) ) //<=====Just_while_[3~10]sec!_Only!!=====//
				{
				  if ( (distXYout > 0) && (distXY > 99) && (distXY < 999999) )
				  {
					sprintf (WORKt, "\n\r$%08Xh (%.2f)\r", distXYout, (float)distXY / 100.0 );  	//""[cm]""
					SB_SendSerial (SYS.sfd2, WORKt, strlen(WORKt));
					//==========================================================>>[Send-to-RIO]!!
				  }
				}
				
/** (test....math.h)
				tmpD = sin((distWcnt * 10) * (3.1415927 / 180));
				sprintf (WORKt, ">%d %d -(sin)-> %9.5f \n\r", distWcnt, distWcnt * 10, tmpD );  	
				//SB_SendSerial (SYS.sfd2, WORKt, strlen(WORKt));
				SB_SendUdpClient (SYS.mon_lfd, WORKt, strlen(WORKt), SYS.mon_Rip, SYS.mon_Rport);

				tmpD = sqrt((distWcnt * 10) );
				sprintf (WORKt, ">%d %d -(sqrt)-> %9.5f \n\r", distWcnt, distWcnt * 10, tmpD );  	
				//SB_SendSerial (SYS.sfd2, WORKt, strlen(WORKt));
				SB_SendUdpClient (SYS.mon_lfd, WORKt, strlen(WORKt), SYS.mon_Rip, SYS.mon_Rport);
**/
				
				
			}
		//@
			
		
			
		if (receive_from_lan () < 0) return;
		if (receive_from_port(protocol) < 0) return;

		/****
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

	if (SYS.mon_lfd > 0)
		{
		shutdown(SYS.mon_lfd, SHUT_RDWR);	
		close (SYS.mon_lfd);
		SYS.mon_lfd = 0;
		}

	if (SYS.PLC_lfd > 0)
		{
		shutdown(SYS.PLC_lfd, SHUT_RDWR);	
		close (SYS.PLC_lfd);
		SYS.PLC_lfd = 0;
		}

	if (SB_DEBUG) SB_LogMsgPrint ("Close Socket & Serial Handle\n");
}	

//===============================================================================
int receive_from_lan(void)
{
int len, sio_len; 

char WORK2 [MAX_BUFFER_SIZE];
int idx;
//---------------------------
char *cp1;  //char *cp2;
	
char *endp;  //...for strtod( ,&endp)

int gMode;


	sio_len = ioctl (SYS.sfd, TIOTGTXCNT);
	if (sio_len < 32) return 0;	
	if (sio_len > MAX_SOCKET_READ_SIZE) sio_len = MAX_SOCKET_READ_SIZE;
	len = SB_ReadUdp (SYS.lfd, WORK, sio_len);
	if (len <= 0) return len;

	//if (portview) PSM_PORT.status = SB_ACTIVE;


	WORK[len] = 0;
	//------------!

	//-------------------------------------------------------------// GDM_EC //
	// if ( ( port_no == 0 ) && ( strncmp( cfg.system.device_name, "GDM_EC", 6) == 0 ) )
	if (  strncmp( CFG_SYS.device_name, "GDM_EC", 6) == 0 ) 
	{
		//...if GPS-DATA fixed...??
		
		// if "$PASHR,GDC, 
		//		if """fixed""" ----------> {{nonefix_cnt = 0;}} && {memcpy{ work_fixed <= work }}
		//		if """nonefixed"""  -----> if (nonefix_cnt < 999) --> {{nonefix_cnt ++;}}
		
		//	if (( nonefix_cnt < nonefix_max ))  ==> {{ memcpy{ work <= work_fixed } }}
		
		
		
		//----------------------------------------------------------------"$PTNL,PJK,..."
		if ( !strncmp( WORK, "$PTNL,PJK,", 10) )
		{
			//if (strlen(WORK) > 70)
			//{	if ( pjk2gdc(len) < 0 ) return 0; }
			//else { 						return 0; }
			
			
			/****/
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
				
				
				if ( (gMode > 0) && (Xcur1 > 0) && (Ycur1 > 0) )  //...Mask_for_Init...//
				{
				
				// Header++Time++XXXXXX.XXX,YYYYYY.YYY,g,SK,DoP,,,
				sprintf (WORK_2, "$PASHR,GDC,%s,TM83,%6.4f,%6.4f,%d,%s,%s,+00034.56,M,+0023.45,M,0,12,W84*00\n\r", &gdcA[2][0], Xcur1, Ycur1, gMode, &gdcA[9][0], &gdcA[10][0] );
				
				len = strlen( WORK_2 ); 
				memcpy (WORK, WORK_2, len);
				WORK[len]=0;
				
				}
				
				//<====RUN!!
				
			}
			else { 	return 0; }
			/****/
			
			/** **
			SB_SendUdpClient (SYS.mon_lfd, WORK, strlen(WORK), SYS.mon_Rip, SYS.mon_Rport);	
			sprintf (WORK2, "\n\rDebug: X=%f Y=%f \n\r", Xcur1, Ycur1 ); 
			SB_SendUdpClient (SYS.mon_lfd, WORK2, strlen(WORK2), SYS.mon_Rip, SYS.mon_Rport);
			return 0;
			** **/
		}
		
		//<====RUN!!
		
		if ( !strncmp( WORK, "$PASHR,GDC,", 11) && (strlen(WORK) > 79) )  //85
		{
		
			////cp1 = strstr(WORK,"W84*");  *(cp1+6) = 0;
			//cp1 = strstr(WORK,"\n");  *(cp1) = 0;  //<----------------------"Segmentation fault" Error??
			//cp1 = strstr(WORK,"\r");  *(cp1) = 0;
			//---------------------------------------------// Trim_CrLF //
		
			idx = gdcPass( WORK );
			//-----------------------------//
			
			
			/****
			sprintf (WORK2, "Debug: %s", WORK ); 
			SB_SendUdpClient (SYS.mon_lfd, WORK2, strlen(WORK2), SYS.mon_Rip, SYS.mon_Rport);
			return 0;
			****/
			
			
			strcpy(&gdcX[0], &gdcA[4][0]);
			strcpy(&gdcY[0], &gdcA[5][0]);			
			//cp1 = strchr(&gdcX[0],'.');
			
			Xcur1 = (double) strtod (&gdcX[0],&endp);
			Ycur1 = (double) strtod (&gdcY[0],&endp);
			
			
			if ( Xcur1 < 1000.0 ) goto GPSrxErr;  //return 0;  //...GPS__Error!
			if ( Ycur1 < 1000.0 ) goto GPSrxErr;  //return 0;  //...GPS__Error!
			
			
			//(j < 0 ? -j : j)
			Xcur1 = ( Xzero1 > Xcur1 ? (Xzero1 - Xcur1) : (Xcur1 - Xzero1) );
			Ycur1 = ( Yzero1 > Ycur1 ? (Yzero1 - Ycur1) : (Ycur1 - Yzero1) );
			
			
		//	sprintf (WORK2, "\n\rLength@ X:%06.4f \t Y:%06.4f", Xcur1, Ycur1);
		//	SB_SendSerial (SYS.sfd2, WORK2, strlen(WORK2));
			
			
			//-----------------------------------------------------------------------------------//
			//distTMP = sqrt( (Xcur1 * Xcur1) + (Ycur1 * Ycur1) ) ;
			
			//Xcur1L =  (double) ( (unsigned long) (Xcur1 * 1000L) );
			//Ycur1L =  (double) ( (unsigned long) (Ycur1 * 1000L) );
			
			//distTMP = M_llsqrt( (Xcur1 * Xcur1) + (Ycur1 * Ycur1) ) ;
			//distTMP = M_llsqrt( (Xcur1L * Xcur1L) + (Ycur1L * Ycur1L) ) ;
			
			/**
			distTMP = DDsqrt3f( (Xcur1 * Xcur1) + (Ycur1 * Ycur1) ) ;  //<==double!!
			//-----------------------------------------------------------------------------------//
			sprintf (WORK2, "\n\rDistanc X:%f \t Y:%f  Dist=%f  Dist=%.3f", Xcur1, Ycur1, distTMP, distTMP);
			SB_SendSerial (SYS.sfd2, WORK2, strlen(WORK2));
			**/
			
			
			//distTMP = DDsqrt3f( (Xcur1 * Xcur1) + (Ycur1 * Ycur1) ) ;  //<==double!!
			// //distTMP = hypot( Xcur1, Ycur1) ;
			//-----------------------------------------------------------------------------------//			
			distTMP = sqrt( (Xcur1 * Xcur1) + (Ycur1 * Ycur1) ) ;  //<==double!!
			//------------------------------------------------------//((math.h))
			
			distTMP = distTMP - Dzero1;								//=======//:double!
			
			
			sprintf (WORK2,"%.2f", distTMP );	//sprintf (WORK2,"%.3f", distTMP );	
			//
			distOUT = (double) strtod( WORK2, &endp );
			//
			distXY = (unsigned long) (distOUT * 100) ;		//100.0;  //1000.0; ""cm<=mm""
			//----------------------------------------------//:unsigned long!
			
			
		//	sprintf (WORK2, "\n\rDistanc X:%f \t Y:%f  Dist=%f  Dist=%d", Xcur1, Ycur1, tmpD, distXY);
		//	SB_SendSerial (SYS.sfd2, WORK2, strlen(WORK2));
			
			
			if ( distXY < 100 ) distXY = 100;  							//=="1.00m"	 			//60000==0x0EA60
			if ( distXY > 999999 ) distXY = 999999;  					// 999999;  ""cm<=mm""  //99999==0x1869F
			
			distXYout = (0x00000007 & atoi(&gdcA[6][0]) ) << 20 ; 	//<---GPS-RQ   // RQ: &gdcA[6][0]
			
			//distXYout = distXYout | 0x00800000 | distXY ;				//<---GPS-ACT!
			if ( useGPS ) { distXYout = distXYout | 0x00800000 | distXY ; }	//<---GPS-ACT!
			else {			distXYout = distXYout | distXY ; }					//<---GPS-UnACT!OFF!!
			
			
			distWcnt=0;
			//-------//
			
			//sprintf (WORK2, "\n\r$%06Xh (%.3f)\r", distXYout, (float)distXY / 1000.0 );	//""[mm]""
			//sprintf (WORK2, "\n\r$%08Xh (%.2f)\r", distXYout, (float)distXY / 100.0 );  	//""[cm]""
			sprintf (WORK2, "\n\r$%08Xh (%06d)\r", distXYout, distXY );  					//""[cm]""
			//--------------------------------------Rio::Mask!!!-----------------------//
			SB_SendSerial (SYS.sfd2, WORK2, strlen(WORK2));
			//==========================================================>>[Send-to-RIO]!!
			
			
			SB_SendUdpClient (SYS.PLC_lfd, WORK2, strlen(WORK2), SYS.PLC_Rip, SYS.PLC_Rport);
			//================================================================================>> to PLC
			
			
			/**
			//Ethernut:: sprintf(str1, "SK:%2s  RT:%2s  RQ:%2s ",&gdcA[7][0], &gdcA[13][0], &gdcA[6][0] );
			01234567890123456789
			--------------------
			Dist:999.99M  RQ:4
			X:123456.1234 RT:0
			Y:123456.1234 SK:12
			DGPS Distance Meter
			--------------------
			**/	
			
			//sprintf (WORK2, "Dist:%7.3f", (double)distXY / 1000.0 );  //...hunt~
			
			sprintf (WORK2, "Dist:%6.2fm ", distOUT); 	uif_xySTR ( 0, 0, WORK2 );
			sprintf (WORK2, "X:%s ", &gdcX[0] );		uif_xySTR ( 0, 1, WORK2 );
			sprintf (WORK2, "Y:%s ", &gdcY[0] );		uif_xySTR ( 0, 2, WORK2 );
			
			sprintf (WORK2, "RQ:%2s", &gdcA[6][0]); 	uif_xySTR ( 15, 0, WORK2 );
			sprintf (WORK2, "RT:%2s", &gdcA[13][0] );	uif_xySTR ( 15, 1, WORK2 );
			sprintf (WORK2, "SK:%2s", &gdcA[7][0] );	uif_xySTR ( 15, 2, WORK2 );
			
			
			if ( mCnt++ >= 4 )
			{
			mCnt=0;
			
			SB_SendUdpClient (SYS.mon_lfd, WORK, strlen(WORK), SYS.mon_Rip, SYS.mon_Rport);
			
			sprintf(WORK2, "L=%06.2f,SK:%2s,RT:%2s,RQ:%2s, 0x%X ", (distOUT), &gdcA[7][0], &gdcA[13][0], &gdcA[6][0], distXYout );
			SB_SendUdpClient (SYS.mon_lfd, WORK2, strlen(WORK2), SYS.mon_Rip, SYS.mon_Rport);
			
			}
			
		}
		else if ( !strncmp( WORK, "$GPS_ACT_MODE", 13) )
		{
			if ( !strncmp( WORK, "$GPS_ACT_MODE,ADMIN,ON", 22) )
			{
				sprintf(WORK, "/sbin/def useGPS ON "); 		system(WORK); SB_msleep (100);
				sprintf(WORK, "/sbin/def save "); 			system(WORK); SB_msleep (100);
				sprintf(WORK, "/bin/sync "); 				system(WORK); SB_msleep (100);
				sprintf(WORK, "/sbin/reboot "); 			system(WORK); SB_msleep (100);
				SB_msleep (2000);  //....................................................
			}
			else if ( !strncmp( WORK, "$GPS_ACT_MODE,ADMIN,OFF", 23) )
			{
				sprintf(WORK, "/sbin/def useGPS OFF "); 	system(WORK); SB_msleep (100);
				sprintf(WORK, "/sbin/def save "); 			system(WORK); SB_msleep (100);
				sprintf(WORK, "/bin/sync "); 				system(WORK); SB_msleep (100);
				sprintf(WORK, "/sbin/reboot "); 			system(WORK); SB_msleep (100);
				SB_msleep (2000);  //....................................................			
			}
		}

	}
	else
	{
	SB_SendSerial (SYS.sfd, WORK, len);  //..org
	}



	if (SB_DEBUG) SB_LogDataPrint ("L->S", WORK, len); 
	//if (portview)  PSM_write ('S', WORK, len);

	return 0;
	
GPSrxErr:
	uif_xySTR ( 0, 0, "GDM_err:Rx_XY_none!" );
	return 0;
} 

//===============================================================================
int receive_from_port (int protocol)
{
int len;

//
int idx;
char WORK2 [MAX_BUFFER_SIZE];


	len = SB_ReadSerial (SYS.sfd, WORK, MAX_SERIAL_READ_SIZE, SYS.wait_msec);

		//  len = read(SYS.sfd,WORK,512);  //<---??
		//	WORK[len]=0;
	
	if (len <= 0) return 0;	
	
	
	if(1)  //..LJS..
	{
		WORK[len]=0;
		
		//sprintf (WORK2, "\r\n==[RX_cnt]==> %d == %d \r\n", len, strlen(WORK) );
		//SB_SendUdpClient (SYS.lfd, WORK2, strlen(WORK2), SYS.server_ip, SYS.server_port);
		
		if ( (WORK[len-1] == '\r') || (WORK[len-1] == '\n') )
		{
			;	//--Nomal--//
		}
		else
		{
			//WORK[len]='\r'; WORK[len+1]='\n'; WORK[len+2]=0;
			//SB_SendUdpClient (SYS.lfd, WORK, len+2, SYS.server_ip, SYS.server_port);
			
			SB_msleep (20);  // wait(~200byte@115200bps)....for...Line-CRLF
			//
			idx = SB_ReadSerial (SYS.sfd, WORK2, MAX_SERIAL_READ_SIZE, SYS.wait_msec);
			//
			if ( idx )
			{
				//SB_SendUdpClient (SYS.lfd, WORK2, idx, SYS.server_ip, SYS.server_port);
				//
				memcpy (&WORK[len], WORK2, idx);
				len = len + idx;
				
				WORK[len]=0;
			}
			
			//sprintf (WORK2, "\r\n==[XX_cnt]==> %d == %d \r\n", len, strlen(WORK) );
			//SB_SendUdpClient (SYS.lfd, WORK2, strlen(WORK2), SYS.server_ip, SYS.server_port);		
			
		}
		
	}
	
	
	
	if (protocol == SB_UDP_SERVER_MODE)	// server
	{
		SB_SendUdpServer (SYS.lfd, WORK, len);
		
		
		/**
		WORK2[0] = 0x1B;  //27;  //ESC
		WORK2[1] = 0x43;  //"C";  //clr
		SB_SendSerial (SYS.sfd, WORK2, 2);  fflush(0);  SB_msleep (20);
		
		WORK2[0] = 0x1B;  //27;  //ESC
		WORK2[1] = 0x4C;  //"L";  //Locate
		WORK2[2] = 0; //X
		WORK2[3] = 0; //Y
		SB_SendSerial (SYS.sfd, WORK2, 4);  fflush(0);  SB_msleep (1);  //fflush(SYS.sfd);
		
		WORK2[0] = 0x30 + uifKeyTable[ WORK[0] ];  //==EC: uif_Key
		SB_SendSerial (SYS.sfd, WORK2, 1);
		**/
	}
	else {
		SB_SendUdpClient (SYS.lfd, WORK, len, SYS.server_ip, SYS.server_port); 
	}

	if (SB_DEBUG) SB_LogDataPrint ("S->L", WORK, len); 
	//if (portview) PSM_PORT.status = SB_ACTIVE;		
	//if (portview) PSM_write ('R', WORK, len);

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
//==(GDM_EC)==>


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
		SB_SendSerial (SYS.sfd, chD, 2);  fflush(0);  SB_msleep (30);  //20
}

void uif_xy (int x, int y)  // x:0~19 y:0~3 //
{
char chD[8];

		chD[0] = 0x1B;  //27;  //ESC
		chD[1] = 0x4C;  //"L";  //Locate
		chD[2] = x; //X
		chD[3] = y; //Y
		
		SB_SendSerial (SYS.sfd, chD, 4);  fflush(0);  //SB_msleep (1);  //fflush(SYS.sfd);
}

void uif_xyPut (int x, int y, char ch)  // x:0~19 y:0~3 //
{
char chD[8];

		chD[0] = 0x1B;  //27;  //ESC
		chD[1] = 0x4C;  //"L";  //Locate
		chD[2] = x; //X
		chD[3] = y; //Y
		chD[4] = ch;
		
		SB_SendSerial (SYS.sfd, chD, 5);  fflush(0);  //SB_msleep (1);  //fflush(SYS.sfd);
}

void uif_xySTR (int x, int y, char *str)  // x:0~19 y:0~3 //
{
		uif_xy (x,y);
		SB_SendSerial (SYS.sfd, str, strlen(str));  fflush(0);
}

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
/****
static int M_isqrt(int n)
{
	int y, x;

	if (n == 0)
		return 0;
	x = 1;
	do {
		y = x;
		x = n / x;
		x += y;
		x /= 2;
	} while ((x - y) > 1 || (x - y) < -1);

	return x;
}


static long
M_llsqrt(long long a)
{
    long long       prev = ~((long long) 1 << 63);
    long long       x = a;

    if (x > 0) {
        while (x < prev) {
            prev = x;
            x = (x + (a / x)) / 2;
        }
    }

    return (long) x;
}


static double 
DDsqrt(double k)
{
	double a,b,p;
	double e=0.00001;  //<------------ERROR????????????????????????????????????

	a=k;
	p=a*a;
	
	while(p-k>=e)
	{
		b=(a+(k/a))/2;
		a=b;
		p=a*a;
	}
		
	return (double) a;
 }

static double 
DDsqrt3f(double k)
{
	double a,b,p;
	double e=0.001;  //0.00001;

	a=k;
	p=a*a;
	
	while(p-k>=e)
	{
		b=(a+(k/a))/2;
		a=b;
		p=a*a;
	}
		
	return (double) a;
 }

****/


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

//==(GDM_EC)==<
//===============================================================================
/**

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
//===============================================================================

int pjk2gdc(int len)
{

//Segment...fault// char WORK2 [MAX_SOCKET_READ_SIZE];  //[MAX_BUFFER_SIZE];

int idx;
//---------------------------
char *cp1;
	
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

//==(GDM_EC)==<
//===============================================================================



