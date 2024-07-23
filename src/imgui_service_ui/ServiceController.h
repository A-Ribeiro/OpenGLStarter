#pragma once

#include <stdio.h>
#include <InteractiveToolkit/Platform/Core/ObjectBuffer.h>
#include <rapidjson/document.h>
#include <InteractiveToolkit/Platform/Process.h>
#include <InteractiveToolkit/Platform/Thread.h>

#include <InteractiveToolkit/ITKCommon/FileSystem/File.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

// 128 bytes
const uint32_t NAME_MAX_SIZE = 128;
// 512 bytes
const uint32_t COMMAND_MAX_SIZE = 512;

enum class ServiceType : uint8_t
{
    Service,
    InstallUninstall,
    RunOnce
};

class ServiceEntry
{
public:

    ServiceType type;

    union
    {
        struct
        {
            char name[NAME_MAX_SIZE];
            char command[COMMAND_MAX_SIZE];
            char args[COMMAND_MAX_SIZE];
            bool has_windows_special_signal;
        } service;

        struct
        {
            char name[NAME_MAX_SIZE];
            struct
            {
                char command[COMMAND_MAX_SIZE];
                char args[COMMAND_MAX_SIZE];
            } install;
            struct
            {
                char command[COMMAND_MAX_SIZE];
                char args[COMMAND_MAX_SIZE];
            } uninstall;
            struct
            {
                char command[COMMAND_MAX_SIZE];
                char args[COMMAND_MAX_SIZE];
            } check_installed;
        } install_uninstall;

        struct
        {
            char name[NAME_MAX_SIZE];
            char command[COMMAND_MAX_SIZE];
            char args[COMMAND_MAX_SIZE];
            uint32_t timeout_ms;
            int64_t timeout_counter_ms;
        } run_once;
    };

    Platform::Process *process;
    char process_status[32];

    //int64_t time_acc_micro;

    ServiceEntry() {
        process = nullptr;
        //time_acc_micro = 0;
    }

    void closeProcess() {

        bool has_windows_special_signal = false;
        if (type == ServiceType::Service)
            has_windows_special_signal = service.has_windows_special_signal;

        if (process != nullptr) {
            #if defined(_WIN32)
                if (has_windows_special_signal)
                    process->signal(SIGINT);
                else
                    process->horribleForceTermination();
            #elif defined(__linux__) || defined(__APPLE__)
                process->signal(SIGINT);
            #else
                #error "Platform not supported..."
            #endif
            delete process;
            process = nullptr;
        }
    }

    void start() {
        if(type != ServiceType::Service)
            return;
        if (process != nullptr)
            return;
        process = new Platform::Process(
            service.command,
            ITKCommon::StringUtil::parseArgv(service.args)
        );
    }

    void stop() {
        if(type != ServiceType::Service)
            return;
        if (process == nullptr)
            return;
        closeProcess();
    }

    void runOnce() {
        if(type != ServiceType::RunOnce)
            return;
        if (process != nullptr)
            return;
        run_once.timeout_counter_ms = run_once.timeout_ms;
        process = new Platform::Process(
            run_once.command,
            ITKCommon::StringUtil::parseArgv(run_once.args)
        );
    }

    void install() {
        if(type != ServiceType::InstallUninstall)
            return;
        if (process != nullptr)
            return;
        // process = new PlatformProcess(
        //     install_uninstall.install.command,
        //     install_uninstall.install.args
        // );
    }

    void uninstall() {
        if(type != ServiceType::InstallUninstall)
            return;
        if (process != nullptr)
            return;
        // process = new PlatformProcess(
        //     install_uninstall.uninstall.command,
        //     install_uninstall.uninstall.args
        // );
    }

    bool is_installed() {
        if(type != ServiceType::InstallUninstall)
            return false;
        if (process != nullptr)
            return false;
        // process = new PlatformProcess(
        //     install_uninstall.check_installed.command,
        //     install_uninstall.check_installed.args
        // );

        return false;
    }

    void process_status_update() {
        switch(type){
            case ServiceType::Service:
                if (process != nullptr){
                    if (process->isRunning())
                        strcpy(process_status, "[Running]");
                    else
                        strcpy(process_status, "[Defunct]");
                }else{
                    strcpy(process_status, "[Stopped]");
                }
                break;
            case ServiceType::InstallUninstall:
                strcpy(process_status, "[NotInstalled]");
                /*
                if (process != nullptr){
                    result = "[Running]";
                }else{
                    result = "[Stopped]";
                }
                */
                break;
            case ServiceType::RunOnce:
                if (process != nullptr){

                    run_once.timeout_counter_ms -= 500;
                    if (run_once.timeout_counter_ms <= 0) {
                        closeProcess();
                    } else if (!process->isRunning()) {
                        strcpy(process_status, "[Stopped]");
                        closeProcess();
                    }
                    else
                        strcpy(process_status, "[Running]");
                }else{
                    strcpy(process_status, "[Stopped]");
                }
                break;
        }
    }

