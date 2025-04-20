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

    u8 attackCount = 0;
    i8 attackCost1 = 0;
    i16 attackDamage1 = 0;
    i8 attackCost2 = 0;
    i16 attackDamage2 = 0;
    i8 retreatCost = 0;

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

    std::vector<u8> attack1CostType = { 0, 0, 0, 0, 0 };
    std::vector<u8> attack2CostType = { 0, 0, 0, 0, 0 };

    bool operator==(const Pokemon& other) const
    {
        return (currentType == other.currentType) && (currentStage == other.currentStage) && (currentRarity == other.currentRarity);
    }
};

sf::Texture* LoadTemplate(Pokemon& mon)
{
    std::string path = "./Assets/Templates/";

    switch (mon.currentStage)
    {
    case BASIC: path += "Basic/";
        break;
    case STAGE_1: path += "Stage1/";
        break;
    case STAGE_2: path += "Stage2/";
        break;
    default:
        break;
    }

    switch (mon.currentRarity)
    {
    case PROMO:
    case COMMON:
    case UNCOMMON:
    case RARE: path += "Basic/";
        break;
    case DOUBLE_RARE: path += "EX/";
        break;
    case ART_RARE: path += "FA/";
        break;
    case SUPER_RARE: path += "EX FA/";
        break;
    case SPECIAL_ART_RARE: path += "RR/";
        break;
    case IMMERSIVE_RARE: path += "IMS/";
        break;
    case CROWN_RARE: path += "CR/";
        break;
    case SHINY_RARE:
    case D_SHINY_RARE:
    default:
        break;
    }

    path += mon.typeNames[mon.currentType] + ".png";

    if (!std::filesystem::exists(path))
    {
        return nullptr;
    }

    sf::Texture* texture = new sf::Texture;
    if (!texture->loadFromFile(path))
    {
        delete texture;
        return nullptr;
    }

    return texture;
}

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
void Row(std::string_view name, T& value, f32 dummyWidth, i32 step = 1)
{
    BeginRow(name, dummyWidth);
    ImGui::InputInt(("##" + std::string(name)).c_str(), &value, step);
}

void Row(std::string_view name, std::vector<std::string>& vector, u8& index, f32 dummyWidth)
{
    BeginRow(name, dummyWidth);
    Combo("##" + std::string(name), vector, index);
}

void Row(std::string_view name, bool& value, f32 dummyWidth)
{
    BeginRow(name, dummyWidth);
    ImGui::Checkbox(("##" + std::string(name)).c_str(), &value);
}

void Row(std::string_view name, std::string& value, f32 dummyWidth)
{
    BeginRow(name, dummyWidth);
    ImGui::InputText(("##" + std::string(name)).c_str(), &value);
}

void CreateText(std::vector<sf::Text>& texts, sf::Font& font, std::string_view s, sf::Vector2f pos, u32 size = 18, sf::Color color = sf::Color::Black, bool centerOrigin = false)
{
    sf::Text t(font, std::string(s), size);

    if (centerOrigin)
        t.setOrigin({ t.getGlobalBounds().size.x / 2.0f, t.getGlobalBounds().size.y / 2.0f });

    t.setPosition(pos);
    t.setFillColor(color);

    texts.push_back(t);
}

std::string GetWrappedText(const std::string& text, sf::Font& font, unsigned int characterSize, float maxWidth)
{
    std::istringstream words(text);
    std::string word, line, result;
    sf::Text sfText(font, text, characterSize);

    while (words >> word)
    {
        std::string testLine = line + (line.empty() ? "" : " ") + word;
        sfText.setString(testLine);
        if (sfText.getLocalBounds().size.x > maxWidth)
        {
            result += line + "\n";
            line = word;
        }
        else
        {
            line = testLine;
        }
    }
    result += line;
    return result;
}

std::vector<sf::Texture> LoadAssets(std::string_view s)
{
    std::vector<sf::Texture> textures;

    for (const auto& entry : std::filesystem::directory_iterator(s))
    {
        if (entry.path().extension() == ".png")
        {
            sf::Texture texture;
            if (texture.loadFromFile(entry.path().string()))
            {
                texture.setSmooth(true);
                textures.push_back(texture);
            }
        }
    }

    return textures;
}

