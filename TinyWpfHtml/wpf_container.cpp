#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")

#pragma unmanaged
#pragma warning( disable : 4311 4302 )

#include "stdafx.h"
#include "wpf_container_binding.h"
#include "wpf_container.h"
#include "el_ol.h"
#include <assert.h>
namespace litehtml {

	

	wpf_container::wpf_container(wpf_container_binding *binding)
	{
		_binding = std::shared_ptr<wpf_container_binding>(binding);
		_fonts = new vector<font_desc*>();
	}


	wpf_container::~wpf_container()
	{	
		delete _fonts;
	}

	litehtml::uint_ptr wpf_container::create_font(const litehtml::tchar_t * faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics * fm)
	{
		font_desc *fd = new font_desc();
		fd->faceName = faceName;
		fd->size = size;
		fd->weight = weight;
		fd->italic = italic;
		fd->decoration = decoration;
		fd->fm = fm; // TODO, may be invalid - but we don't really need that
		_binding->fill_font_metrics(fd, fm);
		
		_fonts->push_back(fd);
		return (litehtml::uint_ptr)(_fonts->size() - 1);
	}

	void wpf_container::delete_font(litehtml::uint_ptr hFont)
	{
		font_desc* fd = _fonts->at((int)hFont);
		//_fonts.erase(_fonts.begin()+hFont); that's a bad idea ^^
		delete fd;
	}

	int wpf_container::text_width(const litehtml::tchar_t * text, litehtml::uint_ptr hFont)
	{
		return _binding->text_width(text, _fonts->at((int)hFont));
	}

	void wpf_container::draw_text(litehtml::uint_ptr hdc, const litehtml::tchar_t * text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position & pos)
	{
		_binding->draw_text(hdc, text, _fonts->at((int)hFont), color, pos);
	}

	int wpf_container::pt_to_px(int pt)
	{
		auto hdc = GetDC(0);
		if (hdc != 0)
		{
			auto dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
			ReleaseDC(0, hdc);
			return (int)(dpiX / 96.0 * pt);
		}
		return pt;
	}

	int wpf_container::get_default_font_size() const
	{
		defaults def;
		_binding->get_defaults(def);
		return def.font_size;
	}

	const litehtml::tchar_t * wpf_container::get_default_font_name() const
	{		
		defaults def;		
		_binding->get_defaults(def);
		tchar_t* font = (tchar_t*)calloc(def.font_face_name.length() + 1, 2);
		wcscpy_s(font, def.font_face_name.length()+1, def.font_face_name.c_str());
		return font;
	}

	void wpf_container::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker & marker)
	{
		//_binding->draw_list_marker(marker);
	}

	void wpf_container::draw_list_marker_ex(litehtml::uint_ptr hdc, const litehtml::list_marker_ex & marker)
	{
		_binding->draw_list_marker(marker, _fonts->at(marker.hFont));
	}

	void wpf_container::load_image(const litehtml::tchar_t * src, const litehtml::tchar_t * baseurl, bool redraw_on_ready)
	{
		auto img = _binding->load_image(src, baseurl);
		if (img) { delete img; }
	}

	void wpf_container::get_image_size(const litehtml::tchar_t * src, const litehtml::tchar_t * baseurl, litehtml::size & sz)
	{
		auto img = _binding->load_image(src, baseurl);

		if (img) {
			sz.width = img->size.width;
			sz.height = img->size.height;
			delete img;
		}
	}

	void wpf_container::draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint & bg)
	{
		_binding->draw_background(bg);
	}

	void wpf_container::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders & borders, const litehtml::position & draw_pos, bool root)
	{
		_binding->draw_borders(borders, draw_pos, root);
	}

	void wpf_container::set_caption(const litehtml::tchar_t * caption)
	{
	}

	void wpf_container::set_base_url(const litehtml::tchar_t * base_url)
	{
	}

	void wpf_container::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr & el)
	{
	}

	void wpf_container::on_anchor_click(const litehtml::tchar_t * url, const litehtml::element::ptr & el)
	{
		_binding->on_anchor_click(url);
	}

	void wpf_container::set_cursor(const litehtml::tchar_t * cursor)
	{
		_binding->set_cursor(cursor);
	}

	void wpf_container::transform_text(litehtml::tstring & text, litehtml::text_transform tt)
	{
		if (text.empty()) return;
		switch (tt)
		{
		case litehtml::text_transform_capitalize:
			if (!text.empty())
			{
				text[0] = (WCHAR)CharUpper((LPWSTR)text[0])[0];
			}
			break;
		case litehtml::text_transform_uppercase:
			for (size_t i = 0; i < text.length(); i++)
			{
				text[i] = (WCHAR)CharUpper((LPWSTR)text[i])[0];
			}
			break;
		case litehtml::text_transform_lowercase:
			for (size_t i = 0; i < text.length(); i++)
			{
				text[i] = (WCHAR)CharLower((LPWSTR)text[i])[0];
			}
			break;
		}
	}

	void wpf_container::import_css(litehtml::tstring & text, const litehtml::tstring & url, litehtml::tstring & baseurl)
	{
	}

	void wpf_container::set_clip(const litehtml::position & pos, const litehtml::border_radiuses & bdr_radius, bool valid_x, bool valid_y)
	{
	}

	void wpf_container::del_clip()
	{
	}

	void wpf_container::get_client_rect(litehtml::position & client) const
	{
		client = _binding->get_client_rect();
	}

	std::shared_ptr<litehtml::element> wpf_container::create_element(const litehtml::tchar_t * tag_name, const litehtml::string_map & attributes, const std::shared_ptr<litehtml::document>& doc)
	{
		// callback on element creation
		if(!t_strcmp(tag_name, _t("li")))
		{
			return std::make_shared<litehtml::el_ol>(doc);
		}

		return 0;
	}

	void wpf_container::get_media_features(litehtml::media_features & media) const
	{
		litehtml::position client;
		get_client_rect(client);
		HDC hdc = GetDC(NULL);

		media.type = litehtml::media_type_screen;
		media.width = client.width;
		media.height = client.height;
		media.color = 8;
		media.monochrome = 0;
		media.color_index = 256;
		media.resolution = GetDeviceCaps(hdc, LOGPIXELSX);
		media.device_width = GetDeviceCaps(hdc, HORZRES);
		media.device_height = GetDeviceCaps(hdc, VERTRES);

		ReleaseDC(NULL, hdc);

	}

	void wpf_container::get_language(litehtml::tstring & language, litehtml::tstring & culture) const
	{
		defaults def;
		_binding->get_defaults(def);
		tchar_t* wlanguage = (tchar_t*)calloc(def.language.length() + 1, 2);
		wcscpy_s(wlanguage, def.language.length() + 1, def.language.c_str());
		tchar_t* wculture = (tchar_t*)calloc(def.culture.length() + 1, 2);
		wcscpy_s(wculture, def.culture.length() + 1, def.culture.c_str());
		language = wlanguage;
		culture = wculture;
	}


}