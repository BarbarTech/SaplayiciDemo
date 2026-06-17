#include <ntddk.h>
#include <wdf.h>

#define NDIS_SUPPORT_NDIS6 1

#include <initguid.h>
#include <fwpsk.h>
#include <fwpmk.h>
#include <guiddef.h>
#include <ndis.h>


#define NTSTRSAFE_LIB
#include <ntstrsafe.h>


#define DEVICE_NAME      L"\\Device\\SaplayiciDevice"
#define SYMBOLIC_LINK    L"\\DosDevices\\SaplayiciDevice"



DEFINE_GUID(SAPLAYICI_SUBLAYER_GUID,
	0x79378d80, 0xd48b, 0x47ba, 0x8e, 0x2c, 0x93, 0xa4, 0x0, 0xad, 0x37, 0x2b);


HANDLE g_EngineHandle = NULL;



#define MAX_FILTERS 1024 

UINT64 g_FilterIds[MAX_FILTERS];
int g_FilterCount = 0;

BOOLEAN g_IsActive = FALSE;



typedef struct _APPID_NODE {
	FWP_BYTE_BLOB* AppIdBlob;
	struct _APPID_NODE* Next;
} APPID_NODE, * PAPPID_NODE;

PAPPID_NODE g_AppIdList = NULL;


void ListeTemizle()
{
	PAPPID_NODE node = g_AppIdList;

	while (node)
	{
		PAPPID_NODE next = node->Next;

		if (node->AppIdBlob)
		{
			if (node->AppIdBlob->data)
				ExFreePool(node->AppIdBlob->data);

			ExFreePool(node->AppIdBlob);
		}

		ExFreePool(node);
		node = next;
	}

	g_AppIdList = NULL;
}

void ListeyeEkle(FWP_BYTE_BLOB* blob)
{
	PAPPID_NODE node =
		(PAPPID_NODE)ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(APPID_NODE), 'APID');

	if (!node) return;

	node->AppIdBlob = blob;
	node->Next = g_AppIdList;
	g_AppIdList = node;
}



/*
ipv4 filtre ekleme 
connect
condition tipi byte blob
filtre ağırlıgı 15 max

*/
NTSTATUS AddFilterIPv4_AppId(FWP_BYTE_BLOB* appId)
{
	FWPM_FILTER0 filter = { 0 };
	FWPM_FILTER_CONDITION0 condition = { 0 };
	UINT64 filterId = 0;

	filter.displayData.name = L"APP BLOCK V4 CONN";
	filter.layerKey = FWPM_LAYER_ALE_AUTH_CONNECT_V4;
	filter.subLayerKey = SAPLAYICI_SUBLAYER_GUID;

	condition.fieldKey = FWPM_CONDITION_ALE_APP_ID;
	condition.matchType = FWP_MATCH_EQUAL;
	condition.conditionValue.type = FWP_BYTE_BLOB_TYPE;
	condition.conditionValue.byteBlob = appId;

	filter.numFilterConditions = 1;
	filter.filterCondition = &condition;

	filter.action.type = FWP_ACTION_BLOCK;

	filter.weight.type = FWP_UINT8;
	filter.weight.uint8 = 0xF;

	NTSTATUS status = FwpmFilterAdd0(
		g_EngineHandle,
		&filter,
		NULL,
		&filterId
	);

	if (NT_SUCCESS(status))
	{
		if (g_FilterCount < MAX_FILTERS)
			g_FilterIds[g_FilterCount++] = filterId;
	}

	return status;
}


/*

Recv ipv4
*/
NTSTATUS AddFilterIPv4_Recv(FWP_BYTE_BLOB* appId)
{

	FWPM_FILTER0 filter = { 0 };
	FWPM_FILTER_CONDITION0 condition = { 0 };
	UINT64 filterId = 0;

	filter.displayData.name = L"APP BLOCK V4 RECV";
	filter.layerKey = FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4;
	filter.subLayerKey = SAPLAYICI_SUBLAYER_GUID;

	condition.fieldKey = FWPM_CONDITION_ALE_APP_ID;
	condition.matchType = FWP_MATCH_EQUAL;
	condition.conditionValue.type = FWP_BYTE_BLOB_TYPE;
	condition.conditionValue.byteBlob = appId;

	filter.numFilterConditions = 1;
	filter.filterCondition = &condition;

	filter.action.type = FWP_ACTION_BLOCK;

	filter.weight.type = FWP_UINT8;
	filter.weight.uint8 = 0xF;

	NTSTATUS status = FwpmFilterAdd0(
		g_EngineHandle,
		&filter,
		NULL,
		&filterId
	);

	if (NT_SUCCESS(status))
	{
		if (g_FilterCount < MAX_FILTERS)
			g_FilterIds[g_FilterCount++] = filterId;
	}

	return status;


}

