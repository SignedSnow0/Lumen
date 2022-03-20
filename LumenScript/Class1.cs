using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Lumen
{
    public class Test
    {
        public static void Print()
        {
            System.Console.WriteLine("Hello world!");
            HelloBack();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void HelloBack();
    }
}