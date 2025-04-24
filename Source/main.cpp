#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_stdlib.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <StormLib.h>
#include <iostream>

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
    bool hasAbility = false;

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
    u8 attackDamageAffix1 = 0;
    i8 attackCost2 = 0;
    i16 attackDamage2 = 0;
    u8 attackDamageAffix2 = 0;
    i8 retreatCost = 0;

    u32 currentPreEvo = 0;
    u32 currentIllus = 0;
    u32 currentFlair = 0;
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

    std::vector<std::string> damageAffixes = { "", "×", "+", "-" };

    std::vector<u8> attack1CostType = { 0, 0, 0, 0, 0 };
    std::vector<u8> attack2CostType = { 0, 0, 0, 0, 0 };

    bool operator==(const Pokemon& other) const
    {
        return (currentType == other.currentType) && (currentStage == other.currentStage) && (currentRarity == other.currentRarity)
            && (currentFlair == other.currentFlair) && (currentIllus == other.currentIllus) && (isEX == other.isEX);
    }
};

template<typename T>
void Combo(std::string_view name, std::vector<std::string>& vector, T& index, ImGuiComboFlags flags = 0)
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

void BeginRow(std::string_view name)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", name.data());
    ImGui::TableSetColumnIndex(1);
}

template<typename T>
void Row(std::string_view name, T& value, i32 step = 1)
{
    BeginRow(name);
    ImGui::InputInt(("##" + std::string(name)).c_str(), &value, step);
}

template<typename T>
void Row(std::string_view name, std::vector<std::string>& vector, T& index)
{
    BeginRow(name);
    Combo("##" + std::string(name), vector, index);
}

void Row(std::string_view name, bool& value)
{
    BeginRow(name);
    ImGui::Checkbox(("##" + std::string(name)).c_str(), &value);
}

void Row(std::string_view name, std::string& value)
{
    BeginRow(name);
    ImGui::InputText(("##" + std::string(name)).c_str(), &value);
}

void DummyRow()
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Dummy({ 0.0f, 30.0f});
    ImGui::TableSetColumnIndex(1);
}

void CreateText(std::vector<sf::Text>& texts, sf::Font& font, std::string_view s, sf::Vector2f pos, u32 size = 18,
    sf::Color color = sf::Color::Black, bool doOutline = false, sf::Color outlineColor = sf::Color::White, bool centerOrigin = false)
{
    sf::Text t(font, std::string(s), size);

    if (centerOrigin)
        t.setOrigin({ t.getGlobalBounds().size.x / 2.0f, t.getGlobalBounds().size.y / 2.0f });

    t.setPosition(pos);
    t.setFillColor(color);

    if (doOutline)
    {
        t.setOutlineThickness(0.5f);
        t.setOutlineColor(outlineColor);
    }
    texts.push_back(t);
}

std::string GetWrappedText(std::string_view text, sf::Font& font, u32 characterSize, f32 maxWidth)
{
    std::istringstream words(text.data());
    std::string word;
    std::string line;
    std::string result;
    sf::Text sfText(font, text.data(), characterSize);

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

std::vector<std::string> GetRightAlignedWrappedLines(std::string_view text, sf::Font& font, u32 characterSize, f32 maxWidth)
{
    std::istringstream words(text.data());
    std::string word;
    std::string line;
    std::vector<std::string> result;
    sf::Text sfText(font, "", characterSize);

    while (words >> word)
    {
        std::string testLine = line + (line.empty() ? "" : " ") + word;
        sfText.setString(testLine);
        if (sfText.getLocalBounds().size.x > maxWidth)
        {
            result.push_back(line);
            line = word;
        }
        else
        {
            line = testLine;
        }
    }

    result.push_back(line);

    return result;
}

void CreateRightAlignedWrappedText(std::vector<sf::Text>& texts, std::vector<std::string>& lines, sf::Font& font, u32 fontSize, f32 rightEdge, f32 yPos, f32 lineHeight,
    sf::Color color, bool doOutline = false, sf::Color outline = sf::Color::White, bool centerOrigin = false)
{
    sf::Text temp(font, "", fontSize);

    for (size_t i = 0; i < lines.size(); ++i)
    {
        const std::string& line = lines[i];
        if (line.empty())
            continue;

        temp.setString(line);
        f32 lineWidth = temp.getLocalBounds().size.x;

        sf::Vector2f pos(rightEdge - lineWidth, yPos + i * lineHeight);
        CreateText(texts, font, line, pos, fontSize, color, doOutline, outline, centerOrigin);
    }
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

void SaveCard(sf::RenderTexture& texture, sf::Sprite* illus, sf::Sprite* nTemplate, sf::Sprite* nBorder, sf::Sprite* flair, std::vector<sf::Sprite> sprites, std::vector<sf::Text> texts, Pokemon mon)
{

    texture.clear(sf::Color::Transparent);

    sf::RenderStates states;
    states.blendMode = sf::BlendAlpha;

    sf::Vector2f cardOrigin = nTemplate->getPosition();
    sf::Transform offset;
    offset.translate({ -cardOrigin.x + 0.0f, -cardOrigin.y + 0.0f });
    states.transform = offset;

    if (illus)
    {
        texture.draw(*illus, states);
    }

    texture.draw(*nTemplate, states);

    if (nBorder)
    {
        texture.draw(*nBorder, states);
    }

    if (flair)
    {
        texture.draw(*flair, states);
    }

    for (auto& sprite : sprites)
    {
        texture.draw(sprite, states);
    }

    for (auto& text : texts)
    {
        texture.draw(text, states);
    }

    texture.display();

    sf::Image image = texture.getTexture().copyToImage();
    std::string fileName = mon.isEX ? mon.name + "_EX.png" : mon.name + ".png";

    std::string folderPath = "./My Cards/";
    if (!std::filesystem::exists(folderPath))
    {
        std::filesystem::create_directory(folderPath);
    }

    if (!image.saveToFile(folderPath + fileName)) {}
}

std::vector<std::string> GetFilesNamesInDirectory(std::string_view path)
{
    std::vector<std::string> fileNames = { "None" };
    const std::vector<std::string> validExtensions = { ".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tga" };

    if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (entry.is_regular_file())
            {
                std::string ext = entry.path().extension().string();

                if (std::find(validExtensions.begin(), validExtensions.end(), ext) != validExtensions.end())
                {
                    fileNames.push_back(entry.path().filename().string());
                }
            }
        }
    }

    return fileNames;
}

