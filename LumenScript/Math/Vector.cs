using System.Runtime.InteropServices;

namespace Lumen.Math
{
    [StructLayout(LayoutKind.Explicit)]
    public class Vector2
    {
        [FieldOffset(0)]
        public float X;
        [FieldOffset(0)]
        public float U;
        
        [FieldOffset(4)]
        public float Y;
        [FieldOffset(4)]
        public float V;

        public Vector2(float scalar)
        {
            X = Y = scalar;
        }

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }
    }
    
    [StructLayout(LayoutKind.Explicit)]
    public class Vector3
    {
        [FieldOffset(0)]
        public float X;
        [FieldOffset(0)]
        public float R;
        [FieldOffset(0)]
        public float U;
        
        [FieldOffset(4)]
        public float Y;
        [FieldOffset(4)]
        public float G;
        [FieldOffset(4)]
        public float V;
        
        [FieldOffset(8)]
        public float Z;
        [FieldOffset(8)]
        public float B;
        [FieldOffset(8)]
        public float W;
        
        public Vector3(float scalar)
        {
            X = Y = Z = scalar;
        }
        
        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
    }
    
    [StructLayout(LayoutKind.Explicit)]
    public class Vector4
    {
        [FieldOffset(0)]
        public float X;
        [FieldOffset(0)]
        public float R;
        
        [FieldOffset(4)]
        public float Y;
        [FieldOffset(4)]
        public float G;
        
        [FieldOffset(8)]
        public float Z;
        [FieldOffset(8)]
        public float B;
        
        [FieldOffset(12)]
        public float W;
        [FieldOffset(12)]
        public float A;

        public Vector4(float scalar)
        {
            X = Y = Z = W = scalar;
        }

        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }
    }
}