/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.column_enums.conf 12909 2005-09-29 22:16:22Z hardaker $
 */
#ifndef RS232_ENUMS_H
#define RS232_ENUMS_H

/* enums for column rs232PortType */
           #define RS232PORTTYPE_RS232		1
           #define RS232PORTTYPE_RS422		2
           #define RS232PORTTYPE_RS423		3
/*           #define RS232PORTTYPE_OTHER		1
           #define RS232PORTTYPE_RS232		2
           #define RS232PORTTYPE_RS422		3
           #define RS232PORTTYPE_RS423		4
           #define RS232PORTTYPE_V35		5
           #define RS232PORTTYPE_X21		6
*/
/* enums for column rs232PortInFlowType */
           #define RS232PORTINFLOWTYPE_NONE		0
           #define RS232PORTINFLOWTYPE_CTSRTS		1
           #define RS232PORTINFLOWTYPE_DSRDTR		2

/* enums for column rs232PortOutFlowType */
           #define RS232PORTOUTFLOWTYPE_NONE		0
           #define RS232PORTOUTFLOWTYPE_CTSRTS		1
           #define RS232PORTOUTFLOWTYPE_DSRDTR		2

/* enums for column rs232AsyncPortStopBits */
           #define RS232ASYNCPORTSTOPBITS_ONE		1
           #define RS232ASYNCPORTSTOPBITS_TWO		2
           #define RS232ASYNCPORTSTOPBITS_ONEANDHALF		3
           #define RS232ASYNCPORTSTOPBITS_DYNAMIC		4

/* enums for column rs232AsyncPortParity */
           #define RS232ASYNCPORTPARITY_NONE		1
           #define RS232ASYNCPORTPARITY_ODD		2
           #define RS232ASYNCPORTPARITY_EVEN		3
           #define RS232ASYNCPORTPARITY_MARK		4
           #define RS232ASYNCPORTPARITY_SPACE		5

/* enums for column rs232AsyncPortAutobaud */
           #define RS232ASYNCPORTAUTOBAUD_ENABLED		1
           #define RS232ASYNCPORTAUTOBAUD_DISABLED		2

/* enums for column rs232SyncPortClockSource */
           #define RS232SYNCPORTCLOCKSOURCE_INTERNAL		1
           #define RS232SYNCPORTCLOCKSOURCE_EXTERNAL		2
           #define RS232SYNCPORTCLOCKSOURCE_SPLIT		3

/* enums for column rs232SyncPortRole */
           #define RS232SYNCPORTROLE_DTE		1
           #define RS232SYNCPORTROLE_DCE		2

/* enums for column rs232SyncPortEncoding */
           #define RS232SYNCPORTENCODING_NRZ		1
           #define RS232SYNCPORTENCODING_NRZI		2

/* enums for column rs232SyncPortRTSControl */
           #define RS232SYNCPORTRTSCONTROL_CONTROLLED		1
           #define RS232SYNCPORTRTSCONTROL_CONSTANT		2

/* enums for column rs232SyncPortMode */
           #define RS232SYNCPORTMODE_FDX		1
           #define RS232SYNCPORTMODE_HDX		2
           #define RS232SYNCPORTMODE_SIMPLEX_RECEIVE		3
           #define RS232SYNCPORTMODE_SIMPLEX_SEND		4

/* enums for column rs232SyncPortIdlePattern */
           #define RS232SYNCPORTIDLEPATTERN_MARK		1
           #define RS232SYNCPORTIDLEPATTERN_SPACE		2

/* enums for column rs232InSigName */
           #define RS232INSIGNAME_RTS		1
           #define RS232INSIGNAME_CTS		2
           #define RS232INSIGNAME_DSR		3
           #define RS232INSIGNAME_DTR		4
           #define RS232INSIGNAME_RI		5
           #define RS232INSIGNAME_DCD		6
           #define RS232INSIGNAME_SQ		7
           #define RS232INSIGNAME_SRS		8
           #define RS232INSIGNAME_SRTS		9
           #define RS232INSIGNAME_SCTS		10
           #define RS232INSIGNAME_SDCD		11

/* enums for column rs232InSigState */
           #define RS232INSIGSTATE_NONE		1
           #define RS232INSIGSTATE_ON		2
           #define RS232INSIGSTATE_OFF		3

/* enums for column rs232OutSigName */
           #define RS232OUTSIGNAME_RTS		1
           #define RS232OUTSIGNAME_CTS		2
           #define RS232OUTSIGNAME_DSR		3
           #define RS232OUTSIGNAME_DTR		4
           #define RS232OUTSIGNAME_RI		5
           #define RS232OUTSIGNAME_DCD		6
           #define RS232OUTSIGNAME_SQ		7
           #define RS232OUTSIGNAME_SRS		8
           #define RS232OUTSIGNAME_SRTS		9
           #define RS232OUTSIGNAME_SCTS		10
           #define RS232OUTSIGNAME_SDCD		11

/* enums for column rs232OutSigState */
           #define RS232OUTSIGSTATE_NONE		1
           #define RS232OUTSIGSTATE_ON		2
           #define RS232OUTSIGSTATE_OFF		3


#endif /* RS232_ENUMS_H */
