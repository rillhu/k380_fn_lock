/*
 * k380fn_ctrl.c
 *
 *  Created on: 2022年5月6日
 *      Author: hongfei
Logitech K380蓝牙键盘，默认最顶上一排键是各种快捷功能。用Logitech的软件可以改成默认F1~F12。但是为了改个Fn键，装200M软件也太那什么了。
此程序可用VC编译，在Windows下运行，找到K380键盘后，写入相应的指令，打开Fn锁定功能，最顶上一排键默认为F1~F12。
每次开关、重新连接键盘后，需重新运行此程序。如果想关闭Fn lock，可以发送程序中带的k380_seq_fkeys_off数据串给键盘。更简单的办法是关掉再打开。
此程序原本是劫持条码扫描枪用的。前面查找HID设备名的功能都一样。后面发送指令的功能是从：
https://github.com/jergusg/k380-function-keys-conf
这里抠出来的。
 */


/*
 *   To compile:
 *
 *   cl k380_fn_on.c /link user32.lib kernel32.lib
 *
 *   Based on:
 *   http://www.trial-n-error.de/posts/2012/12/31/logitech-k810-keyboard-configurator/
 *   https://github.com/embuc/k480_conf
 *   https://github.com/jergusg/k380-function-keys-conf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <windows.h>
#include <stdio.h>
#include <Strsafe.h>

int error(int errnum){
	printf("Customized error:%d\nSystem errno:%d\n",errnum,errno);
	return(-1);
}

const char k380_seq_fkeys_on[]  = {0x10, 0xff, 0x0b, 0x1e, 0x00, 0x00, 0x00};
const char k380_seq_fkeys_off[] = {0x10, 0xff, 0x0b, 0x1e, 0x01, 0x00, 0x00};

const char opt_on[]  = "on";
const char opt_off[] = "off";

int main(int argc, char* argv[]){

int i;
UINT nSize;
UINT dwSize;
LPVOID pData;
LPBYTE lpb;
HANDLE hBarcodeScanner;
HRESULT hResult;
UINT nDevices;
PRAWINPUTDEVICELIST pRawInputDeviceList;
RAWINPUTDEVICE RID[2];
RAWINPUT* raw;
LPTSTR szTempOutput;

if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) {
	error(1);
}

if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST) malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) {
	error(2);
}
i=GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)); //(<dtype rid="UINT"/>)

if (i== -1) {
	error(3);
}

for (i--;i>=0;i--){

	GetRawInputDeviceInfo(
		pRawInputDeviceList[i].hDevice,
		RIDI_DEVICENAME,
		NULL,
		&nSize);
	pData=malloc(nSize);

	if (pData<0){
		error(4);
	}
	GetRawInputDeviceInfo(
		pRawInputDeviceList[i].hDevice,
		RIDI_DEVICENAME,
		pData,
		&nSize);

	printf("Device #%d: type%d, name: %s\n",i,pRawInputDeviceList[i].dwType, pData);
	if(strstr( (const char* )pData,"VID&0002046d_PID&b342&Col06")!=NULL){
	//if(strncmp( (const char* )pData,"\\\\?\\HID#{00001124-0000-1000-8000-00805f9b34fb}_VID&0002046d_PID&b342&Col06",74)==0){
	// would also work

		printf("Device %d selected\n",i);

		HANDLE k380;

		k380=CreateFile(pData,
						GENERIC_WRITE,      //GENERIC_READ |
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);

		if (k380 == INVALID_HANDLE_VALUE ){
			fprintf(stderr, "Unable to open device. ERRNO=%d\n",GetLastError());
		}else{
			BOOL res;
			DWORD bw;

			res = WriteFile(k380, k380_seq_fkeys_on, 7, &bw, NULL);

			if (res = 0)
			{
				fprintf(stderr, "ERRNO=%d\n", GetLastError());
			}
			else if (bw == 7)
				{
				printf("Configuration sent.\n");
			}
			else
			{
				printf("write: %d were written instead of %d.\n", bw, 7);
			}
		}
	}
	free(pData);
}
free(pRawInputDeviceList);
}

