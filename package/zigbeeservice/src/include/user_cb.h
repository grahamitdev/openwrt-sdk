/*************************************************************************
 *
    > File Name: user_cb.h
    > Author: lunan
    > Mail: 6616@shuncom.com 
    > Created Time: 2015年09月11日 星期五 11时55分19秒
 ************************************************************************/

#ifndef _USER_CB_H_
#define _USER_CB_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include "user_types.h"
#include "types.h"

#define MAX_DEVICESTATE_CBS   10
//SDK(.so)首先拿到这些函数的指针，开始事件循环，适当的时机，SDK核心部分会携带对应的参数进入这些函数，其实就是SDK通知用户现在网关在做什么事情
//供SDK使用的回掉函数，SDK拿到函数指针，准备从函数入口进入相关函数，并传入相关参数
typedef Zstatus_t (*user_allState_cb_t)( deviceInfo_t *devices,uint16_t num );
//通知用户：获取网关下面所有zigbee节点的状态,可自己修改该函数，把节点信息读出来

typedef Zstatus_t (*user_stateChange_cb_t)( deviceInfo_t *dataInfo,uint16_t clusterId);
//通知用户：当zigbee网关下的节点状态发生改变时，SDK会进入该回掉并打印状态改变的那个zigbee节点最新状态，在终端打印

typedef Zstatus_t (*user_zigbeeModuleStatus_cb_t)(ZigbeeModuleStatus_t status);
//通知用户：sdk和网关模块初始化状态已经完成，当SDK进入这个回调时zigbee网关部分初始化完成，可对节点进行操作

typedef Zstatus_t (*user_whiteList_cb_t)(uint8_t *extAddr,uint8_t num);
//通知用户：白名单列表

typedef uint16_t (*user_Zap_znpOutgoing_cb_t)(uint8_t *data,uint16_t len);

typedef int (*user_zigbee_update_online_status_cb_t)(unsigned char ieee_addr[8], char online_or_not);
//如果设备从在线变成离线，或者从离线变成在线，会进入回掉

typedef int (*user_zigbee_register_device_cb_t)(unsigned char ieee_addr[8],
        unsigned int model_id, const char *rand, const char *sign);
//有设备向网关注册

typedef int (*user_zigbee_unregister_device_cb_t)(unsigned char ieee_addr[8]);
//有设备向网关注销

typedef int (*user_zigbee_groupStateChange_cb_t)(uint8_t ieee_Addr[8],uint8_t epId,uint8_t cmdId,uint8_t status,uint8_t grpCnt,uint16_t *grpList,uint8_t capacity,char *grpName);
//组的状态发生改变

typedef int (*user_zigbee_sceneStateChange_cb_t)(uint8_t ieee_Addr[8],uint8_t epId,uint8_t cmdId,uint8_t status,uint8_t grpCnt,uint16_t *grpList,uint8_t capacity,char *grpName);
//场景改变会进入该会掉

typedef uint16_t (*user_quanshiData_cb_t)(uint8_t ieee_Addr[8],uint16_t clusterId,uint8_t *data,uint16_t len);

typedef int (*user_zigbee_announce_cb_t)(unsigned char ieee_addr[8],uint16_t nwk_addr);

typedef int (*user_zigbee_cmdIncoming_cb_t)(unsigned char ieee_addr[8],uint8_t epId,uint16_t clusterId,uint8_t cmdId,void *pCmd);
//有设备发送命令数据，会进入该回掉

typedef int (*user_zigbee_attrChange_cb_t)(unsigned char ieee_addr[8],uint8_t epId,char *attrName,char  *attValue);
//有设备的属性发生改变

typedef int (*user_zigbee_cmdChange_cb_t)(unsigned char ieee_addr[8],uint8_t epId,char *cmdName,char *cmdValue);
//有设备的cmd发生改变

typedef int (*user_zigbee_reportIn_cb_t)(device_info_t *device,uint8_t epId,uint16_t clusterId,deviceReportCmd_t *deviceReport);
//设备上报心跳信息，会进入该回掉

typedef int (*shuncom_interpanTest_cb_t)(uint8_t tx_LinkQuality,uint8_t rx_LinkQuality,uint16_t zigbeeModule_version);
//网关

typedef int (*user_sendResult_cb_t)(uint8_t *send_result);
//网关发送结果

typedef struct {

	user_allState_cb_t pfn_allState;
  //通知用户：获取网关下面所有zigbee节点的状态,可自己修改该函数，把节点信息读出来
	user_stateChange_cb_t pfn_stateChange;
  //通知用户：当zigbee网关下的节点状态发生改变时，SDK会进入该回掉并打印状态改变的那个zigbee节点最新状态，在终端打印
  user_zigbeeModuleStatus_cb_t pfn_zigbeeModuleStatus;
  //通知用户：sdk和网关模块初始化状态已经完成，当SDK进入这个回调时zigbee网关部分初始化完成，可对节点进行操作
  user_zigbee_update_online_status_cb_t pfn_zigbeeUpdateOnlineStatue;
  //如果设备从在线变成离线，或者从离线变成在线，会进入回掉
  user_zigbee_register_device_cb_t pfn_zigbeeRegisterDevice;
  //有设备向网关注册
  user_zigbee_unregister_device_cb_t pfn_zigbeeUnRegisterDevice;
  //有设备向网关注销
  user_zigbee_groupStateChange_cb_t pfn_zigbeeGroupStateChange;
  //组的状态发生改变
  user_zigbee_sceneStateChange_cb_t pfn_zigbeeSceneStateChange;
  //场景改变会进入该会掉
  user_zigbee_announce_cb_t pfn_zigbee_announce;
  
  user_zigbee_cmdIncoming_cb_t pfn_zigbee_cmdIncoming;
  //有设备发送命令数据，会进入该回掉
  user_zigbee_attrChange_cb_t pfn_zigbee_attrChange;
  //有设备的属性发生改变
  user_zigbee_cmdChange_cb_t pfn_zigbee_cmdChange;
  //有设备的cmd发生改变
  user_zigbee_reportIn_cb_t pfn_zigbee_reportIn;
  //设备上报心跳信息，会进入该回掉
  user_sendResult_cb_t pfn_sendResultIn;
  //网关发送结果
} user_zigbeeCBs_t;

Zstatus_t user_registerZigbeeCBs(user_zigbeeCBs_t *callbacks);

Zstatus_t user_registerZigbeeModuleDataCBs(user_Zap_znpOutgoing_cb_t callbacks);

Zstatus_t user_registerQuanshi(user_quanshiData_cb_t callbacks);

#ifdef __cplusplus
}
#endif


#endif
