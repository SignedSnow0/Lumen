using Lumen.Components;
using Lumen.Core;

namespace Lumen
{
    public class TestScript : Script
    {
        public override void Start()
        {
            var e = Entity.Create();
            var translation = (e.Components[0] as Transform)!.Translation;
            translation.X += 1.0f;
            (e.Components[0] as Transform)!.SetTranslation(translation);
        }

        public override void Update()
        {
            
        }
    }
}