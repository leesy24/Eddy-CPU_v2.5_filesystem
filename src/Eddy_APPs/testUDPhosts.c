

//#include <stdio.h>

#include "include/sb_include.h"
#include "include/sb_define.h"
#include "include/sb_shared.h"
#include "include/sb_config.h"
#include "include/sb_extern.h"
#include "include/sb_ioctl.h"
#include "include/sb_misc.h"
#include <time.h>
#include <linux/rtc.h>
#include <sys/mount.h>


int main()
{
FILE *fd;
char cmd[130];
int i,n;
char tmp[20][110];

int udp_hosts = 0;
char udp_host_ip [20][50];
int udp_host_port [20];	



	while(1)
	{
		//printf("UDP-Host-List!\n");
		//sleep(1);
		
		
		sprintf(cmd, "/bin/cp /flash/UDPhosts.conf /etc/UDPhosts.conf ");
		system(cmd);
		sprintf(cmd, "/usr/bin/dos2unix /etc/UDPhosts.conf ");
		system(cmd);
		sprintf(cmd, "/bin/chmod 777 /etc/UDPhosts.conf ");
		system(cmd);
		sprintf(cmd, "/bin/cp /etc/UDPhosts.conf /tmp/UDPhosts.conf");
		system(cmd);
		//sprintf(cmd, "/etc/GDM_start.sh ");
		//system(cmd);


		if ((fd = fopen("/etc/UDPhosts.conf", "r")) == NULL)
		{
			printf("UDP-Host-List~ FILE-NOT-FOUND!\n");
			return;
		}
		fclose(fd);

		sprintf(cmd, "/bin/grep UDPhosts /etc/UDPhosts.conf | cut -f2 -d' ' > /tmp/udpHosts "); 
		system(cmd);
		
		if ((fd = fopen("/tmp/udpHosts", "r")) == NULL) return;
		i=0;
		while (1) 
		{
			if (fgets(tmp[i], 30, fd) == NULL)	break;
			
			udp_hosts = atoi(tmp[i]);
			
			printf("UDP-Host-List: N==%d \n", udp_hosts);
			
			i++;	
		}
		fclose(fd);
		
		sprintf(cmd,"/bin/rm -f /tmp/UDP-List.txt");
		system(cmd);
		sprintf(cmd,"/bin/touch /tmp/UDP-List.txt");
		system(cmd);		
		
		for(i=1;i<(udp_hosts+1);i++)
		{
		sprintf(cmd, "/bin/grep UDPhost%d /etc/UDPhosts.conf | cut -f2 -d' ' > /tmp/udpHostIP%d ", i, i);
		system(cmd);
		//
			sprintf(cmd,"/tmp/udpHostIP%d",i);
			if ((fd = fopen(cmd, "r")) != NULL) {	fgets(udp_host_ip[i], 30, fd);  fclose(fd);  }
			//
			n = strlen(udp_host_ip[i]);  if(udp_host_ip[i][n-1]=='\n') udp_host_ip[i][n-1]=0;
		
		sprintf(cmd, "/bin/grep UDPhost%d /etc/UDPhosts.conf | cut -f3 -d' ' > /tmp/udpHostPORT%d ", i, i);
		system(cmd);
		//
			sprintf(cmd,"/tmp/udpHostPORT%d",i);
			if ((fd = fopen(cmd, "r")) != NULL) {	fgets(cmd, 10, fd);  udp_host_port[i]=atoi(cmd);  fclose(fd);  }
			
			
		printf("UDP-Host-List: N==%d : [%d] : %s - %d \n", udp_hosts, i, udp_host_ip[i], udp_host_port[i]);
		
		sprintf(cmd,"echo \"[%d] %s:%d\" >> /tmp/UDP-List.txt", i, udp_host_ip[i], udp_host_port[i]);
		system(cmd);
		}
		
		break;
		
	}
}

