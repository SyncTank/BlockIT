#include "List.h"

namespace App 
{
    ImGuiTabBarFlags parentFlags =
        ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_Borders;

    ImGuiTabBarFlags tableFlags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;

    ImGuiInputFlags route_options = ImGuiInputFlags_Repeat;
    ImGuiInputFlags route_type = ImGuiInputFlags_RouteFocused;

    ImGuiInputFlags flags = route_type | route_options;

    std::unordered_map<std::wstring, std::vector<DWORD>> totalProcessList;
    
    // Used as current contextbuffer for data send off
    static char str1[9] = "";
    static char str2[9] = "";
    static char str3[16] = "";
    static char str4[9] = "";
    static int item_current_2 = 0;

    std::unordered_map<std::wstring, std::vector<DWORD>> blackList;

    dataBuffer presets[5];
    static bool isRunning = false;

    // Used for fileSaving
    std::string configPath;
    std::string filePath;

    void init()
    {
        Core::ProcessList(totalProcessList);

        if (App::folderSetup()) // Startups default to first tab
        {
            std::cerr << "Shit dead. \n" << std::endl;
            return;
        }

        // Load in the reference fields for the first tab into the buffer
        loadBuffer(0);
    }

    // checks if directory exists if not make it, sets file path and inital data map
    bool folderSetup() 
    {
        char* homeDir = nullptr;
        //size_t len = 0;
        errno_t err = _dupenv_s(&homeDir, 0, "HOME");
        
        if (err != 0 || homeDir == nullptr) {
        #ifdef _WIN32
            err = _dupenv_s(&homeDir, 0, "LOCALAPPDATA"); // Use LOCALAPPDATA on Windows
        #endif
            if (err != 0 || homeDir == nullptr) {
                std::cerr << "HOME or LOCALAPPDATA environment variable not found." << std::endl;
                return 1;
            }
        }

        configPath = std::string(homeDir) + "\\BlockIT";
        filePath = configPath + "\\settings.json";

        // Create the directory if it doesn't exist
        if (_mkdir(configPath.c_str()) && errno != EEXIST) {
            std::cerr << "Failed to create directory: " << strerror_s(homeDir,0,errno) << std::endl;
            free(homeDir); // Free the allocated memory
            return 1;
        }

        free(homeDir); // Free the allocated memory when done

        // buffer that reads settings to load up
        char inStreamBuffer[4096]; // size of struct + alignment * 5, plus addition char in writeAppFileOut 
        std::ifstream inFile(filePath);
        if (inFile.is_open())
        {
            inFile.read(inStreamBuffer, 4096);
            inFile.close();
        }   

        readAppFileIn(inStreamBuffer);

        return 0;
    }

    void clearBuffer()
    {
        strncpy_s(str1, "", 0);
        strncpy_s(str2, "", 0);
        strncpy_s(str3, "", 0);
        strncpy_s(str4, "", 0);
        item_current_2 = 0;
        blackList.clear();
    }

    void loadBuffer(int bufferData)
    {
        strncpy_s(str1, presets[bufferData].str1, 9);
        strncpy_s(str2, presets[bufferData].str2, 9);
        strncpy_s(str3, presets[bufferData].str3, 16);
        strncpy_s(str4, presets[bufferData].str4, 9);
        item_current_2 = presets[bufferData].item_current_2;
        blackList = presets[bufferData].list;
    }

    // Write out all Buffers out to filePath (Updates the saveSet data thats currently in buffer)
    void writeAppFileOut(int saveSets)
    {
        // Saving the current contextBuffer into the state's preset
        strncpy_s(presets[saveSets].str1, str1, 9);
        strncpy_s(presets[saveSets].str2, str2, 9);
        strncpy_s(presets[saveSets].str3, str3, 16);
        strncpy_s(presets[saveSets].str4, str4, 9);
        presets[saveSets].item_current_2 = item_current_2;
        presets[saveSets].list = blackList;

        std::wofstream outFile(filePath);
        if (outFile.is_open()) 
        {
            for (int i = 0; i < 5; ++i)
            {
                outFile << i << '{' << '\n';
                outFile << "str1" << ':' << presets[i].str1 << '\n';
                outFile << "str2" << ':' << presets[i].str2 << '\n';
                outFile << "str3" << ':' << presets[i].str3 << '\n';
                outFile << "str4" << ':' << presets[i].str4 << '\n';
                outFile << "Item_current_2" << ':' << presets[i].item_current_2 << '\n';
                outFile << "Processes" << ':';
                for (const auto& [key, value] : presets[i].list)
                {
                    outFile << key << ',';
                }
                outFile << '\n';
            }
            outFile.close();
        }
        else 
        {
            std::cerr << "Failed to open file for writing." << std::endl;
        }
    }

