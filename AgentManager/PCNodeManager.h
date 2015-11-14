#ifndef _PCMANAGER_H_
#define _PCMANAGER_H_

#include "../generic.h"
#include "PCNodeInfo.h"

// 1. 节点管理初始化
//    用主机自身信息，初始化节点管理器
#define PCMANAGER_INIT_ERROR   -1
#define PCMANAGER_INIT_OK       1
int PCMANAGER_INIT(pPCNodeInfo nodeself);

// 2. 添加临近节点
//    向主机添加临近节点，如果已经更新了管理树，则向管理树添加临近节点
#define PCMANAGER_ADDNEIGHBOR_ERROR  -1
#define PCMANAGER_ADDNEIGHBOR_OK      1
int PCMANAGER_ADDNeighbor(pPCNodeInfo newnode);

// 3. 添加远端节点
//    向管理树添加远程节点。
#define PCMANAGER_ADDREMOTE_ERROR    -1
#define PCMANAGER_ADDREMOTE_OK        1
int PCMANAGER_ADDRemote(int fatherid,pPCNodeInfo newnode);

// 4. 设置上游节点
//    在临近节点中找到管理节点，并更新管理树。
#define PCMANAGER_SETUPPER_ERROR     -1
#define PCMANAGER_SETUPPER_OK         1
int PCMANAGER_SETUpperAdmin(int nodeid);

// 5. 上游节点失联
//    清空管理树，保持临近节点状态。
#define PCMANAGER_UPPERADMINLOST_ERROR  -1
#define PCMANAGER_UPPERADMINLOST_OK      1
int PCMANAGER_UpperAdminLost();
//----------------------------------------------need restart from here
// 6. 移除节点
//    自动判定被删节点为本地或远端节点，然后删除相应节点
#define PCMANAGER_REMOVENODE_ERROR   -1
#define PCMANAGER_REMOVENODE_OK       1
int PCMANAGER_RemoveNode(int id);

// 7. 更新ID
//    通过id找到节点，并重新更新其ID值
#define PCMANAGER_REPLACEID_ERROR   -1
#define PCMANAGER_REPLACEID_OK       1
int PCMANAGER_ReplaceID(int oldid,int newid);

// 8. 获得节点信息
//    通过ID定位节点，并返回节点的结构
#define PCMANAGER_GETNODEINFO_ERROR  NULL
pPCNodeInfo PCMANAGER_GETNodeInfo(int id);

// 9. 获得下一跳节点信息
//    通过ID定位节点，并找到其所在支路的下一跳。
#define PCMANAGER_GETNEXTJUMP_ERROR   NULL
pPCNodeInfo PCMANAGER_GETNextJump(int id);

// 10. 现实节点图
//     展示该节点所有的数据结构
#define PCMANAGER_SHOWMAP_ERROR       -1
#define PCMANAGER_SHOWMAP_OK           1
int PCMANAGER_ShowMap();

// 11. 获得直接节点个树
#define PCMANAGER_GETNEIGHBORNUM_ERROR -1
int PCMANAGER_GETNeighborNum();

// 12. 获得所有节点个树
#define PCMANAGER_GETALLNODENUM_ERROR  -1
int PCMANAGER_GETAllNodeNum();

#define PCMANAGER_HAVANODE_NO           0
#define PCMANAGER_HAVANODE_YES          1
int PCMANAGER_HAVENode(int id);

#define PCMANAGER_TRAVERSAL_NEIGHBOR_ERROR -1
#define PCMANAGER_TRAVERSAL_NEIGHBOR_OK     1
int PCMANAGER_Traversal_Neighbor(pfunNodeCall callback);
#endif
