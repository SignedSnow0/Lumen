using Lumen.Components;

namespace Lumen
{
    public class TestClass
    {
        public static void Test()
        {
            var e = Entity.Create();
            System.Console.WriteLine((e.Components[0] as Transform)!.Translation.X);
        }
    }
}