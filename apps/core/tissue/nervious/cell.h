#ifndef __CELL_H
#define __CELL_H

#include "common.h"

#define CELL_CHANNEL_MAX    (6)         /* 细胞最大通道数，影响细胞可互相连接的最大数量 */

typedef struct cell_t cell_t;           

typedef enum
{
    CELL_TYPE_NORMAL,
    CELL_TYPE_PSEUDOUNIPOLAR,           /* 假单极神经元 */
    CELL_TYPE_BIPOLAR,                  /* 双极神经元 */
    CELL_TYPE_MULTIPOLAR,               /* 多级神经元 */
    CELL_TYPE_MAX,
}cell_type_t;

typedef void (*cell_work_t)(cell_t *cell, void *param);

/**
 * @brief 细胞基本结构
*/
struct cell_t
{
    void *private;
    cell_work_t work;
    cell_t **chnl;
    int chnl_len;
    cell_type_t type;
};

/**
 * @brief 细胞工厂结构
*/
typedef struct
{
    /**
     * @brief 创造一个细胞
     * @param [in]  type    细胞类型
     * @param [in]  doing   细胞工作函数
     * @return 返回细胞指针
    */
    cell_t *(*create)(cell_type_t type, cell_work_t work);

    /**
     * @brief 消灭一个细胞
     * @param [in]  cell    细胞指针的指针,执行后该指针会被设为NULL
     * @return
    */
    void (*destory)(cell_t **cell);

    /**
     * @brief 连通两个细胞
     * @param [in]  dst 目标细胞
     * @param [in]  src 源细胞
     * @param [in]  bilateral 是否双向连通?1,双向连通;0,只连通一个方向，方向为源细胞->目标细胞
     * @return
    */
    err_t (*connect)(cell_t *dst, cell_t *src, int bilateral);

    /**
     * @brief 断开两个细胞的连接
     * @param [in]  dst 目标细胞
     * @param [in]  src 源细胞
     * @param [in]  bilateral 是否双向断开?1,双向断开;0,只断开一个方向，方向为源细胞->目标细胞
     * 
     * @return
    */
    err_t (*disconnect)(cell_t *dst, cell_t *src, int bilateral);

    /**
     * @brief 打印细胞通道
     * @param [in]  cell 目标细胞
     * @return
    */
    void (*list_channel)(const cell_t *cell);
}cell_factory_t;

/**
 * @brief 细胞工厂
*/
extern cell_factory_t cell_tools;

#endif /* __CELL_H */