bool LoadFileFromMpq(HANDLE archive, const std::string& fileName, std::vector<u8>& outBuffer)
{
    HANDLE hFile;
    if (!SFileOpenFileEx(archive, fileName.c_str(), 0, &hFile))
        return false;

    DWORD fileSize = SFileGetFileSize(hFile, NULL);
    if (fileSize == SFILE_INVALID_SIZE)
    {
        SFileCloseFile(hFile);
        return false;
    }

    outBuffer.resize(fileSize);
    DWORD bytesRead;
    if (!SFileReadFile(hFile, outBuffer.data(), fileSize, &bytesRead, NULL))
    {
        SFileCloseFile(hFile);
        return false;
    }

    SFileCloseFile(hFile);
    return true;
}

sf::Texture* LoadTextureFromMpq(HANDLE archive, const std::string& path)
{
    std::vector<u8> buffer;
    if (!LoadFileFromMpq(archive, path, buffer))
    {
        return nullptr;
    }

    sf::Texture* texture = new sf::Texture();
    if (!texture->loadFromMemory(buffer.data(), buffer.size()))
    {
        delete texture;
        return nullptr;
    }

    return texture;
}

struct FontAsset
{
    sf::Font fontVar;
    std::vector<u8> buffer;
};

FontAsset LoadFontFromMpq(HANDLE archive, const std::string& path)
{
    FontAsset font;
    if (!LoadFileFromMpq(archive, path, font.buffer))
    {
        return {};
    }

    if (!font.fontVar.openFromMemory(font.buffer.data(), font.buffer.size()))
    {
        return {};
    }

    return font;
}

std::vector<sf::Texture*> LoadTextureFolderFromMpq(HANDLE archive, const std::string& folder)
{
    std::vector<std::string> matchingFiles;
    std::vector<sf::Texture*> textures;

    std::string searchPath = folder + "*";
    SFILE_FIND_DATA findData;
    HANDLE hFind = SFileFindFirstFile(archive, searchPath.c_str(), &findData, NULL);

    if (hFind)
    {
        do
        {
            std::string fileName = findData.cFileName;

            if (fileName.ends_with(".png"))
            {
                matchingFiles.push_back(fileName);
            }

        } while (SFileFindNextFile(hFind, &findData));

        SFileFindClose(hFind);
    }

    std::sort(matchingFiles.begin(), matchingFiles.end());

    for (const auto& fileName : matchingFiles)
    {
        std::vector<u8> buffer;

        if (LoadFileFromMpq(archive, fileName, buffer))
        {
            sf::Texture* tex = new sf::Texture();
            if (tex->loadFromMemory(buffer.data(), buffer.size()))
            {
                tex->setSmooth(true);
                textures.push_back(tex);
            }
            else
            {
                delete tex;
            }
        }
    }

    return textures;
}

std::vector<std::string> GetFileNamesInMpqDirectory(HANDLE archive, const std::string& folder)
{
    std::vector<std::string> fileNames = { "None" };
    const std::vector<std::string> validExtensions = { ".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tga" };

    std::string searchPath = folder + "*";
    HANDLE hFind = NULL;
    SFILE_FIND_DATA findData;

    hFind = SFileFindFirstFile(archive, searchPath.c_str(), &findData, NULL);

    if (hFind)
    {
        do
        {
            std::string filename = findData.cFileName;

            std::string ext = std::filesystem::path(filename).extension().string();
            if (std::find(validExtensions.begin(), validExtensions.end(), ext) != validExtensions.end())
            {
                fileNames.push_back(filename); // e.g. "my_icon.png"
            }
        } while (SFileFindNextFile(hFind, &findData));

        SFileFindClose(hFind);
    }

    return fileNames;
}

