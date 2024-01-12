## DO-Y103

### ���롢����˵��

`bootloader` �� `APP` �ı�������ѡ�λ�ڹ��̶�Ӧ�� `BSP/board_config.h` �ļ��С�
* `DISABLE_WDG` ���ڹر�������Ź����ܣ�
   ��ʹ�õ��������ߵ���ʱ������ʹ�ܸú꣬ͬʱ�Ͽ�Ӳ�����Ź��� `RST` �ź�Դ�ϵĵ��� `R40` ��
* `ENABLE_FLASH_RDP` ����ʹ�� flash �Ķ��������ܡ�
   һ�����ú�ͨ���ⲿ��¼����¼����ᵼ����Ƭ flash ���ݱ�������
* `ENABLE_MBEDTLS_MD5` ����ʹ�� flash �������ݼ��ܺ��Իٹ��ܡ�
   1����Ҫ���ô˹��ܣ�����Ҫͬʱʹ�� `bootloader` �� `APP` �еĸúꣻ
   2���ù������ú����״� `bootloader` ����ʱ��Ҫ�� `PC11` ���Ŷ̽� `GND` ������Ӧ�� passwrod ���ݣ�
   3���ù������ú�`APP` �����лᶨʱ(1min)У�� password ���ݡ��� password ����ȷ�������� flash �����Ի٣�
`bootloader` �� `APP` ����Ĭ������Ϊ��

```C
// #define DISABLE_WDG 
#define ENABLE_FLASH_RDP 
// #define ENABLE_MBEDTLS_MD5 
```

### log ��Ϣ˵��

��ӡ���� log ��Ϣʹ�õ�ʱ `UART3`��������Ϊ 115200 .

### IAP ����˵��

ͨ�� `RS485` �ӿ�����Խ��д���ͨ�ţ�������Ϊ 9600 ��ͨ��Э��Ϊ `XMODEM` ����֡��������ݳ���Ϊ 1024 Bytes ��
* ��ϸ�������裬�ο���DO-Y103��������Բ�.docx����
* `IAP` ���������У�Ϊ��ֹ flash ��������ʱ���������Ӳ�����Ź���λ��flash ���÷ֶβ����ķ�ʽ��ÿ�β��� 32K ��

### flash ����˵��

���� chip �� flash �ֲ���
```C
address             type                maxlength
0x8000000           bootloader          0x1B000 - 108k 
0x801B000           APP1 settings       0x800   - 2k 
0x801B800           APP2 settings       0x800   - 2k 
0x801C000           User data           0x4000  - 16k 
0x8020000           APP1                0x30000 - 192k 
0x8050000           APP2                0x30000 - 192k 

User data �� flash �ֲ���
offset              type                maxlength
0x0000              app config setting  0x400   - 1k 
0x0400              mbedtls md5         0x400   - 1k 
```

### mbedtls �ļ��ܷ�ʽ˵��

���ܲ��� "�̶�����" + "�ɱ���Կ" ���� "�̶���������" ����ʽ������Ϊ content �� 32 Bytes ��������ԿΪ key �� 5*4 Bytes �������� HMAC_MD5 �ļ����㷨�õ����� password �� 16 Bytes��
* content : "ZYIOT" + "," + `PN_CODE` + "," + `SN_CODE` + ������ݡ�Ʃ�� `PN_CODE` = "DO-Y103", `SN_CODE` = "3106"���� 
`content = "ZYIOT,DO-Y103,3106"00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D`
* key : ΪоƬ�� `UID` ��
