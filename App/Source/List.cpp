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
    std::vector<std::wstring> blackList;
    const char* allDay[4] = { "Hour" , "Day", "Week", "Year" };
    int databuffers::item_current_2 = 0;

    char databuffers::str1[9] = "";
    char databuffers::str2[9] = "";
    char databuffers::str3[9] = "";
    char databuffers::str4[9] = "";

    // Used for saving
    std::string configPath;
    std::string filePath;

    void init()
    {
        Core::ProcessList(totalProcessList);

        if (App::folderSetup())
        {
            std::cerr << "Shit dead. \n" << std::endl;
            return;
        }

        //writeAppFile();

    }

    // checks if directory exists if not make it, sets file path
    bool folderSetup() 
    {
        const char* homeDir = getenv("HOME");
        if (!homeDir) {
        #ifdef _WIN32
            homeDir = getenv("LOCALAPPDATA"); // Use USERPROFILE on Windows
        #endif
            if (!homeDir) {
                std::cerr << "HOME environment variable not found." << std::endl;
                return 1;
            }
        }

        configPath = std::string(homeDir) + "\\BlockIT";
        filePath = configPath + "\\settings.json";

        // Create the directory if it doesn't exist
        if (MKDIR(configPath.c_str()) && errno != EEXIST) {
            std::cerr << "Failed to create directory: " << strerror(errno) << std::endl;
            return 1;
        }

        // buffer that reads settings to load up
        char inStreamBuffer[128];
        std::ifstream inFile(filePath);
        if (inFile.is_open())
        {
            inFile.read(inStreamBuffer, 128);
            inFile.close();

            readAppFileIn(inStreamBuffer);
        }
        
        return 0;
    }

    // Write DATA out to filePath
    void writeAppFileOut()
    {
        // # TODO: Add argument into function to output
        std::ofstream outFile(filePath);
        if (outFile.is_open()) {
            outFile << "Hello, .fdsfconfig!";
            outFile.close();
            //std::cout << "Data written to " << filePath << std::endl;
        }
        else {
            std::cerr << "Failed to open file for writing." << std::endl;
        }
    }

    // Load the variables from the file into the application's presets
    void readAppFileIn(const char* inStreamBuffer)
    {

    }

    void convertWStringToCString(const std::wstring& wstr, char* cstr, size_t cstrSize) {
        size_t convertedChars = 0;
        errno_t err = wcstombs_s(&convertedChars, cstr, cstrSize, wstr.c_str(), _TRUNCATE);
        if (err != 0) {
            std::cerr << "Conversion error occurred." << std::endl;
        }
    }

    std::wstring convertToWString(const char* charArray) {
        size_t length = std::mbstowcs(nullptr, charArray, 0) + 1; // Get the length of the wide string
        std::wstring wstr(length, L'\0'); // Create a wide string with the required length
        std::mbstowcs(&wstr[0], charArray, length); // Convert the char array to a wide string
        return wstr;
    }

    std::wstring sliceName(const char* name)
    {
        size_t sizeName = std::strlen(name);
        int stop = 0;
        std::wstring newName = L"";

        for (int i = sizeName; i > 0; i--)
        {
            if (name[i] == '\\' )
            {
                stop = i;
                break;
            }
        }

        for (int i = stop + 1; i < sizeName; i++)
        {
            newName += name[i];
        }

        return newName;
    }

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

    void renderWindowContext(int preset)
    {
        static int statusFlag;
        ImGui::SeparatorText("Block Now");
        ImGui::Spacing();
        {
            HelpMarker("This Countdown Timer Set in Minutes.");
            ImGui::SameLine();
            ImGui::PushItemWidth(75);
            
            ImGui::InputTextWithHint("Hour(s)", "H", databuffers::str1, IM_ARRAYSIZE(databuffers::str1), 0, 0, 0);

            ImGui::SameLine();
            ImGui::InputTextWithHint("Minute(s)", "Mins", databuffers::str2, IM_ARRAYSIZE(databuffers::str2));
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

            ImGui::InputTextWithHint(" ", "Example. Input TimeSpan 0900-1200", databuffers::str3, IM_ARRAYSIZE(databuffers::str3));
            ImGui::SameLine();

            if (ImGui::Button("All Day"))
            {

            }

            ImGui::PopItemWidth();
            ImGui::Spacing();
            ImGui::PushItemWidth(75);

            ImGui::Text("Enter the time limit after which to block these sites:");

            ImGui::InputTextWithHint("Minutes in every", "Mins", databuffers::str4, IM_ARRAYSIZE(databuffers::str4));
            ImGui::SameLine();

            ImGui::Combo("Span", &databuffers::item_current_2, "Hour\0Day\0Week\0Year\0\0");

            ImGui::PopItemWidth();
        }

        ImGui::Spacing();

        ImGui::SeparatorText("Processes");
        setTable(totalProcessList);
    }
        
    void setTable(std::unordered_map<std::wstring, std::vector<DWORD>>& totalProcessList)
    {
        HelpMarker("Below is a list of active processes that can be designate to kill, the 'Browse' allows manual addition.");
        ImGui::SameLine();
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_S, flags | ImGuiInputFlags_Tooltip);
        
        if (ImGui::Button("Browse"))
        {
            NFD_Init();

            nfdu8char_t* outPath;
            nfdu8filteritem_t filters[1] = { { "JSON files", "json" }};
            nfdopendialogu8args_t args = { 0 };
            args.filterList = filters;
            args.filterCount = 1;
            nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);

            if (result == NFD_OKAY)
            {
                puts("Success!");
                puts(outPath);
                std::wstring cutName = sliceName(outPath);
                if (std::find(blackList.begin(), blackList.end(), cutName) == blackList.end())
                {
                    blackList.emplace_back(cutName);
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

        // #TODO add logic to pass back to each other
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 130.0f);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {}
        ImGui::SameLine(0.0f, 5.0f);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {}

        ImGui::Spacing();

        static std::wstring item_selected_idx = L""; // Here we store our selected data as an index.
        static bool item_highlight = false;
        std::wstring  item_highlighted_idx = L""; // Here we store our highlighted data as an index.

        if (ImGui::BeginTable("Parent_Table", 2, parentFlags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
        {
            ImGui::TableSetupColumn("Active Processes");
            ImGui::TableSetupColumn("Murder List");
            ImGui::TableHeadersRow();
            ImGui::TableNextColumn();

            // #TODO
            // Update this in intervals to not bog down the system
            // Also look into deffering it with a thread to update and check if a update to main list is needed
            if (ImGui::BeginTable("table1", 1, tableFlags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
            {
                for (const auto& [key, value] : totalProcessList)
                {
                    const size_t bufferSize = 50;
                    char cstr[bufferSize];
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

            static std::wstring item_selected_idx2 = L""; // Here we store our selected data as an index.
            static bool item_highlight2 = false;
            std::wstring  item_highlighted_idx2 = L""; // Here we store our highlighted data as an index.

            // #TODO
            // This table will only get updated if a new item is added and when loaded
            if (ImGui::BeginTable("table2", 1, tableFlags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
            {
                for (const auto& key : blackList) 
                {
                    const size_t bufferSize = 50;
                    char cstr[bufferSize];
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