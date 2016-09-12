enum ECardType : UINT {
	ect_none		 = 0,
	ect_clean_temic	 = 0x10,
	ect_temic		 = 0x20,
	ect_bad_password = 0x30,
	esi_em_marine	 = 0x40,
	ect_EEPROM_error = 0x80,
	ect_data_error	 = 0x81,
	ect_unknow		 = 0xF0,	
};

enum ELicenseStatus : UINT {
	els_no_info		= 0,
	els_ok			= 0xAA,
	els_none		= 0xFE,
	els_conclude	= 0xFD,
	els_hard_error	= 0xFC,
};

struct SDeviceInfoA {
	char			Port[16];
	char			DeviceName[128];
	DWORD			SerialNumber;
	DWORD			ModelNumber;
	DWORD			FirmwareVersion;	
	ELicenseStatus  LicenseStatus;
	DWORD			LicenseYear;
	DWORD			LicenseMonth;
	DWORD			LicenseDay;
	DWORD			CountCard;
};

struct SCardInfo {
	DWORD		Flags;
	ECardType	CardType;
	BYTE		TemicCode[8];
	BYTE		EmMarineCode[8];
	DWORD		LimitLocks;
	DWORD		CountBorrow;
	DWORD		NumLastLocks;
	BOOL		BatteryCondition;
};

typedef DWORD (*pfnInit)();
typedef DWORD (*pfnOpenDeviceA)(char *port);
typedef DWORD (*pfnCloseDevice)();
typedef BOOL  (*pfnIsDeviceOpen)();
typedef DWORD (*pfnGetDeviceInfoA)(SDeviceInfoA *DevInfo);
typedef DWORD (*pfnReadCard)(SCardInfo* CardInfo);					

typedef DWORD (*pfnWriteCard)(DWORD LimitLocks);
typedef DWORD (*pfnInitCard)(BYTE *EmMarineCode);
typedef DWORD (*pfnClearCard)();

HMODULE lib;
SCardInfo CardInfo;
SDeviceInfoA DeviceInfo;

pfnInit InitLib;
pfnOpenDeviceA OpenDeviceA;
pfnIsDeviceOpen IsDeviceOpen;
pfnReadCard ReadCard;
pfnCloseDevice CloseDevice;
pfnGetDeviceInfoA GetDeviceInfoA;

pfnWriteCard WriteCard;
pfnInitCard	InitCard;
pfnClearCard ClearCard;
