/*
    This header declares functions and values that can be platform dependant.
    In the definitions goes part of the non-portable code.
    By default it depends on NasmMath so it must be linked.
*/

#ifndef _LD_MATH_H
#define _LD_MATH_H

   /*  The same identity matrix can be defined in many othar places
        if it's not defined, define it here */
    #ifndef __IDENTITY_MATRIX_4X4_FLOAT_
    #define __IDENTITY_MATRIX_4X4_FLOAT_
        float Identity_Matrix_4x4[16]=
        {
            1.f,0.f,0.f,0.f,
            0.f,1.f,0.f,0.f,
            0.f,0.f,1.f,0.f,
            0.f,0.f,0.f,1.f
        };
    #endif  

#ifdef __cplusplus
extern "C" 
{
#endif

    void M4x4MUL(float * A, float *B, float * Result);
    void M4x4V4MUL(float * Matrix, float *Vector, float * Result);
    void M4x4V4_PseudoV3_W1_MUL (float * Matrix, float * Vector, float * VectorResultado);
    void M4x4_PseudoM3x3_V4_PseudoV3_W1_MUL 
    (float * Matrix, float * Vector, float * VectorResultado);

    void NormalizeVec2 (float * Vec2, float * Result);
    void NormalizeDoubleVec2 (double * Vec2, double * Result);
    void NormalizeVec3(float *A, float *R);
    void NormalizeVec4(float * Vec4, float * Result);

    void QuaternionMUL (float * A, float *B, float * Result);
    void QuaternionToMatrix4x4(float * Quaternion, float * Matrix);
    void AxisAngleToQuaternion(float * Axis, float Angle, float * NewQuartenion);  

    float V2V2Dot (float * Vec2_A, float * Vec2_B);
    float V3V3Dot (float * Vec3_A, float * Vec3_B);
    void CrossProductVec3(float *A, float *B, float *C);

    void V2ScalarMUL(float * Vec2_A, float Scalar_B, float * Vec2_Result);

    void V2V2ADD(float * Vec2_A, float * Vec2_B, float * Vec2_Result);
    void V2V2SUB(float * Vec2_A, float * Vec2_B, float * Vec2_Result);
    void V3V3ADD(float * Vec3_A, float * Vec3_B,float * Vec3_Result);
    void V3V3SUB(float * Vec3_A, float * Vec3_B,float * Vec3_Result);
    void V4V4ADD (float * A, float *B, float * Result);
    void V4V4SUB (float * A, float *B, float * Result);

    void V2CalculatePerpendicular
    (float * Vec2_A,float * Vec2_Result,float ClockWise_Multiplier);

    void V2Rotate_FPU(float * Vec2_A, float * Angle_Degrees, float * Vec2_Result);

    void PerspectiveProjectionMatrix4x4
    (float *matrix, float fovyInDegrees, float aspectRatio,float znear, float zfar);
    void OrthogonalProjectionMatrix4x4
    (float *matrix, float Width, float Height, float znear, float zfar);
    void ViewLookAt(float * matrix, float * Vec3From,float * Vec3to, float * Vec3Up);

#ifdef __cplusplus
}
#endif

#endif
