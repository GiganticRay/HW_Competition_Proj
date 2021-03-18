#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>

#define TEST

using namespace std;

class ServerType{
    public:
        string type_name;
        int cpu_size;
        int memory_size;
        int hard_cost;
        int power_cost;
    public:
        ServerType(){

        }
        ServerType(string _type_name, int _cpu_size, int _memory_size, int _hard_cost, int _power_cost){
            type_name =     _type_name;
            cpu_size =      _cpu_size;
            memory_size =   _memory_size;
            hard_cost =     _hard_cost;
            power_cost =    _power_cost;
        }
};

// is_double_D: is distributed both in A and B?
class VMType{
    public:
        string type_name;
        int cpu_size;
        int memory_size;
        bool is_double_D;    
    
    public:
        VMType(){

        }
        VMType(string _type_name, int _cpu_size, int _memory_size, bool _is_double_D){
            type_name =     _type_name;
            cpu_size =      _cpu_size;
            memory_size =   _memory_size;
            is_double_D =   _is_double_D;
        }
};

/*
 * 已购入的服务器对象
 * curr:            current
 * loading_VM_list: 挂载在该服务器上的VM集合，存储VM.vm_ID 
 * server_id:       the usage of server_id is...
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
       VM(string _vm_type_name, int _vm_ID, int _server_ID, int _is_in_A=-1){
           vm_type_name =    _vm_type_name;
           server_ID =  _server_ID;
           vm_ID =      _vm_ID;
           is_in_A =    _is_in_A;
       }
};

/*
 * 请求订单项
 * vm_type: 虚拟机的类型，当type=add时才用，默认为null
 * id:      待处理对象的id，如del就是删除id对应的虚拟机，add就是添加该虚拟机后的id
 */
class RequestOrder{
    public:
        string op;
        string vm_type_name;
        int id;
    public:
        RequestOrder(){}
        RequestOrder(string _op, int _id, string _vm_type_name = ""){
            op =            _op;
            vm_type_name =  _vm_type_name;
            id =            _id;
        }

};

// 因为后面购买虚拟机序列中 add 操作提供的是vmType_name参数，所以要有个的vmType_name -> VmType obj的映射
unordered_map<string, VMType> VMTypeInfo;
// 与上同理
unordered_map<string, ServerType> ServerTypeInfo;

// 现有服务器信息，由于服务器买入就不会被退货了，所以其在vector中的下标，就是其ID
vector<Server> ServerList;
// 已售出的VM对象，通过id可以直接索引得到该VM对象。
unordered_map<int, VM> VMMap;

// 截至在del之前的add订单集合
vector<RequestOrder> RequestAddList;

// 对一系列添加订单做处理，扩容、迁移、分配工作全在这里做。
void DealOrder(vector<RequestOrder> RequestAddList){
    return;
}

// 对单一删除的订单做处理, 运行该函数时要进行判断 RequestOrder 是否为空
void DealOrder(RequestOrder Requestorder){
    return;
}

// 根据输入生成服务器类型信息，有服务器名->服务器类对象的map映射
void GenerateServerType(string type_name, int cpu_size, int memory_size, int hard_cost, int power_cost){
    ServerType _server_type = ServerType(type_name, cpu_size, memory_size, hard_cost, power_cost);
    ServerTypeInfo[type_name] = _server_type;
}

void GenerateVMType(string type_name, int cpu_size, int memory_size, bool is_double_D){
    VMType _vm_type = VMType(type_name, cpu_size, memory_size, is_double_D);
    VMTypeInfo[type_name] = _vm_type;
}

int main(){
    const string file_path = "./data.txt";
    std::freopen(file_path.c_str(), "rb", stdin);

    int server_type_num;
    int vm_type_num;
    int total_day_num;
    string _tmp;

    // 输入可购入的服务器信息
    scanf("%d", &server_type_num);
    ServerType _server_type = ServerType();
    string type_name, cpu_size, memory_size, hard_cost, power_cost;
    for(int i=0; i<server_type_num; i++){
        cin >> type_name >> cpu_size >> memory_size >> hard_cost >> power_cost;
        GenerateServerType(type_name.substr(1, type_name.size()-2), 
                stoi(cpu_size.substr(0, cpu_size.size()-1)), 
                stoi(memory_size.substr(0, memory_size.size()-1)), 
                stoi(hard_cost.substr(0, hard_cost.size()-1)), 
                stoi(power_cost.substr(0, power_cost.size()-1)));
    }

    // 输入可售出的虚拟机信息
    scanf("%d", &vm_type_num);
    VMType _vm_type = VMType();
    string is_double_D;
    for(int i=0; i<vm_type_num; i++){
        cin >> type_name >> cpu_size >> memory_size >> is_double_D;
        GenerateVMType(type_name.substr(1, type_name.size()-2), 
                stoi(cpu_size.substr(0, cpu_size.size()-1)),
                stoi(memory_size.substr(0, memory_size.size()-1)), 
                stoi(is_double_D.substr(0, is_double_D.size()-1)));
    }

    scanf("%d", &total_day_num);

    // 遍历所有天
    for(int curr_day_index=0; curr_day_index<total_day_num; curr_day_index++){
        int curr_day_request_num;
        string op, vm_id; 

        scanf("%d", &curr_day_request_num);
        
        // 遍历当天所有请求
        for(int curr_request_index=0; curr_request_index<curr_day_request_num; curr_request_index++){
            cin >> op;
            op = op.substr(1, op.size()-2);
            // 订单处理顺序逻辑：由于del请求很少，所以批处理add请求(这样就可以对该add集合内部的add进行乱序分配了，便于优化), 注意dealOrder(add集合)调用的位置。
            if(op.compare("add") == 0){
                cin >> type_name >> vm_id;
                RequestOrder request_order = RequestOrder(op, 
                        stoi(vm_id.substr(0, vm_id.size()-1)), 
                        type_name.substr(0, type_name.size()-1));
                RequestAddList.push_back(request_order);
            }
            else if(op.compare("del") == 0){
                RequestOrder request_order = RequestOrder(op, 
                        stoi(vm_id.substr(0, vm_id.size()-1)));
                DealOrder(RequestAddList);
                DealOrder(request_order);
            }
        }
        DealOrder(RequestAddList);
    }



    return 0;
}
