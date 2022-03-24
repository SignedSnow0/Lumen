using System.Runtime.CompilerServices;
using Lumen.Components;

namespace Lumen
{
    public class Entity
    {
        public uint Id { get; }

        public static Entity Create() => new Entity(CreateEntity());
        public static Entity? Get(uint id) => Exists(id) ? new Entity(id) : null;
        public static Entity GetOrCreate(uint id) => Exists(id) ? new Entity(id) : new Entity(CreateEntity());

        public Entity()
        {
            Id = uint.MaxValue;
        }

        public Entity(uint id)
        {
            Id = id;
        }
        
        public void AddComponent<T>()
            where T : Component
        {
            AddComponent(Id, typeof(T).Name);
        }

        public T? GetComponent<T>()
            where T : Component, new()
        {
            if (HasComponent<T>())
            {
                return new T
                {
                    Owner = this
                };
            }

            return null;
        }

        public bool HasComponent<T>()
            where T : Component
        {
            return HasComponent(Id, typeof(T).Name);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern uint CreateEntity();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Exists(uint id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void AddComponent(uint id, string compName);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool HasComponent(uint id, string compName);
    }
}