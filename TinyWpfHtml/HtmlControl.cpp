#include "stdafx.h"
#include "HtmlControl.h"
#include <memory>
#include <vcclr.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "TreeHelpers.h"

using namespace LiteHtml;
using namespace litehtml;
using namespace std;
using namespace System;
using namespace System::Reflection;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Media;
using namespace System::Globalization;
using namespace System::Windows;
using namespace System::Windows::Media::Imaging;
using namespace System::Windows::Controls::Primitives;

#pragma warning( disable : 4244 )


struct Radii
{
	double LeftTop;

	double TopLeft;

	double TopRight;

	double RightTop;

	double RightBottom;

	double BottomRight;

	double BottomLeft;

	double LeftBottom;

	Radii(CornerRadius radii, Thickness borders, bool outer)
	{
		double num = 0.5 * borders.Left;
		double num2 = 0.5 * borders.Top;
		double num3 = 0.5 * borders.Right;
		double num4 = 0.5 * borders.Bottom;
		if (!outer)
		{
			LeftTop = max(0.0, radii.TopLeft - num);
			TopLeft = max(0.0, radii.TopLeft - num2);
			TopRight = max(0.0, radii.TopRight - num2);
			RightTop = max(0.0, radii.TopRight - num3);
			RightBottom = max(0.0, radii.BottomRight - num3);
			BottomRight = max(0.0, radii.BottomRight - num4);
			BottomLeft = max(0.0, radii.BottomLeft - num4);
			LeftBottom = max(0.0, radii.BottomLeft - num);
			return;
		}
		if (radii.TopLeft==0)
		{
			LeftTop = (TopLeft = 0.0);
		}
		else
		{
			LeftTop = radii.TopLeft + num;
			TopLeft = radii.TopLeft + num2;
		}
		if (radii.TopRight == 0)
		{
			TopRight = (RightTop = 0.0);
		}
		else
		{
			TopRight = radii.TopRight + num2;
			RightTop = radii.TopRight + num3;
		}
		if (radii.BottomRight == 0)
		{
			RightBottom = (BottomRight = 0.0);
		}
		else
		{
			RightBottom = radii.BottomRight + num3;
			BottomRight = radii.BottomRight + num4;
		}
		if (radii.BottomLeft == 0)
		{
			BottomLeft = (LeftBottom = 0.0);
			return;
		}
		BottomLeft = radii.BottomLeft + num4;
		LeftBottom = radii.BottomLeft + num;
	}
};



static HtmlControl::HtmlControl() {
	_context = new context();
	_bitmaps = gcnew ImageCache();
}

void HtmlControl::SetMasterStylesheet(String^ css) {
	pin_ptr<const wchar_t> pcss = PtrToStringChars(css);
	_context->load_master_stylesheet(pcss);
}

HtmlControl::HtmlControl()	
{
	RenderOptions::SetBitmapScalingMode(this, BitmapScalingMode::HighQuality);

	_instanceBitmaps = gcnew List<String^>();	
	
	this->CreateContainerBinding();
	_clientSize = Size(Width, Height);
	_usercss = nullptr;

	_wpf_container = new wpf_container(_container_binding);
	Unloaded += gcnew RoutedEventHandler(this, &HtmlControl::OnUnloaded);
}


HtmlControl::!HtmlControl() {	
	// we cannot free those .. it crashes because the _document still needs it
	//_document = nullptr;
	//delete _wpf_container;
	//delete _container_binding;	
	//delete _wpf_container;
	//delete _container_binding;
	CleanupImages();
}

