/*
 * k380fn_ctrl.c
 *
 *  Created on: 2022��5��6��
 *      Author: hongfei
Logitech K380�������̣�Ĭ�����һ�ż��Ǹ��ֿ�ݹ��ܡ���Logitech��������Ըĳ�Ĭ��F1~F12������Ϊ�˸ĸ�Fn����װ200M���Ҳ̫��ʲô�ˡ�
�˳������VC���룬��Windows�����У��ҵ�K380���̺�д����Ӧ��ָ���Fn�������ܣ����һ�ż�Ĭ��ΪF1~F12��
ÿ�ο��ء��������Ӽ��̺����������д˳��������ر�Fn lock�����Է��ͳ����д���k380_seq_fkeys_off���ݴ������̡����򵥵İ취�ǹص��ٴ򿪡�
�˳���ԭ���ǽٳ�����ɨ��ǹ�õġ�ǰ�����HID�豸���Ĺ��ܶ�һ�������淢��ָ��Ĺ����Ǵӣ�
https://github.com/jergusg/k380-function-keys-conf
����ٳ����ġ�
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

