#include "inc/db.h"
#include "inc/facedb.h"

db_t db;

/**
 * @brief Choose db type
 * @return 
*/
db_err_t db_choose(db_type_t type)
{
    db_err_t err = MF_OK;

    switch (type)
    {
    case DB_TYPE_FACE:
        memcpy(&db, get_facedb_handle(), sizeof(db_t));
        err = MF_OK;
        break;
    default:
        err = MF_ERR_UNINIT;
        break;
    }

    return err;
}


