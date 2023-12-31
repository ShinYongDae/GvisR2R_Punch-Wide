﻿#if !defined(AFX_HARDWAREINFODEFINE_H__INCLUDED_)
#define AFX_HARDWAREINFODEFINE_H__INCLUDED_

#pragma once

enum FieldIdxPc
{
	PcAdminPasswordStatus=0,
	PcAutomaticManagedPagefile,
	PcAutomaticResetBootOption,
	PcAutomaticResetCapability,
	PcBootOptionOnLimit,
	PcBootOptionOnWatchDog,
	PcBootROMSupported,
	PcBootupState,
	PcCaption,
	PcChassisBootupState,
	PcCreationClassName,
	PcCurrentTimeZone,
	PcDaylightInEffect,
	PcDescription,
	PcDNSHostName,
	PcDomain,
	PcDomainRole,
	PcEnalbleDaylightSavingsTime,
	PcFrontPanelResetStatus,
	PcInfraredSupported,
	PcInitialLoadInfo,
	PcInstallDate,
	PcKeyboartPasswordStatus,
	PcLastLoadInfo,
	PcManufacturer,
	PcModel,
	PcName,
	PcNameFormat,
	PcNetworkServerModelEnabled,
	PcNumberOfLogicalProcessors,
	PcNumberOfProcessors,
	PcPartOfDomain,
	PcPauseAfterReset,
	PcPCSystemType,
	PcPowerOnPasswordStatus,
	PcPowerState,
	PcPowerSupplyState,
	PcPrimaryOwnerContact,
	PcPrimaryOwnerName,
	PcResetCapability,
	PcResetCount,
	PcResetLimit,
	PcStatus,
	PcSystemType,
	PcThermalState,
	PcTotalPhysicalMemory,
	PcUserName,
	PcWakeUpType,
	PcWorkgroup,
	PcTotal
};

wchar_t* FieldObjPc[] =
{
	OLESTR("AdminPasswordStatus"),
	OLESTR("AutomaticManagedPagefile"),
	OLESTR("AutomaticResetBootOption"),
	OLESTR("AutomaticResetCapability"),
	OLESTR("BootOptionOnLimit"),
	OLESTR("BootOptionOnWatchDog"),
	OLESTR("BootROMSupported"),
	OLESTR("BootupState"),
	OLESTR("Caption"),
	OLESTR("ChassisBootupState"),
	OLESTR("CreationClassName"),
	OLESTR("CurrentTimeZone"),
	OLESTR("DaylightInEffect"),
	OLESTR("Description"),
	OLESTR("DNSHostName"),
	OLESTR("Domain"),
	OLESTR("DomainRole"),
	OLESTR("EnalbleDaylightSavingsTime"),
	OLESTR("FrontPanelResetStatus"),
	OLESTR("InfraredSupported"),
	OLESTR("InitialLoadInfo"),
	OLESTR("InstallDate"),
	OLESTR("KeyboartPasswordStatus"),
	OLESTR("LastLoadInfo"),
	OLESTR("Manufacturer"),
	OLESTR("Model"),
	OLESTR("Name"),
	OLESTR("NameFormat"),
	OLESTR("NetworkServerModelEnabled"),
	OLESTR("NumberOfLogicalProcessors"),
	OLESTR("NumberOfProcessors"),
	OLESTR("PartOfDomain"),
	OLESTR("PauseAfterReset"),
	OLESTR("PCSystemType"),
	OLESTR("PowerOnPasswordStatus"),
	OLESTR("PowerState"),
	OLESTR("PowerSupplyState"),
	OLESTR("PrimaryOwnerContact"),
	OLESTR("PrimaryOwnerName"),
	OLESTR("ResetCapability"),
	OLESTR("ResetCount"),
	OLESTR("ResetLimit"),
	OLESTR("Status"),
	OLESTR("SystemType"),
	OLESTR("ThermalState"),
	OLESTR("TotalPhysicalMemory"),
	OLESTR("UserName"),
	OLESTR("WakeUpType"),
	OLESTR("Workgroup")
};

