#include "Character.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

// 静态成员定义
std::vector<const ElementType*> ElementType::msValues;
std::unordered_map<int, const ElementType*> ElementType::msValueMap;
bool ElementType::msInitialized = false;

// 预定义静态实例
const ElementType ElementType::Inherit(0);
const ElementType ElementType::Aqua(1, 90018, { 17, 23 });
const ElementType ElementType::Fire(2, 90019, { 18, 24 });
const ElementType ElementType::Earth(3, 90021, { 19, 25 });
const ElementType ElementType::Wind(4, 90020, { 20, 26 });
const ElementType ElementType::Light(5, 90022, { 21, 27 });
const ElementType ElementType::Dark(6, 90023, { 22, 28 });
const ElementType ElementType::None(7);

// 构造函数
ElementType::ElementType(int value, int subNoteSkillItemId, const std::unordered_set<int>& attrTypes)
    : mValue(value)
    , mSubNoteSkillItemId(subNoteSkillItemId)
    , mGemAttrTypes(attrTypes) {
}

// 静态初始化
void ElementType::Initialize() {
    if (msInitialized) return;

    // 注册所有预定义实例
    msValues = {
        &Inherit, &Aqua, &Fire, &Earth, &Wind, &Light, &Dark, &None
    };

    for (const auto* type : msValues) {
        msValueMap[type->mValue] = type;
    }

    msInitialized = true;
}

// 获取所有值
const std::vector<const ElementType*>& ElementType::Values() {
    Initialize();
    return msValues;
}

// 通过 value 获取
const ElementType* ElementType::GetByValue(int value) {
    Initialize();
    auto it = msValueMap.find(value);
    if (it != msValueMap.end()) {
        return it->second;
    }
    return nullptr;
}

int CharacterDef::GetSkillsUpgradeGroup(int index) const {
    if (index < 0 || static_cast<size_t>(index) >= mSkillsUpgradeGroup.size()) {
        return -1;
    }
    return mSkillsUpgradeGroup[index];
}

CharGemDef* CharacterDef::GetCharGemData(int slotId) const {
    // 检查 slotId 是否有效（从1开始）
    if (slotId <= 0 || static_cast<size_t>(slotId) > mGemSlots.size()) {
        return nullptr;
    }

    int id = mGemSlots[slotId - 1];
    return GameData::GetCharGemDataTable()->Get(id);
}

bool CharacterDef::LoadFromPb(std::string data) {
    return true;
}