void HtmlControl::CreateContainerBinding() {
	_container_binding = new wpf_container_binding();
	_textWidthtDel = gcnew TextWidthDelegate(this, &HtmlControl::GetTextWidth);
	_getClientRectDel = gcnew GetClientRectDelegate(this, &HtmlControl::GetClientRect);
	_drawTextDel = gcnew DrawTextDelegate(this, &HtmlControl::DrawText);
	_fillFontMetricsDel = gcnew FillFontMetricsDelegate(this, &HtmlControl::FillFontMetrics);
	_drawBgDel = gcnew DrawBackgroundDelegate(this, &HtmlControl::DrawBackground);
	_setCursorDel = gcnew SetCursorDelegate(this, &HtmlControl::SetCursor);
	_loadImageDel = gcnew LoadImageDelegate(this, &HtmlControl::LoadHtmlImage);
	_drawBordersDel = gcnew DrawBordersDelegate(this, &HtmlControl::DrawBorders);
	_drawListMarkerDel = gcnew DrawListMarkerDelegate(this, &HtmlControl::DrawListMarker);
	_getDefaultsDel = gcnew GetDefaultsDelegate(this, &HtmlControl::GetDefaults);
	_onAnchorClick = gcnew OnAnchorClickDelegate(this, &HtmlControl::OnAnchorClick);


	_container_binding->text_width = (text_width)Marshal::GetFunctionPointerForDelegate(_textWidthtDel).ToPointer();
	_container_binding->get_client_rect = (get_client_rect)Marshal::GetFunctionPointerForDelegate(_getClientRectDel).ToPointer();
	_container_binding->draw_text = (draw_text)Marshal::GetFunctionPointerForDelegate(_drawTextDel).ToPointer();
	_container_binding->fill_font_metrics = (fill_font_metrics)Marshal::GetFunctionPointerForDelegate(_fillFontMetricsDel).ToPointer();
	_container_binding->draw_background = (draw_background)Marshal::GetFunctionPointerForDelegate(_drawBgDel).ToPointer();
	_container_binding->set_cursor = (set_cursor)Marshal::GetFunctionPointerForDelegate(_setCursorDel).ToPointer();
	_container_binding->draw_borders = (draw_borders)Marshal::GetFunctionPointerForDelegate(_drawBordersDel).ToPointer();
	_container_binding->load_image = (load_image)Marshal::GetFunctionPointerForDelegate(_loadImageDel).ToPointer();
	_container_binding->draw_list_marker = (draw_list_marker)Marshal::GetFunctionPointerForDelegate(_drawListMarkerDel).ToPointer();
	_container_binding->get_defaults = (get_defaults)Marshal::GetFunctionPointerForDelegate(_getDefaultsDel).ToPointer();
	_container_binding->on_anchor_click = (on_anchor_click)Marshal::GetFunctionPointerForDelegate(_onAnchorClick).ToPointer();
}

void HtmlControl::CleanupImages() {
	for each (auto src in _instanceBitmaps)
	{
		auto img = _bitmaps->GetImage(src, false);
		if (img != nullptr) {
			img->Release();
		}
	}
	_bitmaps->Clean();
	_instanceBitmaps->Clear();
}




ScrollViewer^ FindSurroundingScrollViewer(HtmlControl^ ctrl) {
	auto items = gcnew List<FrameworkElement^>();
	auto finder = gcnew TypeFinder<ScrollViewer^>();
	finder->StopAfterFirst = true;
	TreeHelpers::FindUpInTree(items, ctrl, nullptr, finder);
	if (items->Count == 1) {
		return safe_cast<ScrollViewer^>(items->default[0]);
	}
	return nullptr;
}


void HtmlControl::OnScrollViewerSizeChanged(Object^ sender, System::Windows::SizeChangedEventArgs^ e)
{
	// needed!
	InvalidateMeasure();
}

void HtmlControl::EnsureScrollViewerHookedUp() {
	if (_scrollViewerHost == nullptr) {
		_scrollViewerHost = FindSurroundingScrollViewer(this);
		if (_scrollViewerHost != nullptr) {
			_scrollViewerHost->SizeChanged += gcnew SizeChangedEventHandler(this, &HtmlControl::OnScrollViewerSizeChanged);
		}
	}
}


void HtmlControl::OnUnloaded(Object^ sender, RoutedEventArgs^ e)
{
	if (_scrollViewerHost != nullptr) {
		_scrollViewerHost->SizeChanged -= gcnew SizeChangedEventHandler(this, &HtmlControl::OnScrollViewerSizeChanged);
		_scrollViewerHost = nullptr;
	}
}


