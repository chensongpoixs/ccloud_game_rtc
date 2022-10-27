# 一、云游戏的架构设计和技术实现


![wayland](https://github.com/chensongpoixs/ccloud_game_rtc/blob/encode/wayland.png?raw=true)

## 1、什么是云游戏？

云游戏这个概念非常简单，就是我把游戏放到服务器上去运行，把游戏渲染出来的的音视频画面，通过流的形式传送到终端，终端上不再需要安装游戏，各种终端比如说电视、手机、PC、平板都可以运行。这样我们就不需要关心游戏怎么去适配不同的软硬件平台、终端性能够不够等等这些问题。这个概念本身是非常好的，在2009年的时候，这个技术就已经出现了，美国有家叫Onlive 的公司第一个推出云游戏服务，但是他最终在商业上还是失败了，技术最后被索尼公司收购，并运用在PS Now上。云游戏的概念虽然非常好，但里面技术挑战性非常高，有非常多的技术问题需要解决，那个时代可能还比较早，软硬件都还不太成熟，所以最后没有能够成功的商业化。到了现在这个时间点上，云游戏技术开始慢慢成熟起来，已经具备了商业化的基础。
 
对云游戏来说，用户主要会关心延迟问题，玩一个对抗性很强的游戏，如果中间卡个几百毫秒那肯定受不了，游戏体验就会非常差。所以我们最核心的关注点就是要把延迟降低到最小、并且把画质保持在一个相对可以接受的程度。目前我们产品的整体延迟（从用户按下操作按钮到看到画面变化）可以控制到50毫秒以下，在这样的延迟水平下玩格斗游戏赛车游戏感觉都是非常流畅的，画面可以支持到720P/1080P，网络带宽只要4兆以上就可以了。我们单台服务器可以支持 20-50 路的并发游戏数量，也就是单台服务器可以同时为 50 个玩家提供服务，单个并发用户的整体服务器硬件成本在500元左右，可以说是一个非常有竞争力的成本。当年 OnLive 失败的主要原因是因为他的硬件成本非常高，他的一台服务器仅能服务一个用户，单个并发用户的成本可能就要上万，在这样的成本水平上要实现商业上的成功是非常困难的。目前这个项目已经在小范围的内测，他们主要是 toB 的业务，为宽带运营商提供增值游戏服务。



## 2、云游戏的技术挑战

### ①、第一个是实时性

游戏的整体延迟包括了游戏逻辑运算时间、音画渲染的时间，加上编码的延时、网路传输的延时、客户端解码的延时、客户端向服务端发送控制信息的延时，云游戏的实时性要达到一个可令玩家接受的程度，这个技术挑战是非常高的，当然也要依靠硬件和网络本身的性能，如果没有足够的带宽也不可能做到。

### ②、第二是虚拟化技术

虚拟化在服务端已经非常成熟，我们有虚拟机技术以及各种容器技术，但是在桌面上就不是那么成熟，普通的虚拟桌面不支持 GPU 的虚拟化，而游戏非常依赖 GPU 渲染，若没有 GPU 的虚拟化就没办法实现云游戏了，所以虚拟化是一个很大的技术瓶颈。

### ③、第三是经济性

每个并发用户的服务器硬件成本关系到这个模式能否成功商业化，如果成本超出了用户可接受的范围，那就没有办法实现盈利。

### ④、最后是运维管理

云游戏的运维管理跟传统的服务器运维管理不一样，因为用到的服务器硬件不一样，同时硬件负载又很高，这对运维管理提出了新的挑战，所以在技术上就要解决这些问题。



## 3、平台选择

游戏的运行平台非常多，各种各样，但是比较适合的只有windows平台。Linux 平台虽然开放，但是它没有什么游戏支持，其他的主机游戏平台基本都属于封闭技术，微软和索尼自己都在研发主机上的云游戏，那我们是没有办法去做的。

android平台也是非常适合做云游戏。服务器跑个android游戏再传到android设备上这个概念看上去比较怪异，但实际上IPTV运营商非常喜欢这个概念，因为机顶盒不允许安装第三方的应用，监控比较严，那我们通过云端化来绕过这种限制，这对机顶盒这种产品非常有帮助，所以android平台也是我们要考虑的。但今天主要是介绍 windows 平台游戏的虚拟化，android上是用硬件方案跑的，所以就不介绍了。

windows游戏的虚拟化技术主要是两条路线。一个是虚拟机方案，但主要问题是 GPU 虚拟化技术不成熟，可能需要一些专业级的显卡支持，成本非常高、性能损耗非常大，每一个游戏都跑一个 Guest OS 非常浪费内存，所以这条方案就被我们否掉了。同时windows 上也缺少可用的容器级技术，我们只能采取 API Hook 方式手工实现虚拟化，我们称之为 Sandbox 方案。

Sandbox方案就是把游戏所用到的系统 API 全部hook接管，让游戏认为自己运行在一个正常的 OS 上面，但实际上是我们接管的一个 OS。这样做的好处是性能损耗很小，基本上没有额外的损耗，但是比较痛苦的要针对每个 API 做适配，需要对每个游戏进行适配，而且游戏通常不开源，游戏开发商通常也不会配合你去修改代码，需要一些 hack 技术来针对每个游戏做适配。



## 4、技术实现细节

### ①、图像和声音的采集

图形API有 DirectX 9,10,11,12还有OpenGL，接管这些API后我们就可以把画面重定向到视频编码器，不不在屏幕上输出了。音频比较简单，只要接管Windows Audio Session API就可以了。

### ②、输入操作的虚拟化

手柄比较麻烦，因为手柄支持的API接口比较多样化，比如 DirectInput, XInput, RawInput，还有些游戏直接读 USB 设备，实现这些API的接管工作是比较琐碎的。

### ③、存储的虚拟化分

一是游戏的资源部分，比如执行程序、图片、声音等等。这些资源文件都是只读的，需要一个共享存储来放这些文件，因为这些文件体积比较大，通常一个游戏需要几十个G的容量，如果全部都放在本地节点上的话，对节点的存储容量要求很大，而且以后更新维护起来也比较困难。所以我们用 NAS 来共享这些文件，这么做的网络 I/O 开销会非常大，后面我会介绍如何来优化这一块。第二是用户配置和存档数据等等可变数据，这些数据需要集中化存储，同时可能存在跨机房的访问需求。用户离机房越近延迟越小，所以需要多地、异地部署服务器，让玩家在全球漫游访问你的服务，这需要有跨机房文件共享的能力。

### ④、其他需要适配的内容

比如游戏一般都是单实例，我们需要绕过游戏的防多启动机制。还有些游戏无法后台窗口运行，我们需要通过 API Hook 的方式，让游戏认为它处于一个正常的状态。最理想的适配方式是通过 SDK，让 CP 来适配你的云游戏平台，但目前来说还不实际，因为云游戏的商业化还没有完全的落地，需要技术去慢慢的推进。



### ⑤、音视频编码技术

视频流采用的是 H.264 编码，主要是 720P/1080P@30fps，1080P@60fps 对网络和硬件的要求过高，暂时还做不到。音频编码使用AAC。因为标准的封装格式不含控制流，不能传输用户的操作数据，所以我们自己定义了一种封装格式，简单的把 H.264 和 AAC 的裸流封装起来传送给客户端。

目前用软件编码器基本不可行，一路视频编码就要消耗掉一个CPU核的资源，跑个三四路就把 CPU 资源吃光了，游戏就没办法运行了。幸运的是三大硬件厂商 Intel、AMD 和 NVIDIA 都推出了自己的硬件编码器，Intel的CPU自带硬件编码器，支持20+路的720P实时编码没有问题。NVIDIA 的硬件编码性能更高，可以直接对GPU的 FrameBuffer 做编码并传到 CPU 上，节省了很多内存的拷贝，性能是最好的。

### ⑥、视频编码的参数调优

首先避免使用 B 帧以减小延迟；较大的 GOP 设置来减少 I 帧的比例，保证每一帧消耗的码率都在一个最大可控的范围内；0 延迟设置，保证每输入一帧数据编码器都立刻输出这帧的编码数据，避免编码器缓冲帧数据；bitrate控制，使用固定比特率的算法是不适合的，因为游戏中经常会存在一段时间的静止画面，此时比特率很低，对接下来的变化帧编码器就会分配大量的比特来编码，这就会造成这一帧数据特别巨大，从而带来了额外的网络数据传输延迟。所以我们采用了自适应算法，在保证比特率总体在最大范围内的同时，保证每一帧消耗的码率都在一个最大可控的范围内，确保每帧的数据传输延迟可控。

### ⑦、终端的视频解码优化

H264 的解码是比较头疼的，因为android平台适配起来比较痛苦，尤其是它的硬件解码坑非常多。如果直接使用mediacodec封装的硬件解码器，那个延迟非常高，基本没有办法用。有一些芯片厂商会提供一个后门，让你把缓冲关掉直接输出画面，但是这需要对接具体的芯片厂商，无法做到通用，只适合一些机顶盒类的产品。所以还是需要用软件解码的方式来支持 0 延迟的输出。android设备的性能参差不齐，早期的低端芯片性能不满足实时解码 ，需要利用 GPU 做一些加速。

### ⑧、网络传输的优化

用UDP传输的话，因为H264 本身不支持容错，一旦丢包就会出现花屏，在下一个I帧到来前都无法恢复，通常要持续好几秒，严重影响用户体验，无法接受；而TCP 丢包的话只是出现几百毫秒的卡顿，实测还是可以接受的，所以我们放弃了 UDP 协议传输，利用TCP在网络层做一些调优使延迟降低。实测下来，现在的宽带网络延迟基本没有问题，主要问题反而是在用户侧的 WiFi 上，一旦出现无线信号干扰，网络抖动会比较厉害。

### 九、服务器和客户端的同步算法

我们的云游戏把所有环节的缓冲都关掉了，全部是零延迟自出，原来缓冲设计的目的就是为了抵抗颠簸，比如网络的颠簸、或某一个编解码环节出现了抖动，通过缓冲把这个抖动抹平，现在把缓冲都关掉后对同步会造成很大的影响。有很多因素会造成颠簸，比如服务器发送数据过快，客户端来不及消费，造成的结果就是延迟非常大。所以我们自己设计了一套算法来解决这个同步的问题。具体的做法就是让客户端在完成一帧画面的显示后向服务器反馈一个消息，服务端根据客户端反馈的消息就知道客户端消费到了第几帧，跟服务器现在编码的帧数做比较，在一定的阈值内就继续传输下一帧，否则等待客户端的确认消息，直到客户端赶上来。这样做的结果就是当颠簸发生时服务器能及时感知并停止发送数据，等颠簸消除后再继续发送最新的游戏画面，实测下来获得了比较理想的同步效果。

存储的优化。只读资源数据是放在 NAS 上的，几百个游戏共享一个 NAS，加载游戏时的网络 I/O 开销非常大，所以我们做了一个优化来本地缓存这些共享文件，利用dokan实现了一个虚拟磁盘来访问资源文件，再把虚拟磁盘重定向到NAS上，同时利用节点的本地 SSD 硬盘来缓存热点文件，从而降低了网络 I/O 的开销。

### shi、更多的云游戏玩法

旁观模式，一个玩家玩的时候其他玩家可以接入这个视频流，看他怎么玩；对战模式，其他玩家可以切到这个游戏流里面两个人在一起对战；还有直播模式，把视频流封装为 HLS，推送到 CDN 上进行直播，这是非常流行的主播模式，云游戏都可以支持。



## 5、云游戏运维方面的挑战

云游戏需要维护大量的服务器节点，而且跟普通的服务器管理不一样，需要自己造一些轮子。由于所有的硬件资源都是高负荷运行，我们要最大化的增加硬件利用率，一般的服务器 CPU 占 10% 就很不错了，而云游戏的 CPU 都是在接近 100% 的情况下运行，另外还需要GPU的参与，这导致了硬件的可靠性相对比较低。

软件因为没有隔离性，可靠性也会降低，一旦出现问题怎么维护、怎么恢复，成了比较麻烦的问题，因为没有现成的方案，就我们需要自己设计服务器集群来解决这些问题。另外还有跨机房部署的问题。

硬件方案的选型，我们主要有三套方案，一套是 GRID 显卡方案，这是 NVIDIA 为云游戏专门设计的专业显卡，上面带有编码器可以将游戏画面直接编码输出，但它的缺点是价格比较昂贵，一台服务器的硬件成本大约在 5 万元左右。

还有就是消费级独显方案，去掉了昂贵的专业显卡的同时还能获得更好的GPU性能，所以这套方案的性价比要高很多，每路并发的硬件成本可以降低到 500 元以下。

最后一个方案是 Intel 核显方案。完全不需要用独立显卡，但 Intel 核心显卡的性能偏弱，运行大型的 3D 游戏会比较吃力，运行一些休闲游戏没有问题。这个方案的优点是不需要显卡，1U 的尺寸下可以装入多个节点，集成度提高，而且易于维护，也是一个值得考虑的方案。

下面来解释一下云游戏一下集群的概念。Node（节点）对应一台物理计算机，一个节点可以同时运行多个游戏实例为用户提供服务。多个节点组成一个 Group（节点组），一个Group内包含了若干节点和NAS，对应于一个机柜， 多个机柜用万兆交换机串连起来，部署在一个机房，称之为 Cluster（集群），再上面一层是云游戏平台，包括用户的入口管理、登录计费等，可以跨越多个机房。

  

User Profile Storage  用来存放用户的存档数据，Log Storage 用来存储日志数据，还有数库等等。

Group 内的各 Node 组成对等网络，可以任意添加或者删除 Node，各个 Node 通过竞争算法选举出来一个 Master，由 Master 与 Manager 建立连接，对整个 Group 进行管理，如果Master出现故障则由剩余的节点重新选举出一个新的Master进行接管，从而保证了任何节点的故障不会影响到其他节点的正常服务。在Node 上仅需要安装好操作系统和 Daemon 服务，无须配置，即插即用。Node daemon对服务器进行管理和监控；游戏文件存放于 NAS 上，由各 Node 共享；内网/外网流量隔离，防止互相影响。

Manager 用于对集群内的所有 Node 进行管理， 配置/更新/上线/负载均衡/监控，游戏数据管理更新，用户数据管理等等。提供 web 后台给运维操作，实现运维的自动化和可视化操作。Manager使用双机热备模式实现高可用，避免单点故障造成整体系统瘫痪。

日志和监控。我们需要有完整的日志来记录和追踪系统行为，保障整个系统的可维护性。同时系统会实时监控每个游戏实例以及 Node 的状态，包括 cpu、gpu、网络io 的使用率，游戏帧率、延迟等等数据，所有数据保存下来，后面可以通过一些数据分析的手段来找到性能的瓶颈，然后再针对性的进行优化，进一步优化我们的系统。

   

提问：对家用的wifi做一些支持，能详细说说吗？

乔捷：首先，要提示用户wifi信号不好会造成延迟，终端检测到网络信号不好时及时的提示用户。其次，对于网络延迟的抖动，我们的同步控制算法能够补偿一部分抖动。最后，可以在服务器上调优一下TCP参数，比如说减小数据重传的超时时间，加快数据包的重传，可以有效缓解抖动。



提问：对用户体验有影响吗？

乔捷：目前肯定有，我们是标清的 720P 的画面质量，因为要考虑硬件成本和网络传输成本。但随着成本的逐步降低，未来要支持1080P甚至4K画质也是没有问题的。



提问：对于 CP 的开发模式有哪些影响？

乔捷：目前没有影响，我们只是买一个授权，然后由我们进行对接，不需要CP方去改动代码。当然如果 CP 方愿意来对接我们的SDK话那是最好的，可以加入对战、排名、内购等各种功能，利用云游戏的特点为游戏增加更多的玩法。



提问：我们这么多年下来的计算，最早开始所有的计算都是在中心，随着终端计算能力的增强，计算很多功能都到终端上面去，现在你的方案是把所有的终端都放在中心，这对服务器成本要求很高？如果能够容纳一些用户同时运行大型游戏，服务器成本是否会非常高？

乔捷：对。为什么我们要中心化？因为终端的种类太多了，手机、平板、电视、PC，这么多平台，你一个游戏要去移植这么多平台，本身的工作量就非常大，而且用户要去下载安装，推广的成本非常高，网络游戏单个用户的获客成本已经到了几十到上百块钱。所以，服务器成本表面上看是有点高，但是算上开发成本分发成本推广成本，这点服务器成本已经完全可以接受。

这就和视频一样，最早我们看视频是买光盘的，后来有了网络以后是从网络上下载，而现在宽带普及了之后已经没有人下载了，都是直接视频点播，因为它方便，门槛越低越容易被用户接受，现在还会有人买光盘吗？基本上已经没有了吧。电视电脑都不是我们的工具了，大家现在用的比较多就是手机。计算资源越来越中心化集中，管理成本不断降低。现在买游戏机、ps3、ps4，每隔 5 年换一个游戏机，以后不需要游戏机更新换代了，更新换代对于厂商来说是一个比较痛苦的过程，有一个漫长的迁移过程。将来根本不用关心什么硬件，比如今年的“吃鸡”游戏非常流行，但是很多玩家的显卡性能不足，跑不起来。将来游戏都是放在服务器上跑，用户根本不用担心跑不跑得动，接上就可以玩。一旦这个服务模式成立，硬件厂商都会向这个方向投入资源，最早2011年的时候我们就预研过云游戏的技术，当时做了以后就放弃，后来看到这个机会以后推出来了grid显卡，一下子拉很高，看这一块商业模式什么时候落地，现在还是在探索的过程当中，将来是大趋势。



提问：除了服务上面成本，要求终端的网络非常好吗？对解码要求高么？

乔捷：对，因为网络非常普及的情况下，宽带已经无处不在了，所以这个问题基本已经被解决了。现在的主流中低端芯片可以实时软件解码720P的视频流。



提问：我知道游戏有很多种类，目前云游戏技术支持的范围怎么样？云游戏的交互目前为止是否还很有限？

乔捷：主要是主机游戏，用手柄玩的游戏。看类型，使用键盘鼠标的游戏比如FPS在电脑上比较好操作，在电视上就不太方便了，目前主要还是适配手柄操作的游戏。





# 二、RTC中线程说明



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
 