sf::Texture* LoadTemplate(HANDLE archive, Pokemon& mon)
{
    std::string path = "Templates\\";

    switch (mon.currentStage)
    {
    case BASIC: path += "Basic\\";
        break;
    case STAGE_1: path += "Stage1\\";
        break;
    case STAGE_2: path += "Stage2\\";
        break;
    default:
        break;
    }

    switch (mon.currentRarity)
    {
    case PROMO:
    case COMMON:
    case UNCOMMON:
    case RARE: path += "Basic\\";
        break;
    case DOUBLE_RARE: path += "EX\\";
        break;
    case ART_RARE: path += "FA\\";
        break;
    case SUPER_RARE: path += "EX FA\\";
        break;
    case SPECIAL_ART_RARE: path += "RR\\";
        break;
    case IMMERSIVE_RARE: path += "IMS\\";
        break;
    case CROWN_RARE: path += "CR\\";
        break;
    case SHINY_RARE: path += "SR\\";
        break;
    case D_SHINY_RARE: path += "EX SR\\";
        break;
    default:
        break;
    }

    path += mon.typeNames[mon.currentType] + ".png";

    printf("%s\n", path.c_str());
    sf::Texture* texture = LoadTextureFromMpq(archive, path);
    if (!texture)
    {
        delete texture;
        return nullptr;
    }

    return texture;
}

