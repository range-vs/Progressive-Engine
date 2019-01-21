#include "buffers.h"

ConstantBufferVS::operator ConstantBufferNoLight()
{
	return ConstantBufferNoLight(Translation, Rotation, Scale, View, Projection, center);
}

ConstantBufferVSInstansing::operator ConstantBufferNoLightInstansing()
{
	return ConstantBufferNoLightInstansing(View, Projection, center);
}
