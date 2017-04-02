#include "stdafx.h"
#include "el_ol.h"
#include "wpf_container.h"

litehtml::el_ol::el_ol(const std::shared_ptr<litehtml::document>& doc) : litehtml::html_tag(doc)
{

}

litehtml::el_ol::~el_ol()
{

}

void litehtml::el_ol::draw(uint_ptr hdc, int x, int y, const position* clip)
{
	position pos = m_pos;
	pos.x += x;
	pos.y += y;

	draw_background(hdc, x, y, clip);

	if (m_display == display_list_item && m_list_style_type != list_style_type_none)
	{
		if (m_overflow > overflow_visible)
		{
			position border_box = pos;
			border_box += m_padding;
			border_box += m_borders;

			border_radiuses bdr_radius = m_css_borders.radius.calc_percents(border_box.width, border_box.height);

			bdr_radius -= m_borders;
			bdr_radius -= m_padding;

			get_document()->container()->set_clip(pos, bdr_radius, true, true);
		}

		draw_list_marker(hdc, pos);

		if (m_overflow > overflow_visible)
		{
			get_document()->container()->del_clip();
		}
	}
}

void litehtml::el_ol::draw_list_marker(uint_ptr hdc, const position &pos)
{
	list_marker_ex lm;

	const tchar_t* list_image = get_style_property(_t("list-style-image"), true, 0);
	size img_size;

	if (list_image)
	{
		css::parse_css_url(list_image, lm.image);
		lm.baseurl = get_style_property(_t("list-style-image-baseurl"), true, 0);
		get_document()->container()->get_image_size(lm.image.c_str(), lm.baseurl, img_size);
	}
	else
	{
		lm.baseurl = 0;
	}


	int ln_height = line_height();
	int sz_font = get_font_size();
	lm.pos.x = pos.x;
	lm.pos.width = sz_font - sz_font * 2 / 3;
	lm.pos.height = sz_font - sz_font * 2 / 3;
	lm.pos.y = pos.y + ln_height / 2 - lm.pos.height / 2;

	if (img_size.width && img_size.height)
	{
		if (lm.pos.y + img_size.height > pos.y + pos.height)
		{
			lm.pos.y = pos.y + pos.height - img_size.height;
		}
		if (img_size.width > lm.pos.width)
		{
			lm.pos.x -= img_size.width - lm.pos.width;
		}

		lm.pos.width = img_size.width;
		lm.pos.height = img_size.height;
	}
	if (m_list_style_position == list_style_position_outside)
	{
		lm.pos.x -= sz_font;
	}
	if (wcscmp(L"ol", this->parent()->get_tagName()) == 0) {
		// we are in ordered list
		// get position in list and inject to baseurl - this isn't nice, but so we don't need to change that much in litehtml
		auto children = this->parent()->get_children_count();
		int j = 1;
		for (size_t i = 0; i < children; i++) {
			auto child = this->parent()->get_child((int)i);
			if (wcscmp(L"li", child->get_tagName()) == 0) {
				if (child.get() == this) {
					lm.index_in_list = j;
					break;
				}
				j++;
			}
		}
	}

	lm.color = get_color(_t("color"), true, web_color(0, 0, 0));
	lm.marker_type = m_list_style_type;
	lm.hFont = get_font();

	switch (lm.marker_type)
	{
	case list_style_type_lower_alpha:
	case list_style_type_lower_latin:
	case list_style_type_upper_alpha:
	case list_style_type_upper_latin:
	case list_style_type_decimal:
		lm.pos.y = pos.y;
		break;
	}

	
	((wpf_container*)get_document()->container())->draw_list_marker_ex(hdc, lm);	
}