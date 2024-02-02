#include "GuiCls.h"
#include <iostream>

GuiCls::GuiCls()
{
}

void GuiCls::Init(sf::RenderWindow* window, uint16_t initialSpawnInterval)
{
	Window = window;
	ImGui::SFML::Init(*Window);
	// scale the imgui ui by a given factor, does not affect text size
	ImGui::GetStyle().ScaleAllSizes(1.0f);
	SpawnInterval = initialSpawnInterval;
}

void GuiCls::Update(sf::Clock& deltaClock, EntityMap& EntitiesMap)
{
	ManuelSpawnFlag = false;
	// update imgui for this freame with the time that the last frame took
	ImGui::SFML::Update(*Window, deltaClock.restart());

	ImGui::Begin("Debug Panel");
	//ImGui::Text("Parameters of Shapes");

	if (ImGui::BeginTabBar("xxx"))
	{
		/*Systems Lab*/
		if (ImGui::BeginTabItem("Systems"))
		{
			ImGui::Checkbox("Movement", &IsMovementActive);
			ImGui::Checkbox("LifeSpan", &IsLifeSpanActive);
			ImGui::Checkbox("Collision", &IsCollisionActive);
			ImGui::Checkbox("Spawning", &IsSpawnerActive);
			ImGui::Indent();
			ImGui::SliderInt("Spawn Interval", &SpawnInterval, 10, 240);
			ManuelSpawnFlag = ImGui::Button("Manuel Spawn", ImVec2(120, 30));
			ImGui::Unindent();
			ImGui::Checkbox("Special Shoot", &IsSpecialShootActive);

			ImGui::EndTabItem();
		}


		/* Entity Manager Tab*/
		if (ImGui::BeginTabItem("Entity Manager"))
		{
			if (ImGui::CollapsingHeader("Entities by Tags"))
			{
				ImGui::Indent();
				for (auto e : EntitiesMap)
				{
					std::string name = e.first + " Entities";
					
					if (ImGui::CollapsingHeader(name.c_str()))
					{
						if (e.second.size() != 0) {
							if (ImGui::BeginTable(e.first.c_str(), 3))
							{

								for (size_t i = 0; i < e.second.size(); i++)
								{
									ImGui::TableNextRow();

									ImGui::TableSetColumnIndex(0);
									ImGui::Text("%d", e.second[i]->GetId());

									ImGui::TableSetColumnIndex(1);
									ImGui::Text("%s", e.first.c_str());

									auto pos = e.second[i]->cTransform->Position;

									ImGui::TableSetColumnIndex(2);
									ImGui::Text("(%d, %d)", (int)pos.x, (int)pos.y);
								}

								ImGui::EndTable();
							}
						}
					}
				}
				ImGui::Unindent();
			}
			

			if (ImGui::CollapsingHeader("All Entities"))
			{
				if (ImGui::BeginTable("table1", 4))
				{
					for (auto e : EntitiesMap)
					{
						for (size_t i = 0; i < e.second.size(); i++)
						{
							ImGui::TableNextRow();

							ImGui::TableSetColumnIndex(0);
							std::string btnName = "D";
							if (ImGui::Button(btnName.c_str(), ImVec2(40, 40))) {
								e.second[i]->Destroy();
							}

							ImGui::TableSetColumnIndex(1);
							ImGui::Text("%d", e.second[i]->GetId());

							ImGui::TableSetColumnIndex(2);
							ImGui::Text("%s", e.first.c_str());

							auto pos = e.second[i]->cTransform->Position;

							ImGui::TableSetColumnIndex(3);
							ImGui::Text("(%d, %d)", (int)pos.x, (int)pos.y);
						}
					}
					ImGui::EndTable();
				}
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void GuiCls::Render()
{
	ImGui::SFML::Render(*Window);
}

void GuiCls::ProcessEvent(sf::Event& event)
{
	ImGui::SFML::ProcessEvent(*Window, event);
}

int GuiCls::GetSpawnInterval()
{
	return SpawnInterval;
}

bool GuiCls::GetManuelSpawnFlag()
{
	return ManuelSpawnFlag;
}

bool GuiCls::GetAnyItemHovered()
{
	return ImGui::IsAnyItemHovered();
}
