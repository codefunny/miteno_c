-----------------------------------
--* file 128*64.lua
--* brief ���ļ���������Ļ�ֱ���Ϊ128*64�Ļ���.
--* version  1.0
--* author hezk
--* date 20140701
-----------------------------------

-------------------------------------------
--main.c--
-------------------------------------------
function LuaSetUIStyle()
	UI.PubSetUiStyle(0, "��", "��");
	UI.PubSetUIFunType(0);
	UI.PubClearAll();
	return 0;
end

function LuaUnSetUIStyle()
	return 0
end

function LuaFirstRunChk()
	UI.PubDisplayGen(2, "��ʼ��POS");
	UI.PubDisplayGen(4, "���Ժ�....");
	UI.PubUpdateWindow();
	return 0;
end


function LuaMenuFuncSel()
	UI.PubClearAll();
	return UI.PubDispPicList(0, 10, nil, "1�ֻ�����ȯ|2������ȯ|3���п���ȯ|4��  ��|5��  ��|6��  ӡ|7��  ��", 60, 0);
end

function LuaMenuFunc(szMenu)
	UI.PubClearAll();
	return UI.PubMenu("÷̩ŵ", 2, 5, 0, 0, 60, 0, szMenu);
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
	UI.PubDisplayGen(2, "��ʼ��ͨѶ");
	UI.PubDisplayGen(3, "���Ժ�...");
	UI.PubUpdateWindow();
	return 0
end

function LuaCommConnect(nDialFlag)
	UI.PubClear2To4();
	if nDialFlag == 1 then
		UI.PubDisplayStrInline(0, 2, "���ڲ���...");
	else
		UI.PubDisplayStrInline(0, 2, "������������...");
	end
	UI.PubDisplayStrInline(0, 4, "��<ȡ����>�˳�");
	UI.PubUpdateWindow();
	UI.PubSetShowXY(9,2,0);
	return 0
end

function LuaCommSend()
	UI.PubClear2To4();
	UI.PubDisplayStrInline(0, 2, "��������������");
	UI.PubDisplayStrInline(0, 3, "������...");
	UI.PubDisplayStrInline(0, 4, "�������ݰ�...");
	UI.PubUpdateWindow();
	return 0
end


function LuaCommRecv()
	UI.PubClear2To4();
	UI.PubDisplayStrInline(0, 2, "��������������");
	UI.PubDisplayStrInline(0, 3, "������...");
	UI.PubDisplayStrInline(0, 4, "���շ���...");
	UI.PubUpdateWindow();
	UI.PubSetShowXY(9,4,0);
	return 0
end

