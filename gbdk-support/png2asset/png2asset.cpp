#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include <fstream>
#include <cstdint>

#include "lodepng.h"
#include "mttile.h"
#include "export.h"
#include "map_attributes.h"
#include "palettes.h"

#include "cmp_int_color.h"

#include "png2asset.h"
#include "image_utils.h"
#include "image_data.h"
#include "maps.h"
#include "metasprites.h"
#include "metasprites_functions.h"
#include "process_arguments.h"
#include "png_image.h"
#include "tiles.h"

using namespace std;

int main(int argc, char* argv[])
{
	int errorCode = 0;

	// Read all arguments
	PNG2AssetArguments arguments;

	errorCode = processPNG2AssetArguments(argc, argv, &arguments);

	// Make sure we had no errors
	if(errorCode != 0) {
		return errorCode;
	}

	PNG2AssetData png2AssetInstance;

	// If we have a source tilest
	if(arguments.source_tilesets.size()>0) {

		// Create an exact copy of our tileset arguments
		PNG2AssetArguments sourceTilesetArguments;
		sourceTilesetArguments = arguments;

		vector<string>::iterator sourceTilesetsIterator = arguments.source_tilesets.begin();

		// Iterate through each source tileset and execute
		for(sourceTilesetsIterator; sourceTilesetsIterator < arguments.source_tilesets.end(); sourceTilesetsIterator++) {

			// Change our input filename to our current source tileset
			sourceTilesetArguments.input_filename = *sourceTilesetsIterator;

			// Run with our source tileset arguments
			errorCode = png2AssetInstance.Execute(&sourceTilesetArguments);

			// Return the error code if the function returns non-zero
			if(errorCode != 0) {
				return errorCode;
			}

		}

		arguments.source_tileset_size = png2AssetInstance.tiles.size();
		arguments.source_total_color_count = png2AssetInstance.image.total_color_count;
	}

	// Return the error code if the function returns non-zero
	if((errorCode = png2AssetInstance.Execute(&arguments)) != 0) {
		return errorCode;
	}

	return png2AssetInstance.Export();
}


int PNG2AssetData::Execute(PNG2AssetArguments* arguments){

	this->args = arguments;

	if(arguments->sprite_mode == SPR_8x8)
	{
		this->image.tile_w = 8;
		this->image.tile_h = 8;
	}
	else if(arguments->sprite_mode == SPR_8x16)
	{
		this->image.tile_w = 8;
		this->image.tile_h = 16;
	}
	else if(arguments->sprite_mode == SPR_16x16_MSX)
	{
		this->image.tile_w = 16;
		this->image.tile_h = 16;
	}

	int readImageDataValue = ReadImageData(this);

	// Return the error code if the function returns non-zero
	if(readImageDataValue != 0) {
		return readImageDataValue;
	}

	// Get the data depending on the type
	if(this->args->export_as_map)GetMap(this);
	else GetAllMetasprites(this);

	return 0;
}

int PNG2AssetData::Export() {

	// Header file export
	if(export_h_file(this) == false) return 1; // Exit with Fail

	if((this->args->export_as_map) && (this->args->output_binary)) {
		// Handle special case of binary map export
		export_map_binary(this);
	}
	else {
		// Normal source file export
		if(export_c_file(this) == false) return 1; // Exit with Fail
	}

	return 0;
}


bool FindTile(const Tile& t, size_t& idx, unsigned char& props, PNG2AssetData* assetData)
{
	vector< Tile >::iterator it;
	it = find(assetData->tiles.begin(), assetData->tiles.end(), t);
	if(it != assetData->tiles.end())
	{
		idx = (size_t)(it - assetData->tiles.begin());
		props = assetData->args->props_default;
		return true;
	}

	if(assetData->args->flip_tiles)
	{
		Tile tile = FlipV(t);
		it = find(assetData->tiles.begin(), assetData->tiles.end(), tile);
		if(it != assetData->tiles.end())
		{
			idx = (size_t)(it - assetData->tiles.begin());
			props = assetData->args->props_default | (1 << 5);
			return true;
		}

		tile = FlipH(tile);
		it = find(assetData->tiles.begin(), assetData->tiles.end(), tile);
		if(it != assetData->tiles.end())
		{
			idx = (size_t)(it - assetData->tiles.begin());
			props = assetData->args->props_default | (1 << 5) | (1 << 6);
			return true;
		}

		tile = FlipV(tile);
		it = find(assetData->tiles.begin(), assetData->tiles.end(), tile);
		if(it != assetData->tiles.end())
		{
			idx = (size_t)(it - assetData->tiles.begin());
			props = assetData->args->props_default | (1 << 6);
			return true;
		}
	}

	return false;
}