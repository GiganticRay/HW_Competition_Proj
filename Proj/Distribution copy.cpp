#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstdlib>
#include <time.h>

// 粗糙的处理订单的版本
#define COARSE
// 输出
#define OutPut
// 是否是用判题器进行判定的版本
#define LOCAL

using namespace std;

class ServerType{
    public:
        string type_name;
        int cpu_size;
        int mem_size;
        int hard_cost;
        int power_cost;
    public:
        ServerType(){

        }
        ServerType(string _type_name, int _cpu_size, int _mem_size, int _hard_cost, int _power_cost){
            type_name =     _type_name;
            cpu_size =      _cpu_size;
            mem_size =   _mem_size;
            hard_cost =     _hard_cost;
            power_cost =    _power_cost;
        }
};

// is_double_D: is distributed both in A and B?
class VMType{
    public:
        string type_name;
        int cpu_size;
        int mem_size;
        bool is_double_D;

    public:
        VMType(){

        }
        VMType(string _type_name, int _cpu_size, int _mem_size, bool _is_double_D){
            type_name =     _type_name;
            cpu_size =      _cpu_size;
            mem_size =   _mem_size;
            is_double_D =   _is_double_D;
        }
};

/*
 * 已购入的服务器对象
 * curr:            current
 * loading_VM_list: 挂载在该服务器上的VM集合，存储VM.vm_ID
 * server_ID:       the usage of server_ID is...
 */
class Server{
    public:
        string server_type_name;
        int server_ID;
        int A_curr_cpu_size;
        int A_curr_mem_size;
        int B_curr_cpu_size;
        int B_curr_mem_size;

        vector<int> loading_VM_list;

    public:
        Server(){

        }
        Server(string _server_type_name, int _server_ID, int _A_curr_cpu_size, int _A_curr_mem_size, int _B_curr_cpu_size, int _B_curr_mem_size){
            server_type_name =  _server_type_name;
            server_ID =         _server_ID;
            A_curr_cpu_size =   _A_curr_cpu_size;
            A_curr_mem_size =   _A_curr_mem_size;
            B_curr_cpu_size =   _B_curr_cpu_size;
            B_curr_mem_size =   _B_curr_mem_size;
        }
};

/*
 * 当前运行的虚拟机对象
 * is_in_A: 表明该对象是否挂载在server_ID服务器的A节点上，0表示在B节点，1表示在A节点，默认为-1，表示双节点挂载
 */
class VM{
    public:
       string vm_type_name;
       int vm_ID;
       int server_ID;
       int is_in_A;

    public:
       VM(){

       }
       VM(string _vm_type_name, int _vm_ID){
           vm_type_name =   _vm_type_name;
           vm_ID =          _vm_ID;
       }
       VM(string _vm_type_name, int _vm_ID, int _server_ID, int _is_in_A=-1){
           vm_type_name =   _vm_type_name;
           server_ID =      _server_ID;
           vm_ID =          _vm_ID;
           is_in_A =        _is_in_A;
       }
};

/*
 * 请求订单项
 * vm_type: 虚拟机的类型，当type=add时才用，默认为null
 * vm_ID:      待处理对象的ID，如del就是删除ID对应的虚拟机，add就是添加该虚拟机后的ID
 */
class RequestOrder{
    public:
        string op;
        string vm_type_name;
        int vm_ID;
    public:
        RequestOrder(){}
        RequestOrder(string _op, int _vm_ID, string _vm_type_name = ""){
            op =            _op;
            vm_type_name =  _vm_type_name;
            vm_ID =         _vm_ID;
        }

};

// 因为后面购买虚拟机序列中 add 操作提供的是vmType_name参数，所以要有个的vmType_name -> VmType obj的映射
unordered_map<string, VMType> VMTypeInfo;
// 与上同理
unordered_map<string, ServerType> ServerTypeInfo;

// 现有服务器信息，由于服务器买入就不会被退货了，所以其在vector中的下标，就是其ID
vector<Server> ServerList;
// 已售出的VM对象，通过ID可以直接索引得到该VM对象。
unordered_map<int, VM> VMMap;

// 截至在del之前的add订单集合
vector<RequestOrder> RequestAddList;

// 输出所需要的variables
#ifdef OutPut
unordered_map<string, int> DayPurchaseNumMap;
string TotalOutput;
string DayOutput;
vector<int> DayPurchaseVMIDList;
#endif // OutPut