/*

İPV4 FLOW ESTABLED

*/
NTSTATUS AddFilterIPv4Flow(FWP_BYTE_BLOB* appId)
{

	FWPM_FILTER0 filter = { 0 };
	FWPM_FILTER_CONDITION0 condition = { 0 };
	UINT64 filterId = 0;

	filter.displayData.name = L"APP BLOCK V4 ESTABLISHED";
	filter.layerKey = FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4;
	filter.subLayerKey = SAPLAYICI_SUBLAYER_GUID;

	condition.fieldKey = FWPM_CONDITION_ALE_APP_ID;
	condition.matchType = FWP_MATCH_EQUAL;
	condition.conditionValue.type = FWP_BYTE_BLOB_TYPE;
	condition.conditionValue.byteBlob = appId;

	filter.numFilterConditions = 1;
	filter.filterCondition = &condition;

	filter.action.type = FWP_ACTION_BLOCK;

	filter.weight.type = FWP_UINT8;
	filter.weight.uint8 = 0xF;

	NTSTATUS status = FwpmFilterAdd0(
		g_EngineHandle,
		&filter,
		NULL,
		&filterId
	);

	if (NT_SUCCESS(status))
	{
		if (g_FilterCount < MAX_FILTERS)
			g_FilterIds[g_FilterCount++] = filterId;
	}

	return status;


}

/*
ipv6 filtreleri
connect
condition tipi byte blob 
filtre ağırlıgı 15 max

*/

NTSTATUS AddfilteripV6_AppId(FWP_BYTE_BLOB* appId)
{
	FWPM_FILTER0 filter = { 0 };
	FWPM_FILTER_CONDITION0 condition = { 0 };
	UINT64 filterId = 0;

	filter.displayData.name = L"APP BLOCK V6 CONN";
	filter.layerKey = FWPM_LAYER_ALE_AUTH_CONNECT_V6;
	filter.subLayerKey = SAPLAYICI_SUBLAYER_GUID;

	condition.fieldKey = FWPM_CONDITION_ALE_APP_ID;
	condition.matchType = FWP_MATCH_EQUAL;
	condition.conditionValue.type = FWP_BYTE_BLOB_TYPE;
	condition.conditionValue.byteBlob = appId;

	filter.numFilterConditions = 1;
	filter.filterCondition = &condition;

	filter.action.type = FWP_ACTION_BLOCK;

	filter.weight.type = FWP_UINT8;
	filter.weight.uint8 = 0xF;

	NTSTATUS status = FwpmFilterAdd0(
		g_EngineHandle,
		&filter,
		NULL,
		&filterId
	);

	if (NT_SUCCESS(status))
	{
		if (g_FilterCount < MAX_FILTERS)
			g_FilterIds[g_FilterCount++] = filterId;
	}

	return status;

}

//İPv6 Recv
NTSTATUS AddFilterIPv6_Recv(FWP_BYTE_BLOB* appId)
{

	FWPM_FILTER0 filter = { 0 };
	FWPM_FILTER_CONDITION0 condition = { 0 };
	UINT64 filterId = 0;

	filter.displayData.name = L"APP BLOCK V6 RECV";
	filter.layerKey = FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6;
	filter.subLayerKey = SAPLAYICI_SUBLAYER_GUID;

	condition.fieldKey = FWPM_CONDITION_ALE_APP_ID;
	condition.matchType = FWP_MATCH_EQUAL;
	condition.conditionValue.type = FWP_BYTE_BLOB_TYPE;
	condition.conditionValue.byteBlob = appId;

	filter.numFilterConditions = 1;
	filter.filterCondition = &condition;

	filter.action.type = FWP_ACTION_BLOCK;

	filter.weight.type = FWP_UINT8;
	filter.weight.uint8 = 0xF;

	NTSTATUS status = FwpmFilterAdd0(
		g_EngineHandle,
		&filter,
		NULL,
		&filterId
	);

	if (NT_SUCCESS(status))
	{
		if (g_FilterCount < MAX_FILTERS)
			g_FilterIds[g_FilterCount++] = filterId;
	}

	return status;


}

