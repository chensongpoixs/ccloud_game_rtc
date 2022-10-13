namespace cpp CloudAgent

 

enum command {
   start = 0, //打开程序
   stop = 1, //停程序
   pause = 2, // 停流
   resume = 3, //恢复流
   restart = 4 // 重新来一次
}
 
struct AgentWebRtc 
{
	1:string 			agentId,
	2:string 			collectionId, // 
	3:command 			cmd, // --> start -> 下面3和4字段有效
	4:string 			roomName,
	5:string 			clientName,
	6:string 			taskId, // hashid 
}


service CloudAgent 
{
	i32 Command(1:AgentWebRtc  agentcmd)
}







