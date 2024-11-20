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

    std::vector<std::wstring> totalProcessList;

    // Used as current context buffer for data send off
    static char str1[4] = "";
    static char str2[6] = "";
    static char str3[20] = "";
    static char str4[6] = "";
    //&item_current_2, " 15 Mins\0 30 Mins\0 1 Hour \0 90 Mins\0 2 Hours\0 3 Hours
    const int items[6] = { 15, 30, 60, 90, 120, 180 }; // hours are converted to mins
    static int item_current_2 = 0;
    std::vector<std::wstring> blackList;

    static int hr = 0;
    static int min = 0;

    // Checks if the application is running
    static bool isRunning = false;
    static int safeGuardTimer = 100;

    // deltaClock for running process threads
    static float accumulatedTimeThreads;

    // optional flags for running
    static bool isEveryTime;
    static bool isAllDay;

    static bool isTimeSpanned;
    std::vector<std::string> timeSpans;
    std::vector<std::string> vaildTimes;
    static bool isWaiting = false;

    static std::string warningFlagsText = " ";

    ImGuiIO globalIO;

    // Globals
    static Timer timer;
    dataBuffer presets[5];
    bool isThreadRunning = false;
    std::thread firstThread;

    // Used for fileSaving
    std::string configPath;
    std::string filePath;

    void init(ImGuiIO io)
    {
        Core::GetProcessNames(totalProcessList);

        globalIO = io;

        if (App::folderSetup()) // Startups default to first tab
        {
            std::cerr << "Shit dead. \n" << std::endl;
            return;
        }

        // Load in the reference fields for the first tab into the buffer
        loadBuffer(0);
    }

    bool getIsRunning()
    {
        return isRunning;
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
        if (!isRunning)
        {
            strncpy_s(str1, "", 0);
            strncpy_s(str2, "", 0);
            strncpy_s(str3, "", 0);
            strncpy_s(str4, "", 0);
            item_current_2 = 0;
            blackList.clear();
        } 
    }

    void loadBuffer(int bufferData)
    {
        if (!isRunning)
        {
            strncpy_s(str1, presets[bufferData].str1, 9);
            strncpy_s(str2, presets[bufferData].str2, 9);
            strncpy_s(str3, presets[bufferData].str3, 16);
            strncpy_s(str4, presets[bufferData].str4, 9);
            item_current_2 = presets[bufferData].item_current_2;
            blackList = presets[bufferData].List;
        }
    }

    // Write out all Buffers out to filePath (Updates the saveSet data that currently in buffer)
    void writeAppFileOut(int saveSets)
    {
        // Saving the current contextBuffer into the state's preset
        strncpy_s(presets[saveSets].str1, str1, 9);
        strncpy_s(presets[saveSets].str2, str2, 9);
        strncpy_s(presets[saveSets].str3, str3, 16);
        strncpy_s(presets[saveSets].str4, str4, 9);
        presets[saveSets].item_current_2 = item_current_2;
        presets[saveSets].List = blackList;

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
                for (const auto& names : presets[i].List)
                {
                    outFile << names << ',';
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
                presets[presetSets].List.emplace_back(stringToWstring(strBuffer));
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

    bool is_digits(char* str)
    {
        bool isdig = false;
        for (size_t i = 0; i < strnlen_s(str, 20); i++)
        {
            if (!isdigit(static_cast< unsigned char > (str[i])) )
            {
                return false;
            }
            else if (str[i] == '.')
            {
                return false;
            }
            else 
            {
                isdig = true;
            }
        }
        return isdig;
    }

    bool is_digits_Withspace(char* str)
    {
        bool isdig = false;
        for (size_t i = 0; i < strnlen_s(str, 20); i++)
        {
            if (!isdigit(static_cast<unsigned char> (str[i])) && str[i] != ' ')
            {
                return false;
            }
            else if (str[i] == '.')
            {
                return false;
            }
            else
            {
                isdig = true;
            }
        }
        return isdig;
    }

    static void startNameThreadProcess(std::vector<std::wstring>& process)
    {
        if (isThreadRunning == false)
        {
            firstThread = std::thread(Core::GetProcessNames, std::ref(process));
            isThreadRunning = true;
        }
        else if (firstThread.joinable() && isThreadRunning == true)
        {
            firstThread.join();
            isThreadRunning = false;
        }
    }

    void getToKilling()
    {
        for (const auto& ps : blackList)
        {
            std::vector<DWORD> ids = Core::GetProcessIDs(ps);
            Core::KillProcess(ids);
        }
    }

    //static void startThreadProcessToKill(std::vector<std::wstring>& process)
    //{
    //    if (isThreadRunning == false)
    //    {
    //        firstThread = std::thread(Core::GetProcessNames, std::ref(process));
    //        isThreadRunning = true;
    //    }
    //    else if (firstThread.joinable() && isThreadRunning == true)
    //    {
    //        firstThread.join();
    //        isThreadRunning = false;
    //    }
    //}

    void renderWindowContext()
    { 
        ImGui::SeparatorText("Block Now");
        ImGui::Spacing();
        {
            if ((strnlen(str1, 4) > 0 || strnlen(str2, 6) > 0) && !isRunning)
            {
                if (is_digits(str1) || is_digits(str2))
                {
                    hr = 0;
                    min = 0;
                    if (is_digits(str1))
                    {
                        hr = std::stoi(str1);
                    }
                    if (is_digits(str2))
                    {
                        min = std::stoi(str2);
                    }
            
                    if (hr > -1 || min > -1)
                    {
                        timer.updateCurrentTime();
                        timer.updateTargetTime(hr, min);
                        ImGui::Text("Set Time: ");
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), timer.toTime);
                    }
                    else
                    {
                        timer.updateCurrentTime();
                        ImGui::Text("Expected Time: ");
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), timer.nowTime);
                    }
                }
                else
                {
                    timer.updateCurrentTime();
                    ImGui::Text("Expected Time: ");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), timer.nowTime);
                }
            }
            else if (!isRunning)
            {
                timer.updateCurrentTime();
                ImGui::Text("Expected Time: ");
                ImGui::SameLine();
                ImGui::Text(timer.nowTime);
            }
            else
            {
                ImGui::Text("Set Time: ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), timer.toTime);
            }

            {
                HelpMarker("This Countdown Timer Set in Minutes.");
                ImGui::SameLine();
                ImGui::PushItemWidth(75);

                ImGui::InputTextWithHint("Hour(s)", "H", str1, IM_ARRAYSIZE(str1));

                ImGui::SameLine();
                ImGui::InputTextWithHint("Minute(s)", "Mins", str2, IM_ARRAYSIZE(str2));
                ImGui::Spacing();
            };

            if (ImGui::Button("Block"))
            {
                if ((strnlen(str1, 4) > 0 || strnlen(str2, 6) > 0) && is_digits(str1) || is_digits(str2))
                {
                    isRunning = true;
                    warningFlagsText = " ";
                    safeGuardTimer = 0;
                }
                else
                {
                    warningFlagsText = "Please Input a valid values.";
                }
            }
            
            ImGui::SameLine(); 
            if (ImGui::Button("Cancel"))
            {
                if (isRunning)
                {
                    warningFlagsText = "Timer has been stopped";
                }
                isRunning = false;
            }

            accumulatedTimeThreads += globalIO.DeltaTime * 100;

            if (isRunning)
            {
                timer.updateCurrentTime();
                timer.updateTimeLeft();

                if (timer.elapsed_timeLeft.count() > 0.0)
                {
                    ImGui::SameLine();
                    ImGui::Text("Time Left: ");
                    ImGui::SameLine();
                    ImGui::Text(timer.timeLeft);
                    ImGui::SameLine();
                    ImGui::Text("Secs");

                    ImGui::Text(timer.nowTime);
                    ImGui::Text(timer.toTime);

                }
                else 
                {
                    isRunning = false;
                }
                if (accumulatedTimeThreads > 200)
                {
                    //getToKilling();
                    accumulatedTimeThreads = 0;
                }
            }
            // Optional flags don't active it if block now is active
            // optional flags are always active
            else if (isTimeSpanned && !isRunning)
            {
                if (!isWaiting)
                {
                    timer.updateStartTimeMilitary(std::stoi(vaildTimes[0].substr(0, 2)), std::stoi(vaildTimes[0].substr(2, 4)));
                    vaildTimes.erase(vaildTimes.begin());
                    isWaiting = true;
                }
                if (isWaiting)
                {
                    if (timer.compareStartTime())
                    {
                        timer.updateTargetTimeMilitary(std::stoi(vaildTimes[0].substr(0, 2)), std::stoi(vaildTimes[0].substr(2, 4)));
                        vaildTimes.erase(vaildTimes.begin());
                        isWaiting = false;
                        isRunning = true;
                    }
                }
                if (vaildTimes.size() == 0)
                {
                    isTimeSpanned = false;
                }
            }
            else if (isEveryTime && !isRunning)
            {
                if (timer.compareStartTime())
                {
                    timer.updateTargetTime(0, std::stoi(str4));
                    isRunning = true;
                    isEveryTime = false;
                }
            }
            else if (isAllDay && !isRunning)
            {
                timer.updateTargetTime(24, 0);
                isRunning = true;
            }
            
            ImGui::PopItemWidth();

            ImGui::SameLine();
            ImGui::Text(warningFlagsText.c_str());
        }

        ImGui::SeparatorText("When to Block");
        ImGui::Spacing();

        {
            HelpMarker("This Countdown Timer Set in Minutes.");
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::Text("Option Flags");

            ImGui::Text("Enter the time period within which to block these sites");

            ImGui::InputTextWithHint(" ", "Example. 0900 1200", str3, IM_ARRAYSIZE(str3));

            ImGui::SameLine();
            if (ImGui::Button("Ok"))
            {
                if (strnlen(str3, 19) > 0 && is_digits_Withspace(str3) && isTimeSpanned != true)
                {
                    std::string timeSplits;
                    for (size_t i = 0; i <= strnlen(str3, 19); i++)
                    {
                        if (str3[i] == ' ') 
                        {
                            timeSpans.emplace_back(timeSplits);
                            timeSplits = "";
                        }
                        else if (str3[i] == '\0')
                        {
                            timeSpans.emplace_back(timeSplits);
                            timeSplits = "";
                        }
                        else
                        {
                            timeSplits += str3[i];
                        }
                    }

                    for (size_t i = 0; i < timeSpans.size(); i+=2)
                    {
                        std::string timeref = timeSpans[i];
                        auto checkVaild = timer.isVaildTime(std::stoi(timeref.substr(0,2)), std::stoi(timeref.substr(2, 4)));
                        
                        if (checkVaild)
                        {
                            vaildTimes.emplace_back(timeSpans[i]);
                            vaildTimes.emplace_back(timeSpans[i+1]);
                        }

                    }
                    if (vaildTimes.size() > 0)
                    {
                        safeGuardTimer = 0;
                        isTimeSpanned = true;
                    }
                }
            }

            if (isTimeSpanned)
            {
                ImGui::SameLine();
                ImGui::Text("Time Span Active");
            }
            else
            {
                ImGui::SameLine();
                ImGui::Text(" ");
            }

            if (ImGui::Button("All Day"))
            {
                isAllDay = !isAllDay;
            }

            ImGui::SameLine();
            static std::string allDayText;
            if (isAllDay)
            {
                allDayText = "Active";
            }
            else
            {
                allDayText = " ";
            }
            ImGui::Text(allDayText.c_str());

            ImGui::PopItemWidth();
            ImGui::Spacing();
            ImGui::PushItemWidth(75);

            ImGui::Text("Enter the time limit after which to block these sites:");

            ImGui::InputTextWithHint("Minutes in every", "Mins", str4, IM_ARRAYSIZE(str4));
            ImGui::SameLine();

            ImGui::Combo("|", &item_current_2, " 15 Mins\0 30 Mins\0 1 Hour \0 90 Mins\0 2 Hours\0 3 Hours\0\0");
            ImGui::SameLine();

            static std::string spanText;
            if (isEveryTime)
            {
                spanText = "Active";
            }
            else
            {
                spanText = " ";
            }

            if ((strnlen(str4, 6) > 0 && is_digits(str4)) && isEveryTime != true)
            {         
                timer.updateStartTime(0, items[item_current_2]);
                isEveryTime = true;

            }
            else if((strnlen(str4, 6) == 0))
            {
                isEveryTime = false;
            }

            ImGui::PopItemWidth();
        }

        ImGui::Spacing();

        
        if (ImGui::CollapsingHeader("Processes"))
        {
            setTable(); 

            if (!isRunning)
            {
                if (accumulatedTimeThreads > 300) // Refresh the process list
                {
                    startNameThreadProcess(totalProcessList);
                    accumulatedTimeThreads = 0;
                }
            }
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

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 130.0f);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        {
            std::erase(blackList, item_selected_idx2);
        }
        ImGui::SameLine(0.0f, 5.0f);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
        {
            if (item_selected_idx != L"")
            {
                blackList.emplace_back(item_selected_idx);
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
                for (const auto& ps : totalProcessList)
                {
                    convertWStringToCString(ps, cstr, bufferSize);

                    const bool is_selected = (item_selected_idx == ps);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(cstr, is_selected, ImGuiSelectableFlags_SpanAllColumns))
                        item_selected_idx = ps;

                    if (item_highlight && ImGui::IsItemHovered())
                        item_highlighted_idx = ps;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();

                }
                ImGui::EndTable();
            }

            ImGui::TableNextColumn();

            if (ImGui::BeginTable("table2", 1, tableFlags, ImVec2(ImGui::GetContentRegionAvail().x, 275)))
            {
                for (const auto& name : blackList)
                {
                    convertWStringToCString(name, cstr, bufferSize);

                    const bool is_selected2 = (item_selected_idx2 == name);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(cstr, is_selected2, ImGuiSelectableFlags_SpanAllColumns))
                        item_selected_idx2 = name;

                    if (item_highlight2 && ImGui::IsItemHovered())
                        item_highlighted_idx2 = name;

                    if (is_selected2)
                        ImGui::SetItemDefaultFocus();

                }
                ImGui::EndTable();
            }

            ImGui::EndTable();
        }

    }

}