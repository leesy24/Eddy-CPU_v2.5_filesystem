/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.column_defines.conf 7011 2002-05-08 05:42:47Z hardaker $
 */
#ifndef MIB-2_COLUMNS_H
#define MIB-2_COLUMNS_H

/* column number definitions for table sysORTable */
       #define COLUMN_SYSORINDEX		1
       #define COLUMN_SYSORID		2
       #define COLUMN_SYSORDESCR		3
       #define COLUMN_SYSORUPTIME		4

/* column number definitions for table ifTable */
       #define COLUMN_IFINDEX		1
       #define COLUMN_IFDESCR		2
       #define COLUMN_IFTYPE		3
       #define COLUMN_IFMTU		4
       #define COLUMN_IFSPEED		5
       #define COLUMN_IFPHYSADDRESS		6
       #define COLUMN_IFADMINSTATUS		7
       #define COLUMN_IFOPERSTATUS		8
       #define COLUMN_IFLASTCHANGE		9
       #define COLUMN_IFINOCTETS		10
       #define COLUMN_IFINUCASTPKTS		11
       #define COLUMN_IFINNUCASTPKTS		12
       #define COLUMN_IFINDISCARDS		13
       #define COLUMN_IFINERRORS		14
       #define COLUMN_IFINUNKNOWNPROTOS		15
       #define COLUMN_IFOUTOCTETS		16
       #define COLUMN_IFOUTUCASTPKTS		17
       #define COLUMN_IFOUTNUCASTPKTS		18
       #define COLUMN_IFOUTDISCARDS		19
       #define COLUMN_IFOUTERRORS		20
       #define COLUMN_IFOUTQLEN		21
       #define COLUMN_IFSPECIFIC		22

/* column number definitions for table atTable */
       #define COLUMN_ATIFINDEX		1
       #define COLUMN_ATPHYSADDRESS		2
       #define COLUMN_ATNETADDRESS		3

/* column number definitions for table ipAddrTable */
       #define COLUMN_IPADENTADDR		1
       #define COLUMN_IPADENTIFINDEX		2
       #define COLUMN_IPADENTNETMASK		3
       #define COLUMN_IPADENTBCASTADDR		4
       #define COLUMN_IPADENTREASMMAXSIZE		5

/* column number definitions for table ipRouteTable */
       #define COLUMN_IPROUTEDEST		1
       #define COLUMN_IPROUTEIFINDEX		2
       #define COLUMN_IPROUTEMETRIC1		3
       #define COLUMN_IPROUTEMETRIC2		4
       #define COLUMN_IPROUTEMETRIC3		5
       #define COLUMN_IPROUTEMETRIC4		6
       #define COLUMN_IPROUTENEXTHOP		7
       #define COLUMN_IPROUTETYPE		8
       #define COLUMN_IPROUTEPROTO		9
       #define COLUMN_IPROUTEAGE		10
       #define COLUMN_IPROUTEMASK		11
       #define COLUMN_IPROUTEMETRIC5		12
       #define COLUMN_IPROUTEINFO		13

/* column number definitions for table ipNetToMediaTable */
       #define COLUMN_IPNETTOMEDIAIFINDEX		1
       #define COLUMN_IPNETTOMEDIAPHYSADDRESS		2
       #define COLUMN_IPNETTOMEDIANETADDRESS		3
       #define COLUMN_IPNETTOMEDIATYPE		4

/* column number definitions for table tcpConnTable */
       #define COLUMN_TCPCONNSTATE		1
       #define COLUMN_TCPCONNLOCALADDRESS		2
       #define COLUMN_TCPCONNLOCALPORT		3
       #define COLUMN_TCPCONNREMADDRESS		4
       #define COLUMN_TCPCONNREMPORT		5

/* column number definitions for table udpTable */
       #define COLUMN_UDPLOCALADDRESS		1
       #define COLUMN_UDPLOCALPORT		2

/* column number definitions for table egpNeighTable */
       #define COLUMN_EGPNEIGHSTATE		1
       #define COLUMN_EGPNEIGHADDR		2
       #define COLUMN_EGPNEIGHAS		3
       #define COLUMN_EGPNEIGHINMSGS		4
       #define COLUMN_EGPNEIGHINERRS		5
       #define COLUMN_EGPNEIGHOUTMSGS		6
       #define COLUMN_EGPNEIGHOUTERRS		7
       #define COLUMN_EGPNEIGHINERRMSGS		8
       #define COLUMN_EGPNEIGHOUTERRMSGS		9
       #define COLUMN_EGPNEIGHSTATEUPS		10
       #define COLUMN_EGPNEIGHSTATEDOWNS		11
       #define COLUMN_EGPNEIGHINTERVALHELLO		12
       #define COLUMN_EGPNEIGHINTERVALPOLL		13
       #define COLUMN_EGPNEIGHMODE		14
       #define COLUMN_EGPNEIGHEVENTTRIGGER		15

