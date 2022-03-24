using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using GlmSharp;
using Lumen.Math;

namespace Lumen.Components
{
    public class Component
    {
        public Entity Owner { get => _owner; }
        
        protected Component(Entity owner)
        {
            _owner = owner;
        }

        protected readonly Entity _owner;
    }

    public class Transform : Component
    {
        public Vector3 Translation => GetTranslation(_owner.Id);
        public Vector3 Rotation => GetRotation(_owner.Id);
        public Vector3 Scale => GetScale(_owner.Id);

        public Transform(Entity owner) 
            : base(owner)
        { }
        
        public void SetTranslation(Vector3 translation) => SetTranslation(_owner.Id, translation.X, translation.Y, translation.Z);
        public void SetRotation(Vector3 translation) => SetRotation(_owner.Id, translation.X, translation.Y, translation.Z);
        public void SetScale(Vector3 translation) => SetScale(_owner.Id, translation.X, translation.Y, translation.Z);

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