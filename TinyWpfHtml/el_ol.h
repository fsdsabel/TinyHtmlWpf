#pragma once
#include <Windows.h>
#include <litehtml.h>
#include "..\litehtml\src\html_tag.h"

namespace litehtml
{
	class el_ol : public html_tag
	{
	public:
		el_ol(const std::shared_ptr<litehtml::document>& doc);
		virtual ~el_ol();

		virtual void draw(uint_ptr hdc, int x, int y, const position* clip) override;
	protected:
		void draw_list_marker(uint_ptr hdc, const position &pos);
	};
}
