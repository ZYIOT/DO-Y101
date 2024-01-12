## DO-Y103

### 编译、调试说明

`bootloader` 和 `APP` 的编译配置选项都位于工程对应的 `BSP/board_config.h` 文件中。
* `DISABLE_WDG` 用于关闭软件看门狗功能；
   当使用调试器在线调试时，建议使能该宏，同时断开硬件看门狗的 `RST` 信号源上的电阻 `R40` ；
* `ENABLE_FLASH_RDP` 用于使能 flash 的读保护功能。
   一旦启用后，通过外部烧录器烧录程序会导致整片 flash 数据被擦除；
* `ENABLE_MBEDTLS_MD5` 用于使能 flash 程序数据加密和自毁功能。
   1、若要启用此功能，则需要同时使能 `bootloader` 和 `APP` 中的该宏；
   2、该功能启用后，在首次 `bootloader` 启动时需要将 `PC11` 引脚短接 `GND` ，填充对应的 passwrod 数据；
   3、该功能启用后，`APP` 程序中会定时(1min)校验 password 数据。若 password 不正确，则启动 flash 数据自毁；
`bootloader` 和 `APP` 程序默认配置为：

```C
// #define DISABLE_WDG 
#define ENABLE_FLASH_RDP 
// #define ENABLE_MBEDTLS_MD5 
```

### log 信息说明

打印调试 log 信息使用的时 `UART3`，波特率为 115200 .

### IAP 升级说明

通过 `RS485` 接口与电脑进行串口通信，波特率为 9600 ，通信协议为 `XMODEM` ，单帧最大传输数据长度为 1024 Bytes 。
* 详细操作步骤，参考《DO-Y103软件功能自测.docx》；
* `IAP` 升级过程中，为防止 flash 擦除操作时间过长导致硬件看门狗复位，flash 采用分段擦除的方式，每次擦除 32K ；

### flash 分区说明

整个 chip 的 flash 分布：
```C
address             type                maxlength
0x8000000           bootloader          0x1B000 - 108k 
0x801B000           APP1 settings       0x800   - 2k 
0x801B800           APP2 settings       0x800   - 2k 
0x801C000           User data           0x4000  - 16k 
0x8020000           APP1                0x30000 - 192k 
0x8050000           APP2                0x30000 - 192k 

User data 的 flash 分布：
offset              type                maxlength
0x0000              app config setting  0x400   - 1k 
0x0400              mbedtls md5         0x400   - 1k 
```

### mbedtls 的加密方式说明

加密采用 "固定明文" + "可变密钥" 生成 "固定长度密文" 的形式。明文为 content 共 32 Bytes ，加密密钥为 key 共 5*4 Bytes ，采用了 HMAC_MD5 的加密算法得到密文 password 共 16 Bytes；
* content : "ZYIOT" + "," + `PN_CODE` + "," + `SN_CODE` + 填充数据。譬如 `PN_CODE` = "DO-Y103", `SN_CODE` = "3106"，则 
`content = "ZYIOT,DO-Y103,3106"00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D`
* key : 为芯片的 `UID` ；
