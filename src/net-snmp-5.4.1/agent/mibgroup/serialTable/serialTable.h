/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 14170 $ of $
 *
 * $Id:$
 */
#ifndef SERIALTABLE_H
#define SERIALTABLE_H

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup misc misc: Miscellaneous routines
 *
 * @{
 */
#include <net-snmp/library/asn1.h>

/* other required module components */
    /* *INDENT-OFF*  */
config_add_mib(SYSBAS-MIB)
config_require(SYSBAS-MIB/serialTable/serialTable_interface)
config_require(SYSBAS-MIB/serialTable/serialTable_data_access)
config_require(SYSBAS-MIB/serialTable/serialTable_data_get)
config_require(SYSBAS-MIB/serialTable/serialTable_data_set)
    /* *INDENT-ON*  */

/* OID and column number definitions for serialTable */
#include "serialTable_oids.h"

/* enum definions */
#include "serialTable_enums.h"

/* *********************************************************************
 * function declarations
 */
void init_serialTable(void);
void shutdown_serialTable(void);

/* *********************************************************************
 * Table declarations
 */
/**********************************************************************
 **********************************************************************
 ***
 *** Table serialTable
 ***
 **********************************************************************
 **********************************************************************/
/*
 * SYSBAS-MIB::serialTable is subid 3 of sysbas.
 * Its status is Current.
 * OID: .1.3.6.1.4.1.1.3, length: 8
*/
/* *********************************************************************
 * When you register your mib, you get to provide a generic
 * pointer that will be passed back to you for most of the
 * functions calls.
 *
 * TODO:100:r: Review all context structures
 */
    /*
     * TODO:101:o: |-> Review serialTable registration context.
     */
typedef netsnmp_data_list serialTable_registration;

/**********************************************************************/
/*
 * TODO:110:r: |-> Review serialTable data context structure.
 * This structure is used to represent the data for serialTable.
 */
/*
 * This structure contains storage for all the columns defined in the
 * serialTable.
 */
typedef struct serialTable_data_s {
    
        /*
         * portName(2)/OCTETSTR/ASN_OCTET_STR/char(char)//L/A/W/e/r/d/h
         */
   char   portName[65535];
size_t      portName_len; /* # of char elements, not bytes */
    
        /*
         * socketNumber(3)/INTEGER32/ASN_INTEGER/long(long)//l/A/w/e/r/d/h
         */
   long   socketNumber;
    
        /*
         * protocol(4)/INTEGER/ASN_INTEGER/long(u_long)//l/A/w/E/r/d/h
         */
   u_long   protocol;
    
        /*
         * deviceType(5)/INTEGER/ASN_INTEGER/long(u_long)//l/A/w/E/r/d/h
         */
   u_long   deviceType;
    
        /*
         * remoteIPAddress(6)/IPADDR/ASN_IPADDRESS/u_long(u_long)//l/A/w/e/r/d/h
         */
   u_long   remoteIPAddress;
    
        /*
         * remoteSocketNumber(7)/INTEGER32/ASN_INTEGER/long(long)//l/A/w/e/r/d/h
         */
   long   remoteSocketNumber;
    
        /*
         * bypassEnable(8)/TruthValue/ASN_INTEGER/long(u_long)//l/A/w/E/r/d/h
         */
   u_long   bypassEnable;
    
        /*
         * loginEnable(9)/TruthValue/ASN_INTEGER/long(u_long)//l/A/w/E/r/d/h
         */
   u_long   loginEnable;
    
        /*
         * loginUsername(10)/OCTETSTR/ASN_OCTET_STR/char(char)//L/A/w/e/r/d/h
         */
   char   loginUsername[65535];
size_t      loginUsername_len; /* # of char elements, not bytes */
    
        /*
         * loginPassword(11)/OCTETSTR/ASN_OCTET_STR/char(char)//L/A/w/e/r/d/h
         */
   char   loginPassword[65535];
size_t      loginPassword_len; /* # of char elements, not bytes */
    
        /*
         * connectionCount(12)/COUNTER/ASN_COUNTER/u_long(u_long)//l/A/w/e/r/d/h
         */
   u_long   connectionCount;
    
        /*
         * connectionStatus(13)/INTEGER/ASN_INTEGER/long(u_long)//l/A/w/E/r/d/h
         */
   u_long   connectionStatus;
    
        /*
         * serialRowStatus(14)/RowStatus/ASN_INTEGER/long(u_long)//l/A/W/E/r/d/h
         */
   u_long   serialRowStatus;
    
} serialTable_data;


/* *********************************************************************
 * TODO:115:o: |-> Review serialTable undo context.
 * We're just going to use the same data structure for our
 * undo_context. If you want to do something more efficent,
 * define your typedef here.
 */
typedef serialTable_data serialTable_undo_data;

/*
 * TODO:120:r: |-> Review serialTable mib index.
 * This structure is used to represent the index for serialTable.
 */
typedef struct serialTable_mib_index_s {

        /*
         * portIndex(1)/INTEGER32/ASN_INTEGER/long(long)//l/a/w/e/r/d/h
         */
   long   portIndex;


} serialTable_mib_index;

    /*
     * TODO:121:r: |   |-> Review serialTable max index length.
     * If you KNOW that your indexes will never exceed a certain
     * length, update this macro to that length.
*/
#define MAX_serialTable_IDX_LEN     1


/* *********************************************************************
 * TODO:130:o: |-> Review serialTable Row request (rowreq) context.
 * When your functions are called, you will be passed a
 * serialTable_rowreq_ctx pointer.
 */
typedef struct serialTable_rowreq_ctx_s {

    /** this must be first for container compare to work */
    netsnmp_index        oid_idx;
    oid                  oid_tmp[MAX_serialTable_IDX_LEN];
    
    serialTable_mib_index        tbl_idx;
    
    serialTable_data              data;
    serialTable_undo_data       * undo;
    unsigned int                column_set_flags; /* flags for set columns */


    /*
     * flags per row. Currently, the first (lower) 8 bits are reserved
     * for the user. See mfd.h for other flags.
     */
    u_int                       rowreq_flags;

    /*
     * TODO:131:o: |   |-> Add useful data to serialTable rowreq context.
     */
    
    /*
     * storage for future expansion
     */
    netsnmp_data_list             *serialTable_data_list;

} serialTable_rowreq_ctx;

typedef struct serialTable_ref_rowreq_ctx_s {
    serialTable_rowreq_ctx *rowreq_ctx;
} serialTable_ref_rowreq_ctx;

/* *********************************************************************
 * function prototypes
 */
    int serialTable_pre_request(serialTable_registration * user_context);
    int serialTable_post_request(serialTable_registration * user_context,
        int rc);

    int serialTable_rowreq_ctx_init(serialTable_rowreq_ctx *rowreq_ctx,
                                   void *user_init_ctx);
    void serialTable_rowreq_ctx_cleanup(serialTable_rowreq_ctx *rowreq_ctx);

    int serialTable_commit(serialTable_rowreq_ctx * rowreq_ctx);

    serialTable_rowreq_ctx *
                  serialTable_row_find_by_mib_index(serialTable_mib_index *mib_idx);

extern oid serialTable_oid[];
extern int serialTable_oid_size;


#include "serialTable_interface.h"
#include "serialTable_data_access.h"
#include "serialTable_data_get.h"
#include "serialTable_data_set.h"

/*
 * DUMMY markers, ignore
 *
 * TODO:099:x: *************************************************************
 * TODO:199:x: *************************************************************
 * TODO:299:x: *************************************************************
 * TODO:399:x: *************************************************************
 * TODO:499:x: *************************************************************
 */

#ifdef __cplusplus
}
#endif

#endif /* SERIALTABLE_H */
/** @} */
