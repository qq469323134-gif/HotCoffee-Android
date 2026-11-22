// =====================================================================
//  热咖啡 HotCoffee —— 原生 ASI（给 GTA V 启动器的 ASI 解释器用）
//
//  原版 scripts/HotCoffeeJK.dll 是 ScriptHookVDotNet 的 C# 脚本，
//  这个启动器不跑 .NET 脚本，只解释 .asi；所以这里按原版逻辑
//  重写成一个原生 ASI：
//    · 地图上的「热咖啡」服务点（粉色标记）
//    · 服务点附近会站着可搭讪的街头女郎，地图上同样有标记
//    · 走近按 E 搭讪，菜单选「交谈 / 性爱 / 姿势」
//    · 开车停在女郎旁边按 E，可以让她上车
//    · 全部提示、菜单、标记名都是简体中文
//
//  日志写到 ASI 同目录 HotCoffee.log，方便排查。
// =====================================================================

typedef unsigned long long u64;
typedef long long s64;
typedef unsigned int u32;
typedef int s32;
typedef unsigned char u8;
typedef void* HINST;
typedef void* HANDLE;

extern "C" {
extern "C" void* __stdcall CreateFileA(const char*, u32, u32, void*, u32, u32, HANDLE);
extern "C" int __stdcall WriteFile(HANDLE, const void*, u32, u32*, void*);
extern "C" int __stdcall ReadFile(HANDLE, void*, u32, u32*, void*);
extern "C" int __stdcall CloseHandle(HANDLE);
extern "C" int __stdcall SetFilePointerEx(HANDLE, long long, long long*, u32);
extern "C" u32 __stdcall GetModuleFileNameA(void*, char*, u32);
extern "C" short __stdcall GetAsyncKeyState(int);

void nativeInit(u64) __asm__("\"?nativeInit@@YAX_K@Z\"");
u64 nativeCallRaw(void) __asm__("\"?nativeCall@@YAPEA_KXZ\"");
void nativePush64(u64) __asm__("\"?nativePush64@@YAX_K@Z\"");
void scriptRegister(HINST, void (*)(void)) __asm__("\"?scriptRegister@@YAXPEAUHINSTANCE__@@P6AXXZ@Z\"");
void scriptWait(unsigned long) __asm__("\"?scriptWait@@YAXK@Z\"");
}

extern "C" void* memcpy(void* d, const void* s, u64 n){ u8* a=(u8*)d; const u8* b=(const u8*)s; while(n--) *a++=*b++; return d; }
extern "C" void* memset(void* d, int c, u64 n){ u8* a=(u8*)d; while(n--) *a++=(u8)c; return d; }

// ---------------- native 哈希 ----------------
#define N_PLAYER_PED_ID            0xD80958FC74E988A6ull
#define N_GET_PLAYER_PED           0x43A66C31C68491C0ull
#define N_IS_PED_IN_ANY_VEHICLE    0x997ABD671D25CA0Bull
#define N_GET_VEHICLE_PED_IS_USING 0x6094AD011A2EA87Dull
#define N_IS_VEHICLE_SEAT_FREE     0x22AC59A870E6A669ull
#define N_GET_VEHICLE_CLASS        0x29439776AAA00A62ull
#define N_GET_ENTITY_COORDS        0x3FEF770D40960D5Aull
#define N_GET_ENTITY_MODEL         0x9F47B058362C84B5ull
#define N_PLAYER_ID                0x4F5F0E2F0EEBC7E0ull
#define N_SET_PLAYER_MODEL         0x00A1CADD00108836ull
#define N_IS_ENTITY_AT_COORD       0x20B60995556D004Full
#define N_IS_ENTITY_AT_ENTITY      0x751B70C3D034E187ull
#define N_GET_ENTITY_SPEED         0xD5037BA82E12416Full
#define N_GET_PED_BONE_COORDS      0x17C07FC640E86B4Eull
#define N_GET_PED_DRAWABLE         0x67F3780DD425D4FCull
#define N_GET_PED_TEXTURE          0x04A355E041E004E6ull
#define N_SET_PED_COMPONENT        0x262B14F48D29DE80ull
#define N_GET_PED_DRAWABLE_COUNT   0x27561561732A7842ull
#define N_DELETE_PED               0x9614299DCB53E54Bull
#define N_PLACE_ON_GROUND          0x58A850EAEE20FAA3ull
#define N_PED_AMBIENT_ANIMS        0x6373D1349925A70Eull
#define N_PED_GESTURE_ANIMS        0xBAF20C5432058024ull
#define N_GET_ENTITY_HEADING       0xE83D4F9BA2A38914ull
#define N_GET_ENTITY_FORWARD       0x0A794A5A57F8DF91ull
#define N_IS_PED_DEAD_OR_DYING     0x3317DEDB88C95038ull
#define N_IS_PED_MALE              0x6D9F5FAA7488BA46ull
#define N_GET_PED_NEARBY_PEDS      0x23F8F5FC7E8C4A6Bull
#define N_DOES_ENTITY_EXIST        0x7239B21A38F536BAull
#define N_IS_ENTITY_A_PED          0x524AC5ECEA15343Eull
#define N_SET_ENTITY_AS_MISSION    0xAD738C3085FE7E11ull
#define N_SET_ENTITY_AS_NO_LONGER  0xB736A491E64A32CFull
#define N_SET_BLOCKING_NON_TEMP    0x9F8AA94D6D97DBF4ull
#define N_TASK_TURN_TO_FACE        0x5AD23D40115353ACull
#define N_TASK_GO_TO_ENTITY        0x6A071245EB0D1882ull
#define N_TASK_ENTER_VEHICLE       0xC20E50AA46D09CA8ull
#define N_TASK_LEAVE_VEHICLE       0xD3DBCE61A490BE02ull
#define N_CLEAR_PED_TASKS          0xE1EF3C1216AFF2CDull
#define N_CLEAR_PED_TASKS_IMM      0xAAA34F8A7CB32098ull
#define N_IS_PED_IN_VEHICLE        0xA3EE4A07279BB9DBull
#define N_SET_ENTITY_NO_COLLISION  0xA53ED5520C07654Aull
#define N_SET_ENTITY_HEADING       0x8E2530AA8ADA980Eull
#define N_SET_ENTITY_COORDS        0x06843DA7060A026Bull
#define N_SET_ENTITY_COORDS_NO_OFF 0x239A3351AC1DA385ull
#define N_FREEZE_ENTITY_POSITION   0x428CA6DBD1094446ull
#define N_GET_ENTITY_HEALTH        0xEEF059FAD016D209ull
#define N_GET_ENTITY_MAX_HEALTH    0x15D757606D170C3Cull
#define N_SET_ENTITY_HEALTH        0x6B76DC1F3AE6E6A3ull
#define N_PLAY_PED_SPEECH          0x8E04FEDD28D42462ull
#define N_STOP_PED_SPEECH          0xB8BEC0CA6F0EDB0Full
#define N_IS_SPEECH_PLAYING        0x9072C8B49907BFADull
#define N_SET_AUDIO_FLAG           0xB9EFD5C25018725Aull
#define N_SET_VOICE_NAME           0x6C8065A3B780185Bull
#define N_REQUEST_ANIM_DICT        0xD3BD40951412FEF6ull
#define N_HAS_ANIM_DICT            0xD031A9162D01088Cull
#define N_TASK_PLAY_ANIM           0xEA47FE3719165B94ull
#define N_SET_ENTITY_ANIM_SPEED    0x28D1A16553C51776ull
#define N_GET_ENTITY_ANIM_TIME     0x346D81500D088F42ull
#define N_IS_ENTITY_PLAYING_ANIM   0x1F0B79228E461EC9ull
#define N_SET_ENTITY_ANIM_TIME     0x4487C259F0F70977ull
#define N_ADD_BLIP_FOR_COORD       0x5A039BB0BCA604B6ull
#define N_ADD_BLIP_FOR_ENTITY      0x5CDE92C702A8FCE7ull
#define N_SET_BLIP_SPRITE          0xDF735600A4696DAFull
#define N_SET_BLIP_COLOUR          0x03D7FB09E75D6B7Eull
#define N_SET_BLIP_SCALE           0xD38744167B2FA257ull
#define N_SET_BLIP_SHORT_RANGE     0xBE8BE4FE60E27B72ull
#define N_SET_BLIP_FLASHES         0xB14552383D39CE3Eull
#define N_REMOVE_BLIP              0x86A652570E5F25DDull
#define N_CLEAR_ALL_HELP           0x6178F68A87A4D3A0ull
#define N_BLIP_NAME_BEGIN          0xF9113A30DE5C6670ull
#define N_BLIP_NAME_END            0xBC38B49BCB83BC9Bull
#define N_REQUEST_MODEL            0x963D27A58DF860ACull
#define N_HAS_MODEL                0x98A4EB5D89A0C952ull
#define N_SET_MODEL_NO_LONGER      0xE532F5D78798DAABull
#define N_CREATE_PED               0xD49F9B0955C367DEull
#define N_PED_DEFAULT_VARIATION    0x45EEE61580806D63ull
#define N_TASK_SCENARIO_IN_PLACE   0x142A02425FF02BD9ull
#define N_GET_GAME_TIMER           0x9CD27B0045628463ull
#define N_IS_CONTROL_JUST_PRESSED  0x580417101DDB492Full
#define N_IS_DISABLED_CONTROL_JUST 0x91AEF906BCA88877ull
#define N_DISABLE_CONTROL_ACTION   0xFE99B66D079CF6BCull
#define N_DRAW_RECT                0x3A618A217E5154F0ull
#define N_SET_TEXT_FONT            0x66E0276CC5F6B9DAull
#define N_SET_TEXT_SCALE           0x07C837F9A01C34C9ull
#define N_SET_TEXT_COLOUR          0xBE6B23FFA53FB442ull
#define N_SET_TEXT_CENTRE          0xC02F4DBFB51D988Bull
#define N_SET_TEXT_WRAP            0x63145D9C883A1A70ull
#define N_SET_TEXT_DROP_SHADOW     0x1CA3E9EAC9D93E5Eull
#define N_BEGIN_TEXT_CMD           0x25FBB336DF1804CBull
#define N_END_TEXT_CMD             0xCD015E5BB0D96A57ull
#define N_BEGIN_HELP               0x8509B634FBE7DA11ull
#define N_ADD_TEXT_COMPONENT       0x6C188BE134E074AAull
#define N_END_HELP                 0x238FFE5C7B0498A6ull
#define N_GET_GROUND_Z             0xC906A7DAB05C8D2Bull
#define N_GET_HASH_KEY             0xD24D37CC275948CCull

