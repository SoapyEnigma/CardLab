#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_stdlib.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

enum ELEMENT_TYPES
{
    NONE,
    COLORLESS,
    DARKNESS,
    DRAGON,
    FIGHTING,
    FIRE,
    GRASS,
    LIGHTNING,
    METAL,
    PSYCHIC,
    WATER
};

enum STAGES
{
    BASIC,
    STAGE_1,
    STAGE_2
};

enum RARITY
{
    PROMO,              // Promo
    COMMON,             // 1 Diamond
    UNCOMMON,           // 2 Diamond
    RARE,               // 3 Diamond
    DOUBLE_RARE,        // 4 Diamond
    ART_RARE,           // 1 Star
    SUPER_RARE,         // 2 Star
    SPECIAL_ART_RARE,   // 2 Star (Rainbow Border)
    IMMERSIVE_RARE,     // 3 Star
    CROWN_RARE,         // Crown
    SHINY_RARE,         // 1 Star Shiny
    D_SHINY_RARE,       // 2 Star Shiny
};

struct Pokemon
{
    bool isEX = false;
    bool hasAbility = true;

    std::string name = "";
    std::string evolvesFrom = "";
    std::string basicInfo = "";
    std::string abilityName = "";
    std::string abilityEffect = "";
    std::string attackName1 = "";
    std::string attackEffect1 = "";
    std::string attackName2 = "";
    std::string attackEffect2 = "";
    std::string illustrator = "";
    std::string flavorText = "";

    i16 health = 0;

    u8 attackCount = 2;
    i8 attackCost1 = 0;
    u8 attackDamage1 = 0;
    i8 attackCost2 = 0;
    u8 attackDamage2 = 0;
    u8 retreatCost = 0;

    ELEMENT_TYPES currentType = NONE;
    ELEMENT_TYPES currentWeakness = NONE;
    STAGES currentStage = BASIC;
    RARITY currentRarity = PROMO;

    std::vector<std::string> typeNames =
    {
        "None", "Colorless", "Darkness", "Dragon", "Fighting",
        "Fire", "Grass", "Lightning", "Metal", "Psychic", "Water"
    };

    std::vector<std::string> rarityNames =
    {
        "Promo", "Common", "Uncommon", "Rare", "Double Rare",
        "Art Rare", "Super Rare", "Special Art Rare",
        "Immersive Rare", "Crown Rare",
        "Shiny Rare", "Double Shiny Rare"
    };

    std::vector<std::string> stageNames = { "Basic", "Stage 1", "Stage 2" };
    std::vector<std::string> attackCountNames = { "Zero", "One", "Two" };

    std::vector<u8> attack1CostType = { 0, 0, 0, 0, 0, 0 };
    std::vector<u8> attack2CostType = { 0, 0, 0, 0, 0, 0 };
};

