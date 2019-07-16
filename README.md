# kuq-groupmanager 说明

酷Q机器人插件 功能进群欢迎语，自动回复(ini中配置关键字和回复内容)

# 搭建环境
1.首先按照https://github.com/richardchien/coolq-cpp-sdk安装好环境

2.更新本工程 替换上边环境中的相同文件和目录



# 酷Q on Docker 

现在，你可以使用 Docker 以在大部分操作系统下使用最新版本的 酷Q 。
酷Q on Docker 基于 Docker 和 wine 技术，让你可以在任意 x86_64 架构的系统上使用 酷Q 。

注意：由于 wine 技术原理为 Windows 系统，实现与原生系统尚有差距，部分应用运行或有兼容性问题。
故 Docker 镜像仅供熟悉 Linux / wine 的应用开发者使用，涉及应用兼容性问题，酷Q 团队无法解答。

若该镜像出现问题，请通过问题反馈区反馈。<br>
* 如果您拥有支持 Docker 的群晖（Synology）NAS 设备，请参阅：在群晖（Synology）NAS 上使用 酷Q。<br>
使用方法<br>

请先安装 docker，建议使用 Ubuntu Server 16.04 LTS & Docker CE 最新版。本镜像在 Docker 17.06.1-ce 下测试通过。<br>
安装 docker 后，在命令行中键入以下命令下载 酷Q Docker 镜像：<br>

docker pull coolq/wine-coolq<br>

下载后，在任意目录创建一个空文件夹，用于持久化存放 酷Q 数据：<br>

mkdir /root/coolq-data <br>
任意路径均可<br>

然后运行 酷Q 镜像：<br>
docker run --name=coolq --rm -p 8080:9000 -v /root/coolq-data:/home/user/coolq -e VNC_PASSWD=12345678 -e COOLQ_ACCOUNT=123456 coolq/wine-coolq

其中标红部分参数根据自己的需求替换，参见下表：<br>
参数含义	参数示例<br>
远程监听端口	8080<br>
数据存放位置	/root/coolq-data<br>
远程访问密码	12345678<br>
机器人帐号	123456<br>

运行后，会看到控制台中输出一系列日志。当你看到 [CQDaemon] Started CoolQ . 时，说明已启动成功。<br>
此时，在浏览器中访问 http://你的服务器IP:你的端口 即可看到远程操作登录页面，输入密码，即可看到 酷Q Air 的登录界面啦。<br>
在登录后，右键点击悬浮窗 -> 昵称 -> 勾选「自动登录」，即可保证 酷Q 能自动登录。<br>

安装后台服务

【启动容器】<br>
只需要在上述执行的命令，--rm 替换为 -d 即可。注意执行完后只会输出一个容器 ID。

docker run --name=coolq -d -p 8080:9000 -v /root/coolq-data:/home/user/coolq -e VNC_PASSWD=12345678 -e COOLQ_ACCOUNT=123456 coolq/wine-coolq

【查看运行状况】<br>
docker logs coolq


【启动/停止服务】<br>
docker start coolq<br>
docker stop coolq<br>

安装应用 / 管理数据

在启动容器时创建的数据目录存放着 酷Q 的所有数据，包括插件、日志等。你只需利用 FileZilla 等工具，连接服务器管理即可。
在数据目录以外创建的所有文件，容器重启时可能会丢失。<br>

更新日志

v2.0
升级 novnc 完整版，支持剪贴板、调整分辨率、自动重连<br>
新增点阵字体，使用更清晰<br>
新增原生字体，标题不再乱码<br>
增强守护，酷Q 退出后 10 秒内自动重启

