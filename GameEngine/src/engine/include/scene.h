#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <memory>

#include "imgui/imgui.h"
#include "camera.h"

class Scene_t
{
public:
	Scene_t() = default;
	virtual ~Scene_t() = default;

	virtual void OnUpdate(float deltaTime) {}
	virtual void OnImGuiRender() {}

	inline static std::shared_ptr<unsigned int> pScreenWidth;
	inline static std::shared_ptr<unsigned int> pScreenHeight;

	inline static std::shared_ptr<BaseCam> pCamera;
};

class Menu : public Scene_t
{
private:
	Scene_t*& m_CurrentScene;
	std::vector< std::pair< std::string, std::function<Scene_t* ()>>> m_Scenes;

	friend class Application;

public:
	std::string c_SceneName = "Main Menu";
	
	explicit Menu(Scene_t*& currentTestPointer)
		:m_CurrentScene(currentTestPointer) {}

	void OnImGuiRender() override
	{
		for (auto const& scene : m_Scenes)
		{
			if (ImGui::Button(scene.first.c_str()))
			{
				m_CurrentScene = scene.second();
				c_SceneName = scene.first;
			}
		}
	}

	template <typename T>
	void RegisterApp(const std::string& name)
	{
		std::cout << "Registering Scene: " << name << "\n";
		m_Scenes.push_back(std::make_pair(name, []() { return new T(); }));
	}
};
