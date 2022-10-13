namespace java com.syz.cloud.render

enum status {
   pathError = 0,
   deactivate = 1,
   active = 2,
   active_stream =3,
   appError = 4
}

enum command {
   start = 0, //打开程序
   stop = 1, //停程序
   pause = 2, // 停流
   resume = 3, //恢复流
   restart = 4 // 重新来一次
}

struct CloudAppStatus {
   1:string clientId,
   2:status status,
   3:string path,
}

struct HeatBeat {
   1:list<CloudAppStatus> cloudAppStatus,
   2:string agentId,
}

service CloudRender{
    void pingServer(1:HeatBeat heatBeat),
    list<CloudAppStatus> getApps(1:string agentId),
}