static void pI(s64 v){ nativePush64((u64)v); }
static void pF(float f){ u32 b; memcpy(&b,&f,4); nativePush64((u64)b); }
static void pP(const void* p){ nativePush64((u64)(unsigned long long)p); }
static u64 rv(void){ u64 p=nativeCallRaw(); if(!p) return 0; return *(const u64*)(unsigned long long)p; }
static float rvF(void){ u64 p=nativeCallRaw(); if(!p) return 0.0f; return *(const float*)(unsigned long long)p; }
static void rvV3(float* out){ u64 p=nativeCallRaw(); if(!p){ out[0]=0; out[1]=0; out[2]=0; return; }
  // 这个解释器把返回值按 8 字节一个槽存放：x=[0], y=[2], z=[4]
  const float* f=(const float*)(unsigned long long)p; out[0]=f[0]; out[1]=f[2]; out[2]=f[4]; }

// ---------------- 日志 ----------------
static char g_dir[260];
static char g_logPath[300];
static int g_pathMode=-1;
static u32 slen(const char* s){ u32 n=0; while(s[n]) n++; return n; }
static char* app(char* p,const char* s){ while(*s) *p++=*s++; return p; }
static char* appnum(char* p,s64 v){
  char t[24]; int n=0; int neg=v<0; u64 u=neg?(u64)(-v):(u64)v;
  if(!u) t[n++]='0';
  while(u){ t[n++]=(char)('0'+u%10); u/=10; }
  if(neg) *p++='-';
  while(n) *p++=t[--n];
  return p;
}
static HANDLE openLog(void){
  if(g_pathMode>=0){
    HANDLE h=CreateFileA(g_logPath,0x40000000u,0x00000001u,0,4,0x80,0);
    if(h!=(HANDLE)-1) return h;
  }
  char cand[300];
  for(int mode=0;mode<3;mode++){
    u32 i=0;
    if(mode==0){
      for(;g_dir[i] && i<230;i++) cand[i]=g_dir[i];
      char* p=app(cand+i,"HotCoffee.log"); *p=0;
    } else if(mode==1){
      char* p=app(cand,"scripts\\HotCoffee.log"); *p=0;
    } else {
      char* p=app(cand,"HotCoffee.log"); *p=0;
    }
    HANDLE h=CreateFileA(cand,0x40000000u,0x00000001u,0,4,0x80,0);
    if(h!=(HANDLE)-1){
      u32 k=0; for(;cand[k] && k<290;k++) g_logPath[k]=cand[k]; g_logPath[k]=0;
      g_pathMode=mode; return h;
    }
  }
  return (HANDLE)-1;
}
static void logmsg(const char* a, s64 num, const char* b){
  HANDLE h=openLog();
  if(h==(HANDLE)-1) return;
  s64 zero=0;
  SetFilePointerEx(h,(long long)0,&zero,2);
  char line[460]; char* p=line;
  p=app(p,"[HC] "); p=app(p,a);
  if(num!=0x7fffffffffffffffLL) p=appnum(p,num);
  p=app(p,b); *p=0;
  u32 w=0; WriteFile(h,line,slen(line),&w,0); WriteFile(h,"\r\n",2,&w,0);
  CloseHandle(h);
}
static void logS(const char* s){ logmsg(s,0x7fffffffffffffffLL,""); }

// ---------------- 基础 native 包装 ----------------
static u32 gameTimer(void){ nativeInit(N_GET_GAME_TIMER); return (u32)rv(); }
static int playerPed(void){ nativeInit(N_PLAYER_PED_ID); return (int)rv(); }
static int pedInAnyVehicle(int ped){ nativeInit(N_IS_PED_IN_ANY_VEHICLE); pI(ped); pI(0); return (int)rv(); }
static int vehicleOfPed(int ped){ nativeInit(N_GET_VEHICLE_PED_IS_USING); pI(ped); return (int)rv(); }
static int isSeatFree(int veh,int seat){ nativeInit(N_IS_VEHICLE_SEAT_FREE); pI(veh); pI(seat); pI(0); return (int)rv(); }
static int vehicleClass(int veh){ nativeInit(N_GET_VEHICLE_CLASS); pI(veh); return (int)rv(); }
static int isPedDead(int ped){ nativeInit(N_IS_PED_DEAD_OR_DYING); pI(ped); pI(0); return (int)rv(); }
static int isPedMale(int ped){ nativeInit(N_IS_PED_MALE); pI(ped); return (int)rv(); }
static int exists(int e){ nativeInit(N_DOES_ENTITY_EXIST); pI(e); return (int)rv(); }
static int isPed(int e){ nativeInit(N_IS_ENTITY_A_PED); pI(e); return (int)rv(); }
static int pedInVehicle(int ped,int veh){ nativeInit(N_IS_PED_IN_VEHICLE); pI(ped); pI(veh); pI(0); return (int)rv(); }

static void coords(int e,float* out){
  nativeInit(N_GET_ENTITY_COORDS); pI(e); pI(1); rvV3(out);
}
static float heading(int e){ nativeInit(N_GET_ENTITY_HEADING); pI(e); return rvF(); }
static void forward(int e,float* out){
  nativeInit(N_GET_ENTITY_FORWARD); pI(e); rvV3(out);
}
static float distTo(int e,float x,float y,float z){
  float p[3]; coords(e,p);
  float dx=p[0]-x, dy=p[1]-y, dz=p[2]-z;
  return dx*dx+dy*dy+dz*dz;
}
static float distBetween(int a,int b){
  float p[3],q[3]; coords(a,p); coords(b,q);
  float dx=p[0]-q[0], dy=p[1]-q[1], dz=p[2]-q[2];
  return dx*dx+dy*dy+dz*dz;
}
static int entAtCoord(int e,float x,float y,float z,float sx,float sy,float sz){
  nativeInit(N_IS_ENTITY_AT_COORD);
  pI(e); pF(x); pF(y); pF(z); pF(sx); pF(sy); pF(sz); pI(0); pI(1); pI(0);
  return (int)rv();
}
static int entAtEnt(int a,int b,float sx,float sy,float sz){
  nativeInit(N_IS_ENTITY_AT_ENTITY);
  pI(a); pI(b); pF(sx); pF(sy); pF(sz); pI(0); pI(1); pI(0);
  return (int)rv();
}
static float entSpeed(int e){ nativeInit(N_GET_ENTITY_SPEED); pI(e); return rvF(); }
static void setHeading(int e,float h){ nativeInit(N_SET_ENTITY_HEADING); pI(e); pF(h); rv(); }
static void setCoords(int e,float x,float y,float z){
  nativeInit(N_SET_ENTITY_COORDS_NO_OFF); pI(e); pF(x); pF(y); pF(z); pI(0); pI(0); pI(0); rv();
}
static void freezePos(int e,int on){ nativeInit(N_FREEZE_ENTITY_POSITION); pI(e); pI(on); rv(); }
static void noCollision(int a,int b,int thisFrame){
  nativeInit(N_SET_ENTITY_NO_COLLISION); pI(a); pI(b); pI(thisFrame); rv();
}
static u64 joaatStr(const char* s){
  nativeInit(N_GET_HASH_KEY); pP(s); return rv();
}
static float groundZ(float x,float y,float z){
  float gz=0.0f;
  nativeInit(N_GET_GROUND_Z); pF(x); pF(y); pF(z); pP(&gz); pI(0); pI(0); rv();
  return gz;
}

