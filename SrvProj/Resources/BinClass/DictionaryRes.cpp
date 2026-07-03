#include "DictionaryRes.h"
#include "../ResourceJsonUtil.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool DictionaryTabRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    return true;
}

bool DictionaryTabRes::LoadFromPb(std::string data)
{
    DictionaryTab dictionaryTab;
    if (!dictionaryTab.ParseFromString(data)) {
        return false;
    }

    Id = dictionaryTab.id();

    return true;
}

bool DictionaryEntryRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Tab", Tab);
    ReadResourceJsonField(data, "Index", Index);
    return true;
}

bool DictionaryEntryRes::LoadFromPb(std::string data)
{
    DictionaryEntry dictionaryEntry;
    if (!dictionaryEntry.ParseFromString(data)) {
        return false;
    }

    Id = dictionaryEntry.id();
    Tab = dictionaryEntry.tab();
    Index = dictionaryEntry.index();

    return true;
}

