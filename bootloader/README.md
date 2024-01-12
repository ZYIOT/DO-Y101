# DO-Y102 Bootloader


### 离线升级步骤
1. 在开机`3秒`内，向传感器发送`UPLOAD_APP`
2. 当收到`0x15`后，`30秒`内,向传感器使用`X-Modem`协议发送bin格式的固件文件

### 注意事项：
1. 如果`bin格式的固件文件`的大小`大于32k`,`X-Modem`协议的组大小设置为`1024字节`
2. 如果`bin格式的固件文件`的大小`小于32k`,`X-Modem`协议的组大小设置为`128字节`
3. 最大`bin格式的固件文件`的大小为`64k`


### 启动步骤
1. 检查离线升级
2. 加载 `APP1 settings` 和 `APP1 settings`
3. APP2 settings的数据不合法，则运行APP1
4. APP1 settings合法， APP2 settings合法， APP1 settings等于APP2 settings，则运行APP1
5. APP2 settings合法，APP1 settings的数据不合法或APP1 settings不等于APP2 settings ，则复制APP2到APP1,并运行APP1
   