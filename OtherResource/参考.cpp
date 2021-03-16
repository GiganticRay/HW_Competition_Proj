#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <algorithm>
#include <cassert>
using namespace std;

// 提交
//#define UPLOAD

#define TEST

// 服务器信息
unordered_map<string,vector<int>> serverInfos;
// 虚拟机信息
unordered_map<string,vector<int>> vmInfos;
// 请求信息
vector<vector<string>> requestInfos;

// 购买的服务器信息
int serverNumber = 0;
unordered_map<int,vector<int>> sysServerResource;

// 当前开机服务器
vector<int> serverRunVms;
// 记录虚拟机运行在那个服务器上
unordered_map<string,vector<int>> vmOnServer;

vector<string> res;

#ifdef TEST
    const string filePath = "../training-data/training-1.txt";
#endif
// 成本
long long SERVERCOST = 0,POWERCOST=0,TOTALCOST =0;

// 按照服务器A点内存降序排序
// std::sort(serverInfos.begin(),serverInfos.end(),cmpServerMemoryDown)
// 按照服务器A点内存升序排序
//bool cmpServerMemoryUp(const Server &server1, const Server &server2)
//{
//    return server1.memoryA>server2.memoryA;
//}


void generateServer(string &serverType,string &cpuCores,string &memorySize,string &serverCost,string &powerCost){
    string _serverType="";
    for(int i =1;i<serverType.size() -1;i++){
        _serverType += serverType[i];
    }
    int _cpuCores =0,_memorySize=0,_serverCost=0,_powerCost=0;

    for(int i=0;i<cpuCores.size() -1;i++){
        _cpuCores = 10*_cpuCores + cpuCores[i] - '0';
    }
    for(int i=0;i<memorySize.size() -1;i++){
        _memorySize = 10*_memorySize + memorySize[i] - '0';
    }
    for(int i=0;i<serverCost.size() -1;i++){
        _serverCost = 10*_serverCost + serverCost[i] - '0';
    }
    for(int i=0;i<powerCost.size()-1;i++){
        _powerCost = 10*_powerCost + powerCost[i] - '0';
    }
    serverInfos[_serverType] = vector<int>{_cpuCores/2 ,_cpuCores/2,_memorySize/2,_memorySize/2,_serverCost,_powerCost};
}


void generateVm(string &vmType,string &vmCpuCores,string &vmMemory,string &vmTwoNodes){
    string _vmType;

    for(int i=1;i<vmType.size() -1;i++){
        _vmType += vmType[i];
    }

    int _vmCpuCores = 0,_vmMemory=0,_vmTwoNodes=0;
    for(int i=0;i<vmCpuCores.size()-1;i++){
        _vmCpuCores = _vmCpuCores*10 + vmCpuCores[i] - '0';
    }
    for(int i=0;i<vmMemory.size()-1;i++){
        _vmMemory = _vmMemory*10 + vmMemory[i] - '0';
    }
    if(vmTwoNodes[0] == '1'){
        _vmTwoNodes = 1;
    }
    else{
        _vmTwoNodes =0;
    }
    vmInfos[_vmType] = vector<int>{_vmCpuCores,_vmMemory,_vmTwoNodes};
}
// 解析用户添加请求
void generateRequest(string &op,string &reqVmType,string &reqId){
    string _op,_reqVmType,_reqId;
    _op = op.substr(1,op.size() -1);
    _reqVmType = reqVmType.substr(0,reqVmType.size() -1);
    _reqId = reqId.substr(0,reqId.size() -1);
    requestInfos.push_back(vector<string>{_op,_reqVmType,_reqId});
}

// 解析用户删除请求
void generateRequest(string &op,string &reqId){
    string _op,_reqId;
    _reqId = reqId.substr(0,reqId.size() -1);
    _op = op.substr(1,op.size() -1);
    requestInfos.push_back(vector<string>{_op,_reqId});
}

// 尝试在服务器上分配虚拟机资源
bool choseServer(vector<int> &server,vector<int> &vm,int serverId,string vmId){
    int vmCores = vm[0],vmMemory = vm[1],vmTwoNodes = vm[2];
    int &serverCoreA = server[0],&serverCoreB = server[1],&serverMemoryA = server[2],&serverMemoryB = server[3];
    if(vmTwoNodes){
        int needCores = vmCores/2,needMemory = vmMemory/2;
        if(serverCoreA >= needCores && serverCoreB >=needCores && serverMemoryA >= needMemory && serverMemoryB >= needMemory){
            serverCoreA -= needCores;
            serverCoreB -= needCores;
            serverMemoryA -= needMemory;
            serverMemoryB -= needMemory;
            vmOnServer[vmId] = vector<int>{serverId,vmCores,vmMemory,1,2};
            res.push_back("("+to_string(serverId)+")\n");
            return true;
        }
        else{
            return false;
        }
    }
    else if(serverCoreA >= vmCores && serverMemoryA >= vmMemory){
        serverCoreA -= vmCores;
        serverMemoryA -= vmMemory;
        vmOnServer[vmId] = vector<int>{serverId,vmCores,vmMemory,1};
        res.push_back("("+to_string(serverId)+", A)\n");
        return true;
    }
    else if(serverCoreB >= vmCores && serverMemoryB >= vmMemory){
        serverCoreB -= vmCores;
        serverMemoryB -= vmMemory;
        vmOnServer[vmId] = vector<int>{serverId,vmCores,vmMemory,2};
        res.push_back("("+to_string(serverId)+", B)\n");
        return true;
    }
    return false;
}