// ---------------- 文字 / HUD ----------------
static void clearHelp(void){ nativeInit(N_CLEAR_ALL_HELP); rv(); }
static void helpText(const char* s){
  nativeInit(N_BEGIN_HELP); pP("STRING"); rv();
  nativeInit(N_ADD_TEXT_COMPONENT); pP(s); rv();
  nativeInit(N_END_HELP); pI(0); pI(1); pI(1); pI(-1); rv();
}
static void drawRect(float cx,float cy,float w,float h,int r,int g,int b,int a){
  nativeInit(N_DRAW_RECT); pF(cx); pF(cy); pF(w); pF(h);
  pI(r); pI(g); pI(b); pI(a); pI(0); rv();
}
static void drawText(const char* s,float x,float y,float scale,int r,int g,int b,int a,int centre){
  nativeInit(N_SET_TEXT_FONT); pI(0); rv();
  nativeInit(N_SET_TEXT_SCALE); pF(0.0f); pF(scale); rv();
  nativeInit(N_SET_TEXT_COLOUR); pI(r); pI(g); pI(b); pI(a); rv();
  nativeInit(N_SET_TEXT_CENTRE); pI(centre); rv();
  nativeInit(N_SET_TEXT_DROP_SHADOW); rv();
  nativeInit(N_BEGIN_TEXT_CMD); pP("STRING"); rv();
  nativeInit(N_ADD_TEXT_COMPONENT); pP(s); rv();
  nativeInit(N_END_TEXT_CMD); pF(x); pF(y); pI(0); rv();
}

// ---------------- 声音 ----------------
static void speech(int ped,const char* name,int force){
  if(!ped) return;
  if(force){
    nativeInit(N_STOP_PED_SPEECH); pI(ped); rv();
  }
  nativeInit(N_SET_AUDIO_FLAG); pP("IsDirectorModeActive"); pI(1); rv();
  nativeInit(N_PLAY_PED_SPEECH); pI(ped); pP(name); pP("SPEECH_PARAMS_FORCE"); pI(0); rv();
  nativeInit(N_SET_AUDIO_FLAG); pP("IsDirectorModeActive"); pI(0); rv();
}
static int speechPlaying(int ped){
  if(!ped) return 0;
  nativeInit(N_IS_SPEECH_PLAYING); pI(ped); return (int)rv();
}

// ---------------- 让路人停下 ----------------
static void stopPed(int ped){
  if(!ped) return;
  nativeInit(N_CLEAR_PED_TASKS); pI(ped); rv();
  nativeInit(N_SET_BLOCKING_NON_TEMP); pI(ped); pI(1); rv();
  nativeInit(N_PED_AMBIENT_ANIMS); pI(ped); pI(0); rv();
  nativeInit(N_PED_GESTURE_ANIMS); pI(ped); pI(0); rv();
}
static void restorePed(int ped){
  if(!ped) return;
  nativeInit(N_PED_AMBIENT_ANIMS); pI(ped); pI(1); rv();
  nativeInit(N_PED_GESTURE_ANIMS); pI(ped); pI(1); rv();
}

// ---------------- 脱衣 / 穿衣 ----------------
#define NCOMP 5
static const int COMP_LIST[NCOMP]={3,4,6,8,11};   // 躯干/裤子/鞋/内衬/上衣
struct Outfit { int comp[NCOMP]; int draw[NCOMP]; int tex[NCOMP]; int valid; };
static Outfit g_outP={ {0},{0},{0},0 };
static Outfit g_outG={ {0},{0},{0},0 };

static void undress(int ped,Outfit* o){
  o->valid=0;
  if(!ped || !exists(ped)) return;
  for(int i=0;i<NCOMP;i++){
    int c=COMP_LIST[i];
    o->comp[i]=c;
    nativeInit(N_GET_PED_DRAWABLE); pI(ped); pI(c); o->draw[i]=(int)rv();
    nativeInit(N_GET_PED_TEXTURE);  pI(ped); pI(c); o->tex[i]=(int)rv();
    nativeInit(N_GET_PED_DRAWABLE_COUNT); pI(ped); pI(c); int cnt=(int)rv();
    if(cnt<=0) continue;
    nativeInit(N_SET_PED_COMPONENT); pI(ped); pI(c); pI(0); pI(0); pI(0); rv();
  }
  o->valid=1;
}
static void redress(int ped,Outfit* o){
  if(!o->valid) return;
  if(ped && exists(ped)){
    for(int i=0;i<NCOMP;i++){
      nativeInit(N_SET_PED_COMPONENT); pI(ped); pI(o->comp[i]); pI(o->draw[i]); pI(o->tex[i]); pI(0); rv();
    }
  }
  o->valid=0;
}

static void groundEntity(int e){
  if(!e) return;
  nativeInit(N_PLACE_ON_GROUND); pI(e); rv();
}

static int spawnModelAt(const char* model,float x,float y,float z,float hdg){
  u64 mh=joaatStr(model);
  nativeInit(N_REQUEST_MODEL); pI((s64)mh); rv();
  int loaded=0;
  for(int i=0;i<80;i++){
    nativeInit(N_HAS_MODEL); pI((s64)mh); if(rv()){ loaded=1; break; }
    scriptWait(0);
  }
  if(!loaded) return 0;
  nativeInit(N_CREATE_PED); pI(4); pI((s64)mh);
  pF(x); pF(y); pF(z); pF(hdg); pI(0); pI(0);
  int ped=(int)rv();
  nativeInit(N_SET_MODEL_NO_LONGER); pI((s64)mh); rv();
  return ped;
}

// ---------------- 标记 ----------------
static int addBlipName(int blip,const char* name){
  if(!blip) return blip;
  nativeInit(N_BLIP_NAME_BEGIN); pP("STRING"); rv();
  nativeInit(N_ADD_TEXT_COMPONENT); pP(name); rv();
  nativeInit(N_BLIP_NAME_END); pI(blip); rv();
  return blip;
}
static int makeBlip(float x,float y,float z,const char* name,int sprite,int colour,float scale){
  nativeInit(N_ADD_BLIP_FOR_COORD); pF(x); pF(y); pF(z); int b=(int)rv();
  if(!b) return 0;
  nativeInit(N_SET_BLIP_SPRITE); pI(b); pI(sprite); rv();
  nativeInit(N_SET_BLIP_COLOUR); pI(b); pI(colour); rv();
  nativeInit(N_SET_BLIP_SCALE); pI(b); pF(scale); rv();
  nativeInit(N_SET_BLIP_SHORT_RANGE); pI(b); pI(0); rv();
  addBlipName(b,name);
  return b;
}
static int makeEntityBlip(int e,const char* name,int sprite,int colour,float scale){
  nativeInit(N_ADD_BLIP_FOR_ENTITY); pI(e); int b=(int)rv();
  if(!b) return 0;
  nativeInit(N_SET_BLIP_SPRITE); pI(b); pI(sprite); rv();
  nativeInit(N_SET_BLIP_COLOUR); pI(b); pI(colour); rv();
  nativeInit(N_SET_BLIP_SCALE); pI(b); pF(scale); rv();
  nativeInit(N_SET_BLIP_SHORT_RANGE); pI(b); pI(0); rv();
  addBlipName(b,name);
  return b;
}
static void removeBlip(int b){ if(!b) return; int v=b; nativeInit(N_REMOVE_BLIP); pP(&v); rv(); }

// ---------------- 数据 ----------------
#define NSPOT 6
struct SpotDef { const char* name; float x,y,z; };
static SpotDef g_spots[NSPOT]={
  {"热咖啡·独角兽俱乐部",-1388.0f,-585.0f,30.2f},
  {"热咖啡·军团广场",      215.0f,-810.0f,30.7f},
  {"热咖啡·维斯普奇海滩",-1220.0f,-1500.0f,4.0f},
  {"热咖啡·德尔佩罗码头",-1850.0f,-1250.0f,8.0f},
  {"热咖啡·桑迪海岸",     1961.0f,3740.0f,32.0f},
  {"热咖啡·佩立托湾",     -380.0f,6045.0f,31.0f},
};
static int   g_spotBlip[NSPOT];
static int   g_hooker[NSPOT];
static int   g_hookerBlip[NSPOT];
static u32   g_respawnAt[NSPOT];

