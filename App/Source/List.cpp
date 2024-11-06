#include "List.h"

namespace App 
{

	void testData()
	{
        //simdjson::dom::parser parser;
        //simdjson::dom::element jsonData = parser.parse(R"({
        //"name": "John Doe",
        //"age": 30,
        //"city": "New York"
        //})");
        //
        //std::string jsonString = simdjson::to_string(jsonData);
        //saveData("data.json", jsonData);
	}

    void testLoad()
    {
       //simdjson::dom::element jsonData = loadData("data.json");
       //
       //std::string name = jsonData["name"];
       //int age = jsonData["age"];
       //std::string city = jsonData["city"];
       //
       //std::cout << "Name: " << name << "\n";
       //std::cout << "Age: " << age << "\n";
       //std::cout << "City: " << city << "\n";
    }

    void saveData(const std::string& filename, const simdjson::dom::element& jsonData) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << jsonData;
            file.close();
        }
    }

    //simdjson::dom::element loadData(const std::string& filename) {
    //    std::ifstream file(filename);
    //    //std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    //    return simdjson::dom::parser().parse(jsonContent);
    //}

    void convertWStringToCString(const std::wstring& wstr, char* cstr, size_t cstrSize) {
        size_t convertedChars = 0;
        errno_t err = wcstombs_s(&convertedChars, cstr, cstrSize, wstr.c_str(), _TRUNCATE);
        if (err != 0) {
            std::cerr << "Conversion error occurred." << std::endl;
        }
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

    void renderWindowContext()
    {
        // #TODO - take these out
        std::unordered_map<std::wstring, std::vector<DWORD>> totalProcessList;
        Core::ProcessList(totalProcessList);

        ImGui::SeparatorText("Block IT Now");
        ImGui::Spacing();
        {
            HelpMarker("This Countdown Timer Set in Minutes.");
            ImGui::SameLine();
            ImGui::PushItemWidth(75);

            // #TODO add variables to save data
            static char str1[9] = "";
            ImGui::InputTextWithHint("Hour(s)", "H", str1, IM_ARRAYSIZE(str1), 0, 0, 0);

            ImGui::SameLine();

            // #TODO add variables to save 
            static char str2[9] = "";
            ImGui::InputTextWithHint("Minute(s)", "Mins", str2, IM_ARRAYSIZE(str2));

            ImGui::Spacing();

            // #TODO add logic to buttons 
            ImGui::Button("Block");
            ImGui::SameLine();
            ImGui::Button("Pause");
            ImGui::SameLine();
            ImGui::Button("Cancel");

            ImGui::PopItemWidth();
        }

        ImGui::SeparatorText("When to Block");
        ImGui::Spacing();

        {
            ImGui::PushItemWidth(150);

            ImGui::Text("Enter the time period within which to block these sites");

            // #TODO add variables to save 
            static char str3[128] = "";
            ImGui::InputTextWithHint(" ", "Example", str3, IM_ARRAYSIZE(str3));
            ImGui::SameLine();

            // #TODO add logic
            ImGui::Button("All Day");

            ImGui::PopItemWidth();
            ImGui::Spacing();
            ImGui::PushItemWidth(75);

            // #TODO add variables to save 
            static char str4[128] = "";
            ImGui::InputTextWithHint("Minutes in every", "Mins", str4, IM_ARRAYSIZE(str4));
            ImGui::SameLine();

            // #TODO add variables to save 
            static int item_current_2 = 0;
            ImGui::Combo("Span", &item_current_2, "Hour\0Day\0Week\0Year\0\0");

            ImGui::PopItemWidth();
        }

        ImGui::Spacing();

        ImGui::SeparatorText("Processes");
        setTable(totalProcessList);
    }

    void setTable(std::unordered_map<std::wstring, std::vector<DWORD>>& totalProcessList)
    {

        // #TODO - take this all out
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

        static ImGuiTabBarFlags parentFlags =
            ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_Borders;

        static ImGuiTabBarFlags tableflags =
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;

        static ImGuiInputFlags route_options = ImGuiInputFlags_Repeat;
        static ImGuiInputFlags route_type = ImGuiInputFlags_RouteFocused;

        ImGuiInputFlags flags = route_type | route_options; // Merged flags
        if (route_type != ImGuiInputFlags_RouteGlobal)
            flags &= ~(ImGuiInputFlags_RouteOverFocused | ImGuiInputFlags_RouteOverActive | ImGuiInputFlags_RouteUnlessBgFocused);

        HelpMarker("This is a List of Processes which you can designate which to kill, the 'save' button sets the present.");
        ImGui::SameLine();
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_S, flags | ImGuiInputFlags_Tooltip);
        ImGui::Button("Save");
        ImGui::SameLine();

        // #TODO add logic to pass back to each other
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 140.0f);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {}
        ImGui::SameLine(0.0f, spacing);
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
            if (ImGui::BeginTable("table1", 1, tableflags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
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
            if (ImGui::BeginTable("table2", 1, tableflags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
            {
                for (const auto& [key, value] : totalProcessList) // change totalProcessList to blacklist
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

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
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