#ifdef COARSE
// 将vm对象与server对象的资源做对比，看能否放得下(vm与server都可以是不完整变量，这里只做资源对比)
bool IsServerSatisfyVM(VM &_vm, Server &_server){
    VMType _vm_type = VMTypeInfo[_vm.vm_type_name];
	bool result = false;
	if(_vm_type.is_double_D){
		if(_server.A_curr_cpu_size > (_vm_type.cpu_size)/2 &&
				_server.B_curr_cpu_size > (_vm_type.cpu_size)/2 &&
				_server.A_curr_mem_size > (_vm_type.mem_size)/2 &&
				_server.B_curr_mem_size > (_vm_type.mem_size)/2 ){
			result = true;
		}
	}else{  // 单节点部署, 先看A节点，不够再B节点
		if(_server.A_curr_cpu_size > (_vm_type.cpu_size) &&
				_server.A_curr_mem_size > (_vm_type.mem_size)){
			result = true;
		}else if(_server.B_curr_cpu_size > (_vm_type.cpu_size) &&
				_server.B_curr_mem_size > (_vm_type.mem_size)){
			result = true;
		}
	}
	return result;
}

// 扣除服务器资源, 并且将VM与服务器相关联, 注意, 这个一定要在检查是否能够放入之后(IsServerSatisfyVM)再调用！
void PlantVMInServerItem(VM &_vm, Server &_server){
	VMType _vm_type = VMTypeInfo[_vm.vm_type_name];

	if(_vm_type.is_double_D){
		_server.A_curr_cpu_size -= (_vm_type.cpu_size)/2;
		_server.B_curr_cpu_size -= (_vm_type.cpu_size)/2;
		_server.A_curr_mem_size -= (_vm_type.mem_size)/2;
		_server.B_curr_mem_size -= (_vm_type.mem_size)/2;

		// 表明虚拟机的挂载方式
		_vm.is_in_A = -1;

	}else{  // 单节点部署, 先看A节点，不够再B节点
		if(_server.A_curr_cpu_size > (_vm_type.cpu_size) &&
				_server.A_curr_mem_size > (_vm_type.mem_size)){
			_server.A_curr_cpu_size -= (_vm_type.cpu_size);
			_server.A_curr_mem_size -= (_vm_type.mem_size);

			_vm.is_in_A = 1;

		}else if(_server.B_curr_cpu_size > (_vm_type.cpu_size) &&
				_server.B_curr_mem_size > (_vm_type.mem_size)){
			_server.B_curr_cpu_size -= (_vm_type.cpu_size);
			_server.B_curr_mem_size -= (_vm_type.mem_size);

			_vm.is_in_A = 0;
		}
	}
	// 处理服务器与虚拟机的联系信息
	_vm.server_ID = _server.server_ID;
	VMMap[_vm.vm_ID] = _vm;
	_server.loading_VM_list.push_back(_vm.vm_ID);
}

// 将虚拟机放入现有服务器列表资源，成功返回true、失败返回false
bool PlantVMInServerList(VM &_vm){
	bool result = false;

	// 遍历所有服务器，等待刘兄最小二乘按照浪费成本排序遍历
    vector<Server>::iterator curr_it = ServerList.begin() + (int)(ServerList.size()/1.33);
//    vector<Server>::iterator curr_it = ServerList.begin();
    vector<Server>::iterator end_it = ServerList.end();
    for(; curr_it != end_it; curr_it++){
		if(IsServerSatisfyVM(_vm, *curr_it)){
			PlantVMInServerItem(_vm, *curr_it);
			result = true;

#ifdef OutPut
            DayPurchaseVMIDList.push_back(_vm.vm_ID);
#endif // OutPut

			break;
		}
    }
    return result;

}

// 将虚拟机从服务器中移除，恢复服务器资源
void RemoveVMFromServerItem(VM &_vm, Server &_server){
	VMType _vm_type = VMTypeInfo[_vm.vm_type_name];

	if(_vm.is_in_A == -1){
		_server.A_curr_cpu_size += (_vm_type.cpu_size)/2;
		_server.B_curr_cpu_size += (_vm_type.cpu_size)/2;
		_server.A_curr_mem_size += (_vm_type.mem_size)/2;
		_server.B_curr_mem_size += (_vm_type.mem_size)/2;
	}else if(_vm.is_in_A == 1){
		_server.A_curr_cpu_size += (_vm_type.cpu_size);
		_server.A_curr_mem_size += (_vm_type.mem_size);
	}else if(_vm.is_in_A == 0){
		_server.B_curr_cpu_size += (_vm_type.cpu_size);
		_server.B_curr_mem_size += (_vm_type.mem_size);
	}
	// 处理服务器与虚拟机的联系信息，以及删除该虚拟机
	_server.loading_VM_list.erase(std::remove(_server.loading_VM_list.begin(),
											_server.loading_VM_list.end(),
											_vm.vm_ID),
											_server.loading_VM_list.end());
	VMMap.erase(_vm.vm_ID);

}
#endif // COARSE

