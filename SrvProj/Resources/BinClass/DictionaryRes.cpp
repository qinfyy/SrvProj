#include "DictionaryRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool DictionaryTabRes::LoadFromPb(std::string data)
{
    DictionaryTab dictionaryTab;
    if (!dictionaryTab.ParseFromString(data)) {
        return false;
    }

    Id = dictionaryTab.id();

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