    // Load the variables from the file into the application's presets ran in startup
    void readAppFileIn(const char* inStreamBuffer)
    {
        size_t len = std::strlen(inStreamBuffer);
        std::string strBuffer = "";
        int presetSets = 0;
        bool captureFlag = 0;
        int fieldCheck = 0;
        for (size_t i = 0; i <= len; i++)
        {
            auto testChar = inStreamBuffer[i];

            if (testChar == '\n' and fieldCheck != 5)
            {
                captureFlag = false;
                continue;
            }

            if (testChar == '{')
            {
                fieldCheck = 0;
                presetSets = inStreamBuffer[i-1] - '0'; // Asi conversion to digit/int
            }

            if (captureFlag && fieldCheck < 5)
            {
                strBuffer += testChar;
            }

            if (fieldCheck == 5 && testChar == '\n')
            {
                presets[presetSets].item_current_2 = inStreamBuffer[i - 1] - '0';
                continue;
            }

            if (fieldCheck == 6 && captureFlag && testChar != ',')
            {
                strBuffer += testChar;
            }

            if (testChar == ',')
            {
                presets[presetSets].list[stringToWstring(strBuffer)];
                strBuffer = "";
            }

            if (testChar == ':')
            {
                if (strBuffer != "")
                {
                    switch (fieldCheck) {
                        case 1:
                            strncpy_s(presets[presetSets].str1, strBuffer.c_str(), strBuffer.length());
                            break;
                        case 2:
                            strncpy_s(presets[presetSets].str2, strBuffer.c_str(), strBuffer.length());
                            break;
                        case 3:
                            strncpy_s(presets[presetSets].str3, strBuffer.c_str(), strBuffer.length());
                            break;
                        case 4:
                            strncpy_s(presets[presetSets].str4, strBuffer.c_str(), strBuffer.length());
                            break;
                    }
                }

                strBuffer = "";
                fieldCheck++;
                captureFlag = true;
            }

        }
    }

    void convertWStringToCString(const std::wstring& wstr, char* cstr, size_t cstrSize) {
        size_t convertedChars = 0;
        errno_t err = wcstombs_s(&convertedChars, cstr, cstrSize, wstr.c_str(), _TRUNCATE);
        if (err != 0) {
            std::cerr << "Conversion error occurred." << std::endl;
        }
    }

    std::wstring convertToWString(const char* charArray) 
    {
        // Initialize the length and convertedChars variables
        size_t length = 0;
        size_t convertedChars = 0;

        // First, determine the required buffer size
        mbstowcs_s(&convertedChars, nullptr, 0, charArray, _TRUNCATE);
        length = convertedChars + 1; // Add one for the null terminator

        std::wstring wstr(length, L'\0'); // Create a wide string with the required length

        // Perform the actual conversion
        mbstowcs_s(&convertedChars, &wstr[0], length, charArray, _TRUNCATE);

        // Resize to remove the null terminator
        wstr.resize(convertedChars - 1);

        return wstr;
    }

    // Function to convert std::string to std::wstring
    std::wstring stringToWstring(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(str);
    }

    // Name slice for filePath
    std::wstring sliceName(const char* name)
    {
        size_t sizeName = std::strlen(name);
        size_t stop = 0;
        std::wstring newName = L"";

        for (size_t i = sizeName; i > 0; i--)
        {
            if (name[i] == '\\' )
            {
                stop = i;
                break;
            }
        }

        for (size_t i = stop + 1; i < sizeName; i++)
        {
            newName += name[i];
        }

        return newName;
    }