/*
ipv6 FLOW
*/

NTSTATUS AddFilterIPv6Flow(FWP_BYTE_BLOB* appId)
{

	FWPM_FILTER0 filter = { 0 };
	FWPM_FILTER_CONDITION0 condition = { 0 };
	UINT64 filterId = 0;

	filter.displayData.name = L"APP BLOCK V6 ESTABLISHED";
	filter.layerKey = FWPM_LAYER_ALE_FLOW_ESTABLISHED_V6;
	filter.subLayerKey = SAPLAYICI_SUBLAYER_GUID;

	condition.fieldKey = FWPM_CONDITION_ALE_APP_ID;
	condition.matchType = FWP_MATCH_EQUAL;
	condition.conditionValue.type = FWP_BYTE_BLOB_TYPE;
	condition.conditionValue.byteBlob = appId;

	filter.numFilterConditions = 1;
	filter.filterCondition = &condition;

	filter.action.type = FWP_ACTION_BLOCK;

	filter.weight.type = FWP_UINT8;
	filter.weight.uint8 = 0xF;

	NTSTATUS status = FwpmFilterAdd0(
		g_EngineHandle,
		&filter,
		NULL,
		&filterId
	);

	if (NT_SUCCESS(status))
	{
		if (g_FilterCount < MAX_FILTERS)
			g_FilterIds[g_FilterCount++] = filterId;
	}

	return status;


}

/*
Katmanlar
ALE_AUTH_CONNECT (V4 + V6) var

ALE_AUTH_RECV_ACCEPT (V4 + V6) var

ALE_FLOW_ESTABLISHED (V4 + V6) var

Eklenebilir ALE_RESOURCE_ASSIGNMENT 
*/




void ClearAllFilters()
{
	if (!g_EngineHandle)
		return;

	for (int i = 0; i < g_FilterCount; i++)
	{

		NTSTATUS status = FwpmFilterDeleteById0(g_EngineHandle, g_FilterIds[i]);
		if (!NT_SUCCESS(status)) {
			DbgPrint("silme basarisiz: %x\n", status);
		}
	}

	g_FilterCount = 0;
}

void StopSaplayici()
{
	if (!g_IsActive)
		return;

	DbgPrint("Saplayici STOP\n");

	ClearAllFilters();
	ListeTemizle();

	g_IsActive = FALSE;
}


void StartSaplayici()
{
	if (!g_EngineHandle)
		return;

	if (g_IsActive)
		StopSaplayici(); 

	DbgPrint("SAPLAYICI START\n");

	PAPPID_NODE node = g_AppIdList;

	NTSTATUS status;
	while (node)
		{
			//ipv4conn
			
			status = AddFilterIPv4_AppId(node->AppIdBlob);

			if (NT_SUCCESS(status))
			{
			DbgPrint("ipv4 conn eklendi\n");
			}
			else {
			DbgPrint("ipv4 conn basarisiz: 0x%X\n", status);
			}

			status = AddFilterIPv4_Recv(node->AppIdBlob);

			if(NT_SUCCESS(status)){
				DbgPrint("ipv4 Recv eklendi\n");

			}
			else{
			DbgPrint("ipv4 Recv basarisiz: 0x%X\n", status);
			}
			
			status = AddFilterIPv4Flow(node->AppIdBlob);
			
			if(NT_SUCCESS(status)){
				DbgPrint("ipv4 Flow eklendi\n");

			}
			else{
			DbgPrint("ipv4 Flow basarisiz: 0x%X\n", status);
			}
				

				
			//ipv6conn 
			status = AddfilteripV6_AppId(node->AppIdBlob);

			if (NT_SUCCESS(status)) {

			DbgPrint("ipv6 conn eklendi \n");
			}
			else {
			DbgPrint("ipv6 conn basarisiz: 0x%X\n", status);
		}
			status = AddFilterIPv6_Recv(node->AppIdBlob);

			if(NT_SUCCESS(status)){
				DbgPrint("ipv6 Recv eklendi\n");

			}
			else{
			DbgPrint("ipv6 Recv basarisiz: 0x%X\n", status);
			}
			
			status = AddFilterIPv6Flow(node->AppIdBlob);
			
			if(NT_SUCCESS(status)){
				DbgPrint("ipv6 Flow eklendi\n");

			}
			else{
			DbgPrint("ipv6 Flow basarisiz: 0x%X\n", status);
			}

			
			
		node = node->Next;
			
		}

	/*
	//ipv4 connect 
	while (node)
	{
		NTSTATUS status = AddFilterIPv4_AppId(node->AppIdBlob);

		if (NT_SUCCESS(status)) {
			DbgPrint("ipv4 conn eklendi\n");
		}
		else {
			DbgPrint("ipv4 conn basarisiz: 0x%X\n", status);
		}
		

		node = node->Next;
	}


	//ipv6 conn
	node = g_AppIdList;
	while (node) {
		NTSTATUS status = AddfilteripV6_AppId(node->AppIdBlob);

		if (NT_SUCCESS(status)) {

			DbgPrint("ipv6 conn eklendi \n");
		}
		else {
			DbgPrint("ipv6 conn basarisiz: 0x%X\n", status);
		}
		node = node->Next;
	}
*/

	g_IsActive = TRUE;
}