// 对一系列添加订单做处理，扩容、迁移、分配工作全在这里做。处理完添加订单后要将RequestOrder列表清空
void DealOrder(vector<RequestOrder> &RequestAddList){
    vector<RequestOrder>::iterator curr_order_it = RequestAddList.begin();
    for(; curr_order_it!=RequestAddList.end(); curr_order_it++){

		// 由该添加订单先构建部分虚拟机实例
		VM vm = VM((*curr_order_it).vm_type_name, (*curr_order_it).vm_ID);

        if(!PlantVMInServerList(vm)){
            // 放置失败就扩容, 扩容策略：随机选择一个服务器进行购买
			// 这里应该单独一个函数出来的，但是由于扩容与当前订单密切相关，暂没想到分离的好办法
			// 初始化一个该类型的服务器，尝试将vm放入其中，如果不行则循环再次尝试, 直到成功位置

			while(true){
				int anticipate_server_index = rand() % ServerTypeInfo.size();
				unordered_map<string, ServerType>::iterator _it = ServerTypeInfo.begin();
				for(int i=0; i<anticipate_server_index; i++){
					_it++;   // 由于map不是顺序容器，所以只能这样做了...
				}
				ServerType _server_type = _it->second;

				Server server_item = Server(_server_type.type_name,
						ServerList.size()-1,
						_server_type.cpu_size/2,
						_server_type.mem_size/2,
						_server_type.cpu_size/2,
						_server_type.mem_size/2);
                // 给该服务器对象设置id，注意，新建的服务器不一定就可以放入服务器列表
                server_item.server_ID = ServerList.size();

				if(IsServerSatisfyVM(vm, server_item)){

					PlantVMInServerItem(vm, server_item);
					ServerList.push_back(server_item);
#ifdef OutPut
                    DayPurchaseVMIDList.push_back(vm.vm_ID);
					DayPurchaseNumMap[server_item.server_type_name]++;
#endif // OutPut

					break;
				}
			}
		}
    }
	// 清空
	RequestAddList.clear();
    return;
}

// 对单一删除的订单做处理, 运行该函数前进行判断 RequestOrder 是否为空
// 逻辑: requestOrderObject.vm_ID -> 得到VM_object -> vm.server_ID -> serverObj引用
// 恢复服务器的内存、移除服务器上维护的挂载节点列表
void DealOrder(RequestOrder requestorder){

    VM del_vm = VMMap[requestorder.vm_ID];
    Server& server = ServerList[del_vm.server_ID];
    RemoveVMFromServerItem(del_vm, server);
    return;
}

// 根据输入生成服务器类型信息，有服务器名->服务器类对象的map映射
void GenerateServerType(string type_name, int cpu_size, int mem_size, int hard_cost, int power_cost){
    ServerType _server_type = ServerType(type_name, cpu_size, mem_size, hard_cost, power_cost);
    ServerTypeInfo[type_name] = _server_type;
}

void GenerateVMType(string type_name, int cpu_size, int mem_size, bool is_double_D){
    VMType _vm_type = VMType(type_name, cpu_size, mem_size, is_double_D);
    VMTypeInfo[type_name] = _vm_type;
}

// 为了适应赛题输出而使用的工具函数
vector<Server>::iterator Find(vector<Server>::iterator start_it, vector<Server>::iterator end_it, string server_type_name){
    vector<Server>::iterator result_it;
    for(vector<Server>::iterator curr_it = start_it; start_it!=end_it; curr_it++){
        if(server_type_name.compare(curr_it->server_type_name) == 0){
            result_it = curr_it;
            break;
        }
    }
    return result_it;
}

bool IsLessThan(Server _server1, Server _server2){
    return _server1.server_ID < _server2.server_ID;
}


