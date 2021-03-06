/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 14170 $ of $ 
 *
 * $Id:$
 */
/* standard Net-SNMP includes */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/* include our parent header */
#include "serialTable.h"


#include "serialTable_data_access.h"

/** @ingroup interface
 * @addtogroup data_access data_access: Routines to access data
 *
 * These routines are used to locate the data used to satisfy
 * requests.
 * 
 * @{
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

/**
 * initialization for serialTable data access
 *
 * This function is called during startup to allow you to
 * allocate any resources you need for the data table.
 *
 * @param serialTable_reg
 *        Pointer to serialTable_registration
 *
 * @retval MFD_SUCCESS : success.
 * @retval MFD_ERROR   : unrecoverable error.
 */
int
serialTable_init_data(serialTable_registration * serialTable_reg)
{
    DEBUGMSGTL(("verbose:serialTable:serialTable_init_data","called\n"));

    /*
     * TODO:303:o: Initialize serialTable data.
     */
    /*
    ***************************************************
    ***             START EXAMPLE CODE              ***
    ***---------------------------------------------***/
    /*
     * if you are the sole writer for the file, you could
     * open it here. However, as stated earlier, we are assuming
     * the worst case, which in this case means that the file is
     * written to by someone else, and might not even exist when
     * we start up. So we can't do anything here.
     */
    /*
    ***---------------------------------------------***
    ***              END  EXAMPLE CODE              ***
    ***************************************************/

    return MFD_SUCCESS;
} /* serialTable_init_data */

/**
 * container overview
 *
 */

/**
 * container initialization
 *
 * @param container_ptr_ptr A pointer to a container pointer. If you
 *        create a custom container, use this parameter to return it
 *        to the MFD helper. If set to NULL, the MFD helper will
 *        allocate a container for you.
 *
 *  This function is called at startup to allow you to customize certain
 *  aspects of the access method. For the most part, it is for advanced
 *  users. The default code should suffice for most cases. If no custom
 *  container is allocated, the MFD code will create one for your.
 *
 * @remark
 *  This would also be a good place to do any initialization needed
 *  for you data source. For example, opening a connection to another
 *  process that will supply the data, opening a database, etc.
 */
void
serialTable_container_init(netsnmp_container **container_ptr_ptr)
{
    DEBUGMSGTL(("verbose:serialTable:serialTable_container_init","called\n"));
    
    if (NULL == container_ptr_ptr) {
        snmp_log(LOG_ERR,"bad container param to serialTable_container_init\n");
        return;
    }

    /*
     * For advanced users, you can use a custom container. If you
     * do not create one, one will be created for you.
     */
    *container_ptr_ptr = NULL;

} /* serialTable_container_init */

/**
 * container shutdown
 *
 * @param container_ptr A pointer to the container.
 *
 *  This function is called at shutdown to allow you to customize certain
 *  aspects of the access method. For the most part, it is for advanced
 *  users. The default code should suffice for most cases.
 *
 *  This function is called before serialTable_container_free().
 *
 * @remark
 *  This would also be a good place to do any cleanup needed
 *  for you data source. For example, closing a connection to another
 *  process that supplied the data, closing a database, etc.
 */
void
serialTable_container_shutdown(netsnmp_container *container_ptr)
{
    DEBUGMSGTL(("verbose:serialTable:serialTable_container_shutdown","called\n"));
    
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,"bad params to serialTable_container_shutdown\n");
        return;
    }

} /* serialTable_container_shutdown */

/**
 * load initial data
 *
 * TODO:350:M: Implement serialTable data load
 *
 * @param container container to which items should be inserted
 *
 * @retval MFD_SUCCESS              : success.
 * @retval MFD_RESOURCE_UNAVAILABLE : Can't access data source
 * @retval MFD_ERROR                : other error.
 *
 *  This function is called to load the index(es) (and data, optionally)
 *  for the every row in the data set.
 *
 * @remark
 *  While loading the data, the only important thing is the indexes.
 *  If access to your data is cheap/fast (e.g. you have a pointer to a
 *  structure in memory), it would make sense to update the data here.
 *  If, however, the accessing the data invovles more work (e.g. parsing
 *  some other existing data, or peforming calculations to derive the data),
 *  then you can limit yourself to setting the indexes and saving any
 *  information you will need later. Then use the saved information in
 *  serialTable_row_prep() for populating data.
 *
 * @note
 *  If you need consistency between rows (like you want statistics
 *  for each row to be from the same time frame), you should set all
 *  data here.
 *
 */
