#include "mm_utils.h"
#include <string.h>
#include <stdio.h>
#include <Shlwapi.h>
#include <Shlobj.h>

static mm_vehicle_data vehicles[] = {
	{ "6rb", "MG Metro 6R4", CLASS_GRB, 0 },
	{ "20b", "Peugeot 205 T16", CLASS_GRB, 0 },
	{ "20p", "Peugeot 205 T16 PP", CLASS_PIKESPEAK, 0 },
	{ "20x", "Peugeot 208 WRX", CLASS_RALLYX, 0 },
	{ "037", "Lancia 037 Evo 2", CLASS_GRB_RWD, 0 },
	{ "131", "Fiat 131 Abarth", CLASS_70, 0 },
	{ "306", "Peugeot 306 Maxi", CLASS_F2, 0 },
	{ "405", "Peugeot 405 T16 PP", CLASS_PIKESPEAK, 0 },
	{ "555", "Subaru Impreza 555", CLASS_GRA, 0 },
	{ "aqb", "Audi Sport Quattro Rallye", CLASS_GRB, 0 },
	{ "aqp", "Audi Sport Quattro S1 PP", CLASS_PIKESPEAK, 0 },
	{ "c4r", "Citro�n C4 WRC", CLASS_WRC00, 0 },
	{ "cme", "Ford Escort Mk II", CLASS_70, 0 },
	{ "dsx", "Citro�n DS3", CLASS_RALLYX, 0 },
	{ "ers", "Ford Escort RS Cosworth", CLASS_GRA, 0 },
	{ "ffr", "Ford Fiesta RS WRC", CLASS_WRC10, 0 },
	{ "for", "Ford Focus RS WRC 07", CLASS_WRC00, 0 },
	{ "fow", "Ford Focus RS WRC 01", CLASS_WRC00, 0 },
	{ "frx", "Ford Fiesta WRX", CLASS_RALLYX, 0 },
	{ "ful", "Lancia Fulvia HF", CLASS_60, 0 },
	{ "i20", "Hyundai i20 WRC", CLASS_WRC10, 0 },
	{ "imp", "Subaru Impreza WRC", CLASS_WRC00, 0 },
	{ "kad", "Opel Kadett GT/E 16v", CLASS_70, 0 },
	{ "ldi", "Lancia Delta HF Integrale", CLASS_GRA, 0 },
	{ "lds", "Lancia Delta S4", CLASS_GRB, 0 },
	{ "m3r", "BMW E30 M3 Evo Rally", CLASS_80, 0 },
	{ "mcr", "Mini Countryman JCW WRC", CLASS_WRC10, 0 },
	{ "mcs", "Mini Cooper S", CLASS_60, 0 },
	{ "mcx", "Mini Countryman JCW WRX", CLASS_RALLYX, 0 },
	{ "mer", "Mitsubishi Lancer Evolution X", CLASS_R4, 0 },
	{ "mta", "Opel Manta 400", CLASS_GRB_RWD, 0 },
	{ "pol", "Volkswagen Polo R WRC", CLASS_WRC10, 0 },
	{ "pox", "Volkswagen Polo RX", CLASS_RALLYX, 0 },
	{ "rsb", "Ford RS200", CLASS_GRB, 0 },
	{ "sik", "SEAT Ibiza Kit Car", CLASS_F2, 0 },
	{ "srs", "Ford Sierra Cosworth RS500", CLASS_80, 0 },
	{ "sti", "Subaru Impreza WRX STI", CLASS_R4, 0 },
	{ "str", "Lancia Stratos", CLASS_70, 0 },
	{ "stx", "Subaru Impreza WRX STi", CLASS_RALLYX, 0 },

	{ 0, "-", 0 }
};

static mm_stage_data stages[] = {
	{ "finland", "ouninpohja_rally_01", "Finland #1" },
	{ "finland", "ouninpohja_rally_02", "Finland #2" },
	{ "france", "col_de_turini_rally_01", "Monaco #1" },
	{ "france", "col_de_turini_rally_02", "Monaco #2" },
	{ "germany", "panzerplatte_rally_01", "Germany #1" },
	{ "germany", "panzerplatte_rally_02", "Germany #2" },
	{ "greece", "ghymno_rally_01", "Greece #1" },
	{ "greece", "ghymno_rally_02", "Greece #2" },
	{ "norway", "hell_rallycross", "Norway, Hell" },
	{ "sweden", "holjes_rallycross", "Sweden, Holjes" },
	{ "uk", "hafren_rally_01", "Wales #1" },
	{ "uk", "hafren_rally_02", "Wales #2" },
	{ "uk", "lydden_hill_rallycross", "UK, Lydden Hill" },
	{ "usa", "pikes_peak", "USA, Pikes Peak" },
	{ "usa", "pikes_peak_historic", "USA, Pikes Peak (Mixed)" }
};


