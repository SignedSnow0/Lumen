using System.Runtime.CompilerServices;
using Lumen.Math;

namespace Lumen.Components
{
    public class Component
    {
        protected Component(Entity owner)
        {
            _owner = owner;
        }

        protected Entity _owner;
    }
    
    public class Transform : Component
    {
        public Vector3 Translation { get => GetTranslation(_owner.Id); }
        public Vector3 Rotation { get; }
        public Vector3 Scale { get; }

        public Transform(Entity owner) 
            : base(owner)
        {
            Rotation = new Vector3(0);
            Scale = new Vector3(1.0f);
        }
        
        public void SetTranslation(Vector3 translation)
        {
            SetTranslation(_owner.Id, translation.X, translation.Y, translation.Z);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetTranslation(uint entityId, float x, float y, float z);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 GetTranslation(uint entityId);
    }
}