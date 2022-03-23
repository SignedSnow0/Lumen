using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Lumen.Components;

namespace Lumen
{
    public class Entity
    {
        public uint Id { get => _id; }
        public List<Component> Components { get; } = new List<Component>();

        public static Entity Create() => new Entity(CreateEntity());
        public static Entity? Get(uint id) => Exists(id) ? new Entity(id) : null;
        public static Entity GetOrCreate(uint id) => Exists(id) ? new Entity(id) : new Entity(CreateEntity());
        
        public void AddComponent<T>()
            where T : Component
        {
            AddComponent(_id, typeof(T).Name);
            Components.Add(new Transform(this));
        }
        
        public Entity(uint id)
        {
            _id = id;
            Components.Add(new Transform(this));
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern uint CreateEntity();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Exists(uint id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void AddComponent(uint id, string compName);
        
        private readonly uint _id;
    }
}