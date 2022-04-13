#include "ObjectData.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD, float3 normal : NORMAL)
{
	VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
	output.svpos = mul(mat, pos); //���W�ɍs�����Z
	output.normal = normal;
	output.uv = uv;
	return output;
}