int
serialTable_container_load(netsnmp_container *container)
{
    serialTable_rowreq_ctx *rowreq_ctx;
    size_t                 count = 0;

    /*
     * temporary storage for index values
     */
        /*
         * portIndex(1)/INTEGER32/ASN_INTEGER/long(long)//l/a/w/e/r/d/h
         */
   long   portIndex;

    
    /*
     * this example code is based on a data source that is a
     * text file to be read and parsed.
     */
    FILE *filep;
    char line[MAX_LINE_SIZE];

    DEBUGMSGTL(("verbose:serialTable:serialTable_container_load","called\n"));

    /*
    ***************************************************
    ***             START EXAMPLE CODE              ***
    ***---------------------------------------------***/
    /*
     * open our data file.
     */
    filep = fopen("/etc/dummy.conf", "r");
    if(NULL ==  filep) {
        return MFD_RESOURCE_UNAVAILABLE;
    }

    /*
    ***---------------------------------------------***
    ***              END  EXAMPLE CODE              ***
    ***************************************************/
    /*
     * TODO:351:M: |-> Load/update data in the serialTable container.
     * loop over your serialTable data, allocate a rowreq context,
     * set the index(es) [and data, optionally] and insert into
     * the container.
     */
    while( 1 ) {
    /*
    ***************************************************
    ***             START EXAMPLE CODE              ***
    ***---------------------------------------------***/
    /*
     * get a line (skip blank lines)
     */
    do {
        if (!fgets(line, sizeof(line), filep)) {
            /* we're done */
            fclose(filep);
            filep = NULL;
        }
    } while (filep && (line[0] == '\n'));

    /*
     * check for end of data
     */
    if(NULL == filep)
        break;

    /*
     * parse line into variables
     */
    /*
    ***---------------------------------------------***
    ***              END  EXAMPLE CODE              ***
    ***************************************************/

        /*
         * TODO:352:M: |   |-> set indexes in new serialTable rowreq context.
         * data context will be set from the param (unless NULL,
         *      in which case a new data context will be allocated)
         */
        rowreq_ctx = serialTable_allocate_rowreq_ctx(NULL);
        if (NULL == rowreq_ctx) {
            snmp_log(LOG_ERR, "memory allocation failed\n");
            return MFD_RESOURCE_UNAVAILABLE;
        }
        if(MFD_SUCCESS != serialTable_indexes_set(rowreq_ctx
                               , portIndex
               )) {
            snmp_log(LOG_ERR,"error setting index while loading "
                     "serialTable data.\n");
            serialTable_release_rowreq_ctx(rowreq_ctx);
            continue;
        }

        /*
         * TODO:352:r: |   |-> populate serialTable data context.
         * Populate data context here. (optionally, delay until row prep)
         */
    /*
     * TRANSIENT or semi-TRANSIENT data:
     * copy data or save any info needed to do it in row_prep.
     */
    /*
     * setup/save data for portName
     * portName(2)/OCTETSTR/ASN_OCTET_STR/char(char)//L/A/W/e/r/d/h
     */
    /** no mapping */
    /*
     * make sure there is enough space for portName data
     */
    if ((NULL == rowreq_ctx->data.portName) ||
        (rowreq_ctx->data.portName_len <
         (portName_len* sizeof(portName[0])))) {
        snmp_log(LOG_ERR,"not enough space for value\n");
        return MFD_ERROR;
    }
    rowreq_ctx->data.portName_len = portName_len* sizeof(portName[0]);
    memcpy( rowreq_ctx->data.portName, portName, portName_len* sizeof(portName[0]) );
    
    /*
     * setup/save data for socketNumber
     * socketNumber(3)/INTEGER32/ASN_INTEGER/long(long)//l/A/w/e/r/d/h
     */
    /** no mapping */
    rowreq_ctx->data.socketNumber = socketNumber;
    
    /*
     * setup/save data for protocol
     * protocol(4)/INTEGER/ASN_INTEGER/long(u_long)//l/A/w/E/r/d/h
     */
    /** no mapping */
    rowreq_ctx->data.protocol = protocol;
    
    /*
     * setup/save data for deviceType
     * deviceType(5)/INTEGER/ASN_INTEGER/long(u_long)//l/A/w/E/r/d/h
     */
    /** no mapping */
    rowreq_ctx->data.deviceType = deviceType;
    
    /*
     * setup/save data for remoteIPAddress
     * remoteIPAddress(6)/IPADDR/ASN_IPADDRESS/u_long(u_long)//l/A/w/e/r/d/h
     */
    /** no mapping */
    rowreq_ctx->data.remoteIPAddress = remoteIPAddress;
    
    /*
     * setup/save data for remoteSocketNumber
     * remoteSocketNumber(7)/INTEGER32/ASN_INTEGER/long(long)//l/A/w/e/r/d/h
     */
    /** no mapping */
    rowreq_ctx->data.remoteSocketNumber = remoteSocketNumber;
    
    /*
     * setup/save data for bypassEnable
     * bypassEnable(8)/TruthValue/ASN_INTEGER/long(u_long)//l/A/w/E/r/d/h
     */
    /** no mapping */
    rowreq_ctx->data.bypassEnable = bypassEnable;
    
    /*
     * setup/save data for loginEnable
     * loginEnable(9)/TruthValue/ASN_INTEGER/long(u_long)//l/A/w/E/r/d/h
     */
    /** no mapping */
    rowreq_ctx->data.loginEnable = loginEnable;
    
    /*
     * setup/save data for loginUsername
     * loginUsername(10)/OCTETSTR/ASN_OCTET_STR/char(char)//L/A/w/e/r/d/h
     */
    /** no mapping */
    /*
     * make sure there is enough space for loginUsername data
     */
    if ((NULL == rowreq_ctx->data.loginUsername) ||
        (rowreq_ctx->data.loginUsername_len <
         (loginUsername_len* sizeof(loginUsername[0])))) {
        snmp_log(LOG_ERR,"not enough space for value\n");
        return MFD_ERROR;
    }
    rowreq_ctx->data.loginUsername_len = loginUsername_len* sizeof(loginUsername[0]);
    memcpy( rowreq_ctx->data.loginUsername, loginUsername, loginUsername_len* sizeof(loginUsername[0]) );
    
    /*
     * setup/save data for loginPassword
     * loginPassword(11)/OCTETSTR/ASN_OCTET_STR/char(char)//L/A/w/e/r/d/h
     */
    /** no mapping */
    /*
     * make sure there is enough space for loginPassword data
     */
    if ((NULL == rowreq_ctx->data.loginPassword) ||
        (rowreq_ctx->data.loginPassword_len <
         (loginPassword_len* sizeof(loginPassword[0])))) {
        snmp_log(LOG_ERR,"not enough space for value\n");
        return MFD_ERROR;
    }
    rowreq_ctx->data.loginPassword_len = loginPassword_len* sizeof(loginPassword[0]);
    memcpy( rowreq_ctx->data.loginPassword, loginPassword, loginPassword_len* sizeof(loginPassword[0]) );
    
    /*
     * setup/save data for connectionCount
     * connectionCount(12)/COUNTER/ASN_COUNTER/u_long(u_long)//l/A/w/e/r/d/h
     */
    /** no mapping */
    rowreq_ctx->data.connectionCount = connectionCount;
    
    /*
     * setup/save data for connectionStatus
     * connectionStatus(13)/INTEGER/ASN_INTEGER/long(u_long)//l/A/w/E/r/d/h
     */
    /** no mapping */
    rowreq_ctx->data.connectionStatus = connectionStatus;
    
    /*
     * setup/save data for serialRowStatus
     * serialRowStatus(14)/RowStatus/ASN_INTEGER/long(u_long)//l/A/W/E/r/d/h
     */
    /** no mapping */
    rowreq_ctx->data.serialRowStatus = serialRowStatus;
    
        
        /*
         * insert into table container
         */
        CONTAINER_INSERT(container, rowreq_ctx);
        ++count;
    }

    /*
    ***************************************************
    ***             START EXAMPLE CODE              ***
    ***---------------------------------------------***/
    if(NULL != filep)
        fclose(filep);
    /*
    ***---------------------------------------------***
    ***              END  EXAMPLE CODE              ***
    ***************************************************/

    DEBUGMSGT(("verbose:serialTable:serialTable_container_load",
               "inserted %d records\n", count));

    return MFD_SUCCESS;
} /* serialTable_container_load */

