namespace Lumen.Core
{
    public class Script
    {
        public Entity entity;
        
        public virtual void Start()
        { }
        
        public virtual void Update()
        { }

        private void SetEntity(uint id)
        {
            entity = new Entity(id);
        }
    }
}