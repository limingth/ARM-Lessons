
课堂笔记
========

day1: 
======

#1 环境搭建

## 网络设置
	ip 地址设置为自动分配	
	测试网关 ping 192.168.1.1 
	测试外网 ping 8.8.8.8 （域名）
	测试网页 chrome www.google.com
	测试共享 \\192.168.1.100
	测试 VNC 桌面共享： 	192.168.1.100 有线 akaedu	
				192.168.1.131 无线 123

## 凌波多媒体教学软件
	\\192.168.1.100\tools\lingbo6.94
	安装学生端，应用破解补丁
	连接教师机，远程查看桌面

## 课程安排介绍
	上课时间	周一/周二/周三  周五/周六  （周四周日休息）
			上午 8:30 - 12:00	下午 13:00 - 17:00
	课时 	8天 * 7.5学时/天 = 60 学时
	班长发言

## 工具安装
	ADS1.2
		1. 选择 full （不是 typical） 包含 pdf 目录
		2. install licence from file (browser zip)
			选择 LICENSE.DAT
		成功之后运行 CodeWarrior 代码战士，看到 IDE 就是成功
		启动 cmd 窗口，运行 armcc
		path 命令查看安装路径: C:\Program Files\ARM\ADSv1_2\bin
	FoxitReader
	Ultraedit
	make.exe
	H-jtag.exe
	Terminal.exe
	Git for windows

## 开发板连接和测试
	开发板测试
		2440 
			有7段数码管和 8个LED灯
			串口线 （双母头）
		2410 
			没有7段数码管和 8个LED灯
			      
		1. 电源线 5v 内正+ 外负- 
		2. 串口线 公口，母口
		3. 并口线 + Jtag 小板子
		4. 计算机 pc 的并口
		
	测试成功： 串口有输出 115200
	如果不成功： 	1. 波特率有问题 改为 19200 
			2. 程序错误，改为用 Jtag 工具检测
				安装 H-JTAG V0.9.2.EXE

	H-Jtag 测试
		检测 CPU -> ARM920T
		烧写 bin -> S3C2440/S3C2410 + K9F1208 -> RAM at 0 -> Plain Binary
	
	串口测试
		uboot 3秒之内按回车键，进入到 shell
		vivi 3秒之内按空格键，进入到 shell

	USB转串口	usbtoserial product 

	超级终端 （Terminal） hypertrm.exe (dll)
		1. 波特率 115200
		2. 数据流控制 无

## bin 文件的下载和执行
	### 2440.bin
	AKAE2440 # loadb
	## Ready for binary (kermit) download to 0x32000000 at 115200 bps...
	## Total Size      = 0x00000058 = 88 Bytes
	## Start Addr      = 0x32000000
	AKAE2440 #
	AKAE2440 # go 0x32000000
	## Starting application at 0x32000000 ...


	### 2410.bin
	vivi> load
	invalid 'load' command: too few(many) arguments
	Usage:
	  load <flash|ram> [ <partname> | <addr> <size> ] <x|y|z|t>
	vivi> load ram 0x32000000 128 x	
	AKAE2440 # loadb
	## Ready for binary (kermit) download to 0x32000000 at 115200 bps...
	## Total Size      = 0x00000058 = 88 Bytes
	## Start Addr      = 0x32000000
	AKAE2440 #
	AKAE2440 # go 0x32000000
	## Starting application at 0x32000000 ...

#2 芯片手册导读

## 福昕阅读器工具使用技巧
	书签功能
	ctrl + 滚轮 实现放大缩小
	ctrl + f 搜索功能
	ctrl + shift + n 跳转到页码
	图形标注工具 (菜单 -> 视图 -> 工具栏 下面)
	
## 硬件原理图介绍
	核心板
	SoC: s3c2440 (289-pin)
	NandFlash： K9F1208 (48-pin)	64M (NorFlash)
	SDRAM:  K4S561632 (2chips) 32M * 2 = 64M 
	Y2: 12M 外部时钟供给SoC
	
	底板
	J1 J2 插头
	LED0 - LED7 8个 LED 灯
	Beep 蜂鸣器
	SW1 - SW4 用户按键 + Reset 键
	4 位拨码开关
	SEG7 7段数码管
	MAX3232 电平转换，给串口用 5v - 15v
	UART0, UART1 串口
	USB (J8, J7)
	SD-CARD (J10)
	CS8900A (U7) 网卡
	CH7005C (U16) VGA 转换芯片 -> VGA 接口
	UDA1341 (U8) 声卡芯片 -> 耳机接口
	IMP811 (U14) 复位芯片 -> 输出reset信号
	JTAG (J19) Jtag接口(14/20针) (TDI/TDO, TCK, TMS, TRST)
	CPLD (U15) 可改变硬件连接逻辑
	IDE (J15) 40针 
	LCD (J12) 50针
	
#3 基本开发流程
	make.exe - copy it to c:\program files\arm\adsv1_2\bin
	armcc test.c -o test.axf
	fromelf -bin test.axf -o test.bin
	串口输出测试程序： 通过 UTXH 串口发送寄存器，循环输出字符 'a'	
	
课后作业
	提前预习芯片手册 io ports, clock, uart 章节
	

day2: 
======	
#1 S3C2440/2410 芯片手册综述
## 芯片里面有什么？ (看完手册后，找出最重要的3个组成部分)
	1. ARM920T 内核(处理器内核) - 运算
	2. BUS (AMBA总线)
	3. Peripheral Controller (外设控制器) - 控制
		1. Clock & Power (心跳和血液)
		2. GPIO (管脚复用功能)
		3. Memory Controller (存储控制器)
		4. Interrupt Controller (中断控制器)
		5. DMA Controller (直接存储访问)

#2 GPIO 控制器
## S3C2440 Pins (管脚)
	Pin Assignments 管脚定义
		17 * 17 = 289 FBGA (Fine-Pitch Ball Grid Array) 球栅阵列
	Pin Number 管脚编号 (物理位置)
	Pin Name 管脚名称 (逻辑含义)
		以串口 UART0 为例
		TXD0 - GPH2 (复用)
		RXD0 - GPH3
	Selectable Pin Functions (可选择的管脚功能) 
	Signal Descriptions  (信号描述)
		以 NandFlash 为例
		子板原理图 -> P3 (NandFlash) 
		2440 芯片手册 
			-> P49 (Signal Descriptions)  信号描述
			-> P275 (Pin Functions) 管脚功能
		nFCE	片选信号		GPA22
		CLE	命令锁存使能信号	GPA17
		ALE	地址锁存使能信号	GPA18
		nFWE	写使能信号		GPA19
		nFRE	读使能信号		GPA20
		RnB	Ready/not Busy就绪/忙等待信号	无
	GPA Port 管理 P280
		GPACON 配置寄存器
		GPADAT 数据寄存器
	SFR (Special Function Register) 特殊功能寄存器
		1 pin -> control bits 位数
			-> Input/Output (标准输入和输出)
			-> Selectable Function (复用功能)
			-> Default Value (默认值)
## GPIO 编程
	assign EINT[0] = KBIN[1];	-> GPF0
	assign EINT[2] = KBIN[0];	-> GPF2
	assign KBOUT[0] = GPB9;		-> GPB9
	assign KBOUT[1] = GPB8;		-> GPB8
	
#3 Clock 时钟管理

#4 UART 串口驱动
	
	
	
	
	
	
	







