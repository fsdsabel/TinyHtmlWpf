using System.Windows;

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
                "<p>This is an example for an image:</p><image src=\"Semperoper.jpg\" style=\"width:100%\" />";

        public string Html2
            =>
                "<style>table { margin: 10px auto; border:2px solid red; border-collapse:collapse; } th { text-align:center;font-size:15pt }</style>" +
                "<table><th>Column 1</th><th>Column 2</th><tr><td>Some content with some text.</td><td>Some other content<br/>with more text.</td></tr></table>";
    }
}
