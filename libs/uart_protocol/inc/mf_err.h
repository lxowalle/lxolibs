#ifndef __MF_ERR_H
#define __MF_ERR_H

typedef enum
{
    MF_ERR = -1,
    MF_OK,
    MF_ERR_PARAM,
    MF_ERR_MEM,
    MF_ERR_NORMAL,
    MF_ERR_REDEFINE,
    MF_ERR_UNDEFINE,
    MF_ERR_TODO
}mf_err_t;

#endif /** __MF_ERR_H */