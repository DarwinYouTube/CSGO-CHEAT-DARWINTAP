D3DCOLOR FLOAT4TOD3DCOLOR(float Col[])
{
	ImU32 col32_no_alpha = ImGui::ColorConvertFloat4ToU32(ImVec4(Col[0], Col[1], Col[2], Col[3]));
	float a = (col32_no_alpha >> 24) & 255;
	float r = (col32_no_alpha >> 16) & 255;
	float g = (col32_no_alpha >> 8) & 255;
	float b = col32_no_alpha & 255;
	return D3DCOLOR_ARGB((int)a, (int)b, (int)g, (int)r);
}

namespace Colors {

	// ARGB
	float boxColor[] = {255, 255, 255, 255}; 
	float cornerColor[] = { 255, 255, 255, 255 }; 
	float crosshairColor[] = { 255, 255, 255, 255 };
	float aimbotFovColor[] = { 255, 255, 255, 255 };
	//RGB
	float glowColor[] = { 255, 255, 255 }; 

}