int main()
{
    std::string name = "Enigma's Card Lab ";
    std::string version = "v0.11.";
    std::string date = "250424a";

    ImVec2 windowSize = { 800, 600 };
    ImVec2 windowHalf = { 400, 300 };

    sf::RenderWindow window(sf::VideoMode(windowSize), name + version + date, sf::Style::Titlebar | sf::Style::Close);

    bool saveScreenshotNow = false;
    bool showSaveSuccessPopup = false;
    bool showSaveFailurePopup = false;

    if (!ImGui::SFML::Init(window)) {}

    sf::RenderTexture renderTexture({ 367, 512 });
    Pokemon mon;
    Pokemon tempMon;

    HANDLE hMpq = nullptr;
    if (!SFileOpenArchive(L"Data.mpq", 0, 0, &hMpq))
    {
        printf("Failed to open MPQ archive.\n");
        return false;
    }

    sf::Texture* bgTexture = LoadTextureFromMpq(hMpq, "Background\\background.png");
    sf::Sprite* bgSprite = new sf::Sprite(*bgTexture);
    bgSprite->setPosition({ 0, 0 });

    sf::Texture* githubTexture = LoadTextureFromMpq(hMpq, "Icons\\Links\\github.png");
    sf::Sprite* githubSprite = new sf::Sprite(*githubTexture);
    sf::Texture* discordTexture = LoadTextureFromMpq(hMpq, "Icons\\Links\\discord.png");
    sf::Sprite* discordSprite = new sf::Sprite(*discordTexture);
    sf::Texture* coffeeTexture = LoadTextureFromMpq(hMpq, "Icons\\Links\\coffee.png");
    sf::Sprite* coffeeSprite = new sf::Sprite(*coffeeTexture);

    std::string platesPath = "Icons\\Plates\\";
    sf::Texture* stage2CRPlateTexture = LoadTextureFromMpq(hMpq, platesPath + "stage2_cr_plate.png");
    sf::Sprite* stage2CRPlateSprite = new sf::Sprite(*stage2CRPlateTexture);
    sf::Texture* stage1CRPlateTexture = LoadTextureFromMpq(hMpq, platesPath + "stage1_cr_plate.png");
    sf::Sprite* stage1CRPlateSprite = new sf::Sprite(*stage1CRPlateTexture);
    sf::Texture* basicCRPlateTexture = LoadTextureFromMpq(hMpq, platesPath + "basic_cr_plate.png");
    sf::Sprite* basicCRPlateSprite = new sf::Sprite(*basicCRPlateTexture);

    sf::Texture* stage2PlateTexture = LoadTextureFromMpq(hMpq, platesPath + "stage2_plate.png");
    sf::Sprite* stage2PlateSprite = new sf::Sprite(*stage2PlateTexture);
    sf::Texture* stage1PlateTexture = LoadTextureFromMpq(hMpq, platesPath + "stage1_plate.png");
    sf::Sprite* stage1PlateSprite = new sf::Sprite(*stage1PlateTexture);
    sf::Texture* basicPlateTexture = LoadTextureFromMpq(hMpq, platesPath + "basic_plate.png");
    sf::Sprite* basicPlateSprite = new sf::Sprite(*basicPlateTexture);

    sf::Texture* exRuleTexture = LoadTextureFromMpq(hMpq, platesPath + "ex_rule.png");
    sf::Sprite* exRuleSprite = new sf::Sprite(*exRuleTexture);

    sf::Texture* weaknessCRPlateTexture = LoadTextureFromMpq(hMpq, platesPath + "weak_treat_cr_plate.png");
    sf::Sprite* weaknessCRPlateSprite = new sf::Sprite(*weaknessCRPlateTexture);
    sf::Texture* weaknessPlateTexture = LoadTextureFromMpq(hMpq, platesPath + "weak_treat_plate.png");
    sf::Sprite* weaknessPlateSprite = new sf::Sprite(*weaknessPlateTexture);

    sf::Sprite* preEvoSprite = nullptr;
    sf::Texture* preEvoTexture = nullptr;
    sf::Sprite* illusSprite = nullptr;
    sf::Texture* illusTexture = nullptr;
    sf::Sprite* flairSprite = nullptr;
    sf::Texture* flairTexture = nullptr;
    sf::Sprite* cardTemplate = nullptr;
    sf::Texture* cardTexture = nullptr;
    sf::Sprite* borderSprite = nullptr;
    sf::Sprite* typeSprite = nullptr;

    std::vector<std::string> customEvolvesFromFileNames = GetFilesNamesInDirectory("./Custom/Evolves_From/");
    std::vector<std::string> customIllustrationFileNames = GetFilesNamesInDirectory("./Custom/Illustrations/");
    std::vector<std::string> customFlairFileNames = GetFilesNamesInDirectory("./Custom/Flairs/");

    sf::Texture* abilityPlate = LoadTextureFromMpq(hMpq, "Icons\\Ability\\ability_plate.png");

    std::vector<sf::Texture*> borders = LoadTextureFolderFromMpq(hMpq, "Templates/Borders/");
    std::vector<sf::Texture*> rarityIcons = LoadTextureFolderFromMpq(hMpq, "Icons\\Rarity\\");
    std::vector<sf::Texture*> typeIcons = LoadTextureFolderFromMpq(hMpq, "Icons\\Types\\");
    std::vector<sf::Texture*> attackTypeIcons = LoadTextureFolderFromMpq(hMpq, "Icons\\AttackTypes\\");
    std::vector<sf::Texture*> exIcons = LoadTextureFolderFromMpq(hMpq, "Icons\\EX\\");
    std::vector<sf::Sprite> sprites;
    std::vector<sf::Sprite*> postFlairSprites;

    std::vector<sf::Text> texts;
    FontAsset fontItalic = LoadFontFromMpq(hMpq, "Fonts\\gs_italic.ttf");
    FontAsset fontLight = LoadFontFromMpq(hMpq, "Fonts\\gs_light.ttf");
    FontAsset fontBold = LoadFontFromMpq(hMpq, "Fonts\\gs_bold.ttf");
    FontAsset fontMTBoldCond = LoadFontFromMpq(hMpq, "Fonts\\gs_mt_bold_cond.ttf");
    FontAsset fontSemiBold = LoadFontFromMpq(hMpq, "Fonts\\gs_semi_bold.ttf");
    FontAsset fontReg = LoadFontFromMpq(hMpq, "Fonts\\gs.ttf");

    ImFontConfig fontCfg;
    fontCfg.FontDataOwnedByAtlas = false;
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    ImFont* imFont = io.Fonts->AddFontFromMemoryTTF(fontReg.buffer.data(), static_cast<i32>(fontReg.buffer.size()), 22.0f, &fontCfg, io.Fonts->GetGlyphRangesDefault());

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_PopupBg] = ImVec4(0.33f, 0.33f, 0.33f, 0.94f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.55f, 0.55f, 0.55f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(1.00f, 1.00f, 1.00f, 0.62f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    if (!ImGui::SFML::UpdateFontTexture()) {}

    ImGuiWindowFlags guiWindowFlags = {};
    guiWindowFlags |= ImGuiWindowFlags_NoResize;
    guiWindowFlags |= ImGuiWindowFlags_NoMove;
    guiWindowFlags |= ImGuiWindowFlags_NoBackground;
    guiWindowFlags |= ImGuiWindowFlags_NoTitleBar;
    guiWindowFlags |= ImGuiWindowFlags_NoResize;
    guiWindowFlags |= ImGuiWindowFlags_NoCollapse;

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

        // Card Value inputs
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushFont(imFont);
        ImGui::SetNextWindowPos(ImVec2{ windowHalf.x, 30.0f });
        ImGui::SetNextWindowSize({ windowHalf.x, windowSize.y });
        if (ImGui::Begin("Settings", nullptr, guiWindowFlags))
        {
            if (ImGui::BeginTable("##Table", 2))
            {
                ImGui::TableSetupColumn("##Names", ImGuiTableColumnFlags_WidthFixed, windowHalf.x * 0.45f);
                ImGui::TableSetupColumn("##Values", ImGuiTableColumnFlags_WidthFixed, windowHalf.x * 0.55f);

                Row("Name", mon.name);

                Row("Illustration", customIllustrationFileNames, mon.currentIllus);

                Row("Flair", customFlairFileNames, mon.currentFlair);

                Row("Type", mon.typeNames, reinterpret_cast<u8&>(mon.currentType));

                Row("Stage", mon.stageNames, reinterpret_cast<u8&>(mon.currentStage));
                
                if (mon.currentStage != BASIC)
                {
                    Row("Evolves From", mon.evolvesFrom);

                    Row("Pre-Evo Icon", customEvolvesFromFileNames, mon.currentPreEvo);
                }

                Row("Health", mon.health, 10);

                if (!mon.isEX)
                {
                    Row("Info", mon.basicInfo);
                }

                Row("Is Ex", mon.isEX);

                Row("Has Ability", mon.hasAbility);
                
                if (mon.hasAbility && mon.attackCount < 2)
                {
                    Row("Ability Name", mon.abilityName);
                    Row("Ability Effect", mon.abilityEffect);
                }

                Row("Attack Count", mon.attackCountNames, mon.attackCount);
                
                if (mon.attackCount >= 1)
                {
                    Row("Attack Cost 1", mon.attackCost1);

                    for (i32 i = 0; i < mon.attackCost1 && i < 5; i++)
                    {
                        Row("Cost 1 Type " + std::to_string(i + 1), mon.typeNames, mon.attack1CostType[i]);
                    }

                    Row("Attack 1 Name", mon.attackName1);
                    Row("Damage 1 Affix", mon.damageAffixes, mon.attackDamageAffix1);
                    Row("Attack 1 Damage", mon.attackDamage1, 10);
                    Row("Attack 1 Effect", mon.attackEffect1);
                }

                if (mon.attackCount == 2 && !mon.hasAbility)
                {
                    Row("Attack Cost 2", mon.attackCost2);

                    for (i32 i = 0; i < mon.attackCost2 && i < 5; i++)
                    {
                        Row("Cost 2 Type " + std::to_string(i + 1), mon.typeNames, mon.attack2CostType[i]);
                    }

                    Row("Attack 2 Name", mon.attackName2);
                    Row("Damage 2 Affix", mon.damageAffixes, mon.attackDamageAffix2);
                    Row("Attack 2 Damage", mon.attackDamage2, 10);
                    Row("Attack 2 Effect", mon.attackEffect2);
                }

                Row("Weakness", mon.typeNames, reinterpret_cast<u8&>(mon.currentWeakness));

                Row("Retreat Cost", mon.retreatCost);

                Row("Illustrator", mon.illustrator);

                Row("Rarity", mon.rarityNames, reinterpret_cast<u8&>(mon.currentRarity));

                if (!mon.isEX)
                {
                    Row("Flavor Text", mon.flavorText);
                }

                DummyRow();

                ImGui::EndTable();
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

        ImGui::SetNextWindowPos({ 0.0f, windowSize.y - 50.0f });
        ImGui::SetNextWindowSize({ windowHalf.x, 100.0f });
        if (ImGui::Begin("##Save", nullptr, guiWindowFlags))
        {
            ImGui::SetCursorPosX(153.5f);
            if (ImGui::Button("Save", { 100.0f, 30.0f }))
            {
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.1f));

                if (mon.name.empty() || !cardTemplate)
                {
                    showSaveFailurePopup = true;
                    ImGui::OpenPopup("Save Failed");
                }
                else
                {
                    showSaveSuccessPopup = true;
                    ImGui::OpenPopup("Save Successful");
                    saveScreenshotNow = true;
                }
            }
        }

        if (showSaveFailurePopup && ImGui::BeginPopupModal("Save Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextWrapped("Image not saved: name is blank or not enough options were picked.");
            ImGui::Separator();

            if (ImGui::Button("Okay", ImVec2(200, 0)))
            {
                showSaveFailurePopup = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (showSaveSuccessPopup && ImGui::BeginPopupModal("Save Successful", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Image saved successfully!");
            ImGui::Separator();

            if (ImGui::Button("Okay", ImVec2(200, 0)))
            {
                showSaveSuccessPopup = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::End();

        ImGui::SetNextWindowPos({ 10.0f, 550.0f });
        if (ImGui::Begin("##Links", nullptr, guiWindowFlags))
        {
            if (ImGui::ImageButton("Github Repo", *githubSprite, { 20.0f, 20.0f }))
            {
                system("start https://github.com/SoapyEnigma/CardLab");
            }

            ImGui::SameLine();
            if (ImGui::ImageButton("Discord Server", *discordSprite, { 20.0f, 20.0f }))
            {
                system("start https://discord.gg/kFZ7WjSZQT");
            }

            ImGui::SameLine();
            if (ImGui::ImageButton("Buy Me A Coffee", *coffeeSprite, { 20.0f, 20.0f }))
            {
                system("start https://buymeacoffee.com/soapyenigma");
            }
        }
        ImGui::End();

        ImGui::PopFont();
        ImGui::PopStyleColor();

        // Generate Base Template
        if (mon != tempMon)
        {
            delete cardTexture;
            cardTexture = nullptr;

            delete cardTemplate;
            cardTemplate = nullptr;

            delete borderSprite;
            borderSprite = nullptr;

            sf::Texture* tempTexture = LoadTemplate(hMpq, mon);
            if (tempTexture)
            {
                cardTexture = tempTexture;
                cardTemplate = new sf::Sprite(*cardTexture);

                cardTemplate->setPosition({ 20.0f, 40.0f });

                u8 rarity = mon.currentRarity == RARE ? 0 : mon.currentRarity;
                u8 index = mon.currentStage != BASIC ? rarity / 2 + 1 : rarity / 2;

                if (mon.currentRarity == RARE || rarity == DOUBLE_RARE || rarity == IMMERSIVE_RARE)
                {
                    borderSprite = new sf::Sprite(*borders[index]);
                    borderSprite->setPosition({ 20.0f, 40.0f });
                }
            }
            else
            {
                delete tempTexture;
            }
        }

        // Illustration
        if (mon.currentIllus > 0 && mon != tempMon)
        {
            if (sf::Texture* tempTexture = new sf::Texture("./Custom/Illustrations/" + customIllustrationFileNames[mon.currentIllus]))
            {
                delete illusTexture;
                illusTexture = tempTexture;

                delete illusSprite;
                illusSprite = new sf::Sprite(*illusTexture);

                if (mon.currentRarity < ART_RARE)
                {
                    illusSprite->setPosition({ 20.0f + 30.0f, 88.0f });
                }
                else
                {
                    illusSprite->setPosition({ 20.0f + 14.0f, 50.0f });
                }
            }
        }
        else if (mon.currentIllus == 0)
        {
            delete illusTexture;
            illusTexture = nullptr;

            delete illusSprite;
            illusSprite = nullptr;
        }

        // Flair
        if (!mon.currentFlair)
        {
            postFlairSprites.clear();
        }

        if (mon.currentFlair > 0 && mon != tempMon)
        {
            postFlairSprites.clear();

            if (sf::Texture* tempTexture = new sf::Texture("./Custom/Flairs/" + customFlairFileNames[mon.currentFlair]))
            {
                delete flairTexture;
                flairTexture = tempTexture;

                delete flairSprite;
                flairSprite = new sf::Sprite(*flairTexture);

                flairSprite->setPosition({ 20.0f, 40.0f });
            }

            sf::Sprite* typeSprite = new sf::Sprite(*attackTypeIcons[mon.currentType]);
            typeSprite->setPosition({ 20.0f + 321.0f, 55.0f });
            typeSprite->setScale({ 1.27f, 1.27f });
            postFlairSprites.push_back(typeSprite);

            if (mon.currentRarity != CROWN_RARE)
            {
                if (mon.currentStage == BASIC)
                {
                    basicPlateSprite->setPosition({ 20.0f + 9.0f, 60.0f });
                    postFlairSprites.push_back(basicPlateSprite);
                }

                if (mon.currentStage == STAGE_1)
                {
                    stage1PlateSprite->setPosition({ 20.0f + 9.0f, 60.0f });
                    postFlairSprites.push_back(stage1PlateSprite);
                }

                if (mon.currentStage == STAGE_2)
                {
                    stage2PlateSprite->setPosition({ 20.0f + 9.0f, 60.0f });
                    postFlairSprites.push_back(stage2PlateSprite);
                }

                weaknessPlateSprite->setPosition({ 20.0f + 26.0f, 479.0f });
                postFlairSprites.push_back(weaknessPlateSprite);
            }
            else if (mon.currentRarity == CROWN_RARE)
            {
                if (mon.currentStage == BASIC)
                {
                    basicCRPlateSprite->setPosition({ 20.0f + 9.0f, 60.0f });
                    postFlairSprites.push_back(basicCRPlateSprite);
                }

                if (mon.currentStage == STAGE_1)
                {
                    stage1CRPlateSprite->setPosition({ 20.0f + 9.0f, 60.0f });
                    postFlairSprites.push_back(stage1CRPlateSprite);
                }

                if (mon.currentStage == STAGE_2)
                {
                    stage2CRPlateSprite->setPosition({ 20.0f + 9.0f, 60.0f });
                    postFlairSprites.push_back(stage2CRPlateSprite);
                }

                weaknessCRPlateSprite->setPosition({ 20.0f + 26.0f, 479.0f });
                postFlairSprites.push_back(weaknessCRPlateSprite);
            }

            if (mon.isEX)
            {
                exRuleSprite->setPosition({ 20.0f + 119.0f, 504.0f });
                postFlairSprites.push_back(exRuleSprite);
            }
        }
        else if (mon.currentFlair == 0)
        {
            delete flairSprite;
            flairSprite = nullptr;

            delete flairTexture;
            flairTexture = nullptr;
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

            sf::Color textColor = mon.currentType == DARKNESS && mon.currentRarity < ART_RARE ? sf::Color::White : sf::Color::Black;
            bool doOutline = mon.currentRarity >= ART_RARE ? true : false;

            CreateText(texts, fontSemiBold.fontVar, mon.name, { cardPos.x + 66.0f, cardPos.y + 13.0f }, 26, textColor, doOutline);

            if (mon.health > 0 && mon.health < 100)
            {
                healthValuePos.x -= 14.0f;
            }
            else if (mon.health >= 100)
            {
                healthValuePos.x -= 28.0f;
            }

            if (mon.health > 0)
            {
                CreateText(texts, fontSemiBold.fontVar, std::to_string(mon.health), { healthValuePos }, 26, textColor, doOutline);
                CreateText(texts, fontMTBoldCond.fontVar, "HP", { healthValuePos.x - 14.0f, healthValuePos.y + 13.0f }, 12, textColor, doOutline);
            }

            if (mon.currentStage != BASIC)
            {
                CreateText(texts, fontReg.fontVar, "Evolves from " + mon.evolvesFrom, { cardPos.x + 64.0f, cardPos.y + 47.0f }, 8);
            }

            if (!mon.isEX && mon.currentRarity <= RARE)
            {
                CreateText(texts, fontReg.fontVar, mon.basicInfo, { cardPos.x + (cardSize.x / 2.0f), cardPos.y + 249.0f }, 8, sf::Color::Black, false, sf::Color::White, true);
            }

            if (mon.currentWeakness != NONE)
            {
                CreateText(texts, fontSemiBold.fontVar, "+20", { cardPos.x + 120.0f, cardPos.y + 438.0f }, 16);
            }

            if (!mon.illustrator.empty())
            {
                CreateText(texts, fontItalic.fontVar, "Illus. " + mon.illustrator, { cardPos.x + 27.0f, cardPos.y + 458.0f }, 9, textColor, doOutline);
            }

            std::vector<std::string> flavorLines = GetRightAlignedWrappedLines(mon.flavorText, fontReg.fontVar, 8, 155.0f);
            CreateRightAlignedWrappedText(texts, flavorLines, fontReg.fontVar, 8, cardPos.x + 340.0f, cardPos.y + 460.0f, 11.0f, textColor, doOutline);

            if (mon.hasAbility)
            {
                CreateText(texts, fontBold.fontVar, mon.abilityName, { cardPos.x + 112.0f, cardPos.y + 272.0f }, 18, sf::Color{ 164, 12, 19 }, mon.currentRarity > DOUBLE_RARE ? true : false);

                std::string text = GetWrappedText(mon.abilityEffect, fontReg.fontVar, 14, 320.0f);
                CreateText(texts, fontReg.fontVar, text, { cardPos.x + 26.0f, cardPos.y + 293.0f }, 14, textColor, doOutline);
            }

            sf::Text dummyAbilityEffectText(fontReg.fontVar, GetWrappedText(mon.abilityEffect, fontReg.fontVar, 14, 320.0f), 14);
            sf::Vector2f attackNamePos1 = { cardPos.x + 118.0f, mon.hasAbility ? cardPos.y + dummyAbilityEffectText.getLocalBounds().size.y : cardPos.y };

            if (mon.attackCount > 0)
            {
                if (mon.hasAbility)
                {
                    attackNamePos1.y += 328.0f;
                }
                else if (mon.attackCount == 2)
                {
                    attackNamePos1.y += 292.0f;
                }
                else
                {
                    if (!mon.attackEffect1.empty())
                    {
                        attackNamePos1.y += 314.0f;
                    }
                    else
                    {
                        attackNamePos1.y += 330.0f;
                    }
                }

                if (mon.attackCost1 == 5)
                {
                    attackNamePos1.x += 24.0f;
                }

                CreateText(texts, fontSemiBold.fontVar, mon.attackName1, { attackNamePos1 }, 18, textColor, doOutline);

                if (mon.attackDamage1 > 0)
                {
                    f32 xMod = mon.attackDamage1 >= 100 ? 312.0f : 322.0f;

                    CreateText(texts, fontSemiBold.fontVar, std::to_string(mon.attackDamage1), {cardPos.x + xMod, attackNamePos1.y}, 18, textColor, doOutline);

                    if (!mon.damageAffixes[mon.attackDamageAffix1].empty())
                    {
                        if (mon.damageAffixes[mon.attackDamageAffix1] == "-")
                        {
                            xMod += mon.attackDamage2 >= 100 ? 31.0f : 21.0f;
                            CreateText(texts, fontSemiBold.fontVar, mon.damageAffixes[mon.attackDamageAffix1], { cardPos.x + xMod , attackNamePos1.y }, 16, textColor, doOutline);
                        }
                        else
                        {
                            xMod += mon.attackDamage2 >= 100 ? 30.0f : 20.0f;
                            CreateText(texts, fontSemiBold.fontVar, mon.damageAffixes[mon.attackDamageAffix1], { cardPos.x + xMod , attackNamePos1.y + 5.0f }, 12, textColor, doOutline);
                        }
                    }
                }

                std::string text = GetWrappedText(mon.attackEffect1, fontReg.fontVar, 14, 320.0f);
                CreateText(texts, fontReg.fontVar, text, { cardPos.x + 26.0f, attackNamePos1.y + 24.0f }, 14, textColor, doOutline);
            }

            sf::Text dummyAttackEffect1Text(fontReg.fontVar, GetWrappedText(mon.attackEffect1, fontReg.fontVar, 14, 320.0f), 14);
            f32 attackInfoPosY = !mon.attackEffect1.empty() ? dummyAttackEffect1Text.getLocalBounds().size.y : 0.0f;
            sf::Vector2f attackNamePos2 = { cardPos.x + 118.0f, attackNamePos1.y + 50.0f + attackInfoPosY };

            if (mon.attackCount > 1 && !mon.hasAbility)
            {
                if (mon.attackCost2 == 5)
                {
                    attackNamePos2.x += 24.0f;
                }

                CreateText(texts, fontSemiBold.fontVar, mon.attackName2, { attackNamePos2 }, 18, textColor, doOutline);

                if (mon.attackDamage2 > 0)
                {
                    f32 xMod = mon.attackDamage2 >= 100 ? 312.0f : 322.0f;

                    CreateText(texts, fontSemiBold.fontVar, std::to_string(mon.attackDamage2), { cardPos.x + xMod, attackNamePos2.y }, 18, textColor, doOutline);

                    if (!mon.damageAffixes[mon.attackDamageAffix2].empty())
                    {
                        if (mon.damageAffixes[mon.attackDamageAffix2] == "-")
                        {
                            xMod += mon.attackDamage2 >= 100 ? 31.0f : 21.0f;
                            CreateText(texts, fontSemiBold.fontVar, mon.damageAffixes[mon.attackDamageAffix2], { cardPos.x + xMod , attackNamePos2.y }, 16, textColor, doOutline);
                        }
                        else
                        {
                            xMod += mon.attackDamage2 >= 100 ? 30.0f : 20.0f;
                            CreateText(texts, fontSemiBold.fontVar, mon.damageAffixes[mon.attackDamageAffix2], { cardPos.x + xMod , attackNamePos2.y + 5.0f }, 12, textColor, doOutline);
                        }
                    }
                }

                std::string text = GetWrappedText(mon.attackEffect2, fontReg.fontVar, 14, 320.0f);
                CreateText(texts, fontReg.fontVar, text, { cardPos.x + 26.0f, attackNamePos2.y + 24.0f }, 14, textColor, doOutline);
            }

            // Icons/Sprites
            if (mon.isEX && !mon.name.empty())
            {
                u8 index = mon.currentRarity >= ART_RARE ? 1 : 0;

                sf::Sprite sprite(*exIcons[index]);

                sf::Text dummyNameText(fontSemiBold.fontVar, mon.name, 28);
                
                f32 x = cardPos.x + 63.0f + dummyNameText.getGlobalBounds().size.length();
                f32 y = index ? cardPos.y + 20.0f : cardPos.y + 22.0f; // index 1 EX sprite is slightly bigger
                sprite.setPosition({ x, y });

                sprites.push_back(sprite);
            }

            if (mon.currentStage != BASIC && mon.currentPreEvo > 0)
            {
                sf::Texture* tempTexture = new sf::Texture();
                if (tempTexture->loadFromFile("./Custom/Evolves_From/" + customEvolvesFromFileNames[mon.currentPreEvo]))
                {
                    delete preEvoTexture;
                    preEvoTexture = tempTexture;

                    delete preEvoSprite;
                    preEvoSprite = new sf::Sprite(*preEvoTexture);
                    preEvoSprite->setOrigin({ preEvoTexture->getSize().x / 2.0f, preEvoTexture->getSize().y / 2.0f });
                    preEvoSprite->setPosition({ 20.0f + 36.0f, 40.0f + 58.0f });

                    sprites.push_back(*preEvoSprite);
                }
                else
                {
                    delete tempTexture;
                }
            }

            if (mon.hasAbility)
            {
                sf::Sprite sprite(*abilityPlate);
                sprite.setPosition({ cardPos.x + 26.0f, cardPos.y + 275.0f });

                sprites.push_back(sprite);
            }

            if (mon.attackCount > 0)
            {
                for (u8 i = 0; i < mon.attackCost1; i++)
                {
                    sf::Sprite sprite(*attackTypeIcons[mon.attack1CostType[i]]);
                    sprite.setPosition({ cardPos.x + 24.0f + (i * 22.0f), attackNamePos1.y + 2.0f });

                    sprites.push_back(sprite);
                }
            }

            if (mon.attackCount > 1 && !mon.hasAbility)
            {
                for (u8 i = 0; i < mon.attackCost2; i++)
                {
                    sf::Sprite sprite(*attackTypeIcons[mon.attack2CostType[i]]);
                    sprite.setPosition({ cardPos.x + 24.0f + (i * 22.0f), attackNamePos2.y + 2.0f });

                    sprites.push_back(sprite);
                }
            }

            if (mon.currentWeakness != NONE)
            {
                sf::Sprite sprite(*typeIcons[mon.currentWeakness]);
                sprite.setPosition({ cardPos.x + 104.0f, cardPos.y + 441.0f });

                sprites.push_back(sprite);
            }

            for (u8 i = 0; i < mon.retreatCost; i++)
            {
                sf::Sprite sprite(*typeIcons[COLORLESS]);
                sprite.setPosition({ cardPos.x + 250.0f + (i * 15.0f), cardPos.y + 441.0f });

                sprites.push_back(sprite);
            }

            sf::Vector2f rarityPos = { cardPos.x + 27.0f, cardPos.y + 470.0f };
            if (mon.currentRarity <= DOUBLE_RARE)
            {
                for (u8 i = 0; i < mon.currentRarity; i++)
                {
                    sf::Sprite sprite(*rarityIcons[0]);
                    sprite.setPosition({ rarityPos.x + (i * 15.0f), rarityPos.y });

                    sprites.push_back(sprite);
                }
            }
            else if (mon.currentRarity >= ART_RARE && mon.currentRarity <= IMMERSIVE_RARE)
            {
                u8 count = mon.currentRarity > SUPER_RARE ? mon.currentRarity - 5 : mon.currentRarity - 4;

                for (u8 i = 0; i < count; i++)
                {
                    sf::Sprite sprite(*rarityIcons[1]);
                    sprite.setPosition({ rarityPos.x + (i * 15.0f), rarityPos.y });

                    sprites.push_back(sprite);
                }
            }
            else if (mon.currentRarity == CROWN_RARE)
            {
                sf::Sprite sprite(*rarityIcons[2]);
                sprite.setPosition({ rarityPos });

                sprites.push_back(sprite);
            }
            else if (mon.currentRarity >= SHINY_RARE)
            {
                for (u8 i = 0; i < mon.currentRarity - 9; i++)
                {
                    sf::Sprite sprite(*rarityIcons[3]);
                    sprite.setPosition({ rarityPos.x + (i * 15.0f), rarityPos.y });

                    sprites.push_back(sprite);
                }
            }
        }

        window.clear();

        if (saveScreenshotNow)
        {
            SaveCard(renderTexture, illusSprite, cardTemplate, borderSprite, flairSprite, sprites, texts, mon);
            saveScreenshotNow = false;
        }

        window.draw(*bgSprite);

        if (cardTemplate)
        {
            if (illusSprite)
            {
                window.draw(*illusSprite);
            }

            window.draw(*cardTemplate);

            if (borderSprite)
            {
                window.draw(*borderSprite);
            }

            if (flairSprite)
            {
                window.draw(*flairSprite);
            }

            for (auto& sprite : postFlairSprites)
            {
                window.draw(*sprite);
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

    return 0;
}