// 处理创建虚拟机操作
int createVM(vector<string> &createVmInfo){
    string _reqVmType = createVmInfo[1],_reqId = createVmInfo[2];
    vector<int> vm = vmInfos[_reqVmType];
    int success = -1;
    for(int i=0;i<serverNumber;i++){
        auto &server = sysServerResource[i];
        if(choseServer(server,vm,i,_reqId)) {
            serverRunVms[i]++;
            success = 1;
            break;
        }
        assert(server[0]>=0 && server[1]>=0 && server[2]>=0 && server[3]>=0);
    }
    return success;
}

// 处理删除虚拟机操作
void delVM(vector<string> &delVmInfo){
    string _vmId = delVmInfo[1];
    vector<int> _vmInfo = vmOnServer[_vmId];
    int _serverId = _vmInfo[0];

    serverRunVms[_serverId]--;

    vector<int> &server = sysServerResource[_serverId];
    if(_vmInfo.size() == 5){
        int cores = _vmInfo[1]/2,memory = _vmInfo[2]/2;
        server[0] += cores;
        server[1] += cores;
        server[2] += memory;
        server[3] += memory;
    }
    else{
        int cores = _vmInfo[1],memory = _vmInfo[2];
        if(_vmInfo[3] == 1){
            server[0] += cores;
            server[2] += memory;
        }else{
            server[1] += cores;
            server[3] += memory;
        }
    }
}

// 初始化server，如何初始化购买的服务器是一个大的优化
void bugServer(){
    string serverType = "hostUY41I";
    int n =2500;
    serverRunVms.resize(n,0);
    string initBuy = "(purchase, ";
    initBuy += to_string(2)+")\n";

    res.push_back(initBuy);

    string pauseInfo ="("+serverType+", ";
    pauseInfo+= std::to_string(n/2)+")\n";

    res.push_back(pauseInfo);

    for(int i=0;i<n/2;i++){
        sysServerResource[serverNumber++] = serverInfos[serverType];
        SERVERCOST += serverInfos[serverType][4];

    }
    serverType = "host78BMY";
    pauseInfo ="("+serverType+", ";
    pauseInfo+= std::to_string(serverNumber)+")\n";

    res.push_back(pauseInfo);

    for(int i=0;i<n/2;i++){
        sysServerResource[serverNumber++] = serverInfos[serverType];
        SERVERCOST += serverInfos[serverType][4];
    }

}

// 扩容服务器策略
void expansion(){
    string s = "(purchase, 0)\n";
    res.push_back(s);
}

// 迁移虚拟机策略
void migrate(){
    string s = "(migration, 0)\n";
    res.push_back(s);
}

// 分配策略
void match(int day){
    if(day!=0) expansion();
    migrate();
//    printf("There are %d requests waiting to matching !!\n",requestInfos.size());
    for(auto req:requestInfos){
        // 创建虚拟机 还是 删除虚拟机
        int opType = req.size() == 3 ? 1:0;
        if(opType){
            int resourceEnough = createVM(req);
            assert(resourceEnough != -1);
        }
        else{
            // 修复删除虚拟机bug
            delVM(req);
        }
    }
}

void serverPower(){
    for(int i=0;i<serverNumber;i++){
        if(serverRunVms[i] != 0){
            POWERCOST += sysServerResource[i][5];
        }
    }
}


int main() {
    clock_t start, finish;
    start = clock();
#ifdef TEST
    std::freopen(filePath.c_str(),"rb",stdin);
#endif
    int serverNum;
    string serverType,cpuCores,memorySize,serverCost,powerCost;
    scanf("%d",&serverNum);

    for(int i =0;i<serverNum;i++){
        cin>>serverType>>cpuCores>>memorySize>>serverCost>>powerCost;
        generateServer(serverType,cpuCores,memorySize,serverCost,powerCost);
    }

    int vmNumber = 0;
    scanf("%d",&vmNumber);

    string vmType,vmCpuCores,vmMemory,vmTwoNodes;
    for(int i =0;i<vmNumber;i++){
        cin>>vmType>>vmCpuCores>>vmMemory>>vmTwoNodes;
        generateVm(vmType,vmCpuCores,vmMemory,vmTwoNodes);
    }

    int requestdays = 0,dayRequestNumber = 0;
    scanf("%d",&requestdays);
    string op,reqVmType,reqId;

    // 开始处理请求
    bugServer();
    for(int day=0;day<requestdays;day++){
        scanf("%d",&dayRequestNumber);
        requestInfos.clear();
        for(int i =0;i<dayRequestNumber;i++){
            cin>>op;
            if(op[1] == 'a'){
                cin>>reqVmType>>reqId;
                generateRequest(op,reqVmType,reqId);
            }else{
                cin>>reqId;
                generateRequest(op,reqId);
            }
        }
#ifdef TEST
        if(day == 0 || (day+1)%100 == 0){
            printf("The %d day begin matching!!!\n",day+1);
        }
#endif
        match(day);
        serverPower();
//        break;
    }

    fclose(stdin);
    finish = clock();
    TOTALCOST = SERVERCOST + POWERCOST;
#ifdef UPLOAD
    for (auto &s:res) std::cout<<s;
#endif
#ifdef TEST
    printf("\nusr time: %f s \n",double(finish - start) / CLOCKS_PER_SEC);
    printf("server cost: %lld \npower cost: %lld \ntotal cost: %lld \n",SERVERCOST,POWERCOST,TOTALCOST);
#endif

    return 0;
}
