using System.Diagnostics;
using System.IO;
using TinyHtml.Wpf;

namespace HtmlTestApp
{
    public class EmbeddedWpfHtmlControl : WpfHtmlControl
    {
        protected override Stream OnLoadResource(string url)
        {
            try
            {
                return typeof(EmbeddedWpfHtmlControl).Assembly.GetManifestResourceStream(typeof(EmbeddedWpfHtmlControl), url);
            }
            catch
            {
                return null;
            }
        }

        protected override void OnAnchorClicked(string url)
        {
            Process.Start(url);
        }
    }
}