// 人物名单从 HotCoffee_Peds.ini 读取。容量可扩展，默认最多 64 项。
// 配置格式：中文显示名|英文模型 spawn name；模型必须已安装并可流式加载。
#define MODEL_CAP 128
static char g_modelNameBuf[MODEL_CAP][64];
static char g_modelLabelBuf[MODEL_CAP][128];
static const char* g_modelNames[MODEL_CAP];
static const char* g_modelDisplay[MODEL_CAP];
static int g_modelCount=0;
static void copyText(char* dst,const char* src,int cap){ int i=0; if(cap<=0)return; while(src && src[i] && i<cap-1){dst[i]=src[i];i++;} dst[i]=0; }
static void addModel(const char* label,const char* model){
  if(g_modelCount>=MODEL_CAP || !label || !model || !model[0]) return;
  for(int i=0;i<g_modelCount;i++){ int same=1,j=0; while(model[j] && g_modelNameBuf[i][j] && model[j]==g_modelNameBuf[i][j])j++; if(model[j]==0 && g_modelNameBuf[i][j]==0) same=1; else same=0; if(same)return; }
  int n=g_modelCount++;
  copyText(g_modelNameBuf[n],model,64); g_modelNames[n]=g_modelNameBuf[n];
  char* p=g_modelLabelBuf[n]; int i=0; while(label[i] && i<70){p[i]=label[i];i++;}
  if(i<126){p[i++]=' ';p[i++]='(';}
  int j=0; while(model[j] && i<126) p[i++]=model[j++];
  if(i<127)p[i++]=')'; p[i]=0; g_modelDisplay[n]=g_modelLabelBuf[n];
}
static void loadModelConfig(void){
  g_modelCount=0;
  const char* paths[3]={"Mods\\scripthook\\scripts\\HotCoffee_Peds.ini","scripts\\HotCoffee_Peds.ini","HotCoffee_Peds.ini"};
  HANDLE h=(HANDLE)-1;
  for(int i=0;i<3;i++){ h=CreateFileA(paths[i],0x80000000u,1,0,3,0x80,0); if(h!=(HANDLE)-1)break; }
  if(h!=(HANDLE)-1){
    char data[32768]; u32 got=0; ReadFile(h,data,32767,&got,0); CloseHandle(h); data[got]=0;
    char line[512]; int li=0;
    for(u32 i=0;i<=got;i++){
      char c=data[i];
      if(c=='\r') continue;
      if(c=='\n' || c==0){
        line[li]=0; li=0;
        if(line[0] && line[0]!='#'){
          char* sep=0; for(int k=0;line[k];k++){ if(line[k]=='|'){sep=&line[k];break;} }
          if(sep){ *sep=0; char* label=line; char* model=sep+1; while(*label==' '||*label=='\t')label++; while(*model==' '||*model=='\t')model++;
            int a=0; while(label[a])a++; while(a>0&&(label[a-1]==' '||label[a-1]=='\t'))label[--a]=0;
            int b=0; while(model[b])b++; while(b>0&&(model[b-1]==' '||model[b-1]=='\t'))model[--b]=0;
            if(label[0]&&model[0]) addModel(label,model);
          }
        }
        if(c==0)break;
      } else if(li<510){ line[li++]=c; }
    }
  }
  if(g_modelCount==0){
    addModel("街头女郎 01","s_f_y_hooker_01");
    addModel("街头女郎 02","s_f_y_hooker_02");
    addModel("街头女郎 03","s_f_y_hooker_03");
    addModel("比佛利女性","a_f_y_bevhills_01");
    addModel("游客女性","a_f_y_tourist_01");
    addModel("商务女性","a_f_y_business_01");
    addModel("张蓓蓓","zhangbeibei");
  }
  logmsg("model config loaded count=",g_modelCount,"");
}
static const char* g_hookerModels[6]={
  "s_f_y_hooker_01","s_f_y_hooker_02","s_f_y_hooker_03",
  "a_f_y_bevhills_01","a_f_y_tourist_01","a_f_y_business_01"
};
static const char* g_hookerVoices[8]={
  "s_f_y_hooker_01_white_full_01","s_f_y_hooker_01_white_full_02","s_f_y_hooker_01_white_full_03",
  "s_f_y_hooker_02_white_full_01","s_f_y_hooker_02_white_full_02","s_f_y_hooker_02_white_full_03",
  "s_f_y_hooker_03_black_full_01","s_f_y_hooker_03_black_full_03"
};

// ---------------- 状态 ----------------
enum {
  S_IDLE=0, S_GREET, S_MENU, S_POSE, S_SEX, S_ENTER, S_CAR_MENU, S_CAR_SEX
};
static int   g_state=S_IDLE;
static int   g_ped=0;
static int   g_modelSel=0;
static int   g_modelMenuOpen=0;
static int   g_keyF2=0;
static int   g_keyF2Prev=0;
static int   g_veh=0;
static int   g_sel=0;
static int   g_sex=0;
static float g_speed=1.0f;
static u32   g_lock=0;
static u32   g_stT=0;
static int   g_chat=0;
static int   g_enterTask=0;
static int   g_blipsDone=0;
static int   g_nearPed=0;
static int   g_cand=0;
static int   g_candAt=0;
static int   g_scanAt=0;
static int   g_spotAt=0;
static int   g_summonSpot=-1;
static int   g_sessSpot=-1;
static int   g_diagAt=0;
static float g_stayX=0.0f, g_stayY=0.0f, g_stayZ=0.0f, g_stayH=0.0f;
static int   g_helpOn=0;
static float g_hOff=0.0f;
static const char* g_d1=0; static const char* g_a1=0;
static const char* g_d2=0; static const char* g_a2=0;
static u32   g_animChk=0;
static u32   g_lastPromptT=0;
static u32   g_promptLock=0;
static int   g_groundFix=0;
static u32   g_sexT=0;
static float g_playerZ0=0.0f;

static const char* MENU_MAIN[3]={"交谈","性爱","离开"};
static const char* MENU_POSE[5]={"口 (站立)","性 (站立)","口 (坐着)","性 (坐着)","性 (躺下)"};
static const char* MENU_CAR[3]={"吹箫","啪啪啪","拒绝服务"};

// 将当前互动对象替换为菜单选中的模型；保留位置、朝向及服务点引用
static int swapCurrentToModel(const char* model){
  if(!g_ped || !exists(g_ped)) return 1; // 尚无互动对象：只更新后续生成模型
  float c[3]; coords(g_ped,c); float h=heading(g_ped);
  int np=spawnModelAt(model,c[0],c[1],c[2],h);
  if(!np){ logmsg("model load failed: ",0,model); return 0; }
  nativeInit(N_PED_DEFAULT_VARIATION); pI(np); rv();
  nativeInit(N_SET_BLOCKING_NON_TEMP); pI(np); pI(1); rv();
  nativeInit(N_SET_ENTITY_AS_MISSION); pI(np); pI(1); pI(1); rv();
  nativeInit(N_PED_AMBIENT_ANIMS); pI(np); pI(0); rv();
  nativeInit(N_PED_GESTURE_ANIMS); pI(np); pI(0); rv();
  int old=g_ped;
  for(int i=0;i<NSPOT;i++){
    if(g_hooker[i]==old){
      g_hooker[i]=np;
      if(g_hookerBlip[i]){ removeBlip(g_hookerBlip[i]); g_hookerBlip[i]=makeEntityBlip(np,g_spots[i].name,1,27,0.75f); }
    }
  }
  g_ped=np;
  g_outG.valid=0;
  freezePos(old,0);
  restorePed(old);
  nativeInit(N_DELETE_PED); pP(&old); rv();
  logmsg("partner model changed to ",0,model);
  return 1;
}

// 原版交互流程仍使用可脱衣模型；现在使用当前菜单选择的人物模型
static void swapToNude(void){
  if(!g_ped || !exists(g_ped)) return;
  int idx=g_modelSel;
  // 若选中的模型加载失败，回退到原版兼容模型，避免中断互动
  if(!swapCurrentToModel(g_modelNames[idx])){
    swapCurrentToModel("a_f_y_topless_01");
  }
}

