#include "ASCIIMAGE.h"
#include "UI/UI.hpp"

ASCIIMAGE::ASCIIMAGE()
{
}

void ASCIIMAGE::OnUpdate(float deltaTime)
{
}

void ASCIIMAGE::OnImGuiRender()
{
    if (ImGui::Button("Select File"))
    {
        auto file_path = UI::FileDialog::GetFilePath();
        Texture::FromPixelArrayToASCII(file_path, light_to_dark, shading);
    }
    ImGui::Checkbox("Light to Dark", &light_to_dark);

    ImGui::Text("Shading");

    if(ImGui::RadioButton("0", shading == 0))
		shading = 0;
    ImGui::SameLine();
    if(ImGui::RadioButton("1", shading == 1))
		shading = 1;
    ImGui::SameLine();
	if(ImGui::RadioButton("2", shading == 2))
        shading = 2;

}
