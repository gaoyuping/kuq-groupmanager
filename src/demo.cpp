#include "cqsdk/cqsdk.h"
#include <vector>
#include <Windows.h>
#include <winnt.h>
#include <ini.h>
// namespace cq::app 包含插件本身的生命周期事件和管理
// namespace cq::event 用于注册 QQ 相关的事件处理函数
// namespace cq::api 用于调用酷 Q 提供的接口
// namespace cq::logging 用于日志
// namespace cq::message 提供封装了的 Message 等类

// 插件入口，在静态成员初始化之后，app::on_initialize 事件发生之前被执行，用于配置 SDK 和注册事件回调
int icount = 0;
struct KeyMsg {
    std::string strkey;
    std::string strmsg;
};
struct inidata{
    int groupid;
    std::string strin; //进群欢迎语
    std::vector<KeyMsg> keymsg;
};
std::vector<inidata> g_data;
#define  bufferlen 4096
CQ_MAIN {
    cq::app::on_enable = [] {
        // cq::logging、cq::api、cq::dir 等命名空间下的函数只能在事件回调函数内部调用，而不能直接在 CQ_MAIN 中调用
        cq::logging::debug(u8"启用", u8"插件已启动");
		TCHAR exeFullPath[MAX_PATH]; // Full path
        GetModuleFileName(NULL, exeFullPath, MAX_PATH);
        std::string strFullPath = (std::string)(exeFullPath);
        int nStart = strFullPath.find_last_of(TEXT("\\"));
        std::string strPath = strFullPath.substr(0, nStart + 1);
        std::string strExeName = strFullPath.substr(nStart + 1, strFullPath.length() - 4);
        strPath.append("app\\demo.ini");
        cq::logging::info(u8"demo-ini path ", strPath);
		ini::IniFile myIni;
		myIni.load(strPath);
        for (int i = 1; i < 100; i++) {
            std::string strgroup = "group" + std::to_string(i);
            //int value = GetPrivateProfileInt("groupdata", strgroup.c_str(), 0, strPath.c_str());
			int value = myIni["groupdata"][strgroup].as<int>();
            if (value < 1) 
			{
                cq::logging::info(u8"demo-get groupend ", std::to_string(i));
                break;
			}
            inidata ss;
            ss.groupid = value;
            g_data.push_back(ss);
            cq::logging::info(u8"demo-group", std::to_string(value));
        }
        char buffer[bufferlen + 1];
		char buffertmp[bufferlen + 1];
		int itmplen = 0;
        std::string strkey;
		std::string strtmp;
		for (int i = 0; i < g_data.size(); i++)
		{
            std::string strgroup =  std::to_string(g_data[i].groupid);
			//GetPrivateProfileString(strgroup.c_str(), "inmsg","", buffer, bufferlen, strPath.c_str());
			g_data[i].strin = myIni[strgroup]["inmsg"].as<std::string>();

			for (int j = 1; j < 101; j++) {
                strkey = "ret" + std::to_string(j) + "rule";
				cq::logging::info(u8"rule name", strkey);
				strtmp = myIni[strgroup][strkey].as<std::string>();
                if (strtmp.length() == 0) 
				{
                    break;
				}
                KeyMsg keymsg;
				keymsg.strkey = strtmp;
                strkey = "ret" + std::to_string(j) + "msg";
                strtmp = myIni[strgroup][strkey].as<std::string>();
				cq::logging::info(u8"buffer", strtmp);
				keymsg.strmsg = strtmp;
                g_data[i].keymsg.push_back(keymsg);
			}
			cq::logging::info(u8"keymsg size ", std::to_string(g_data[i].keymsg.size()));
		}
    };
	/***************************************************************************************************************/
    cq::event::on_private_msg = [](const cq::PrivateMessageEvent &e) {
        cq::logging::debug(u8"消息", u8"收到私聊消息：" + e.message + u8"，发送者：" + std::to_string(e.user_id));

        if (e.user_id != 1002647525) return;

        try {
            cq::api::send_private_msg(e.user_id, e.message); // echo 回去

            cq::api::send_msg(e.target, e.message); // 使用 e.target 指定发送目标

            // MessageSegment 类提供一些静态成员函数以快速构造消息段
            cq::Message msg = cq::MessageSegment::contact(cq::MessageSegment::ContactType::GROUP, 201865589);
            msg.send(e.target); // 使用 Message 类的 send 成员函数
        } catch (const cq::exception::ApiError &err) {
            // API 调用失败
            cq::logging::debug(u8"API", u8"调用失败，错误码：" + std::to_string(err.code));
        }

        e.block(); // 阻止事件继续传递给其它插件
    };
	/***************************************************************************************************************/
    cq::event::on_group_msg = [](const auto &e /* 使用 C++ 的 auto 关键字 */) {
		int igid = 0;
		//cq::logging::info(u8"demo-retdata11111111111111 ","0000000000");
		for (int i = 0; i < g_data.size(); i++)
		{
			if (g_data[i].groupid == e.group_id)
			{
				igid = i;
				cq::logging::info(u8"gouupid ", std::to_string(e.group_id));
				goto msg1;
			}
		}
		//cq::logging::info(u8"demo-retdata11111111111111 ","11111111111111111111");
        return;
		cq::logging::info(u8"demo-retdata11111111111111 ","222");
		msg1:;
		const auto memlist = cq::api::get_group_member_list(e.group_id); // 获取数据接口
        icount++;
        for (int i = 0; i < memlist.size(); i++) {
			if (memlist[i].user_id == e.user_id && (int)memlist[i].role == 1)
			{
				cq::logging::info(u8"user_id ",  std::to_string(e.user_id));
				goto msg2;
			}
        }
		//cq::logging::info(u8"demo-retdata11111111111111 ","113333333333333333333333");
        return;
		//cq::logging::info(u8"demo-retdata33333333333333333", "11");
	msg2:;
		std::string getsendmsg = e.message;
		cq::logging::info(u8"demo-get 规则数量 ", std::to_string(g_data[igid].keymsg.size()));
		for (int i =0; i < g_data[igid].keymsg.size(); i++)
		{
			cq::logging::info(u8"find msg", std::to_string(i));
			if (std::regex_match(getsendmsg, std::regex(g_data[igid].keymsg[i].strkey)))
			{ 
				cq::logging::info(u8"sendmsg id ",  std::to_string(i));
				cq::logging::info(u8"sendmsg", g_data[igid].keymsg[i].strmsg);
				cq::message::send(e.target, g_data[igid].keymsg[i].strmsg);
			}
		}
    };
	/***************************************************************************************************************/
	cq::event::on_group_member_increase = [](const auto &e /* 使用 C++ 的 auto 关键字 */) {
		int igid = 0;
		for (int i = 0; i < g_data.size(); i++)
		{
			if (g_data[i].groupid == e.group_id)
			{
				igid = i;
				goto msg1;
			}
		}
		return;
	msg1:;
		if ("" == g_data[igid].strin)
		{
			return;
		}
		std::string sendmsg = "[CQ:at,qq=";
		sendmsg.append(std::to_string(e.user_id)).append("]");
		sendmsg.append(g_data[igid].strin);
		//cq::logging::info(u8"demo-member increase ", sendmsg);
		cq::message::send(e.target, sendmsg);
	};
	
}

// 添加菜单项，需要同时在 <appid>.json 文件的 menu 字段添加相应的条目，function 字段为 menu_demo_1
CQ_MENU(menu_demo_1) {
    cq::logging::info(u8"菜单", u8"点击了示例菜单1");
    try {
        cq::api::send_private_msg(10000, "hello");
    } catch (const cq::exception::ApiError &) {
        cq::logging::warning(u8"菜单", u8"发送失败");
    }
}

// 不像 CQ_MAIN，CQ_MENU 可以多次调用来添加多个菜单
CQ_MENU(menu_demo_2) {
    cq::logging::info(u8"菜单", u8"点击了示例菜单2");
    MessageBoxW(nullptr, L"这是一个提示", L"提示", MB_OK | MB_SETFOREGROUND | MB_TASKMODAL | MB_TOPMOST);
}