Size GetScrollViewerScrollInfoViewportSize(ScrollViewer^ scrollViewer) {
	try {
		// we need to use reflection, because official properties are not set when we need them :/
		auto svtype = scrollViewer->GetType();
		auto scinfo = safe_cast<IScrollInfo^>(svtype->GetField("_scrollInfo", BindingFlags::Instance | BindingFlags::NonPublic)->GetValue(scrollViewer));
		if (scinfo == nullptr) {
			return Size::Empty;
		}
		
		return Size(scinfo->ViewportWidth, scinfo->ViewportHeight);
	}
	catch (Exception^) {
		return Size(scrollViewer->ViewportWidth, scrollViewer->ViewportHeight);
	}
}


Size HtmlControl::MeasureOverride(Size availableSize)
{
	/*
	this code snipped could be used to get ideal width - but it works for our purposes i think

	int best_width = m_doc->render(max_width);
	if(best_width < max_width)
	{
		m_doc->render(best_width);
	}
	*/
	
	if (_document.get() != nullptr && _document->root()) {
		if (!double::IsPositiveInfinity(availableSize.Width)) {
			// fixed viewport with .. go for that, this is quite safe and works well
			_measureMode = MeasureMode::FixedWidth;
			_clientSize = availableSize;
			_document->render(availableSize.Width);
			return Size(_document->width(), _document->height());
		}
		else {
			// this gets a tad more complicated, we need to get the minimum width we can render to
			// problem are 100% elements - we have to have some ScrollViewer around us, otherwise we cannot measure reliably
			EnsureScrollViewerHookedUp();
			if (_scrollViewerHost == nullptr) {
				System::Diagnostics::Debug::Assert(false, "HtmlControl cannot measure infinite width without surrounding scrollviewer!");
				_measureMode = MeasureMode::Undefined;
				return Size::Empty;
			}
			
			if (_scrollViewerHost->ViewportWidth == 0.0) {
				System::Diagnostics::Debug::WriteLine("HtmlControl is embedded in ScrollViewer that is not fully loaded yet. This will result in wrong rendering!");
			}

			_measureMode = MeasureMode::InfiniteWidth;
			auto vpsize = GetScrollViewerScrollInfoViewportSize(_scrollViewerHost);
			_wasVScrollbarVisibleDuringMeasure = _scrollViewerHost->ComputedVerticalScrollBarVisibility == System::Windows::Visibility::Visible;
			_clientSize = vpsize;
			_document->render(vpsize.Width);
			return Size(_document->width(), _document->height());
		}
	}
	return Size::Empty;
}

Size HtmlControl::ArrangeOverride(Size availableSize)
{
	switch (_measureMode) {
	case MeasureMode::FixedWidth:
	case MeasureMode::Undefined:
		// easy peasy
		return availableSize;
	case MeasureMode::InfiniteWidth:
		if (_document.get() != nullptr && _document->root()) {
			bool sbvisible = _scrollViewerHost->ComputedVerticalScrollBarVisibility == System::Windows::Visibility::Visible;
			if (sbvisible != _wasVScrollbarVisibleDuringMeasure) {
				// a vertical scrollbar came into view or disappeared -> remeasure
				auto size = GetScrollViewerScrollInfoViewportSize(_scrollViewerHost);
				// size with/without scrollbar is already available in IScrollInfo, but not in ViewportWidth/Height Props :/
				if (size.Width < availableSize.Width) {		
					// measured size from first measure path is not valid anymore
					InvalidateMeasure();
				}
			}
		}
		break;
	}	
	return availableSize;
}

void HtmlControl::OnRender(System::Windows::Media::DrawingContext^ drawingContext)
{
	drawingContext->DrawRectangle(GetBackground(this), nullptr, Rect(0, 0, RenderSize.Width, RenderSize.Height));
	if (_document.get() != nullptr && _document->root()) {
		_clientSize = Size(RenderSize.Width, RenderSize.Height);
		_document->render(RenderSize.Width); // quite slow but we need it :/
		_drawingContext = drawingContext;
		_document->draw(0, 0, 0, new position(0, 0, _clientSize.Width, _clientSize.Height));
		_drawingContext = nullptr;
	}
}

