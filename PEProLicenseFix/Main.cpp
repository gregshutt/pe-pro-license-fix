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
	if (readSector(L"\\\\.\\PhysicalDrive0", buffer, 0) != 0) {
		fprintf(stderr, "Could not read first sector");
		exit(1);
	}

	if (buffer[508] == 0x00 && buffer[509] == 0x45 && buffer[510] == 0x56 && buffer[511] == 0x1D) {
		printf("found.\n");
	}
	else {
		printf("not found. Are you sure PE Pro is installed?\n");
		exit(0);
	}

	getchar();
}