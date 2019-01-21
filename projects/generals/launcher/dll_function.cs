using System;
using System.Runtime.InteropServices;
using System.Text;

namespace lanch
{
    class dll_function
    {
        // adaptres.dll
        [DllImport("adapters.dll")]
        public static extern IntPtr GetListAdapters(ref int size);

        [DllImport("adapters.dll")]
        public static extern IntPtr SetAdapter(int size); //

        [DllImport("adapters.dll"/*, CallingConvention = CallingConvention.StdCall*/)]
        public static extern IntPtr InfoTypeDevice();

        [DllImport("adapters.dll")]
        public static extern void ReleaseData(); //
        
        [DllImport("adapters.dll")]
        public static extern IntPtr GetTypeAdapter(ref int t); // 

    }
}