void HtmlControl::OnFontChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs e)
{
	// not the most efficient implementation - but just rerendering doesn't do it (default font size isn't updated)
	((HtmlControl^)d)->OnHtmlChanged(GetHtml(((HtmlControl^)d)));	
}

void HtmlControl::OnHtmlChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs e)
{
	((HtmlControl^)d)->OnHtmlChanged(dynamic_cast<String^>(e.NewValue));
}

void HtmlControl::OnForegroundChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs e) {
	// not the most efficient implementation - but just rerendering doesn't do it (default foreground isn't updated)
	((HtmlControl^)d)->OnHtmlChanged(GetHtml(((HtmlControl^)d)));
}

void HtmlControl::OnHtmlChanged(String^ html) 
{
	this->LoadHtml(html, _usercss);
}

void HtmlControl::LoadHtml(String^ html, String^ usercss) {	
	CleanupImages();

	if (html) {
		pin_ptr<const wchar_t> phtml = PtrToStringChars(html);
		String^ loadusercss = usercss;

		if (Foreground != nullptr) {
			SolidColorBrush^ fb = dynamic_cast<SolidColorBrush^>(Foreground);
			if (fb) {				
				String^ additional = "html { color:rgba(" + fb->Color.R + "," + fb->Color.G + "," + fb->Color.B + "," + (fb->Color.A / 255.0) + "); }";
				if (loadusercss) {
					loadusercss += additional;
				}
				else {
					loadusercss = additional;
				}
			}
		}

		if (loadusercss == nullptr) {
			loadusercss = L"";
		}

		// this is a bit hacky but works - I couldn't find a way to parse css without creating a fake
		// document (parse_stylesheet REQUIRES a document, but document takes usercss?!)
		pin_ptr<const wchar_t> pusercss = PtrToStringChars(loadusercss);
		css cssuser;
		this->CreateContainerBinding();
		_wpf_container = new wpf_container(_container_binding);
		std::shared_ptr<document> tempdoc = document::createFromString(L"", _wpf_container, _context);
		std::shared_ptr<media_query_list> mlist;
		cssuser.parse_stylesheet(pusercss, L"", tempdoc, mlist);
		this->CreateContainerBinding();
		_wpf_container = new wpf_container(_container_binding);
		_document = document::createFromString(phtml, _wpf_container, _context, &cssuser);
		_usercss = usercss;

	}
	else {
		_document = nullptr;
	}
	InvalidateVisual();
}

void HtmlControl::OnMouseMove(System::Windows::Input::MouseEventArgs^ e)
{
	Point p = e->GetPosition(this);
	std::vector<position> redraw;
	_document->on_mouse_over(p.X, p.Y, p.X, p.Y, redraw);
	
	if (redraw.size() > 0) {
		InvalidateVisual();
	}
}
void HtmlControl::OnMouseLeave(System::Windows::Input::MouseEventArgs^ e)
{
	std::vector<position> redraw;
	_document->on_mouse_leave(redraw);
	if (redraw.size() > 0) {
		InvalidateVisual();
	}
}
void HtmlControl::OnMouseLeftButtonDown(System::Windows::Input::MouseButtonEventArgs^ e)
{
	Point p = e->GetPosition(this);
	std::vector<position> redraw;
	_document->on_lbutton_down(p.X, p.Y, p.X, p.Y, redraw);
}
void HtmlControl::OnMouseLeftButtonUp(System::Windows::Input::MouseButtonEventArgs^ e)
{
	Point p = e->GetPosition(this);
	std::vector<position> redraw;
	_document->on_lbutton_up(p.X, p.Y, p.X, p.Y, redraw);
}



Point clampPositive(Point v) {
	if (v.X < 0) v.X = 0;
	if (v.Y < 0) v.Y = 0;
	return v;
}

