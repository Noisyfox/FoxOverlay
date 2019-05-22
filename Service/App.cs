using System;
using System.Threading;
using System.Windows;

namespace Service
{
    internal class App : Application
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
        public static int Main(string[] args)
        {
            using (var mutex = new Mutex(false, "Global\\FoxOverlay_Service_Mutex"))
            {
                // Don't allow running multiple service instance
                if (!mutex.WaitOne(0, false))
                {
                    return -1;
                }

                try
                {
                    using (var cfg = new GlobalConfiguration.Builder().Build())
                    {
                        cfg.Activate();

                        var app = new App();
                        app.InitializeComponent();
                        app.Run();
                    }
                }
                finally
                {
                    mutex.ReleaseMutex();
                }
            }

            return 0;
        }
    }
}