/**
 * container clean up
 *
 * @param container container with all current items
 *
 *  This optional callback is called prior to all
 *  item's being removed from the container. If you
 *  need to do any processing before that, do it here.
 *
 * @note
 *  The MFD helper will take care of releasing all the row contexts.
 *
 */
void
serialTable_container_free(netsnmp_container *container)
{
    DEBUGMSGTL(("verbose:serialTable:serialTable_container_free","called\n"));

    /*
     * TODO:380:M: Free serialTable container data.
     */
} /* serialTable_container_free */

/**
 * prepare row for processing.
 *
 *  When the agent has located the row for a request, this function is
 *  called to prepare the row for processing. If you fully populated
 *  the data context during the index setup phase, you may not need to
 *  do anything.
 *
 * @param rowreq_ctx pointer to a context.
 *
 * @retval MFD_SUCCESS     : success.
 * @retval MFD_ERROR       : other error.
 */
int
serialTable_row_prep( serialTable_rowreq_ctx *rowreq_ctx)
{
    DEBUGMSGTL(("verbose:serialTable:serialTable_row_prep","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    /*
     * TODO:390:o: Prepare row for request.
     * If populating row data was delayed, this is the place to
     * fill in the row for this request.
     */

    return MFD_SUCCESS;
} /* serialTable_row_prep */

/*
 * TODO:420:r: Implement serialTable index validation.
 */
/*---------------------------------------------------------------------
 * SYSBAS-MIB::serialEntry.portIndex
 * portIndex is subid 1 of serialEntry.
 * Its status is Current, and its access level is NoAccess.
 * OID: .1.3.6.1.4.1.1.3.1.1
 * Description:
Column Description
 *
 * Attributes:
 *   accessible 0     isscalar 0     enums  0      hasdefval 0
 *   readable   0     iscolumn 1     ranges 0      hashint   0
 *   settable   0
 *
 *
 * Its syntax is INTEGER32 (based on perltype INTEGER32)
 * The net-snmp type is ASN_INTEGER. The C type decl is long (long)
 *
 *
 *
 * NOTE: NODE portIndex IS NOT ACCESSIBLE
 *
 *
 */
/**
 * check validity of portIndex index portion
 *
 * @retval MFD_SUCCESS   : the incoming value is legal
 * @retval MFD_ERROR     : the incoming value is NOT legal
 *
 * @note this is not the place to do any checks for the sanity
 *       of multiple indexes. Those types of checks should be done in the
 *       serialTable_validate_index() function.
 *
 * @note Also keep in mind that if the index refers to a row in this or
 *       some other table, you can't check for that row here to make
 *       decisions, since that row might not be created yet, but may
 *       be created during the processing this request. If you have
 *       such checks, they should be done in the check_dependencies
 *       function, because any new/deleted/changed rows should be
 *       available then.
 *
 * The following checks have already been done for you:
 *
 * If there a no other checks you need to do, simply return MFD_SUCCESS.
 */
int
portIndex_check_index( serialTable_rowreq_ctx *rowreq_ctx )
{
    DEBUGMSGTL(("verbose:serialTable:portIndex_check_index","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    /*
     * TODO:426:M: |-> Check serialTable index portIndex.
     * check that index value in the table context is legal.
     * (rowreq_ctx->tbl_index.portIndex)
     */

    return MFD_SUCCESS; /* portIndex index ok */
} /* portIndex_check_index */

/**
 * verify specified index is valid.
 *
 * This check is independent of whether or not the values specified for
 * the columns of the new row are valid. Column values and row consistency
 * will be checked later. At this point, only the index values should be
 * checked.
 *
 * All of the individual index validation functions have been called, so this
 * is the place to make sure they are valid as a whole when combined. If
 * you only have one index, then you probably don't need to do anything else
 * here.
 * 
 * @note Keep in mind that if the indexes refer to a row in this or
 *       some other table, you can't check for that row here to make
 *       decisions, since that row might not be created yet, but may
 *       be created during the processing this request. If you have
 *       such checks, they should be done in the check_dependencies
 *       function, because any new/deleted/changed rows should be
 *       available then.
 *
 *
 * @param serialTable_reg
 *        Pointer to the user registration data
 * @param serialTable_rowreq_ctx
 *        Pointer to the users context.
 * @retval MFD_SUCCESS            : success
 * @retval MFD_CANNOT_CREATE_NOW  : index not valid right now
 * @retval MFD_CANNOT_CREATE_EVER : index never valid
 */
int
serialTable_validate_index( serialTable_registration * serialTable_reg,
                           serialTable_rowreq_ctx *rowreq_ctx)
{
    int rc = MFD_SUCCESS;

    DEBUGMSGTL(("verbose:serialTable:serialTable_validate_index","called\n"));

    /** we should have a non-NULL pointer */
    netsnmp_assert( NULL != rowreq_ctx );

    /*
     * TODO:430:M: |-> Validate potential serialTable index.
     */
    if(1) {
        snmp_log(LOG_WARNING,"invalid index for a new row in the "
                 "serialTable table.\n");
        /*
         * determine failure type.
         *
         * If the index could not ever be created, return MFD_NOT_EVER
         * If the index can not be created under the present circumstances
         * (even though it could be created under other circumstances),
         * return MFD_NOT_NOW.
         */
        if(0) {
            return MFD_CANNOT_CREATE_EVER;
        }
        else {
            return MFD_CANNOT_CREATE_NOW;
        }
    }

    return rc;
} /* serialTable_validate_index */

/** @} */
