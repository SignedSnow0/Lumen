using System.Runtime.CompilerServices;
using Lumen.Math;

namespace Lumen.Components
{
    public abstract class Component
    {
        public Entity Owner { get; set; } = new Entity();
    }

    public class Transform : Component
    {
        public Vector3 Translation => GetTranslation(Owner.Id);
        public Vector3 Rotation => GetRotation(Owner.Id);
        public Vector3 Scale => GetScale(Owner.Id);
        
        public void SetTranslation(Vector3 translation) => SetTranslation(Owner.Id, translation.X, translation.Y, translation.Z);
        public void SetRotation(Vector3 translation) => SetRotation(Owner.Id, translation.X, translation.Y, translation.Z);
        public void SetScale(Vector3 translation) => SetScale(Owner.Id, translation.X, translation.Y, translation.Z);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void SetTranslation(uint entityId, float x, float y, float z);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 GetTranslation(uint entityId);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetRotation(uint entityId, float x, float y, float z);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 GetRotation(uint entityId);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetScale(uint entityId, float x, float y, float z);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 GetScale(uint entityId);
    }
}