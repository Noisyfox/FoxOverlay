using System;
using System.Collections.Generic;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Web.Script.Serialization;

namespace Service
{
    /// <summary>
    /// Class holds the current global configurations and exposed
    /// with named shared memory.
    /// </summary>
    class GlobalConfiguration : IDisposable
    {
        // Make sure to be consistent with constants in GlobalConfiguration.h file
        private const string SHARED_MEM_NAME = "Global\\FoxOverlay_Configuration";
        private const string SHARED_MEM_MUTEX_NAME = "Global\\FoxOverlay_Configuration_Mutex";

        /// <summary>
        /// Current configuration version.
        /// Will be the first 64 bits in the shared memory.
        /// Client will only parse the configuration data if the version matches.
        /// </summary>
        public const ulong Version = 1;

        /// <summary>
        /// TCP port that accept incoming requests from clients & overlays.
        /// </summary>
        public readonly uint IpcPort;

        private MemoryMappedFile _sharedMemory;

        private GlobalConfiguration(Builder builder)
        {
            IpcPort = builder.IpcPort;
        }

        public void Activate()
        {
            // Create the configurations DTO instance
            var cfgData = new ConfigurationData
            {
                IpcPort = IpcPort
            };

            // Serialize the DTO to JSON string
            var serializer = new JavaScriptSerializer();
            var serializedResult = serializer.Serialize(cfgData);

            // Convert JSON string to UTF-8 byte array
            var cfgBytes = Encoding.UTF8.GetBytes(serializedResult);

            // 8 bytes version, 8 bytes content length, then with cfgBytes
            var sharedMemSize = 8L + 8L + cfgBytes.Length;

            // Acquire the exclusive ownership of the shared memory
            using (var mutex = new Mutex(true, SHARED_MEM_MUTEX_NAME))
            {
                // Create the shared memory
                var file = MemoryMappedFile.CreateNew(SHARED_MEM_NAME, sharedMemSize);
                _sharedMemory = file;

                // Write config
                using (var stream = file.CreateViewStream())
                {
                    // Write data in little-endian
                    var writer = new BinaryWriter(stream);
                    writer.Write(Version);
                    writer.Write((ulong) cfgBytes.Length);
                    writer.Write(cfgBytes);
                }
            }
        }

        public void Dispose()
        {
            // Acquire the exclusive ownership of the shared memory
            using (var mutex = new Mutex(true, SHARED_MEM_MUTEX_NAME))
            {
                _sharedMemory?.Dispose();
                _sharedMemory = null;
            }
        }

        private class ConfigurationData
        {
            public uint IpcPort { get; set; }
        }

        public class Builder
        {
            internal uint IpcPort = 31415;

            public GlobalConfiguration Build()
            {
                return new GlobalConfiguration(this);
            }

            public Builder WithIpcPort(uint port)
            {
                IpcPort = port;
                return this;
            }
        }
    }
}