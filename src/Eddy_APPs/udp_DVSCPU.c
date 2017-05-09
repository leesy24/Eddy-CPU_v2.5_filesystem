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

#include <math.h>  ////-DVSCPU- VisualSpeedSensor


#define MAX_BUFFER_SIZE 		3000
#define MAX_SOCKET_READ_SIZE 	512
#define MAX_SERIAL_READ_SIZE 	512


//============================================== MESSAGE OPTICAL_FLOW PACKING

#define MAVLINK_MSG_ID_OPTICAL_FLOW 100

////typedef struct __mavlink_optical_flow_t
struct mavlink_optical_flow_t
{
 uint64_t time_usec; ///< Timestamp (UNIX)
 float flow_comp_m_x; ///< Flow in meters in x-sensor direction, angular-speed compensated
 float flow_comp_m_y; ///< Flow in meters in y-sensor direction, angular-speed compensated
 float ground_distance; ///< Ground distance in meters. Positive value: distance known. Negative value: Unknown distance
 int16_t flow_x; ///< Flow in pixels in x-sensor direction
 int16_t flow_y; ///< Flow in pixels in y-sensor direction
 uint8_t sensor_id; ///< Sensor ID
 uint8_t quality; ///< Optical flow quality / confidence. 0: bad, 255: maximum quality
////} mavlink_optical_flow_t;
};

struct mavlink_optical_flow_t PX4;

#define MAVLINK_MSG_ID_OPTICAL_FLOW_LEN 26
#define MAVLINK_MSG_ID_100_LEN 26 

/****
#define MAVLINK_MESSAGE_INFO_OPTICAL_FLOW { \
        "OPTICAL_FLOW", \
        8, \
        {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_optical_flow_t, time_usec) }, \
         { "flow_comp_m_x", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_optical_flow_t, flow_comp_m_x) }, \
         { "flow_comp_m_y", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_optical_flow_t, flow_comp_m_y) }, \
         { "ground_distance", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_optical_flow_t, ground_distance) }, \
         { "flow_x", NULL, MAVLINK_TYPE_INT16_T, 0, 20, offsetof(mavlink_optical_flow_t, flow_x) }, \
         { "flow_y", NULL, MAVLINK_TYPE_INT16_T, 0, 22, offsetof(mavlink_optical_flow_t, flow_y) }, \
         { "sensor_id", NULL, MAVLINK_TYPE_UINT8_T, 0, 24, offsetof(mavlink_optical_flow_t, sensor_id) }, \
         { "quality", NULL, MAVLINK_TYPE_UINT8_T, 0, 25, offsetof(mavlink_optical_flow_t, quality) }, \
         } \
} 
****/

//============================================== MESSAGE OPTICAL_FLOW PACKING



struct eddy_gpio_struct
{
    unsigned int value[3];
    unsigned int mode[3];
    unsigned int pullup[3];
    unsigned int enable[3];
};

/**
void Multi_output_ioctl (int repeat);
void Port_output_ioctl (int repeat);
void Multi_input_ioctl ();
void Port_input_ioctl ();
**/
void BX_Port_output_ioctl (int OnOff);
void MultiX_Port_output_ioctl (int OnOff);
//
//#define RelayON  BX_Port_output_ioctl(0);
//#define RelayOFF BX_Port_output_ioctl(1);
//
#define RelayON  MultiX_Port_output_ioctl(0);
#define RelayOFF MultiX_Port_output_ioctl(1);



int Product_ID;



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
	int				wait_msec;      //---! DVSCPU !   (Run/Stop-width!!!)
	int				socket;
	char  			bps;
	char 			dps;
	char 			flow;
	char			interface;
	char 			server_ip [100];
	int				server_port;	
	//
	int 			keepalive;      //---! DVSCPU !   (Run/Stop-Point!!!)
	
	
	//-----------------------------------! DVSCPU !   (Com-2)....PX4FLOW
	int				sfd2;
	int				port_no2;
	int				protocol2;
	char  			bps2;
	char 			dps2;
	char 			flow2;
	char			interface2;
	
	//-----------------------------------! DVSCPU !   (Com-3/4)
	int				sfd3;
	int				port_no3;
	int				protocol3;
	char  			bps3;
	char 			dps3;
	char 			flow3;
	char			interface3;
	//
	int				Vdelay;  ////<-----------For Delay_Value_of_OUTPUT-RELAY
	//-----------------------------------!!
	int				sfd4;
	int				port_no4;
	int				protocol4;
	char  			bps4;
	char 			dps4;
	char 			flow4;
	char			interface4;
	//-----------------------------------!!	
	
};	

struct sys_info  SYS;

int port_no;
int portview;
int snmp;
struct SB_PORTVIEW_STRUCT *PSM;
struct SB_SNMP_STRUCT	  *SSM;

char WORK [MAX_BUFFER_SIZE];
char WORK2 [MAX_BUFFER_SIZE];  //// temp_BUF
//
char WORK3 [MAX_BUFFER_SIZE];
char WORK4 [MAX_BUFFER_SIZE];
//

char v2Last[MAX_BUFFER_SIZE];
int  v2LastN;



int v3Mode;
int v3Cnt;
int v3CntS;
int v3CntSL;
char v3BUF[MAX_BUFFER_SIZE];
//
char v3Last[MAX_BUFFER_SIZE];
int  v3LastN;
//
char v3Xbuf[10];
int v3Xcnt;
int v3Xd;  //, v3Xt;
long v3Xt;
int v3Xerr;
//
char v3Ybuf[10];
int v3Ycnt;
int v3Yd;  //, v3Yt;
long v3Yt;
int v3Yerr;

char* cp0;
char* cp1;
char* cp2;
//
char* cp3;  //sQuality...A3080
int v3SQ;
char chSQ[5];
//
int v3RXcnt;


unsigned long tM1, tM2; 	////SB_GetTick (Void);
int F5cnt;
int Fmode;
int FkeyLast;
int FkeyTout;
//
int F1cnt;
int F2cnt;
int F3cnt;
int F4cnt;
//
int Rdelay;



char RX_last[256];
int  RX_LastN;
unsigned long  RX_lastTick;


char IDs[32][10];

int	SB_DEBUG	=	1;