int main(){
    const string file_path = "/home/lei/Document/Proj/HW_Competition_Proj/OtherResource/TestData1.txt";

#ifdef LOCAL
    std::freopen(file_path.c_str(), "rb", stdin);
#endif // LOCAL

    srand((unsigned)time(NULL));

    int server_type_num;
    int vm_type_num;
    int total_day_num;
    string _tmp;

    // 输入可购入的服务器信息
    scanf("%d", &server_type_num);
    ServerType _server_type = ServerType();
    string type_name, cpu_size, mem_size, hard_cost, power_cost;
    for(int i=0; i<server_type_num; i++){
        cin >> type_name >> cpu_size >> mem_size >> hard_cost >> power_cost;
        GenerateServerType(type_name.substr(1, type_name.size()-2),
                stoi(cpu_size.substr(0, cpu_size.size()-1)),
                stoi(mem_size.substr(0, mem_size.size()-1)),
                stoi(hard_cost.substr(0, hard_cost.size()-1)),
                stoi(power_cost.substr(0, power_cost.size()-1)));
    }

    // 输入可售出的虚拟机信息
    scanf("%d", &vm_type_num);
    VMType _vm_type = VMType();
    string is_double_D;
    for(int i=0; i<vm_type_num; i++){
        cin >> type_name >> cpu_size >> mem_size >> is_double_D;
        GenerateVMType(type_name.substr(1, type_name.size()-2),
                stoi(cpu_size.substr(0, cpu_size.size()-1)),
                stoi(mem_size.substr(0, mem_size.size()-1)),
                stoi(is_double_D.substr(0, is_double_D.size()-1)));
    }

    scanf("%d", &total_day_num);

    // 遍历所有天的请求
    for(int curr_day_index=0; curr_day_index<total_day_num; curr_day_index++){
        int curr_day_request_num;
        string op, vm_ID;

        scanf("%d", &curr_day_request_num);

        // 遍历当天所有请求
        for(int curr_request_index=0; curr_request_index<curr_day_request_num; curr_request_index++){
            cin >> op;
            op = op.substr(1, op.size()-2);
            // 订单处理顺序逻辑：由于del请求很少，所以批处理add请求(这样就可以对该add集合内部的add进行乱序分配了，便于优化), 注意dealOrder(add集合)调用的位置。
            if(op.compare("add") == 0){
                cin >> type_name >> vm_ID;
                RequestOrder request_order = RequestOrder(op,
                        stoi(vm_ID.substr(0, vm_ID.size()-1)),
                        type_name.substr(0, type_name.size()-1));
                RequestAddList.push_back(request_order);
            }
            else if(op.compare("del") == 0){
                cin >> vm_ID;
                RequestOrder request_order = RequestOrder(op,
                        stoi(vm_ID.substr(0, vm_ID.size()-1)));
                DealOrder(RequestAddList);
                DealOrder(request_order);
            }
        }
        DealOrder(RequestAddList);

#ifdef OutPut   // append single day's output to the TotalOutput
        TotalOutput += ("(purchase, " + std::to_string(DayPurchaseNumMap.size()) + ")\n");
        for(unordered_map<string, int>::iterator it = DayPurchaseNumMap.begin(); it != DayPurchaseNumMap.end(); it++){
            TotalOutput += ("(" + it->first + ", " + std::to_string(it->second) + ")\n");
        }
        TotalOutput += ("(migration, 0)\n");

        // 计算出当天购买的服务器数量、当天购买的服务器在ServerList中的起始下标
        int num_of_day_purchase = 0;
        for(auto &curr_type_it : DayPurchaseNumMap){
            num_of_day_purchase += curr_type_it.second;
        }
        int curr_day_begin_idx = ServerList.size() - num_of_day_purchase;
        int modified_curr_server_id = curr_day_begin_idx;

        for(auto &curr_type_it : DayPurchaseNumMap){
            vector<Server>::iterator curr_server_it = ServerList.begin() + curr_day_begin_idx;
            for(int i=0; i<curr_type_it.second; i++){
                // 在ServerList中对应今天的区间内依次寻找该类型的服务器，找到后修正该服务器的id，以及修改挂载在该服务器上的虚拟机的id
                curr_server_it = Find(curr_server_it, ServerList.end(), curr_type_it.first);
                curr_server_it->server_ID = modified_curr_server_id;
                for(auto &curr_vm_id_it : curr_server_it->loading_VM_list){
                    VMMap[curr_vm_id_it].server_ID = modified_curr_server_id;
                }
                curr_server_it++;
                modified_curr_server_id++;
            }
        }

        // 在修改完当天的server_id之后，要将其与ServerList中下标对应起来。
        sort(ServerList.begin()+curr_day_begin_idx, ServerList.end(), IsLessThan);

        for(auto& it : DayPurchaseVMIDList){
            if(VMMap[it].is_in_A == -1){
                DayOutput += ("(" + std::to_string(VMMap[it].server_ID) + ")" + "\n");
            }else if(VMMap[it].is_in_A == 1){
                DayOutput += ("(" + std::to_string(VMMap[it].server_ID) + ", A" + ")\n");
            }else {
                DayOutput += ("(" + std::to_string(VMMap[it].server_ID) + ", B" + ")\n");
            }
        }
        TotalOutput += DayOutput;


        DayPurchaseVMIDList.clear();
        DayOutput.clear();
        DayPurchaseNumMap.clear();
#endif // OutPut

        // cout << "finished day " << curr_day_index << endl;
    }

#ifdef LOCAL
    freopen("out.txt", "w", stdout);
#endif // LOCAL

    cout << TotalOutput.substr(0, TotalOutput.size()-1) << endl;
//    cout << "total number of server resources: " << ServerList.size() << endl;

    return 0;
}
