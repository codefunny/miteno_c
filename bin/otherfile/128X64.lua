-----------------------------------
--* file 128*64.lua
--* brief 本文件适用于屏幕分辨率为128*64的机具.
--* version  1.0
--* author hezk
--* date 20140701
-----------------------------------

-------------------------------------------
--main.c--
-------------------------------------------
function LuaSetUIStyle()
	UI.PubSetUiStyle(0, "↓", "↑");
	UI.PubSetUIFunType(0);
	UI.PubClearAll();
	return 0;
end

function LuaUnSetUIStyle()
	return 0
end

function LuaFirstRunChk()
	UI.PubDisplayGen(2, "初始化POS");
	UI.PubDisplayGen(4, "请稍候....");
	UI.PubUpdateWindow();
	return 0;
end


function LuaMenuFuncSel()
	UI.PubClearAll();
	return UI.PubDispPicList(0, 10, nil, "1手机号验券|2串码验券|3银行卡验券|4撤  销|5退  货|6打  印|7管  理", 60, 0);
end

function LuaMenuFunc(szMenu)
	UI.PubClearAll();
	return UI.PubMenu("梅泰诺", 2, 5, 0, 0, 60, 0, szMenu);
end

function LuaEnableDispDefault(szSoftVer, szShowName, szShowInfo)
	UI.PubClearAll();
	UI.Picture(0, 0, 64, 48, "TT64x54.bmp");
	UI.PubDisplayStr(8, 1, 0, szShowName);
	UI.PubDisplayStr(8, 2, 0, szSoftVer);
	UI.PubDisplayStr(8, 3, 0, szShowInfo);
	UI.PubUpdateWindow();
	UI.PubShowStatusBar();
	return 0;
end

function LuaDisableDispDefault()
	UI.PubCloseShowStatusBar();
	return 0;
end

-------------------------------------------
--comm.c
------------------------------------------
function LuaCommInit()
	UI.PubClearAll();
	UI.PubDisplayGen(2, "初始化通讯");
	UI.PubDisplayGen(3, "请稍候...");
	UI.PubUpdateWindow();
	return 0
end

function LuaCommConnect(nDialFlag)
	UI.PubClear2To4();
	if nDialFlag == 1 then
		UI.PubDisplayStrInline(0, 2, "正在拨号...");
	else
		UI.PubDisplayStrInline(0, 2, "正在连接网络...");
	end
	UI.PubDisplayStrInline(0, 4, "按<取消键>退出");
	UI.PubUpdateWindow();
	UI.PubSetShowXY(9,2,0);
	return 0
end

function LuaCommSend()
	UI.PubClear2To4();
	UI.PubDisplayStrInline(0, 2, "已连接贴贴中心");
	UI.PubDisplayStrInline(0, 3, "处理中...");
	UI.PubDisplayStrInline(0, 4, "发送数据包...");
	UI.PubUpdateWindow();
	return 0
end


function LuaCommRecv()
	UI.PubClear2To4();
	UI.PubDisplayStrInline(0, 2, "已连接贴贴中心");
	UI.PubDisplayStrInline(0, 3, "处理中...");
	UI.PubDisplayStrInline(0, 4, "接收返回...");
	UI.PubUpdateWindow();
	UI.PubSetShowXY(9,4,0);
	return 0
end

