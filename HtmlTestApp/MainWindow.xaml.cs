using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace HtmlTestApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;
        }
        
        public string Html1
            =>
                "<p style=\"white-space:pre;\"><a href=\"#\">sddj</a><span style=\"background-color:yellow\">Das     </span><b>ist</b> Test-HTML</p><p>U can use <span style=\"color:red\">css</span> too.</p><p>A longer line is wrapping around if you want to. Most things should work as expected from HTML.</p>" +
                "<ul><li>jsdidjsi</li><li>jidijsdi</li></ul>";

        public string Html2
            =>
                "<p>uhsduhsduh sadhuasdusd<br/>sdsdudsuh</p>";
    }
}