StreamGeometry^ CreateRect(const litehtml::borders & borders, const litehtml::position & draw_pos) {
	auto radii = Radii(CornerRadius(borders.radius.top_left_x, borders.radius.top_right_x, borders.radius.bottom_right_x, borders.radius.bottom_left_x),
		Thickness(borders.left.width, borders.top.width, borders.right.width, borders.bottom.width), false);

	Rect rect = Rect(draw_pos.left(), draw_pos.top(), draw_pos.width, draw_pos.height);
	Point point = Point(radii.LeftTop, 0.0);
	Point point2 = Point(rect.Width - radii.RightTop, 0.0);
	Point point3 = Point(rect.Width, radii.TopRight);
	Point point4 = Point(rect.Width, rect.Height - radii.BottomRight);
	Point point5 = Point(rect.Width - radii.RightBottom, rect.Height);
	Point point6 = Point(radii.LeftBottom, rect.Height);
	Point point7 = Point(0.0, rect.Height - radii.BottomLeft);
	Point point8 = Point(0.0, radii.TopLeft);
	if (point.X > point2.X)
	{
		double x = radii.LeftTop / (radii.LeftTop + radii.RightTop) * rect.Width;
		point.X = x;
		point2.X = x;
	}
	if (point3.Y > point4.Y)
	{
		double y = radii.TopRight / (radii.TopRight + radii.BottomRight) * rect.Height;
		point3.Y = y;
		point4.Y = y;
	}
	if (point5.X < point6.X)
	{
		double x2 = radii.LeftBottom / (radii.LeftBottom + radii.RightBottom) * rect.Width;
		point5.X = x2;
		point6.X = x2;
	}
	if (point7.Y < point8.Y)
	{
		double y2 = radii.TopLeft / (radii.TopLeft + radii.BottomLeft) * rect.Height;
		point7.Y = y2;
		point8.Y = y2;
	}
	Vector vector = Vector(rect.TopLeft.X, rect.TopLeft.Y);
	point += vector;
	point2 += vector;
	point3 += vector;
	point4 += vector;
	point5 += vector;
	point6 += vector;
	point7 += vector;
	point8 += vector;

	// align to pixel raster

	if (borders.left.width >= 1) {
		auto adjfactor = (float)(borders.left.width)/2;
		auto adjustx = Vector(adjfactor, 0);
		auto adjusty = Vector(0, adjfactor);
		auto adjust = Vector(adjustx.X, adjusty.Y);
		point = clampPositive(point + adjust);
		point2 = clampPositive(point2 - adjustx + adjusty);
		point3 = clampPositive(point3 - adjustx + adjusty);
		point4 = clampPositive(point4 - adjust);
		point5 = clampPositive(point5 - adjust);
		point6 = clampPositive(point6 + adjustx - adjusty);
		point7 = clampPositive(point7 + adjustx - adjusty);
		point8 = clampPositive(point8 + adjust);
	}

	StreamGeometry^ g = gcnew StreamGeometry();
	StreamGeometryContext^ ctx = g->Open();

	ctx->BeginFigure(point, true, true);
	ctx->LineTo(point2, true, false);
	double num = rect.TopRight.X - point2.X;
	double num2 = point3.Y - rect.TopRight.Y;
	double halfstroke = (float)(borders.left.width) / 2;
	
	if (abs(num) > halfstroke || abs(num2) > halfstroke)
	{
		ctx->ArcTo(point3, Size(num, num2), 0.0, false, SweepDirection::Clockwise, true, false);
	}
	ctx->LineTo(point4, true, false);
	num = rect.BottomRight.X - point5.X;
	num2 = rect.BottomRight.Y - point4.Y;
	if (abs(num) > halfstroke || abs(num2) > halfstroke)
	{
		ctx->ArcTo(point5, Size(num, num2), 0.0, false, SweepDirection::Clockwise, true, false);
	}
	ctx->LineTo(point6, true, false);
	num = point6.X - rect.BottomLeft.X;
	num2 = rect.BottomLeft.Y - point7.Y;
	if (abs(num) > halfstroke || abs(num2) > halfstroke)
	{
		ctx->ArcTo(point7, Size(num, num2), 0.0, false, SweepDirection::Clockwise, true, false);
	}
	ctx->LineTo(point8, true, false);
	num = point.X - rect.TopLeft.X;
	num2 = point8.Y - rect.TopLeft.Y;
	if (abs(num) > halfstroke || abs(num2) > halfstroke)
	{
		ctx->ArcTo(point, Size(num, num2), 0.0, false, SweepDirection::Clockwise, true, false);
	}

	delete ctx;
	g->Freeze();
	return g;
}

