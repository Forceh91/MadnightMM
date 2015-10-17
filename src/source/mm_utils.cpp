#include "mm_utils.h"
#include <string.h>

static mm_vehicle_data vehicles[] = {
	{ "6rb", "MG Metro 6R4", CLASS_GRB },
	{ "20b", "Peugeot 205 T16", CLASS_GRB },
	{ "20p", "Peugeot 205 T16 PP", CLASS_PIKESPEAK },
	{ "20x", "Peugeot 208 WRX", CLASS_RALLYX },
	{ "037", "Lancia 037 Evo 2", CLASS_GRB_RWD },
	{ "131", "Fiat 131 Abarth", CLASS_70 },
	{ "306", "Peugeot 306 Maxi", CLASS_F2 },
	{ "405", "Peugeot 405 T16 PP", CLASS_PIKESPEAK },
	{ "555", "Subaru Impreza 555", CLASS_GRA },
	{ "aqb", "Audi Sport Quattro Rallye", CLASS_GRB },
	{ "aqp", "Audi Sport Quattro S1 PP", CLASS_PIKESPEAK },
	{ "cme", "Ford Escort Mk II", CLASS_70 },
	{ "dsx", "Citroën DS3", CLASS_RALLYX },
	{ "ers", "Ford Escort RS Cosworth", CLASS_GRA },
	{ "ffr", "Ford Fiesta RS WRC", CLASS_WRC10 },
	{ "fow", "Ford Focus RS WRC", CLASS_WRC00 },
	{ "frx", "Ford Fiesta WRX", CLASS_RALLYX },
	{ "ful", "Lancia Fulvia HF", CLASS_60 },
	{ "imp", "Subaru Impreza WRC", CLASS_WRC00 },
	{ "kad", "Opel Kadett GT/E 16v", CLASS_70 },
	{ "ldi", "Lancia Delta HF Integrale", CLASS_GRA },
	{ "lds", "Lancia Delta S4", CLASS_GRB },
	{ "m3r", "BMW E30 M3 Evo Rally", CLASS_80 },
	{ "mcr", "Mini Countryman JCW WRC", CLASS_WRC10 },
	{ "mcs", "Mini Cooper S", CLASS_60 },
	{ "mcx", "Mini Countryman JCW WRX", CLASS_RALLYX },
	{ "mta", "Opel Manta 400", CLASS_GRB_RWD },
	{ "pox", "Volkswagen Polo RX", CLASS_RALLYX },
	{ "rsb", "Ford RS200", CLASS_GRB },
	{ "sik", "SEAT Ibiza Kit Car", CLASS_F2 },
	{ "srs", "Ford Sierra Cosworth RS500", CLASS_80 },
	{ "str", "Lancia Stratos", CLASS_70 },
	{ "stx", "Subaru Impreza WRX STi", CLASS_RALLYX },

	{ 0, "-", 0 }
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
