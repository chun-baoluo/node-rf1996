#include <atlbase.h>
#include <v8.h>
#include <node.h>
#include "rf1996.h"

using namespace v8;

// Establish connection with device
void open(const FunctionCallbackInfo<Value>& args) {
	char *error = "";
	String::Utf8Value utfValue(args[0]->ToString());
	char *port = (*utfValue);
	Isolate* isolate = args.GetIsolate();
	Local<Function> cb = Local<Function>::Cast(args[1]);

	lib = LoadLibrary(_T("./node_modules/rf1996/src/rf1996.dll"));
	//lib = LoadLibrary(_T("./src/rf1996.dll"));
	if(!lib) {
		error = "RF1996.dll not found! The default path is './node_modules/rf1996/src/rf1996.dll'. Try to change the path in rf1996.cpp and recompile the module using node-gyp rebuild.";
	} else {
		InitLib	= (pfnInit)GetProcAddress(lib,  "Init");
		OpenDeviceA = (pfnOpenDeviceA)GetProcAddress(lib,  "OpenDeviceA");
		IsDeviceOpen = (pfnIsDeviceOpen)GetProcAddress(lib, "IsDeviceOpen");
		ReadCard = (pfnReadCard)GetProcAddress(lib,	"ReadCard");
		CloseDevice = (pfnCloseDevice)GetProcAddress(lib,  "CloseDevice");	

		GetDeviceInfoA = (pfnGetDeviceInfoA)GetProcAddress(lib,	"GetDeviceInfoA");

		WriteCard = (pfnWriteCard)GetProcAddress(lib,	"WriteCard");
		InitCard = (pfnInitCard)GetProcAddress(lib, "InitCard");
		ClearCard	= (pfnClearCard)GetProcAddress(lib,		"ClearCard");
		InitLib();

    	OpenDeviceA(port);

    	if(!IsDeviceOpen()) {
    		error = "Couldn\'t open the device.";
    	}
	}

    Local<Value> argv[1] = {String::NewFromUtf8(isolate, error)};
    if(error == "") {
        argv[0] = Null(isolate);
    }
    cb->Call(Null(isolate), 1, argv);
}

// Read card data
void read(const FunctionCallbackInfo<Value>& args) {
	int len = 0;
	TCHAR str[128], strEm[128];
	Isolate* isolate = args.GetIsolate();
	Local<Object> obj = Object::New(isolate);
	
	Local<Function> cb = Local<Function>::Cast(args[1]);

	ReadCard(&CardInfo);

	switch(CardInfo.CardType) {			
		case ect_clean_temic:
			wsprintf(str, _T("Empty temic"));
			break;
		case ect_temic:
			wsprintf(str, _T("Temic protected by password"));
			break;
		case ect_bad_password:
			wsprintf(str, _T("Wrong password"));
			break;
		case esi_em_marine:
			wsprintf(str, _T("Em-Marine"));
			break;
		default:
			wsprintf(str, _T("Unknown value"));
			break;
	}

	obj->Set(String::NewFromUtf8(isolate, "card"), String::NewFromUtf8(isolate, str));

	for( int i=0; i < 8; i++) {
		len += wsprintf(str + len, _T("%02x "), CardInfo.TemicCode[i]);
	}	
	obj->Set(String::NewFromUtf8(isolate, "temic"), String::NewFromUtf8(isolate, str));

	u_short *SquareBrackets = reinterpret_cast<u_short*>(&(CardInfo.EmMarineCode[3]));
	u_short *Decimal = reinterpret_cast<u_short*>(&(CardInfo.EmMarineCode[0]));
	wsprintf(strEm, _T("[%04i] %03i,%05i"), *SquareBrackets, CardInfo.EmMarineCode[2], *Decimal);
	
	obj->Set(String::NewFromUtf8(isolate, "emMarine"), String::NewFromUtf8(isolate, strEm));

	wsprintf(str, _T("%i"), CardInfo.LimitLocks);
	obj->Set(String::NewFromUtf8(isolate, "locksLimit"), String::NewFromUtf8(isolate, str));

	wsprintf(str, _T("%i"), CardInfo.CountBorrow);
	obj->Set(String::NewFromUtf8(isolate, "locks"), String::NewFromUtf8(isolate, str));

	wsprintf(str, _T("%i"), CardInfo.NumLastLocks);
	obj->Set(String::NewFromUtf8(isolate, "lastBoxId"), String::NewFromUtf8(isolate, str));

	obj->Set(String::NewFromUtf8(isolate, "battery"), String::NewFromUtf8(isolate, (CardInfo.BatteryCondition ? "Discharged" : "Normal")));

    args.GetReturnValue().Set(obj);
	
}

