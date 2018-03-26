using System.Diagnostics;
using System.IO;

namespace TinyHtml.Wpf
{
    /// <summary>
    /// Extension of <see cref="WpfHtmlControlBase"/> that loads the default master stylesheet. Usually this is the control
    /// you want to use as your base class.
    /// </summary>
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