NTSTATUS WfpInit()
{
	NTSTATUS status;

	status = FwpmEngineOpen0(NULL, RPC_C_AUTHN_WINNT, NULL, NULL, &g_EngineHandle);
	if (!NT_SUCCESS(status))
		return status;

	FWPM_SUBLAYER0 subLayer = { 0 };
	subLayer.subLayerKey = SAPLAYICI_SUBLAYER_GUID;
	subLayer.displayData.name = L"Saplayici Sublayer";
	subLayer.weight = 0x90000;

	return FwpmSubLayerAdd0(g_EngineHandle, &subLayer, NULL);
}


NTSTATUS IoControlDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;

	NTSTATUS status = STATUS_SUCCESS;

	switch (code)
	{

	case 0x222000:
	{
		BYTE* cursor = (BYTE*)Irp->AssociatedIrp.SystemBuffer;
		ULONG len = stack->Parameters.DeviceIoControl.InputBufferLength;
		BYTE* end = cursor + len;

		ListeTemizle();

		while (cursor + sizeof(UINT32) <= end)
		{
			UINT32 size = *(UINT32*)cursor;
			cursor += sizeof(UINT32);

			if (size == 0 || cursor + size > end)
				break;

			FWP_BYTE_BLOB* blob =
				(FWP_BYTE_BLOB*)ExAllocatePool2(
					POOL_FLAG_NON_PAGED,
					sizeof(FWP_BYTE_BLOB),
					'APID'
				);

			if (!blob)
				break;

			blob->size = size;
			blob->data =
				(UINT8*)ExAllocatePool2(
					POOL_FLAG_NON_PAGED,
					size,
					'APID'
				);

			if (!blob->data)
			{
				ExFreePool(blob);
				break;
			}

			RtlCopyMemory(blob->data, cursor, size);

			ListeyeEkle(blob);

			cursor += size;
		}

		StopSaplayici();
		StartSaplayici();

		break;
	}

	case 0x222004:
	{
		StopSaplayici();
		ListeTemizle();
		break;
	}

	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}


NTSTATUS CreateCloseDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

void UnloadDriver(PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING symLink;
	RtlInitUnicodeString(&symLink, SYMBOLIC_LINK);

	//sonradan ekledim driver kalkarken arka planda asılı kalıyordu
	ListeTemizle();
	StopSaplayici();
	ClearAllFilters();

	if (g_EngineHandle)
		FwpmEngineClose0(g_EngineHandle);

	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	g_IsActive = FALSE;
	g_FilterCount = 0;
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING devName, symLink;

	RtlInitUnicodeString(&devName, DEVICE_NAME);
	RtlInitUnicodeString(&symLink, SYMBOLIC_LINK);

	IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	IoCreateSymbolicLink(&symLink, &devName);

	DeviceObject->Flags |= DO_BUFFERED_IO;

	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = CreateCloseDispatch;

	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControlDispatch;
	DriverObject->DriverUnload = UnloadDriver;

	WfpInit();

	return STATUS_SUCCESS;
}
