#pragma once
#include "litehtml.h"

namespace litehtml {

	struct font_desc {
		const litehtml::tchar_t* faceName;
		int size;
		int weight;
		litehtml::font_style italic;
		unsigned int decoration;
		litehtml::font_metrics* fm;
	};

	struct image {
		size size;
	};

	class defaults {
	public:
		int font_size;
		litehtml::tstring font_face_name;
		litehtml::tstring language;
		litehtml::tstring culture;
	public:
		defaults() {
			font_size = 0;
		}
	};

	struct list_marker_ex
	{
		tstring			image;
		const tchar_t*	baseurl;
		list_style_type	marker_type;
		web_color		color;
		position		pos;
		int				index_in_list;
		uint_ptr		hFont;
	};

	typedef int (*text_width)(const litehtml::tchar_t * text, font_desc *hFont);
	typedef position(*get_client_rect)();
	typedef void(*draw_text)(litehtml::uint_ptr hdc, const litehtml::tchar_t * text, font_desc* hFont, litehtml::web_color color, const litehtml::position & pos);
	typedef void(*fill_font_metrics)(font_desc* hFont, font_metrics * fm);
	typedef void(*draw_background)(const background_paint &  bg);
	typedef void(*set_cursor)(const litehtml::tchar_t * cursor);
	typedef void(*draw_borders)(const litehtml::borders & borders, const litehtml::position & draw_pos, bool root);
	typedef image* (*load_image)(const litehtml::tchar_t * src, const litehtml::tchar_t * baseurl) ;
	typedef void(*draw_list_marker)(const litehtml::list_marker_ex & marker, font_desc* hFont);
	typedef void(*get_defaults)(defaults & defaults);

	struct wpf_container_binding {		
		
		text_width text_width;
		get_client_rect get_client_rect;
		draw_text draw_text;
		fill_font_metrics fill_font_metrics;
		draw_background draw_background;
		set_cursor set_cursor;
		load_image load_image;
		draw_borders draw_borders;
		draw_list_marker draw_list_marker;
		get_defaults get_defaults;
	};

}