#include "ObjectData.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	float4 texColor = float4(tex.Sample(smp, input.uv));
	float3 light = normalize(lightVec); //�����ւ̃x�N�g���𐳋K��
	float diffuse = saturate(dot(-light, input.normal)); //�����ւ̃x�N�g���Ɩ@���x�N�g���̓���([0, 1]�͈̔͂ɃN�����v)
	float brightness = diffuse + 0.3f; //�A���r�G���g����0.3�Ƃ��Čv�Z
	return float4(texColor.rgb * brightness, texColor.a) * color;
	//return color;
}