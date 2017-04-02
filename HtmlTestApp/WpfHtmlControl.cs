using System.Diagnostics;
using System.IO;
using TinyHtml.Wpf;

namespace HtmlTestApp
{
    public class WpfHtmlControl : WpfHtmlControlBase
    {
        static WpfHtmlControl()
        {
            using (var s = typeof (WpfHtmlControl).Assembly.GetManifestResourceStream(typeof (WpfHtmlControl), "master.css"))
            {
                Debug.Assert(s != null, "s != null");
                SetMasterStylesheet(new StreamReader(s).ReadToEnd());
            }
        }
    }
}
