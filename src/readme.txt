    以下是本目录下所包含的各个源程序的主要目的,关联文件,以及要注意的内容;新增的函数一定要按文件的主要目的归类,关联文件,以及要注意的内容应在设计过程中根据应用的实际情况适当修改和填写.此文件夹中不能再增加新的.C文件.

card.c		
主要目的:刷卡文件,读卡,得到卡号,卡有效期,一,二,三磁道内容.
关联方:magtxn.c
要注意的内容:可包含预拨号.

comm.c		
主要目的:通讯文件,通迅初始化,拨号,发送,接收,挂断,通迅设置
关联方:magtxn.c managetxn.c
要注意的内容:挂断的方式

debug.c		
主要目的:调试文件,常用的有把通迅发送,接收到的数据送到串口,PC端可通过POSDEBUG来监控调试.
关联方:comm.c,magtxn.c,managetxn.c等
要注意的内容:正式提交必须删除其它文件调用的调试函数.

inout.c		
主要目的:输入输出文件,密码输入等
关联方:magtxn.c
要注意的内容:

magtxn.c	
主要目的:卡交易文件,消费,撤消,预授权等卡交易函数
关联方:main.c packet.c comm.c card.c,print.c等等
要注意的内容:

main.c		
主要目的:主程序入口,主菜单,分菜单
关联方:magtxn.c manage.c managetxn.c settle.c 
要注意的内容:

manage.c	
主要目的:管理文件,包含参数设置菜单,密钥管理
关联方:param.c,oper.c等
要注意的内容:

managetxn.c	
主要目的:管理类交易文件,如签到,签退,参数下载等
关联方:main.c packet.c comm.c manage.c等
要注意的内容:

oper.c		
主要目的:操作员管理文件,包含操作员初始化,增加/删除操作员等
关联方:main.c manage.c,param.c等
要注意的内容:

packet.c	
主要目的:打包解包文件, ISO8583包各域的定义,打包,解包,计算MAC等
关联方:magtxn.c managetxn.c等
要注意的内容:ISO8583包各域的定义

param.c		
主要目的:参数管理函数,包含参数设置,参数保存,参数读取等等
关联方:manage.c等
要注意的内容:参数的保存是文件还是FRAM

print.c		
主要目的:打印文件,流水打印,结算打印,明细打印,汇总打印,故障单打印等
关联方:magtxn.c,settle.c等
要注意的内容:

settle.c	
主要目的:结算文件,结算,批上送,批上送完成交易
关联方:main.c,print.c等
要注意的内容:

tms.c		
主要目的:TMS文件,远程更新,远程下载等
关联方:main.c
要注意的内容:只有需要TMS的程序才包含这个文件

tool.c		
主要目的:工具文件,用于各函数调用的工具函数,如金额转换,日期设置等等
关联方:其它所有的.C文件
要注意的内容:

water.c		
主要目的:流水文件,初始化流水,保存流水,读流水,查流水等
关联方:main.c magtxn.c print.c等
要注意的内容:

ic_op.c		
主要目的:IC卡操作文件,如IC卡上电,下电,读,写,校验等
关联方:magtxn.c main.c等
要注意的内容:

emv_acq.c	
主要目的:EMV收单行相关文件,如55域打包,EMV存流水,更新公钥,更新AID等
关联方:emv_interface.c emv_mana.c emv_txn.c tlv.c 
要注意的内容:

emv_interface.c	
主要目的:EMV卡接口文件,导入EMV库,装载EMV库,初始化EMV参数,下载EMV参数等
关联方:emv_acq.c emv_mana.c emv_txn.c tlv.c 
要注意的内容:

emv_mana.c	
主要目的:EMV卡管理文件,EMV参数传递,查EMV交易流水等
关联方:emv_acq.c emv_interface.c emv_txn.c tlv.c 
要注意的内容:

emv_txn.c
主要目的:EMV卡交易文件,EMV卡消费,EMV卡预授权,EMV卡查余额等
关联方:emv_acq.c emv_interface.c emv_mana.c tlv.c 
要注意的内容:

tlv.c 	
主要目的:TLV标签定义文件,增加TLV标签等
关联方:emv_txn.c
要注意的内容:

