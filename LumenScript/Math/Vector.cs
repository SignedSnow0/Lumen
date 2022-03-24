using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Lumen.Math
{
    public struct Vector2
    {
        public float X;
        public float Y;

        public float U { get => X; set => X = value; }
        public float V { get => Y; set => Y = value; }
        
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
    
    public struct Vector3
    {
        public int Count => 3;

        public float X;
        public float Y;
        public float Z;

        public float R { get => X; set => X = value; }
        public float G { get => Y; set => Y = value; }
        public float B { get => Z; set => Z = value; }
        
        public float U { get => X; set => X = value; }
        public float V { get => Y; set => Y = value; }
        public float W { get => Z; set => Z = value; }

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
    
    public struct Vector4
    {
        public float X;
        public float Y;
        public float Z;
        public float W;

        public float R { get => X; set => X = value; }
        public float G { get => Y; set => Y = value; }
        public float B { get => Z; set => Z = value; }
        public float A { get => W; set => W = value; }

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