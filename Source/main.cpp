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
    std::string name = "";
    i32 health = 0;
    ELEMENT_TYPES type = NONE;
    STAGES stage = BASIC;
    std::string evolvesFrom = "";
    std::string basicInfo = "";
    bool isEX = false;
    bool hasAbility = false;
    std::string abilityName = "";
    std::string abilityEffect = "";
    u8 attackCount = 0;
    i32 attackCost1 = 0;
    std::string attackName1 = "";
    u8 attackDamage1 = 0;
    std::string attackEffect1 = "";
    i32 attackCost2 = 0;
    std::string attackName2 = "";
    u8 attackDamage2 = 0;
    std::string attackEffect2 = "";
    ELEMENT_TYPES weakness = NONE;
    i32 retreatCost = 0;
    std::string illustrator = "";
    RARITY rarity = PROMO;
    std::string flavorText = "";
};

f32 GetDummyWidth(std::string name)
{
    f32 w = ImGui::CalcTextSize(name.c_str()).x;

    return 100 - w < 0 ? 0 : 100 - w;
}

void Prefix(std::string name)
{
    ImGui::PushItemWidth(-100.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::Text(name.c_str());
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(GetDummyWidth(name.c_str()), 0.0f));
    ImGui::SameLine();
}

int main()
{
    ImVec2 windowSize = { 1280, 720 };
    sf::RenderWindow window(sf::VideoMode(windowSize), "Enigma's Card Lab v0.01.250418a");

    if (!ImGui::SFML::Init(window))
    {
    
    }

    std::string cardName = "";
    Pokemon mon;

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
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // ImGui
        ImGui::SetNextWindowPos(ImVec2{ 0, 0 });
        ImGui::SetNextWindowSize(windowSize);
        if (ImGui::Begin("Settings", nullptr, guiWindowflags))
        {
            Prefix("Name");
            ImGui::InputText("##Name", &mon.name);
            cardName = mon.name;

            Prefix("Health");
            ImGui::InputInt("##Health", &mon.health, 10);

            if (mon.health > 300)
            {
                mon.health = 300;
            }

            if (mon.health < 0)
            {
                mon.health = 0;
            }

            //Prefix("Type");
            //if (ImGui::Combo("##Element Type", &mon_CurrentType, typesCStrs.data(), static_cast<i32>(typesCStrs.size())))
            //{
            //    std::string selected = _cardTemplate.types[mon_CurrentType];
            //}

            //Prefix("Stage");
            //if (ImGui::Combo("##Stage", &mon_CurrentStage, stagesCStrs.data(), static_cast<i32>(stagesCStrs.size())))
            //{
            //    std::string selected = _cardTemplate.stages[mon_CurrentStage];
            //}

            Prefix("Info");
            ImGui::InputText("##Info", &mon.basicInfo);

            Prefix("Has Ability?");
            ImGui::Checkbox("##Has Ability?", &mon.hasAbility);

            Prefix("Is Ex?");
            ImGui::Checkbox("##Is Ex?", &mon.isEX);

            if (mon.hasAbility)
            {
                Prefix("Ability Name");
                ImGui::InputText("##Ability Name", &mon.abilityName);

                Prefix("Ability Effect");
                ImGui::InputText("##Ability Effect", &mon.abilityEffect);
            }

            //Prefix("Attack Count");
            //if (ImGui::Combo("##Attack Count", &mon_CurrentAttackCount, attackCountCStrs.data(), static_cast<i32>(attackCountCStrs.size())))
            //{
            //    std::string selected = _cardTemplate.attackCounts[mon_CurrentAttackCount];
            //}

            i32 mon_CurrentAttackCount = 2;

            if (mon_CurrentAttackCount >= 1)
            {
                Prefix("Attack 1 Cost");
                ImGui::InputInt("##Attack 1 Cost", &mon.attackCost1);

                if (mon.attackCost1 > 6 || mon.attackCost1 < 0)
                {
                    mon.attackCost1 = 6;
                }

                //for (i32 i = 0; i < mon->attackCost1; i++)
                //{
                //    ImGui::PushID(5000 + i);
                //    std::string name = "Cost Type " + std::to_string(i + 1);
                //    Prefix(name);
                //    name = "##" + name;
                //    if (ImGui::Combo(name.c_str(), &mon_CostTypes1[i], typesCStrs.data(), static_cast<i32>(typesCStrs.size())))
                //    {
                //        std::string selected = _cardTemplate.types[mon_CostTypes1[i]];
                //    }
                //    ImGui::PopID();
                //}

                Prefix("Attack 1 Name");
                ImGui::InputText("##Attack 1 Name", &mon.attackName1);

                Prefix("Attack 1 Effect");
                ImGui::InputText("##Attack 1 Effect", &mon.attackEffect1);
            }

            if (mon_CurrentAttackCount == 2)
            {
                Prefix("Attack 2 Cost");
                ImGui::InputInt("##Attack 2 Cost", &mon.attackCost2);

                if (mon.attackCost2 > 6 || mon.attackCost2 < 0)
                {
                    mon.attackCost2 = 6;
                }

                //for (i32 i = 0; i < mon->attackCost2; i++)
                //{
                //    ImGui::PushID(6000 + i);
                //    std::string name = "Cost Type " + std::to_string(i + 1);
                //    Prefix(name);
                //    name = "##" + name;
                //    if (ImGui::Combo(name.c_str(), &mon_CostTypes2[i], typesCStrs.data(), static_cast<i32>(typesCStrs.size())))
                //    {
                //        std::string selected = _cardTemplate.types[mon_CostTypes2[i]];
                //    }
                //    ImGui::PopID();
                //}

                Prefix("Attack 2 Name");
                ImGui::InputText("##Attack 2 Name", &mon.attackName2);

                Prefix("Attack 2 Effect");
                ImGui::InputText("##Attack 2 Effect", &mon.attackEffect2);
            }

            //Prefix("Weakness");
            //if (ImGui::Combo("##Weakness", &mon->currentWeakness, typesCStrs.data(), static_cast<i32>(typesCStrs.size())))
            //{
            //    std::string selected = _cardTemplate.types[mon_CurrentWeakness];
            //}

            Prefix("Retreat Cost");
            ImGui::InputInt("##Retreat Cost", &mon.retreatCost);

            if (mon.retreatCost > 6)
            {
                mon.retreatCost = 6;
            }

            if (mon.retreatCost < 0)
            {
                mon.retreatCost = 0;
            }

            Prefix("Illustrator");
            ImGui::InputText("##Illustrator", &mon.illustrator);

            if (!mon.isEX)
            {
                Prefix("Flavor Text");
                ImGui::InputText("##Flavor Text", &mon.flavorText);
            }

            //Prefix("Rarity");
            //if (ImGui::Combo("##Rarity", &mon->rarity, rarityCStrs.data(), static_cast<i32>(rarityCStrs.size())))
            //{
            //    std::string selected = _cardTemplate.rarities[mon_CurrentRarity];
            //}
        }

        if (ImGui::Button("Save"))
        {
            //saveScreenshotNow = true;
        }

        ImGui::End();
        //ImGui::PopStyleVar(2);
        //ImGui::PopFont();

        window.clear();

        // Window Draw

        ImGui::SFML::Render(window);
        
        window.display();
    }

    ImGui::SFML::Shutdown();
}