    // Helper text functions
    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    void renderWindowContext()
    {
        // # TODO add logic to get Process id from name process

        static int statusFlag;

        ImGui::SeparatorText("Block Now");
        ImGui::Spacing();
        {
            HelpMarker("This Countdown Timer Set in Minutes.");
            ImGui::SameLine();
            ImGui::PushItemWidth(75);
            
            ImGui::InputTextWithHint("Hour(s)", "H", str1, IM_ARRAYSIZE(str1), 0, 0, 0);


            ImGui::SameLine();
            ImGui::InputTextWithHint("Minute(s)", "Mins", str2, IM_ARRAYSIZE(str2));
            ImGui::Spacing();

            if (ImGui::Button("Block"))
            {

            }
            
            ImGui::SameLine();
            if (ImGui::Button("Pause"))
            {

            }
            
            ImGui::SameLine(); 
            if (ImGui::Button("Cancel"))
            {
                //testLoad();
            }
            
            ImGui::PopItemWidth();
        }

        ImGui::SeparatorText("When to Block");
        ImGui::Spacing();

        {
            HelpMarker("This Countdown Timer Set in Minutes.");
            ImGui::PushItemWidth(150);

            ImGui::Text("Enter the time period within which to block these sites");

            ImGui::InputTextWithHint(" ", "Example 0900-1200", str3, IM_ARRAYSIZE(str3));
            ImGui::SameLine();

            if (ImGui::Button("All Day"))
            {

            }

            ImGui::PopItemWidth();
            ImGui::Spacing();
            ImGui::PushItemWidth(75);

            ImGui::Text("Enter the time limit after which to block these sites:");

            ImGui::InputTextWithHint("Minutes in every", "Mins", str4, IM_ARRAYSIZE(str4));
            ImGui::SameLine();

            ImGui::Combo("Span", &item_current_2, "Hour\0Day\0Week\0Year\0\0");

            ImGui::PopItemWidth();
        }

        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Processes"))
        {
            setTable();
        }
        
    }
        
    void setTable()
    {
        HelpMarker("Below is a list of active processes that can be designate to kill, the 'Browse' allows manual addition.");
        ImGui::SameLine();
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_S, flags | ImGuiInputFlags_Tooltip);
        
        static std::wstring item_selected_idx = L""; // Here we store our selected data as an index.
        static bool item_highlight = false;
        std::wstring  item_highlighted_idx = L""; // Here we store our highlighted data as an index.

        static std::wstring item_selected_idx2 = L""; // Here we store our selected data as an index.
        static bool item_highlight2 = false;
        std::wstring  item_highlighted_idx2 = L""; // Here we store our highlighted data as an index.

        const size_t bufferSize = 50;
        char cstr[bufferSize];

        if (ImGui::Button("Browse"))
        {
            NFD_Init();

            nfdu8char_t* outPath;
            nfdu8filteritem_t filters[1] = { { "Exe files", "exe" }};
            nfdopendialogu8args_t args = { 0 };
            args.filterList = filters;
            args.filterCount = 1;
            nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);

            if (result == NFD_OKAY)
            {
                puts("Success!");
                puts(outPath);
                std::wstring cutName = sliceName(outPath);
                
                if (auto search = blackList.find(cutName); search == blackList.end())
                {
                    blackList[cutName];
                }
                
                NFD_FreePathU8(outPath); // Free Mem
            }
            else if (result == NFD_CANCEL)
            {
                puts("User pressed cancel.");
            }
            else
            {
                printf("Error: %s\n", NFD_GetError());
            }

            NFD_Quit();

        }
        
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 130.0f);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        {
            blackList.erase(item_selected_idx2);
        }
        ImGui::SameLine(0.0f, 5.0f);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
        {
            if (item_selected_idx != L"")
            {
                if (auto search = blackList.find(item_selected_idx); search == blackList.end())
                {
                    blackList[item_selected_idx] = totalProcessList[item_selected_idx];
                }
            }
        }

        ImGui::Spacing();      

        if (ImGui::BeginTable("Parent_Table", 2, parentFlags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
        {
            ImGui::TableSetupColumn("Active Processes");
            ImGui::TableSetupColumn("Murder List");
            ImGui::TableHeadersRow();
            ImGui::TableNextColumn();

            if (ImGui::BeginTable("table1", 1, tableFlags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
            {
                for (const auto& [key, value] : totalProcessList)
                {
                    convertWStringToCString(key, cstr, bufferSize);

                    const bool is_selected = (item_selected_idx == key);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(cstr, is_selected, ImGuiSelectableFlags_SpanAllColumns))
                        item_selected_idx = key;

                    if (item_highlight && ImGui::IsItemHovered())
                        item_highlighted_idx = key;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();

                }
                ImGui::EndTable();
            }

            ImGui::TableNextColumn();

            if (ImGui::BeginTable("table2", 1, tableFlags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
            {
                for (const auto& [key, value] : blackList)
                {

                    convertWStringToCString(key, cstr, bufferSize);

                    const bool is_selected2 = (item_selected_idx2 == key);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(cstr, is_selected2, ImGuiSelectableFlags_SpanAllColumns))
                        item_selected_idx2 = key;

                    if (item_highlight2 && ImGui::IsItemHovered())
                        item_highlighted_idx2 = key;

                    if (is_selected2)
                        ImGui::SetItemDefaultFocus();

                }
                ImGui::EndTable();
            }

            ImGui::EndTable();
        }
        //ImGui::SameLine();
    }

}