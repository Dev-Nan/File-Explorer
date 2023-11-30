#include<iostream>
#include<stdlib.h>
#include<dirent.h>
#include<vector>
#include <termios.h>
#include <unistd.h>
#include<algorithm>
#include<sys/stat.h>
#include<sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <stack>
using namespace std;
struct termios TermChng;
void EnterAllow() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &TermChng);
}
void EnterNotAllow() {
  tcgetattr(STDIN_FILENO, &TermChng);
  atexit(EnterAllow);
  struct termios trm = TermChng;
  trm.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &trm);
}
int ret=0;
stack<string>pr;
void show(string);
void Home(void);
void Command_Mode(void);
int Find(string,string);
string GiveParent(string);
string Permission(const char*);
vector<struct dirent*> SortVec(vector<struct dirent*>);


bool ShowbyLen(pair<const char*,struct dirent*>p1,pair<const char*,struct dirent*>p2)
{  string s1,s2;
   s1=p1.first;
   s2=p2.first;
    return s1.size()<s2.size();
}
string GiveParent(string str)
{
  int len=str.size()-1;
  while(str[len]!='/')
  len--;
  if(len==0)
  return "/";
  return str.substr(0,len);
}
int main(int argc, char *argv[])
{ EnterNotAllow();
string str=".";
show(str);
return 0;
}


void show(string str)
{ struct stat st;
int it=0,k=0,m1=0,m2=0,mu=0,md=0,ml=0,mr=0;;
char key;
const char* cs;
cs=str.c_str();
DIR *drct;
drct=opendir(cs);
vector<struct dirent*>vd;
struct dirent *ent;
ent=readdir(drct);
//while(ret)
//return;
while(ent!=NULL)
{
vd.push_back(ent);
ent=readdir(drct);
}
string ot="--> ",sp="    ",perm;
int i=0;
while(1)
{

if(it==27)
m1=1;
if(it==91)
m2=1;
if(it==65)
mu=1;
if(it==66)
md=1;
if(it==68)
ml=1;
if(it==67)
mr=1;
if(m1==1&&m2==0&&mu==0&&md==0&&ml==0&&mr==0)
{ret=1;
m1=0;}
if(it==127&&str!="/")
{
  show(GiveParent(str));
}
if(m1&&m2&&md)
{k++;
if(k==vd.size())
k=vd.size()-1;
cout<<"\e[1;1H\e[2J";
m1=0;m2=0;md=0;
}
if(m1&&m2&&mu)
{k--;
if(k<0)
k=0;
m1=0;m2=0;mu=0;
cout<<"\e[1;1H\e[2J";
}

if(it==10)
{ cout<<"\e[1;1H\e[2J";
show(str+"/"+vd[k]->d_name);
}
if(!pr.empty()&&m1&&m2&&mr)
{
  show(pr.top());
  pr.pop();
  m1=0;m2=0;mr=0;
}
if(m1&&m2&&ml)
{ cout<<"\e[1;1H\e[2J";
m1=0;m2=0;ml=0;
pr.push(str);
return;}
if(it==104)
{ cout<<"\e[1;1H\e[2J";
Home();
}
if(it==58)
Command_Mode();
vd=SortVec(vd);
for(i=0;i<vd.size();i++)
{ if(i==k)
  cout<<ot;
else{cout<<sp;}
stat(vd[i]->d_name,&st);
perm=Permission(vd[i]->d_name);
struct passwd *usr = getpwuid(st.st_uid);
struct group  *grp = getgrgid(st.st_gid);
cout<<vd[i]->d_name<<"    "<<st.st_size<<"    "<<usr->pw_name<<"  "<<grp->gr_name<<"    "<<perm<<"    "<<ctime(&st.st_mtime)<<endl;
}
cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
cout<<"Normal Mode : "<<str;
key=getchar();
it=key;
}
closedir(drct);
return;
}
void Home(void )
{
cout<<"\e[1;1H\e[2J";
DIR *d;
d=opendir("/home");
struct dirent *itr;
itr=readdir(d);
string usr;
while(itr!=NULL)
{ if(itr->d_name[0]!='.')
   usr=itr->d_name;
itr=readdir(d);
}
usr=("/home/"+usr);
cout<<"\e[1;1H\e[2J";
show(usr);
return;
}


void Command_Mode()
{ cout<<endl<<endl;
cout<<"\e[1;1H\e[2J";
EnterAllow();
cout<<"Command Mode :Enter Your Command"<<" ";
int it;
char key;
key=getchar();
it=key;
if(it==27)
return;
string inp,name;
getline(cin,inp);
EnterNotAllow();
if(inp=="quit")
{
  ret=1;
  return;
}
if(inp.substr(0,4)=="goto")
{ show(inp.substr(5,inp.size()-5));
}
if(inp.substr(0,6)=="search")
{name=inp.substr(7,inp.size()-7);
cout<<endl<<endl;
cout<<name<<endl; 
cout<<"\e[1;1H\e[2J";
cout<<Find("/",name);
}

}

int Find(string path,string name)
{int ans=0;
//cout<<"mila "<<path<<endl;
const char *rpath;
rpath=path.c_str();
DIR *dr;
string cmp,perm;
dr=opendir(rpath);
struct dirent *ptr;
//if(dr==NULL)
//cout<<"haan"<<endl;

ptr=readdir(dr);
while(ptr!=NULL)
{cmp=ptr->d_name;
perm=Permission(ptr->d_name);
if(cmp[0]=='.'||(ptr->d_type==DT_DIR&&(perm[1]=='-')))
{ ptr=readdir(dr);
continue;
}
else{
if(ptr->d_type==DT_DIR&&perm[1]=='r')
{ //cout<<"bheja "<<path<<endl;
ans=max(ans,Find(path+"/"+ptr->d_name,name));
}
else{
if(ptr->d_name==name)
return 1;
}
}
ptr=readdir(dr);
}
return ans;
}
string Permission(const char *DorFi_Name)
{
   struct stat st;
    stat(DorFi_Name,&st);
string perm="";
    perm+=( (S_ISDIR(st.st_mode)) ? "d" : "-");
    perm+=( (st.st_mode & S_IRUSR) ? "r" : "-");
    perm+=( (st.st_mode & S_IWUSR) ? "w" : "-");
    perm+=( (st.st_mode & S_IXUSR) ? "x" : "-");
    perm+=( (st.st_mode & S_IRGRP) ? "r" : "-");
    perm+=( (st.st_mode & S_IWGRP) ? "w" : "-");
    perm+=( (st.st_mode & S_IXGRP) ? "x" : "-");
    perm+=( (st.st_mode & S_IROTH) ? "r" : "-");
    perm+=( (st.st_mode & S_IWOTH) ? "w" : "-");
    perm+=( (st.st_mode & S_IXOTH) ? "x" : "-");
return perm;
}
vector<struct dirent*> SortVec(vector<struct dirent*>vd)
{ struct dirent *pt;
vector<pair<const char*,struct dirent*>>Vec(vd.size());
int len;
for(len=0;len<vd.size();len++)
Vec[len]=make_pair(vd[len]->d_name,vd[len]);

sort(Vec.begin(),Vec.end(),ShowbyLen);

for(len=0;len<vd.size();len++)
vd[len]=Vec[len].second;

return vd;
}
