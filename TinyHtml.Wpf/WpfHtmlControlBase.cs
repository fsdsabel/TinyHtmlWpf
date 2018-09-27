using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;
using LiteHtml;

namespace TinyHtml.Wpf
{
    /// <summary>
    /// Base Control for rendering HTML.
    /// </summary>
    public class WpfHtmlControlBase : Control
    {
        private readonly HtmlControl _htmlControl;

        /// <inheritdoc />
        public WpfHtmlControlBase()
        {
            _htmlControl = new HtmlControl();
            _htmlControl.LoadResource += OnLoadResource;
            _htmlControl.AnchorClicked += OnAnchorClicked;
            _htmlControl.SetBinding(HtmlControl.HtmlProperty, new Binding(nameof(Html)) {Source = this});
            _htmlControl.SetBinding(HtmlControl.BackgroundProperty, new Binding(nameof(Background)) { Source = this });
            _htmlControl.SetBinding(HtmlControl.ForegroundProperty, new Binding(nameof(Foreground)) { Source = this });
            _htmlControl.SetBinding(HtmlControl.FontFamilyProperty, new Binding(nameof(FontFamily)) {Source = this});
            _htmlControl.SetBinding(HtmlControl.FontSizeProperty, new Binding(nameof(FontSize)) { Source = this });
            AddVisualChild(_htmlControl);
        }
        
        #region Overrides of FrameworkElement

        /// <inheritdoc />
        protected override int VisualChildrenCount => 1;


        /// <inheritdoc />
        protected override Visual GetVisualChild(int index)
        {
            if (index == 0)
            {
                return _htmlControl;
            }
            throw new ArgumentOutOfRangeException(nameof(index));
        }
        
        #endregion


        /// <inheritdoc />
        protected override Size MeasureOverride(Size constraint)
        {
            _htmlControl.Measure(constraint);
            return _htmlControl.DesiredSize;
        }

        /// <inheritdoc />
        protected override Size ArrangeOverride(Size arrangeBounds)
        {
            _htmlControl.Arrange(new Rect(arrangeBounds));
            return arrangeBounds;
        }

        
        /// <summary>
        /// Sets the master style sheet that is used to define default HTML rendering.
        /// A default css file comes with the package.
        /// </summary>
        /// <param name="css">CSS to use</param>
        public static void SetMasterStylesheet(string css)
        {
            HtmlControl.SetMasterStylesheet(css);
        }

        /// <summary>
        /// HTML content to use for rendering.
        /// </summary>
        public string Html
        {
            get { return (string)GetValue(HtmlProperty); }
            set { SetValue(HtmlProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Html.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty HtmlProperty =
            DependencyProperty.Register("Html", typeof(string), typeof(WpfHtmlControlBase), new PropertyMetadata(null));


        /// <summary>
        /// Override this to implement resource loading. Currently used for images.
        /// </summary>
        /// <param name="url">Url like it is defined in HTML.</param>
        /// <returns>Stream that contains the resource. The control will dispose this, if it is no longer needed.</returns>
        protected virtual Stream OnLoadResource(string url)
        {
            return null;
        }

        /// <summary>
        /// This method is called when the user clicks on a link.
        /// </summary>
        /// <param name="url">The url behind the anchor.</param>
        protected virtual void OnAnchorClicked(string url) { }

        /// <summary>
        /// Loads the given HTML and CSS content.
        /// </summary>
        /// <param name="html">HTML to display.</param>
        /// <param name="usercss">CSS to use.</param>
        public void LoadHtml(string html, string usercss)
        {
            _htmlControl.LoadHtml(html, usercss);
        }



        /// <summary>
        /// Font size to use as default font size when rendering HTML.
        /// </summary>
        public new double FontSizeProperty
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
