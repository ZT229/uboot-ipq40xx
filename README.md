## ## 中文 ## | [English](https://github.com/1980490718/uboot-ipq40xx/blob/master/README_EN.md)
## U-Boot mod for IPQ40xx devices

## 项目描述
本项目提供针对IPQ40xx系列开发板的带web固件升级功能的U-Boot编译和构建解决方案，支持多种开发板配置。

## 快速开始

## 1. 获取源代码
```bash
git clone https://github.com/1980490718/uboot-ipq40xx.git
git clone https://github.com/1980490718/openwrt-sdk-ipq806x-qsdk53
```
修改顶层Makefile第2行的STAGING_DIR，指向openwrt-sdk-ipq806x/staging_dir，然后执行：
```
make
```
或者修改顶层的build.sh中的第30行的STAGING_DIR，指向openwrt-sdk-ipq806x/staging_dir，然后执行：
```
./build.sh cdp
```
或者：
```
./build.sh ap4220
```
## 生成关系
```
make
```
生成的uboot为：bin/openwrt-ipq40xx-u-boot-stripped.elf

```
./build.sh cdp
```

生成的uboot为：bin/openwrt-cdp-u-boot-stripped.elf

```
./build.sh ap4220
```

生成的uboot为：bin/openwrt-ap4220-u-boot-stripped.elf

## 刷入带web固件的uboot
1. 将设备的LAN口通过网线接到电脑，并设置电脑IP为192.168.1.2
2. 将TTL链接到板子上对应的TTL引脚
3. 打开你的console软件，并设置好波特率为115200
4. 板子上电的时候中断boot启动
5.  在console软件中输入以下命令：
```
setenv serverip 192.168.1.2
setenv ipaddr 192.168.1.1
saveenv
```
6. 打开tftp软件，设置tftp服务器为192.168.1.2，将文件名为openwrt-ipq40xx-u-boot-stripped.elf的文件复制到tftp服务器的根目录。
7. 输入以下命令先备份原设备的u-boot：
```
sf probe && sf read 0x84000000 0xf0000 0x80000 && tftpput 0x84000000 0x80000 stock_u-boot.bin
```
8. 输入以下命令刷入带web界面的u-boot：
```
tftpboot 0x84000000 openwrt-ipq40xx-u-boot-stripped.elf
sf probe && sf erase 0xf0000 0x80000 && sf write 0x84000000 0xf0000 0x80000
```
可选：
```
env default -f && saveenv
```
9. 重启设备：
```
reset
```
10. 等待console界面重新开始跑码的时候按任意键中断启动输入以下命令，或者重新上电按住reset按键保持不放3s以上，（二选一）。
```
httpd
```
11.  打开浏览器（推荐Google浏览器或者Edge浏览器）输入以下网址即可进入uboot的web界面：
```
http://192.168.1.1
```

## web更新uboot

1. 将设备的LAN口通过网线接到电脑，并设置电脑IP为192.168.1.2
2. 按住RESET键上电，并保持3s以上后松开即可。
3. 在浏览器（推荐Google浏览器或者Edge浏览器）输入：
```
http://192.168.1.1/uboot.html
```
或者
```
http://192.168.1.1
```
4. 进入web界面后点击‘uboot更新’菜单后选择openwrt-ipq40xx-u-boot-stripped.elf文件进行uboot升级。

## 固件升级
1. 将设备的LAN口通过网线接到电脑，并设置电脑IP为192.168.1.2
2. 按住RESET键上电，并保持3s以上后松开即可。
3. 在浏览器（推荐Google浏览器或者Edge浏览器）输入：
```
http://192.168.1.1/index.html
```
4. 选择你要升级的openwrt固件（对应nand仅支持openwrt-factory.bin/openwrt-factory.ubi/QSDK.img固件,对应nor的仅支持factory.bin结尾的openwrt固件以及以.img结尾的QSDK固件），点击“确认更新”按钮。
5. 等待升级完成。
6. 设置电脑IP为自动获取。