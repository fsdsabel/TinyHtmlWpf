#pragma once
#include <Windows.h>
#include "litehtml.h"
#include "wpf_container_binding.h"
#include "m_shared_ptr.h"
#include "wpf_container.h"
#pragma managed
#include <vcclr.h>



using namespace litehtml;
using namespace System;
using namespace System::IO;
using namespace System::Windows;
using namespace System::Windows::Media;
using namespace System::Windows::Controls;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Windows::Media::Imaging;
using namespace System::Threading;

namespace LiteHtml {

	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	delegate int TextWidthDelegate(const litehtml::tchar_t * text, font_desc * hFont);
	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	delegate position GetClientRectDelegate();
	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	delegate void DrawTextDelegate(litehtml::uint_ptr hdc, const litehtml::tchar_t * text, font_desc* hFont, litehtml::web_color color, const litehtml::position & pos);
	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	delegate void FillFontMetricsDelegate(font_desc* hFont, font_metrics * fm);
	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	delegate void DrawBackgroundDelegate(const background_paint&  bg);
	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	delegate void SetCursorDelegate(const litehtml::tchar_t * cursor);
	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	delegate void DrawBordersDelegate(const litehtml::borders & borders, const litehtml::position & draw_pos, bool root);
	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	delegate image* LoadImageDelegate(const litehtml::tchar_t * src, const litehtml::tchar_t * baseurl);
	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	delegate void DrawListMarkerDelegate(const litehtml::list_marker_ex & marker, font_desc* hFont);
	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	delegate void GetDefaultsDelegate(defaults & defaults);

	public delegate Stream^ LoadImageDataDelegate(String^ url);

	public delegate Stream^ LoadResourceDelegate(String^ url);


	ref class Lock {
		Object^ m_pObject;
	public:
		Lock(Object ^ pObject) : m_pObject(pObject) {
			Monitor::Enter(m_pObject);
		}
		~Lock() {
			Monitor::Exit(m_pObject);
		}
	};


	ref class Bitmap {
	private:
		int _refcount;

	public:
		Bitmap(BitmapImage^ img) {
			Image = img;
			_refcount = 1;
		}

		property BitmapImage^ Image;

		void Addref() {
			System::Threading::Interlocked::Increment(_refcount);
		}
		void Release() {
			System::Threading::Interlocked::Decrement(_refcount);
		}

		bool CanRelease() {
			return _refcount <= 0;
		}
	};

	ref class ImageCache {
	private:
		Dictionary<String^, Bitmap^>^ _bitmaps;
	public:
		ImageCache() {
			_bitmaps = gcnew Dictionary<String^, Bitmap^>();
		}

		Bitmap^ GetImage(String^ src) {
			return GetImage(src, true);
		}

		Bitmap^ GetImage(String^ src, bool addref) {
			Lock lock(this);
			Bitmap^ bmp;
			if (_bitmaps->TryGetValue(src, bmp)) {
				if (addref) {
					bmp->Addref();
				}
				return bmp;
			}
			return nullptr;
		}

		Bitmap^ GetOrCreateImage(String^ src, LoadImageDataDelegate^ onload) {
			auto bmp = GetImage(src);
			if (bmp != nullptr) {
				return bmp;
			}

			auto stream = onload(src);
			if (stream == nullptr) {
				return nullptr;
			}

			try {
				auto bmpimg = gcnew BitmapImage();
				bmpimg->BeginInit();
				bmpimg->CacheOption = BitmapCacheOption::OnLoad;
				bmpimg->StreamSource = stream;
				bmpimg->EndInit();

				bmp = gcnew Bitmap(bmpimg);

				_bitmaps->default[src] = bmp;
				return bmp;
			}
			catch (Exception^) {
				return nullptr;
			}
		}

		void Clean() {
			Lock lock(this);
			auto torelease = gcnew List<String^>();
			for each (auto bmp in _bitmaps)
			{
				if (bmp.Value->CanRelease()) {
					torelease->Add(bmp.Key);
				}
			}
			for each (auto src in torelease)
			{
				_bitmaps->Remove(src);
			}
		}
	};