////struct SB_CONFIG cfg;

int main (int argc, char *argv[]);

void mainloop(int protocol);
int receive_from_lan(void);

int receive_from_port (int protocol);
int receive_from_port2 (int protocol);
int receive_from_port3 (int protocol);

void close_init ();


/****
void get_portview_memory ();
void get_snmp_memory ();
//int PA_login();

void SSM_write ();
void PSM_write (char sw, char *data, int len);
****/


/**
char * sHEX(int dat)
{
int d1;

	d1 = dat >> 4;
	if( d1 < 10 ) 	{	Send232(d1 + 0x30); }
	else			{	Send232(d1-10 + 0x41); }
	d1 = dat & 0x0f;
	if( d1 < 10 ) 	{	Send232(d1 + 0x30); }
	else			{	Send232(d1-10 + 0x41); }
}
**/



void uif_clr ();
void uif_xy (int x, int y);
void uif_xyPut (int x, int y, char ch);
void uif_xySTR (int x, int y, char *str);

void ConfigPP( int kp );  ////DVSCPU
void ConfigWW( int kp );  ////DVSCPU

void ConfigDELAY( int V );  ////DVSCPU


//...Kalman...
double transition_noise = 0.1; // covariance of Gaussian noise to control
double measurement_noise = 0.3; // covariance of Gaussian noise to measurement
double x = 0.0, v = 1.0;
double cov = 0.5;
double x_p, c_p; // prediction of x and cov
double gain; // Kalman gain
double x_pre, m;
//...

int Kalman( double RXm )
{
	// prediction
	x_pre = x;
	x_p = x + v;
	c_p = cov + transition_noise;
	m = RXm;  //<---!

	// update
	gain = c_p / (c_p + measurement_noise);
	x = x_p + gain * (m - x_p);
	cov = ( 1 - gain ) * c_p;
	v = x - x_pre;

	return( (int)x );
}



