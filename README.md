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



# webrtc  的指标

自顶而下观察这幅图，我们将这个图分为3个部分来说明：

第一个部分，也就是绿线的上方（图的顶部）有两个方块，第一个方块展示的是PeerConnection的相关信息，如果有多个PeerConnection的话，那么就会有多个类似的标签页，另外一个方块展示的是GetUserMedia的相关信息，在本文中我们暂时不涉及。
第二个部分，在图中标示的Time和Event区域（图的上方偏左侧），这个部分主要描述的是PeerConnection建立的流程，相关方法调用的次序以及调用的时间。
第三个部分，在图中标示的Stats Tables部分（图的右侧以及下半部分），这个部分主要展示的是PeerConnection相关的统计数据，而我们所要说明的就是就是音频以及视频流的统计信息。

所有这些统计数据都来源于Chrome M56版本，可能会与老版本的Chrome存在不一致的地方，如果有些字段的描述不正确，还请指正，我会首先列出发送端的部分描述信息，后续会将接收端的补全。

音频统计数据
aecDivergentFilterFraction：TBD（后续会补全）

audioInputLevel：发送端采集的音频能量大小。

bitsSentPerSecond：每秒发送出去的比特数。

packetsSentPerSecond：每秒发送出去的音频包数。

googEchoCancellationQualityMin：TBD（后续会补全）

googEchoCancellationReturnLoss：TBD（后续会补全）

googEchoCancellationReturnLossEnhancement：TBD（后续会补全）

googResidualEchoLikelihood：Chrome 56中新增的，主要用来标识是否存在回声，范围为0 （没有回声）- 1（有回声），当值大于0.5时表明存在回声。

视频统计数据
bitsSentPerSecond：每秒发送出去的比特数，根据当前网络情况会进行动态调整。

framesEncoded：累计编码出来的视频帧数，没有异常情况的话会一直增长。

packetsLost：发送端从接收端发送过来的RTCP Receiver Report中得到的累积丢包数量，可以和googNacksReceived数据进行对照。

googRtt：Rtt全称为Round-trip time，是发送端从接受端发送过来的RTCP Receiver Report中得到的时间戳通过计算得到的往返时延。

packetsSentPerSecond：Chrome 56中新增的，每秒发送出去的视频包数量。

qpSum：发送端编码出的带有QP值的帧的数量，QP全称为Quantization Parameter。

googAdaptationChanges：发送端因为CPU的负载变化导致的分辨变高或者变低的次数，需要设置googCpuOveruseDetection。

googAvgEncodeMs：发送端平均编码时间，越小越好。

googEncodeUsagePercent：发送端（平均每帧编码时间）／（平均每帧采集时间），反应编码效率。

googFirsReceived：发送端收到的关键帧请求数量，FIR全称为Full Intra Request，一般来说在video conference模式下，有新的参与者进来会发出。

googPlisReceived：发送端收到的关键帧请求数量，PLI全称为Picture Loss Indication，一般来说在解码失败时会发出。

googNacksReceived：发送端收到的重传包请求数量，Nack全称为Negative ACKnowledgement可以和packetsLost数据进行对照。

googFrameHeightSent：发送端发送的分辨率高度，根据当前网络会进行动态调整。

googFrameWidthSent：发送端发送的分辨率宽度，根据当前网络会进行动态调整。

googFrameRateInput：发送端设置的初始帧率。

googFrameRateSent：发送端实际发送的帧率，根据当前网络会进行动态调整。
 
