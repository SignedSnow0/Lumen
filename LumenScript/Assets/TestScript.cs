using System.Runtime.InteropServices;
using Lumen.Components;
using Lumen.Core;
using Lumen.Math;

namespace Lumen
{
    public class TestScript : Script
    {
        public override void Start()
        {
            var translation = (entity.Components[0] as Transform)!.Translation;
            System.Console.WriteLine($"Received value: {translation.X}, {translation.Y}, {translation.Z}");
        }

        public override void Update()
        {
            
        }
    }
}