enum FieldIdxCpu
{
	CpuAddressWidth=0,
	CpuArchitecture,
	CpuAvailability,
	CpuCaption,
	CpuConfigManagerErrorCode,
	CpuCreationClassName,
	CpuDataWidth,
	CpuDescription,
	CpuDeviceID,
	CpuL2CacheSize,
	CpuL3CacheSize,
	CpuManufacturer,
	CpuMaxClockSpeed,
	CpuName,
	CpuNumberOfCores,
	CpuNumberOfLogicalProcessors,
	CpuProcessorId,
	CpuRevision,
	CpuStatus,
	CpuSystemCreationClassName,
	CpuSystemName,
	CpuTotal
};

wchar_t* FieldObjCpu[] =
{
	OLESTR("AddressWidth"),
	OLESTR("Architecture"),
	OLESTR("Availability"),
	OLESTR("Caption"),
	OLESTR("ConfigManagerErrorCode"),
	OLESTR("CreationClassName"),
	OLESTR("DataWidth"),
	OLESTR("Description"),
	OLESTR("DeviceID"),
	OLESTR("L2CacheSize"),
	OLESTR("L3CacheSize"),
	OLESTR("Manufacturer"),
	OLESTR("MaxClockSpeed"),
	OLESTR("Name"),
	OLESTR("NumberOfCores"),
	OLESTR("NumberOfLogicalProcessors"),
	OLESTR("ProcessorId"),
	OLESTR("Revision"),
	OLESTR("Status"),
	OLESTR("SystemCreationClassName"),
	OLESTR("SystemName")
};


enum FieldIdxBios
{
	BiosBuildNumber=0,
	BiosCaption,
	BiosCodeSet,
	BiosCurrentLanguage,
	BiosDescription,
	BiosIdentificationCode,
	BiosInstallableLanguages,
	BiosInstallDate,
	BiosLanguageEdition,
	BiosListOfLanguages,
	BiosManufacturer,
	BiosName,
	BiosOtherTargetOS,
	BiosPrimaryBIOS,
	BiosReleaseDate,
	BiosSerialNumber,
	BiosSMBIOSBIOSVersion,
	BiosSMBIOSMajorVersion,
	BiosSMBIOSMinorVersion,
	BiosSMBIOSPresent,
	BiosSoftwareElementID,
	BiosSoftwareElementState,
	BiosStatus,
	BiosTargetOperatingSystem,
	BiosVersion,
 	BiosTotal
};

wchar_t* FieldObjBios[] =
{
	OLESTR("BuildNumber"),
	OLESTR("Caption"),
	OLESTR("CodeSet"),
	OLESTR("CurrentLanguage"),
	OLESTR("Description"),
	OLESTR("IdentificationCode"),
	OLESTR("InstallableLanguages"),
	OLESTR("InstallDate"),
	OLESTR("LanguageEdition"),
	OLESTR("ListOfLanguages"),
	OLESTR("Manufacturer"),
	OLESTR("Name"),
	OLESTR("OtherTargetOS"),
	OLESTR("PrimaryBIOS"),
	OLESTR("ReleaseDate"),
	OLESTR("SerialNumber"),
	OLESTR("SMBIOSBIOSVersion"),
	OLESTR("SMBIOSMajorVersion"),
	OLESTR("SMBIOSMinorVersion"),
	OLESTR("SMBIOSPresent"),
	OLESTR("SoftwareElementID"),
	OLESTR("SoftwareElementState"),
	OLESTR("Status"),
	OLESTR("TargetOperatingSystem"),
	OLESTR("Version")
};


enum FieldIdxMainBoard
{
	MainBoardCaption=0,
	MainBoardConfigOptions,
	MainBoardCreationClassName,
	MainBoardDepth,
	MainBoardDescription,
	MainBoardHeight,
	MainBoardHostingBoard,
	MainBoardHotSwappable,
	MainBoardInstallDate,
	MainBoardManufacturer,
	MainBoardModel,
	MainBoardName,
	MainBoardOtherIdentifyingInfo,
	MainBoardPartNumber,
	MainBoardPoweredOn,
	MainBoardProduct,
	MainBoardRemovable,
	MainBoardReplaceable,
	MainBoardRequirementsDescription,
	MainBoardRequiresDaughterBoard,
	MainBoardSerialNumber,
	MainBoardSKU,
	MainBoardSlotLayout,
	MainBoardSpecialRequirements,
	MainBoardStatus,
	MainBoardTag,
	MainBoardVersion,
	MainBoardWeight,
	MainBoardWidth,
 	MainBoardTotal
};