//===============================================================================	
int main (int argc, char *argv[])
{
int i;

int fd;
struct eddy_gpio_struct gpio;


	//Not-USE@2.5  //SB_SetPriority (5);						// Min 1 ~ Max 99 
	

	//SB_ReadConfig (CFGFILE_ETC_SIO, (char *)&cfg[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);
	//
	//------------------------- Read /etc Config Data
	SB_ReadConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, 		sizeof (struct SB_SYSTEM_CONFIG));
	SB_ReadConfig  (CFGFILE_ETC_SIO,     (char *)&CFG_SIO[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
	SB_ReadConfig  (CFGFILE_ETC_SNMP,    (char *)&CFG_SNMP,		sizeof(struct SB_SNMP_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_GPIO,    (char *)&CFG_GPIO,		sizeof(struct SB_GPIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_DIO,     (char *)&CFG_DIO,		sizeof(struct SB_DIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_WIFI,    (char *)&CFG_WIFI,		sizeof(struct SB_WIFI_CONFIG));

  /******
	Product_ID = SB_GetModel ();

	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) { 
		
		uif_clr ();
		uif_xy (0,0);
		sprintf (WORK, "DVS-CPU ..IO-Open-Erro..");
		SB_SendSerial (SYS.sfd, WORK, strlen(WORK));
		
		if (SB_DEBUG) SB_LogDataPrint ("L->S", WORK, strlen(WORK) );
		
		return 0;
	}

	if (Product_ID == EDDY_S4M) {
		gpio.enable[0]	= S4M_GPIO_MASK_PORT_A;					// Port A, Serial 2=2					Total 2  Gpio Ports
		gpio.enable[1]	= S4M_GPIO_MASK_PORT_B;					// Port B, Serial 0=8, 1=4, 2=2, 3=2	Total 16 Gpio Ports
		gpio.enable[2]	= S4M_GPIO_MASK_PORT_C  | 0x00000003;	// Port C, Serial 3=2					Total 2  Gpio Ports

		gpio.mode[0]	= S4M_GPIO_MASK_PORT_A;					// Output Mode
		gpio.mode[1]	= S4M_GPIO_MASK_PORT_B;					// Output Mode
		gpio.mode[2]	= S4M_GPIO_MASK_PORT_C  | 0x00000003;	// Output Mode

		gpio.value[0]	= S4M_GPIO_MASK_PORT_A;					// Value High
		gpio.value[1]	= S4M_GPIO_MASK_PORT_B;					// Value High
		gpio.value[2]	= S4M_GPIO_MASK_PORT_C;					// Value High

		gpio.pullup[0]	= S4M_GPIO_MASK_PORT_A;					// pullup High
		gpio.pullup[1]	= S4M_GPIO_MASK_PORT_B;					// pullup High
		gpio.pullup[2]	= S4M_GPIO_MASK_PORT_C | 0x00000003;	// pullup High
	}

	ioctl (fd, SETGPIOINIT, &gpio);	// Init GPIO	
	close (fd);

	RelayOFF;
  ******/
	
	
	RX_last[0]=0;
	RX_lastTick=0;
	RX_LastN=0;



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
		//
		SYS.keepalive	= CFG_SIO[port_no].keepalive;  			////DVSCPU
		//
		if ( SYS.keepalive < 100 ) { SYS.keepalive = 100; } 	////DVSCPU

		/****/
		if ( (port_no == 0) && !strncmp( CFG_SYS.device_name, "DVSCPU", 6) ) 
		{
			if (CFG_SIO[1].protocol == SB_DISABLE_MODE)
			{
				//-------(for SYS.sfd2)------------------------!! PX4FLOW
				SYS.bps2		= CFG_SIO[1].speed;
				SYS.dps2		= CFG_SIO[1].dps;
				SYS.flow2		= CFG_SIO[1].flow;
				SYS.interface2	= CFG_SIO[1].interface;
			}		
			if (CFG_SIO[2].protocol == SB_DISABLE_MODE)
			{
				//-------(for SYS.sfd3)------------------------!!
				SYS.bps3		= CFG_SIO[2].speed;
				SYS.dps3		= CFG_SIO[2].dps;
				SYS.flow3		= CFG_SIO[2].flow;
				SYS.interface3	= CFG_SIO[2].interface;
				//
				SYS.Vdelay =  ( CFG_SIO[2].remote_socket_no );  // & 0x0003;  //<----0,1,2
			}
			if (CFG_SIO[3].protocol == SB_DISABLE_MODE)
			{
				//-------(for SYS.sfd4)------------------------!!
				SYS.bps4		= CFG_SIO[3].speed;
				SYS.dps4		= CFG_SIO[3].dps;
				SYS.flow4		= CFG_SIO[3].flow;
				SYS.interface4	= CFG_SIO[3].interface;
			}
		}
		/****/
		

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
		
		
		v3Xt = 0;
		v3Yt = 0;
		v3Xerr = 0;
		v3Yerr = 0;
		
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
unsigned long t1, t2; 	////SB_GetTick (Void);
//int len1;
int mID;

int i,j;
double Dtmp, Dx, Dy;
int DelayCNToff;
int DelayCNTon;
char msg1[256];
//
double DV[11];
int DVcnt;
int DVidx;
//
int DVkalman;  //Kalman

//char Sc[] = { '|', '/','-','\\','|',' ' };
//char Sc[] = { '.', 'o','O','0','o',' ' };
char Sc[] = { '.', 'o','O','*',' ',' ' };


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
	

	if ( (port_no < 2) && !strncmp( CFG_SYS.device_name, "DVSCPU", 6) )  
	{
		if (CFG_SIO[1].protocol == SB_DISABLE_MODE)
		{
			if (SYS.sfd2 <= 0)
			{
				SYS.sfd2 = SB_OpenSerial (1);   //<----[2==P2] ////-DVSCPU--------PX4FLOW:::RS-232
				if (SYS.sfd2 <= 0) return;
			}
			SB_InitSerial (SYS.sfd2, SYS.bps2, SYS.dps2, SYS.flow2);
			SB_ReadSerial (SYS.sfd2, WORK, MAX_BUFFER_SIZE, 0);
			//
			SB_SetDtr (SYS.sfd2, SB_ENABLE);
			SB_SetRts (SYS.sfd2, SB_ENABLE);
		}
		
		if (CFG_SIO[2].protocol == SB_DISABLE_MODE)
		{
			if (SYS.sfd3 <= 0)
			{
				SYS.sfd3 = SB_OpenSerial (2);   //<----[2==P3] ////-DVSCPU-
				if (SYS.sfd3 <= 0) return;
			}
			SB_InitSerial (SYS.sfd3, SYS.bps3, SYS.dps3, SYS.flow3);
			SB_ReadSerial (SYS.sfd3, WORK3, MAX_BUFFER_SIZE, 0);
			//
			interface = SB_RS485_NONE_ECHO;
			ioctl (SYS.sfd3, INTERFACESEL, &interface);
		}
		if (CFG_SIO[3].protocol == SB_DISABLE_MODE)
		{
			if (SYS.sfd4 <= 0)
			{
				SYS.sfd4 = SB_OpenSerial (3);   //<----[3==P4] ////-DVSCPU-
				if (SYS.sfd4 <= 0) return;
			}
			SB_InitSerial (SYS.sfd4, SYS.bps4, SYS.dps4, SYS.flow4);
			SB_ReadSerial (SYS.sfd4, WORK4, MAX_BUFFER_SIZE, 0);
			//
			interface = SB_RS485_NONE_ECHO;
			ioctl (SYS.sfd4, INTERFACESEL, &interface);
		}
		
		uif_clr ();
		uif_xy (0,0);
		sprintf (WORK, "DVS-CPU V1.5 __boot");
		SB_SendSerial (SYS.sfd, WORK, strlen(WORK));
		uif_xy (0,1);
		sprintf (WORK, "P=%04d W=%03d ", SYS.keepalive, SYS.wait_msec);
		SB_SendSerial (SYS.sfd, WORK, strlen(WORK));
		
		
		v2LastN = 0;
		
		v3Mode=0;
		v3CntSL=0;
		//
		v3LastN = 0;
		//
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
	
	DelayCNToff = 0;
	DelayCNTon = 0;
	
	tM1 = SB_GetTick ();  ////
	F5cnt = 0;
	Fmode = 0;
	FkeyLast = 0;
	FkeyTout = 0;

	F1cnt = 0;
	F2cnt = 0;
	F3cnt = 0;
	F4cnt = 0;
	//
	Rdelay = SYS.Vdelay;
	//
	if ( (Rdelay<0) || (Rdelay>2) )  { ConfigDELAY( 0 ); SYS.Vdelay=0; Rdelay=0; }


	DVcnt=0;
	DVidx=0;
	
	v3RXcnt=0;

	while (1)
	{
		if ( (SB_GetTick() - tM1) > 2000)  ////
		{
			tM1 = SB_GetTick();
			
			
			if ( Fmode == 0 ) {
				if ( F1cnt > 3 )  { ConfigDELAY( 0 ); SYS.Vdelay=0; Rdelay=0; }
				if ( F2cnt > 3 )  { ConfigDELAY( 1 ); SYS.Vdelay=1; Rdelay=1; }
				if ( F3cnt > 3 )  { ConfigDELAY( 2 ); SYS.Vdelay=2; Rdelay=2; }
			}
			
			if ( F5cnt > 5 )
			{
				uif_clr ();
				uif_xy (0,0);
				sprintf (WORK, "DVS-CPU V1.5 Reboot!");
				SB_SendSerial (SYS.sfd, WORK, strlen(WORK));
			
				sprintf(msg1, "reboot"); system (msg1);
			}
			else if ( F5cnt > 2 )
			{
				if ( Fmode < 1 ) Fmode=1;  //==>Start--Config!!
			}
			else if ( F5cnt > 0 )
			{
				if ( Fmode > 0 ) Fmode=0;  //==>Stop--Config!!
			}
			
			if ( Fmode > 0 )
			{
					if ( (FkeyLast > 0) && (FkeyLast < 5) )  //// F1 F2 F3 F4
					{
						switch ( FkeyLast )
						{
							case 1 :	////if ( SYS.keepalive < 9900 )  ConfigPP( 100 );
										if ( SYS.keepalive < 500 ) 			ConfigPP( 20 * F1cnt );
										else if ( SYS.keepalive < 9900 )  	ConfigPP( 100 );
								break;
								
							case 2 :	//if ( SYS.keepalive > 100 )  ConfigPP( -100 );
										if ( SYS.keepalive > 599 )  ConfigPP( -100 );
										else if ( SYS.keepalive > 149)  ConfigPP( -20 * F2cnt  );
								break;
								
							case 3 :	if ( SYS.wait_msec < 900 )  ConfigWW( 10 * F3cnt );
								break;
								
							case 4 :	if ( SYS.wait_msec >= 20 )  ConfigWW( -10 * F4cnt );
								break;
								
							default :
								break;
						}
						uif_xy (0,1);
						sprintf (WORK, "P=%04d W=%03d ", SYS.keepalive, SYS.wait_msec);
						SB_SendSerial (SYS.sfd, WORK, strlen(WORK));
						
						FkeyTout = 0;
					}
					else
					{
						if ( FkeyTout++ > 5 )
						{
							FkeyTout=0;
							Fmode=0;  //==>Stop--Config!!
						}
					}
					
				uif_xy (15,1);  SB_SendSerial(SYS.sfd, "Edit!", 5);
			}
			else
			{
					uif_xy (0,0);
					sprintf (WORK, "DVS-CPU V1.5 1310oF1");
					SB_SendSerial (SYS.sfd, WORK, strlen(WORK));  //fflush(0); //===>
					uif_xy (0,1);
					sprintf (WORK, "P=%04d W=%03d ", SYS.keepalive, SYS.wait_msec);  
					SB_SendSerial (SYS.sfd, WORK, strlen(WORK));  //fflush(0); //===>
			
				uif_xy (15,1);  SB_SendSerial(SYS.sfd, "_run_", 5);  //fflush(0); //===> 
				
					uif_xy (0,2);
					sprintf (WORK, ":%1d", Rdelay);
					SB_SendSerial (SYS.sfd, WORK, 2);				//fflush(0); //===> 
			}
			
			F5cnt = 0;
			FkeyLast = 0;
			
			F1cnt=0;
			F2cnt=0;
			F3cnt=0;			
			F4cnt=0;			
			
			fflush(0);  ////<===============>////
		}
		
		
		if ( (SB_GetTick() - t1) > 300)  ////
		{
			t2 = SB_GetTick() - t1;
			t1 = SB_GetTick();
		
			//if ( (v3Xt > 0) || (v3Yt > 0) ) {
			
				if ( v3Xt > 5999 )  { v3Xt = 5999; }
				if ( v3Yt > 5999 )  { v3Yt = 5999; }
			
			
				//sQ_Dir//
				if ( v3RXcnt == 0 ) { strcpy(chSQ, "Qx");  v3SQ=0; }
				else {
					strcpy(chSQ, "Q:");
					if ( (v3Xd != 0) && (v3Yd != 0) ) {
						if ( ( (abs(v3Yd) * 100) / abs(v3Xd) ) > 15 ) 	//Dir====Angle!! 	
						{
							if ( v3Yd < 0 )  { strcpy(chSQ, "Q\\"); } 	// <<---YY
							else 			 { strcpy(chSQ, "Q/"); } 	// YY--->>
						}
					}
				}
				
			
				v3Xd = abs(v3Xd);
				v3Yd = abs(v3Yd);
			
				Dx = (double)v3Xd;  ////v3Xt;
				Dy = (double)v3Yd;  ////v3Yt;
				Dtmp = sqrt( (Dx * Dx) + (Dy * Dy) ) ;  //<==SQRT:double!!
				
				
				//DVkalman = Kalman ( Dtmp );
				///////////////////////////
				//
				//sprintf( msg1,"%05d",DVkalman);
				//uif_xy(15,2);  SB_SendSerial(SYS.sfd, msg1, strlen(msg1));				
				
				
				DV[DVidx] = Dtmp;
				DVcnt++;  if ( DVcnt > 5 ) DVcnt=5;
				DVidx++;  if ( DVidx > 4 ) DVidx=0;
				
				if ( DVcnt >= 5 ) {   ////_Filt1___Average!!....5-Time!!!
					DV[10]=0;
					for ( i=0 ; i<5 ; i++ )  DV[10] = DV[10] + DV[i];
					Dtmp = DV[10] / 5;
				}
				
			/*****/
				msg1[0]=0;
				if ( Dtmp > (SYS.keepalive ) )   // + SYS.wait_msec ) )  //==400
				{
					if ( DelayCNTon++ > Rdelay ) {  RelayON;  DelayCNTon = 0;    strcpy( msg1,"BELT=RUN!!"); }
					DelayCNToff = 0;
				}
				//
				if ( Dtmp < (SYS.keepalive - SYS.wait_msec ) )  //==200
				{
					if ( DelayCNToff++ > Rdelay ) { RelayOFF;  DelayCNToff = 0;  strcpy( msg1,"BELT=STOP!"); }
					DelayCNTon = 0;
				}
				//
				if ( v3RXcnt == 0 ) { strcpy( msg1,"SENSOR_off"); }
				if ( strlen(msg1) ) { uif_xy(4,2);  SB_SendSerial(SYS.sfd, msg1, strlen(msg1)); }
				//
			/*****/
				//fflush(0); //===>
				
				/****/
				if ( (v3SQ < 0) || (v3SQ > 255) ) v3SQ=0;
				uif_xy (0,3);
				sprintf (  WORK   ,"%3d %s%03d SP=%d", (int)t2, chSQ, v3SQ, (int)Dtmp);   //<=============//sQuality::
				////sprintf ( &WORK[strlen(WORK)]," %d,%d", (int)v3Xt, (int)v3Yt);
				//sprintf ( &WORK[strlen(WORK)]," %d,%d", (int)v3Xd, (int)v3Yd);
				//
				j = strlen(WORK);
				if (j < 20) {  for ( i=j ; i<20 ; i++ )  WORK[i]=' ';  }
				//
				WORK[19]=Sc[DVidx];
				//
				WORK[20]=0;
				//
				SB_SendSerial (SYS.sfd, WORK, strlen(WORK));  fflush(0); //===>
				/****/
				
				sprintf ( WORK2,"\n\r%s %s\n\r", WORK, msg1);
				SB_SendUdpClient (SYS.lfd, WORK2, strlen(WORK2), SYS.server_ip, SYS.server_port);  fflush(0); //===>
					
				//if ( strlen(msg1) ) {
				//	SB_SendUdpClient (SYS.lfd, msg1, strlen(msg1), SYS.server_ip, SYS.server_port);  fflush(0); //===>
				//}
				
				
				v3Xt = 0;
				v3Yt = 0;
				
				v3Xd = 0;
				v3Yd = 0;
				
				v3RXcnt=0;
				
				//fflush(0);  ////<===============>////
				
			//}
			
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
		
		if (receive_from_port(protocol) < 0) return;    // DVSCPU..UIF
		
		if (receive_from_port2(protocol) < 0) return;   // DVSCPU..RS232--PX4FLOW
		
		if (receive_from_port3(protocol) < 0) return;   // DVSCPU..485A--Sensor

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
	
	SB_SendSerial (SYS.sfd, WORK, len);		
	
	if (SB_DEBUG) SB_LogDataPrint ("L->S", WORK, len); 

	////if (portview) PSM_PORT.status = SB_ACTIVE;	
	////if (portview)  PSM_write ('S', WORK, len);
	
	return 0;
}


//===============================================================================
int receive_from_port (int protocol)  //<-----"KEY-IN---UIF" {1,2,3,4,5}
{
int len;

	len = SB_ReadSerial (SYS.sfd, WORK, MAX_SERIAL_READ_SIZE, 0);  //// SYS.wait_msec);
	if (len <= 0) return 0;


	if ( (WORK[len-1] > 0) && (WORK[len-1] < 6) )
	{
		FkeyLast = WORK[len-1];
		
		//if ( WORK[len-1] == 5 )  F5cnt++;
		//
		switch ( WORK[len-1] )
		{
			case 5 :	F5cnt++;
				break;
				
			case 1 :	F1cnt++;
				break;
				
			case 2 :	F2cnt++;
				break;
				
			case 3 :	F3cnt++;
				break;
				
			case 4 :	F4cnt++;
				break;
				
			default :
				break;
		}		
		
		
		len = WORK[len-1];
		sprintf( WORK,"%d",len);
		uif_xy (19,1);  SB_SendSerial(SYS.sfd, WORK, 1);  fflush(0); //===>
	}


	/****
	if (protocol == SB_UDP_SERVER_MODE)	// server
		SB_SendUdpServer (SYS.lfd, WORK, len);
	else
		SB_SendUdpClient (SYS.lfd, WORK, len, SYS.server_ip, SYS.server_port);
	****/

	//if (portview) PSM_PORT.status = SB_ACTIVE;		
	if (SB_DEBUG) SB_LogDataPrint ("S->L", WORK, len); 
	//if (portview) PSM_write ('R', WORK, len);
	return 0;
}

//===============================================================================
int receive_from_port2 (int protocol)  //<-----""""""PX4FLOW-Serial""""""
{
int len;

int i;


	len = SB_ReadSerial (SYS.sfd2, WORK2, MAX_SERIAL_READ_SIZE, 0);  //// SYS.wait_msec);
	if (len <= 0) return 0;

	if (v2LastN > 0) {
		//memcpy (WORK2 + len, v2Last, v2LastN); ////<=========<<<Coding_ERROR???????>>>?????
		memcpy (v2Last + v2LastN, WORK2, len);  //...Add!
		len = len + v2LastN;
		memcpy (WORK2, v2Last, len); //.............Move!
		v2LastN = 0;
		WORK2[len] = 0;
	}

	cp1 = memchr( WORK2, 0x0fe, 1 );
	///
	if ( cp1 != WORK2 )
	{
		if ( len > (int)(cp1 - WORK2) )
		{
			len = len - (int)(cp1 - WORK2);
			memcpy (v2Last, cp1, len);    ////..delete_before_0xFE..
			memcpy (WORK2, v2Last, len);
		}
	}

	if (len < 34 ) {
		memcpy (v2Last, WORK2, len); 
		v2LastN = len;
		v2Last[v2LastN] = 0;
		return 0;
	}

	/**** (PX4FLOW_DATA)==34Byte!!
	00000000h: FE 1A E0 51 32 64 20 70 29 14 00 00 00 00 00 00 ; ??2d p).......
	00000010h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4D 00 ; ..............M.
	00000020h: D2 42 FE 1A E1 51 32 64 E8 A2 29 14 00 00 00 00 ; ???2dè¢).....
	00000030h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
	00000040h: 4D 00 22 36 FE 1A E2 51 32 64 B0 D5 29 14 00 00 ; M."6??2d°Õ)...
	00000050h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
	00000060h: 00 00 4D 00 49 AF 
	
	[00-01-02-03-04-05-06-07-08-09-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-26-27-28-29-30-31-32-33]
	"FE 1A E0 51 32 64 20 70 29 14 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4D 00 D2 42"
	[xx-xx-xx-xx-xx-xx-00-01-02-03-04-05-06-07-08-09-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-CC-CC]	
	
	FE-1A-7B-51-32-64-D8-11-64-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-FF-FF-FD-FF-4D-00-50-27
	FE-1A-7C-51-32-64-B8-40-64-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-02-00-02-00-4D-00-32-8D
	390 00000 0,0        BELT=STOP!
	FE-1A-7D-51-32-64-98-6F-64-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-FF-FF-FC-FF-4D-00-A9-65
	FE-1A-7E-51-32-64-90-9A-64-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-02-00-FE-FF-4D-00-7B-85
	FE-1A-7F-51-32-64-58-CD-64-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-01-00-FD-FF-4D-00-3F-22
	400 00000 0,0      . 
	FE-1A-80-51-32-64-50-F8-64-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-FF-FF-4D-00-DB-43
	FE-1A-81-51-32-64-30-27-65-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-01-00-00-00-4D-00-A8-DB
	FE-1A-82-51-32-64-10-56-65-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-4D-00-9D-C6
	390 00000 0,0      o 
	FE-1A-83-51-32-64-F0-84-65-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-03-00-FE-FF-4D-00-07-BA
	FE-1A-84-51-32-64-E8-AF-65-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-FF-FF-FE-FF-4D-00-71-C2
	FE-1A-85-51-32-64-C8-DE-65-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-03-00-01-00-4D-00-81-29
	FE-1A-86-51-32-64-60-19-66-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-FF-FF-4D-00-36-3F
	FE-1A-87-51-32-64-40-48-66-1B-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-4D-00-AE-6C
	
	****/

	if ( (WORK2[0]==0xfe)&&(WORK2[1]==0x1a)&&(WORK2[5]==0x64) )
	{
		memcpy (&PX4, &WORK2[6], 26);
		//
		sprintf(v2Last,"\n\r=>PX4: %d, %f, %f, %f, %d, %d, %d, %d", \
		PX4.time_usec, \
		PX4.flow_comp_m_x, \
		PX4.flow_comp_m_y, \
		PX4.ground_distance, \
		PX4.flow_x, \
		PX4.flow_y, \
		PX4.sensor_id, \
		PX4.quality \
		);
		//
		if ( ((0x00ff & WORK2[2])%10)==0 )
			SB_SendUdpClient (SYS.lfd, v2Last, strlen(v2Last), SYS.server_ip, SYS.server_port);  // 1-of-10 //
	
		/***
		for ( i=0; i<34 ; i++ ) {	////(j < 0 ? -j : j)////
			v2Last[i*3 ]   = ( (WORK2[i]>>4) < 10 ? (WORK2[i]>>4) + 0x30 : ((WORK2[i]>>4)-10) + 0x41 );
			v2Last[i*3+1 ] = ( (WORK2[i]&0xf) < 10 ? (WORK2[i]&0xf) + 0x30 : ((WORK2[i]&0xf)-10) + 0x41 );
			v2Last[i*3+2 ] = '-';
		}
		v2Last[ i*3 -1 ] = 0x0D;
		v2Last[ i*3 ]    = 0x0A;
		v2Last[ i*3 +1 ] = 0;
		
		SB_SendUdpClient (SYS.lfd, v2Last, strlen(v2Last), SYS.server_ip, SYS.server_port);
		***/
		
		if (len > 34 ) {
			memcpy (v2Last, WORK2 + 34, len - 34); 
			v2LastN = len - 34;
			v2Last[v2LastN] = 0;
		}
	}
	else
	{
		v2LastN=0;
	}





	/****
	if (protocol == SB_UDP_SERVER_MODE)	// server
		SB_SendUdpServer (SYS.lfd, WORK, len);
	else
		SB_SendUdpClient (SYS.lfd, WORK, len, SYS.server_ip, SYS.server_port);
	****/

	//if (portview) PSM_PORT.status = SB_ACTIVE;		
	if (SB_DEBUG) SB_LogDataPrint ("S->L", WORK, len); 
	//if (portview) PSM_write ('R', WORK, len);
	return 0;
}

//===============================================================================
int receive_from_port3 (int protocol)
{
int len;
//
int idx;
int i;

//<=//char* cp1;
//<=//char* cp2;
//
int cnt;

char BUF[128];


/****
$ XDd=    0, YDd=    0, CC=16, C=240, X=   -38, Y=   -10, XH=FFDA, YH=FFF6 ;
$ XDd=  284, YDd=  134, CC=15, C=000, X=  -284, Y=   134, XH=FEE4, YH=0086 ;
$ XDd=  626, YDd=  290, CC=16, C=016, X=  -626, Y=   290, XH=FD8E, YH=0122 ;
$ XDd=    0, YDd=    0, CC=16, C=032, X=   -50, Y=   -13, XH=FFCE, YH=FFF3 ;
$ XDd=    0, YDd=    0, CC=15, C=096, X=   -48, Y=   -21, XH=FFD0, YH=FFEB ;
$ XDd=    0, YDd=    0, CC=16, C=112, X=    -5, Y=   -38, XH=FFFB, YH=FFDA ;
$ XDd=  453, YDd=  331, CC=16, C=128, X=   453, Y=  -331, XH=01C5, YH=FEB5 ;
$ XDd=  372, YDd=  311, CC=16, C=144, X=   372, Y=  -311, XH=0174, YH=FEC9 ;
$ XDd=    0, YDd=    0, CC=16, C=160, X=   -39, Y=     3, XH=FFD9, YH=0003 ;
$ XDd=    0, YDd=    0, CC=16, C=176, X=   -20, Y=   -13, XH=FFEC, YH=FFF3 ;
****/

	/////////////////////////////////////////
	//// ---Port3 for PX4--485--115200bps
	/////////////////////////////////////////
	
	len = SB_ReadSerial (SYS.sfd3, WORK3, MAX_SERIAL_READ_SIZE, 0);  /// 50   //SYS.wait_msec);
	if (len <= 0) return 0;

	if (len > 500) return 0;  //...soBig@New-Connect

	if (v3LastN > 0) {
		////memcpy (WORK3 + len, v3Last, v3LastN); ////<=========<<<Coding_ERROR???????>>>?????
		memcpy (v3Last + v3LastN, WORK3, len);  //...Add!
		len = len + v3LastN;
		memcpy (WORK3, v3Last, len); //.............Move!
		v3LastN = 0;
		WORK3[len] = 0;
	}

	if (len <= 75 ) {
		memcpy (v3Last, WORK3, len); 
		v3LastN = len;
		v3Last[v3LastN] = 0;
		return 0;
	}


	cp0 = strstr( WORK3, "$ XDd=" );
	cp1 = strstr( WORK3, ", X=" );
	cp2 = strstr( WORK3, ", Y=" );
	//
	cp3 = strstr( WORK3, ", C=" );


	//uif_xy (0,2);
	//
	if ( (cp0==0) || (cp1==0) || (cp2==0) ) {
		uif_xy (0,2);
		SB_SendSerial (SYS.sfd, "? ", 2);  fflush(0); //===>
		return 0;  ////==========================>
	}
	//
	if ( (cp0 > cp1) || (cp1 > cp2) ) {
		uif_xy (0,2);
		SB_SendSerial (SYS.sfd, "??", 2);  fflush(0); //===>
		return 0;  ////==========================>
	}
	//
	//SB_SendSerial (SYS.sfd, ": ", 2);


	strncpy(BUF, cp1 + 4, 6);
	v3Xd = atoi( BUF );  //<--------Pre-ABS()ed!! from Mega328P
	
	strncpy(BUF, cp2 + 4, 6);
	v3Yd = atoi( BUF );  //<--------Pre-ABS()ed!! from Mega328P

	strncpy(BUF, cp3 + 4, 3);
	v3SQ = atoi( BUF );  //<-------//sQuality!!


	//v3Xd = abs( v3Xd / 2 ) - 10;
	//v3Yd = abs( v3Yd / 2 ) - 10;

	//v3Xd = abs( v3Xd ) - 10;
	//v3Yd = abs( v3Yd ) - 10;
	

	/////////////////////////////////////////////--(Port4==>Port3)
	v3Xt = v3Xt + v3Xd;    ////v3Xt + v3Xd;
	v3Yt = v3Yt + v3Yd;    ////v3Yt + v3Yd;
	/////////////////////////////////////////////--(Port4==>Port3)


	SB_SendUdpClient (SYS.lfd, WORK3, len, SYS.server_ip, SYS.server_port);  fflush(0); //===>


	v3RXcnt++;

	return 0;
	//===========>>>




	/////////////////////////////////////////
	//// ---Port3 for Mice--485--9600bps
	/////////////////////////////////////////
	

	//len = SB_ReadSerial (SYS.sfd3, WORK3, MAX_SERIAL_READ_SIZE, 0);  //SYS.wait_msec);
	len = SB_ReadSerial (SYS.sfd3, WORK3, MAX_SERIAL_READ_SIZE, 0);  /// 50   //SYS.wait_msec);
	if (len <= 0) return 0;

	if (v3LastN > 0) {
		////memcpy (WORK3 + len, v3Last, v3LastN); ////<=========<<<Coding_ERROR???????>>>?????
		memcpy (v3Last + v3LastN, WORK3, len);  //...Add!
		len = len + v3LastN;
		memcpy (WORK3, v3Last, len); //.............Move!
		v3LastN = 0;
		WORK3[len] = 0;
	}

	if (len <= 35 ) {
		memcpy (v3Last, WORK3, len); 
		v3LastN = len;
		v3Last[v3LastN] = 0;
		return 0;
	}


	/*****
	while ( len > 35 )
	{
	*****/
	
		for ( i=0 ; i<len ; i++ )
		{
			if ( WORK3[i] == 0x0A ) {
				v3Cnt=0;
				v3CntS=0;
				v3Mode = 1;   
				
				//i++;  //--->
				continue;
			}
			
			switch ( v3Mode )
			{
			case 1 : 
					if ( WORK3[i] == 0x0D )	v3Mode = 2;
					//					
					break;
					
			case 2 :
					v3BUF[v3Cnt++] = WORK3[i];
					if ( WORK3[i] == ',' )  v3CntS++;
					
				switch ( v3CntS )
				{
					case 4 :
						v3Xcnt = 0;
						v3Ycnt = 0;
						break;
						
					case 5 :
						if ( (WORK3[i] >= '0') && (WORK3[i] <= '9') )
								v3Xbuf[v3Xcnt++] = WORK3[i];
						break;
						
					case 6 :
						if ( (WORK3[i] >= '0') && (WORK3[i] <= '9') )					
								v3Ybuf[v3Ycnt++] = WORK3[i];
						break;
						
					case 7 :
						
						if (v3Xcnt > 0) {
							v3Xbuf[v3Xcnt] = 0;
							v3Xd = atoi(v3Xbuf);
						}
						//
						if (v3Ycnt > 0) {
							v3Ybuf[v3Ycnt] = 0;
							v3Yd = atoi(v3Ybuf);
						}
						
						/********
						if ( (v3Xcnt > 255) || (v3Xcnt < 0) ) {
							v3Xcnt = 0;   v3Xerr++;
						}
						if ( (v3Ycnt > 255) || (v3Ycnt < 0) ) {
							v3Ycnt = 0;   v3Yerr++;
						}
						********/
						
						/****
						uif_xy (14,3);
						sprintf (WORK," %02d,%02d", v3Xerr, v3Yerr);
						SB_SendSerial (SYS.sfd, WORK, strlen(WORK));
						****/
						
						
						// (j < 0 ? -j : j)
						//if (v3Xd > 0)  v3Xd = ( v3Xd > 127 ? (v3Xd - 256) : v3Xd );
						//if (v3Yd > 0)  v3Yd = ( v3Yd > 127 ? (v3Yd - 256) : v3Yd );
						
						if (v3Xd > 127)  v3Xd = (v3Xd - 256);
						if (v3Yd > 127)  v3Yd = (v3Yd - 256);
						//
						
						/********/
						if ( (v3Xcnt > 127) || (v3Xcnt < -127) ) {
							v3Xcnt = 0;   v3Xerr++;
						}
						if ( (v3Ycnt > 127) || (v3Ycnt < -127) ) {
							v3Ycnt = 0;   v3Yerr++;
						}
						/********/
						
						/****
						uif_xy (14,3);
						sprintf (WORK," %02d,%02d", v3Xerr, v3Yerr);
						SB_SendSerial (SYS.sfd, WORK, strlen(WORK));
						****/
						
						
						v3Xd = abs(v3Xd);
						v3Yd = abs(v3Yd);
						
						
						v3Xt = v3Xt + v3Xd;    ////v3Xt + v3Xd;
						v3Yt = v3Yt + v3Yd;    ////v3Yt + v3Yd;
						//
						/****
						if ( v3Xt > 4998 )  v3Xt = 4998;
						if ( v3Xt < 0 )  v3Xt = 0;
						if ( v3Yt > 4998 )  v3Yt = 4998;
						if ( v3Yt < 0 )  v3Yt = 0;
						****/
						
						if ( v3Xt > 5999 )  v3Xt = 5999;   ////{ v3Xt = 0;  v3Yt = 0;}
						if ( v3Yt > 5999 )  v3Yt = 5999;   ////{ v3Xt = 0;  v3Yt = 0;}
						
						/****
						uif_xy (0,2);
						sprintf (WORK,"%3d,%3d : %d,%d    ", v3Xd, v3Yd, v3Xt, v3Yt);   //// v3Xd, v3Yd);
						SB_SendSerial (SYS.sfd, WORK, strlen(WORK));
						****/
						
						
						v3BUF[v3Cnt++] = WORK3[i+1];  //<---last_char!
						v3BUF[v3Cnt] = 0;  //..End
						
						
						
						/*****  ////////////////////(BUF--ºÎÁ¤ÇÕ-ÁßÃ¸-Error??)
						v3LastN = len - (i+2);
						if ( v3LastN > 0 ) {
							memcpy (v3Last, &WORK3[i+2], v3LastN ); 
							v3Last[v3LastN] = 0;
						}
						else v3LastN=0;
						*****/
						
						//--------------------------(ÀÜ¿©-ÁßÃ¸DATA::""°¡»ó-ÃßÁ¤ºÐ"")
						v3LastN = len - (i+2);
						if ( v3LastN > (i+2) ) {
							v3Xt = v3Xt + (v3Xd * (v3LastN / (i+2)) );
							v3Yt = v3Yt + (v3Yd * (v3LastN / (i+2)) );
						}
						//
						v3LastN=0;  //<==============!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						
						
						/******
						uif_xy (0,1);
						sprintf (WORK,"%s!   ",v3BUF);
						SB_SendSerial (SYS.sfd, WORK, strlen(WORK));
						//SB_SendSerial (SYS.sfd, v3BUF, v3Cnt);
						******/
						
						
						v3Mode = 0;
						///////////
						
						return 0; //===========>>>
						//
						////goto v3NEXT;
						
						
					default :
						
						break;
				}
				
				break;
				
			default :
				
				break;			
			}
			
		}
	
	
	/*****
v3NEXT:
		if ( v3LastN > 35 ) {
			memcpy (WORK3, v3Last, v3LastN ); 
			len = v3LastN;
			WORK3[len] = 0;
		}
		else {
			len = 0;
		}
		
	}
	*****/
	
	return 0;
	//===========>>>
	
	
	
	

	
	if (SB_DEBUG) SB_LogDataPrint ("S->L", WORK3, len); 
	
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
		
		SB_SendSerial (SYS.sfd, chD, 4);  ////fflush(0);  //SB_msleep (1);  //fflush(SYS.sfd);
}

void uif_xyPut (int x, int y, char ch)  // x:0~19 y:0~3 //
{
char chD[8];

		chD[0] = 0x1B;  //27;  //ESC
		chD[1] = 0x4C;  //"L";  //Locate
		chD[2] = x; //X
		chD[3] = y; //Y
		chD[4] = ch;
		
		SB_SendSerial (SYS.sfd, chD, 5);  ////fflush(0);  //SB_msleep (1);  //fflush(SYS.sfd);
}

void uif_xySTR (int x, int y, char *str)  // x:0~19 y:0~3 //
{
		uif_xy (x,y);
		SB_SendSerial (SYS.sfd, str, strlen(str));  ////fflush(0);
}


void MultiX_Port_output_ioctl (int OnOff)
{
int fd;
int value[3];
	
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;

	if ( OnOff )  	///value = 0x3fc00ff0;
	{
			value[0] = S4M_GPIO_MASK_PORT_A;
			value[1] = S4M_GPIO_MASK_PORT_B;
			value[2] = S4M_GPIO_MASK_PORT_C  | 0x00000003;
	}
	else 			///value = 0x00000000;
	{
			value[0] = 0x00000000;
			value[1] = 0x00000000;
			value[2] = 0x00000000;
	}
	
	//ioctl (fd, SETGPIOVAL_LB, &value);		// Port B,  value high!Low!
	//
	ioctl (fd, SETGPIOVAL_LM, &value[0]);
	
	usleep (1000);
	
	close (fd);
}

void BX_Port_output_ioctl (int OnOff)
{
int fd;
int value;
	
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;

	if ( OnOff )  	value = 0x3fc00ff0;
	else 			value = 0x00000000;
	//
	ioctl (fd, SETGPIOVAL_LB, &value);		// Port B,  value high!Low!
	
	close (fd);
}


void ConfigPP( int kp )  ////DVSCPU
{
	if ( SYS.wait_msec > ((SYS.keepalive + kp) / 2) )  ConfigWW( (SYS.keepalive + kp) / 2 );
	
	SYS.keepalive = SYS.keepalive + kp;   // ++ --
	CFG_SIO[0].keepalive = SYS.keepalive;

	SB_WriteConfig (CFGFILE_FLASH_SIO, (char *)&CFG_SIO[0],  sizeof(struct SB_SIO_CONFIG)*1);  ///SB_MAX_SIO_PORT);
	SB_WriteConfig (CFGFILE_ETC_SIO,   (char *)&CFG_SIO[0],  sizeof(struct SB_SIO_CONFIG)*1);  ///SB_MAX_SIO_PORT);
}

void ConfigWW( int kp )  ////DVSCPU
{
	if ( (SYS.wait_msec + kp) > (SYS.keepalive / 2) )   SYS.wait_msec = SYS.keepalive / 2;
	else												SYS.wait_msec = SYS.wait_msec + kp;  // ++ --
	CFG_SIO[0].packet_latency_time	= SYS.wait_msec;

	SB_WriteConfig (CFGFILE_FLASH_SIO, (char *)&CFG_SIO[0],  sizeof(struct SB_SIO_CONFIG)*1);  ///SB_MAX_SIO_PORT);
	SB_WriteConfig (CFGFILE_ETC_SIO,   (char *)&CFG_SIO[0],  sizeof(struct SB_SIO_CONFIG)*1);  ///SB_MAX_SIO_PORT);
}


void ConfigDELAY( int Vd )  ////DVSCPU
{	
	if ( (Vd < 0) || (Vd > 2) ) Vd = 0;
		
	////SYS.Vdelay =  ( CFG_SIO[2].remote_socket_no ) & 0x0003;  //<----0,1,2

	//CFG_SIO[2].remote_socket_no	= (CFG_SIO[2].remote_socket_no & 0xfffc) + Vd;  //<----0,1,2
	CFG_SIO[2].remote_socket_no	=  Vd;  //<----0,1,2

	SB_WriteConfig (CFGFILE_FLASH_SIO, (char *)&CFG_SIO[0],  sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);
	SB_WriteConfig (CFGFILE_ETC_SIO,   (char *)&CFG_SIO[0],  sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);
}



//////
//////