void HtmlControl::DrawBorders(const litehtml::borders & borders, const litehtml::position & draw_pos, bool root)
{
	if (!_drawingContext) {
		return;
	}
	// we do not support multiple colors/thicknesses on borders
	auto pen = gcnew Pen(gcnew SolidColorBrush(Color::FromArgb(borders.right.color.alpha, borders.right.color.red, borders.right.color.green, borders.right.color.blue)),
		(double)borders.right.width);
	_drawingContext->DrawGeometry(nullptr, pen, CreateRect(borders, draw_pos));
}


void HtmlControl::DrawBackground(const background_paint&  bg)
{
	if (!_drawingContext) {
		return;
	}

	if (bg.image != wstring(L"")) {
		// draw image
		auto img = _bitmaps->GetImage(gcnew String(bg.image.c_str()));		
		if (img != nullptr) {
			_drawingContext->DrawImage(img->Image, Rect(bg.position_x, bg.position_y, bg.image_size.width, bg.image_size.height));
			img->Release();
		}
	}
	else {

		// we do not support multiple colors/thicknesses on borders or styles .. keep it simple, but this can be expanded if necessary
		Brush^ color = gcnew SolidColorBrush(Color::FromArgb(bg.color.alpha, bg.color.red, bg.color.green, bg.color.blue));
		borders b;
		b.radius = bg.border_radius;

		_drawingContext->DrawGeometry(color, nullptr,
			CreateRect(b, position(bg.border_box.x, bg.border_box.y, bg.border_box.width, bg.border_box.height)));
	}
}

void HtmlControl::FillFontMetrics(font_desc* hFont, font_metrics * fm)
{
	if (fm)
	{
		System::Drawing::FontStyle fs = System::Drawing::FontStyle::Regular;
		if (hFont->italic) {
			fs = fs | System::Drawing::FontStyle::Italic;
		}
		if (hFont->weight>700) {
			fs = fs | System::Drawing::FontStyle::Bold;
		}
		System::Drawing::Font^ font = gcnew System::Drawing::Font(gcnew System::String(hFont->faceName), hFont->size, fs, System::Drawing::GraphicsUnit::Pixel);

		float em_height =
			font->FontFamily->GetEmHeight(font->Style);
		
		float ascent = font->FontFamily->GetCellAscent(font->Style);
		fm->ascent = font->Size * ascent / font->FontFamily->GetEmHeight(font->Style);

		float descent = font->FontFamily->GetCellDescent(font->Style);

		fm->descent =
			font->Size * descent / font->FontFamily->GetEmHeight(font->Style);

		fm->height = fm->ascent + fm->descent;
		fm->x_height = font->Height;
				
		if (hFont->italic == litehtml::fontStyleItalic || hFont->decoration)
		{
			fm->draw_spaces = true;
		}
		else
		{
			fm->draw_spaces = false;
		}
	}
}

String^ GetFontFacename(const tchar_t * faceName) {
	std::wstring fnt_name = L"sans-serif";

	litehtml::string_vector fonts;
	litehtml::split_string(faceName, fonts, _t(","));
	if (!fonts.empty())
	{
		litehtml::trim(fonts[0]);

		fnt_name = fonts[0];
		if (fnt_name.front() == '"')
		{
			fnt_name.erase(0, 1);
		}
		if (fnt_name.back() == '"')
		{
			fnt_name.erase(fnt_name.length() - 1, 1);
		}
	}

	LOGFONT lf;
	ZeroMemory(&lf, sizeof(lf));
	if (!lstrcmpi(fnt_name.c_str(), L"monospace"))
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Courier New");
	}
	else if (!lstrcmpi(fnt_name.c_str(), L"serif"))
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Times New Roman");
	}
	else if (!lstrcmpi(fnt_name.c_str(), L"sans-serif"))
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Arial");
	}
	else if (!lstrcmpi(fnt_name.c_str(), L"fantasy"))
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Impact");
	}
	else if (!lstrcmpi(fnt_name.c_str(), L"cursive"))
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Comic Sans MS");
	}
	else
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, fnt_name.c_str());
	}

	return gcnew String(lf.lfFaceName);
}