// ---------------- 菜单绘制 ----------------
static void drawMenu(const char* title,const char** items,int n,int sel){
  float x=0.705f, w=0.275f, ih=0.034f, y0=0.29f;
  float th=0.045f;
  int visible=n>9?9:n; int first=0;
  if(n>visible){ first=sel-4; if(first<0)first=0; if(first>n-visible)first=n-visible; }
  float total=th+ih*visible;
  drawRect(x+w*0.5f, y0+total*0.5f, w, total, 0,0,0,175);
  drawRect(x+w*0.5f, y0+th*0.5f, w, th, 205,45,105,240);
  drawText(title, x+w*0.5f, y0+0.010f, 0.36f, 255,255,255,255, 1);
  for(int row=0;row<visible;row++){
    int i=first+row; float iy=y0+th+ih*row;
    if(i==sel) drawRect(x+w*0.5f, iy+ih*0.5f, w, ih, 235,90,140,225);
    else       drawRect(x+w*0.5f, iy+ih*0.5f, w, ih, 22,22,22,165);
    drawText(items[i], x+0.014f, iy+0.008f, 0.30f, 255,255,255,255, 0);
  }
}
static void buildMenuHelp(char* out,const char* title,const char** items,int n,int sel){
  char* p=out;
  if(sel<0) sel=0;
  if(sel>=n) sel=n-1;
  p=app(p,"~y~"); p=app(p,title); p=app(p,"~s~  ");
  p=app(p,items[sel]);
  p=app(p,"   ~INPUT_CELLPHONE_UP~~INPUT_CELLPHONE_DOWN~/~INPUT_JUMP~ 换项   ~INPUT_CONTEXT~ 确认   ~INPUT_CELLPHONE_CANCEL~ 返回");
  *p=0;
}

// ---------------- 动画 ----------------
static void playAnimLoop(int ped,const char* dict,const char* name){
  nativeInit(N_REQUEST_ANIM_DICT); pP(dict); rv();
  for(int i=0;i<90;i++){
    nativeInit(N_HAS_ANIM_DICT); pP(dict); if(rv()) break;
    scriptWait(0);
  }
  nativeInit(N_TASK_PLAY_ANIM);
  pI(ped); pP(dict); pP(name);
  pF(8.0f); pF(-4.0f); pI(-1); pI(9); pF(g_speed);
  pI(0); pI(0); pI(0);
  rv();
}
static int animPlaying(int ped,const char* dict,const char* name){
  if(!ped || !dict || !name) return 1;
  nativeInit(N_IS_ENTITY_PLAYING_ANIM); pI(ped); pP(dict); pP(name); pI(3);
  return (int)rv();
}
static void setAnimSpeed(int ped,const char* dict,const char* name){
  nativeInit(N_SET_ENTITY_ANIM_SPEED); pI(ped); pP(dict); pP(name); pF(g_speed); rv();
}

// ---------------- 生成 ----------------
static int spawnHooker(float x,float y,float z,float hdg,int voice){
  int start=voice%6;
  for(int k=0;k<4;k++){
    const char* model=(k==0)?g_modelNames[g_modelSel]:g_hookerModels[(start+k-1)%6];
    u64 mh=joaatStr(model);
    nativeInit(N_REQUEST_MODEL); pI((s64)mh); rv();
    int loaded=0;
    for(int i=0;i<50;i++){
      nativeInit(N_HAS_MODEL); pI((s64)mh); if(rv()){ loaded=1; break; }
      scriptWait(0);
    }
    if(!loaded) continue;
    nativeInit(N_CREATE_PED); pI(4); pI((s64)mh);
    pF(x); pF(y); pF(z); pF(hdg); pI(0); pI(0);
    int ped=(int)rv();
    nativeInit(N_SET_MODEL_NO_LONGER); pI((s64)mh); rv();
    if(!ped) continue;
    nativeInit(N_PED_DEFAULT_VARIATION); pI(ped); rv();
    nativeInit(N_SET_BLOCKING_NON_TEMP); pI(ped); pI(1); rv();
    nativeInit(N_SET_ENTITY_AS_MISSION); pI(ped); pI(1); pI(1); rv();
    nativeInit(N_SET_VOICE_NAME); pI(ped); pP(g_hookerVoices[voice%8]); rv();
    nativeInit(N_TASK_SCENARIO_IN_PLACE); pI(ped); pP("WORLD_HUMAN_PROSTITUTE_LOW_CLASS"); pI(0); pI(1); rv();
    return ped;
  }
  logS("model/create failed");
  return 0;
}

static void spawnAtSpot(int i){
  SpotDef* s=&g_spots[i];
  float z=s->z;
  int pp=playerPed();
  if(pp){
    float pc[3]; coords(pp,pc);
    float d=(pc[0]-s->x)*(pc[0]-s->x)+(pc[1]-s->y)*(pc[1]-s->y);
    if(d<2025.0f && pc[2]>0.5f) z=pc[2];     // 玩家就在 45 米内，用他脚下的高度
  }
  u32 t=gameTimer();
  int ped=spawnHooker(s->x,s->y,z+0.03f,(float)((t/7)%360),i);
  if(!ped){ g_respawnAt[i]=t+30000; logS("spot spawn failed"); return; }
  g_hooker[i]=ped;
  g_hookerBlip[i]=makeEntityBlip(ped,s->name,1,27,0.75f);
  g_respawnAt[i]=0;
  logmsg("hooker spawned at spot",i,"");
}

static void removeHooker(int i){
  if(g_hookerBlip[i]){ removeBlip(g_hookerBlip[i]); g_hookerBlip[i]=0; }
  if(g_hooker[i] && exists(g_hooker[i]) && !isPedDead(g_hooker[i])){
    int h=g_hooker[i];
    nativeInit(N_CLEAR_PED_TASKS_IMM); pI(h); rv();
    nativeInit(N_SET_ENTITY_AS_NO_LONGER); pP(&h); rv();
  }
  g_hooker[i]=0;
  g_respawnAt[i]=gameTimer()+25000;
}

// ---------------- 结束 ----------------
static void End(void){
  int p=playerPed();
  redress(p,&g_outP);
  if(g_ped) redress(g_ped,&g_outG);
  if(g_sex>=3 && g_sex<=5 && p && g_playerZ0>0.0f){
    float c[3]; coords(p,c);
    if(c[2] < g_playerZ0-0.6f) setCoords(p,c[0],c[1],g_playerZ0);
  }
  if(p) freezePos(p,0);
  if(g_ped && exists(g_ped)){
    freezePos(g_ped,0);
    restorePed(g_ped);
    nativeInit(N_SET_ENTITY_NO_COLLISION); pI(g_ped); pI(p); pI(0); rv();
    nativeInit(N_CLEAR_PED_TASKS); pI(g_ped); rv();
    if(g_veh && exists(g_veh) && pedInVehicle(g_ped,g_veh)){
      nativeInit(N_TASK_LEAVE_VEHICLE); pI(g_ped); pI(g_veh); pI(0); rv();
    }
    nativeInit(N_SET_BLOCKING_NON_TEMP); pI(g_ped); pI(0); rv();
    int h=g_ped;
    nativeInit(N_SET_ENTITY_AS_NO_LONGER); pP(&h); rv();
  }
  if(p){ nativeInit(N_CLEAR_PED_TASKS); pI(p); rv(); }
  g_ped=0; g_veh=0; g_sex=0; g_speed=1.0f; g_state=S_IDLE; g_sel=0;
  g_chat=0; g_enterTask=0; g_playerZ0=0.0f;
  clearHelp(); g_helpOn=0; g_promptLock=gameTimer()+1500;
  logS("session end");
}

static void finishSex(void){
  int p=playerPed();
  if(p){
    nativeInit(N_GET_ENTITY_MAX_HEALTH); pI(p); int mx=(int)rv();
    if(mx<=0) mx=200;
    nativeInit(N_SET_ENTITY_HEALTH); pI(p); pI(mx); pI(0); pI(0); rv();
    speech(p,"GENERIC_SHOCKED_MED",1);
  }
  if(g_ped) speech(g_ped,"SEX_FINISHED",1);
  End();
}

// ---------------- 开始一次会话 ----------------
static int eligiblePed(int ped){
  if(!ped) return 0;
  if(!exists(ped)) return 0;
  if(!isPed(ped)) return 0;
  if(isPedMale(ped)) return 0;
  if(isPedDead(ped)) return 0;
  if(pedInAnyVehicle(ped)) return 0;
  return 1;
}

