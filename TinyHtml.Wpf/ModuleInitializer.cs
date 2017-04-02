using System;
using System.IO;
using System.Reflection;

/// <summary>
/// Used by the ModuleInit. All code inside the Initialize method is ran as soon as the assembly is loaded.
/// </summary>
internal static class ModuleInitializer
{
    /// <summary>
    /// Initializes the module.
    /// </summary>
    public static void Initialize()
    {
        AppDomain.CurrentDomain.AssemblyResolve += OnAssemblyResolve;
    }

    static Assembly OnAssemblyResolve(object sender, ResolveEventArgs args)
    {
        var asmname = new AssemblyName(args.Name);
        if (string.Equals(asmname.Name, "TinyWpfHtml", StringComparison.OrdinalIgnoreCase))
        {
            string subdir = "x64";
            if (IntPtr.Size == sizeof(int))
            {
                subdir = "x86";
            }
            // ReSharper disable once AssignNullToNotNullAttribute
            return
                Assembly.LoadFrom(Path.Combine(Path.GetDirectoryName(typeof(ModuleInitializer).Assembly.Location),
                    subdir, "TinyWpfHtml.dll"));
        }
        return null;
    }
}

