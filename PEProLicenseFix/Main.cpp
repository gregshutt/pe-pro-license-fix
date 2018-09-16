#include <stdio.h>
#include <Windows.h>

int readSector(const wchar_t* disk, char* buffer, unsigned int sector)
{
	DWORD dwRead;
	HANDLE hDisk = CreateFile(disk, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);

	if (hDisk == INVALID_HANDLE_VALUE) {
		CloseHandle(hDisk);
		fprintf(stderr, "Could not open disk %ls: 0x%x\n", disk, GetLastError());
		return(-1);
	}

	if (SetFilePointer(hDisk, sector * 512, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		CloseHandle(hDisk);
		return(-1);
	}

	if (ReadFile(hDisk, buffer, 512, &dwRead, NULL) == FALSE) {
		CloseHandle(hDisk);
		return(-1);
	}

	if (dwRead != 512) {
		CloseHandle(hDisk);
		return(-1);
	}

	CloseHandle(hDisk);

	return(0);
}

int writeSector(const wchar_t* disk, char* buffer, unsigned int sector)
{
	DWORD dwWritten;
	HANDLE hDisk = CreateFile(disk, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);

	if (hDisk == INVALID_HANDLE_VALUE) {
		CloseHandle(hDisk);
		fprintf(stderr, "Could not open disk %ls: 0x%x\n", disk, GetLastError());
		return(-1);
	}

	if (SetFilePointer(hDisk, sector * 512, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		CloseHandle(hDisk);
		return(-1);
	}

	if (WriteFile(hDisk, buffer, 512, &dwWritten, NULL) == FALSE) {
		CloseHandle(hDisk);
		return(-1);
	}

	if (dwWritten != 512) {
		CloseHandle(hDisk);
		return(-1);
	}

	CloseHandle(hDisk);

	return(0);
}

int main()
{
	char buffer[512];

	// read sector 0
	printf("Checking sector 0...");
	if (readSector(L"\\\\.\\PhysicalDrive0", buffer, 0) != 0) {
		fprintf(stderr, "Could not read first sector");
		exit(1);
	}
	printf("done.\n");

	// read sector 60
	printf("Checking for license sector...");
	if (readSector(L"\\\\.\\PhysicalDrive0", buffer, 60) != 0) {
		fprintf(stderr, "Could not read first sector");
		exit(1);
	}

	UINT32 magic = buffer[511] << 24 | buffer[510] << 16 | buffer[509] << 8 | buffer[508];

	if (magic == 0x0045561D) {
		printf("found.\n");
	}
	else {
		printf("not found. Are you sure PE Pro is installed? (Expected 0x0045561D, got 0x%0x)\n", magic);
		exit(0);
	}

	memset(buffer, 0, sizeof(buffer));

	printf("Clearing license...");
	if (writeSector(L"\\\\.\\PhysicalDrive0", buffer, 60) != 0) {
		printf("failed.\n");
	}
	else {
		printf("succeeded.\n");
	}
}