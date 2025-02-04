cbuffer ModelData : register(b0)
{
    float4x4 Model;
}

cbuffer CameraData : register(b1)
{
    float4x4 View;
    float4x4 Projection;
}


struct Input {
	float4 pos : POSITION0;
};

cbuffer ModelData : register(b0) {
    float4x4 Model;
};
cbuffer CameraData : register(b1)
{
    float4x4 View;
    float4x4 Projection;
};

struct Output {
	float4 pos : SV_POSITION;
};

Output main(Input input) {
	Output output = (Output)0;

    output.pos = mul(input.pos, Model);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    return output;
}