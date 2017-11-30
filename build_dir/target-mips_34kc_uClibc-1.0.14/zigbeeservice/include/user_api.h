/*************************************************************************
 *
    > File Name: user_api.h
    > Author: lunan
    > Mail: 6616@shuncom.com 
    > Created Time: 2015年09月11日 星期五 13时32分55秒
 ************************************************************************/

#ifndef _USER_API_H_
#define _USER_API_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include "user_types.h"
#include <sys/types.h>
#include <stdint.h>
#include "alink_export_zigbee.h"

/**
 * 所谓API,     就是提供给程序员使用的函数，函数的定义实现在SDK里.so里
 * 这些API用于网关的功能开发
 */

/*********************************************************通用API***********************************************/

extern int bytes2hexStr(const unsigned char *buf,char *hexStr,int bufLen);//字节数组->16进制字符串

extern int hexStr2bytes(const char *hexStr,unsigned char *buf,int bufLen);//16进制字符串->字节数组

extern void zha_get_ieeeaddr_by_string(char *ieeeaddr_string,uint8_t *extAddr);//MAC地址字符串->字符数组

extern Zstatus_t zigbee_init(char *dbname);//初始化zigbee模块

extern void zigbee_loop(void);//事件循环的主入口

/**********************************************************设备API*************************************************/

extern void device_scanNet(void);
//扫描当前网关

extern void device_getDevicesList(void);//获取当前网关下的所有节点状态
//结果在获取所有zigbee节点状态信息回掉函数中打印typedef Zstatus_t(*user_allState_cb_t)(deviceInfo_t *devices,uint16_t num)
//不维护，使用avl_for_each_element_safe()遍历当前网关下的所有节点信息
//在defines.h中的一个宏函数

extern Zstatus_t device_getDeviceClusterId(uint8_t ieee_addr[8],uint8_t endpoint_id,uint16_t clusterId,uint16_t attribute_id);
//获取单个设备下的某一簇cluster下的某一属性值attribute，调用本接口，网关将直接发读取命令到设备上进行读取

extern uint8_t device_getDevice(uint8_t *ieeeAddr,deviceInfo_t **device);
//获取单个设备下的所有端点的状态，结果保存在结构体devices中
//返回端点个数

extern uint16_t device_getDevicesList_fill(deviceInfo_t **devices);
//获取当前网关下的所有节点状态，结果保存在结构体devices中
//返回设备个数
//avl_for_each_element_safe(&(device_table_head.hdr),device,avl,pdevice)

extern Zstatus_t device_setDeviceAttr(uint8_t *ieeeAddr,uint8_t endpointId,char *attrName,void *attrValue);
//根据关键字，设置节点属性的值或状态，开关量/光照/...

extern Zstatus_t device_sendDeviceCmd(uint8_t *ieeeAddr,uint8_t endpointId,uint16_t clusterId,uint8_t cmdId,uint16 cmdFormatLen, uint8 *cmdFormat );
//发送cmd命令给设备，对设备进行控制

extern Zstatus_t device_writeDeviceAttr(uint8_t *ieeeAddr,uint8_t endpointId,uint16_t clusterId,uint16_t attrId,uint8_t dataType,void *data);
//根据簇cluster id和attribute id，设置节点的值或状态

extern Zstatus_t device_setDeviceState(deviceBasic_t *deviceBasic,deviceState_t *deviceState);
//同时设置节点的多个状态
//不维护
//device_setDeviceAttr();

extern Zstatus_t device_setPermitJoin(uint8_t *ieeeaddr,uint8_t duration);
//开始permit join许可加入
//第一个参数：全0为网关地址，全F为所有zigbee节点地址
//第二个参数：持续时间s，0为关闭，255为一直打开

extern Zstatus_t device_leaveReq(uint8_t *ieeeaddr);
//删除单个zigbee节点

extern void device_refreshHeartbeat(void);
//刷新心跳
//暂未实现

extern void device_loadManufacturerSupperversion(char *ManufacturerName,char *ModelId,uint16_t supperversion);
//加载设备制造商版本信息

/**********************************************************白名单API****************************************************/
//白名单存在网关的数据库里"/etc/config/zha.db"

extern Zstatus_t device_cleanWhitelist(void);
//清空白名单

extern Zstatus_t device_writeWhitelist(uint8_t *ieeeaddr);
//添加节点到白名单

extern Zstatus_t device_deleteWhitelist(uint8_t *ieeeaddr);
//从白名单删除设备