wchar_t* FieldObjMainBoard[] =
{
	OLESTR("Caption"),
	OLESTR("ConfigOptions"),
	OLESTR("CreationClassName"),
	OLESTR("Depth"),
	OLESTR("Description"),
	OLESTR("Height"),
	OLESTR("HostingBoard"),
	OLESTR("HotSwappable"),
	OLESTR("InstallDate"),
	OLESTR("Manufacturer"),
	OLESTR("Model"),
	OLESTR("Name"),
	OLESTR("OtherIdentifyingInfo"),
	OLESTR("PartNumber"),
	OLESTR("PoweredOn"),
	OLESTR("Product"),
	OLESTR("Removable"),
	OLESTR("Replaceable"),
	OLESTR("RequirementsDescription"),
	OLESTR("RequiresDaughterBoard"),
	OLESTR("SerialNumber"),
	OLESTR("SKU"),
	OLESTR("SlotLayout"),
	OLESTR("SpecialRequirements"),
	OLESTR("Status"),
	OLESTR("Tag"),
	OLESTR("Version"),
	OLESTR("Weight"),
	OLESTR("Width")
};


enum FieldIdxRam
{
	RamCaption=0,
	RamCreationClassName,
	RamDescription,
	RamDeviceID,
	RamEndingAddress,
	RamName,
	RamNumberOfBlocks,
	RamPnPDeviceID,
	RamPowerManagementCapabilities,
	RamPowerManagementSupported,
	RamPurpose,
	RamStartingAddress,
	RamStatus,
	RamStatusInfo,
	RamSystemCreationClassName,
	RamSystemLevelAddress,
	RamSystemName,
 	RamTotal
};

wchar_t* FieldObjRam[] =
{
	OLESTR("Caption"),
	OLESTR("CreationClassName"),
	OLESTR("Description"),
	OLESTR("DeviceID"),
	OLESTR("EndingAddress"),
	OLESTR("Name"),
	OLESTR("NumberOfBlocks"),
	OLESTR("PnPDeviceID"),
	OLESTR("PowerManagementCapabilities"),
	OLESTR("PowerManagementSupported"),
	OLESTR("Purpose"),
	OLESTR("StartingAddress"),
	OLESTR("Status"),
	OLESTR("StatusInfo"),
	OLESTR("SystemCreationClassName"),
	OLESTR("SystemLevelAddress"),
	OLESTR("SystemName")
};


enum FieldIdxHdd
{
	HddAvailability=0,
	HddBytesPerSector,
	HddCaption,
	HddCompressionMethod,
	HddConfigManagerErrorCode,
	HddConfigManagerUserConfig,
	HddCreationClassName,
	HddDefaultBlockSize,
	HddDescription,
	HddDeviceID,
	HddFirmwareRevision,
	HddIndex,
	HddInstallDate,
	HddInterfaceType,
	HddManufacturer,
	HddMaxBlockSize,
	HddMaxMediaSize,
	HddMediaLoaded,
	HddMediaType,
	HddMinBlockSize,
	HddModel,
	HddName,
	HddPartitions,
	HddPNPDeviceID,
	HddPowerManagementCapabilities,
	HddPowerManagementSupported,
	HddSCSIBus,
	HddSCSILogicalUnit,
	HddSCSIPort,
	HddSCSITargetId,
	HddSectorsPerTrack,
	HddSerialNumber,
	HddSignature,
	HddSize,
	HddStatus,
	HddStatusInfo,
	HddSystemCreationClassName,
	HddSystemName,
	HddTotalCylinders,
	HddTotalHeads,
	HddTotalSectors,
	HddTotalTracks,
	HddTracksPerCylinder,
 	HddTotal
};

