using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace types
{
    struct ParamsRenderThread
    {
        public IntPtr hwndMain;
        public IntPtr hInstCurrentProcess;

        public ParamsRenderThread(IntPtr hwndMain, IntPtr hInstCurrentProcess)
        {
            this.hwndMain = hwndMain;
            this.hInstCurrentProcess = hInstCurrentProcess;
        }
    }

}