extern uint16_t device_getWhitelist(uint8_t *ieeeaddr);
//获取白名单列表，返回白名单结点个数，保存设备8*8 = 64位ieee地址到数组中

extern void device_enableWhitelist(uint8_t status);
//开启白名单列表（1开0关）

extern Zstatus_t user_quanshiWriteData(uint8_t *ieeeAddr,uint16_t clusterId,uint8_t *data,uint8_t len);

/**********************************************************组API***********************************************************/
//组也是存储在网关中"/etc/config/zha.db"
extern uint16_t group_create(char *groupName);
//创建组

extern Zstatus_t group_removeAllDevices(uint16_t groupId);
//删除组中所有设备

extern Zstatus_t group_addDevice(uint16_t groupId, groupMemberInfo_t *groupMember);
//添加单个设备到组

extern Zstatus_t group_viewDevice(uint16_t groupId, groupMemberInfo_t *groupMember);
//获取设备所属分组，触发zigbeeGroupStateChange回掉，在回掉函数中，返回设备的分组列表

extern Zstatus_t group_removeDevice(uint16_t groupId, groupMemberInfo_t *groupMember);
//删除分组中单个设备

extern Zstatus_t group_changeName(uint16_t groupId,char *name);
//修改组名

extern Zstatus_t group_setDeviceAttr(uint16_t groupId,char *attrName,void *attrValue);
//修改组的某一属性

extern groupInfo_t *group_getList(uint16_t *nums);
//获取所有组列表
//该api返回的指针数组是sdk中存放组信息的地址，不能对它进行修改，也不要free

extern Zstatus_t group_get(uint16_t id,groupInfo_t *group);
//获取单个组的信息

extern Zstatus_t group_del(uint16_t id);
//删除组

/**********************************************************场景API****************************************************/
//"/etc/config/zha.db"

extern uint8_t scene_create(char * sceneName,uint16_t groupID,uint16_t iconId);
//创建场景

extern Zstatus_t scene_set(uint8_t id,char *name,uint16_t groupId,uint16_t iconId);
//修改场景名字/组id/图标，如果有不需要修改的参数，传原来的值

extern Zstatus_t scene_store(uint8_t sceneId);
//保存场景

extern Zstatus_t scene_recall(uint8_t sceneId);
//激活场景

extern sceneInfo_t *scene_getList(uint16_t *nums);
//获取所有场景列表信息

extern Zstatus_t scene_del(uint8_t id);
//删除场景

/**********************************************************策略规则API*******************************************************/
//策略存储在网关的数据库里"/etc/config/zha.db"
extern uint16_t rule_create(rule_t_t * newRule);
//创建策略

extern Zstatus_t rule_enableRule(uint16_t ruleId,uint8_t status);
//使用/禁用 策略

extern rule_t_t * rule_find(uint16_t ruleId);
//获取策略id对应的，策略信息

extern void rule_update(rule_t_t  *rule);
//再次修改策略后，调用这个API把修改后的策略更新到数据库

extern Zstatus_t rule_del(uint16_t id);
//删除策略

/*********************************************************协调器API**********************************************************/
// coordinator 协调者;

extern void coord_getInfo(coordInfo_t *coordInfo);
//获取协调器信息

extern void coord_changeChannel(uint8_t channel);
//改变协调器的信道

extern void coord_cleanDeviceList(void);
//清除协调器上的所有设备

extern void coord_resetFactNew(void);
//协调器恢复出厂设置，所有网络参数都会重新分配

/*
* @brif 
*	ManufacturerName:³§¼ÒÃû£¬¿ÉÒÔ²»Ìî£¬²»ÌîµÄÊ±ºòÖÃNULL
*	ModelId:Ò»¶¨ÒªÌî
*	supperversion:Éè±¸µÄÐÄÌøÖÜÆÚ£¬ÉèÎª0xfffe±íÊ¾¸ÃÀàÉè±¸Ã»ÓÐÐÄÌø
*
*/
extern void device_loadManufacturerSupperversion(char *ManufacturerName,char *ModelId,uint16_t supperversion);
//加载设备制造商版本信息

extern Zstatus_t Zap_znpIncoming(uint8_t *buf,uint8_t len);
//当用户串口无法开放时，通过这个api处理zigbee通过串口传进来的数据

extern uint16_t device_getNwkAddrByIeeeAddr(uint8_t *ieeeAddr);
//根据ieeeAddr获取networkAddr

extern char *lib_compileTime(void);
//获取动态链接库编译时间


#ifdef __cplusplus
}
#endif


#endif
