#include "FormatUtils.hpp"


QString imageFormatToString(QImage::Format format)
{
	switch (format) {
#define imageFormatToStringStanza(A)  case(QImage::Format_##A):{return #A;}
		imageFormatToStringStanza(Invalid);
		imageFormatToStringStanza(Mono);
		imageFormatToStringStanza(MonoLSB);
		imageFormatToStringStanza(Indexed8);
		imageFormatToStringStanza(RGB32);
		imageFormatToStringStanza(ARGB32);
		imageFormatToStringStanza(ARGB32_Premultiplied);
		imageFormatToStringStanza(RGB16);
		imageFormatToStringStanza(ARGB8565_Premultiplied);
		imageFormatToStringStanza(RGB666);
		imageFormatToStringStanza(ARGB6666_Premultiplied);
		imageFormatToStringStanza(RGB555);
		imageFormatToStringStanza(ARGB8555_Premultiplied);
		imageFormatToStringStanza(RGB888);
		imageFormatToStringStanza(RGB444);
		imageFormatToStringStanza(ARGB4444_Premultiplied);
		imageFormatToStringStanza(RGBX8888);
		imageFormatToStringStanza(RGBA8888);
		imageFormatToStringStanza(RGBA8888_Premultiplied);
		imageFormatToStringStanza(BGR30);
		imageFormatToStringStanza(A2BGR30_Premultiplied);
		imageFormatToStringStanza(RGB30);
		imageFormatToStringStanza(A2RGB30_Premultiplied);
		imageFormatToStringStanza(Alpha8);
		imageFormatToStringStanza(Grayscale8);
		imageFormatToStringStanza(RGBX64);
		imageFormatToStringStanza(RGBA64);
		imageFormatToStringStanza(RGBA64_Premultiplied);
		imageFormatToStringStanza(Grayscale16);
		imageFormatToStringStanza(BGR888);
		imageFormatToStringStanza(RGBX16FPx4);
		imageFormatToStringStanza(RGBA16FPx4);
		imageFormatToStringStanza(RGBA16FPx4_Premultiplied);
		imageFormatToStringStanza(RGBX32FPx4);
		imageFormatToStringStanza(RGBA32FPx4);
		imageFormatToStringStanza(RGBA32FPx4_Premultiplied);
		imageFormatToStringStanza(CMYK8888);
	default: return "Unknown Format";
#undef imageFormatToStringStanza
		
	}
}


QString videoFormatToString(QVideoFrameFormat::PixelFormat format)
{
	switch (format) {
#define videoFormatToStringStanza(A)  case(QVideoFrameFormat::Format_##A):{return #A;}
		videoFormatToStringStanza(Invalid);
		videoFormatToStringStanza(ARGB8888);
		videoFormatToStringStanza(ARGB8888_Premultiplied);
		videoFormatToStringStanza(XRGB8888);
		videoFormatToStringStanza(BGRA8888);
		videoFormatToStringStanza(BGRA8888_Premultiplied);
		videoFormatToStringStanza(BGRX8888);
		videoFormatToStringStanza(ABGR8888);
		videoFormatToStringStanza(XBGR8888);
		videoFormatToStringStanza(RGBA8888);
		videoFormatToStringStanza(RGBX8888);
		videoFormatToStringStanza(AYUV);
		videoFormatToStringStanza(AYUV_Premultiplied);
		videoFormatToStringStanza(YUV420P);
		videoFormatToStringStanza(YUV422P);
		videoFormatToStringStanza(YV12);
		videoFormatToStringStanza(UYVY);
		videoFormatToStringStanza(YUYV);
		videoFormatToStringStanza(NV12);
		videoFormatToStringStanza(NV21);
		videoFormatToStringStanza(IMC1);
		videoFormatToStringStanza(IMC2);
		videoFormatToStringStanza(IMC3);
		videoFormatToStringStanza(IMC4);
		videoFormatToStringStanza(Y8);
		videoFormatToStringStanza(Y16);
		videoFormatToStringStanza(P010);
		videoFormatToStringStanza(P016);
		videoFormatToStringStanza(SamplerExternalOES);
		videoFormatToStringStanza(Jpeg);
		videoFormatToStringStanza(SamplerRect);
		videoFormatToStringStanza(YUV420P10);
	default: return "Unknown Format";
#undef videoFormatToStringStanza
	}
}