const char *mm_get_file_extension(const char *file, size_t length)
{
	return mm_get_token_from_end(file, '.', length);
}

const char *mm_get_token_from_end(const char *str, char token, size_t length)
{
	if (length == 0)
		length = strlen(str);

	const char *substr = str;

	if (length > 0)
	{
		for (size_t i = length - 1; i >= 0; --i)
		{
			if (str[i] == token)
			{
				substr = &str[i + 1];
				break;
			}
		}
	}

	return substr;
}

void mm_str_cpy(char *dest, const char *src, size_t size)
{
	if (src != NULL)
	{
		while (--size && *src)
		{
			*dest++ = *src++;
		}
	}

	*dest = '\0';
}

char *mm_str_duplicate(const char *text)
{
	size_t len = strlen(text) + 1;
	char *str = new char[len];
	mm_str_cpy(str, text, len);

	return str;
}

void mm_str_replace_livery_slot(char *file_name, unsigned char livery)
{
	size_t l = strlen(file_name);

	// Go backwards until the extension has been stripped
	for (; l >= 0; --l)
	{
		if (file_name[l] == '.')
		{
			if (l >= 2)
				sprintf(&file_name[l - 2], "%02u.pssg", livery);

			break;
		}
	}
}

void mm_ensure_folder_exists(const char *path)
{
	DWORD attrib = GetFileAttributes(path);

	if (attrib == INVALID_FILE_ATTRIBUTES || (attrib & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		// If the entire path doesn't exist yet, create it.
		SHCreateDirectoryEx(NULL, path, NULL);
	}
}

bool mm_file_exists(const char *path)
{
	FILE *f = fopen(path, "r");

	if (f != NULL)
	{
		fclose(f);
		return true;
	}

	return false;
}

bool mm_folder_exists(const char *path)
{
	DWORD attrib = GetFileAttributes(path);

	if (attrib == INVALID_FILE_ATTRIBUTES || (attrib & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		return false;
	}

	return true;
}

mm_vehicle_data *mm_get_vehicle_data(const char *short_name)
{
	unsigned int index;

	for (index = 0; vehicles[index].short_name != 0; ++index)
	{
		if (strcmp(vehicles[index].short_name, short_name) == 0)
		{
			return &vehicles[index];
		}
	}

	return NULL;
}

void mm_scan_livery_list(const char *gamePath)
{
	static bool liveriesScanned = false;

	if (liveriesScanned)
		return;

	for (unsigned int i = 0; vehicles[i].short_name != NULL; ++i)
	{
		mm_vehicle_data *data = &vehicles[i];

		// Find all the sub-directories in the car model directory.
		char carModelPath[MAX_PATH];
		sprintf_s(carModelPath, sizeof(carModelPath), "%s\\cars\\models\\%s\\*", gamePath, data->short_name);

		WIN32_FIND_DATA findData;
		HANDLE findHandle = FindFirstFile(carModelPath, &findData);

		if (findHandle == INVALID_HANDLE_VALUE)
			continue;

		// Count all the sub-directories starting with 'livery_' and parse the livery index from the name.
		do {
			if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				const char *livery = findData.cFileName;

				if (strstr(livery, "livery_") != NULL &&
					livery[7] != 0 &&
					data->livery_count < MAX_LIVERIES)
				{
					data->liveries[data->livery_count++] = (unsigned char)atoi(&livery[7]);
				}
			}
		} while (FindNextFile(findHandle, &findData) != 0);

		FindClose(findHandle);
	}

	liveriesScanned = true;
}

mm_stage_data *mm_get_stage_data(const char *location, const char *short_name)
{
	unsigned int index;

	for (index = 0; stages[index].location != 0; ++index)
	{
		if (strcmp(stages[index].location, location))
			continue;

		if (strcmp(stages[index].short_name, short_name) == 0)
			return &stages[index];
	}

	return NULL;
}

//
// CRC32 checksum calculation
//

static unsigned int crc_32[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

inline unsigned int mm_update_crc32(int octet, unsigned int crc)
{
	return (crc_32[((crc) ^ ((unsigned char)octet)) & 0xff] ^ ((crc) >> 8));
}

unsigned int mm_calculate_file_crc32(const char *file_name)
{
	FILE *file = fopen(file_name, "r");

	// Can't open the file to calculate the checksum.
	if (file == NULL)
		return 0;
	
	unsigned int crc = 0xFFFFFFFF;
	int c;

	while ((c = getc(file)) != EOF)
	{
		crc = mm_update_crc32(c, crc);
	}

	fclose(file);

	crc = ~crc;
	return crc;
}