static void startOnFoot(int ped){
  int p=playerPed();
  g_ped=ped; g_veh=0; g_sex=0; g_speed=1.0f; g_sel=0; g_chat=0;
  g_sessSpot=-1;
  for(int i=0;i<NSPOT;i++){ if(g_hooker[i]==ped) g_sessSpot=i; }
  nativeInit(N_SET_ENTITY_AS_MISSION); pI(ped); pI(1); pI(1); rv();
  stopPed(ped);
  speech(p,"GENERIC_HI",1);
  nativeInit(N_TASK_TURN_TO_FACE); pI(p); pI(ped); pI(3000); rv();
  nativeInit(N_TASK_TURN_TO_FACE); pI(ped); pI(p); pI(3000); rv();
  g_state=S_GREET; g_stT=gameTimer(); g_lock=g_stT+400;
  logmsg("start on foot ped=",ped,"");
}

static void startVehicle(int ped,int veh){
  g_ped=ped; g_veh=veh; g_sex=0; g_speed=1.0f; g_sel=0;
  nativeInit(N_SET_ENTITY_AS_MISSION); pI(ped); pI(1); pI(1); rv();
  nativeInit(N_SET_BLOCKING_NON_TEMP); pI(ped); pI(1); rv();
  speech(ped,"NICE_CAR",1);
  nativeInit(N_TASK_TURN_TO_FACE); pI(ped); pI(playerPed()); pI(3000); rv();
  g_state=S_ENTER; g_stT=gameTimer()+1500; g_enterTask=0; g_lock=gameTimer()+400;
  logS("start vehicle");
}

static void summonHere(void){
  int p=playerPed();
  if(!p) return;
  float c[3], f[3]; coords(p,c); forward(p,f);
  int si=g_summonSpot;
  int bad=(c[0]*c[0]+c[1]*c[1] < 1.0f);           // 坐标读不出来？
  if(bad && si>=0){ c[0]=g_spots[si].x; c[1]=g_spots[si].y; c[2]=g_spots[si].z; }
  if(f[0]*f[0]+f[1]*f[1] < 0.01f){ f[0]=0.0f; f[1]=1.0f; }
  float x=c[0]+f[0]*0.8f, y=c[1]+f[1]*0.8f;
  float z=c[2];
  u32 t=gameTimer();
  int voice=(int)((t/13)%8);
  int ped=spawnHooker(x,y,z+0.03f,heading(p)+180.0f,voice);
  if(!ped){ logS("summon failed"); return; }
  g_summonSpot=-1;
  startOnFoot(ped);
  if(bad){
    nativeInit(N_TASK_GO_TO_ENTITY); pI(ped); pI(p); pI(-1); pF(1.2f); pF(1.0f); pF(0.5f); pI(0); rv();
  }
  g_state=S_MENU; g_sel=0; g_lock=gameTimer()+400;
  speech(ped,"HOOKER_OFFER_SERVICE",1);
  logS("summoned");
}

// ---------------- 姿势 ----------------
static float boneZ(int ped,int bone){
  nativeInit(N_GET_PED_BONE_COORDS); pI(ped); pI(bone); pF(0.0f); pF(0.0f); pF(0.0f);
  float v[3]; rvV3(v); return v[2];
}
static void setupOnFoot(int type){
  int p=playerPed();
  float c[3], f[3]; coords(p,c); forward(p,f);
  int bad=(c[0]*c[0]+c[1]*c[1] < 1.0f);
  if(f[0]*f[0]+f[1]*f[1] < 0.01f){ f[0]=0.0f; f[1]=1.0f; }
  float h=heading(p);
  float rx=f[1], ry=-f[0];
  g_sex=type;
  g_playerZ0=c[2];
  g_sexT=gameTimer(); g_groundFix=0; g_hOff=0.0f;

  nativeInit(N_CLEAR_PED_TASKS); pI(g_ped); rv();
  freezePos(g_ped,1);
  undress(p,&g_outP);

  if(bad){
    nativeInit(N_TASK_TURN_TO_FACE); pI(g_ped); pI(p); pI(-1); rv();
  } else if(type==1){
    setHeading(g_ped,h-180.0f);
    setCoords(g_ped,c[0]+f[0]*0.55f,c[1]+f[1]*0.55f,c[2]);
  } else if(type==2){
    setHeading(g_ped,h);
    setCoords(g_ped,c[0]+f[0]*0.15f,c[1]+f[1]*0.15f,c[2]-0.10f);
  } else if(type==3){
    freezePos(p,1);
    setCoords(p,c[0],c[1],c[2]-0.85f);
    setHeading(g_ped,h-180.0f);
    setCoords(g_ped,c[0]-rx*0.85f+f[0]*0.35f,c[1]-ry*0.85f+f[1]*0.35f,c[2]-0.95f);
  } else if(type==4){
    freezePos(p,1);
    setCoords(p,c[0],c[1],c[2]-0.85f);
    setHeading(g_ped,h);
    setCoords(g_ped,c[0]+rx*0.8f+f[0]*0.07f,c[1]+ry*0.8f+f[1]*0.07f,c[2]-0.85f);
  } else {
    freezePos(p,1);
    setCoords(p,c[0],c[1],c[2]-0.90f);
    setHeading(g_ped,h);
    setCoords(g_ped,c[0]-rx*0.8f-f[0]*0.07f,c[1]-ry*0.8f-f[1]*0.07f,c[2]-0.90f);
  }
  {
    float sc[3]; coords(g_ped,sc);
    g_stayX=sc[0]; g_stayY=sc[1]; g_stayZ=sc[2]; g_stayH=heading(g_ped);
  }
  noCollision(g_ped,p,1);

  const char* d1; const char* a1; const char* d2; const char* a2; const char* sp;
  if(type==1){ d1="misscarsteal2pimpsex"; a1="pimpsex_punter"; d2=d1; a2="pimpsex_hooker"; sp="SEX_ORAL_FEM"; }
  else if(type==2){ d1="rcmpaparazzo_2"; a1="shag_loop_a"; d2=d1; a2="shag_loop_poppy"; sp="SEX_GENERIC"; }
  else if(type==3){ d1="mini@prostitutes@sexnorm_veh"; a1="bj_loop_male"; d2=d1; a2="bj_loop_prostitute"; sp="SEX_ORAL_FEM"; }
  else if(type==4){ d1="mini@prostitutes@sexnorm_veh"; a1="sex_loop_male"; d2=d1; a2="sex_loop_prostitute"; sp="SEX_GENERIC"; }
  else { d1="random@drunk_driver_2"; a1="cardrunksex_loop_m"; d2=d1; a2="cardrunksex_loop_f"; sp="SEX_GENERIC"; }
  g_d1=d1; g_a1=a1; g_d2=d2; g_a2=a2; g_animChk=gameTimer()+800;
  playAnimLoop(p,d1,a1);
  playAnimLoop(g_ped,d2,a2);
  speech(g_ped,sp,1);
  speech(p,"GENERIC_SHOCKED_MED",1);
  logmsg("pose ",type,"");
}

static void setupVehicle(int type){
  int p=playerPed();
  g_sex=type;
  g_sexT=gameTimer();
  undress(p,&g_outP);
  if(type==1){
    playAnimLoop(p,"mini@prostitutes@sexnorm_veh","bj_loop_male");
    playAnimLoop(g_ped,"mini@prostitutes@sexnorm_veh","bj_loop_prostitute");
    speech(g_ped,"SEX_ORAL_FEM",0);
  } else {
    playAnimLoop(p,"mini@prostitutes@sexnorm_veh","sex_loop_male");
    playAnimLoop(g_ped,"mini@prostitutes@sexnorm_veh","sex_loop_prostitute");
    speech(g_ped,"SEX_GENERIC",0);
  }
  speech(p,"GENERIC_SHOCKED_MED",0);
}

// ---------------- 输入 ----------------
static int pressed(int c){ nativeInit(N_IS_CONTROL_JUST_PRESSED); pI(0); pI(c); return (int)rv(); }
static int g_keyKPrev=0;
static int g_keyK=0;
static void pollKeys(void){
  int now=(GetAsyncKeyState(0x4B)&0x8000)?1:0;   // K
  g_keyK=(now && !g_keyKPrev);
  g_keyKPrev=now;
  int f2=(GetAsyncKeyState(0x71)&0x8000)?1:0;
  g_keyF2=(f2 && !g_keyF2Prev);
  g_keyF2Prev=f2;
}
static int dpressed(int c){ nativeInit(N_IS_DISABLED_CONTROL_JUST); pI(0); pI(c); return (int)rv(); }
static void disableControl(int c){ nativeInit(N_DISABLE_CONTROL_ACTION); pI(0); pI(c); pI(1); rv(); }

