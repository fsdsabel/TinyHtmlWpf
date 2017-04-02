using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using LiteHtml;

namespace TinyHtml.Wpf
{
    /// <summary>
    /// This is just here as a proxy to C++/CLI
    /// </summary>
    public class WpfHtmlControlBase : Control
    {
        private HtmlControl _htmlControl;
        
        public WpfHtmlControlBase()
        {
            _htmlControl = new HtmlControl();
            _htmlControl.LoadResource += OnLoadResource;
            _htmlControl.SetBinding(HtmlControl.HtmlProperty, new Binding(nameof(Html)) {Source = this});
            _htmlControl.SetBinding(HtmlControl.BackgroundProperty, new Binding(nameof(Background)) { Source = this });
            _htmlControl.SetBinding(HtmlControl.ForegroundProperty, new Binding(nameof(Foreground)) { Source = this });
            _htmlControl.SetBinding(HtmlControl.FontFamilyProperty, new Binding(nameof(FontFamily)) {Source = this});
            _htmlControl.SetBinding(HtmlControl.FontSizeProperty, new Binding(nameof(FontSize)) { Source = this });
            AddVisualChild(_htmlControl);
        }
        
        #region Overrides of FrameworkElement


        protected override int VisualChildrenCount => 1;
        

        protected override Visual GetVisualChild(int index)
        {
            if (index == 0)
            {
                return _htmlControl;
            }
            throw new ArgumentOutOfRangeException(nameof(index));
        }
        
        #endregion

        

        protected override Size MeasureOverride(Size constraint)
        {
            _htmlControl.Measure(constraint);
            return _htmlControl.DesiredSize;
        }
        
        protected override Size ArrangeOverride(Size arrangeBounds)
        {
            _htmlControl.Arrange(new Rect(arrangeBounds));
            return arrangeBounds;
        }

        

        public static void SetMasterStylesheet(string css)
        {
            HtmlControl.SetMasterStylesheet(css);
        }


        public string Html
        {
            get { return (string)GetValue(HtmlProperty); }
            set { SetValue(HtmlProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Html.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty HtmlProperty =
            DependencyProperty.Register("Html", typeof(string), typeof(WpfHtmlControlBase), new PropertyMetadata(null));


        protected virtual Stream OnLoadResource(string url)
        {
            return null;
        }

        public void LoadHtml(string html, string usercss)
        {
            _htmlControl.LoadHtml(html, usercss);
        }




        public double FontSizeProperty
        {
            get { return (double)GetValue(FontSizePropertyProperty); }
            set { SetValue(FontSizePropertyProperty, value); }
        }

        // Using a DependencyProperty as the backing store for FontSizeProperty.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty FontSizePropertyProperty =
            DependencyProperty.Register("FontSizeProperty", typeof (double), typeof (WpfHtmlControlBase),
                new FrameworkPropertyMetadata(12.0, FrameworkPropertyMetadataOptions.AffectsRender | FrameworkPropertyMetadataOptions.AffectsMeasure));



    }
}