    void ImGUIDrawAndProcessLogic(int id, Platform::Time *time) {

        //bool check_process_status = false;

        //time_acc_micro += time->deltaTimeMicro;

        //500 ms
        //if (time_acc_micro >= 500 * 1000) {
            //check_process_status = true;
            //time_acc_micro = 0;

            //process_status_update();
        //}


        char btn_label[32];

        switch(type){
            case ServiceType::Service: {
                sprintf(btn_label,"Start##%i",id);
                if (ImGui::Button(btn_label)){start();}
                ImGui::SameLine();
                sprintf(btn_label, "Stop##%i", id);
                if (ImGui::Button(btn_label)){stop();}
                ImGui::SameLine();
                if (strcmp(process_status,"[Stopped]") == 0)
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 1), "[Stopped]");
                else if (strcmp(process_status, "[Running]") == 0)
                    ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1), "[Running]");
                else if (strcmp(process_status, "[Defunct]") == 0)
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1), "[Defunct]");
                ImGui::SameLine();
                ImGui::Text("%s",service.name);
                break;
            }
            case ServiceType::InstallUninstall:{
                sprintf(btn_label, "Install##%i", id);
                if (ImGui::Button(btn_label)){install();}
                ImGui::SameLine();
                sprintf(btn_label, "Uninstall##%i", id);
                if (ImGui::Button(btn_label)){uninstall();}
                ImGui::SameLine();
                if (strcmp(process_status, "[NotInstalled]") == 0)
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 1), "[NotInstalled]");
                else if (strcmp(process_status, "[Installed]") == 0)
                    ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1), "[Installed]");
                ImGui::SameLine();
                ImGui::Text("%s",install_uninstall.name);
                break;
            }
            case ServiceType::RunOnce:{
                sprintf(btn_label, "Run##%i", id);
                if (ImGui::Button(btn_label)){runOnce();}
                ImGui::SameLine();
                if (strcmp(process_status, "[Stopped]") == 0)
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 1), "[Stopped]");
                else if (strcmp(process_status, "[Running]") == 0)
                    ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1), "[Running]");
                else if (strcmp(process_status, "[Defunct]") == 0)
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1), "[Defunct]");
                ImGui::SameLine();
                ImGui::Text("%s",run_once.name);
                break;
            }
        }
    }

};

class ServiceController: public EventCore::HandleCallback
{