// Write card
void write(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	Local<Object> returnObj = Object::New(isolate);
	if(args[0]->IsObject()) {
		Handle<Object> obj = Handle<Object>::Cast(args[0]);
		BYTE	EmMarineCode[5];
		u_short *SquareBrackets = reinterpret_cast<u_short*>(&(EmMarineCode[3]));
		u_short *Decimal = reinterpret_cast<u_short*>(&(EmMarineCode[0]));

		*SquareBrackets = u_short(obj->Get(String::NewFromUtf8(isolate, "brackets"))->NumberValue());
		*Decimal = u_short(obj->Get(String::NewFromUtf8(isolate, "decimal"))->NumberValue());
		EmMarineCode[2] = (BYTE)obj->Get(String::NewFromUtf8(isolate, "group"))->NumberValue();
		InitCard(EmMarineCode);
		returnObj->Set(String::NewFromUtf8(isolate, "success"), True(isolate));
	} else {
		returnObj->Set(String::NewFromUtf8(isolate, "success"), False(isolate));
		returnObj->Set(String::NewFromUtf8(isolate, "reason"), String::NewFromUtf8(isolate, "First argument isn't an object!"));
	}
	args.GetReturnValue().Set(returnObj);
}

// Get device info
void device(const FunctionCallbackInfo<Value>& args) {
	TCHAR str[128];
	Isolate* isolate = args.GetIsolate();
	Local<Object> obj = Object::New(isolate);;
	
	Local<Function> cb = Local<Function>::Cast(args[1]);

	GetDeviceInfoA(&DeviceInfo);

	obj->Set(String::NewFromUtf8(isolate, "port"), String::NewFromUtf8(isolate, DeviceInfo.Port));
	obj->Set(String::NewFromUtf8(isolate, "device"), String::NewFromUtf8(isolate, DeviceInfo.DeviceName));

	wsprintf(str, _T("%i"), DeviceInfo.SerialNumber);
	obj->Set(String::NewFromUtf8(isolate, "serial"), String::NewFromUtf8(isolate, str));
	
	wsprintf(str, _T("%i"), DeviceInfo.ModelNumber);
	obj->Set(String::NewFromUtf8(isolate, "model"), String::NewFromUtf8(isolate, str));

	wsprintf(str, _T("%i"), DeviceInfo.FirmwareVersion);
	obj->Set(String::NewFromUtf8(isolate, "firmware"), String::NewFromUtf8(isolate, str));
	
	switch(DeviceInfo.LicenseStatus) {
		case els_no_info: 
			wsprintf(str, _T("No info"));
			break;
		
		case els_none:
			wsprintf(str, _T("License is not valid"));
			break;
		case els_conclude:
			wsprintf(str, _T("License expired"));
			break;
		case els_hard_error:
			wsprintf(str, _T("Device error"));
			break;

		case els_ok:		
			wsprintf(str, _T("License %02i.%02i.%04i"), DeviceInfo.LicenseDay, DeviceInfo.LicenseMonth, DeviceInfo.LicenseYear);			
			break;

		default:
			wsprintf(str, _T("%i"), _T("Unknown error"));
			break;
	}	

	obj->Set(String::NewFromUtf8(isolate, "license"), String::NewFromUtf8(isolate, str));

	wsprintf(str, _T("%i"), DeviceInfo.CountCard);
	obj->Set(String::NewFromUtf8(isolate, "cards"), String::NewFromUtf8(isolate, str));

    args.GetReturnValue().Set(obj);
}

// Clear card
void clear(const FunctionCallbackInfo<Value>& args) {
	ClearCard();
}

// Close connection
void close(const FunctionCallbackInfo<Value>& args) {
	CloseDevice();
}

// Регистрируем наши методы
void RegisterModule(Local<Object> exports) {
	NODE_SET_METHOD(exports, "open", open);
	NODE_SET_METHOD(exports, "read", read);
	NODE_SET_METHOD(exports, "write", write);
	NODE_SET_METHOD(exports, "clear", clear);
	NODE_SET_METHOD(exports, "device", device);
	NODE_SET_METHOD(exports, "close", close);
}

NODE_MODULE(modulename, RegisterModule);