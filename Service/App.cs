using System;
using System.Windows;

namespace Service
{
    class App: Application
    {
        /// <summary>
        /// InitializeComponent
        /// </summary>
        private void InitializeComponent()
        {
            StartupUri = new Uri("MainWindow.xaml", System.UriKind.Relative);
        }

        /// <summary>
        /// Application Entry Point.
        /// </summary>
        [STAThread]
        public static void Main(string[] args)
        {
            var app = new App();
            app.InitializeComponent();
            app.Run();
        }
    }
}
