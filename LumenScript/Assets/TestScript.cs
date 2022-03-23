using Lumen.Components;
using Lumen.Core;

namespace Lumen
{
    public class TestScript : Script
    {
        public override void Start()
        {
            System.Console.WriteLine("Hello world");
            var translation = (entity.Components[0] as Transform)!.Translation;
            translation.X += 1.0f;
            (entity.Components[0] as Transform)!.SetTranslation(translation);
        }

        public override void Update()
        {
            
        }
    }
}