using Lumen.Components;
using Lumen.Core;

namespace Lumen
{
    public class TestScript : Script
    {
        public override void Start()
        {
            
        }

        public override void Update()
        {
            var translation = entity.GetComponent<Transform>()!.Translation;
            translation.X += 0.01f;
            entity.GetComponent<Transform>()!.SetTranslation(translation);
        }

        ~TestScript()
        {
            System.Console.WriteLine("Test script destroyed");
        }
    }
}