wchar_t* FieldObjHdd[] =
{
	OLESTR("Availability"),
	OLESTR("BytesPerSector"),
	OLESTR("Caption"),
	OLESTR("CompressionMethod"),
	OLESTR("ConfigManagerErrorCode"),
	OLESTR("ConfigManagerUserConfig"),
	OLESTR("CreationClassName"),
	OLESTR("DefaultBlockSize"),
	OLESTR("Description"),
	OLESTR("DeviceID"),
	OLESTR("FirmwareRevision"),
	OLESTR("Index"),
	OLESTR("InstallDate"),
	OLESTR("InterfaceType"),
	OLESTR("Manufacturer"),
	OLESTR("MaxBlockSize"),
	OLESTR("MaxMediaSize"),
	OLESTR("MediaLoaded"),
	OLESTR("MediaType"),
	OLESTR("MinBlockSize"),
	OLESTR("Model"),
	OLESTR("Name"),
	OLESTR("Partitions"),
	OLESTR("PNPDeviceID"),
	OLESTR("PowerManagementCapabilities"),
	OLESTR("PowerManagementSupported"),
	OLESTR("SCSIBus"),
	OLESTR("SCSILogicalUnit"),
	OLESTR("SCSIPort"),
	OLESTR("SCSITargetId"),
	OLESTR("SectorsPerTrack"),
	OLESTR("SerialNumber"),
	OLESTR("Signature"),
	OLESTR("Size"),
	OLESTR("Status"),
	OLESTR("StatusInfo"),
	OLESTR("SystemCreationClassName"),
	OLESTR("SystemName"),
	OLESTR("TotalCylinders"),
	OLESTR("TotalHeads"),
	OLESTR("TotalSectors"),
	OLESTR("TotalTracks"),
	OLESTR("TracksPerCylinder")
};

enum FieldIdxLogicalDisk
{
	LogicalDiskAccess=0,
	LogicalDiskAvailability,
	LogicalDiskBlockSize,
	LogicalDiskCaption,
	LogicalDiskCompressed,
	LogicalDiskCreationClassName,
	LogicalDiskDescription,
	LogicalDiskDeviceID,
	LogicalDiskDriveType,
	LogicalDiskFileSystem,
	LogicalDiskFreeSpace,
	LogicalDiskMaximumComponentLength,
	LogicalDiskMediaType,
	LogicalDiskName,
	LogicalDiskNumberOfBlocks,
	LogicalDiskPNPDeviceID,
	LogicalDiskProviderName,
	LogicalDiskPurpose,
	LogicalDiskSize,
	LogicalDiskStatus,
	LogicalDiskStatusInfo,
	LogicalDiskSystemCreationClassName,
	LogicalDiskSystemName,
	LogicalDiskVolumeDirty,
	LogicalDiskVolumeName,
	LogicalDiskVolumeSerialNumber,
 	LogicalDiskTotal
};

wchar_t* FieldObjLogicalDisk[] =
{
	OLESTR("Access"),
	OLESTR("Availability"),
	OLESTR("BlockSize"),
	OLESTR("Caption"),
	OLESTR("Compressed"),
	OLESTR("CreationClassName"),
	OLESTR("Description"),
	OLESTR("DeviceID"),
	OLESTR("DriveType"),
	OLESTR("FileSystem"),
	OLESTR("FreeSpace"),
	OLESTR("MaximumComponentLength"),
	OLESTR("MediaType"),
	OLESTR("Name"),
	OLESTR("NumberOfBlocks"),
	OLESTR("PNPDeviceID"),
	OLESTR("ProviderName"),
	OLESTR("Purpose"),
	OLESTR("Size"),
	OLESTR("Status"),
	OLESTR("StatusInfo"),
	OLESTR("SystemCreationClassName"),
	OLESTR("SystemName"),
	OLESTR("VolumeDirty"),
	OLESTR("VolumeName"),
	OLESTR("VolumeSerialNumber")
};



enum FieldIdxUsb
{
	UsbAvailability=0,
	UsbCaption,
	UsbClassCode,
	UsbConfigManagerErrorCode,
	UsbConfigManagerUserConfig,
	UsbCreationClassName,
	UsbCurrentAlternateSettings,
	UsbCurrentConfigValue,
	UsbDescription,
	UsbDeviceID,
	UsbGangSwitched,
	UsbInstallDate,
	UsbName,
	UsbNumberOfConfigs,
	UsbNumberOfPorts,
	UsbPNPDeviceID,
	UsbPowerManagementCapabilities,
	UsbPowerManagementSupported,
	UsbProtocolCode,
	UsbStatus,
	UsbStatusInfo,
	UsbSubclasscode,
	UsbSystemCreationClassName,
	UsbSystemName,
	UsbUSBVersion,
 	UsbTotal
};

