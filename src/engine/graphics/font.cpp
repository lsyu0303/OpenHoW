/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <PL/platform_filesystem.h>

#include "../engine.h"

#include "font.h"
#include "display.h"

BitmapFont* g_fonts[NUM_FONTS];

PLMesh* font_mesh = nullptr;

void Font_DrawBitmapCharacter( BitmapFont* font, int x, int y, float scale, PLColour colour, uint8_t character ) {
	if ( font == nullptr || scale == 0 ) {
		return;
	}

	int dw = g_state.ui_camera->viewport.w;
	int dh = g_state.ui_camera->viewport.h;
	if ( x > dw || y > dh ) {
		return;
	}

	// ensure that the character we're being passed fits within HoW's bitmap set
	if ( character < 33 || character > 138 ) {
		return;
	}
	character -= 33;

	if ( font->texture == nullptr ) {
		Error( "attempted to draw bitmap font with invalid texture, aborting!\n" );
	}

	if ( font_mesh == nullptr ) {
		Error( "attempted to draw font before font init, aborting!\n" );
	}

	plSetTexture( font->texture, 0 );

	plClearMesh( font_mesh );

	plSetMeshUniformColour( font_mesh, colour );

	BitmapChar* bitmap_char = &font->chars[ character ];
	plSetMeshVertexPosition( font_mesh, 0, PLVector3( x, y, 0 ) );
	plSetMeshVertexPosition( font_mesh, 1, PLVector3( x, y + ( bitmap_char->h * scale ), 0 ) );
	plSetMeshVertexPosition( font_mesh, 2, PLVector3( x + ( bitmap_char->w * scale ), y, 0 ) );
	plSetMeshVertexPosition( font_mesh,
							 3,
							 PLVector3( x + ( bitmap_char->w * scale ), y + ( bitmap_char->h * scale ), 0 ) );

	float tw = ( float ) bitmap_char->w / font->width;
	float th = ( float ) bitmap_char->h / font->height;
	float tx = ( float ) bitmap_char->x / font->width;
	float ty = ( float ) bitmap_char->y / font->height;
	plSetMeshVertexST( font_mesh, 0, tx, ty );
	plSetMeshVertexST( font_mesh, 1, tx, ty + th );
	plSetMeshVertexST( font_mesh, 2, tx + tw, ty );
	plSetMeshVertexST( font_mesh, 3, tx + tw, ty + th );

	plSetNamedShaderUniformMatrix4( NULL, "pl_model", plMatrix4Identity(), false );
	plUploadMesh( font_mesh );
	plDrawMesh( font_mesh );
}

void Font_DrawBitmapString( BitmapFont* font, int x, int y, unsigned int spacing, float scale, PLColour colour,
							const char* msg ) {
	if ( font == nullptr ) {
		return;
	}

	if ( scale == 0 ) {
		return;
	}

	auto num_chars = strlen( msg );
	if ( num_chars == 0 ) {
		return;
	}

	if ( font->texture == nullptr ) {
		Error( "attempted to draw bitmap font with invalid texture, aborting!\n" );
	}

	plSetBlendMode( PL_BLEND_ADDITIVE );

	int n_x = x;
	int n_y = y;
	for ( size_t i = 0; i < num_chars; ++i ) {
		Font_DrawBitmapCharacter( font, n_x, n_y, scale, colour, ( uint8_t ) msg[ i ] );
		if ( msg[ i ] >= 33 && msg[ i ] <= 122 ) {
			n_x += font->chars[ msg[ i ] - 33 ].w + spacing;
		} else if ( msg[ i ] == '\n' ) {
			n_y += font->chars[ 0 ].h;
			n_x = x;
		} else {
			n_x += 5;
		}
	}

	plSetBlendMode( PL_BLEND_DEFAULT );
}