	public ref class HtmlControl : System::Windows::FrameworkElement
	{
	private:
		enum class MeasureMode {
			Undefined,
			FixedWidth,
			InfiniteWidth
		};



		wpf_container_binding* _container_binding;
		TextWidthDelegate^ _textWidthtDel;
		GetClientRectDelegate^ _getClientRectDel;
		DrawTextDelegate^ _drawTextDel;
		FillFontMetricsDelegate^ _fillFontMetricsDel;
		SetCursorDelegate^  _setCursorDel;
		DrawBackgroundDelegate^ _drawBgDel;
		DrawBordersDelegate^ _drawBordersDel;
		LoadImageDelegate^ _loadImageDel;
		DrawListMarkerDelegate^ _drawListMarkerDel;
		GetDefaultsDelegate^ _getDefaultsDel;
		m_shared_ptr<document> _document;
		static context* _context;
		wpf_container* _wpf_container;
		static ImageCache^ _bitmaps;
		List<String^>^ _instanceBitmaps;
		String^ _usercss;

		Size _clientSize;
		System::Windows::Media::DrawingContext^ _drawingContext;
		ScrollViewer^ _scrollViewerHost;
		MeasureMode _measureMode;
		bool _wasVScrollbarVisibleDuringMeasure;

		void CreateContainerBinding();
		
		void OnUnloaded(Object^ sender, RoutedEventArgs^ e);
		void OnScrollViewerSizeChanged(Object^ sender, SizeChangedEventArgs^ e);

		void EnsureScrollViewerHookedUp();
		void CleanupImages();

		int GetTextWidth(const litehtml::tchar_t * text, font_desc* hFont);
		position GetClientRect();
		void DrawText(litehtml::uint_ptr hdc, const litehtml::tchar_t * text, font_desc* hFont, litehtml::web_color color, const litehtml::position & pos);
		void FillFontMetrics(font_desc* hFont, font_metrics * fm);
		void DrawBackground(const background_paint& bg);
		void SetCursor(const litehtml::tchar_t * cursor);
		void DrawBorders(const litehtml::borders & borders, const litehtml::position & draw_pos, bool root);
		image* LoadHtmlImage(const litehtml::tchar_t * src, const litehtml::tchar_t * baseurl);
		void DrawListMarker(const litehtml::list_marker_ex & marker, font_desc* hFont);
		void GetDefaults(defaults & defaults);

		static void OnHtmlChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs e);
		static void OnFontChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs e);
		static void OnForegroundChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs e);
	protected:
		virtual void OnRender(System::Windows::Media::DrawingContext^ drawingContext) override;
		virtual System::Windows::Size MeasureOverride(Size availableSize) override;
		virtual System::Windows::Size ArrangeOverride(Size availableSize) override;
		virtual void OnMouseMove(System::Windows::Input::MouseEventArgs^ e) override;
		virtual void OnMouseLeave(System::Windows::Input::MouseEventArgs^ e) override;
		virtual void OnMouseLeftButtonDown(System::Windows::Input::MouseButtonEventArgs^ e) override;
		virtual void OnMouseLeftButtonUp(System::Windows::Input::MouseButtonEventArgs^ e) override;

		virtual void OnHtmlChanged(String^ html);
		virtual Stream^ LoadResourceImpl(String^ src);
		
		
	public:
		static HtmlControl();
		HtmlControl();
		!HtmlControl();

		event LoadResourceDelegate^ LoadResource;

		/// usercss will be applied after all other css, can be used by subclasses to inject some styles (override LoadHtml and call base LoadHtml with css)
		virtual void LoadHtml(String^ html, String^ usercss);

		// this needs to be set to show some meaningful HTML!!
		static void SetMasterStylesheet(String^ css);
		
		static String^ GetHtml(DependencyObject ^obj)
		{
			return safe_cast<String^>(obj->GetValue(HtmlProperty));
		}
		static void SetHtml(DependencyObject ^obj, String^ value)
		{
			obj->SetValue(HtmlProperty, value);
		}

		property String^ Html {
			String^ get() { return GetHtml(this); }
			void set(String^ value) { SetHtml(this, value); }
		}

		// HTML to load to view 
		static initonly DependencyProperty ^HtmlProperty = DependencyProperty::RegisterAttached("Html", 
			String::typeid, HtmlControl::typeid, gcnew FrameworkPropertyMetadata(nullptr, FrameworkPropertyMetadataOptions::AffectsMeasure | FrameworkPropertyMetadataOptions::AffectsRender, gcnew PropertyChangedCallback(HtmlControl::OnHtmlChanged)));
				
		static FontFamily^ GetFontFamily(DependencyObject ^obj)
		{
			return safe_cast<System::Windows::Media::FontFamily^>(obj->GetValue(FontFamilyProperty));
		}
		static void SetFontFamily(DependencyObject ^obj, FontFamily^ value)
		{
			obj->SetValue(FontFamilyProperty, value);
		}

		property FontFamily^ FontFamily {
			System::Windows::Media::FontFamily^ get() { return GetFontFamily(this); }
			void set(System::Windows::Media::FontFamily^ value) { SetFontFamily(this, value); }
		}

		static initonly DependencyProperty ^FontFamilyProperty = System::Windows::Documents::TextElement::FontFamilyProperty->AddOwner(HtmlControl::typeid, 
			gcnew FrameworkPropertyMetadata(gcnew PropertyChangedCallback(HtmlControl::OnFontChanged)));

		static double GetFontSize(DependencyObject ^obj)
		{
			return safe_cast<double>(obj->GetValue(FontSizeProperty));
		}
		static void SetFontSize(DependencyObject ^obj, double value)
		{
			obj->SetValue(FontSizeProperty, value);
		}

		property double FontSize {
			double get() { return GetFontSize(this); }
			void set(double value) { SetFontSize(this, value); }
		}

		static initonly DependencyProperty ^FontSizeProperty = System::Windows::Documents::TextElement::FontSizeProperty->AddOwner(HtmlControl::typeid, 
			gcnew FrameworkPropertyMetadata(System::Windows::Documents::TextElement::FontSizeProperty->DefaultMetadata->DefaultValue, 
				FrameworkPropertyMetadataOptions::AffectsRender | FrameworkPropertyMetadataOptions::AffectsMeasure,
				gcnew PropertyChangedCallback(HtmlControl::OnFontChanged)));


		static Brush^ GetBackground(DependencyObject ^obj)
		{
			return safe_cast<Brush^>(obj->GetValue(BackgroundProperty));
		}
		static void SetBackground(DependencyObject ^obj, Brush^ value)
		{
			obj->SetValue(BackgroundProperty, value);
		}

		property Brush^ Background {
			Brush^ get() { return GetBackground(this); }
			void set(Brush^ value) { SetBackground(this, value); }
		}

		static initonly DependencyProperty ^BackgroundProperty = System::Windows::Controls::Panel::BackgroundProperty->AddOwner(HtmlControl::typeid, gcnew FrameworkPropertyMetadata(System::Windows::Controls::Panel::BackgroundProperty->DefaultMetadata->DefaultValue, FrameworkPropertyMetadataOptions::AffectsRender));

		static Brush^ GetForeground(DependencyObject ^obj)
		{
			return safe_cast<Brush^>(obj->GetValue(ForegroundProperty));
		}
		static void SetForeground(DependencyObject ^obj, Brush^ value)
		{
			obj->SetValue(ForegroundProperty, value);
		}

		property Brush^ Foreground {
			Brush^ get() { return GetForeground(this); }
			void set(Brush^ value) { SetForeground(this, value); }
		}

		static initonly DependencyProperty ^ForegroundProperty = System::Windows::Documents::TextElement::ForegroundProperty->AddOwner(
			HtmlControl::typeid, gcnew FrameworkPropertyMetadata(System::Windows::Documents::TextElement::ForegroundProperty->DefaultMetadata->DefaultValue, 
				FrameworkPropertyMetadataOptions::AffectsRender, gcnew PropertyChangedCallback(HtmlControl::OnForegroundChanged)));
	};
}