FormattedText^ CreateFormattedText(String^ text, font_desc* hFont, Brush^ brush, FlowDirection flow = FlowDirection::LeftToRight) {
	FormattedText^ ft = gcnew FormattedText(gcnew System::String(text),
		CultureInfo::CurrentUICulture,
		flow,
		gcnew Typeface(GetFontFacename(hFont->faceName)),
		hFont->size, brush);
	ft->SetFontWeight(System::Windows::FontWeight::FromOpenTypeWeight(hFont->weight));
	if (hFont->italic) {
		ft->SetFontStyle(System::Windows::FontStyles::Italic);
	}

	auto decorations = gcnew TextDecorationCollection();
	if (hFont->decoration & litehtml::font_decoration_underline) {
		decorations->Add(TextDecorations::Underline);
	}
	if (hFont->decoration & litehtml::font_decoration_linethrough) {
		decorations->Add(TextDecorations::Strikethrough);
	}
	if (hFont->decoration & litehtml::font_decoration_overline) {
		decorations->Add(TextDecorations::OverLine);
	}

	ft->SetTextDecorations(decorations);
	return ft;
}


void HtmlControl::DrawText(litehtml::uint_ptr hdc, const litehtml::tchar_t * text, font_desc* hFont, litehtml::web_color color, const litehtml::position & pos)
{
	if (!_drawingContext) {
		return;
	}
	
	if (wcscmp(text, L" ")==0) {
		// fix underline of spaces (WPF doesn't draw underline otherwise)
		text = L"\u00A0";
	}

	auto brush = gcnew SolidColorBrush(Color::FromArgb(color.alpha, color.red, color.green, color.blue));
	FormattedText^ ft = CreateFormattedText(gcnew String(text), hFont, brush);
	_drawingContext->DrawText(ft, Point(pos.left(), pos.top()));

}


int HtmlControl::GetTextWidth(const litehtml::tchar_t * text, font_desc* hFont)
{
	auto ft = CreateFormattedText(gcnew String(text), hFont, Brushes::Black);
	return ft->WidthIncludingTrailingWhitespace;
}

position HtmlControl::GetClientRect() {
	position r;
	r.x = 0;
	r.y = 0;
	r.width = _clientSize.Width;
	r.height = _clientSize.Height;
	return r;
}


void HtmlControl::SetCursor(const litehtml::tchar_t * cursor) 
{
	System::String^ sc = gcnew System::String(cursor);
	if (sc == "pointer") {
		Cursor = System::Windows::Input::Cursors::Hand;
	} else {
		Cursor = System::Windows::Input::Cursors::Arrow;
	}
}

image* ImageFromBitmap(BitmapImage^ bmp) {
	image* img = new image();
	img->size.width = bmp->PixelWidth;
	img->size.height = bmp->PixelHeight;
	return img;
}

Stream^ HtmlControl::LoadResourceImpl(String^ src) {	
	return LoadResource(src);
	/*
	if (System::IO::File::Exists(src)) {
		return System::IO::File::OpenRead(src);
	}
	return nullptr;*/
}


image* HtmlControl::LoadHtmlImage(const litehtml::tchar_t * src, const litehtml::tchar_t * baseurl)
{	
	
	String^ msrc = gcnew String(src);
	Bitmap^ bmp = _bitmaps->GetOrCreateImage(msrc, gcnew LoadImageDataDelegate(this, &HtmlControl::LoadResourceImpl));
	if (bmp != nullptr) {
		if (!_instanceBitmaps->Contains(msrc)) {
			_instanceBitmaps->Add(msrc);
		}
		else {
			// we know it already
			bmp->Release();
		}
		// refcount is incremented, no matter what
		return ImageFromBitmap(bmp->Image);
	}

	return nullptr;
}