/* column number definitions for table rs232PortTable */
       #define COLUMN_RS232PORTINDEX		1
       #define COLUMN_RS232PORTTYPE		2
       #define COLUMN_RS232PORTINSIGNUMBER		3
       #define COLUMN_RS232PORTOUTSIGNUMBER		4
       #define COLUMN_RS232PORTINSPEED		5
       #define COLUMN_RS232PORTOUTSPEED		6
       #define COLUMN_RS232PORTINFLOWTYPE		7
       #define COLUMN_RS232PORTOUTFLOWTYPE		8

/* column number definitions for table rs232AsyncPortTable */
       #define COLUMN_RS232ASYNCPORTINDEX		1
       #define COLUMN_RS232ASYNCPORTBITS		2
       #define COLUMN_RS232ASYNCPORTSTOPBITS		3
       #define COLUMN_RS232ASYNCPORTPARITY		4
       #define COLUMN_RS232ASYNCPORTAUTOBAUD		5
       #define COLUMN_RS232ASYNCPORTPARITYERRS		6
       #define COLUMN_RS232ASYNCPORTFRAMINGERRS		7
       #define COLUMN_RS232ASYNCPORTOVERRUNERRS		8

/* column number definitions for table rs232SyncPortTable */
       #define COLUMN_RS232SYNCPORTINDEX		1
       #define COLUMN_RS232SYNCPORTCLOCKSOURCE		2
       #define COLUMN_RS232SYNCPORTFRAMECHECKERRS		3
       #define COLUMN_RS232SYNCPORTTRANSMITUNDERRUNERRS		4
       #define COLUMN_RS232SYNCPORTRECEIVEOVERRUNERRS		5
       #define COLUMN_RS232SYNCPORTINTERRUPTEDFRAMES		6
       #define COLUMN_RS232SYNCPORTABORTEDFRAMES		7
       #define COLUMN_RS232SYNCPORTROLE		8
       #define COLUMN_RS232SYNCPORTENCODING		9
       #define COLUMN_RS232SYNCPORTRTSCONTROL		10
       #define COLUMN_RS232SYNCPORTRTSCTSDELAY		11
       #define COLUMN_RS232SYNCPORTMODE		12
       #define COLUMN_RS232SYNCPORTIDLEPATTERN		13
       #define COLUMN_RS232SYNCPORTMINFLAGS		14

/* column number definitions for table rs232InSigTable */
       #define COLUMN_RS232INSIGPORTINDEX		1
       #define COLUMN_RS232INSIGNAME		2
       #define COLUMN_RS232INSIGSTATE		3
       #define COLUMN_RS232INSIGCHANGES		4

/* column number definitions for table rs232OutSigTable */
       #define COLUMN_RS232OUTSIGPORTINDEX		1
       #define COLUMN_RS232OUTSIGNAME		2
       #define COLUMN_RS232OUTSIGSTATE		3
       #define COLUMN_RS232OUTSIGCHANGES		4

/* column number definitions for table ifXTable */
       #define COLUMN_IFNAME		1
       #define COLUMN_IFINMULTICASTPKTS		2
       #define COLUMN_IFINBROADCASTPKTS		3
       #define COLUMN_IFOUTMULTICASTPKTS		4
       #define COLUMN_IFOUTBROADCASTPKTS		5
       #define COLUMN_IFHCINOCTETS		6
       #define COLUMN_IFHCINUCASTPKTS		7
       #define COLUMN_IFHCINMULTICASTPKTS		8
       #define COLUMN_IFHCINBROADCASTPKTS		9
       #define COLUMN_IFHCOUTOCTETS		10
       #define COLUMN_IFHCOUTUCASTPKTS		11
       #define COLUMN_IFHCOUTMULTICASTPKTS		12
       #define COLUMN_IFHCOUTBROADCASTPKTS		13
       #define COLUMN_IFLINKUPDOWNTRAPENABLE		14
       #define COLUMN_IFHIGHSPEED		15
       #define COLUMN_IFPROMISCUOUSMODE		16
       #define COLUMN_IFCONNECTORPRESENT		17
       #define COLUMN_IFALIAS		18
       #define COLUMN_IFCOUNTERDISCONTINUITYTIME		19

/* column number definitions for table ifStackTable */
       #define COLUMN_IFSTACKHIGHERLAYER		1
       #define COLUMN_IFSTACKLOWERLAYER		2
       #define COLUMN_IFSTACKSTATUS		3

/* column number definitions for table ifTestTable */
       #define COLUMN_IFTESTID		1
       #define COLUMN_IFTESTSTATUS		2
       #define COLUMN_IFTESTTYPE		3
       #define COLUMN_IFTESTRESULT		4
       #define COLUMN_IFTESTCODE		5
       #define COLUMN_IFTESTOWNER		6

/* column number definitions for table ifRcvAddressTable */
       #define COLUMN_IFRCVADDRESSADDRESS		1
       #define COLUMN_IFRCVADDRESSSTATUS		2
       #define COLUMN_IFRCVADDRESSTYPE		3
#endif /* MIB-2_COLUMNS_H */