    void fillServiceList(const rapidjson::Document &document)
    {
        if (!document.IsObject())
            return;
        
        const rapidjson::Value& title = document["title"];

        if(title.IsString())
            this->title = title.GetString();

        // Using a reference for consecutive access is handy and faster.
        const rapidjson::Value& entries = document["entries"]; 
        if (entries.IsArray()) {
            ServiceEntry auxEntry;
            // rapidjson uses SizeType instead of size_t.
            for (rapidjson::SizeType i = 0; i < entries.Size(); i++)  {
                const rapidjson::Value& entry = entries[i];
                const rapidjson::Value& type = entry["type"];
                if (type.IsString()){
                    if (strcmp(type.GetString(), "service") == 0){
                        const rapidjson::Value& name = entry["name"];
                        const rapidjson::Value& command = entry["command"];
                        const rapidjson::Value& args = entry["args"];
                        const rapidjson::Value& has_windows_special_signal = entry["has_windows_special_signal"];
                        if (name.IsString() && command.IsString() && args.IsString() && has_windows_special_signal.IsBool()) {
                            memset(&auxEntry, 0, sizeof(ServiceEntry));
                            //type
                            auxEntry.type = ServiceType::Service;
                            //name
                            rapidjson::SizeType name_size = strlen(name.GetString());
                            if (name_size >= NAME_MAX_SIZE) name_size = NAME_MAX_SIZE-1;
                            strncpy(auxEntry.service.name, name.GetString(), name_size);
                            //command
                            rapidjson::SizeType command_size = strlen(command.GetString());
                            if (command_size >= COMMAND_MAX_SIZE) command_size = COMMAND_MAX_SIZE-1;
                            strncpy(auxEntry.service.command, command.GetString(), command_size);
                            //args
                            rapidjson::SizeType args_size = strlen(args.GetString());
                            if (args_size >= COMMAND_MAX_SIZE) args_size = COMMAND_MAX_SIZE - 1;
                            strncpy(auxEntry.service.args, args.GetString(), args_size);
                            //has_windows_special_signal
                            auxEntry.service.has_windows_special_signal = has_windows_special_signal.GetBool();

                            this->entries.push_back(auxEntry);
                        }
                    } else if (strcmp(type.GetString(), "install_uninstall") == 0){
                        const rapidjson::Value& name = entry["name"];
                        const rapidjson::Value& command_install = entry["install"]["command"];
                        const rapidjson::Value& command_install_args = entry["install"]["args"];
                        const rapidjson::Value& command_uninstall = entry["uninstall"]["command"];
                        const rapidjson::Value& command_uninstall_args = entry["uninstall"]["args"];
                        const rapidjson::Value& command_check_installed = entry["check_installed"]["command"];
                        const rapidjson::Value& command_check_installed_args = entry["check_installed"]["args"];
                        if (name.IsString() && command_install.IsString() && command_uninstall.IsString() && command_check_installed.IsString()){
                            memset(&auxEntry, 0, sizeof(ServiceEntry));
                            //type
                            auxEntry.type = ServiceType::InstallUninstall;
                            //name
                            rapidjson::SizeType name_size = strlen(name.GetString());
                            if (name_size >= NAME_MAX_SIZE) name_size = NAME_MAX_SIZE-1;
                            strncpy(auxEntry.install_uninstall.name, name.GetString(), name_size);
                            //command_install
                            rapidjson::SizeType command_size = strlen(command_install.GetString());
                            if (command_size >= COMMAND_MAX_SIZE) command_size = COMMAND_MAX_SIZE-1;
                            strncpy(auxEntry.install_uninstall.install.command, command_install.GetString(), command_size);
                            //args
                            rapidjson::SizeType args_size = strlen(command_install_args.GetString());
                            if (args_size >= COMMAND_MAX_SIZE) args_size = COMMAND_MAX_SIZE - 1;
                            strncpy(auxEntry.install_uninstall.install.args, command_install_args.GetString(), args_size);
                            //command_uninstall
                            command_size = strlen(command_uninstall.GetString());
                            if (command_size >= COMMAND_MAX_SIZE) command_size = COMMAND_MAX_SIZE-1;
                            strncpy(auxEntry.install_uninstall.uninstall.command, command_uninstall.GetString(), command_size);
                            //args
                            args_size = strlen(command_uninstall_args.GetString());
                            if (args_size >= COMMAND_MAX_SIZE) args_size = COMMAND_MAX_SIZE - 1;
                            strncpy(auxEntry.install_uninstall.uninstall.args, command_uninstall_args.GetString(), args_size);
                            //command_check_installed
                            command_size = strlen(command_check_installed.GetString());
                            if (command_size >= COMMAND_MAX_SIZE) command_size = COMMAND_MAX_SIZE-1;
                            strncpy(auxEntry.install_uninstall.check_installed.command, command_check_installed.GetString(), command_size);
                            //args
                            args_size = strlen(command_check_installed_args.GetString());
                            if (args_size >= COMMAND_MAX_SIZE) args_size = COMMAND_MAX_SIZE - 1;
                            strncpy(auxEntry.install_uninstall.check_installed.args, command_check_installed_args.GetString(), args_size);

                            this->entries.push_back(auxEntry);
                        }
                    } else if (strcmp(type.GetString(), "run_once") == 0){
                        const rapidjson::Value& name = entry["name"];
                        const rapidjson::Value& command = entry["command"];
                        const rapidjson::Value& args = entry["args"];
                        const rapidjson::Value& timeout_ms = entry["timeout_ms"];
                        if (name.IsString() && command.IsString() && timeout_ms.IsUint()){
                            memset(&auxEntry, 0, sizeof(ServiceEntry));
                            //type
                            auxEntry.type = ServiceType::RunOnce;
                            //name
                            rapidjson::SizeType name_size = strlen(name.GetString());
                            if (name_size >= NAME_MAX_SIZE) name_size = NAME_MAX_SIZE-1;
                            strncpy(auxEntry.run_once.name, name.GetString(), name_size);
                            //command
                            rapidjson::SizeType command_size = strlen(command.GetString());
                            if (command_size >= COMMAND_MAX_SIZE) command_size = COMMAND_MAX_SIZE-1;
                            strncpy(auxEntry.run_once.command, command.GetString(), command_size);
                            //args
                            rapidjson::SizeType args_size = strlen(args.GetString());
                            if (args_size >= COMMAND_MAX_SIZE) args_size = COMMAND_MAX_SIZE - 1;
                            strncpy(auxEntry.run_once.args, args.GetString(), args_size);
                            //timeout_ms
                            auxEntry.run_once.timeout_ms = timeout_ms.GetUint();

                            this->entries.push_back(auxEntry);
                        }
                    }
                }
            }
        }


    }

