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
                return typeof(WpfHtmlControl).Assembly.GetManifestResourceStream(typeof(WpfHtmlControl), url);
            }
            catch
            {
                return null;
            }
        }
    }
}
