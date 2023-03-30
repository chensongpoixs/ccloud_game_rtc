# xinput


在linux中，xinput是xorg中的一个工具，是用来配置和测试X输入设备的工具，包括但不限于键盘、鼠标等。直接键入xinput或者跟上list参数，就会列出本机所有输入设备，包括名称、ID、层级；而使用“xinput list-props”命令可列出某个设备的详细信息，后接参数为设备ID



xinput是什么

xinput是用来配置和测试X输入设备的工具，包括但不限于键盘，鼠标等，在这里用作关闭触摸板，基于同样的道理可以关闭键盘等。

xinput是Linux下的一款配置测试X输入设备的神器，具有以下功能（我们将会用到的）

- xinput (list) : 直接键入xinput或者跟上list参数，就会列出本机所有输入设备，包括名称、ID、层级

- xinput list –id-only : 仅仅列出设备ID（序号，每次开机都可能改变）

- xinput map-to-output : 将输入设备映射到输出，比如将触屏映射到某个屏幕

- xinput list-props : 列出某个设备的详细信息，后接参数为设备ID\



```
xinput list 

chensong@ubuntu:~/Work/20220805_webrtc/src$ xinput  list
⎡ Virtual core pointer                    	id=2	[master pointer  (3)]
⎜   ↳ Virtual core XTEST pointer              	id=4	[slave  pointer  (2)]
⎜   ↳ VMware VMware Virtual USB Mouse         	id=7	[slave  pointer  (2)]
⎜   ↳ VirtualPS/2 VMware VMMouse              	id=9	[slave  pointer  (2)]
⎜   ↳ VirtualPS/2 VMware VMMouse              	id=10	[slave  pointer  (2)]
⎣ Virtual core keyboard                   	id=3	[master keyboard (2)]
    ↳ Virtual core XTEST keyboard             	id=5	[slave  keyboard (3)]
    ↳ Power Button                            	id=6	[slave  keyboard (3)]
    ↳ AT Translated Set 2 keyboard            	id=8	[slave  keyboard (3)]


```


查看设备的详细信息：


```
xinput list-props 6
chensong@ubuntu:~/Work/20220805_webrtc/src$ xinput  list-props 6
Device 'Power Button':
	Device Enabled (127):	1
	Coordinate Transformation Matrix (129):	1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000
	libinput Send Events Modes Available (248):	1, 0
	libinput Send Events Mode Enabled (249):	0, 0
	libinput Send Events Mode Enabled Default (250):	0, 0
	Device Node (251):	"/dev/input/event0"
	Device Product ID (252):	0, 1

```


把其中的Device Enabled设置为0即可禁用：

```
xinput set-prop 6 'Device Enabled' 0
```



或者使用Device Enabled后面的数字也可以禁用：


```
xinput set-prop 6 127 0
```


启用进行相反的处理即可，设置为1：


```
xinput set-prop 12 'Device Enabled' 1
# 或
xinput set-prop 12 154 1
```



https://xcb.freedesktop.org/manual/group__XCB__Input__API.html