BitmapFont* LoadBitmapFont( const char* name, const char* tab_name ) {
	char path[PL_SYSTEM_MAX_PATH];
	snprintf( path, sizeof( path ) - 1, "frontend/text/%s.tab", tab_name );
	PLFile* tab_file = plOpenFile( path, false );
	if ( tab_file == nullptr ) {
		LogWarn( "Failed to load tab \"%s\", aborting (%s)!\n", path, plGetError() );
		return nullptr;
	}

	plFileSeek( tab_file, 16, PL_SEEK_CUR );

#define MAX_CHARS   256
	struct {
		uint16_t x;
		uint16_t y;
		uint16_t w;
		uint16_t h;
	} tab_indices[MAX_CHARS];
	auto num_chars = ( unsigned int ) plReadFile( tab_file, tab_indices, sizeof( tab_indices ) / MAX_CHARS, MAX_CHARS );
	if ( num_chars == 0 ) {
		Error( "Invalid number of characters for \"%s\", aborting!\n", path );
	}
	plCloseFile( tab_file );

	// Load in the image
	snprintf( path, sizeof( path ) - 1, "frontend/text/%s", name );
	const char* tex_path = u_find2( path, supported_image_formats, true );
	PLImage image;
	if ( !plLoadImage( tex_path, &image ) ) {
		Error( "Failed to load in image, %s, aborting (%s)!\n", tex_path, plGetError() );
	}

	plReplaceImageColour( &image, PLColour( 255, 0, 255, 255 ), PLColour( 0, 0, 0, 0 ) );

	auto* font = static_cast<BitmapFont*>(u_alloc( 1, sizeof( BitmapFont ), true ));
	memset( font, 0, sizeof( BitmapFont ) );

	font->width = image.width;
	font->height = image.height;
	font->num_chars = num_chars;

	unsigned int origin_x = tab_indices[ 0 ].x;
	unsigned int origin_y = tab_indices[ 0 ].y;
	for ( unsigned int i = 0; i < font->num_chars; ++i ) {
		font->chars[ i ].w = tab_indices[ i ].w;
		font->chars[ i ].h = tab_indices[ i ].h;
		font->chars[ i ].x = tab_indices[ i ].x - origin_x;
		font->chars[ i ].y = tab_indices[ i ].y - origin_y;
#if 0 // debug
		print(
				"font char %d: w(%d) h(%d) x(%d) y(%d)\n",
				i,
				font->chars[i].w,
				font->chars[i].h,
				font->chars[i].x,
				font->chars[i].y
		);
#endif
	}

	// upload the texture to the GPU

	font->texture = plCreateTexture();
	if ( font->texture == nullptr ) {
		Error( "Failed to create texture for font, %s, aborting (%s)!\n", name, plGetError() );
	}

	font->texture->filter = PL_TEXTURE_FILTER_LINEAR;

	plUploadTextureImage( font->texture, &image );
	plFreeImage( &image );

	return font;
}

//////////////////////////////////////////////////////////////////////////

void CacheFontData() {
	font_mesh = plCreateMesh( PL_MESH_TRIANGLE_STRIP, PL_DRAW_DYNAMIC, 2, 4 );
	if ( font_mesh == nullptr ) {
		Error( "failed to create font mesh, %s, aborting!\n", plGetError() );
	}

	g_fonts[ FONT_BIG ] = LoadBitmapFont( "big", "big" );
	g_fonts[ FONT_BIG_CHARS ] = LoadBitmapFont( "bigchars", "bigchars" );
	g_fonts[ FONT_CHARS2 ] = LoadBitmapFont( "chars2l", "chars2" );
	g_fonts[ FONT_CHARS3 ] = LoadBitmapFont( "chars3", "chars3" );
	g_fonts[ FONT_GAME_CHARS ] = LoadBitmapFont( "gamechars", "gamechars" );
	g_fonts[ FONT_SMALL ] = LoadBitmapFont( "small", "small" );
}

void ClearFontData() {
	for ( auto& g_font : g_fonts ) {
		if ( g_font == nullptr ) {
			/* if we hit a null slot, it's possible the fonts
			 * failed loading at this point. so we'll just
			 * break here. */
			LogDebug( "hit null font in shutdown fonts, skipping the rest!\n" );
			break;
		}

		plDestroyTexture( g_font->texture, true );
		free( g_font );
	}
}