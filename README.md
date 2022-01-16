# cmediasoup_push
cmediasoup_push websocket client



|线程类型	|线程名称|	全局变量|	功能|	所属模块|
|:---:|:---:|:---:|:---:|:---:|
|Thread|	signaling_thread|	signaling_thread_|	信令线程，主要负责事件和状态	|消息处理|
|Thread	|worker_thread|	worker_thread_	|工作者线程，主要跟各种数据流相关|	消息处理|
|Thread	|network_thread|	network_thread_	|网络线程，处理接收网络报文|	网络|
|PlatformThread|	PacerThread|	pacer_thread_	pacer平滑发送报文|	网络|
|PlatformThread|	VoiceProcessThread|	_moduleProcessThreadPtr|	好像是VOD检测？还是会场混音算法？	待确认|
|PlatformThread|	call_worker_queue|	worker_queue_|	音视频网络状态检测|	资源管理|
|PlatformThread|	ModuleProcessThread|	module_process_thread_	|挂载定时调用函数|	资源管理|
|PlatformThread|	EncoderQueue	|encoder_queue_	|视频编码	|视频|
|PlatformThread|	DecodingThread	|decode_thread_	|视频解码（kHighestPriority）|	视频|
|PlatformThread|	IncomingVideoStream|	incoming_render_queue_	|视频渲染（HIGH）|	视频|
|PlatformThread|	AudioEncoderQueue|	encoder_queue_	|音频编码	|音频|
|CreateThread	| |	_hRecThread|	音频采集|	音频|
|CreateThread	| |	_hPlayThread|	音频渲染（解码和渲染在一个线程里）|	音频|
|CreateThread	| |	_hGetCaptureVolumeThread|	获取音量参数|	音频|
|CreateThread	| |	_hSetCaptureVolumeThread|	配置音频参数|	音频|
|PlatformThread|	rtc-low-prio|	low_priority_worker_queue_|音频录制维护功能（LOW）	维护|
|PlatformThread|	AudioDeviceBufferTimer|	task_queue_|	周期调用LogStats|	维护|
|PlatformThread|	rtc_event_log|	task_queue_	|维护功能	|维护|
 