static int menuAccept(void){
  if(g_keyK) return 1;
  if(pressed(176)) return 1;
  if(pressed(201)) return 1;
  if(pressed(38))  return 1;
  if(pressed(51))  return 1;
  return 0;
}
static int menuBack(void){
  if(pressed(177)) return 1;
  if(pressed(202)) return 1;
  return 0;
}
static int menuUp(void){ return pressed(172) || pressed(174) || pressed(21); }
static int menuDown(void){ return pressed(173) || pressed(175) || pressed(22); }

static void tickModelMenu(int t){
  static const char* title="热咖啡 · 人物选择";
  char help[512];
  buildMenuHelp(help,title,g_modelDisplay,g_modelCount,g_modelSel);
  helpText(help);
  drawMenu(title,g_modelDisplay,g_modelCount,g_modelSel);
  disableControl(24); disableControl(25); disableControl(21); disableControl(22);
  if(t<(int)g_lock) return;
  if(menuUp()){ g_modelSel--; if(g_modelSel<0) g_modelSel=g_modelCount-1; g_lock=t+140; return; }
  if(menuDown()){ g_modelSel++; if(g_modelSel>=g_modelCount) g_modelSel=0; g_lock=t+140; return; }
  if(menuBack()){
    g_modelMenuOpen=0; g_lock=t+250; clearHelp();
    return;
  }
  if(menuAccept()){
    if(swapCurrentToModel(g_modelNames[g_modelSel])){
      logmsg("selected model index ",g_modelSel,"");
      helpText("人物已切换");
    } else {
      helpText("人物加载失败：请检查模型名称和 Add-on 安装");
    }
    g_modelMenuOpen=0; g_lock=t+350;
    return;
  }
}


// ---------------- 扫描附近女郎 ----------------
static int scanNearby(int p){
  int buf[17];
  memset(buf,0,sizeof(buf));
  buf[0]=16;
  nativeInit(N_GET_PED_NEARBY_PEDS); pI(p); pP(buf); pI(-1); rv();
  int n=buf[0];
  if(n<0) n=0;
  if(n>16) n=16;
  int best=0; float bestd=25.0f;
  for(int i=1;i<=n;i++){
    int h=buf[i];
    if(!h || h==p) continue;
    if(!exists(h)) continue;
    if(!isPed(h)) continue;
    if(isPedMale(h)) continue;
    if(isPedDead(h)) continue;
    if(pedInAnyVehicle(h)) continue;
    float d=distBetween(p,h);
    if(d<bestd){ bestd=d; best=h; }
  }
  return best;
}

static int eligiblePed(int ped);

// 最近的可搭讪对象（4 米内）：扫描结果 + 我们自己生成的女郎
static int girlNear(int p){
  float bd=36.0f; int best=0;
  if(eligiblePed(g_nearPed)){
    float d=distBetween(p,g_nearPed);
    if(d<bd){ bd=d; best=g_nearPed; }
  }
  for(int i=0;i<NSPOT;i++){
    if(eligiblePed(g_hooker[i])){
      float d=distBetween(p,g_hooker[i]);
      if(d<bd){ bd=d; best=g_hooker[i]; }
    }
  }
  return best;
}

static void diagLog(int p,int inVeh,float spd,int girl){
  float c[3]; coords(p,c);
  int si=-1;
  for(int i=0;i<NSPOT;i++){
    if(entAtCoord(p,g_spots[i].x,g_spots[i].y,g_spots[i].z,20.0f,20.0f,12.0f)){ si=i; break; }
  }
  HANDLE h=openLog();
  if(h==(HANDLE)-1) return;
  s64 zero=0;
  SetFilePointerEx(h,(long long)0,&zero,2);
  char line[420]; char* q=line;
  q=app(q,"[HC] diag px="); q=appnum(q,(s64)(c[0]*10.0f));
  q=app(q," py="); q=appnum(q,(s64)(c[1]*10.0f));
  q=app(q," pz="); q=appnum(q,(s64)(c[2]*10.0f));
  q=app(q," spd="); q=appnum(q,(s64)(spd*10.0f));
  q=app(q," car="); q=appnum(q,(s64)inVeh);
  q=app(q," girl="); q=appnum(q,(s64)girl);
  q=app(q," spot="); q=appnum(q,(s64)si);
  *q=0;
  u32 w=0; WriteFile(h,line,slen(line),&w,0); WriteFile(h,"\r\n",2,&w,0);
  CloseHandle(h);
}

// ---------------- 每帧状态 ----------------
static void tickIdle(int p,int t){
  if(g_helpOn && t>(int)g_lastPromptT+250){ clearHelp(); g_helpOn=0; }
  if(t>=(u32)g_scanAt){
    g_scanAt=(int)t+600;
    g_nearPed=scanNearby(p);
  }

  int inVeh=pedInAnyVehicle(p);
  int veh=0; int seatFree=0; int carClass=0; float spd=0.0f;
  if(inVeh){
    veh=vehicleOfPed(p);
    if(veh){
      seatFree=isSeatFree(veh,0);
      spd=entSpeed(veh);
      int vc=vehicleClass(veh);
      carClass=(vc>=0 && vc<=12 && vc!=8 && vc!=13);
    }
  }

  // 服务点管理（每 2 秒一次，一次最多生成一个，避免卡顿）
  if(t>=(u32)g_spotAt){
    g_spotAt=(int)t+2000;
    int spent=0;
    for(int i=0;i<NSPOT;i++){
      if(g_hooker[i] && (!exists(g_hooker[i]) || isPedDead(g_hooker[i]))){
        if(g_hookerBlip[i]){ removeBlip(g_hookerBlip[i]); g_hookerBlip[i]=0; }
        g_hooker[i]=0;
        g_respawnAt[i]=t+20000;
      }
      (void)spent;
    }
  }

  int girl=girlNear(p);

  if(t>=(int)g_promptLock && !inVeh && girl){
    helpText("按 ~INPUT_CONTEXT~ 或 K 搭讪  （热咖啡）");
    g_lastPromptT=t; g_helpOn=1;
    if((g_keyK || pressed(38) || pressed(51)) && t>g_lock){ startOnFoot(girl); return; }
    return;
  }

  if(t>=(int)g_promptLock && inVeh && seatFree && carClass && girl && spd<1.2f){
    helpText("按 ~INPUT_CONTEXT~ 或 K 让她上车  （热咖啡）");
    g_lastPromptT=t; g_helpOn=1;
    if((g_keyK || pressed(38) || pressed(51)) && t>g_lock){ startVehicle(girl,veh); return; }
    return;
  }

  if(t>=(int)g_promptLock && !inVeh){
    for(int i=0;i<NSPOT;i++){
      if(distTo(p,g_spots[i].x,g_spots[i].y,g_spots[i].z)<144.0f){
        int busy=0;
        for(int k=0;k<NSPOT;k++){
          if(eligiblePed(g_hooker[k]) && distBetween(p,g_hooker[k])<9.0f) busy=1;
        }
        if(!busy){
          helpText("按 ~INPUT_CONTEXT~ 或 K 叫一位  （热咖啡 · 服务点）");
          g_lastPromptT=t; g_helpOn=1;
          if((g_keyK || pressed(38) || pressed(51)) && t>g_lock){
            g_summonSpot=i; g_sessSpot=i; summonHere(); return;
          }
        }
        break;
      }
    }
  }

  if(t>=(u32)g_diagAt){
    g_diagAt=(int)t+5000;
    diagLog(p,inVeh,spd,girl);
  }
}

static void tickGreet(int p,int t){
  int done = (t>g_stT+700 && !speechPlaying(p));
  if(t>g_stT+4000) done=1;
  if(done){
    speech(g_ped,"GENERIC_HI",1);
    g_state=S_MENU; g_sel=0; g_lock=t+400; g_stT=t;
    return;
  }
}

static void tickMenu(int p,int t,int carMenu){
  disableControl(24); disableControl(25); disableControl(21); disableControl(22);
  if(g_chat && !speechPlaying(p)){
    speech(g_ped,"CHAT_RESP",1);
    g_chat=0;
  }
  const char** items = carMenu?MENU_CAR:MENU_MAIN;
  int n = 3;
  char help[512];
  buildMenuHelp(help, carMenu?"热咖啡 · 车内":"热咖啡", items, n, g_sel);
  helpText(help);
  drawMenu(carMenu?"热咖啡 · 车内":"热咖啡", items, n, g_sel);

  if(t<g_lock) return;
  if(menuUp()){ g_sel--; if(g_sel<0) g_sel=n-1; g_lock=t+140; return; }
  if(menuDown()){ g_sel++; if(g_sel>=n) g_sel=0; g_lock=t+140; return; }
  if(menuBack()){ speech(g_ped,"GENERIC_WHATEVER",1); End(); return; }
  if(menuAccept()){
    g_lock=t+250;
    if(!carMenu){
      if(g_sel==0){ speech(p,"CHAT_STATE",1); g_chat=1; }
      else if(g_sel==1){ swapToNude(); speech(g_ped,"HOOKER_OFFER_SERVICE",1); g_state=S_POSE; g_sel=0; g_lock=t+300; }
      else { speech(g_ped,"GENERIC_WHATEVER",1); End(); }
    } else {
      if(g_sel==0){ setupVehicle(1); g_state=S_CAR_SEX; }
      else if(g_sel==1){ setupVehicle(2); g_state=S_CAR_SEX; }
      else { speech(g_ped,"GENERIC_WHATEVER",1); End(); }
    }
  }
}

