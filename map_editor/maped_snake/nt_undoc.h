#ifndef __NT_UNDOC_H_DEF__
#define __NT_UNDOC_H_DEF__

#include <windows.h>

#define FILE_OPENED 0x00000001

typedef enum _FILE_INFORMATION_CLASS 
{
	FileDirectoryInformation = 1, 
	FileFullDirectoryInformation, 
	FileBothDirectoryInformation, 
	FileBasicInformation, 
	FileStandardInformation, 
	FileInternalInformation, 
	FileEaInformation, 
	FileAccessInformation, 
	FileNameInformation, 
	FileRenameInformation, 
	FileLinkInformation, 
	FileNamesInformation, 
	FileDispositionInformation, 
	FilePositionInformation, 
	FileFullEaInformation, 
	FileModeInformation, 
	FileAlignmentInformation, 
	FileAllInformation, 
	FileAllocationInformation, 
	FileEndOfFileInformation, 
	FileAlternateNameInformation, 
	FileStreamInformation, 
	FilePipeInformation, 
	FilePipeLocalInformation, 
	FilePipeRemoteInformation, 
	FileMailslotQueryInformation, 
	FileMailslotSetInformation, 
	FileCompressionInformation, 
	FileCopyOnWriteInformation, 
	FileCompletionInformation, 
	FileMoveClusterInformation, 
	FileQuotaInformation, 
	FileReparsePointInformation, 
	FileNetworkOpenInformation, 
	FileObjectIdInformation, 
	FileTrackingInformation, 
	FileOleDirectoryInformation, 
	FileContentIndexInformation, 
	FileInheritContentIndexInformation, 
	FileOleInformation, 
	FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef struct _FILE_DISPOSITION_INFORMATION {
	BOOLEAN DeleteFile;
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION;

typedef struct _LSA_UNICODE_STRING 
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;


typedef struct _OBJECT_ATTRIBUTES 
{
	ULONG           Length;
	HANDLE          RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG           Attributes;
	PVOID           SecurityDescriptor;
	PVOID           SecurityQualityOfService;
}  OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _IO_STATUS_BLOCK 
{
	union 
	{
		NTSTATUS Status;
		PVOID    Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


typedef VOID (NTAPI *PIO_APC_ROUTINE)(IN PVOID ApcContext, IN PIO_STATUS_BLOCK IoStatusBlock, IN ULONG Reserved);

typedef VOID (__stdcall *_RtlUnicodeStringInit)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString
	);

#define FILE_CREATE 0x00000002
#define FILE_NON_DIRECTORY_FILE 0x00000040
#define OBJ_CASE_INSENSITIVE 0x00000040L

#define InitializeObjectAttributes( i, o, a, r, s ) {    \
	(i)->Length = sizeof(OBJECT_ATTRIBUTES);         \
	(i)->RootDirectory = r;                            \
	(i)->Attributes = a;                               \
	(i)->ObjectName = o;                               \
	(i)->SecurityDescriptor = s;                       \
	(i)->SecurityQualityOfService = NULL;              \
}

typedef NTSTATUS (NTAPI *NtQueryDirectoryFile_t)(
	_In_      HANDLE FileHandle,
	_In_opt_  HANDLE Event,
	_In_opt_  PIO_APC_ROUTINE ApcRoutine,
	_In_opt_  PVOID ApcContext,
	_Out_     PIO_STATUS_BLOCK IoStatusBlock,
	_Out_     PVOID FileInformation,
	_In_      ULONG Length,
	_In_      FILE_INFORMATION_CLASS FileInformationClass,
	_In_      BOOLEAN ReturnSingleEntry,
	_In_opt_  PUNICODE_STRING FileName,
	_In_      BOOLEAN RestartScan
	);

typedef NTSTATUS (NTAPI *NtCreateFile_t)(
	_Out_     PHANDLE FileHandle,
	_In_      ACCESS_MASK DesiredAccess,
	_In_      POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_     PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_  PLARGE_INTEGER AllocationSize,
	_In_      ULONG FileAttributes,
	_In_      ULONG ShareAccess,
	_In_      ULONG CreateDisposition,
	_In_      ULONG CreateOptions,
	_In_      PVOID EaBuffer,
	_In_      ULONG EaLength
	);

typedef NTSTATUS(NTAPI *NtOpenFile_t)(
	_Out_     PHANDLE FileHandle,
	_In_      ACCESS_MASK DesiredAccess,
	_In_      POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_     PIO_STATUS_BLOCK IoStatusBlock,
	_In_	  ULONG ShareAccess,
	_In_      ULONG OpenOptions
	);

typedef void *PFILE_BASIC_INFORMATION;

typedef NTSTATUS (NTAPI *NtQueryAttributesFile_t)(
    _In_  POBJECT_ATTRIBUTES      ObjectAttributes,
    _Out_ PFILE_BASIC_INFORMATION FileInformation
    );

typedef NTSTATUS (NTAPI *NtWriteFile_t)(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
	);

typedef NTSTATUS (NTAPI *NtSetInformationFile_t)(
	IN HANDLE FileHandle, 
	OUT PIO_STATUS_BLOCK IoStatusBlock, 
	IN PVOID FileInformation, 
	IN ULONG Length, 
	IN FILE_INFORMATION_CLASS FileInformationClass
	);

typedef NTSTATUS (NTAPI *NtClose_t)(HANDLE Handle);

#endif