const int ColumnBase = 26;
const int DigitMax = 7; // ceil(log26(Int32.Max))
String^ IndexToAlpha(int index, String^ digits)
{
	if (index <= 0)
		return "";

	if (index <= ColumnBase)
		return digits[index - 1].ToString();

	auto sb = gcnew System::Text::StringBuilder();
	sb->Append(' ', DigitMax);
	auto current = index;
	auto offset = DigitMax;
	while (current > 0)
	{
		sb[--offset] = digits[--current % ColumnBase];
		current /= ColumnBase;
	}
	return sb->ToString(offset, DigitMax - offset);
}


void HtmlControl::DrawListMarker(const litehtml::list_marker_ex & marker, font_desc* hFont) {
	if (!_drawingContext) {
		return;
	}
	if (!marker.image.empty())
	{
		auto img = _bitmaps->GetImage(gcnew String(marker.image.c_str()));
		if (img != nullptr) {
			_drawingContext->DrawImage(img->Image, Rect(marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height));
			img->Release();
		}
	}
	else
	{
		auto brush = gcnew SolidColorBrush(Color::FromArgb(marker.color.alpha, marker.color.red, marker.color.green, marker.color.blue));

		switch (marker.marker_type)
		{
		case litehtml::list_style_type_circle:
		{
			_drawingContext->DrawEllipse(nullptr, gcnew Pen(brush, 0.5), Point(marker.pos.x + marker.pos.width / 2, marker.pos.y + marker.pos.height / 2),
				marker.pos.width / 2, marker.pos.height / 2);
		}
		break;
		case litehtml::list_style_type_disc:
		{
			_drawingContext->DrawEllipse(brush, nullptr, Point(marker.pos.x + marker.pos.width / 2, marker.pos.y + marker.pos.height / 2),
				marker.pos.width / 2, marker.pos.height / 2);
		}
		break;
		case litehtml::list_style_type_square:
		{
			_drawingContext->DrawRectangle(brush, nullptr, Rect(marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height));
			break;
		}
		case litehtml::list_style_type_decimal:		
		case litehtml::list_style_type_lower_alpha:
		case litehtml::list_style_type_upper_alpha:
		{
			String^ text;
			switch (marker.marker_type) {
			case litehtml::list_style_type_decimal:
				text = "." + marker.index_in_list.ToString();
				break;
			case litehtml::list_style_type_lower_alpha:
			case litehtml::list_style_type_lower_latin:
				text = "." + IndexToAlpha(marker.index_in_list, "abcdefghijklmnopqrstuvwxyz");
				break;
			case litehtml::list_style_type_upper_alpha:
			case litehtml::list_style_type_upper_latin:
				text = "." + IndexToAlpha(marker.index_in_list, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
				break;
			}
			
			auto ft = CreateFormattedText(text, hFont, brush, System::Windows::FlowDirection::RightToLeft);
			_drawingContext->DrawText(ft, Point(marker.pos.x, marker.pos.y));


			break;
		}
		}
	}
}

void HtmlControl::GetDefaults(defaults & defaults)
{
	//pin_ptr<const wchar_t> culture = PtrToStringChars(Thread::CurrentThread->CurrentUICulture->Name);
	pin_ptr<const wchar_t> language = PtrToStringChars(Thread::CurrentThread->CurrentUICulture->TwoLetterISOLanguageName);
	defaults.culture = _t("");
	defaults.language = language;
	defaults.font_size = GetFontSize(this);
	pin_ptr<const wchar_t> font = PtrToStringChars(GetFontFamily(this)->ToString());
	defaults.font_face_name = font;
}

void HtmlControl::OnAnchorClick(const litehtml::tchar_t * url)
{
	System::String^ surl = gcnew System::String(url);
	AnchorClicked(surl);
}