static void tickPose(int p,int t){
  disableControl(24); disableControl(25); disableControl(21); disableControl(22);
  char help[512];
  buildMenuHelp(help,"热咖啡 · 姿势",MENU_POSE,5,g_sel);
  helpText(help);
  drawMenu("热咖啡 · 姿势",MENU_POSE,5,g_sel);

  if(t<g_lock) return;
  if(menuUp()){ g_sel--; if(g_sel<0) g_sel=4; g_lock=t+140; return; }
  if(menuDown()){ g_sel++; if(g_sel>=5) g_sel=0; g_lock=t+140; return; }
  if(menuBack()){ g_state=S_MENU; g_sel=0; g_lock=t+250; return; }
  if(menuAccept()){
    g_lock=t+300;
    setupOnFoot(g_sel+1);
    g_state=S_SEX;
  }
}

static void tickSex(int p,int t,int inCar){
  if(t>(int)g_sexT+15000){ finishSex(); return; }
  int left=15-((int)t-(int)g_sexT)/1000;
  if(left<0) left=0;
  char hb[200]; char* hp=hb;
  hp=app(hp,"剩余 "); hp=appnum(hp,left); hp=app(hp," 秒   ~INPUT_CONTEXT~ 结束");
  *hp=0;
  helpText(hb);
  noCollision(g_ped,p,1);
  if(!inCar && t>(int)g_animChk){
    g_animChk=t+800;
    if(!animPlaying(p,g_d1,g_a1)) playAnimLoop(p,g_d1,g_a1);
    if(!animPlaying(g_ped,g_d2,g_a2)) playAnimLoop(g_ped,g_d2,g_a2);
  }
  if(!inCar){
    if(pressed(172) || pressed(173)){
      float d=pressed(172)?0.05f:-0.05f;
      float cc[3];
      coords(p,cc); setCoords(p,cc[0],cc[1],cc[2]+d);
      coords(g_ped,cc); setCoords(g_ped,cc[0],cc[1],cc[2]+d);
      g_stayZ+=d;
      g_hOff+=d;
      logmsg("height ",(s64)(g_hOff*100.0f),"");
    }
    freezePos(g_ped,1);
    float sc[3]; coords(g_ped,sc);
    float dx=sc[0]-g_stayX, dy=sc[1]-g_stayY;
    if(dx*dx+dy*dy > 4.0f){
      setCoords(g_ped,g_stayX,g_stayY,g_stayZ);
      setHeading(g_ped,g_stayH);
    }
  }
  if(pressed(174)){ g_speed-=0.05f; if(g_speed<0.55f) g_speed=0.55f; }
  if(pressed(175)){ g_speed+=0.05f; if(g_speed>1.50f) g_speed=1.50f; }
  if(inCar){
    setAnimSpeed(p,"mini@prostitutes@sexnorm_veh", g_sex==1?"bj_loop_male":"sex_loop_male");
    setAnimSpeed(g_ped,"mini@prostitutes@sexnorm_veh", g_sex==1?"bj_loop_prostitute":"sex_loop_prostitute");
  } else {
    if(g_sex==1){ setAnimSpeed(p,"misscarsteal2pimpsex","pimpsex_punter"); setAnimSpeed(g_ped,"misscarsteal2pimpsex","pimpsex_hooker"); }
    else if(g_sex==2){ setAnimSpeed(p,"rcmpaparazzo_2","shag_loop_a"); setAnimSpeed(g_ped,"rcmpaparazzo_2","shag_loop_poppy"); }
    else if(g_sex==3){ setAnimSpeed(p,"mini@prostitutes@sexnorm_veh","bj_loop_male"); setAnimSpeed(g_ped,"mini@prostitutes@sexnorm_veh","bj_loop_prostitute"); }
    else if(g_sex==4){ setAnimSpeed(p,"mini@prostitutes@sexnorm_veh","sex_loop_male"); setAnimSpeed(g_ped,"mini@prostitutes@sexnorm_veh","sex_loop_prostitute"); }
    else { setAnimSpeed(p,"random@drunk_driver_2","cardrunksex_loop_m"); setAnimSpeed(g_ped,"random@drunk_driver_2","cardrunksex_loop_f"); }
  }
  if(pressed(51)){ finishSex(); }
}

static void tickEnter(int p,int t){
  if(!g_enterTask && t>g_stT){
    nativeInit(N_TASK_ENTER_VEHICLE); pI(g_ped); pI(g_veh); pI(15000); pI(0); pF(1.0f); pI(1); pP(0); rv();
    g_enterTask=1;
  }
  if(g_enterTask && pedInVehicle(g_ped,g_veh)){
    speech(g_ped,"HOOKER_SECLUDED",1);
    g_state=S_CAR_MENU; g_sel=0; g_lock=t+400;
    return;
  }
  if(t>g_stT+16000){ logS("enter vehicle timeout"); End(); }
}

// ---------------- 主循环 ----------------
static void Main(void){
  logS("script started v28 F2 multi-ped");
  loadModelConfig();
  while(1){
    scriptWait(0);
    pollKeys();
    int p=playerPed();
    if(!p) continue;
    u32 t=gameTimer();

    if(!g_blipsDone){
      for(int i=0;i<NSPOT;i++){
        g_spotBlip[i]=makeBlip(g_spots[i].x,g_spots[i].y,g_spots[i].z,g_spots[i].name,1,27,0.8f);
        g_hooker[i]=0; g_hookerBlip[i]=0; g_respawnAt[i]=0;
      }
      g_blipsDone=1; g_scanAt=(int)t; g_spotAt=(int)t;
      logS("blips created");
    }

    if(isPedDead(p)){ if(g_state!=S_IDLE) End(); g_modelMenuOpen=0; continue; }

    // F2 多人物菜单：在非动画互动阶段可随时打开；菜单期间暂停原状态逻辑
    if(g_keyF2 && !g_modelMenuOpen && g_state!=S_SEX && g_state!=S_CAR_SEX){
      g_modelMenuOpen=1; g_lock=(u32)t+180; g_sel=0;
    }
    if(g_modelMenuOpen){ tickModelMenu((int)t); continue; }

    if(g_state==S_IDLE){ tickIdle(p,(int)t); continue; }

    if(g_state==S_GREET){ tickGreet(p,(int)t); }
    else if(g_state==S_MENU){ tickMenu(p,(int)t,0); }
    else if(g_state==S_POSE){ tickPose(p,(int)t); }
    else if(g_state==S_SEX){ tickSex(p,(int)t,0); }
    else if(g_state==S_ENTER){
      if(t>g_stT+17000){ End(); }
      else tickEnter(p,(int)t);
    }
    else if(g_state==S_CAR_MENU){ tickMenu(p,(int)t,1); }
    else if(g_state==S_CAR_SEX){ tickSex(p,(int)t,1); }

    // 车内阶段：人下车 / 车没了就结束
    if((g_state==S_CAR_MENU || g_state==S_CAR_SEX)){
      if(!g_veh || !exists(g_veh) || !pedInVehicle(p,g_veh)){
        logS("left the car");
        End();
        continue;
      }
    }
    if(g_state!=S_IDLE && g_ped && isPedDead(g_ped)){
      logS("ped died");
      End();
      continue;
    }

    // 走远自动结束
    if(g_state!=S_IDLE && g_ped && exists(g_ped)){
      float lim=(g_state==S_ENTER)?400.0f:100.0f;
      if(distBetween(p,g_ped)>lim){
        logS("walked away");
        End();
      }
    }
  }
}

extern "C" int DllMain(HINST h,u32 reason,void*){
  if(reason==1){
    u32 n=GetModuleFileNameA(h,g_dir,260);
    u32 k;
    for(k=n;k>0;k--){ if(g_dir[k-1]=='\\'||g_dir[k-1]=='/'){ g_dir[k]=0; break; } }
    logS("dll attached");
    scriptRegister(h,Main);
  }
  return 1;
}