int main()
{
    std::string name = "Enigma's Card Lab ";
    std::string version = "v0.01.";
    std::string date = "250420a";

    ImVec2 windowSize = { 1280, 720 };
    sf::ContextSettings contextSettings;
    contextSettings.antiAliasingLevel = 0;

    sf::RenderWindow window(sf::VideoMode(windowSize), name + version + date, sf::State::Windowed, contextSettings);

    ImGui::SFML::Init(window);

    Pokemon mon;
    Pokemon tempMon;

    sf::Sprite* cardTemplate = nullptr;
    sf::Texture* cardTexture = nullptr;
    sf::Sprite* borderSprite = nullptr;

    f32 rowWidth = 100.0f;

    sf::Texture abilityPlate("./Assets/Icons/Ability/ability_plate.png");

    std::vector<sf::Texture> borders = LoadAssets("./Assets/Templates/Borders/");
    std::vector<sf::Texture> rarityIcons = LoadAssets("./Assets/Icons/Rarity/");
    std::vector<sf::Texture> typeIcons = LoadAssets("./Assets/Icons/Types/");
    std::vector<sf::Sprite> sprites;

    std::vector<sf::Text> texts;
    sf::Font fontBoldCon("Assets/Fonts/gs_bold_cond.ttf");
    sf::Font fontBold("Assets/Fonts/gs_bold.ttf");
    sf::Font fontReg("Assets/Fonts/gs.ttf");

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    ImFont* imFont = io.Fonts->AddFontFromFileTTF("./Assets/Fonts/gs.ttf", 22, NULL, io.Fonts->GetGlyphRangesDefault());

    ImGui::SFML::UpdateFontTexture();

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

                if (cardTemplate)
                    cardTemplate->setPosition({ windowSize.x / 2.0f, 40.0f });
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Card Value inputs
        ImGui::PushFont(imFont);
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
                if (mon.currentStage != BASIC)
                    Row("Evolves From", mon.evolvesFrom, rowWidth);

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

                    for (i32 i = 0; i < mon.attackCost1 && i < 5; i++)
                    {
                        Row("Cost 1 Type " + std::to_string(i + 1), mon.typeNames, mon.attack1CostType[i], rowWidth);
                    }

                    Row("Attack 1 Name", mon.attackName1, rowWidth);
                    Row("Attack 1 Damage", mon.attackDamage1, rowWidth, 10);
                    Row("Attack 1 Effect", mon.attackEffect1, rowWidth);
                }

                if (mon.attackCount == 2)
                {
                    Row("Attack Cost 2", mon.attackCost2, rowWidth);

                    for (i32 i = 0; i < mon.attackCost2 && i < 5; i++)
                    {
                        Row("Cost 2 Type " + std::to_string(i + 1), mon.typeNames, mon.attack2CostType[i], rowWidth);
                    }

                    Row("Attack 2 Name", mon.attackName2, rowWidth);
                    Row("Attack 2 Damage", mon.attackDamage2, rowWidth, 10);
                    Row("Attack 2 Effect", mon.attackEffect2, rowWidth);
                }

                Row("Weakness", mon.typeNames, reinterpret_cast<u8&>(mon.currentWeakness), rowWidth);

                Row("Retreat Cost", mon.retreatCost, rowWidth);

                Row("Illustrator", mon.illustrator, rowWidth);

                Row("Rarity", mon.rarityNames, reinterpret_cast<u8&>(mon.currentRarity), rowWidth);

                if (!mon.isEX)
                    Row("Flavor Text", mon.flavorText, rowWidth);

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

                if (mon.attackCost1 > 5)
                    mon.attackCost1 = 0;
                else if (mon.attackCost1 < 0)
                    mon.attackCost1 = 5;

                if (mon.attackDamage1 > 500)
                    mon.attackDamage1 = 0;
                else if (mon.attackDamage1 < 0)
                    mon.attackDamage1 = 500;

                if (mon.attackCost2 > 5)
                    mon.attackCost2 = 0;
                else if (mon.attackCost2 < 0)
                    mon.attackCost2 = 5;

                if (mon.attackDamage2 > 500)
                    mon.attackDamage2 = 0;
                else if (mon.attackDamage2 < 0)
                    mon.attackDamage2 = 500;

                if (mon.retreatCost > 6)
                    mon.retreatCost = 0;
                else if (mon.retreatCost < 0)
                    mon.retreatCost = 6;
            }
        }
        ImGui::End();
        ImGui::PopFont();

        // Generate Base Template
        if (mon != tempMon)
        {
            delete cardTexture;
            cardTexture = nullptr;

            delete cardTemplate;
            cardTemplate = nullptr;

            delete borderSprite;
            borderSprite = nullptr;

            if (sf::Texture* tempTexture = LoadTemplate(mon))
            {
                cardTexture = tempTexture;
                cardTemplate = new sf::Sprite(*cardTexture);

                cardTemplate->setPosition({ windowSize.x / 2.0f, 40.0f });

                u8 rarity = mon.currentRarity == RARE ? 0 : mon.currentRarity;
                u8 index = mon.currentStage != BASIC ? rarity / 2 + 1 : rarity / 2;

                if (mon.currentRarity == RARE || rarity == DOUBLE_RARE || rarity == IMMERSIVE_RARE)
                {
                    borderSprite = new sf::Sprite(borders[index]);
                    borderSprite->setPosition({ windowSize.x / 2.0f, 40.0f });
                }
            }
        }

        tempMon = mon;
        texts.clear();
        sprites.clear();

        // Card Visual Structure
        if (cardTemplate)
        {
            // Texts
            sf::Vector2f cardSize = { cardTemplate->getGlobalBounds().size.x, cardTemplate->getLocalBounds().size.y };
            sf::Vector2f cardPos = cardTemplate->getPosition();
            sf::Vector2f healthValuePos = { cardPos.x + 305.0f, cardPos.y + 13.0f };

            sf::Color TextColor = mon.currentType == DARKNESS && mon.currentRarity < ART_RARE ? sf::Color::White : sf::Color::Black;

            CreateText(texts, fontBold, mon.name, { cardPos.x + 66.0f, cardPos.y + 13.0f }, 26, TextColor);

            if (mon.health > 0 && mon.health < 100)
                healthValuePos.x -= 14.0f;

            if (mon.health >= 100)
                healthValuePos.x -= 28.0f;

            if (mon.health > 0)
            {
                CreateText(texts, fontBold, std::to_string(mon.health), { healthValuePos }, 26, TextColor);
                CreateText(texts, fontBoldCon, "HP", { healthValuePos.x - 14.0f, healthValuePos.y + 13.0f }, 12, TextColor);
            }

            if (mon.currentStage != BASIC)
                CreateText(texts, fontReg, "Evolves from " + mon.evolvesFrom, { cardPos.x + 64.0f, cardPos.y + 47.0f }, 8);

            CreateText(texts, fontReg, mon.basicInfo, { cardPos.x + (cardSize.x / 2.0f), cardPos.y + 250.0f }, 8, sf::Color::Black, true);

            if (mon.currentWeakness != NONE)
                CreateText(texts, fontBoldCon, "+20", { cardPos.x + 120.0f, cardPos.y + 438.0f }, 16);

            CreateText(texts, fontReg, "Illus. " + mon.illustrator, { cardPos.x + 27.0f, cardPos.y + 458.0f }, 9, TextColor);

            CreateText(texts, fontReg, mon.flavorText, { cardPos.x + (cardSize.x / 4.0f), cardPos.y }, 18, TextColor); // Needs wrap text box

            if (mon.hasAbility)
            {
                CreateText(texts, fontBold, mon.abilityName, { cardPos.x + 112.0f, cardPos.y + 272.0f }, 18, sf::Color{ 164, 12, 19 });

                std::string text = GetWrappedText(mon.abilityEffect, fontReg, 14, 320.0f);
                CreateText(texts, fontReg, text, { cardPos.x + 26.0f, cardPos.y + 293.0f }, 14, TextColor);
            }

            std::string dummyString = GetWrappedText(mon.abilityEffect, fontReg, 14, 320.0f);
            sf::Text dummyText(fontReg, dummyString, 14);
            f32 dummyHeight = dummyText.getLocalBounds().size.y;

            sf::Vector2f attackNamePos = { cardPos.x + 112.0f, cardPos.y + dummyHeight };
            if (mon.attackCount > 0)
            {
                if (mon.attackCount == 2 && mon.hasAbility)
                    attackNamePos.y += 330.0f; // Re-align
                else if (mon.attackCount == 2 && !mon.hasAbility)
                    attackNamePos.y += 330.0f; // Re-align
                else if (mon.hasAbility)
                    attackNamePos.y += 330.0f; // Re-align
                else
                    attackNamePos.y += 330.0f; // Re-align

                if (mon.attackCost1 == 5)
                    attackNamePos.x += 26.0f;

                CreateText(texts, fontBold, mon.attackName1, { attackNamePos }, 18, TextColor);

                if (mon.attackDamage1 > 0)
                {
                    f32 xMod = mon.attackDamage1 >= 100 ? 312.0f : 322.0f;

                    CreateText(texts, fontBold, std::to_string(mon.attackDamage1), { cardPos.x + xMod, attackNamePos.y }, 18, TextColor);
                }

                CreateText(texts, fontReg, mon.attackEffect1, { 0, 0 }, 18, TextColor);
            }

            //// Need to do this section
            if (mon.attackCount > 1)
            {
                sf::Vector2f attackNamePos = { 0, 0 };
                if (mon.hasAbility)
                    attackNamePos = { cardPos.x + 97.0f, cardPos.y + 336.0f };
                else
                    attackNamePos = { cardPos.x + 97.0f, cardPos.y + 336.0f };

                CreateText(texts, fontBold, mon.attackName2, { attackNamePos }, 18, TextColor);

                if (mon.attackDamage2 > 0)
                    CreateText(texts, fontBold, std::to_string(mon.attackDamage2), { 0, 0 }, 18, TextColor);

                CreateText(texts, fontReg, mon.attackEffect2, { 0, 0 }, 18, TextColor);
            }
            ////

            // Icons/Sprites
            if (mon.hasAbility)
            {
                sf::Sprite sprite(abilityPlate);
                sprite.setPosition({ cardPos.x + 26.0f, cardPos.y + 275.0f });

                sprites.push_back(sprite);
            }

            if (mon.attackCount > 0)
            {
                for (u8 i = 0; i < mon.attackCost1; i++)
                {
                    sf::Sprite sprite(typeIcons[mon.attack1CostType[i]]);
                    sprite.setPosition({ cardPos.x + 24.0f + (i * 22.0f), attackNamePos.y + 2.0f });
                    sprite.setScale({ 1.3f, 1.3f });

                    sprites.push_back(sprite);
                }
            }

            if (mon.currentWeakness != NONE)
            {
                sf::Sprite sprite(typeIcons[mon.currentWeakness]);
                sprite.setPosition({ cardPos.x + 104.0f, cardPos.y + 441.0f });

                sprites.push_back(sprite);
            }

            for (u8 i = 0; i < mon.retreatCost; i++)
            {
                sf::Sprite sprite(typeIcons[COLORLESS]);
                sprite.setPosition({ cardPos.x + 250.0f + (i * 15.0f), cardPos.y + 441.0f });

                sprites.push_back(sprite);
            }

            sf::Vector2f rarityPos = { cardPos.x + 27.0f, cardPos.y + 470.0f };
            if (mon.currentRarity <= DOUBLE_RARE)
            {
                for (u8 i = 0; i < mon.currentRarity; i++)
                {
                    sf::Sprite sprite(rarityIcons[0]);
                    sprite.setPosition({ rarityPos.x + (i * 15.0f), rarityPos.y });

                    sprites.push_back(sprite);
                }
            }
            else if (mon.currentRarity >= ART_RARE && mon.currentRarity <= IMMERSIVE_RARE)
            {
                u8 count = mon.currentRarity == ART_RARE || mon.currentRarity == SUPER_RARE ? 1 : 2;

                for (u8 i = 0; i < count; i++)
                {
                    sf::Sprite sprite(rarityIcons[1]);
                    sprite.setPosition({ rarityPos.x + (i * 15.0f), rarityPos.y });

                    sprites.push_back(sprite);
                }
            }
            else if (mon.currentRarity == CROWN_RARE)
            {
                sf::Sprite sprite(rarityIcons[2]);
                sprite.setPosition({ rarityPos });

                sprites.push_back(sprite);
            }
            else if (mon.currentRarity >= SHINY_RARE)
            {
                for (u8 i = 0; i < SHINY_RARE - 9; i++)
                {
                    sf::Sprite sprite(rarityIcons[3]);
                    sprite.setPosition({ rarityPos.x + (i * 15.0f), rarityPos.y });

                    sprites.push_back(sprite);
                }
            }
        }

        window.clear();

        if (cardTemplate)
        {
            window.draw(*cardTemplate);

            if (borderSprite)
            {
                window.draw(*borderSprite);
            }

            for (auto& sprite : sprites)
            {
                window.draw(sprite);
            }

            for (auto& text : texts)
            {
                window.draw(text);
            }
        }

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();

    delete cardTemplate;
    delete cardTexture;

    return 0;
}