wchar_t* FieldObjUsb[] =
{
	OLESTR("Availability"),
	OLESTR("Caption"),
	OLESTR("ClassCode"),
	OLESTR("ConfigManagerErrorCode"),
	OLESTR("ConfigManagerUserConfig"),
	OLESTR("CreationClassName"),
	OLESTR("CurrentAlternateSettings"),
	OLESTR("CurrentConfigValue"),
	OLESTR("Description"),
	OLESTR("DeviceID"),
	OLESTR("GangSwitched"),
	OLESTR("InstallDate"),
	OLESTR("Name"),
	OLESTR("NumberOfConfigs"),
	OLESTR("NumberOfPorts"),
	OLESTR("PNPDeviceID"),
	OLESTR("PowerManagementCapabilities"),
	OLESTR("PowerManagementSupported"),
	OLESTR("ProtocolCode"),
	OLESTR("Status"),
	OLESTR("StatusInfo"),
	OLESTR("Subclasscode"),
	OLESTR("SystemCreationClassName"),
	OLESTR("SystemName"),
	OLESTR("USBVersion")
};

wchar_t* FieldObjOs[] =
{
	OLESTR("BootDevice"),
	OLESTR("BuildNumber"),
	OLESTR("BuildType"),
	OLESTR("Caption"),
	OLESTR("CodeSet"),
	OLESTR("CountryCode"),
	OLESTR("CreationClassName"),
	OLESTR("CSCreationClassName"),
	OLESTR("CSDVersion"),
	OLESTR("CSName"),
	OLESTR("CurrentTimeZone"),
	OLESTR("DataExecutionPrevention_32BitApplications"),
	OLESTR("DataExecutionPrevention_Available"),
	OLESTR("DataExecutionPrevention_Drivers"),
	OLESTR("DataExecutionPrevention_SupportPolicy"),
	OLESTR("Debug"),
	OLESTR("Description"),
	OLESTR("Distributed"),
	OLESTR("EncryptionLevel"),
	OLESTR("ForegroundApplicationBoost"),
	OLESTR("FreePhysicalMemory"),
	OLESTR("FreeSpaceInPagingFiles"),
	OLESTR("FreeVirtualMemory"),
	OLESTR("InstallDate"),
	OLESTR("LargeSystemCache"),
	OLESTR("LastBootUpTime"),
	OLESTR("LocalDateTime"),
	OLESTR("Locale"),
	OLESTR("Manufacturer"),
	OLESTR("MaxNumberOfProcesses"),
	OLESTR("MaxProcessMemorySize"),
	OLESTR("MUILanguages"),
	OLESTR("Name"),
	OLESTR("NumberOfLicensedUsers"),
	OLESTR("NumberOfProcesses"),
	OLESTR("NumberOfUsers"),
	OLESTR("OperatingSystemSKU"),
	OLESTR("Organization"),
	OLESTR("OSArchitecture"),
	OLESTR("OSLanguage"),
	OLESTR("OSProductSuite"),
	OLESTR("OSType"),
	OLESTR("OtherTypeDescription"),
	OLESTR("PAEEnabled"),
	OLESTR("PlusProductID"),
	OLESTR("PlusVersionNumber"),
	OLESTR("Primary"),
	OLESTR("ProductType"),
	OLESTR("RegisteredUser"),
	OLESTR("SerialNumber"),
	OLESTR("ServicePackMajorVersion"),
	OLESTR("ServicePackMinorVersion"),
	OLESTR("SizeStoredInPagingFiles"),
	OLESTR("Status"),
	OLESTR("SuiteMask"),
	OLESTR("SystemDevice"),
	OLESTR("SystemDirectory"),
	OLESTR("SystemDrive"),
	OLESTR("TotalSwapSpaceSize"),
	OLESTR("TotalVirtualMemorySize"),
	OLESTR("TotalVisibleMemorySize"),
	OLESTR("Version"),
	OLESTR("WindowsDirectory")
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HARDWAREINFODEFINE_H__INCLUDED_)