void Combo(std::string_view name, std::vector<std::string>& vector, u8& index, ImGuiComboFlags flags = 0)
{
    if (ImGui::BeginCombo(name.data(), vector[index].c_str(), flags))
    {
        for (u8 n = 0; n < vector.size(); n++)
        {
            const bool is_selected = (index == n);
            if (ImGui::Selectable(vector[n].c_str(), is_selected))
                index = n;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void BeginRow(std::string_view name, f32 dummyWidth)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", name.data());
    ImGui::Dummy({ dummyWidth, 0 });
    ImGui::TableSetColumnIndex(1);
}

template<typename T>
concept SmallInt = std::integral<T> && (sizeof(T) <= sizeof(int));

template<SmallInt T>
void Row(const std::string& name, T& value, f32 dummyWidth, i32 step = 1)
{
    BeginRow(name, dummyWidth);
    ImGui::InputInt(("##" + name).c_str(), &value, step);
}

void Row(const std::string& name, std::vector<std::string>& vector, u8& index, f32 dummyWidth)
{
    BeginRow(name, dummyWidth);
    Combo("##" + name, vector, index);
}

void Row(const std::string& name, bool& value, f32 dummyWidth)
{
    BeginRow(name, dummyWidth);
    ImGui::Checkbox(("##" + name).c_str(), &value);
}

void Row(const std::string& name, std::string& value, f32 dummyWidth)
{
    BeginRow(name, dummyWidth);
    ImGui::InputText(("##" + name).c_str(), &value);
}

int main()
{
    std::string name = "Enigma's Card Lab ";
    std::string version = "v0.01.";
    std::string date = "250419a";

    ImVec2 windowSize = { 720, 800 };

    sf::RenderWindow window(sf::VideoMode(windowSize), name + version + date);

    if (!ImGui::SFML::Init(window))
    {
    
    }

    Pokemon mon;
    f32 rowWidth = 100.0f;

    ImGuiWindowFlags guiWindowflags = {};
    guiWindowflags |= ImGuiWindowFlags_NoResize;
    guiWindowflags |= ImGuiWindowFlags_NoMove;
    guiWindowflags |= ImGuiWindowFlags_NoDecoration;
    guiWindowflags |= ImGuiWindowFlags_NoBackground;

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                sf::View view = window.getView();
                view.setSize({ static_cast<float>(resized->size.x), static_cast<float>(resized->size.y) });
                view.setCenter({ static_cast<float>(resized->size.x) / 2.f, static_cast<float>(resized->size.y) / 2.f });
                window.setView(view);
                windowSize = view.getSize();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // ImGui
        ImGui::SetNextWindowPos(ImVec2{ 0, 0 });
        ImGui::SetNextWindowSize({ windowSize.x / 2.0f, windowSize.y });
        if (ImGui::Begin("Settings", nullptr, guiWindowflags))
        {
            if (ImGui::BeginTable("##Table", 2, ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("##Names");
                ImGui::TableSetupColumn("##Values", ImGuiTableColumnFlags_WidthFixed, windowSize.x / 3.0f);

                Row("Name", mon.name, rowWidth);
                Row("Type", mon.typeNames, reinterpret_cast<u8&>(mon.currentType), rowWidth);
                Row("Stage", mon.stageNames, reinterpret_cast<u8&>(mon.currentStage), rowWidth);
                Row("Health", mon.health, rowWidth, 10);
                Row("Info", mon.basicInfo, rowWidth);
                Row("Is Ex", mon.isEX, rowWidth);

                Row("Has Ability", mon.hasAbility, rowWidth);
                if (mon.hasAbility)
                {
                    Row("Ability Name", mon.abilityName, rowWidth);
                    Row("Ability Effect", mon.abilityEffect, rowWidth);
                }

                Row("Attack Count", mon.attackCountNames, mon.attackCount, rowWidth);
                if (mon.attackCount >= 1)
                {
                    Row("Attack Cost 1", mon.attackCost1, rowWidth);

                    for (i32 i = 0; i < mon.attackCost1 && i < 6; i++)
                    {
                        Row("Cost 1 Type " + std::to_string(i + 1), mon.typeNames, mon.attack1CostType[i], rowWidth);
                    }

                    Row("Attack 1 Name", mon.attackName1, rowWidth);
                    Row("Attack 1 Effect", mon.attackEffect1, rowWidth);
                }

                if (mon.attackCount == 2)
                {
                    Row("Attack Cost 2", mon.attackCost2, rowWidth);

                    for (i32 i = 0; i < mon.attackCost2 && i < 6; i++)
                    {
                        Row("Cost 2 Type " + std::to_string(i + 1), mon.typeNames, mon.attack2CostType[i], rowWidth);
                    }

                    Row("Attack 2 Name", mon.attackName2, rowWidth);
                    Row("Attack 2 Effect", mon.attackEffect2, rowWidth);
                }

                Row("Weakness", mon.typeNames, reinterpret_cast<u8&>(mon.currentWeakness), rowWidth);
                Row("Retreat Cost", mon.retreatCost, rowWidth);
                Row("Illustrator", mon.illustrator, rowWidth);
                Row("Rarity", mon.rarityNames, reinterpret_cast<u8&>(mon.currentRarity), rowWidth);

                if (!mon.isEX)
                {
                    Row("Flavor Text", mon.flavorText, rowWidth);
                }

                ImGui::EndTable();
            }

            if (ImGui::Button("Save"))
            {
                //saveScreenshotNow = true;
            }

            // Int Value Bounds Checks -- Written this way to provide wrap around
            {
                if (mon.health > 300)
                    mon.health = 0;
                else if (mon.health < 0)
                    mon.health = 300;

                if (mon.attackCost1 > 6)
                    mon.attackCost1 = 0;
                else if (mon.attackCost1 < 0)
                    mon.attackCost1 = 6;

                if (mon.attackCost2 > 6)
                    mon.attackCost2 = 0;
                else if (mon.attackCost2 < 0)
                    mon.attackCost2 = 6;

                if (mon.retreatCost > 6)
                    mon.retreatCost = 0;
                else if (mon.retreatCost < 0)
                    mon.retreatCost = 6;
            }
        }
        ImGui::End();

        window.clear();

        // Window Draw

        ImGui::SFML::Render(window);
        
        window.display();
    }

    ImGui::SFML::Shutdown();
}