    void updateEntryThread() {
        while (!Platform::Thread::isCurrentThreadInterrupted()) {
            datamutex.lock();
            for (auto& entry : entries) {
                entry.process_status_update();
            }
            datamutex.unlock();
            Platform::Sleep::millis(500);
        }
    }

public:

    std::string title;
    std::vector<ServiceEntry> entries;

    Platform::Mutex datamutex;
    Platform::Thread updateThread;

    ServiceController():updateThread( EventCore::CallbackWrapper( &ServiceController::updateEntryThread, this ) )
    {
        updateThread.start();
    }

    ~ServiceController()
    {
        updateThread.interrupt();
        updateThread.wait();
        for (auto& entry : entries) {
            entry.closeProcess();
        }
    }


    bool isLoadSuccess()
    {
        return entries.size() > 0;
    }

    void loadFromMemory(const uint8_t *buffer_param, uint32_t size)
    {
        entries.clear();
        Platform::ObjectBuffer buffer;
        // Default template parameter uses UTF8 and MemoryPoolAllocator.
        rapidjson::Document document;

        buffer.setSize(size+1);
        memcpy(buffer.data, buffer_param, size * sizeof(uint8_t));
        buffer.data[size] = 0;//add \0 at end
        document.ParseInsitu((char*)buffer.data);

        if (!document.HasParseError())
            fillServiceList(document);
    }

    bool loadFromFile(const char *filename, std::string *errorStr = nullptr)
    {
        entries.clear();
        
        // Default template parameter uses UTF8 and MemoryPoolAllocator.
        rapidjson::Document document;

        std::string fileContent;
        auto file = ITKCommon::FileSystem::File::FromPath(filename);
        if (!file.readContentToString(&fileContent,errorStr))
            return false;

        document.ParseInsitu(&fileContent[0]);

        // Platform::ObjectBuffer buffer;
        // FILE *in = ITKCommon::FileSystem::File::fopen(filename, "rb");
        // if (!in)
        //     return false;

        // {
        //     fseek(in, 0, SEEK_END);
        //     int32_t size = (int32_t)ftell(in);
        //     buffer.setSize(size + 1);
        //     fseek(in, 0, SEEK_SET);
        //     uint32_t readed_size = (uint32_t)fread(buffer.data, sizeof(uint8_t), size, in);
        //     fclose(in);
        //     ITK_ABORT(readed_size != size, "Read Command Error.\n");
        //     buffer.data[size] = 0;//add \0 at end
        // }
        // document.ParseInsitu((char*)buffer.data);

        if (document.HasParseError()){
            if (errorStr != nullptr)
                *errorStr = ITKCommon::PrintfToStdString("Error on load JSON file.\n");
            return false;    
        }

        fillServiceList(document);        
        return true;
    }

    void printContent() {

        printf("title: %s\n", title.c_str());
        for(const auto &entry:entries){
            printf("------------\n");
            switch(entry.type){
                case ServiceType::Service:
                printf("    type: ServiceType::Service\n");
                printf("    name: %s\n", entry.service.name);
                printf("    command: %s\n", entry.service.command);
                printf("    args: %s\n", entry.service.args);
                printf("    has_windows_special_signal: %i\n", (int)entry.service.has_windows_special_signal);
                break;
                case ServiceType::InstallUninstall:
                printf("    type: ServiceType::InstallUninstall\n");
                printf("    name: %s\n", entry.install_uninstall.name);
                printf("    install -> \n");
                printf("        command: %s\n", entry.install_uninstall.install.command);
                printf("        args: %s\n", entry.install_uninstall.install.args);
                printf("    uninstall -> \n");
                printf("        command: %s\n", entry.install_uninstall.uninstall.command);
                printf("        args: %s\n", entry.install_uninstall.uninstall.args);
                printf("    check_installed -> \n");
                printf("        command: %s\n", entry.install_uninstall.check_installed.command);
                printf("        args: %s\n", entry.install_uninstall.check_installed.args);
                break;
                case ServiceType::RunOnce:
                printf("    type: ServiceType::RunOnce\n");
                printf("    name: %s\n", entry.run_once.name);
                printf("    command: %s\n", entry.run_once.command);
                printf("    args: %s\n", entry.run_once.args);
                break;

            }
        }
        printf("------------\n");

    }
};