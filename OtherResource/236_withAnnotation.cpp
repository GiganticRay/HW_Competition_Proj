#include <iostream> 
#include <algorithm>
#include <cstring>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#define f first
#define s second
//此版本以提交为目的 流程如下
//在读入时筛选出一个cpu和nc都大于等于256的（因为虚拟机最大性能要求为两个256）-单核配置对cpu要求128的情况下 
//硬件和能耗成本尽可能低的服务器 
//然后 一个请求配置一个服务器 无视删除操作
//没了 其余都是些细节性的 
using namespace std;
int N,M;//可以采购的服务器类型数量,表示售卖的虚拟机类型数量
int T;//T天 每天有r条请求
long long kj_num,a[1010];
int fw_kj;//存开辟的服务器个数
vector <pair<int,pair<string,int>>> zsq;//存放全部的申请 本程序未使用 
			//0+1-    型号  对应编号 
unordered_map <string,int> xn_num;//存虚拟机型号对应的在给定虚拟机的列表中的下标
//unordered_map <int,int> fwid_kjid;//存服务器id对应的总空间下标
unordered_map <int,int> xnid_kjid;//存虚拟机的id对应的服务器id 本程序未使用 
unordered_map <int,int> xnid_xnxb;//存虚拟机的id对应其在列表中的下标 
unordered_map <int,int> xnid_bs;//存虚拟机的id对应其部署于哪个节点或双核部署 0A 1B 2双节点 
struct fwq{
	string na;
	int cpu,nc,yc,rc;
	int sums,jsum;//sums cpu和nc和 jsum两者差的绝对值 
	//cpu核数 内存 硬件成本 每日成本 
//重载小于号 以排出降序的cpu和nc和高 相差最小的结构体数组 
	bool operator< (const fwq& s)const{
	    if (sums!=s.sums) return sums>s.sums;
	    else if (jsum!=s.jsum) return jsum<s.jsum;
	    else return rc<s.rc;
	    return 0;
	}
}fw[109];
struct xnj{
	string nas;
	int cpus,ncs,bs;
	//cpu核数，内存大小，是否双节点部署0单1双 
}xn[1009];
//struct zj{
////子机结构体 存有服务器id 子机型号
//    int id;
//    string na;
//};
struct zkj{
	int id;
	//unordered_map <int,int> maps;
	//int mapj[1010]={0};
//虚拟机下标 及其数量 
	int cpu1,cpu2,nc1,nc2;
	int l=0;//l用于轮换 -不用了 属于废案 
}zkj[10009];
int bianli(int p,int na){
//0在当前服务器空间遍历 1在列表遍历
	if (p==1) {
		for (int i=0;i<T;i++){
			if (xn[na].bs==0){
				if (fw[i].cpu/2<xn[na].cpus||fw[i].nc/2<xn[na].ncs) continue;
				else return i;
			}
			else {
				if (fw[i].cpu/2<xn[na].cpus/2||fw[i].nc/2<xn[na].ncs/2) continue;
				else return i;
			}
		}
	}
	if (p==0) {
//		for (int i=0;i<fw_kj;i++){
//			if (xn[na].bs==0){
//				if ((zkj[i].cpu1>=xn[na].cpus&&zkj[i].nc1>=xn[na].ncs)||
//					zkj[i].cpu2>=xn[na].cpus&&zkj[i].nc2>=xn[na].ncs)	{
//					return i;
//				}
//				else continue;
//			}
//			else {
//				if ((zkj[i].cpu1>=xn[na].cpus/2&&zkj[i].cpu2>=xn[na].cpus/2)&&
//					zkj[i].nc1>=xn[na].ncs/2&&zkj[i].nc2>=xn[na].ncs/2){
//					return i;		
//				}
//				else continue;
//			}
//		}
		return -1;
	}
	return -1;
}
//void new_jia(int x,int y,int id,vector <int> &sum,vector <pair<int,int>> &bsqk){
//void new_jia(int x,int y,int id,unordered_set <int> &sum,vector <pair<int,int>> &bsqk){
void new_jia(int x,int y,int id,unordered_set <int> &sum,vector <pair<int,int>> &bsqk,int nums[]){
//x	是加入的服务器下标 y是虚拟机下标 id是虚拟机的id sum存服务器的下标 
//bsqk存子机在服务器中的部署情况 第一个数存服务器编号 第二个数存012 表示第一第二或双节点 
	zkj[fw_kj].id=fw_kj;
	//zkj[fw_kj].mapj[y]++;
	xnid_kjid[id]=fw_kj;//虚拟机id对于哪个服务器报备上 
	xnid_xnxb[id]=y;
	//printf("y=%d bs=%d ",y,xn[y].bs);
	//cout<<xn[y].nas<<endl;
	if (xn[y].bs==0){
		zkj[fw_kj].cpu1=zkj[fw_kj].cpu2=fw[x].cpu/2;
		zkj[fw_kj].nc1=zkj[fw_kj].nc2=fw[x].nc/2;
		zkj[fw_kj].cpu1-=xn[y].cpus;
		zkj[fw_kj].nc1-=xn[y].ncs;
		//sum.push_back(x);
		sum.insert(x);
		nums[x]++;//nums数组存下标为x的服务器的购买数量 
		bsqk.push_back({fw_kj,zkj[fw_kj].l});
		xnid_bs[id]=0;
		zkj[fw_kj].l=(zkj[fw_kj].l+1)%2;
	}
	else {
		zkj[fw_kj].cpu1=zkj[fw_kj].cpu2=fw[x].cpu/2-xn[y].cpus/2;
		zkj[fw_kj].nc1=zkj[fw_kj].nc2=fw[x].nc/2-xn[y].ncs/2;
		//sum.push_back(x);
		sum.insert(x);
		nums[x]++;
		bsqk.push_back({fw_kj,2});
		xnid_bs[id]=2;
	}
	fw_kj++;	 
}
//void jia(int x,int y,int id,vector <int> &sum,vector <pair<int,int>> &bsqk) {
void jia(int x,int y,int id,unordered_set <int> &sum,vector <pair<int,int>> &bsqk) {
//x是在总空间中的下标  y是虚拟机下标 id是虚拟机id 
	//zkj[x].mapj[y]++;
	xnid_kjid[id]=x;
	xnid_xnxb[id]=y;
	if (xn[y].bs==0){
		if ((min(zkj[x].cpu1,zkj[x].cpu2)>=xn[y].cpus)&&(min(zkj[x].nc1,zkj[x].nc2)>=xn[y].ncs)){
			if (zkj[x].cpu1>=zkj[x].cpu2){
				zkj[x].cpu1-=xn[y].cpus;
				zkj[x].nc1-=xn[y].ncs;
				bsqk.push_back({x,0});
				xnid_bs[id]=0;
			}
			else {
				zkj[x].cpu2-=xn[y].cpus;
				zkj[x].nc2-=xn[y].ncs;
				bsqk.push_back({x,1});
				xnid_bs[id]=1;
			}
		}
		else {
			if (zkj[x].cpu1>=xn[y].cpus&&zkj[x].nc1>=xn[y].ncs) {
				zkj[x].cpu1-=xn[y].cpus;
				zkj[x].nc1-=xn[y].ncs;
				bsqk.push_back({fw_kj,0});
				xnid_bs[id]=0;
			}
			else {
				zkj[x].cpu2-=xn[y].cpus;
				zkj[x].nc2-=xn[y].ncs;
				bsqk.push_back({fw_kj,1});
				xnid_bs[id]=1;
			}
		}
	}
	else {
		zkj[x].cpu1-=xn[y].cpus/2;
		zkj[x].cpu2-=xn[y].cpus/2;
		zkj[x].nc1-=xn[y].ncs/2;
		zkj[x].nc2-=xn[y].ncs/2;
		bsqk.push_back({x,2});
		xnid_bs[id]=2;
	}
}
void jian(int id){
//虚拟机id
//	int y=xnid_xnxb[id];//找到对应虚拟机下标 
//	int x=xnid_kjid[id];//找到对应的空间下标 
//	if (xnid_bs[id]==2){
//		zkj[x].cpu1+=xn[y].cpus/2;
//		zkj[x].cpu2+=xn[y].cpus/2;
//		zkj[x].nc1+=xn[y].ncs/2;
//		zkj[x].nc2+=xn[y].ncs/2;
//	}
//	else if (xnid_bs[id]==1){
//		zkj[x].cpu2+=xn[y].cpus;
//		zkj[x].nc2+=xn[y].ncs;
//	}
//	else if (xnid_bs[id]==0){
//		zkj[x].cpu1+=xn[y].cpus;
//		zkj[x].cpu1+=xn[y].ncs;
//	}
}
int main (){
	int i,j,k;
	scanf ("%d",&N);
	int zx=0;
	for (i=0;i<N;i++){
		char s[1010],u;
		//if (i==1) break;
		u=getchar();
		scanf ("%s",s);
		fw[i].na=(s);
		scanf (" %d, %d, %d, %d",&fw[i].cpu,&fw[i].nc,&fw[i].yc,&fw[i].rc);
		//fw[i].c1=fw[i].rc*100.0/fw[i].yc;
		if (fw[i].cpu>=256&&fw[i].nc>=256) {
			if (fw[i].rc<fw[zx].rc||fw[i].yc<fw[zx].yc) zx=i;
		}
		fw[i].sums=fw[i].cpu+fw[i].nc;
		fw[i].jsum=abs(fw[i].cpu-fw[i].nc);
		u=getchar();
	}
	//sort(fw,fw+N);
	scanf ("%d",&M);
	int max1,max2=0;
	max1=max2;
	for (i=0;i<M;i++){
		char s[1010],u;
		u=getchar();
		scanf ("%s",s);
		xn[i].nas=(s+1);
		scanf (" %d, %d, %d",&xn[i].cpus,&xn[i].ncs,&xn[i].bs);	
		xn_num[xn[i].nas]=i;
		max1=max(max1,xn[i].cpus);
		max2=max(max2,xn[i].ncs);
		//max12找出虚拟器对配置的最高要求 某一节点核数和内存数都要有128 
		u=getchar();
		//cout<<xn[i].nas<<' '<<endl;
	}
	scanf("%d",&T) ;
	int ss=0;
	//cout<<"ssT="<<T<<endl;
	//int a[19];
	int num=0;
	char u;
	for (int cs=0;cs<T;cs++){
	    int r;
	    scanf ("%d",&a[cs]);//a数组存第i+1天的最后一条申请编号+1（因为从0开始编号） 
	    u=getchar();
	    num=0;
	    vector <pair <int,int>> sqs;//申请s 数组 存一个虚拟机的对应的服务器编号和其id 
	    for (i=0;i<a[cs];i++){
	        string a1,a2,a3;
	        int ss;
	        cin>>a1;
	        if (a1=="(add,"){
	            cin>>a2;
	            scanf ("%d",&ss);
	            zsq.push_back({0,{a2,ss}});
	            num++;//num存一天申请的服务器个数  
	            sqs.push_back({fw_kj,xn_num[a2]});
				fw_kj++;//fw_kj总服务器个数 也是最后一个申请的服务器的编号 
	        }
	        else {
	            scanf ("%d",&ss);
	            zsq.push_back({1,{"0",ss}});
	        }
	        u=getchar();
	    }
	    if (num>0) {
			printf ("(purchase, 1)\n");
			cout<<fw[zx].na;
			printf (" %d)\n",num);
		}
	    else printf ("(purchase, 0)\n");
	    printf ("(migration, 0)\n");//本程序未涉及迁移操作 所以固定输出此语句 
	    for (auto za:sqs){
			if (xn[za.s].bs==0){
				printf ("(%d, A)\n",za.f);
			}
			else printf ("(%d)\n",za.f);
		}
	    if (cs>=1) a[cs]+=a[cs-1];
	}
//以下为废案 基本可以无视 废除原因 不明的格式错误 复杂度过高导致超时	
//	cout<<fw[0].cpu<<' '<<fw[0].nc<<endl;
//	cout<<max1<<' '<<max2; 
//				for (i=0;i<N;i++){
//					cout<<i<<'c'<<fw[i].na;
//					printf ("%d %d %d %d\n",fw[i].cpu,fw[i].nc,fw[i].yc,fw[i].rc);		
//				}
//				for (i=0;i<M;i++){
//					cout<<i<<' '<<xn[i].nas;
//					printf ("%d %d %d\n",xn[i].cpus,xn[i].ncs,xn[i].bs);
//				}
	//cout<<zsq.size();
///*---------------------------------------至此 完成读入 -----------------------------------*/
//	for(i=0;i<T;i++){
//	    if (i==0) j=0;
//	    //vector <int> sum;
//	    unordered_set <int> sum;
//		vector <pair<int,int>> bsqk;
//		int nums[100009]={0};
//		int gs=0;
//		//cout<<"从i开始"<<i<<endl;
//	    for(j;j<a[i];j++){
//	        
//	        
//	       // if (zsq[j].f==0){
//	       //     cout<<zsq[j].f<<" "<<zsq[j].s.f<<zsq[j].s.s<<endl;;
//	       //     cout<<xn[xn_num[zsq[j].s.f]+1].nas<<' '<<xn[xn_num[zsq[j].s.f]+1].bs<<endl;
//	       // }
//	       // //0+1-
//	        if (zsq[j].f==0){
////对于申请是加入的情况	        
//					gs++;
//					fw_kj++;
////				if (fw_kj==0){//若当前为空 
////					int x=bianli(1,xn_num[zsq[j].s.f]);					
////					//new_jia(x,xn_num[zsq[j].s.f],zsq[j].s.s,sum,bsqk);
////					new_jia(x,xn_num[zsq[j].s.f],zsq[j].s.s,sum,bsqk,nums);
////				}
////				else {//有运行服务器 
////					int x=bianli(0,xn_num[zsq[j].s.f]);
////					if (x==-1) {
////						x=bianli(1,xn_num[zsq[j].s.f]);
////						//new_jia(x,xn_num[zsq[j].s.f],zsq[j].s.s,sum,bsqk);
////						new_jia(x,xn_num[zsq[j].s.f],zsq[j].s.s,sum,bsqk,nums);
////					}
////					else {//有可用的运行服务器 
////						jia(x,xn_num[zsq[j].s.f],zsq[j].s.s,sum,bsqk);
////					}
////				}
//			}
//			else {
//				//jian(zsq[j].s.s);
//			}
//	    }
//	    //printf ("(purchase, %d)\n",sum.size());
//	    printf ("(purchase, 1)\n");
////	    if (sum.size()!=0){
////			for (auto z:sum){
////			//for (int i=sum.begin();i<sum.end();i++){
////				//printf ("")
////				//cout<<z;
////				cout<<fw[z].na;
////				//printf (" %d)\n",sum.count(z));
////				printf (" %d)\n",nums[z]);
////			}
////		}
////		printf ("(migration, 0)\n");
////		for (auto ss:bsqk){
////			//printf ("(%d,%d)\n")
////			if (ss.s!=2){
////				printf ("(%d, %c)\n",ss.f,'A'+ss.s);
////			}
////			else printf ("(%d)\n",ss.f);
////		}
	//}
	return 0;
}
