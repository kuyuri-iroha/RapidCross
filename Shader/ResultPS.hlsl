cbuffer g_cb : register(b0)
{
	matrix World;
	float Param;
	float Alpha;
	float Time;
	float dammy;
}

// Data of input
struct PSInput
{
	float4 mPos : SV_POSITION;
	float2 mUV : TEXCOORD0;
};

// Data of output
struct PSOutput
{
	float4 mColor : SV_TARGET;
};


// Inter-pixel shared data.
static const float PI =3.14159265f;
static const float angle =30.0f;
static const float fov =angle *0.5f *PI /180.0f;
static const float3  initCam =float3(-0.0f, 0.1f, 4.0f);
static const float3 lightDir = normalize(float3(0.3f, -2.0f, 1.0f));


float3 Rotate(float3 pos, float angle, float3 axis)
{
    float3 a =normalize(axis);
    float s =sin(angle);
    float c =cos(angle);
    float r =1.0f -c;
    float3x3 m =float3x3(
        a.x *a.x *r +c,
        a.y *a.x *r +a.z *s,
        a.z *a.x *r -a.y *s,
        a.x *a.y *r -a.z *s,
        a.y *a.y *r +c,
        a.z *a.y *r +a.x *s,
        a.x *a.z *r +a.y *s,
        a.y *a.z *r -a.x *s,
        a.z *a.z *r +c
    );
    return mul(m, pos);
}


float Mod(float x, float y)
{
	return x -y *floor(x /y);
}


float Distance(float3 pos)
{
    float3 q = abs(Rotate(Rotate(pos, Time, float3(0.0f, -1.0f, 0.0f)), Time, float3(-1.0f, 0.0f, 0.0f)));
	return length(max(q -float3(0.5f, 0.5f, 0.5f), 0.0f));
}


float3 Normal(float3 pos)
{
	float delta = 0.0001;
	return normalize(float3(
		Distance(pos +float3(delta, 0.0f, 0.0f)) -Distance(pos + float3(-delta, 0.0f, 0.0f)),
		Distance(pos +float3(0.0f, delta, 0.0f)) -Distance(pos + float3(0.0f, -delta, 0.0f)),
		Distance(pos +float3(0.0f, 0.0f, delta)) -Distance(pos + float3(0.0f, 0.0f, -delta))
	));
}


PSOutput ResultPS(PSInput input)
{
	PSOutput op;

    float2 pos = float2((input.mUV.x * 2.0f - 1.0f) * (16.0f / 9.0f), (input.mUV.y * 2.0f - 1.0f) * 9.0f / 9.0f);
	
	float3 cam =initCam;

	float3 ray = normalize(float3(sin(fov) *pos.x, sin(fov) *pos.y, -cos(fov)));
	
	float dist =0.0f;
	float len =0.0f;
	float3  rayPos =cam;
	for(int i =0; i < 126; i++)
	{
		dist =Distance(rayPos);
		len +=dist;
		rayPos =cam +ray *len;
	}
	
	if(abs(dist) < 0.001f)
	{
		float3 normal =Normal(rayPos);
		float diff =clamp(dot(normalize(lightDir), normal), 0.015f, 0.5f);
		op.mColor =float4(float3(diff, diff, diff), 1.0f);
	}
	else
	{
		op.mColor =float4(float3(0.0f, 0.0f, 0.0f), 1.0f);
	}

	op.mColor.rgb =op.mColor.rgb;
	return op;
}