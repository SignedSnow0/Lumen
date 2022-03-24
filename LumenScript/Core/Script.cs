namespace Lumen.Core
{
    public class Script
    {
        public Entity entity { get; private set; }  = new Entity();

        public virtual void Start()
        { }
        
        public virtual void Update()
        { }

        private void SetEntity(uint id)
        {
            entity = new Entity(id);
        }

        ~Script()
        {
            System.Console.WriteLine("Script destroyed");
        }
    }
}